/*----------------------------------------------------------------------------*/
/** \file CommandEditTopo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2/12/2011
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDEDITTOPO_H_
#define COMMANDEDITTOPO_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
#include <vector>
#include <map>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Geom {
class Vertex;
class Surface;
class Curve;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class TopoManager;
class TopoEntity;
class TopoProperty;
class Vertex;
class VertexTopoProperty;
class VertexGeomProperty;
class Edge;
class EdgeTopoProperty;
class EdgeMeshingProperty;
class CoEdgeTopoProperty;
class CoEdgeMeshingProperty;
class Face;
class FaceTopoProperty;
class FaceMeshingProperty;
class CoFaceTopoProperty;
class CoFaceMeshingProperty;
class Block;
class BlockTopoProperty;
class BlockMeshingProperty;
class FaceMeshingPropertyOrthogonal;

/*----------------------------------------------------------------------------*/
/** \class CommandEditTopo
 *  \brief Partie commune à toutes les commandes de modification de topologies
 */
/*----------------------------------------------------------------------------*/
class CommandEditTopo: public Internal::CommandInternal {

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le contexte
     * \param le nom de la commande
     */
    CommandEditTopo(Internal::Context& c, std::string name);

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction de la topologie
     */
    virtual ~CommandEditTopo();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    void internalUndo();

    /*------------------------------------------------------------------------*/
    /** \brief  rejoue la commande
     */
    void internalRedo();

