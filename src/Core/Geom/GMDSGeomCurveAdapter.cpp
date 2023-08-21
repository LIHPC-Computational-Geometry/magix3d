/*----------------------------------------------------------------------------*/
/*
 * GMDSGeomCurveAdapter.cpp
 *
 *  Created on: 18 juin 2013
 *      Author: legoff
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <set>
/*----------------------------------------------------------------------------*/
#include "Geom/GMDSGeomCurveAdapter.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/

GMDSGeomCurveAdapter::GMDSGeomCurveAdapter(Geom::Curve& ACurve)
:m_mgx3d_curve(ACurve)
{

}
/*----------------------------------------------------------------------------*/
GMDSGeomCurveAdapter::~GMDSGeomCurveAdapter()
{
    // TODO [FL] Auto-generated destructor stub
}
/*----------------------------------------------------------------------------*/
void GMDSGeomCurveAdapter::
get(std::vector<gmds::geom::GeomPoint*>& APnt) const
{
    APnt.clear();
    APnt.resize(m_points.size(),NULL);

    for(unsigned int i=0; i<m_points.size(); i++) {
        APnt[i] = m_points[i];
    }
}
/*----------------------------------------------------------------------------*/
void GMDSGeomCurveAdapter::
get(std::vector<gmds::geom::GeomSurface*>& ASurf) const
{
    ASurf.clear();
    ASurf.resize(m_surfaces.size(),NULL);

    for(unsigned int i=0; i<m_surfaces.size(); i++) {
        ASurf[i] = m_surfaces[i];
    }
}
/*----------------------------------------------------------------------------*/
void GMDSGeomCurveAdapter::
get(std::vector<gmds::geom::GeomVolume*>& AVol) const
{
    std::set<gmds::geom::GeomVolume* > vol_set;
    for(unsigned int i=0;i<m_surfaces.size();i++)
    {
        std::vector<gmds::geom::GeomVolume* > vol_i;
        m_surfaces[i]->get(vol_i);
        vol_set.insert(vol_i.begin(),vol_i.end());
    }
    AVol.clear();
    AVol.insert(AVol.begin(),vol_set.begin(),vol_set.end());
}
/*----------------------------------------------------------------------------*/
double GMDSGeomCurveAdapter::
length() const{
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomCurveAdapter::length pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
gmds::TCoord GMDSGeomCurveAdapter::
computeDihedralAngle() const{
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomCurveAdapter::computeDihedralAngle pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void GMDSGeomCurveAdapter::
project(gmds::math::Point& AP) const{
    Utils::Math::Point pt(AP.X(), AP.Y(), AP.Z());
    m_mgx3d_curve.project(pt);
    AP.setXYZ(pt.getX(),pt.getY(),pt.getZ());
}
///*----------------------------------------------------------------------------*/
//void GMDSGeomSurfaceAdapter::computeNormal(
//        const GEPETO::Point<3,double>& AP,
//        GEPETO::Vector<3,double>& AV) const{
//
//}
///*----------------------------------------------------------------------------*/
//GEPETO::Point<3,double> GMDSGeomSurfaceAdapter::
//closestPoint(const GEPETO::Point<3,double>& AP) const{
//    GEPETO::Point<3,double> p;
//
//    return p;
//}
/*----------------------------------------------------------------------------*/
gmds::TCoord GMDSGeomCurveAdapter::
computeArea() const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomCurveAdapter::computeArea pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void GMDSGeomCurveAdapter::
computeBoundingBox(gmds::TCoord minXYZ[3], gmds::TCoord maxXYZ[3]) const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomCurveAdapter::computeBoundingBox pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
gmds::TCoord GMDSGeomCurveAdapter::
computeDistanceHaussdorf(
        const gmds::math::Segment& ASegment) const
{
    double firstCurviline;
    double lastCurviline;
    m_mgx3d_curve.getParameters(firstCurviline,lastCurviline);

    Utils::Math::Point pt0;
    Utils::Math::Point pt1;
    m_mgx3d_curve.getPoint(firstCurviline,pt0);
    m_mgx3d_curve.getPoint(lastCurviline,pt1);

    // TODO : beware this hard-coded constant
    const uint nbPoints = 11;
    double l_ratios[nbPoints];
    std::vector<Utils::Math::Point> points;

    for(uint iPoint=0; iPoint<nbPoints; iPoint++) {
        l_ratios[iPoint] = iPoint*(1./(nbPoints-1));
    }
    l_ratios[0] = 0.;
    l_ratios[nbPoints-1] = 1.;

    points.push_back(pt0);
    m_mgx3d_curve.getParametricsPoints(pt0,pt1,nbPoints-2,&l_ratios[1],points);
    points.push_back(pt1);


    unsigned int nbSegments = nbPoints - 1;
    gmds::TCoord  dist = HUGE_VALF;

    for(unsigned int iSegment=0; iSegment<nbSegments; iSegment++) {

        gmds::math::Point p0(points[iSegment].getX(),points[iSegment].getY(),points[iSegment].getZ());
        gmds::math::Point p1(points[iSegment+1].getX(),points[iSegment+1].getY(),points[iSegment+1].getZ());

        gmds::math::Segment seg(p0,p1);

        gmds::TCoord  dist_tmp = seg.distanceInf(ASegment);
        if(dist_tmp<dist)
            dist = dist_tmp;
    }

    return dist;

//    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomCurveAdapter::computeDistanceHaussdorf pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
gmds::TCoord GMDSGeomCurveAdapter::
computeDistanceHaussdorfSubCurve(
        const int& ANbPoints,
        const int& AIPoint,
        const gmds::math::Segment& ASegment) const
{
//    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomCurveAdapter::computeDistanceHaussdorfSubCurve pas disponible.", TkUtil::Charset::UTF_8));

    // first compute length of the curve
    gmds::TCoord length = 0.;

    double firstCurviline;
    double lastCurviline;
    m_mgx3d_curve.getParameters(firstCurviline,lastCurviline);

    Utils::Math::Point pt0;
    Utils::Math::Point pt1;
    m_mgx3d_curve.getPoint(firstCurviline,pt0);
//    m_mgx3d_curve.getPoint(lastCurviline,pt1);
    m_mgx3d_curve.getPoint(lastCurviline-(lastCurviline-firstCurviline)*10e-10,pt1);

    double first_tmp;
    double last_tmp;
    m_mgx3d_curve.getParameter(pt0,first_tmp);
    m_mgx3d_curve.getParameter(pt1,last_tmp);

    // TODO : beware this hard-coded constant
    const uint nbPoints = 101;
    double l_ratios[nbPoints];
    Utils::Math::Point l_points[nbPoints];

//    for(uint iPoint=0; iPoint<nbPoints; iPoint++) {
//        l_ratios[iPoint] = firstCurviline + iPoint*(1./(nbPoints-1)) * (lastCurviline-firstCurviline);
//    }
//    l_ratios[0] = firstCurviline;
//    l_ratios[nbPoints-1] = lastCurviline;
    for(uint iPoint=0; iPoint<nbPoints; iPoint++) {
        l_ratios[iPoint] = iPoint*(1./(nbPoints-1));
    }
    l_ratios[0] = 0.;
    l_ratios[nbPoints-1] = 1.;

//    m_mgx3d_curve.getParametricsPoints(pt0,pt1,nbPoints,l_ratios,l_points);
    m_mgx3d_curve.getParametricsPointsLoops(firstCurviline,lastCurviline,nbPoints,l_ratios,l_points);
//    l_points[0] = pt0;
//    l_points[nbPoints-1] = pt1;

    unsigned int nbSegments = nbPoints - 1;
    std::vector<gmds::math::Segment > geom_representation;
    geom_representation.resize(nbSegments);

    for(unsigned int iSegment=0; iSegment<nbSegments; iSegment++) {

        gmds::math::Point p0(l_points[iSegment].getX(),l_points[iSegment].getY(),l_points[iSegment].getZ());
        gmds::math::Point p1(l_points[iSegment+1].getX(),l_points[iSegment+1].getY(),l_points[iSegment+1].getZ());

        gmds::math::Segment seg(p0,p1);
        geom_representation[iSegment] = seg;
    }

    for(unsigned int iSegment=0; iSegment<geom_representation.size(); iSegment++) {
        length += geom_representation[iSegment].computeLength();
    }

    gmds::TCoord step_length = length / ANbPoints;

    // create all points
    std::vector<gmds::math::Point > points;
    points.resize(ANbPoints);
    points[0] = gmds::math::Point (l_points[0].getX(),l_points[0].getY(),l_points[0].getZ());

    unsigned int iPoint = 1;
    unsigned int iSegment = 0;
    bool isNewInSegment = true;
    gmds::math::Point current_point = points[0];

    std::vector<gmds::math::Point > points_tmp;

    length = 0.;

    while(iPoint<ANbPoints+1) {

        if((iPoint == AIPoint+1) && (points_tmp.size() == 0)) {
            points_tmp.push_back(current_point);
        }
        if((iPoint == AIPoint+1) && (AIPoint == ANbPoints-1)) {
            for(unsigned int iSegment_tmp=iSegment; iSegment_tmp<geom_representation.size(); iSegment_tmp++) {
                gmds::math::Point point = geom_representation[iSegment_tmp].getPoint(1);
                points_tmp.push_back(point);
            }
            break;
        }


        if((length < step_length*iPoint)
        && (length + current_point.distance(geom_representation[iSegment].getPoint(1)) >= step_length*iPoint)) {

            gmds::math::Point point;
            if(isNewInSegment) {
                point  = geom_representation[iSegment].getPoint(0);
            } else {
                point = points[iPoint-1];
            }

            gmds::math::Vector vect(geom_representation[iSegment].getPoint(0),geom_representation[iSegment].getPoint(1));
            vect.normalize();
            point = point + vect*(step_length*iPoint - length);

            points[iPoint] = point;
            current_point = points[iPoint];
            length = step_length*iPoint;
            iPoint++;
            isNewInSegment = false;

            if(iPoint == AIPoint+2) {
                points_tmp.push_back(point);
                break;
            }

        } else {
            if(isNewInSegment) {
                length += geom_representation[iSegment].computeLength();
            } else {
                length += points[iPoint-1].distance(geom_representation[iSegment].getPoint(1));
            }
            current_point = geom_representation[iSegment].getPoint(1);
            isNewInSegment = true;
            iSegment++;

            points_tmp.push_back(current_point);
        }

    } // while(iPoint<ANbPoints) {

    // compute haussdorf distance
    gmds::TCoord dist = HUGE_VALF;

    for(unsigned int iSegment=0; iSegment<points_tmp.size()-1; iSegment++) {

        gmds::math::Segment segment(points_tmp[iSegment],points_tmp[iSegment+1]);

        gmds::TCoord dist_tmp = segment.distanceInf(ASegment);
        if(dist_tmp<dist)
            dist = dist_tmp;
    }

    return dist;
}
/*----------------------------------------------------------------------------*/
void GMDSGeomCurveAdapter::
computeVector(
        const gmds::geom::GeomPoint& AP,
        gmds::math::Vector& AV) const
{
    gmds::math::Point point1 = AP.getPoint();
//    gmds::math::Point pointlast = getSecondPoint()->getPoint();

    gmds::geom::GeomPoint* firstPoint = getFirstPoint();
////    if(fabs(AP.distance(firstPoint)) < 10.e-13) {
////
////    }

    Utils::Math::Point pt1(AP.X(),AP.Y(), AP.Z());
    double alpha1;
    m_mgx3d_curve.getParameter(pt1,alpha1);

    double firstCurviline;
    double lastCurviline;
    m_mgx3d_curve.getParameters(firstCurviline,lastCurviline);

    // TODO : beware this hard-coded constant
    double alpha2;

    if(fabs((alpha1-firstCurviline)/(lastCurviline-firstCurviline)) < 10.e-13) {
        alpha2 = alpha1 + (lastCurviline-firstCurviline)*0.001;
    } else {
        alpha2 = alpha1 - (lastCurviline-firstCurviline)*0.001;
    }
//    std::cout<<alpha1<<" "<<alpha2<<" "<<firstCurviline<<" "<<lastCurviline<<std::endl;

//    std::cout<<"computeVector "<<alpha1<<std::endl;
//    std::cout<<"point1 "<<point1<<std::endl;
////    std::cout<<"pointlast "<<pointlast<<std::endl;
//    std::cout<<"firstPoint "<<firstPoint->getPoint()<<std::endl;
//
//
//    std::cout<<"firstCurviligne "<<firstCurviligne<<"lastCurviligne "<<lastCurviligne<<std::endl;

    Utils::Math::Point pt2;
    m_mgx3d_curve.getPoint(alpha2,pt2);

    gmds::math::Point point2(pt2.getX(),pt2.getY(),pt2.getZ());

    AV = gmds::math::Vector (point1,point2);
    AV.normalize();
}
/*----------------------------------------------------------------------------*/
void GMDSGeomCurveAdapter::
getMultiplePoints(
        const int& ANbPoints,
        gmds::math::Point* APoints) const
{
    double firstCurviline;
    double lastCurviline;
    m_mgx3d_curve.getParameters(firstCurviline,lastCurviline);


    for(unsigned int iPoint=0; iPoint<ANbPoints; iPoint++) {
        Utils::Math::Point pt;
        double alpha = firstCurviline + iPoint*(1./(ANbPoints)) * (lastCurviline-firstCurviline);
        m_mgx3d_curve.getPoint(alpha,pt);

        APoints[iPoint] = gmds::math::Point (pt.getX(),pt.getY(),pt.getZ());
    }
}
/*----------------------------------------------------------------------------*/
gmds::geom::GeomPoint* GMDSGeomCurveAdapter::
getFirstPoint() const
{
    if(getNbPoints() < 1) {
        throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomCurveAdapter::getFirstPoint appelé sur une courbe sans sommet.", TkUtil::Charset::UTF_8));
    }

    std::vector<gmds::geom::GeomPoint*> points;
    get(points);

    return points[0];
}
/*----------------------------------------------------------------------------*/
gmds::geom::GeomPoint* GMDSGeomCurveAdapter::
getSecondPoint() const
{
    if(getNbPoints() < 2) {
        throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomCurveAdapter::getSecondPoint appelé sur une courbe sans "
                "au moins deux sommets.", TkUtil::Charset::UTF_8));
    }

    std::vector<gmds::geom::GeomPoint*> points;
    get(points);

    return points[1];
}
/*----------------------------------------------------------------------------*/
bool GMDSGeomCurveAdapter::
isALoop() const
{
    std::vector<gmds::geom::GeomPoint*> points;
    this->get(points);

    if(1 == points.size()) {
        return true;
    } else {
        return false;
    }
}
/*----------------------------------------------------------------------------*/
gmds::geom::GeomSurface* GMDSGeomCurveAdapter::
getLeftSurface() const
{
	const double epsilonYEAH = 10e-10;
//	throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomCurveAdapter::getLeftSurface no more implemented.", TkUtil::Charset::UTF_8));
    gmds::geom::GeomPoint* firstPoint = getFirstPoint();
    gmds::math::Vector curveVector;
    computeVector(*firstPoint,curveVector);
    curveVector.normalize();

    std::vector<gmds::geom::GeomSurface* > surfaces;
    get(surfaces);

    gmds::geom::GeomSurface* surface1 = surfaces[0];
    gmds::geom::GeomSurface* surface2 = surfaces[1];

    std::vector<gmds::math::Triangle> triangles;
    surface1->getTriangulation(triangles);

    gmds::math::Point point = firstPoint->getPoint();
    gmds::TCoord distMin = HUGE_VALF;
    unsigned int triangleIndex;
    bool triangleFound = false;

    for(unsigned int iTriangle=0; iTriangle<triangles.size(); iTriangle++) {
        gmds::math::Point point0 = triangles[iTriangle].getPoint(0);
        gmds::math::Point point1 = triangles[iTriangle].getPoint(1);
        gmds::math::Point point2 = triangles[iTriangle].getPoint(2);

        gmds::math::Point projectedPoint0 = point0;
        gmds::math::Point projectedPoint1 = point1;
        gmds::math::Point projectedPoint2 = point2;
        this->project(projectedPoint0);
        this->project(projectedPoint1);
        this->project(projectedPoint2);

        gmds::TCoord dist0 = point0.distance2(projectedPoint0);
        gmds::TCoord dist1 = point1.distance2(projectedPoint1);
        gmds::TCoord dist2 = point2.distance2(projectedPoint2);

        // does this triangle contain firstpoint?
        gmds::TCoord firstDist0 = point.distance2(point0);
        gmds::TCoord firstDist1 = point.distance2(point1);
        gmds::TCoord firstDist2 = point.distance2(point2);

        if((firstDist0>epsilonYEAH) && (firstDist1>epsilonYEAH) && (firstDist2>epsilonYEAH)) {
            continue;
        }

        // does this triangle have another point on the curve.
        // If yes, check the dot product to ensure that this second point is
        // indeed on the curve, and not on the other side
        if(dist0<epsilonYEAH && firstDist0>epsilonYEAH) {
            gmds::math::Vector v(point,point0);

            if(curveVector.dot(v)>=0.) {
                triangleIndex = iTriangle;
                triangleFound = true;
                break;
            }
        }
        if(dist1<epsilonYEAH && firstDist1>epsilonYEAH) {
            gmds::math::Vector v(point,point1);

            if(curveVector.dot(v)>=0.) {
                triangleIndex = iTriangle;
                triangleFound = true;
                break;
            }
        }
        if(dist2<epsilonYEAH && firstDist2>epsilonYEAH) {
            gmds::math::Vector v(point,point2);

            if(curveVector.dot(v)>=0.) {
                triangleIndex = iTriangle;
                triangleFound = true;
                break;
            }
        }

    } // for(unsigned int iTriangle=0; iTriangle<triangles.size(); iTriangle++)

    if(!triangleFound) {
        throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomCurveAdapter::getLeftSurface triangle in the surface not found.", TkUtil::Charset::UTF_8));
    }

    gmds::math::Point trianglePoint = triangles[triangleIndex].getCenter();
    gmds::math::Vector triangleNormal = triangles[triangleIndex].getNormal();
    triangleNormal.normalize();

//    std::cout<<"GMDSGeomCurveAdapter::getLeftSurface "<<triangleIndex<<" "<<distMin<<std::endl;
//    std::cout<<triangles[triangleIndex].getPoint(0)<<std::endl;
//    std::cout<<triangles[triangleIndex].getPoint(1)<<std::endl;
//    std::cout<<triangles[triangleIndex].getPoint(2)<<std::endl;
//    std::cout<<trianglePoint<<std::endl;

    gmds::math::Vector uVector(triangleNormal - (triangleNormal.dot(curveVector))*curveVector);

    gmds::math::Vector ppVector(point,trianglePoint);

//    std::cout<<"getLeftSurface "<<std::endl;
//    std::cout<<point<<std::endl;
//    std::cout<<curveVector<<std::endl;
//    std::cout<<trianglePoint<<std::endl;
//    std::cout<<triangleNormal<<std::endl;
//    std::cout<<uVector<<std::endl;
//    std::cout<<ppVector<<std::endl;
//    std::cout<<(curveVector.cross(uVector)).dot(ppVector)<<" "<<((curveVector.cross(uVector)).dot(ppVector) >= 0.)<<std::endl;

    if((curveVector.cross(uVector)).dot(ppVector) >= 0.) {
        return surface2;
    } else {
        return surface1;
    }
}
/*----------------------------------------------------------------------------*/
gmds::geom::GeomSurface* GMDSGeomCurveAdapter::
getRightSurface() const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomCurveAdapter::getRightSurface no more implemented.", TkUtil::Charset::UTF_8));
//    gmds::geom::GeomPoint* firstPoint = getFirstPoint();
//    gmds::math::Vector curveVector;
//    computeVector(*firstPoint,curveVector);
//    curveVector.normalize();
//
//    std::vector<gmds::geom::GeomSurface* > surfaces;
//    get(surfaces);
//
//    gmds::geom::GeomSurface* surface1 = surfaces[0];
//    gmds::geom::GeomSurface* surface2 = surfaces[1];
//
//    std::vector<GEPETO::Triangle<3,gmds::TCoord > > triangles;
//    surface1->getTriangulation(triangles);
//
//    gmds::math::Point point = firstPoint->getPoint();
//    gmds::TCoord distMin = HUGE_VALF;
//    unsigned int triangleIndex;
//
//    for(unsigned int iTriangle=0; iTriangle<triangles.size(); iTriangle++) {
//        gmds::math::Point point0 = triangles[iTriangle].getPoint(0);
//        gmds::math::Point point1 = triangles[iTriangle].getPoint(1);
//        gmds::math::Point point2 = triangles[iTriangle].getPoint(2);
//
//        gmds::TCoord dist = point.distance(point0);
//        if(dist < distMin) {
//            distMin = dist;
//            triangleIndex = iTriangle;
//        }
//        dist = point.distance(point1);
//        if(dist < distMin) {
//            distMin = dist;
//            triangleIndex = iTriangle;
//        }
//        dist = point.distance(point2);
//        if(dist < distMin) {
//            distMin = dist;
//            triangleIndex = iTriangle;
//        }
//
//        if(distMin.isNull()) {
//            break;
//        }
//    }
//
//    gmds::math::Point trianglePoint = triangles[triangleIndex].getCenter();
//    gmds::math::Vector triangleNormal = triangles[triangleIndex].getNormal();
//    triangleNormal.normalize();
//
//    gmds::math::Vector uVector(triangleNormal - (triangleNormal.dot(curveVector))*curveVector);
//
//    gmds::math::Vector ppVector(point,trianglePoint);
//
//    if((curveVector.cross(uVector)).dot(ppVector) >= 0.) {
//        return surface1;
//    } else {
//        return surface2;
//    }
}
/*----------------------------------------------------------------------------*/
gmds::TInt GMDSGeomCurveAdapter::
getNbPoints() const
{
    std::vector<gmds::geom::GeomPoint*> points;
    get(points);
    return points.size();
}
/*----------------------------------------------------------------------------*/
void GMDSGeomCurveAdapter::
add(gmds::geom::GeomPoint* APoint)
{
    m_points.push_back(APoint);
}
/*----------------------------------------------------------------------------*/
void GMDSGeomCurveAdapter::
add(gmds::geom::GeomSurface* ASurf)
{
    m_surfaces.push_back(ASurf);
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
