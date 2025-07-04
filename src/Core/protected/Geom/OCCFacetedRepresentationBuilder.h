#ifndef MGX3D_GEOM_OCCFACETEDREPRESENTATIONBUILDER_H_
#define MGX3D_GEOM_OCCFACETEDREPRESENTATIONBUILDER_H_
/*----------------------------------------------------------------------------*/
#include <gmds/math/Triangle.h>
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
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
    OCCFacetedRepresentationBuilder(const TopoDS_Shape& shape);

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

    /* shape dont on doit construire une représentation */
    const TopoDS_Shape& m_shape;

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

