#include "Group/Group0D.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Group {
/*----------------------------------------------------------------------------*/
const char* Group0D::typeNameGroup0D = "Group0D";
/*----------------------------------------------------------------------------*/
Group0D::Group0D(
    Internal::Context& ctx,
    const std::string & nom,
    bool isDefaultGroup,
    uint level)
: GroupEntity(
    ctx, 
    ctx.newProperty(Utils::Entity::Group0D, nom), 
    ctx.newDisplayProperties(Utils::Entity::Group0D), 
    isDefaultGroup, 
    level)
{
}
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D::Group
/*----------------------------------------------------------------------------*/
