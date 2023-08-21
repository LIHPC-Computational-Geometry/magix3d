/*----------------------------------------------------------------------------*/
/*
 * \file CommandExtendSplitBlock.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 14/9/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDEXTENDSPLITBLOCK_H_
#define COMMANDEXTENDSPLITBLOCK_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandExtendSplitBlock
 *  \brief Commande permettant de découper un bloc (structuré) en 2
 *  en prolongeant la coupe existante sur une face
 */
/*----------------------------------------------------------------------------*/
class CommandExtendSplitBlock: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param bloc le bloc à découper
     *  \param arete l'arête de début de coupe
     */
    CommandExtendSplitBlock(Internal::Context& c, Block* bloc, CoEdge* arete);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExtendSplitBlock();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /// Compte le nombre d'arêtes reliées à chacun des sommets de la topologie concernée par la commande
    virtual void countNbCoEdgesByVertices(std::map<Topo::Vertex*, uint> &nb_coedges_by_vertex);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités modifiées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:
    /// le bloc à découper
    Block* m_bloc;

    /// l'arête de début de coupe
    CoEdge* m_arete;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDEXTENDSPLITBLOCK_H_ */
/*----------------------------------------------------------------------------*/
