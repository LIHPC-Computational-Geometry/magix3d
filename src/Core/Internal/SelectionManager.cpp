#include "Internal/SelectionManager.h"
#include "Internal/SelectionHelper.h"
#include "Utils/Common.h"
#include "Utils/DisplayProperties.h"

#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/LogOutputStream.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/TraceLog.h>

#include <assert.h>
#include <float.h>
#include <algorithm>

using namespace TkUtil;
using namespace std;


/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/


// =========================================================================
//                          SelectionManagerObserver
// =========================================================================

SelectionManagerObserver::SelectionManagerObserver (SelectionManager* selectionManager)
	: _selectionManager (0), _mutex (0), _selectionPolicyAdaptation (false)
{
	_mutex	= new Mutex ( );

	setSelectionManager (selectionManager);
}	// SelectionManagerObserver::SelectionManagerObserver


SelectionManagerObserver::SelectionManagerObserver (const SelectionManagerObserver&)
	: _selectionManager (0), _mutex (0), _selectionPolicyAdaptation (false)
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


void SelectionManagerObserver::setSelectionManager (SelectionManager* selectionManager)
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


void SelectionManagerObserver::entitiesAddedToSelection (const vector<Utils::Entity*>&)
{
}	// SelectionManagerObserver::entitiesAddedToSelection

void SelectionManagerObserver::entitiesAddedToSelection (const vector<string>& uniqueNames)
{
}	// SelectionManagerObserver::entitiesAddedToSelection


void SelectionManagerObserver::entitiesRemovedFromSelection (const vector<Utils::Entity*>& entities, bool clear)
{
}	// SelectionManagerObserver::entitiesRemovedFromSelection

void SelectionManagerObserver::entitiesRemovedFromSelection (const vector<string>& uniqueNames)
{
}	// SelectionManagerObserver::entitiesRemovedFromSelection

void SelectionManagerObserver::selectionPolicyModified (void* smp)
{
}	// SelectionManagerObserver::selectionPolicyModified

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

const SelectionManager::DIM SelectionManager::ALL_DIMENSIONS	=
					(SelectionManager::DIM)(SelectionManager::D0 | SelectionManager::D1 |
					SelectionManager::D2 | SelectionManager::D3);
const SelectionManager::DIM	SelectionManager::NOT_NO_DIM	=
					(SelectionManager::DIM)~SelectionManager::NO_DIM;
const SelectionManager::DIM	SelectionManager::NOT_D0		=
					(SelectionManager::DIM)~SelectionManager::D0;
const SelectionManager::DIM	SelectionManager::NOT_D1		=
					(SelectionManager::DIM)~SelectionManager::D1;
const SelectionManager::DIM	SelectionManager::NOT_D2		=
					(SelectionManager::DIM)~SelectionManager::D2;
const SelectionManager::DIM	SelectionManager::NOT_D3		=
					(SelectionManager::DIM)~SelectionManager::D3;

SelectionManager::SelectionManager (const string& name, TkUtil::LogOutputStream* los)
	: _name (name), _entities ( ), _observers ( ), _mutex (0),
	  _undoStack ( ), _currentAction ((size_t)-1), m_logOutputStream(los)
{
	_mutex	= new Mutex ( );
}	// SelectionManager::SelectionManager


SelectionManager::SelectionManager (const SelectionManager&)
	: _name ("Invalid name"), _entities ( ), _observers ( ), _mutex (0),
	  _undoStack ( ), _currentAction ((size_t)-1), m_logOutputStream (0)
{
    MGX_FORBIDDEN("SelectionManager::SelectionManager (const SelectionManager&) forbidden.");
}	// SelectionManager::SelectionManager (const SelectionManager&)


