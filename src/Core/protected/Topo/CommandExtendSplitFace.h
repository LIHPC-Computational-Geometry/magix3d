/*----------------------------------------------------------------------------*/
/*
 * \file CommandExtendSplitFace.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15/5/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDEXTENDSPLITFACE_H_
#define COMMANDEXTENDSPLITFACE_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandExtendSplitFace
 *  \brief Commande permettant de découper une coface suivant un sommet
 *
 */
/*----------------------------------------------------------------------------*/
class CommandExtendSplitFace: public Topo::CommandEditTopo {

public:


    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une face (2D ou 3D) à découper
     *
     *  \param c le contexte
     *  \param coface la face commune à découper
     *  \param sommet celui qui sert à déterminer la direction de la coupe structurée
     */
    CommandExtendSplitFace(Internal::Context& c, CoFace*  coface, Vertex* sommet);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandExtendSplitFace();

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
    /// la face commune structurée à découper
    CoFace* m_coface;

    /// le sommet qui sert à déterminer la direction de la coupe structurée
    Vertex* m_sommet;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDEXTENDSPLITFACE_H_ */
/*----------------------------------------------------------------------------*/
