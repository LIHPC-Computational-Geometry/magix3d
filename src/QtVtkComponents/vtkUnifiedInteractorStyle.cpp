#include "Internal/ContextIfc.h"

#include "QtVtkComponents/vtkUnifiedInteractorStyle.h"
#include "QtVtkComponents/VTKECMPicker.h"

#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkPicker.h>
#include <vtkPropPicker.h>
#include <vtkWorldPointPicker.h>
#include <vtkProp.h>
#include <vtkCoordinate.h>

#include <assert.h>


char	vtkUnifiedInteractorStyle::increaseEyeAngleKey	= '>';
char	vtkUnifiedInteractorStyle::decreaseEyeAngleKey	= '<';
char	vtkUnifiedInteractorStyle::toggleStereoKey	= '3';
double	vtkUnifiedInteractorStyle::eyeAngleModifier	= 0.25;		// en degres
bool	vtkUnifiedInteractorStyle::upZoom		= false;

vtkUnifiedInteractorStyle::vtkUnifiedInteractorStyle ( )
	: vtkInteractorStyleTrackballCamera ( ), _motionRatio (0.1)
{
}	// vtkUnifiedInteractorStyle::vtkUnifiedInteractorStyle


vtkUnifiedInteractorStyle::vtkUnifiedInteractorStyle (
		const vtkUnifiedInteractorStyle&)
	: vtkInteractorStyleTrackballCamera ( ), _motionRatio (0.1)
{
	assert (0 && "vtkUnifiedInteractorStyle copy constructor is not allowed.");
}	// vtkUnifiedInteractorStyle copy constructor


vtkUnifiedInteractorStyle& vtkUnifiedInteractorStyle::operator = (
		const vtkUnifiedInteractorStyle&)
{
	assert (0 && "vtkUnifiedInteractorStyle::operator = is not allowed.");
	return *this;
}	// vtkUnifiedInteractorStyle::operator =


vtkUnifiedInteractorStyle::~vtkUnifiedInteractorStyle ( )
{
}	// vtkUnifiedInteractorStyle::~vtkUnifiedInteractorStyle


void vtkUnifiedInteractorStyle::PrintSelf (ostream& os, vtkIndent indent)
{
	vtkInteractorStyleTrackballCamera::PrintSelf (os, indent);
}	// vtkUnifiedInteractorStyle::PrintSelf


void vtkUnifiedInteractorStyle::OnChar ( )
{
	vtkRenderWindowInteractor*	interactor	= this->Interactor;
	assert ((0 != interactor) && "vtkUnifiedInteractorStyle::OnChar : null interactor.");

	switch (interactor->GetKeyCode ( ))
	{
		case 'f'	:	// Jamais appele, c'est toujours 'F'
		case 'F'	:
			FlyTo (false == isShiftKeyPressed ( ) ? true : false);
			break;
		case 'x'	:
		case 'X'	:
			DisplayyOzViewPlane ( );
			break;
		case 'y'	:
		case 'Y'	:
			DisplayxOzViewPlane ( );
			break;
		case 'z'	:
		case 'Z'	:
			DisplayxOyViewPlane ( );
			break;
		case 'a'	:	// Suppression du roulis
		case 'A'	:
			ResetRoll ( );
			break;
		case 'r'	:
		case 'R'	:	// Reset. Annulation au prealable d'un eventuel zoom.
			ResetView ( );
			break;
		case '3'	:	// stereo <-> non stereo : on l'annule => traite plus loin
			break;
		default		:
			vtkInteractorStyleTrackballCamera::OnChar ( );
	}	// switch (interactor->GetKeyCode ( ))
}	// vtkUnifiedInteractorStyle::OnChar


void vtkUnifiedInteractorStyle::OnMouseMove ( )
{
	switch (this->State)
	{
		case VTKIS_ROTATE	:
			this->Rotate ( );
			this->InvokeEvent (vtkCommand::InteractionEvent, NULL);
			break;
		case VTKIS_PAN		:
			this->Pan ( );
			this->InvokeEvent (vtkCommand::InteractionEvent, NULL);
			break;
		case VTKIS_DOLLY	:
			this->Dolly ( );
			this->InvokeEvent (vtkCommand::InteractionEvent, NULL);
			break;
		case VTKIS_SPIN		:
			this->Spin ( );
			this->InvokeEvent (vtkCommand::InteractionEvent, NULL);
			break;
	}	// switch (this->State)
}	// vtkUnifiedInteractorStyle::OnMouseMove


