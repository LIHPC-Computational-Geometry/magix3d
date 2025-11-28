/*----------------------------------------------------------------------------*/
#include "Group/GroupHelperForCommand.h"
#include "Group/GroupManager.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
GroupHelperForCommand::
GroupHelperForCommand(Internal::InfoCommand& info_command, GroupManager& group_manager)
: m_info_command(info_command)
, m_group_manager(group_manager)
{
}
/*----------------------------------------------------------------------------*/
GroupEntity* GroupHelperForCommand::
addToGroup(const std::string& group_name, Geom::GeomEntity* e)
{
    GroupEntity* group = m_group_manager.getNewGroup(group_name, e->getDim(), &m_info_command);

    // il faut peut-être enlever le groupe par défaut
    std::vector<GroupEntity*> groups = m_group_manager.getGroupsFor(e);
    if (groups.size() == 1) {
        if (groups[0]->isDefaultGroup()) {
            groups[0]->remove(e);
            m_group_manager.removeGroupFor(e, groups[0]);
            if (groups[0]->empty())
                m_info_command.addGroupInfoEntity(groups[0], Internal::InfoCommand::DISABLE);
            else
                m_info_command.addGroupInfoEntity(groups[0], Internal::InfoCommand::DISPMODIFIED);
        }
    }

    // on ajoute sauf si l'entité est déjà dans un groupe
    // et que le groupe correspond au groupe par defaut
    if (! (m_group_manager.getGroupsFor(e).size() > 0 && group->isDefaultGroup())) {
        group->add(e);
        m_group_manager.addGroupFor(e, group);
        m_info_command.addGeomInfoEntity (e, Internal::InfoCommand::DISPMODIFIED);
        m_info_command.addGroupInfoEntity(group, Internal::InfoCommand::DISPMODIFIED);	
    }

    return group;
}
/*----------------------------------------------------------------------------*/
GroupEntity* GroupHelperForCommand::
removeFromGroup(const std::string& group_name, Geom::GeomEntity* e)
{
    GroupEntity* group = m_group_manager.getGroup(group_name, e->getDim(), &m_info_command);
    group->remove(e);
    m_group_manager.removeGroupFor(e, group);

    // il faut peut-être ajouter le groupe par défaut
    if (m_group_manager.getGroupsFor(e).empty()) {
        addToGroup("", e);
    }

    if (group->empty())
        m_info_command.addGroupInfoEntity(group, Internal::InfoCommand::DISABLE);
    else
        m_info_command.addGroupInfoEntity(group, Internal::InfoCommand::DISPMODIFIED);

    return group;
}
/*----------------------------------------------------------------------------*/
GroupEntity* GroupHelperForCommand::
addToGroup(const std::string& group_name, Topo::TopoEntity* e)
{
    switch (e->getDim())
    {
        case 0: return addToGroup<Group0D>(group_name, dynamic_cast<Topo::Vertex*>(e));
        case 1: return addToGroup<Group1D>(group_name, dynamic_cast<Topo::CoEdge*>(e));
        case 2: return addToGroup<Group2D>(group_name, dynamic_cast<Topo::CoFace*>(e));
        default: return addToGroup<Group3D>(group_name, dynamic_cast<Topo::Block*>(e));
    }
}
/*----------------------------------------------------------------------------*/
GroupEntity* GroupHelperForCommand::
removeFromGroup(const std::string& group_name, Topo::TopoEntity* e)
{
    switch (e->getDim())
    {
        case 0: return removeFromGroup<Group0D>(group_name, dynamic_cast<Topo::Vertex*>(e));
        case 1: return removeFromGroup<Group1D>(group_name, dynamic_cast<Topo::CoEdge*>(e));
        case 2: return removeFromGroup<Group2D>(group_name, dynamic_cast<Topo::CoFace*>(e));
        default: return removeFromGroup<Group3D>(group_name, dynamic_cast<Topo::Block*>(e));
    }
}
/*----------------------------------------------------------------------------*/
std::vector<GroupEntity*>  GroupHelperForCommand::
getGroupsFor(const Topo::TopoEntity* e)
{
    std::vector<GroupEntity*> groups;
    switch (e->getDim())
    {
        case 0: {
            auto v = dynamic_cast<const Topo::Vertex*>(e)->getGroups();
            for (Group0D* g : v) groups.push_back(g);
            break;
        }
        case 1: {
            auto v = dynamic_cast<const Topo::CoEdge*>(e)->getGroups();
            for (Group1D* g : v) groups.push_back(g);
            break;
        }
        case 2: {
            auto v = dynamic_cast<const Topo::CoFace*>(e)->getGroups();
            for (Group2D* g : v) groups.push_back(g);
            break;
        }
        default: {
            auto v = dynamic_cast<const Topo::Block*>(e)->getGroups();
            for (Group3D* g : v) groups.push_back(g);
            break;
        }
    }
    return groups;
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
