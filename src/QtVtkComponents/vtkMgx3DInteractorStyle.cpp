#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "QtVtkComponents/vtkMgx3DInteractorStyle.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtVtkComponents/VTKMgx3DActor.h"
#include "VtkComponents/vtkECMAreaPicker.h"

#include <TkUtil/Exception.h>

#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkAreaPicker.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkAbstractPropPicker.h>
#include <vtkAssemblyPath.h>
#include <vtkProp3DCollection.h>

#include <assert.h>
#include <strings.h>

using namespace TkUtil;
using namespace std;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::QtComponents;
using namespace Mgx3D::QtVtkComponents;


const unsigned long		vtkMgx3DInteractorStyle::ViewRedefinedEvent	= 105000;


vtkMgx3DInteractorStyle::vtkMgx3DInteractorStyle ( )
	: vtkUnifiedInteractorStyle ( ), Mgx3DPicker (0), Mgx3DPickerCommand (0), SelectionManager (0), SeizureManager (0), 
	  InteractiveSelectionActivated (false), RubberButtonDown (false), RubberBand (false), CompletelyInsideSelection (false),
	  PixelArray (vtkSmartPointer<vtkUnsignedCharArray>::New ( )), TmpPixelArray (vtkSmartPointer<vtkUnsignedCharArray>::New ( ))
{
	ButtonPressPosition [0]	= ButtonPressPosition [1]	= 0;
	StartPosition [0]		= StartPosition [1]			= 0;
	EndPosition [0]			= EndPosition [1]			= 0;
	PixelArray->Initialize ( );
	TmpPixelArray->Initialize ( );
	PixelArray->SetNumberOfComponents (4);
	TmpPixelArray->SetNumberOfComponents (4);
}	// vtkMgx3DInteractorStyle::vtkMgx3DInteractorStyle


vtkMgx3DInteractorStyle::vtkMgx3DInteractorStyle (const vtkMgx3DInteractorStyle&)
	: vtkUnifiedInteractorStyle ( ), Mgx3DPicker (0), Mgx3DPickerCommand (0), SelectionManager (0), SeizureManager (0),
	  InteractiveSelectionActivated (false), RubberButtonDown (false), RubberBand (false), CompletelyInsideSelection (false),
	  PixelArray ( ), TmpPixelArray ( )
{
	assert (0 && "vtkMgx3DInteractorStyle copy constructor is not allowed.");
}	// vtkMgx3DInteractorStyle copy constructor


vtkMgx3DInteractorStyle& vtkMgx3DInteractorStyle::operator = (const vtkMgx3DInteractorStyle&)
{
	assert (0 && "vtkMgx3DInteractorStyle::operator = is not allowed.");
	return *this;
}	// vtkMgx3DInteractorStyle::operator =


vtkMgx3DInteractorStyle::~vtkMgx3DInteractorStyle ( )
{
	SelectionManager	= 0;
	if (0 != Mgx3DPickerCommand)
		Mgx3DPickerCommand->UnRegister (this);
	Mgx3DPickerCommand	= 0;
	if (0 != Mgx3DPicker)
		Mgx3DPicker->UnRegister (this);
	Mgx3DPicker			= 0;
}	// vtkMgx3DInteractorStyle::~vtkMgx3DInteractorStyle


void vtkMgx3DInteractorStyle::PrintSelf (ostream& os, vtkIndent indent)
{
	vtkUnifiedInteractorStyle::PrintSelf (os, indent);
	os << "xyzCancelRoll : " << (true == Resources::instance ( )._xyzCancelRoll.getValue ( ) ? "True" : "False") << endl
	   << "InteractiveSelectionActivated : " << (true == InteractiveSelectionActivated ? "True" : "False") << endl
	   << "RubberBand : " << (true == RubberBand ? "True" : "False") << endl
	   << "CompletelyInsideSelection : " << (true == CompletelyInsideSelection ? "True" : "False") << endl
	   << "PickOnLeftButtonDown : " << (true == Resources::instance ( )._pickOnLeftButtonDown.getValue( ) ? "True" : "False") << endl
	   << "PickOnRightButtonDown : " << (true == Resources::instance ( )._pickOnRightButtonDown.getValue( ) ? "True" : "False") << endl;
}	// vtkMgx3DInteractorStyle::PrintSelf


