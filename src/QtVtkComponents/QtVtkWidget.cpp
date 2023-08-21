#ifndef VTK_9

/**
 * \file        QVTKWidget.cpp
 * \author      Charles PIGNEROL
 * \date        04/06/2012
 */

#include "Internal/ContextIfc.h"

#include "QtVtkComponents/QtVtkWidget.h"
#include "Utils/Common.h"

#include "TkUtil/MemoryError.h"

#include <QThread>

#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>

#include <iostream>


using namespace std;
using namespace TkUtil;


namespace Mgx3D
{

namespace QtVtkComponents
{



// ===========================================================================
//                        LA CLASSE QtVtkWidget
// ===========================================================================


QtVtkWidget::QtVtkWidget (QWidget* parent, vtkRenderWindow* window)
	: QVTKWidget (parent),
	  _imageCache (0), _maxImageWidth (0), _maxImageHeight (0), _imageDepth (0),
	  _imageCacheMutex ( ), _updateFromCache (false)
{
	// La bonne utilisation de la transparence en VTK requiert Qt::WA_TranslucentBackground qui active le canal alpha :
	setAttribute (Qt::WA_TranslucentBackground, true);
	// SetAlphaBitPlanes : à faire après un premier rendu qui appel OpenGLInitState et positionne l'attribut AlphaBitPlanes :
	if (0 != window)
	{
		window->SetAlphaBitPlanes (8);
		window->SetMultiSamples (0);
		vtkRendererCollection*	renderers	= 0 == mRenWin ? 0 : mRenWin->GetRenderers ( );			
		vtkRenderer*			renderer	= 0 == renderers ? 0 : renderers->GetFirstRenderer ( );
		if (0 != renderer)
			renderer->SetUseDepthPeeling (true);
	}	// if (0 != window)
	SetRenderWindow (window);
	setAutomaticImageCacheEnabled (false);		// Usefull ?
	connect (this, SIGNAL (needUpdate (bool)), this,
	         SLOT (updateView (bool)));
}	// QtVtkWidget::QtVtkWidget


QtVtkWidget::QtVtkWidget (const QtVtkWidget&)
	: QVTKWidget (0),
	  _imageCache (0), _maxImageWidth (0), _maxImageHeight (0), _imageDepth (0),
	  _imageCacheMutex ( ), _updateFromCache (false)
{
	MGX_FORBIDDEN ("QtVtkWidget copy constructor is not allowed.");
}	// QtVtkWidget::QtVtkWidget (const QtVtkWidget&)


QtVtkWidget& QtVtkWidget::operator = (const QtVtkWidget&)
{
	MGX_FORBIDDEN ("QtVtkWidget assignment operator is not allowed.");
	return *this;
}	// QtVtkWidget::QtVtkWidget (const QtVtkWidget&)


QtVtkWidget::~QtVtkWidget ( )
{
	if (0 != _imageCache)
		_imageCache->Delete ( );
	_imageCache	= 0;
}	// QtVtkWidget::~QtVtkWidget


void QtVtkWidget::activateClientServerMode (
		unsigned short maxWidth, unsigned short maxHeight, unsigned char depth)
{
	if ((0 == maxWidth) || (0 == maxHeight) || (0 == depth))
		throw Exception (UTF8String ("QtVtkWidget::activateClientServerMode. Dimension nulle.", Charset::UTF_8));
	if (3 != depth)
		throw Exception (UTF8String ("QtVtkWidget::activateClientServerMode. Seule une profondeur de 3 est actuellement supportée.", Charset::UTF_8));

	AutoMutex	cacheMutex (&getImageCacheMutex ( ));
	if (0 != _imageCache)
		_imageCache->Delete ( );
	_imageCache				= vtkUnsignedCharArray::New ( );
	_maxImageWidth	= maxWidth;
	_maxImageHeight	= maxHeight;
	_imageDepth		= depth;
	const size_t	size	= _maxImageWidth * _maxImageHeight * (unsigned short)_imageDepth;
	unsigned char*	image	= new unsigned char [size];
	memset (image, 0, size * sizeof (unsigned char));
	_imageCache->SetNumberOfComponents (_imageDepth);
	_imageCache->SetNumberOfValues (size);
	_imageCache->SetVoidArray (image, size, 0);
	vtkRendererCollection*	renderers	=
						0 == mRenWin ? 0 : mRenWin->GetRenderers ( );	
	vtkRenderer*			renderer	=
						0 == renderers ? 0 : renderers->GetFirstRenderer ( );
	if (0 != renderer)
		renderer->DrawOff ( );
}	// QtVtkWidget::activateClientServerMode


void QtVtkWidget::deactivateClientServerMode ( )
{
	AutoMutex	cacheMutex (&getImageCacheMutex ( ));
	if (0 != _imageCache)
		_imageCache->Delete ( );
	_imageCache		= 0;
	_maxImageWidth	= 0;
	_maxImageHeight	= 0;
	_imageDepth		= 0;
	vtkRendererCollection*	renderers	=
						0 == mRenWin ? 0 : mRenWin->GetRenderers ( );	
	vtkRenderer*			renderer	=
						0 == renderers ? 0 : renderers->GetFirstRenderer ( );
	if (0 != renderer)
		renderer->DrawOn ( );
}	// QtVtkWidget::deactivateClientServerMode


vtkUnsignedCharArray& QtVtkWidget::getImageCache ( )
{
	CHECK_NULL_PTR_ERROR (_imageCache)
	return *_imageCache;
}	// QtVtkWidget::getImageCache


void QtVtkWidget::imageCacheUpdated ( )
{
	if (0 == _imageCache)
		return;

	_updateFromCache	= true;

	// Réafficher le cache dans le bon thread.
	// false : ne pas provoquer d'appel à Render.
	updateView (false);
}	// QtVtkWidget::imageCacheUpdated


Mutex& QtVtkWidget::getImageCacheMutex ( )
{
	return _imageCacheMutex;
}	// QtVtkWidget::getImageCacheMutex


void QtVtkWidget::updateView (bool render)
{
	if (thread ( ) == QThread::currentThread ( ))
	{	// On est dans le thread de l'IHM :
		if ((true == _updateFromCache) && (0 != _imageCache))
		{
			AutoMutex	cacheMutex (&getImageCacheMutex ( ));
			if (_imageCache->GetNumberOfTuples  ( ) *
						_imageCache->GetNumberOfComponents ( ) !=
													width ( ) * height ( ) * 3)
			{
				cerr << "QtVtkWidget::updateView. Image data count ("
				     << _imageCache->GetNumberOfTuples  ( ) *
									_imageCache->GetNumberOfComponents ( )
				     << " is different of image data required ("
				     << (3 * width ( ) * height ( )) << "). Window is "
				     << width ( ) << "x" << height ( ) << " with depth = 3."
				     << endl;
				return;
			}	// if (image->GetNumberOfTuples  ( ) * ...
			if (mCachedImage->GetPointData ( )->GetScalars ( ) != _imageCache)
				mCachedImage->GetPointData ( )->SetScalars (_imageCache);
			mRenWin->SetPixelData (0, 0, width ( ) - 1, height ( ) - 1,
			                       _imageCache, !mRenWin->GetDoubleBuffer ( ));

// Notes prises par CP lors des tests de faisabilité
// (SSHTunneling/src/commonlib/QtClientWidget.cpp) :
// mRenWin->GetDoubleBuffer ( ) : ON
// mRenWin->GetErase ( ) : OFF
// Surtout pas un appel à Render, on souhaite ici afficher ce qu'on a mis dans
// le cache, pas la scène (nulle côté client).
//  if (0 != mRenWin)
//      mRenWin->Render ( );
// => Provoquer le réaffichage du buffer :
//  mRenWin->show ( );
// => updateView est appelé pour provoquer le réaffichage dans le bon thread.
		}	// if ((true == _updateFromCache) && (0 != _imageCache))
		if ((true == render) && (0 != GetRenderWindow ( )))
			GetRenderWindow ( )->Render ( );
		_updateFromCache	= false;
		update ( );
	}	// if (thread ( ) == QThread::currentThread ( ))
	else
	{	// Passer dans le thread de l'IHM :
		emit needUpdate (render);
	}	// else if (thread ( ) == QThread::currentThread ( ))
}	// QtVtkWidget::updateView


void QtVtkWidget::enterEvent (QEvent* event)
{
	setFocus ( );
	QVTKWidget::enterEvent (event);
}	// QtVtkWidget::enterEvent


void QtVtkWidget::focusInEvent (QFocusEvent* event)
{
// CP le 07/10/19 : ce repaint est à l'origine de blocages en mode C/S. Faut il le maintenir ???	
//	repaint ( );
}	// QtVtkWidget::enterEvent


void QtVtkWidget::keyReleaseEvent (QKeyEvent* event)
{
	QVTKWidget::keyReleaseEvent (event);
	emit keyRelease (event);
}	// QtVtkWidget::keyReleaseEvent



// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// VTK_9