void vtkUnifiedInteractorStyle::Dolly (double factor)
{
	double	f	= true == upZoom ? factor : 1. / factor;

	vtkInteractorStyleTrackballCamera::Dolly (f);
}   // vtkUnifiedInteractorStyle::Dolly


void vtkUnifiedInteractorStyle::Dolly ( )
{
	vtkInteractorStyleTrackballCamera::Dolly ( );
}	// vtkUnifiedInteractorStyle::Dolly


vtkUnifiedInteractorStyle* vtkUnifiedInteractorStyle::New ( )
{
	vtkObject*	object	= 
		vtkObjectFactory::CreateInstance ("vtkUnifiedInteractorStyle");
	if (0 != object)
		return (vtkUnifiedInteractorStyle*)object;

	return new vtkUnifiedInteractorStyle ( );
}	// vtkUnifiedInteractorStyle::New


bool vtkUnifiedInteractorStyle::isControlKeyPressed ( )
{
	vtkRenderWindowInteractor*	interactor	= GetInteractor ( );
	if (0 == interactor)
	{
		vtkWarningMacro(<<"vtkUnifiedInteractorStyle::isControlKeyPressed : null interactor.");
		return false;
	}

	return 0 != interactor->GetControlKey ( ) ? true : false;
}	// vtkUnifiedInteractorStyle::isControlKeyPressed


bool vtkUnifiedInteractorStyle::isShiftKeyPressed ( )
{
	vtkRenderWindowInteractor*	interactor	= GetInteractor ( );
	if (0 == interactor)
	{
		vtkWarningMacro("vtkUnifiedInteractorStyle::isShiftKeyPressed : null interactor.");
		return false;
	}

	return 0 != interactor->GetShiftKey ( ) ? true : false;
}	// vtkUnifiedInteractorStyle::isShiftKeyPressed


void vtkUnifiedInteractorStyle::OnKeyRelease ( )
{
	vtkRenderWindowInteractor*	interactor		= this->Interactor;
	vtkRenderWindow*			renderWindow	= (0 == CurrentRenderer ? 
						0 : CurrentRenderer->GetRenderWindow ( ));
	assert ((0 != interactor) && "vtkUnifiedInteractorStyle::OnKeyRelease : null interactor.");
	char keycode = interactor->GetKeyCode ( );

	if (keycode == increaseEyeAngleKey)
		IncreaseEyeAngle (true);
	else if (keycode == decreaseEyeAngleKey)
		IncreaseEyeAngle (false);
	else if (keycode == toggleStereoKey && 0 != renderWindow)
	{
		
		if (renderWindow->GetStereoRender()) 
		{
			renderWindow->StereoRenderOff();
		}
		else
		{
			renderWindow->StereoRenderOn();
		}
		renderWindow->Render();
	}	// else if (keycode == toggleStereoKey)

	switch (interactor->GetKeyCode ( ))
	{
		case (char)0x1012			:	// Qt Key_Left
			OnLeftArrow ( );
			break;
		case (char)0x1013			:	// Qt Key_Up
			OnUpArrow ( );
			break;
		case (char)0x1014			:	// Qt Key_Right
			OnRightArrow ( );
			break;
		case (char)0x1015			:	// Qt Key_Down
			OnDownArrow ( );
			break;
		default						:
			vtkInteractorStyleTrackballCamera::OnKeyRelease ( );
	}	// switch (interactor->GetKeyCode ( ))
}	// vtkUnifiedInteractorStyle::OnKeyRelease


void vtkUnifiedInteractorStyle::SetMotionRatio (double ratio)
{
	if (0. >= ratio)
		return;

	_motionRatio	= ratio;
}	// vtkUnifiedInteractorStyle::SetMotionRatio


