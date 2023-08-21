/*----------------------------------------------------------------------------*/
/** \file CoFace
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 13/02/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef TOPO_COFACE_H_
#define TOPO_COFACE_H_
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#include "Topo/TopoEntity.h"
#include "Topo/CoFaceTopoProperty.h"
#include "Topo/CoFaceMeshingProperty.h"
#include "Topo/CoFaceMeshingData.h"
/*----------------------------------------------------------------------------*/
namespace gmds {
class Node;
}
namespace Mgx3D {

namespace Internal {
class InfoCommand;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class CoEdgeMeshingProperty;
class Vertex;
class CoEdge;
class Edge;
class Face;
class Block;
/*----------------------------------------------------------------------------*/
/**
   @brief Face commune de la topologie (partie commune entre deux blocs)

 */
class CoFace : public TopoEntity {

    static const char* typeNameTopoCoFace;

public:

    /// identification des arêtes pour le cas structuré
    enum eEdgeOnCoFace {i_min = 0, j_min, i_max, j_max};

    /// énumération des directions pour une face structuréeGeomInfo.h
    enum eDirOnCoFace {i_dir, j_dir};

    /*------------------------------------------------------------------------*/
    /// Constructeur avec des arêtes en nombre quelconque (cas non structuré par défaut)
    CoFace(Internal::Context& ctx,
         std::vector<Edge* > &edges,
         bool isStructured = false);

    /// Constructeur avec des arêtes et des noeuds en nombre quelconque (cas non structuré par défaut), noeuds et arêtes ordonnés
    CoFace(Internal::Context& ctx,
         std::vector<Edge* > &edges,
         std::vector<Topo::Vertex* > &vertices,
         bool isStructured,
         bool hasHole);

    /// Constructeur par défaut, maillage structuré
    CoFace(Internal::Context& c, int ni=0, int nj=0,
    		CoFaceMeshingProperty::meshLaw ml = CoFaceMeshingProperty::directional,
    		CoFaceMeshingProperty::meshDirLaw md = CoFaceMeshingProperty::dir_i);


    /*------------------------------------------------------------------------*/
    /// Constructeur pour le cas structuré avec 4 arêtes
    CoFace(Internal::Context& ctx,
         Edge* e1,
         Edge* e2,
         Edge* e3,
         Edge* e4);

    /// Constructeur pour le cas structuré dégénéré avec 3 arêtes
    CoFace(Internal::Context& ctx,
         Edge* e1,
         Edge* e2,
         Edge* e3);

    /*------------------------------------------------------------------------*/
    /// Constructeur par copie
    CoFace(const CoFace& v);

    /*------------------------------------------------------------------------*/
    /// Destructeur
    virtual ~CoFace();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité topologique
     */
    int getDim() const {return 2;}

    /*------------------------------------------------------------------------*/
    /** \brief   Suppression des dépendances (entités topologiques incidentes)
     */
    virtual void clearDependancy() {m_topo_property->clearDependancy();}


    /*------------------------------------------------------------------------*/
    /// ajoute une relation vers une face

    virtual void addFace(Face* f) {m_topo_property->getFaceContainer().add(f);}


    /// enlève une relation vers une face

    virtual void removeFace(Face* f, const bool exceptionIfNotFound=true)
    {m_topo_property->getFaceContainer().remove(f, exceptionIfNotFound);}


    /** \brief  Fournit l'accès aux faces topologiques qui référencent cette face commune
     *
     *  \param faces les faces incidentes
     */

    void getFaces(std::vector<Face* >& faces) const
    {m_topo_property->getFaceContainer().get(faces);}


    /** \brief  Fournit l'accès aux faces topologiques incidentes sans copie
     */

    const std::vector<Face* > getFaces() const
    {return m_topo_property->getFaceContainer().get();}



    uint getNbFaces() const
    { return m_topo_property->getFaceContainer().getNb(); }



    Face* getFace(uint ind) const
    { return m_topo_property->getFaceContainer().get(ind); }


    /*------------------------------------------------------------------------*/
    /** Constitue la liste des Blocs adjacents */
    void getBlocks(std::vector<Block* >& blocks) const;

    /** Retourne la taille de la liste des Blocs adjacents (en général soit 0 pour le 2D, soit 1, soit 2 pour les cofaces entres 2 blocs) */
    uint getNbBlocks() const;

    /** Renseigne sur le bloc adjacent à la face et la position de cette face par rapport au bloc (structuré) */
    TkUtil::UTF8String getInfoBlock() const;

