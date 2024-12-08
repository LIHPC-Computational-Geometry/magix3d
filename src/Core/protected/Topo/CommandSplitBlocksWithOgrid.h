/*----------------------------------------------------------------------------*/
/*
 * \file CommandSplitBlocksWithOgrid.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22/8/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSPLITBLOCKSWITHOGRID_H_
#define COMMANDSPLITBLOCKSWITHOGRID_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandSplitBlocksWithOgrid
 *  \brief Commande permettant de découper des blocs (structurés et non dégénérés)
 *  suivant un schéma en o-grid
 *
 */
/*----------------------------------------------------------------------------*/
class CommandSplitBlocksWithOgrid: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une liste de blocs
     *
     *  \param c le contexte
     *  \param blocs les blocs à découper
     *  \param cofaces la liste des faces communes à considérer comme étant entre 2 blocs
     *  (ce qui permet de ne pas construire le bloc contre ces faces communes)
     *  \param ratio_ogrid un paramètre pour positionner les sommets de l'ogrid
     *  \param nb_bras le nombre de bras pour les couches autour du bloc créé au centre
     *  \param create_internal_vertices pour création ou non des noeuds internes
     *  \param propagate_neighbor_block propage le découpage des blocs aux voisins des faces sélectionnées (option par défaut avant 2019)
     */
    CommandSplitBlocksWithOgrid(Internal::Context& c,
            std::vector<Topo::Block* > &blocs,
            std::vector<Topo::CoFace* > &cofaces,
            double ratio_ogrid,
            int nb_bras,
			bool create_internal_vertices,
			bool propagate_neighbor_block);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSplitBlocksWithOgrid();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();


    /*------------------------------------------------------------------------*/
    /// Compte le nombre d'arêtes reliées à chacun des sommets de la topologie concernée par la commande
    virtual void countNbCoEdgesByVertices(std::map<Topo::Vertex*, uint> &nb_coedges_by_vertex);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités modifiées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:
    /** Remplissage des filtres pour les sommets, arêtes communes,
     * faces communes et les blocs
     * On met à 1 ce qui est sur le bord et
     *        à 2 ce qui est à l'intérieur
     * Une face commune est à l'intérieur si elle est entre deux blocs sélectionnés
     * Les faces aux bords permettent de marquer les arêtes, sommets et blocs au bord
     * On met à 3 les arêtes et faces communes des blocs au centre
     *
     * Pour le filtre des faces: à 1 si toutes ses cofaces sont externes,
     * à 2 sinon
     *
     * Par la suite, les sommets sont marqués à 4 lorsqu'ils sont créés sur une courbe
     */
    void computeFilters(std::map<Vertex*, uint> & filtre_vertex,
            std::map<CoEdge*, uint> & filtre_coedge,
            std::map<CoFace*, uint> & filtre_coface,
            std::map<Face*,  uint> & filtre_face,
            std::map<Block*,  uint> & filtre_bloc);

    /** Sauvegarde les relations topologiques pour permettre le undo
     */
    void saveTopoProperty(std::map<Vertex*, uint> & filtre_vertex,
            std::map<CoEdge*, uint> & filtre_coedge,
            std::map<CoFace*, uint> & filtre_coface,
            std::map<Block*,  uint> & filtre_bloc);

    /** Création des sommets au centre de l'o-grid
     */
    void createVertices(std::map<Vertex*, uint> & filtre_vertex,
            std::map<CoEdge*, uint> & filtre_coedge,
            std::map<CoFace*, uint> & filtre_coface,
            std::map<Vertex*, Vertex*> & corr_vertex);

    /** Création des arêtes communes entre les sommets du bord et ceux au centre de l'o-grid
     */
    void createCoEdges(std::map<Vertex*, uint> & filtre_vertex,
            std::map<Vertex*, Vertex*> & corr_vertex,
            std::map<Vertex*, CoEdge*> & corr_vtx_coedge);

    /** Création des arêtes communes et face communes (internes à l'ogrid)
     */
    void createCoEdgeAndFace(std::map<Vertex*, uint> & filtre_vertex,
            std::map<Vertex*, Vertex*>  & corr_vertex,
            std::map<Vertex*, CoEdge*> & corr_vtx_coedge,
            std::map<CoEdge*, uint> & filtre_coedge,
            std::map<CoFace*, uint> & filtre_coface,
            std::map<Face*,  uint> & filtre_face,
            std::map<Block*,  uint> & filtre_bloc,
            std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge,
            std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface);

    /** Création des arêtes communes et face communes (internes à l'ogrid)
     *  en fonction d'un couple de sommets
     */
    void createCoEdgeAndFace(Vertex* vtx0,
            Vertex* vtx1,
            std::vector<CoEdge* > & iCoedges,
            std::vector<CoEdge* > & jCoedges,
            std::map<Vertex*, Vertex*>  & corr_vertex,
            std::map<Vertex*, CoEdge*> & corr_vtx_coedge,
            std::map<Vertex*, uint> & filtre_vertex,
            std::map<CoEdge*, uint> & filtre_coedge,
            std::vector<std::string> & groupsName,
            std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge,
            std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface);

    /** Création d'une arête (interne à l'ogrid) pour 2 sommets
     */
    Edge* createEdge(Vertex* vtx0,
            Vertex* vtx1,
            std::vector<CoEdge* > & iCoedges,
            std::vector<CoEdge* > & jCoedges,
            std::map<Vertex*, Vertex*>  & corr_vertex,
            std::map<Vertex*, uint> & filtre_vertex,
            std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge);

    /** Creation des faces communes au centre
     * et conserve une relation entre face commune issues de la sélection et celles au centre
     */
    void createCoFace(std::map<Vertex*, uint> & filtre_vertex,
            std::map<Vertex*, Vertex*>  & corr_vertex,
            std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge,
            std::map<CoFace*, uint> & filtre_coface,
            std::map<Face*,  uint> & filtre_face,
            std::map<Block*,  uint> & filtre_bloc,
            std::map<CoFace*, CoFace*> & corr_coface);

    /** Création d'une face à partir de 2 sommets (cofaces entre bord et o-grid)
     */
    Face* createFace(Vertex* vtx0,
            Vertex* vtx1,
            std::vector<CoEdge* > & iCoedges,
            std::vector<CoEdge* > & jCoedges,
            std::map<Vertex*, Vertex*>  & corr_vertex,
            std::map<Vertex*, uint> & filtre_vertex,
            std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface);

    /** Création d'une face (sur le bloc interne) à partir d'une face du bloc initial
     * (cofaces prises sur le centre de l'o-grid)
     */
    Face* createFace(Face* face,
            std::map<Vertex*, Vertex*>  & corr_vertex,
            std::map<CoFace*, CoFace*> & corr_coface);

    /** Création des blocs
     * un par coface au bord de la sélection */
    void createBlock(std::map<Vertex*, uint> & filtre_vertex,
            std::map<Vertex*, Vertex*>  & corr_vertex,
            std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface,
            std::map<CoFace*, uint> & filtre_coface,
            std::map<CoFace*, CoFace*> & corr_coface,
            std::map<Face*,  uint> & filtre_face,
            std::map<Block*,  uint> & filtre_bloc);

    /** Supression des entités initiales qui ne sont plus utilisées: les faces internes et les blocs,
     * réutilisation des projections pour les faces internes
     * mise à jour des entitées adjacentes
     */
    void freeUnused(std::map<Vertex*, uint> & filtre_vertex,
            std::map<CoEdge*, uint> & filtre_coedge,
            std::map<CoFace*, uint> & filtre_coface,
            std::map<Block*,  uint> & filtre_bloc,
            std::map<Vertex*, CoEdge*> & corr_vtx_coedge,
			std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge,
            std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface,
            std::map<CoFace*, CoFace*> & corr_coface);

    /** Retourne la liste des sommets sur le bord (dans une des listes iCoedges ou jCoedges)
     * suivant les 2 sommets donnés comme extrémité
     * On ne prend que les sommets marqués
     */
    std::vector<Vertex*> getSelectedVertices(Vertex* vtx0,
            Vertex* vtx1,
            std::vector<CoEdge* > & iCoedges,
            std::vector<CoEdge* > & jCoedges,
            std::map<Vertex*, uint> & filtre_vertex);

    /** marque à 1 (si non traité) les edges qui partagent une arête commune
     * filtre_edge, filtre sur les edges pour stocker d'une part celles mises à jour (marque à 2)
     * celles à mettre à jour par la suite (marque à 1)
     */
    void updateNeighBorFilter(Edge* edge, std::map<Edge*,  uint> & filtre_edge);

    /** met à jour les discrétisations des arêtes voisines de celles créées
     *  pour garantir la structuration
     */
    void updateNbMeshingEdges();

private:
    /// les blocs structurés à découper
    std::vector<Block* > m_blocs;

    /// les faces communes à considérer comme étant au bord
    std::vector<CoFace* > m_cofaces;

    /// paramètre pour déterminer la position du sommet d'un ogrid
    double m_ratio_ogrid;

    /// le nombre de bras pour les les couches autour du bloc au centre
    int m_nb_meshing_edges;

    /** Création des sommets internes systématiquement suivant ceux au bord.
    	Option mise en place pour maintenir la compatibilité avec anciennes versions.
    	Par défaut, les sommets internes ne sont plus créés autant que possible.
    */
    bool m_create_internal_vertices;

    /** découpage des faces sélectionnées sans propager aux blocs voisins même s'il en existe */
    bool m_propagate_neighbor_block;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSPLITBLOCKSWITHOGRID_H_ */
/*----------------------------------------------------------------------------*/
