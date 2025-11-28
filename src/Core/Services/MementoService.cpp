#include "Services/MementoService.h"
#include "Geom/GeomEntity.h"
#include "Geom/GeomEntityVisitor.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Topo/TopoManager.h"
#include "Internal/Context.h"
#include "Group/GroupManager.h"
#include <TopoDS.hxx>

namespace Mgx3D::Services
{

    const Memento MementoService::createMemento(const Geom::GeomEntity *e) const
    {
        struct : Geom::ConstGeomEntityVisitor
        {
            void visit(const Geom::Vertex *v) override
            {
                fillCommonFields(v);
                mem.curves = const_cast<std::vector<Geom::Curve *> &>(v->getCurves());
                mem.occ_shapes = {v->getOCCVertex()};
            }

            void visit(const Geom::Curve *c) override
            {
                fillCommonFields(c);
                mem.surfaces = const_cast<std::vector<Geom::Surface *> &>(c->getSurfaces());
                mem.vertices = const_cast<std::vector<Geom::Vertex *> &>(c->getVertices());
                std::vector<TopoDS_Shape> shapes;
                for (auto e : c->getOCCEdges())
                    shapes.push_back(e);
                mem.occ_shapes = shapes;
            }

            void visit(const Geom::Surface *s) override
            {
                fillCommonFields(s);
                mem.curves = const_cast<std::vector<Geom::Curve *> &>(s->getCurves());
                mem.volumes = const_cast<std::vector<Geom::Volume *> &>(s->getVolumes());
                std::vector<TopoDS_Shape> shapes;
                for (auto f : s->getOCCFaces())
                    shapes.push_back(f);
                mem.occ_shapes = shapes;
            }

            void visit(const Geom::Volume *v) override
            {
                fillCommonFields(v);
                mem.surfaces = const_cast<std::vector<Geom::Surface *> &>(v->m_surfaces);
                mem.occ_shapes = {v->getOCCShape()};
            }

            void fillCommonFields(const Geom::GeomEntity *e)
            {
        		Topo::TopoManager& tm = e->getContext().getTopoManager();
                mem.topo_entities = tm.getRefTopos(e);
        		Group::GroupManager& gm = e->getContext().getGroupManager();
                mem.groups = gm.getGroupsFor(e);
                mem.property = e->getGeomProperty();
            }

            Memento mem;
        } create_memento_visitor;

        e->accept(create_memento_visitor);
        return create_memento_visitor.mem;
    }

    void MementoService::saveMemento(Geom::GeomEntity *e, const Memento &mem)
    {
        m_mementos.insert({e, mem});
    }

    void MementoService::permMementos()
    {
        for (auto it = m_mementos.begin(); it != m_mementos.end(); it++)
        {
            Geom::GeomEntity *e = it->first;
            Memento mem_saved = it->second;
            it->second = createMemento(e);
            setFromMemento(e, mem_saved);
        }
    }

    void MementoService::setFromMemento(Geom::GeomEntity *e, const Memento &mem)
    {
        struct : Geom::GeomEntityVisitor
        {
            void visit(Geom::Vertex *v) override
            {
                v->m_curves = m_mem.curves;
                v->m_occ_vertex = TopoDS::Vertex(m_mem.occ_shapes[0]);
                setCommonFields(v);
            }

            void visit(Geom::Curve *c) override
            {
                c->m_surfaces = m_mem.surfaces;
                c->m_vertices = m_mem.vertices;
                c->m_occ_edges.clear();
                for (auto sh : m_mem.occ_shapes)
                    c->m_occ_edges.push_back(TopoDS::Edge(sh));
                setCommonFields(c);
            }

            void visit(Geom::Surface *s) override
            {
                s->m_curves = m_mem.curves;
                s->m_volumes = m_mem.volumes;
                s->m_occ_faces.clear();
                for (auto sh : m_mem.occ_shapes)
                    s->m_occ_faces.push_back(TopoDS::Face(sh));
                setCommonFields(s);
            }

            void visit(Geom::Volume *v) override
            {
                v->m_surfaces = m_mem.surfaces;
                v->m_occ_shape = m_mem.occ_shapes[0];
                setCommonFields(v);
            }

            void setCommonFields(Geom::GeomEntity *e)
            {
        		Topo::TopoManager& tm = e->getContext().getTopoManager();
                tm.setRefTopos(e, m_mem.topo_entities);
        		Group::GroupManager& gm = e->getContext().getGroupManager();
                gm.setGroupsFor(e, m_mem.groups);
                e->setGeomProperty(m_mem.property);
                e->forceComputeArea();
            }

            Memento m_mem;
        } set_memento_visitor;

        set_memento_visitor.m_mem = mem;
        e->accept(set_memento_visitor);
    }
}
