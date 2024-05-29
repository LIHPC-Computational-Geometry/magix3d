/**
 * \file		VTKMgx3DSelectionManager.cpp
 * \author		Charles PIGNEROL
 * \date		23/04/2013
 */

#include "QtVtkComponents/VTKMgx3DSelectionManager.h"
#include "QtVtkComponents/VTKMgx3DActor.h"
#include "QtVtkComponents/VTKConfiguration.h"

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
//                           VTKMgx3DSelectionManager
// =========================================================================


VTKMgx3DSelectionManager::VTKMgx3DSelectionManager (
									const string& name, LogOutputStream* los)
	: SelectionManager (name, los), VTKECMSelectionManager (los),
	  _renderingManager (0)
{
}	// VTKMgx3DSelectionManager::VTKMgx3DSelectionManager


VTKMgx3DSelectionManager::VTKMgx3DSelectionManager (
											const VTKMgx3DSelectionManager&)
	: SelectionManager ("Invalid selection manager", 0),
	  VTKECMSelectionManager (0),
	  _renderingManager (0)
{
    MGX_FORBIDDEN("VTKMgx3DSelectionManager::VTKMgx3DSelectionManager (const VTKMgx3DSelectionManager&) forbidden.");
}	// SelectionManager::SelectionManager (const SelectionManager&)


VTKMgx3DSelectionManager::~VTKMgx3DSelectionManager ( )
{
}	// VTKMgx3DSelectionManager::~VTKMgx3DSelectionManager


VTKMgx3DSelectionManager& VTKMgx3DSelectionManager::operator = (
											const VTKMgx3DSelectionManager&)
{
    MGX_FORBIDDEN("VTKMgx3DSelectionManager::operator = (const VTKMgx3DSelectionManager&) forbidden.");
	return *this;
}	// VTKMgx3DSelectionManager::operator = (const VTKMgx3DSelectionManager&)


VTKRenderingManager& VTKMgx3DSelectionManager::getRenderingManager ( )
{
	AutoMutex	autoMutex (&GetMutex ( ));

	if (0 == _renderingManager)
		throw Exception (UTF8String ("VTKMgx3DSelectionManager::getRenderingManager : absence de gestionnaire de rendu associé.", Charset::UTF_8));

	return *_renderingManager;
}	// VTKMgx3DSelectionManager::getRenderingManager


const VTKRenderingManager&
					VTKMgx3DSelectionManager::getRenderingManager ( ) const
{
	AutoMutex	autoMutex (&GetMutex ( ));

	if (0 == _renderingManager)
		throw Exception (UTF8String ("VTKMgx3DSelectionManager::getRenderingManager : absence de gestionnaire de rendu associé.", Charset::UTF_8));

	return *_renderingManager;
}	// VTKMgx3DSelectionManager::getRenderingManager


void VTKMgx3DSelectionManager::setRenderingManager (
										VTKRenderingManager* renderingManager)
{
	AutoMutex	autoMutex (&GetMutex ( ));

	if (renderingManager != _renderingManager)
	{
		_renderingManager	= renderingManager;
	}	// if (renderingManager != _renderingManager)
}	// VTKMgx3DSelectionManager::setRenderingManager


void VTKMgx3DSelectionManager::AddToSelection (vtkActor& actor)
{
	AutoMutex	autoMutex (&GetMutex ( ));

	// API générique :
	VTKECMSelectionManager::AddToSelection (actor);

	// API Mgx 3D :
	VTKMgx3DActor*	mgxActor	= dynamic_cast<VTKMgx3DActor*>(&actor);
	Entity*		entity		= 0 == mgxActor ? 0 : mgxActor->GetEntity ( );
	if (0 != entity)
	{
		vector<Entity*>	entities;
		entities.push_back (entity);
		addToSelection (entities);
	}	// if (0 != entity)
}	// VTKMgx3DSelectionManager::AddToSelection


void VTKMgx3DSelectionManager::RemoveFromSelection (vtkActor& actor)
{
	AutoMutex	autoMutex (&GetMutex ( ));

	// Version du 03/08/18 : une entité peut être représentée par plusieurs vecteurs,
	// et celui transmis en argument n'est pas forcément celui recensé ...
	VTKMgx3DActor*	mgxActor	= dynamic_cast<VTKMgx3DActor*>(&actor);
	Entity*		entity		= 0 == mgxActor ? 0 : mgxActor->GetEntity ( );

	if (0 == entity)
	{
		VTKECMSelectionManager::RemoveFromSelection (actor);
		return;
	}	// if (0 == entity)

	if (0 != entity)
	{
		vector<Entity*>	entities;
		entities.push_back (entity);
		removeFromSelection (entities);
	}	// if (0 != entity)

	// API générique :
	VTKECMSelectionManager::RemoveFromSelection (actor);
}	// VTKMgx3DSelectionManager::RemoveFromSelection


bool VTKMgx3DSelectionManager::IsSelected (const vtkActor& actor) const
{
	AutoMutex	autoMutex (&GetMutex ( ));
	const VTKMgx3DActor*	mgxActor	= dynamic_cast<const VTKMgx3DActor*>(&actor);
	const Entity*		entity		= 0 == mgxActor ? 0 : mgxActor->GetEntity ( );

	if (0 == entity)
		return VTKECMSelectionManager::IsSelected (actor);

	const vector<Entity*>	entities	= getEntities ( );
	for (vector<Entity*>::const_iterator ite = entities.begin ( );
	     entities.end ( ) != ite; ite++)
		if (*ite == entity)
			return true;

	return VTKECMSelectionManager::IsSelected (actor);
}	// VTKMgx3DSelectionManager::IsSelected


void VTKMgx3DSelectionManager::ClearSelection ( )
{
	AutoMutex	autoMutex (&GetMutex ( ));

	// API générique :
	VTKECMSelectionManager::ClearSelection ( );

	// API Mgx 3D :
	clearSelection ( );
}	// VTKMgx3DSelectionManager::ClearSelection


string VTKMgx3DSelectionManager::GetName (const vtkActor& actor) const
{
	const VTKMgx3DActor*	mgxActor	=
								dynamic_cast<const VTKMgx3DActor*>(&actor);
	const Entity*			entity		= 0 == mgxActor ? 0 : mgxActor->GetEntity ( );

	return 0 == entity ? string ( ) : entity->getName ( );
}	// VTKMgx3DSelectionManager::GetName


/*----------------------------------------------------------------------------*/
} // end namespace QtVtkComponents

/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D

/*----------------------------------------------------------------------------*/

