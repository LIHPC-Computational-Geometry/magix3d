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

    Group3D* removeFromGroup(const std::string group_name, Geom::Volume* v);
    Group2D* removeFromGroup(const std::string group_name, Geom::Surface* s);
    Group1D* removeFromGroup(const std::string group_name, Geom::Curve* c);
    Group0D* removeFromGroup(const std::string group_name, Geom::Vertex* v);

private:
    template <typename T>
    void removeDefaultGroup(T* entity, const int dim)
    {
        if (entity->getNbGroups() == 1) {
            std::vector<std::string> gn;
            entity->getGroupsName(gn);
            if (gn[0] == m_group_manager.getDefaultName(dim))
                removeFromGroup("", entity);
        }
    }

    template <typename TGroup, typename TGeom>
    void addGeomEntityToGroup(TGroup* group, TGeom* geom_entity)
    {
        // il faut peut-être enlever le groupe par défaut
        removeDefaultGroup(geom_entity, geom_entity->getDim());

        // on ajoute sauf si l'entité est déjà dans un groupe
        // est que le groupe correspond au groupe par defaut
        if (! (geom_entity->getNbGroups() > 0 && group->isDefaultGroup())) {
            group->add(geom_entity);
            geom_entity->add(group);
            m_info_command.addGroupInfoEntity(group, Internal::InfoCommand::DISPMODIFIED);	
        }
    }

    template <typename TGroup, typename TGeom>
    void removeGeomEntityFromGroup(TGroup* group, TGeom* geom_entity)
    {
        group->remove(geom_entity);
        geom_entity->remove(group);
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
