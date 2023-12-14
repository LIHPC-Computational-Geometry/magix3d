/*----------------------------------------------------------------------------*/
/*
 * GMDSGeomPointAdapter.cpp
 *
 *  Created on: 18 juin 2013
 *      Author: legoff
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <set>
/*----------------------------------------------------------------------------*/
#include "Geom/GMDSGeomPointAdapter.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/

GMDSGeomPointAdapter::GMDSGeomPointAdapter(Geom::Vertex& AVertex)
:m_mgx3d_vertex(AVertex)
{

}
/*----------------------------------------------------------------------------*/
GMDSGeomPointAdapter::~GMDSGeomPointAdapter()
{
    // TODO [FL] Auto-generated destructor stub
}
/*----------------------------------------------------------------------------*/
void GMDSGeomPointAdapter::
get(std::vector<gmds::cad::GeomCurve*>& ACur) const
{
    ACur.clear();
    ACur.resize(m_curves.size());

    for(unsigned int i=0; i<m_curves.size(); i++) {
        ACur[i] = m_curves[i];
    }
}
/*----------------------------------------------------------------------------*/
void GMDSGeomPointAdapter::
get(std::vector<gmds::cad::GeomSurface*>& ASurf) const
{
    std::set<gmds::cad::GeomSurface* > surf_set;
    for(unsigned int i=0;i<m_curves.size();i++)
    {
        std::vector<gmds::cad::GeomSurface*> surf_i(m_curves[i]->surfaces());
        surf_set.insert(surf_i.begin(),surf_i.end());
    }
    ASurf.clear();
    ASurf.insert(ASurf.begin(),surf_set.begin(),surf_set.end());
}
/*----------------------------------------------------------------------------*/
void GMDSGeomPointAdapter::
get(std::vector<gmds::cad::GeomVolume*>& AVol) const
{
    std::set<gmds::cad::GeomVolume* > vol_set;
    for(unsigned int i=0;i<m_curves.size();i++)
    {
        std::vector<gmds::cad::GeomVolume* > vol_i(m_curves[i]->volumes());
        vol_set.insert(vol_i.begin(),vol_i.end());
    }
    AVol.clear();
    AVol.insert(AVol.begin(),vol_set.begin(),vol_set.end());
}
/*------------------------------------------------------------------------*/
//void GMDSGeomPointAdapter::
//getOrdered(std::vector<gmds::cad::GeomCurve*>& ACur) const
//{
//
//}
/*----------------------------------------------------------------------------*/
//void GMDSGeomPointAdapter::
//getOrderedDirect(std::vector<gmds::cad::GeomCurve*>& ACur) const
//{
//
//
//    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomPointAdapter::getOrderedDirect pas disponible.", TkUtil::Charset::UTF_8);
//}
/*----------------------------------------------------------------------------*/
gmds::TInt GMDSGeomPointAdapter::
getNbCurves() const
{
    return m_mgx3d_vertex.getNbCurves();
}
/*----------------------------------------------------------------------------*/
gmds::TCoord GMDSGeomPointAdapter::X() const
{
    return m_mgx3d_vertex.getX();
}
/*----------------------------------------------------------------------------*/
gmds::TCoord GMDSGeomPointAdapter::Y() const
{
    return m_mgx3d_vertex.getY();
}
/*----------------------------------------------------------------------------*/
gmds::TCoord GMDSGeomPointAdapter::Z() const
{
    return m_mgx3d_vertex.getZ();
}
/*----------------------------------------------------------------------------*/
void GMDSGeomPointAdapter::
getXYZ(gmds::TCoord ACoordinates[3]) const
{
    ACoordinates[0] = X();
    ACoordinates[1] = Y();
    ACoordinates[2] = Z();
}
/*----------------------------------------------------------------------------*/
gmds::math::Point GMDSGeomPointAdapter::
getPoint() const
{
    gmds::TCoord coordinates[3];
    getXYZ(coordinates);
    return gmds::math::Point(coordinates[0], coordinates[1], coordinates[2]);
}
/*----------------------------------------------------------------------------*/
void GMDSGeomPointAdapter::
add(gmds::cad::GeomCurve* ACurve)
{
    m_curves.push_back(ACurve);
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