    /*------------------------------------------------------------------------*/
    /** \brief Remplace un sommet v1 par le sommet v2
     *
     * Si la face contenait déjà le sommet v2, il est fait une reorientation de la face
     * et une suppression de v1 (la face est alors dégénérée en j_max)
     *
     * Propage vers les blocks si propagate_up
     */

    virtual void replace(Topo::Vertex* v1, Topo::Vertex* v2, bool propagate_up, bool propagate_down, Internal::InfoCommand* icmd);


    /** \brief Remplace une arête e1 par e2
     *
     */

   virtual void replace(Edge* e1, Edge* e2, Internal::InfoCommand* icmd);


   /** \brief Remplace une arête commune e1 par e2
    *
    */
   virtual void replace(CoEdge* e1, CoEdge* e2, Internal::InfoCommand* icmd);

    /** \brief Fusionne 2 faces ensembles
     *
     *  Les relations avec les blocs sont mises à jour
     *  fa est remplacé par this
     */

    void merge(CoFace* fa, Internal::InfoCommand* icmd);


    /*------------------------------------------------------------------------*/
    /** Détruit et libère la face commune
     *
     * Elle est marquée comme détruite
     * et les liens dessus sont retirés
     */

    void free(Internal::InfoCommand* icmd);


    /*------------------------------------------------------------------------*/
    /** Coupe une face commune en deux ou trois
     * Propage cette modification (aux faces)
     *
     * Elle est marquée comme détruite
     * On retourne les arêtes créées
     *
     * Si la coupe passe par une extrémité, on conserve cette face commune
     * et on retourne l'arête à cette extrémité
     *
     * Sont données les nouvelles arêtes et les sommets le long de la coupe
     */

    std::vector<Edge*> split(eDirOnCoFace dir, std::vector<Edge*>& edges1, std::vector<Edge*>& edges3,
            std::vector<Topo::Vertex*>& vtx1, std::vector<Topo::Vertex*>& vtx2,
            Internal::InfoCommand* icmd);


    /*------------------------------------------------------------------------*/
    /** Coupe une face commune dégénérée en trois ou quatre
     * Ce n'est adapté qu'au cas 2D avec découpage par insertion d'un o-grid
     * Ne propage pas cette modification (aux faces)
     *
     * Elle est marquée comme détruite
     *
     * Sont données les nouvelles arêtes et les sommets le long de la coupe
     */

    void splitOgrid(eDirOnCoFace dir,
            std::vector<Edge*>& edges0,
            std::vector<Edge*>& edges1,
            std::vector<Edge*>& edges2,
            std::vector<Topo::Vertex*>& vtx0,
            std::vector<Topo::Vertex*>& vtx1,
            std::vector<Topo::Vertex*>& vtx2,
            double& ratio,
            Internal::InfoCommand* icmd);


    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux sommets topologiques incidents
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

    /// retourne le nombre de sommets
    uint getNbVertices() const;

    /// retourne l'indice d'un sommet
    uint getIndex(Topo::Vertex* v)
    {return m_topo_property->getVertexContainer().getIndex(v);}

    /// vérifie si cette face commune contient ou non ce sommet
     bool find(Topo::Vertex* v) const
     {return m_topo_property->getVertexContainer().find(v);}

     /// accès à tous les sommets y compris ceux internes aux Edges
     void getAllVertices(std::vector<Topo::Vertex* >& vertices, const bool unique=true) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux arêtes topologiques incidentes avec copie
     *
     *  \param edges les arêtes incidentes
     */

    void getEdges(std::vector<Edge* >& edges) const
       {m_topo_property->getEdgeContainer().get(edges);}


    /// Fournit l'accès aux arêtes topologiques qui utilisent cette CoEdge sans copie

    const std::vector<Edge* > & getEdges() const
    {return m_topo_property->getEdgeContainer().get();}


    /// fournit l'accès à l'une des arêtes

    Edge* getEdge(uint ind) const
       {return m_topo_property->getEdgeContainer().get(ind);}


    /// retourne le nombre d'arêtes

    uint getNbEdges() const {return m_topo_property->getEdgeContainer().getNb();}


    /// retourne l'indice d'une arête

    uint getIndex(Edge* e) const
    {return m_topo_property->getEdgeContainer().getIndex(e);}


    /// enlève une relation vers une arête

    void removeEdge(Edge* e, const bool exceptionIfNotFound=true)
    {m_topo_property->getEdgeContainer().remove(e, exceptionIfNotFound);}


    /// accès à l'une des arêtes suivant 2 sommets

