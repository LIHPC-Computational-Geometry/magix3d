/*----------------------------------------------------------------------------*/
/** \file Edge.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 19/11/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef TOPO_EDGE_H_
#define TOPO_EDGE_H_
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#include "Topo/Vertex.h"
#include "Topo/Face.h"
#include "Topo/EdgeTopoProperty.h"
#include "Topo/EdgeMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace gmds {
class Node;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Internal {
class InfoCommand;
}

/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class Vertex;
class Face;
class CoEdge;
class CoEdgeMeshingProperty;
/*----------------------------------------------------------------------------*/
/**
   @brief Simple lien entre une face et une liste de CoEdges
 */
class Edge : public TopoEntity {

    static const char* typeNameTopoEdge;

public:

    /*------------------------------------------------------------------------*/
    /// Constructeur (avec une CoEdge)
    Edge(Internal::Context& ctx,
         CoEdge* ce);

    /// Constructeur (avec une liste de CoEdges et les 2 sommets extrémités)
    Edge(Internal::Context& ctx,
            Topo::Vertex* v1,
            Topo::Vertex* v2,
            std::vector<CoEdge* >& coedges);

    /*------------------------------------------------------------------------*/
    /// Constructeur par copie (interdit)
    Edge(const Edge& v);

    /*------------------------------------------------------------------------*/
    /// Constructeur par copie
    Edge* clone();

    /*------------------------------------------------------------------------*/
    /// Destructeur
    virtual ~Edge();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité topologique */
    int getDim() const {return 1;}

    /*------------------------------------------------------------------------*/
    /** Assure que l'ordre des arêtes communes est celui qui permet
     * d'aller du sommet 0 au 1 */
    void sortCoEdges();

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nombre de bras du maillage
    uint getNbMeshingEdges() const;

    /** Calcul le nombre de bras entre un sommet et un noeud déterminé
     * par une arête commune et un nombre de bras (depuis le début de cette CoEdge)
     * sens_coedge renseigne sur le sens de l'arête par rapport à celui de l'arête en partant du sommet v_dep
     */
    void computeCorrespondingNbMeshingEdges(Vertex* v_dep,
    		const CoEdge* coedge, uint nbBras_coedge,
            uint& nbBras_edge, bool& sens_coedge);

    /** Réciproque de la fonction précédente.
     * Recherche une arête commune et un nombre de bras (depuis le début de cette CoEdge)
     * en faisant un parcours sur l'arête depuis un sommet.
     */
    void computeCorrespondingCoEdgeAndNbMeshingEdges(Vertex* v_dep, uint nbBras_edge, bool coedge_after_cut,
            CoEdge*& coedge, uint& nbBras_coedge, bool& sens_coedge);

    /** retourne vrai si l'arête commune est dans le même sens que l'arête
     * lorsqu'elle est parcourue en partant du sommet spécifié
     */
    bool isSameSense(Vertex* vtx, CoEdge* coedge);

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nombre de noeuds
    uint getNbNodes() const;

    /*------------------------------------------------------------------------*/
    /// accesseur sur les propriétés de maillage, valable pour une Edge avec une seule CoEdge
    CoEdgeMeshingProperty* getMeshingProperty();
    const CoEdgeMeshingProperty* getMeshingProperty() const;

    /// Retourne le ratio avec une arête commune
    uint getRatio(CoEdge* ce) const
    { return m_mesh_property->getRatio(ce); }

    /// Modifie le ratio avec une arête commune
    void setRatio(CoEdge* ce, uint ratio)
    { m_mesh_property->setRatio(ce, ratio); }

    /// Modifie le ratio pour toutes les arêtes communes
    void setRatio(uint ratio);

    /*------------------------------------------------------------------------*/
    /** \brief   Suppression des dépendances (entités topologiques incidentes)
     */
    virtual void clearDependancy() {m_topo_property->clearDependancy();}

    /*------------------------------------------------------------------------*/
    /// ajoute une relation vers une face
    void addCoFace(CoFace* f)
    {m_topo_property->getCoFaceContainer().add(f);}

    /// enlève une relation vers une face
    void removeCoFace(CoFace* f, const bool exceptionIfNotFound=true)
    {m_topo_property->getCoFaceContainer().remove(f, exceptionIfNotFound);}

