/*----------------------------------------------------------------------------*/
/*
 * \file MeshImplementationGMSH.cpp
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
#include "Topo/FaceMeshingPropertyDelaunayGMSH.h"
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
void MeshImplementation::meshDelaunayGMSH(Mesh::CommandCreateMesh* command, Topo::CoFace* fa)
{
#ifdef _DEBUG_MESH
    std::cout <<"Maillage de la face commune "<<fa->getName()<<" avec la méthode de Delaunay (version GMSH)"<<std::endl;
#endif

    gmds::IGMesh& gmds_mesh = getGMDSMesh();
    Msg::ResetErrorCounter();

    Topo::FaceMeshingPropertyDelaunayGMSH* prop =
            dynamic_cast<Topo::FaceMeshingPropertyDelaunayGMSH*>(fa->getCoFaceMeshingProperty());
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
                << " ne peut pas être maillée en triangles : absence de géométrie associée";
        throw TkUtil::Exception (message);
    }
    Geom::OCCGeomRepresentation* occ_rep =
     dynamic_cast<Geom::OCCGeomRepresentation*>(geo_entity->getComputationalProperty());

    if(occ_rep==0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "La face  "<< fa->getName()
                << " ne peut pas être maillée en triangles : absence de shape OCC";
        throw TkUtil::Exception (message);
    }
    TopoDS_Face face_shape = TopoDS::Face(occ_rep->getShape());

    TopTools_IndexedMapOfShape  mapEdgeOfFace;
    TopExp::MapShapes(face_shape,TopAbs_EDGE, mapEdgeOfFace);

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

    // correspondance entre noeuds GMDS et GMSH pour éviter les doublons
    std::map<gmds::TCellID, MVertex*> cor_gmdsNode_gmshVertex;
    std::map<MVertex*, gmds::TCellID> cor_gmshVertex_gmdsNode;
    bool have_periodic_edge = false;

    std::map<Topo::Edge*, uint> filtre_edges;

    for(unsigned int i=0;i<edges.size();i++){
        Topo::Edge* edge =edges[i];
#ifdef _DEBUG_MESH
        std::cout <<"--Traitement de l'arête "<<edge->getName()<<std::endl;
#endif
        std::vector<Topo::Vertex* > v_edge = edge->getVertices();

        // cas d'une arête déjà vue
        if (filtre_edges[edge] == 1){
            have_periodic_edge = true;
            // on passe le traitement de l'arête, elle a déja été vue
            continue;
        }

        filtre_edges[edge] += 1;

        if(v_edge.size()!=2){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "La face  "<< fa->getName()
                    << " ne peut pas être maillée en triangles :\n"
                    <<" une arete est incidente à "
                    <<v_edge.size()<<" sommets";
            throw TkUtil::Exception (message);
        }

        // récupération de la courbe associée
        Geom::GeomEntity* ge = edge->getCoEdge(0)->getGeomAssociation();
        if (ge == 0){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "L'arête "<< edge->getName()
                    << " n'est pas projetée, ce cas n'est pas prévu pour le cas GMSH ";
            throw TkUtil::Exception (message);
        }
#ifdef _DEBUG_MESH
        std::cout <<"    associée à "<<ge->getName()<<" avec "
        		<<ge->getRefTopo().size()<<" entités topologiques en relation"<<std::endl;
#endif
        Geom::Curve* curve = dynamic_cast<Geom::Curve*>(ge);
        if (curve == 0){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "L'arête "<< edge->getName()
                    << " n'est pas projetée sur une courbe, ce qui n'est pas prévu pour le cas GMSH ";
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
            uint ind1 = _addGMDSVertex2GVertex(edge, gv1, cor_gmdsNode_gmshVertex, cor_gmshVertex_gmdsNode);
            if (!gedge->periodic(0)){
                GVertex *gv2 = gedge->getEndVertex();
                _addGMDSVertex2GVertex(edge, gv2, cor_gmdsNode_gmshVertex, cor_gmshVertex_gmdsNode);
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


            for (uint j=1; j<edge_nodes.size()-1; j++){
                //MVertex* mvtx = cor_gmdsNode_gmshVertex[edge_nodes[j]];
                //if (mvtx == 0){
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

                    MVertex* mvtx = new MEdgeVertex(pt.getX(), pt.getY(),pt.getZ(), gedge, param);

                    cor_gmdsNode_gmshVertex[nd.getID()] = mvtx;
                    cor_gmshVertex_gmdsNode[mvtx] = nd.getID();

                    mesh_vertices.push_back(mvtx);
                //}
            }

            for (uint j=0; j<edge_nodes.size()-1; j++){
                MVertex* mvtx1 = cor_gmdsNode_gmshVertex[edge_nodes[j].getID()];
                MVertex* mvtx2 = cor_gmdsNode_gmshVertex[edge_nodes[j+1].getID()];
                lines.push_back(new MLine(mvtx1, mvtx2));
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


    // appel à GMSH pour mailler la surface
    for(GModel::fiter it = model->firstFace(); it != model->lastFace(); ++it)
    {
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
        std::cout<<"###################################################"<<std::endl;
        std::cout<<"triangles.size() = "<<triangles.size()<<std::endl;
#endif
        std::vector<gmds::TCellID>& fa_node_ids = fa->nodes();
        std::vector<gmds::TCellID>& fa_face_ids = fa->faces();
        for (uint i=0; i<triangles.size(); i++){
            MTriangle* tri = triangles[i];
            for (uint j=0; j<3; j++){
                MVertex* mvtx = tri->getVertex(j);
#ifdef _DEBUG_MESH
              std::cout<<"Creation of ("<<mvtx->x()<<", "<<mvtx->y()<<", "<<mvtx->z()<<")"<<std::endl;
#endif
                gmds::Node nd;

                if(cor_gmshVertex_gmdsNode.find(mvtx)==cor_gmshVertex_gmdsNode.end())
                {
                    nd = getGMDSMesh().newNode(mvtx->x(), mvtx->y(), mvtx->z());
                    fa_node_ids.push_back(nd.getID());
                    command->addCreatedNode(nd.getID());
                    cor_gmshVertex_gmdsNode[mvtx] = nd.getID();
                }
                else
                    nd = gmds_mesh.get<gmds::Node>(cor_gmshVertex_gmdsNode[mvtx]);
            }
            gmds::TCellID n1 = cor_gmshVertex_gmdsNode[tri->getVertex(0)];
            gmds::TCellID n2 = cor_gmshVertex_gmdsNode[tri->getVertex(1)];
            gmds::TCellID n3 = cor_gmshVertex_gmdsNode[tri->getVertex(2)];
#ifdef _DEBUG_MESH
            std::cout<<" triangle: "<<std::endl;
            gmds::Node node1 = gmds_mesh.get<gmds::Node>(n1);
            gmds::Node node2 = gmds_mesh.get<gmds::Node>(n2);
            gmds::Node node3 = gmds_mesh.get<gmds::Node>(n3);
            std::cout<<"  node 1 : "<<node1.X()<<", "<<node1.Y()<<", "<<node1.Z()<<std::endl;
            std::cout<<"  node 2 : "<<node2.X()<<", "<<node2.Y()<<", "<<node2.Z()<<std::endl;
            std::cout<<"  node 3 : "<<node3.X()<<", "<<node3.Y()<<", "<<node3.Z()<<std::endl;
#endif

            gmds::Face t1 = getGMDSMesh().newTriangle(n1,n2,n3);
            fa_face_ids.push_back(t1.getID());
            command->addCreatedFace(t1.getID());
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
uint MeshImplementation::_addGMDSVertex2GVertex(Topo::Edge* edge,
        GVertex* gv1,
        std::map<gmds::TCellID, MVertex*>& cor_gmdsNode_gmshVertex,
        std::map<MVertex*, gmds::TCellID>& cor_gmshVertex_gmdsNode)
{
	// on cherche le sommet magix correspondant
    std::vector<Topo::Vertex*> vertices;
    edge->getVertices(vertices);
    Topo::Vertex* vertex = 0;
    uint ind = 0;
    uint ind_vtx = 0;
    //coordonnées du point recherché
    Utils::Math::Point p1(gv1->point().x(), gv1->point().y(), gv1->point().z());
    //std::cout<<setprecision(14)<<"MeshImplementation::_addGMDSVertex2GVertex, p1 = "<<p1<<std::endl;

    //std::cout<<"MeshImplementation::_addGMDSVertex2GVertex pour edge "<<edge->getName()<<"  avec "<<vertices.size()<<" sommets"<<std::endl;
    for (std::vector<Topo::Vertex*>::const_iterator iter = vertices.begin();
            iter != vertices.end(); ++iter, ++ind){
        //pb de epsilon ??
        Utils::Math::Point currentPnt = (*iter)->getCoord();
        double dist = currentPnt.length(p1);
        //std::cout<<setprecision(14)<<" currentPnt = "<<currentPnt<<", dist = "<<dist<<std::endl;
        double precision = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon;
        if ((*iter)->getGeomAssociation())
        	precision = (*iter)->getGeomAssociation()->getComputationalProperty()->getPrecision();
        if ( dist <= precision ){
            vertex = *iter;
            ind_vtx = ind;
        }
    }
    if (vertex == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "Pb avec arête "<< edge->getName()
                << ", on ne trouve pas de sommet correspondant au point géométrique ("
                <<gv1->point().x()<<", "<<gv1->point().y()<<", "<<gv1->point().z()<<")";
        throw TkUtil::Exception (message);
    }
    else {
        std::vector<gmds::Node> edge_nodes;
        gmds::TCellID nd = vertex->getNode();
        MVertex* mvtx = cor_gmdsNode_gmshVertex[nd];
        if (mvtx == 0){
            //Utils::Math::Point pt = getCoordNode(nd);
            gmds::Node node = getGMDSMesh().get<gmds::Node>(nd);
            mvtx = new MVertex( node.X(), node.Y(), node.Z(), gv1, 0);

            cor_gmdsNode_gmshVertex[nd] = mvtx;
            cor_gmshVertex_gmdsNode[mvtx] = nd;

            gv1->addMeshVertex(mvtx);
            gv1->points.push_back(new MPoint(mvtx));
        }
    }
    return ind_vtx;
} // end _addGMDSVertex2GVertex
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
