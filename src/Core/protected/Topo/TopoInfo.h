#ifndef TOPOINFO_H_
#define TOPOINFO_H_

#include <vector>
#include <string>

namespace Mgx3D::Topo
{
    struct TopoInfo
    {
        std::string name;
        int dimension;
        std::string geom_entity;

        std::vector<std::string> vertices() const { return _vertices; }
        std::vector<std::string> coedges() const { return _coedges; }
        std::vector<std::string> edges() const { return _edges; }
        std::vector<std::string> cofaces() const { return _cofaces; }
        std::vector<std::string> faces() const { return _faces; }
        std::vector<std::string> blocks() const { return _blocks; }
        std::vector<std::string> groups() const { return _groups; }

#ifndef SWIG
        std::vector<std::string> _vertices;
        std::vector<std::string> _coedges;
        std::vector<std::string> _edges;
        std::vector<std::string> _cofaces;
        std::vector<std::string> _faces;
        std::vector<std::string> _blocks;
        std::vector<std::string> _groups;
#endif
    };
} 
#endif /* TOPOINFO_H_ */

