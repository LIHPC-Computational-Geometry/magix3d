/*----------------------------------------------------------------------------*/
/*
 * \file CommandRefineAllCoEdges.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 26/2/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDREFINEALLCOEDGES_H_
#define COMMANDREFINEALLCOEDGES_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandRefineAllCoEdges
 *  \brief Commande permettant de raffiner toutes les arêtes existantes
 *
 */
/*----------------------------------------------------------------------------*/
class CommandRefineAllCoEdges: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param ratio le facteur de raffinement
     */
    CommandRefineAllCoEdges(Internal::Context& c, int ratio);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandRefineAllCoEdges();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  appel non prévu
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:
    /// le ratio pour le raffinement
    int m_ratio;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDREFINEALLCOEDGES_H_ */
/*----------------------------------------------------------------------------*/
