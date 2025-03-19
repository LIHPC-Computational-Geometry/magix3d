#ifndef INCIDENTGEOMENTITIESVISITOR_H_
#define INCIDENTGEOMENTITIESVISITOR_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Entity.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/GeomEntityVisitor.h"
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

    const std::set<GeomEntity*, decltype(&Utils::Entity::compareEntity)>& get() {return m_down_incident_entities;}
    const std::set<Surface*, decltype(&Utils::Entity::compareEntity)>& getSurfaces() {return m_down_incident_surfaces;}
    const std::set<Curve*, decltype(&Utils::Entity::compareEntity)>& getCurves() {return m_down_incident_curves;}
    const std::set<Vertex*, decltype(&Utils::Entity::compareEntity)>& getVertices() {return m_down_incident_vertices;}

    GetDownIncidentGeomEntitiesVisitor()
    : m_down_incident_entities(&Utils::Entity::compareEntity)
    , m_down_incident_surfaces(&Utils::Entity::compareEntity)
    , m_down_incident_curves(&Utils::Entity::compareEntity)
    , m_down_incident_vertices(&Utils::Entity::compareEntity)
    {}
    virtual ~GetDownIncidentGeomEntitiesVisitor() = default;

private:
    std::set<GeomEntity*, decltype(&Utils::Entity::compareEntity)> m_down_incident_entities;
    std::set<Surface*, decltype(&Utils::Entity::compareEntity)> m_down_incident_surfaces;
    std::set<Curve*, decltype(&Utils::Entity::compareEntity)> m_down_incident_curves;
    std::set<Vertex*, decltype(&Utils::Entity::compareEntity)> m_down_incident_vertices;
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

    const std::set<GeomEntity*, decltype(&Utils::Entity::compareEntity)>& get() {return m_up_incident_entities;}
    const std::set<Volume*, decltype(&Utils::Entity::compareEntity)>& getVolumes() {return m_up_incident_volumes;}
    const std::set<Surface*, decltype(&Utils::Entity::compareEntity)>& getSurfaces() {return m_up_incident_surfaces;}
    const std::set<Curve*, decltype(&Utils::Entity::compareEntity)>& getCurves() {return m_up_incident_curves;}

    GetUpIncidentGeomEntitiesVisitor()
    : m_up_incident_entities(&Utils::Entity::compareEntity)
    , m_up_incident_volumes(&Utils::Entity::compareEntity)
    , m_up_incident_surfaces(&Utils::Entity::compareEntity)
    , m_up_incident_curves(&Utils::Entity::compareEntity)
    {}
    virtual ~GetUpIncidentGeomEntitiesVisitor() = default;

private:    
    std::set<GeomEntity*, decltype(&Utils::Entity::compareEntity)> m_up_incident_entities;
    std::set<Volume*, decltype(&Utils::Entity::compareEntity)> m_up_incident_volumes;
    std::set<Surface*, decltype(&Utils::Entity::compareEntity)> m_up_incident_surfaces;
    std::set<Curve*, decltype(&Utils::Entity::compareEntity)> m_up_incident_curves;
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

    const std::set<GeomEntity*, decltype(&Utils::Entity::compareEntity)>& get() {return m_adjacent_entities;}
    const std::set<Volume*, decltype(&Utils::Entity::compareEntity)>& getVolumes() {return m_adjacent_volumes;}
    const std::set<Surface*, decltype(&Utils::Entity::compareEntity)>& getSurfaces() {return m_adjacent_surfaces;}
    const std::set<Curve*, decltype(&Utils::Entity::compareEntity)>& getCurves() {return m_adjacent_curves;}
    const std::set<Vertex*, decltype(&Utils::Entity::compareEntity)>& getVertices() {return m_adjacent_vertices;}
    
    GetAdjacentGeomEntitiesVisitor()
    : m_adjacent_entities(&Utils::Entity::compareEntity)
    , m_adjacent_volumes(&Utils::Entity::compareEntity)
    , m_adjacent_surfaces(&Utils::Entity::compareEntity)
    , m_adjacent_curves(&Utils::Entity::compareEntity)
    , m_adjacent_vertices(&Utils::Entity::compareEntity)
    {}
    virtual ~GetAdjacentGeomEntitiesVisitor() = default;

private:    
    std::set<GeomEntity*, decltype(&Utils::Entity::compareEntity)> m_adjacent_entities; 
    std::set<Volume*, decltype(&Utils::Entity::compareEntity)> m_adjacent_volumes;
    std::set<Surface*, decltype(&Utils::Entity::compareEntity)> m_adjacent_surfaces;
    std::set<Curve*, decltype(&Utils::Entity::compareEntity)> m_adjacent_curves;
    std::set<Vertex*, decltype(&Utils::Entity::compareEntity)> m_adjacent_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* INCIDENTGEOMENTITIESVISITOR_H_ */
/*----------------------------------------------------------------------------*/