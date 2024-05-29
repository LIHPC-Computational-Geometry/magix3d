/*----------------------------------------------------------------------------*/
/** \file CommandDestroyMesh.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 23/9/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMAND_DESTROY_MESH_H_
#define COMMAND_DESTROY_MESH_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** \class CommandDestroyMesh
 *  \brief Commande de destruction de tout le maillage sans retour arrière possible
 */
/*----------------------------------------------------------------------------*/
class CommandDestroyMesh: public Internal::CommandInternal{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     */
    CommandDestroyMesh(Internal::Context& c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandDestroyMesh();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    virtual void internalUndo();

    /** \brief  rejoue la commande
     */
    void internalRedo();

private:

};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMAND_DESTROY_MESH_H_ */
/*----------------------------------------------------------------------------*/

