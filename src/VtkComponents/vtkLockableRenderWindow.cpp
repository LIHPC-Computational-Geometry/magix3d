/**
 * \file		vtkLockableRenderWindow.cpp
 * \author		Charles PIGNEROL
 * \date		19/11/2018
 */

#include "VtkComponents/vtkLockableRenderWindow.h"

#include <assert.h>


vtkLockableRenderWindow::vtkLockableRenderWindow ( )
#if VTK_MAJOR_VERSION < 8
#	ifdef VTK_USE_X
	: vtkECMXOpenGLRenderWindow ( ),
#	else
	: vtkOSOpenGLRenderWindow ( ),
#	endif  // VTK_USE_X
#else 	// VTK_MAJOR_VERSION < 8
	: vtkDMASynchronizedOpenGLRenderWindow ( ),
#endif	// VTK_MAJOR_VERSION < 8
	  Locked (false)
{
}	// vtkLockableRenderWindow::vtkLockableRenderWindow


vtkLockableRenderWindow::vtkLockableRenderWindow (const vtkLockableRenderWindow&)
#if VTK_MAJOR_VERSION < 8
#	ifdef VTK_USE_X
	: vtkECMXOpenGLRenderWindow ( ),
#	else
	: vtkOSOpenGLRenderWindow ( ),
#	endif  // VTK_USE_X
#else 	// VTK_MAJOR_VERSION < 8
	: vtkDMASynchronizedOpenGLRenderWindow ( ),
#endif	// VTK_MAJOR_VERSION < 8
	  Locked (false)
{
	assert (0 && "vtkLockableRenderWindow copy constructor is not allowed.");
}	// vtkLockableRenderWindow::vtkLockableRenderWindow


vtkLockableRenderWindow& vtkLockableRenderWindow::operator = (const vtkLockableRenderWindow&)
{
	assert (0 && "vtkLockableRenderWindow assignment operator is not allowed.");
	return *this;
}	// vtkLockableRenderWindow::operator =


vtkLockableRenderWindow::~vtkLockableRenderWindow ( )
{
}	// vtkLockableRenderWindow::vtkLockableRenderWindow


vtkLockableRenderWindow* vtkLockableRenderWindow::New ( )
{
	return new vtkLockableRenderWindow ( );
}	// vtkLockableRenderWindow::New


void vtkLockableRenderWindow::PrintSelf (ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	os << indent << "Locked: " << (int)this->Locked << "\n";
}	// vtkLockableRenderWindow::PrintSelf


void vtkLockableRenderWindow::Render ( )
{
cout << __FILE__ << ' ' << __LINE__ << " vtkLockableRenderWindow::Render" << endl;
	if (true == Locked)
		return;

#if VTK_MAJOR_VERSION < 8
#	ifdef VTK_USE_X
	vtkECMXOpenGLRenderWindow::Render ( );
#	else
	vtkOSOpenGLRenderWindow::Render ( );
#	endif  // VTK_USE_X
#else 	// VTK_MAJOR_VERSION < 8
cout << __FILE__ << ' ' << __LINE__ << " vtkLockableRenderWindow::Render" << endl;
	vtkDMASynchronizedOpenGLRenderWindow::Render ( );
cout << __FILE__ << ' ' << __LINE__ << " vtkLockableRenderWindow::Render" << endl;
#endif	// VTK_MAJOR_VERSION < 8

}	// vtkLockableRenderWindow::Render


void vtkLockableRenderWindow::LockRendering (bool lock)
{	
	Locked	= lock;
}	// vtkLockableRenderWindow::lock


bool vtkLockableRenderWindow::LockRendering ( )
{
	return Locked;
}	// vtkLockableRenderWindow::unlock


RenderWindowLock::RenderWindowLock (vtkLockableRenderWindow* window)
	: RenderWindow (window)
{
	lock ( );
}	// RenderWindowLock::RenderWindowLock


RenderWindowLock::RenderWindowLock (const RenderWindowLock&)
{
	assert (0 && "RenderWindowLock copy constructor is not allowed.");
}	// RenderWindowLock::RenderWindowLock


RenderWindowLock& RenderWindowLock::operator = (const RenderWindowLock&)
{
	assert (0 && "RenderWindowLock assignment operator is not allowed.");
	return *this;
}	// RenderWindowLock::operator =


RenderWindowLock::~RenderWindowLock ( )
{
	unlock ( );
}	// RenderWindowLock::~RenderWindowLock


void RenderWindowLock::lock ( )
{
	if (0 != RenderWindow)
		RenderWindow->LockRendering (true);
}	// RenderWindowLock::lock


void RenderWindowLock::unlock ( )
{
	if (0 != RenderWindow)
		RenderWindow->LockRendering (false);
}	// RenderWindowLock::unlock

