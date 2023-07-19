/*----------------------------------------------------------------------------*/
/*
 * ExportBlocksImplementation.cpp
 *
 *  Created on: 27/06/23
 *      Author: calderan
 */
/*----------------------------------------------------------------------------*/
#include "Mesh/ExportBlocksImplementation.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Mesh/MeshItf.h"
#include "Mesh/Surface.h"
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/IGMesh.h>
#include <GMDS/IO/VTKWriter.h>
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
#include <iostream>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
    namespace Mesh {
/*----------------------------------------------------------------------------*/
        ExportBlocksImplementation::
        ExportBlocksImplementation(Internal::Context& c, const std::string& n)
                : m_context(c), m_filename(n)
        {}

/*----------------------------------------------------------------------------*/
        ExportBlocksImplementation::~ExportBlocksImplementation()
        {}
/*----------------------------------------------------------------------------*/
        void ExportBlocksImplementation::perform(Internal::InfoCommand* icmd)
        {

            std::vector<Topo::Vertex*> topo_vertices;
            std::vector<Topo::Block*> topo_blocs;
            std::vector<Topo::CoFace*> topo_faces;

            std::map<gmds::TCellID,unsigned long> n2v;

            gmds::IGMesh mesh(gmds::MeshModel(gmds::DIM3 | gmds::N | gmds::R | gmds::R2N | gmds::N2R));

            gmds::IGMesh localMesh = m_context.getLocalMeshManager().getMesh()->getGMDSMesh();


            for(int iVertex = 0; iVertex < localMesh.getNbNodes(); iVertex++){
                gmds::Node n_local = localMesh.get<gmds::Node>(iVertex);
                gmds::Node n = mesh.newNode(n_local.X(),n_local.Y(),n_local.Z());
            }

            m_context.getLocalTopoManager().getBlocks(topo_blocs);
            m_context.getLocalTopoManager().getCoFaces(topo_faces);

            for(int iBlock = 0; iBlock < topo_blocs.size(); iBlock++){
                std::vector<unsigned long> ids;
                ids.push_back(topo_blocs[iBlock]->getVertex(false,false,false)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(true,false,false)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(true,true,false)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(false,true,false)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(false,false,true)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(true,false,true)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(true,true,true)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(false,true,true)->getNode());

                mesh.newHex(ids[0],ids[1],ids[2],ids[3],
                            ids[4],ids[5],ids[6],ids[7]);

            }

            gmds::Variable<int>* discrI = mesh.newVariable<int>(gmds::GMDS_REGION,"discrI");
            gmds::Variable<int>* discrJ = mesh.newVariable<int>(gmds::GMDS_REGION,"discrJ");
            gmds::Variable<int>* discrK = mesh.newVariable<int>(gmds::GMDS_REGION,"discrK");

            for(int iBlock = 0; iBlock < topo_blocs.size(); iBlock++){

                gmds::Variable<int>* internalNodes = mesh.newVariable<int>(gmds::GMDS_NODE,"block"+std::to_string(iBlock));

                uint b_discrI,b_discrJ,b_discrK;
                topo_blocs[iBlock]->getNbMeshingEdges(b_discrI,b_discrJ,b_discrK);
                discrI->set(iBlock, b_discrI+1);
                discrJ->set(iBlock, b_discrJ+1);
                discrK->set(iBlock, b_discrK+1);

                std::vector<gmds::TCellID> b_nodes = topo_blocs[iBlock]->nodes();
                for(int iNode = 0; iNode < b_nodes.size(); iNode++){
                    //std::cout<<mesh.get<gmds::Node>(b_nodes[iNode])<<std::endl;
                    internalNodes->set(b_nodes[iNode], iNode+1);
                }
            }

            gmds::Variable<int>* farfield = mesh.newVariable<int>(gmds::GMDS_NODE,"Farfield");
            gmds::Variable<int>* paroi = mesh.newVariable<int>(gmds::GMDS_NODE,"Paroi");


            gmds::Variable<int>* Axi = mesh.newVariable<int>(gmds::GMDS_NODE,"Axisymetrie");


            for(auto f : topo_faces){
                std::vector<string> groupsName;
                f->getGroupsName(groupsName);
                std::vector<gmds::TCellID> f_nodes = f->nodes();
                if(std::find(groupsName.begin(),groupsName.end(), "Farfield") != groupsName.end()){
                    for(auto n : f_nodes){
                        farfield->set(n, 1);
                    }
                }else if(std::find(groupsName.begin(),groupsName.end(), "Paroi") != groupsName.end()){
                    for(auto n : f_nodes){
                        paroi->set(n, 1);
                    }
                }else if(std::find(groupsName.begin(),groupsName.end(), "Axisymetrie") != groupsName.end()){
                    for(auto n : f_nodes){
                        Axi->set(n, 1);
                    }
                }
            }




            gmds::VTKWriter<gmds::IGMesh> writer(mesh);
            writer.write(m_filename,gmds::N|gmds::R);

        }
/*----------------------------------------------------------------------------*/
    } // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
