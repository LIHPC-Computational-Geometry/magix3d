#include "Internal/ContextIfc.h"

#include "QtVtkComponents/VTKECMPickerCommand.h"
#include "QtVtkComponents/vtkCustomizableInteractorStyleTrackball.h"

#include <Utils/Common.h>

#include <TkUtil/Exception.h>
#include <TkUtil/ErrorLog.h>
#include <TkUtil/InformationLog.h>
#include <TkUtil/MemoryError.h>

#include <assert.h>
#include <map>
#include <stdexcept>

using namespace std;
using namespace TkUtil;


VTKECMPickerCommand::VTKECMPickerCommand (vtkRenderer* renderer, vtkRenderWindowInteractor* interactor)
	: vtkCommand ( ), _interactor (interactor), _renderer (renderer),
	  _selectionManager (0), _logDispatcher (0), _mutex ( ), _pointedObjects ( ), _pointedObject ((size_t)-1)
{
	if (0 != _interactor)
		_interactor->Register (0);
}	// VTKECMPickerCommand::VTKECMPickerCommand


VTKECMPickerCommand::VTKECMPickerCommand (const VTKECMPickerCommand&)
	: vtkCommand ( ), _interactor (0), _renderer (0),
	  _selectionManager (0), _logDispatcher (0), _mutex ( ), _pointedObjects ( ), _pointedObject ((size_t)-1)
{
	assert (0 && "VTKECMPickerCommand copy constructor is not allowed.");
}	// VTKECMPickerCommand copy constructor


VTKECMPickerCommand& VTKECMPickerCommand::operator = (const VTKECMPickerCommand&)
{
	assert (0 && "VTKECMPickerCommand::operator = is not allowed.");
	return *this;
}	// VTKECMPickerCommand::operator =


VTKECMPickerCommand::~VTKECMPickerCommand ( )
{
	AutoMutex	autoMutex (&_mutex);

	if (0 != _interactor)
		_interactor->UnRegister (0);
}	// VTKECMPickerCommand::~VTKECMPickerCommand


void VTKECMPickerCommand::Execute (vtkObject* caller, unsigned long eventId, void* callData)
{
	AutoMutex	autoMutex (&_mutex);

	assert ((0 != caller) && "VTKECMPickerCommand::Execute : null caller.");
	VTKECMPicker*	picker	= dynamic_cast<VTKECMPicker*>(caller);
	assert ((0 != picker) && "VTKECMPickerCommand::Execute : caller is not a magix 3d picker.");

	try
	{
		if (0 == GetSelectionManager ( ))
			return;

		// Extension de la selection ou non ? Par defaut, oui.
		bool	extend	= true;
		if (0 != _interactor)
		{
			vtkInteractorObserver*						style	= _interactor->GetInteractorStyle ( );
			vtkCustomizableInteractorStyleTrackball*	tStyle	= dynamic_cast<vtkCustomizableInteractorStyleTrackball*>(style);
			// Si c'est un vtkCustomizableInteractorStyleTrackball et que control n'est pas pressé, on annule la selection en cours.
			if ((0 != tStyle) && (false == tStyle->isControlKeyPressed ( )))
				extend	= false;
		}	// if (0 != _interactor)

		vtkProp*		prop	= picker->GetViewProp ( );
		vtkActor*		actor	= dynamic_cast<vtkActor*>(prop);
		UTF8String		message (Charset::UTF_8);
		if (0 != actor)
		{
			bool	selected	= GetSelectionManager ( )->IsSelected (*actor);
			if (false == selected)
			{
				if (false == extend)
				{
					message << "Suppression de la sélection.\n";
					GetSelectionManager ( )->ClearSelection ( );
				}	// if (false == extend)
				message << "Sélection de l'objet " << GetName (*actor) << ".";
				InformationLog	ilog (message);
				Log (ilog);

				_pointedObjects	= picker->GetPickedActors ( );
				_pointedObject	= 0;
				GetSelectionManager ( )->AddToSelection (*(_pointedObjects[0]));
			}	// if (false == selected)
			else
			{
				_pointedObjects.clear ( );
				_pointedObject	= (size_t)-1;

				if (false == extend)
				{
					message << "Suppression de la sélection.";
					GetSelectionManager ( )->ClearSelection ( );
				}
				else
				{
					// Pour message : attention, la vue peut être détruite après appel de RemoveFromSelection.
					message << "Retrait de la sélection de l'objet "
					        << GetName (*actor) << ".";
					GetSelectionManager ( )->RemoveFromSelection (*actor);
				}	// else if (false == extend)
			}	// else if (false == selected)
		}	// if (0 != actor)
		else if (false == extend)
		{
			_pointedObjects.clear ( );
			_pointedObject	= (size_t)-1;
			message << "Suppression de la sélection.";
			GetSelectionManager ( )->ClearSelection ( );
		}	// else if (false == extend)

		InformationLog	ilog (message);
		Log (ilog);
	}
	catch (const Exception& exc)
	{
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg <<"Une erreur est survenue lors d'une opération de pointage :\n";
		errorMsg << exc.getMessage ( );
		ErrorLog	error (errorMsg);
		Log (error);
	}
	catch (...)
	{
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg << "Erreur non renseignée lors d'une opération de pointage.";
		ErrorLog	error (errorMsg);
		Log (error);
	}
}	// VTKECMPickerCommand::Execute


