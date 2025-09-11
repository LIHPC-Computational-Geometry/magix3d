/*----------------------------------------------------------------------------*/
#ifndef GROUP_HELPER_FOR_COMMAND_H_
#define GROUP_HELPER_FOR_COMMAND_H_
/*----------------------------------------------------------------------------*/
#include "Internal/InfoCommand.h"
#include "Group/GroupManager.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
class GroupHelperForCommand {

public:
	GroupHelperForCommand(Internal::InfoCommand& info_command, GroupManager& group_manager);
	virtual ~GroupHelperForCommand() = default;

    Group3D* addToGroup(const std::string group_name, Geom::Volume* v);
    Group2D* addToGroup(const std::string group_name, Geom::Surface* s);
    Group1D* addToGroup(const std::string group_name, Geom::Curve* c);
    Group0D* addToGroup(const std::string group_name, Geom::Vertex* v);
    GroupEntity* addToGroup(const std::string group_name, Geom::GeomEntity* e);

    Group3D* addToGroup(const std::string group_name, Topo::Block* b);
    Group2D* addToGroup(const std::string group_name, Topo::CoFace* f);
    Group1D* addToGroup(const std::string group_name, Topo::CoEdge* e);
    Group0D* addToGroup(const std::string group_name, Topo::Vertex* v);

    Group3D* removeFromGroup(const std::string group_name, Geom::Volume* v);
    Group2D* removeFromGroup(const std::string group_name, Geom::Surface* s);
    Group1D* removeFromGroup(const std::string group_name, Geom::Curve* c);
    Group0D* removeFromGroup(const std::string group_name, Geom::Vertex* v);

    Group3D* removeFromGroup(const std::string group_name, Topo::Block* b);
    Group2D* removeFromGroup(const std::string group_name, Topo::CoFace* f);
    Group1D* removeFromGroup(const std::string group_name, Topo::CoEdge* e);
    Group0D* removeFromGroup(const std::string group_name, Topo::Vertex* v);

private:
    template <typename TGroup, typename TEntity>
    void removeDefaultGroup(TEntity* entity)
    {
        if (entity->getNbGroups() == 1) {
            std::vector<TGroup*> groups = entity->getGroups();
            if (groups[0]->isDefaultGroup()) {
                groups[0]->remove(entity);
                entity->remove(groups[0]);
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
