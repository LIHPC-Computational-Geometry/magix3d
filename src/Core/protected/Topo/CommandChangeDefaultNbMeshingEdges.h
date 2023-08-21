/*----------------------------------------------------------------------------*/
/** \file CommandChangeDefaultNbMeshingEdges.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 8/11/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCHANGEDEFAULTNBMESHINGEDGES_H_
#define COMMANDCHANGEDEFAULTNBMESHINGEDGES_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class TopoManager;
/*----------------------------------------------------------------------------*/
/** \class CommandChangeDefaultNbMeshingEdges
 *  \brief Permet de changer le nombre de bras par défaut pour la discrétisation d'une arête
 */
/*----------------------------------------------------------------------------*/
class CommandChangeDefaultNbMeshingEdges: public Internal::CommandInternal {

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le contexte
     * \param le nom de la commande
     */
    CommandChangeDefaultNbMeshingEdges(Internal::Context& c, int nb);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction de la topologie
     */
    virtual ~CommandChangeDefaultNbMeshingEdges();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    void internalUndo();

    /*------------------------------------------------------------------------*/
    /** \brief  rejoue la commande
     */
    void internalRedo();

protected:

    /*------------------------------------------------------------------------*/
    /** \brief  accesseur sur le manageur topologique
     */
    virtual TopoManager& getTopoManager();

private:

   /// nouveau nombre de bras pour discrétiser une arête
   int m_new_nb_meshing_edges;

   /// ancien nombre de bras pour discrétiser une arête
   int m_old_nb_meshing_edges;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCHANGEDEFAULTNBMESHINGEDGES_H_ */
/*----------------------------------------------------------------------------*/

