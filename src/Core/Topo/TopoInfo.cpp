/*----------------------------------------------------------------------------*/
/*
 * \file TopoInfo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 14 mars 2019
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include <Topo/TopoInfo.h>

#include <Topo/Vertex.h>
#include <Topo/CoEdge.h>
#include <Topo/Edge.h>
#include <Topo/CoFace.h>
#include <Topo/Face.h>
#include <Topo/Block.h>

#include "Geom/GeomEntity.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoInfo::vertices() const{
    std::vector<std::string> v;
    for(int i=0;i<incident_vertices.size();i++)
        v.push_back(incident_vertices[i]->getName());
    return v;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoInfo::coedges() const{
    std::vector<std::string> v;
    for(int i=0;i<incident_coedges.size();i++)
        v.push_back(incident_coedges[i]->getName());
    return v;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoInfo::edges() const{
    std::vector<std::string> v;
    for(int i=0;i<incident_edges.size();i++)
        v.push_back(incident_edges[i]->getName());
    return v;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoInfo::cofaces() const{
    std::vector<std::string> v;
    for(int i=0;i<incident_cofaces.size();i++)
        v.push_back(incident_cofaces[i]->getName());
    return v;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoInfo::faces() const{
    std::vector<std::string> v;
    for(int i=0;i<incident_faces.size();i++)
        v.push_back(incident_faces[i]->getName());
    return v;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoInfo::blocks() const{
    std::vector<std::string> v;
    for(int i=0;i<incident_blocks.size();i++)
        v.push_back(incident_blocks[i]->getName());
    return v;
}
/*----------------------------------------------------------------------------*/
std::string TopoInfo::geomEntity() const
{
	if (geom_entity)
		return geom_entity->getName();
	else
		return std::string();
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> TopoInfo::groupsName() const
{
    return groups_name;
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
