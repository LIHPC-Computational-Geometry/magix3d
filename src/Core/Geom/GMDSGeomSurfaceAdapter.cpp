/*----------------------------------------------------------------------------*/
/*
 * GMDSGeomSurfaceAdapter.cpp
 *
 *  Created on: 9 nov. 2011
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <set>
/*----------------------------------------------------------------------------*/
#include "Geom/GMDSGeomSurfaceAdapter.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/

GMDSGeomSurfaceAdapter::GMDSGeomSurfaceAdapter(Geom::Surface& ASurf)
:m_mgx3d_surface(ASurf)
{
    m_mgx3d_surface.getFacetedRepresentation(m_triangulation);

    std::vector<gmds::math::Triangle>::iterator itt = m_triangulation.begin();
    for(; itt!=m_triangulation.end(); ) {
        gmds::math::Point pt0 = itt->getPoint(0);
        gmds::math::Point pt1 = itt->getPoint(1);
        gmds::math::Point pt2 = itt->getPoint(2);

        if(pt0.areColinear(pt1,pt2)) {
            std::cout<<"GMDSGeomSurfaceAdapter::GMDSGeomSurfaceAdapter "
                    "a triangle is flat"<<std::endl;
            std::cout<<pt0<<std::endl;
            std::cout<<pt1<<std::endl;
            std::cout<<pt2<<std::endl;

            itt = m_triangulation.erase(itt);
        } else {
            itt++;
        }
    }

//    this->isBoundingBoxStored_ = false;
}
/*----------------------------------------------------------------------------*/
GMDSGeomSurfaceAdapter::~GMDSGeomSurfaceAdapter()
{
    // TODO [FL] Auto-generated destructor stub
}
/*----------------------------------------------------------------------------*/
void GMDSGeomSurfaceAdapter::
get(std::vector<gmds::geom::GeomPoint*>& APnt) const
{
    std::set<gmds::geom::GeomPoint* > points_set;
    for(unsigned int i=0;i<m_curves.size();i++)
    {
        std::vector<gmds::geom::GeomPoint* > p_i;
        m_curves[i]->get(p_i);
        points_set.insert(p_i.begin(),p_i.end());
    }
    APnt.clear();
    APnt.insert(APnt.begin(),points_set.begin(),points_set.end());
}
/*----------------------------------------------------------------------------*/
void GMDSGeomSurfaceAdapter::
get(std::vector<gmds::geom::GeomCurve*>& ACur) const
{
    ACur.clear();

    ACur.resize(m_curves.size());
    for(unsigned int i=0; i<m_curves.size(); i++)
        ACur[i] = m_curves[i];
}
/*----------------------------------------------------------------------------*/
void GMDSGeomSurfaceAdapter::
get(std::vector<gmds::geom::GeomVolume*>& AVol) const
{
    AVol.clear();

    AVol.resize(m_volumes.size());
    for(unsigned int i=0; i<m_volumes.size(); i++)
        AVol[i] = m_volumes[i];
}
/*----------------------------------------------------------------------------*/
//void GMDSGeomSurfaceAdapter::
//project(GEPETO::Point<3,GEPETO::Numeric<double> >& AP) const
//{
//    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomSurfaceAdapter::project pas disponible.", TkUtil::Charset::UTF_8);
//}
/*----------------------------------------------------------------------------*/
void GMDSGeomSurfaceAdapter::
computeNormal( const gmds::math::Point& AP, gmds::math::Vector& AV) const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomSurfaceAdapter::computeNormal pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
gmds::math::Point GMDSGeomSurfaceAdapter::
closestPoint(const gmds::math::Point& AP) const
{
    Utils::Math::Point pt1(AP.X(),AP.Y(),AP.Z());
    Utils::Math::Point pt2;
    m_mgx3d_surface.project(pt1,pt2);

    return gmds::math::Point(pt2.getX(),pt2.getY(),pt2.getZ());
}
/*----------------------------------------------------------------------------*/
gmds::TCoord GMDSGeomSurfaceAdapter::
computeArea() const
{
    return m_mgx3d_surface.getArea();
}
/*----------------------------------------------------------------------------*/
void GMDSGeomSurfaceAdapter::
computeBoundingBox(gmds::TCoord minXYZ[3],gmds::TCoord maxXYZ[3]) const
{
    double bounds[6];
    m_mgx3d_surface.getBounds(bounds);

    minXYZ[0] = bounds[0];
    minXYZ[1] = bounds[2];
    minXYZ[2] = bounds[4];
    maxXYZ[0] = bounds[1];
    maxXYZ[1] = bounds[3];
    maxXYZ[2] = bounds[5];
}
/*----------------------------------------------------------------------------*/
void GMDSGeomSurfaceAdapter::
getTriangulation(std::vector<gmds::math::Triangle>& ATri) const
{
    if(m_triangulation.empty()) {
        throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomSurfaceAdapter::getTriangulation la "
                "triangulation est vide, c'est potentiellement un problème.", TkUtil::Charset::UTF_8));
    }
    ATri = m_triangulation;
}
///*----------------------------------------------------------------------------*/
//std::vector<gmds::math::Triangle>* GMDSGeomSurfaceAdapter::
//getTriangulation()
//{
//    if(m_triangulation.empty()) {
//        throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomSurfaceAdapter::getTriangulation la "
//                "triangulation est vide, c'est potentiellement un problème.", TkUtil::Charset::UTF_8);
//    }
//
//    return &m_triangulation;
//}
/*----------------------------------------------------------------------------*/
void GMDSGeomSurfaceAdapter::
reorientTriangulation()
{
//    if(m_triangulation.empty()) {
//        throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomSurfaceAdapter::getTriangulation la "
//                "triangulation est vide, c'est potentiellement un problème.", TkUtil::Charset::UTF_8);
//    }
//
//    for(unsigned int iTri=0; iTri<m_triangulation.size(); iTri++) {
//    	GEPETO::Point<3,GEPETO::Numeric<double> > p0 = m_triangulation[iTri].getPoint(1);
//    	GEPETO::Point<3,GEPETO::Numeric<double> > p1 = m_triangulation[iTri].getPoint(0);
//    	GEPETO::Point<3,GEPETO::Numeric<double> > p2 = m_triangulation[iTri].getPoint(2);
//
//    	m_triangulation[iTri].set(p0,p1,p2);
//    }
}
/*----------------------------------------------------------------------------*/
void GMDSGeomSurfaceAdapter::
add(gmds::geom::GeomCurve* ACurve)
{
    m_curves.push_back(ACurve);
}
/*----------------------------------------------------------------------------*/
void GMDSGeomSurfaceAdapter::
add(gmds::geom::GeomVolume* AVol)
{
    m_volumes.push_back(AVol);
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
