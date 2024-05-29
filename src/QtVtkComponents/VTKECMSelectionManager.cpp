#include "QtVtkComponents/VTKECMSelectionManager.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/LogOutputStream.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/InformationLog.h>

#include <assert.h>
#include <float.h>

using namespace TkUtil;
using namespace std;


// =========================================================================
//                              VTKECMSelectionManager
// =========================================================================


VTKECMSelectionManager::VTKECMSelectionManager (LogOutputStream* los)
	  : _actors ( ), _mutex ( ), _logOutputStream (los)
{
}	// VTKECMSelectionManager::VTKECMSelectionManager


VTKECMSelectionManager::VTKECMSelectionManager (const VTKECMSelectionManager&)
	  : _actors ( ), _mutex ( ), _logOutputStream (0)
{
    assert (0 && "VTKECMSelectionManager copy constructor is forbidden.");
}	// VTKECMSelectionManager::VTKECMSelectionManager


VTKECMSelectionManager::~VTKECMSelectionManager ( )
{
	AutoMutex	autoMutex (&_mutex);

	ClearSelection ( );
}	// VTKECMSelectionManager::~VTKECMSelectionManager


VTKECMSelectionManager& VTKECMSelectionManager::operator = (
												const VTKECMSelectionManager&)
{
    assert (0 && "VTKECMSelectionManager assignment operator is forbidden.");

	return *this;
}	// VTKECMSelectionManager::operator 


void VTKECMSelectionManager::GetBounds (double bounds [6])
{
	bounds [0]	= bounds [2]	= bounds [4]	= DBL_MAX;
	bounds [1]	= bounds [3]	= bounds [5]	= -DBL_MAX;

	AutoMutex	autoMutex (&_mutex);

	for (vector<vtkActor*>::iterator	it = _actors.begin ( );
	     _actors.end ( ) != it; it++)
	{
		double	actorBounds [6];
		(*it)->GetBounds (actorBounds);
		for (int i = 0; i < 6; i += 2)
		{
			bounds [i]		= bounds [i] <= actorBounds [i] ?
							  bounds [i] :  actorBounds [i];
			bounds [i + 1]	= bounds [i + 1] >= actorBounds [i + 1] ?
							  bounds [i + 1] :  actorBounds [i + 1];
		}	// for (int i = 0; i < 6; i += 2)
	}	// for (vector<vtkActor*>::iterator   it = ...
}	// VTKECMSelectionManager::GetBounds


bool VTKECMSelectionManager::IsSelectionEmpty ( ) const
{
	return _actors.empty ( );
}	// VTKECMSelectionManager::IsSelectionEmpty


vector<vtkActor*> VTKECMSelectionManager::GetActors ( ) const
{
	return _actors;
}	// VTKECMSelectionManager::GetActors


void VTKECMSelectionManager::AddToSelection (vtkActor& actor)
{
	AutoMutex	autoMutex (&GetMutex ( ));

	if (true == IsSelected (actor))
	{
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg << "Sélection. Erreur : l'objet " << GetName (actor)
		         << " est déjà sélectionné.";
		throw Exception (errorMsg);
	}	// if (true == IsSelected (actor)
	_actors.push_back (&actor);

	UTF8String	message (Charset::UTF_8);
	message << "Sélection de l'objet " << GetName (actor);
	Log (InformationLog (message));
}	// VTKECMSelectionManager::AddToSelection


void VTKECMSelectionManager::RemoveFromSelection (vtkActor& actor)
{
	AutoMutex	autoMutex (&GetMutex ( ));

	if (false == IsSelected (actor))
	{
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg << "Désélection. Erreur : l'objet "
		         << GetName (actor) << " n'est pas sélectionné.";
		throw Exception (errorMsg);
	}	// if (false == IsSelected (actor) 

	UTF8String	message (Charset::UTF_8);
	message << "Désélection de l'acteur " << GetName (actor);
	Log (InformationLog (message));

	for (vector<vtkActor*>::iterator it = _actors.begin ( );
	      _actors.end ( ) != it; it++)
		if (*it == &actor)
		{
			// On l'élimine de la liste des objets selectionnés pour éviter
			// toute nouvelle requête type RemoveFromSelection.
			_actors.erase (it);

			return;
		}	// if (*it == &actor)
}	// VTKECMSelectionManager::RemoveFromSelection


bool VTKECMSelectionManager::IsSelected (const vtkActor& actor) const
{
    // TODO [EB]: {Optimisation} partie pouvant être couteuse il me semble
	AutoMutex	autoMutex (&GetMutex ( ));
	for (vector<vtkActor*>::const_iterator it = _actors.begin ( );
	     _actors.end ( ) != it; it++)
			if (*it == &actor)
				return true;

	return false;
}	// VTKECMSelectionManager::IsSelected


void VTKECMSelectionManager::ClearSelection ( )
{
	AutoMutex	autoMutex (&GetMutex ( ));

	while (false == _actors.empty ( ))
		_actors.erase (_actors.begin ( ));

	Log (InformationLog (UTF8String ("Annulation de la sélection.", Charset::UTF_8)));
}	// VTKECMSelectionManager::ClearSelection


string VTKECMSelectionManager::GetName (const vtkActor&) const
{
	return "";
}	// VTKECMSelectionManager::GetName


void VTKECMSelectionManager::Log (const TkUtil::Log& log)
{
	AutoMutex	autoMutex (&_mutex);

	if (0 != _logOutputStream)
		_logOutputStream->log (log);
}	// VTKECMSelectionManager::Log


Mutex& VTKECMSelectionManager::GetMutex ( ) const
{
	return _mutex;
}	// VTKECMSelectionManager::GetMutex



