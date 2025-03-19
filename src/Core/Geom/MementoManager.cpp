#include "Geom/MementoManager.h"
#include "Geom/GeomEntity.h"
#include "Geom/GeomEntityVisitor.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
const MementoEntity MementoManager::
createMemento(const GeomEntity* e) const
{
    struct : ConstGeomEntityVisitor {
        void visit(const Vertex* v) override {
            fillCommonFields(v);
            mem.setCurves(const_cast<std::vector<Curve*>&>(v->getCurves()));
            mem.setGroups0D(const_cast<std::vector<Group::Group0D*>&>(v->getGroups()));
            std::vector<TopoDS_Shape> shapes = { v->getOCCVertex() };
            mem.setOCCShapes(shapes);
        }

        void visit(const Curve* c) override {
            fillCommonFields(c);
            mem.setSurfaces(const_cast<std::vector<Surface*>&>(c->getSurfaces()));
            mem.setVertices(const_cast<std::vector<Vertex*>&>(c->getVertices()));
            mem.setGroups1D(const_cast<std::vector<Group::Group1D*>&>(c->getGroups()));
            std::vector<TopoDS_Shape> shapes;
            for (auto e : c->getOCCEdges())
                shapes.push_back(e);
            mem.setOCCShapes(shapes);
        }

        void visit(const Surface* s) override {
            fillCommonFields(s);
            mem.setCurves(const_cast<std::vector<Curve*>&>(s->getCurves()));
            mem.setVolumes(const_cast<std::vector<Volume*>&>(s->getVolumes()));
            mem.setGroups2D(const_cast<std::vector<Group::Group2D*>&>(s->getGroups()));
            std::vector<TopoDS_Shape> shapes;
            for (auto f : s->getOCCFaces())
                shapes.push_back(f);
            mem.setOCCShapes(shapes);
        }

        void visit(const Volume* v) override {
            fillCommonFields(v);
            mem.setSurfaces(const_cast<std::vector<Surface*>&>(v->m_surfaces));
            mem.setGroups3D(const_cast<std::vector<Group::Group3D*>&>(v->m_groups));
            mem.setOCCShapes({ v->getOCCShape() });
        }

        void fillCommonFields(const GeomEntity* e) {
            mem.setTopoEntities(const_cast<std::vector<Topo::TopoEntity*>&>(e->getRefTopo()));
            mem.setProperty(e->getGeomProperty());
        }

        MementoEntity mem;
    } create_memento_visitor;

    e->accept(create_memento_visitor);
    return create_memento_visitor.mem;
}
/*----------------------------------------------------------------------------*/
void MementoManager::
saveMemento(GeomEntity* e, const MementoEntity& mem)
{
	m_mementos.insert({e, mem});
}
/*----------------------------------------------------------------------------*/
void MementoManager::
permMementos()
{
    for(auto v : m_mementos) {
        GeomEntity *e = v.first;
        MementoEntity mem_saved = v.second;
        v.second = createMemento(e);
        setFromMemento(e, mem_saved);
    }
}
/*----------------------------------------------------------------------------*/
void MementoManager::
setFromMemento(GeomEntity* e, const MementoEntity& mem)
{
    struct : GeomEntityVisitor {
        void visit(Vertex* v) override {
            v->m_curves = m_mem.getCurves();
            v->m_groups = m_mem.getGroups0D();
            v->m_occ_vertex = TopoDS::Vertex(m_mem.getOCCShapes()[0]);
            setCommonFields(v);
        }

        void visit(Curve* c) override {
            c->m_surfaces = m_mem.getSurfaces();
            c->m_vertices = m_mem.getVertices();
            c->m_groups = m_mem.getGroups1D();
            c->m_occ_edges.clear();
            for (auto sh : m_mem.getOCCShapes())
                c->m_occ_edges.push_back(TopoDS::Edge(sh));
            setCommonFields(c);

            // pour le cas des courbes composites, on force la mise à jour des paramètres
            if (!c->m_vertices.empty())
                c->computeParams(c->m_vertices[0]->getPoint());
        }

        void visit(Surface* s) override {
            s->m_curves = m_mem.getCurves();
            s->m_volumes = m_mem.getVolumes();
            s->m_groups  = m_mem.getGroups2D();
            s->m_occ_faces.clear();
            for (auto sh : m_mem.getOCCShapes())
                s->m_occ_faces.push_back(TopoDS::Face(sh));
            setCommonFields(s);
        }

        void visit(Volume* v) override {
            v->m_surfaces = m_mem.getSurfaces();
            v->m_groups  = m_mem.getGroups3D();
            v->m_occ_shape = m_mem.getOCCShapes()[0];
            setCommonFields(v);
        }

        void setCommonFields(GeomEntity* e) {
            e->m_topo_entities = m_mem.getTopoEntities();
            e->m_geomProp = m_mem.getProperty();
            e->m_computedAreaIsUpToDate = false;
        }

        MementoEntity m_mem;
    } set_memento_visitor;

    set_memento_visitor.m_mem = mem;
    e->accept(set_memento_visitor);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/