void vtkMgx3DInteractorStyle::OnChar ( )
{
	vtkRenderWindowInteractor*	interactor	= this->Interactor;
	assert ((0 != interactor) && "vtkMgx3DInteractorStyle::OnChar : null interactor.");

	switch (interactor->GetKeyCode ( ))
	{
		case 'f'	:
		case 'F'	:
		{
			// Pb : Fly to repose sur le caractère pickable des acteurs, caractère sur lequel on joue dans Magix pour filtrer selon la
			// dimension ce qui est sélectionnable ...
			// On mémorise l'état pickable des acteurs, on les rends tous pickables, et on restaure l'état initial à la fin.
			if (0 == CurrentRenderer)
				return;
			vtkPropCollection*	props	= CurrentRenderer->GetViewProps ( );
			vtkCollectionSimpleIterator	pit;
			vtkProp				*prop	= 0, *propCandidate	= 0;
			vtkAssemblyPath*	path	= 0;
			vector<vtkProp*>	unpickables;
			for ( props->InitTraversal(pit); (prop=props->GetNextProp(pit)); )
			{
				for ( prop->InitPathTraversal(); (path=prop->GetNextPath()); )
				{
					propCandidate	= path->GetLastNode()->GetViewProp();
					if (false == propCandidate->GetPickable ( ))
					{
						unpickables.push_back (propCandidate);
						propCandidate->SetPickable (true);
					}	// if (false == propCandidate->GetPickable ( ))
				}	// for ( prop->InitPathTraversal(); ...
			}	// for ( props->InitTraversal(pit); ...
			vtkUnifiedInteractorStyle::OnChar ( );
			for (vector<vtkProp*>::iterator it = unpickables.begin( ); unpickables.end ( ) != it; it++)
				(*it)->SetPickable (false);
			break;
		}
		case 'h'    :   // pour masquer l'entité sélectionnée
			HideSelection ( );
			break;
		case 'r'	:
			vtkUnifiedInteractorStyle::OnChar ( );
			break;
		case 'R'	:
			ResetViewOnSelection ( );
			break;
		default     :
			vtkUnifiedInteractorStyle::OnChar ( );
	}
} // vtkMgx3DInteractorStyle::OnChar


void vtkMgx3DInteractorStyle::OnKeyRelease ( )
{
	// OnChar n'est pas appelé pour SHIFT + TAB => OnKeyRelease
	if (0 != Resources::instance ( )._nextSelectionKey.getValue ( ).length ( ))
	{
		vtkRenderWindowInteractor*	interactor	= this->Interactor;
		assert ((0 != interactor) && "vtkMgx3DInteractorStyle::OnKeyRelease : null interactor.");
		const string	key	= Resources::instance ( )._nextSelectionKey.getValue( );
		if (0 == strcasecmp (interactor->GetKeySym ( ), (char*)key.c_str ( )))
			NextSelection ( );
		else
			vtkUnifiedInteractorStyle::OnKeyRelease ( );
	}	// if (0 != Resources::instance ( )._nextSelectionKey.getValue ( )...
	else
		vtkUnifiedInteractorStyle::OnKeyRelease ( );
}	// vtkMgx3DInteractorStyle::OnChar


vtkMgx3DInteractorStyle* vtkMgx3DInteractorStyle::New ( )
{
	vtkObject*	object	= vtkObjectFactory::CreateInstance ("vtkMgx3DInteractorStyle");
	if (0 != object)
		return (vtkMgx3DInteractorStyle*)object;

	return new vtkMgx3DInteractorStyle ( );
}	// vtkMgx3DInteractorStyle::New


