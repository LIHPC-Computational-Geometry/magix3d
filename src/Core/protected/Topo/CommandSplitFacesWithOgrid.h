/*----------------------------------------------------------------------------*/
/*
 * \file CommandSplitFacesWithOgrid.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/11/2017
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSPLITFACESWITHOGRID_H_
#define COMMANDSPLITFACESWITHOGRID_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandSplitFacesWithOgrid
 *  \brief Commande permettant de découper des faces (structurées et non dégénérées)
 *  suivant un schéma en o-grid
 *
 */
/*----------------------------------------------------------------------------*/
class CommandSplitFacesWithOgrid: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une liste de faces
     *
     *  \param c le contexte
     *  \param faces les faces à découper
     *  \param coedges la liste des faces communes à considérer comme étant entre 2 blocs
     *  (ce qui permet de ne pas construire le bloc contre ces faces communes)
     *  \param ratio_ogrid un paramètre pour positionner les sommets de l'ogrid
     *  \param nb_bras le nombre de bras pour les couches autour du bloc créé au centre
     *  \param create_internal_vertices pour création ou non des noeuds internes
     */
    CommandSplitFacesWithOgrid(Internal::Context& c,
            std::vector<Topo::CoFace* > &cofaces,
	        std::vector<Topo::CoEdge* > &coedges,
            double ratio_ogrid,
            int nb_bras);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSplitFacesWithOgrid();

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
    /** Remplissage des filtres pour les sommets, arêtes communes et faces communes
     * On met à 1 ce qui est sur le bord et
     *        à 2 ce qui est à l'intérieur
     * Une arête commune est à l'intérieur si elle est entre deux faces sélectionnées
     * Les arêtes aux bords permettent de marquer les arêtes et sommets au bord
     * On met à 3 les arêtes des faces au centre
     *
     * Pour le filtre des arêtes: à 1 si toutes ses coedges sont externes,
     * à 2 sinon
     *
     * Par la suite, les sommets sont marqués à 4 lorsqu'ils sont créés sur une courbe
     */
    void computeFilters(std::map<Vertex*, uint> & filtre_vertex,
            std::map<CoEdge*, uint> & filtre_coedge,
            std::map<Edge*,  uint> & filtre_edge,
            std::map<CoFace*, uint> & filtre_coface);

    /** Sauvegarde les relations topologiques pour permettre le undo
     */
    void saveTopoProperty(std::map<Vertex*, uint> & filtre_vertex,
            std::map<CoEdge*, uint> & filtre_coedge,
            std::map<CoFace*, uint> & filtre_coface);

    /** Création des sommets au centre de l'o-grid
     */
    void createVertices(std::map<Vertex*, uint> & filtre_vertex,
            std::map<CoEdge*, uint> & filtre_coedge,
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
			std::map<Edge*,  uint> & filtre_edge,
            std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge,
            std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface);

    /** Création des arêtes communes et face communes (internes à l'ogrid)
     *  en fonction d'un couple de sommets
     */
    void createCoEdgeAndFace(Vertex* vtx0,
            Vertex* vtx1,
			Edge* edge,
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
            std::map<Vertex*, Vertex*>  & corr_vertex,
            std::map<Vertex*, uint> & filtre_vertex,
            std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge);


    /** Creation des faces communes au centre
     */
    void createCoFace(std::map<Vertex*, uint> & filtre_vertex,
            std::map<Vertex*, Vertex*>  & corr_vertex,
            std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge,
            std::map<CoFace*, uint> & filtre_coface,
            std::map<CoFace*, CoFace*> & corr_coface);


    /** Supression des entités initiales qui ne sont plus utilisées: les faces internes et les blocs,
     * réutilisation des projections pour les faces internes
     */
    void freeUnused(std::map<Vertex*, uint> & filtre_vertex,
            std::map<CoEdge*, uint> & filtre_coedge,
            std::map<CoFace*, uint> & filtre_coface,
            std::map<Vertex*, CoEdge*> & corr_vtx_coedge,
            std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface,
            std::map<CoFace*, CoFace*> & corr_coface);

    /** Retourne la liste des sommets sur le bord (dans une des listes iCoedges ou jCoedges)
     * suivant les 2 sommets donnés comme extrémité
     * On ne prend que les sommets marqués
     */
    std::vector<Vertex*> getSelectedVertices(Vertex* vtx0,
            Vertex* vtx1,
            std::vector<CoEdge* > & coedges,
            std::map<Vertex*, uint> & filtre_vertex);

private:
    /// les faces structurées à découper
    std::vector<CoFace* > m_cofaces;

    /// les arêtes communes à considérer comme étant au bord
    std::vector<CoEdge* > m_coedges;

    /// paramètre pour déterminer la position du sommet d'un ogrid
    double m_ratio_ogrid;

    /// le nombre de bras pour les couches autour de la face au centre
    int m_nb_meshing_edges;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSPLITFACESWITHOGRID_H_ */
/*----------------------------------------------------------------------------*/
