/*----------------------------------------------------------------------------*/
/*
 * GeomNewSurfaceImplementation.h
 *
 *  Created on: 4 nov. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMNEWSURFACEIMPLEMENTATION_H_
#define GEOMNEWSURFACEIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_WireData.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Volume;
class Surface;
class Curve;
class Vertex;
/*----------------------------------------------------------------------------*/
/**
 * \class GeomNewSurfaceImplementation
 * \brief Classe réalisant la création d'une surface à partir d'un contour
 *
 */
class GeomNewSurfaceImplementation: public GeomModificationBaseClass{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param es les entités sur lesquelles appliquer le splitting

     */
    GeomNewSurfaceImplementation(Internal::Context& c, std::vector<Curve*>& es);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomNewSurfaceImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la création
     *          Les entités créées sont stockées dans res
     */
    void perform(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();

protected:
        TopoDS_Shape newSurface(TopoDS_Shape& AShape);
        TopoDS_Edge buildEdge(const TopoDS_Edge& aE1, const TopoDS_Edge& aE2);
        void updateWire (Handle(ShapeExtend_WireData)& AWireData);


private:

    /* entités passées en argument à la commande. */
    std::vector<Curve*> m_curves_param;


    Handle(ShapeBuild_ReShape) m_context;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMNEWSURFACEIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/