SelectionManager::~SelectionManager ( )
{
	_mutex->lock ( );

	for (vector<SelectionManagerObserver*>::iterator it = _observers.begin ( ); _observers.end ( ) != it; it++)
		(*it)->selectionManagerDeleted ( );

	clearSelection (false);

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

	for (vector<Utils::Entity*>::iterator	it = _entities.begin ( ); _entities.end ( ) != it; it++)
	{
		double	entityBounds [6];
		(*it)->getBounds (entityBounds);
		for (int i = 0; i < 6; i += 2)
		{
			bounds [i]		= bounds [i] <= entityBounds [i] ? bounds [i] :  entityBounds [i];
			bounds [i + 1]	= bounds [i + 1] >= entityBounds [i + 1] ? bounds [i + 1] :  entityBounds [i + 1];
		}	// for (int i = 0; i < 6; i += 2)
	}	// for (vector<Entity*>::iterator   it = ...
}	// SelectionManager::getBounds


bool SelectionManager::isSelectionEmpty ( ) const
{
	return _entities.empty ( );
}	// SelectionManager::isSelectionEmpty


vector<Utils::Entity*> SelectionManager::getEntities ( ) const
{
	return _entities;
}	// SelectionManager::getMeshes

vector<unsigned long> SelectionManager::getEntitiesIds ( ) const
{
	vector<unsigned long>		ids;
	const vector<Utils::Entity*>		entities	= getEntities ( );

	for (vector<Utils::Entity*>::const_iterator it = entities.begin ( );
		 entities.end ( ) != it; it++)
		ids.push_back ((*it)->getUniqueId ( ));

	return ids;
}	// SelectionManager::getEntitiesIds

std::vector<std::string> SelectionManager::getEntitiesNames ( ) const
{
    std::vector<std::string> entites;
    for (vector<Mgx3D::Utils::Entity*>::const_iterator  it = _entities.begin ( ); _entities.end ( ) != it; it++)
        entites.push_back((*it)->getName());

        return entites;
}

std::vector<Mgx3D::Utils::Entity*> SelectionManager::getEntities (Utils::Entity::objectType type) const
{
    std::vector<Mgx3D::Utils::Entity*> entites_filtrees;

    for (vector<Mgx3D::Utils::Entity*>::const_iterator  it = _entities.begin( ); _entities.end ( ) != it; it++)
        if ((*it)->getType() == type)
            entites_filtrees.push_back(*it);

    return entites_filtrees;
}


std::vector<std::string> SelectionManager::getEntitiesNames (Utils::Entity::objectType type) const
{
    std::vector<std::string> entites_filtrees;

    for (vector<Mgx3D::Utils::Entity*>::const_iterator  it = _entities.begin( ); _entities.end ( ) != it; it++)
        if ((*it)->getType() == type)
            entites_filtrees.push_back((*it)->getName());

    return entites_filtrees;
}


std::vector<std::string> SelectionManager::getEntitiesNames (Utils::FilterEntity::objectType mask) const
{
	std::vector<std::string> entites_filtrees;

	for (vector<Mgx3D::Utils::Entity*>::const_iterator  it = _entities.begin( ); _entities.end ( ) != it; it++)
		if (0 != ((*it)->getFilteredType() & mask))
			entites_filtrees.push_back((*it)->getName());

	return entites_filtrees;
}


void SelectionManager::addToSelection (const vector<Utils::Entity*>& entities)
{
	addToSelection (entities, true);
}	// SelectionManager::addToSelection


void SelectionManager::removeFromSelection (const vector<Utils::Entity*>& entities)
{
	removeFromSelection (entities, true);
}	// SelectionManager::removeFromSelection


bool SelectionManager::isSelected (const Utils::Entity& entity) const
{
    // TODO [EB]: {Optimisation} partie pouvant être couteuse il me semble
    // [CP] : OPTIMISATION
    if (0 != entity.getDisplayProperties ( ).getGraphicalRepresentation ( ))
		return entity.getDisplayProperties ( ).getGraphicalRepresentation ( )->isSelected ( );

	// A priori on ne devrait pas passer ci-dessous :
	AutoMutex	autoMutex (getMutex ( ));

	for (vector<Utils::Entity*>::const_iterator it = _entities.begin ( ); _entities.end ( ) != it; it++)
		if (*it == &entity)
			return true;

	return false;
}	// SelectionManager::isSelected


