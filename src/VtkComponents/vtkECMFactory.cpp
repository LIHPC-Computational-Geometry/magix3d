/**
 * \file		vtkECMFactory.cpp
 * \author		Charles PIGNEROL
 * \date		21/06/2012
 *
 * \warning		Fournit la classe <I>vtkECMXOpenGLRenderWindow</I> pourles
 *				rendus off-screen.
 */

#include "VtkComponents/vtkECMFactory.h"
#include "VtkComponents/vtkECMXOpenGLRenderWindow.h"
#include "VtkComponents/vtkDMAParallelRenderManager.h"

#include <vtkVersion.h>
#ifdef VTK_USE_X    // Eventuellement défini dans vtkRenderingOpenGLConfigure.h de la distribution VTK.
#include <vtkXOpenGLRenderWindow.h>
#endif  // VTK_USE_X

#include <assert.h>
#include <iostream>


using namespace std;


void restoreMapState (vtkRenderWindow& window)
{
#ifdef VTK_USE_X
	vtkXOpenGLRenderWindow*    xw    =
					dynamic_cast<vtkXOpenGLRenderWindow*>(&window);
	if (0 != xw)
	{
		if (0 == window.GetMapped ( ))
			XUnmapWindow (xw->GetDisplayId ( ), xw->GetWindowId ( ));
		else
			XMapWindow (xw->GetDisplayId ( ), xw->GetWindowId ( ));

		XSync (xw->GetDisplayId ( ), False);
	}	// if (0 != xw)
#endif  // VTK_USE_X
}	// restoreMapState


// ============================================================================================================

#ifdef VTK_USE_X
#ifdef VTK_5
VTK_CREATE_CREATE_FUNCTION (vtkECMXOpenGLRenderWindow);
#endif	// VTK_5
#endif  // VTK_USE_X

VTK_CREATE_CREATE_FUNCTION (vtkDMAParallelRenderManager);


vtkECMFactory::vtkECMFactory ( )
{
	cout << "Using vtkECMFactory that provides :" << endl;


/* // Activé, sur certaines station de travail récentes (juin 2012), provoque
	// l'affichage des découpes de quads en triangles. 
	// => On l'inactive car le besoin est avant tout sur calculateur, en mode
	// off-screen, que la render window ne soit pas affichée.
	// Activé on ne voit que la fenêtre de rendu du processeur MPI maître.
	// MAIS inactivé :
	// - On voit la fenêtre de rendu de chaque processeur
	// - On ne voit pas les fenêtres se redimensionner (lorsque c'est provoqué
	// côté client)
	// CP, le 01/10/12 : Problème apparemment élucidé, semble provenir d'un
	// appel à _renderWindow->PolygonSmoothingOn ( ) qui provoque cet effet
	// lorsqu'on utilise des QVTKWidget.
*/
/*
#ifdef VTK_USE_X
#ifdef VTK_5
	this->RegisterOverride("vtkXOpenGLRenderWindow",
	                       "vtkECMXOpenGLRenderWindow",
	                       "Offscreen window not mapped.",
	                       1,
	                       vtkObjectFactoryCreatevtkECMXOpenGLRenderWindow);
	cout << "- vtkECMXOpenGLRenderWindow that overload vtkXOpenGLRenderWindow in "
	     << "order to provide unmapped render window in offscreen rendering." << endl
	     << endl;
#endif	// VTK_5
#endif  // VTK_USE_X
*/


	this->RegisterOverride("vtkCompositeRenderManager",
	                       "vtkDMAParallelRenderManager",
	                       "Corrects bug when bounds of a render windows are not initialized (e.g. when nothing is displayed).",
	                       1,
	                       vtkObjectFactoryCreatevtkDMAParallelRenderManager);
	cout << "- vtkDMAParallelRenderManager that overload vtkCompositeRenderManager in "
	     << "order to corrects a bug when bounds of a render windows are not initialized (e.g. when nothing is displayed)." << endl
	     << endl;
}	// vtkECMFactory::vtkECMFactory


vtkECMFactory::vtkECMFactory (const vtkECMFactory&)
	: vtkObjectFactory ( )
{
	assert (0 && "vtkECMFactory copy constructor is not allowed.");
}	// vtkECMFactory::vtkECMFactory (const vtkECMFactory&)


vtkECMFactory& vtkECMFactory::operator = (const vtkECMFactory&)
{
	assert (0 && "vtkECMFactory assignment operator is not allowed.");
	return *this;
}	// vtkECMFactory::operator = (const vtkECMFactory&)


vtkECMFactory::~vtkECMFactory ( )
{
}	// vtkECMFactory::~vtkECMFactory


const char* vtkECMFactory::GetVTKSourceVersion ( )
{
	return VTK_SOURCE_VERSION;
}	// vtkECMFactory::GetVTKSourceVersion


const char* vtkECMFactory::GetDescription ( )
{
	return "ECM object factory.";
}	// vtkECMFactory::GetDescription


vtkECMFactory* vtkECMFactory::New ( )
{
	vtkECMFactory*	factory	= new vtkECMFactory ( );
#ifndef VTK_5
    	factory->InitializeObjectBase ( );
#endif  // VTK_5

	return factory;
}	// vtkECMFactory::New

