/*----------------------------------------------------------------------------*/
/*
 * GMDSGeomVolumeAdapter.cpp
 *
 *  Created on: 18 juin 2013
 *      Author: legoff
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <set>
/*----------------------------------------------------------------------------*/
#include "Geom/GMDSGeomVolumeAdapter.h"
/*----------------------------------------------------------------------------*/
#include "Geom/GMDSGeomPointAdapter.h"
#include "Geom/GMDSGeomCurveAdapter.h"
#include "Geom/GMDSGeomSurfaceAdapter.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/

GMDSGeomVolumeAdapter::GMDSGeomVolumeAdapter(Geom::Volume& AVol)
:m_mgx3d_volume(AVol)
{

}
/*----------------------------------------------------------------------------*/
GMDSGeomVolumeAdapter::~GMDSGeomVolumeAdapter()
{
    // TODO [FL] Auto-generated destructor stub
}
/*----------------------------------------------------------------------------*/
void GMDSGeomVolumeAdapter::
get(std::vector<gmds::geom::GeomPoint*>& APnt) const
{
    std::set<gmds::geom::GeomPoint* > points_set;
    for(unsigned int i=0;i<m_surfaces.size();i++)
    {
        std::vector<gmds::geom::GeomPoint* > p_i;
        m_surfaces[i]->get(p_i);
        points_set.insert(p_i.begin(),p_i.end());
    }
    APnt.clear();
    APnt.insert(APnt.begin(),points_set.begin(),points_set.end());
}
/*----------------------------------------------------------------------------*/
void GMDSGeomVolumeAdapter::
get(std::vector<gmds::geom::GeomCurve*>& ACur) const
{
    std::set<gmds::geom::GeomCurve* > curves_set;
    for(unsigned int i=0;i<m_surfaces.size();i++)
    {
        std::vector<gmds::geom::GeomCurve* > c_i;
        m_surfaces[i]->get(c_i);
        curves_set.insert(c_i.begin(),c_i.end());
    }
    ACur.clear();
    ACur.insert(ACur.begin(),curves_set.begin(),curves_set.end());
}
/*----------------------------------------------------------------------------*/
void GMDSGeomVolumeAdapter::
get(std::vector<gmds::geom::GeomSurface*>& ASurf) const
{
    ASurf.clear();

    ASurf.resize(m_surfaces.size());
    for(unsigned int i=0; i<m_surfaces.size(); i++) {
        ASurf[i] = m_surfaces[i];
    }
}
/*----------------------------------------------------------------------------*/
void GMDSGeomVolumeAdapter::
computeBoundingBox(gmds::TCoord minXYZ[3], gmds::TCoord maxXYZ[3]) const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomSurfaceAdapter::computeBoundingBox pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
gmds::TCoord GMDSGeomVolumeAdapter::
computeArea() const
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomVolumeAdapter::computeArea pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void GMDSGeomVolumeAdapter::
add(gmds::geom::GeomSurface* ASurf)
{
    m_surfaces.push_back(ASurf);
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
