/**
 * \file		vtkXLockableRenderWindow.cpp
 * \author		Charles PIGNEROL
 * \date		04/12/2020
 */

#include "VtkComponents/vtkXLockableRenderWindow.h"

#include <assert.h>


vtkXLockableRenderWindow::vtkXLockableRenderWindow ( )
	: vtkXOpenGLRenderWindow ( ), Locked (false)
{
}	// vtkXLockableRenderWindow::vtkXLockableRenderWindow


vtkXLockableRenderWindow::vtkXLockableRenderWindow (const vtkXLockableRenderWindow&)
	: vtkXOpenGLRenderWindow ( ), Locked (false)
{
	assert (0 && "vtkXLockableRenderWindow copy constructor is not allowed.");
}	// vtkXLockableRenderWindow::vtkXLockableRenderWindow


vtkXLockableRenderWindow& vtkXLockableRenderWindow::operator = (const vtkXLockableRenderWindow&)
{
	assert (0 && "vtkXLockableRenderWindow assignment operator is not allowed.");
	return *this;
}	// vtkXLockableRenderWindow::operator =


vtkXLockableRenderWindow::~vtkXLockableRenderWindow ( )
{
}	// vtkXLockableRenderWindow::vtkXLockableRenderWindow


vtkXLockableRenderWindow* vtkXLockableRenderWindow::New ( )
{
	return new vtkXLockableRenderWindow ( );
}	// vtkXLockableRenderWindow::New


void vtkXLockableRenderWindow::PrintSelf (ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	os << indent << "Locked: " << (int)this->Locked << "\n";
}	// vtkXLockableRenderWindow::PrintSelf


void vtkXLockableRenderWindow::Render ( )
{
	if (true == Locked)
		return;

	vtkXOpenGLRenderWindow::Render ( );
}	// vtkXLockableRenderWindow::Render


void vtkXLockableRenderWindow::LockRendering (bool lock)
{	
	Locked	= lock;
}	// vtkXLockableRenderWindow::lock


bool vtkXLockableRenderWindow::LockRendering ( )
{
	return Locked;
}	// vtkXLockableRenderWindow::unlock


XRenderWindowLock::XRenderWindowLock (vtkXLockableRenderWindow* window)
	: RenderWindow (window)
{
	lock ( );
}	// XRenderWindowLock::XRenderWindowLock


XRenderWindowLock::XRenderWindowLock (const XRenderWindowLock&)
{
	assert (0 && "XRenderWindowLock copy constructor is not allowed.");
}	// XRenderWindowLock::XRenderWindowLock


XRenderWindowLock& XRenderWindowLock::operator = (const XRenderWindowLock&)
{
	assert (0 && "XRenderWindowLock assignment operator is not allowed.");
	return *this;
}	// XRenderWindowLock::operator =


XRenderWindowLock::~XRenderWindowLock ( )
{
	unlock ( );
}	// XRenderWindowLock::~XRenderWindowLock


void XRenderWindowLock::lock ( )
{
	if (0 != RenderWindow)
		RenderWindow->LockRendering (true);
}	// XRenderWindowLock::lock


void XRenderWindowLock::unlock ( )
{
	if (0 != RenderWindow)
		RenderWindow->LockRendering (false);
}	// XRenderWindowLock::unlock

