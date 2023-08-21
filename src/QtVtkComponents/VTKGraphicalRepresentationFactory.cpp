/**
 * \file		VTKGraphicalRepresentationFactory.cpp
 * \author		Charles PIGNEROL
 * \date		02/07/2012
 */

#include "Internal/ContextIfc.h"

#include "QtVtkComponents/VTKGraphicalRepresentationFactory.h"
#include "QtVtkComponents/VTKGMDSEntityRepresentation.h"
#include "QtVtkComponents/VTKMgx3DGeomEntityRepresentation.h"
#include "QtVtkComponents/VTKMgx3DMeshEntityRepresentation.h"
#include "QtVtkComponents/VTKMgx3DTopoEntityRepresentation.h"
#include "QtVtkComponents/VTKMgx3DSysCoordEntityRepresentation.h"
#include "QtVtkComponents/VTKMgx3DStructuredMeshEntityRepresentation.h"
#include "Utils/GraphicalEntityRepresentation.h"
#include "Utils/Common.h"
#include "Mesh/Volume.h"
#include "Mesh/SubSurface.h"
#include "Mesh/SubVolume.h"
#include "Mesh/SubVolume.h"
#include "SysCoord/SysCoord.h"
#include "Structured/StructuredMeshEntity.h"

#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Mesh;
using namespace Mgx3D::Structured;
using namespace Mgx3D::CoordinateSystem;


namespace Mgx3D
{

namespace QtVtkComponents
{


VTKGraphicalRepresentationFactory::VTKGraphicalRepresentationFactory ( )
	: GraphicalRepresentationFactoryIfc ( )
{
}   // VTKGraphicalRepresentationFactory::VTKGraphicalRepresentationFactory


VTKGraphicalRepresentationFactory::VTKGraphicalRepresentationFactory (
                                       const VTKGraphicalRepresentationFactory&)
	: GraphicalRepresentationFactoryIfc ( )
{
	MGX_FORBIDDEN ("VTKGraphicalRepresentationFactory copy constructor is not allowed.")
}   // VTKGraphicalRepresentationFactory::VTKGraphicalRepresentationFactory


VTKGraphicalRepresentationFactory& VTKGraphicalRepresentationFactory::operator = (
                                      const VTKGraphicalRepresentationFactory&)
{
	MGX_FORBIDDEN ("VTKGraphicalRepresentationFactory assignment operator is not allowed.")
    return *this;
}   // VTKGraphicalRepresentationFactory::operator =


VTKGraphicalRepresentationFactory::~VTKGraphicalRepresentationFactory ( )
{
}	// VTKGraphicalRepresentationFactory::~VTKGraphicalRepresentationFactory


DisplayProperties::GraphicalRepresentation* VTKGraphicalRepresentationFactory::create (Entity& entity)
{
	if (false == entity.getDisplayProperties ( ).isDisplayable ( ))
		return 0;

	GeomEntity*			geomEntity		= dynamic_cast<GeomEntity*>(&entity);
	TopoEntity*			topoEntity		= dynamic_cast<TopoEntity*>(&entity);
	MeshEntity*			meshEntity		= dynamic_cast<MeshEntity*>(&entity);
	StructuredMeshEntity*		structuredMeshEntity	= dynamic_cast<StructuredMeshEntity*>(&entity);
	CoordinateSystem::SysCoord*	repEntity		= dynamic_cast<CoordinateSystem::SysCoord*>(&entity);

	DisplayProperties::GraphicalRepresentation*	representation	= 0;
	if (0 != geomEntity)
		representation	= new VTKMgx3DGeomEntityRepresentation (*geomEntity);
	else if (0 != topoEntity)
		representation	= new VTKMgx3DTopoEntityRepresentation (*topoEntity);
	else if (0 != repEntity)
		representation	= new VTKMgx3DSysCoordEntityRepresentation (*repEntity);
	else if (0 != meshEntity)
	{
	        // cas particulier pour les sous-volumes, il n'y a pas de groupe
	        // associé dans GMDS. De même en sortie de Qualif.
	        bool		useGMDSOptimization	= true;
	        SubVolume*	sv			= dynamic_cast<SubVolume*>(&entity);
	        SubSurface*	ss			= dynamic_cast<SubSurface*>(&entity);
		bool		hasBlocks		= true;
		if (3 == meshEntity->getDim ( ))
		{
			Mesh::Volume*	volume	= dynamic_cast<Mesh::Volume*>(meshEntity);
			if (0 != volume)
			{
				vector<Topo::Block*>	blocks;
				volume->getBlocks (blocks);
				//std::cout<<"blocks.size() "<<blocks.size()<<std::endl;

				hasBlocks = !blocks.empty();
				//std::cout<<"hasBlocks "<<hasBlocks<<std::endl;

			}	// if (0 != volume)
		}	// if (3 == meshEntity->getDim ( ))
		if ((0 != sv) || (0 != ss) || (false == hasBlocks))
			useGMDSOptimization	= false;

		// Si on affiche une valeur aux noeuds/mailles alors on prend la
		// représentation non optimisée :
		// Pour NICO : décommenter les 2 lignes ci-dessous si nécessaire.
//		if (false != entity.getDisplayProperties ( ).getValueName ( ).empty ( ))
//			useGMDSOptimization	= false;

		//std::cout<<"entity "<<entity.getName()<<", useGMDSOptimization "<<useGMDSOptimization<<std::endl;
	        if (false == useGMDSOptimization)
	            representation = new VTKMgx3DMeshEntityRepresentation (*meshEntity);
	        else
	            representation = new VTKGMDSEntityRepresentation (*meshEntity);
	}	// else if (0 != meshEntity)
	else if (0 != structuredMeshEntity)
		representation	= new VTKMgx3DStructuredMeshEntityRepresentation (*structuredMeshEntity);
	
	CHECK_NULL_PTR_ERROR (representation)
	representation->setRepresentationMask (GraphicalEntityRepresentation::getDefaultRepresentationMask (entity.getType ( )));

	return representation;
}	// VTKGraphicalRepresentationFactory::create


}	// namespace Mgx3D

}	// namespace QtVtkComponents