void vtkMgx3DInteractorStyle::DisplayxOyViewPlane ( )
{
	vtkCamera*	camera	= 0 == CurrentRenderer ? 0 : CurrentRenderer->GetActiveCamera ( );
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
    if (true == Resources::instance ( )._xyzCancelRoll.getValue ( ))
        camera->SetRoll (0.);
	if (0 != CurrentRenderer->GetRenderWindow ( ))
		CurrentRenderer->GetRenderWindow ( )->Render ( );
    InvokeEvent (vtkMgx3DInteractorStyle::ViewRedefinedEvent, NULL);
}	// vtkMgx3DInteractorStyle::DisplayxOyViewPlane


void vtkMgx3DInteractorStyle::DisplayxOzViewPlane ( )
{
	vtkCamera*  camera  = 0 == CurrentRenderer ? 0 : CurrentRenderer->GetActiveCamera ( );
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
    if (true == Resources::instance ( )._xyzCancelRoll.getValue ( ))
        camera->SetRoll (0.);
	if (0 != CurrentRenderer->GetRenderWindow ( ))
		CurrentRenderer->GetRenderWindow ( )->Render ( );
    InvokeEvent (vtkMgx3DInteractorStyle::ViewRedefinedEvent, NULL);
}	// vtkMgx3DInteractorStyle::DisplayxOzViewPlane


void vtkMgx3DInteractorStyle::DisplayyOzViewPlane ( )
{
	vtkCamera*	camera	= 0 == CurrentRenderer ? 0 : CurrentRenderer->GetActiveCamera ( );
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
    if (true == Resources::instance ( )._xyzCancelRoll.getValue ( ))
        camera->SetRoll (0.);
	if (0 != CurrentRenderer->GetRenderWindow ( ))
		CurrentRenderer->GetRenderWindow ( )->Render ( );
    InvokeEvent (vtkMgx3DInteractorStyle::ViewRedefinedEvent, NULL);
}	// vtkMgx3DInteractorStyle::DisplayyOzViewPlane


void vtkMgx3DInteractorStyle::OnLeftArrow ( )
{
    vtkUnifiedInteractorStyle::OnLeftArrow ( );
    InvokeEvent (ViewRedefinedEvent, NULL);
}   // vtkMgx3DInteractorStyle::OnLeftArrow


void vtkMgx3DInteractorStyle::OnUpArrow ( )
{
    vtkUnifiedInteractorStyle::OnUpArrow ( );
    InvokeEvent (ViewRedefinedEvent, NULL);
}   // vtkMgx3DInteractorStyle::OnUpArrow


void vtkMgx3DInteractorStyle::OnRightArrow ( )
{
    vtkUnifiedInteractorStyle::OnRightArrow ( );
    InvokeEvent (ViewRedefinedEvent, NULL);
}   // vtkMgx3DInteractorStyle::OnRightArrow


void vtkMgx3DInteractorStyle::OnDownArrow ( )
{
    vtkUnifiedInteractorStyle::OnDownArrow ( );
    InvokeEvent (ViewRedefinedEvent, NULL);
}   // vtkMgx3DInteractorStyle::OnDownArrow


void vtkMgx3DInteractorStyle::FlyTo (bool centerOnActor)
{
    vtkUnifiedInteractorStyle::FlyTo (centerOnActor);
    InvokeEvent (ViewRedefinedEvent, NULL);
}   // vtkMgx3DInteractorStyle::FlyTo


void vtkMgx3DInteractorStyle::ResetRoll ( )
{
    vtkUnifiedInteractorStyle::ResetRoll ( );
    InvokeEvent (ViewRedefinedEvent, NULL);
}   // vtkMgx3DInteractorStyle::ResetRoll


void vtkMgx3DInteractorStyle::ResetView ( )
{
    vtkUnifiedInteractorStyle::ResetView ( );
    InvokeEvent (ViewRedefinedEvent, NULL);
}   // vtkMgx3DInteractorStyle::ResetView


