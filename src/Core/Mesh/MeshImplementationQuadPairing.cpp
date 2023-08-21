/*
 * MeshImplementationQuadPairing.cpp
 *
 *  Created on: 8 juil. 2014
 *      Author: ledouxf
 */




/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/CommandCreateMesh.h"

#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/Edge.h"
#include "Topo/FaceMeshingPropertyQuadPairing.h"
#include "Utils/Common.h"

#include "Internal/Context.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/OCCGeomRepresentation.h"
/*----------------------------------------------------------------------------*/
/// OCC
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
/*----------------------------------------------------------------------------*/
/// GMDS
//#include<SurfaceQTRAN.h>
#include <GMDS/IG/IGMeshDoctor.h>
/*----------------------------------------------------------------------------*/
/// TkUtil
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
/// GMSH
#include "Context.h"
#include "Options.h"
#include "GModel.h"
#include "OCCVertex.h"
#include "OCCEdge.h"
#include "GModelIO_OCC.h"
#include "MVertex.h"
#include "MLine.h"
#include "meshGFace.h"
#include "meshGRegion.h"
#include "MTriangle.h"
#include "GmshMessage.h"
#include "MPoint.h"
#include "robustPredicates.h"


//#define _DEBUG_MESH
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
void MeshImplementation::meshQuadPairing(Mesh::CommandCreateMesh* command, Topo::CoFace* fa)
{
	MGX_NOT_YET_IMPLEMENTED("Appariement")

#ifdef _DEBUG_MESH
    std::cout <<"Maillage de la face commune "<<fa->getName()<<" avec la méthode de Quad Pairing"<<std::endl;
#endif

    gmds::IGMesh& gmds_mesh = getGMDSMesh();

    Topo::FaceMeshingPropertyQuadPairing* prop =
            dynamic_cast<Topo::FaceMeshingPropertyQuadPairing*>(fa->getCoFaceMeshingProperty());
    CHECK_NULL_PTR_ERROR(prop);

    // init des options GMSH
    InitOptions(0);

    // Initialize robust predicates
    robustPredicates::exactinit();

    // récupération des paramètre utilisateur
    CTX::instance()->mesh.lcMin = prop->getMin();
    CTX::instance()->mesh.lcMax = prop->getMax();

    //Geometrie de la coface
    Geom::GeomEntity*  geo_entity = fa->getGeomAssociation();
    if(geo_entity==0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "La face  "<< fa->getName()
                << " ne peut pas être maillée : absence de géométrie associée";
        throw TkUtil::Exception (message);
    }
    Geom::OCCGeomRepresentation* occ_rep =
     dynamic_cast<Geom::OCCGeomRepresentation*>(geo_entity->getComputationalProperty());

    if(occ_rep==0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "La face  "<< fa->getName()
                << " ne peut pas être maillée : absence de shape OCC";
        throw TkUtil::Exception (message);
    }
    TopoDS_Face face_shape = TopoDS::Face(occ_rep->getShape());

    TopTools_IndexedMapOfShape  mapEdgeOfFace;
    TopExp::MapShapes(face_shape,TopAbs_EDGE, mapEdgeOfFace);




    //============================================================
    //      CREATION DU MAILLAGE GMDS SUR LEQUEL ON FERA LE
    //      MAILLAGE QUAD
    //============================================================
    gmds::MeshModel mod_QTRAN =  gmds::DIM3|gmds::F|gmds::E|gmds::F2N|gmds::E2N|gmds::N2F|gmds::E2F|gmds::F2E;
    gmds::IGMesh quad_mesh(mod_QTRAN);

    //correspondance entre les différents maillages
    // Maillage M3D <---> Maillage local GMDS
    std::map<gmds::TCellID, gmds::TCellID> cor_M3DNode_gmdsNode;
    std::map<gmds::TCellID, gmds::TCellID> cor_gmdsNode_M3DNode;

    // Maillage local GMDS <---> Maillage gmsh
    std::map<gmds::TCellID, MVertex*> cor_M3DNode_gmshVertex;
    std::map<MVertex*, gmds::TCellID> cor_gmshVertex_M3DNode;


    std::map<std::pair<gmds::TCellID,gmds::TCellID>, gmds::TCellID > unused_M3DNode;


#ifdef _DEBUG_MESH
    std::cout <<"=== new GModel"<<std::endl;
#endif
    // construction du modèle GMSH à partir de cette shape OCC
    GModel* model = new GModel(std::string("GModel_")+fa->getName());
#ifdef _DEBUG_MESH
    std::cout <<"=== importOCCShape"<<std::endl;
#endif
    model->importOCCShape(&face_shape);

#ifdef _DEBUG_MESH
    std::cout <<"=== traitement des arêtes de la face "<<fa->getName()<<std::endl;
#endif
    // on recupere le maillage des aretes
    std::vector<Topo::Edge* > edges;
    fa->getEdges(edges);

    bool have_periodic_edge = false;

    std::map<Topo::Edge*, uint> filtre_coedges;

    for(unsigned int i=0;i<edges.size();i++){
        Topo::Edge* edge =edges[i];
#ifdef _DEBUG_MESH
        std::cout <<"--Traitement de l'arête "<<edge->getName()<<std::endl;
#endif
        std::vector<Topo::Vertex* > v_edge = edge->getVertices();

        // cas d'une arête déjà vue
        if (filtre_coedges[edge] == 1){
            have_periodic_edge = true;
            // on passe le traitement de l'arête, elle a déja été vue
            continue;
        }

        filtre_coedges[edge] += 1;

        if(v_edge.size()!=2){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "La face  "<< fa->getName()
                    << " ne peut pas être maillée :\n"
                    <<" une arete est incidente à "
                    <<v_edge.size()<<" sommets";
            throw TkUtil::Exception (message);
        }

        if (edge->getNbCoEdges() != 1){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        	message << "L'arête "<< edge->getName()
        	        << " n'est pas composée d'une unique arête commune, ce cas n'est pas prévu ";
        	throw TkUtil::Exception (message);
        }

        // récupération de la courbe associée
        Geom::GeomEntity* ge = edge->getCoEdge(0)->getGeomAssociation();
        if (ge == 0){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "L'arête "<< edge->getCoEdge(0)->getName()
                    << " n'est pas projetée, ce cas n'est pas prévu ";
            throw TkUtil::Exception (message);
        }
#ifdef _DEBUG_MESH
        std::cout <<"    associée à "<<ge->getName()<<std::endl;
#endif
        Geom::Curve* curve = dynamic_cast<Geom::Curve*>(ge);
        if (curve == 0){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "L'arête "<< edge->getName()
                    << " n'est pas projetée sur une courbe, ce qui n'est pas prévu";
            throw TkUtil::Exception (message);
        }

        Geom::OCCGeomRepresentation* crv_occ_rep =
                dynamic_cast<Geom::OCCGeomRepresentation*>(curve->getComputationalProperty());

        if(crv_occ_rep==0){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "La face  "
                    << fa->getName()
                    << " ne peut pas être maillée en triangles : absence de shape OCC pour la courbe "
                    <<curve->getName();
            throw TkUtil::Exception (message);
        }

        TopoDS_Edge tedge = TopoDS::Edge(crv_occ_rep->getShape());
        TopoDS_Edge edgeInFace;
        int         edgeIndexInFace=0;
        for(int j = 1; j <= mapEdgeOfFace.Extent(); j++){
            TopoDS_Edge ej  = TopoDS::Edge(mapEdgeOfFace(j));
            if(Geom::OCCGeomRepresentation::areEquals(tedge,ej))
            {
                edgeInFace=ej;
                edgeIndexInFace=j;
            }
        }

#ifdef _DEBUG_MESH
        std::cout<<"*************************************************"<<std::endl;
        std::cout<<"\t ==> Correspond à l'arête "<<edgeIndexInFace<<" dans la face"<<std::endl;
#endif
        BRepAdaptor_Curve curve_adaptor(edgeInFace);
        double firstInFace = curve_adaptor.FirstParameter();
        double lastInFace  = curve_adaptor.LastParameter();
        BRepAdaptor_Curve curve_adaptor2(tedge);
        double firstInM3D = curve_adaptor2.FirstParameter();
        double lastInM3D  = curve_adaptor2.LastParameter();
        bool mustReparamCurve= false;
        if(firstInFace!=firstInM3D)
            mustReparamCurve = true;

#ifdef _DEBUG_MESH
        std::cout<<"\t edge in OCC face -> ("<<firstInFace<<", "<<lastInFace<<")"<<std::endl;
        std::cout<<"\t edge in M3D model-> ("<<firstInM3D<<", "<<lastInM3D<<")"<<std::endl;
        std::cout<<"*************************************************"<<std::endl;
#endif
        // recherche de la gedge dans le model
        GEdge* gedge = model->getOCCInternals()->getOCCEdgeByNativePtr(model, tedge);

        if (gedge == 0){
            // recherche avec notre propre identification de la bonne gedge

            for (GModel::eiter iter = model->firstEdge(); iter != model->lastEdge(); ++iter){
                GEdge* loc_gedge = *iter;

                OCCEdge* loc_occedge = dynamic_cast<OCCEdge*>(loc_gedge);
                if (loc_occedge == 0){
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message << "Erreur interne, conversion en  OCCEdge impossible !";
                    throw TkUtil::Exception (message);
                }

                TopoDS_Edge loc_tedge = loc_occedge->getTopoDS_Edge();

                if (Geom::OCCGeomRepresentation::areEquals(loc_tedge, tedge))
                    gedge = loc_gedge;
            }
        }

        if (gedge == 0){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "Pb avec arête "
                    << edge->getName()
                    << ", on ne trouve pas de GEdge pour courbe "
                    <<curve->getName();
            throw TkUtil::Exception (message);
        }
        else {
#ifdef _DEBUG_MESH
            std::cout <<"    gedge trouvé... tag "<<gedge->tag()<<std::endl;
#endif

            // association entre GMSH::GVertex et GMDS::Node
            GVertex *gv1 = gedge->getBeginVertex();
            uint ind1 = _addGMDSVertex2GVertex(edge, gv1, cor_M3DNode_gmshVertex, cor_gmshVertex_M3DNode);
            {
                MVertex* created_gmsh_mesh_vertex =   gv1->getMeshVertex(0);
                gmds::TCellID m3D_node_id = cor_gmshVertex_M3DNode[created_gmsh_mesh_vertex];
                gmds::Node created_quad_node = quad_mesh.newNode(created_gmsh_mesh_vertex->x(),
                        created_gmsh_mesh_vertex->y(),
                        created_gmsh_mesh_vertex->z());
                cor_M3DNode_gmdsNode[m3D_node_id] = created_quad_node.getID();
                cor_gmdsNode_M3DNode[created_quad_node.getID()] = m3D_node_id;
#ifdef _DEBUG_MESH
            std::cout <<"   newNode au bord 1 ("<<created_quad_node.getID()<<"): "
            		<<created_quad_node.X()<<", "<<created_quad_node.Y()<<", "<<created_quad_node.Z()<<std::endl;
#endif
            }
            if (!gedge->periodic(0)){
                GVertex *gv2 = gedge->getEndVertex();
                _addGMDSVertex2GVertex(edge, gv2, cor_M3DNode_gmshVertex, cor_gmshVertex_M3DNode);
                {
                    MVertex* created_gmsh_mesh_vertex =   gv2->getMeshVertex(0);
                    gmds::TCellID m3D_node_id = cor_gmshVertex_M3DNode[created_gmsh_mesh_vertex];
                    gmds::Node created_quad_node = quad_mesh.newNode(created_gmsh_mesh_vertex->x(),
                            created_gmsh_mesh_vertex->y(),
                            created_gmsh_mesh_vertex->z());
                    cor_M3DNode_gmdsNode[m3D_node_id] = created_quad_node.getID();
                    cor_gmdsNode_M3DNode[created_quad_node.getID()] = m3D_node_id;
#ifdef _DEBUG_MESH
                    std::cout <<"   newNode au bord 2 ("<<created_quad_node.getID()<<"): "
                    		<<created_quad_node.X()<<", "<<created_quad_node.Y()<<", "<<created_quad_node.Z()<<std::endl;
#endif

                }
            }
            else {
#ifdef _DEBUG_MESH
                std::cout <<"  gedge périodique"<<std::endl;
#endif
            }
            std::vector<MVertex*> &mesh_vertices = gedge->mesh_vertices ;
            std::vector<MLine*> &lines = gedge->lines ;

            std::vector<gmds::Node> edge_nodes;
            if (ind1 == 0)
                edge->getNodes(v_edge[0], v_edge[1], edge_nodes);
            else{
                edge->getNodes(v_edge[1], v_edge[0], edge_nodes);

            }

#ifdef _DEBUG_MESH
            std::cout <<"    nombre de noeuds: "<<edge_nodes.size()<<std::endl;
            std::cout <<"    gedge->getBeginVertex() en "
                    <<gedge->getBeginVertex()->x()<<", "
                    <<gedge->getBeginVertex()->y()<<", "
                    <<gedge->getBeginVertex()->z()<<std::endl;
            std::cout <<"    gedge->getEndVertex() en "
                    <<gedge->getEndVertex()->x()<<", "
                    <<gedge->getEndVertex()->y()<<", "
                    <<gedge->getEndVertex()->z()<<std::endl;
            std::cout <<"    v_edge[0] en "
                    <<v_edge[0]->getX()<<", "
                    <<v_edge[0]->getY()<<", "
                    <<v_edge[0]->getZ()<<std::endl;
            std::cout <<"    v_edge[1] en "
                    <<v_edge[1]->getX()<<", "
                    <<v_edge[1]->getY()<<", "
                    <<v_edge[1]->getZ()<<std::endl;
#endif

            //=======================================================
            //CREATION DES NOEUDS DE MAILLAGE
            //=======================================================

            for (uint j=1; j<edge_nodes.size()-1; j++){
                //MVertex* mvtx = cor_gmdsNode_gmshVertex[edge_nodes[j]];
                //if (mvtx == 0){



                //Noeud du maillage initial
                gmds::Node nd = edge_nodes[j];

                Utils::Math::Point pt = getCoordNode(nd);

                double param = 0.0;
                //                    if(mustReparamCurve){
                // on doit utiliser une paramétrisation
                // propre à la courbe locale à la face et non au
                // modèle M3D
                Geom::OCCGeomRepresentation::getParameterOnTopoDSEdge(
                        edgeInFace, pt,param);
                //                    }
                //                    else
                //                        crv_occ_rep->getParameter(pt, param, curve);
                //Creation du noeud gmsh
                if(j%2==0){
                    MVertex* mvtx = new MEdgeVertex(pt.getX(), pt.getY(),pt.getZ(), gedge, param);
                    cor_M3DNode_gmshVertex[nd.getID()] = mvtx;
                    cor_gmshVertex_M3DNode[mvtx] = nd.getID();

                    mesh_vertices.push_back(mvtx);
                    //Creation du noeud gmds pour le maillage quad
                    gmds::Node nd_quad = quad_mesh.newNode(nd.X(),nd.Y(),nd.Z());

                    cor_gmdsNode_M3DNode[nd_quad.getID()] = nd.getID();
                    cor_M3DNode_gmdsNode[nd.getID()     ] = nd_quad.getID();
#ifdef _DEBUG_MESH
                    std::cout <<"   newNode aux centres des bords ("<<nd_quad.getID()<<"): "
                    		<<nd_quad.X()<<", "<<nd_quad.Y()<<", "<<nd_quad.Z()<<std::endl;
#endif

                }
                else
                {
                    gmds::TCellID  nd1 = edge_nodes[j-1].getID();
                    gmds::TCellID  nd2 = edge_nodes[j+1].getID();
                    unused_M3DNode[std::pair<gmds::TCellID,gmds::TCellID>(nd1,nd2)] =nd.getID() ;
                }

            }

            //CREATION DES ARETES DE MAILLAGE POUR GMSH

            for (uint j=0; j<edge_nodes.size()-1; j++){

                if(j%2==0)
                {
                    MVertex* mvtx1 = cor_M3DNode_gmshVertex[edge_nodes[j].getID()];
                    MVertex* mvtx2 = cor_M3DNode_gmshVertex[edge_nodes[j+2].getID()];
                    lines.push_back(new MLine(mvtx1, mvtx2));
                }
            }
#ifdef _DEBUG_MESH
            gedge->meshStatistics.status = GEdge::DONE;
            std::cout<<"========================================="<<std::endl;
            std::cout<<"========================================="<<std::endl;
            for(unsigned int k=0; k<lines.size();k++){
                MLine* l = lines[k];
                MVertex * v0 = l->getVertex(0);
                MVertex * v1 = l->getVertex(1);
                std::cout<<"Edge "<<k<<": "<<v0->getNum()<<", "<<v1->getNum()<<" de "
                        <<"("<<v0->x()<<", "<<v0->y()<<", "<<v0->z()<<")"
                        <<"("<<v1->x()<<", "<<v1->y()<<", "<<v1->z()<<") - ";

                if(dynamic_cast<MEdgeVertex*>(v0)!=0){
                    double p;
                    dynamic_cast<MEdgeVertex*>(v0)->getParameter(0,p);
                    std::cout<<"curve param 0 :"<<p<<" - ";
                }

                if(dynamic_cast<MEdgeVertex*>(v1)!=0){
                    double p;
                    dynamic_cast<MEdgeVertex*>(v1)->getParameter(0,p);
                    std::cout<<"curve param 1 :"<<p;
                }

                std::cout<<std::endl;
            }
            std::cout<<"========================================="<<std::endl;
            std::cout<<"========================================="<<std::endl;
#endif


        }
    } // end for(unsigned int i=0;i<edges.size();i++)






    // appel à GMSH pour mailler la surface. Les triangles créés rempliront directement le
    // maillage quad_mesh.

    std::map<MVertex*,gmds::TCellID> created_nodes;
    for(GModel::fiter it = model->firstFace(); it != model->lastFace(); ++it){

        if (have_periodic_edge){
#ifdef _DEBUG_MESH
            std::cout<<"appel à meshGeneratorPeriodic"<<std::endl;
#endif
            meshGeneratorPeriodic(*it, // la GFace
                    false); // debug
        }
        else {
#ifdef _DEBUG_MESH
            std::cout<<"appel à meshGenerator"<<std::endl;
#endif
            meshGenerator(*it, // la GFace
                    0,     // RECUR_ITER
                    false, // repairSelfIntersecting1dMesh
                    false, // onlyInitialMesh
                    false, // debug
                    0);    // replacement_edges (list)
        }

        if (Msg::GetErrorCount()){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "GMSH sort en erreur (ErrorCount : "
                    << (short)Msg::GetErrorCount()
                    << ") pour la face commune topologique "
                    << fa->getName();
            throw TkUtil::Exception (message);
        }

        // récupération du maillage pour le passer de gmsh à gmds
        std::vector<MTriangle*>& triangles = (*it)->triangles;

#ifdef _DEBUG_MESH
        std::cout<<"###################################################"<<std::endl;
        std::cout<<"triangles.size() = "<<triangles.size()<<std::endl;
#endif

        for (uint i=0; i<triangles.size(); i++){
            MTriangle* tri = triangles[i];
            std::vector<gmds::TCellID> node_ids;
            for (uint j=0; j<3; j++){
                MVertex* mvtx = tri->getVertex(j);
//                std::cout<<"Creation of ("<<mvtx->x()<<", "<<mvtx->y()<<", "<<mvtx->z()<<")"<<std::endl;

                bool not_init = cor_gmshVertex_M3DNode.find(mvtx)==cor_gmshVertex_M3DNode.end();
                bool not_added= created_nodes.find(mvtx)==created_nodes.end();
                if(not_init && not_added)
                {
                    gmds::Node nd = quad_mesh.newNode(mvtx->x(), mvtx->y(), mvtx->z());
                    node_ids.push_back(nd.getID());

                    created_nodes[mvtx]=nd.getID();
#ifdef _DEBUG_MESH
                    std::cout <<"   newNode au centre ("<<nd.getID()<<"): "
                    		<<nd.X()<<", "<<nd.Y()<<", "<<nd.Z()<<std::endl;
#endif

                }
                else if(not_init)
                    node_ids.push_back(created_nodes[mvtx]);
                else//init
                    node_ids.push_back(cor_M3DNode_gmdsNode[cor_gmshVertex_M3DNode[mvtx]]);
            }
            gmds::TCellID n1 = node_ids[0];
            gmds::TCellID n2 = node_ids[1];
            gmds::TCellID n3 = node_ids[2];
#ifdef _DEBUG_MESH
            std::cout<<" triangle: "<<std::endl;
            gmds::Node node1 = gmds_mesh.get<gmds::Node>(n1);
            gmds::Node node2 = gmds_mesh.get<gmds::Node>(n2);
            gmds::Node node3 = gmds_mesh.get<gmds::Node>(n3);
            std::cout<<"  node 1 ("<<n1<<"): "<<node1.X()<<", "<<node1.Y()<<", "<<node1.Z()<<std::endl;
            std::cout<<"  node 2 ("<<n2<<"): "<<node2.X()<<", "<<node2.Y()<<", "<<node2.Z()<<std::endl;
            std::cout<<"  node 3 ("<<n3<<"): "<<node3.X()<<", "<<node3.Y()<<", "<<node3.Z()<<std::endl;
#endif

            gmds::Face t1 = quad_mesh.newTriangle(n1,n2,n3);
        }
    }

    //==============================================================================
    //          CREATION DU MAILLAGE QUAD A PARTIR DU MAILLAGE TRI
    //==============================================================================
#ifdef _DEBUG_MESH
    std::cout<<"triangles -> quadrangles "<<std::endl;
#endif

    gmds::IGMeshDoctor doc(&quad_mesh);
    doc.buildEdgesAndX2E();
    doc.updateUpwardConnectivity();

//    quadSurf::SurfaceQTRAN qtran(quad_mesh);
//    qtran.perform();

    doc.buildN2F(mod_QTRAN);

    //==============================================================================
    //          MISE A JOUR DU MAILLAGE M3D
    //==============================================================================
    std::vector<gmds::TCellID>& fa_node_ids = fa->nodes();
    std::vector<gmds::TCellID>& fa_face_ids = fa->faces();

    gmds::IGMesh::face_iterator it_face = quad_mesh.faces_begin();
    for(;!it_face.isDone();it_face.next()){
        gmds::Face current_face = it_face.value();
        std::vector<gmds::Node> current_nodes = current_face.getAll<gmds::Node>();
        std::vector<gmds::TCellID> m3D_nodesID;

        for (unsigned int i_node = 0;i_node<current_nodes.size();i_node++)
        {
            gmds::Node node_i = current_nodes[i_node];

            if(cor_gmdsNode_M3DNode.find(node_i.getID())==cor_gmdsNode_M3DNode.end())
            {

                //il est possible que le nouveau noeud correspondent a un noeud au bord non utilise
                //dans ce cas, deux voisins sont des noeuds en correspondance avec ceux de M3D
                std::vector<gmds::Face> adj_faces = node_i.get<gmds::Face>();
                if(adj_faces.size()==2){
                    //au bord, le pattern de découpage impose que l'on ait deux faces adj. au noeud
                    //qui nous intéresse
                    gmds::Face f0 = adj_faces[0];
                    gmds::Face f1 = adj_faces[1];
                    gmds::Node n0_0, n0_1, n1_0, n1_1;
                    f0.getAdjacentNodes(node_i,n0_0,n0_1);
                    f1.getAdjacentNodes(node_i,n1_0,n1_1);
                    gmds::Node m3dNode_f0;
                    gmds::Node m3dNode_f1;
                    bool stop_look=false;
                    if(cor_gmdsNode_M3DNode.find(n0_0.getID())!=cor_gmdsNode_M3DNode.end())
                        m3dNode_f0 = getGMDSMesh().get<gmds::Node>(cor_gmdsNode_M3DNode[n0_0.getID()]);
                    else if(cor_gmdsNode_M3DNode.find(n0_1.getID())!=cor_gmdsNode_M3DNode.end())
                        m3dNode_f0 = getGMDSMesh().get<gmds::Node>(cor_gmdsNode_M3DNode[n0_1.getID()]);
                    else
                        stop_look = true;
                    if(cor_gmdsNode_M3DNode.find(n1_0.getID())!=cor_gmdsNode_M3DNode.end())
                        m3dNode_f1 = getGMDSMesh().get<gmds::Node>(cor_gmdsNode_M3DNode[n1_0.getID()]);
                    else if(cor_gmdsNode_M3DNode.find(n1_1.getID())!=cor_gmdsNode_M3DNode.end())
                        m3dNode_f1 = getGMDSMesh().get<gmds::Node>(cor_gmdsNode_M3DNode[n1_1.getID()]);
                    else
                        stop_look = true;

                    if(!stop_look)
                    {
                        std::pair<gmds::TCellID,gmds::TCellID> p1, p2;
                        p1.first = m3dNode_f0.getID(); p1.second= m3dNode_f1.getID();
                        p2.first = m3dNode_f1.getID(); p2.second= m3dNode_f0.getID();
                        if(unused_M3DNode.find(p1)==unused_M3DNode.end()){
                            gmds::TCellID m3d_node = unused_M3DNode[p2];
                            m3D_nodesID.push_back(m3d_node);
                        }
                        else{
                            gmds::TCellID m3d_node = unused_M3DNode[p1];
                            m3D_nodesID.push_back(m3d_node);

                        }
                    }
                }
                else{
                    //new node in M3D
                    Utils::Math::Point pnt(node_i.X(), node_i.Y(), node_i.Z());
                    geo_entity->project(pnt);
                    gmds::Node nd = getGMDSMesh().newNode(pnt.getX(),pnt.getY(),pnt.getZ());
                    fa_node_ids.push_back(nd.getID());
                    command->addCreatedNode(nd.getID());
                    m3D_nodesID.push_back(nd.getID());
#ifdef _DEBUG_MESH
                    std::cout <<"   newNode in M3D ("<<nd.getID()<<"): "
                    		<<nd.X()<<", "<<nd.Y()<<", "<<nd.Z()<<std::endl;
#endif
                }
            }
            else //existing node in M3D
                m3D_nodesID.push_back(cor_gmdsNode_M3DNode[node_i.getID()]);
        }

        if(m3D_nodesID.size()==3)
        {
            gmds::Face t1 = getGMDSMesh().newTriangle(m3D_nodesID[0],m3D_nodesID[1],m3D_nodesID[2]);
            fa_face_ids.push_back(t1.getID());
            command->addCreatedFace(t1.getID());
#ifdef _DEBUG_MESH
            std::cout<<" triangle: "<<std::endl;
            gmds::Node node1 = gmds_mesh.get<gmds::Node>(m3D_nodesID[0]);
            gmds::Node node2 = gmds_mesh.get<gmds::Node>(m3D_nodesID[1]);
            gmds::Node node3 = gmds_mesh.get<gmds::Node>(m3D_nodesID[2]);
            std::cout<<"  node 1 ("<<m3D_nodesID[0]<<"): "<<node1.X()<<", "<<node1.Y()<<", "<<node1.Z()<<std::endl;
            std::cout<<"  node 2 ("<<m3D_nodesID[1]<<"): "<<node2.X()<<", "<<node2.Y()<<", "<<node2.Z()<<std::endl;
            std::cout<<"  node 3 ("<<m3D_nodesID[2]<<"): "<<node3.X()<<", "<<node3.Y()<<", "<<node3.Z()<<std::endl;
#endif
        }
        else if(m3D_nodesID.size()==4)
        {
            gmds::Face q1 = getGMDSMesh().newQuad(m3D_nodesID[0],m3D_nodesID[1],m3D_nodesID[2],m3D_nodesID[3]);
            fa_face_ids.push_back(q1.getID());
            command->addCreatedFace(q1.getID());
#ifdef _DEBUG_MESH
            std::cout<<" quadrangle: "<<std::endl;
            gmds::Node node1 = gmds_mesh.get<gmds::Node>(m3D_nodesID[0]);
            gmds::Node node2 = gmds_mesh.get<gmds::Node>(m3D_nodesID[1]);
            gmds::Node node3 = gmds_mesh.get<gmds::Node>(m3D_nodesID[2]);
            gmds::Node node4 = gmds_mesh.get<gmds::Node>(m3D_nodesID[3]);
            std::cout<<"  node 1 ("<<m3D_nodesID[0]<<"): "<<node1.X()<<", "<<node1.Y()<<", "<<node1.Z()<<std::endl;
            std::cout<<"  node 2 ("<<m3D_nodesID[1]<<"): "<<node2.X()<<", "<<node2.Y()<<", "<<node2.Z()<<std::endl;
            std::cout<<"  node 3 ("<<m3D_nodesID[2]<<"): "<<node3.X()<<", "<<node3.Y()<<", "<<node3.Z()<<std::endl;
            std::cout<<"  node 4 ("<<m3D_nodesID[3]<<"): "<<node4.X()<<", "<<node4.Y()<<", "<<node4.Z()<<std::endl;
#endif
        }
    }

    // nettoyage mémoire de ce qui a servi à GMSH
    delete model;

    std::vector<std::string> groupsName;
    fa->getGroupsName(groupsName);

#ifdef _DEBUG_GROUP_BY_TOPO_ENTITY
    // on ajoute un groupe pour distinguer les faces en mode debug
    groupsName.push_back(fa->getName());
#endif


    // ajout des polygones aux surfaces
    for (size_t i=0; i<groupsName.size(); i++){
        std::string& nom = groupsName[i];

        try {
            getContext().getLocalMeshManager().getSurface(nom);
            command->addModifiedSurface(nom);
        } catch (...) {
            command->addNewSurface(nom);
        }
        // la surface de maillage que l'on vient de créer/modifier
        Mesh::Surface* sf = getContext().getLocalMeshManager().getSurface(nom);
        sf->saveMeshSurfaceTopoProperty(&command->getInfoCommand());
        sf->addCoFace(fa);
    } // end for i<groupsName.size()


#ifdef _DEBUG_MESH
  //  writeMli("/tmp/brieree/toto.mli");
#endif

} // end meshDelaunayGMSH
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
