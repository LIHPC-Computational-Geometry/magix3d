/*----------------------------------------------------------------------------*/
/** \file CoEdge.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 07/02/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef TOPO_COEDGE_H_
#define TOPO_COEDGE_H_
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#include <set>
#include "Topo/TopoEntity.h"
#include "Topo/CoEdgeMeshingProperty.h"
#include "Topo/CoEdgeTopoProperty.h"
#include "Topo/CoEdgeMeshingData.h"
#include <TkUtil/Mutex.h>
/*----------------------------------------------------------------------------*/
namespace gmds {
class Node;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Internal {
class InfoCommand;
}

namespace Geom {
class Curve;
class Surface;
}

/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class Vertex;
class Edge;
class CoFace;
class Block;
/*----------------------------------------------------------------------------*/
/**
   @brief Arête commune de la topologie (partie commune entre 2 faces ou plus)
 */
class CoEdge : public TopoEntity {

    static const char* typeNameTopoCoEdge;

public:

    /*------------------------------------------------------------------------*/
    /// Constructeur (avec 2 sommets)
    CoEdge(Internal::Context& ctx,
         Topo::CoEdgeMeshingProperty* emp,
         Topo::Vertex* v1, Topo::Vertex* v2);

    /*------------------------------------------------------------------------*/
    /// Constructeur par copie
    CoEdge(const CoEdge& v);

    /*------------------------------------------------------------------------*/
    /// Destructeur
    virtual ~CoEdge();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité topologique */
    int getDim() const {return 1;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nombre de bras du maillage
    uint getNbMeshingEdges() const {return m_mesh_property->getNbEdges();}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nombre de noeuds
    uint getNbNodes() const {return m_mesh_property->getNbNodes();}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    virtual CoEdgeMeshingProperty::meshLaw getMeshLaw() const {return m_mesh_property->getMeshLaw();}

    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return m_mesh_property->getMeshLawName();}

    /*------------------------------------------------------------------------*/
    /** \brief  Affecte un pointeur vers une entité géométrique
     */
    virtual void setGeomAssociation(Geom::GeomEntity* ge);

    /** Change la propriété topologique
     * \return l'ancienne propriété (pour destruction)
     */
    virtual TopoProperty* setProperty(TopoProperty* new_tp);

    /*------------------------------------------------------------------------*/
    /** \brief   Suppression des dépendances (entités topologiques incidentes)
     */
    virtual void clearDependancy() {m_topo_property->clearDependancy();}

    /*------------------------------------------------------------------------*/
    /// ajoute une relation vers une arête
    void addEdge(Edge* e) {m_topo_property->getEdgeContainer().add(e);}

    /// enlève une relation vers une arête
    void removeEdge(Edge* e, const bool exceptionIfNotFound=true)
    {m_topo_property->getEdgeContainer().remove(e, exceptionIfNotFound);}

    /// Fournit l'accès aux arêtes topologiques qui utilisent cette CoEdge avec une copie
    void getEdges(std::vector<Edge* >& edges) const
    { m_topo_property->getEdgeContainer().get(edges); }

    /// Fournit l'accès aux arêtes topologiques qui utilisent cette CoEdge sans copie
    const std::vector<Edge* > & getEdges() const
    {return m_topo_property->getEdgeContainer().get();}

    /// fournit l'accès à l'une des arêtes
    Edge* getEdge(uint ind) const
    {return m_topo_property->getEdgeContainer().get(ind);}

    uint getNbEdges() const
    {return m_topo_property->getEdgeContainer().getNb();}

