//
// Created by charles on 04/05/2020.
//

#include "VtkComponents/vtkDMASynchronizedOpenGLRenderWindow.h"
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>


vtkDMASynchronizedOpenGLRenderWindow::vtkDMASynchronizedOpenGLRenderWindow ( )
	: vtkGenericOpenGLRenderWindow ( )
{
}	// vtkDMASynchronizedOpenGLRenderWindow::vtkDMASynchronizedOpenGLRenderWindow


vtkDMASynchronizedOpenGLRenderWindow::vtkDMASynchronizedOpenGLRenderWindow (const vtkDMASynchronizedOpenGLRenderWindow&)
	: vtkGenericOpenGLRenderWindow ( )
{
}	// vtkDMASynchronizedOpenGLRenderWindow::vtkDMASynchronizedOpenGLRenderWindow


vtkDMASynchronizedOpenGLRenderWindow& vtkDMASynchronizedOpenGLRenderWindow::operator = (const vtkDMASynchronizedOpenGLRenderWindow&)

{
	return *this;
}	// vtkDMASynchronizedOpenGLRenderWindow::operator =


vtkDMASynchronizedOpenGLRenderWindow::~vtkDMASynchronizedOpenGLRenderWindow ( )
{

}	// vtkDMASynchronizedOpenGLRenderWindow::~vtkDMASynchronizedOpenGLRenderWindow


vtkDMASynchronizedOpenGLRenderWindow* vtkDMASynchronizedOpenGLRenderWindow::New ( )
{
	return new vtkDMASynchronizedOpenGLRenderWindow ( );
}	// vtkDMASynchronizedOpenGLRenderWindow::New


void vtkDMASynchronizedOpenGLRenderWindow::Render ( )	// Code issu de vtkRenderWindow::Render v 7.1.1 dont tout ce qui est affichage d'objets locaux a été enlevé (éviter flickering).
{
#ifdef VTK_8
	if (this->ReadyForRendering)
	{
#endif	// VTK_8
	int *size;
	int x,y;
	float *p1;

	// if we are in the middle of an abort check then return now
	if (this->InAbortCheck)
	{
		return;
	}

	// if we are in a render already from somewhere else abort now
	if (this->InRender)
	{
		return;
	}

	// if SetSize has not yet been called (from a script, possible off
	// screen use, other scenarios?) then call it here with reasonable
	// default values
	if (0 == this->Size[0] && 0 == this->Size[1])
	{
		this->SetSize(300, 300);
	}

	// reset the Abort flag
	this->AbortRender = 0;
	this->InRender = 1;

	vtkDebugMacro(<< "Starting Render Method.\n");
	this->InvokeEvent(vtkCommand::StartEvent,NULL);

	this->NeverRendered = 0;

	if ( this->Interactor && ! this->Interactor->GetInitialized() )
	{
		this->Interactor->Initialize();
	}

	this->InRender = 0;
	this->InvokeEvent(vtkCommand::EndEvent,NULL);
#ifdef VTK_8
	}	// if (this->ReadyForRendering)
#endif	// VTK_8
}	// vtkDMASynchronizedOpenGLRenderWindow::Render
