/*----------------------------------------------------------------------------*/
/*
 * GeomInfo.cpp
 *
 *  Created on: 12 avr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include <Geom/GeomInfo.h>
#include <Geom/Vertex.h>
#include <Geom/Curve.h>
#include <Geom/Surface.h>
#include <Geom/Volume.h>
#include "Topo/TopoEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomInfo::vertices() const{
    std::vector<std::string> v;
    for(int i=0;i<incident_vertices.size();i++)
        v.push_back(incident_vertices[i]->getName());
    return v;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomInfo::curves() const{
    std::vector<std::string> v;
    for(int i=0;i<incident_curves.size();i++)
        v.push_back(incident_curves[i]->getName());
    return v;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomInfo::surfaces() const{
    std::vector<std::string> v;
    for(int i=0;i<incident_surfaces.size();i++)
        v.push_back(incident_surfaces[i]->getName());
    return v;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomInfo::volumes() const{
    std::vector<std::string> v;
    for(int i=0;i<incident_volumes.size();i++)
        v.push_back(incident_volumes[i]->getName());
    return v;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomInfo::topoEntities() const
{
    std::vector<std::string> v;
    for(int i=0;i<topo_entities.size();i++)
        v.push_back(topo_entities[i]->getName());
    return v;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomInfo::groupsName() const
{
    return groups_name;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