    /*------------------------------------------------------------------------*/
    /** \brief Remplace un sommet v1 par le sommet v2
     *
     * Renvoie une exception si le sommet n'est pas trouvé
     *
     * Propage vers les faces
     */
    void replace(Topo::Vertex* v1, Topo::Vertex* v2, bool propagate_up, bool propagate_down, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** \brief Fusionne 2 arêtes ensembles
     *
     *  Les relations avec les faces sont mises à jour
     *  ed est remplacé par this
     */
    void merge(CoEdge* ed, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** \brief Fussionne les 2 sommets de l'arête qui disparait ensuite
     *
     * Propage vers les autres entités
     */
    void collapse(Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Détruit et libère l'arête commune
     *
     * Elle est marquée comme détruite
     * et les liens dessus sont retirés
     */
    void free(Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Coupe une arête commune en deux
     * Propage cette modification (aux arêtes)
     *
     * Elle est marquée comme détruite
     * On retourne le sommet créé
     *
     * Si la coupe passe par une extrémité, on conserve cette arête
     * et on retourne le sommet à cette extrémité
     */
    Topo::Vertex* split(uint nbMeshingEdges, Internal::InfoCommand* icmd);

    /** Coupe une arête commune en deux ou trois
     * Propage cette modification (aux arêtes)
     *
     * Elle est marquée comme détruite
     * On retourne le[s] sommet[s] créé[s]
     *
     * Si la coupe passe par une extrémité, on conserve cette arête
     * et on retourne le sommet à cette extrémité
     */
    std::vector<Topo::Vertex*> split(std::vector<uint> nbMeshingEdges, Internal::InfoCommand* icmd);

    /** Coupe une arête commune en deux suivant un sommet déjà créé
     * Propage cette modification (aux arêtes)
     *
     * Elle est marquée comme détruite
     * On retourne les arêtes créées
     *
     * Il est fait un tri des coedges dans les edges si c'est demandé
     */
    std::vector<CoEdge*> split(Topo::Vertex* vtx, uint nbMeshingEdges, Internal::InfoCommand* icmd, bool sortCoEdges);

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux sommets topologiques incidents avec une copie
     *
     *  \param vertices les sommets incidents
     */
    void getVertices(std::vector<Topo::Vertex* >& vertices) const
    {m_topo_property->getVertexContainer().get(vertices);}

    /** Fournit l'accès aux sommets topologiques incidents sans copie
     */
    const std::vector<Topo::Vertex* >& getVertices() const
        {return m_topo_property->getVertexContainer().get();}

    /// fournit l'accès à l'un des sommets
    Topo::Vertex* getVertex(uint ind) const
    {return m_topo_property->getVertexContainer().get(ind);}

    /// \return le nombre de Vertex
    uint getNbVertices() const;

    /// vérifie si cette arête commune contient ou non ce sommet
    bool find(Topo::Vertex* v) const
    {return m_topo_property->getVertexContainer().find(v);}

    /// accès à tous les sommets
    void getAllVertices(std::vector<Topo::Vertex* >& vertices, const bool unique=true) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux sommets topologiques opposé à un sommet
     *
     *  \param un sommet de l'arête
     *  \return le sommet opposé
     */
    Topo::Vertex* getOppositeVertex(Topo::Vertex* v) const;

    /*------------------------------------------------------------------------*/
    /** Constitue la liste des Blocs adjacents */
    void getBlocks(std::vector<Block* >& blocks) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation affichable de l'entité en se basant
     *          sur la représentation passée en paramètre
     *
     *  \param  dr la représentation que l'on demande à afficher
     *  \param	Lève une exception si checkDestroyed vaut true
     */
    virtual void getRepresentation(
			Mgx3D::Utils::DisplayRepresentation& dr, bool checkDestroyed) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation textuelle de l'entité.
	 * \param	true si l'entité fourni la totalité de sa description, false si
	 * 			elle ne fournit que les informations non calculées (objectif :
	 * 			optimisation)
     * \return  Description, à détruire par l'appelant.
     */
    virtual Mgx3D::Utils::SerializedRepresentation* getDescription (
													bool alsoComputed) const;

    /*------------------------------------------------------------------------*/
    /** Vérification de l'arête
     * Il est juste vérifié qu'elle n'est pas détruite
     * et que les arêtes référencées (si c'est le cas) via l'interpolation ne le sont pas non plus
     */
    virtual void check() const;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameTopoCoEdge;}

    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::TopoCoEdge;}

   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "Ar";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est une CoEdge suivant son nom
     */
    static bool isA(const std::string& name);

    /*------------------------------------------------------------------------*/
    /** Duplique le CoEdgeTopoProperty pour en conserver une copie
     *  (non modifiée par les accesseurs divers)
     *  Si le CoEdgeTopoProperty a déjà une copie de sauvegarde, il n'est rien fait
     *
     *  Si InfoCommand n'est pas nul, il est informé de la modification de l'entité
     */
    void saveCoEdgeTopoProperty(Internal::InfoCommand* icmd);

    /** Echange le CoEdgeTopoProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
    CoEdgeTopoProperty* setProperty(CoEdgeTopoProperty* prop);

    /*------------------------------------------------------------------------*/
    /** Duplique le CoEdgeMeshingProperty pour en conserver une copie
     */
    void saveCoEdgeMeshingProperty(Internal::InfoCommand* icmd);

    /** Remplace la propriété et met l'ancienne dans la sauvegarde interne
     */
    void switchCoEdgeMeshingProperty(Internal::InfoCommand* icmd, CoEdgeMeshingProperty* prop);

    /** Echange le CoEdgeMeshingProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
    CoEdgeMeshingProperty* setProperty(CoEdgeMeshingProperty* prop);

    /// accesseur sur les propriétés de maillage
    CoEdgeMeshingProperty* getMeshingProperty() {return m_mesh_property;}
    const CoEdgeMeshingProperty* getMeshingProperty() const {return m_mesh_property;}

    /*------------------------------------------------------------------------*/
    /** Fusionne deux arêtes en remplacant les noeuds de l'arête B par ceux de A (this)
     */
   virtual void fuse(CoEdge* edge_B,
            const std::vector<Topo::Vertex* >& l_sommets_A,
            const std::vector<Topo::Vertex* >& l_sommets_B,
            Internal::InfoCommand* icmd);

    /** Fonction équivalente qui recherche les noeuds qui sont en correspondance
     * c'est à dire ceux qui ont la même position géométrique
     */
    virtual void fuse(CoEdge* edge_B,
            Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Duplique le CoEdgeMeshingData pour en conserver une copie
     */
    void saveCoEdgeMeshingData(Internal::InfoCommand* icmd);

    /** Echange le CoEdgeMeshingData avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
   CoEdgeMeshingData* setProperty(CoEdgeMeshingData* prop);

    /// accesseur sur les propriétés de maillage
    CoEdgeMeshingData* getMeshingData() {return m_mesh_data;}
    const CoEdgeMeshingData* getMeshingData() const {return m_mesh_data;}

    /*------------------------------------------------------------------------*/
    /** Transmet les propriétés sauvegardées
     *  dont la destruction est alors à la charge du CommandEditTopo
     *  On ne garde pas de lien dessus
     */
    virtual void  saveInternals(CommandEditTopo* cet);

    /** Transmet les propriétés sauvegardées
     *  dont la destruction est alors à la charge du CommandCreateMesh
     *  On ne garde pas de lien dessus
     */
    virtual void  saveInternals(Mesh::CommandCreateMesh* ccm);

    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const CoEdge &);
    friend std::ostream & operator << (std::ostream & , const CoEdge &);

    /*------------------------------------------------------------------------*/
    /** Récupère des informations relatives à l'entité topologique */
    Topo::TopoInfo getInfos() const;

    /*------------------------------------------------------------------------*/
    /** Accesseur sur la liste des noeuds ordonnés suivant les sommets
     * \param nodes en sortie, la liste des pointeurs sur les noeuds
     */
    void getNodes(Topo::Vertex* v1, Topo::Vertex* v2,
            std::vector<gmds::Node> &nodes);

    /** Accesseur sur la liste des points */
    const std::vector<Utils::Math::Point>& points() const {return m_mesh_data->points();}

    /** Accesseur sur la liste des noeuds gmds */
    std::vector<gmds::TCellID>& nodes() {return m_mesh_data->nodes();}

    /** Accesseur sur la liste des bras gmds */
    std::vector<gmds::TCellID>& edges() {return m_mesh_data->edges();}

    /*------------------------------------------------------------------------*/
    /** Retourne un vecteur avec les positions des noeuds qui seront créés
     *  pour la discrétisation passée en paramètre
     *  On tient compte de la projection si demandé
     */
    void getPoints(CoEdgeMeshingProperty* prop,
    		std::vector<Utils::Math::Point> &points,
    		bool project) const;

    /** Retourne un vecteur avec les positions des noeuds qui sont ou seront créés
     *  Les points sont stockés dans le premesh s'il n'y sont pas déjà
     */
    void getPoints(std::vector<Utils::Math::Point> &points) const;

    /// nettoyage du preMesh (les points)
    void clearPoints();

    /// calcul du preMesh
    void computePoints();

    /*------------------------------------------------------------------------*/
    /** \brief   détruit l'objet (mais pas ses dépendances !) */
    virtual void setDestroyed(bool b);

    /*------------------------------------------------------------------------*/
    /** Accesseur sur l'état du maillage */
    virtual bool isMeshed() const {return m_mesh_data->isMeshed();}

    /** Accesseur sur l'état des points du maillage */
    virtual bool isPreMeshed() const;

    /*------------------------------------------------------------------------*/
    /** Constitue la liste des CoFaces adjacentes */
    void getCoFaces(std::vector<Topo::CoFace* >& cofaces) const;

    /// le nombre de cofaces adjacentes
    uint getNbCofaces() const;

    /** Booléen qui signale que l'on est en cours d'édition de l'objet
     * (une des propriétées a sa copie non nulle) */
    virtual bool isEdited() const;

    /// Calcul le ratio pour un point projeté sur une CoEdge (sur sa courbe s'il y a une association)
    virtual double computeRatio(const Utils::Math::Point& pt);

    /// Accesseur sur la liste de noms de groupes
    virtual void getGroupsName (std::vector<std::string>& gn, bool byGeom=true, bool byTopo=true) const;

    /// Accesseur sur le conteneur pour les groupes
    Utils::Container<Group::Group1D>& getGroupsContainer() {return m_topo_property->getGroupsContainer();}
    const Utils::Container<Group::Group1D>& getGroupsContainer() const {return m_topo_property->getGroupsContainer();}

    void getGroups(std::vector<Group::Group1D*>& grp) const
          {grp = getGroupsContainer().get();}

    /*------------------------------------------------------------------------*/
    /// Nombre de noeuds internes: le nombre de noeuds sans ceux du bord
    virtual unsigned long getNbInternalMeshingNodes();


private:
    /*------------------------------------------------------------------------*/
    /** Coupe une arête commune en trois
     * Propage cette modification (aux arêtes)
     *
     * Elle est marquée comme détruite
     * On retourne les 2 sommets créés
     *
     */
    std::vector<Topo::Vertex*> split(uint nbMeshingEdges1, uint nbMeshingEdges2, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /// Lève une exception si les coedges_ref dépendent de coedge
    void detectLoopReference(const Topo::CoEdge* coedge, std::vector<Topo::CoEdge*>& coedges_ref, std::set<const CoEdge*>& filtre_coedges) const;
    /// Lève une exception si les coedges_ref (en face de coedge via la coface) dépendent de coedge
    void detectLoopReference(const Topo::CoEdge* coedge_dep, const Topo::CoEdge* coedge, Topo::CoFace* coface, std::set<const CoEdge*>& filtre_coedges) const;

    /// reporte la date la plus récente des coedges sur l'arête de référence
    void updateModificationTime(const Topo::CoEdge* coedge_ref, std::vector<Topo::CoEdge*>& coedges) const;

    /*------------------------------------------------------------------------*/
    /// recherche un ensemble de coedges à partir des noms
    void getCoEdges(std::vector<std::string>& coedges_names,
    		std::vector<Topo::CoEdge*>& coedges) const;

    /*------------------------------------------------------------------------*/
    /// Création d'une courbe b-spline par projection de points de l'arête sur la surface
    Geom::Curve* createBSplineByProj(Utils::Math::Point& pt0, Utils::Math::Point& pt1, Geom::Surface* surface) const;
    Geom::Curve* createBSplineByProjWithOrthogonalIntersection(Utils::Math::Point& pt0, Utils::Math::Point& pt1, Geom::Surface* surface) const;

    /*------------------------------------------------------------------------*/

    /// Propriétés topologiques de l'arête (liens sur les faces et les sommets)
    CoEdgeTopoProperty* m_topo_property;

    /// Sauvegarde du m_topo_property
    CoEdgeTopoProperty* m_save_topo_property;

    /// Propriété de maillage (la discrétisation)
    CoEdgeMeshingProperty* m_mesh_property;

    /// Sauvegarde de m_mesh_property
    CoEdgeMeshingProperty* m_save_mesh_property;

    /// Etat et relation avec le maillage associé
    CoEdgeMeshingData* m_mesh_data;

    /// Sauvegarde de l'etat et relation avec le maillage associé
    CoEdgeMeshingData* m_save_mesh_data;

    /// Protection pour l'accès au premaillages (m_mesh_data->points())
    mutable TkUtil::Mutex   preMeshMutex;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* TOPO_COEDGE_H_ */
/*----------------------------------------------------------------------------*/
