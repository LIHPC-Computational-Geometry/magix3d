#ifndef INTERSECTIONSURFACELINE_H
#define INTERSECTIONSURFACELINE_H
/*----------------------------------------------------------------------------*/
#include "Geom/GeomModificationBaseClass.h"
#include "Geom/GeomEntity.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <IntCurvesFace_Intersector.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Surface;
class Vertex;
/*----------------------------------------------------------------------------*/
class IntersectionSurfaceLine
{
public:
    IntersectionSurfaceLine(Surface* s, const Utils::Math::Point pnt, const Utils::Math::Vector dir);
    virtual ~IntersectionSurfaceLine();
    void perform();
    int getNbPnt();
    Utils::Math::Point getIntersection();

private:
    IntCurvesFace_Intersector* m_intersector;
    Surface* m_surf;
    Utils::Math::Point m_point;
    Utils::Math::Vector m_dir;
    TopoDS_Shape m_restricted_intersection_tool;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/


#endif //INTERSECTIONSURFACELINE_H
