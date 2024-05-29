/*----------------------------------------------------------------------------*/
/*
 * \file CommandSplitEdge.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 8/11/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSPLITEDGE_H_
#define COMMANDSPLITEDGE_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandSplitEdge
 *  \brief Commande permettant de découper une arête (commune) en 2
 *
 */
/*----------------------------------------------------------------------------*/
class CommandSplitEdge: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour découper une arête
     *
     *  \param c le contexte
     *  \param arete l'arête à couper
     *  \param ratio_dec un paramètre pour déterminer la position du sommet sur l'arête
     */
    CommandSplitEdge(Internal::Context& c, CoEdge* arete, double ratio_dec);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour découper une arête
     *
     *  \param c le contexte
     *  \param arete l'arête à couper
     *  \param pt un point qui donnera un paramètre pour déterminer la position du sommet sur l'arête par projection sur cette dernière
     */
    CommandSplitEdge(Internal::Context& c, CoEdge* arete, const Point& pt);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSplitEdge();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités créées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

    /*------------------------------------------------------------------------*/

private:
    /// l'arête coupée
    CoEdge* m_arete;

    /// paramètre pour déterminer la position du sommet sur l'arête
    double m_ratio_dec;

    /// s'il faut replacer le sommet créé à une position donnée
    bool m_use_point;

    /// point où il faut placer le sommet
    Utils::Math::Point m_pt;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSPLITEDGE_H_ */
/*----------------------------------------------------------------------------*/
