#include "Group/Group1D.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Group {
/*----------------------------------------------------------------------------*/
const char* Group1D::typeNameGroup1D = "Group1D";
/*----------------------------------------------------------------------------*/
Group1D::Group1D(
    Internal::Context& ctx,
    const std::string & nom,
    bool isDefaultGroup,
    uint level)
: GroupEntity (
    ctx,
    ctx.newProperty(Utils::Entity::Group1D, nom),
    ctx.newDisplayProperties(Utils::Entity::Group1D),
	isDefaultGroup,
    level)
{
}
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D::Group
/*----------------------------------------------------------------------------*/
