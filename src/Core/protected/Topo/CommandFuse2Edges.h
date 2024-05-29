/*----------------------------------------------------------------------------*/
/*
 * \file CommandFuse2Edges.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 27/6/2016
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDFUSE2EDGES_H_
#define COMMANDFUSE2EDGES_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandFuse2Edges
 *  \brief Commande permettant de fusionner 2 arêtes pour unir 2 faces
 *
 *
 */
/*----------------------------------------------------------------------------*/
class CommandFuse2Edges: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param edge_A une arête commune qui sera conservée
     *  \param edge_B une arête commune qui est remplacée par edge_A
     */
    CommandFuse2Edges(Internal::Context& c, CoEdge* edge_A, CoEdge* edge_B);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandFuse2Edges();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** Ce qui est fait après la commande suivant le cas en erreur ou non
     */
    virtual void postExecute(bool hasError);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités modifiées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:
    /// edge qui sera conservée
    CoEdge* m_edge_A;
    /// face qui perd sa place
    CoEdge* m_edge_B;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDFUSE2EDGES_H_ */
/*----------------------------------------------------------------------------*/
