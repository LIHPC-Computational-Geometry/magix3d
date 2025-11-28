/*----------------------------------------------------------------------------*/
#ifndef GROUP_HELPER_FOR_COMMAND_H_
#define GROUP_HELPER_FOR_COMMAND_H_
/*----------------------------------------------------------------------------*/
#include "Internal/InfoCommand.h"
#include "Group/GroupManager.h"
#include "Group/GroupEntity.h"
#include "Topo/Block.h"
#include "Topo/CoFace.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
class GroupManager;
/*----------------------------------------------------------------------------*/
class GroupHelperForCommand {

public:
	GroupHelperForCommand(Internal::InfoCommand& info_command, GroupManager& group_manager);
	virtual ~GroupHelperForCommand() = default;

    GroupEntity* addToGroup(const std::string& group_name, Geom::GeomEntity* e);
    GroupEntity* addToGroup(const std::string& group_name, Topo::TopoEntity* e);

    GroupEntity* removeFromGroup(const std::string& group_name, Geom::GeomEntity* e);
    GroupEntity* removeFromGroup(const std::string& group_name, Topo::TopoEntity* e);

    // temporaire en attendant de restructurer la Topo
    // (même remarque pour les templates privées ci-dessous)
    std::vector<GroupEntity*> getGroupsFor(const Topo::TopoEntity* e);

private:
    template <
        typename TGroup, typename = std::enable_if_t<std::is_base_of<GroupEntity, TGroup>::value>,
        typename TEntity, typename = std::enable_if_t<std::is_base_of<Topo::TopoEntity, TEntity>::value>
    >
    TGroup* addToGroup(const std::string& group_name, TEntity* e)
    {
        TGroup* group = m_group_manager.getNewGroup<TGroup>(group_name, &m_info_command);

        // il faut peut-être enlever le groupe par défaut
        if (e->getNbGroups() == 1) {
            std::vector<TGroup*> groups = e->getGroups();
            if (groups[0]->isDefaultGroup()) {
                groups[0]->remove(e);
                e->remove(groups[0]);
                if (groups[0]->empty())
                    m_info_command.addGroupInfoEntity(groups[0], Internal::InfoCommand::DISABLE);
                else
                    m_info_command.addGroupInfoEntity(groups[0], Internal::InfoCommand::DISPMODIFIED);
            }
        }

        // on ajoute sauf si l'entité est déjà dans un groupe
        // et que le groupe correspond au groupe par defaut
        if (! (e->getNbGroups() > 0 && group->isDefaultGroup())) {
            group->add(e);
            e->add(group);
            m_info_command.addGroupInfoEntity(group, Internal::InfoCommand::DISPMODIFIED);	
        }

        return group;
    }

    template <
        typename TGroup, typename = std::enable_if_t<std::is_base_of<GroupEntity, TGroup>::value>,
        typename TEntity, typename = std::enable_if_t<std::is_base_of<Topo::TopoEntity, TEntity>::value>
    >
    TGroup* removeFromGroup(const std::string& group_name, TEntity* e)
    {
        TGroup* group = m_group_manager.getGroup<TGroup>(group_name, &m_info_command);
        group->remove(e);
        e->remove(group);

        // il faut peut-être ajouter le groupe par défaut
        if (e->getNbGroups() == 0) {
            addToGroup("", e);
        }

        if (group->empty())
            m_info_command.addGroupInfoEntity(group, Internal::InfoCommand::DISABLE);
        else
            m_info_command.addGroupInfoEntity(group, Internal::InfoCommand::DISPMODIFIED);

        return group;
    }

private:
    Internal::InfoCommand& m_info_command;
    GroupManager& m_group_manager;
};
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GROUP_HELPER_FOR_COMMAND_H_ */