    Edge* getEdge(Topo::Vertex* v1, Topo::Vertex* v2) const;


    /// retourne l'arête contenant une arête commune, lève une exception en cas d'erreur

    Edge* getEdgeContaining(const CoEdge* coedge) const;


    /// accesseur sur la présence ou non de trous
    bool hasHole() { return m_topo_property->hasHole();}

    /// retourne l'indice du côté contenant une arête commune
    uint getIndex(CoEdge* coedge) const
    { return getIndex(getEdgeContaining(coedge)); }

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès à l'arête topologiques opposé à une arête
     *
     *  \param e une arête de la face
     *  \return l'arête opposée ou 0 si la face n'est pas structurée
     */

    Edge* getOppositeEdge(Edge* e) const;



    /*------------------------------------------------------------------------*/
    /** \brief  Retourne l'arête commune qui fait face à une arête donnée.
     * Cette arête doit faire parfaitement face, les sommets se font face au niveau
     * de la discrétisation.
     * On retourne égallement une information sur le sens des 2 arêtes
     * La face doit être structurée.
     *
     *  \param coedge_dep une arête commune de la face
     *  \param inverse_sens [modifié]
     *  \return l'arête commune opposée ou 0 si non trouvé
     */

    CoEdge* getOppositeCoEdge(CoEdge* coedge_dep, bool& inverse_sens) const;


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
     */
    virtual bool isStructured() const;

    /** \brief  Retourne vrai si la face peut être structurée */
    virtual bool structurable();

    /** \brief  Retourne vrai si la face peut être non-structurée */
    virtual bool unstructurable();

    /** \brief  Rend la face structurée, lève une exception si ce n'est pas possible
     *
     * Attention, il est considéré que les sommets sont déjà ordonnés
     * */

    virtual void structure(Internal::InfoCommand* icmd);


    /** \brief  Rend la face non-structurée, lève une exception si ce n'est pas possible */

    virtual void unstructure(Internal::InfoCommand* icmd);


    /** Vérification de la face
     * Si elle est structurée, il faut
     * que les arêtes pour une même direction aient toutes le même nombre de bras
     * Si elle n'est pas structurée, pas de contraintes
     * retourne le nombre de polygones pour le cas structuré
     */
    virtual uint check() const;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameTopoCoFace;}

    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::TopoCoFace;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
     */
    static std::string getTinyName() {return "Fa";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est une CoFace suivant son nom
     */
    static bool isA(const std::string& name);

    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations

    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const CoFace &);
    friend std::ostream & operator << (std::ostream & , const CoFace &);

    /*------------------------------------------------------------------------*/
    /** Récupère des informations relatives à l'entité topologique */
    Topo::TopoInfo getInfos() const;

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le type de maillage

    virtual CoFaceMeshingProperty::meshLaw getMeshLaw() const {return m_mesh_property->getMeshLaw();}


    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return m_mesh_property->getMeshLawName();}

    /** Recherche d'une méthode de maillage unidirectionnelle autant que possible,
     *  sinon on utilise la méthode transfinie
     */

    virtual void selectBasicMeshLaw(Internal::InfoCommand* icmd, bool forceCompute = false);


    /*------------------------------------------------------------------------*/
    /** Accesseur sur la liste des noeuds ordonnés suivant les sommets
     * \param nodes en sortie, la liste des pointeurs sur les noeuds
     */

    void getNodes(Topo::Vertex* v1,
            Topo::Vertex* v2,
            Topo::Vertex* v3,
            Topo::Vertex* v4,
            std::vector<gmds::Node> &nodes);


    /** Accesseur sur la liste des noeuds ordonnés suivant les sommets
     * \param nodes en sortie, la liste des pointeurs sur les noeuds
     */

    void getNodes(uint decalage,
            bool sensDirecte,
            std::vector<gmds::Node> &nodes);


    /*------------------------------------------------------------------------*/
    /** Accesseur sur la liste des points */
    Utils::Math::Point*& points() {return m_mesh_data->points();}

     /** Accesseur sur la liste des noeuds gmds */

    std::vector<gmds::TCellID>& nodes() { return m_mesh_data->nodes();}


    /** Accesseur sur la liste des faces gmds */

    std::vector<gmds::TCellID>& faces() {return m_mesh_data->faces();}


    /*------------------------------------------------------------------------*/
    /// retourne l'ensemble des arêtes pour une direction donnée (une ou deux suivant que c'est dégénéré ou non)

    virtual void getEdges(eDirOnCoFace dir, std::vector<Edge* > & edges) const;


