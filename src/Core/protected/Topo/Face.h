/*----------------------------------------------------------------------------*/
/** \file Face
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 19/11/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef TOPO_FACE_H_
#define TOPO_FACE_H_
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#include "Topo/TopoEntity.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/Block.h"
#include "Topo/FaceTopoProperty.h"
#include "Topo/FaceMeshingProperty.h"
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
class CoEdge;
class Edge;
/*----------------------------------------------------------------------------*/
/**
   @brief Face de la topologie

   Visualisation des indices pour le cas structuré
   \image html face.gif


 */
class Face : public TopoEntity {

    static const char* typeNameTopoFace;

public:

    /// identification des arêtes pour le cas structuré
    enum eEdgeOnFace {i_min = 0, j_min, i_max, j_max};

    /// énumération des directions pour une face structurée
    enum eDirOnFace {i_dir, j_dir, undef_dir};

    /*------------------------------------------------------------------------*/
    /// Constructeur avec une seule face commune
    Face(Internal::Context& ctx,
            CoFace* coface);

    /** Constructeur avec une ou plusieurs faces communes,
    * on indique les sommets extrémité
    * on indique si l'ensemble est structuré
    */
    Face(Internal::Context& ctx,
            std::vector<CoFace* >& cofaces,
            std::vector<Vertex* >& vertices,
            bool isStr);

    /*------------------------------------------------------------------------*/
    /// Constructeur par copie
    Face(const Face& v);

    /*------------------------------------------------------------------------*/
    /// Destructeur
    virtual ~Face();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité topologique
     */
    int getDim() const {return 2;}

    /*------------------------------------------------------------------------*/
    /// Retourne le ratio avec une face commune
    uint getRatio(CoFace* cf, uint dir) const
    { return m_mesh_property->getRatio(cf, dir); }

    /// Modifie le ratio avec une face commune
    void setRatio(CoFace* cf, uint ratio, uint dir)
    { m_mesh_property->setRatio(cf, ratio, dir); }

    /*------------------------------------------------------------------------*/
    /** \brief   Suppression des dépendances (entités topologiques incidentes)
     */
    virtual void clearDependancy()
    {m_topo_property->clearDependancy();}

    /*------------------------------------------------------------------------*/
    /// ajoute une relation vers un bloc
    void addBlock(Block* b)
    {m_topo_property->getBlockContainer().add(b);}

    /// enlève une relation vers un bloc
    void removeBlock(Block* b, const bool exceptionIfNotFound=true)
    {m_topo_property->getBlockContainer().remove(b, exceptionIfNotFound);}

    /// le nombre de block incidents
    uint getNbBlocks() const
    {return m_topo_property->getBlockContainer().getNb();}

    /// accès à un des blocks
    Block* getBlock(uint ind) const
    {return m_topo_property->getBlockContainer().get(ind);}

    /** \brief  Fournit l'accès aux blocs topologiques incidents avec une copie
     *
     *  \param blocks les blocs incidents
     */
    void getBlocks(std::vector<Block* >& blocks) const
    {m_topo_property->getBlockContainer().get(blocks);}

    /** \brief  Fournit l'accès aux blocs topologiques incidents sans copie
    */
    const std::vector<Block* >& getBlocks() const
    {return m_topo_property->getBlockContainer().get();}

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

    /// retourne le nombre de sommets
    uint getNbVertices() const;

    /// vérifie si cette face contient ou non ce sommet
    bool find(Vertex* v) const
    {return m_topo_property->getVertexContainer().find(v);}

    /// retourne l'indice d'un sommet
    uint getIndex(Vertex* v) const
    {return m_topo_property->getVertexContainer().getIndex(v);}

    /// accès à tous les sommets y compris ceux internes
    void getAllVertices(std::vector<Topo::Vertex* >& vertices, const bool unique=true) const;


    /*------------------------------------------------------------------------*/
    void addCoFace(CoFace* f)
    {
        m_topo_property->getCoFaceContainer().add(f);
    }

    void addCoFaces(std::vector<CoFace*> faces)
    {
        m_topo_property->getCoFaceContainer().add(faces);
    }

    void removeCoFace(CoFace* f, const bool exceptionIfNotFound=true)
    {
        m_topo_property->getCoFaceContainer().remove(f, exceptionIfNotFound);
    }

    /** \brief  Fournit l'accès aux faces topologiques communes avec copie
     *
     *  \param faces les faces communes
     */
    void getCoFaces(std::vector<CoFace* >& faces) const
    {m_topo_property->getCoFaceContainer().get(faces);}

    /** Fournit l'accès aux faces topologiques incidentes sans copie
     */
    const std::vector<CoFace* > & getCoFaces() const
    {return m_topo_property->getCoFaceContainer().get();}

    /// Accesseur pour l'une des faces communes
    CoFace* getCoFace(uint id) const
    {return m_topo_property->getCoFaceContainer().get(id);}

    /// retourne le nombre de faces communes
    uint getNbCoFaces() const
    {return m_topo_property->getCoFaceContainer().getNb();}

