/*----------------------------------------------------------------------------*/
/*
 * \file MesquiteMeshImplAdapter.cpp
 *
 *  \author Nicolas Le Goff
 *
 *  \date 21 mar. 2014
 */
/*----------------------------------------------------------------------------*/
#include "Smoothing/MesquiteMeshImplAdapter.h"
/*----------------------------------------------------------------------------*/
//#include "GMDSMesh/Node.h"
//#include "GMDSMesh/Face.h"
///*----------------------------------------------------------------------------*/
//#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
// Mesquite
#include <MeshImplData.hpp>
#include <MsqError.hpp>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
MesquiteMeshImplAdapter::
MesquiteMeshImplAdapter(
		Mgx3D::Mesh::MeshManager& AMeshManager,
		Mgx3D::Geom::GeomManager& AGeomManager)
: m_meshManager(AMeshManager), m_geomManager(AGeomManager)
{
	Mesquite::MsqError err;

	MeshItf* mesh = m_meshManager.getMesh();
	gmds::IGMesh& gmdsMesh = mesh->getGMDSMesh();

	// Tous les noeuds sont pris
	unsigned int vertexCount = gmdsMesh.getNbNodes();

	// Remplissage des structures pour Mesquite
	myMesh->allocate_vertices (vertexCount, err);
	MSQ_CHKERR (err);

	// table de correspondance entre noeuds Gmds et indices pour Mesquite
	std::map<gmds::TCellID, uint> num_insurf;

	gmds::IGMesh::node_iterator itn  = gmdsMesh.nodes_begin();

	unsigned int iVertexCount = 0;
	for(;!itn.isDone();itn.next()) {
		gmds::Node current_node = itn.value();

		num_insurf[current_node.getID()] = iVertexCount;

		myMesh->reset_vertex (
				iVertexCount,
				Mesquite::Vector3D (
						current_node.X(),
						current_node.Y(),
						current_node.Z()),
						false,
						err);
		MSQ_CHKERR (err);

		iVertexCount++;
	}

	{
		std::vector<size_t> verts;
		myMesh->all_vertices(verts,err);
		MSQ_CHKERR (err);

		std::vector<MESQUITE_NS::Mesh::VertexHandle> vertices;

		get_all_vertices(vertices,err);
		MSQ_CHKERR (err);

		unsigned int iVertexCount = 0;
		gmds::IGMesh::node_iterator itn  = gmdsMesh.nodes_begin();

		for(;!itn.isDone();itn.next()) {
			gmds::Node current_node = itn.value();

			m_mesquite2GMDSNodes[vertices[iVertexCount]] = current_node;
			iVertexCount++;
		}
	}

	unsigned int elementCount = gmdsMesh.getNbRegions();
	myMesh->allocate_elements (elementCount, err);
	MSQ_CHKERR (err);

	std::vector < size_t > vertices;
	Mesquite::EntityTopology elem_type;

	gmds::IGMesh::region_iterator itr  = gmdsMesh.regions_begin();

	unsigned int iElementCount = 0;
	for(;!itr.isDone();itr.next()) {
		gmds::Region current_region = itr.value();

		std::vector<gmds::TCellID> nodes = current_region.getIDs<gmds::Node>();

		switch (current_region.getType()) {
		case gmds::GMDS_HEX:
			elem_type = MESQUITE_NS::HEXAHEDRON;
			vertices.resize (nodes.size());
			for (size_t iNode = 0; iNode < nodes.size(); iNode++)
				vertices[iNode] = num_insurf[nodes[iNode]];
			break;
		case gmds::GMDS_TETRA:
			elem_type = MESQUITE_NS::TETRAHEDRON;
			vertices.resize (nodes.size());
			for (size_t iNode = 0; iNode < nodes.size(); iNode++)
				vertices[iNode] = num_insurf[nodes[iNode]];
			break;
		case gmds::GMDS_PYRAMID:
			elem_type = MESQUITE_NS::PYRAMID;
			vertices.resize (nodes.size());
			for (size_t iNode = 0; iNode < nodes.size(); iNode++)
				vertices[iNode] = num_insurf[nodes[iNode]];
			break;
		case gmds::GMDS_PRISM3:
			elem_type = MESQUITE_NS::PRISM;
			vertices.resize (nodes.size());
			for (size_t iNode = 0; iNode < nodes.size(); iNode++)
				vertices[iNode] = num_insurf[nodes[iNode]];
			break;
		default:
			throw TkUtil::Exception (TkUtil::UTF8String ("MesquiteMeshImplAdapter::MesquiteMeshImplAdapter element type not recognized.", TkUtil::Charset::UTF_8));
			break;
		}

		myMesh->reset_element (iElementCount, vertices, elem_type, err);
		MSQ_CHKERR (err);
		iElementCount++;
	}

	MSQ_CHKERR (err);
#ifdef _DEBUG2
	std::cout << "iElementCount = "<<iElementCount<<std::endl;
#endif

}
/*----------------------------------------------------------------------------*/
MesquiteMeshImplAdapter::~MesquiteMeshImplAdapter()
{

}
/*----------------------------------------------------------------------------*/
std::map<MESQUITE_NS::Mesh::VertexHandle,gmds::Node>*
MesquiteMeshImplAdapter::getMesquite2GMDSNodes()
{
	return &m_mesquite2GMDSNodes;
}
/*----------------------------------------------------------------------------*/
std::map<MESQUITE_NS::Mesh::ElementHandle,gmds::Face>*
MesquiteMeshImplAdapter::getMesquite2GMDSCells2D()
{
    return &m_mesquite2GMDSCells2D;
}
/*----------------------------------------------------------------------------*/
std::map<MESQUITE_NS::Mesh::ElementHandle,gmds::Region>*
MesquiteMeshImplAdapter::getMesquite2GMDSCells3D()
{
    return &m_mesquite2GMDSCells3D;
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
