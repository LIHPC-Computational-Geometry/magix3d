/*----------------------------------------------------------------------------*/
/** \file CommandProjectEdgesOnCurves.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/11/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDPROJECTEDGESONCURVES_H_
#define COMMANDPROJECTEDGESONCURVES_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class Curve;
class Surface;
class GeomEntity;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class CoEdge;
class TopoEntity;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/** \class CommandProjectEdgesOnCurves
 *  \brief Commande permettant de projetter ces arêtes topologiques vers la courbe
 *  en relation avec ses sommets, s'il y en a une
 */
/*----------------------------------------------------------------------------*/
class CommandProjectEdgesOnCurves: public Topo::CommandEditTopo{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une liste de CoEdge
     *
     *  \param c le contexte
     *  \param coedges une liste d'arêtes topologiques
     */
    CommandProjectEdgesOnCurves(Internal::Context& c,
    		std::vector<CoEdge*> &coedges);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour toutes les CoEdge du manager
     *
     *  \param c le contexte
     */
    CommandProjectEdgesOnCurves(Internal::Context& c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandProjectEdgesOnCurves();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités modifiées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:

    /** projection pour une arête */
    void project(CoEdge* coedge, Geom::GeomEntity* ge);

    /** les sommets à projeter */
    std::vector<CoEdge*> m_coedges;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDPROJECTEDGESONCURVES_H_ */
/*----------------------------------------------------------------------------*/

