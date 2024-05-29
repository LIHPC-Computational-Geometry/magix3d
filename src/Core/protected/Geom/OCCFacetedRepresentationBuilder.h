/*----------------------------------------------------------------------------*/
/** \file OCCFacetedRepresentationBuilder.h
 *
 *  \author legoff
 *
 *  \date 08/07/2013
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_OCCFACETEDREPRESENTATIONBUILDER_H_
#define MGX3D_GEOM_OCCFACETEDREPRESENTATIONBUILDER_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomRepresentation.h"
//#include "Geom/GeomDisplayRepresentation.h"
/*----------------------------------------------------------------------------*/
//#include <TopoDS_Shape.hxx>
class TopoDS_Face;
class TopoDS_Shape;
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomEntity;
/*----------------------------------------------------------------------------*/
/**
 * \class OCCFacetedRepresentationBuilder
 *
 * \brief Calcul des propriétés géométriques d'un objet géométrique lorsqu'il
 *        connait OCC
 */
class OCCFacetedRepresentationBuilder {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    OCCFacetedRepresentationBuilder(
            const GeomEntity* entity,
            TopoDS_Shape& shape);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~OCCFacetedRepresentationBuilder();

    /*------------------------------------------------------------------------*/
    /** \brief  construit la représentation associée à la shape
     */
    void execute(
            std::vector<gmds::math::Triangle >& AVec);

    void execute(
            std::vector<gmds::math::Triangle>& AVec,
            TopoDS_Shape& AShape);

protected:

    void buildSurfaceRepresentation(
            std::vector<gmds::math::Triangle >& AVec);
    void buildSurfaceRepresentation(
            std::vector<gmds::math::Triangle >& AVec,
            TopoDS_Shape& AShape);
    void buildCurveRepresentation(std::vector<gmds::math::Triangle >& AVec);
    void buildVertexRepresentation(std::vector<gmds::math::Triangle >& AVec);
    void buildCurves(std::vector<gmds::math::Triangle >& AVec);
    void buildSurfaces(
            std::vector<gmds::math::Triangle >& AVec);
    void buildSurfaces(
                std::vector<gmds::math::Triangle >& AVec,
                TopoDS_Shape& AShape);

    void computeFaces   (
            std::vector<gmds::math::Triangle >& AVec);
    void computeFaces   (
                std::vector<gmds::math::Triangle >& AVec,
                TopoDS_Shape& AShape);
    void computeEdges   (std::vector<gmds::math::Triangle >& AVec);
    void computeVertices(std::vector<gmds::math::Triangle >& AVec);

    void fillRepresentation(
            const TopoDS_Face& aFace,
            std::vector<gmds::math::Triangle >& AVec);
protected:

    /* Entité géométrique dont on doit construire une représentation sachant que
     * sa représentation est de type OCC */
    const GeomEntity* m_entity;

    /* shape dont on doit construire une représentation */
    TopoDS_Shape& m_shape;

    /* représentation stockant les paramètre d'entrée et de sortie*/
//    GeomDisplayRepresentation* m_rep;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_OCCFACETEDREPRESENTATIONBUILDER_H_ */
/*----------------------------------------------------------------------------*/

