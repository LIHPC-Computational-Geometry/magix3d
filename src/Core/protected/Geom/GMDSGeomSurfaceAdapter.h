/*----------------------------------------------------------------------------*/
/*
 * GMDSGeomSurfaceAdapter.h
 *
 *  Created on: 9 nov. 2011
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_GMDSGEOMSURFACEADAPTER_H_
#define MGX3D_GEOM_GMDSGEOMSURFACEADAPTER_H_
/*----------------------------------------------------------------------------*/
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"


#include "Geom/GMDSGeomPointAdapter.h"
#include "Geom/GMDSGeomCurveAdapter.h"
#include "Geom/GMDSGeomVolumeAdapter.h"
/*----------------------------------------------------------------------------*/
#include <GMDS/Math/Triangle.h>
#include <GMDS/CAD/GeomVolume.h>
#include <GMDS/CAD/GeomSurface.h>
#include <GMDS/CAD/GeomCurve.h>
#include <GMDS/CAD/GeomPoint.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class GMDSGeomSurfaceAdapter
 * \brief Classe utilisant une surface géométrique de Magix3D pour implémenter
 *        les services requis par l'interface GeomSurface de GMDS
 *
 */
class GMDSGeomSurfaceAdapter: public gmds::geom::GeomSurface
{
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    GMDSGeomSurfaceAdapter(Geom::Surface& ASurf);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~GMDSGeomSurfaceAdapter();

    /*------------------------------------------------------------------------*/
    /** \brief  computes the area of the entity.
     */
    virtual gmds::TCoord computeArea() const;

    /*------------------------------------------------------------------------*/
    /** \brief  computes the bounding box
     *
     *  \param minXYZ The minimum coordinate of the bounding box.
     *  \param maxXYZ The maximum coordinate of the bounding box.
     */
    virtual void computeBoundingBox(gmds::TCoord minXYZ[3], gmds::TCoord maxXYZ[3]) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the adjacent points.
     *
     *  \param APnt the adjacent points.
     */
   virtual void get(std::vector<gmds::geom::GeomPoint*>& APnt) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the adjacent curves.
     *
     *  \param ACur the adjacent curves.
     */
    virtual void get(std::vector<gmds::geom::GeomCurve*>& ACur) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the adjacent volumes
     *
     *  \param AVol the adjacent volumes.
     */
    virtual void get(std::vector<gmds::geom::GeomVolume*>& AVol) const;

    /*------------------------------------------------------------------------*/
    /** \brief Move a point AP near the surface to the closest point on the
     *         surface.
     *  \param AP
     */
//    virtual void project(GEPETO::Point<3,gmds::TCoord >& AP) const;

    /*------------------------------------------------------------------------*/
    /** \brief  computes normal at the closest point to AP in 3D.
     *
     *  \param AP the point
     *
     *  \param AV   normal vector at the closest point of AP on this
     */
    virtual void computeNormal( const gmds::math::Point& AP,
            gmds::math::Vector& AV) const;

    /*------------------------------------------------------------------------*/
    /** \brief Get the closest point from AP on the surface
     *  \param AP a 3D point
     *
     *  \return the closest point of APoint on the surface
     */
    virtual gmds::math::Point
    closestPoint(const gmds::math::Point& AP) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Returns a copy of the internal triangulation
     *
     *  \param ATri a triangulation
     */
    virtual void getTriangulation(std::vector<gmds::math::Triangle >& ATri) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Reorient the triangulation.
     *
     */
    virtual void reorientTriangulation();

    /*------------------------------------------------------------------------*/
    /** \brief  Add an adjacent curve
     *
     *  \param ACurve the new adjacent curve to add
     */
    virtual void add(gmds::geom::GeomCurve* ACurve);

    /*------------------------------------------------------------------------*/
    /** \brief  Add an adjacent volume
     *
     *  \param AVol the new adjacent volumee to add
     */
    virtual void add(gmds::geom::GeomVolume* AVol);


private:
    /* surface geometrique adaptee pour une utilisation dans les algorithmes de
     * GMDS */
    Geom::Surface& m_mgx3d_surface;


    std::vector<gmds::geom::GeomCurve*> m_curves;
    std::vector<gmds::geom::GeomVolume*> m_volumes;

    /* stored faceted representation of the surface */
    std::vector<gmds::math::Triangle> m_triangulation;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_GMDSGEOMSURFACEADAPTER_H_ */
/*----------------------------------------------------------------------------*/
