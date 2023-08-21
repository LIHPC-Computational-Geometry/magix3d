/*----------------------------------------------------------------------------*/
/*
 * ExportVTKImplementation.cpp
 *
 *  Created on: 14 avr. 2014
 *      Author: legoff
 */
/*----------------------------------------------------------------------------*/
#include <Geom/ExportVTKImplementation.h>
#include <Geom/Volume.h>
#include <Geom/Surface.h>
#include <Geom/Curve.h>
#include <Geom/Vertex.h>
#include <Mesh/MeshItf.h>
#include <Mesh/Surface.h>
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/IGMesh.h>
#include <GMDS/IO/VTKWriter.h>
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
#include <iostream>
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ExportVTKImplementation::
ExportVTKImplementation(Internal::Context& c, const std::string& n)
: m_context(c), m_filename(n)
{}
/*----------------------------------------------------------------------------*/
ExportVTKImplementation::
ExportVTKImplementation(Internal::Context& c,
    		const std::vector<GeomEntity*>& geomEntities,
    		const std::string& n)
: m_context(c), m_filename(n)
{
	MGX_NOT_YET_IMPLEMENTED("Cas VTK avec sélection a implémenter");
}
/*----------------------------------------------------------------------------*/
ExportVTKImplementation::~ExportVTKImplementation()
{}
/*----------------------------------------------------------------------------*/
void ExportVTKImplementation::perform(Internal::InfoCommand* icmd)
{


	std::vector<std::string> volumesNames = m_context.getLocalGeomManager().getVolumes();

	if(1 != volumesNames.size()) {
		std::cerr<<"ExportVTKImplementation::perform can only be called when there is "
				"one and only one volume"<<std::endl;
		throw TkUtil::Exception (TkUtil::UTF8String ("ExportVTKImplementation::perform can only be called when there is "
				"one and only one volume", TkUtil::Charset::UTF_8));
	}

	Geom::Volume* volume = m_context.getLocalGeomManager().getVolume(volumesNames[0]);
//	std::vector<Geom::Surface*> surfaces;
//	volume->get(surfaces);

//	std::vector<std::string> surfacesNames = m_context.getLocalGeomManager().getSurfaces();
//	for(unsigned int iSurf=0; iSurf<surfaces.size(); iSurf++) {
//		m_context.getLocalTopoManager().newUnstructuredTopoOnGeometry(surfaces[0]->getName());
//	}

	//m_context.getLocalTopoManager().newUnstructuredTopoOnGeometry(volumesNames[0]);
//	m_context.getLocalTopoManager().newUnstructuredTopoOnGeometry(volume);
//	m_context.getTopoManager().newUnstructuredTopoOnGeometry(volumesNames[0]);

//	m_context.getLocalMeshManager().newAllFacesMesh();

//	Geom::Volume* volume = m_context.getLocalGeomManager().getVolume(volumesNames[0]);
//	std::vector<Geom::Surface*> surfaces;
//	std::vector<Geom::Curve*> curves;
//	volume->get(surfaces);
//	volume->get(curves);

	unsigned int nbBlocks = m_context.getLocalTopoManager().getNbBlocks();
	if(1 != nbBlocks) {
		std::cerr<<"ExportVTKImplementation::perform can only be called when there is "
				"one and only one block"<<std::endl;
		throw TkUtil::Exception (TkUtil::UTF8String ("ExportVTKImplementation::perform can only be called when there is "
				"one and only one block", TkUtil::Charset::UTF_8));
	}

	std::vector<Topo::Block*> topo_blocs;
	m_context.getLocalTopoManager().getBlocks(topo_blocs);

	std::vector<Topo::CoFace*> topo_cofaces;
	std::vector<Topo::CoEdge*> topo_coedges;
	std::vector<Topo::Vertex*> topo_vertices;
	topo_blocs[0]->getCoFaces(topo_cofaces);
	topo_blocs[0]->getCoEdges(topo_coedges);
	topo_blocs[0]->getVertices(topo_vertices);
//
//	std::vector<Mesh::Surface*> surfaces;
//	std::vector<Mesh::Cloud*> clouds;
//	m_context.getLocalMeshManager().getSurfaces(surfaces);
//	m_context.getLocalMeshManager().getClouds(clouds);

	gmds::IGMesh& mesh = m_context.getLocalMeshManager().getMesh()->getGMDSMesh();

	std::cerr<<"m_context.getLocalMeshManager().getNbNodes() "<<m_context.getLocalMeshManager().getNbNodes()<<std::endl;
	std::cerr<<"mesh.getNbNodes() "<<mesh.getNbNodes()<<std::endl;

	{
		gmds::IGMesh::node_iterator itn = mesh.nodes_begin();

		for(; !itn.isDone(); itn.next()) {

		}

		// surfaces
		for(unsigned int iCoFace=0; iCoFace<topo_cofaces.size(); iCoFace++) {

			Geom::Surface* surf_tmp = dynamic_cast<Geom::Surface*> (topo_cofaces[iCoFace]->getGeomAssociation());
			CHECK_NULL_PTR_ERROR(surf_tmp);
			gmds::IGMesh::surface& surf = mesh.newSurface(surf_tmp->getName());

			std::vector<gmds::TCellID>& elems = topo_cofaces[iCoFace]->faces();
			std::vector<gmds::Face> face_elems;
			face_elems.resize(elems.size());
			for(unsigned int iGMDSFace=0; iGMDSFace<elems.size(); iGMDSFace++) {
			    face_elems[iGMDSFace] = mesh.get<gmds::Face>(elems[iGMDSFace]);
			}
			for(unsigned int iGMDSFace=0; iGMDSFace<elems.size(); iGMDSFace++) {
				surf.add(face_elems[iGMDSFace]);
			}
		}

		// curves
		for(unsigned int iCoEdge=0; iCoEdge<topo_coedges.size(); iCoEdge++) {

		    Geom::Curve* curv_tmp = dynamic_cast<Geom::Curve*> (topo_coedges[iCoEdge]->getGeomAssociation());
		    CHECK_NULL_PTR_ERROR(curv_tmp);
		    gmds::IGMesh::cloud& cl = mesh.newCloud(curv_tmp->getName());

		    std::vector<gmds::TCellID>& elems = topo_coedges[iCoEdge]->nodes();

		    std::vector<gmds::Node> node_elems;
		    node_elems.resize(elems.size());
		    for(unsigned int iGMDSFace=0; iGMDSFace<elems.size(); iGMDSFace++) {
		        node_elems[iGMDSFace] = mesh.get<gmds::Node>(elems[iGMDSFace]);
		    }
		    for(unsigned int iGMDSNode=0; iGMDSNode<elems.size(); iGMDSNode++) {
		        cl.add(node_elems[iGMDSNode]);
		    }
		}

		// vertices
		for(unsigned int iVertex=0; iVertex<topo_vertices.size(); iVertex++) {

			Geom::Vertex* vert_tmp = dynamic_cast<Geom::Vertex*> (topo_vertices[iVertex]->getGeomAssociation());
			CHECK_NULL_PTR_ERROR(vert_tmp);
			gmds::IGMesh::cloud& cl = mesh.newCloud(vert_tmp->getName());

			gmds::Node elem = mesh.get<gmds::Node>(topo_vertices[iVertex]->getNode());

			cl.add(elem);
		}
	}

//	//post process
//    {
//        gmds::IGMesh::cloud& cl1 = mesh.getCloud("Crb0006");
//        gmds::IGMesh::cloud& cl2 = mesh.getCloud("Crb0007");
//        gmds::IGMesh::cloud& cl  = mesh.newCloud("composite6-7");
//
//        std::vector<gmds::TCellID> cl1_nodes = cl1.cellIDs();
//        std::vector<gmds::TCellID> cl2_nodes = cl2.cellIDs();
//        std::set<gmds::TCellID> cl_ids;
//
//        for(unsigned int i=0; i<cl1_nodes.size();i++) {
//            cl_ids.insert(cl1_nodes[i]);
//        }
//        for(unsigned int i=0; i<cl2_nodes.size();i++) {
//            cl_ids.insert(cl2_nodes[i]);
//        }
//
//        std::set<gmds::TCellID>::iterator it_set;
//        for(it_set=cl_ids.begin();it_set!=cl_ids.end();it_set++)
//        {
//            gmds::TCellID node_id = *it_set;
//            cl.add(mesh.get<gmds::Node>(node_id));
//        }
//
//    }
//    {
//        gmds::IGMesh::cloud& cl1 = mesh.getCloud("Crb0009");
//        gmds::IGMesh::cloud& cl2 = mesh.getCloud("Crb0010");
//        gmds::IGMesh::cloud& cl  = mesh.newCloud("composite9-10");
//
//        std::vector<gmds::TCellID> cl1_nodes = cl1.cellIDs();
//        std::vector<gmds::TCellID> cl2_nodes = cl2.cellIDs();
//        std::set<gmds::TCellID> cl_ids;
//
//        for(unsigned int i=0; i<cl1_nodes.size();i++) {
//            cl_ids.insert(cl1_nodes[i]);
//        }
//        for(unsigned int i=0; i<cl2_nodes.size();i++) {
//            cl_ids.insert(cl2_nodes[i]);
//        }
//
//        std::set<gmds::TCellID>::iterator it_set;
//        for(it_set=cl_ids.begin();it_set!=cl_ids.end();it_set++)
//        {
//            gmds::TCellID node_id = *it_set;
//            cl.add(mesh.get<gmds::Node>(node_id));
//        }
//
//    }
	gmds::VTKWriter<gmds::IGMesh> writer(mesh);
	writer.write(m_filename,gmds::N|gmds::F);

    //std::cerr<<"ExportVTKImplementation::perform --->PAS IMPLEMENTE"<<std::endl;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
