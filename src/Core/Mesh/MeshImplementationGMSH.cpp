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

    gmds::Mesh& gmds_mesh = getGMDSMesh();

    Topo::FaceMeshingPropertyDelaunayGMSH* prop =
            dynamic_cast<Topo::FaceMeshingPropertyDelaunayGMSH*>(fa->getCoFaceMeshingProperty());
    CHECK_NULL_PTR_ERROR(prop);

    // init des options GMSH
    gmsh::initialize();
    gmsh::logger::start();

    // Initialize robust predicates
    //robustPredicates::exactinit();

    // récupération des paramètre utilisateur
    gmsh::option::setNumber("Mesh.MeshSizeMin", prop->getMin());
    gmsh::option::setNumber("Mesh.MeshSizeMax", prop->getMax());

    // algo Delaunay
    gmsh::option::setNumber("Mesh.Algorithm", 5);

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
    std::cout <<"=== new gmsh model"<<std::endl;
#endif
    // construction du modèle GMSH à partir de cette shape OCC
    gmsh::model::add(std::string("GModel_")+fa->getName());
    
#ifdef _DEBUG_MESH
    std::cout <<"=== importOCCShape"<<std::endl;
#endif
    gmsh::vectorpair outDimTags;
    gmsh::model::occ::importShapesNativePointer(&face_shape, outDimTags);
    //gmsh::model::occ::synchronize();

    // Recherche de la face = seule shape de dimension 2
    auto it = std::find_if(outDimTags.begin(), outDimTags.end(), [](const std::pair<int, int>& p) {
        return p.first == 2;
    });
    if (it == outDimTags.end()) {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "L'import de la face OCC dans GMSH a échoué : "<< fa->getName();
        throw TkUtil::Exception (message);
    }
    int gFaceId = it->second;

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
        Topo::Edge* edge =edges[i];
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
 
        std::vector<gmds::Node> edge_nodes;
        edge->getNodes(vertices[0], vertices[1], edge_nodes);

        std::vector<std::size_t> gmsh_node_tags;
        gmsh_node_tags.reserve(edge_nodes.size() + 2);
        std::vector<double> gmsh_node_coords;
        gmsh_node_coords.reserve(3*edge_nodes.size());
        std::vector<double> gmsh_parametric_coords;

       // On stoke la liste des noeuds gmds rencontrés dans l'algorithme
       if (edge_nodes.size() > 0)
       {
            gmds_node_ids.insert(edge_nodes[0].id());
            gmds_node_ids.insert(edge_nodes[edge_nodes.size() - 1].id());
       }

        // Création des noeuds internes
       for (uint j=1; j<edge_nodes.size()-1; j++)
       {
            gmds::Node nd = edge_nodes[j];
            gmds_node_ids.insert(nd.id());
            Utils::Math::Point pt = getCoordNode(nd);
            int gmsh_point_tag = gmsh::model::occ::addPoint(pt.getX(), pt.getY(), pt.getZ());
            std::cout << "Noeud " << j << " de coord " << pt  << std::endl;
            gmsh_point_dim_tags.push_back({0, gmsh_point_tag});
        }
    } // end for(unsigned int i=0;i<edges.size();i++)

    gmsh::vectorpair gmsh_entities_dim_tags;
    gmsh::model::occ::getEntities(gmsh_entities_dim_tags);
    gmsh::vectorpair out_dim_tags;
    std::vector<gmsh::vectorpair> out_dim_tags_map;
    gmsh::model::occ::fragment(gmsh_point_dim_tags, gmsh_entities_dim_tags, out_dim_tags, out_dim_tags_map);

    gmsh::model::occ::synchronize();

    gmsh_entities_dim_tags.clear();
    gmsh::model::occ::getEntities(gmsh_entities_dim_tags, 1);
    for (auto dt : gmsh_entities_dim_tags)
    {
        int lineId = dt.second;
        std::string type;
        gmsh::model::getType(1, dt.second, type);  // 1 pour dimension 1
        if (type == "Line")
            gmsh::model::mesh::setTransfiniteCurve(dt.second, 2);
    }

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

    std::vector<int> gmsh_elt_types;                      // Types des éléments
    std::vector<std::vector<std::size_t>> gmsh_elt_tags;  // Tags des éléments
    std::vector<std::vector<std::size_t>> gmsh_elt_node_tags; // Connectivité des éléments (les indices des nœuds)

    // Récupérer les éléments de la dimension 2 (triangles, quadrangles, etc.)
    gmsh::model::mesh::getElements(gmsh_elt_types, gmsh_elt_tags, gmsh_elt_node_tags, 2);

    // correspondance entre noeuds internes GMSH et GMDS pour éviter les doublons
    std::map<std::size_t, gmds::TCellID> gmshNodeTag_to_gmdsCellId;

    for (std::size_t i = 0; i < gmsh_elt_types.size(); ++i) {
        if (gmsh_elt_types[i] == 2) {  // Le type 2 correspond aux triangles linéaires
#ifdef _DEBUG_MESH
            std::cout << "Triangles found, number of elements: " << gmsh_elt_tags[i].size() << std::endl;
#endif

            std::vector<gmds::TCellID>& fa_node_ids = fa->nodes();
            std::vector<gmds::TCellID>& fa_face_ids = fa->faces();
            for (std::size_t j = 0; j < gmsh_elt_node_tags[i].size(); j += 3) {
                gmds::TCellID tcellids[3];
                for (std::size_t k = 0; k < 3; ++k) {
                    size_t gmsh_node_tag = gmsh_elt_node_tags[i][j+k];

                    gmds::TCellID gmds_node_id = -1;

                    auto iter =  gmshNodeTag_to_gmdsCellId.find(gmsh_node_tag);
                    if (iter == gmshNodeTag_to_gmdsCellId.end())
                    {
                        // Coordonnées du noeud GMSH
                        std::vector<double> coord;
                        std::vector<double> parametricCoord;
                        int dim, tag;
                        gmsh::model::mesh::getNode(gmsh_node_tag, coord, parametricCoord, dim, tag);
                        Utils::Math::Point gmsh_point(coord[0], coord[1], coord[2]);

                        const double precision = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon;
                        
                        // Recherche du noeud GMDS correspondant
                        gmds::Mesh& gmds_mesh = getGMDSMesh();
                        for (auto node_id : gmds_node_ids)
                        {
                            gmds::Node nd = gmds_mesh.get<gmds::Node>(node_id);
                            Utils::Math::Point gmds_point(nd.X(), nd.Y(), nd.Z());
                            double dist = gmds_point.length(gmsh_point);
                            if (dist < precision)
                            {
                                gmds_node_id = node_id;
                                gmshNodeTag_to_gmdsCellId[gmsh_node_tag] = node_id;
                                break;
                            }
                        }

                        // Le noeud GMDS n'existait pas, il faut le créer
                        if (gmds_node_id == -1)
                        {
                            gmds_node_id = getGMDSMesh().newNode(coord[0], coord[1], coord[2]).id();
                            gmshNodeTag_to_gmdsCellId[gmsh_node_tag] = gmds_node_id;
                            fa_node_ids.push_back(gmds_node_id);
                            command->addCreatedNode(gmds_node_id);
                        }
                    }
                    else
                        gmds_node_id = iter->second;
                }
                gmds::TCellID n1 = gmshNodeTag_to_gmdsCellId[gmsh_elt_node_tags[i][j]];
                gmds::TCellID n2 = gmshNodeTag_to_gmdsCellId[gmsh_elt_node_tags[i][j+1]];
                gmds::TCellID n3 = gmshNodeTag_to_gmdsCellId[gmsh_elt_node_tags[i][j+2]];

#ifdef _DEBUG_MESH
                std::cout<<" triangle: "<<std::endl;
                gmds::Node node1 = gmds_mesh.get<gmds::Node>(n1);
                gmds::Node node2 = gmds_mesh.get<gmds::Node>(n2);
                gmds::Node node3 = gmds_mesh.get<gmds::Node>(n3);
                std::cout<<"  node 1 : "<<node1.X()<<", "<<node1.Y()<<", "<<node1.Z()<<std::endl;
                std::cout<<"  node 2 : "<<node2.X()<<", "<<node2.Y()<<", "<<node2.Z()<<std::endl;
                std::cout<<"  node 3 : "<<node3.X()<<", "<<node3.Y()<<", "<<node3.Z()<<std::endl;
#endif

                gmds::Face t1 = getGMDSMesh().newTriangle(n1, n2, n3);
                fa_face_ids.push_back(t1.id());
                command->addCreatedFace(t1.id());
            }
        }
    }
    // nettoyage mémoire de ce qui a servi à GMSH
    gmsh::model::remove();

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
