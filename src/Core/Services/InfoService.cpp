#include "Services/InfoService.h"
#include "Internal/Context.h"
#include "Geom/GeomEntity.h"
#include "Geom/IncidentGeomEntitiesVisitor.h"
#include "Topo/TopoEntity.h"
#include "Group/GroupManager.h"

namespace Mgx3D::Services
{
    Geom::GeomInfo InfoService::getInfos(const Geom::GeomEntity* e)
    {
        Geom::GeomInfo info;
        info.name = e->getName();
        info.dimension = e->getDim();
        info.area = e->computeArea();

        Geom::GetUpIncidentGeomEntitiesVisitor vup;
        Geom::GetDownIncidentGeomEntitiesVisitor vdown;
        Geom::GetAdjacentGeomEntitiesVisitor vadj;
        e->accept(vup);
        e->accept(vdown);
        e->accept(vadj);

        for (Geom::Vertex* v : vdown.getVertices())
            info._vertices.push_back(v->getName());
        for (Geom::Vertex* v : vadj.getVertices())
            info._vertices.push_back(v->getName());

        for (Geom::Curve* c : vup.getCurves())
            info._curves.push_back(c->getName());
        for (Geom::Curve* c : vdown.getCurves())
            info._curves.push_back(c->getName());
        for (Geom::Curve* c : vadj.getCurves())
            info._curves.push_back(c->getName());

        for (Geom::Surface* s : vup.getSurfaces())
            info._surfaces.push_back(s->getName());
        for (Geom::Surface* s : vdown.getSurfaces())
            info._surfaces.push_back(s->getName());
        for (Geom::Surface* s : vadj.getSurfaces())
            info._surfaces.push_back(s->getName());

        for (Geom::Volume* v : vup.getVolumes())
            info._volumes.push_back(v->getName());
        for (Geom::Volume* v : vadj.getVolumes())
            info._volumes.push_back(v->getName());

		Topo::TopoManager& tm = e->getContext().getTopoManager();
        for (Topo::TopoEntity* te : tm.getRefTopos(e))
            info._topo_entities.push_back(e->getName());

		Group::GroupManager& gm = e->getContext().getGroupManager();
		info._groups = Utils::toNames(gm.getGroupsFor(e));

        return info;
    }
}
