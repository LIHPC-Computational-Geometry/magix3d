/*----------------------------------------------------------------------------*/
#ifndef GEOMPROJECTIMPLEMENTATION_H_
#define GEOMPROJECTIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Curve;
class Vertex;
class Surface;
/*----------------------------------------------------------------------------*/
using Point = Utils::Math::Point;
/*----------------------------------------------------------------------------*/
class GeomProjectImplementation
{
public:
    GeomProjectImplementation() = default;
    ~GeomProjectImplementation() = default;

    /*------------------------------------------------------------------------*/
    /** \brief Retourne le projete du point P sur le sommet.
     *  \return Le projete de P sur v
     */
    Point project(const Vertex* v, const Point& P) const;

    /*------------------------------------------------------------------------*/
    /** \brief Projette le point P sur la courbe.
     *  \return Le point projeté et l'indice de la edge OCC sur laquelle le
     *  point est projeté.
     */
    std::pair<Point, uint> project(const Curve* c, const Point& P) const;

    /*------------------------------------------------------------------------*/
    /** \brief Projette le point P sur la surface.
     *  \return Le point projeté et l'indice de la face OCC sur laquelle le
     *  point est projeté.
     */
    std::pair<Point, uint> project(const Surface* s, const Point& P) const;

private:
    Point _project(const TopoDS_Vertex& shape, const Point& P) const;
    Point _project(const TopoDS_Edge& shape, const Point& P) const;
    Point _project(const TopoDS_Face& shape, const Point& P) const;
    Point _shapeProject(const TopoDS_Shape& shape, const Point& P) const;

    template<typename T> std::pair<Point, uint> 
    _project(const std::vector<T> shapes, const Point& P) const;
};
/*----------------------------------------------------------------------------*/
class GeomProjectVisitor : public ConstGeomEntityVisitor 
{
public:
    GeomProjectVisitor(const Utils::Math::Point& pIn) : m_p_in(pIn) {}
    ~GeomProjectVisitor() = default;

    void visit(const Geom::Vertex* v) override {
        m_p_out = m_gpi.project(v, m_p_in);
    }

    void visit(const Geom::Curve* c) override {
        m_p_out = m_gpi.project(c, m_p_in).first;
    }

    void visit(const Geom::Surface* s) override {
        m_p_out = m_gpi.project(s, m_p_in).first;
    }

    void visit(const Geom::Volume* v) override {
        throw TkUtil::Exception (TkUtil::UTF8String ("Projection impossible sur un volume", TkUtil::Charset::UTF_8));
    }

    const Utils::Math::Point& getProjectedPoint() const { return m_p_out; }

private:
    GeomProjectImplementation m_gpi;
    Utils::Math::Point m_p_in;
    Utils::Math::Point m_p_out;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMPROJECTIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/