    /*------------------------------------------------------------------------*/
    /** Ce qui est fait après la commande suivant le cas en erreur ou non
     */
    virtual void postExecute(bool hasError);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités modifiées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr) =0;


    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande, pour le preview (pas de notification)
     * \see similaire à getPreviewRepresentation
     */
    virtual void previewBegin();
    /** \brief  annule la commande, pour le preview (pas de notification)
     *
     *  La commande doit ensuite être détruite
     */
    virtual void previewEnd();


    /*------------------------------------------------------------------------*/

    void addTopoInfoProperty(TopoEntity* te, TopoProperty* tp);
    void addVertexInfoTopoProperty(Vertex* te, VertexTopoProperty* tp);
    void addVertexInfoGeomProperty(Vertex* te, VertexGeomProperty* tp);
    void addCoEdgeInfoTopoProperty(CoEdge* te, CoEdgeTopoProperty* tp);
    void addCoEdgeInfoMeshingProperty(CoEdge* te, CoEdgeMeshingProperty* tp);
    void addEdgeInfoTopoProperty(Edge* te, EdgeTopoProperty* tp);
    void addEdgeInfoMeshingProperty(Edge* te, EdgeMeshingProperty* tp);
    void addCoFaceInfoTopoProperty(CoFace* te, CoFaceTopoProperty* tp);
    void addCoFaceInfoMeshingProperty(CoFace* te, CoFaceMeshingProperty* tp);
    void addFaceInfoTopoProperty(Face* te, FaceTopoProperty* tp);
    void addFaceInfoMeshingProperty(Face* te, FaceMeshingProperty* tp);
    void addBlockInfoTopoProperty(Block* te, BlockTopoProperty* tp);
    void addBlockInfoMeshingProperty(Block* te, BlockMeshingProperty* tp);

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des nouvelles arêtes (celles avec modification des relations)
     *
     *  /see getPreviewRepresentation et countNbCoEdgesByVertices
     */
    virtual void getPreviewRepresentationTopoModif(Utils::DisplayRepresentation& dr);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des nouvelles arêtes
     *
     *  ATTENTION, semble poser pb dans la cas de destructions, à réserver pour la création
     *
     *  /see getPreviewRepresentation
     */
    virtual void getPreviewRepresentationNewCoedges(Utils::DisplayRepresentation& dr);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités avec une relation vers la géométrie changée
     *
     *  /see getPreviewRepresentation
     */
    virtual void getPreviewRepresentationCoedgeDisplayModified(Utils::DisplayRepresentation& dr);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités topologiques détruites
     *
     *  /see getPreviewRepresentation
     */
    virtual void getPreviewRepresentationDeletedTopo(Utils::DisplayRepresentation& dr);

    /*------------------------------------------------------------------------*/
    /// Enregistre auprès du TopoManager la création des entités de type Block, CoFace, CoEdge et Vertex
    void registerToManagerCreatedEntities();

    /*------------------------------------------------------------------------*/
    /** \brief  accesseur sur le manageur topologique
     */
    virtual TopoManager& getTopoManager();

    /*------------------------------------------------------------------------*/
    /// parcours les entités modifiés pour sauvegarder leur état avant la commande
    virtual void saveInternalsStats();

    /// permute les propriétés internes avec leur sauvegarde
    virtual void permInternalsStats();

    /// fait le ménage pour le cas d'une commande en erreur
    virtual void cancelInternalsStats();

    /// destruction des propriétés internes des entités modifiés
    virtual void deleteInternalsStats();

    /** destructions des entités créées par la commande
     * (dans le cas où on ne veut plus faire machine arrière)
     *  et les retire du topomanager
     */
    virtual void deleteCreated();

    /** destructions des entités créées par la commande
     *  celles qui sont marquées comme tel
     */
    virtual void deleteMarkedDeleted();

    /*------------------------------------------------------------------------*/
    /// supprime du InfoCommand les entités temporaires (celles en NONE)
    virtual void cleanTemporaryEntities();

    /*------------------------------------------------------------------------*/
    /// Compte le nombre d'arêtes reliées à chacun des sommets de la topologie concernée par la commande
    virtual void countNbCoEdgesByVertices(std::map<Topo::Vertex*, uint> &nb_coedges_by_vertex);

    /*------------------------------------------------------------------------*/
    /** Dans les cas d'une méthode de maillage initiale directionnelle ou rotationnelle
     *  potentiellement invalide
     *  recherche la méthode la plus simple
     */
    void updateMeshLaw(std::list<Topo::CoFace*>& l_f, std::list<Topo::Block*>& l_b);

    /*------------------------------------------------------------------------*/
    /// change la discrétisation pour les arêtes d'une coface en mettant la discrétisation orthogonale
    virtual void setOrthogonalCoEdges(CoFace* coface, FaceMeshingPropertyOrthogonal* mp);

    /// Change la discrétisation pour une ou plusieurs arêtes en la mettant orthogonale
    virtual void setOrthogonalCoEdges(Edge* edge, Vertex* vertex, int nbLayers);

    /*------------------------------------------------------------------------*/
    /** Duplique les CoFaces
     */
    virtual void duplicate(std::vector<CoFace*>& cofaces_dep,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<CoFace*, uint>& filtre_coface,
            std::vector<CoFace*>& cofaces_arr);

    /** Met à jour les noms des arêtes ou coface pour une arête dont la discrétisation est interpolée
     */
    virtual void updateCoEdgesInterpolateNames(CoEdge* coedge, std::map<std::string, CoEdge*>& corr_coedge,
    		std::string oldCofaceName, std::string newCofaceName);


    /** Met à jour les associations entre les entités topologiques
     *  avec leur ancienne association géométrique, vers la nouvelle
     *  entité géométrique
     */
    virtual void updateGeomAssociation(std::vector<CoFace*>& cofaces,
            std::map<Geom::Vertex*,Geom::Vertex*>& V2V,
            std::map<Geom::Curve*,Geom::Curve*>& C2C,
            std::map<Geom::Surface*,Geom::Surface*>& S2S,
			bool onlyIfDestroyed);

    /** Met à jour les associations entre les entités topologiques
     *  avec leur ancienne association géométrique, vers la nouvelle
     *  entité géométrique
     *  pour les entités qui ne sont pas sur un plan de symétrie
     *  Supprime les projections des arêtes sur l'ogrid
     */
    virtual void updateGeomAssociation(std::vector<CoFace*>& cofaces,
            std::map<Geom::Vertex*,Geom::Curve*>& V2C,
            std::map<Geom::Curve*,Geom::Surface*>& C2S,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge);

    /*------------------------------------------------------------------------*/
    /// fusionne les coedges si possible autour du sommet
    void mergeCoEdges(Vertex* vtx, std::map<CoEdge*,uint>& keep_coedges);

    /*------------------------------------------------------------------------*/
    /// fusionne les cofaces si possible autour du sommet
    void mergeCoFaces(Vertex* vtx, std::map<CoFace*,uint>& keep_cofaces);

    /*------------------------------------------------------------------------*/
    struct TopoPropertyInfo {
        TopoEntity* m_entity;
        TopoProperty* m_property;
    };

    struct VertexTopoPropertyInfo {
        Vertex* m_entity;
        VertexTopoProperty* m_property;
    };

    struct VertexGeomPropertyInfo {
        Vertex* m_entity;
        VertexGeomProperty* m_property;
    };

    struct CoEdgeTopoPropertyInfo {
        CoEdge* m_entity;
        CoEdgeTopoProperty* m_property;
    };

    struct CoEdgeMeshingPropertyInfo {
        CoEdge* m_entity;
        CoEdgeMeshingProperty* m_property;
    };

    struct EdgeTopoPropertyInfo {
        Edge* m_entity;
        EdgeTopoProperty* m_property;
    };

    struct EdgeMeshingPropertyInfo {
        Edge* m_entity;
        EdgeMeshingProperty* m_property;
    };

    struct CoFaceTopoPropertyInfo {
        CoFace* m_entity;
        CoFaceTopoProperty* m_property;
    };

    struct CoFaceMeshingPropertyInfo {
        CoFace* m_entity;
        CoFaceMeshingProperty* m_property;
    };

    struct FaceTopoPropertyInfo {
        Face* m_entity;
        FaceTopoProperty* m_property;
    };

    struct FaceMeshingPropertyInfo {
        Face* m_entity;
        FaceMeshingProperty* m_property;
    };

    struct BlockTopoPropertyInfo {
        Block* m_entity;
        BlockTopoProperty* m_property;
    };

    struct BlockMeshingPropertyInfo {
        Block* m_entity;
        BlockMeshingProperty* m_property;
    };

    /*------------------------------------------------------------------------*/