    /*------------------------------------------------------------------------*/
    /** Fournit l'accès aux faces topologiques incidentes avec une copie
     *
     *  \param faces les faces incidentes
     */
    void getCoFaces(std::vector<CoFace* >& faces) const
    {m_topo_property->getCoFaceContainer().get(faces);}

    /** Fournit l'accès aux faces topologiques incidentes sans copie
     */
    const std::vector<CoFace* > & getCoFaces() const
    {return m_topo_property->getCoFaceContainer().get();}

    /// fournit l'accès à l'une des faces communes
    CoFace* getCoFace(uint ind) const
    {return m_topo_property->getCoFaceContainer().get(ind);}

    /// \return le nombre de faces
    uint getNbCoFaces() const
    {return m_topo_property->getCoFaceContainer().getNb();}

    /*------------------------------------------------------------------------*/
    /** \brief Remplace un sommet v1 par le sommet v2
     *
     * Renvoie une exception si le sommet n'est pas trouvé
     *
     * Propage vers les faces
     */
    void replace(Vertex* v1, Vertex* v2, bool propagate_up, bool propagate_down, Internal::InfoCommand* icmd);

    /** \brief Remplace une arête commune e1 par e2
     *
     */
    virtual void replace(CoEdge* e1, CoEdge* e2, Internal::InfoCommand* icmd);

