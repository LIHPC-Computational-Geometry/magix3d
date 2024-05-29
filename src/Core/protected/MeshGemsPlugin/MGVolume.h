/*----------------------------------------------------------------------------*/
/** \file    GeomVolume.h
 *  \author  F. LEDOUX
 *  \date    09/21/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHGEMS_MGVOLUME_H_
#define MESHGEMS_MGVOLUME_H_
/*----------------------------------------------------------------------------*/
// GMDS File Headers
/*----------------------------------------------------------------------------*/
#include "gmds/utils/CommonTypes.h"
/*----------------------------------------------------------------------------*/
#include "MGEntity.h"
/*----------------------------------------------------------------------------*/
namespace MeshGems{
    /*----------------------------------------------------------------------------*/
    class GeomPoint;
    class GeomCurve;
    class GeomSurface;
    /*----------------------------------------------------------------------------*/
    /** \class GeomVolume
     *  \brief This class describe the services that are required by the
     *  	   mesh to the geometrical model. As a consequence, this interface only
     *  	   contains query methods.
     */
    /*----------------------------------------------------------------------------*/
    class MGVolume : public MGEntity {
    public:


    	/*------------------------------------------------------------------------*/
    	/** \brief  Constructor
    	 */
    	MGVolume() :MGEntity(){ ; }

    	/*------------------------------------------------------------------------*/
    	/** \brief  Destructor
    	 */
    	virtual ~MGVolume() { }

    	/*------------------------------------------------------------------------*/
    	/** \brief  provides the dimension of the geometrical entity.
    	 */
    	int getDim() const { return 3; }

    	/*------------------------------------------------------------------------*/
    	/** \brief  computes the volume of the entity.
    	 */
    	virtual gmds::TCoord volume() const = 0;


    };
  /*----------------------------------------------------------------------------*/
} // namespace gmds
/*----------------------------------------------------------------------------*/
#endif /* MESHGEMS_MGVOLUME_H_ */