    /*------------------------------------------------------------------------*/
    /// retourne les 2 listes de CoEdge, une par direction, pour une face commune structurée

    void getOrientedCoEdges(std::vector<CoEdge* > & iCoedges,
                            std::vector<CoEdge* > & jCoedges) const;


    /// retourne les nombres de bras suivant les 2 directions pour une face commune structurée

    void getNbMeshingEdges(uint& nbI, uint& nbJ) const;


    /// retourne l'ensemble des arêtes communes toute directions confondues
    void getCoEdges(std::vector<CoEdge* >& coedges, bool unique = true) const;

    /*------------------------------------------------------------------------*/
    /** Retrouve la direction dans une coface structurée à partir de 2 sommets
     *  Ces sommets sont ceux de la coface (pas des sommets internes liés à une CoEdge)
     */
    eDirOnCoFace getDir(Vertex* v1, Vertex* v2) const;

    /*------------------------------------------------------------------------*/
    /** retourne la propriété de maillage pour les arêtes suivant une direction
     *  Cela n'est valable qu'à condition qu'il y est une arête qui ne soit pas composée
     *  suivant cette direction.
     *  Renseigne sur l'inversion de sens de parcours de la face / arête
     */
    CoEdgeMeshingProperty* getMeshingProperty(eDirOnCoFace dir, bool & is_inverted);

    /*------------------------------------------------------------------------*/
    /** Fusionne deux faces en remplacant les noeuds de la face B par ceux de A (this)
     *
     *  Cette fonction est une version simplifiée de bibop3d::Bloc::fusionFaces
     *  (pas de semi-conformité ni de Face composée)
     */

   virtual void fuse(CoFace* face_B,
            const std::vector<Topo::Vertex* >& l_sommets_A,
            const std::vector<Topo::Vertex* >& l_sommets_B,
            Internal::InfoCommand* icmd);


    /** Fonction équivalente qui recherche les noeuds qui sont en correspondance
     * c'est à dire ceux qui ont la même position géométrique
     */

    virtual void fuse(CoFace* face_B,
            Internal::InfoCommand* icmd);


    /*------------------------------------------------------------------------*/
    /** Duplique le CoFaceTopoProperty pour en conserver une copie
     *  (non modifiée par les accesseurs divers)
     *  Si le CoFaceTopoProperty a déjà une copie de sauvegarde, il n'est rien fait
     *
     *  Si InfoCommand n'est pas nul, il est informé de la modification de l'entité
     */

    void saveCoFaceTopoProperty(Internal::InfoCommand* icmd);


    /** Echange le CoFaceTopoProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */

    CoFaceTopoProperty* setProperty(CoFaceTopoProperty* prop);


    /*------------------------------------------------------------------------*/
    /** Duplique le CoFaceMeshingProperty pour en conserver une copie
     */

    void saveCoFaceMeshingProperty(Internal::InfoCommand* icmd);


    /** Remplace la propriété et met l'ancienne dans la sauvegarde interne
     */

    void switchCoFaceMeshingProperty(Internal::InfoCommand* icmd, CoFaceMeshingProperty* prop);


    /** Echange le CoFaceMeshingProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */

    CoFaceMeshingProperty* setProperty(CoFaceMeshingProperty* prop);


    /** Accesseur sur le  CoFaceMeshingProperty
     *
     * A utiliser prudemment
     */

    CoFaceMeshingProperty* getCoFaceMeshingProperty()
    { return m_mesh_property; }
    const CoFaceMeshingProperty* getCoFaceMeshingProperty() const
    { return m_mesh_property; }


    /*------------------------------------------------------------------------*/
    /** Duplique le CoFaceMeshingData pour en conserver une copie
     */

    void saveCoFaceMeshingData(Internal::InfoCommand* icmd);


    /** Echange le CoFaceMeshingData avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */

   CoFaceMeshingData* setProperty(CoFaceMeshingData* prop);


    /// accesseur sur les propriétés de maillage

    CoFaceMeshingData* getMeshingData() {return m_mesh_data;}


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
    /** \brief   détruit l'objet (mais pas ses dépendances !) */

    virtual void setDestroyed(bool b);


    /*------------------------------------------------------------------------*/
    /** Accesseur sur l'état du maillage */
    virtual bool isMeshed() const {return m_mesh_data->isMeshed();}

    /** Accesseur sur l'état des points du maillage */
    virtual bool isPreMeshed() const {return m_mesh_data->isPreMeshed();}