void vtkMgx3DInteractorStyle::ResetViewOnSelection ( )
{
	vtkCamera*	camera	= 0 == CurrentRenderer ? 0 : CurrentRenderer->GetActiveCamera ( );
	if ((0 != SelectionManager) && (0 != camera))
	{
		double	bounds [6]	= { 0., 0., 0., 0., 0., 0. };
		SelectionManager->getBounds (bounds);
		const double	fx	= (bounds [0] + bounds [1]) / 2.;
		const double	fy	= (bounds [2] + bounds [3]) / 2.;
		const double	fz	= (bounds [4] + bounds [5]) / 2.;
		camera->SetFocalPoint (fx, fy, fz);
		camera->ComputeViewPlaneNormal ( );
		if (0 != CurrentRenderer->GetRenderWindow ( ))
			CurrentRenderer->GetRenderWindow ( )->Render ( );
    	InvokeEvent (ViewRedefinedEvent, NULL);
	}	// if ((0 != SelectionManager) && (0 != camera))
}	// vtkMgx3DInteractorStyle::ResetViewOnSelection


void vtkMgx3DInteractorStyle::SetPickingTools (VTKMgx3DPicker* picker, VTKMgx3DPickerCommand* pickerCommand)
{
	if ((0 != Mgx3DPickerCommand) && (Mgx3DPickerCommand != pickerCommand))
		Mgx3DPickerCommand->UnRegister (this);
	Mgx3DPickerCommand	= pickerCommand;
	if (0 != Mgx3DPickerCommand)
		Mgx3DPickerCommand->Register (this);
	if ((0 != Mgx3DPicker) && (Mgx3DPicker != picker))
		Mgx3DPicker->UnRegister (this);
	Mgx3DPicker	= picker;
	if (0 != Mgx3DPicker)
		Mgx3DPicker->Register (this);
}	// vtkMgx3DInteractorStyle::SetPickingTools


EntitySeizureManager* vtkMgx3DInteractorStyle::GetEntitySeizureManager ( )
{
	return SeizureManager;
}	// vtkMgx3DInteractorStyle::GetEntitySeizureManager


void vtkMgx3DInteractorStyle::SetEntitySeizureManager (EntitySeizureManager* sm)
{
	SeizureManager	= sm;
}	// vtkMgx3DInteractorStyle::SetEntitySeizureManager


VTKMgx3DPicker* vtkMgx3DInteractorStyle::GetMgx3DPicker ( )
{
	return Mgx3DPicker;
}	// vtkMgx3DInteractorStyle::GetMgx3DPicker


void vtkMgx3DInteractorStyle::OnMouseMove ( )
{
	if (false == RubberBand)
		vtkUnifiedInteractorStyle::OnMouseMove ( );
	else
	{
		if ((GetInteractiveSelectionActivated ( ) == true)  && (true == RubberButtonDown))
		{
			vtkRenderWindowInteractor*	rwi	= this->Interactor;
			if ((0 != rwi) && (0 != rwi->GetRenderWindow ( )))
			{
				EndPosition [0]	= rwi->GetEventPosition ( )[0];
				EndPosition [1]	= rwi->GetEventPosition ( )[1];
				int*	size	= rwi->GetRenderWindow ( )->GetSize ( );
				if (EndPosition [0] > (size [0] - 1))
					EndPosition [0]	= size [0] - 1;
				if (EndPosition [0] < 0)
					EndPosition [0] = 0;
				if (EndPosition [1] > (size [1] - 1))
					EndPosition [1] = size [1] - 1;
				if (EndPosition [1] < 0)
					EndPosition [1] = 0;
				RedrawRubberBand ( );
			}	// if ((0 != rwi) && (0 != rwi->GetRenderWindow ( )))
		}	// if ((GetInteractiveSelectionActivated ( ) == true)  && (true == RubberButtonDown))
	}	// if (false == RubberBand)
}	// vtkMgx3DInteractorStyle::OnMouseMove


