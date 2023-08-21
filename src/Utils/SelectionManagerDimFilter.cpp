/**
 * \file		SelectionManagerDimFilter.cpp
 * \author		Charles PIGNEROL
 * \date		14/10/2013
 */

#include "Utils/SelectionManagerDimFilter.h"

#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>


using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace std;


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Utils {

/*----------------------------------------------------------------------------*/


// =========================================================================
//                           SelectionManagerDimFilter
// =========================================================================


SelectionManagerDimFilter::SelectionManagerDimFilter (const string& name, LogOutputStream* los)
	: SelectionManager (name, los), _dimensions (SelectionManagerIfc::NO_DIM),
	  _filteredTypes (FilterEntity::All)
{
}	// SelectionManagerDimFilter::SelectionManagerDimFilter


SelectionManagerDimFilter::SelectionManagerDimFilter (const SelectionManagerDimFilter&)
	: SelectionManager ("Invalid selection manager", 0),
	  _dimensions (SelectionManagerIfc::NO_DIM),
	  _filteredTypes (FilterEntity::All)
{
    MGX_FORBIDDEN("SelectionManagerDimFilter::SelectionManagerDimFilter (const SelectionManagerDimFilter&) forbidden.");
}	// SelectionManager::SelectionManager (const SelectionManager&)


SelectionManagerDimFilter::~SelectionManagerDimFilter ( )
{
}	// SelectionManagerDimFilter::~SelectionManagerDimFilter


SelectionManagerDimFilter& SelectionManagerDimFilter::operator = (const SelectionManagerDimFilter&)
{
    MGX_FORBIDDEN("SelectionManagerDimFilter::operator = (const SelectionManagerDimFilter&) forbidden.");
	return *this;
}	// SelectionManagerDimFilter::operator = (const SelectionManagerDimFilter&)


bool SelectionManagerDimFilter::isSelectionActivated (const Entity& e) const
{
	AutoMutex	autoMutex (getMutex ( ));

	if (0 == (e.getFilteredType ( ) & getFilteredTypes ( )))
		return false;

	switch (e.getDim ( ))
	{
		case	0	: return is0DSelectionActivated ( );
		case	1	: return is1DSelectionActivated ( );
		case	2	: return is2DSelectionActivated ( );
		case	3	: return is3DSelectionActivated ( );
		default		:
		{
			UTF8String	msg (Charset::UTF_8);
			msg << "SelectionManagerDimFilter::isSelectionActivated "
			    << " erreur : entité " << e.getName ( ) << " de dimension "
			    << (unsigned long)e.getDim ( ) << " non supportée.";
			throw Exception (msg);
		}
	}	// switch (e.getDim ( ))
}	// SelectionManagerDimFilter::isSelectionActivated


FilterEntity::objectType SelectionManagerDimFilter::getFilteredTypes ( ) const
{
	return _filteredTypes;
}	// SelectionManagerDimFilter::getFilteredTypes


SelectionManagerIfc::DIM SelectionManagerDimFilter::getFilteredDimensions ( ) const
{
	return _dimensions;
}	// SelectionManagerDimFilter::getFilteredDimensions


bool SelectionManagerDimFilter::is0DSelectionActivated ( ) const
{
	return 0 != (_dimensions & SelectionManagerIfc::D0) ? true : false;
}	// SelectionManagerDimFilter::is0DSelectionActivated


bool SelectionManagerDimFilter::is1DSelectionActivated ( ) const
{
	return 0 != (_dimensions & SelectionManagerIfc::D1) ? true : false;
}	// SelectionManagerDimFilter::is1DSelectionActivated


bool SelectionManagerDimFilter::is2DSelectionActivated ( ) const
{
	return 0 != (_dimensions & SelectionManagerIfc::D2) ? true : false;
}	// SelectionManagerDimFilter::is2DSelectionActivated


bool SelectionManagerDimFilter::is3DSelectionActivated ( ) const
{
	return 0 != (_dimensions & SelectionManagerIfc::D3) ? true : false;
}	// SelectionManagerDimFilter::is3DSelectionActivated


void SelectionManagerDimFilter::activateSelection (SelectionManagerIfc::DIM dimensions, FilterEntity::objectType mask)
{
	AutoMutex	autoMutex (getMutex ( ));

	if ((dimensions != _dimensions) || (getFilteredTypes ( ) != mask))
	{
		_dimensions		= dimensions;
		_filteredTypes	= mask;

		notifyObserversForNewPolicy ( );
	}	// if (dimensions != _dimensions)
}	// SelectionManagerDimFilter::activate3DSelection



/*----------------------------------------------------------------------------*/
} // end namespace Utils

/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D

/*----------------------------------------------------------------------------*/

