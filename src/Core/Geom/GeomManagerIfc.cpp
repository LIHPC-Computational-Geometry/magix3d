/*----------------------------------------------------------------------------*/
/** \file GeomManagerIfc.cpp
 *
 *  \author Franck Ledoux, Eric Brière de l'Isle, Charles Pignerol
 *
 *  \date 01/02/2012
 *
 *
 *
 *  Modified on: 21/02/2022
 *      Author: Simon C
 *      ajout de la possibilité de conserver les entités pour la fonction makeExtrude
 */
/*----------------------------------------------------------------------------*/
#include "Geom/GeomManagerIfc.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"
#include "Utils/CommandManager.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomManagerIfc::GeomManagerIfc(const std::string& name, Internal::ContextIfc* c)
:Internal::CommandCreator(name, c)
{
}
/*----------------------------------------------------------------------------*/
GeomManagerIfc::GeomManagerIfc(const GeomManagerIfc& gm)
:Internal::CommandCreator(gm)
{
	MGX_FORBIDDEN ("GeomManagerIfc copy constructor is not allowed.")
}
/*----------------------------------------------------------------------------*/
GeomManagerIfc& GeomManagerIfc::operator =(const GeomManagerIfc& gm)
{
	MGX_FORBIDDEN ("GeomManagerIfc::operator = is not allowed.")
	return *this;
}
/*----------------------------------------------------------------------------*/
GeomManagerIfc::~GeomManagerIfc ( )
{
}
/*----------------------------------------------------------------------------*/
void GeomManagerIfc::clear()
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::clear should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Geom::GeomInfo GeomManagerIfc::getInfos(std::string e, int dim)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getInfos should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Utils::Math::Point GeomManagerIfc::getCoord(const std::string& name) const
{
	 throw TkUtil::Exception ("GeomManagerIfc::getCoord should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::
newVertex(std::string vertexName,std::string curveName, std::string groupName){
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newVertex should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::copy(std::vector<std::string>& e, bool withTopo, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::copy should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newVolume(std::vector<std::string>& e, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newVolume should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newVertex(std::string curveName, const double& param, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newVertex should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newVertex(std::string vertex1Name, std::string vertex2Name, const double& param, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newVertex should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newVertex(const Point& p, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newVertex should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newVertex(const double& x, const double& y, const double& z, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newVertex should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newSegment(std::string n1,std::string n2, std::string groupName)
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newSegment should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newPrism(std::string base, const Vector& v,
                              std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newPrism should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newPrismWithTopo(std::string base, const Vector& v,
                              std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newPrismWithTopo should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
GeomManagerIfc::newCone(const double& dr1, const double& dr2,
		const Vector& dv, const double& da, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newCone should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
GeomManagerIfc::newCone(const double& dr1, const double& dr2,
		const Vector& dv, const  Utils::Portion::Type& dt, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newCone should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::makeRevol( std::vector<std::string>& entities,
        const Utils::Math::Rotation& rot, const bool keep)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::makeRevol should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::makeExtrude(std::vector<std::string>& entities,
		const Vector& dp, const bool keep)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::makeExtrude should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::makeBlocksByExtrude(std::vector<std::string>& entities,
		const Vector& dp, const bool keep)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::makeBlocksByExtrude should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newSurfaceByOffset(std::string name, const double& offset, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newSurfaceByOffset should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newPlanarSurface(std::vector<std::string>& curve_names, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newPlanarSurface should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newFacetedSurface(std::string nom)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newFacetedSurface should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
GeomManagerIfc::newVerticesCurvesAndPlanarSurface(std::vector<Utils::Math::Point>& points, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newVerticesCurvesAndPlanarSurface should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
GeomManagerIfc::newCurveByCurveProjectionOnSurface(const std::string& curveName, const std::string& surfaceName, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newCurveByCurveProjectionOnSurface should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::destroy(std::vector<std::string>& es, bool propagateDown)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::destroy should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::destroyWithTopo(std::vector<std::string>& es, bool propagateDown)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::destroyWithTopo should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::scale(std::vector<std::string>& geo, const double factor)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::scale should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::scale(std::vector<std::string>& geo, const double factor, const Point& pcentre)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::scale should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* GeomManagerIfc::scaleAll(const double factor, const Point& pcentre)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::scaleAll should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::scale(std::vector<std::string>& geo,
		const double factorX,
		const double factorY,
		const double factorZ)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::scale should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* GeomManagerIfc::scaleAll(
		const double factorX,
		const double factorY,
		const double factorZ)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::scaleAll should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::copyAndScale(std::vector<std::string>& geo, const double factor, bool withTopo, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::copyAndScale should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::copyAndScale(std::vector<std::string>& geo, const double factor, const Point& pcentre, bool withTopo, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::copyAndScale should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* GeomManagerIfc::copyAndScaleAll(const double factor, const Point& pcentre, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::copyAndScaleAll should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::copyAndScale(std::vector<std::string>& geo,
		const double factorX,
		const double factorY,
		const double factorZ,
		bool withTopo,
		std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::copyAndScale should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc* GeomManagerIfc::copyAndScaleAll(
		const double factorX,
		const double factorY,
		const double factorZ,
		std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::copyAndScaleAll should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
GeomManagerIfc::mirror(std::vector<std::string>& geo, Utils::Math::Plane* plane)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::mirror should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Mgx3D::Internal::M3DCommandResultIfc*
GeomManagerIfc::copyAndMirror(std::vector<std::string>& geo, Utils::Math::Plane* plane, bool withTopo, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::copyAndMirror should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newBox(
				const Utils::Math::Point& pmin, const Utils::Math::Point& pmax, std::string groupName)
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newBox should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::
newCircle(std::string p1, std::string p2, std::string p3, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newCircle should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::
newArcCircle(std::string pc, std::string pd, std::string pe, const bool direct, const Vector& normal, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newArcCircle should be overloaded.", TkUtil::Charset::UTF_8));
}
Internal::M3DCommandResultIfc* GeomManagerIfc::
newArcCircle(std::string pc, std::string pd, std::string pe, const bool direct, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newArcCircle should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::
newArcCircle(const double& angleDep, const double& angleFin, const double& rayon, std::string sysCoordName , std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newArcEllipse should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::
newArcEllipse(std::string pc, std::string pd, std::string pe, const bool direct, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newArcEllipse should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newCylinder(
    const Utils::Math::Point& pcentre, const double& dr,
    const Utils::Math::Vector& dv,
    const double& da,
    std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newCylinder should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newCylinder(
    const Utils::Math::Point& pcentre, const double& dr,
    const Utils::Math::Vector& dv,
    const Utils::Portion::Type& dt,
    std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newCylinder should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newHollowCylinder(
    const Utils::Math::Point& pcentre,
    const double& dr_int,
    const double& dr_ext,
    const Utils::Math::Vector& dv,
    const double& da,
    std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newHollowCylinder should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newHollowCylinder(
    const Utils::Math::Point& pcentre,
    const double& dr_int,
    const double& dr_ext,
    const Utils::Math::Vector& dv,
    const Utils::Portion::Type& dt,
    std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newHollowCylinder should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newSphere(const Point& pnt,  const double& rad,
        const double& angl,
        std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newSphere should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newSphere(const Point& pnt,  const double& rad,
        const Utils::Portion::Type& dt,
        std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newSphere should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newSpherePart(const double& radius,
                      const double& angleY,
                      const double& angleZ,
                      std::string groupName)
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newSpherePart should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newHollowSpherePart(const double& dr_int,
		const double& dr_ext,
		const double& angleY,
		const double& angleZ,
		std::string groupName)
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newHollowSpherePart should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newHollowSphere(
        const Point& pnt,
        const double& radius_int,
        const double& radius_ext,
        const double& angl,
        std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newSphere should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newHollowSphere(
        const Point& pnt,
        const double& radius_int,
        const double& radius_ext,
        const Utils::Portion::Type& dt,
        std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newSphere should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::newBSpline(std::string n1,
		std::vector<Utils::Math::Point>& vp,
		std::string n2,
		int deg_min,
		int deg_max,
		std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::newBSpline should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::translate(
            std::vector<std::string>& ve, const Utils::Math::Vector& dp)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::translate should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::translateAll(
            const Utils::Math::Vector& dp)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::translateAll should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::copyAndTranslate(
            std::vector<std::string>& ve, const Utils::Math::Vector& dp, bool withTopo, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::copyAndTranslate should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::copyAndTranslateAll(
            const Utils::Math::Vector& dp, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::copyAndTranslateAll should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::joinCurves(
            std::vector<std::string>& ve)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::joinCurves should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::joinSurfaces(
            std::vector<std::string>& ve)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::joinSurfaces should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::rotate( std::vector<std::string>& entities,
		const Utils::Math::Rotation& rot)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::rotate should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::rotateAll( const Utils::Math::Rotation& rot)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::rotateAll should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::copyAndRotate( std::vector<std::string>& entities,
		const Utils::Math::Rotation& rot, bool withTopo, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::copyAndRotate should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::copyAndRotateAll( const Utils::Math::Rotation& rot, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::copyAndRotateAll should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::fuse(std::vector<std::string>& entities)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::fuse should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::common(std::vector<std::string>& entities)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::common should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::common2D(std::string entity1, std::string entity2, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::common2D should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::common2DOnCopy(std::string entity1, std::string entity2, std::string groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::common2DOnCopy should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::cut(std::string tokeep,std::vector<std::string>& tocut)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::cut should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::
cut(std::vector<std::string>& tokeep,std::vector<std::string>& tocut)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::cut should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::glue(std::vector<std::string>& entities)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::glue should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::section(std::vector<std::string>& entities,
        std::string tool)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::section should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::sectionByPlane(std::vector<std::string>& entities,
        Plane* tool, std::string planeGroupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::section should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::splitCurve(std::string entity, const Point& pt)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::splitCurve should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::importCATIA(std::string n, const bool testVolumicProperties, const bool splitCompoundCurves)
{
    throw TkUtil::Exception(TkUtil::UTF8String ("GeomManagerIfc::importCATIA should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::importSTEP(std::string n, const bool testVolumicProperties, const bool splitCompoundCurves)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::importSTEP should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::importSTL(std::string n)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::importSTL should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::importIGES(std::string n, const bool splitCompoundCurves)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::importIGES should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::importMDL(std::string n, const bool all, const bool useAreaName, std::string groupName, int deg_min, int deg_max)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::importMDL should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::importMDL(std::string n, std::string groupe)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::importMDL should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::importMDL(std::string n, std::vector<std::string>& zones)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::importMDL should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void GeomManagerIfc::mdl2CommandesMagix3D(std::string n, const bool withTopo)
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::mdl2CommandesMagix3D should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::exportMDL(std::vector<std::string>& ge,
            const std::string& n)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::exportMDL should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::exportVTK(const std::string& n)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::exportVTK should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::exportVTK(std::vector<std::string>& ge,
		const std::string& n)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::exportVTK should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::exportSTL(const std::string& ge,
    		const std::string& n)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::exportSTL should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::exportMLI(const std::string& n)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::exportMLI should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::exportMLI(std::vector<std::string>& ge,
		const std::string& n)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::exportMLI should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::exportSTEP(const std::string& n)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::exportSTEP should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::exportSTEP(std::vector<std::string>& ge,
		const std::string& n)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::exportSTEP should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::exportIGES(const std::string& n)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::exportIGES should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::exportIGES(std::vector<std::string>& ge,
		const std::string& n)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::exportIGES should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomManagerIfc::getVolumes() const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getVolumes should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomManagerIfc::getSurfaces() const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getSurfaces should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomManagerIfc::getCurves() const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getCurves should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GeomManagerIfc::getVertices() const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getVertices should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
int GeomManagerIfc::getNbVertices() const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getNbVertices should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
int GeomManagerIfc::getNbCurves() const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getNbCurves should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
int GeomManagerIfc::getNbSurfaces() const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getNbSurfaces should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
int GeomManagerIfc::getNbVolumes() const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getNbVolumes should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Volume* GeomManagerIfc::getVolume(
				const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getVolume should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Surface* GeomManagerIfc::getSurface(
				const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getSurface should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Curve* GeomManagerIfc::getCurve(
				const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getCurve should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Vertex* GeomManagerIfc::getVertex(
				const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getVertex should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
GeomEntity* GeomManagerIfc::getEntity(
				const std::string& name, const bool exceptionIfNotFound) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getEntity should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::string GeomManagerIfc::getVertexAt(const Point& pt1) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getVertexAt should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::string GeomManagerIfc::getCurveAt(const Point& pt1, const Point& pt2, const Point& pt3) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getCurveAt should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::string GeomManagerIfc::getSurfaceAt(std::vector<Point>& pts) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getSurfaceAt should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::string GeomManagerIfc::getVolumeAt(std::vector<Point>& pts) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getVolumeAt should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
int GeomManagerIfc::getIndexOf(Vertex* v) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getIndexOf should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
int GeomManagerIfc::getIndexOf(Curve* c) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getIndexOf should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
int GeomManagerIfc::getIndexOf(Surface* s) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getIndexOf should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
int GeomManagerIfc::getIndexOf(Volume* v) const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getIndexOf should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::addToGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::addToGroup should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::removeFromGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::removeFromGroup should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* GeomManagerIfc::setGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::setGroup should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::vector<Volume*> GeomManagerIfc::getVolumesObj() const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getVolumesObj should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::vector<Surface*> GeomManagerIfc::getSurfacesObj() const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getSurfacesObj should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::vector<Curve*> GeomManagerIfc::getCurvesObj() const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getCurvesObj should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::vector<Vertex*> GeomManagerIfc::getVerticesObj() const
{
	throw TkUtil::Exception (TkUtil::UTF8String ("GeomManagerIfc::getVerticesObj should be overloaded.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
