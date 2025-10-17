/*----------------------------------------------------------------------------*/
#include "Group/Group0D.h"
#include "Internal/Context.h"
#include "Internal/EntitiesHelper.h"
#include "Geom/Vertex.h"
#include "Utils/SerializedRepresentation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
const char* Group0D::typeNameGroup0D = "Group0D";
/*----------------------------------------------------------------------------*/
Group0D::Group0D(Internal::Context& ctx,
        const std::string & nom,
		   bool isDefaultGroup,
		   uint level)
: GroupEntity (ctx,
        ctx.newProperty(Utils::Entity::Group0D, nom),
        ctx.newDisplayProperties(Utils::Entity::Group0D),
		isDefaultGroup, level)
{
}
/*----------------------------------------------------------------------------*/
Group0D::~Group0D()
{
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Group0D::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            GroupEntity::getDescription (alsoComputed));

    std::vector<Geom::Vertex*> vertices = getFilteredEntities<Geom::Vertex>();
    if (!vertices.empty()){
        Utils::SerializedRepresentation sr ("Sommets géométriques",
                TkUtil::NumericConversions::toStr(vertices.size()));
        for (Geom::Vertex* v : vertices)
            sr.addProperty(Utils::SerializedRepresentation::Property(v->getName(), *v));
        description->addPropertiesSet (sr);
    }

    std::vector<Topo::Vertex*> topo_vertices = getFilteredEntities<Topo::Vertex>();
    if (!topo_vertices.empty()){
        Utils::SerializedRepresentation sr ("Sommets topologiques",
                TkUtil::NumericConversions::toStr(topo_vertices.size()));
        for (Topo::Vertex* v : topo_vertices)
            sr.addProperty(Utils::SerializedRepresentation::Property(v->getName(), *v));
        description->addPropertiesSet (sr);
    }

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
