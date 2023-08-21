/*----------------------------------------------------------------------------*/
/** \file CommandProjectFacesOnSurfaces.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/11/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDPROJECTFACESONSURFACES_H_
#define COMMANDPROJECTFACESONSURFACES_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class Curve;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class CoEdge;
class TopoEntity;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/** \class CommandProjectFacesOnSurfaces
 *  \brief Commande permettant de projetter ces faces topologiques vers la surface
 *  en relation avec ses arêtes, s'il y en a une
 */
/*----------------------------------------------------------------------------*/
class CommandProjectFacesOnSurfaces: public Topo::CommandEditTopo{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une liste de CoFace
     *
     *  \param c le contexte
     *  \param cofaces une liste de faces topologiques
     */
    CommandProjectFacesOnSurfaces(Internal::Context& c,
    		std::vector<CoFace*> &cofaces);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour toutes les CoFace
     *
     *  \param c le contexte
     */
    CommandProjectFacesOnSurfaces(Internal::Context& c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandProjectFacesOnSurfaces();

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
    void project(CoFace* coface, Geom::Surface* surface);

    /** les sommets à projeter */
    std::vector<CoFace*> m_cofaces;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDPROJECTFACESONSURFACES_H_ */
/*----------------------------------------------------------------------------*/

