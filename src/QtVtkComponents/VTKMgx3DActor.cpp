#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"
#include "QtVtkComponents/vtkMgx3DInteractorStyle.h"

#include "QtVtkComponents/VTKMgx3DActor.h"
#include "Utils/DisplayProperties.h"
#include "Utils/Entity.h"

#include <vtkRenderer.h>

#include <assert.h>
#include <random>


using namespace std;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


// On préfère uniform_int_distribution à random pour avoir une distribution homogène des nombres aléatoires :
static random_device				rd;					// a seed source for the random number engine
static mt19937						gen (rd ( ));		// mersenne_twister_engine seeded with rd()
static uniform_int_distribution<>	distrib (0, 100);

VTKMgx3DActor::VTKMgx3DActor ( )
	: vtkLODActor ( ), _entity (0), _representationType ((DisplayRepresentation::type)0), _lodAlea (distrib (gen))
{
}	// VTKMgx3DActor::VTKMgx3DActor


VTKMgx3DActor::VTKMgx3DActor (const VTKMgx3DActor&)
	: vtkLODActor ( ), _entity (0), _representationType ((DisplayRepresentation::type)0), _lodAlea (-1)
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
	if (0 == GetEntity ( ))
		return;

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


bool VTKMgx3DActor::IsLodDisplayable ( ) const
{
	if (0 == GetEntity ( ))
		return false;

	// On veut qu'à cet instant au plus Resources::instance ( )._topo*LodNumberThreshold.getValue ( ) soit affiché.
	// _lodAlea est un nombre aléatoire compris entre 0 et 100 à l'acteur.
	switch (GetEntity ( )->getType ( ))
	{
		case Entity::TopoVertex	:
		{
			const size_t	topoVertexLodNumberThreshold	= Resources::instance ( )._topoVertexLodNumberThreshold.getValue ( );
			const int		threshold						= (int)(100. * topoVertexLodNumberThreshold / (double)Entity::getDisplayedTopoVertexCount ( ));
			if ((_lodAlea >= threshold) && (Entity::getDisplayedTopoVertexCount ( ) > topoVertexLodNumberThreshold))
				return false;
		}
		break;
		case Entity::TopoEdge	:
		case Entity::TopoCoEdge	:
		{
			const size_t	topoEdgeLodNumberThreshold	= Resources::instance ( )._topoEdgeLodNumberThreshold.getValue ( );
			const int		threshold					= (int)(100. * topoEdgeLodNumberThreshold / (double)Entity::getDisplayedTopoEdgeCount ( ));
			if ((_lodAlea >= threshold) && (Entity::getDisplayedTopoEdgeCount ( ) > topoEdgeLodNumberThreshold))
				return false;
		}
		break;
		case Entity::TopoFace	:
		case Entity::TopoCoFace	:
		{
			const size_t	topoFaceLodNumberThreshold	= Resources::instance ( )._topoFaceLodNumberThreshold.getValue ( );
			const int		threshold					= (int)(100. * topoFaceLodNumberThreshold / (double)Entity::getDisplayedTopoFaceCount ( ));
			if ((_lodAlea >= threshold) && (Entity::getDisplayedTopoFaceCount ( ) > topoFaceLodNumberThreshold))
				return false;
		}
		break;
		case Entity::TopoBlock	:
		{
			const size_t	topoBlockLodNumberThreshold	= Resources::instance ( )._topoBlockLodNumberThreshold.getValue ( );
			const int		threshold					= (int)(100. * topoBlockLodNumberThreshold / (double)Entity::getDisplayedTopoBlockCount ( ));
			if ((_lodAlea >= threshold) && (Entity::getDisplayedTopoBlockCount ( ) > topoBlockLodNumberThreshold))
				return false;
		}
		break;
	}	// switch (GetEntity ( )->getType ( ))

	return true;
}	// VTKMgx3DActor::IsLodDisplayable


void VTKMgx3DActor::CreateOwnLODs ( )
{
	// On ne veut pas de LODs pour les entités topologiques :
	if ((0 != GetEntity ( )) && (false == GetEntity ( )->isTopoEntity ( )))
		vtkLODActor::CreateOwnLODs ( );
}	// VTKMgx3DActor::CreateOwnLODs




