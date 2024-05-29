/*----------------------------------------------------------------------------*/
/** \file OCCDisplayRepresentationBuilder.h
 *
 *  \author Franck Ledoux
 *
 *  \date 02/12/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_OCCDISPLAYREPRESENTATIONBUILDER_H_
#define MGX3D_GEOM_OCCDISPLAYREPRESENTATIONBUILDER_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomDisplayRepresentation.h"
/*----------------------------------------------------------------------------*/
//#include <TopoDS_Shape.hxx>
#include <GeomAbs_IsoType.hxx>
#include <BRepAdaptor_Surface.hxx>
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
 * \class OCCDisplayRepresentationBuilder
 *
 * \brief Calcul des propriétés géométriques d'un objet géométrique lorsqu'il
 *        connait OCC
 */
class OCCDisplayRepresentationBuilder {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    OCCDisplayRepresentationBuilder(const GeomEntity* entity,
                                    TopoDS_Shape& shape,
                                    GeomDisplayRepresentation* rep);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~OCCDisplayRepresentationBuilder();

    /*------------------------------------------------------------------------*/
    /** \brief  construit la représentation associée à la shape
     */
    void execute();

protected:

    void buildSurfaceRepresentation();
    void buildCurveRepresentation();
    void buildVertexRepresentation();
    void buildCurves();
    void buildIsoCurves();
    void buildSurfaces();

    void computeFaces   ();
    void computeEdges   ();
    void computeIsoEdges();
    void computeVertices();


    void DrawIso(const TopoDS_Face& AFace, GeomAbs_IsoType T,
            Standard_Real Par, Standard_Real T1, Standard_Real T2,
            Standard_Integer& startidx);
    void computeIsoEdges(const TopoDS_Face& AFace);
    // return true en cas d'erreur
    bool MoveToISO(Utils::Math::Point& pnt);
    void AddToISO(Utils::Math::Point& pnt);
    void fillRepresentation(const TopoDS_Face& aFace);
    void PlotIso (BRepAdaptor_Surface& S, GeomAbs_IsoType T,
            Standard_Real& U, Standard_Real& V,
            Standard_Real Step, Standard_Boolean& halt);
protected:

    /* Entité géométrique dont on doit construire une représentation sachant que
     * sa représentation est de type OCC */
    const GeomEntity* m_entity;

    /* shape dont on doit construire une représentation */
    TopoDS_Shape& m_shape;

    /* représentation stockant les paramètre d'entrée et de sortie*/
    GeomDisplayRepresentation* m_rep;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_OCCDISPLAYREPRESENTATIONBUILDER_H_ */
/*----------------------------------------------------------------------------*/

