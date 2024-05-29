#include "Utils/SelectionManager.h"
#include "Utils/Common.h"
#include "Utils/DisplayProperties.h"

#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/LogOutputStream.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/TraceLog.h>

#include <float.h>

using namespace TkUtil;
using namespace std;


/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/


// =========================================================================
//                          SelectionManagerObserver
// =========================================================================

SelectionManagerObserver::SelectionManagerObserver (SelectionManagerIfc* selectionManager)
	: SelectionManagerObserverIfc (selectionManager),
	  _selectionManager (0), _mutex (0), _selectionPolicyAdaptation (false)
{
	_mutex	= new Mutex ( );

	setSelectionManager (selectionManager);
}	// SelectionManagerObserver::SelectionManagerObserver


SelectionManagerObserver::SelectionManagerObserver (const SelectionManagerObserver&)
	: SelectionManagerObserverIfc (0),
	  _selectionManager (0), _mutex (0), _selectionPolicyAdaptation (false)
{
    MGX_FORBIDDEN("SelectionManagerObserver::SelectionManagerObserver is not allowed.");
}	// SelectionManagerObserver::SelectionManagerObserver


SelectionManagerObserver& SelectionManagerObserver::operator = (const SelectionManagerObserver&)
{
    MGX_FORBIDDEN("SelectionManagerObserver::operator = is not allowed.");
	return *this;
}	// SelectionManagerObserver::operator =


SelectionManagerObserver::~SelectionManagerObserver ( )
{
	_mutex->lock ( );

	setSelectionManager (0);

	delete _mutex;
}	// SelectionManagerObserver::~SelectionManagerObserver


void SelectionManagerObserver::setSelectionManager (SelectionManagerIfc* selectionManager)
{
	AutoMutex	autoMutex (getMutex ( ));

	if (0 != _selectionManager)
		_selectionManager->removeSelectionObserver (*this);
	_selectionManager	= selectionManager;
	if (0 != _selectionManager)
		_selectionManager->addSelectionObserver (*this);
}	// SelectionManagerObserver::setSelectionManager


void SelectionManagerObserver::selectionManagerDeleted ( )
{
	_selectionManager	= 0;
}	// SelectionManagerObserver::selectionManagerDeleted


void SelectionManagerObserver::selectionModified ( )
{
}	// SelectionManagerObserver::selectionModified


void SelectionManagerObserver::selectionCleared ( )
{
}	// SelectionManagerObserver::selectionCleared


void SelectionManagerObserver::entitiesAddedToSelection (const vector<Entity*>&)
{
}	// SelectionManagerObserver::entitiesAddedToSelection


void SelectionManagerObserver::entitiesRemovedFromSelection (const vector<Entity*>& entities, bool clear)
{
}	// SelectionManagerObserver::entitiesRemovedFromSelection


bool SelectionManagerObserver::isSelectionPolicyAdaptationEnabled ( ) const
{
	return _selectionPolicyAdaptation;
}	// SelectionManagerObserver::isSelectionPolicyAdaptationEnabled


void SelectionManagerObserver::enableSelectionPolicyAdaptation (bool enable)
{
	AutoMutex	autoMutex (getMutex ( ));

	_selectionPolicyAdaptation	= enable;
}	// SelectionManagerObserver::enableSelectionPolicyAdaptation


Mutex* SelectionManagerObserver::getMutex ( ) const
{
	return _mutex;
}	// SelectionManagerObserver::getMutex



// =========================================================================
//                              SelectionManager
// =========================================================================


SelectionManager::SelectionManager (const string& name, TkUtil::LogOutputStream* los)
	: SelectionManagerIfc ( ),
	  _name (name), _entities ( ), _observers ( ), _mutex (0),
	  m_logOutputStream(los)
{
	_mutex	= new Mutex ( );
}	// SelectionManager::SelectionManager


SelectionManager::SelectionManager (const SelectionManager&)
	: SelectionManagerIfc ( ),
	  _name ("Invalid name"), _entities ( ), _observers ( ), _mutex (0),
	  m_logOutputStream (0)
{
    MGX_FORBIDDEN("SelectionManager::SelectionManager (const SelectionManager&) forbidden.");
}	// SelectionManager::SelectionManager (const SelectionManager&)


SelectionManager::~SelectionManager ( )
{
	_mutex->lock ( );

	for (vector<SelectionManagerObserverIfc*>::iterator it =
						_observers.begin ( ); _observers.end ( ) != it; it++)
		(*it)->selectionManagerDeleted ( );

	clearSelection ( );

	delete _mutex;
}	// SelectionManager::~SelectionManager


SelectionManager& SelectionManager::operator = (const SelectionManager&)
{
    MGX_FORBIDDEN("SelectionManager::operator = (const SelectionManager&) forbidden.");
	return *this;
}	// SelectionManager::operator = (const SelectionManager&)


const string& SelectionManager::getName ( ) const
{
	return _name;
}	// SelectionManager::getName


