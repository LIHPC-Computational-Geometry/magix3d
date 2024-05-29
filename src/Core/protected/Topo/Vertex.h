/*----------------------------------------------------------------------------*/
/** \file Vertex.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef TOPO_VERTEX_H_
#define TOPO_VERTEX_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <map>
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
#include "Topo/TopoEntity.h"
#include "Topo/VertexTopoProperty.h"
#include "Topo/VertexGeomProperty.h"
#include "Topo/VertexMeshingData.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Internal {
class InfoCommand;
}

/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class CoEdge;
class CoFace;
class Block;
/*----------------------------------------------------------------------------*/
/**
   @brief Sommet de la topologie

   On change le nom de la classe pour les pythoneries, sinon il y a un conflit avec Geom::Vertex
 */
class Vertex : public TopoEntity {

    static const char* typeNameTopoVertex;

public:

    /// Constructeur par défaut (sommet en 0,0,0)

    Vertex(Internal::Context& ctx);


    /// Constructeur avec sommet en pt

    Vertex(Internal::Context& ctx,
           const Utils::Math::Point& pt);


    /// Constructeur par copie interdit

    Vertex(const Vertex& v);


    /// Constructeur par copie, sans relation topologique ni maillage

    Vertex* clone();


    /// Destructeur
    virtual ~Vertex();

    /// Opérateur de copie

    Vertex & operator = (const Vertex& v);


    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité topologique
     */
    int getDim() const {return 0;}

    /*------------------------------------------------------------------------*/
    /// accès à tous les sommets (un seul pour un sommet)
    void getAllVertices(std::vector<Topo::Vertex* >& vertices, const bool unique=true) const;

    /*------------------------------------------------------------------------*/
    /** \brief   Suppression des dépendances (entités topologiques incidentes)
     */

    virtual void clearDependancy() {m_topo_property->clearDependancy();}


    /*------------------------------------------------------------------------*/
    /// Accesseur sur la coordonnée en X
    double getX() const {return m_geom_property->getCoord().getX();}

    /// Accesseur sur la coordonnée en Y
    double getY() const {return m_geom_property->getCoord().getY();}

    /// Accesseur sur la coordonnée en Z
    double getZ() const {return m_geom_property->getCoord().getZ();}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur les coordonnées
    Utils::Math::Point getCoord() const;

    /*------------------------------------------------------------------------*/
    /** Modificateur sur les coordonnées
     *  Attention à sauvegarder pour le undo
     *  \see saveVertexTopoProperty
     */

    void setCoord(const Utils::Math::Point & pt, bool acceptMeshed = false);


    /*------------------------------------------------------------------------*/
    /**  Ajoute une relation vers une arête commune
     * Attention à sauvegarder pour le undo
     * \see saveVertexTopoProperty
     */

    void addCoEdge(CoEdge* e)
    {m_topo_property->getCoEdgeContainer().add(e);}


    /*------------------------------------------------------------------------*/
    /** \brief Enlève une relation vers une arête commune
     *
     * La relation doit exister sinon une exception est levée
     * Attention à sauvegarder pour le undo
     * \see saveVertexTopoProperty
     */

   void removeCoEdge(CoEdge* e, const bool exceptionIfNotFound=true)
    {m_topo_property->getCoEdgeContainer().remove(e, exceptionIfNotFound);}


    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux arêtes topologiques communes incidentes avec copie
     *
     *  \param edges les arêtes incidentes
     */
    void getCoEdges(std::vector<CoEdge* >& edges) const
    {m_topo_property->getCoEdgeContainer().get(edges);}

    /// Fournit l'accès aux arêtes topologiques communes incidentes sans copie
    const std::vector<CoEdge* > & getCoEdges() const
    {return m_topo_property->getCoEdgeContainer().get();}

    /*------------------------------------------------------------------------*/
    /// fournit l'accès à l'une des arêtes communes
    CoEdge* getCoEdge(uint ind) const
    {return m_topo_property->getCoEdgeContainer().get(ind);}

    /// retourne le nombre d'arêtes communes
    uint getNbCoEdges() const
    {return m_topo_property->getCoEdgeContainer().getNb();}

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux arêtes topologiques incidentes
     *
     *  \param edges les arêtes incidentes
     */

    void getEdges(std::vector<Edge* >& edges) const;


    /*------------------------------------------------------------------------*/
    /** Constitue la liste des CoFaces adjacentes */
    void getCoFaces(std::vector<CoFace* >& cofaces) const;

    /*------------------------------------------------------------------------*/
    /** Constitue la liste des Blocs adjacents */
    void getBlocks(std::vector<Block* >& blocks) const;

    /*------------------------------------------------------------------------*/
    /** \brief Fusionne 2 sommets ensembles
     *
     *  Les relations avec les arêtes sont mises à jour
     *  ve est remplacé par this
     *
     *  A faire dans le cadre d'une fusion entre topo initiallement disjointes
     */

    void merge(Vertex* ve, Internal::InfoCommand* icmd);


    /*------------------------------------------------------------------------*/
    /** Détruit et libère le sommet
     *
     * Il est marqué comme détruit
     * et les liens dessus sont retirés
     */

