/*----------------------------------------------------------------------------*/
/** \file    GeomSurface.h
 *  \author  F. LEDOUX
 *  \date    09/21/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHGEMS_MGSURFACE_H_
#define MESHGEMS_MGSURFACE_H_
/*----------------------------------------------------------------------------*/
// GMDS File Headers
/*----------------------------------------------------------------------------*/
#include "gmds/utils/CommonTypes.h"
#include "gmds/math/Point.h"
#include "gmds/math/Vector.h"
/*----------------------------------------------------------------------------*/
#include "MGEntity.h"
/*----------------------------------------------------------------------------*/
namespace MeshGems{
    /*----------------------------------------------------------------------------*/
    /** \class Surface
     *  \brief This class describe the services that are required by the
     *  	   mesh to the geometrical model. As a consequence, this interface only
     *  	   contains query methods.
     */
    /*----------------------------------------------------------------------------*/
    class MGSurface : public MGEntity {

    public:

      /*------------------------------------------------------------------------*/
      /** \brief  Constructor
       */
    MGSurface() :MGEntity(){ ; }

    /*------------------------------------------------------------------------*/
    /** \brief  Destructor
     */
    virtual ~MGSurface() { }

    /*------------------------------------------------------------------------*/
      /** \brief  provides the dimension of the geometrical entity.
       */
      int getDim() const { return 2; }

      /*------------------------------------------------------------------------*/
      /** \brief Moves a point AP near the surface to the closest point on the
       * 		   surface.
       *  \param AP
       */
      virtual void project(gmds::math::Point& AP) const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  computes normal at the closest point to AP in 3D.
       *
       *  \param AP the point
       *  \param AV  normal vector at the closest point of AP on this
       */
      virtual void normal(const gmds::math::Point& AP, gmds::math::Vector& AV) const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief Get the closest point from AP on the surface
       *  \param AP a 3D point
       *
       *  \return the closest point of APoint on the surface
       */
      virtual gmds::math::Point closestPoint(const gmds::math::Point& AP) const = 0;


      /*------------------------------------------------------------------------*/
      /** \brief Get the parametrics bounds of this surface
       *
       *  \return AUMin u min value of the paramtric space
       *  \return AUMax u max value of the paramtric space
       *  \return AVMin v min value of the paramtric space
       *  \return AVMax v max value of the paramtric space
       */
      virtual void bounds(gmds::TCoord& AUMin, gmds::TCoord& AUMax,
			  gmds::TCoord& AVMin, gmds::TCoord& AVMax) const = 0;


      /*------------------------------------------------------------------------*/
      /** \brief Computes the 3D point AP, the first and second derivative in the
       *			directions u and v at the point of parametric coordinates
       *			(AU, AV)
       *
       *	\param  AU u parameter
       *  \param  AV v parameter
       *  \return AP 3D point in (u,v) on this surface
       *  \return ADU  first derivative in u at point (u,v) on this surface
       *  \return ADV  first derivative in v at point (u,v) on this surface
       *  \return ADUU second derivative in u   at point (u,v) on this surface
       *  \return ADUV second derivative in u,v at point (u,v) on this surface
       *  \return ADVV second derivative in v   at point (u,v) on this surface
       */
      virtual void d2(const gmds::TCoord& AU, const gmds::TCoord& AV,
		      gmds::math::Point& AP,
		      gmds::math::Vector& ADU,
		      gmds::math::Vector& ADV,
		      gmds::math::Vector& ADUU,
		      gmds::math::Vector& ADUV,
		      gmds::math::Vector& ADVV) const=0;

      /*------------------------------------------------------------------------*/
      /** \brief  computes the area of the entity.
       */
      virtual gmds::TCoord area() const = 0;


      /*------------------------------------------------------------------------*/
      /** \brief  Provides the orientation of this surface.
	  */
      virtual MGOrientation orientation() const = 0;
    };
  /*----------------------------------------------------------------------------*/
} // end namespace MeshGems
/*----------------------------------------------------------------------------*/
#endif /* MESHGEMS_MGSURFACE_H_ */