void vtkMgx3DInteractorStyle::OnLeftButtonDown ( )
{
	vtkRenderWindowInteractor*	rwi	= this->Interactor;
	if (0 != rwi)
		rwi->GetEventPosition (ButtonPressPosition);

	if (true == GetInteractiveSelectionActivated ( ))
	{
		if (false == RubberBand)
		{
			if (true == Resources::instance ( )._pickOnLeftButtonDown.getValue ( ))
				Pick ( );
			else
				vtkUnifiedInteractorStyle::OnLeftButtonDown ( );
		}	// if (false == RubberBand)
		else
		{
			if ((0 == rwi) || (0 == rwi->GetRenderWindow ( )))
				return;

			RubberButtonDown	= true;
			StartPosition [0]	= EndPosition [0]	= rwi->GetEventPosition ( )[0];
			StartPosition [1]	= EndPosition [1]	= rwi->GetEventPosition ( )[1];
			int*	size		= rwi->GetRenderWindow ( )->GetSize ( );
			PixelArray->SetNumberOfTuples (size [0] * size [1]);
			TmpPixelArray->SetNumberOfTuples (size [0] * size [1]);
//			rwi->GetRenderWindow ( )->GetRGBACharPixelData (0, 0, size [0] - 1, size [1] - 1, 1, PixelArray);
			// Si on est en double buffering on prend l'image dans le back buffer, sinon dans le front buffer.
			rwi->GetRenderWindow ( )->GetRGBACharPixelData (0, 0, size [0] - 1, size [1] - 1, !rwi->GetRenderWindow ( )->GetDoubleBuffer ( ), PixelArray);
			FindPokedRenderer (StartPosition [0], StartPosition [1]);
		}	// else if (false == RubberBand)
	}
	else
		vtkUnifiedInteractorStyle::OnLeftButtonDown ( );
}	// vtkMgx3DInteractorStyle::OnLeftButtonDown


void vtkMgx3DInteractorStyle::OnMiddleButtonDown ( )
{
	if ((false == GetInteractiveSelectionActivated ( )) || (false == isControlKeyPressed ( )))
	{
		vtkUnifiedInteractorStyle::OnMiddleButtonDown ( );
		return;
	}	// if (false == GetInteractiveSelectionActivated ( ))

	if (0 != GetEntitySeizureManager ( ))
	{
		EntitySeizureManager*	next	= GetEntitySeizureManager ( )->getNextSeizureManager ( );
		if (0 != next)
			next->setInteractiveMode (true);
		else
		{	// C'est le dernier champ de saisie => validation de l'opération :
			try
			{
				QtMgx3DMainWindow&	mainWindow	= GetEntitySeizureManager ( )->getMainWindow ( );
				QtMgx3DOperationPanel*	panel	= mainWindow.getOperationsPanel ( ).getOperationsPanel ( );
				QtMgx3DOperationAction*	action	= 0 == panel ? 0 : panel->getMgx3DOperationAction ( );
				if (0 == action)
					throw Exception (UTF8String ("Absence d'action associée.", Charset::UTF_8));
				action->executeOperation ( );
				panel->operationCompleted ( );

				// Réinitialisation au premier champ :
				EntitySeizureManager*	sm	= GetEntitySeizureManager ( );
				while ((0 != sm) && (0 != sm->getPreviousSeizureManager ( )))
					sm	= sm->getPreviousSeizureManager ( );
				if (0 != sm)
					sm->setInteractiveMode (true);
			}
			catch (const Exception& exc)
			{
				cout << "Erreur dans vtkMgx3DInteractorStyle::OnMiddleButtonDown : " << exc.getFullMessage ( ) << endl;
			}
			catch (...)
			{
				cout << "vtkMgx3DInteractorStyle::OnMiddleButtonDown : erreur non documentée." << endl;
			}
		}	// else if (0 != next)
	}	// if (0 != GetEntitySeizureManager ( ))
}	// vtkMgx3DInteractorStyle::OnMiddleButtonDown


