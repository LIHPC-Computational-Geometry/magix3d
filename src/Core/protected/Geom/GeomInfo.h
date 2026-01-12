#ifndef GEOMINFO_H_
#define GEOMINFO_H_

#include <string>
#include <vector>

namespace Mgx3D::Geom
{
    struct GeomInfo
    {
        std::string name;
        int dimension;
        /// longueur d'une courbe, aire d'une surface, volume d'un volume
        double area;

        std::vector<std::string> vertices() const { return _vertices; }
        std::vector<std::string> curves() const { return _curves; }
        std::vector<std::string> surfaces() const { return _surfaces; }
        std::vector<std::string> volumes() const { return _volumes; }
        std::vector<std::string> topoEntities() const { return _topo_entities; }
        std::vector<std::string> groups() const { return _groups; }

#ifndef SWIG
        std::vector<std::string> _vertices;
        std::vector<std::string> _curves;
        std::vector<std::string> _surfaces;
        std::vector<std::string> _volumes;
        std::vector<std::string> _topo_entities;
        std::vector<std::string> _groups;
#endif
    };
}
#endif /* GEOMINFO_H_ */
