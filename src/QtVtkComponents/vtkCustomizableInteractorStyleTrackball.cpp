#include "Internal/ContextIfc.h"

#include "QtVtkComponents/vtkCustomizableInteractorStyleTrackball.h"

#include <vtkObjectFactory.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkProp.h>
#include <vtkRenderWindow.h>

#include <string>
#include <assert.h>


vtkCustomizableInteractorStyleTrackball::vtkCustomizableInteractorStyleTrackball ( )
	: vtkMgx3DInteractorStyle ( ),
	  _displayBoundingBox (true), _zoomOnWheelEvent (false), _zoomFactor (1.1)
{
}	// vtkCustomizableInteractorStyleTrackball::vtkCustomizableInteractorStyleTrackball


vtkCustomizableInteractorStyleTrackball::vtkCustomizableInteractorStyleTrackball (
		const vtkCustomizableInteractorStyleTrackball&)
	: vtkMgx3DInteractorStyle ( ),
	  _displayBoundingBox (true), _zoomOnWheelEvent (false), _zoomFactor (1.1)
{
	assert (0 && "vtkCustomizableInteractorStyleTrackball copy constructor is not allowed.");
}	// vtkCustomizableInteractorStyleTrackball copy constructor


vtkCustomizableInteractorStyleTrackball& vtkCustomizableInteractorStyleTrackball::operator = (
		const vtkCustomizableInteractorStyleTrackball&)
{
	assert (0 && "vtkCustomizableInteractorStyleTrackball::operator = is not allowed.");
	return *this;
}	// vtkCustomizableInteractorStyleTrackball::operator =


vtkCustomizableInteractorStyleTrackball::~vtkCustomizableInteractorStyleTrackball ( )
{
}	// vtkCustomizableInteractorStyleTrackball::~vtkCustomizableInteractorStyleTrackball


void vtkCustomizableInteractorStyleTrackball::OnChar ( )
{
	vtkRenderWindowInteractor*	interactor	= this->Interactor;
	assert ((0 != interactor) && "vtkCustomizableInteractorStyleTrackball::OnChar : null interactor.");
// SetKeyCode (GetKeySym ( )) : issu de Lem, vtk 5.1.0/Qt 4.6. Pourquoi ???
// Commenté dans Magix car, en sa présence, TAB remplacé par T.
//	if (0 != interactor->GetKeySym ( ))
//		interactor->SetKeyCode (*interactor->GetKeySym ( ));
	// S'assurer d'avoir un renderer dans vtkMgx3DInteractorStyle::OnChar ( ) :
	FindPokedRenderer (interactor->GetEventPosition ( )[0], interactor->GetEventPosition ( )[1]);
	vtkMgx3DInteractorStyle::OnChar ( );
}	// vtkCustomizableInteractorStyleTrackball::OnChar


void vtkCustomizableInteractorStyleTrackball::OnLeftButtonDown ( )
{
	if ((true == isShiftKeyPressed ( )) && (true == isControlKeyPressed ( )))
	{
		assert (VTKIS_NONE == State);
		State	= VTKIS_GUI_ZOOM;
		assert (0 != Interactor);
		int			x		= 0, y	= 0;
		Interactor->GetEventPosition (x, y);
		FindPokedRenderer (x, y);
		vtkCamera*	camera	= 0 == CurrentRenderer ? 0 : CurrentRenderer->GetActiveCamera ( );
		setGuiZoomParameters (x, y, this);
		InvokeEvent(vtkCommand::StartInteractionEvent,&getGuiZoomParameters( ));
	}
	else
		vtkMgx3DInteractorStyle::OnLeftButtonDown ( );
}	// vtkCustomizableInteractorStyleTrackball::OnLeftButtonDown


void vtkCustomizableInteractorStyleTrackball::OnLeftButtonUp ( )
{
	if (VTKIS_GUI_ZOOM != State)
	{
		vtkMgx3DInteractorStyle::OnLeftButtonUp ( );
		return;
	}

	State	= VTKIS_NONE;
	assert (0 != Interactor);
	int			x		= 0, y	= 0;
	Interactor->GetEventPosition (x, y);
	FindPokedRenderer (x, y);
	vtkCamera*	camera	= 0 == CurrentRenderer ? 0 : CurrentRenderer->GetActiveCamera ( );
	setGuiZoomParameters (x, y, this);
	InvokeEvent (vtkCommand::EndInteractionEvent, &getGuiZoomParameters ( ));
}	// vtkCustomizableInteractorStyleTrackball::OnLeftButtonUp


