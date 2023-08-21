/*----------------------------------------------------------------------------*/
/** \file    MGOCCSurface.h
 *  \author  F. LEDOUX
 *  \date    03/12/2015
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHGEMS_MGOCCSURFACE_H_
#define MESHGEMS_MGOCCSURFACE_H_
/*----------------------------------------------------------------------------*/
// GMDS File Headers
/*----------------------------------------------------------------------------*/
#include <GMDS/Utils/CommonTypes.h>
#include <GMDS/Math/Point.h>
#include <GMDS/Math/Vector.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Face.hxx>
/*----------------------------------------------------------------------------*/
#include "MGSurface.h"
/*----------------------------------------------------------------------------*/
namespace MeshGems{
    class MGOCCManager;
    /*----------------------------------------------------------------------------*/
    /** \class MGOCCSurface
     *  \brief This class implements the GeomSurface interface using OpenCascade.
     */
    /*----------------------------------------------------------------------------*/
    class MGOCCSurface : public MGSurface
    {

    public:

      /*------------------------------------------------------------------------*/
      /** \brief  Constructor
       */
      MGOCCSurface(const int AIndex, MGOCCManager* AMng);

      /*------------------------------------------------------------------------*/
      /** \brief  Destructor
       */
      virtual ~MGOCCSurface() { }

      /*------------------------------------------------------------------------*/
      /** \brief Moves a point AP near the surface to the closest point on the
       * 		   surface.
       *  \param AP
       */
      virtual void project(gmds::math::Point& AP) const;

      /*------------------------------------------------------------------------*/
      /** \brief  computes normal at the closest point to AP in 3D.
       *
       *  \param AP the point
       *  \param AV  normal vector at the closest point of AP on this
       */
      virtual void normal(const gmds::math::Point& AP, gmds::math::Vector& AV) const ;

      /*------------------------------------------------------------------------*/
      /** \brief Get the closest point from AP on the surface
       *  \param AP a 3D point
       *
       *  \return the closest point of APoint on the surface
       */
      virtual gmds::math::Point closestPoint(const gmds::math::Point& AP) const;


      /*------------------------------------------------------------------------*/
      /** \brief Get the parametrics bounds of this surface
       *
       *  \return AUMin u min value of the paramtric space
       *  \return AUMax u max value of the paramtric space
       *  \return AVMin v min value of the paramtric space
       *  \return AVMax v max value of the paramtric space
       */
      virtual void bounds(gmds::TCoord& AUMin, gmds::TCoord& AUMax,
			  gmds::TCoord& AVMin, gmds::TCoord& AVMax) const;


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
		      gmds::math::Vector& ADVV) const;

      /*------------------------------------------------------------------------*/
      /** \brief  computes the area of the entity.
       */
      virtual gmds::TCoord area() const ;


      /*------------------------------------------------------------------------*/
      /** \brief  Provides the orientation of this surface.
	  */
      virtual MGOrientation orientation() const;

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

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the geom_entity id.
       *
       *	\return the entity id
       */
      TopoDS_Face getOCCFace() const;

    private:
      int m_index;
      MGOCCManager* m_manager;
      TopoDS_Face m_face;
    };

  /*----------------------------------------------------------------------------*/
} // end namespace gmds
/*----------------------------------------------------------------------------*/
#endif /* GMDS_GEOM_OCCSURFACE_H_ */

