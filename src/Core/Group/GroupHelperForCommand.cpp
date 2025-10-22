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
    switch (e->getDim())
    {
        case 0: return addToGroup<Group0D>(group_name, dynamic_cast<Geom::Vertex*>(e));
        case 1: return addToGroup<Group1D>(group_name, dynamic_cast<Geom::Curve*>(e));
        case 2: return addToGroup<Group2D>(group_name, dynamic_cast<Geom::Surface*>(e));
        default: return addToGroup<Group3D>(group_name, dynamic_cast<Geom::Volume*>(e));
    }
}
/*----------------------------------------------------------------------------*/
GroupEntity* GroupHelperForCommand::
removeFromGroup(const std::string& group_name, Geom::GeomEntity* e)
{
    switch (e->getDim())
    {
        case 0: return removeFromGroup<Group0D>(group_name, dynamic_cast<Geom::Vertex*>(e));
        case 1: return removeFromGroup<Group1D>(group_name, dynamic_cast<Geom::Curve*>(e));
        case 2: return removeFromGroup<Group2D>(group_name, dynamic_cast<Geom::Surface*>(e));
        default: return removeFromGroup<Group3D>(group_name, dynamic_cast<Geom::Volume*>(e));
    }
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
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
