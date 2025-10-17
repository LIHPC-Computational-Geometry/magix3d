/*----------------------------------------------------------------------------*/
#include "Group/Group2D.h"
#include "Internal/Context.h"
#include "Internal/EntitiesHelper.h"
#include "Geom/Surface.h"
#include "Utils/SerializedRepresentation.h"
#include "Mesh/Surface.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
const char* Group2D::typeNameGroup2D = "Group2D";
/*----------------------------------------------------------------------------*/
Group2D::Group2D(Internal::Context& ctx,
		const std::string & nom,
		bool isDefaultGroup,
		uint level)
: GroupEntity (ctx,
        ctx.newProperty(Utils::Entity::Group2D, nom),
        ctx.newDisplayProperties(Utils::Entity::Group2D),
		isDefaultGroup, level)
{
}
/*----------------------------------------------------------------------------*/
Group2D::~Group2D()
{
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Group2D::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            GroupEntity::getDescription (alsoComputed));

    std::vector<Geom::Surface*> surfaces = getFilteredEntities<Geom::Surface>();
    if (!surfaces.empty()){
        Utils::SerializedRepresentation sr ("Surfaces géométriques",
                TkUtil::NumericConversions::toStr(surfaces.size()));
        for (Geom::Surface* surface : surfaces)
            sr.addProperty(Utils::SerializedRepresentation::Property(surface->getName(), *surface));
        description->addPropertiesSet(sr);
    }

    std::vector<Topo::CoFace*> cofaces = getFilteredEntities<Topo::CoFace>();
    if (!cofaces.empty()){
        Utils::SerializedRepresentation sr ("Faces topologiques",
                TkUtil::NumericConversions::toStr(cofaces.size()));
        for (Topo::CoFace* coface : cofaces)
        	sr.addProperty(Utils::SerializedRepresentation::Property(coface->getName ( ), *coface));
        description->addPropertiesSet(sr);
    }

    // recherche du Mesh::Surface associé (suivant le nom)
    Mesh::Surface* msurf = getContext().getMeshManager().getSurface(getName(), false);
    if (msurf){
        Utils::SerializedRepresentation sr ("Surface maillage", std::string("1"));
        sr.addProperty(Utils::SerializedRepresentation::Property (msurf->getName(), *msurf));
        description->addPropertiesSet(sr);
    } else {
        description->addProperty (Utils::SerializedRepresentation::Property(
                        "Surface maillage", std::string("Aucune")));
    }

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
