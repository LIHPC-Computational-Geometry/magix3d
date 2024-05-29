/*----------------------------------------------------------------------------*/
/*
 * \file CommandFuse2Vertices.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 13/1/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMANDFUSE2VERTICES_H_
#define COMANDFUSE2VERTICES_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {

class Vertex;

/*----------------------------------------------------------------------------*/
/** \class CommandFuse2Vertices
 *  \brief Commande permettant de fusionner 2 sommets
 *
 *  Les arêtes et cofaces adjacentes ne sont pas modifiées (pas fusionnées)
 */
/*----------------------------------------------------------------------------*/
class CommandFuse2Vertices: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param vtx_A un sommet qui sera conservé
     *  \param vtx_B un sommet qui est remplacé par vtx_A
     */
    CommandFuse2Vertices(Internal::Context& c, Vertex* vtx_A, Vertex* vtx_B);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandFuse2Vertices();

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
    /// sommet qui sera conservé
    Vertex* m_vtx_A;
    /// sommet qui perd sa place
    Vertex* m_vtx_B;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMANDFUSE2VERTICES_H_ */
/*----------------------------------------------------------------------------*/
