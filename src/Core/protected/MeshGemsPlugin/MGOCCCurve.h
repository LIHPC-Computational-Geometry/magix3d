/*----------------------------------------------------------------------------*/
/** \file    MGOCCCurve.h
 *  \author  F. LEDOUX
 *  \date    03/12/2015
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHGEMS_MGOCCCurve_H_
#define MESHGEMS_MGOCCCurve_H_
/*----------------------------------------------------------------------------*/
// GMDS File Headers
/*----------------------------------------------------------------------------*/
#include <GMDS/Utils/CommonTypes.h>
#include <GMDS/Math/Point.h>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include "MGCurve.h"
/*----------------------------------------------------------------------------*/
namespace MeshGems{

    class MGOCCManager;
    /*----------------------------------------------------------------------------*/
    /** \class MGCurve
     *  \brief This class describe the services that are required by the
     *  	   mesh to the geometrical model. As a consequence, this interface only
     *  	   contains query methods.
     */
    /*----------------------------------------------------------------------------*/
    class MGOCCCurve : public MGCurve {
    public:


      /*------------------------------------------------------------------------*/
      /** \brief  Constructor
       */
      MGOCCCurve(const int AIndex,
    		  const int AI2, MGOCCManager* AMng);

      /*------------------------------------------------------------------------*/
      /** \brief  Destructor
       */
      virtual ~MGOCCCurve() { }

      /*------------------------------------------------------------------------*/
      /** \brief Move a point AP near the surface to the closest point on the
       * 		   surface.
       *  \param AP
       */
      virtual void project(gmds::math::Point& AP) const ;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the first end point
       *
       */
      virtual MGPoint* firstPoint() const ;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the second end point
       *
       */
      virtual MGPoint* secondPoint() const ;

      /*------------------------------------------------------------------------*/
      /** \brief  Return whether the curve is a loop or not
       *
       *  \return a boolean
       */
       virtual bool isALoop() const;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the parameterization interval of curve *this in the
       *			surface ASurf.
       *
       *	\param ASurf the surface we want to compute the curve parameterization
       *	\return ATMin min value of the parameterization
       *	\return ATMax max value of the parameterization
       */
      virtual void bounds(
				      MGSurface* ASurf, gmds::TCoord& ATMin, gmds::TCoord& ATMax) const;

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
      virtual void parametricData(
					      MGSurface* ASurf, gmds::TCoord& AT,
					      gmds::TCoord AUV[2], gmds::TCoord ADT[2], gmds::TCoord ADTT[2]) const;

      /*------------------------------------------------------------------------*/
      /** \brief  Provides the orientation of *this.
	  */
      virtual MGOrientation orientation() const;


      /*------------------------------------------------------------------------*/
      /** \brief Indicates if *this is a curve internal to a surface. By internal,
       *		we mean that the curve does not belong to a wire enclosing ASurf but
       *		is an imprint inside ASurf
       *	\param  ASurf the surface we want to compute the curve parameterization
       *	\return a boolean value that means internal(true)/boundary(false)
       */
      virtual bool isInternal() const ;

      /*------------------------------------------------------------------------*/
      /** \brief  computes the length of the entity.
       */
      virtual gmds::TCoord length() const ;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the adjacent points.
       *
       *	  Inherited implementation must ensure to always access adjacent
       *	  entities in the same way.
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

      TopoDS_Edge getEdge() const {return m_edge;};
    private:
      /*------------------------------------------------------------------------*/
      /** \brief Check if a geometrical surface is an instance of OCCSurface, and
       *         if yes, provide the corresponding TopoDS_Face
       *  \return true if it works, false otherwise
       */
      bool getOCCFace(MGSurface* ASurf, TopoDS_Face& AFace) const;

    private:
      int m_index;
      int m_index2;
      MGOCCManager* m_manager;

      TopoDS_Edge m_edge;
      MGPoint* m_first_pnt;
      MGPoint* m_second_pnt;
    };
  /*----------------------------------------------------------------------------*/
} // namespace MeshGems

/*----------------------------------------------------------------------------*/
#endif /* MESHGEMS_MGMGOCCCurve_H_ */
