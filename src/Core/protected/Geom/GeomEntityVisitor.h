#ifndef GEOMENTITYVISITOR_H_
#define GEOMENTITYVISITOR_H_
/*----------------------------------------------------------------------------*/
#include <iostream>
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
struct GeomEntityVisitor
{
    virtual void visit(const Vertex*) = 0;
    virtual void visit(const Curve*) = 0;
    virtual void visit(const Surface*) = 0;
    virtual void visit(const Volume*) = 0;
    virtual ~GeomEntityVisitor() = default;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMENTITYVISITOR_H_ */
/*----------------------------------------------------------------------------*/