/*----------------------------------------------------------------------------*/
/*
 * GeomInfo.h
 *
 *  Created on: 11 avr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMINFO_H_
#define GEOMINFO_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class TopoEntity;
}
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Vertex;
class Curve;
class Surface;
class Volume;
/*----------------------------------------------------------------------------*/
class GeomInfo{

public:
    std::string name;
    int dimension;
    /// longueur d'une courbe, aire d'une surface, volume d'un volume
    double area;
#ifndef SWIG
    std::vector<Vertex*> incident_vertices;
    std::vector<Curve*> incident_curves;
    std::vector<Surface*> incident_surfaces;
    std::vector<Volume*> incident_volumes;
    std::vector<Topo::TopoEntity*> topo_entities;
    std::vector<std::string> groups_name;
#endif

    std::vector<std::string> vertices() const;
    std::vector<std::string> curves() const;
    std::vector<std::string> surfaces() const;
    std::vector<std::string> volumes() const;
    std::vector<std::string> topoEntities() const;
    std::vector<std::string> groupsName() const;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMINFO_H_ */

/*----------------------------------------------------------------------------*/