protected:

    /// Sauvegarde de la partie interne des topo entity
    std::vector <TopoPropertyInfo> m_topo_property_info;

    /// Sauvegarde de la partie géométrique interne des sommets
    std::vector <VertexGeomPropertyInfo> m_vertex_geom_property_info;

    /// Sauvegarde de la partie topologique interne des sommets
    std::vector <VertexTopoPropertyInfo> m_vertex_topo_property_info;

    /// Sauvegarde de la partie maillage interne des arêtes communes
    std::vector <CoEdgeMeshingPropertyInfo> m_coedge_mesh_property_info;

    /// Sauvegarde de la partie topologique interne des arêtes communes
    std::vector <CoEdgeTopoPropertyInfo> m_coedge_topo_property_info;

    /// Sauvegarde de la partie topologique interne des arêtes
    std::vector <EdgeTopoPropertyInfo> m_edge_topo_property_info;

    /// Sauvegarde de la partie maillage (ratios) interne des arêtes
    std::vector <EdgeMeshingPropertyInfo> m_edge_mesh_property_info;

    /// Sauvegarde de la partie maillage interne des faces
    std::vector <CoFaceMeshingPropertyInfo> m_coface_mesh_property_info;

    /// Sauvegarde de la partie topologique interne des faces communes
    std::vector <CoFaceTopoPropertyInfo> m_coface_topo_property_info;

    /// Sauvegarde de la partie topologique interne des faces
    std::vector <FaceTopoPropertyInfo> m_face_topo_property_info;

    /// Sauvegarde de la partie maillage (ratios) interne des faces
    std::vector <FaceMeshingPropertyInfo> m_face_mesh_property_info;

    /// Sauvegarde de la partie maillage interne des blocs
    std::vector <BlockMeshingPropertyInfo> m_block_mesh_property_info;

    /// Sauvegarde de la partie topologique interne des blocs
    std::vector <BlockTopoPropertyInfo> m_block_topo_property_info;

protected:

    /// mode preview activé
    bool m_isPreview;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDEDITTOPO_H_ */
/*----------------------------------------------------------------------------*/