void SelectionManager::clearSelection ( )
{
	clearSelection (true);
}	// SelectionManager::clearSelection


void SelectionManager::undo ( )
{
	if (false == isUndoable ( ))
	{
		INTERNAL_ERROR (exc, "Annulation de la dernière opération de sélection demandée.", "Absence d'opération à annuler.")
		throw (exc);
	}	// if (false == isUndoable ( ))
	
	assert ((size_t)-1 != _currentAction);
	pair<STACK_ACTION, vector<Mgx3D::Utils::Entity*>>	p	= _undoStack [_currentAction];
	_currentAction--;
	switch (p.first)
	{
		case SelectionManager::ADD		: removeFromSelection (p.second, false);	break;
		case SelectionManager::REMOVE	: addToSelection (p.second, false);			break;
		case SelectionManager::CLEAR	: addToSelection (p.second, false);			break;
		default							: 
		{
			INTERNAL_ERROR (exc, "Annulation de la dernière opération de sélection demandée.", "Type d'opération inconnu.")
			throw (exc);
		}
	}	// switch (p.first)	
}	// SelectionManager::undo


void SelectionManager::redo ( )
{
	if (false == isRedoable ( ))
	{
		INTERNAL_ERROR (exc, "Rejeu de la dernière opération de sélection demandée.", "Absence d'opération à rejouer.")
		throw (exc);
	}	// if (false == isRedoable ( ))
	
	assert (_currentAction + 1 < _undoStack.size ( ));
	pair<STACK_ACTION, vector<Mgx3D::Utils::Entity*>>	p	= _undoStack [++_currentAction];
	switch (p.first)
	{
		case SelectionManager::ADD		: addToSelection (p.second, false);			break;
		case SelectionManager::REMOVE	: removeFromSelection (p.second, false);	break;
		case SelectionManager::CLEAR	: removeFromSelection (p.second, false);	break;
		default							: 
		{
			INTERNAL_ERROR (exc, "Rejeu de la dernière opération de sélection demandée.", "Type d'opération inconnu.")
			throw (exc);
		}
	}	// switch (p.first)	
}	// SelectionManager::redo


void SelectionManager::resetUndoStack ( )
{
	_currentAction	= (size_t)-1;
	_undoStack.clear ( );
}	// SelectionManager::resetUndoStack


bool SelectionManager::isUndoable ( ) const
{
	return (size_t)-1 != _currentAction ? true : false;
}	// SelectionManager::isUndoable


bool SelectionManager::isRedoable ( ) const
{
	if (false == _undoStack.empty ( ))
		return (_currentAction < _undoStack.size ( ) - 1) || ((size_t)-1 == _currentAction) ? true : false;

	return false;
}	// SelectionManager::isRedoable


