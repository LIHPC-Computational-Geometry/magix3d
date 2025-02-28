//
// Created by calderans on 10/29/24.
//
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Geom/IntersectionSurfaceLine.h"
#include "Geom/Surface.h"
#include "Geom/Vertex.h"
#include "Geom/GeomModificationBaseClass.h"
/*----------------------------------------------------------------------------*/
#include <gp_Lin.hxx>
#include <TopoDS.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
IntersectionSurfaceLine::IntersectionSurfaceLine(Surface *s, const Point pnt, const Vector dir)
: m_intersector(0),m_surf(s),m_point(pnt),m_dir(dir)
{
}
/*----------------------------------------------------------------------------*/
IntersectionSurfaceLine::~IntersectionSurfaceLine()
{
    m_intersector->Destroy();
}
/*----------------------------------------------------------------------------*/
void IntersectionSurfaceLine::perform()
{
    TopoDS_Shape tool_shape = m_surf->getOCCFaces()[0];
    TopoDS_Face tool_face = TopoDS::Face(tool_shape);

    m_intersector = new IntCurvesFace_Intersector(tool_face, 1);

    gp_Pnt gpPnt(m_point.getX(), m_point.getY(), m_point.getZ());
    gp_Dir gpDir(m_dir.getX(), m_dir.getY(), m_dir.getZ());
    gp_Lin gpLin(gpPnt, gpDir);

    m_intersector->Perform(gpLin, -RealLast(),RealLast());
}
/*----------------------------------------------------------------------------*/
int IntersectionSurfaceLine::getNbPnt()
{
    return m_intersector->NbPnt();
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point IntersectionSurfaceLine::getIntersection()
{
    gp_Pnt res = m_intersector->Pnt(1);
    return {res.X(),res.Y(),res.Z()};
}
/*----------------------------------------------------------------------------*/
} //namespace Geom
} // namespace Mgx3D