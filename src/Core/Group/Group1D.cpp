/*----------------------------------------------------------------------------*/
#include "Group/Group1D.h"
#include "Internal/Context.h"
#include "Internal/EntitiesHelper.h"
#include "Geom/Curve.h"
#include "Utils/SerializedRepresentation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
const char* Group1D::typeNameGroup1D = "Group1D";
/*----------------------------------------------------------------------------*/
Group1D::Group1D(Internal::Context& ctx,
        const std::string & nom,
		   bool isDefaultGroup, uint level)
: GroupEntity (ctx,
        ctx.newProperty(Utils::Entity::Group1D, nom),
        ctx.newDisplayProperties(Utils::Entity::Group1D),
		isDefaultGroup, level)
{
}
/*----------------------------------------------------------------------------*/
Group1D::~Group1D()
{
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Group1D::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            GroupEntity::getDescription (alsoComputed));

    std::vector<Geom::Curve*> curves = getFilteredEntities<Geom::Curve>();
    if (!curves.empty()){
        Utils::SerializedRepresentation sr ("Courbes géométriques",
                TkUtil::NumericConversions::toStr(curves.size()));
        for (Geom::Curve* curve : curves)
            sr.addProperty(Utils::SerializedRepresentation::Property(curve->getName(), *curve));
        description->addPropertiesSet(sr);
    }

    std::vector<Topo::CoEdge*> coedges = getFilteredEntities<Topo::CoEdge>();
    if (!coedges.empty()){
        Utils::SerializedRepresentation sr ("Arêtes topologiques",
                TkUtil::NumericConversions::toStr(coedges.size()));
        for (Topo::CoEdge* coedge : coedges)
        	sr.addProperty(Utils::SerializedRepresentation::Property(coedge->getName(), *coedge));
        description->addPropertiesSet(sr);
    }

   return description.release ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
