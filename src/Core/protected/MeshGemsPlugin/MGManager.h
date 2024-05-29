/*----------------------------------------------------------------------------*/
/** \file    MGManager.h
 *  \author  F. LEDOUX
 *  \date    30/06/11
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHGEMS_MGMANAGER_H_
#define MESHGEMS_MGMANAGER_H_
/*----------------------------------------------------------------------------*/
// GMDS File Headers
#include "gmds/utils/CommonTypes.h"
/*----------------------------------------------------------------------------*/
#include <vector>
#include <string>
/*----------------------------------------------------------------------------*/
namespace MeshGems{
    /*----------------------------------------------------------------------------*/
    //forward declarations
    class MGPoint;
    class MGCurve;
    class MGSurface;
    class MGVolume;
    /*----------------------------------------------------------------------------*/
    /** \class MGManager
     *  \brief This interface gathers the factory methods required for every
     *  		geometric model, the access to all the geom entities stored in the
     *  		model as the responsability to delete geometric entities.
     *
     *  \param TBase the basic type used to store geometric coordinates.
     */
    /*----------------------------------------------------------------------------*/
    class MGManager {
      
    public:
      
      /*------------------------------------------------------------------------*/
      /** \brief  Get the number of points of the model.
       *
       *	\return the number of points.
       */
      virtual gmds::TInt getNbPoints() const = 0;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Get the number of curves of the model.
       *
       *	\return the number of curves.
       */
      virtual gmds::TInt getNbCurves() const = 0;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Get the number of surfaces of the model.
       *
       *	\return the number of surfaces.
       */
      virtual gmds::TInt getNbSurfaces() const = 0;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Get the number of volumes of the model.
       *
       *	\return the number of volumes.
       */
      virtual gmds::TInt getNbVolumes() const = 0;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Access to all the points of the model.
       *
       *  \param points the points of the model.
       */
      virtual void getPoints(std::vector<MGPoint*>& points) const = 0;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Access to all the curves of the model.
       *
       *  \param curves the curves of the model.
       */
      virtual void getCurves(std::vector<MGCurve*>& curves) const = 0;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Access to all the surface of the model.
       *
       *  \param surfaces the surfaces of the model.
       */
      virtual void getSurfaces(std::vector<MGSurface*>& surfaces)const = 0;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Access to all the volumes of the model.
       *
       *  \param volumes the volumes of the model.
       */
      virtual void getVolumes(std::vector<MGVolume*>& volumes) const = 0;
      
      
      /*------------------------------------------------------------------------*/
      /** \brief  import a BRep file into the mode
       *
       *  \param AFileName the name of the file to be imported.
       *  \return true if the inmport worked, false otherwise.
       */
      virtual bool importBREP(const std::string& AFileName)=0;

    };
  /*----------------------------------------------------------------------------*/
} // namespace MeshGems
/*----------------------------------------------------------------------------*/
#endif /* MESHGEMS_MGMANAGER_H_ */