void SelectionManager::getBounds (double bounds [6])
{
	bounds [0]	= bounds [2]	= bounds [4]	= DBL_MAX;
	bounds [1]	= bounds [3]	= bounds [5]	= -DBL_MAX;

	AutoMutex	autoMutex (getMutex ( ));

	for (vector<Entity*>::iterator	it = _entities.begin ( );
	     _entities.end ( ) != it; it++)
	{
		double	entityBounds [6];
		(*it)->getBounds (entityBounds);
		for (int i = 0; i < 6; i += 2)
		{
			bounds [i]		= bounds [i] <= entityBounds [i] ?
							  bounds [i] :  entityBounds [i];
			bounds [i + 1]	= bounds [i + 1] >= entityBounds [i + 1] ?
							  bounds [i + 1] :  entityBounds [i + 1];
		}	// for (int i = 0; i < 6; i += 2)
	}	// for (vector<Entity*>::iterator   it = ...
}	// SelectionManager::getBounds


bool SelectionManager::isSelectionEmpty ( ) const
{
	return _entities.empty ( );
}	// SelectionManager::isSelectionEmpty


vector<Entity*> SelectionManager::getEntities ( ) const
{
	return _entities;
}	// SelectionManager::getMeshes

std::vector<std::string> SelectionManager::getEntitiesNames ( ) const
{
    std::vector<std::string> entites;
    for (vector<Mgx3D::Utils::Entity*>::const_iterator  it = _entities.begin ( );
             _entities.end ( ) != it; it++)
        entites.push_back((*it)->getName());

        return entites;
}

std::vector<Mgx3D::Utils::Entity*> SelectionManager::getEntities ( Entity::objectType type) const
{
    std::vector<Mgx3D::Utils::Entity*> entites_filtrees;

    for (vector<Mgx3D::Utils::Entity*>::const_iterator  it = _entities.begin( );
         _entities.end ( ) != it; it++)
        if ((*it)->getType() == type)
            entites_filtrees.push_back(*it);

    return entites_filtrees;
}

std::vector<std::string> SelectionManager::getEntitiesNames ( Entity::objectType type) const
{
    std::vector<std::string> entites_filtrees;

    for (vector<Mgx3D::Utils::Entity*>::const_iterator  it = _entities.begin( );
         _entities.end ( ) != it; it++)
        if ((*it)->getType() == type)
            entites_filtrees.push_back((*it)->getName());

    return entites_filtrees;
}


std::vector<std::string> SelectionManager::getEntitiesNames (FilterEntity::objectType mask) const
{
	std::vector<std::string> entites_filtrees;

	for (vector<Mgx3D::Utils::Entity*>::const_iterator  it = _entities.begin( );
	     _entities.end ( ) != it; it++)
		if (0 != ((*it)->getFilteredType() & mask))
			entites_filtrees.push_back((*it)->getName());

	return entites_filtrees;
}


void SelectionManager::addToSelection (const vector<Entity*>& entities)
{
	AutoMutex	autoMutex (getMutex ( ));

	if (0 == entities.size ( ))
		return;

	bool			hasAlreadySelected	= false, first = true;
	UTF8String	alreadySelected (Charset::UTF_8);
	UTF8String   message (Charset::UTF_8);
	message << (1 == entities.size ( ) ?
	           "Sélection de l'entité " : "Sélection des entités ");

	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		CHECK_NULL_PTR_ERROR (*it)

		if (true == isSelected (**it))
		{
			hasAlreadySelected	= true;
			if (0 != alreadySelected.length ( ))
				alreadySelected << ", ";
			alreadySelected << (*it)->getName ( );

			continue;
		}	// if (true == isSelected (**it)

		_entities.push_back (*it);
		if (false == first)
			message << ", ";
		message << (*it)->getName ( );
		first	= false;
	}	// for (vector<Entity*>::const_iterator it = entities.begin ( ); ...

	if (0 != m_logOutputStream)
		m_logOutputStream->log (TraceLog (message, Log::INFORMATION));

	for (vector<SelectionManagerObserverIfc*>::iterator it = _observers.begin ( ); _observers.end ( ) != it; it++)
	{
		(*it)->selectionModified ( );
		(*it)->entitiesAddedToSelection (entities);
	}	// for (vector<SelectionManagerObserverIfc*>::iterator it = ...
}	// SelectionManager::addToSelection


