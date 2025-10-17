/*----------------------------------------------------------------------------*/
#include "Group/Group3D.h"
#include "Internal/Context.h"
#include "Internal/EntitiesHelper.h"
#include "Geom/Volume.h"
#include "Mesh/Volume.h"
#include "Utils/SerializedRepresentation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/WarningLog.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
const char* Group3D::typeNameGroup3D = "Group3D";
/*----------------------------------------------------------------------------*/
Group3D::Group3D(Internal::Context& ctx,
        const std::string & nom,
		   bool isDefaultGroup, uint level)
: GroupEntity (ctx,
        ctx.newProperty(Utils::Entity::Group3D, nom),
        ctx.newDisplayProperties(Utils::Entity::Group3D),
		isDefaultGroup, level)
{
}
/*----------------------------------------------------------------------------*/
Group3D::~Group3D()
{
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Group3D::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            GroupEntity::getDescription (alsoComputed));

    std::vector<Geom::Volume*> volumes = getFilteredEntities<Geom::Volume>();
    if (!volumes.empty()){
        Utils::SerializedRepresentation sr ("Volumes géométriques",
                TkUtil::NumericConversions::toStr(volumes.size()));
        for (Geom::Volume* vol : volumes)
            sr.addProperty(Utils::SerializedRepresentation::Property(vol->getName ( ), *vol));
        description->addPropertiesSet(sr);
    }

    std::vector<Topo::Block*> blocks = getFilteredEntities<Topo::Block>();
    if (!blocks.empty()){
        Utils::SerializedRepresentation sr ("Blocs topologiques",
                TkUtil::NumericConversions::toStr(blocks.size()));
        for (Topo::Block* b : blocks)
        	sr.addProperty(Utils::SerializedRepresentation::Property(b->getName ( ), *b));
        description->addPropertiesSet(sr);
    }

    // recherche du Mesh::Volume associé (suivant le nom)
    Mesh::Volume* mvol = getContext().getMeshManager().getVolume(getName(), false);
    if (mvol){
        Utils::SerializedRepresentation sr ("Volume maillage", std::string("1"));
        sr.addProperty(Utils::SerializedRepresentation::Property (mvol->getName(), *mvol));
        description->addPropertiesSet(sr);
    } else {
        description->addProperty(Utils::SerializedRepresentation::Property(
                        "Volume maillage", std::string("Aucun")));
    }

    std::vector<CoordinateSystem::SysCoord*> scs = getFilteredEntities<CoordinateSystem::SysCoord>();
    if (!scs.empty()){
        Utils::SerializedRepresentation sr ("Repères",
                TkUtil::NumericConversions::toStr(scs.size()));
        for (CoordinateSystem::SysCoord* sc : scs)
        	sr.addProperty(Utils::SerializedRepresentation::Property(sc->getName(), *sc));
        description->addPropertiesSet(sr);
    }

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
