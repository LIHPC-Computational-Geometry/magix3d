#ifndef INCIDENTGEOMENTITIESVISITOR_H_
#define INCIDENTGEOMENTITIESVISITOR_H_
/*----------------------------------------------------------------------------*/
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/GeomEntityVisitor.h"
#include "Utils/Entity.h"
/*----------------------------------------------------------------------------*/
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GetDownIncidentGeomEntitiesVisitor : public ConstGeomEntityVisitor
{
public:   
    virtual void visit(const Vertex*) {}
    virtual void visit(const Curve* c) 
    {
        for (auto v : c->getVertices()) {
            m_down_incident_entities.insert(v);
            m_down_incident_vertices.insert(v);
        }
    }
    virtual void visit(const Surface* s)
    {
        for (auto c : s->getCurves())
        {
            m_down_incident_entities.insert(c);
            m_down_incident_curves.insert(c);
            for (auto v : c->getVertices()) {
                m_down_incident_entities.insert(v);
                m_down_incident_vertices.insert(v);
            }
        }
    }
    virtual void visit(const Volume* vol)
    {
        for (auto s : vol->getSurfaces())
        {
            m_down_incident_entities.insert(s);
            m_down_incident_surfaces.insert(s);
            for (auto c : s->getCurves())
            {
                m_down_incident_entities.insert(c);
                m_down_incident_curves.insert(c);
                for (auto v : c->getVertices()) {
                    m_down_incident_entities.insert(v);
                    m_down_incident_vertices.insert(v);
                }
            }      
        } 
    }

    const Utils::EntitySet<GeomEntity*>& get() {return m_down_incident_entities;}
    const Utils::EntitySet<Surface*>& getSurfaces() {return m_down_incident_surfaces;}
    const Utils::EntitySet<Curve*>& getCurves() {return m_down_incident_curves;}
    const Utils::EntitySet<Vertex*>& getVertices() {return m_down_incident_vertices;}

    GetDownIncidentGeomEntitiesVisitor()
    : m_down_incident_entities(Utils::Entity::compareEntity)
    , m_down_incident_surfaces(Utils::Entity::compareEntity)
    , m_down_incident_curves(Utils::Entity::compareEntity)
    , m_down_incident_vertices(Utils::Entity::compareEntity)
    {}
    virtual ~GetDownIncidentGeomEntitiesVisitor() = default;

private:
    Utils::EntitySet<GeomEntity*> m_down_incident_entities;
    Utils::EntitySet<Surface*> m_down_incident_surfaces;
    Utils::EntitySet<Curve*> m_down_incident_curves;
    Utils::EntitySet<Vertex*> m_down_incident_vertices;
};

class GetUpIncidentGeomEntitiesVisitor : public ConstGeomEntityVisitor
{
public:
    virtual void visit(const Vertex* vertex)
    {
        for (auto c : vertex->getCurves())
        {
            m_up_incident_entities.insert(c);
            m_up_incident_curves.insert(c);
            for (auto s : c->getSurfaces())
            {
                m_up_incident_entities.insert(s);
                m_up_incident_surfaces.insert(s);
                for (auto v : s->getVolumes()) {
                    m_up_incident_entities.insert(v);
                    m_up_incident_volumes.insert(v);
                }
            }
        }
    }
    virtual void visit(const Curve* c)
    {
        for (auto s : c->getSurfaces())
        {
            m_up_incident_entities.insert(s);
            m_up_incident_surfaces.insert(s);
            for (auto v : s->getVolumes()) {
                m_up_incident_entities.insert(v);
                m_up_incident_volumes.insert(v);
            }
        }
    }
    virtual void visit(const Surface* s)
    {
        for (auto v : s->getVolumes()) {
            m_up_incident_entities.insert(v);
            m_up_incident_volumes.insert(v);
        }
    }
    virtual void visit(const Volume*) {}

