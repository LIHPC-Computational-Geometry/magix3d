/*----------------------------------------------------------------------------*/
/*
 * \file CommandGlue2Blocks.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2 déc. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDGLUE2BLOCKS_H_
#define COMMANDGLUE2BLOCKS_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandGlue2Blocks
 *  \brief Commande permettant de fusionner les faces de 2 blocs pour les unir
 *
 *
 */
/*----------------------------------------------------------------------------*/
class CommandGlue2Blocks: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param bl_A un bloc dont les faces communes seront concervées
     *  \param bl_B un deuxième bloc dont les faces seront remplacées par celles de bl_A (pour une Face)
     */
    CommandGlue2Blocks(Internal::Context& c, Block* bl_A, Block* bl_B);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandGlue2Blocks();

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

    /// recherche la CoFace topo la plus proche d'un point
    CoFace* getNearestCoFace(std::vector<CoFace*>& cofaces, Utils::Math::Point pt);

    /// bloc qui sera inchangé
    Block* m_bl_A;
    /// bloc dont des faces seront fusionnées
    Block* m_bl_B;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDGLUE2BLOCKS_H_ */
/*----------------------------------------------------------------------------*/
