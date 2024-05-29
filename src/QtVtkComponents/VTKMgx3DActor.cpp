#include "Internal/ContextIfc.h"

#include "QtVtkComponents/VTKMgx3DActor.h"
#include "Utils/DisplayProperties.h"
#include "Utils/Entity.h"

#include <vtkRenderer.h>

#include <assert.h>


using namespace std;
using namespace Mgx3D::Utils;


VTKMgx3DActor::VTKMgx3DActor ( )
	: vtkLODActor ( ), _entity (0), _representationType ((DisplayRepresentation::type)0)
{
}	// VTKMgx3DActor::VTKMgx3DActor


VTKMgx3DActor::VTKMgx3DActor (const VTKMgx3DActor&)
	: vtkLODActor ( ), _entity (0), _representationType ((DisplayRepresentation::type)0)
{
	assert (0 && "VTKMgx3DActor copy constructor is not allowed.");
}	// VTKMgx3DActor copy constructor


VTKMgx3DActor& VTKMgx3DActor::operator = (const VTKMgx3DActor&)
{
	assert (0 && "VTKMgx3DActor::operator = is not allowed.");
	return *this;
}	// VTKMgx3DActor::operator =


VTKMgx3DActor::~VTKMgx3DActor ( )
{
}	// VTKMgx3DActor::~VTKMgx3DActor


void VTKMgx3DActor::PrintSelf (ostream& os, vtkIndent indent)
{
	vtkLODActor::PrintSelf (os, indent);
}	// VTKMgx3DActor::PrintSelf


VTKMgx3DActor* VTKMgx3DActor::New ( )
{
	VTKMgx3DActor*	actor	= new VTKMgx3DActor ( );
#ifndef VTK_5
	actor->InitializeObjectBase ( );
#endif	// VTK_5

	return actor;
}	// VTKMgx3DActor::New


const char* VTKMgx3DActor::GetClassName ( )
{
	return "VTKMgx3DActor";
}	// VTKMgx3DActor::GetClassName


void VTKMgx3DActor::Render (vtkRenderer* renderer, vtkMapper* mapper)
{
//cout << __FILE__ << ' ' << __LINE__ << " VTKMgx3DActor::Render ALLOCATED RENDER TIME = " << AllocatedRenderTime << " ESTIMATED RENDER TIME = " << EstimatedRenderTime << " SAVED ESTIMATED RENDER TIME = " << SavedEstimatedRenderTime << " RENDERER ALLOCATED TIME = " << renderer->GetAllocatedRenderTime ( ) << endl;
	vtkLODActor::Render (renderer, mapper);
}	// VTKMgx3DActor::Render


Entity* VTKMgx3DActor::GetEntity ( )
{
	return _entity;
}	// VTKMgx3DActor::GetEntity


const Entity* VTKMgx3DActor::GetEntity ( ) const
{
	return _entity;
}	// VTKMgx3DActor::GetEntity


void VTKMgx3DActor::SetEntity (Entity* entity)
{
	_entity	= entity;
	if ((0 != _entity) &&
	    (0 != _entity->getDisplayProperties ( ).getGraphicalRepresentation ( )))
		SetPickable (_entity->getDisplayProperties ( ).getGraphicalRepresentation ( )->isSelectable ( ));
}	// VTKMgx3DActor::SetEntity


void VTKMgx3DActor::SetRepresentationType (DisplayRepresentation::type type)
{
	_representationType	= type;
}	// VTKMgx3DActor::SetRepresentationType


DisplayRepresentation::type VTKMgx3DActor::GetRepresentationType ( ) const
{
	return _representationType;
}	// VTKMgx3DActor::GetRepresentationType


bool VTKMgx3DActor::IsSolid ( ) const
{
	if (0 != (_representationType & DisplayRepresentation::SOLID))
		return true;

	return false;
}	// VTKMgx3DActor::IsSolid


bool VTKMgx3DActor::IsWire ( ) const
{
	if ((0 != (_representationType & DisplayRepresentation::WIRE)) ||
	    (0 != (_representationType & DisplayRepresentation::ISOCURVE)))
		return true;

	return false;
}	// VTKMgx3DActor::IsWire


void VTKMgx3DActor::CreateOwnLODs ( )
{
	// On ne veut pas de LODs pour les entités topologiques :
	if ((0 != GetEntity ( )) && (false == GetEntity ( )->isTopoEntity ( )))
		vtkLODActor::CreateOwnLODs ( );
}	// VTKMgx3DActor::CreateOwnLODs