void VTKECMPickerCommand::NextSelection (bool next)
{
	AutoMutex	autoMutex (&_mutex);

	if ((1 >= _pointedObjects.size ( )) || (0 == GetSelectionManager ( )))
	{
		return;
	}	// if ((1 >= _pointedObjects.size ( )) || ...

	// On enlève la sélection en cours :
	try
	{
		vtkActor*	actor	= _pointedObjects [_pointedObject];
		CHECK_NULL_PTR_ERROR (actor)
		UTF8String	message (Charset::UTF_8);
		message << "Modification de la sélection. Retrait de l'objet " << GetName (*actor) << ", ajout de l'objet ";
		GetSelectionManager ( )->RemoveFromSelection (*actor);

		// On passe à l'objet suivant :
		if (true == next)
			_pointedObject	= _pointedObject == _pointedObjects.size ( ) - 1 ? 0 : _pointedObject + 1;
		else
			_pointedObject	= 0 == _pointedObject ? _pointedObjects.size ( ) - 1 : _pointedObject - 1;
		actor	= _pointedObjects [_pointedObject];
		CHECK_NULL_PTR_ERROR (actor)
		message << GetName (*actor) << ".";
		GetSelectionManager ( )->AddToSelection (*actor);

		InformationLog	ilog (message);
		Log (ilog);
	}
	catch (const Exception& exc)
	{
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg <<"Une erreur est survenue lors d'une opération de modification de la sélection :\n";
		errorMsg << exc.getMessage ( );
		ErrorLog	error (errorMsg);
		Log (error);
	}
	catch (const exception& e)
	{
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg <<"Une erreur C++ standard est survenue lors d'une opération de modification de la sélection :\n";
		errorMsg << e.what ( );
		ErrorLog	error (errorMsg);
		Log (error);
	}
	catch (...)
	{
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg << "Erreur non renseignée lors d'une opération de modification de la sélection.";
		ErrorLog	error (errorMsg);
		Log (error);
	}
}	// VTKECMPickerCommand::NextSelection


void VTKECMPickerCommand::SetLogDispatcher (LogDispatcher* dispatcher)
{
	AutoMutex	autoMutex (&_mutex);

	_logDispatcher	= dispatcher;
}	// VTKECMPickerCommand::SetLogDispatcher


string VTKECMPickerCommand::GetName (const vtkActor& actor) const
{
	return "";
}	// VTKECMPickerCommand::GetName


void VTKECMPickerCommand::SetSelectionManager (VTKECMSelectionManager* manager)
{
	AutoMutex	autoMutex (&_mutex);

	_selectionManager	= manager;
}	// VTKECMPickerCommand::SetSelectionManager


VTKECMSelectionManager* VTKECMPickerCommand::GetSelectionManager ( )
{
	return _selectionManager;
}	// VTKECMPickerCommand::GetSelectionManager


const VTKECMSelectionManager* VTKECMPickerCommand::GetSelectionManager ( ) const
{
	return _selectionManager;
}	// VTKECMPickerCommand::GetSelectionManager


vtkRenderWindowInteractor& VTKECMPickerCommand::getInteractor ( )
{
	CHECK_NULL_PTR_ERROR (_interactor)
	return *_interactor;
}	// VTKECMPickerCommand::getInteractor

	 
vtkRenderer& VTKECMPickerCommand::getRenderer ( )
{
	CHECK_NULL_PTR_ERROR (_renderer)
	return *_renderer;
}	// VTKECMPickerCommand::getRenderer


void VTKECMPickerCommand::Log (const TkUtil::Log& log)
{
	AutoMutex	autoMutex (&_mutex);

	if (0 != GetLogDispatcher ( ))
		GetLogDispatcher ( )->log (log);
}	// VTKECMPickerCommand::Log


Mutex& VTKECMPickerCommand::GetMutex ( ) const
{
	return _mutex;
}	// VTKECMPickerCommand::GetMutex


