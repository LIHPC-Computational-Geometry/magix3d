/*----------------------------------------------------------------------------*/
/** \file CommandFuseCoEdges.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 31/5/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMAND_FUSE_TOPO_H_
#define COMMAND_FUSE_TOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class CoEdge;
class TopoEntity;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/** \class CommandFuseCoEdges
 *  \brief Commande permettant de fusionner des contours topologiques (arêtes communes)
 */
/*----------------------------------------------------------------------------*/
class CommandFuseCoEdges: public Topo::CommandEditTopo{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param coedges une liste d'arêtes communes
     */
    CommandFuseCoEdges(Internal::Context& c, std::vector<CoEdge*> &coedges);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandFuseCoEdges();

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

private:
    /** les arêtes à fusionner */
    std::vector<Topo::CoEdge* > m_coedges;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMAND_FUSE_TOPO_H_ */
/*----------------------------------------------------------------------------*/