void vtkMgx3DInteractorStyle::OnRightButtonDown ( )
{
	vtkRenderWindowInteractor*	rwi	= this->Interactor;
	if (0 != rwi)
		rwi->GetEventPosition (ButtonPressPosition);

	if ((true == GetInteractiveSelectionActivated ( )) && (true == Resources::instance ( )._pickOnRightButtonDown.getValue ( )))
		Pick ( );
	else
		vtkUnifiedInteractorStyle::OnRightButtonDown ( );
}	// vtkMgx3DInteractorStyle::OnRightButtonDown


void vtkMgx3DInteractorStyle::OnLeftButtonUp ( )
{
	// Arrêter un éventuel déclenchement d'interaction :
	vtkUnifiedInteractorStyle::OnLeftButtonUp ( );

	if (true == GetInteractiveSelectionActivated ( ))
	{
		vtkRenderWindowInteractor*	rwi	= this->Interactor;
		
		if (false == RubberBand)
		{
			// Si le curseur n'a pas bougé depuis la pression sur le bouton, et que c'est paramétré tel que, on fait un picking :
			if ((0 == rwi) || (true == Resources::instance ( )._pickOnLeftButtonUp.getValue ( )))
			{
				int		EventPosition [2]	= {0, 0};
				if (0 != rwi)
				{
					rwi->GetEventPosition (EventPosition);

					if ((EventPosition [0] == ButtonPressPosition [0]) && (EventPosition [1] == ButtonPressPosition [1]))
						Pick ( );
				}	// if (0 != rwi)
			}	// if ((0 == rwi) || (true == Resources::instance ( )._pickOnLeftButtonUp.getValue ( )))
		}	// if (false == RubberBand)
		else
		{
			const int* const	size	= rwi->GetRenderWindow ( )->GetSize ( );
			// Si on est en double buffering on met l'image dans le back buffer, Frame ( )la rebasculera dans le front buffer, sinon on la met dans le front buffer.
			rwi->GetRenderWindow ( )->SetRGBACharPixelData (0, 0, size [0] - 1, size [1] - 1, PixelArray->GetPointer (0), !rwi->GetRenderWindow ( )->GetDoubleBuffer ( ));
			rwi->GetRenderWindow ( )->Frame ( );
			RubberButtonDown		= false;

			vtkSmartPointer<vtkECMAreaPicker>		areaPicker		= vtkSmartPointer<vtkECMAreaPicker>::New ( );
//			areaPicker->SelectCompletelyInside (CompletelyInsideSelection);
// Rem : CTRL + i mappé en Tab !!! d'où le choix de 'd' (dedans) plutôt que 'i' (inside, intérieur) (:
			if ((true == CompletelyInsideSelection) || ((0 != rwi) && (0 != rwi->GetKeySym ( )) && ('d' == *rwi->GetKeySym ( ))))
				areaPicker->SelectCompletelyInside (true);
			areaPicker->SetRenderer (CurrentRenderer);
			int	result	= areaPicker->AreaPick (StartPosition [0], StartPosition [1], EndPosition [0], EndPosition [1], CurrentRenderer);
			vtkProp3DCollection*		pickedProps	= areaPicker->GetProp3Ds ( );
			vtkProp3D*					p			= 0;
			vtkCollectionSimpleIterator	csi;
			pickedProps->InitTraversal (csi);
			vector<Entity*>	entities;
			while (0 != (p = pickedProps->GetNextProp3D (csi)))
			{
				bool			add	= true;
				VTKMgx3DActor*	a	= dynamic_cast<VTKMgx3DActor*>(p);
				if (0 != a)
					entities.push_back (a->GetEntity ( ));
			}	// while (0 != (p = pickedProps->GetNextProp3D (csi)))
			if (0 != SelectionManager)
			{
				if (false == isControlKeyPressed ( ))
					SelectionManager->clearSelection ( );
				SelectionManager->addToSelection (entities);
			}	// if (0 != SelectionManager)
		}	// else if (false == RubberBand)
	}	// if (true == GetInteractiveSelectionActivated ( ))
}	// vtkMgx3DInteractorStyle::OnLeftButtonUp