void vtkCustomizableInteractorStyleTrackball::OnMouseMove ( )
{
	if (VTKIS_GUI_ZOOM != State)
	{
		vtkMgx3DInteractorStyle::OnMouseMove ( );
		return;
	}

	assert (0 != Interactor);
	int			x		= 0, y	= 0;
	Interactor->GetEventPosition (x, y);
	FindPokedRenderer (x, y);
	vtkCamera*	camera	= 0 == CurrentRenderer ? 0 : CurrentRenderer->GetActiveCamera ( );
	setGuiZoomParameters (x, y, this);
	InvokeEvent (vtkCommand::InteractionEvent, &getGuiZoomParameters ( ));
}	// vtkCustomizableInteractorStyleTrackball::OnMouseMove


void vtkCustomizableInteractorStyleTrackball::OnKeyRelease ( )
{
	if (0 == Interactor)
		return;

	// Flèches de déplacement, F* : KeyCode = 0 !
	if ((0 == Interactor->GetKeyCode ( )) && (0 != Interactor->GetKeySym ( )))
	{
		if (std::string ("Left") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1012);	// Qt Key_Left
		else if (std::string ("Up") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1013);	// Qt Key_Up
		else if (std::string ("Right") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1014);	// Qt Key_Right
		else if (std::string ("Down") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1015);	// Qt Key_Down
		else if (std::string ("F1") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1030);	// Qt F1
		else if (std::string ("F2") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1031);
		else if (std::string ("F3") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1032);
		else if (std::string ("F4") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1033);
		else if (std::string ("F5") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1034);
		else if (std::string ("F6") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1035);
		else if (std::string ("F7") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1036);
		else if (std::string ("F8") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1037);
		else if (std::string ("F9") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1038);
		else if (std::string ("F10") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x1039);
		else if (std::string ("F11") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x103a);
		else if (std::string ("F12") == Interactor->GetKeySym ( ))
			Interactor->SetKeyCode (0x103b);	// Qt F12
	}	// if ((0 == Interactor->GetKeyCode ( )) && ...
	if (VTKIS_GUI_ZOOM != State)
	{
		vtkMgx3DInteractorStyle::OnKeyRelease ( );
		return;
	}

	if ((false == isShiftKeyPressed ( )) || (false == isControlKeyPressed ( )))
	{
		int			x		= 0, y	= 0;
		Interactor->GetEventPosition (x, y);
		FindPokedRenderer (x, y);
		vtkCamera*	camera	= 0 == CurrentRenderer ?
							  0 : CurrentRenderer->GetActiveCamera ( );
		setGuiZoomParameters (x, y, this);
		State	= VTKIS_NONE;
		InvokeEvent (vtkCommand::EndInteractionEvent, &getGuiZoomParameters( ));
	}
}	// vtkCustomizableInteractorStyleTrackball::OnKeyRelease


void vtkCustomizableInteractorStyleTrackball::OnMouseWheelForward ( )
{
	if (1 > GetZoomFactor ( ))
	{
		cout << "Erreur dans vtkCustomizableInteractorStyleTrackball::OnMouseWheelForwward, facteur de zoom inférieur à 1 (" << GetZoomFactor ( ) << ")." << endl;
		return;
	}	// if (1 > GetZoomFactor ( ))

	// on inverse le fonctionnement si la touche CTRL est enfoncée
	bool inverse = false;
	if (0 != Interactor->GetControlKey())
	    inverse = true;

	if (inverse == isZoomOnWheelEventEnabled ( ))
		vtkMgx3DInteractorStyle::OnMouseWheelForward ( );
	else	// if (false == isZoomOnWheelEventEnabled ( ))
	{
		vtkCamera*	camera	= 0 == CurrentRenderer ? 0 : CurrentRenderer->GetActiveCamera ( );
		if (0 != camera)
		{
			const double	zoomFactor	= true == vtkUnifiedInteractorStyle::upZoom ? GetZoomFactor ( ) : 1. / GetZoomFactor ( );
			camera->Zoom (zoomFactor);
			if (0 != CurrentRenderer->GetRenderWindow ( ))
				CurrentRenderer->GetRenderWindow ( )->Render ( );
		}	// if (0 != camera)
	}	// else if (false == isZoomOnWheelEventEnabled ( ))
}	// vtkCustomizableInteractorStyleTrackball::OnMouseWheelForward


