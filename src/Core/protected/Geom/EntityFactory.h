/*----------------------------------------------------------------------------*/
/** \file EntityFactory.h
 *
 *  \author Franck Ledoux
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef ENTITYFACTORY_H_
#define ENTITYFACTORY_H_
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/PropertyBox.h"
#include "Geom/PropertyCylinder.h"
#include "Geom/PropertyCone.h"
#include "Geom/PropertySphere.h"
#include "Geom/PropertySpherePart.h"
#include "Geom/PropertyHollowSpherePart.h"
#include "Geom/PropertyPrism.h"
#include "Geom/PropertyHollowCylinder.h"
#include "Geom/PropertyHollowSphere.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
class ImportMDLImplementation;
}

namespace Topo {
class CommandAlignVertices;
class CoEdge;
}

namespace Mesh {
class MeshImplementation;
}

namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class EntityFactory
 *
 * \brief Fabrique permettant de centraliser la création des entités
 *        géométriques. Seules les classes amies peuvent l'appeler.
 *
 *        Attention, toutes commandes créant des entités géométriques doit être
 *        déclarée comme ami de la fabrique EntityFactory
 */
class EntityFactory
{
    friend class CommandCreateGeom;
    friend class CommandGeomCopy;
    friend class CommandNewVertex;
    friend class CommandNewVertexByProjection;
    friend class CommandNewVertexByCurveParameterization;
    friend class CommandNewSegment;
    friend class CommandNewSphere;
    friend class CommandNewSpherePart;
    friend class CommandNewHollowSpherePart;
    friend class CommandNewHollowSphere;
    friend class CommandNewHollowCylinder;
    friend class CommandNewBox;
    friend class CommandNewCylinder;
    friend class CommandNewCone;
    friend class CommandNewCircle;
    friend class CommandNewEllipse;
    friend class CommandNewArcCircle;
    friend class CommandNewArcCircleWithAngles;
    friend class CommandNewArcEllipse;
    friend class CommandNewBSpline;
    friend class CommandNewSurface;
    friend class CommandNewCurveByCurveProjectionOnSurface;
    friend class OCCGeomRepresentation;
    friend class GeomModificationBaseClass;
    friend class GeomNewPrismImplementation;
    friend class GeomCutImplementation;
    friend class GeomCommonImplementation;
    friend class GeomFuseImplementation;
    friend class GeomGluingImplementation;
    friend class GeomSectionImplementation;
    friend class GeomSectionByPlaneImplementation;
    friend class GeomRevolImplementation;
    friend class GeomExtrudeImplementation;
    friend class GeomImport;
    friend class GeomJoinCurvesImplementation;
    friend class Internal::ImportMDLImplementation;
    friend class Mesh::MeshImplementation;
    friend class Topo::CommandAlignVertices;
    friend class Topo::CoEdge;
    friend class Curve;
    friend class CommandNewFacetedSurfaces;
    friend class GeomSplitCurveImplementation;
    friend class CommandNewSurfaceByOffset;

public:
    /*------------------------------------------------------------------------*/
    /** \brief
     */
    GeomEntity* copy(GeomEntity* E);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Surface* newSurfaceByCopyWithOffset(Surface* E, const double& offset);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Vertex* newVertex(const Utils::Math::Point& P);
    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Curve* newSegment(const Geom::Vertex* start, const Geom::Vertex* end);
    /*------------------------------------------------------------------------*/
    /** \brief Cercle passant par les 3 sommets passés en arguments.
     */
    Curve* newCircle(
            const Geom::Vertex* p1,
            const Geom::Vertex* p2,
            const Geom::Vertex* p3);
    /*------------------------------------------------------------------------*/
    /** \brief Ellipse centrée sur le point center, plan défini par center/p1/p2, 
     *         grand axe défini par center/p1, 
     *         grand rayon défini par la distance center-p1, 
     *         petit rayon défini par la distance p2-axe principal.

     */
    Curve* newEllipse(
            const Geom::Vertex* p1,
            const Geom::Vertex* p2,
            const Geom::Vertex* center);
    /*------------------------------------------------------------------------*/
    /** \brief Arc de cercle dans le plan XY avec normale fixée
     */
    Curve* newArcCircle2D(const Geom::Vertex* center,
            const Geom::Vertex* start,
            const Geom::Vertex* end,
            const bool direction=true);
    /** \brief Arc de cercle pour le cas général en 3D avec points au centre et aux extrémités
     */
    Curve* newArcCircle(const Geom::Vertex* center,
            const Geom::Vertex* start,
            const Geom::Vertex* end,
            const bool direction=true,
            const Utils::Math::Vector& vec= Utils::Math::Vector(0,0,0),
            const bool circumCircle=false);

