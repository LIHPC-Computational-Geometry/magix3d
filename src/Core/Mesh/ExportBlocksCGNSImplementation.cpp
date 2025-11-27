/*----------------------------------------------------------------------------*/
/*
 * ExportBlocksCGNSImplementation.cpp
 *
 *  Created on: 27/06/23
 *      Author: calderan
 */
/*----------------------------------------------------------------------------*/
#include "Mesh/ExportBlocksCGNSImplementation.h"
#include "Mesh/MeshItf.h"
#include "Mesh/Surface.h"
#include "Group/GroupManager.h"
/*----------------------------------------------------------------------------*/
#include <gmds/ig/Mesh.h>
#include <gmds/io/IGMeshIOService.h>
#include <gmds/aero/CGNSWriterND.h>
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
#include <iostream>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
    namespace Mesh {
/*----------------------------------------------------------------------------*/
        ExportBlocksCGNSImplementation::
        ExportBlocksCGNSImplementation(Internal::Context& c, const std::string& filename, int dim)
                : m_context(c), m_filename(filename), m_dim(dim)
        {}

/*----------------------------------------------------------------------------*/
        ExportBlocksCGNSImplementation::~ExportBlocksCGNSImplementation()
        {}
/*----------------------------------------------------------------------------*/
        void ExportBlocksCGNSImplementation::perform(Internal::InfoCommand* icmd)
        {
            if (m_dim == 2) perform2D();
            else if (m_dim == 3) perform3D();

        }
/*----------------------------------------------------------------------------*/
        void ExportBlocksCGNSImplementation::perform3D()
        {
            gmds::Mesh mesh(gmds::MeshModel(gmds::DIM3 | gmds::N | gmds::F | gmds::R |
                                            gmds::R2N | gmds::N2R | gmds::R2F |  gmds::F2R | gmds::F2N | gmds::N2F));

            gmds::Mesh* localMesh = &m_context.getMeshManager().getMesh()->getGMDSMesh();


            for(int iVertex = 0; iVertex < localMesh->getNbNodes(); iVertex++){
                gmds::Node n_local = localMesh->get<gmds::Node>(iVertex);
                gmds::Node n = mesh.newNode(n_local.X(),n_local.Y(),n_local.Z());
            }

            std::vector<Topo::CoFace*> topo_faces = m_context.getTopoManager().getCoFacesObj();
            std::map<std::string, int> topoF_2_gmdsF;
            for (auto cf : topo_faces) {
                int n0 = cf->getVertices()[0]->getNode();
                int n1 = cf->getVertices()[1]->getNode();
                int n2 = cf->getVertices()[2]->getNode();
                int n3 = cf->getVertices()[3]->getNode();

                gmds::Face f = mesh.newQuad(n0,n1,n2,n3);

                topoF_2_gmdsF[cf->getName()] = f.id();
            }

            std::map<std::string, int> topoB_2_gmdsB;

            std::vector<Topo::Block*> topo_blocs = m_context.getTopoManager().getBlocksObj();
            for(int iBlock = 0; iBlock < topo_blocs.size(); iBlock++){
                Topo::Block* block = topo_blocs[iBlock];
                std::vector<unsigned long> ids;
                std::vector<Topo::Vertex*> vertices;
                vertices.resize(8);

                vertices[0] = block->getVertex(false,false,false);
                vertices[1] = block->getVertex(true,false,false);
                vertices[2] = block->getVertex(true,true,false);
                vertices[3] = block->getVertex(false,true,false);
                vertices[4] = block->getVertex(false,false,true);
                vertices[5] = block->getVertex(true,false,true);
                vertices[6] = block->getVertex(true,true,true);
                vertices[7] = block->getVertex(false,true,true);


                ids.push_back(topo_blocs[iBlock]->getVertex(false,false,false)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(true,false,false)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(true,true,false)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(false,true,false)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(false,false,true)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(true,false,true)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(true,true,true)->getNode());
                ids.push_back(topo_blocs[iBlock]->getVertex(false,true,true)->getNode());


                gmds::Region r = mesh.newHex(ids[0],ids[1],ids[2],ids[3],
                                             ids[4],ids[5],ids[6],ids[7]);

                if(topo_blocs[iBlock]->getCoFaces().size() != 6 ){
                    throw TkUtil::Exception(TkUtil::UTF8String ("Le bloc "+topo_blocs[iBlock]->getName()+" n'est pas conforme.", TkUtil::Charset::UTF_8));
                }

                int iface0 = topoF_2_gmdsF[block->getFace(vertices[0],vertices[1],vertices[2],vertices[3])->getCoFaces()[0]->getName()];
                int iface1 = topoF_2_gmdsF[block->getFace(vertices[0],vertices[1],vertices[5],vertices[4])->getCoFaces()[0]->getName()];
                int iface2 = topoF_2_gmdsF[block->getFace(vertices[1],vertices[2],vertices[6],vertices[5])->getCoFaces()[0]->getName()];
                int iface3 = topoF_2_gmdsF[block->getFace(vertices[2],vertices[3],vertices[7],vertices[6])->getCoFaces()[0]->getName()];
                int iface4 = topoF_2_gmdsF[block->getFace(vertices[3],vertices[0],vertices[4],vertices[7])->getCoFaces()[0]->getName()];
                int iface5 = topoF_2_gmdsF[block->getFace(vertices[4],vertices[5],vertices[6],vertices[7])->getCoFaces()[0]->getName()];

                r.add<gmds::Face>(iface0);
                r.add<gmds::Face>(iface1);
                r.add<gmds::Face>(iface2);
                r.add<gmds::Face>(iface3);
                r.add<gmds::Face>(iface4);
                r.add<gmds::Face>(iface5);

                gmds::Face f0 = mesh.get<gmds::Face>(iface0);
                f0.add<gmds::Region>(iBlock);
                gmds::Face f1 = mesh.get<gmds::Face>(iface1);
                f1.add<gmds::Region>(iBlock);
                gmds::Face f2 = mesh.get<gmds::Face>(iface2);
                f2.add<gmds::Region>(iBlock);
                gmds::Face f3 = mesh.get<gmds::Face>(iface3);
                f3.add<gmds::Region>(iBlock);
                gmds::Face f4 = mesh.get<gmds::Face>(iface4);
                f4.add<gmds::Region>(iBlock);
                gmds::Face f5 = mesh.get<gmds::Face>(iface5);
                f5.add<gmds::Region>(iBlock);

                topoB_2_gmdsB[topo_blocs[iBlock]->getName()] = iBlock;

            }

            gmds::Variable<int>* discrI = mesh.newVariable<int, gmds::GMDS_REGION>("discrI");
            gmds::Variable<int>* discrJ = mesh.newVariable<int, gmds::GMDS_REGION>("discrJ");
            gmds::Variable<int>* discrK = mesh.newVariable<int, gmds::GMDS_REGION>("discrK");

            for(int iBlock = 0; iBlock < topo_blocs.size(); iBlock++){

                gmds::Variable<int>* internalNodes = mesh.newVariable<int, gmds::GMDS_NODE>("block"+std::to_string(iBlock));


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

            /* Ici ce sont les informations dont on aura besoin dans le fichier cgns final ça peut aussi bien
             * etre les conditions au bord ou meme les noms des familles des interfaces entre les blocs etc
             *
             * Pour se faire on ecrit juste des variables de maillages sur les noeuds en donnant le nom qui sera
             * utilisé dans le fichier cgns
             */
            //gmds::Variable<int>* farfield   = mesh.newVariable<int, gmds::GMDS_NODE>("Farfield");
            //gmds::Variable<int>* paroi      = mesh.newVariable<int, gmds::GMDS_NODE>("Paroi");
            //gmds::Variable<int>* out        = mesh.newVariable<int, gmds::GMDS_NODE>("Sortie");
            //gmds::Variable<int>* sym        = mesh.newVariable<int, gmds::GMDS_NODE>("Symetrie");


            std::vector<Group::GroupEntity*> groups2D = getStdContext()->getGroupManager().getGroups(Internal::SelectionManager::DIM::D2);
            std::vector<std::string> groupsname;
            for(auto g : groups2D){
                std::string name = g->getName();
                if(name.find("_cgns") != std::string::npos){
                    groupsname.push_back(g->getName());
                    std::string varName = name.substr(0, name.length()-5);
                    mesh.newVariable<int, gmds::GMDS_FACE>(varName);
                }
            }

            /*
             * Pour récupérer les variables à écrire on utilise les groupes des entités topologiques et on va mettre
             * à 1 la valeur de la variable correspondante pour tous les noeuds générés par cette entité topo
             */

            /*
             * Dans le cas des 4 variables ci-dessus on sait que ça va devenir des noms d'interface dans le fichier
             * cgns donc ça correspond ici a un groupe sur les surfaces topo. Pour chaque surface topo on teste si
             * elle appartient au groupe en question, si oui on prend tous ses noeuds et on met la variable à 1
             */
            for(auto f : topo_faces){
                std::vector<std::string> f_groupsName;
                f->getGroupsName(f_groupsName);
                std::vector<gmds::TCellID> f_nodes = f->nodes();
                for(const auto& name : groupsname){
                    if(std::find(f_groupsName.begin(),f_groupsName.end(), name) != f_groupsName.end()){
                        std::string varName = name.substr(0, name.length()-5);
                        gmds::Variable<int>* var = mesh.getVariable<int, gmds::GMDS_FACE>(varName);
                        var->set(topoF_2_gmdsF[f->getName()], 1);
                    }
                }
                /*
                if(std::find(f_groupsName.begin(),f_groupsName.end(), "Farfield") != f_groupsName.end()){
                    for(auto n : f_nodes){
                        farfield->set(n, 1);
                    }
                }else if(std::find(f_groupsName.begin(),f_groupsName.end(), "Paroi") != f_groupsName.end()){
                    for(auto n : f_nodes){
                        paroi->set(n, 1);
                    }
                }else if(std::find(f_groupsName.begin(),f_groupsName.end(), "Sortie") != f_groupsName.end()){
                    for(auto n : f_nodes){
                        out->set(n, 1);
                    }
                }else if(std::find(f_groupsName.begin(),f_groupsName.end(), "Symetrie") != f_groupsName.end()){
                    for(auto n : f_nodes){
                        sym->set(n, 1);
                    }
                }*/
            }

            //gmds::Variable<int>* fluide = mesh.newVariable<int>(gmds::GMDS_NODE,"Fluide"); //Valeur par défaut
            //gmds::Variable<int>* solide = mesh.newVariable<int, gmds::GMDS_NODE>("Solide");

            std::vector<Group::GroupEntity*> groups3D = getStdContext()->getGroupManager().getGroups(Internal::SelectionManager::DIM::D3);

            groupsname.clear();
            for(auto g : groups3D){
                std::string name = g->getName();
                if(name.find("_cgns") != std::string::npos){
                    groupsname.push_back(g->getName());
                    std::string varName = name.substr(0, name.length()-5);
                    mesh.newVariable<int, gmds::GMDS_REGION>(varName);
                }
            }

            for(int iBlock = 0; iBlock < topo_blocs.size(); iBlock++) {
                std::vector<std::string> b_groupsName;
                topo_blocs[iBlock]->getGroupsName(b_groupsName);
                for(const auto& name : groupsname) {
                    if (std::find(b_groupsName.begin(), b_groupsName.end(), name) != b_groupsName.end()) {
                        std::string varName = name.substr(0, name.length()-5);
                        gmds::Variable<int>* var = mesh.getVariable<int, gmds::GMDS_REGION>(varName);
                        var->set(iBlock, 1);
                    }
                }
            }

            int position = m_filename.find_last_of('/');

            m_basename = m_filename.substr(position+1, m_filename.length()-position+1);

            m_path = m_filename.substr(0, position+1);

            gmds::aero::CGNSWriterND writer(&mesh, m_dim);
            writer.write("",m_basename,m_path);
        }
/*----------------------------------------------------------------------------*/
        void ExportBlocksCGNSImplementation::perform2D()
        {
            gmds::Mesh mesh(gmds::MeshModel(gmds::DIM2 | gmds::N | gmds::E | gmds::F |
                                            gmds::F2N | gmds::N2F | gmds::F2E |  gmds::E2F | gmds::E2N | gmds::N2E));

            gmds::Mesh* localMesh = &m_context.getMeshManager().getMesh()->getGMDSMesh();


            for(int iVertex = 0; iVertex < localMesh->getNbNodes(); iVertex++){
                gmds::Node n_local = localMesh->get<gmds::Node>(iVertex);
                gmds::Node n = mesh.newNode(n_local.X(),n_local.Y(),n_local.Z());
            }

            std::map<std::string, int> topoE_2_gmdsE;
            const std::vector<Topo::CoEdge*>& topo_edges = m_context.getTopoManager().getCoEdgesObj();
            for (auto ce : topo_edges) {
                int n0 = ce->getVertices()[0]->getNode();
                int n1 = ce->getVertices()[1]->getNode();

                gmds::Edge e = mesh.newEdge(n0,n1);

                topoE_2_gmdsE[ce->getName()] = e.id();
            }

            std::map<std::string, int> topoF_2_gmdsF;
            const std::vector<Topo::CoFace*>& topo_faces = m_context.getTopoManager().getCoFacesObj();
            for(int iFace = 0; iFace < topo_faces.size(); iFace++){
                Topo::CoFace* face = topo_faces[iFace];
                std::vector<unsigned long> ids;
                std::vector<Topo::Vertex*> vertices;
                vertices.resize(4);

                vertices[0] = face->getVertices()[0];
                vertices[1] = face->getVertices()[1];
                vertices[2] = face->getVertices()[2];
                vertices[3] = face->getVertices()[3];


                ids.push_back(topo_faces[iFace]->getVertices()[0]->getNode());
                ids.push_back(topo_faces[iFace]->getVertices()[1]->getNode());
                ids.push_back(topo_faces[iFace]->getVertices()[2]->getNode());
                ids.push_back(topo_faces[iFace]->getVertices()[3]->getNode());


                gmds::Face f = mesh.newQuad(ids[0],ids[1],ids[2],ids[3]);

                if(topo_faces[iFace]->getEdges().size()!= 4 ){
                    throw TkUtil::Exception(TkUtil::UTF8String ("La face "+topo_faces[iFace]->getName()+" n'est pas conforme.", TkUtil::Charset::UTF_8));
                }

                int iedge0 = topoE_2_gmdsE[face->getEdge(vertices[0],vertices[1])->getCoEdges()[0]->getName()];
                int iedge1 = topoE_2_gmdsE[face->getEdge(vertices[1],vertices[2])->getCoEdges()[0]->getName()];
                int iedge2 = topoE_2_gmdsE[face->getEdge(vertices[2],vertices[3])->getCoEdges()[0]->getName()];
                int iedge3 = topoE_2_gmdsE[face->getEdge(vertices[3],vertices[0])->getCoEdges()[0]->getName()];

                f.add<gmds::Edge>(iedge0);
                f.add<gmds::Edge>(iedge1);
                f.add<gmds::Edge>(iedge2);
                f.add<gmds::Edge>(iedge3);

                gmds::Edge e0 = mesh.get<gmds::Edge>(iedge0);
                e0.add<gmds::Face>(iFace);
                gmds::Edge e1 = mesh.get<gmds::Edge>(iedge1);
                e1.add<gmds::Face>(iFace);
                gmds::Edge e2 = mesh.get<gmds::Edge>(iedge2);
                e2.add<gmds::Face>(iFace);
                gmds::Edge e3 = mesh.get<gmds::Edge>(iedge3);
                e3.add<gmds::Face>(iFace);

                topoF_2_gmdsF[topo_faces[iFace]->getName()] = iFace;

            }

            gmds::Variable<int>* discrI = mesh.newVariable<int, gmds::GMDS_FACE>("discrI");
            gmds::Variable<int>* discrJ = mesh.newVariable<int, gmds::GMDS_FACE>("discrJ");

            for(int iFace = 0; iFace < topo_faces.size(); iFace++){

                gmds::Variable<int>* internalNodes = mesh.newVariable<int, gmds::GMDS_NODE>("block"+std::to_string(iFace));


                uint b_discrI,b_discrJ;
                topo_faces[iFace]->getNbMeshingEdges(b_discrJ,b_discrI);
                discrI->set(iFace, b_discrI+1);
                discrJ->set(iFace, b_discrJ+1);

                std::vector<gmds::TCellID> b_nodes = topo_faces[iFace]->nodes();
                for(int iNode = 0; iNode < b_nodes.size(); iNode++){
                    //std::cout<<mesh.get<gmds::Node>(b_nodes[iNode])<<std::endl;
                    internalNodes->set(b_nodes[iNode], iNode+1);
                }
            }

            std::vector<Group::GroupEntity*> groups1D = getStdContext()->getGroupManager().getGroups(Internal::SelectionManager::DIM::D1);
            std::vector<std::string> groupsname;
            for(auto g : groups1D){
                std::string name = g->getName();
                if(name.find("_cgns") != std::string::npos){
                    groupsname.push_back(g->getName());
                    std::string varName = name.substr(0, name.length()-5);
                    mesh.newVariable<int, gmds::GMDS_EDGE>(varName);
                }
            }

            for(auto e : topo_edges){
                std::vector<std::string> f_groupsName;
                e->getGroupsName(f_groupsName);
                std::vector<gmds::TCellID> f_nodes = e->nodes();
                for(const auto& name : groupsname){
                    if(std::find(f_groupsName.begin(),f_groupsName.end(), name) != f_groupsName.end()){
                        std::string varName = name.substr(0, name.length()-5);
                        gmds::Variable<int>* var = mesh.getVariable<int, gmds::GMDS_EDGE>(varName);
                        var->set(topoE_2_gmdsE[e->getName()], 1);
                    }
                }
            }

            std::vector<Group::GroupEntity*> groups2D = getStdContext()->getGroupManager().getGroups(Internal::SelectionManager::DIM::D2);
            groupsname.clear();
            for(auto g : groups2D){
                std::string name = g->getName();
                if(name.find("_cgns") != std::string::npos){
                    groupsname.push_back(g->getName());
                    std::string varName = name.substr(0, name.length()-5);
                    mesh.newVariable<int, gmds::GMDS_FACE>(varName);
                }
            }

            for(int iFace = 0; iFace < topo_faces.size(); iFace++) {
                std::vector<std::string> b_groupsName;
                topo_faces[iFace]->getGroupsName(b_groupsName);
                for(const auto& name : groupsname) {
                    if (std::find(b_groupsName.begin(), b_groupsName.end(), name) != b_groupsName.end()) {
                        std::string varName = name.substr(0, name.length()-5);
                        gmds::Variable<int>* var = mesh.getVariable<int, gmds::GMDS_FACE>(varName);
                        var->set(iFace, 1);
                    }
                }
            }


            int position = m_filename.find_last_of('/');

            m_basename = m_filename.substr(position+1, m_filename.length()-position+1);

            m_path = m_filename.substr(0, position+1);

            gmds::aero::CGNSWriterND writer(&mesh, m_dim);
            writer.write("",m_basename,m_path);

        }
/*----------------------------------------------------------------------------*/
    } // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
