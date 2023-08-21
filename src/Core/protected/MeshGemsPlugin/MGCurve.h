/*----------------------------------------------------------------------------*/
/** \file    GeomCurve.h
 *  \author  F. LEDOUX
 *  \date    09/21/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHGEMS_MGCURVE_H_
#define MESHGEMS_MGCURVE_H_
/*----------------------------------------------------------------------------*/
// GMDS File Headers
/*----------------------------------------------------------------------------*/
#include <GMDS/Utils/CommonTypes.h>
#include <GMDS/Math/Point.h>
#include "MGEntity.h"
/*----------------------------------------------------------------------------*/
namespace MeshGems{

    class MGPoint;
    class MGSurface;
    /*----------------------------------------------------------------------------*/
    /** \class MGCurve
     *  \brief This class describe the services that are required by the
     *  	   mesh to the geometrical model. As a consequence, this interface only
     *  	   contains query methods.
     */
    /*----------------------------------------------------------------------------*/
    class MGCurve : public MGEntity {
    public:


      /*------------------------------------------------------------------------*/
      /** \brief  Constructor
       */
      MGCurve() :MGEntity(){ ; }

      /*------------------------------------------------------------------------*/
      /** \brief  Destructor
       */
      virtual ~MGCurve() { }

      /*------------------------------------------------------------------------*/
      /** \brief  provides the dimension of the geometrical entity.
       */
      int getDim() const { return 1; }

      /*------------------------------------------------------------------------*/
      /** \brief Move a point AP near the surface to the closest point on the
       * 		   surface.
       *  \param AP
       */
      virtual void project(gmds::math::Point& AP) const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the first end point
       *
       */
      virtual MGPoint* firstPoint() const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the second end point
       *
       */
      virtual MGPoint* secondPoint() const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Return whether the curve is a loop or not
       *
       *  \return a boolean
       */
       virtual bool isALoop() const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the parameterization interval of curve *this in the
       *			surface ASurf.
       *
       *	\param ASurf the surface we want to compute the curve parameterization
       *	\return ATMin min value of the parameterization
       *	\return ATMax max value of the parameterization
       */
      virtual void bounds(
				      MGSurface* ASurf, gmds::TCoord& ATMin, gmds::TCoord& ATMax) const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Provides 2D data about the curve in the parametric space of the
	  surface ASurf
	  *
	  *	\param  ASurf the surface we want to compute the curve parameterization
	  *	\param  AT   the curve parameter we want extract data from
	  *	\return AUV  the 2D (u,v) parameter
	  *	\return ADT  the 2D 1st derivative to *this in the param. space of ASurf
	  *	\return ADTT the 2D 2nd derivative to *this in the param. space of ASurf
	  */
      virtual void parametricData(MGSurface* ASurf, gmds::TCoord& AT,
					      gmds::TCoord AUV[2], gmds::TCoord ADT[2], gmds::TCoord ADTT[2]) const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Provides the orientation of *this.
	  */
      virtual MGOrientation orientation() const =0;

      /*------------------------------------------------------------------------*/
      /** \brief Indicates if *this is a curve internal to a surface. By internal,
       *		we mean that the curve does not belong to a wire enclosing ASurf but
       *		is an imprint inside ASurf
       *	\return a boolean value that means internal(true)/boundary(false)
       */
      virtual bool isInternal() const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  computes the length of the entity.
       */
      virtual gmds::TCoord length() const = 0;

    };
  /*----------------------------------------------------------------------------*/
} // namespace MeshGems
/*----------------------------------------------------------------------------*/
#endif /* MESHGEMS_MGCURVE_H_ */

