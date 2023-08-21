/*----------------------------------------------------------------------------*/
/*
 * \file CommandCreateMesh.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCREATEMESH_H_
#define COMMANDCREATEMESH_H_
/*----------------------------------------------------------------------------*/
#include "Internal/MultiTaskedCommand.h"
#include "Utils/Container.h"

#include "Mesh/Cloud.h"
#include "Mesh/Line.h"
#include "Mesh/Surface.h"
#include "Mesh/Volume.h"
/*----------------------------------------------------------------------------*/
namespace gmds {
class Node;
class Face;
class Region;
}
namespace Topo {
class CoFaceMeshingProperty;
class BlockMeshingProperty;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
class CommandCreateMesh: public Internal::MultiTaskedCommand {

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le contexte
     * \param le nom de la commande
     * \param le nombre de tâches qui seront au maximum lancées en concurrence
     */
    CommandCreateMesh(Internal::Context& c, std::string name, size_t tasksNum);
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction du maillage créé
     */
    virtual ~CommandCreateMesh();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    void internalUndo();

    /** \brief  rejoue la commande
     */
    void internalRedo();

    /*------------------------------------------------------------------------*/
    /** Ce qui est fait après la commande suivant le cas en erreur ou non
     */
    virtual void postExecute(bool hasError);
    /*------------------------------------------------------------------------*/
    /// Accesseur sur le vecteur des noeuds créés par la commande
    std::vector<gmds::TCellID>& createdNodes() {return m_created_nodes;}

    /// Accesseur sur le vecteur des bras créés par la commande
    std::vector<gmds::TCellID>& createdEdges() {return m_created_edges;}

    /// Accesseur sur le vecteur des polygones créés par la commande
    std::vector<gmds::TCellID>& createdFaces() {return m_created_faces;}

