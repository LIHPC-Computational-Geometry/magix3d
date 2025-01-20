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
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Curve.hxx>
/*----------------------------------------------------------------------------*/
/// TkUtil
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MESH

/// GMSH
#ifdef USE_GMSH
#include "gmsh.h"
#endif

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
void MeshImplementation::meshDelaunayGMSH(Mesh::CommandCreateMesh* command, Topo::CoFace* fa)
{
#ifndef USE_GMSH
	std::cerr<<"meshDelaunayGMSH not available"<<std::endl;
	return;
#else
#ifdef _DEBUG_MESH
    std::cout <<"Maillage de la face commune "<<fa->getName()<<" avec la méthode de Delaunay (version GMSH)"<<std::endl;
#endif

    // init des options GMSH
    gmsh::initialize();
    gmsh::logger::start();

    Topo::FaceMeshingPropertyDelaunayGMSH* prop =
            dynamic_cast<Topo::FaceMeshingPropertyDelaunayGMSH*>(fa->getCoFaceMeshingProperty());
    CHECK_NULL_PTR_ERROR(prop);

    // récupération des paramètre utilisateur
    gmsh::option::setNumber("Mesh.MeshSizeMin", prop->getMin());
    gmsh::option::setNumber("Mesh.MeshSizeMax", prop->getMax());
    gmsh::option::setNumber("Mesh.Algorithm", 6); // algo Frontal-Delaunay

    // Initialize robust predicates
    //robustPredicates::exactinit();

    //Geometrie de la coface
    Geom::GeomEntity* geo_entity = fa->getGeomAssociation();
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

#ifdef _DEBUG_MESH
    std::cout <<"=== new gmsh model"<<std::endl;
#endif
    // construction du modèle GMSH à partir de cette shape OCC
    gmsh::model::add(std::string("GModel_")+fa->getName());
    
#ifdef _DEBUG_MESH
    std::cout <<"=== importOCCShape"<<std::endl;
#endif
    gmsh::vectorpair gmsh_out_dim_tags;
    gmsh::model::occ::importShapesNativePointer(&face_shape, gmsh_out_dim_tags);

#ifdef _DEBUG_MESH
    std::cout <<"=== traitement des arêtes de la face "<<fa->getName()<<std::endl;
#endif
    // on recupere le maillage des aretes
    std::vector<Topo::Edge* > edges;
    fa->getEdges(edges);

    // Set qui contient tous les noeuds gmds
    std::set<gmds::TCellID> gmds_node_ids;

    gmsh::vectorpair gmsh_point_dim_tags;

    for(unsigned int i=0;i<edges.size();i++){
        Topo::Edge* edge = edges[i];
#ifdef _DEBUG_MESH
        std::cout <<"--Traitement de l'arête "<<edge->getName()<<std::endl;
#endif
        std::vector<Topo::Vertex* > vertices = edge->getVertices();

        if(vertices.size()!=2){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "La face  "<< fa->getName()
                    << " ne peut pas être maillée en triangles :\n"
                    <<" une arete est incidente à "
                    <<vertices.size()<<" sommets";
            throw TkUtil::Exception (message);
        }
 
        std::vector<gmds::Node> gmds_edge_nodes;
        edge->getNodes(vertices[0], vertices[1], gmds_edge_nodes);

       // On stocke la liste des noeuds gmds rencontrés dans l'algorithme
       // afin de ne pas les recréer lors de la récupération des triangles
       if (gmds_edge_nodes.size() > 0)
       {
            gmds_node_ids.insert(gmds_edge_nodes[0].id());
            gmds_node_ids.insert(gmds_edge_nodes[gmds_edge_nodes.size() - 1].id());
       }

        // Création des noeuds internes de l'arête dans gmsh selon la discrétisation choisie
        for (uint j=1; j<gmds_edge_nodes.size()-1; j++)
        {
            gmds::Node gmds_node = gmds_edge_nodes[j];
            gmds_node_ids.insert(gmds_node.id());
            Utils::Math::Point pt = getCoordNode(gmds_node);
            int gmsh_point_tag = gmsh::model::occ::addPoint(pt.getX(), pt.getY(), pt.getZ());
            gmsh_point_dim_tags.push_back({0, gmsh_point_tag});
        }
    } // end for(unsigned int i=0;i<edges.size();i++)

    // La fragmentation permet d'ajouter les points de contraintes sur les arêtes tels que définis par la discrétisation 
    // et récupérés via GMDS
    gmsh::vectorpair gmsh_entities_dim_tags;
    gmsh::model::occ::getEntities(gmsh_entities_dim_tags);
    gmsh_out_dim_tags.clear();
    std::vector<gmsh::vectorpair> gmsh_out_dim_tags_map;
    gmsh::model::occ::fragment(gmsh_point_dim_tags, gmsh_entities_dim_tags, gmsh_out_dim_tags, gmsh_out_dim_tags_map);

    gmsh::model::occ::synchronize();

    // On précise à GMSH qu'on veut seulement deux noeuds de maillage sur chaque Line de bord (pour éviter qu'il en ajoute)
    // L'opération fragment a changé les tags des entités
    gmsh_entities_dim_tags.clear();
    gmsh::model::occ::getEntities(gmsh_entities_dim_tags, 1);
    for (auto gmsh_dim_tag : gmsh_entities_dim_tags)
    {
        int gmsh_line_tag = gmsh_dim_tag.second;
        gmsh::model::mesh::setTransfiniteCurve(gmsh_line_tag, 2);
    }

    // génération du maillage
    gmsh::model::mesh::generate(2);
    std::string error;
    gmsh::logger::getLastError(error);
    if (!error.empty()){
        TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "GMSH sort en erreur (lastError : "
                << error 
                << ") pour la face commune topologique "
                << fa->getName();
        throw TkUtil::Exception (message);
    }

    // récupération du maillage pour le passer de gmsh à gmds

    // Récupérer les éléments de la dimension 2 (triangles, quadrangles, etc.)
    std::vector<int> gmsh_elt_types;                      // Types des éléments
    std::vector<std::vector<std::size_t>> gmsh_elt_tags;  // Tags des éléments
    std::vector<std::vector<std::size_t>> gmsh_elt_node_tags; // Connectivité des éléments (les indices des nœuds)
    gmsh::model::mesh::getElements(gmsh_elt_types, gmsh_elt_tags, gmsh_elt_node_tags, 2);

    gmds::Mesh& gmds_mesh = getGMDSMesh();
    // correspondance entre noeuds internes GMSH et GMDS pour éviter les doublons
    std::map<std::size_t, gmds::TCellID> gmshNodeTag_to_gmdsCellId;

    for (std::size_t i = 0; i < gmsh_elt_types.size(); ++i) {
        if (gmsh_elt_types[i] == 2) {  // Le type 2 correspond aux triangles linéaires
#ifdef _DEBUG_MESH
            std::cout << "Triangles found, number of elements: " << gmsh_elt_tags[i].size() << std::endl;
#endif

            std::vector<gmds::TCellID>& gmds_fa_node_ids = fa->nodes();
            std::vector<gmds::TCellID>& gmds_fa_face_ids = fa->faces();
            for (std::size_t j = 0; j < gmsh_elt_node_tags[i].size(); j += 3) {
                for (std::size_t k = 0; k < 3; ++k) {
                    gmds::TCellID gmds_node_id = -1;

                    std::size_t gmsh_node_tag = gmsh_elt_node_tags[i][j+k];
                    if (gmshNodeTag_to_gmdsCellId.find(gmsh_node_tag) == gmshNodeTag_to_gmdsCellId.end())
                    {
                        // Coordonnées du noeud GMSH
                        std::vector<double> gmsh_node_coord;
                        std::vector<double> gmsh_node_parametric_coord;
                        int dim, tag;
                        gmsh::model::mesh::getNode(gmsh_node_tag, gmsh_node_coord, gmsh_node_parametric_coord, dim, tag);
                        Utils::Math::Point gmsh_point(gmsh_node_coord[0], gmsh_node_coord[1], gmsh_node_coord[2]);

                        const double precision = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon;
                        
                        // Recherche du noeud GMDS correspondant
                        bool gmds_node_found = false;
                        for (auto gmds_node_id : gmds_node_ids)
                        {
                            gmds::Node gmds_node = gmds_mesh.get<gmds::Node>(gmds_node_id);
                            Utils::Math::Point gmds_point(gmds_node.X(), gmds_node.Y(), gmds_node.Z());
                            double dist = gmds_point.length(gmsh_point);
                            if (dist < precision)
                            {
                                gmds_node_found = true;
                                gmshNodeTag_to_gmdsCellId[gmsh_node_tag] = gmds_node_id;
                                break;
                            }
                        }

                        // Le noeud GMDS n'existait pas, il faut le créer
                        if (!gmds_node_found)
                        {
                            gmds_node_id = gmds_mesh.newNode(gmsh_node_coord[0], gmsh_node_coord[1], gmsh_node_coord[2]).id();
                            gmshNodeTag_to_gmdsCellId[gmsh_node_tag] = gmds_node_id;
                            gmds_fa_node_ids.push_back(gmds_node_id);
                            command->addCreatedNode(gmds_node_id);
                        }
                    }
                }

                // Création des triangles GMDS
                gmds::TCellID n1 = gmshNodeTag_to_gmdsCellId[gmsh_elt_node_tags[i][j]];
                gmds::TCellID n2 = gmshNodeTag_to_gmdsCellId[gmsh_elt_node_tags[i][j+1]];
                gmds::TCellID n3 = gmshNodeTag_to_gmdsCellId[gmsh_elt_node_tags[i][j+2]];
                gmds::Face gmds_triangle = gmds_mesh.newTriangle(n1, n2, n3);
                gmds_fa_face_ids.push_back(gmds_triangle.id());
                command->addCreatedFace(gmds_triangle.id());

#ifdef _DEBUG_MESH
                std::cout<<" triangle: "<<std::endl;
                gmds::Node node1 = gmds_mesh.get<gmds::Node>(n1);
                gmds::Node node2 = gmds_mesh.get<gmds::Node>(n2);
                gmds::Node node3 = gmds_mesh.get<gmds::Node>(n3);
                std::cout<<"  node 1 : "<<node1.X()<<", "<<node1.Y()<<", "<<node1.Z()<<std::endl;
                std::cout<<"  node 2 : "<<node2.X()<<", "<<node2.Y()<<", "<<node2.Z()<<std::endl;
                std::cout<<"  node 3 : "<<node3.X()<<", "<<node3.Y()<<", "<<node3.Z()<<std::endl;
#endif

            }
        }
    }
    // nettoyage mémoire de ce qui a servi à GMSH
    gmsh::model::remove();

    std::vector<std::string> group_names;
    fa->getGroupsName(group_names);

#ifdef _DEBUG_GROUP_BY_TOPO_ENTITY
    // on ajoute un groupe pour distinguer les faces en mode debug
    group_names.push_back(fa->getName());
#endif

    // ajout des polygones aux surfaces
    for (size_t i=0; i<group_names.size(); i++){
        std::string& nom = group_names[i];

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
    } // end for i<group_names.size()

#ifdef _DEBUG_MESH
    // Inspect the log:
    std::vector<std::string> log;
    gmsh::logger::get(log);
    std::cout << "Logger has recorded " << log.size() << " lines" << std::endl;
#endif

    gmsh::logger::stop();
    gmsh::finalize();
#endif // USE_GMSH
} // end meshDelaunayGMSH

/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
