#include "Utils/SelectionManagerIfc.h"
#include "Utils/Common.h"

#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <float.h>

using namespace TkUtil;
using namespace std;


/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/


// =========================================================================
//                        SelectionManagerObserverIfcIfc
// =========================================================================

SelectionManagerObserverIfc::SelectionManagerObserverIfc (SelectionManagerIfc* selectionManager)
{
}	// SelectionManagerObserverIfc::SelectionManagerObserverIfc


SelectionManagerObserverIfc::SelectionManagerObserverIfc (const SelectionManagerObserverIfc&)
{
    MGX_FORBIDDEN("SelectionManagerObserverIfc::SelectionManagerObserverIfc is not allowed.");
}	// SelectionManagerObserverIfc::SelectionManagerObserverIfc


SelectionManagerObserverIfc& SelectionManagerObserverIfc::operator = (const SelectionManagerObserverIfc&)
{
    MGX_FORBIDDEN("SelectionManagerObserverIfc::operator = is not allowed.");
	return *this;
}	// SelectionManagerObserverIfc::operator =


SelectionManagerObserverIfc::~SelectionManagerObserverIfc ( )
{
}	// SelectionManagerObserverIfc::~SelectionManagerObserverIfc


SelectionManagerIfc* SelectionManagerObserverIfc::getSelectionManager ( )
{
	throw Exception ("SelectionManagerObserverIfc::getSelectionManager should be overloaded.");
}	// SelectionManagerObserverIfc::getSelectionManager


const SelectionManagerIfc*
					SelectionManagerObserverIfc::getSelectionManager ( ) const
{
	throw Exception ("SelectionManagerObserverIfc::getSelectionManager should be overloaded.");
}	// SelectionManagerObserverIfc::getSelectionManager


void SelectionManagerObserverIfc::setSelectionManager (SelectionManagerIfc* selectionManager)
{
	throw Exception ("SelectionManagerObserverIfc::setSelectionManager should be overloaded.");
}	// SelectionManagerObserverIfc::setSelectionManager


void SelectionManagerObserverIfc::selectionManagerDeleted ( )
{
	throw Exception ("SelectionManagerObserverIfc::selectionManagerDeleted should be overloaded.");
}	// SelectionManagerObserverIfc::selectionManagerDeleted


void SelectionManagerObserverIfc::selectionModified ( )
{
	throw Exception ("SelectionManagerObserverIfc::selectionModified should be overloaded.");
}	// SelectionManagerObserverIfc::selectionModified


void SelectionManagerObserverIfc::selectionCleared ( )
{
	throw Exception ("SelectionManagerObserverIfc::selectionCleared should be overloaded.");
}	// SelectionManagerObserverIfc::selectionCleared


void SelectionManagerObserverIfc::entitiesAddedToSelection (const vector<Entity*>&)
{
	throw Exception ("SelectionManagerObserverIfc::entitiesAddedToSelection should be overloaded.");
}	// SelectionManagerObserverIfc::entitiesAddedToSelection


void SelectionManagerObserverIfc::entitiesAddedToSelection (const vector<string>& uniqueNames)
{
}	// SelectionManagerObserverIfc::entitiesAddedToSelection


void SelectionManagerObserverIfc::entitiesRemovedFromSelection (const vector<Entity*>&, bool clear)
{
	throw Exception ("SelectionManagerObserverIfc::entitiesRemovedFromSelection should be overloaded.");
}	// SelectionManagerObserverIfc::entitiesRemovedFromSelection


void SelectionManagerObserverIfc::entitiesRemovedFromSelection (const vector<string>& uniqueNames)
{
}	// SelectionManagerObserverIfc::entitiesRemovedFromSelection


void SelectionManagerObserverIfc::selectionPolicyModified (void* smp)
{
}	// SelectionManagerObserverIfc::selectionPolicyModified