    void free(Internal::InfoCommand* icmd);


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
    /** \brief  Fournit un résumé textuel de l'entité.
     */

    virtual std::string getSummary ( ) const;


    /*------------------------------------------------------------------------*/
    /** Vérification du sommet
     * Il est juste vérifié qu'il n'est pas détruit
     */
    virtual void check() const;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameTopoVertex;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::TopoVertex;}

    /*------------------------------------------------------------------------*/
   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "Som";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est un Vertex suivant son nom
     */
    static bool isA(const std::string& name);

    /*------------------------------------------------------------------------*/
    /** Duplique le VertexTopoProperty pour en conserver une copie
     *  (non modifiée par les accesseurs divers)
     *  Si le VertexTopoProperty a déjà une copie de sauvegarde, il n'est rien fait
     *
     *  Si InfoCommand n'est pas nul, il est informé de la modification de l'entité
     */

    void saveVertexTopoProperty(Internal::InfoCommand* icmd);


    /** Echange le VertexTopoProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */

    VertexTopoProperty* setProperty(VertexTopoProperty* prop);


    /*------------------------------------------------------------------------*/
    /** Duplique le VertexGeomProperty pour en conserver une copie
     *
     * \param propagate à vrai si l'on souhaite propager l'info sur la modification
     * géométrique aux entités qui s'appuient sur ce sommets
     *
     * \see saveVertexTopoProperty
     */

    void saveVertexGeomProperty(Internal::InfoCommand* icmd, bool propagate=false);


    /** Echange le VertexGeomProperty avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */

    VertexGeomProperty* setProperty(VertexGeomProperty* prop);


    /*------------------------------------------------------------------------*/
    /** Duplique le VertexMeshingData pour en conserver une copie
     */

    void saveVertexMeshingData(Internal::InfoCommand* icmd);


    /** Echange le VertexMeshingData avec celui donné, retourne l'ancien
     * C'est à l'appelant de le détruire
     */

    VertexMeshingData* setProperty(VertexMeshingData* prop);


    /// accesseur sur les propriétés de maillage

    VertexMeshingData* getMeshingData() {return m_mesh_data;}


    /*------------------------------------------------------------------------*/
    /** Modificateur sur le noeud gmds */

    void setNode(gmds::TCellID nd) {m_mesh_data->setNode(nd);}


    /** Accesseur sur le noeud */

    gmds::TCellID getNode();


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

    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Vertex &);
    friend std::ostream & operator << (std::ostream & , const Vertex &);

    /*------------------------------------------------------------------------*/
    /** Récupère des informations relatives à l'entité topologique */
    Topo::TopoInfo getInfos() const;

    /*------------------------------------------------------------------------*/
    /** \brief   détruit l'objet (mais pas ses dépendances !) */

    virtual void setDestroyed(bool b);


    /*------------------------------------------------------------------------*/
    /** Accesseur sur l'état du maillage */
    virtual bool isMeshed() const {return m_mesh_data->isMeshed();}

    /*------------------------------------------------------------------------*/
    /** Recherche les Vertex confondus entre deux liste et retourne le
     *  résultat sous forme d'association de A vers B
     *
     *  corr_vertex_A_B est modifié
     */

    static void findNearlyVertices(const std::vector<Vertex* >& vertices_A,
            const std::vector<Vertex* >& vertices_B,
            std::map<Topo::Vertex*, Topo::Vertex*>& corr_vertex_A_B);


    /** Booléen qui signale que l'on est en cours d'édition de l'objet
     * (une des propriétées a sa copie non nulle) */
    virtual bool isEdited() const;

    /// Accesseur sur la liste de noms de groupes
    virtual void getGroupsName (std::vector<std::string>& gn, bool byGeom=true, bool byTopo=true) const;

    /// Accesseur sur le conteneur pour les groupes
    Utils::Container<Group::Group0D>& getGroupsContainer() {return m_topo_property->getGroupsContainer();}
    const Utils::Container<Group::Group0D>& getGroupsContainer() const {return m_topo_property->getGroupsContainer();}

    void getGroups(std::vector<Group::Group0D*>& grp) const
          {grp = getGroupsContainer().get();}

    /*------------------------------------------------------------------------*/
    /** Nombre de noeuds internes
     * 1 pour un sommet
     */
    virtual unsigned long getNbInternalMeshingNodes() {return 1;}

    /*------------------------------------------------------------------------*/
    /// met à jour la date (de la représentation) pour les arêtes incidentes
    void updateCoEdgeModificationTime();


private:

    /// propriétés géométriques du vertex (coordonnées du sommet)
    VertexGeomProperty* m_geom_property;

    /// sauvegrade de m_geom_property
    VertexGeomProperty* m_save_geom_property;

    /// propriétés topologiques du vertex (liens sur les arêtes)
    VertexTopoProperty* m_topo_property;

    /// sauvegarde de m_topo_property
    VertexTopoProperty* m_save_topo_property;

    /// Etat et relation avec le maillage associé
    VertexMeshingData* m_mesh_data;

    /// Sauvegarde de l'etat et relation avec le maillage associé
    VertexMeshingData* m_save_mesh_data;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* TOPO_VERTEX_H_ */
/*----------------------------------------------------------------------------*/
