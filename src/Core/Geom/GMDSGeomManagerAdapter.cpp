/*----------------------------------------------------------------------------*/
/** \file GMDSGeomManagerAdapter.cpp
 *
 *  \author legoff
 *
 *  \date 28/06/2013
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/GMDSGeomManagerAdapter.h"
#include <GMDS/Math/Triangle.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GMDSGeomManagerAdapter::GMDSGeomManagerAdapter(Mgx3D::Geom::GeomManager& AMgx3DGeomManager)
:m_mgx3DGeomManager(AMgx3DGeomManager)
{
    std::vector<Mgx3D::Geom::Vertex*> verticesMgx;
    std::vector<Mgx3D::Geom::Curve*> curvesMgx;
    std::vector<Mgx3D::Geom::Surface*> surfacesMgx;
    std::vector<Mgx3D::Geom::Volume*> volumesMgx;

    verticesMgx = m_mgx3DGeomManager.getVerticesObj();
    curvesMgx = m_mgx3DGeomManager.getCurvesObj();
    surfacesMgx = m_mgx3DGeomManager.getSurfacesObj();
    volumesMgx = m_mgx3DGeomManager.getVolumesObj();

    for(unsigned int iPoint=0; iPoint<verticesMgx.size(); iPoint++) {
        Mgx3D::Geom::GMDSGeomPointAdapter* newPoint = new Mgx3D::Geom::GMDSGeomPointAdapter(*(verticesMgx[iPoint]));
        m_points.push_back(newPoint);
        m_mgx2gmdsPoints[verticesMgx[iPoint]] = newPoint;
        m_gmds2mgxPoints[newPoint] = verticesMgx[iPoint];
    }
    for(unsigned int iCurve=0; iCurve<curvesMgx.size(); iCurve++) {
        Mgx3D::Geom::GMDSGeomCurveAdapter* newCurve = new Mgx3D::Geom::GMDSGeomCurveAdapter(*(curvesMgx[iCurve]));
        m_curves.push_back(newCurve);
        m_mgx2gmdsCurves[curvesMgx[iCurve]] = newCurve;
        m_gmds2mgxCurves[newCurve] = curvesMgx[iCurve];
    }
    for(unsigned int iSurf=0; iSurf<surfacesMgx.size(); iSurf++) {
        Mgx3D::Geom::GMDSGeomSurfaceAdapter* newSurf = new Mgx3D::Geom::GMDSGeomSurfaceAdapter(*(surfacesMgx[iSurf]));
        m_surfaces.push_back(newSurf);
        m_mgx2gmdsSurfaces[surfacesMgx[iSurf]] = newSurf;
        m_gmds2mgxSurfaces[newSurf] = surfacesMgx[iSurf];
    }
    for(unsigned int iVol=0; iVol<volumesMgx.size(); iVol++) {
        Mgx3D::Geom::GMDSGeomVolumeAdapter* newVol = new Mgx3D::Geom::GMDSGeomVolumeAdapter(*(volumesMgx[iVol]));
        m_volumes.push_back(newVol);
        m_mgx2gmdsVolumes[volumesMgx[iVol]] = newVol;
        m_gmds2mgxVolumes[newVol] = volumesMgx[iVol];
    }

    for(unsigned int iPoint=0; iPoint<verticesMgx.size(); iPoint++) {
        std::vector<Mgx3D::Geom::Curve*> curves;
        verticesMgx[iPoint]->get(curves);

        for(unsigned int iCurve=0; iCurve<curves.size(); iCurve++) {
            std::vector<gmds::geom::GeomCurve*> curves_tmp;
            m_mgx2gmdsPoints[verticesMgx[iPoint]]->get(curves_tmp);
            bool found = false;
            for(unsigned int iCurve_tmp=0; iCurve_tmp<curves_tmp.size(); iCurve_tmp++) {
                if(curves_tmp[iCurve_tmp] == m_mgx2gmdsCurves[curves[iCurve]]) {
                    found = true;
                }
            }
            if(!found) {
                m_mgx2gmdsPoints[verticesMgx[iPoint]]->add(m_mgx2gmdsCurves[curves[iCurve]]);
            }
        }
    }

    for(unsigned int iCurve=0; iCurve<curvesMgx.size(); iCurve++) {
        std::vector<Mgx3D::Geom::Vertex*> points;
        curvesMgx[iCurve]->get(points);

        for(unsigned int iPoint=0; iPoint<points.size(); iPoint++) {
            std::vector<gmds::geom::GeomPoint*> points_tmp;
            m_mgx2gmdsCurves[curvesMgx[iCurve]]->get(points_tmp);
            bool found = false;
            for(unsigned int iPoint_tmp=0; iPoint_tmp<points_tmp.size(); iPoint_tmp++) {
                if(points_tmp[iPoint_tmp] == m_mgx2gmdsPoints[points[iPoint]]) {
                    found = true;
                }
            }
            if(!found) {
                m_mgx2gmdsCurves[curvesMgx[iCurve]]->add(m_mgx2gmdsPoints[points[iPoint]]);
            }
        }

        std::vector<Mgx3D::Geom::Surface*> surfaces;
        curvesMgx[iCurve]->get(surfaces);

        for(unsigned int iSurf=0; iSurf<surfaces.size(); iSurf++) {
            std::vector<gmds::geom::GeomSurface*> surfaces_tmp;
            m_mgx2gmdsCurves[curvesMgx[iCurve]]->get(surfaces_tmp);
            bool found = false;
            for(unsigned int iSurf_tmp=0; iSurf_tmp<surfaces_tmp.size(); iSurf_tmp++) {
                if(surfaces_tmp[iSurf_tmp] == m_mgx2gmdsSurfaces[surfaces[iSurf]]) {
                    found = true;
                }
            }
            if(!found) {
                m_mgx2gmdsCurves[curvesMgx[iCurve]]->add(m_mgx2gmdsSurfaces[surfaces[iSurf]]);
            }
        }
    }

    for(unsigned int iSurf=0; iSurf<surfacesMgx.size(); iSurf++) {
        std::vector<Mgx3D::Geom::Curve*> curves;
        surfacesMgx[iSurf]->get(curves);

        for(unsigned int iCurve=0; iCurve<curves.size(); iCurve++) {
            std::vector<gmds::geom::GeomCurve*> curves_tmp;
            m_mgx2gmdsSurfaces[surfacesMgx[iSurf]]->get(curves_tmp);
            bool found = false;
            for(unsigned int iCurve_tmp=0; iCurve_tmp<curves_tmp.size(); iCurve_tmp++) {
                if(curves_tmp[iCurve_tmp] == m_mgx2gmdsCurves[curves[iCurve]]) {
                    found = true;
                }
            }
            if(!found) {
                m_mgx2gmdsSurfaces[surfacesMgx[iSurf]]->add(m_mgx2gmdsCurves[curves[iCurve]]);
            }
        }

        std::vector<Mgx3D::Geom::Volume*> volumes;
        surfacesMgx[iSurf]->get(volumes);

        for(unsigned int iVol=0; iVol<volumes.size(); iVol++) {
            std::vector<gmds::geom::GeomVolume*> volumes_tmp;
            m_mgx2gmdsSurfaces[surfacesMgx[iSurf]]->get(volumes_tmp);
            bool found = false;
            for(unsigned int iVol_tmp=0; iVol_tmp<volumes_tmp.size(); iVol_tmp++) {
                if(volumes_tmp[iVol_tmp] == m_mgx2gmdsVolumes[volumes[iVol]]) {
                    found = true;
                }
            }
            if(!found) {
                m_mgx2gmdsSurfaces[surfacesMgx[iSurf]]->add(m_mgx2gmdsVolumes[volumes[iVol]]);
            }
        }
    }

    for(unsigned int iVol=0; iVol<volumesMgx.size(); iVol++) {
        std::vector<Mgx3D::Geom::Surface*> surfaces;
        volumesMgx[iVol]->get(surfaces);

        for(unsigned int iSurf=0; iSurf<surfaces.size(); iSurf++) {
            std::vector<gmds::geom::GeomSurface*> surfaces_tmp;
            m_mgx2gmdsVolumes[volumesMgx[iVol]]->get(surfaces_tmp);
            bool found = false;
            for(unsigned int iSurf_tmp=0; iSurf_tmp<surfaces_tmp.size(); iSurf_tmp++) {
                if(surfaces_tmp[iSurf_tmp] == m_mgx2gmdsSurfaces[surfaces[iSurf]]) {
                    found = true;
                }
            }
            if(!found) {
                m_mgx2gmdsVolumes[volumesMgx[iVol]]->add(m_mgx2gmdsSurfaces[surfaces[iSurf]]);
            }
        }
    }

}
/*----------------------------------------------------------------------------*/
GMDSGeomManagerAdapter::~GMDSGeomManagerAdapter()
{

}
/*----------------------------------------------------------------------------*/
gmds::geom::GeomVolume* GMDSGeomManagerAdapter::
newVolume()
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomManagerAdapter::newVolume pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
gmds::geom::GeomSurface* GMDSGeomManagerAdapter::
newSurface()
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomManagerAdapter::newSurface pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
gmds::geom::GeomCurve* GMDSGeomManagerAdapter::
newCurve()
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomManagerAdapter::newCurve pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
gmds::geom::GeomPoint* GMDSGeomManagerAdapter::
newPoint()
{
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomManagerAdapter::newPoint pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
gmds::TInt GMDSGeomManagerAdapter::
getNbPoints() const
{
    return m_points.size();
}
/*----------------------------------------------------------------------------*/
gmds::TInt GMDSGeomManagerAdapter::
getNbCurves() const
{
    return m_curves.size();
}
/*----------------------------------------------------------------------------*/
gmds::TInt GMDSGeomManagerAdapter::
getNbSurfaces() const
{
    return m_surfaces.size();
}
/*----------------------------------------------------------------------------*/
gmds::TInt GMDSGeomManagerAdapter::
getNbVolumes() const
{
    return m_volumes.size();
}
/*----------------------------------------------------------------------------*/
void GMDSGeomManagerAdapter::
getVolumes(std::vector<gmds::geom::GeomVolume*>& AVolumes) const
{
    AVolumes.clear();

    for(unsigned int iVol=0; iVol<m_volumes.size(); iVol++) {
        AVolumes.push_back(m_volumes[iVol]);
    }
}
/*----------------------------------------------------------------------------*/
void GMDSGeomManagerAdapter::
getSurfaces(std::vector<gmds::geom::GeomSurface*>& ASurfaces) const
{
    ASurfaces.clear();

    for(unsigned int iSurf=0; iSurf<m_surfaces.size(); iSurf++) {
        ASurfaces.push_back(m_surfaces[iSurf]);
    }
}
/*----------------------------------------------------------------------------*/
void GMDSGeomManagerAdapter::
getCurves(std::vector<gmds::geom::GeomCurve*>& ACurves) const
{
    ACurves.clear();

    for(unsigned int iCurve=0; iCurve<m_curves.size(); iCurve++) {
        ACurves.push_back(m_curves[iCurve]);
    }
}
/*----------------------------------------------------------------------------*/
void GMDSGeomManagerAdapter::
getPoints(std::vector<gmds::geom::GeomPoint*>& APoints) const
{
    APoints.clear();

    for(unsigned int iPoint=0; iPoint<m_points.size(); iPoint++) {
        APoints.push_back(m_points[iPoint]);
    }
}
/*----------------------------------------------------------------------------*/
void GMDSGeomManagerAdapter::
exportTriangulation(const std::string& AFilename) const
{
//    Lima::Maillage m;
//
//    std::vector<gmds::geom::GeomVolume*> volumes;
//    this->getVolumes(volumes);
//
//    for(unsigned int iVolume=0; iVolume<volumes.size(); iVolume++) {
//        std::vector<gmds::geom::GeomSurface*> surfaces;
//        volumes[iVolume]->get(surfaces);
//
//        for(unsigned int iSurface=0; iSurface<surfaces.size(); iSurface++) {
//            std::vector<GEPETO::Triangle<3,GEPETO::Numeric<double> > > triangles;
//            surfaces[iSurface]->getTriangulation(triangles);
//        }
//    }
//
//    try{
//        m.ecrire(AFileName, Lima::SUFFIXE,1,false);
//    }
//    catch(Lima::write_erreur& e)
//    {
//        std::cout<<"LIMA ERREUR: "<<e.what()<<std::endl;
//        throw GMDSException(e.what());
//    }
    throw TkUtil::Exception (TkUtil::UTF8String ("GMDSGeomManagerAdapter::exportTriangulation pas disponible.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
gmds::geom::GeomPoint* GMDSGeomManagerAdapter::
getGMDSVertex(Mgx3D::Geom::Vertex* AVertex)
{
    return m_mgx2gmdsPoints[AVertex];
}
/*----------------------------------------------------------------------------*/
gmds::geom::GeomCurve* GMDSGeomManagerAdapter::
getGMDSCurve(Mgx3D::Geom::Curve* ACurve)
{
    return m_mgx2gmdsCurves[ACurve];
}
/*----------------------------------------------------------------------------*/
gmds::geom::GeomSurface* GMDSGeomManagerAdapter::
getGMDSSurface(Mgx3D::Geom::Surface* ASurf)
{
    return m_mgx2gmdsSurfaces[ASurf];
}
/*----------------------------------------------------------------------------*/
gmds::geom::GeomVolume* GMDSGeomManagerAdapter::
getGMDSVolume(Mgx3D::Geom::Volume* AVol)
{
    return m_mgx2gmdsVolumes[AVol];
}
/*----------------------------------------------------------------------------*/
void GMDSGeomManagerAdapter::
reorientSurfacesTriangulation(Mgx3D::Geom::Volume* AVol)
{
	// we regenerate the triangulations for the surfaces of AVol in order to have
	// outward normals.
	{
		std::vector<Mgx3D::Geom::Surface*> surfaces;
		AVol->get(surfaces);

		for(unsigned int iSurf=0; iSurf<surfaces.size(); iSurf++) {

			Mgx3D::Geom::GMDSGeomSurfaceAdapter* surf = dynamic_cast<Mgx3D::Geom::GMDSGeomSurfaceAdapter*> (getGMDSSurface(surfaces[iSurf]));
			CHECK_NULL_PTR_ERROR(surf);
			std::vector<gmds::math::Triangle > triangles;
			surf->getTriangulation(triangles);
			AVol->facetedRepresentationForwardOrient(surfaces[iSurf],&triangles);
		}
	}

	// we remove flat triangles in every surface of the model
	{
		std::vector<gmds::geom::GeomSurface*> surfaces;
		this->getSurfaces(surfaces);

		for(unsigned int iSurf=0; iSurf<surfaces.size(); iSurf++) {
			std::vector<gmds::math::Triangle > triangles;
			surfaces[iSurf]->getTriangulation(triangles);
			//WARNING , We get a copy of the triangulation and not the triangulation, it is a
			//difference with GMDS 1

			std::vector<gmds::math::Triangle >::iterator it;
			it = triangles.begin();

			for(; it!= triangles.end(); ) {
				if(!(it->isGood())) {
					it = triangles.erase(it);
				} else {
					it++;
				}
			}

		}
	}
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
