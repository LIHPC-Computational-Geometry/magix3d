/*----------------------------------------------------------------------------*/
/*
 * TopoInfo.h
 *
 *  Created on: 14/3/2019
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef TOPOINFO_H_
#define TOPOINFO_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class GeomEntity;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class Vertex;
class CoEdge;
class Edge;
class CoFace;
class Face;
class Block;
/*----------------------------------------------------------------------------*/
class TopoInfo{

public:
    std::string name;
    int dimension;
#ifndef SWIG
    std::vector<Vertex*> incident_vertices;
    std::vector<CoEdge*> incident_coedges;
    std::vector<Edge*> incident_edges;
    std::vector<CoFace*> incident_cofaces;
    std::vector<Face*> incident_faces;
    std::vector<Block*> incident_blocks;
    Geom::GeomEntity* geom_entity;
    std::vector<std::string> groups_name;
#endif

    std::vector<std::string> vertices() const;
    std::vector<std::string> coedges() const;
    std::vector<std::string> edges() const;
    std::vector<std::string> cofaces() const;
    std::vector<std::string> faces() const;
    std::vector<std::string> blocks() const;
    std::string geomEntity() const;
    std::vector<std::string> groupsName() const;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* TOPOINFO_H_ */

/*----------------------------------------------------------------------------*/

