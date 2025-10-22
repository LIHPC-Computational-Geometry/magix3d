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

    template <typename TGroup, typename TEntity>
    TGroup* addToGroup(const std::string& group_name, TEntity* e)
    {
        TGroup* grp = m_group_manager.getNewGroup<TGroup>(group_name, &m_info_command);
        addEntityToGroup(grp, e);
        return grp;
    }

    GroupEntity* addToGroup(const std::string& group_name, Geom::GeomEntity* e);
    GroupEntity* addToGroup(const std::string& group_name, Topo::TopoEntity* e);

    template <typename TGroup, typename TEntity>
    TGroup* removeFromGroup(const std::string& group_name, TEntity* e)
    {
        TGroup* grp = m_group_manager.getGroup<TGroup>(group_name, &m_info_command);
        removeEntityFromGroup(grp, e);
        return grp;
    }

    GroupEntity* removeFromGroup(const std::string& group_name, Geom::GeomEntity* e);
    GroupEntity* removeFromGroup(const std::string& group_name, Topo::TopoEntity* e);

private:
    template <typename TGroup, typename TEntity>
    void removeDefaultGroup(TEntity* entity)
    {
        if (entity->getNbGroups() == 1) {
            std::vector<TGroup*> groups = entity->getGroups();
            if (groups[0]->isDefaultGroup()) {
                groups[0]->remove(entity);
                entity->remove(groups[0]);
                if (groups[0]->empty())
                    m_info_command.addGroupInfoEntity(groups[0], Internal::InfoCommand::DISABLE);
                else
                    m_info_command.addGroupInfoEntity(groups[0], Internal::InfoCommand::DISPMODIFIED);
            }
        }
    }

    template <typename TGroup, typename TEntity>
    void addEntityToGroup(TGroup* group, TEntity* entity)
    {
        // il faut peut-être enlever le groupe par défaut
        removeDefaultGroup<TGroup, TEntity>(entity);

        // on ajoute sauf si l'entité est déjà dans un groupe
        // et que le groupe correspond au groupe par defaut
        if (! (entity->getNbGroups() > 0 && group->isDefaultGroup())) {
            group->add(entity);
            entity->add(group);
            m_info_command.addGroupInfoEntity(group, Internal::InfoCommand::DISPMODIFIED);	
        }
    }

    template <typename TGroup, typename TEntity>
    void removeEntityFromGroup(TGroup* group, TEntity* entity)
    {
        group->remove(entity);
        entity->remove(group);

        // il faut peut-être ajouter le groupe par défaut
        if (entity->getNbGroups() == 0) {
            addToGroup("", entity);
        }

        if (group->empty())
            m_info_command.addGroupInfoEntity(group, Internal::InfoCommand::DISABLE);
        else
            m_info_command.addGroupInfoEntity(group, Internal::InfoCommand::DISPMODIFIED);
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