bool SelectionManagerObserverIfc::isSelectionPolicyAdaptationEnabled ( ) const
{
	return false;
}	// SelectionManagerObserverIfc::isSelectionPolicyAdaptationEnabled


void SelectionManagerObserverIfc::enableSelectionPolicyAdaptation (bool enable)
{
}	// SelectionManagerObserverIfc::enableSelectionPolicyAdaptation



// =========================================================================
//                           SelectionManagerIfc
// =========================================================================

const SelectionManagerIfc::DIM SelectionManagerIfc::ALL_DIMENSIONS	= 
					(SelectionManagerIfc::DIM)(SelectionManagerIfc::D0 | SelectionManagerIfc::D1 |
					SelectionManagerIfc::D2 | SelectionManagerIfc::D3);
const SelectionManagerIfc::DIM	SelectionManagerIfc::NOT_NO_DIM	=
					(SelectionManagerIfc::DIM)~SelectionManagerIfc::NO_DIM;
const SelectionManagerIfc::DIM	SelectionManagerIfc::NOT_D0		=
					(SelectionManagerIfc::DIM)~SelectionManagerIfc::D0;
const SelectionManagerIfc::DIM	SelectionManagerIfc::NOT_D1		=
					(SelectionManagerIfc::DIM)~SelectionManagerIfc::D1;
const SelectionManagerIfc::DIM	SelectionManagerIfc::NOT_D2		=
					(SelectionManagerIfc::DIM)~SelectionManagerIfc::D2;
const SelectionManagerIfc::DIM	SelectionManagerIfc::NOT_D3		=
					(SelectionManagerIfc::DIM)~SelectionManagerIfc::D3;


SelectionManagerIfc::SelectionManagerIfc ( )
{
}	// SelectionManagerIfc::SelectionManagerIfc


SelectionManagerIfc::SelectionManagerIfc (const SelectionManagerIfc&)
{
    MGX_FORBIDDEN("SelectionManagerIfc::SelectionManagerIfc (const SelectionManagerIfc&) forbidden.");
}	// SelectionManager::SelectionManager (const SelectionManager&)


SelectionManagerIfc::~SelectionManagerIfc ( )
{
}	// SelectionManagerIfc::~SelectionManagerIfc


SelectionManagerIfc& SelectionManagerIfc::operator = (const SelectionManagerIfc&)
{
    MGX_FORBIDDEN("SelectionManagerIfc::operator = (const SelectionManagerIfc&) forbidden.");
	return *this;
}	// SelectionManagerIfc::operator = (const SelectionManagerIfc&)


const string& SelectionManagerIfc::getName ( ) const
{
	throw Exception ("SelectionManagerObserverIfc::getName should be overloaded.");
}	// SelectionManagerIfc::getName


void SelectionManagerIfc::getBounds (double bounds [6])
{
	throw Exception ("SelectionManagerIfc::getBounds should be overloaded.");
}	// SelectionManager::getBounds


bool SelectionManagerIfc::isSelectionEmpty ( ) const
{
	throw Exception ("SelectionManagerIfc::isSelectionEmpty should be overloaded.");
}	// SelectionManagerIfc::isSelectionEmpty


vector<Entity*> SelectionManagerIfc::getEntities ( ) const
{
	throw Exception ("SelectionManagerIfc::getEntities should be overloaded.");
}	// SelectionManagerIfc::getEntities


vector<unsigned long> SelectionManagerIfc::getEntitiesIds ( ) const
{
	vector<unsigned long>		ids;
	const vector<Entity*>		entities	= getEntities ( );

	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
		ids.push_back ((*it)->getUniqueId ( ));

	return ids;
}	// SelectionManagerIfc::getEntitiesIds

std::vector<std::string> SelectionManagerIfc::getEntitiesNames ( ) const
{
    throw Exception ("SelectionManagerIfc::getEntities should be overloaded.");
}   // SelectionManagerIfc::getEntitiesNames