    /// Retourne la liste de faces communes voisines (avec une CoEdge en commun)
    void getNeighbour(std::vector<CoFace* >& cofaces);

    /*------------------------------------------------------------------------*/
    /** Retourne le nombre de mailles si la face est maillée
     * Sinon, dans le cas structuré, le calcul
     */
    virtual int getNbMeshingFaces();

    /** Booléen qui signale que l'on est en cours d'édition de l'objet
     * (une des propriétées a sa copie non nulle) */
    virtual bool isEdited() const;

    /*------------------------------------------------------------------------*/
    /**
     *  Retourne le barycentre calculé sur la moyenne des Vertex
     */

    Utils::Math::Point getBarycentre() const;


    /// Accesseur sur la liste de noms de groupes
    virtual void getGroupsName (std::vector<std::string>& gn, bool byGeom=true, bool byTopo=true) const;

    /// Accesseur sur le conteneur pour les groupes
    Utils::Container<Group::Group2D>& getGroupsContainer() {return m_topo_property->getGroupsContainer();}
    const Utils::Container<Group::Group2D>& getGroupsContainer() const {return m_topo_property->getGroupsContainer();}

    void getGroups(std::vector<Group::Group2D*>& grp) const
          {grp = getGroupsContainer().get();}

    /*------------------------------------------------------------------------*/
    /// Nombre de noeuds internes: le nombre de noeuds sans ceux du bord
    virtual unsigned long getNbInternalMeshingNodes();

private:

    /*------------------------------------------------------------------------*/
    /// étape d'initialisation commune aux différents constructeurs
    void init(std::vector<Edge* > &edges);

    /// étape d'initialisation commune aux différents constructeurs sans recherche des sommets (ni tri)
    void init(std::vector<Edge* > &edges,
            std::vector<Topo::Vertex* > &vertices);

    /*------------------------------------------------------------------------*/
    /// étape à l'initialisation qui réordonne les arêtes de manières cycliques
    void sortEdges();


    /*------------------------------------------------------------------------*/
    /** Permute les arêtes et met à jour les sommets pour que l'arête
     *  se retrouve en j_max (cas structuré)
     *
     * \param edge l'arête à mettre en j_max
     */
    void permuteToJmaxEdge(Edge* edge, Internal::InfoCommand* icmd);

    /** Permute les arêtes et met à jour les sommets pour que l'arête
     *  se retrouve en dernière (cas non structuré)
     */
    void permuteToLastEdge(Edge* edge, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Coupe une face commune en deux
     * Propage cette modification (aux faces)
     *
     * Elle est marquée comme détruite
     * On retourne l'arête créée
     *
     * Sont données les nouvelles arêtes et les sommets le long de la coupe
     */
    std::vector<Edge*> split2(eDirOnCoFace dir,
            std::vector<Edge*>& edges1,
            std::vector<Edge*>& edges3,
            std::vector<Topo::Vertex*> vtx1,
            std::vector<Topo::Vertex*> vtx2,
            Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Coupe une face commune en trois
     * Propage cette modification (aux faces)
     *
     * Elle est marquée comme détruite
     * On retourne les 2 arêtes créées
     *
     * Sont données les nouvelles arêtes et les sommets le long de la coupe
     */
    std::vector<Edge*> split3(eDirOnCoFace dir,
            std::vector<Edge*>& edges1,
            std::vector<Edge*>& edges3,
            std::vector<Topo::Vertex*> vtx1,
            std::vector<Topo::Vertex*> vtx2,
            Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /// met à jour les interpolations se basant sur cette coface qui est découpée
    void MAJInterpolated(CoFace* newCoface, std::vector<Edge*>& newEdges);

    void MAJInterpolated(CoEdgeMeshingProperty* cemp);

private:


    /// Propriétés topologiques de la face (liens sur les blocs, arêtes et les sommets)
    CoFaceTopoProperty* m_topo_property;

    /// Sauvegarde du m_edge_topo_property
    CoFaceTopoProperty* m_save_topo_property;

    /// Propriété de maillage (la discrétisation)
    CoFaceMeshingProperty* m_mesh_property;

    /// Sauvegarde de m_edge_mesh_property
    CoFaceMeshingProperty* m_save_mesh_property;

    /// Etat et relation avec le maillage associé
    CoFaceMeshingData* m_mesh_data;

    /// Sauvegarde de l'etat et relation avec le maillage associé
    CoFaceMeshingData* m_save_mesh_data;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* TOPO_COFACE_H_ */
/*----------------------------------------------------------------------------*/
