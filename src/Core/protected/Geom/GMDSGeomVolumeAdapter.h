/*----------------------------------------------------------------------------*/
/*
 * GMDSGeomVolumeAdapter.h
 *
 *  Created on: 18 juin 2013
 *      Author: legoff
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_GMDSGEOMVOLUMEADAPTER_H_
#define MGX3D_GEOM_GMDSGEOMVOLUMEADAPTER_H_
/*----------------------------------------------------------------------------*/
#include "Geom/Volume.h"
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
 * \class GMDSGeomVolumeAdapter
 * \brief Classe utilisant un volume géométrique de Magix3D pour implémenter
 *        les services requis par l'interface GeomVolume de GMDS
 *
 */
class GMDSGeomVolumeAdapter: public gmds::geom::GeomVolume
{
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    GMDSGeomVolumeAdapter(Geom::Volume& AVolume);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~GMDSGeomVolumeAdapter();

//    /*------------------------------------------------------------------------*/
//    /** \brief  computes the area of the entity.
//     */
//    virtual double computeArea() const;
//
//    /*------------------------------------------------------------------------*/
//    /** \brief  computes the bounding box
//     *
//     *  \param minXYZ The minimum coordinate of the bounding box.
//     *  \param maxXYZ The maximum coordinate of the bounding box.
//     */
//    virtual void computeBoundingBox(double minXYZ[3], double maxXYZ[3]) const;

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
    /** \brief  Access to the adjacent surfaces.
     *
     *  \param ASurf the adjacent surfaces
     */
    virtual void get(std::vector<gmds::geom::GeomSurface*>& ASurf) const;

//    /*------------------------------------------------------------------------*/
//    /** \brief Move a point AP near the surface to the closest point on the
//     *         surface.
//     *  \param AP
//     */
//    virtual void project(GEPETO::Point<3,double>& AP) const;
//
//    /*------------------------------------------------------------------------*/
//    /** \brief  computes normal at the closest point to AP in 3D.
//     *
//     *  \param AP the point
//     *
//     *  \param AV   normal vector at the closest point of AP on this
//     */
//    virtual void computeNormal( const GEPETO::Point<3,double>& AP,
//            GEPETO::Vector<3,double>& AV) const;
//
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
    virtual void computeBoundingBox(
            gmds::TCoord minXYZ[3],
            gmds::TCoord maxXYZ[3]) const;


//    /*------------------------------------------------------------------------*/
//    /** \brief Get the closest point from AP on the surface
//     *  \param AP a 3D point
//     *
//     *  \return the closest point of APoint on the surface
//     */
//    virtual GEPETO::Point<3,double>
//    closestPoint(const GEPETO::Point<3,double>& AP) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Add an adjacent surface
     *
     *  \param ASurf the new adjacent surface to add
     */
    virtual void add(gmds::geom::GeomSurface* ASurf);

private:
    /* volume geometrique adapte pour une utilisation dans les algorithmes de
     * GMDS */
    Geom::Volume& m_mgx3d_volume;

    std::vector<gmds::geom::GeomSurface*> m_surfaces;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_GMDSGEOMVOLUMEADAPTER_H_ */
/*----------------------------------------------------------------------------*/
