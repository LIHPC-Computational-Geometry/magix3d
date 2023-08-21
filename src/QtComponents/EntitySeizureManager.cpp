/**
 * \file		EntitySeizureManager.cpp
 * \author		Charles PIGNEROL
 * \date		01/03/2013
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/EntitySeizureManager.h"
#include "QtComponents/QtMgx3DMainWindow.h"

#include <Utils/Common.h>

#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


QColor	EntitySeizureManager::interactiveModeBackground	(220, 200, 240);


EntitySeizureManager::EntitySeizureManager (
	QtMgx3DMainWindow* mainWindow, SelectionManagerIfc::DIM dimensions,
	FilterEntity::objectType types)
	: _mainWindow (mainWindow),
	  _dimensions (dimensions), _types (types), _interactiveMode (false),
	  _multiSelectMode(false), _seizuredEntities ( ),
	  _selectionDims (SelectionManagerIfc::NO_DIM),
	  _selectionTypes (FilterEntity::All),
	  _previousSeizureManager (0), _nextSeizureManager (0)
{
}	// EntitySeizureManager::EntitySeizureManager


EntitySeizureManager::EntitySeizureManager (const EntitySeizureManager&)
	: _mainWindow (0),
	  _dimensions (SelectionManagerIfc::NO_DIM), _types (FilterEntity::All),
	  _interactiveMode (false),
      _multiSelectMode(false), _seizuredEntities ( ),
	  _selectionDims (SelectionManagerIfc::NO_DIM),
	  _selectionTypes (FilterEntity::All),
	  _previousSeizureManager (0), _nextSeizureManager (0)
{
	MGX_FORBIDDEN ("EntitySeizureManager copy constructor is forbidden.")
}	// EntitySeizureManager::EntitySeizureManager


EntitySeizureManager& EntitySeizureManager::operator = (const EntitySeizureManager&)
{
	MGX_FORBIDDEN ("EntitySeizureManager assignment operator is forbidden.")
	return *this;
}	// EntitySeizureManager::operator =


EntitySeizureManager::~EntitySeizureManager ( )
{
	if (true == inInteractiveMode ( ))
		setInteractiveMode (false);

	try
	{	// Cette instance est peut être considérée comme gestionnaire de saisie
		// interactive courant, même si inactif => le déréférencer.
		getMainWindow ( ).unregisterEntitySeizureManager (this);
	}
	catch (...)
	{
	}

	_previousSeizureManager	= 0;
	_nextSeizureManager		= 0;
}	// EntitySeizureManager::~EntitySeizureManager


void EntitySeizureManager::selectionCleared ( )
{
    if (true == inInteractiveMode ( )) 
	{
//        if (true == _multiSelectMode)	// CP : quelque soit le mode
		{
            _seizuredEntities.clear();
            setSelectedEntities (vector<Entity*>( ));
        }
    }
}	// EntitySeizureManager::selectionCleared


bool EntitySeizureManager::actualizeGui (bool removeDestroyed)
{
	return false;
	// A completer par les classes dérivées : actualisation d'un champ de
	// texte, ...
}	// EntitySeizureManager::actualizeGui


void EntitySeizureManager::entitiesAddedToSelection (const vector<Entity*>& entities)
{
	if (true == inInteractiveMode ( ))
	{
		// On retient ceux qui seront traités :
		vector<Entity*>	keptEntities;
		for (vector<Entity*>::const_iterator ite = entities.begin ( );
		     entities.end ( ) != ite; ite++)
			if (true == criteriaMatch ((*ite)->getDim ( ), (*ite)->getFilteredType ( )))
				keptEntities.push_back (*ite);

		if (0 == keptEntities.size ( ))
			return;

		if (false == _multiSelectMode)
		{
			// ATTENTION :
			// _seizuredEntities : utile pour certaines fonctions partagées
			// entre les 2 modes.
			const size_t	count	= keptEntities.size ( );
			if (0 != getSelectionManager ( ))
			{
				// On annule l'éventuelle sélection en cours sans désélectionner
				// les entités de la nouvelle sélection :
				vector<Entity*>	selection	= getSelectionManager ( )->getEntities( );
				for (vector<Entity*>::iterator itk = keptEntities.begin ( );
				     itk != keptEntities.end ( ); itk++)
				{
					for (vector<Entity*>::iterator its = selection.begin ( );
					     its != selection.end ( ); its++)
						if (*its == *itk)
						{
							selection.erase (its);
							break;
						}	// for (vector<Entity*>::iterator its = ...
				}	// for (vector<Entity*>::iterator itk = ...
				if (0 != selection.size ( ))
					getSelectionManager ( )->removeFromSelection (selection);
			}	// if (0 != getSelectionManager ( ))
			_seizuredEntities.clear ( );
			_seizuredEntities.push_back (keptEntities [count - 1]->getUniqueId ( ));
			setSelectedEntity (*keptEntities [count - 1]);
		}	// if (false == _multiSelectMode)
		else	// if (false == _multiSelectMode)
		{
			for (vector<Entity*>::const_iterator it = keptEntities.begin ( );
			     keptEntities.end ( ) != it; it++)
					_seizuredEntities.push_back ((*it)->getUniqueId ( ));

			addToSelection (keptEntities);
		}	// if (true == _multiSelectMode)
	}	// if (true == inInteractiveMode ( ))
}	// EntitySeizureManager::entitiesAddedToSelection


void EntitySeizureManager::entitiesAddedToSelection (const vector<string>& uniqueNames)
{	// Callback à surcharger
}	// EntitySeizureManager::entitiesAddedToSelection


void EntitySeizureManager::entitiesRemovedFromSelection (const vector<Entity*>& entities, bool clear)
{
	if (true == inInteractiveMode ( ))
	{
		// On retient ceux qui seront traités :
		vector<Entity*>	keptEntities;
		for (vector<Entity*>::const_iterator ite = entities.begin ( );
		     entities.end ( ) != ite; ite++)
			if (true == criteriaMatch ((*ite)->getDim ( ), (*ite)->getFilteredType ( )))
			{
	            std::vector<unsigned long>::iterator iter;
				const unsigned long	id	= (*ite)->getUniqueId ( );
				iter = find (_seizuredEntities.begin(), _seizuredEntities.end(), id);
	            if (iter != _seizuredEntities.end())
				{
					_seizuredEntities.erase(iter);
					keptEntities.push_back(&getContext( ).uniqueIdToEntity(id));
				}	// if (iter != _seizuredEntities.end())
			}	// if (true == criteriaMatch (

		if (0 == keptEntities.size ( ))
			return;

		removeFromSelection (keptEntities);
	}	// if (true == inInteractiveMode ( ))
}	// EntitySeizureManager::entitiesRemovedFromSelection


void EntitySeizureManager::entitiesRemovedFromSelection (const vector<string>& uniqueNames)
{	// Callback à surcharger
}	// EntitySeizureManager::entitiesRemovedFromSelection


bool EntitySeizureManager::inInteractiveMode ( ) const
{
	return _interactiveMode;
}	// EntitySeizureManager::inInteractiveMode


void EntitySeizureManager::setInteractiveMode (bool enable)
{
	updateGui (enable);

	// On arrête l'éventuel autre gestionnaire en cours :
	try
	{
		if (true == enable)
			getMainWindow ( ).setEntitySeizureManager (this);
	}
	catch (...)
	{
	}

	vector<unsigned long>	seizured	= _seizuredEntities;

	if (enable == _interactiveMode)
		return;

	_interactiveMode	= enable;
	if (0 == getSelectionManager ( ))
		return;

	if (true == _interactiveMode)
	{
		// Passage en mode sélection d'entités de la dimension souhaitée :
		if (0 != getSelectionManager ( ))	
		{
			_selectionTypes	= getSelectionManager( )->getFilteredTypes ( );
			_selectionDims	= getSelectionManager( )->getFilteredDimensions ( );
			getSelectionManager( )->activateSelection (
							getFilteredDimensions ( ), getFilteredTypes ( ));

			// On ajoute au gestionnaire de sélection la ou les entités déjà
			// sélectionnées :
			vector<Entity*>	entities	= getSelectedEntities ( );
			_seizuredEntities.clear ( );
			if (0 != entities.size ( ))
				getSelectionManager ( )->addToSelection (entities);
		}	// if (0 != getSelectionManager ( ))

	}	// if (true == _interactiveMode)
	else
	{
		// Restauration de l'état initial du gestionnaire de sélection :
		if (0 != getSelectionManager ( ))
		{
			getSelectionManager( )->activateSelection (
											_selectionDims, _selectionTypes);
			_selectionDims	= SelectionManagerIfc::NO_DIM;
			_selectionTypes	= FilterEntity::NoneEntity;
		}	// if (0 != getSelectionManager ( ))
	}	// else if (true == _interactiveMode)
}	// EntitySeizureManager::setInteractiveMode


EntitySeizureManager* EntitySeizureManager::getPreviousSeizureManager ( ) const
{
	return _previousSeizureManager;
}	// EntitySeizureManager::getPreviousSeizureManager


EntitySeizureManager* EntitySeizureManager::getNextSeizureManager ( ) const
{
	return _nextSeizureManager;
}	// EntitySeizureManager::getNextSeizureManager


void EntitySeizureManager::setLinkedSeizureManagers (EntitySeizureManager* prev, EntitySeizureManager* next)
{
	_previousSeizureManager	= prev;
	_nextSeizureManager		= next;
}	// EntitySeizureManager::setLinkedSeizureManagers


void EntitySeizureManager::updateGui (bool enable)
{
	// To overload
}	// EntitySeizureManager::updateGui


bool EntitySeizureManager::criteriaMatch (int dimension, FilterEntity::objectType types) const
{
	if ((0 != (SelectionManagerIfc::dimensionToDimensions (dimension) &
			getFilteredDimensions ( ))) && (0 != (types & getFilteredTypes( ))))
		return true;

	return false;
}	// EntitySeizureManager::criteriaMatch


const ContextIfc& EntitySeizureManager::getContext ( ) const
{
	return getMainWindow ( ).getContext ( );
}	// EntitySeizureManager::getContext


ContextIfc& EntitySeizureManager::getContext ( )
{
	return getMainWindow ( ).getContext ( );
}	// EntitySeizureManager::getContext


const QtMgx3DMainWindow& EntitySeizureManager::getMainWindow ( ) const
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return *_mainWindow;
}	// EntitySeizureManager::getMainWindow


QtMgx3DMainWindow& EntitySeizureManager::getMainWindow ( )
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return *_mainWindow;
}	// EntitySeizureManager::getMainWindow


SelectionManagerIfc* EntitySeizureManager::getSelectionManager ( )
{
	try
	{
		return getMainWindow ( ).getSelectionManager ( );
	}
	catch (...)
	{
		return 0;
	}
}	// EntitySeizureManager::getSelectionManager


FilterEntity::objectType EntitySeizureManager::getFilteredTypes ( ) const
{
//	return _types;
	// On retourne les types filtrés en prenant en compte les
	// dimensions filtrées.
	FilterEntity::objectType	types	= FilterEntity::NoneEntity;
	if (0 != (getFilteredDimensions ( ) & SelectionManagerIfc::D0))
	{
		if (0 != (_types & FilterEntity::GeomVertex))
			types	= (FilterEntity::objectType)(types | FilterEntity::GeomVertex);
		if (0 != (_types & FilterEntity::TopoVertex))
			types	= (FilterEntity::objectType)(types | FilterEntity::TopoVertex);
		if (0 != (_types & FilterEntity::MeshCloud))
			types	= (FilterEntity::objectType)(types | FilterEntity::MeshCloud);
		if (0 != (_types & FilterEntity::Group0D))
			types	= (FilterEntity::objectType)(types | FilterEntity::Group0D);
	}	// if (0 != (getFilteredDimensions ( ) & SelectionManagerIfc::D0))
	if (0 != (getFilteredDimensions ( ) & SelectionManagerIfc::D1))
	{
		if (0 != (_types & FilterEntity::GeomCurve))
			types	= (FilterEntity::objectType)(types | FilterEntity::GeomCurve);
		if (0 != (_types & FilterEntity::TopoEdge))
			types	= (FilterEntity::objectType)(types | FilterEntity::TopoEdge);
		if (0 != (_types & FilterEntity::TopoCoEdge))
			types	= (FilterEntity::objectType)(types | FilterEntity::TopoCoEdge);
		if (0 != (_types & FilterEntity::MeshLine))
			types	= (FilterEntity::objectType)(types | FilterEntity::MeshLine);
		if (0 != (_types & FilterEntity::Group1D))
			types	= (FilterEntity::objectType)(types | FilterEntity::Group1D);
	}	// if (0 != (getFilteredDimensions ( ) & SelectionManagerIfc::D1))
	if (0 != (getFilteredDimensions ( ) & SelectionManagerIfc::D2))
	{
		if (0 != (_types & FilterEntity::GeomSurface))
			types	= (FilterEntity::objectType)(types | FilterEntity::GeomSurface);
		if (0 != (_types & FilterEntity::TopoFace))
			types	= (FilterEntity::objectType)(types | FilterEntity::TopoFace);
		if (0 != (_types & FilterEntity::TopoCoFace))
			types	= (FilterEntity::objectType)(types | FilterEntity::TopoCoFace);
		if (0 != (_types & FilterEntity::MeshSurface))
			types	= (FilterEntity::objectType)(types | FilterEntity::MeshSurface);
		if (0 != (_types & FilterEntity::Group2D))
			types	= (FilterEntity::objectType)(types | FilterEntity::Group2D);
	}	// if (0 != (getFilteredDimensions ( ) & SelectionManagerIfc::D2))
	if (0 != (getFilteredDimensions ( ) & SelectionManagerIfc::D3))
	{
		if (0 != (_types & FilterEntity::GeomVolume))
			types	= (FilterEntity::objectType)(types | FilterEntity::GeomVolume);
		if (0 != (_types & FilterEntity::TopoBlock))
			types	= (FilterEntity::objectType)(types | FilterEntity::TopoBlock);
		if (0 != (_types & FilterEntity::MeshVolume))
			types	= (FilterEntity::objectType)(types | FilterEntity::MeshVolume);
		if (0 != (_types & FilterEntity::MeshSubVolume))
			types	= (FilterEntity::objectType)(types | FilterEntity::MeshSubVolume);
		if (0 != (_types & FilterEntity::Group3D))
			types	= (FilterEntity::objectType)(types | FilterEntity::Group3D);
		if (0 != (_types & FilterEntity::SysCoord))
			types	= (FilterEntity::objectType)(types | FilterEntity::SysCoord);
	}	// if (0 != (getFilteredDimensions ( ) & SelectionManagerIfc::D3))

	return types;
}	// EntitySeizureManager::getFilteredTypes


Entity::objectType EntitySeizureManager::getObjectType ( ) const
{
	if (0 == _seizuredEntities.size ( ))
		return Entity::undefined;

	Entity::objectType	type	= Entity::undefined;
	vector<Entity*>	selection	= getSelectedEntities ( );
	if (0 != selection.size ( ))
	{
		type	= selection [0]->getType ( );
		for (vector<Entity*>::const_iterator it = selection.begin ( );
		     selection.end ( ) != it; it++)
			if ((*it)->getType ( ) != type)
				return Entity::undefined;
	}

	return type;
}	// EntitySeizureManager::getObjectType


size_t EntitySeizureManager::getObjectsCount ( ) const
{
	return _seizuredEntities.size ( );
}	// EntitySeizureManager::getObjectsCount


bool EntitySeizureManager::isMultiSelectMode ( ) const
{
	return _multiSelectMode;
}	// EntitySeizureManager::isMultiSelectMode


void EntitySeizureManager::setMultiSelectMode (bool multi)
{
	_multiSelectMode	= multi;
	if ((false == _multiSelectMode) && (1 < getObjectsCount ( )))
	{
		_seizuredEntities.clear ( );
		setSelectedEntities (vector<Entity*>( ));	
	}
}	// EntitySeizureManager::setMultiSelectMode


void EntitySeizureManager::setSelectedEntity (const Entity& entity)
{
	// A surcharger
}	// EntitySeizureManager::setSelectedEntity


void EntitySeizureManager::setSelectedEntities (const std::vector<Mgx3D::Utils::Entity*>)
{
    // A surcharger
}   // EntitySeizureManager::setSelectedEntities


void EntitySeizureManager::addToSelection (const vector<Entity*>& entities)
{
	for (vector<Entity*>::const_iterator ite = entities.begin ( );
	     entities.end ( ) != ite; ite++)
		_seizuredEntities.push_back ((*ite)->getUniqueId ( ));
}	// EntitySeizureManager::addToSelection


void EntitySeizureManager::removeFromSelection (const vector<Entity*>& entities)
{
	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		const unsigned long	id	= (*it)->getUniqueId ( );
		for (vector<unsigned long>::iterator its = _seizuredEntities.begin ( );
		     _seizuredEntities.end ( ) != its; its++)
		{
			if (*its == id)
			{
				_seizuredEntities.erase (its);
				break;
			}	// if (*its == id)
		}	// for (vector<Entity*>::iterator its = _seizuredEntities.begin ( );
	}	// for (vector<Entity*>::const_iterator it = entities.begin ( ); ...
}	// EntitySeizureManager::removeFromSelection


const vector<Entity*> EntitySeizureManager::getSelectedEntities ( ) const
{
	return getContext ( ).get (_seizuredEntities, false);
}	// EntitySeizureManager::getSelectedEntities


void EntitySeizureManager::reset ( )
{
	clearSeizuredEntities ( );
}	// EntitySeizureManager::reset


void EntitySeizureManager::clearSeizuredEntities ( )
{
	_seizuredEntities.clear ( );
}	// EntitySeizureManager::clearSeizuredEntities


void EntitySeizureManager::setFilteredDimensions (SelectionManagerIfc::DIM dimensions)
{
    _dimensions = dimensions;
	if ((true == inInteractiveMode ( )) && (0 != getSelectionManager( )))
			getSelectionManager( )->activateSelection (
							getFilteredDimensions ( ), getFilteredTypes ( ));
}	// EntitySeizureManager::setFilteredDimensions


SelectionManagerIfc::DIM EntitySeizureManager::getFilteredDimensions ( ) const
{
	return _dimensions;
}	// EntitySeizureManager::getFilteredDimensions


void EntitySeizureManager::setFilteredTypes (FilterEntity::objectType types)
{
    _types = types;
}

}	// namespace QtComponents

}	// namespace Mgx3D
