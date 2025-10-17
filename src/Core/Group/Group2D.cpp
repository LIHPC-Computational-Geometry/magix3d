#include "Group/Group0D.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Group {
/*----------------------------------------------------------------------------*/
const char* Group2D::typeNameGroup2D = "Group2D";
/*----------------------------------------------------------------------------*/
Group2D::Group2D(
    Internal::Context& ctx,
	const std::string & nom,
	bool isDefaultGroup,
	uint level)
: GroupEntity(
    ctx,
    ctx.newProperty(Utils::Entity::Group2D, nom),
    ctx.newDisplayProperties(Utils::Entity::Group2D),
	isDefaultGroup,
    level)
{
}
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D::Group
/*----------------------------------------------------------------------------*/
