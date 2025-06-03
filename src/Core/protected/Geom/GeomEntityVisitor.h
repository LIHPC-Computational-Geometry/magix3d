#ifndef GEOMENTITYVISITOR_H_
#define GEOMENTITYVISITOR_H_
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Vertex;
class Curve;
class Surface;
class Volume;
/*----------------------------------------------------------------------------*/
struct ConstGeomEntityVisitor
{
    virtual void visit(const Vertex*) = 0;
    virtual void visit(const Curve*) = 0;
    virtual void visit(const Surface*) = 0;
    virtual void visit(const Volume*) = 0;
    virtual ~ConstGeomEntityVisitor() = default;
};
struct GeomEntityVisitor
{
    virtual void visit(Vertex*) = 0;
    virtual void visit(Curve*) = 0;
    virtual void visit(Surface*) = 0;
    virtual void visit(Volume*) = 0;
    virtual ~GeomEntityVisitor() = default;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMENTITYVISITOR_H_ */
/*----------------------------------------------------------------------------*/