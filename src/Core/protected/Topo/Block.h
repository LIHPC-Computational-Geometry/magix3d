/*----------------------------------------------------------------------------*/
/** \file Block
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22/11/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_TOPO_BLOCK_H_
#define MGX3D_TOPO_BLOCK_H_
/*----------------------------------------------------------------------------*/
#include <map>
/*----------------------------------------------------------------------------*/
#include "Topo/TopoEntity.h"
#include "Topo/BlockTopoProperty.h"
#include "Topo/BlockMeshingProperty.h"
#include "Topo/BlockMeshingData.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Internal {
class Context;
class InfoCommand;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class Vertex;
class Edge;
class CoEdge;
class Face;
class CoFace;
class CommandSetBlockMeshingProperty;
/*----------------------------------------------------------------------------*/
/**
   @brief Bloc de la topologie

   Visualisation des indices pour le cas structuré
   \image html block.gif

 */
class Block : public TopoEntity {

	friend class CommandSetBlockMeshingProperty;

    static const char* typeNameTopoBlock;

public:

    /*------------------------------------------------------------------------*/
    /// énumération des côtés d'un bloc structuré
    enum eFaceOnBlock {i_min, i_max, j_min, j_max, k_min, k_max};

    static std::string toString(eFaceOnBlock ind);

    /// énumération des directions pour un bloc structuré
    enum eDirOnBlock {i_dir, j_dir, k_dir, unknown};

    static std::string toString(eDirOnBlock ind);

    /*------------------------------------------------------------------------*/
    /** Constructeur avec des faces en nombre quelconque
     *
     *  Les sommets sont ordonnés pour le cas structuré (si nécessaire)
     */
    Block(Internal::Context& ctx,
            std::vector<Face* > &faces,
            std::vector<Topo::Vertex* > &vertices,
            bool isStructured = false);

    /*------------------------------------------------------------------------*/
    /// Constructeur par défaut, maillage structuré
    Block(Internal::Context& c, int ni=0, int nj=0, int nk=0,
            BlockMeshingProperty::meshLaw ml = BlockMeshingProperty::directional,
            BlockMeshingProperty::meshDirLaw md = BlockMeshingProperty::dir_i);

    /*------------------------------------------------------------------------*/
    /// Constructeur par copie
    Block(const Block& v);

    /*------------------------------------------------------------------------*/
    /// Destructeur
    virtual ~Block();

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité topologique
     */
    int getDim() const {return 3;}

	/*------------------------------------------------------------------------*/
    /** \brief   détruit l'objet (mais pas ses dépendances !)
     */
    virtual void setDestroyed(bool b);

    /*------------------------------------------------------------------------*/
    /** \brief   Suppression des dépendances (entités topologiques incidentes)
     */
    virtual void clearDependancy() {m_topo_property->clearDependancy();}

    /*------------------------------------------------------------------------*/
    /** \brief Remplace un sommet v1 par le sommet v2
     *
     */
    virtual void replace(Topo::Vertex* v1, Topo::Vertex* v2, bool propagate_up, bool propagate_down, Internal::InfoCommand* icmd);

    /** \brief Remplace une face f1 par f2
     *
     */
    virtual void replace(Face* f1, Face* f2, Internal::InfoCommand* icmd);


