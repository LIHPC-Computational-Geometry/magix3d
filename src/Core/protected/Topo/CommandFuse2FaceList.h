/*----------------------------------------------------------------------------*/
/*
 * \file CommandFuse2FaceList.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/12/14
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDGLUE2FACELIST_H_
#define COMMANDGLUE2FACELIST_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandFuse2FaceList
 *  \brief Commande permettant de fusionner 2 ensembles de listes de cofaces
 *
 *
 */
/*----------------------------------------------------------------------------*/
class CommandFuse2FaceList: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cofaces1 une liste de faces communes qui seront conservées
     *  \param cofaces2 une secondes liste de faces communes qui seront collées
     */
    CommandFuse2FaceList(Internal::Context& c,
    		std::vector<Topo::CoFace*> &cofaces1,
    		std::vector<Topo::CoFace*> &cofaces2);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandFuse2FaceList();

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
    /// faces qui seront conservées
    std::vector<Topo::CoFace*> m_cofaces1;
    /// faces qui seront collées et doivent disparaitre
    std::vector<Topo::CoFace*> m_cofaces2;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDGLUE2FACELIST_H_ */
/*----------------------------------------------------------------------------*/
