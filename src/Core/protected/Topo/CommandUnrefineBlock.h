/*----------------------------------------------------------------------------*/
/*
 * \file CommandUnrefineBlock.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 27/4/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDUNREFINEBLOCK_H_
#define COMMANDUNREFINEBLOCK_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandUnrefineBlock
 *  \brief Commande permettant de déraffiner un bloc (structuré) suivant une direction
 *
 */
/*----------------------------------------------------------------------------*/
class CommandUnrefineBlock: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param bloc le bloc (structuré) à découper
     *  \param arete l'arête qui sert à déterminer la direction
     *  \param ratio le facteur de deraffinement
     */
    CommandUnrefineBlock(Internal::Context& c, Block* bloc, CoEdge* arete, int ratio);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandUnrefineBlock();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  appel non prévu
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:
    /// le bloc à découper
    Block* m_bloc;

    /// la direction suivant laquelle on déraffine
    Block::eDirOnBlock m_dir;

    /// le ratio pour le déraffinement
    int m_ratio;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDUNREFINEBLOCK_H_ */
/*----------------------------------------------------------------------------*/
