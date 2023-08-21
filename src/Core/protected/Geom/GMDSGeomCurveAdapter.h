/*----------------------------------------------------------------------------*/
/*
 * GMDSGeomCurveAdapter.h
 *
 *  Created on: 18 juin 2013
 *      Author: legoff
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_GMDSGEOMCURVEADAPTER_H_
#define MGX3D_GEOM_GMDSGEOMCURVEADAPTER_H_
/*----------------------------------------------------------------------------*/
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"

#include "Geom/GMDSGeomPointAdapter.h"
#include "Geom/GMDSGeomSurfaceAdapter.h"
/*----------------------------------------------------------------------------*/
#include "GMDS/CAD/GeomVolume.h"
#include "GMDS/CAD/GeomSurface.h"
#include "GMDS/CAD/GeomCurve.h"
#include "GMDS/CAD/GeomPoint.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class GMDSGeomCurveAdapter
 * \brief Classe utilisant une courbe géométrique de Magix3D pour implémenter
 *        les services requis par l'interface GeomCurve de GMDS
 *
 */
class GMDSGeomCurveAdapter: public gmds::geom::GeomCurve
{
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    GMDSGeomCurveAdapter(Geom::Curve& ACurve);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~GMDSGeomCurveAdapter();

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the adjacent points.
     *
     *  \param APnt the adjacent points.
     */
    virtual void get(std::vector<gmds::geom::GeomPoint*>& APnt) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the adjacent surfaces.
     *
     *  \param ASurf the adjacent surfaces
     */
    virtual void get(std::vector<gmds::geom::GeomSurface*>& ASurf) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the adjacent volumes
     *
     *  \param AVol the adjacent volumes.
     */
    virtual void get(std::vector<gmds::geom::GeomVolume*>& AVol) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Length of the curve
     */
    virtual double length() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Compute the dihedral angle (max of each edge of the curve)
     *
     *  \return the dihedral angle
     */
    virtual gmds::TCoord computeDihedralAngle() const;

    /*------------------------------------------------------------------------*/
    /** \brief Move a point AP near the surface to the closest point on the
     *         surface.
     *  \param AP
     */
    virtual void project(gmds::math::Point& AP) const;

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

    virtual gmds::TCoord computeDistanceHaussdorf(
            const gmds::math::Segment& ASegment) const;
    virtual gmds::TCoord computeDistanceHaussdorfSubCurve(
            const int& ANbPoints,
            const int& AIPoint,
            const gmds::math::Segment& ASegment) const;

    /*------------------------------------------------------------------------*/
    /** \brief Compute the tangent vector at one of the endpoints.
     *  \param AP one of the endpoints
     *  \param AV the tangent vector
     */
    virtual void computeVector(
            const gmds::geom::GeomPoint& AP,
            gmds::math::Vector& AV) const;

    virtual void getMultiplePoints(const int& ANbPoints, gmds::math::Point* APoints) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the first end point
     *
     */
    virtual gmds::geom::GeomPoint* getFirstPoint() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the second end point
     *
     */
    virtual gmds::geom::GeomPoint* getSecondPoint() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Return whether the curve is a loop or not
     *
     *  \return a boolean
     */
    virtual bool isALoop() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the surface on the left;
     *          left is defined as triangles oriented
     *          in the same direction as the curve's segments
     *
     */
    virtual gmds::geom::GeomSurface* getLeftSurface() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the surface on the left;
     *          left is defined as triangles oriented
     *          in the same direction as the curve's segments
     *
     */
    virtual gmds::geom::GeomSurface* getRightSurface() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Add an adjacent point
     *
     *  \param APoint the new adjacent surface to add
     */
    virtual void add(gmds::geom::GeomPoint* APoint);

    /*------------------------------------------------------------------------*/
    /** \brief  Add an adjacent surface
     *
     *  \param ASurf the new adjacent surface to add
     */
    virtual void add(gmds::geom::GeomSurface* ASurf);

private:

    /*------------------------------------------------------------------------*/
    /** \brief  Return the number of vertices owned by this curve.
     *
     */
    gmds::TInt getNbPoints() const;

    /* courbe geometrique adaptee pour une utilisation dans les algorithmes de
     * GMDS */
    Geom::Curve& m_mgx3d_curve;

    std::vector<gmds::geom::GeomPoint*> m_points;
    std::vector<gmds::geom::GeomSurface*> m_surfaces;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_GMDSGEOMCURVEADAPTER_H_ */
/*----------------------------------------------------------------------------*/