void vtkUnifiedInteractorStyle::OnLeftArrow ( )
{
	vtkRenderWindowInteractor*	interactor	= this->Interactor;
	assert ((0 != interactor) && "vtkUnifiedInteractorStyle::OnLeftArrow : null interactor.");
	
	int		lastX	= 0, lastY	= 0, newX = 0;
	int		width	= 0, height	= 0;
	interactor->GetEventPosition (lastX, lastY);
	interactor->GetSize (width, height);
	int		delta	= ((int)((double)width * GetMotionRatio ( )));
	newX			= lastX - delta;
	interactor->SetEventPosition (newX, lastY);
	FindPokedRenderer (newX, lastY);
	Pan ( );
	InvokeEvent (vtkCommand::InteractionEvent, NULL);
}	// vtkUnifiedInteractorStyle::OnLeftArrow


void vtkUnifiedInteractorStyle::OnUpArrow ( )
{
	vtkRenderWindowInteractor*	interactor	= this->Interactor;
	assert ((0 != interactor) && "vtkUnifiedInteractorStyle::OnUpArrow : null interactor.");
	
	int		lastX	= 0, lastY	= 0, newY = 0;
	int		width	= 0, height	= 0;
	interactor->GetEventPosition (lastX, lastY);
	interactor->GetSize (width, height);
	int		delta	= ((int)((double)height * GetMotionRatio ( )));
	newY			= lastY + delta;
	interactor->SetEventPosition (lastX, newY);
	FindPokedRenderer (lastX, newY);
	Pan ( );
	InvokeEvent (vtkCommand::InteractionEvent, NULL);
}	// vtkUnifiedInteractorStyle::OnUpArrow


void vtkUnifiedInteractorStyle::OnRightArrow ( )
{
	vtkRenderWindowInteractor*	interactor	= this->Interactor;
	assert ((0 != interactor) && "vtkUnifiedInteractorStyle::OnRightArrow : null interactor.");
	
	int		lastX	= 0, lastY	= 0, newX = 0;
	int		width	= 0, height	= 0;
	interactor->GetEventPosition (lastX, lastY);
	interactor->GetSize (width, height);
	int		delta	= ((int)((double)width * GetMotionRatio ( )));
	newX			= lastX + delta;
	interactor->SetEventPosition (newX, lastY);
	FindPokedRenderer (newX, lastY);
	Pan ( );
	InvokeEvent (vtkCommand::InteractionEvent, NULL);
}	// vtkUnifiedInteractorStyle::OnRightArrow


void vtkUnifiedInteractorStyle::OnDownArrow ( )
{
	vtkRenderWindowInteractor*	interactor	= this->Interactor;
	assert ((0 != interactor) && "vtkUnifiedInteractorStyle::OnDownArrow : null interactor.");
	
	int		lastX	= 0, lastY	= 0, newY = 0;
	int		width	= 0, height	= 0;
	interactor->GetEventPosition (lastX, lastY);
	interactor->GetSize (width, height);
	int		delta	= ((int)((double)height * GetMotionRatio ( )));
	newY			= lastY - delta;
	interactor->SetEventPosition (lastX, newY);
	FindPokedRenderer (lastX, newY);
	Pan ( );
	InvokeEvent (vtkCommand::InteractionEvent, NULL);
}	// vtkUnifiedInteractorStyle::OnDownArrow


void vtkUnifiedInteractorStyle::IncreaseEyeAngle (bool increase)
{
	vtkRenderWindowInteractor*	interactor		= this->Interactor;
	vtkRenderWindow*			renderWindow	= (0 == CurrentRenderer ? 0 : CurrentRenderer->GetRenderWindow ( ));
	assert ((0 != interactor) && "vtkUnifiedInteractorStyle::IncreaseEyeAngle : null interactor.");
	FindPokedRenderer (interactor->GetEventPosition ( )[0], interactor->GetEventPosition ( )[1]);
	vtkCamera*	camera	= 0 == CurrentRenderer ?  0 : CurrentRenderer->GetActiveCamera ( );
	if ((0 == camera) || (0 == renderWindow) || (0 == renderWindow->GetStereoRender ( )))
		return;

	double	angle	= camera->GetEyeAngle ( );
	if (true == increase)
		angle	+= eyeAngleModifier;
	else	
		angle	-= eyeAngleModifier;

	if (0 != renderWindow)
		renderWindow->Render ( );
}	// vtkUnifiedInteractorStyle::IncreaseEyeAngle


