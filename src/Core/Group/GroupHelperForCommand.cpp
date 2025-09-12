/*----------------------------------------------------------------------------*/
#include "Group/GroupHelperForCommand.h"
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
Group3D* GroupHelperForCommand::
addToGroup(const std::string group_name, Geom::Volume* v)
{
	Group3D* grp = m_group_manager.getNewGroup3D(group_name, &m_info_command);
    addEntityToGroup(grp, v);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group2D* GroupHelperForCommand::
addToGroup(const std::string group_name, Geom::Surface* s)
{
	Group2D* grp = m_group_manager.getNewGroup2D(group_name, &m_info_command);
    addEntityToGroup(grp, s);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group1D* GroupHelperForCommand::
addToGroup(const std::string group_name, Geom::Curve* c)
{
	Group1D* grp = m_group_manager.getNewGroup1D(group_name, &m_info_command);
    addEntityToGroup(grp, c);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group0D* GroupHelperForCommand::
addToGroup(const std::string group_name, Geom::Vertex* v)
{
	Group0D* grp = m_group_manager.getNewGroup0D(group_name, &m_info_command);
    addEntityToGroup(grp, v);
    return grp;
}
/*----------------------------------------------------------------------------*/
GroupEntity* GroupHelperForCommand::
addToGroup(const std::string group_name, Geom::GeomEntity* e)
{
    switch (e->getDim())
    {
        case 0: return addToGroup(group_name, dynamic_cast<Geom::Vertex*>(e));
        case 1: return addToGroup(group_name, dynamic_cast<Geom::Curve*>(e));
        case 2: return addToGroup(group_name, dynamic_cast<Geom::Surface*>(e));
        default: return addToGroup(group_name, dynamic_cast<Geom::Volume*>(e));
    }
}
/*----------------------------------------------------------------------------*/
Group3D* GroupHelperForCommand::
addToGroup(const std::string group_name, Topo::Block* b)
{
	Group3D* grp = m_group_manager.getNewGroup3D(group_name, &m_info_command);
    addEntityToGroup(grp, b);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group2D* GroupHelperForCommand::
addToGroup(const std::string group_name, Topo::CoFace* f)
{
	Group2D* grp = m_group_manager.getNewGroup2D(group_name, &m_info_command);
    addEntityToGroup(grp, f);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group1D* GroupHelperForCommand::
addToGroup(const std::string group_name, Topo::CoEdge* e)
{
	Group1D* grp = m_group_manager.getNewGroup1D(group_name, &m_info_command);
    addEntityToGroup(grp, e);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group0D* GroupHelperForCommand::
addToGroup(const std::string group_name, Topo::Vertex* v)
{
	Group0D* grp = m_group_manager.getNewGroup0D(group_name, &m_info_command);
    addEntityToGroup(grp, v);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group3D* GroupHelperForCommand::
removeFromGroup(const std::string group_name, Geom::Volume* v)
{
	Group3D* grp = m_group_manager.getGroup3D(group_name, &m_info_command);
    removeEntityFromGroup(grp, v);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group2D* GroupHelperForCommand::
removeFromGroup(const std::string group_name, Geom::Surface* s)
{
	Group2D* grp = m_group_manager.getGroup2D(group_name, &m_info_command);
    removeEntityFromGroup(grp, s);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group1D* GroupHelperForCommand::
removeFromGroup(const std::string group_name, Geom::Curve* c)
{
	Group1D* grp = m_group_manager.getGroup1D(group_name, &m_info_command);
    removeEntityFromGroup(grp, c);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group0D* GroupHelperForCommand::
removeFromGroup(const std::string group_name, Geom::Vertex* v)
{
	Group0D* grp = m_group_manager.getGroup0D(group_name, &m_info_command);
    removeEntityFromGroup(grp, v);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group3D* GroupHelperForCommand::
removeFromGroup(const std::string group_name, Topo::Block* b)
{
	Group3D* grp = m_group_manager.getGroup3D(group_name, &m_info_command);
    removeEntityFromGroup(grp, b);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group2D* GroupHelperForCommand::
removeFromGroup(const std::string group_name, Topo::CoFace* f)
{
	Group2D* grp = m_group_manager.getGroup2D(group_name, &m_info_command);
    removeEntityFromGroup(grp, f);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group1D* GroupHelperForCommand::
removeFromGroup(const std::string group_name, Topo::CoEdge* e)
{
	Group1D* grp = m_group_manager.getGroup1D(group_name, &m_info_command);
    removeEntityFromGroup(grp, e);
    return grp;
}
/*----------------------------------------------------------------------------*/
Group0D* GroupHelperForCommand::
removeFromGroup(const std::string group_name, Topo::Vertex* v)
{
	Group0D* grp = m_group_manager.getGroup0D(group_name, &m_info_command);
    removeEntityFromGroup(grp, v);
    return grp;
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
