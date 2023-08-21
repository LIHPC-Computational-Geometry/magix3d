/*----------------------------------------------------------------------------*/
/*
 * GMDSGeomPointAdapter.h
 *
 *  Created on: 18 juin 2013
 *      Author: legoff
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_GMDSGEOMPOINTADAPTER_H_
#define MGX3D_GEOM_GMDSGEOMPOINTADAPTER_H_
/*----------------------------------------------------------------------------*/
#include "Geom/Vertex.h"
#include "Geom/GMDSGeomCurveAdapter.h"
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
 * \class GMDSGeomPointAdapter
 * \brief Classe utilisant un point géométrique de Magix3D pour implémenter
 *        les services requis par l'interface GeomPoint de GMDS
 *
 */
class GMDSGeomPointAdapter: public gmds::geom::GeomPoint
{
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    GMDSGeomPointAdapter(Geom::Vertex& AVertex);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~GMDSGeomPointAdapter();

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the adjacent curves.
     *
     *  \param APnt the adjacent points.
     */
    virtual void get(std::vector<gmds::geom::GeomCurve*>& ACur) const;

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
    /** \brief  Access to the adjacent curves in an ordered fashion.
     *
     *  \param ACur the ordered adjacent curves.
     */
//    virtual void getOrdered(std::vector<gmds::geom::GeomCurve*>& ACur) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the adjacent curves in a direct ordered fashion.
     *
     *  \param ACur the direct ordered adjacent curves.
     */
//    virtual void getOrderedDirect(std::vector<gmds::geom::GeomCurve*>& ACur) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Return the number of curve incident to this point.
     *
     */
    virtual gmds::TInt getNbCurves() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to X coordinate
     *
     *  \return value of the X coordinate
     */
    virtual gmds::TCoord X() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to Y coordinate
     *
     *  \return value of the Y coordinate
     */
    virtual gmds::TCoord Y() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to Z coordinate
     *
     *  \return value of the Z coordinate
     */
    virtual gmds::TCoord Z() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to X, Y and Z coordinates
     *
     *  \param  ACoordinates will receive the value of the X, Y and Z coordinates
     */
    virtual void getXYZ(gmds::TCoord ACoordinates[3]) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the point as a NumericPoint
     *
     *  \return a numeric point
     */
    gmds::math::Point getPoint() const;

    /*------------------------------------------------------------------------*/
    /** \brief  computes the area of the entity.
     */
    virtual gmds::TCoord computeArea() const {return 0;}

    /*------------------------------------------------------------------------*/
    /** \brief  computes the bounding box
     *
     *  \param minXYZ The minimum coordinate of the bounding box.
     *  \param maxXYZ The maximum coordinate of the bounding box.
     */
    virtual void computeBoundingBox(gmds::TCoord minXYZ[3], gmds::TCoord maxXYZ[3]) const
    {
        minXYZ[0]=X();  maxXYZ[0]=X();
        minXYZ[1]=Y();  maxXYZ[1]=Y();
        minXYZ[2]=Z();  maxXYZ[2]=Z();
    }

    virtual int getId() const {
//        m_mgx3d_vertex.getID();
    	return 0;
    }

    /*------------------------------------------------------------------------*/
    /** \brief  Add an adjacent curve
     *
     *  \param ACurve the new adjacent curve to add
     */
    virtual void add(gmds::geom::GeomCurve* ACurve);

private:
    /* courbe geometrique adaptee pour une utilisation dans les algorithmes de
     * GMDS */
    Geom::Vertex& m_mgx3d_vertex;

    std::vector<gmds::geom::GeomCurve*> m_curves;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_GMDSGEOMPOINTADAPTER_H_ */
/*----------------------------------------------------------------------------*/