void vtkMgx3DInteractorStyle::OnRightButtonUp ( )
{
	// Arrêter un éventuel déclenchement d'interaction :
	vtkUnifiedInteractorStyle::OnRightButtonUp ( );

	// Si le curseur n'a pas bougé depuis la pression sur le bouton, et que
	// c'est paramétré tel que, on fait un picking :
	if ((true == GetInteractiveSelectionActivated ( )) && (true == Resources::instance ( )._pickOnRightButtonUp.getValue ( )))
	{
		int							EventPosition [2]	= {0, 0};
		vtkRenderWindowInteractor*	rwi	= this->Interactor;
		if (0 != rwi)
		{
			rwi->GetEventPosition (EventPosition);

			if ((EventPosition [0] == ButtonPressPosition [0]) && (EventPosition [1] == ButtonPressPosition [1]))
				Pick ( );
		}
	}	// if ((true == GetInteractiveSelectionActivated ( )) && ...
}	// vtkMgx3DInteractorStyle::OnRightButtonUp


void vtkMgx3DInteractorStyle::SetSelectionManager (SelectionManagerIfc* mgr)
{
	SelectionManager	= mgr;
}	// vtkMgx3DInteractorStyle::SetSelectionManager


void vtkMgx3DInteractorStyle::SetInteractiveSelectionActivated (bool activate)
{
	InteractiveSelectionActivated	= activate;
}	// vtkMgx3DInteractorStyle::SetInteractiveSelectionActivated


bool vtkMgx3DInteractorStyle::GetInteractiveSelectionActivated ( ) const
{
	return InteractiveSelectionActivated;
}	// vtkMgx3DInteractorStyle::GetInteractiveSelectionActivated


void vtkMgx3DInteractorStyle::SetRubberBand (bool on)
{
	RubberBand	= on;
}	// vtkMgx3DInteractorStyle::SetRubberBand


bool vtkMgx3DInteractorStyle::GetRubberBand ( ) const
{
	return RubberBand;
}	// vtkMgx3DInteractorStyle::GetRubberBand


void vtkMgx3DInteractorStyle::SetCompletelyInsideSelection (bool on)
{
	CompletelyInsideSelection	= on;
}	// vtkMgx3DInteractorStyle::SetCompletelyInsideSelection


bool vtkMgx3DInteractorStyle::GetCompletelyInsideSelection ( ) const
{
	return CompletelyInsideSelection;
}	// vtkMgx3DInteractorStyle::GetCompletelyInsideSelection


void vtkMgx3DInteractorStyle::Pick ( )
{
	// Code issu de vtkInteractorStyle::OnChar de VTK 5.10.0, char == P
	vtkRenderWindowInteractor *rwi = this->Interactor;
	if (this->State == VTKIS_NONE)
	{
		vtkAssemblyPath *path = NULL;
		int *eventPos = rwi->GetEventPosition();
		this->FindPokedRenderer(eventPos[0], eventPos[1]);
		rwi->StartPickCallback();
		vtkAbstractPropPicker *picker = vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker());
		if ( picker != NULL )
		{
			picker->Pick(eventPos[0], eventPos[1], 0.0, this->CurrentRenderer);
			path = picker->GetPath();
		}
		if ( path == NULL )
		{
			this->HighlightProp(NULL);
			this->PropPicked = 0;
		}
		else
		{
			this->HighlightProp(path->GetFirstNode()->GetViewProp());
			this->PropPicked = 1;
		}
		rwi->EndPickCallback();
	}
}	// vtkMgx3DInteractorStyle::Pick


void vtkMgx3DInteractorStyle::NextSelection ( )
{
	if (0 != Mgx3DPickerCommand)
		Mgx3DPickerCommand->NextSelection (!isShiftKeyPressed ( ));
}	// vtkMgx3DInteractorStyle::NextSelection


void vtkMgx3DInteractorStyle::HideSelection()
{
//    if (0 != Mgx3DPickerCommand)
//        Mgx3DPickerCommand->HideSelection ( );
} // vtkMgx3DInteractorStyle::HideSelection


