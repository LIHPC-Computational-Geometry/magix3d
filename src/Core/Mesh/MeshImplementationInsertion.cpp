/*----------------------------------------------------------------------------*/
/*
 * \file MeshImplementationInsertion.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18 oct. 2013
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/CommandCreateMesh.h"

#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/Edge.h"
#include "Topo/BlockMeshingPropertyInsertion.h"

#include "Utils/Common.h"
#include "Internal/Context.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/OCCGeomRepresentation.h"
/*----------------------------------------------------------------------------*/
/// TkUtil
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>
/*----------------------------------------------------------------------------*/
#include "Geom/GMDSGeomManagerAdapter.h"
#include "Geom/GMDSGeomVolumeAdapter.h"

#include "GMDS/CAD/FacetedGeomManager.h"
//#include "GMDS/CAD/FacetedMeshIntersectionService.h"
//#include "CaGe/MeshInsertDetailInOut.h"
//#include "CaGe/MeshSplitter3Refinement.h"
//#include "CaGe/LaplacianSmoothingGeomClassificationNew.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
void MeshImplementation::
meshInsertion(Mesh::CommandCreateMesh* command, Topo::Block* ABlock)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("MeshImplementation::meshInsertion. No more imlemented", TkUtil::Charset::UTF_8));
////#ifdef _DEBUG_MESH
//    std::cout <<"Maillage du bloc "<<ABlock->getName()<<" avec la méthode par insertion"<<std::endl;
////#endif
//
//
//    Topo::BlockMeshingPropertyInsertion* meshingProp =
//                dynamic_cast<Topo::BlockMeshingPropertyInsertion*>(ABlock->getBlockMeshingProperty());
//
//    const int mask = DIM3|N|E|F|R|E2N|F2N|R2N|F2E|R2F|F2R|N2F|N2E|E2F|N2R|E2R|R2E;
//    gmds::Mesh<mask> mesh_tmp;
//    mesh_tmp.initializeGeometryClassification();
//
//    std::map<Node*,Node*> old2new_meshNodes;
//    std::map<Node*,Node*> new2old_meshNodes;
//    std::map<Face*,Face*> old2new_meshFaces;
//    std::map<Face*,Face*> new2old_meshFaces;
//    std::map<Region*,Region*> old2new_meshRegions;
//    std::map<Region*,Region*> new2old_meshRegions;
//
//    // copie du maillage car le mask nécessaire pour l'insertion est différent du mask de Magix3D
//    {
//        gmds::Mesh<TMask>::nodes_iterator it  = this->getGMDSMesh().nodes_begin();
//
//        for(;!it->isDone();it->next()) {
//            Node* current_node = it->currentItem();
//            GEPETO::Point<3,gmds::TCoord> current_point = current_node->getPoint();
//            Node* node = mesh_tmp.newNode(current_point.getX(),current_point.getY(),current_point.getZ());
//            old2new_meshNodes[current_node] = node;
//            new2old_meshNodes[node] = current_node;
//        }
//    }
//    {
//        gmds::Mesh<TMask>::faces_iterator it  = this->getGMDSMesh().faces_begin();
//
//        for(;!it->isDone();it->next()) {
//            Face* current_face = it->currentItem();
//
//            std::vector<Node*> nodes = current_face->getNodes();
//            std::vector<Node*> nodes_new(nodes.size(),NULL);
//
//            for(unsigned int iNode=0; iNode<nodes.size(); iNode++) {
//                nodes_new[iNode] = old2new_meshNodes[nodes[iNode]];
//            }
//
//            Face* newFace = mesh_tmp.newFace(nodes_new);
//            new2old_meshFaces[newFace] = current_face;
//            old2new_meshFaces[current_face] = newFace;
//        }
//    }
//    {
//        gmds::Mesh<TMask>::regions_iterator it  = this->getGMDSMesh().regions_begin();
//
//        for(;!it->isDone();it->next()) {
//            Region* current_region = it->currentItem();
//
//            if(current_region->getType() != gmds::GMDS_HEX) {
//                std::cout<<"MeshImplementation::meshInsertion can not work with "
//                        "cell types other than hexahedra."<<std::endl;
//                throw TkUtil::Exception ("MeshImplementation::meshInsertion échec.");
//            }
//
//            std::vector<Node*> nodes = current_region->getNodes();
//
//            std::vector<Node*> nodes_new(nodes.size(),NULL);
//
//            for(unsigned int iNode=0; iNode<nodes.size(); iNode++) {
//                nodes_new[iNode] = old2new_meshNodes[nodes[iNode]];
//            }
//
//            Region* newHex = mesh_tmp.newHex(nodes_new);
//            new2old_meshRegions[newHex] = current_region;
//            old2new_meshRegions[current_region] = newHex;
//        }
//    }
//
//
//
//    Mgx3D::Geom::GeomManager& poyop = getContext().getLocalGeomManager();
//    Mgx3D::Geom::GMDSGeomManagerAdapter model_bis(poyop);
//
//    gmds::LimaWriter<mask> meshWriter(mesh_tmp);
//    meshWriter.write("meshInsertion_0.mli",R|F|E|N);
//
//    typedef GEPETO::Numeric<double> GeomMeshTCoord;
//
////    std::vector<gmds::geom::GeomVolume<GeomMeshTCoord>* > vols;
//    gmds::geom::FacetedGeomManager<GeomMeshTCoord> model_tmp;
//
//    gmds::geom::FacetedGeomManager<GeomMeshTCoord> model;
//
//
//    gmds::geom::FacetedMeshIntersectionService<GeomMeshTCoord>* service =
//            new gmds::geom::FacetedMeshIntersectionService<GeomMeshTCoord> ();
//
//    Geom::GeomEntity* vol_tmp = ABlock->getGeomAssociation();
//    Geom::Volume* vol_tmp2 = dynamic_cast<Geom::Volume*> (vol_tmp);
//    Mgx3D::Geom::GMDSGeomVolumeAdapter* vol = dynamic_cast<Mgx3D::Geom::GMDSGeomVolumeAdapter*> (model_bis.getGMDSVolume(vol_tmp2));
//
//    model_bis.reorientSurfacesTriangulation(vol_tmp2);
//
////    {
////        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* volumeClassification = mesh_tmp.getGeometricClassification(3);
////
////        std::vector<Topo::Block* > blocks;
////        getContext().getLocalTopoManager().getBlocks(blocks);
////
////        for(unsigned int iBlock=0; iBlock<getContext().getLocalTopoManager().getNbBlocks(); iBlock++) {
////            if(blocks[iBlock] != ABlock) {
////                std::vector<gmds::Region*> regions = blocks[iBlock]->regions();
////                Geom::GeomEntity* vol_tmp = blocks[iBlock]->getGeomAssociation();
////                Geom::Volume* vol_tmp2 = dynamic_cast<Geom::Volume*> (vol_tmp);
////                Mgx3D::Geom::GMDSGeomVolumeAdapter* vol = dynamic_cast<Mgx3D::Geom::GMDSGeomVolumeAdapter*> (model_bis.getGMDSVolume(vol_tmp2));
////
////                for(unsigned int iRegion=0; iRegion<regions.size(); iRegion++) {
////                    (*volumeClassification)[old2new_meshRegions[regions[iRegion]]->getID()] = vol;
////                }
////            }
////        }
////    }
//
//    //DEFINITION DE LA CLASSIFICATION GEOMETRIQUE
//    // association des régions au bloc
//    {
//        std::vector<Topo::CoFace* > block_faces;
//        std::vector<Topo::CoEdge* > block_edges;
//        std::vector<Topo::Vertex* > block_vertices;
//        ABlock->getCoFaces(block_faces);
//        ABlock->getCoEdges(block_edges);
//        ABlock->getVertices(block_vertices);
//
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* cloudClassification  = mesh_tmp.getGeometricClassification(0);
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* lineClassification   = mesh_tmp.getGeometricClassification(1);
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* surfaceClassification= mesh_tmp.getGeometricClassification(2);
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* volumeClassification = mesh_tmp.getGeometricClassification(3);
//
//        std::vector<Topo::Block* > blocks;
//        getContext().getLocalTopoManager().getBlocks(blocks);
//
//        for(unsigned int iBlock=0; iBlock<getContext().getLocalTopoManager().getNbBlocks(); iBlock++) {
//            std::vector<gmds::Region*>& regions = blocks[iBlock]->regions();
//            Geom::GeomEntity* vol_tmp = blocks[iBlock]->getGeomAssociation();
//            Geom::Volume* vol_tmp2 = dynamic_cast<Geom::Volume*> (vol_tmp);
//            Mgx3D::Geom::GMDSGeomVolumeAdapter* vol = dynamic_cast<Mgx3D::Geom::GMDSGeomVolumeAdapter*> (model_bis.getGMDSVolume(vol_tmp2));
//
//            for(unsigned int iR=0;iR<regions.size();iR++){
//                Region* r = regions[iR];
//                (*volumeClassification)[old2new_meshRegions[r]->getID()] = vol;
//                std::vector<gmds::Node*> nodes = r->getNodes();
//                for(unsigned iN=0;iN<nodes.size();iN++)
//                {
//                    Node* ni = nodes[iN];
//                    std::cout<<"NODE IN VOLUME"<<std::endl;
//                    (*cloudClassification)[old2new_meshNodes[ni]->getID()] = vol;
//
//                }
//            }
//
//        }
//
//        std::vector<Topo::CoFace* > topo_faces;
//        getContext().getLocalTopoManager().getCoFaces(topo_faces);
//
//        for(unsigned int iCoFace=0; iCoFace<topo_faces.size(); iCoFace++) {
//
//            Topo::CoFace* current  = topo_faces[iCoFace];
//            if(std::find(block_faces.begin(),block_faces.end(),current)==block_faces.end())
//            {
//                std::vector<gmds::Face*>& elems = current->faces();
//                Geom::Surface* surf_tmp = dynamic_cast<Geom::Surface*> (topo_faces[iCoFace]->getGeomAssociation());
//                Mgx3D::Geom::GMDSGeomSurfaceAdapter* surf = dynamic_cast<Mgx3D::Geom::GMDSGeomSurfaceAdapter*> (model_bis.getGMDSSurface(surf_tmp));
//
//
//                for(unsigned int iF=0;iF<elems.size();iF++){
//                    Face* f = elems[iF];
//                    (*surfaceClassification)[old2new_meshFaces[f]->getID()]= surf;
//                    std::vector<gmds::Node*> nodes = f->getNodes();
//                    for(unsigned iN=0;iN<nodes.size();iN++)
//                    {
//                        Node* ni = nodes[iN];
//                        std::cout<<"NODE ON SURF"<<std::endl;
//                        (*cloudClassification)[old2new_meshNodes[ni]->getID()] = surf;
//
//                    }
//                }
//            }
//        }
//
//        std::vector<Topo::Vertex* > topo_vertices;
//        getContext().getLocalTopoManager().getVertices(topo_vertices);
//        for(unsigned int iV=0; iV<topo_vertices.size(); iV++)
//        {
//            Topo::Vertex* v_i =topo_vertices[iV];
//
//            if(std::find(block_vertices.begin(),block_vertices.end(),v_i)==block_vertices.end())
//            {
//
//                try{
//                    gmds::Node* n =v_i->getNode();
//                    Geom::Vertex* geom_v_tmp = dynamic_cast<Geom::Vertex*> (v_i->getGeomAssociation());
//                    Mgx3D::Geom::GMDSGeomPointAdapter* geom_v = dynamic_cast<Mgx3D::Geom::GMDSGeomPointAdapter*> (model_bis.getGMDSVertex(geom_v_tmp));
//
//
//                    std::cout<<"NODE ON POINT"<<std::endl;
//                    (*cloudClassification)[old2new_meshNodes[n]->getID()] = geom_v;
//                }
//                catch(TkUtil::Exception &e){;}
//            }
//        }
//
//        std::vector<Topo::CoEdge* > topo_edges;
//        getContext().getLocalTopoManager().getCoEdges(topo_edges);
//        for(unsigned int iCoEdge=0; iCoEdge<topo_edges.size(); iCoEdge++)
//        {
//            Topo::CoEdge* co_edge_i =topo_edges[iCoEdge];
//            if(std::find(block_edges.begin(),block_edges.end(),co_edge_i)==block_edges.end())
//            {
//
//                std::vector<gmds::Node*>& elems = co_edge_i->nodes();
//                Geom::Curve* curv_tmp = dynamic_cast<Geom::Curve*> (co_edge_i->getGeomAssociation());
//                Mgx3D::Geom::GMDSGeomCurveAdapter* curv = dynamic_cast<Mgx3D::Geom::GMDSGeomCurveAdapter*> (model_bis.getGMDSCurve(curv_tmp));
//
//
//                for(unsigned int iF=0;iF<elems.size();iF++){
//                    Node *ni=elems[iF];
//                    std::cout<<"NODE ON CURVE"<<std::endl;
//                    (*cloudClassification)[old2new_meshNodes[ni]->getID()] = curv;
//                }
//            }
//        }
//    }
//
//
//    gmds::MeshInsertDetailInOut<mask,GeomMeshTCoord> insertion(mesh_tmp,model_bis,*service);
//
////    insertion.exportLima("meshInsertion_1.mli");
//    gmds::MeshSplitter3Refinement<mask,GeomMeshTCoord> meshSplitter(mesh_tmp,model_bis,*service);
//    if(meshingProp->withRefinement()) {
//    	meshSplitter.splitMesh();
//    }
////    std::set<Region*> splitRegions   = meshSplitter.getSplitRegions();
////    std::set<Face*  > splitFaces     = meshSplitter.getSplitFaces();
////    std::set<Edge*  > splitEdges     = meshSplitter.getSplitEdges();
////    std::set<Region*> createdRegions = meshSplitter.getCreatedRegions();
////    std::set<Face*  > createdFaces   = meshSplitter.getCreatedFaces();
////    std::set<Edge*  > createdEdges   = meshSplitter.getCreatedEdges();
////    std::set<Node*  > createdNodes   = meshSplitter.getCreatedNodes();
//    insertion.exportLima("meshInsertion_2.unf");
//
////    insertion.exportLima("meshInsertion_2.mli");
//    gmds::MeshDoctor<mask> doc(mesh_tmp);
//    doc.buildFacesAndR2F();
//    doc.buildEdgesAndX2E();
//    doc.updateUpwardConnectivity();
//
//
//
//    insertion.exportLima("meshInsertion_3.mli");
//
//    insertion.autoInsert(vol);
////#ifdef GMDS_DEBUG_INSERTION_FILES
//    insertion.exportLima("meshInsertion_4.mli");
////#endif
//    insertion.project();
//
//#ifdef GMDS_DEBUG_INSERTION_FILES
//    insertion.exportLima("meshInsertion_5.mli");
//#endif
////#ifdef GMDS_DEBUG_INSERTION_FILES
//    insertion.exportLima("meshInsertion_6.mli");
//
//    // RELATIVEMENT AU VOLUME QUI NOUS INTERESSE
//    //Mgx3D::Geom::GMDSGeomVolumeAdapter* vol = dynamic_cast<Mgx3D::Geom::GMDSGeomVolumeAdapter*> (model_bis.getGMDSVolume(vol_tmp2));
//
//    insertion.insertFunSheets(vol);
//    insertion.exportLima("meshInsertion_7.mli");
////#endif
//
//    std::vector<Node*  > newN, delN;
//    std::vector<Face*  > newF, delF;
//    std::vector<Region*> newR, delR;
//
//    int removed_mark = this->getGMDSMesh().getNewMark();
//    // must put back the mesh_tmp modifications.
//    // - modification of nodes positions.
//    // - node creation in case of refinement
//    {
//        // update node position
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* nodeClassificationTmp = mesh_tmp.getGeometricClassification(0);
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* nodeClassification = this->getGMDSMesh().getGeometricClassification(0);
//
//
//        gmds::Mesh<TMask>::nodes_iterator it  =  mesh_tmp.nodes_begin();
//        for(;!it->isDone();it->next()) {
//            gmds::Node* current_node = it->currentItem();
//            gmds::Node* node_old = 0;
//
//            if(new2old_meshNodes.find(current_node)==new2old_meshNodes.end())
//            {
//                GEPETO::Point<3,gmds::TCoord> p = current_node->getPoint();
//                node_old= this->getGMDSMesh().newNode(p.getX(),p.getY(),p.getZ());
//                newN.push_back(node_old);
//                new2old_meshNodes[current_node]=node_old;
//                old2new_meshNodes[node_old]=current_node;
//                (*nodeClassification)[node_old->getID()]=(*nodeClassificationTmp)[current_node->getID()];
//
//            }
//            else {
//                node_old = new2old_meshNodes[current_node];
//                GEPETO::Point<3,gmds::TCoord> current_point = current_node->getPoint();
//                node_old->setPoint(current_point);
//                (*nodeClassification)[node_old->getID()]=(*nodeClassificationTmp)[current_node->getID()];
//            }
//            this->getGMDSMesh().mark(node_old,removed_mark);
//
//        }
//        // il est possible que des noeuds aient été supprimés, ce sont qui ne sont
//        // pas marqués removed_mark
//        gmds::Mesh<TMask>::nodes_iterator it2  = this->getGMDSMesh().nodes_begin();
//
//        for(;!it2->isDone();it2->next()) {
//            Node* n = it2->currentItem();
//            if(!this->getGMDSMesh().isMarked(n,removed_mark)){
//                //C'est un sommet à supprimer
//
//                delN.push_back(n);
//            }
//        }
////        // create new nodes inserted during refinement
////        std::set<gmds::Node*>::iterator itn =  createdNodes.begin();
////        std::set<gmds::Node*>::iterator itnend =  createdNodes.end();
////
////        for(; itn != itnend; itn++) {
////            gmds::Node* current_node = *itn;
////
////            gmds::Node* node_old = this->getGMDSMesh().newNode(
////                    current_node->getPoint().getX(),current_node->getPoint().getY(),current_node->getPoint().getZ());
////            old2new_meshNodes[node_old] = current_node;
////            new2old_meshNodes[current_node] = node_old;
//        //        }
//    }
//    {
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* faceClassificationTmp = mesh_tmp.getGeometricClassification(2);
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* faceClassification = this->getGMDSMesh().getGeometricClassification(2);
//        gmds::Mesh<TMask>::faces_iterator it  =  mesh_tmp.faces_begin();
//        for(;!it->isDone();it->next()) {
//            gmds::Face* current_face= it->currentItem();
//            gmds::Face* face_old = NULL;
//
//            if(new2old_meshFaces.find(current_face)==new2old_meshFaces.end())
//            {
//                // C'est une nouvelle face
//            	// On ne la créé que si cette face est associée à une surface
//            	if((*faceClassificationTmp)[current_face->getID()] != NULL) {
//            		if((*faceClassificationTmp)[current_face->getID()]->getDim() == 2) {
//
//            			std::vector<Node*> fnodes= current_face->getNodes();
//            			std::vector<Node*> f2nodes;
//            			for(unsigned int i=0;i<fnodes.size();i++)
//            				f2nodes.push_back(new2old_meshNodes[fnodes[i]]);
//
//            			face_old = this->getGMDSMesh().newFace(f2nodes);
//            			newF.push_back(face_old);
//            			new2old_meshFaces[current_face]=face_old;
//            			old2new_meshFaces[face_old]=current_face;
//            		}
//            	}
//            }
//            else{
//                face_old = new2old_meshFaces[current_face];
//                //meme si la face existait bien, ses sommets ont pu être modifie
//                //car sur la surface de pillow
//                std::vector<Node*> fnodes= current_face->getNodes();
//                std::vector<Node*> f2nodes;
//                for(unsigned int i=0;i<fnodes.size();i++)
//                    f2nodes.push_back(new2old_meshNodes[fnodes[i]]);
//
//                face_old->setNodes(f2nodes);
//            }
//            if(face_old != NULL) {
//            	this->getGMDSMesh().mark(face_old,removed_mark);
//            	(*faceClassification)[face_old->getID()] =(*faceClassificationTmp)[current_face->getID()];
//            }
//
//        }
//
//        // il est possible que des noeuds aient été supprimés, ce sont qui ne sont
//        // pas marqués removed_mark
//        gmds::Mesh<TMask>::faces_iterator it2  = this->getGMDSMesh().faces_begin();
//        for(;!it2->isDone();it2->next()) {
//            Face* f = it2->currentItem();
//            if(!this->getGMDSMesh().isMarked(f,removed_mark)){
//                //C'est une face à supprimer
//                delF.push_back(f);
//            }
//        }
//    }
//    {
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* regionClassificationTmp = mesh_tmp.getGeometricClassification(3);
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* regionClassification = this->getGMDSMesh().getGeometricClassification(3);
//        gmds::Mesh<TMask>::regions_iterator it  =  mesh_tmp.regions_begin();
//        for(;!it->isDone();it->next())
//        {
//            gmds::Region* current_region = it->currentItem();
//            gmds::Region* region_old = 0;
//
//            if(new2old_meshRegions.find(current_region)==new2old_meshRegions.end())
//            {
//                // C'est une nouvelle region
//                std::vector<Node*> rnodes= current_region->getNodes();
//                std::vector<Node*> r2nodes;
//                for(unsigned int i=0;i<rnodes.size();i++)
//                    r2nodes.push_back(new2old_meshNodes[rnodes[i]]);
//
//                region_old = this->getGMDSMesh().newRegion(current_region->getType(),r2nodes);
//                newR.push_back(region_old);
//                new2old_meshRegions[current_region]=region_old;
//                old2new_meshRegions[region_old]=current_region;
//
//            }
//            else{
//                region_old = new2old_meshRegions[current_region];
//                //meme si la face existait bien, ses sommets ont pu être modifie
//                //car sur la surface de pillow
//                std::vector<Node*> rnodes= current_region->getNodes();
//                std::vector<Node*> r2nodes;
//                for(unsigned int i=0;i<rnodes.size();i++)
//                    r2nodes.push_back(new2old_meshNodes[rnodes[i]]);
//
//                region_old->setNodes(r2nodes);
//            }
//            this->getGMDSMesh().mark(region_old,removed_mark);
//            (*regionClassification)[region_old->getID()] =(*regionClassificationTmp)[current_region->getID()];
//        }
//
//
//        // il est possible que des regions aient été supprimés, ce sont qui ne sont
//        // pas marqués removed_mark
//        gmds::Mesh<TMask>::regions_iterator it2  = this->getGMDSMesh().regions_begin();
//        for(;!it2->isDone();it2->next()) {
//            Region* r = it2->currentItem();
//            if(!this->getGMDSMesh().isMarked(r,removed_mark)){
//                //C'est une face à supprimer
//                delR.push_back(r);
//            }
//        }
//    }
//
//
//    this->getGMDSMesh().unmarkAll(removed_mark);
//    this->getGMDSMesh().freeMark(removed_mark);
//
//    std::cout<<"======== SUPPRESSION (N,F,R) = ("<<delN.size()<<", "<<delF.size()<<", "<<delR.size()<<")"<<std::endl;
//    std::cout<<"======== AJOUT       (N,F,R) = ("<<newN.size()<<", "<<newF.size()<<", "<<newR.size()<<")"<<std::endl;
//    // On supprime vraiment les noeuds maintenant
//    for(unsigned int iN=0;iN<delN.size();iN++)
//    {
//        Node *n = delN[iN];
//        this->getGMDSMesh().deleteNode(n);
//        gmds::Mesh<TMask>::clouds_iterator cl_it = this->getGMDSMesh().clouds_begin();
//        for(;cl_it!=this->getGMDSMesh().clouds_end();cl_it++)
//        {
//            gmds::Mesh<TMask>::cloud& current_cloud = *cl_it;
//            current_cloud.del(n);
//        }
//
//    }
//
//
//    // association des régions au bloc
////    {
////        Variable<geom::GeomEntity<GeomMeshTCoord>* >* regionClassification = mesh_tmp.getGeometricClassification(3);
////
////        std::vector<gmds::Region*>& elem = bl->regions();
////
////        gmds::Mesh<TMask>::regions_iterator it  = mesh_tmp.regions_begin();
////
////        for(;!it->isDone();it->next()) {
////            Region* current_region = it->currentItem();
////
////            if(new2old_meshRegions.find(current_region) == new2old_meshRegions.end()) {
////
////            } else {
////                if((*regionClassification)[current_region->getID()] == vol) {
////                    Region* r = new2old_meshRegions[current_region];
////                    elem.push_back(r);
//////                    command->addCreatedRegion(r);
////                }
////            }
////        }
////
////    }
//    // association des régions au bloc
//    std::map<Topo::Block*,std::set<gmds::Region*> > blocksRegionsSwitched;
//    {
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* volumeClassification = mesh_tmp.getGeometricClassification(3);
//
//        std::vector<Topo::Block* > blocks;
//        getContext().getLocalTopoManager().getBlocks(blocks);
//
//        for(unsigned int iBlock=0; iBlock<getContext().getLocalTopoManager().getNbBlocks(); iBlock++)
//        {
//            //on recupere les regions du bloc de numéro iBlock
//            std::vector<gmds::Region*>& regions = blocks[iBlock]->regions();
//
//            // On recupere le volume geometrique correspondant à ce bloc
//            // Attention, plusieurs blocs peuvent correspondre au meme volume geometrique.
//            // Est-ce pris en charge?
//            Geom::GeomEntity* vol_tmp = blocks[iBlock]->getGeomAssociation();
//            Geom::Volume* vol_tmp2 = dynamic_cast<Geom::Volume*> (vol_tmp);
//            Mgx3D::Geom::GMDSGeomVolumeAdapter* vol =
//                    dynamic_cast<Mgx3D::Geom::GMDSGeomVolumeAdapter*> (model_bis.getGMDSVolume(vol_tmp2));
//            if(blocks[iBlock] != ABlock)
//            {
//                //On est dans un bloc autre que celui inséré
//                //On parcourt les régions qui lui sont associées initialement
//                std::vector<gmds::Region*>::iterator itr = regions.begin();
//                for(;itr!=regions.end();itr++)
//                {
//                    //On regarde si la region correspondante dans le maillage générée
//                    //est toujours dans ce volume
//                    if((*volumeClassification)[old2new_meshRegions[(*itr)]->getID()] != vol)
//                    { //NON, on la marque comme à supprimer
//                        blocksRegionsSwitched[blocks[iBlock]].insert(*itr);
//                    }
//                }
//            }
//            else
//            {
//                //On est dans le bloc à insérer. Avant l'insertion, aucun élément de maillage
//                //ne lui était associé
//                gmds::Mesh<TMask>::regions_iterator it  = mesh_tmp.regions_begin();
//
//                for(;!it->isDone();it->next())
//                {
//                    Region* current_region = it->currentItem();
//
//                    if((*volumeClassification)[current_region->getID()] == vol)
//                    {
//                        regions.push_back(new2old_meshRegions[current_region]);
//                    }
//                }
//            } // if(blocks[iBlock] != ABlock)
//        }
//
//    }
//
//    // retrait des polyedres aux volumes
//    // TODO Probleme de noms de groupes qui ne sont pas reporté du maillage M3D au maillage utilise
//    // pour l'insertion
////    {
////        std::map<Topo::Block*,std::set<gmds::Region*> >::iterator itBlocks = blocksRegionsSwitched.begin();
////
////        for(; itBlocks != blocksRegionsSwitched.end(); itBlocks++) {
////            Topo::Block* bl = (*itBlocks).first;
////            std::cout<<"On a un bloc dont on supprime des regions"<<std::endl;
////            std::vector<std::string> groupsName;
////            bl->getGroupsName(groupsName);
////            std::cout<<"\t il est associe a "<<groupsName.size()<<" groupes"<<std::endl;
////            for(int i=0;i<groupsName.size();i++){
////                std::cout<<"\t  -- "<<groupsName[i]<<std::endl;
////            }
////            for(size_t i=0; i<groupsName.size(); i++) {
////
////                std::string& nom = groupsName[i];
////                std::cout<<"Name = "<<nom<<std::endl;
////                try{
////                    gmds::Mesh<TMask>::volume& vo = getGMDSMesh().getVolume(nom);
////                    //                _removeRegionsFromVolume((*itBlocks).second,vo);
////                }
////                catch(gmds::GMDSException& e)
////                {
////                    std::cout<<"ERREUR"<<std::endl;
////                }
////
////            }
////        }
////    }
//
//    // ajout des polyedres aux volumes
//    std::vector<std::string> groupsName;
//    ABlock->getGroupsName(groupsName);
//
//#ifdef _DEBUG_GROUP_BY_TOPO_ENTITY
//    // on ajoute un groupe pour distinguer les blocs en mode debug
//    if (groupsName.empty()){
//        groupsName.push_back(ABlock->getName());
//	 	   TkUtil::UTF_8   message1 (Charset::UTF_8);
//        message1 <<"Le bloc "<<ABlock->getName()<<" n'a pas de groupe pour les mailles";
//        getContext().getLogStream()->log (TkUtil::TraceLog (message1, TkUtil::Log::ERROR));
//    }
//#endif
//
//    for (size_t i=0; i<groupsName.size(); i++){
//
//        std::string& nom = groupsName[i];
//
//        bool isNewVolume = false;
//        try {
//            getContext().getLocalMeshManager().getVolume(nom);
//            command->addModifiedVolume(nom);
//        } catch (...) {
//            command->addNewVolume(nom);
//            isNewVolume = true;
//        }
//
//        // le volume de maillage que l'on vient de créer/modifier
//        Mesh::Volume* vo = getContext().getLocalMeshManager().getVolume(nom);
//        vo->saveMeshVolumeTopoProperty(&command->getInfoCommand());
//        vo->addBlock(ABlock);
//        if (isNewVolume)
//             getContext().newGraphicalRepresentation (*vo);
//    } // end for i<groupsName.size()
//
//    // association des faces GMDS aux cofaces du bloc
//    {
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* surfaceClassification = mesh_tmp.getGeometricClassification(2);
//
//        std::vector<Topo::Face* > faces;
//        ABlock->getFaces(faces);
//
//        for(unsigned int iFace=0; iFace<faces.size(); iFace++) {
//            for(unsigned int iCoFace=0; iCoFace<faces[iFace]->getNbCoFaces(); iCoFace++) {
//
//                std::vector<gmds::Face*>& elems = faces[iFace]->getCoFace(iCoFace)->faces();
//
//                Geom::Surface* surf_tmp = dynamic_cast<Geom::Surface*> (faces[iFace]->getCoFace(iCoFace)->getGeomAssociation());
//                Mgx3D::Geom::GMDSGeomSurfaceAdapter* surf = dynamic_cast<Mgx3D::Geom::GMDSGeomSurfaceAdapter*> (model_bis.getGMDSSurface(surf_tmp));
//
//                gmds::Mesh<TMask>::faces_iterator it  = mesh_tmp.faces_begin();
//
//                for(;!it->isDone();it->next()) {
//                    gmds::Face* current_face = it->currentItem();
//
//                    if((*surfaceClassification)[current_face->getID()] == surf) {
//                        if(new2old_meshFaces.find(current_face) == new2old_meshFaces.end()) {
//                            std::vector<Node*> nodes = current_face->getNodes();
//                            std::vector<Node*> nodes_old(nodes.size(),NULL);
//                            for(unsigned int iNode=0; iNode<nodes.size(); iNode++) {
//                                nodes_old[iNode] = new2old_meshNodes[nodes[iNode]];
//                            }
//                            Face* face_old = this->getGMDSMesh().newFace(nodes_old);
//                            new2old_meshFaces[current_face] = face_old;
//                            old2new_meshFaces[face_old] = current_face;
//                            elems.push_back(face_old);
//                        } else {
//                            elems.push_back(new2old_meshFaces[current_face]);
//                        }
//                    }
//                }
//
//                std::vector<std::string> groupsName;
//                faces[iFace]->getCoFace(iCoFace)->getGroupsName(groupsName);
//
//                // ajout des polygones aux surfaces
//                for (size_t i=0; i<groupsName.size(); i++){
//                    std::string& nom = groupsName[i];
//
//                    try {
//                        getContext().getLocalMeshManager().getSurface(nom);
//                        command->addModifiedSurface(nom);
//                    } catch (...) {
//                        command->addNewSurface(nom);
//                    }
//                    // la surface de maillage que l'on vient de créer/modifier
//                    Mesh::Surface* sf = getContext().getLocalMeshManager().getSurface(nom);
//                    sf->addCoFace(faces[iFace]->getCoFace(iCoFace));
//                } // end for i<groupsName.size()
//            }
//
//        }
//
//    } // association des faces GMDS aux cofaces du bloc
//
//    // association des noeuds GMDS aux coEdges du bloc
//    {
//        gmds::Variable<gmds::geom::GeomEntity<gmds::TCoord>* >* curveClassification = mesh_tmp.getGeometricClassification(1);
//
//        std::vector<Topo::Edge* > topoEdges;
//        ABlock->getEdges(topoEdges);
//
//        for(unsigned int iTopoEdge=0; iTopoEdge<topoEdges.size(); iTopoEdge++) {
//            for(unsigned int iCoEdge=0; iCoEdge<topoEdges[iTopoEdge]->getNbCoEdges(); iCoEdge++) {
//
//            	std::list<gmds::Node*> nodesList;
//
//                std::vector<gmds::Node*>& elems = topoEdges[iTopoEdge]->getCoEdge(iCoEdge)->nodes();
//
//                Geom::Curve* curve_tmp = dynamic_cast<Geom::Curve*> (topoEdges[iTopoEdge]->getCoEdge(iCoEdge)->getGeomAssociation());
//                Mgx3D::Geom::GMDSGeomCurveAdapter* curve = dynamic_cast<Mgx3D::Geom::GMDSGeomCurveAdapter*> (model_bis.getGMDSCurve(curve_tmp));
//
//                gmds::Mesh<TMask>::edges_iterator it  = mesh_tmp.edges_begin();
//
//                for(;!it->isDone();it->next()) {
//                    gmds::Edge* current_edge = it->currentItem();
//
//                    if((*curveClassification)[current_edge->getID()] == curve) {
//
//                    	std::vector<Node*> nodes = current_edge->getNodes();
//                    	nodesList.push_back(new2old_meshNodes[nodes[0]]);
//                    	nodesList.push_back(new2old_meshNodes[nodes[1]]);
//                    }
//                }
//
//                nodesList.sort();
//                nodesList.unique();
//                elems.assign(nodesList.begin(),nodesList.end());
//
//                std::vector<std::string> groupsName;
//                topoEdges[iTopoEdge]->getCoEdge(iCoEdge)->getGroupsName(groupsName);
//
//                // ajout des polygones aux surfaces
//                for (size_t i=0; i<groupsName.size(); i++){
//                    std::string& nom = groupsName[i];
//
//                    try {
//                        getContext().getLocalMeshManager().getCloud(nom);
//                        command->addModifiedCloud(nom);
//                    } catch (...) {
//                        command->addNewCloud(nom);
//                    }
//                    // la surface de maillage que l'on vient de créer/modifier
//                    Mesh::Cloud* cl = getContext().getLocalMeshManager().getCloud(nom);
//                    cl->addCoEdge(topoEdges[iTopoEdge]->getCoEdge(iCoEdge));
//                } // end for i<groupsName.size()
//            }
//
//        }
//    } // association des noeuds GMDS aux coEdges du bloc
//
    return;
} // end meshInsertion
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
