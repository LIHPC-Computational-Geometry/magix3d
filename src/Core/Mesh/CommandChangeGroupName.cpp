/*----------------------------------------------------------------------------*/
#include "Mesh/CommandChangeGroupName.h"
#include "Internal/Context.h"
#include "Group/GroupEntity.h"
#include "Group/GroupManager.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandChangeGroupName::CommandChangeGroupName(Internal::Context& c,
        const std::string& oldName,
        const std::string& newName,
        const int dim)
: CommandCreateMesh(c, (std::string("Change le nom du groupe ") + oldName),0)
, m_old_group_name(oldName)
, m_new_group_name(newName)
, m_dim(dim)
{
}
/*----------------------------------------------------------------------------*/
void CommandChangeGroupName::internalExecute()
{
	changeGroupName(m_old_group_name, m_new_group_name, m_dim);
}
/*----------------------------------------------------------------------------*/
void CommandChangeGroupName::internalUndo()
{
	changeGroupName(m_new_group_name, m_old_group_name, m_dim);
}
/*----------------------------------------------------------------------------*/
void CommandChangeGroupName::internalRedo()
{
	changeGroupName(m_old_group_name, m_new_group_name, m_dim);
}
/*----------------------------------------------------------------------------*/
void CommandChangeGroupName::changeGroupName(const std::string& oldName, const std::string& newName, const int dim)
{
	Group::GroupManager& gm = getContext().getGroupManager();
	Group::GroupEntity* group = gm.getGroup(newName, dim, false);
	if (group == 0) {
		// le groupe n'existe pas déjà => on peut autoriser le nouveau nom
		group = gm.getGroup(oldName, dim, true);
		if (group->isDefaultGroup()) {
	        throw TkUtil::Exception (TkUtil::UTF8String("Il n'est pas possible de changer le nom du groupe par défaut", TkUtil::Charset::UTF_8));
		}
		group->setProperties(getContext().newProperty(group->getType(), newName));
		getInfoCommand().addGroupInfoEntity(group, Internal::InfoCommand::DISPMODIFIED);
	} else {
		throw TkUtil::Exception (TkUtil::UTF8String("Nom de groupe déjà existant : " + newName, TkUtil::Charset::UTF_8));
	}
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
