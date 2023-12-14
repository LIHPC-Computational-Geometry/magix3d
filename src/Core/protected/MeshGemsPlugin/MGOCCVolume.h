/*----------------------------------------------------------------------------*/
/** \file    OCCVolume.h
 *  \author  F. LEDOUX
 *  \date    03/12/2015
 */
/*----------------------------------------------------------------------------*/
/** \file    OCCVolume.h
 *  \author  F. LEDOUX
 *  \date    03/12/2015
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHGEMS_MGOCCVOLUME_H_
#define MESHGEMS_MGOCCVOLUME_H_
/*----------------------------------------------------------------------------*/
// GMDS File Headers
/*----------------------------------------------------------------------------*/
#include <gmds/utils/CommonTypes.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Solid.hxx>

#include "MGVolume.h"
/*----------------------------------------------------------------------------*/
namespace MeshGems{
    class MGOCCManager;
    /*----------------------------------------------------------------------------*/
    /** \class MGOCCVolume
     *  \brief This class implements the volume interface with OpenCascade.
     */
    /*----------------------------------------------------------------------------*/
    class MGOCCVolume : public MGVolume {
    public:


      /*------------------------------------------------------------------------*/
      /** \brief  Constructor
       */
      MGOCCVolume(const int AIndex, MGOCCManager* AMng);
      
      /*------------------------------------------------------------------------*/
      /** \brief  Destructor
       */
      virtual ~MGOCCVolume() { }

      /*------------------------------------------------------------------------*/
      /** \brief  computes the volume of the entity.
       */
      virtual gmds::TCoord volume() const ;

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
      TopoDS_Solid m_solid;
    };
  /*----------------------------------------------------------------------------*/
} // namespace MeshGems
/*----------------------------------------------------------------------------*/
#endif /* MESHGEMS_MGOCCVOLUME_H_ */