void vtkCustomizableInteractorStyleTrackball::OnMouseWheelBackward ( )
{
	if (1 > GetZoomFactor ( ))
	{
		cout << "Erreur dans vtkCustomizableInteractorStyleTrackball::OnMouseWheelBackward, facteur de zoom inférieur à 1 (" << GetZoomFactor ( ) << ")." << endl;
		return;
	}	// if (1 > GetZoomFactor ( ))

	if (false == isZoomOnWheelEventEnabled ( ))
		vtkMgx3DInteractorStyle::OnMouseWheelBackward ( );
	else	// if (false == isZoomOnWheelEventEnabled ( ))
	{
		vtkCamera*	camera	= 0 == CurrentRenderer ? 0 : CurrentRenderer->GetActiveCamera ( );
		if (0 != camera)
		{
			const double	factor	= true == vtkUnifiedInteractorStyle::upZoom ? 1. / GetZoomFactor ( ) : GetZoomFactor ( );
			camera->Zoom (factor);
			if (0 != CurrentRenderer->GetRenderWindow ( ))
				CurrentRenderer->GetRenderWindow ( )->Render ( );
		}	// if (0 != camera)
	}	// else if (false == isZoomOnWheelEventEnabled ( ))
}	// vtkCustomizableInteractorStyleTrackball::OnMouseWheelBackward


void vtkCustomizableInteractorStyleTrackball::PrintSelf (ostream& os, vtkIndent indent)
{
	vtkMgx3DInteractorStyle::PrintSelf (os, indent);
}	// vtkCustomizableInteractorStyleTrackball::PrintSelf


vtkCustomizableInteractorStyleTrackball* vtkCustomizableInteractorStyleTrackball::New ( )
{
	vtkObject*	object	=  vtkObjectFactory::CreateInstance ("vtkCustomizableInteractorStyleTrackball");
	if (0 != object)
		return (vtkCustomizableInteractorStyleTrackball*)object;

	return new vtkCustomizableInteractorStyleTrackball ( );
}	// vtkCustomizableInteractorStyleTrackball::New


void vtkCustomizableInteractorStyleTrackball::HighlightActor2D (vtkActor2D* actor)
{
	if (true == IsActorHighlightingEnabled ( ))
		vtkMgx3DInteractorStyle::HighlightActor2D (actor);
}	// vtkCustomizableInteractorStyleTrackball::HighlightActor2D


void vtkCustomizableInteractorStyleTrackball::HighlightProp3D (vtkProp3D* prop)
{
	if (true == IsActorHighlightingEnabled ( ))
		vtkMgx3DInteractorStyle::HighlightProp3D (prop);
}	// vtkCustomizableInteractorStyleTrackball::HighlightProp3D


void vtkCustomizableInteractorStyleTrackball::SetZoomFactor (double factor)
{
	if (1 > factor)
	{
		cout << "Erreur dans vtkCustomizableInteractorStyleTrackball::SetZoomFactor, facteur de zoom inférieur à 1 (" << factor << ")." << endl;
		return;
	}	// if (1 > factor)

	_zoomFactor	= factor;
}	// vtkCustomizableInteractorStyleTrackball::SetZoomFactor


void vtkCustomizableInteractorStyleTrackball::setGuiZoomParameters (
								int x, int y, vtkInteractorStyle* interactor)
{
	GuiZoomEvent&	parameters	= getGuiZoomParameters ( );
	parameters.setCoordinates (x, y);
	parameters.setInteractorStyle (interactor);
}	// vtkCustomizableInteractorStyleTrackball::setGuiZoomParameters