    /** \brief Remplace une arête commune e1 par e2
     *
     */
    virtual void replace(CoEdge* e1, CoEdge* e2, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Fusionne deux faces entre deux blocs
     *
     *  Cette fonction recherche les faces suivant les coordonnées des sommets
     *  qui sont confondus
     */
    void fuse(Block* bloc_B,
            Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Découpe un bloc en deux
     *
     *  Cette fonction découpe 4 arêtes communes, 4 faces communes
     *  et créé une face commune (ainsi que le deuxième bloc)
     *
     *  Les blocs créés sont mis dans newBlocs
     */
    void split(CoEdge* arete,
            double ratio,
            std::vector<Block* > & newBlocs,
            Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Découpe un bloc en deux en prolongeant la coupe d'une face suivant une arête
     */
    void extendSplit(CoEdge* arete,
    		std::vector<Block* > & newBlocs,
            Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Déraffine un bloc suivant une direction
     *
     *  Cette fonction modifie les ratios pour les arêtes ou les faces du bloc
     */
    void unrefine(eDirOnBlock dir,
            int ratio,
            Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Détruit et libère le bloc
     *
     * Il est marqué comme détruit
     * et les liens dessus sont retirés
     */
    void free(Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Fusionne les 4 sommets d'une face d'un bloc pour le dégénérer
     *  en une pyramide
     *
     *  Les faces et les sommets sont permutés si nécessaire pour que la face
     *  à  dégénérer soit celle en k_max (la dernière de la liste)
     *  Le bloc ne sera ensuite composé plus que de 5 sommets
     *
     *  \param id l'indice de la face
     */
    void degenerateFaceInVertex(uint id, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Fusionne les 4 sommets (deux à deux) d'une face d'un bloc pour le dégénérer
     *  en un prisme
     *
     *  Les faces et les sommets sont permutés si nécessaire pour que la face
     *  à  dégénérer soit celle en k_max (la dernière de la liste)
     *  Le bloc ne sera ensuite composé plus que de 6 sommets
     *
     *  v2 est remplacé par v1, de même pour les 2 autres sommets de la face
     *
     *  \param id l'indice de la face
     *  \param v1 un sommet de la face choisie
     *  \param v2 un deuxième sommet de la face choisie qui sera remplacé par v1
     */
    void degenerateFaceInEdge(uint id, Topo::Vertex* v1, Topo::Vertex* v2,
            Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Fournit l'accès aux sommets topologiques incidents avec une copie
     *
     *  \param vertices les sommets incidents
     */
    void getVertices(std::vector<Topo::Vertex* >& vertices) const
    {m_topo_property->getVertexContainer().get(vertices);}

    /** Fournit l'accès aux sommets topologiques incidents sans copie
     */
    const std::vector<Topo::Vertex* >& getVertices() const
            {return m_topo_property->getVertexContainer().get();}

    /// \return le nombre de Vertex
    uint getNbVertices() const
    {return m_topo_property->getVertexContainer().getNb();}

    /// fournit l'accès à l'un des sommets
    Topo::Vertex* getVertex(uint ind) const
    {return m_topo_property->getVertexContainer().get(ind);}

    /** Retourne le sommet suivant ses coordonnées logiques dans le bloc lorsqu'il est structuré
    @param cote_i faux pour i minimum
    @param cote_j faux pour j minimum
    @param cote_k faux pour k minimum
     */
    Topo::Vertex* getVertex(bool cote_i, bool cote_j, bool cote_k);

    /// retourne l'indice d'un sommet
    uint getIndex(Topo::Vertex* v)
    {return m_topo_property->getVertexContainer().getIndex(v);}

    /// retourne la liste des 8 sommets en duplicant les derniers en cas de dégénérescence
    void getHexaVertices(std::vector<Topo::Vertex* >& vertices) const;

    /// accès à tous les sommets y compris ceux internes
    void getAllVertices(std::vector<Topo::Vertex* >& vertices, bool unique = true) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux faces topologiques incidentes avec copie
     *
     *  \param faces les faces incidentes
     */
    void getFaces(std::vector<Face* >& faces) const
    {m_topo_property->getFaceContainer().get(faces);}

    /** \brief  Fournit l'accès aux faces topologiques incidentes sans copie
     */
    const std::vector<Face* > getFaces() const
            {return m_topo_property->getFaceContainer().get();}

    /// Accesseur pour l'une des faces
    Face* getFace(uint id) const
    {return m_topo_property->getFaceContainer().get(id);}

    /// retourne le nombre de faces
    uint getNbFaces() const
    {return m_topo_property->getFaceContainer().getNb();}

    /// retourne l'indice d'une face
    uint getIndex(Face* f)
    {return m_topo_property->getFaceContainer().getIndex(f);}

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux faces communes topologiques incidentes
     *
     *  \param cofaces les faces communes incidentes en retour
     */
    void getCoFaces(std::vector<CoFace* >& cofaces) const;

    uint getNbCoFaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux arêtes topologiques incidentes
     *
     *  \param edges les arêtes incidentes
     */
    void getEdges(std::vector<Edge* >& edges) const;

    uint getNbEdges() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux arêtes communes topologiques incidentes
     *
     *  \param coedges les arêtes communes incidentes
     */
    void getCoEdges(std::vector<CoEdge* >& coedges, bool unique = true) const;

    uint getNbCoEdges() const;

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
    /** \brief  Indique si le bloc est structuré
     */
    virtual bool isStructured() const {return m_mesh_property->isStructured();}

    /** \brief  Retourne vrai si le bloc peut être structuré */
    virtual bool structurable();

    /** \brief  Rend le bloc structuré, lève une exception sinon
     *
     * Attention, il est considéré que les sommets sont déjà ordonnés
     * */
    virtual void structure(Internal::InfoCommand* icmd);

    /** \brief  Rend le bloc non-structuré */
    virtual void unstructure(Internal::InfoCommand* icmd);

    /** Change la propriété stucturée ou non */
    virtual void setStructured(Internal::InfoCommand* icmd, bool str);


    /** Vérification du bloc
     * S'il est structuré, il faut que les faces soient toutes structurée
     * et que les arêtes pour une même direction aient toutes le même nombre de bras
     * S'il est non structuré, pas de contraintes
     * Les Face sont également vérifiées
     *
     * \return le nombre de mailles (calculé pour le cas structuré, 0 en non-structuré)
     */
    virtual int check() const;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameTopoBlock;}

    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::TopoBlock;}

    /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
     */
    static std::string getTinyName() {return "Bl";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est un Block suivant son nom
     */
    static bool isA(const std::string& name);

    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Block &);
    friend std::ostream & operator << (std::ostream & , const Block &);

    /*------------------------------------------------------------------------*/
    /** Récupère des informations relatives à l'entité topologique */
    Topo::TopoInfo getInfos() const;

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    virtual BlockMeshingProperty::meshLaw getMeshLaw() const {return m_mesh_property->getMeshLaw();}

    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return m_mesh_property->getMeshLawName();}

    /** spécifie le type de maillage pour un bloc et le propage aux faces
     *  on remplace la précédente méthode par nouvelle,
     *  on détruit cette ancienne méthode
     */
    virtual void setMeshLaw(BlockMeshingProperty* new_ppty);

    /** Recherche d'une méthode de maillage unidirectionnelle autant que possible,
     *  sinon on utilise la méthode transfinie
     */
    virtual void selectBasicMeshLaw(Internal::InfoCommand* icmd, bool forceCompute = false);

    /*------------------------------------------------------------------------*/
    /** Accesseur sur la liste des points */
    Utils::Math::Point*& points() {return m_mesh_data->points();}

    /** Accesseur sur la liste des noeuds gmds */
    std::vector<gmds::TCellID>& nodes() {return m_mesh_data->nodes();}

    /** Accesseur sur la liste des polyêdres gmds */
    std::vector<gmds::TCellID>& regions() {return m_mesh_data->regions();}

    /*------------------------------------------------------------------------*/
    //    /// retourne l'ensemble des arêtes pour une direction donnée
    // abandonné, car ne supporte pas le cas de plusieurs cofaces suivant la direction demandée
    //    void getEdges(eDirOnBlock dir, std::vector<Edge* > & edges) const;

    /// Retourne la direction d'une CoEdge dans un bloc
    eDirOnBlock getDir(CoEdge* ce);

    /*------------------------------------------------------------------------*/
    /// retourne les 3 listes de CoEdge, une par direction, pour un bloc structuré
    void getOrientedCoEdges(std::vector<CoEdge* > & iCoedges,
            std::vector<CoEdge* > & jCoedges,
            std::vector<CoEdge* > & kCoedges) const;

    /// retourne les nombres de bras suivant les 3 directions pour un bloc structuré
    void getNbMeshingEdges(uint& nbI, uint& nbJ, uint& nbK, bool accept_error = false) const;

    /*------------------------------------------------------------------------*/
    /** Duplique le BlockTopoProperty pour en conserver une copie
     *  (non modifiée par les accesseurs divers)
     *  Si le BlockTopoProperty a déjà une copie de sauvegarde, il n'est rien fait
     *
     *  Si InfoCommand n'est pas nul, il est informé de la modification de l'entité
     */
    void saveBlockTopoProperty(Internal::InfoCommand* icmd);

    /** Echange le BlockTopoProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
    BlockTopoProperty* setProperty(BlockTopoProperty* prop);

    /** Accesseur sur les propriétés topologique.
     *
     *  A utiliser avec précaution
     */
    BlockTopoProperty* getBlockTopoProperty()
    { return m_topo_property; }

    /*------------------------------------------------------------------------*/
    /** Duplique le BlockMeshingProperty pour en conserver une copie
     */
    void saveBlockMeshingProperty(Internal::InfoCommand* icmd);

    /** Remplace la propriété et met l'ancienne dans la sauvegarde interne
     */
    void switchBlockMeshingProperty(Internal::InfoCommand* icmd, BlockMeshingProperty* prop);

    /** Echange le BlockMeshingProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
    BlockMeshingProperty* setProperty(BlockMeshingProperty* prop);

    /** Accesseur sur le  BlockMeshingProperty
     *
     * A utiliser prudemment
     */
    BlockMeshingProperty* getBlockMeshingProperty()
    { return m_mesh_property; }
    const BlockMeshingProperty* getBlockMeshingProperty() const
    { return m_mesh_property; }

    /*------------------------------------------------------------------------*/
    /** Duplique le BlockMeshingData pour en conserver une copie
     */
    void saveBlockMeshingData(Internal::InfoCommand* icmd);

    /** Echange le BlockMeshingData avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */
    BlockMeshingData* setProperty(BlockMeshingData* prop);

    /// accesseur sur les propriétés de maillage
    BlockMeshingData* getMeshingData() {return m_mesh_data;}

    /*------------------------------------------------------------------------*/
    /** Transmet les propriétés sauvegardées
     *  dont la destruction est alors à la charge du CommandEditTopo
     *  On ne garde pas de lien dessus
     */
    virtual void saveInternals(CommandEditTopo* cet);

    /** Transmet les propriétés sauvegardées
     *  dont la destruction est alors à la charge du CommandCreateMesh
     *  On ne garde pas de lien dessus
     */
    virtual void  saveInternals(Mesh::CommandCreateMesh* ccm);

    /*------------------------------------------------------------------------*/
    /** Accesseur sur l'état du maillage */
    virtual bool isMeshed() const {return m_mesh_data->isMeshed();}

    /** Accesseur sur l'état des points du maillage */
    virtual bool isPreMeshed() const {return m_mesh_data->isPreMeshed();}

    /*------------------------------------------------------------------------*/
    /** Retourne le nombre de mailles si le bloc est maillé
     * Sinon, dans le cas structuré, le calcul
     */
    virtual int getNbRegions();

    /** Booléen qui signale que l'on est en cours d'édition de l'objet
     * (une des propriétées a sa copie non nulle) */
    virtual bool isEdited() const;

    /*------------------------------------------------------------------------*/
    /** Permute les faces et met à jour les sommets pour que la face d'indice id
     *  se retrouve en k_max
     *
     * \param id indice initial de la face
     */
    void permuteToKmaxFace(uint id, Internal::InfoCommand* icmd);

    /*------------------------------------------------------------------------*/
    /** Permute les faces et met à jour les sommets
     */
    void reverseOrder7Vertices();

    /*------------------------------------------------------------------------*/
    /**
     *  Renseigne une table de correspondance entre chacune des arêtes communes
     *  et le ratio pour ce bloc (ratio de semi-conformité)
     *
     *  Cette table est construite lors de cet appel
     *
     */
    void getRatios(std::map<CoEdge*,uint> &ratios);

    /*------------------------------------------------------------------------*/
    /**
     *  Retourne le barycentre calculé sur la moyenne des Vertex
     */
    Utils::Math::Point getBarycentre() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès à une face topologiques suivant 4 sommets spécifiés
     *
     */
    Face* getFace(Topo::Vertex* v0, Topo::Vertex* v1, Topo::Vertex* v2, Topo::Vertex* v3);

    /// Accesseur sur la liste de noms de groupes
    virtual void getGroupsName (std::vector<std::string>& gn, bool byGeom=true, bool byTopo=true) const;

    /// Accesseur sur le conteneur pour les groupes
    Utils::Container<Group::Group3D>& getGroupsContainer() {return m_topo_property->getGroupsContainer();}
    const Utils::Container<Group::Group3D>& getGroupsContainer() const {return m_topo_property->getGroupsContainer();}

    void getGroups(std::vector<Group::Group3D*>& g3) const
         {g3 = getGroupsContainer().get();}

    /*------------------------------------------------------------------------*/
    /// Nombre de noeuds internes: le nombre de noeuds sans ceux du bord
    virtual unsigned long getNbInternalMeshingNodes();

private:
    /*------------------------------------------------------------------------*/
    /// met à jour les associations
    void _init();

    /*------------------------------------------------------------------------*/
    /** Affecte à une face une méthode de maillage suivant une direction (ou rotation)
     *  si cela est possible
     *  c'est à dire, si cela suit une des arêtes de la face
     *  Ne fait rien si c'est orthogonal à la face
     *
     * \param new_ppty une méthode de maillage suivant une direction ou une rotation
     * \param dirCoedges les arêtes qui suivent la direction de la méthode de maillage
     * \param coface la face pour laquelle on cherche à mettre la même direction de maillage
     */
    void _setDirectionMeshLawToFace(BlockMeshingProperty* new_ppty,
            std::vector<Topo::CoEdge* >& dirCoedges,
            Topo::CoFace* coface);

    /*------------------------------------------------------------------------*/
    /// propriétés topologiques de l'arête (liens sur les faces et les sommets)
    BlockTopoProperty* m_topo_property;

    /// sauvegarde du m_topo_property
    BlockTopoProperty* m_save_topo_property;

    /// Propriété de maillage (la discrétisation)
    BlockMeshingProperty* m_mesh_property;

    /// Psauvegarde de m_mesh_property
    BlockMeshingProperty* m_save_mesh_property;

    /// Etat et relation avec le maillage associé
    BlockMeshingData* m_mesh_data;

    /// Sauvegarde de l'etat et relation avec le maillage associé
    BlockMeshingData* m_save_mesh_data;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_TOPO_BLOCK_H_ */
/*----------------------------------------------------------------------------*/
