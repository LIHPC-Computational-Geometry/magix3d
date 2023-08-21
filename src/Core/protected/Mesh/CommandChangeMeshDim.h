/*----------------------------------------------------------------------------*/
/*
 * \file CommandChangeMeshDim.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 29/11/2018
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCHANGEMESHDIM_H_
#define COMMANDCHANGEMESHDIM_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** \class CommandChangeMeshDim
 *  \brief Commande permettant de changer la dimension du maillage en sortie
 */
/*----------------------------------------------------------------------------*/
class CommandChangeMeshDim: public Internal::CommandInternal{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param context  le contexte
     *  \param dim la nouvelle dimension
     */
    CommandChangeMeshDim(Internal::Context& context, const Internal::ContextIfc::meshDim& dim);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandChangeMeshDim();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();

    /** Ce qui est fait de propre à la commande pour annuler une commande */
    virtual void internalUndo();

    /** Ce qui est fait de propre à la commande pour rejouer une commande */
    virtual void internalRedo();

private:
    /// met à jour la structure de stockage du maillage (GMDS -> Lima)
    void updateMeshManager();

protected:

    /* nouvelle unité */
    Internal::ContextIfc::meshDim m_new_mesh_dim;

    /* ancienne unité */
    Internal::ContextIfc::meshDim m_old_mesh_dim;
};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCHANGEMESHDIM_H_ */
/*----------------------------------------------------------------------------*/
