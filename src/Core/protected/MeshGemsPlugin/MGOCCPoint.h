/*----------------------------------------------------------------------------*/
/** \file    OCCPoint.h
 *  \author  F. LEDOUX
 *  \date    03/12/2015
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHGEMS_MGOCCPOINT_H_
#define MESHGEMS_MGOCCPOINT_H_
/*----------------------------------------------------------------------------*/
// GMDS File Headers
/*----------------------------------------------------------------------------*/
#include "GMDS/Math/Point.h"
#include <gp_Pnt.hxx>
/*----------------------------------------------------------------------------*/
#include "MGPoint.h"
/*----------------------------------------------------------------------------*/
namespace MeshGems{

    class MGOCCManager;
    /*----------------------------------------------------------------------------*/
    /** \class MGOCCPoint
     *  \brief This class implementts the GeomPoint interface using OpenCascade
     */
    /*----------------------------------------------------------------------------*/
    class MGOCCPoint : public MGPoint
    {

    public:

      /*------------------------------------------------------------------------*/
      /** \brief  Constructor
       */
      MGOCCPoint(const int AIndex, MGOCCManager* AMng);

      /*------------------------------------------------------------------------*/
      /** \brief  Destructor
       */
      virtual ~MGOCCPoint() { }

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
      virtual void XYZ(gmds::TCoord ACoordinates[3]) const;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the point as a NumericPoint
       *
       *  \return a numeric point
       */
      virtual gmds::math::Point getPoint() const;
      /*------------------------------------------------------------------------*/
      /** \brief  Access to the adjacent points.
       *
       *			Inherited implementation must ensure to always access adjacent
       *			entities in the same way.
       *
       *  \param APnt the adjacent points.
       */
      virtual void get(std::vector<MGPoint*>& APnt) const;
      /*------------------------------------------------------------------------*/
      /** \brief  Access to the adjacent curves.
       *
       *			Inherited implementation must ensure to always access adjacent
       *			entities in the same way.
       *
       *  \param ACur the adjacent curves.
       */
      virtual void get(std::vector<MGCurve*>& ACur) const;
      /*------------------------------------------------------------------------*/
      /** \brief  Access to the adjacent surfaces.
       *
       *			Inherited implementation must ensure to always access adjacent
       *			entities in the same way.
       *
       *  \param ASurf the adjacent surfaces
       */
      virtual void get(std::vector<MGSurface*>& ASurf) const;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the adjacent volumes
       *
       *			Inherited implementation must ensure to always access adjacent
       *			entities in the same way.
       *
       *  \param AVol the adjacent volumes.
       */
      virtual void get(std::vector<MGVolume*>& AVol) const;

      /*------------------------------------------------------------------------*/
      /** \brief  computes the bounding box
       *
       *	\param minXYZ The minimum coordinate of the bounding box.
       *	\param maxXYZ The maximum coordinate of the bounding box.
       */
      virtual void computeBoundingBox(gmds::TCoord minXYZ[3], gmds::TCoord maxXYZ[3]) const;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the geom_entity id.
       *
       *	\return the entity id
       */
      virtual int getId() const;

    private:
      int m_index;
      MGOCCManager* m_manager;
      gp_Pnt m_pnt;
    };
  /*----------------------------------------------------------------------------*/
} // end namespace MeshGems
/*----------------------------------------------------------------------------*/
#endif /* MESHGEMS_MGOCCPOINT_H_ */

