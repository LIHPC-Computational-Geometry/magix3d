/*----------------------------------------------------------------------------*/
/*
 * \file CommandFuse2EdgeList.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18/04/2019
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDGLUE2EDGELIST_H_
#define COMMANDGLUE2EDGELIST_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandFuse2EdgeList
 *  \brief Commande permettant de fusionner 2 ensembles de listes de coedges
 *
 *
 */
/*----------------------------------------------------------------------------*/
class CommandFuse2EdgeList: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param coedges1 une liste d'arêtes communes qui seront conservées
     *  \param coedges2 une secondes liste d'arêtes communes qui seront collées
     */
    CommandFuse2EdgeList(Internal::Context& c,
    		std::vector<Topo::CoEdge*> &coedges1,
    		std::vector<Topo::CoEdge*> &coedges2);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandFuse2EdgeList();

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
    /// edges qui seront conservées
    std::vector<Topo::CoEdge*> m_coedges1;
    /// edges qui seront collées et doivent disparaitre
    std::vector<Topo::CoEdge*> m_coedges2;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDGLUE2EDGELIST_H_ */
/*----------------------------------------------------------------------------*/
