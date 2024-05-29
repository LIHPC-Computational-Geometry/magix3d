/*----------------------------------------------------------------------------*/
/*
 * \file CommandFuse2Blocks.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 11/6/2015
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDFUSE2BLOCKS_H_
#define COMMANDFUSE2BLOCKS_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandFuse2Blocks
 *  \brief Commande permettant de fusionner les 2 blocs pour les unir en un seul
 *
 *
 */
/*----------------------------------------------------------------------------*/
class CommandFuse2Blocks: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param bl_A un bloc
     *  \param bl_B un deuxième bloc
     */
    CommandFuse2Blocks(Internal::Context& c, Block* bl_A, Block* bl_B);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandFuse2Blocks();

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
    /// recherche la Face topo la plus proche d'un point
    Face* getNearestFace(Block* bl, Utils::Math::Point pt);

    /// un premier bloc
    Block* m_bl_A;
    /// un second bloc
    Block* m_bl_B;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDFUSE2BLOCKS_H_ */
/*----------------------------------------------------------------------------*/