    /** \brief Arc de cercle pour le cas général en 3D avec rayon, angles et repère
     */
    Curve* newArcCircle(const double& angleDep,
                        const double& angleFin,
                        const double& rayon,
                        CoordinateSystem::SysCoord* rep);
    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Curve* newArcEllipse(const Geom::Vertex* center,
    		const Geom::Vertex* start,
            const Geom::Vertex* end,
            const bool direction=true);
    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Curve* newBSpline(const std::vector<Utils::Math::Point>& points, int degMin, int degMax);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Volume* newBox(PropertyBox* prop);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Volume* newCylinder(PropertyCylinder* prop);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Volume* newCone(PropertyCone* prop);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Volume* newSphere(PropertySphere* prop);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Volume* newSpherePart(PropertySpherePart* prop);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Volume* newHollowSpherePart(PropertyHollowSpherePart* prop);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Volume* newHollowCylinder(PropertyHollowCylinder* prop);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Volume* newHollowSphere(PropertyHollowSphere* prop);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Volume* newOCCVolume(TopoDS_Solid& s);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Volume* newOCCVolume(TopoDS_Shell& s);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Volume* newOCCShape(TopoDS_Shape& s);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Surface* newOCCSurface(TopoDS_Face& f);
    Surface* newOCCCompositeSurface(std::vector<TopoDS_Face>& v_ds_face);

    Surface* newFacetedSurface(uint gmds_id, std::vector<gmds::Face> faces);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Curve* newOCCCurve(TopoDS_Edge& e);
    Curve* newOCCCurve(TopoDS_Wire& w);

    Curve* newOCCCompositeCurve(std::vector<TopoDS_Edge>& v_ds_edge,
    		Utils::Math::Point& extremaFirst, Utils::Math::Point& extremaLast);

    Curve* newFacetedCurve(uint gmds_id, std::vector<gmds::Node> nodes);

    //    /*------------------------------------------------------------------------*/
//    /** \brief
//     */
//    Curve* newOCCCurve(TopoDS_Wire& w);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    Vertex* newOCCVertex(TopoDS_Vertex& v);

    Vertex* newFacetedVertex(uint gmds_id, gmds::Node node);

    /*------------------------------------------------------------------------*/
    /** Création d'une courbe
     *  par projection d'un segment défini par deux points,
     *  segment projeté sur une surface
     */
    Curve* newCurveByEdgeProjectionOnSurface(const Utils::Math::Point& P1,
            const Utils::Math::Point& P2,
            Surface* surface);

    /*------------------------------------------------------------------------*/
    /** Création d'une courbe
     *  par projection orthogonale d'une courbe sur une surface
     */
    Curve* newCurveByCurveProjectionOnSurface(Curve* curve,
            Surface* surface);

    /*------------------------------------------------------------------------*/
    Surface* newSurface(const std::vector<Curve*>& curves);

protected:

    /// factorisation de la crétion d'une courbe par projection
    Curve* newCurveByTopoDS_ShapeProjectionOnSurface(TopoDS_Shape shape,
    		Surface* surface);


 /*------------------------------------------------------------------------*/
    /** \brief
     */
    bool checkClosedWire(const std::vector<Geom::Curve*>& curves) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *  \param c le contexte
     */
    EntityFactory(Internal::Context& c);

    /*------------------------------------------------------------------------*/
    /** \brief
     */
    gp_Vec buildValidNormalForArcs(
            const gp_Pnt& p1,
            const gp_Pnt& p2,
            const gp_Pnt& p3,
            const gp_Vec& vec);

    /*------------------------------------------------------------------------*/
    /** Fait une coupe du volume suivant un plan et retourne le volume
     *  qui ne contient pas de point en une position donnée
     */
    TopoDS_Shape getShapeAfterPlaneCut(TopoDS_Shape shape, gp_Pln gp_plane, gp_Pnt pnt_to_eliminate);

private:
    /** le contexte */
    Internal::Context& m_context;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* ENTITYFACTORY_H_ */
/*----------------------------------------------------------------------------*/