    /*------------------------------------------------------------------------*/
    /** \brief Remplace un sommet v1 par le sommet v2
     *
     * Si la face contenait déjà le sommet v2, il est fait une reorientation de la face
     * et une suppression de v1 (la face est alors dégénérée en j_max)
     *
     * Propage vers les blocks si propagate_up
     * propage vers les coedges si propagate_down
      */
     void replace(Vertex* v1, Vertex* v2, bool propagate_up, bool propagate_down, Internal::InfoCommand* icmd);

     /** \brief Remplace une face commune cf1 par cf2
      *
      */
     void replace(CoFace* cf1, CoFace* cf2, Internal::InfoCommand* icmd);

     /** \brief Remplace une arête commune e1 par e2
      *
      */
     void replace(CoEdge* e1, CoEdge* e2, Internal::InfoCommand* icmd);

     /** \brief Fusionne 2 faces ensembles
      *
      *  Les relations avec les blocs sont mises à jour
      *  fa est remplacé par this
      */
     void merge(Face* fa, Internal::InfoCommand* icmd);

     /*------------------------------------------------------------------------*/
     /** Détruit et libère la face
      *
      * Elle est marquée comme détruite
      * et les liens dessus sont retirés
      */
     void free(Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /**  Fournit l'accès à l'arête topologiques opposé à une arête
     *
     *  \param e une arête de la face
     *  \return l'arête opposée ou 0 si la face n'est pas structurée
     */
    Edge* getOppositeEdge(Edge* e) const;

    /**  Retourne le sommet opposé à un sommet suivant une direction */
    Vertex* getOppositeVertex(Vertex* v, eDirOnFace& dir) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès au bloc topologiques opposé à un bloc s'il existe
     *
     *  \param b un bloc qui contient la face
     *  \return le bloc opposé ou 0 si la face n'est liée qu'à un bloc
     */
    Block* getOppositeBlock(Block* b) const;

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
    /** \brief  Indique si la face est structurée
     *  (toutes les faces communes doivent être structurées)
     */
    virtual bool isStructured() const;

    /** \brief  Retourne vrai si la face peut être structurée */
    virtual bool structurable();

    /** \brief  Retourne vrai si la face peut être non-structurée */
    virtual bool unstructurable();

    /** \brief  Rend la face structurée, lève une exception si ce n'est pas possible
     *
     * Attention, il est considéré que les sommets sont déjà ordonnés
     * Propage aux faces communes
     * */
    virtual void structure(Internal::InfoCommand* icmd);

    /** \brief  Rend la face non-structurée, lève une exception si ce n'est pas possible
     * Propage aux faces communes
     */
    virtual void unstructure(Internal::InfoCommand* icmd);

    /** Change la propriété stucturée ou non
     * et progage aux blocs */
    virtual void setStructured(Internal::InfoCommand* icmd, bool str);

    /** Vérification de la face
     * On lance la vérification sur les faces communes
     */
    virtual void check() const;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameTopoFace;}

    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::TopoFace;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
     */
    static std::string getTinyName() {return "Face";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est une Face suivant son nom
     */
    static bool isA(const std::string& name);
    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Face &);
    friend std::ostream & operator << (std::ostream & , const Face &);

    /*------------------------------------------------------------------------*/
    /** Récupère des informations relatives à l'entité topologique */
    Topo::TopoInfo getInfos() const;

    /*------------------------------------------------------------------------*/
    /** Accesseur sur la liste des noeuds ordonnés suivant les sommets
     * \param nodes en sortie, la liste des pointeurs sur les noeuds
     */
    void getNodes(Vertex* v1,
            Vertex* v2,
            Vertex* v3,
            Vertex* v4,
            std::vector<gmds::Node> &nodes);

    /** Accesseur sur la liste des noeuds ordonnés suivant un décalage et un sens
     * N'est utilisable que pour une face dégénérée en un triangle (une seule CoFace)
     * \param nodes en sortie, la liste des pointeurs sur les noeuds
     */
    void getNodes(uint decalage,
            bool sensDirecte,
            std::vector<gmds::Node> &nodes);

    /*------------------------------------------------------------------------*/
    /// retourne l'ensemble des arêtes toute directions confondues
    void getEdges(std::vector<Edge* >& edges) const;

    /// accès à l'une des arêtes suivant 2 sommets (recherchée parmis les CoFace)
    Edge* getEdge(Vertex* v1, Vertex* v2) const;

    /// retourne l'une des arêtes d'un côté pour le cas avec une seule face commune
    Edge* getEdge(uint ind) const;

    /*------------------------------------------------------------------------*/
    /// retourne l'ensemble des arêtes communes toute directions confondues
    void getCoEdges(std::vector<CoEdge* >& coedges) const;

    /*------------------------------------------------------------------------*/
    /// retourne les 2 listes de CoEdge, une par direction, pour une face structurée
    void getOrientedCoEdges(std::vector<CoEdge* > & iCoedges,
                            std::vector<CoEdge* > & jCoedges) const;

    /// retourne les nombres de bras suivant les 2 directions pour une face structurée
    void getNbMeshingEdges(uint& nbI, uint& nbJ, bool accept_error = false) const;