    const Utils::EntitySet<GeomEntity*>& get() {return m_up_incident_entities;}
    const Utils::EntitySet<Volume*>& getVolumes() {return m_up_incident_volumes;}
    const Utils::EntitySet<Surface*>& getSurfaces() {return m_up_incident_surfaces;}
    const Utils::EntitySet<Curve*>& getCurves() {return m_up_incident_curves;}

    GetUpIncidentGeomEntitiesVisitor()
    : m_up_incident_entities(Utils::Entity::compareEntity)
    , m_up_incident_volumes(Utils::Entity::compareEntity)
    , m_up_incident_surfaces(Utils::Entity::compareEntity)
    , m_up_incident_curves(Utils::Entity::compareEntity)
    {}
    virtual ~GetUpIncidentGeomEntitiesVisitor() = default;

private:    
    Utils::EntitySet<GeomEntity*> m_up_incident_entities;
    Utils::EntitySet<Volume*> m_up_incident_volumes;
    Utils::EntitySet<Surface*> m_up_incident_surfaces;
    Utils::EntitySet<Curve*> m_up_incident_curves;
};

class GetAdjacentGeomEntitiesVisitor : public ConstGeomEntityVisitor
{
public:
    virtual void visit(const Vertex* vertex)
    {
        for (auto c : vertex->getCurves())
            for (auto adjv : c->getVertices())
                m_adjacent_vertices.insert(adjv);
                
        m_adjacent_vertices.erase(const_cast<Vertex*>(vertex));
        m_adjacent_entities.insert(m_adjacent_vertices.begin(), m_adjacent_vertices.end());
    }
    virtual void visit(const Curve* c)
    {
        for (auto v : c->getVertices())
            for (auto adjc : v->getCurves())
                m_adjacent_curves.insert(adjc);

        m_adjacent_curves.erase(const_cast<Curve*>(c)); 
        m_adjacent_entities.insert(m_adjacent_curves.begin(), m_adjacent_curves.end());
   }
    virtual void visit(const Surface* s)
    {
        for (auto c : s->getCurves())
            for (auto adjs : c->getSurfaces())
                m_adjacent_surfaces.insert(adjs);

        m_adjacent_surfaces.erase(const_cast<Surface*>(s)); 
        m_adjacent_entities.insert(m_adjacent_surfaces.begin(), m_adjacent_surfaces.end());
   }
    virtual void visit(const Volume* v)
    {
        for (auto s : v->getSurfaces())
            for (auto adjv : s->getVolumes())
                m_adjacent_volumes.insert(adjv);

        m_adjacent_volumes.erase(const_cast<Volume*>(v));
        m_adjacent_entities.insert(m_adjacent_volumes.begin(), m_adjacent_volumes.end());
    }

    const Utils::EntitySet<GeomEntity*>& get() {return m_adjacent_entities;}
    const Utils::EntitySet<Volume*>& getVolumes() {return m_adjacent_volumes;}
    const Utils::EntitySet<Surface*>& getSurfaces() {return m_adjacent_surfaces;}
    const Utils::EntitySet<Curve*>& getCurves() {return m_adjacent_curves;}
    const Utils::EntitySet<Vertex*>& getVertices() {return m_adjacent_vertices;}
    
    GetAdjacentGeomEntitiesVisitor()
    : m_adjacent_entities(Utils::Entity::compareEntity)
    , m_adjacent_volumes(Utils::Entity::compareEntity)
    , m_adjacent_surfaces(Utils::Entity::compareEntity)
    , m_adjacent_curves(Utils::Entity::compareEntity)
    , m_adjacent_vertices(Utils::Entity::compareEntity)
    {}
    virtual ~GetAdjacentGeomEntitiesVisitor() = default;

private:    
    Utils::EntitySet<GeomEntity*> m_adjacent_entities; 
    Utils::EntitySet<Volume*> m_adjacent_volumes;
    Utils::EntitySet<Surface*> m_adjacent_surfaces;
    Utils::EntitySet<Curve*> m_adjacent_curves;
    Utils::EntitySet<Vertex*> m_adjacent_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* INCIDENTGEOMENTITIESVISITOR_H_ */
/*----------------------------------------------------------------------------*/