void SelectionManager::addToSelection (const vector<Utils::Entity*>& entities, bool undoable)
{
	AutoMutex	autoMutex (getMutex ( ));

	if (0 == entities.size ( ))
		return;

	bool		hasAlreadySelected	= false, first = true;
	UTF8String	alreadySelected (Charset::UTF_8);
	UTF8String   message (Charset::UTF_8);
	message << (1 == entities.size ( ) ? "Sélection de l'entité " : "Sélection des entités ");

	for (vector<Utils::Entity*>::const_iterator it = entities.begin ( ); entities.end ( ) != it; it++)
	{
		CHECK_NULL_PTR_ERROR (*it)

		// On filtre, certaines entités peuvent être présentes 2 fois (acteurs VTK filaire + isofilaire par ex) :
		vector<Utils::Entity*>::const_iterator itnext	= it;	itnext++;
		vector<Utils::Entity*>::const_iterator itfound	= find (itnext, entities.end ( ), *it);
		if (entities.end ( ) != itfound)
			continue;	// La dernière occurence sera ajoutée

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

	if (true == undoable)
		pushAction (SelectionManager::ADD, entities);

	if (0 != m_logOutputStream)
		m_logOutputStream->log (TraceLog (message, Log::INFORMATION));

	for (vector<SelectionManagerObserver*>::iterator it = _observers.begin ( ); _observers.end ( ) != it; it++)
	{
		(*it)->selectionModified ( );
		(*it)->entitiesAddedToSelection (entities);
	}	// for (vector<SelectionManagerObserver*>::iterator it = ...
}	// SelectionManager::addToSelection


void SelectionManager::removeFromSelection (const vector<Utils::Entity*>& entities, bool undoable)
{
	AutoMutex	autoMutex (getMutex ( ));

	if (0 == entities.size ( ))
		return;

	bool		hasAlreadyRemoved	= false, first = true;
	UTF8String	alreadyRemoved (Charset::UTF_8);
	UTF8String   message (Charset::UTF_8);
	message << (1 == entities.size ( ) ? "Désélection de l'entité " : "Désélection des entités ");

	for (vector<Utils::Entity*>::const_iterator it1 = entities.begin ( ); entities.end ( ) != it1; it1++)
	{
		CHECK_NULL_PTR_ERROR (*it1)

		bool	removed	= false;
		for (vector<Utils::Entity*>::iterator it2 = _entities.begin ( ); _entities.end ( ) != it2; it2++)
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
	
	if (true == undoable)
		pushAction (SelectionManager::REMOVE, entities);

	if (0 != m_logOutputStream)
		m_logOutputStream->log (TraceLog (message, Log::INFORMATION));

	for (vector<SelectionManagerObserver*>::iterator it = _observers.begin ( ); _observers.end ( ) != it; it++)
	{
		(*it)->selectionModified ( );
		(*it)->entitiesRemovedFromSelection (entities, false);
	}	// for (vector<SelectionManagerObserver*>::iterator it = ...
}	// SelectionManager::removeFromSelection


void SelectionManager::clearSelection (bool undoable)
{
	AutoMutex	autoMutex (getMutex ( ));

	// ATTENTION : on enlève ici, dès le début, chaque entité de la liste, et ce avant d'invoquer les observateurs pour les avertir de l'évènement. En 
	// effet, ceux-ci peuvent etre tentés d'effectuer des requêtes auprès de ce gestionnaire de sélection, notamment un removeFromSelection. C'est
	// par exemple le cas d'un observateur Qt où il est difficile de savoir si l'évènement de désenregistrement provient de ce gestionnaire ou d'un 
	// évenement au niveau de l'IHM.
	const vector<Utils::Entity*>	unselected	= _entities;
	if (0 != _entities.size ( ))
	{
		_entities.clear ( );
		for (vector<SelectionManagerObserver*>::iterator	its	= _observers.begin ( ); _observers.end ( ) != its; its++)
		{
			(*its)->selectionModified ( );
			(*its)->entitiesRemovedFromSelection (unselected, true);
		}	// for (vector<SelectionManagerObserver*>::iterator its = ...
	}	// if (0 != _entities.size ( ))
	
	if (true == undoable)
		pushAction (SelectionManager::CLEAR, unselected);	// Le clear n'est jamais qu'un remove ;)

	if (0 != m_logOutputStream)
		m_logOutputStream->log (TraceLog (UTF8String("Annulation de la sélection.", Charset::UTF_8), Log::INFORMATION));
}	// SelectionManager::clearSelection


void SelectionManager::pushAction (STACK_ACTION action, const std::vector<Mgx3D::Utils::Entity*>& entities)
{
	switch (action)
	{
		case SelectionManager::CLEAR	:	// Ne pas engorger la pile de "clear" successifs.
			if ((true == _undoStack.empty ( )) || ((size_t)-1 == _currentAction) || (_undoStack.at (_currentAction).first == action) || (false == entities.size ( )))
			{
				return;
			}
	}	// switch (action)
	
	if (_currentAction < _undoStack.size ( ) - 1)	// Se débarasser des sélections annulées préalablement
	{
//cout << "BEFORE ERASE :" << endl;	printActionsStack (cout);
		_undoStack.erase (_undoStack.begin ( ) + _currentAction + 1, _undoStack.end ( ));
//cout << "AFTER ERASE :" << endl;	printActionsStack (cout);
	}	// if (_currentAction < _undoStack.size ( ) - 1)

	_undoStack.push_back (pair<STACK_ACTION, vector<Mgx3D::Utils::Entity*>>(action, entities));
	_currentAction++;
//cout << "AFTER PUSH :" << endl;	printActionsStack (cout);	cout << __FILE__ << ' ' << __LINE__ << " 	SelectionManager::pushAction. CURRENT=" << getCurrentAction ( ) << endl;
}	// SelectionManager::pushAction


string SelectionManager::getCurrentAction ( ) const
{
	UTF8String	action (Charset::UTF_8);
	
	if ((size_t)-1 == _currentAction)
		action << "Absence d'action de sélection";
	else
		action << getAction (_currentAction);

	return action.utf8 ( );
}	// SelectionManager::getCurrentAction


string SelectionManager::getAction (size_t i) const
{
	UTF8String	action (Charset::UTF_8);
	
	if (_undoStack.size ( ) <= i)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Absence de " << i << "-ème action dans SelectionManager::getAction.";
		throw (error);
	}

	pair<STACK_ACTION, vector<Mgx3D::Utils::Entity*>>	p	= _undoStack [i];
	switch (p.first)
	{
		case SelectionManager::ADD		: action << "ADD entities ";	break;
		case SelectionManager::REMOVE	: action << "REMOVE entities ";	break;
		case SelectionManager::CLEAR	: action << "CLEAR entities ";	break;
		default							: action << "non renseignée (erreur interne).";	return action.utf8 ( );
	}	// switch (p.first)

	const size_t printCount	= p.second.size ( ) <= 5 ? p.second.size ( ) : 5;
	for (size_t i = 0; i < printCount; i++)
		action << p.second [i]->getName ( ) << " ";

	return action.utf8 ( );
}	// SelectionManager::getAction


void SelectionManager::printActionsStack (ostream& stream) const
{
	if ((size_t)-1 == _currentAction)
		stream << "Absence d'action de sélection." << endl;
	else
	{
		for (size_t i = 0; i < _currentAction; i++)
			stream << "   " << getAction (i) << endl;
		stream << "=> " << getCurrentAction ( ) << endl;
		for (size_t i = _currentAction + 1; i < _undoStack.size ( ); i++)
			stream << "=> " << getAction (i) << endl;
	}	// if ((size_t)-1 == _currentAction)
}	// SelectionManager::printActionsStack

SelectionManager::DIM SelectionManager::dimensionToDimensions (int dimension)
{
	switch (dimension)
	{
	case	0	: return D0;
	case	1	: return D1;
	case	2	: return D2;
	case	3	: return D3;
	}	// switch (dimension)

	UTF8String	message (Charset::UTF_8);
	message << "Dimension " << (long)dimension << " non supportée.";
	throw Exception (message);
}	// SelectionManager::dimensionToDimensions


int SelectionManager::dimensionsToDimension (SelectionManager::DIM dimensions)
{
	switch (dimensions)
	{
	case	D0	: return 0;
	case	D1	: return 1;
	case	D2	: return 2;
	case	D3	: return 3;
	}	// switch (dimensions)

	UTF8String	message (Charset::UTF_8);
	message << "Dimension " << (long)dimensions << " non supportée.";
	throw Exception (message);
}	// SelectionManager::dimensionsToDimension


bool SelectionManager::isSelectionActivated (const Utils::Entity& e) const
{
	return false;
}   // SelectionManager::isSelectionActivated


		Utils::FilterEntity::objectType SelectionManager::getFilteredTypes ( ) const
{
	return Utils::FilterEntity::NoneEntity;
}	// SelectionManager::getFilteredTypes


SelectionManager::DIM SelectionManager::getFilteredDimensions ( ) const
{
	return SelectionManager::NO_DIM;
}	// SelectionManager::getFilteredDimensions


bool SelectionManager::is0DSelectionActivated ( ) const
{
	return false;
}	// SelectionManager::is0DSelectionActivated


bool SelectionManager::is1DSelectionActivated ( ) const
{
	return false;
}	// SelectionManager::is1DSelectionActivated


bool SelectionManager::is2DSelectionActivated ( ) const
{
	return false;
}	// SelectionManager::is2DSelectionActivated


bool SelectionManager::is3DSelectionActivated ( ) const
{
	return false;
}	// SelectionManager::is3DSelectionActivated

	
void SelectionManager::activateSelection (SelectionManager::DIM dimensions, Utils::FilterEntity::objectType mask)
{
}	// SelectionManager::activate3DSelection

	
void SelectionManager::addSelectionObserver (SelectionManagerObserver& observer)
{
	AutoMutex	autoMutex (getMutex ( ));

	for (vector<SelectionManagerObserver*>::iterator  it = _observers.begin ( ); _observers.end ( ) != it; it++)
		if ((*it) == &observer)
			return;

	_observers.push_back (&observer);
}	// SelectionManager::addSelectionObserver


void SelectionManager::removeSelectionObserver (SelectionManagerObserver& observer)
{
	AutoMutex	autoMutex (getMutex ( ));

	for (vector<SelectionManagerObserver*>::iterator  it = _observers.begin ( ); _observers.end ( ) != it; it++)
		if ((*it) == &observer)
		{
			_observers.erase (it);
			return;
		}
}	// SelectionManager::removeSelectionObserver


vector<SelectionManagerObserver*> SelectionManager::getObservers ( )
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

	for (vector<SelectionManagerObserver*>::iterator it = _observers.begin ( ); _observers.end ( ) != it; it++)
		(*it)->selectionPolicyModified (smp);
}	// SelectionManager::notifyObserversForNewPolicy


