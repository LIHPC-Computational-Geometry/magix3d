/*----------------------------------------------------------------------------*/
/*
 * GMDSGeomManagerAdapter.h
 *
 *  Created on: 26 juin 2013
 *      Author: legoff
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_GMDSGEOMMANAGERADAPTER_H_
#define MGX3D_GEOM_GMDSGEOMMANAGERADAPTER_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomManager.h"

#include "Geom/GMDSGeomVolumeAdapter.h"
#include "Geom/GMDSGeomSurfaceAdapter.h"
#include "Geom/GMDSGeomCurveAdapter.h"
#include "Geom/GMDSGeomPointAdapter.h"
/*----------------------------------------------------------------------------*/
#include "gmds/cad/GeomManager.h"
#include "gmds/cad/GeomVolume.h"
#include "gmds/cad/GeomSurface.h"
#include "gmds/cad/GeomCurve.h"
#include "gmds/cad/GeomPoint.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class GMDSGeomManagerAdapter
 * \brief Classe utilisant un volume géométrique de Magix3D pour implémenter
 *        les services requis par l'interface GeomVolume de GMDS
 *
 */
class GMDSGeomManagerAdapter: public gmds::cad::GeomManager
{
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    GMDSGeomManagerAdapter(Mgx3D::Geom::GeomManager& AMgx3DGeomManager);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~GMDSGeomManagerAdapter();

    /*------------------------------------------------------------------------*/
    /** \brief  creation of a geometric volume
     */
    virtual gmds::cad::GeomVolume* newVolume();

    /*------------------------------------------------------------------------*/
    /** \brief  creation of a geometric surface
     */
    virtual gmds::cad::GeomSurface* newSurface();

    /*------------------------------------------------------------------------*/
    /** \brief  creation of a geometric curve
     */
    virtual gmds::cad::GeomCurve* newCurve();

    /*------------------------------------------------------------------------*/
    /** \brief  creation of a geometric point
     */
    virtual gmds::cad::GeomPoint* newPoint();

    /*------------------------------------------------------------------------*/
    /** \brief  Get the number of points of the model.
     *
     *  \return the number of points.
     */
    virtual gmds::TInt getNbPoints() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Get the number of curves of the model.
     *
     *  \return the number of curves.
     */
    virtual gmds::TInt getNbCurves() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Get the number of surfaces of the model.
     *
     *  \return the number of surfaces.
     */
    virtual gmds::TInt getNbSurfaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Get the number of volumes of the model.
     *
     *  \return the number of volumes.
     */
    virtual gmds::TInt getNbVolumes() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the points of the model.
     *
     *  \param points the points of the model.
     */
    virtual void getPoints(std::vector<gmds::cad::GeomPoint*>& APoints) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the curves of the model.
     *
     *  \param curves the curves of the model.
     */
    virtual void getCurves(std::vector<gmds::cad::GeomCurve*>& ACurves) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the surface of the model.
     *
     *  \param surfaces the surfaces of the model.
     */
    virtual void getSurfaces(std::vector<gmds::cad::GeomSurface*>& ASurfaces) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the volumes of the model.
     *
     *  \param volumes the volumes of the model.
     */
    virtual void getVolumes(std::vector<gmds::cad::GeomVolume*>& AVolumes) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the mapped GMDS vertex of the model.
     *
     *  \param AVertex the Geom::Vertex of the model.
     */
    virtual gmds::cad::GeomPoint* getGMDSVertex(Mgx3D::Geom::Vertex* AVertex);

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the mapped GMDS curve of the model.
     *
     *  \param ACurve the Geom::Curve of the model.
     */
    virtual gmds::cad::GeomCurve* getGMDSCurve(Mgx3D::Geom::Curve* ACurve);

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the mapped GMDS surface of the model.
     *
     *  \param ASurf the Geom::Surface of the model.
     */
    virtual gmds::cad::GeomSurface* getGMDSSurface(Mgx3D::Geom::Surface* ASurf);

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the mapped GMDS volume of the model.
     *
     *  \param AVol the Geom::Volume of the model.
     */
    virtual gmds::cad::GeomVolume* getGMDSVolume(Mgx3D::Geom::Volume* AVol);

    /*------------------------------------------------------------------------*/
    /** \brief  Export the model in a file.
     *
     *  \param AFile the name of the file.
     */
    virtual void exportTriangulation(const std::string& AFile) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Reorient the surfaces triangulations for the surfaces adjacent to
     * 			a volume .
     *
     *  \param AVol the Geom::Volume of the model.
     */
    virtual void reorientSurfacesTriangulation(Mgx3D::Geom::Volume* AVol);

private:

    Mgx3D::Geom::GeomManager& m_mgx3DGeomManager;

    /** volumes gérés par le manager */
    std::vector<Mgx3D::Geom::GMDSGeomVolumeAdapter*>  m_volumes;
    /** surfaces gérées par le manager */
    std::vector<Mgx3D::Geom::GMDSGeomSurfaceAdapter*> m_surfaces;
    /** courbes gérées par le manager */
    std::vector<Mgx3D::Geom::GMDSGeomCurveAdapter*>   m_curves;
    /** sommets gérés par le manager */
    std::vector<Mgx3D::Geom::GMDSGeomPointAdapter*>  m_points;

    std::map<Mgx3D::Geom::Volume *,gmds::cad::GeomVolume *> m_mgx2gmdsVolumes;
    std::map<Mgx3D::Geom::Surface*,gmds::cad::GeomSurface*> m_mgx2gmdsSurfaces;
    std::map<Mgx3D::Geom::Curve  *,gmds::cad::GeomCurve  *> m_mgx2gmdsCurves;
    std::map<Mgx3D::Geom::Vertex *,gmds::cad::GeomPoint  *> m_mgx2gmdsPoints;

    std::map<gmds::cad::GeomVolume *,Mgx3D::Geom::Volume *> m_gmds2mgxVolumes;
    std::map<gmds::cad::GeomSurface*,Mgx3D::Geom::Surface*> m_gmds2mgxSurfaces;
    std::map<gmds::cad::GeomCurve  *,Mgx3D::Geom::Curve  *> m_gmds2mgxCurves;
    std::map<gmds::cad::GeomPoint  *,Mgx3D::Geom::Vertex *> m_gmds2mgxPoints;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_GMDSGEOMMANAGERADAPTER_H_ */
/*----------------------------------------------------------------------------*/