void SelectionManager::removeFromSelection (const vector<Entity*>& entities)
{
	AutoMutex	autoMutex (getMutex ( ));

	if (0 == entities.size ( ))
		return;

	bool			hasAlreadyRemoved	= false, first = true;
	UTF8String	alreadyRemoved (Charset::UTF_8);
	UTF8String   message (Charset::UTF_8);
	message << (1 == entities.size ( ) ?
	           "Désélection de l'entité " : "Désélection des entités ");

	for (vector<Entity*>::const_iterator it1 = entities.begin ( );
	     entities.end ( ) != it1; it1++)
	{
		CHECK_NULL_PTR_ERROR (*it1)

		bool	removed	= false;
		for (vector<Entity*>::iterator it2 = _entities.begin ( );
		     _entities.end ( ) != it2; it2++)
		{
			if (*it1 == *it2)
			{
				removed	= true;
				_entities.erase (it2);
				break;
			}	//if (*it1 == *it2)
		}	// for (vector<Entity*>::iterator it2 = _entities.begin ( );

		if (false == removed)
		{
			hasAlreadyRemoved	= true;
			if (0 != alreadyRemoved.length ( ))
				alreadyRemoved << ", ";
			alreadyRemoved << (*it1)->getName ( );

			continue;
		}	// if (false == removed)

		if (false == first)
			message << ", ";
		message << (*it1)->getName ( );
		first	= false;
	}	// for (vector<Entity*>::const_iterator it1 = entities.begin ( ); ...

	if (0 != m_logOutputStream)
		m_logOutputStream->log (TraceLog (message, Log::INFORMATION));

	for (vector<SelectionManagerObserverIfc*>::iterator it =
						_observers.begin ( ); _observers.end ( ) != it;
		it++)
	{
		(*it)->selectionModified ( );
		(*it)->entitiesRemovedFromSelection (entities, false);
	}	// for (vector<SelectionManagerObserverIfc*>::iterator it = ...
}	// SelectionManager::removeFromSelection


bool SelectionManager::isSelected (const Entity& entity) const
{
    // TODO [EB]: {Optimisation} partie pouvant être couteuse il me semble
    // [CP] : OPTIMISATION
    if (0 != entity.getDisplayProperties ( ).getGraphicalRepresentation ( ))
		return entity.getDisplayProperties ( ).getGraphicalRepresentation ( )->isSelected ( );

	// A priori on ne devrait pas passer ci-dessous :
	AutoMutex	autoMutex (getMutex ( ));

	for (vector<Entity*>::const_iterator it = _entities.begin ( );
	     _entities.end ( ) != it; it++)
			if (*it == &entity)
				return true;

	return false;
}	// SelectionManager::isSelected


void SelectionManager::clearSelection ( )
{
	AutoMutex	autoMutex (getMutex ( ));

	// ATTENTION : on enlève ici, dès le début, chaque entité de la liste, et
	// ce avant d'invoquer les observateurs pour les avertir de l'évènement. En 
	// effet, ceux-ci peuvent etre tentés d'effectuer des requêtes auprès de
	// ce gestionnaire de sélection, notamment un removeFromSelection. C'est
	// par exemple le cas d'un observateur Qt où il est difficile de savoir si
	// l'évènement de désenregistrement provient de ce gestionnaire ou d'un 
	// évenement au niveau de l'IHM.
	if (0 != _entities.size ( ))
	{
		vector<Entity*>	unselected	= _entities;
		_entities.clear ( );
		for (vector<SelectionManagerObserverIfc*>::iterator	its	= _observers.begin ( ); _observers.end ( ) != its; its++)
		{
			(*its)->selectionModified ( );
			(*its)->entitiesRemovedFromSelection (unselected, true);
		}	// for (vector<SelectionManagerObserverIfc*>::iterator its = ...
	}	// if (0 != _entities.size ( ))

	if (0 != m_logOutputStream)
		m_logOutputStream->log (TraceLog (UTF8String("Annulation de la sélection.", Charset::UTF_8), Log::INFORMATION));
}	// SelectionManager::clearSelection


void SelectionManager::addSelectionObserver (SelectionManagerObserverIfc& observer)
{
	AutoMutex	autoMutex (getMutex ( ));

	for (vector<SelectionManagerObserverIfc*>::iterator  it = _observers.begin ( ); _observers.end ( ) != it; it++)
		if ((*it) == &observer)
			return;

	_observers.push_back (&observer);
}	// SelectionManager::addSelectionObserver


void SelectionManager::removeSelectionObserver (SelectionManagerObserverIfc& observer)
{
	AutoMutex	autoMutex (getMutex ( ));

	for (vector<SelectionManagerObserverIfc*>::iterator  it = _observers.begin ( ); _observers.end ( ) != it; it++)
		if ((*it) == &observer)
		{
			_observers.erase (it);
			return;
		}
}	// SelectionManager::removeSelectionObserver


vector<SelectionManagerObserverIfc*> SelectionManager::getObservers ( )
{
	return _observers;
}	// SelectionManager::getObservers


Mutex* SelectionManager::getMutex ( ) const
{
	return _mutex;
}	// SelectionManager::getMutex


void SelectionManager::log (const TkUtil::Log& log)
{
	if (0 != m_logOutputStream)
		m_logOutputStream->log (log);
}	// SelectionManager::log


void SelectionManager::notifyObserversForNewPolicy (void* smp)
{
	AutoMutex	autoMutex (getMutex ( ));

	for (vector<SelectionManagerObserverIfc*>::iterator it =
						_observers.begin ( ); _observers.end ( ) != it;
		it++)
		(*it)->selectionPolicyModified (smp);
}	// SelectionManager::notifyObserversForNewPolicy


/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

