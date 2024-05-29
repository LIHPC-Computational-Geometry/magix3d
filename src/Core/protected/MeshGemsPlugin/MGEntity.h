/*----------------------------------------------------------------------------*/
/** \file    GeomEntity.h
 *  \author  F. LEDOUX
 *  \date    09/21/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHGEMS_MGENTITY_H_
#define MESHGEMS_MGENTITY_H_
/*----------------------------------------------------------------------------*/
#include "gmds/utils/CommonTypes.h"
/*----------------------------------------------------------------------------*/
#include <string>
/*----------------------------------------------------------------------------*/
namespace MeshGems{
    /*------------------------------------------------------------------------*/
    /** \enum Orientation gives a list of orientation option for geometric
     *        entities.
     */
    typedef enum {
      MG_FORWARD,
      MG_REVERSED,
      MG_INTERNAL,
      MG_EXTERNAL
    } MGOrientation;
      
    /*----------------------------------------------------------------------------*/
    //forward declarations
    class MGPoint;
    class MGCurve;
    class MGSurface;
    class MGVolume;
    /*----------------------------------------------------------------------------*/
    /** \class Entity
     *  \brief This interface defines services thar are common to all the geometric
     *  	   entities (volume, surface, curve, point).
     */
    /*----------------------------------------------------------------------------*/
    class MGEntity
    {
    public:


      /*------------------------------------------------------------------------*/
      /** \brief  Constructor
       */
      MGEntity() :is_meshed_(false){;}

      /*------------------------------------------------------------------------*/
      /** \brief  Destructor
       */
      virtual ~MGEntity() { }

      /*------------------------------------------------------------------------*/
      /** \brief  provides the dimension of the geometrical entity.
       */
      virtual int getDim() const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the adjacent points.
       *
       *			Inherited implementation must ensure to always access adjacent
       *			entities in the same way.
       *
       *  \param APnt the adjacent points.
       */
      virtual void get(std::vector<MGPoint*>& APnt) const = 0;
      /*------------------------------------------------------------------------*/
      /** \brief  Access to the adjacent curves.
       *
       *			Inherited implementation must ensure to always access adjacent
       *			entities in the same way.
       *
       *  \param ACur the adjacent curves.
       */
      virtual void get(std::vector<MGCurve*>& ACur) const=0;
      /*------------------------------------------------------------------------*/
      /** \brief  Access to the adjacent surfaces.
       *
       *			Inherited implementation must ensure to always access adjacent
       *			entities in the same way.
       *
       *  \param ASurf the adjacent surfaces
       */
      virtual void get(std::vector<MGSurface*>& ASurf) const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  Access to the adjacent volumes
       *
       *			Inherited implementation must ensure to always access adjacent
       *			entities in the same way.
       *
       *  \param AVol the adjacent volumes.
       */
      virtual void get(std::vector<MGVolume*>& AVol) const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  computes the bounding box
       *
       *	\param minXYZ The minimum coordinate of the bounding box.
       *	\param maxXYZ The maximum coordinate of the bounding box.
       */
      virtual void computeBoundingBox(gmds::TCoord minXYZ[3], gmds::TCoord maxXYZ[3]) const = 0;

      /*------------------------------------------------------------------------*/
      /** \brief  indicates if this entity is already meshed.
       *
       *	\return \a true if it is meshed, \a false otherwise.
       */
      virtual bool isMeshed() const { return is_meshed_; }

      /*------------------------------------------------------------------------*/
      /** \brief  set the mesh flag
       *
       *	\param AB the new value of the mesh flag
       */
      void setMesh(const bool AB){ is_meshed_ = AB; }


      /*------------------------------------------------------------------------*/
      /** \brief  Access to the geom_entity id.
       *
       *	\return the entity id
       */
      virtual int getId() const = 0;
    protected:
      /* boolean determining whether this entity is meshed */
      bool is_meshed_;
    };
  /*----------------------------------------------------------------------------*/
} // namespace MeshGems
/*----------------------------------------------------------------------------*/
#endif /* MESHGEMS_MGENTITY_H_ */

