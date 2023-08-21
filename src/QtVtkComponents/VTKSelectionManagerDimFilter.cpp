/**
 * \file		VTKSelectionManagerDimFilter.cpp
 * \author		Charles PIGNEROL
 * \date		18/10/2012
 */

#include "Internal/ContextIfc.h"

#include "QtVtkComponents/VTKSelectionManagerDimFilter.h"
#include "QtVtkComponents/VTKConfiguration.h"
#include "QtVtkComponents/VTKMgx3DActor.h"
#include "Utils/SelectionManagerDimFilter.h"

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
namespace QtVtkComponents {

/*----------------------------------------------------------------------------*/


// =========================================================================
//                           VTKSelectionManagerDimFilter
// =========================================================================


VTKSelectionManagerDimFilter::VTKSelectionManagerDimFilter (
									const string& name, LogOutputStream* los)
	: SelectionManager (name, los),
	  SelectionManagerDimFilter (name, los),
	  VTKMgx3DSelectionManager (name, los)
{
}	// VTKSelectionManagerDimFilter::VTKSelectionManagerDimFilter


VTKSelectionManagerDimFilter::VTKSelectionManagerDimFilter (
											const VTKSelectionManagerDimFilter&)
	: SelectionManager ("Invalid selection manager", 0),
	  SelectionManagerDimFilter ("Invalid selection manager", 0),
	  VTKMgx3DSelectionManager ("Invalid selection manager", 0)
{
    MGX_FORBIDDEN("VTKSelectionManagerDimFilter::VTKSelectionManagerDimFilter (const VTKSelectionManagerDimFilter&) forbidden.");
}	// SelectionManager::SelectionManager (const SelectionManager&)


VTKSelectionManagerDimFilter::~VTKSelectionManagerDimFilter ( )
{
}	// VTKSelectionManagerDimFilter::~VTKSelectionManagerDimFilter


VTKSelectionManagerDimFilter& VTKSelectionManagerDimFilter::operator = (
											const VTKSelectionManagerDimFilter&)
{
    MGX_FORBIDDEN("VTKSelectionManagerDimFilter::operator = (const VTKSelectionManagerDimFilter&) forbidden.");
	return *this;
}	// VTKSelectionManagerDimFilter::operator = (const VTKSelectionManagerDimFilter&)


void VTKSelectionManagerDimFilter::activateSelection (
			SelectionManagerIfc::DIM dimensions, FilterEntity::objectType mask)
{
	AutoMutex	autoMutex (&GetMutex ( ));

	if ((dimensions != getFilteredDimensions ( )) ||
	    (getFilteredTypes ( ) != mask))
	{
		SelectionManagerDimFilter::activateSelection (dimensions, mask);

		// On garde la sensibilité la plus forte ...
		try
		{
			double	tolerance	= 0.;
			if ((0 != (dimensions & SelectionManagerIfc::D0)) &&
				(tolerance < VTKConfiguration::instance ( )._0DPickerTolerance.getValue ( )))
				tolerance	= VTKConfiguration::instance ( )._0DPickerTolerance.getValue ( );
			if ((0 != (dimensions & SelectionManagerIfc::D1)) &&
				(tolerance < VTKConfiguration::instance ( )._1DPickerTolerance.getValue ( )))
				tolerance	= VTKConfiguration::instance ( )._1DPickerTolerance.getValue ( );
			if ((0 != (dimensions & SelectionManagerIfc::D2)) &&
				(tolerance < VTKConfiguration::instance ( )._2DPickerTolerance.getValue ( )))
				tolerance	= VTKConfiguration::instance ( )._2DPickerTolerance.getValue ( );
			if ((0 != (dimensions & SelectionManagerIfc::D3)) &&
				(tolerance < VTKConfiguration::instance ( )._3DPickerTolerance.getValue ( )))
				tolerance	= VTKConfiguration::instance ( )._3DPickerTolerance.getValue ( );
			getRenderingManager ( ).getPicker ( ).SetTolerance (tolerance);
			getRenderingManager ( ).getPicker ( ).SetCellTolerance (VTKConfiguration::instance ( )._cellPickerTolerance.getValue( ));
			getRenderingManager ( ).getMgx3DInteractorStyle ( ).SetInteractiveSelectionActivated (SelectionManagerIfc::NO_DIM == dimensions ? false : true);
		}
		catch (...)
		{	// Côté serveur il n'y a pas toute cette mécanique ...
		}

		// Si l'entité est de dimension dim et de masque compatible avec mask on
		// lui applique SetPickable ( ).
		// Dans le cas contraire elle n'est pas sélectionnable.
		vtkRenderer&		renderer	= getRenderingManager ( ).getRenderer ( );
		vtkPropCollection*	props		= renderer.GetViewProps ( );
		CHECK_NULL_PTR_ERROR (props)
		props->InitTraversal ( );
		for (vtkProp* prop = props->GetNextProp ( ); 0 != prop; prop = props->GetNextProp ( ))
		{
			VTKMgx3DActor*	mgxActor	= dynamic_cast<VTKMgx3DActor*>(prop);
			Entity*			entity		= 0 == mgxActor ? 0 : mgxActor->GetEntity  ( );
			if (0 != entity)
			{
				if (SelectionManagerIfc::NO_DIM == dimensions)
				{
					entity->getDisplayProperties ( ).getGraphicalRepresentation ( )->setSelectable (false);
					prop->SetPickable (false);
				}
				else
				{
					const bool pickable	= isSelectionActivated (*entity);
					entity->getDisplayProperties ().getGraphicalRepresentation ( )->setSelectable (pickable);
					prop->SetPickable (pickable);
				}
			}	// if (0 != entity)
		}	// for (vtkProp* prop = props->GetNextProp ( );
	}	// if ((dimensions != getFilteredDimensions ( )) || ...
}	// VTKSelectionManagerDimFilter::activateSelection



/*----------------------------------------------------------------------------*/
} // end namespace QtVtkComponents

/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D

/*----------------------------------------------------------------------------*/

