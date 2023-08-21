/*----------------------------------------------------------------------------*/
/*
 * \file MesquiteMeshAdapter.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25 févr. 2014
 */
/*----------------------------------------------------------------------------*/
#include "Smoothing/MesquiteMeshAdapter.h"
/*----------------------------------------------------------------------------*/
#include "GMDS/IG/Node.h"
#include "GMDS/IG/Face.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
// Mesquite
#include <MeshImplData.hpp>
#include <MsqError.hpp>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
//#define _DEBUG2
/*----------------------------------------------------------------------------*/
MesquiteMeshAdapter::
MesquiteMeshAdapter(std::vector<gmds::Face>& gmdsPolygones,
			std::vector<gmds::Node >& gmdsNodes,
			std::map<gmds::TCellID, uint>& filtre_nodes,
			std::map<gmds::TCellID, bool>& isPolyInverted,
			uint maskFixed)
: m_gmdsNodes(gmdsNodes)
{
#ifdef _DEBUG2
	std::cout << "MesquiteMeshAdapter::MesquiteMeshAdapter\n";
#endif
	Mesquite::MsqError err;

	// Tous les noeuds sont pris
	int vertexCount = gmdsNodes.size();
#ifdef _DEBUG2
	std::cout << "vertexCount = "<<vertexCount<<std::endl;
#endif

	// Remplissage des structures pour Mesquite
	myMesh->allocate_vertices (vertexCount, err);
	MSQ_CHKERR (err);

	// table de correspondance entre noeuds Gmds et indices pour Mesquite
	std::map<gmds::TCellID, uint> num_insurf;

	for (uint i=0; i<vertexCount; i++){
		gmds::Node nd = m_gmdsNodes[i];
#ifdef _DEBUG2
		std::cout << " i "<<i<<", "<<nd<<" fixé ? "<<(filtre_nodes[nd.getID()] == maskFixed?"vrai":"faux")<<std::endl;
#endif
		num_insurf[nd.getID()] = i;
		myMesh->reset_vertex (i,
				Mesquite::Vector3D (nd.X(), nd.Y(), nd.Z()),
				filtre_nodes[nd.getID()] == maskFixed,
				err);
	}


	myMesh->allocate_elements (gmdsPolygones.size(), err);
	int elementCount = 0;
	std::vector < size_t > vertices;
	Mesquite::EntityTopology elem_type;
	for (size_t i_poly = 0; i_poly < gmdsPolygones.size(); ++i_poly){
		gmds::Face cell = gmdsPolygones[i_poly];
		std::vector<gmds::TCellID> l_nds = cell.getAllIDs<gmds::Node>();
		switch (l_nds.size()) {
		case 4:
			elem_type = MESQUITE_NS::QUADRILATERAL;
			vertices.resize (4);
			if (isPolyInverted[cell.getID()])
				for (size_t j = 0; j < 4; j++)
					vertices[j] = num_insurf[l_nds[3-j]];
			else
				for (size_t j = 0; j < 4; j++)
					vertices[j] = num_insurf[l_nds[j]];
			break;
		case 3:
			elem_type = MESQUITE_NS::TRIANGLE;
			vertices.resize (3);
			if (isPolyInverted[cell.getID()])
				for (size_t j = 0; j < 3; j++)
					vertices[j] = num_insurf[l_nds[2-j]];
			else
				for (size_t j = 0; j < 3; j++)
					vertices[j] = num_insurf[l_nds[j]];
			break;
		default:
			// Ben là je ne sais pas faire ! Un polygone quoi
			throw TkUtil::Exception (TkUtil::UTF8String ("MesquiteMeshAdapter avec type d'élément non prévu", TkUtil::Charset::UTF_8));
			break;
		}
#ifdef _DEBUG2
		std::cout << " reset_element ("<<elementCount;
		for (uint i=0; i<vertices.size(); i++)
			std::cout <<", "<<vertices[i];
		std::cout <<")"<<std::endl;
#endif
		myMesh->reset_element (elementCount, vertices, elem_type, err);
		elementCount++;
	}
	MSQ_CHKERR (err);
#ifdef _DEBUG2
	std::cout << "elementCount = "<<elementCount<<std::endl;
#endif

}
/*----------------------------------------------------------------------------*/
MesquiteMeshAdapter::~MesquiteMeshAdapter()
{
#ifdef _DEBUG2
    std::cout << "MesquiteMeshAdapter::~MesquiteMeshAdapter\n";
#endif
    MESQUITE_NS::MsqError err;

    for (size_t i = 0; i < m_gmdsNodes.size(); ++i){
    	gmds::Node nd = m_gmdsNodes[i];

	    MESQUITE_NS::Vector3D coord = myMesh->get_vertex_coords(i, err);
	    nd.setX(coord[0]);
	    nd.setY(coord[1]);
	    nd.setZ(coord[2]);
#ifdef _DEBUG2
	    std::cout << " i "<<i<<", "<<nd<<std::endl;
#endif
    }
    MSQ_CHKERR (err);
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