void vtkMgx3DInteractorStyle::RedrawRubberBand ( )
{
	vtkRenderWindowInteractor*	rwi	= this->Interactor;
	if ((0 == rwi) || (0 == rwi->GetRenderWindow ( )))
		return;

	TmpPixelArray->DeepCopy (PixelArray);
	unsigned char*		pixels	= TmpPixelArray->GetPointer (0);
	const int* const	size	= rwi->GetRenderWindow ( )->GetSize ( );

	int min [2], max [2];

	min [0] = StartPosition [0] <= EndPosition [0] ? StartPosition [0] : EndPosition [0];
	if (min [0] < 0)
		min [0] = 0;
	if (min [0] >= size [0])
		min [0] = size [0] - 1;

	min [1] = StartPosition [1] <= EndPosition [1] ? StartPosition [1] : EndPosition [1];
	if (min [1] < 0)
		min [1] = 0;
	if (min [1] >= size [1])
		min [1] = size [1] - 1;

	max [0] = EndPosition [0] > StartPosition [0] ?	EndPosition [0] : StartPosition [0];
	if (max [0] < 0)
		max [0] = 0;
	if (max [0] >= size [0])
		max [0] = size [0] - 1;

	max [1] = EndPosition [1] > StartPosition [1] ?	EndPosition [1] : StartPosition [1];
	if (max [1] < 0)
		max [1] = 0;
	if (max [1] >= size [1])
		max [1] = size [1] - 1;

	for (int i = min [0]; i <= max [0]; i++)
	{
		pixels [4 * (min [1]*size [0] + i)] = 255 ^ pixels [4 * (min [1]*size [0] + i)];
		pixels [4 * (min [1]*size [0] + i) + 1] = 255 ^ pixels [4 * (min [1]*size [0] + i) + 1];
		pixels [4 * (min [1]*size [0] + i) + 2] = 255 ^ pixels [4 * (min [1]*size [0] + i) + 2];
		pixels [4 * (max [1]*size [0] + i)] = 255 ^ pixels [4 * (max [1]*size [0] + i)];
		pixels [4 * (max [1]*size [0] + i) + 1] = 255 ^ pixels [4 * (max [1]*size [0] + i) + 1];
		pixels [4 * (max [1]*size [0] + i) + 2] = 255 ^ pixels [4 * (max [1]*size [0] + i) + 2];
	}	// for (i = min [0]; i <= max [0]; i++)
	for (int i = min [1] + 1; i < max [1]; i++)
	{
		pixels [4 * (i * size [0] + min [0])] = 255 ^ pixels [4 * (i * size [0] + min [0])];
		pixels [4 * (i * size [0] + min [0]) + 1] = 255 ^ pixels [4 * (i * size [0] + min [0]) + 1];
		pixels [4 * (i * size [0] + min [0]) + 2] = 255 ^ pixels [4 * (i * size [0] + min [0]) + 2];
		pixels [4 * (i * size [0] + max [0])] = 255 ^ pixels [4 * (i * size [0] + max [0])];
		pixels [4 * (i * size [0] + max [0]) + 1] = 255 ^ pixels [4 * (i * size [0] + max [0]) + 1];
		pixels [4 * (i * size [0] + max [0]) + 2] = 255 ^ pixels [4 * (i * size [0] + max [0]) + 2];
	}	// for (i = min [1] + 1; i < max [1]; i++)

	// Si on est en double buffering on met l'image dans le back buffer, Frame ( )la rebasculera dans le front buffer, sinon on la met dans le front buffer.
	rwi->GetRenderWindow ( )->SetRGBACharPixelData (0, 0, size [0] - 1, size [1] - 1, pixels, !rwi->GetRenderWindow ( )->GetDoubleBuffer ( ));
	rwi->GetRenderWindow ( )->Frame ( );
}	// vtkMgx3DInteractorStyle::RedrawRubberBand
