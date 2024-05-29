/*----------------------------------------------------------------------------*/
/*
 * \file CommandSplitFaces.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/3/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSPLITFACES_H_
#define COMMANDSPLITFACES_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandSplitFaces
 *  \brief Commande permettant de découper un ensemble de cofaces
 *
 */
/*----------------------------------------------------------------------------*/
class CommandSplitFaces: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour découper une liste de faces (dont on ne garde que celles 2D structurées)
     *
     *  \param c le contexte
     *  \param cofaces les faces communes à découper
     *  \param arete l'arête qui sert à déterminer la direction de la coupe structurée
     *  \param ratio_dec un paramètre pour déterminer la position du sommet sur l'arête
     *  \param ratio_ogrid un autre paramètre pour positionner le sommet du ogrid en cas de rencontre avec une face dégénérée
     */
    CommandSplitFaces(Internal::Context& c, std::vector<Topo::CoFace* > &cofaces, CoEdge* arete, double ratio_dec, double ratio_ogrid);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour découper une liste de faces (dont on ne garde que celles 2D structurées)
     *
     *  \param c le contexte
     *  \param cofaces les faces communes à découper
     *  \param arete l'arête qui sert à déterminer la direction de la coupe structurée
     *  \param pt un point qui donnera un paramètre pour déterminer la position du sommet sur l'arête par projection sur cette dernière
     *  \param ratio_ogrid un autre paramètre pour positionner le sommet du ogrid en cas de rencontre avec une face dégénérée
     */
    CommandSplitFaces(Internal::Context& c, std::vector<Topo::CoFace* > &cofaces, CoEdge* arete, const Point& pt, double ratio_ogrid);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour découper toutes les faces 2D structurées disponibles
     *
     *  \param c le contexte
     *  \param arete l'arête qui sert à déterminer la direction de la coupe structurée
     *  \param ratio_dec un paramètre pour déterminer la position du sommet sur l'arête
     *  \param ratio_ogrid un autre paramètre pour positionner le sommet du ogrid en cas de rencontre avec une face dégénérée
     */
    CommandSplitFaces(Internal::Context& c, CoEdge* arete, double ratio_dec, double ratio_ogrid);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour découper toutes les faces 2D structurées disponibles
     *
     *  \param c le contexte
     *  \param arete l'arête qui sert à déterminer la direction de la coupe structurée
     *  \param pt un point qui donnera un paramètre pour déterminer la position du sommet sur l'arête par projection sur cette dernière
     *  \param ratio_ogrid un autre paramètre pour positionner le sommet du ogrid en cas de rencontre avec une face dégénérée
     */
    CommandSplitFaces(Internal::Context& c, CoEdge* arete, const Point& pt, double ratio_ogrid);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une face (2D ou 3D) à découper
     *
     *  \param c le contexte
     *  \param coface la face commune à découper
     *  \param arete celle qui sert à déterminer la direction de la coupe structurée
     *  \param ratio_dec un paramètre pour déterminer la position du sommet sur l'arête
     *  \param project_on_meshing_edges activation ou non de la projection des sommets issus de la coupe sur la discrétisation initiale
     */
    CommandSplitFaces(Internal::Context& c, CoFace*  coface, CoEdge* coedge, double ratio_dec,  bool project_on_meshing_edges);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une face (2D ou 3D) à découper
     *
     *  \param c le contexte
     *  \param coface la face commune à découper
     *  \param arete celle qui sert à déterminer la direction de la coupe structurée
     *  \param pt un point qui donnera un paramètre pour déterminer la position du sommet sur l'arête par projection sur cette dernière
     *  \param project_on_meshing_edges activation ou non de la projection des sommets issus de la coupe sur la discrétisation initiale
     */
    CommandSplitFaces(Internal::Context& c, CoFace*  coface, CoEdge* coedge, const Point& pt,  bool project_on_meshing_edges);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSplitFaces();

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
    /// stocke les faces communes 2D structurées parmi celles données en paramètre
    void init2D(std::vector<Topo::CoFace* > &cofaces);

    /// Vérification de la face topologique (pas d'arête colinéaires ...)
    void verif(Topo::CoFace* coface);

    /// les faces communes 2D structurées à découper
    std::vector<CoFace* > m_cofaces;

    /// l'arête qui sert à déterminer la direction de la coupe structurée
    CoEdge* m_arete;

    /// paramètre pour déterminer la position du sommet sur l'arête
    double m_ratio_dec;

    /// paramètre pour déterminer la position du sommet d'un ogrid dans une face à 3 côtés
    double m_ratio_ogrid;

    /// activation ou non de la projection des sommets créés sur la discrétisation initiale
    bool m_project_on_meshing_edges;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSPLITFACES_H_ */
/*----------------------------------------------------------------------------*/
