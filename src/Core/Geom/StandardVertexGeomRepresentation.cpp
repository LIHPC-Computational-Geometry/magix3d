/*----------------------------------------------------------------------------*/
/** \file StandardVertexGeomRepresentation.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 04/11/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/StandardVertexGeomRepresentation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/GeomDisplayRepresentation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
StandardVertexGeomRepresentation::
StandardVertexGeomRepresentation(Internal::Context& c, const Utils::Math::Point& pnt)
:m_context(c), m_coord(pnt)
{}
/*----------------------------------------------------------------------------*/
StandardVertexGeomRepresentation::
StandardVertexGeomRepresentation(const StandardVertexGeomRepresentation& rep)
:m_context(rep.m_context), m_coord(rep.m_coord)
{}
/*----------------------------------------------------------------------------*/
StandardVertexGeomRepresentation::~StandardVertexGeomRepresentation()
{}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::split(std::vector<Surface*>& surf,
        std::vector<Curve*  >& curv, std::vector<Vertex* >&  vert,Volume* owner)
{
    surf.clear();
    curv.clear();
    vert.clear();
}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::split(std::vector<Curve*  >& curv,
        std::vector<Vertex* >&  vert,Surface* owner)
{
    curv.clear();
    vert.clear();
}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::split(
        std::vector<Vertex* >&  vert,Curve* owner)
{
    vert.clear();
}
/*----------------------------------------------------------------------------*/
double StandardVertexGeomRepresentation::computeVolumeArea()
{
    return 0;
}
/*----------------------------------------------------------------------------*/
double StandardVertexGeomRepresentation::computeSurfaceArea()
{
    return 0;
}
/*----------------------------------------------------------------------------*/
double StandardVertexGeomRepresentation::computeCurveArea()
{
    return 0;
}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::computeBoundingBox(
                            Utils::Math::Point& pmin,Utils::Math::Point& pmax, double tol) const
{
    double half_tol = tol/2;
    pmin.setX(m_coord.getX()-half_tol);
    pmin.setY(m_coord.getY()-half_tol);
    pmin.setZ(m_coord.getZ()-half_tol);

    pmax.setX(m_coord.getX()+half_tol);
    pmax.setY(m_coord.getY()+half_tol);
    pmax.setZ(m_coord.getZ()+half_tol);
}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::
project( Utils::Math::Point& P, const Curve* C)
{}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::
project( Utils::Math::Point& P, const Surface* S)
{}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::project(
        const Utils::Math::Point& P1, Utils::Math::Point& P2, const Curve* C)
{}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::project(
        const Utils::Math::Point& P1, Utils::Math::Point& P2, const Surface* S)
{}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2, const Surface* S)
{}
/*----------------------------------------------------------------------------*/
void tangent(const Utils::Math::Point& P1, Utils::Math::Vector& V2)
{}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::translate(const Utils::Math::Vector& V)
{
    m_coord.setX(m_coord.getX()+V.getX());
    m_coord.setY(m_coord.getY()+V.getY());
    m_coord.setZ(m_coord.getZ()+V.getZ());
}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::rotate(const Utils::Math::Point& P1,
        const Utils::Math::Point& P2, double Angle)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("Rotation non implementee pour les points geometriques STANDARDS", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::mirror(const Utils::Math::Plane& plane)
{
	throw TkUtil::Exception (TkUtil::UTF8String ("Symétrie non implementee pour les points geometriques STANDARDS", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
//void StandardVertexGeomRepresentation::updateShape(GeomEntity* e)
//{}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::
buildDisplayRepresentation(Utils::DisplayRepresentation& dr,
        const GeomEntity* caller) const
{
    if (dr.getDisplayType()!= Utils::DisplayRepresentation::DISPLAY_GEOM)
        throw TkUtil::Exception (TkUtil::UTF8String ("Invalid display type entity", TkUtil::Charset::UTF_8));

    Geom::GeomDisplayRepresentation* rep =
                dynamic_cast<Geom::GeomDisplayRepresentation*>(&dr);
    CHECK_NULL_PTR_ERROR(rep);

    std::vector<Utils::Math::Point>& rep_points = rep->getPoints();
    rep_points.push_back(m_coord);
}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::
getFacetedRepresentation(std::vector<gmds::math::Triangle >& AVec,
        const GeomEntity* caller) const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("StandardVertexGeomRepresentation::getFacetedRepresentation "
            "pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::
facetedRepresentationForwardOrient(
    		const GeomEntity* ACaller,
    		const GeomEntity* AEntityOrientation,
    		std::vector<gmds::math::Triangle >*) const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("StandardVertexGeomRepresentation::facetedRepresentationForwardOrient "
            "pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void StandardVertexGeomRepresentation::
tangent(const Utils::Math::Point& P1, Utils::Math::Vector& V2)
{
	throw TkUtil::Exception (TkUtil::UTF8String ("StandardVertexGeomRepresentation::tangent "
            "pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
GeomRepresentation* StandardVertexGeomRepresentation::clone() const
{
    GeomRepresentation* e= new StandardVertexGeomRepresentation(*this);

    return e;
}

/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
