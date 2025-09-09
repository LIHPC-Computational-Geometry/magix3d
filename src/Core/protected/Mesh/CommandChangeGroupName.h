/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_COMMANDCHANGEGROUPNAME_H_
#define MGX3D_MESH_COMMANDCHANGEGROUPNAME_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/CommandCreateMesh.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
class Context;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** \class CommandAddRemoveGroupName
 *  \brief Commande permettant d'ajouter des entités à un groupe
 */
/*----------------------------------------------------------------------------*/
class CommandChangeGroupName: public CommandCreateMesh
{
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur avec une liste de volumes
     *
     *  \param c le contexte
     *  \param oldName ancien nom du groupe
     *  \param newName ancien nom du groupe
     *  \param dim dimension du groupe
     */
    CommandChangeGroupName(Internal::Context& c, const std::string& oldName, const std::string& newName, const int dim);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandChangeGroupName() = default;

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
    /// change le nom du groupe
    void changeGroupName(const std::string& oldName, const std::string& newName, const int dim);

    /// ancien nom du groupe
    std::string m_old_group_name;

    /// nouveau nom du groupe
    std::string m_new_group_name;

    /// dimension
    int m_dim;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_MESH_COMMANDCHANGEGROUPNAME_H_ */
/*----------------------------------------------------------------------------*/
