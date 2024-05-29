/*----------------------------------------------------------------------------*/
/*
 * \file CommandReverseDirection.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 4/2/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDREVERSEDIRECTION_H_
#define COMMANDREVERSEDIRECTION_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class CoEdgeMeshingProperty;
class CoEdge;
/*----------------------------------------------------------------------------*/
/** \class CommandReverseDirection
 *  \brief Commande qui change le sens pour des arêtes
 */
/*----------------------------------------------------------------------------*/
class CommandReverseDirection: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param coedges les arêtes dont on va changer le sens
     */
    CommandReverseDirection(Internal::Context& c, std::vector<CoEdge*>& coedges);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandReverseDirection();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  appel non prévu
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:
    /// les arêtes concernées
    std::vector<CoEdge*> m_coedges;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDREVERSEDIRECTION_H_ */
/*----------------------------------------------------------------------------*/
