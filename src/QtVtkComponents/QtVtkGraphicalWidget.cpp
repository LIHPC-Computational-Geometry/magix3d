/**
 * \file        QtVtkGraphicalWidget.cpp
 * \author      Charles PIGNEROL
 * \date        23/11/2011
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "QtVtkComponents/QtVtkGraphicalWidget.h"
#include "QtVtkComponents/vtkCustomizableInteractorStyleTrackball.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "VtkComponents/vtkLockableRenderWindow.h"
#include "Utils/DisplayProperties.h"

#include <QtUtil/QtErrorManagement.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <iostream>
#include <memory>

#include <QBoxLayout>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::QtComponents;


namespace Mgx3D
{

namespace QtVtkComponents
{


// ===========================================================================
//                        LA CLASSE QtVtkGraphicalWidget
// ===========================================================================

QtVtkGraphicalWidget::QtVtkGraphicalWidget (QWidget* parent, vtkRenderWindow* w, RenderingManager* rm)
	: QtComponents::Qt3DGraphicalWidget (parent, rm),
	  _vtkWidget (0), _lockableRenderWindow (0)
{
	createGui (w);
}	// QtVtkGraphicalWidget::QtVtkGraphicalWidget


QtVtkGraphicalWidget::QtVtkGraphicalWidget (const QtVtkGraphicalWidget&)
	: QtComponents::Qt3DGraphicalWidget (0, 0),
	  _vtkWidget (0), _lockableRenderWindow (0)
{
	MGX_FORBIDDEN ("QtVtkGraphicalWidget copy constructor is not allowed.");
}	// QtVtkGraphicalWidget::QtVtkGraphicalWidget (const QtVtkGraphicalWidget&)


QtVtkGraphicalWidget& QtVtkGraphicalWidget::operator = (const QtVtkGraphicalWidget&)
{
	MGX_FORBIDDEN ("QtVtkGraphicalWidget assignment operator is not allowed.");
	return *this;
}	// QtVtkGraphicalWidget::QtVtkGraphicalWidget (const QtVtkGraphicalWidget&)


QtVtkGraphicalWidget::~QtVtkGraphicalWidget ( )
{
}	// QtVtkGraphicalWidget::~QtVtkGraphicalWidget


VTKRenderingManager& QtVtkGraphicalWidget::getVTKRenderingManager ( )
{
	VTKRenderingManager*	rm	= dynamic_cast<VTKRenderingManager*>(&getRenderingManager ( ));

	if (0 == rm)
		throw Exception (UTF8String ("QtVtkGraphicalWidget::getVTKRenderingManager : absence de gestionnaire de rendu associé pour environnement Qt/VTK.", Charset::UTF_8));

	return *rm;
}	// QtVtkGraphicalWidget::getVTKRenderingManager


const QWidget* QtVtkGraphicalWidget::getRenderingWidget ( ) const
{
	return _vtkWidget;
}	// QtVtkGraphicalWidget::getRenderingWidget


QWidget* QtVtkGraphicalWidget::getRenderingWidget ( )
{
	return _vtkWidget;
}	// QtVtkGraphicalWidget::getRenderingWidget


vtkLockableRenderWindow& QtVtkGraphicalWidget::getLockableRenderWindow ( )
{
	CHECK_NULL_PTR_ERROR (_lockableRenderWindow);
	return *_lockableRenderWindow;
}	// QtVtkGraphicalWidget::getLockableRenderWindow


//QtVtkWidget& QtVtkGraphicalWidget::getVTKWidget ( )
QtVtkGraphicWidget& QtVtkGraphicalWidget::getVTKWidget ( )
{
	CHECK_NULL_PTR_ERROR (_vtkWidget)
	return *_vtkWidget;
}	// QtVtkGraphicalWidget::getVTKWidget


//void QtVtkGraphicalWidget::setVTKWidget (QtVtkWidget* widget)
void QtVtkGraphicalWidget::setVTKWidget (QtVtkGraphicWidget* widget)
{
	if (widget == _vtkWidget)
		return;

	if (0 != _vtkWidget)
	{
		if (0 != layout ( ))
			layout ( )->removeWidget (_vtkWidget);
		if (0 != _vtkWidget->parentWidget ( ))
			_vtkWidget->setParent (0);
		delete _vtkWidget;
	}	// if (0 != _vtkWidget)

	_vtkWidget	= widget;
	if (0 != layout ( ))
		layout ( )->addWidget (_vtkWidget);
			
	_lockableRenderWindow	= dynamic_cast<vtkLockableRenderWindow*>(getVTKWidget ( ).getRenderWindow ( ));
}	// QtVtkGraphicalWidget::setVTKWidget


void QtVtkGraphicalWidget::updateConfiguration ( )
{
	try
	{
		Qt3DGraphicalWidget::updateConfiguration ( );

		if (0 != getVTKWidget ( ).getRenderWindow ( ))
		{
			vtkRenderWindow*	window	= getVTKWidget ( ).getRenderWindow ( );
			_lockableRenderWindow	= dynamic_cast<vtkLockableRenderWindow*>(window);
			CHECK_NULL_PTR_ERROR (window)
			try
			{
				getRenderingManager ( ).setFrameRates (
					Resources::instance ( )._stillFrameRate.getValue ( ),
					Resources::instance ( )._desiredFrameRate.getValue ( ));
			}
			catch (...)
			{
			}
/*			window->SetDesiredUpdateRate (
							Resources::instance ( )._desiredFrameRate.getValue ( ));
			interactor->SetStillUpdateRate (
							Resources::instance ( )._stillFrameRate.getValue ( ));
			interactor->SetDesiredUpdateRate (
							Resources::instance ( )._desiredFrameRate.getValue ( ));
*/
			vtkRenderWindowInteractor*	interactor	= getVTKWidget ( ).getInteractor ( );
			CHECK_NULL_PTR_ERROR (interactor)
			vtkCustomizableInteractorStyleTrackball*	interactorStyle	=
				dynamic_cast<vtkCustomizableInteractorStyleTrackball*>(interactor->GetInteractorStyle ( ));
			if (0 != interactorStyle)
			{
				vtkUnifiedInteractorStyle::upZoom	= Resources::instance ( )._mouseUpZoom;
				interactorStyle->EnableZoomOnWheelEvent (Resources::instance ( )._zoomOnWheel);
			}	// if (0 != interactorStyle)
		}	// if (0 != getVTKWidget ( ).getRenderWindow ( ))
	}	// try
	catch (...)
	{
	}

#ifdef MGX_DEBUG_GRAPHICS
	// ce qui suit n'a pas lieu d'être affiché systématiquement, même en mode développement
	// donc désactivé dans common.xml
	if (0 != getVTKWidget ( ).getRenderWindow ( ))
		getVTKWidget( ).getRenderWindow( )->PrintSelf (cout,*vtkIndent::New( ));
#endif	// MGX_DEBUG_GRAPHICS
}	// QtVtkGraphicalWidget::updateConfiguration



void QtVtkGraphicalWidget::createGui (vtkRenderWindow* window)
{
	_lockableRenderWindow	= dynamic_cast<vtkLockableRenderWindow*>(window);
	QHBoxLayout*	layout	= new QHBoxLayout (this);
	setLayout (layout);
//	QtVtkWidget*	widget	= new QtVtkWidget (this, window);
	QtVtkGraphicWidget*	widget	= new QtVtkGraphicWidget (this);
	CHECK_NULL_PTR_ERROR (widget)
	widget->setRenderWindow (window);
	setVTKWidget (widget);
	updateConfiguration ( );
}	// QtVtkGraphicalWidget::createGui


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtVtkComponents

}	// namespace Mgx3D
