/*----------------------------------------------------------------------------*/
//#define TEST_NEW_PROJECTION
/*----------------------------------------------------------------------------*/
#include "Geom/GeomProjectImplementation.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/Surface.h"
/*----------------------------------------------------------------------------*/
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#ifdef TEST_NEW_PROJECTION
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <ShapeAnalysis.hxx>
#endif
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
Point GeomProjectImplementation::
project(const Vertex* v, const Point& P) const
{
	return _project(v->getOCCVertex(), P);
}
/*----------------------------------------------------------------------------*/
std::pair<Point, uint> GeomProjectImplementation::
project(const Curve* c, const Point& P) const
{
	return _project(c->getOCCEdges(), P);
}
/*----------------------------------------------------------------------------*/
std::pair<Point, uint> GeomProjectImplementation::
project(const Surface* s, const Point& P) const
{
	return _project(s->getOCCFaces(), P);
}
/*----------------------------------------------------------------------------*/
Point GeomProjectImplementation::
_project(const TopoDS_Vertex& shape, const Point& P) const
{
    gp_Pnt pnt = BRep_Tool::Pnt(shape);
    return Point(pnt.X(), pnt.Y(), pnt.Z());
}
/*----------------------------------------------------------------------------*/
Point GeomProjectImplementation::
_project(const TopoDS_Edge& shape, const Point& P) const
{
	return _shapeProject(shape, P);
}
/*----------------------------------------------------------------------------*/
Point GeomProjectImplementation::
_project(const TopoDS_Face& shape, const Point& P) const
{
#ifdef TEST_NEW_PROJECTION
    Point PRet;

	// in gmsh ShapeFix_Wire::FixReorder is called in the surface setup step.
    // Is it mandatory when meshing in the UV parametric space
    // and not in our case ?

    double umin, umax, vmin, vmax = 0.;
    TopoDS_Face face = TopoDS::Face(shape);
    ShapeAnalysis::GetFaceUVBounds(face, umin, umax, vmin, vmax);
    Handle(Geom_Surface) surface = BRep_Tool::Surface(face);

    GeomAPI_ProjectPointOnSurf projector;
    gp_Pnt pnt(P.getX(),P.getY(),P.getZ());
    projector.Init(pnt, surface, umin, umax, vmin, vmax);
    bool isDone = false;
    if (projector.NbPoints() > 0) {
        gp_Pnt pnt2 = projector.NearestPoint();
        PRet.setXYZ(pnt2.X(), pnt2.Y(), pnt2.Z());
        isDone = true;
    }
    //    else {
    //        std::cerr<<"OCCGeomRepresentation::project ERROR "<<P<<" on surface " <<S<<std::endl;
    //        throw TkUtil::Exception("Echec d'une projection d'un point sur une courbe ou surface!!");
    //    }

    // this second projection in case the first one fails might not be necessary
    if(!isDone) {
        PRet = _shapeProject(shape, P);
    }

	return PRet;
#else
    return _shapeProject(shape, P);
#endif
}
/*----------------------------------------------------------------------------*/
Point GeomProjectImplementation::
_shapeProject(const TopoDS_Shape& shape, const Point& P) const
{
	Point PRet;

    gp_Pnt pnt(P.getX(),P.getY(),P.getZ());
    TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(pnt);
    BRepExtrema_DistShapeShape extrema(V, shape);
    bool isDone = extrema.IsDone();
    if (!isDone) {
        isDone = extrema.Perform();
    }
    if (!isDone) {
        std::cerr<<"OCCHelper::projectPointOn("<<P<<")\n";
        throw TkUtil::Exception("Echec d'une projection d'un point sur une courbe ou surface!!");
    }
    gp_Pnt pnt2 = extrema.PointOnShape2(1);
    PRet.setXYZ(pnt2.X(), pnt2.Y(), pnt2.Z());

	return PRet;
}
/*----------------------------------------------------------------------------*/
template<typename T> std::pair<Point, uint> GeomProjectImplementation::
_project(const std::vector<T> shapes, const Point& P) const
{
	Point PRet = _project(shapes[0], P);
	Utils::Math::Point pBest = PRet;
	uint idBest = 0;
	double norme2 = (PRet-P).norme2();
	for (uint i=1; i<shapes.size(); i++){
		PRet = _project(shapes[i], P);
		double dist = (PRet-P).norme2();
		if (dist<norme2){
			norme2 = dist;
			pBest = PRet;
        	idBest = i;
		}
	}
	PRet = pBest;
    return { PRet, idBest };
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
