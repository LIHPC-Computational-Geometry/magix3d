/*----------------------------------------------------------------------------*/
/*
 * \file CommandAddMeshModification.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5 déc. 2013
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDADDMESHMODIFICATION_H_
#define COMMANDADDMESHMODIFICATION_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
class MeshModificationItf;
}
/*----------------------------------------------------------------------------*/
namespace Internal {
class Context;
}
/*----------------------------------------------------------------------------*/
namespace Group {

class GroupEntity;
/*----------------------------------------------------------------------------*/
class CommandAddMeshModification : public Internal::CommandInternal{

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur avec un objet de modification du maillage
     *
     *  \param c le contexte
     *  \param grp le groupe auquel on ajoute la modification de maillage
     *  \param modif l'objet qui va permettre de modifer le maillage
     */
	CommandAddMeshModification(Internal::Context& c,
            Group::GroupEntity* grp,
            Mesh::MeshModificationItf* modif);

    /*------------------------------------------------------------------------*/
	virtual ~CommandAddMeshModification();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    void internalUndo();

    /** \brief  rejoue la commande
     */
    void internalRedo();

private:
    /// le groupe concerné par la commande
    Group::GroupEntity* m_grp;

    /// la modification que l'on affecte au groupe
    Mesh::MeshModificationItf* m_new_modif;
};
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDADDMESHMODIFICATION_H_ */
