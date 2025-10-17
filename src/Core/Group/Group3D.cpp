#include "Group/Group0D.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Group {
/*----------------------------------------------------------------------------*/
const char* Group3D::typeNameGroup3D = "Group3D";
/*----------------------------------------------------------------------------*/
Group3D::Group3D(
    Internal::Context& ctx,
    const std::string & nom,
	bool isDefaultGroup,
    uint level)
: GroupEntity(
    ctx,
    ctx.newProperty(Utils::Entity::Group3D, nom),
    ctx.newDisplayProperties(Utils::Entity::Group3D),
	isDefaultGroup,
    level)
{
}
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D::Group
/*----------------------------------------------------------------------------*/
