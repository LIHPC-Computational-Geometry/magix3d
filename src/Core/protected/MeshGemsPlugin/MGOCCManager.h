/*----------------------------------------------------------------------------*/
/** \file    OCCManager.h
 *  \author  F. LEDOUX
 *  \date    30/06/11
 */
/*----------------------------------------------------------------------------*/
#ifndef MESHGEMS_MGOCCMANAGER_H_
#define MESHGEMS_MGOCCMANAGER_H_
/*----------------------------------------------------------------------------*/
// GMDS File Headers
#include <vector>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
/*----------------------------------------------------------------------------*/
#include "MeshGemsPlugin/MGManager.h"
#include "MeshGemsPlugin/MGPoint.h"
#include "MeshGemsPlugin/MGCurve.h"
#include "MeshGemsPlugin/MGSurface.h"
#include "MeshGemsPlugin/MGVolume.h"
/*----------------------------------------------------------------------------*/
namespace MeshGems{

    class MGOCCManager;
    /*----------------------------------------------------------------------------*/
    /** \class MGOCCManager
     *  \brief This class implements the GeomManager interface using OpenCascade
     */
    /*----------------------------------------------------------------------------*/
    class MGOCCManager : public MGManager {
      
      friend class MGOCCPoint;
      friend class MGOCCCurve;
      friend class MGOCCSurface;
      friend class MGOCCVolume;
    public:
      
      /*------------------------------------------------------------------------*/
      /** \brief  Constructor.
       */
      MGOCCManager();

      
      /*------------------------------------------------------------------------*/
      /** \brief  Destructor.
       */
      virtual ~MGOCCManager();
      
      /*------------------------------------------------------------------------*/
      /** \brief  Get the number of points of the model.
       *
       *	\return the number of points.
       */
      virtual gmds::TInt getNbPoints() const ;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Get the number of curves of the model.
       *
       *	\return the number of curves.
       */
      virtual gmds::TInt getNbCurves() const ;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Get the number of surfaces of the model.
       *
       *	\return the number of surfaces.
       */
      virtual gmds::TInt getNbSurfaces() const;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Get the number of volumes of the model.
       *
       *	\return the number of volumes.
       */
      virtual gmds::TInt getNbVolumes() const;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Access to all the points of the model.
       *
       *  \param points the points of the model.
       */
      virtual void getPoints(std::vector<MGPoint*>& points) const;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Access to all the curves of the model.
       *
       *  \param curves the curves of the model.
       */
      virtual void getCurves(std::vector<MGCurve*>& curves) const;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Access to all the surface of the model.
       *
       *  \param surfaces the surfaces of the model.
       */
      virtual void getSurfaces(std::vector<MGSurface*>& surfaces)const;
      
      /*------------------------------------------------------------------------*/
      /** \brief  Access to all the volumes of the model.
       *
       *  \param volumes the volumes of the model
       */
      virtual void getVolumes(std::vector<MGVolume*>& volumes) const;
      
      
      /*------------------------------------------------------------------------*/
      /** \brief  import a BRep file into the mode
       *
       *  \param AFileName the name of the file to be imported.
       *  \return true if the inmport worked, false otherwise.
       */
      virtual bool importBREP(const std::string& AFileName);

      /*------------------------------------------------------------------------*/
      /** \brief  import a face AFace. The whole model is cleaned first.
       *
       *  \param AFace face imported in the model to be meshed
       */
      virtual bool importFace(const TopoDS_Face &AFace);

    private:      


      /*------------------------------------------------------------------------*/
      /** \brief clear all the geom_entities
       */
      void clear();
      /*------------------------------------------------------------------------*/
      /** \brief create a volume
       */
      void newVolume(const TopoDS_Solid& AShape);
      /*------------------------------------------------------------------------*/
      /** \brief create a surface
       */
      void newSurface(const TopoDS_Face& AShape);
      /*------------------------------------------------------------------------*/
      /** \brief create a curve
       */
      void newCurve(const TopoDS_Edge& AShape);
      /*------------------------------------------------------------------------*/
      /** \brief create a point
       */
      void newPoint(const TopoDS_Vertex& AShape);

      /*------------------------------------------------------------------------*/
      /** \brief  import a shape ASahe. The whole model is cleaned first.
       *
       *  \param AShape the shape imported in the model to be meshed
       */
     void importShape(const TopoDS_Shape& AShape);

    private:
      /// map of OCC geometrical elements in the model (solids, shells, faces,...)
      TopTools_IndexedMapOfShape m_OCC_solids;
      TopTools_IndexedMapOfShape m_OCC_shells;
      TopTools_IndexedMapOfShape m_OCC_faces;
      TopTools_IndexedMapOfShape m_OCC_wires;
      TopTools_IndexedMapOfShape m_OCC_edges;
      std::vector<TopoDS_Edge> m_OCC_edges_vec;
      TopTools_IndexedMapOfShape m_OCC_vertices;


      //collection of geom entities
      std::vector<MGVolume* > m_volumes;
      std::vector<MGSurface*> m_surfaces;
      std::vector<MGCurve*  > m_curves;
      std::vector<MGPoint*  > m_points;


    };
    /*----------------------------------------------------------------------------*/
  } // namespace MeshGems
/*----------------------------------------------------------------------------*/
#endif /* MESHGEMS_MGOCCMANAGER_H_ */