void vtkUnifiedInteractorStyle::FlyTo (bool centerOnActor)
{
	vtkRenderWindowInteractor*	interactor	= this->Interactor;
	assert ((0 != interactor) && "vtkUnifiedInteractorStyle::FlyTo : null interactor.");
	this->AnimState	= VTKIS_ANIM_ON;
	// On utilise un autre picker afin de ne pas toucher à la selection.
	// On prend un VTKECMPicker qui retiendra l'acteur le + proche du point
	// pické.
//	vtkPropPicker*	picker	= vtkPropPicker::New ( );
	VTKECMPicker*	picker	= VTKECMPicker::New ( );
	assert (0 != picker);
	// SetMode (VTKECMPicker::DISTANCE) : plus précis, a priori OK en phase
	// CAO. Mais fait appel à un vtkCellPicker qui risque d'être très couteux en
	// mode maillage.
	picker->SetMode (VTKECMPicker::DISTANCE);

	FindPokedRenderer (interactor->GetEventPosition ( )[0],
	                   interactor->GetEventPosition ( )[1]);
	picker->Pick(interactor->GetEventPosition ( )[0],
	             interactor->GetEventPosition ( )[1],
	             0.0, CurrentRenderer);
	vtkProp*	prop	= picker->GetViewProp ( );
	if (0 == prop)
	{
		picker->Delete ( );
		return;
	}	// if (0 == prop)

	vtkFloatingPointType	rotationCenter [3];
	if (true == centerOnActor)
	{
		vtkFloatingPointType*	bounds = prop->GetBounds ( );
assert ((0 != bounds) && "vtkUnifiedInteractorStyle::FlyTo : null bounds");
		rotationCenter [0]	= bounds[0] + (bounds[1] - bounds[0]) / 2.;
		rotationCenter [1]	= bounds[2] + (bounds[3] - bounds[2]) / 2.;
		rotationCenter [2]	= bounds[4] + (bounds[5] - bounds[4]) / 2.;
	}
	else	// if (true == centerOnActor)
	{
//		vtkWorldPointPicker*	picker	= vtkWorldPointPicker::New ( );
		// Que picker soit un VTKECMPicker ou un vtkPropPicker il connait les
		// coordonnées du point pické sur l'acteur.
		// - Si VTKECMPicker alors repose sur un vtkCellPicker => perfs non
		// optimales, qui risquent d'accuser le coup avec des maillages.
		// - Si vtkPropPicker le picking se fait à la boite englobante, puis
		// avec un WorldPointPicker pour déterminer le z à l'aide du zbuffer.
/*
		picker->Pick (interactor->GetEventPosition ( )[0],
		              interactor->GetEventPosition ( )[1],
		              0, CurrentRenderer);*/
		picker->GetPickPosition (rotationCenter);
//		picker->Delete ( );
	}	// else if (true == centerOnActor)
	interactor->FlyTo (CurrentRenderer, rotationCenter);
	this->AnimState	= VTKIS_ANIM_OFF;
	picker->Delete ( );
	InvokeEvent (vtkCommand::InteractionEvent, NULL);
}	// vtkUnifiedInteractorStyle::FlyTo


void vtkUnifiedInteractorStyle::DisplayxOyViewPlane ( )
{
	vtkCamera*	camera	= 0 == CurrentRenderer ? 
						  0 : CurrentRenderer->GetActiveCamera ( );
	if (0 == camera)
		return;

	double	focal [3], position [3];
	camera->GetFocalPoint (focal);
	camera->GetPosition (position);
	double distance = sqrt ((position[0] - focal[0])*(position[0] - focal[0]) +
							(position[1] - focal[1])*(position[1] - focal[1]) +
							(position[2] - focal[2])*(position[2] - focal[2]));
	position [0]	= focal [0];
	position [1]	= focal [1];
	position [2]	= focal [2] + (position[2] < focal [2] ? -distance : distance);
	camera->SetPosition (position);
	camera->ComputeViewPlaneNormal ( );
	if (0 != CurrentRenderer->GetRenderWindow ( ))
		CurrentRenderer->GetRenderWindow ( )->Render ( );
	InvokeEvent (vtkCommand::InteractionEvent, NULL);
}	// vtkUnifiedInteractorStyle::DisplayxOyViewPlane