    /// Accesseur sur le vecteur des polyedres créés par la commande
    std::vector<gmds::TCellID>& createdRegions() {return m_created_regions;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur les noms des nuages créés par la commande
    const std::vector<Cloud*>& createdClouds () const {return m_created_clouds.get();}

    /// Accesseur sur les noms des surfaces de maillage créés par la commande
    const std::vector<Surface*>& createdSurfaces () const {return  m_created_surfaces.get();}

    /// Accesseur sur les noms des volumes de maillage créés par la commande
    const std::vector<Volume*>& createdVolumes () const {return  m_created_volumes.get();}

    /*------------------------------------------------------------------------*/
    /// Ajoute au vecteur le noeud créé par la commande, suivant la strategie
    void addCreatedNode(gmds::TCellID id);

    /// Ajoute au vecteur le bras créé par la commande, suivant la strategie
    void addCreatedEdge(gmds::TCellID id);

    /// Ajoute au vecteur le polygone créé par la commande, suivant la strategie
    void addCreatedFace(gmds::TCellID id);

    /// Ajoute au vecteur le polyèdre créé par la commande, suivant la strategie
    void addCreatedRegion(gmds::TCellID id);

    /*------------------------------------------------------------------------*/
    /// Ajoute un nuage à la structure, ce nuage est déjà créé dans le maillage
    void addNewCloud(const std::string& name);

    /// Ajoute une ligne à la structure, cette ligne est déjà créée dans le maillage
    void addNewLine(const std::string& name);

    /// Ajoute une surface à la structure, cette surface est déjà créée dans le maillage
    void addNewSurface(const std::string& name);

    /** Ajoute un volume à la structure, ce volume est déjà créé dans le maillage
     *  Attention, sa représentation n'est pas créée, il faut le faire aprés avoir mis les blocs
     *  \see Context::newGraphicalRepresentation
     */
    void addNewVolume(const std::string& name);

    /// Notification de la modification d'un nuage
    void addModifiedCloud(const std::string& name);

    /// Notification de la modification d'une ligne
    void addModifiedLine(const std::string& name);

    /// Notification de la modification d'une surface
    void addModifiedSurface(const std::string& name);

    /// Notification de la modification d'un volume
    void addModifiedVolume(const std::string& name);

    /*------------------------------------------------------------------------*/
    void addVertexInfoMeshingData(Topo::Vertex* te, Topo::VertexMeshingData* tp);
    void addCoEdgeInfoMeshingData(Topo::CoEdge* te, Topo::CoEdgeMeshingData* tp);
    void addCoFaceInfoMeshingData(Topo::CoFace* te, Topo::CoFaceMeshingData* tp);
    void addBlockInfoMeshingData(Topo::Block* te, Topo::BlockMeshingData* tp);

    void addVolumeInfoTopoProperty(Mesh::Volume* te, Topo::MeshVolumeTopoProperty* tp);
    void addSurfaceInfoTopoProperty(Mesh::Surface* te, Topo::MeshSurfaceTopoProperty* tp);
    void addLineInfoTopoProperty(Mesh::Line* te, Topo::MeshLineTopoProperty* tp);
    void addCloudInfoTopoProperty(Mesh::Cloud* te, Topo::MeshCloudTopoProperty* tp);

    void addCoFaceInfoMeshingProperty(Topo::CoFace* te, Topo::CoFaceMeshingProperty* tp);
    void addBlockInfoMeshingProperty(Topo::Block* te, Topo::BlockMeshingProperty* tp);

	/* Accès aux méthodes threadedXXX ( ) par une classe extérieure => accès public. */
	/// Pré-maillage du bloc (appelé depuis un thread).
	void threadedPreMesh (Topo::Block* block);
	/// Pré-maillage de la face (appelé depuis un thread).
	void threadedPreMesh (Topo::CoFace* coface);
	/// Pré-maillage de l'arête (appelé depuis un thread).
	void threadedPreMesh (Topo::CoEdge* coedge);

protected:

    /*------------------------------------------------------------------------*/
    /** \brief  accesseur sur le manageur de maillage
     */
    virtual MeshManager& getMeshManager();

    /*------------------------------------------------------------------------*/
    /// parcours les entités modifiés pour sauvegarder leur état avant la commande
    virtual void saveInternalsStats();

    /// permute les propriétés internes avec leur sauvegarde
    virtual void permInternalsStats();

    /// fait le ménage pour le cas d'une commande en erreur
    virtual void cancelInternalsStats();

    /// destruction des propriétés internes des entités (topologiques) modifiées
    virtual void deleteInternalsStats();

    /// suppression des entités du maillage créées
    virtual void deleteCreatedMeshEntities();

    /// suppression des groupes du maillage créées
    virtual void deleteCreatedMeshGroups();

    /*------------------------------------------------------------------------*/
    /// Création du maillage pour une liste de blocs
    void mesh(std::vector<Topo::Block* >& blocs);
    /// Création du maillage pour un bloc
    void mesh(Topo::Block* & bloc);
    /// Création des points du maillage pour les blocs transmis en argument.
    void preMesh(const std::vector<Topo::Block*>& blocks);
    /// Création des points du maillage pour un bloc
    void preMesh(Topo::Block* bloc);

    /// Création du maillage pour une liste de faces
    void mesh(std::vector<Topo::CoFace* >& faces);
    /// Création du maillage pour une face
    void mesh(Topo::CoFace* face);
    /// Création des points du maillage pour les faces transmises en argument.
    void preMesh(const std::vector<Topo::CoFace*>& faces);
    /// Création des points du maillage pour une face
    void preMesh(Topo::CoFace* face);

    /// Création du maillage pour une arête
    void mesh(Topo::CoEdge* arete);
    /// Création des points du maillage pour les arêtes transmises en argument.
    void preMesh(const std::vector<Topo::CoEdge*>& aretes);
    /// Création des points du maillage pour une arête
    void preMesh(Topo::CoEdge* arete);

    /// Création du maillage pour un sommet
    void mesh(Topo::Vertex* sommet);

    /*------------------------------------------------------------------------*/
    /// Maille et modifie le maillage pour les modifications 2D
    void meshAndModify(std::list<Topo::CoFace*>& list_cofaces);

    /*------------------------------------------------------------------------*/
    /// Modifie le maillage pour les modifications 3D (lissage)
    void modify(std::vector<Topo::Block*>& list_blocks);

    /*------------------------------------------------------------------------*/
     struct VertexPropertyInfo {
        Topo::Vertex* m_entity;
        Topo::VertexMeshingData* m_property;
    };

    struct CoEdgePropertyInfo {
        Topo::CoEdge* m_entity;
        Topo::CoEdgeMeshingData* m_property;
    };

    struct CoFacePropertyInfo {
        Topo::CoFace* m_entity;
        Topo::CoFaceMeshingData* m_property;
    };

    struct BlockPropertyInfo {
        Topo::Block* m_entity;
        Topo::BlockMeshingData* m_property;
    };

    struct VolumePropertyInfo {
        Mesh::Volume* m_entity;
        Topo::MeshVolumeTopoProperty* m_property;
    };

    struct SurfacePropertyInfo {
        Mesh::Surface* m_entity;
        Topo::MeshSurfaceTopoProperty* m_property;
    };

    struct CloudPropertyInfo {
    	Mesh::Cloud* m_entity;
    	Topo::MeshCloudTopoProperty* m_property;
    };

    struct LinePropertyInfo {
    	Mesh::Line* m_entity;
    	Topo::MeshLineTopoProperty* m_property;
    };

    struct CoFaceMeshingPropertyInfo {
    	Topo::CoFace* m_entity;
    	Topo::CoFaceMeshingProperty* m_property;
    };

    struct BlockMeshingPropertyInfo {
        Topo::Block* m_entity;
        Topo::BlockMeshingProperty* m_property;
    };

    /// Sauvegarde de la relation avec le maillage pour les sommets
    std::vector <VertexPropertyInfo> m_vertex_property_info;

    /// Sauvegarde de la relation avec le maillage pour les arêtes communes
    std::vector <CoEdgePropertyInfo> m_coedge_property_info;

    /// Sauvegarde de la relation avec le maillage pour les faces communes
    std::vector <CoFacePropertyInfo> m_coface_property_info;

    /// Sauvegarde de la partie maillage interne des faces
    std::vector <CoFaceMeshingPropertyInfo> m_coface_mesh_property_info;

    /// Sauvegarde de la relation avec le maillage pour les blocs
    std::vector <BlockPropertyInfo> m_block_property_info;

    /// Sauvegarde de la partie maillage interne des blocs
    std::vector <BlockMeshingPropertyInfo> m_block_mesh_property_info;

    /// Sauvegarde de la relation des volumes vers les blocs
    std::vector <VolumePropertyInfo> m_volume_property_info;

    /// Sauvegarde de la relation des surfaces vers les cofaces
    std::vector <SurfacePropertyInfo> m_surface_property_info;

    /// Sauvegarde de la relation des lignes vers les coedges
    std::vector <LinePropertyInfo> m_line_property_info;

    /// Sauvegarde de la relation des nuages vers les coedges
    std::vector <CloudPropertyInfo> m_cloud_property_info;


    /// stockage des noeuds créés par la commande
    std::vector<gmds::TCellID> m_created_nodes;

    /// stockage des bras créés par la commande
    std::vector<gmds::TCellID> m_created_edges;

    /// stockage des polygones créés par la commande
    std::vector<gmds::TCellID> m_created_faces;

    /// stockage des polyèdres créés par la commande
    std::vector<gmds::TCellID> m_created_regions;


    /// stockage des nuages créés par la commande
    Utils::Container<Cloud> m_created_clouds;

    /// stockage des lignes créées par la commande
    Utils::Container<Line> m_created_lines;

    /// stockage des surfaces de maillage créés par la commande
    Utils::Container<Surface> m_created_surfaces;

    /// stockage des volumes de maillage créés par la commande
    Utils::Container<Volume> m_created_volumes;


    /// Strategie de stockage pour cette commande
    MeshManager::strategy m_strategy;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCREATEMESH_H_ */
