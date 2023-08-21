/*----------------------------------------------------------------------------*/
/*
 * \file MesquiteDomainImplAdapter.cpp
 *
 *  \author Nicolas Le Goff
 *
 *  \date 21 mar. 2014
 */
/*----------------------------------------------------------------------------*/
#include "Smoothing/MesquiteDomainImplAdapter.h"

#include "Geom/GeomEntity.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"

#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/Edge.h"
#include "Topo/Vertex.h"

#include "Utils/Point.h"
#include "Utils/Vector.h"

#include <Vector3D.hpp>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
MesquiteDomainImplAdapter::
MesquiteDomainImplAdapter(
		Mgx3D::Mesh::MeshManager& AMeshManager,
		Mgx3D::Topo::TopoManager& ATopoManager,
		std::map<MESQUITE_NS::Mesh::VertexHandle, gmds::Node>& AMesquite2GMDSNodes,
        std::map<MESQUITE_NS::Mesh::ElementHandle,gmds::Face>& AMesquite2GMDSCells2D,
        std::map<MESQUITE_NS::Mesh::ElementHandle,gmds::Region>& AMesquite2GMDSCells3D)
: m_meshManager(AMeshManager), m_topoManager(ATopoManager),
  m_mesquite2GMDSNodes(AMesquite2GMDSNodes),
  m_mesquite2GMDSCells2D(AMesquite2GMDSCells2D), m_mesquite2GMDSCells3D(AMesquite2GMDSCells3D)
{
	std::vector<Topo::Block*  > blocks;
	std::vector<Topo::CoFace* > coFaces;
	std::vector<Topo::CoEdge* > coEdges;
	std::vector<Topo::Vertex* > vertices;

	m_topoManager.getBlocks(blocks);
	m_topoManager.getCoFaces(coFaces);
	m_topoManager.getCoEdges(coEdges);
	m_topoManager.getVertices(vertices);

//	for(unsigned int iBlock=0; iBlock<blocks.size(); iBlock++) {
//		std::vector<gmds::Region*>& regions = blocks[iBlock]->regions();
//		Geom::GeomEntity* vol_tmp = blocks[iBlock]->getGeomAssociation();
//		Geom::Volume* vol_tmp2 = dynamic_cast<Geom::Volume*> (vol_tmp);
//	}

	gmds::IGMesh & gmds_mesh = m_meshManager.getMesh()->getGMDSMesh();
	for(unsigned int iCoFace=0; iCoFace<coFaces.size(); iCoFace++) {

		if(coFaces[iCoFace]->isMeshed()) {
			std::vector<gmds::TCellID>& faces = coFaces[iCoFace]->faces();
			Geom::GeomEntity* surf = coFaces[iCoFace]->getGeomAssociation();

			// a CoFace is not necessarily associated to a geometry
			if(NULL != surf) {
				for(unsigned int iFace=0; iFace<faces.size(); iFace++) {

					std::vector<gmds::TCellID> nodes = gmds_mesh.get<gmds::Face>(faces[iFace]).getIDs<gmds::Node>();

					for(unsigned int iNode=0; iNode<nodes.size(); iNode++) {
						m_nodes2GeomEntity[nodes[iNode]] = surf;
						m_nodesIsOnSurface[nodes[iNode]] = true;
					}
				}
			}
		}
	}

	for(unsigned int iCoEdge=0; iCoEdge<coEdges.size(); iCoEdge++) {

		if(coEdges[iCoEdge]->isMeshed()) {
			std::vector<gmds::TCellID>& nodes = coEdges[iCoEdge]->nodes();

			Geom::GeomEntity* curv = coEdges[iCoEdge]->getGeomAssociation();

			// a CoEdge is not necessarily associated to a geometry
			if(NULL != curv) {
				for(unsigned int iNode=0; iNode<nodes.size(); iNode++) {
					m_nodes2GeomEntity[nodes[iNode]] = curv;
					m_nodesIsOnCurve[nodes[iNode]] = true;
				}
			}
		}
	}

	for(unsigned int iVertex=0; iVertex<vertices.size(); iVertex++) {

		if(vertices[iVertex]->isMeshed()) {
			gmds::Node node = gmds_mesh.get<gmds::Node>(vertices[iVertex]->getNode());
			Geom::GeomEntity* vertex = vertices[iVertex]->getGeomAssociation();

			if(NULL != vertex) {

				m_nodes2GeomEntity[node.getID()] = vertex;
				m_nodesIsOnVertex[node.getID()] = true;
			}
		}
	}

//	ABlock->getCoFaces(block_faces);
//	ABlock->getCoEdges(block_edges);
//	ABlock->getVertices(block_vertices);
}
/*----------------------------------------------------------------------------*/
MesquiteDomainImplAdapter::~MesquiteDomainImplAdapter()
{

}
/*----------------------------------------------------------------------------*/
void MesquiteDomainImplAdapter::snap_to (
		Mesquite::Mesh::EntityHandle AEntityHandle,
		Mesquite::Vector3D & coordinate) const
{
	gmds::TCellID node = m_mesquite2GMDSNodes[AEntityHandle].getID();

	if((m_nodesIsOnVertex.find(node)  != m_nodesIsOnVertex.end())
	|| (m_nodesIsOnCurve.find(node)   != m_nodesIsOnCurve.end())
	|| (m_nodesIsOnSurface.find(node) != m_nodesIsOnSurface.end()))
	{
		Utils::Math::Point pt (coordinate.x(),coordinate.y(),coordinate.z());
		Mgx3D::Geom::GeomEntity* geomEntity = m_nodes2GeomEntity.find(node)->second;
		geomEntity->project(pt);
		coordinate.set(pt.getX(), pt.getY(), pt.getZ());
		return;

	} else {
		// in this case the node is free so coordinate is not modified
		return;
	}

}
/*----------------------------------------------------------------------------*/
void MesquiteDomainImplAdapter::vertex_normal_at(
		Mesquite::Mesh::EntityHandle AEntityHandle,
		Mesquite::Vector3D &coordinate) const
{
	gmds::TCellID node = m_mesquite2GMDSNodes[AEntityHandle].getID();

	if(m_nodesIsOnSurface.find(node) == m_nodesIsOnSurface.end()) {
		// in this case the node is not on a surface so the normal cannot be computed
		coordinate.set(0.,0.,0.);
	} else {
		Utils::Math::Point pt (coordinate.x(),coordinate.y(),coordinate.z());
		Mgx3D::Geom::GeomEntity* geomEntity = m_nodes2GeomEntity.find(node)->second;
		Mgx3D::Geom::Surface* surface = dynamic_cast<Mgx3D::Geom::Surface*> (geomEntity);
		CHECK_NULL_PTR_ERROR(surface);

		Utils::Math::Vector vec;
		surface->normal(pt,vec);

		coordinate.set(vec.getX(), vec.getY(), vec.getZ());
	}

}
/*----------------------------------------------------------------------------*/
void MesquiteDomainImplAdapter::element_normal_at(
		Mesquite::Mesh::ElementHandle entity_handle,
		Mesquite::Vector3D &coordinate) const
{
	MGX_NOT_YET_IMPLEMENTED("MesquiteDomainImplAdapter::element_normal_at non prévue")
}
/*----------------------------------------------------------------------------*/
void MesquiteDomainImplAdapter::vertex_normal_at(
		const MESQUITE_NS::Mesh::EntityHandle * m,
		Mesquite::Vector3D coords[],
		unsigned count,
		Mesquite::MsqError& ) const
{
  for (unsigned i = 0; i < count; ++i) {
	  vertex_normal_at(m[i], coords[i]);
  }

}
/*----------------------------------------------------------------------------*/
void MesquiteDomainImplAdapter::closest_point( MESQUITE_NS::Mesh::EntityHandle mesh_entity,
				       const MESQUITE_NS::Vector3D& position,
				       MESQUITE_NS::Vector3D& closest,
				       MESQUITE_NS::Vector3D& normal,
				       MESQUITE_NS::MsqError& ) const
{
  closest = position;
  normal = position;

  snap_to(mesh_entity, closest);
  vertex_normal_at(mesh_entity, normal);
}
/*----------------------------------------------------------------------------*/
void MesquiteDomainImplAdapter::domain_DoF( const MESQUITE_NS::Mesh::EntityHandle* handle_array,
		unsigned short* dof_array,
		size_t num_vertices,
		Mesquite::MsqError& err ) const
{
	for (unsigned i = 0; i < num_vertices; ++i) {
		gmds::TCellID node = m_mesquite2GMDSNodes[handle_array[i]].getID();

		if(m_nodesIsOnVertex.find(node)  != m_nodesIsOnVertex.end()) {
			dof_array[i] = 0;
			continue;
		} else if(m_nodesIsOnCurve.find(node)  != m_nodesIsOnCurve.end()) {
			dof_array[i] = 1;
			continue;
		} else if(m_nodesIsOnSurface.find(node)  != m_nodesIsOnSurface.end()) {
			dof_array[i] = 2;
			continue;
		}

		dof_array[i] = 3;
	}

}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