void SelectionManager::selectSheet(Utils::Entity* e, double* point){
	//We need to filter to topoEntities
	std::vector<Utils::Entity::objectType> validTypes =
			{Utils::Entity::TopoBlock,Utils::Entity::TopoFace,Utils::Entity::TopoCoFace,
			 Utils::Entity::TopoEdge,Utils::Entity::TopoCoEdge};

	if(std::find(validTypes.begin(), validTypes.end(),e->getType()) != validTypes.end()) {
		std::vector<Topo::TopoEntity *> topoEntities =
				Internal::SelectionHelper::selectSheet(dynamic_cast<Topo::TopoEntity *>(e), point);

		std::vector<Utils::Entity *> entities;
		entities.reserve(topoEntities.size());
		for (auto topoEntity: topoEntities) {
			entities.push_back(topoEntity);
		}

		addToSelection(entities);
	}
	//If not topoEntity do nothing
}

void SelectionManager::selectChord(Utils::Entity* e, double* point){
	//We need to filter to topoEntities
	std::vector<Utils::Entity::objectType> validTypes =
			{Utils::Entity::TopoBlock,Utils::Entity::TopoFace,Utils::Entity::TopoCoFace,
			 Utils::Entity::TopoEdge,Utils::Entity::TopoCoEdge};

	if(std::find(validTypes.begin(), validTypes.end(),e->getType()) != validTypes.end()) {
		std::vector<Topo::TopoEntity *> topoEntities =
				Internal::SelectionHelper::selectChord(dynamic_cast<Topo::TopoEntity *>(e), point);

		std::vector<Utils::Entity *> entities;
		entities.reserve(topoEntities.size());
		for (auto topoEntity: topoEntities) {
			entities.push_back(topoEntity);
		}

		addToSelection(entities);
	}
	//If not topoEntity do nothing
}
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

