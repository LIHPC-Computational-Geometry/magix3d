/*----------------------------------------------------------------------------*/
/*
 * \file FacetedVertex.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 29/3/2018
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/FacetedVertex.h"
#include "Geom/FacetedHelper.h"
#include "Mesh/MeshImplementation.h"
#include "Utils/MgxNumeric.h"

#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>

#include <GMDS/Utils/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
FacetedVertex::FacetedVertex(Internal::Context& c, uint gmds_id, gmds::Node node)
:m_context(c), m_gmds_id(gmds_id), m_node(node)
{
}
/*----------------------------------------------------------------------------*/
FacetedVertex::FacetedVertex(const FacetedVertex& rep)
:m_context(rep.m_context), m_gmds_id(rep.m_gmds_id), m_node(rep.m_node)
{
}
/*----------------------------------------------------------------------------*/
FacetedVertex::~FacetedVertex()
{
}
/*----------------------------------------------------------------------------*/
GeomRepresentation* FacetedVertex::clone() const
{
	GeomRepresentation* e= new FacetedVertex(*this);
	return e;
}
/*----------------------------------------------------------------------------*/
double FacetedVertex::computeVolumeArea()
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedVertex::computeVolumeArea non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
double FacetedVertex::computeSurfaceArea()
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedVertex::computeSurfaceArea non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
double FacetedVertex::computeCurveArea()
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedVertex::computeCurveArea non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::computeBoundingBox(Utils::Math::Point& pmin,Utils::Math::Point& pmax, double tol) const
{
	pmin = Utils::Math::Point(m_node.X(), m_node.Y(), m_node.Z());
	pmax = pmin;
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::split(std::vector<Surface* >& surf,
           std::vector<Curve*>& curv,
           std::vector<Vertex* >&  vert,
           Volume* owner)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedVertex::split non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::split(std::vector<Curve*>& curv,
           std::vector<Vertex* >&  vert,
           Surface* owner)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedVertex::split non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::split( std::vector<Vertex* >&  vert,
            Curve* owner)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedVertex::split non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::project( Utils::Math::Point& P, const Curve* C)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("project (Pt, Courbe) non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::project( Utils::Math::Point& P, const Surface* S)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("project (Pt, Surface) non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::project(const Utils::Math::Point& P1, Utils::Math::Point& P2, const Surface* S)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("project Pt, Pt, Surface) non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::project(const Utils::Math::Point& P1, Utils::Math::Point& P2, const Curve* C)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("project Pt, Pt, Courbe) non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2, const Surface* S)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("normal Pt, Courbe) non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::getPoint(const double& p, Utils::Math::Point& Pt, const bool in01)
{
	Pt = Utils::Math::Point(m_node.X(), m_node.Y(), m_node.Z());
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::getIntersection(gp_Pln& plan_cut, Utils::Math::Point& Pt)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedVertex::getIntersection non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::getParameter(const Utils::Math::Point& Pt, double& p, const Curve* C)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedVertex::getParameter non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::getParameters(double& first, double& last)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedVertex::getParameters non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::translate(const Utils::Math::Vector& V)
{
	std::vector<gmds::Node>	nodes;
	nodes.push_back(m_node);


    // création de l'opérateur de translation via OCC
    gp_Trsf transf;
    gp_Vec vec(V.getX(),V.getY(),V.getZ());
    transf.SetTranslation(vec);

	FacetedHelper::transform(nodes, &transf);
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::scale(const double F, const Utils::Math::Point& center)
{
	std::vector<gmds::Node>	nodes;
	nodes.push_back(m_node);

	// création de l'opérateur d'homothétie via OCC
	gp_Trsf transf;
	transf.SetScale(gp_Pnt(center.getX(), center.getY(), center.getZ()), F);

	FacetedHelper::transform(nodes, &transf);
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::scale(const double factorX,
            const double factorY,
            const double factorZ)
{
	std::vector<gmds::Node>	nodes;
	nodes.push_back(m_node);

    // création de l'opérateur d'homothétie via OCC
    gp_GTrsf transf;
    transf.SetValue(1,1, factorX);
    transf.SetValue(2,2, factorY);
    transf.SetValue(3,3, factorZ);

	FacetedHelper::transform(nodes, &transf);
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::rotate(const Utils::Math::Point& P1,
        const Utils::Math::Point& P2, double Angle)
{
	std::vector<gmds::Node>	nodes;
	nodes.push_back(m_node);

	// création de l'opérateur de rotation via OCC
    gp_Trsf transf;

    gp_Pnt p1(P1.getX(),P1.getY(),P1.getZ());
    gp_Dir dir( P2.getX()-P1.getX(),
                P2.getY()-P1.getY(),
                P2.getZ()-P2.getZ());
    gp_Ax1 axis(p1,dir);
    transf.SetRotation(axis, Angle);

    FacetedHelper::transform(nodes, &transf);
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::mirror(const Utils::Math::Plane& plane)
{
	std::vector<gmds::Node>	nodes;
	nodes.push_back(m_node);

    // création de l'opérateur d'homothétie via OCC
    gp_Trsf transf;
    Utils::Math::Point plane_pnt = plane.getPoint();
    Utils::Math::Vector plane_vec = plane.getNormal();
    transf.SetMirror(gp_Ax2(gp_Pnt(plane_pnt.getX(), plane_pnt.getY(), plane_pnt.getZ()),
    		gp_Dir(plane_vec.getX(), plane_vec.getY(), plane_vec.getZ())));

    FacetedHelper::transform(nodes, &transf);
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::buildDisplayRepresentation(Utils::DisplayRepresentation& dr,
                                const GeomEntity*) const
{
    std::vector<Utils::Math::Point>& points = dr.getPoints();
    std::vector<size_t>& indicesFilaire = dr.getCurveDiscretization();

    points.clear();
    indicesFilaire.clear();

    points.push_back(Utils::Math::Point(m_node.X(), m_node.Y(), m_node.Z()));

}
/*----------------------------------------------------------------------------*/
void FacetedVertex::getFacetedRepresentation(
        std::vector<gmds::math::Triangle >& AVec,
        const GeomEntity* caller) const
{
	MGX_NOT_YET_IMPLEMENTED("getFacetedRepresentation à faire ?");
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::facetedRepresentationForwardOrient(
		const GeomEntity* ACaller,
		const GeomEntity* AEntityOrientation,
		std::vector<gmds::math::Triangle>*) const
{
	MGX_NOT_YET_IMPLEMENTED("facetedRepresentationForwardOrient à faire ?");
}
/*----------------------------------------------------------------------------*/
void FacetedVertex::tangent(const Utils::Math::Point& P1, Utils::Math::Vector& V2)
{
	MGX_NOT_YET_IMPLEMENTED("tangent à faire");
}
/*----------------------------------------------------------------------------*/
double FacetedVertex::getPrecision()
{
	return Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