    /** \brief Remplace une arête commune e1 par un ensemble d'arêtes communes
     *
     */
    virtual void replace(CoEdge* e1, std::vector<CoEdge*>& edges, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** \brief Fusionne 2 arêtes ensembles
     *
     *  C'est au niveau du CoEdge qu'il est fait une fusion
     *  "ed" disparait
     *
     *  si updateOtherEdges vrai, alors les arêtes reliées à l'arête commune qui disparait sont mises à jour
     */
    void merge(Edge* ed, Internal::InfoCommand* icmd, bool updateOtherEdges);

    /*------------------------------------------------------------------------*/
    /** \brief Fussionne les 2 sommets de l'arête qui disparait ensuite
     *
     * Propage vers les autres entités
     */
    void collapse(Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Détruit et libère l'arête
     *
     * Elle est marquée comme détruite
     * et les liens dessus sont retirés
     */
    void free(Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Coupe une arête en deux ou trois suivant le[s] sommet[s] passé[s]
     * Ne propage pas cette modification (une CoFace n'ayant qu'une arête par côté logique)
     *
     * On retourne les nouvelles Edges
     *
     * Si la coupe passe par une extrémité, on conserve cette arête
     * et on retourne uniquement cette arête comme nouvelle arête
     */
    void split(std::vector<Vertex*> new_vtx, std::vector<Edge*>& new_edges, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux sommets topologiques incidents avec une copie
     *
     *  \param vertices les sommets incidents
     */
    void getVertices(std::vector<Vertex* >& vertices) const
    {m_topo_property->getVertexContainer().get(vertices);}

    /** Fournit l'accès aux sommets topologiques incidents sans copie
     */
    const std::vector<Vertex* >& getVertices() const
        {return m_topo_property->getVertexContainer().get();}

    /// fournit l'accès à l'un des sommets
    Vertex* getVertex(uint ind) const
    {return m_topo_property->getVertexContainer().get(ind);}

    /// \return le nombre de Vertex
    uint getNbVertices() const;

    /// vérifie si cette arête contient ou non ce sommet
    bool find(Vertex* v) const
    {return m_topo_property->getVertexContainer().find(v);}

    /// accès à tous les sommets y compris ceux internes
     void getAllVertices(std::vector<Topo::Vertex* >& vertices, const bool unique=true) const;

    /*------------------------------------------------------------------------*/
    /// ajoute une relation vers une arête
    void addCoEdge(CoEdge* e)
    {m_topo_property->getCoEdgeContainer().add(e);}

    /// enlève une relation vers une arête
    void removeCoEdge(CoEdge* e, const bool exceptionIfNotFound=true)
    {m_topo_property->getCoEdgeContainer().remove(e, exceptionIfNotFound);}

    /// Fournit l'accès aux arêtes topologiques qui utilisent cette CoEdge avec une copie
    void getCoEdges(std::vector<CoEdge* >& edges) const
    {m_topo_property->getCoEdgeContainer().get(edges);}

    /// Fournit l'accès aux arêtes topologiques qui utilisent cette CoEdge sans copie
    const std::vector<CoEdge* > & getCoEdges() const
    {return m_topo_property->getCoEdgeContainer().get();}

    /// le nombre d'arêtes communes utilisées par cette arête
    uint getNbCoEdges() const
    {return m_topo_property->getCoEdgeContainer().getNb();}

    /// fournit l'accès à l'une des arêtes communes
    CoEdge* getCoEdge(uint ind) const
    {return m_topo_property->getCoEdgeContainer().get(ind);}

    /// vérifie si cette arête contient ou non cette arête commune
    bool find(const CoEdge* e) const
    {return m_topo_property->getCoEdgeContainer().find(e);}

    /// retourne une arête à partir des 2 sommets qui la compose, exception sinon
    CoEdge* getCoEdge(Vertex* vtx1, Vertex* vtx2);

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux sommets topologiques opposé à un sommet
     *
     *  \param un sommet de l'arête
     *  \return le sommet opposé
     */
    Vertex* getOppositeVertex(Vertex* v);

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
     */
    virtual void check() const;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameTopoEdge;}

    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::TopoEdge;}

   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "Edge";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est une Edge suivant son nom
     */
    static bool isA(const std::string& name);

    /*------------------------------------------------------------------------*/
    /** Duplique le EdgeTopoProperty pour en conserver une copie
     *  (non modifiée par les accesseurs divers)
     *  Si le EdgeTopoProperty a déjà une copie de sauvegarde, il n'est rien fait
     *
     *  Si InfoCommand n'est pas nul, il est informé de la modification de l'entité
     */
    void saveEdgeTopoProperty(Internal::InfoCommand* icmd);

    /** Echange le EdgeTopoProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
    EdgeTopoProperty* setProperty(EdgeTopoProperty* prop);

    /*------------------------------------------------------------------------*/
    /** Duplique le EdgeMeshingProperty pour en conserver une copie
     */
    void saveEdgeMeshingProperty(Internal::InfoCommand* icmd);

    /** Echange le EdgeMeshingProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
    EdgeMeshingProperty* setProperty(EdgeMeshingProperty* prop);

    /*------------------------------------------------------------------------*/
    /** Transmet les propriétés sauvegardées
     *  dont la destruction est alors à la charge du CommandEditTopo
     *  On ne garde pas de lien dessus
     */
    virtual void  saveInternals(CommandEditTopo* cet);

    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Edge &);
    friend std::ostream & operator << (std::ostream & , const Edge &);

    /*------------------------------------------------------------------------*/
    /** Récupère des informations relatives à l'entité topologique */
    Topo::TopoInfo getInfos() const;

    /*------------------------------------------------------------------------*/
    /** Accesseur sur la liste des noeuds ordonnés suivant les sommets
     * \param nodes en sortie, la liste des pointeurs sur les noeuds
     */
    void getNodes(Vertex* v1, Vertex* v2,
            std::vector<gmds::Node> &nodes);

    /*------------------------------------------------------------------------*/
    /** Retourne un vecteur avec les positions des noeuds qui seront créés
     *  On tient compte de la projection
     */
    void getPoints(std::vector<Utils::Math::Point> &points) const;

    /*------------------------------------------------------------------------*/
    ///  Affecte un pointeur vers une entité géométrique (reporte la projection sur les arêtes communes)
    virtual void setGeomAssociation(Geom::GeomEntity* ge);

    /** Booléen qui signale que l'on est en cours d'édition de l'objet
     * (une des propriétées a sa copie non nulle) */
    virtual bool isEdited() const;

private:

    /*------------------------------------------------------------------------*/
    /** Coupe une arête en deux suivant le sommet passé
     */
    void split(Vertex* new_vtx, std::vector<Edge*>& new_edges, Internal::InfoCommand* icmd);

    /** Coupe une arête en trois suivant les sommets passés
     */
    void split(Vertex* new_vtx1, Vertex* new_vtx2, std::vector<Edge*>& new_edges, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/


    /// Propriétés topologiques de l'arête (liens sur la face et les 2 sommets)
    EdgeTopoProperty* m_topo_property;

    /// Sauvegarde du m_topo_property
    EdgeTopoProperty* m_save_topo_property;

    /// Propriété de maillage (les ratios par rapport aux discrétisations des arêtes communes)
    EdgeMeshingProperty* m_mesh_property;

    /// Sauvegarde de m_mesh_property
    EdgeMeshingProperty* m_save_mesh_property;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* TOPO_EDGE_H_ */
/*----------------------------------------------------------------------------*/