void vtkUnifiedInteractorStyle::DisplayxOzViewPlane ( )
{
	vtkCamera*  camera  = 0 == CurrentRenderer ?
						  0 : CurrentRenderer->GetActiveCamera ( );
	if (0 == camera)
		return;

	double	focal [3], position [3];
	camera->GetFocalPoint (focal);
	camera->GetPosition (position);
	double	distance = sqrt ((position[0] - focal[0])*(position[0] - focal[0]) +
					         (position[1] - focal[1])*(position[1] - focal[1]) +
					         (position[2] - focal[2])*(position[2] - focal[2]));
	position [0]    = focal [0];
	position [1]    = focal [1] + (position[1] < focal [1] ? -distance : distance);
	position [2]    = focal [2];
	camera->SetPosition (position);
	camera->ComputeViewPlaneNormal ( );
	if (0 != CurrentRenderer->GetRenderWindow ( ))
		CurrentRenderer->GetRenderWindow ( )->Render ( );
	InvokeEvent (vtkCommand::InteractionEvent, NULL);
}	// vtkUnifiedInteractorStyle::DisplayxOzViewPlane


void vtkUnifiedInteractorStyle::DisplayyOzViewPlane ( )
{
	vtkCamera*	camera	= 0 == CurrentRenderer ? 
						  0 : CurrentRenderer->GetActiveCamera ( );
	if (0 == camera)
		return;

	double	focal [3], position [3];
	camera->GetFocalPoint (focal);
	camera->GetPosition (position);
	double distance = sqrt ((position[0] - focal[0])*(position[0] - focal[0]) +
							(position[1] - focal[1])*(position[1] - focal[1]) +
							(position[2] - focal[2])*(position[2] - focal[2]));
	position [0]	= focal [0] + (position[0] < focal [0] ? -distance : distance);
	position [1]	= focal [1];
	position [2]	= focal [2];
	camera->SetPosition (position);
	camera->ComputeViewPlaneNormal ( );
	if (0 != CurrentRenderer->GetRenderWindow ( ))
		CurrentRenderer->GetRenderWindow ( )->Render ( );
	InvokeEvent (vtkCommand::InteractionEvent, NULL);
}	// vtkUnifiedInteractorStyle::DisplayyOzViewPlane


void vtkUnifiedInteractorStyle::ResetRoll ( )
{
	vtkCamera*	camera	= 0 == CurrentRenderer ?
						  0 : CurrentRenderer->GetActiveCamera ( );
	if (0 != camera)
		camera->SetRoll (0.);
	if ((0 != CurrentRenderer) && 
	    (0 != CurrentRenderer->GetRenderWindow ( )))
		CurrentRenderer->GetRenderWindow ( )->Render ( );
	InvokeEvent (vtkCommand::InteractionEvent, NULL);
}	// vtkUnifiedInteractorStyle::ResetRoll


void vtkUnifiedInteractorStyle::ResetView ( )
{
	vtkCamera*	camera	= 0 == CurrentRenderer ?
						  0 : CurrentRenderer->GetActiveCamera ( );
	if (0 != camera)
	{
		camera->SetViewAngle (30.);
		camera->SetParallelScale (1.);	
	}	// if (0 != camera)

//	vtkInteractorStyleTrackballCamera::OnChar ( );
	if (0 != CurrentRenderer)
	{
		CurrentRenderer->ResetCamera ( );
		if (NULL != this->Interactor)
			this->Interactor->Render ( );
	}
	InvokeEvent (vtkCommand::InteractionEvent, NULL);
}	// vtkUnifiedInteractorStyle::ResetView