    /*------------------------------------------------------------------------*/
    /** Retrouve la direction dans une face structurée à partir de 2 sommets
     *  Ces sommets sont ceux de la face (pas des sommets internes liés à une CoFace)
     */
    eDirOnFace getDir(Vertex* v1, Vertex* v2) const;

    /*------------------------------------------------------------------------*/
    /** Fusionne deux faces en remplacant les noeuds de la face B par ceux de A (this)
     *
     *  Il est juste transmis aux faces communes la demande
     *  On est actuellement limité au cas avec une seule face commune de part et d'autre
     */
    virtual void fuse(Face* face_B,
            const std::vector<Vertex* >& l_sommets_A,
            const std::vector<Vertex* >& l_sommets_B,
            Internal::InfoCommand* icmd);

    /** Fonction équivalente qui recherche les noeuds qui sont en correspondance
     * c'est à dire ceux qui ont la même position géométrique
     */
    virtual void fuse(Face* face_B,
            Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Coupe une face en deux
     *
     * Se base sur la liste des Arêtes qui partitionnent en deux
     * Elle est marquée comme détruite
     *
     * Si la coupe passe par une extrémité, on conserve cette face commune
     */
    void split(std::vector<Edge* > & splitingEdges,
            Face* & face1,
            Face* & face2,
            Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Duplique le FaceTopoProperty pour en conserver une copie
     *  (non modifiée par les accesseurs divers)
     *  Si le FaceTopoProperty a déjà une copie de sauvegarde, il n'est rien fait
     *
     *  Si InfoCommand n'est pas nul, il est informé de la modification de l'entité
     */
    void saveFaceTopoProperty(Internal::InfoCommand* icmd);

    /** Echange le FaceTopoProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
    FaceTopoProperty* setProperty(FaceTopoProperty* prop);

    /*------------------------------------------------------------------------*/
    /** Duplique le FaceMeshingProperty pour en conserver une copie
     */
    void saveFaceMeshingProperty(Internal::InfoCommand* icmd);

    /** Echange le FaceMeshingProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
    FaceMeshingProperty* setProperty(FaceMeshingProperty* prop);

   /*------------------------------------------------------------------------*/
    /** Transmet les propriétés sauvegardées
     *  dont la destruction est alors à la charge du CommandEditTopo
     *  On ne garde pas de lien dessus
     */
    virtual void  saveInternals(CommandEditTopo* cet);

    /*------------------------------------------------------------------------*/
    /** Accesseur sur l'état du maillage */
//    virtual bool isMeshed() const;

    /*------------------------------------------------------------------------*/
    ///  Affecte un pointeur vers une entité géométrique (reporte la projection sur les faces communes)
    virtual void setGeomAssociation(Geom::GeomEntity* ge);

    /** Booléen qui signale que l'on est en cours d'édition de l'objet
     * (une des propriétées a sa copie non nulle) */
    virtual bool isEdited() const;

    /*------------------------------------------------------------------------*/
    /**
     *  Retourne le barycentre calculé sur la moyenne des Vertex
     */
    Utils::Math::Point getBarycentre() const;

private:
    /*------------------------------------------------------------------------*/
    /** Permute les sommets de manière à avoir v1 en premier et v2 en dernier
     */
    void _permuteToFirstAndLastVertices(Vertex* v1, Vertex* v2, Internal::InfoCommand* icmd);

    /// Permute les ratios de FaceMeshingProperty suite à _permuteToFirstAndLastVertices
    void _permuteMeshingRatios(Internal::InfoCommand* icmd);

    /// Fonction de calcul des nombres de bras en ayant déjà les listes orientées de CoEdges
    void _getNbMeshingEdges(uint& nbI, uint& nbJ,
            std::vector<CoEdge* > coedgesI,
            std::vector<CoEdge* > coedgesJ) const;

    /** Calcul le nombre de bras pour un côté d'une face
     * On Donne en entrée deux filtres sur les CoFace et Edge de cette face (marqués à 1)
     * On met à 2 le filtre pour les Edges vues
     * On ne modifie pas le filtre sur les CoFaces
     * On donne en entrée la liste des CoEdges sur le côté concerné ainsi que les 2 sommets extrémités
     * On retourne le nombre de bras en tenant compte des semi-conformités
     */
    uint _getNbMeshingEdges(std::map<Edge*, uint> &filtre_edges,
            std::map<CoFace*, uint> &filtre_cofaces,
            std::vector<CoEdge* > &coedges_cote,
            Vertex* v1, Vertex* v2) const;

private:

    /// Propriétés topologiques de la face (liens sur les blocs, arêtes et les sommets)
    FaceTopoProperty* m_topo_property;

    /// Sauvegarde du m_edge_topo_property
    FaceTopoProperty* m_save_topo_property;

    /// Propriété de maillage (les ratios par rapport aux discrétisations des faces communes)
    FaceMeshingProperty* m_mesh_property;

    /// Sauvegarde de m_edge_mesh_property
    FaceMeshingProperty* m_save_mesh_property;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* TOPO_FACE_H_ */
/*----------------------------------------------------------------------------*/
