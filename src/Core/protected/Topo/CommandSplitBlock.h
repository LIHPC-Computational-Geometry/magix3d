/*----------------------------------------------------------------------------*/
/*
 * \file CommandSplitBlock.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/3/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSPLITBLOCK_H_
#define COMMANDSPLITBLOCK_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandSplitBlock
 *  \brief Commande permettant de découper un bloc (structuré) en 2
 *
 */
/*----------------------------------------------------------------------------*/
class CommandSplitBlock: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param bloc le bloc à découper
     *  \param arete l'arête qui sert à déterminer la direction de la coupe structurée
     *  \param ratio un paramètre pour déterminer la position du sommet sur l'arête
     */
    CommandSplitBlock(Internal::Context& c, Block* bloc, CoEdge* arete, double ratio);

    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param bloc le bloc à découper
     *  \param arete l'arête qui sert à déterminer la direction de la coupe structurée
     *  \param pt un point qui donnera un paramètre pour déterminer la position du sommet sur l'arête par projection sur cette dernière
     */
    CommandSplitBlock(Internal::Context& c, Block* bloc, CoEdge* arete, const Point& pt);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSplitBlock();

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

    /// l'arête qui sert à déterminer la direction de la coupe structurée
    CoEdge* m_arete;

    /// paramètre pour déterminer la position du sommet sur l'arête
    double m_ratio;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSPLITBLOCK_H_ */
/*----------------------------------------------------------------------------*/