std::vector<Mgx3D::Utils::Entity*> SelectionManagerIfc::getEntities (Entity::objectType type) const
{
    throw Exception ("SelectionManagerIfc::getEntities should be overloaded.");
}	// SelectionManagerIfc::getEntities

std::vector<std::string> SelectionManagerIfc::getEntitiesNames (Entity::objectType type) const
{
    throw Exception ("SelectionManagerIfc::getEntities should be overloaded.");
}

std::vector<std::string> SelectionManagerIfc::getEntitiesNames (FilterEntity::objectType type) const
{
    throw Exception ("SelectionManagerIfc::getEntities should be overloaded.");
}

uint SelectionManagerIfc::getNbEntities ( )
{
	throw Exception ("SelectionManagerIfc::getNbEntities should be overloaded.");
}	// SelectionManagerIfc::getNbEntities


void SelectionManagerIfc::addToSelection (const vector<Entity*>&)
{
	throw Exception ("SelectionManagerIfc::addToSelection should be overloaded.");
}	// SelectionManagerIfc::addToSelection


void SelectionManagerIfc::removeFromSelection (const vector<Entity*>&)
{
	throw Exception ("SelectionManagerIfc::removeFromSelection should be overloaded.");
}	// SelectionManagerIfc::removeFromSelection


bool SelectionManagerIfc::isSelected (const Entity& entity) const
{
	throw Exception ("SelectionManagerIfc::isSelected should be overloaded.");
}	// SelectionManagerIfc::isSelected


void SelectionManagerIfc::clearSelection ( )
{
	throw Exception ("SelectionManagerIfc::clearSelection should be overloaded.");
}	// SelectionManagerIfc::clearSelection


SelectionManagerIfc::DIM SelectionManagerIfc::dimensionToDimensions (int dimension)
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
}	// SelectionManagerIfc::dimensionToDimensions


int SelectionManagerIfc::dimensionsToDimension (SelectionManagerIfc::DIM dimensions)
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
}	// SelectionManagerIfc::dimensionsToDimension


bool SelectionManagerIfc::isSelectionActivated (const Entity& e) const
{
    return false;
}   // SelectionManagerIfc::isSelectionActivated


FilterEntity::objectType SelectionManagerIfc::getFilteredTypes ( ) const
{
	return FilterEntity::NoneEntity;
}	// SelectionManagerIfc::getFilteredTypes


SelectionManagerIfc::DIM SelectionManagerIfc::getFilteredDimensions ( ) const
{
	return SelectionManagerIfc::NO_DIM;
}	// SelectionManagerIfc::getFilteredDimensions


bool SelectionManagerIfc::is0DSelectionActivated ( ) const
{
	return false;
}	// SelectionManagerIfc::is0DSelectionActivated


bool SelectionManagerIfc::is1DSelectionActivated ( ) const
{
	return false;
}	// SelectionManagerIfc::is1DSelectionActivated


bool SelectionManagerIfc::is2DSelectionActivated ( ) const
{
	return false;
}	// SelectionManagerIfc::is2DSelectionActivated


bool SelectionManagerIfc::is3DSelectionActivated ( ) const
{
	return false;
}	// SelectionManagerIfc::is3DSelectionActivated


void SelectionManagerIfc::activateSelection (SelectionManagerIfc::DIM dimensions, FilterEntity::objectType mask)
{
}	// SelectionManagerIfc::activate3DSelection


void SelectionManagerIfc::addSelectionObserver (SelectionManagerObserverIfc& observer)
{
	throw Exception ("SelectionManagerIfc::addSelectionObserver should be overloaded.");
}	// SelectionManagerIfc::addSelectionObserver


void SelectionManagerIfc::removeSelectionObserver (SelectionManagerObserverIfc& observer)
{
	throw Exception ("SelectionManagerIfc::removeSelectionObserver should be overloaded.");
}	// SelectionManagerIfc::removeSelectionObserver


/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

