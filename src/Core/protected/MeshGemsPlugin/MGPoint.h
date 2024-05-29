/*----------------------------------------------------------------------------*/
/** \file    MGPoint.h
 *  \author  F. LEDOUX
 *  \date    02/08/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHGEMS_MGPOINT_H_
#define MESHGEMS_MGPOINT_H_
/*----------------------------------------------------------------------------*/
// GMDS File Headers
/*----------------------------------------------------------------------------*/
#include "gmds/utils/CommonTypes.h"
#include "gmds/math/Point.h"
/*----------------------------------------------------------------------------*/
#include "MGEntity.h"
/*----------------------------------------------------------------------------*/
namespace MeshGems{
    /*----------------------------------------------------------------------------*/
    /** \class MGPoint
     *  \brief This abstract class describes the services that are required for
     *  accessing to a geometric point. As a consequence, this interface only
     *  	   contains query methods.
     */
    /*----------------------------------------------------------------------------*/
    class MGPoint : public MGEntity{

    public:

      /*------------------------------------------------------------------------*/
      /** \brief  Constructor
       */
      MGPoint() :MGEntity(){ ; }

      /*------------------------------------------------------------------------*/
      /** \brief  Destructor
       */
      virtual ~MGPoint() { }

      /*------------------------------------------------------------------------*/
      /** \brief  provides the dimension of the geometrical entity.
       */
      int getDim() const { return 0; }


      /*------------------------------------------------------------------------*/
      /** \brief  Return the number of curve incident to this point.
       *
       */
      virtual gmds::TInt getNbCurves() const = 0;


      /*------------------------------------------------------------------------*/
      /** \brief  Access to X coordinate
       *
       *  \return value of the X coordinate
       */
      virtual gmds::TCoord X() const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to Y coordinate
       *
       *  \return value of the Y coordinate
       */
      virtual gmds::TCoord Y() const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to Z coordinate
       *
       *  \return value of the Z coordinate
       */
      virtual gmds::TCoord Z() const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to X, Y and Z coordinates
       *
       *  \param  ACoordinates will receive the value of the X, Y and Z coordinates
       */
      virtual void XYZ(gmds::TCoord ACoordinates[3]) const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the point as a NumericPoint
       *
       *  \return a numeric point
       */
      virtual gmds::math::Point getPoint() const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Add an adjacent curve
       *
       *  \param ACurve the new adjacent curve to add
       */
       virtual void add(MGCurve* ACurve){ throw gmds::GMDSException("MGPoint::add Not yet implemented!"); }
    };

  /*----------------------------------------------------------------------------*/
} // end namespace MeshGems
/*----------------------------------------------------------------------------*/
#endif /* MESHGEMS_MGPOINT_H_ */

