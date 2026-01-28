/*----------------------------------------------------------------------------*/
#include "Geom/EntityFactory.h"
#include "Geom/GeomProperty.h"
#include "Geom/GeomProjectImplementation.h"
#include "Geom/PropertyBox.h"
#include "Geom/PropertyCylinder.h"
#include "Geom/PropertyHollowCylinder.h"
#include "Geom/PropertyCone.h"
#include "Geom/PropertySphere.h"
#include "Geom/PropertyHollowSphere.h"
#include "Geom/Vertex.h"
#include "Geom/Surface.h"
#include "Utils/Entity.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"
#include "Utils/MgxException.h"
#include "Utils/MgxNumeric.h"
#include "SysCoord/SysCoord.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <StdFail_NotDone.hxx>
#include <gce_ErrorType.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAlgo_NormalProjection.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <GC_MakeCircle.hxx>
#include <GC_MakeEllipse.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <GeomConvert.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <ElCLib.hxx>
/*----------------------------------------------------------------------------*/
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
//#define _DEBUG2
/*----------------------------------------------------------------------------*/
// Définition de la fonction utilitaire GceErrorToString
std::string GceErrorToString(const gce_ErrorType& error) {
    switch (error) {
        case gce_Done:
            return "Done: Construction was successful.";
        case gce_ConfusedPoints:
            return "Confused Points: Two points are coincident.";
        case gce_NegativeRadius:
            return "Negative Radius: Radius value is negative.";
        case gce_ColinearPoints:
            return "Colinear Points: Three points are collinear.";
        case gce_IntersectionError:
            return "Intersection Error: Intersection cannot be computed.";
        case gce_NullAxis:
            return "Null Axis: Axis is undefined.";
        case gce_NullAngle:
            return "Null Angle: Angle value is invalid (usually null).";
        case gce_NullRadius:
            return "Null Radius: Radius is null.";
        case gce_InvertAxis:
            return "Invert Axis: Axis value is invalid.";
        case gce_BadAngle:
            return "Bad Angle: Angle value is invalid.";
        case gce_InvertRadius:
            return "Invert Radius: Radius value is incorrect (usually with respect to another radius).";
        case gce_NullFocusLength:
            return "Null Focus Length: Focus distance is null.";
        case gce_NullVector:
            return "Null Vector: Vector is null.";
        case gce_BadEquation:
            return "Bad Equation: Coefficients are incorrect (applies to the equation of a geometric object).";
        default:
            return "Unknown Error: An unknown error occurred.";
    }
}
/*----------------------------------------------------------------------------*/
EntityFactory::EntityFactory(Internal::Context& c)
: m_context(c)
{}
/*----------------------------------------------------------------------------*/
GeomEntity* EntityFactory::copy(GeomEntity* E)
{
    GeomEntity* res = E->clone(m_context);
    CHECK_NULL_PTR_ERROR (res)
    m_context.newGraphicalRepresentation (*res);
    return res;
}
/*----------------------------------------------------------------------------*/
Surface* EntityFactory::newSurfaceByCopyWithOffset(Surface* E, const double& offset)
{
	// version avec multiples faces, ce qui pose pb au "split"
	// qui est fait après pour retrouver les courbes adjacentes
	std::vector<TopoDS_Face> new_reps;

	try{

		auto init_reps = E->getOCCFaces();
        for (auto sh : init_reps) {
			BRepOffsetAPI_MakeOffsetShape MF;
            MF.PerformByJoin(sh, offset, Utils::Math::MgxNumeric::mgxDoubleEpsilon);
			if(MF.IsDone()){
				TopoDS_Shape aShape = MF.Shape();
				if (!aShape.IsNull()){
					TopExp_Explorer exp2;
					for(exp2.Init(TopoDS::Shell(aShape), TopAbs_FACE); exp2.More(); exp2.Next()){
					    TopoDS_Face aFace = TopoDS::Face(exp2.Current());
					    new_reps.push_back(aFace);
					  }
				}
#ifdef _DEBUG
				else
					std::cerr<<"aFace.IsNull() !"<<std::endl;
#endif
			}
			else
				throw Utils::BuildingException("Error during surface creation (error...)");
		} // end for i
	}
	catch(StdFail_NotDone& e){
		throw Utils::BuildingException("Erreur OCC lors de la création de la surface avec offset");
	}
	catch(Utils::BuildingException& e){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Error during surface creation, "<<e.getMessage();
		throw Utils::BuildingException(message);
	}

	Surface* surface = new Surface(m_context,
				m_context.newProperty(Utils::Entity::GeomSurface),
				m_context.newDisplayProperties(Utils::Entity::GeomSurface),
				new GeomProperty(), new_reps);

	m_context.newGraphicalRepresentation (*surface);
	return surface;
}
/*----------------------------------------------------------------------------*/
Vertex* EntityFactory::newVertex(const Utils::Math::Point& P)
{
    TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(gp_Pnt(P.getX(),P.getY(),P.getZ()));

    return newOCCVertex(V);
}
/*----------------------------------------------------------------------------*/
Vertex* EntityFactory::newOCCVertex(TopoDS_Vertex& v)
{
    Vertex*	vertex	= new Vertex(m_context,
            m_context.newProperty(Utils::Entity::GeomVertex),
            m_context.newDisplayProperties(Utils::Entity::GeomVertex),
            new GeomProperty(), v);
	CHECK_NULL_PTR_ERROR (vertex)
	m_context.newGraphicalRepresentation (*vertex);
	return vertex;
}
/*----------------------------------------------------------------------------*/
Curve* EntityFactory::newSegment(const Geom::Vertex* start,
                                  const Geom::Vertex* end)
{
    gp_Pnt P1(start->getX(), start->getY(), start->getZ());
    gp_Pnt P2(end->getX(), end->getY(), end->getZ());

    try {
        TopoDS_Edge e = BRepBuilderAPI_MakeEdge(P1,P2);
        Curve* curve = new Curve(m_context,
                    m_context.newProperty(Utils::Entity::GeomCurve),
                    m_context.newDisplayProperties(Utils::Entity::GeomCurve),
                    new GeomProperty(), e);
        m_context.newGraphicalRepresentation (*curve);
        return curve;
    } catch(StdFail_NotDone& e) {
        throw Utils::BuildingException("Error during Segment creation");
    }
}
/*----------------------------------------------------------------------------*/
gp_Vec EntityFactory::buildValidNormalForArcs(
            const gp_Pnt& center,
            const gp_Pnt& start,
            const gp_Pnt& end,
            const gp_Vec& normal)
{
//	std::cout<<"buildValidNormalForArcs ..."<<std::endl;
    if (start.IsEqual(center,0.0) ||
        start.IsEqual(end,0.0) ||
        end.IsEqual(center,0.0) )
        throw Utils::BuildingException(TkUtil::UTF8String ("Param. illicites - 2 pts (au moins) sont égaux", TkUtil::Charset::UTF_8));

    gp_Vec from_center_to_start(center, start);
    gp_Lin line(center, from_center_to_start);
//    std::cout<<"center: ("<<center.X()<<", "<<center.Y()<<", "<<center.Z()<<")"<<std::endl;
//    std::cout<<"start: ("<<start.X()<<", "<<start.Y()<<", "<<start.Z()<<")"<<std::endl;
//    std::cout<<"end: ("<<end.X()<<", "<<end.Y()<<", "<<end.Z()<<")"<<std::endl;
//    std::cout<<"normal: ("<<normal.X()<<", "<<normal.Y()<<", "<<normal.Z()<<")"<<std::endl;
    if(line.Contains(end,1e-7))
    {
//    	std::cout<<"cas aligné ..."<<std::endl;
        // center, start et end sont colinéaires
        if(normal.X()==0.0 &&  normal.Y()==0.0 && normal.Z()==0.0)
        {
            throw Utils::BuildingException(TkUtil::UTF8String ("Param. illicites - trois points alignés et pas de plan défini (normale indéfinie) ", TkUtil::Charset::UTF_8));
        }

        //std::cout<<"from_center_to_start.Dot(normal) = "<<from_center_to_start.Dot(normal)<<std::endl;
        //on vérifie que la normale est orthogonale aux 3 points
        if(fabs(from_center_to_start.Dot(normal))> 1e-5)
        {
            std::cerr<<"vecteur center to start: ("<<from_center_to_start.X()<<", "<<from_center_to_start.Y()<<", "<<from_center_to_start.Z()<<")"<<std::endl;
            std::cerr<<"normal: ("<<normal.X()<<", "<<normal.Y()<<", "<<normal.Z()<<")"<<std::endl;
            throw Utils::BuildingException(TkUtil::UTF8String ("Param. illicites - trois points alignés et plan défini par une normale non orthogonale à la ligne formée de ces trois points", TkUtil::Charset::UTF_8));
        }

        //la normale choisie est alors celle passée en paramètre
        return normal;
    }

    // la normale est calculée à partir des 3 points
    gp_Vec from_center_to_end(center, end);

    return from_center_to_start.Crossed(from_center_to_end);
}
/*----------------------------------------------------------------------------*/
Curve* EntityFactory::newCircle(
        const Geom::Vertex* p1,
        const Geom::Vertex* p2,
        const Geom::Vertex* p3)
{
    gp_Pnt P1(p1->getX(),p1->getY(),p1->getZ());
    gp_Pnt P2(p2->getX(),p2->getY(),p2->getZ());
    gp_Pnt P3(p3->getX(),p3->getY(),p3->getZ());

    if (P1.IsEqual(P2,0.0)|| P1.IsEqual(P3,0.0)|| P2.IsEqual(P3,0.0))
      throw Utils::BuildingException(TkUtil::UTF8String ("Param. illicites - 2 pts (au moins) sont égaux", TkUtil::Charset::UTF_8));

    try {
        GC_MakeCircle factory(P1,P2,P3);
        if (factory.IsDone()) {
            Handle(Geom_Circle) arc = factory.Value();
            TopoDS_Edge e = BRepBuilderAPI_MakeEdge(arc);
            Curve*	curve = new Curve(m_context,
                        m_context.newProperty(Utils::Entity::GeomCurve),
                        m_context.newDisplayProperties(Utils::Entity::GeomCurve),
                        new GeomProperty(), e);
            m_context.newGraphicalRepresentation (*curve);
            return curve;
        } else {
            // Récupérer et afficher le statut de l'erreur
            std::string msg(GceErrorToString(factory.Status()));
            throw Utils::BuildingException(TkUtil::UTF8String (msg, TkUtil::Charset::UTF_8));
        }
    } catch(StdFail_NotDone& e) {
        throw Utils::BuildingException(TkUtil::UTF8String ("Erreur durant la creation d'un cercle", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
Curve* EntityFactory::newEllipse(
        const Geom::Vertex* p1,
        const Geom::Vertex* p2,
        const Geom::Vertex* center)
{
    gp_Pnt P1(p1->getX(),p1->getY(),p1->getZ());
    gp_Pnt P2(p2->getX(),p2->getY(),p2->getZ());
    gp_Pnt Center(center->getX(),center->getY(),center->getZ());

    if (P1.IsEqual(P2,0.0)|| P1.IsEqual(Center,0.0)|| P2.IsEqual(Center,0.0))
      throw Utils::BuildingException(TkUtil::UTF8String ("Param. illicites - 2 pts (au moins) sont égaux", TkUtil::Charset::UTF_8));

    try {
        GC_MakeEllipse factory(P1,P2,Center);
        if (factory.IsDone()) {
            Handle(Geom_Ellipse) arc = factory.Value();
            TopoDS_Edge e = BRepBuilderAPI_MakeEdge(arc);
            Curve*	curve = new Curve(m_context,
                        m_context.newProperty(Utils::Entity::GeomCurve),
                        m_context.newDisplayProperties(Utils::Entity::GeomCurve),
                        new GeomProperty(), e);
            m_context.newGraphicalRepresentation (*curve);
            return curve;
        } else {
            // Récupérer et afficher le statut de l'erreur
            std::string msg(GceErrorToString(factory.Status()));
            throw Utils::BuildingException(TkUtil::UTF8String (msg, TkUtil::Charset::UTF_8));
        }
    } catch(StdFail_NotDone& e) {
        throw Utils::BuildingException(TkUtil::UTF8String ("Erreur durant la creation d'une ellipse", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
Curve* EntityFactory::newArcCircle(
        const Geom::Vertex* center,
        const Geom::Vertex* start,
        const Geom::Vertex* end,
        const bool direction,
        const Utils::Math::Vector& normal_in,
        const bool circumCircle)
{
#ifdef _DEBUG2
    std::cout<<"newArcCircle center "<<center->getPoint()<<" , start "<<start->getPoint()
			<<" , end "<<end->getPoint()<<" , direction "<<(direction?"true":"false")
			<<", normal_in "<<normal_in
			<<std::endl;
#endif
    gp_Pnt p_center(center->getX(),center->getY(),center->getZ());
    gp_Pnt p_start(start->getX(),start->getY(),start->getZ());
    gp_Pnt p_end(end->getX(),end->getY(),end->getZ());
    gp_Vec normal(normal_in.getX(),normal_in.getY(),normal_in.getZ());
    gp_Vec from_center_to_start(p_center, p_start);
    gp_Vec v_normal = buildValidNormalForArcs(p_center,p_start,p_end,normal);
#ifdef _DEBUG2
    std::cout<<"v_normal: ("<<v_normal.X()<<", "<<v_normal.Y()<<", "<<v_normal.Z()<<")"<<std::endl;
#endif

    //vecteur tangent à l'arc de cercle en PStart selon la direction choisie
    gp_Vec vecteurTangent;
    if(direction)
    	vecteurTangent = v_normal.Crossed(from_center_to_start);
    else
        vecteurTangent = from_center_to_start.Crossed(v_normal);

    try {
#ifdef _DEBUG2
    	std::cout<<"vecteurTangent "<<vecteurTangent.X()<<","<<vecteurTangent.Y()<<","<<vecteurTangent.Z()<<std::endl;
#endif
        Handle(Geom_TrimmedCurve) arc;
        if (circumCircle) {
             arc = GC_MakeArcOfCircle(p_start, p_center, p_end);
        } else {
             arc = GC_MakeArcOfCircle(p_start, vecteurTangent, p_end);
        }
        TopoDS_Edge e = BRepBuilderAPI_MakeEdge(arc);
        Curve* curve = new Curve(m_context,
                    m_context.newProperty(Utils::Entity::GeomCurve),
                    m_context.newDisplayProperties(Utils::Entity::GeomCurve),
                    new GeomProperty(), e);
        m_context.newGraphicalRepresentation (*curve);
        return curve;
    } catch(StdFail_NotDone& e) {
        throw Utils::BuildingException(TkUtil::UTF8String ("Erreur durant la creation d'un arc de cercle", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
Curve* EntityFactory::newArcCircle(
                const double& angleDep,
                const double& angleFin,
                const double& rayon,
                CoordinateSystem::SysCoord* syscoord)
{
    Utils::Math::Point center;
    Utils::Math::Point start(rayon*std::cos(angleDep*M_PI/180), rayon*std::sin(angleDep*M_PI/180), 0);
    Utils::Math::Point end(rayon*std::cos(angleFin*M_PI/180), rayon*std::sin(angleFin*M_PI/180), 0);
    Utils::Math::Point normal_in(0,0,1);

    if (syscoord){
        center = syscoord->toGlobal(center);
        start = syscoord->toGlobal(start);
        end = syscoord->toGlobal(end);
        normal_in = syscoord->toGlobal(normal_in);
        normal_in = normal_in - center;
    }

    gp_Pnt p_center(center.getX(),center.getY(),center.getZ());
    gp_Pnt p_start(start.getX(),start.getY(),start.getZ());
    gp_Pnt p_end(end.getX(),end.getY(),end.getZ());
    gp_Vec normal(normal_in.getX(),normal_in.getY(),normal_in.getZ());
    gp_Vec from_center_to_start(p_center, p_start);
    gp_Vec v_normal = buildValidNormalForArcs(p_center,p_start,p_end,normal);
#ifdef _DEBUG2
    std::cout<<"v_normal: ("<<v_normal.X()<<", "<<v_normal.Y()<<", "<<v_normal.Z()<<")"<<std::endl;
#endif

    //vecteur tangent à l'arc de cercle en PStart selon la direction choisie
    gp_Vec vecteurTangent = v_normal.Crossed(from_center_to_start);
    //gp_Vec vecteurTangent = from_center_to_start.Crossed(v_normal);
#ifdef _DEBUG2
    	std::cout<<"vecteurTangent "<<vecteurTangent.X()<<","<<vecteurTangent.Y()<<","<<vecteurTangent.Z()<<std::endl;
#endif

    try {
        Handle(Geom_TrimmedCurve) arc = GC_MakeArcOfCircle(p_start, vecteurTangent, p_end);
        TopoDS_Edge e = BRepBuilderAPI_MakeEdge(arc);
        Curve* curve = new Curve(m_context,
                                    m_context.newProperty(Utils::Entity::GeomCurve),
                                    m_context.newDisplayProperties(Utils::Entity::GeomCurve),
                                    new GeomProperty(), e);
        m_context.newGraphicalRepresentation (*curve);
        return curve;
    } catch(StdFail_NotDone& e) {
        throw Utils::BuildingException(TkUtil::UTF8String ("Erreur durant la creation d'un arc de cercle", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
Curve* EntityFactory::newArcCircle2D(
        const Geom::Vertex* center,
        const Geom::Vertex* start,
        const Geom::Vertex* end,
        const bool direction)
{
#ifdef _DEBUG2
    std::cout<<"newArcCircle2D center "<<center->getPoint()<<" , start "<<start->getPoint()
			<<" , end "<<end->getPoint()<<" , direction "<<(direction?"true":"false")
			<<std::endl;
#endif
    gp_Pnt p_center(center->getX(),center->getY(),center->getZ());
    gp_Pnt p_start(start->getX(),start->getY(),start->getZ());
    gp_Pnt p_end(end->getX(),end->getY(),end->getZ());
    gp_Vec from_center_to_start(p_center, p_start);
    gp_Vec v_normal(0,0,1);

    //vecteur tangent à l'arc de cercle en PStart selon la direction choisie
    gp_Vec vecteurTangent;
    if(direction)
    	vecteurTangent= v_normal.Crossed(from_center_to_start);
    else
        vecteurTangent= from_center_to_start.Crossed(v_normal);
#ifdef _DEBUG2
    std::cout<<"vecteurTangent "<<vecteurTangent.X()<<","<<vecteurTangent.Y()<<","<<vecteurTangent.Z()<<std::endl;
#endif

    try {
    	Handle(Geom_TrimmedCurve) arc = GC_MakeArcOfCircle(p_start, vecteurTangent, p_end);
        TopoDS_Edge e = BRepBuilderAPI_MakeEdge(arc);
        Curve* curve = new Curve(m_context,
                    m_context.newProperty(Utils::Entity::GeomCurve),
                    m_context.newDisplayProperties(Utils::Entity::GeomCurve),
                    new GeomProperty(), e);
        m_context.newGraphicalRepresentation (*curve);
        return curve;
    } catch(StdFail_NotDone& e) {
        throw Utils::BuildingException(TkUtil::UTF8String ("Erreur durant la creation d'un arc de cercle", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
Curve* EntityFactory::newArcEllipse(const Geom::Vertex* center,
    const Geom::Vertex* start, const Geom::Vertex* end,
    const bool direction)
{
#ifdef _DEBUG2
    std::cout<<"newArcEllipse center "<<center->getPoint()<<" , start "<<start->getPoint()
			<<" , end "<<end->getPoint()<<" , direction "<<(direction?"true":"false")<<std::endl;
#endif
    gp_Pnt p_center(center->getX(),center->getY(),center->getZ());
    gp_Pnt p_start(start->getX(),start->getY(),start->getZ());
    gp_Pnt p_end(end->getX(),end->getY(),end->getZ());

    // Vecteurs depuis le centre
    gp_Vec vStart(p_center, p_start);
    gp_Vec vEnd(p_center, p_end);

    if (vStart.Magnitude() < Precision::Confusion())
        throw Utils::BuildingException(TkUtil::UTF8String("Création de l'arc impossible car centre et point de départ sont confondus", TkUtil::Charset::UTF_8));

    // Plan de l’ellipse
    gp_Vec normal = vStart.Crossed(vEnd);
    if (normal.Magnitude() < Precision::Confusion())
        throw Utils::BuildingException(TkUtil::UTF8String("Création de l'arc impossible car les points sont colinéaires", TkUtil::Charset::UTF_8));

    gp_Dir normalDir(normal);
    gp_Dir xDir(vStart);                 // grand axe imposé
    gp_Dir yDir(normalDir.Crossed(xDir));

    // Grand rayon
    Standard_Real a = vStart.Magnitude();

    // Coordonnées de vEnd dans le repère
    Standard_Real x = vEnd.Dot(gp_Vec(xDir));
    Standard_Real y = vEnd.Dot(gp_Vec(yDir));

    // Condition d’existence
    if (Abs(x) >= a)
        throw Utils::BuildingException(TkUtil::UTF8String("Impossible de faire passer l'arc par le point d'arrivée", TkUtil::Charset::UTF_8));

    // Petit rayon (résolution analytique)
    Standard_Real b = Sqrt( (y * y) / (1.0 - (x * x) / (a * a)) );

    if (b < Precision::Confusion())
        throw Utils::BuildingException(TkUtil::UTF8String("Création de l'arc impossible car le petit rayon est trop petit", TkUtil::Charset::UTF_8));

    Standard_Real major = a;
    Standard_Real minor = b;
    gp_Dir majorDir = xDir;
    gp_Dir minorDir = yDir;

    if (b > a)
    {
        major = b;
        minor = a;
        majorDir = yDir;
        minorDir = xDir;
    }

    // Repère de l’ellipse
    gp_Ax2 ellipseAxis(p_center, normalDir, majorDir);
    // Construction de l’ellipse
    Handle(Geom_Ellipse) ellipse = new Geom_Ellipse(ellipseAxis, major, minor);

    // Paramètres angulaires EXACTS
    Standard_Real u1 = ElCLib::EllipseParameter(ellipseAxis, major, minor, p_start);
    Standard_Real u2 = ElCLib::EllipseParameter(ellipseAxis, major, minor, p_end);

    // Arc d’ellipse
    Handle(Geom_TrimmedCurve) arc = new Geom_TrimmedCurve(ellipse, u1, u2, Standard_True);

    TopoDS_Edge e = BRepBuilderAPI_MakeEdge(arc);
    Curve* curve = new Curve(m_context,
                     m_context.newProperty(Utils::Entity::GeomCurve),
                     m_context.newDisplayProperties(Utils::Entity::GeomCurve),
                     new GeomProperty(), e);
    m_context.newGraphicalRepresentation (*curve);
    return curve;
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_BSPLINE
Curve* EntityFactory::newBSpline(const std::vector<Utils::Math::Point>& points, int degMin, int degMax)
{
#ifdef _DEBUG_BSPLINE
	std::cout<<"EntityFactory::newBSpline avec "<<points.size()<<" points"<<std::endl;
#endif
    //gmds::Timer t1;
    // construction d'un tableau avec les points
    // ATTENTION : si deux points consécutifs ont les memes coords, on n'en
    // considère qu'un seul.

    // calcul du nombre de point pour la b-spline (on elimine les points confondus)
    int nb_points=0;

    for(int i=0; i<points.size()-1;++i)
    {
        gp_Pnt  aP1(points[i].getX(),points[i].getY(),points[i].getZ());
        gp_Pnt  aP2(points[i+1].getX(),points[i+1].getY(),points[i+1].getZ());

        if (!aP1.IsEqual(aP2,1e-12))
           nb_points++;

    }
    //le dernier point de la courbe doit etre ajoute
    nb_points++;
#ifdef _DEBUG_BSPLINE
    std::cout<<"aTabPt de taille "<<nb_points<<std::endl;
#endif

 TColgp_Array1OfPnt aTabPt(0,nb_points-1);
//    Handle(TColgp_HArray1OfPnt) tabPnts = new TColgp_HArray1OfPnt(0,nb_points-1);
    int j = 0;
    for(int i=0; i<points.size();++i)
    {
        gp_Pnt  aP1(points[i].getX(),points[i].getY(),points[i].getZ());
        if(i!=points.size()-1)
        {
            gp_Pnt  aP2(points[i+1].getX(),points[i+1].getY(),points[i+1].getZ());
            if (!aP1.IsEqual(aP2,1e-12))
                 aTabPt(j++) = aP1;
#ifdef _DEBUG_BSPLINE
            std::cout<<" "<<points[i]<<std::endl;
#endif
       }
        else {
          aTabPt(j++) = aP1;//  tabPnts->SetValue(j++,aP1);
#ifdef _DEBUG_BSPLINE
          std::cout<<" "<<points[i]<<std::endl;
#endif
        }
   }

    try {
        //gmds::Timer t2;

        // construit une courbe BSpline à partir du tableau
    	// [EB] pb avec Tol3D à 1e-4 pour cas Rémy d'un contour de gaineIni... passe bien avec 1e-3 (qui est la valeur par défaut dans OCC)
    	//      pb avec degré max sur cas d'Eric A. d'ou restriction entre 1 et 2 => degMin,degMax
        GeomAPI_PointsToBSpline mkBSpline(aTabPt,degMin,degMax,GeomAbs_C1, 1e-3);
        //GeomAPI_Interpolate mkBSpline(tabPnts,false,0.01);
        //std::cerr<<"Apres le constructeur"<<std::endl;
        // récupération du pointeur sur la courbe

        //std::cerr<<"Apres le perform() "<<mkBSpline.IsDone()<<std::endl;
        Handle(Geom_BSplineCurve) bspline = mkBSpline.Curve();
        //gmds::Timer t3;

        //std::cerr<<"\t GeomAPI_MakeSpline          = "<<t3-t2<<std::endl;
        // construction de la structure associée à la courbe
        TopoDS_Edge e = BRepBuilderAPI_MakeEdge(bspline);
        //gmds::Timer t4;

        Curve* curve = new Curve(m_context,
                    m_context.newProperty(Utils::Entity::GeomCurve),
                    m_context.newDisplayProperties(Utils::Entity::GeomCurve),
                    new GeomProperty(), e);
        m_context.newGraphicalRepresentation (*curve);

        //gmds::Timer t6;
        //std::cerr<<"GLOBAL time = "<<t6-t1<<std::endl;
        return curve;
        //gmds::Timer t5;
        //std::cerr<<"\t BRepBuilderAPI_MakeEdge     = "<<t4-t3<<std::endl;

    } catch(StdFail_NotDone& e) {
        throw Utils::BuildingException(TkUtil::UTF8String ("Erreur durant la creation d'une bspline", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
bool EntityFactory::checkClosedWire(const std::vector<Geom::Curve*>& curves) const
{
    std::set<Utils::Entity::uniqueID> vertex_ids;
    if (curves.size() == 1){
    	auto v = curves[0]->getVertices();
    	return ((v.size() == 2 && (v[0]->getCoord() == v[1]->getCoord())) || v.size() == 1);
    }
    for(unsigned int i=0; i<curves.size();i++){
        auto v = curves[i]->getVertices();
        if(v.size()>=1){
            vertex_ids.insert(v[0]->getUniqueId());
            if(v.size()==2)
                vertex_ids.insert(v[1]->getUniqueId());
        }
        else
            throw Utils::BuildingException("Error a curve has no end points");
    }
    return (vertex_ids.size()==curves.size());

}
/*----------------------------------------------------------------------------*/
Surface* EntityFactory::newSurface(const std::vector<Geom::Curve*>& curves)
{
    if(!checkClosedWire(curves)){
        throw Utils::BuildingException("Error during surface creation: no closed loop");
    }
    try{
        /* on récupère les courbes OCC sous-jacentes à chaque courbes et on
         * crée un wire
         */
        BRepBuilderAPI_MakeWire mk_wire;
        for (Geom::Curve* current_curve : curves) {
            for (auto sh : current_curve->getOCCEdges()) {
                mk_wire.Add(sh);
            }
        }
        mk_wire.Build();
        TopoDS_Wire w = mk_wire.Wire();

        /* A partir des points des courbes, on cree un plan support de la face.
         * Si on a plus de 3 points, on vérifie la coplanarité. Si elle n'est
         * pas là, on lance une exception.
         */
        std::set<Geom::Vertex*> vertices;
        for(unsigned int i=0;i<curves.size();i++){
            Geom::Curve* current_curve = curves[i];
            auto current_vertices = current_curve->getVertices();
            vertices.insert(current_vertices.begin(),current_vertices.end());
        }

        std::vector<Utils::Math::Point> vec_vertices;
        std::set<Geom::Vertex*>::iterator it_v = vertices.begin();
        while(it_v!=vertices.end()){
            Geom::Vertex* cv = *it_v;
            vec_vertices.push_back(Utils::Math::Point(cv->getX(),cv->getY(),cv->getZ()));
            it_v++;
        }

        // par defaut, on rajoute dans les points à considérer, le milieu de toutes les courbes
        // qui ne sont pas des segments. Cela évite entre autres des problèmes avec les
        // demi-cercles.
        // => 1/3 et 2/3 ajouté pour cas du cercle ... [EB]
        for(unsigned int i=0;i<curves.size();i++){
            Geom::Curve* curve = curves[i];
            if(!curve->isLinear()){
                Utils::Math::Point pnew;
                curve->getPoint(0.3,pnew,true);
                vec_vertices.push_back(pnew);
                curve->getPoint(0.7,pnew,true);
                vec_vertices.push_back(pnew);
            }
        }


        gp_Pln surface_plane;
        if(vec_vertices.size()<3){
            // Au moins l'une des deux courbes n'est pas un segment
                throw Utils::BuildingException(TkUtil::UTF8String ("Creation d'une surface plane réduite à un segment", TkUtil::Charset::UTF_8));
        }
        else
        {
            Utils::Math::Point v0 = vec_vertices[0];
            Utils::Math::Point v1 = vec_vertices[1];
            gp_Pnt p0(v0.getX(),v0.getY(),v0.getZ());
            gp_Pnt p1(v1.getX(),v1.getY(),v1.getZ());

            gp_Vec v01(p0,p1);
            int pnt_index=2;
            gp_Vec plane_normal;
            bool find_third_point = false;
            while(!find_third_point && pnt_index<vec_vertices.size()){
                Utils::Math::Point v2 = vec_vertices[pnt_index];
                gp_Pnt p2(v2.getX(),v2.getY(),v2.getZ());
                gp_Vec v02(p0,p2);

                plane_normal= v01.Crossed(v02);
                if(plane_normal.Magnitude()==0) //vecteur nul problème
                  pnt_index++;
                else
                    find_third_point=true;
            }
            if(!find_third_point)
                throw Utils::BuildingException(TkUtil::UTF8String ("Creation d'une surface plane (curves): Les sommets sont colinéaires", TkUtil::Charset::UTF_8));

            surface_plane = gp_Pln(p0,gp_Dir(plane_normal));

            if(vec_vertices.size()>3){
                /* on regarde si les autres sommets appartiennent au plan que l'on vient
                 * de definir. Sinon, on renvoit une exception.
                 */
                for(unsigned int j=3;j<vec_vertices.size();j++){
                    Utils::Math::Point v = vec_vertices[j];
                    gp_Pnt p(v.getX(),v.getY(),v.getZ());
                    if(surface_plane.SquareDistance(p)>Utils::Math::MgxNumeric::mgxDoubleEpsilon)
                        throw Utils::BuildingException(TkUtil::UTF8String ("Creation d'une surface (plane): Les sommets ne sont pas coplanaires", TkUtil::Charset::UTF_8));
                }
            }
        }

        TopoDS_Face aFace;
        BRepBuilderAPI_MakeFace MF(surface_plane,w,true);


        if(MF.Error()==BRepBuilderAPI_FaceDone){
            aFace = MF.Face();
            Surface* surface = new Surface(m_context,
                        m_context.newProperty(Utils::Entity::GeomSurface),
                        m_context.newDisplayProperties(Utils::Entity::GeomSurface),
                        new GeomProperty(), aFace);
            m_context.newGraphicalRepresentation (*surface);
            return surface;
        }
        else {
            throw Utils::BuildingException("Error during surface creation (error...)");
        }
    } catch(StdFail_NotDone& e) {
        throw Utils::BuildingException("Error during surface creation (not done)");
    } catch(Utils::BuildingException& e) {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message << "Error during surface creation, "<<e.getMessage();
        throw Utils::BuildingException(message);
    }
}
/*----------------------------------------------------------------------------*/
Volume* EntityFactory::newBox(PropertyBox* prop)
{
    Utils::Math::Point pmin = prop->getPnt();
    gp_Pnt A(pmin.getX(),pmin.getY(),pmin.getZ());

    //BRepPrimAPI_MakeBox mkBox(axis,prop->getX(),prop->getY(),prop->getZ());
    BRepPrimAPI_MakeBox mkBox(A, prop->getX(),prop->getY(),prop->getZ());

    try{
        TopoDS_Shape s = mkBox.Solid();
        Volume*	volume	= new Volume(m_context,
                m_context.newProperty(Utils::Entity::GeomVolume),
                m_context.newDisplayProperties(Utils::Entity::GeomVolume),
                prop, s);
        CHECK_NULL_PTR_ERROR (volume)
        m_context.newGraphicalRepresentation (*volume);
        return volume;
    }catch(StdFail_NotDone& e){
        throw Utils::BuildingException("Error during Box creation");
    }
}
/*----------------------------------------------------------------------------*/
void analyse(TopoDS_Shape& s)
{
     ShapeAnalysis_ShapeContents cont;

     cont.Clear();
     cont.Perform(s);
     std::cout<<"vertices: "<<cont.NbVertices()<<std::endl;
     std::cout<<"edges: "<<cont.NbEdges()<<std::endl;
     std::cout<<"faces: "<<cont.NbFaces()<<std::endl;
     std::cout<<"solids:"<<cont.NbSolids()<<std::endl;
     std::cout<<"solids void: "<<cont.NbSolidsWithVoids()<<std::endl;
     std::cout<<"solids shared: "<<cont.NbSharedSolids()<<std::endl;
}
/*----------------------------------------------------------------------------*/
Volume* EntityFactory::newCylinder(PropertyCylinder* prop)
{
    Volume* result_entity=0;

    Utils::Math::Point pcenter = prop->getCenter();
    gp_Pnt A(pcenter.getX(),pcenter.getY(),pcenter.getZ());

    Utils::Math::Vector paxis = prop->getAxis();
    gp_Dir Vx(paxis.getX(),paxis.getY(),paxis.getZ());

    gp_Ax2 axis(A,Vx);
    double angle = prop->getAngle();
    BRepPrimAPI_MakeCylinder mkCyl(axis, prop->getRadius(),
                                   prop->getHeight(),angle*M_PI/180.0);
    try{
        TopoDS_Shape s = mkCyl.Solid();
        Volume* volume  =  new Volume(m_context,
                m_context.newProperty(Utils::Entity::GeomVolume),
                m_context.newDisplayProperties(Utils::Entity::GeomVolume),
                prop, s);
        CHECK_NULL_PTR_ERROR (volume)
        m_context.newGraphicalRepresentation (*volume);
        return volume;
    }catch(StdFail_NotDone& e){
        throw Utils::BuildingException(TkUtil::UTF8String ("Erreur à la création d'un cylindre", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
Volume* EntityFactory::newHollowCylinder(PropertyHollowCylinder* prop)
{
    Utils::Math::Point pcenter = prop->getCenter();
    gp_Pnt A(pcenter.getX(),pcenter.getY(),pcenter.getZ());

    Utils::Math::Vector paxis = prop->getAxis();
    gp_Dir Vx(paxis.getX(),paxis.getY(),paxis.getZ());

    gp_Ax2 axis(A,Vx);
    double angle = prop->getAngle();

    BRepPrimAPI_MakeCylinder mkCylInt(axis, prop->getRadiusInt(),
                                   prop->getHeight(),angle*M_PI/180.0);
    BRepPrimAPI_MakeCylinder mkCylExt(axis, prop->getRadiusExt(),
                                   prop->getHeight(),angle*M_PI/180.0);
    try{
        TopoDS_Shape s_int = mkCylInt.Solid();
        TopoDS_Shape s_ext = mkCylExt.Solid();

        BRepAlgoAPI_Cut Cut_operator(s_ext, s_int);

        if(Cut_operator.IsDone())
        {
            TopoDS_Shape s = Cut_operator.Shape();
            ShapeAnalysis_ShapeContents cont;
            cont.Clear();
            cont.Perform(s);
            if (cont.NbSolids()<1){
                throw TkUtil::Exception(TkUtil::UTF8String ("HollowCylinder: Problème OCC lors de la différence", TkUtil::Charset::UTF_8));
            }
            else
            {
                TopTools_IndexedMapOfShape mapS;
                TopExp::MapShapes(s,TopAbs_SOLID, mapS);
                if(mapS.Extent()>1)
                    throw TkUtil::Exception(TkUtil::UTF8String ("HollowSphere: Problème OCC lors de la différence (plus de 1 solide)", TkUtil::Charset::UTF_8));

                TopoDS_Shape solid = TopoDS::Solid(mapS.FindKey(1));
                Volume* volume  =  new Volume(m_context,
                        m_context.newProperty(Utils::Entity::GeomVolume),
                        m_context.newDisplayProperties(Utils::Entity::GeomVolume),
                        prop, solid);
                CHECK_NULL_PTR_ERROR (volume)
                m_context.newGraphicalRepresentation (*volume);
                return volume;
            }

        }
        else
            throw TkUtil::Exception(TkUtil::UTF8String ("HollowCylinder: Problème OCC lors de la différence", TkUtil::Charset::UTF_8));
    }
    catch(StdFail_NotDone& e){
        throw Utils::BuildingException(TkUtil::UTF8String ("Erreur à la création d'un cylindre creux", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
Volume* EntityFactory::newCone(PropertyCone* prop)
{
    gp_Pnt A(0,0,0);

    Utils::Math::Vector paxis = prop->getAxis();
    gp_Dir Vx(paxis.getX(),paxis.getY(),paxis.getZ());

    gp_Ax2 axis(A,Vx);
    double angle = prop->getAngle();
    BRepPrimAPI_MakeCone mkCone(axis, prop->getRadius1(), prop->getRadius2(),
                                   prop->getHeight(), angle*M_PI/180.0);
    try{
        TopoDS_Shape s = mkCone.Solid();
        Volume*	volume	=  new Volume(m_context,
                m_context.newProperty(Utils::Entity::GeomVolume),
                m_context.newDisplayProperties(Utils::Entity::GeomVolume),
                prop, s);
        CHECK_NULL_PTR_ERROR (volume)
        m_context.newGraphicalRepresentation (*volume);
        return volume;
    }catch(StdFail_NotDone& e){
        throw Utils::BuildingException(TkUtil::UTF8String ("Erreur à la création d'un cône", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
Volume* EntityFactory::newSphere(PropertySphere* prop)
{
    Utils::Math::Point pcenter = prop->getCenter();
    gp_Pnt center(pcenter.getX(),pcenter.getY(),pcenter.getZ());
    double radius= prop->getRadius();
    double angle = prop->getAngle();
    Utils::Portion::Type type = prop->getPortionType();

    TopoDS_Solid s;
    switch(type){
    case (Utils::Portion::ENTIER):{
        BRepPrimAPI_MakeSphere mkSphere(center,radius);
        s = mkSphere.Solid();
    }
    break;
    case (Utils::Portion::TROIS_QUARTS):{
        BRepPrimAPI_MakeSphere mkSphere(center,radius,3*M_PI/4);
        s = mkSphere.Solid();
    }
    break;
    case (Utils::Portion::DEMI):{
        BRepPrimAPI_MakeSphere mkSphere(center,radius,M_PI);
        s = mkSphere.Solid();
    }
    break;
    case (Utils::Portion::QUART):{
        BRepPrimAPI_MakeSphere mkSphere(center,radius,M_PI/2);
        s = mkSphere.Solid();
    }
    break;
    case (Utils::Portion::HUITIEME):{
        BRepPrimAPI_MakeSphere mkSphere(center,radius,0,M_PI/2,M_PI/2);
        s = mkSphere.Solid();
    }
    break;
    case (Utils::Portion::ANGLE_DEF):{
        // l'angle est supposé en degré et non pas en radians
        BRepPrimAPI_MakeSphere mkSphere(center,radius,(angle*M_PI)/180.0);
        s = mkSphere.Solid();
    }
    break;
    }


    try{
        Volume* volume  = new Volume(m_context,
                m_context.newProperty(Utils::Entity::GeomVolume),
                m_context.newDisplayProperties(Utils::Entity::GeomVolume),
                prop, s);
        CHECK_NULL_PTR_ERROR (volume)
        m_context.newGraphicalRepresentation (*volume);
        return volume;
    }catch(StdFail_NotDone& e){
        throw Utils::BuildingException(TkUtil::UTF8String ("Erreur à la création d'une sphère", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
Volume* EntityFactory::newSpherePart(PropertySpherePart* prop)
{
	double radius = prop->getRadius();
	double angleY = prop->getAngleY()*M_PI/180;
	BRepPrimAPI_MakeSphere mkSphere(radius,angleY);
	TopoDS_Shape shape = mkSphere.Shape();

	// on tourne la portion de sphère
	gp_Trsf T;
	gp_Pnt p1(0,0,0);
	gp_Dir dir(0,0,1);
	gp_Ax1 axis(p1,dir);
	T.SetRotation(axis,-angleY/2);
	BRepBuilderAPI_Transform rotat(T);
	rotat.Perform(shape);
	shape = rotat.Shape();


	// première coupe avec un plan
	{
		double angleZ = prop->getAngleZ()*M_PI/180;
		gp_Pln gp_plane(gp_Pnt(0,0,0), gp_Dir(-sin(angleZ/2), 0, cos(angleZ/2)));
		gp_Pnt pnt_to_eliminate(0,0,radius);
		shape = getShapeAfterPlaneCut(shape, gp_plane, pnt_to_eliminate);
	}

	// deuxième coupe avec un plan
	{
		double angleZ = prop->getAngleZ()*M_PI/180;
		gp_Pln gp_plane(gp_Pnt(0,0,0), gp_Dir(sin(angleZ/2), 0, cos(angleZ/2)));
		gp_Pnt pnt_to_eliminate(0,0,-radius);
		shape = getShapeAfterPlaneCut(shape, gp_plane, pnt_to_eliminate);
	}

	try{
        Volume* volume  = new Volume(m_context,
                m_context.newProperty(Utils::Entity::GeomVolume),
                m_context.newDisplayProperties(Utils::Entity::GeomVolume),
                prop, shape);
        CHECK_NULL_PTR_ERROR (volume)
        m_context.newGraphicalRepresentation (*volume);
        return volume;
	}catch(StdFail_NotDone& e){
		throw Utils::BuildingException(TkUtil::UTF8String ("Erreur à la création d'une aiguille", TkUtil::Charset::UTF_8));
	}
}
/*----------------------------------------------------------------------------*/
Volume* EntityFactory::newHollowSpherePart(PropertyHollowSpherePart* prop)
{
	double radius = prop->getRadiusExt();
	double angleY = prop->getAngleY()*M_PI/180;
	BRepPrimAPI_MakeSphere mkSphere(radius,angleY);
	TopoDS_Shape shape = mkSphere.Shape();

	// on tourne la portion de sphère
	gp_Trsf T;
	gp_Pnt p1(0,0,0);
	gp_Dir dir(0,0,1);
	gp_Ax1 axis(p1,dir);
	T.SetRotation(axis,-angleY/2);
	BRepBuilderAPI_Transform rotat(T);
	rotat.Perform(shape);
	shape = rotat.Shape();


	// première coupe avec un plan
	{
		double angleZ = prop->getAngleZ()*M_PI/180;
		gp_Pln gp_plane(gp_Pnt(0,0,0), gp_Dir(-sin(angleZ/2), 0, cos(angleZ/2)));
		gp_Pnt pnt_to_eliminate(0,0,radius);
		shape = getShapeAfterPlaneCut(shape, gp_plane, pnt_to_eliminate);
	}

	// deuxième coupe avec un plan
	{
		double angleZ = prop->getAngleZ()*M_PI/180;
		gp_Pln gp_plane(gp_Pnt(0,0,0), gp_Dir(sin(angleZ/2), 0, cos(angleZ/2)));
		gp_Pnt pnt_to_eliminate(0,0,-radius);
		shape = getShapeAfterPlaneCut(shape, gp_plane, pnt_to_eliminate);
	}

	// la sphère interne à retirer
	radius = prop->getRadiusInt();
	BRepPrimAPI_MakeSphere mkSphereInt(radius,angleY);
	TopoDS_Shape shapeInt = mkSphereInt.Shape();
	rotat.Perform(shapeInt);
	shapeInt = rotat.Shape();

    BRepAlgoAPI_Cut Cut_operator(shape, shapeInt);

    if(Cut_operator.IsDone())
    {
        shape = Cut_operator.Shape();
        ShapeAnalysis_ShapeContents cont;
        cont.Clear();
        cont.Perform(shape);
        if (cont.NbSolids()<1){
            throw TkUtil::Exception(TkUtil::UTF8String ("newHollowSpherePart: Problème OCC lors de la différence", TkUtil::Charset::UTF_8));
        }
        else {
            TopTools_IndexedMapOfShape mapS;
            TopExp::MapShapes(shape,TopAbs_SOLID, mapS);
            if(mapS.Extent()>1)
                throw TkUtil::Exception(TkUtil::UTF8String ("newHollowSpherePart: Problème OCC lors de la différence (plus de 1 solide)", TkUtil::Charset::UTF_8));

            shape = mapS.FindKey(1);
        }
    }

	try{
        Volume* volume  = new Volume(m_context,
                m_context.newProperty(Utils::Entity::GeomVolume),
                m_context.newDisplayProperties(Utils::Entity::GeomVolume),
                prop, shape);
        CHECK_NULL_PTR_ERROR (volume)
        m_context.newGraphicalRepresentation (*volume);
        return volume;
	}catch(StdFail_NotDone& e){
		throw Utils::BuildingException(TkUtil::UTF8String ("Erreur à la création d'une aiguille", TkUtil::Charset::UTF_8));
	}
}
/*----------------------------------------------------------------------------*/
TopoDS_Shape EntityFactory::getShapeAfterPlaneCut(TopoDS_Shape shape, gp_Pln gp_plane, gp_Pnt pnt_to_eliminate)
{
	BRepBuilderAPI_MakeFace mkF(gp_plane);
	TopoDS_Face wf = mkF.Face();

	BRepAlgoAPI_BuilderAlgo splitter;
	TopTools_ListOfShape list_of_arguments;
	list_of_arguments.Append(shape);
	list_of_arguments.Append(wf);
	splitter.SetArguments(list_of_arguments);
	splitter.Build();
	shape = splitter.Shape();

	ShapeAnalysis_ShapeContents cont;
	cont.Clear();
	cont.Perform(shape);
	if (cont.NbSolids()<1){
		std::cerr<<"NbSolids = "<<cont.NbSolids()<<std::endl;
		throw TkUtil::Exception(TkUtil::UTF8String ("newSpherePart: Problème OCC lors du cut", TkUtil::Charset::UTF_8));
	}
	else {
		// on ne prend pas le volume qui a un sommet en pnt_to_eliminate
		bool found = false;
		TopTools_IndexedMapOfShape mapS;
		TopExp::MapShapes(shape,TopAbs_SOLID, mapS);
		for (uint i=1; false == found && i<=cont.NbSolids(); i++){
			shape = mapS.FindKey(i);
			// parcours des sommets
			TopExp_Explorer e;
			bool vol_ok = true;
			for(e.Init(shape, TopAbs_VERTEX); e.More(); e.Next()){
				TopoDS_Vertex V = TopoDS::Vertex(e.Current());
				gp_Pnt pnt = BRep_Tool::Pnt(V);
				if (pnt.Distance(pnt_to_eliminate)<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)
					vol_ok = false;
			}
			if (vol_ok)
				found = true;
		}
	}
	return shape;
}
/*----------------------------------------------------------------------------*/
Volume* EntityFactory::newHollowSphere(PropertyHollowSphere* prop)
{
    Utils::Math::Point pcenter = prop->getCenter();
    gp_Pnt center(pcenter.getX(),pcenter.getY(),pcenter.getZ());
    double radius_int= prop->getRadiusInt();
    double radius_ext= prop->getRadiusExt();
    double angle = prop->getAngle();
    Utils::Portion::Type type = prop->getPortionType();

    TopoDS_Solid s_int, s_ext;
    switch(type){
    case (Utils::Portion::ENTIER):{

        BRepPrimAPI_MakeSphere mkSphereInt(center,radius_int);
        BRepPrimAPI_MakeSphere mkSphereExt(center,radius_ext);
        s_int = mkSphereInt.Solid();
        s_ext = mkSphereExt.Solid();
    }
    break;
    case (Utils::Portion::TROIS_QUARTS):{

        BRepPrimAPI_MakeSphere mkSphereInt(center,radius_int,3*M_PI/4);
        BRepPrimAPI_MakeSphere mkSphereExt(center,radius_ext,3*M_PI/4);
        s_int = mkSphereInt.Solid();
        s_ext = mkSphereExt.Solid();
    }
    break;
    case (Utils::Portion::DEMI):{
        BRepPrimAPI_MakeSphere mkSphereInt(center,radius_int,M_PI);
        BRepPrimAPI_MakeSphere mkSphereExt(center,radius_ext,M_PI);
        s_int = mkSphereInt.Solid();
        s_ext = mkSphereExt.Solid();
    }
    break;
    case (Utils::Portion::QUART):{
        BRepPrimAPI_MakeSphere mkSphereInt(center,radius_int,M_PI/2);
        BRepPrimAPI_MakeSphere mkSphereExt(center,radius_ext,M_PI/2);
        s_int = mkSphereInt.Solid();
        s_ext = mkSphereExt.Solid();
    }
    break;
    case (Utils::Portion::HUITIEME):{
        BRepPrimAPI_MakeSphere mkSphereInt(center,radius_int,0,M_PI/2,M_PI/2);
        BRepPrimAPI_MakeSphere mkSphereExt(center,radius_ext,0,M_PI/2,M_PI/2);
        s_int = mkSphereInt.Solid();
        s_ext = mkSphereExt.Solid();
    }
    break;
    case (Utils::Portion::ANGLE_DEF):{
        // l'angle est supposé en degré et non pas en radians
        BRepPrimAPI_MakeSphere mkSphereInt(center,radius_int,(angle*M_PI)/180.0);
        BRepPrimAPI_MakeSphere mkSphereExt(center,radius_ext,(angle*M_PI)/180.0);
        s_int = mkSphereInt.Solid();
        s_ext = mkSphereExt.Solid();
    }
    break;
    }

    BRepAlgoAPI_Cut Cut_operator(s_ext, s_int);
    if(Cut_operator.IsDone())
    {
        TopoDS_Shape sh = Cut_operator.Shape();
        ShapeAnalysis_ShapeContents cont;
        cont.Clear();
        cont.Perform(sh);
        if (cont.NbSolids()<1){
            throw TkUtil::Exception(TkUtil::UTF8String ("HollowSphere: Problème OCC lors de la différence", TkUtil::Charset::UTF_8));
        } else {
            TopTools_IndexedMapOfShape mapS;
            TopExp::MapShapes(sh,TopAbs_SOLID, mapS);
            if(mapS.Extent()>1)
                throw TkUtil::Exception(TkUtil::UTF8String ("HollowSphere: Problème OCC lors de la différence (plus de 1 solide)", TkUtil::Charset::UTF_8));

            TopoDS_Shape solid = TopoDS::Solid(mapS.FindKey(1));
            Volume* volume  = new Volume(m_context,
                    m_context.newProperty(Utils::Entity::GeomVolume),
                    m_context.newDisplayProperties(Utils::Entity::GeomVolume),
                    prop, solid);
            CHECK_NULL_PTR_ERROR (volume)
            m_context.newGraphicalRepresentation (*volume);
            return volume;
        }
    }
    else
        throw TkUtil::Exception(TkUtil::UTF8String ("HollowSphere: Problème OCC lors de la différence", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Volume* EntityFactory::newOCCVolume(TopoDS_Solid& s)
{
	return newOCCShape(s);
}
/*----------------------------------------------------------------------------*/
Volume* EntityFactory::newOCCVolume(TopoDS_Shell& s)
{
	return newOCCShape(s);
}
/*----------------------------------------------------------------------------*/
Volume* EntityFactory::newOCCShape(TopoDS_Shape& s)
{
    Volume*	volume	= new Volume(m_context,
            m_context.newProperty(Utils::Entity::GeomVolume),
            m_context.newDisplayProperties(Utils::Entity::GeomVolume),
            new GeomProperty(), s);
	CHECK_NULL_PTR_ERROR (volume)
	m_context.newGraphicalRepresentation (*volume);
	return volume;
}
/*----------------------------------------------------------------------------*/
Surface* EntityFactory::newOCCSurface(TopoDS_Face& f)
{
   Surface*	surface	= new Surface(m_context,
            m_context.newProperty(Utils::Entity::GeomSurface),
            m_context.newDisplayProperties(Utils::Entity::GeomSurface),
            new GeomProperty(), f);
	CHECK_NULL_PTR_ERROR (surface)
	m_context.newGraphicalRepresentation (*surface);
	return surface;
}
/*----------------------------------------------------------------------------*/
Surface* EntityFactory::newOCCCompositeSurface(std::vector<TopoDS_Face>& v_ds_face)
{
    std::vector<TopoDS_Face> shapes;
    for (auto f : v_ds_face) shapes.push_back(f);
	Surface* surface = new Surface(m_context,
			m_context.newProperty(Utils::Entity::GeomSurface),
			m_context.newDisplayProperties(Utils::Entity::GeomSurface),
			new GeomProperty(),
			shapes);
	CHECK_NULL_PTR_ERROR (surface)
	m_context.newGraphicalRepresentation (*surface);
	return surface;
}
/*----------------------------------------------------------------------------*/
Curve* EntityFactory::newOCCCurve(TopoDS_Edge& e)
{
    Curve* curve = new Curve(m_context,
            m_context.newProperty(Utils::Entity::GeomCurve),
            m_context.newDisplayProperties(Utils::Entity::GeomCurve),
            new GeomProperty(), e);
    CHECK_NULL_PTR_ERROR (curve)
    m_context.newGraphicalRepresentation (*curve);
    return curve;
}
/*----------------------------------------------------------------------------*/
Curve* EntityFactory::newOCCCompositeCurve(std::vector<TopoDS_Edge>& v_ds_edge,
		Utils::Math::Point& extremaFirst, Utils::Math::Point& extremaLast)
{
//#define _DEBUG_COMPOSITE
#ifdef _DEBUG_COMPOSITE
	std::cout<<"EntityFactory::newOCCCompositeCurve entre "<<extremaFirst<<" et "<<extremaLast<<" avec "<<v_ds_edge.size()<<" TopoDS_Edge: "<<std::endl;
	for (std::vector<TopoDS_Edge>::iterator iter=v_ds_edge.begin(); iter!=v_ds_edge.end(); ++iter){
		TopoDS_Edge ds_edge = *iter;
		Utils::Math::Point extrema1, extrema2;
		gp_Pnt res;
		BRepAdaptor_Curve brepCurve(ds_edge);
		res = brepCurve.Value(brepCurve.FirstParameter());
		extrema1.setXYZ(res.X(), res.Y(), res.Z());
		res = brepCurve.Value(brepCurve.LastParameter());
		extrema2.setXYZ(res.X(), res.Y(), res.Z());
		std::cout<<"  TopoDS_Edge entre  "<<extrema1<<" et "<<extrema2<<std::endl;
	}
#endif

	std::vector<TopoDS_Edge> reps;

	// certains TopoDS_Edge issus de la projection peuvent être des parasites (projection sur surface lointaine)
	// Nous allons donc partir extremaFirst et de proche en proche aller jusqu'à extremaLast
	// sans forcément prendre tous les éléments de v_ds_edge
	double epsilon = extremaFirst.length(extremaLast)*Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon*10;
#ifdef _DEBUG_COMPOSITE
	std::cout<<"epsilon "<<epsilon<<std::endl;
#endif
	Utils::Math::Point ptPrec = extremaFirst;
	do {
		// recherche d'un TopoDS_Edge relié à ptPrec
		std::vector<TopoDS_Edge>::iterator iter_to_erase;
		bool found = false;
		TopoDS_Edge ds_edge_found;
		for (std::vector<TopoDS_Edge>::iterator iter=v_ds_edge.begin(); iter!=v_ds_edge.end() && !found; ++iter){
			TopoDS_Edge ds_edge = *iter;

			Utils::Math::Point extrema1, extrema2;
			gp_Pnt res;
			BRepAdaptor_Curve brepCurve(ds_edge);
			res = brepCurve.Value(brepCurve.FirstParameter());
			extrema1.setXYZ(res.X(), res.Y(), res.Z());
			res = brepCurve.Value(brepCurve.LastParameter());
			extrema2.setXYZ(res.X(), res.Y(), res.Z());
#ifdef _DEBUG_COMPOSITE
	std::cout<<"TopoDS_Edge entre  "<<extrema1<<" et "<<extrema2<<std::endl;
	std::cout<<"distance ptPrec extrema1 : "<<ptPrec.length(extrema1)<<std::endl;
	std::cout<<"distance ptPrec extrema2 : "<<ptPrec.length(extrema2)<<std::endl;
#endif

			if (ptPrec.isEpsilonEqual(extrema1, epsilon)){
				found = true;
				ptPrec = extrema2;
				ds_edge_found = ds_edge;
				iter_to_erase = iter;
			} else if (ptPrec.isEpsilonEqual(extrema2, epsilon)){
				found = true;
				ptPrec = extrema1;
				ds_edge_found = ds_edge;
				iter_to_erase = iter;
			}
		} // end for iter

		// pour ne pas réutiliser ce TopoDS_Edge
		if (found){
#ifdef _DEBUG_COMPOSITE
			std::cout<<"found, ptPrec : "<<ptPrec<<std::endl;
#endif
			reps.push_back(ds_edge_found);
			v_ds_edge.erase(iter_to_erase);
		}
		else {
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "OCC a échoué, création de la courbe composite entre "
					<<extremaFirst<<" et "<<extremaLast;
			throw TkUtil::Exception (message);
		}

	} while(!ptPrec.isEpsilonEqual(extremaLast, epsilon));
#ifdef _DEBUG_COMPOSITE
	std::cout<<"terminé, il reste "<<v_ds_edge.size()<<" TopoDS_Edge non utilisées, pour "<<reps.size()<<" utilisées"<<std::endl;
#endif

	// création de la courbe composée
	Curve*  curve   = new Curve(m_context,
			m_context.newProperty(Utils::Entity::GeomCurve),
			m_context.newDisplayProperties(Utils::Entity::GeomCurve),
			new GeomProperty(),
			reps);
	CHECK_NULL_PTR_ERROR (curve)
	m_context.newGraphicalRepresentation (*curve);
	return curve;
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_EDGEPROJECTION
Curve* EntityFactory::newCurveByEdgeProjectionOnSurface(const Utils::Math::Point& P1,
            const Utils::Math::Point& P2,
            Surface* surface)
{
#ifdef _DEBUG_EDGEPROJECTION
    std::cout<<"EntityFactory::newCurveByEdgeProjectionOnSurface("
            <<P1 << ", "<< P2 << ", "<<surface->getName()<<")"<<std::endl;
#endif

    // création de l'arête à projeter
    gp_Pnt Pt1(P1.getX(),P1.getY(),P1.getZ());
    gp_Pnt Pt2(P2.getX(),P2.getY(),P2.getZ());
    TopoDS_Edge edge_shape = BRepBuilderAPI_MakeEdge(Pt1,Pt2);

    return newCurveByTopoDS_ShapeProjectionOnSurface(edge_shape, surface);
}
/*----------------------------------------------------------------------------*/
Curve* EntityFactory::newCurveByCurveProjectionOnSurface(Curve* curve, Surface* surface)
{
#ifdef _DEBUG2
    std::cout<<"EntityFactory::newCurveByCurveProjectionOnSurface("
            <<curve->getName() << ", "<<surface->getName()<<")"<<std::endl;
#endif

    auto reps = curve->getOCCEdges();
    if (reps.size() != 1) 
        throw TkUtil::Exception(TkUtil::UTF8String ("Pas possible de projeter une courbe composée", TkUtil::Charset::UTF_8));
    return newCurveByTopoDS_ShapeProjectionOnSurface(reps[0], surface);
}
/*----------------------------------------------------------------------------*/
Curve* EntityFactory::newCurveByTopoDS_ShapeProjectionOnSurface(TopoDS_Edge shape, Surface* surface)
{
#ifdef _DEBUG_EDGEPROJECTION
    std::cout<<"EntityFactory::newCurveByTopoDS_ShapeProjectionOnSurface, proj sur "<<surface->getName()<<std::endl;
#endif
	std::vector<TopoDS_Edge> v_ds_edge;

	auto reps = surface->getOCCFaces();
	for (auto surf_shape : reps) {
	    BRepAlgo_NormalProjection proj;
	    proj.Init(surf_shape);
	    proj.Add(shape);
	    proj.Build();

	    if (!proj.IsDone()){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	        message << "OCC a échoué, création de la projection sur la surface  "<<surface->getName();
	        throw TkUtil::Exception (message);
	    }

	    // récupération de la projection (un compound)
	    const TopoDS_Shape proj_shape = proj.Projection();
	    TopExp_Explorer ex;
	    for (ex.Init(proj_shape, TopAbs_EDGE); ex.More(); ex.Next()) {
	    	TopoDS_Edge aEdge = TopoDS::Edge(ex.Current());
	    	v_ds_edge.push_back(aEdge);
#ifdef _DEBUG_EDGEPROJECTION
	    	std::cout<<" aEdge Degenerated ? "<<(BRep_Tool::Degenerated(aEdge)?"vrai":"faux")<<std::endl;
	        GProp_GProps pb;
	        BRepGProp::LinearProperties(aEdge,pb);
	        double res = pb.Mass();
	        std::cout<<" aEdge longueur = "<<res<<std::endl;
#endif

	    }

	} // end for i<reps.size()\

#ifdef _DEBUG_EDGEPROJECTION
		std::cout<<" v_ds_edge.size() = "<<v_ds_edge.size()<<std::endl;
#endif

	if (v_ds_edge.size() == 1)
		return newOCCCurve(v_ds_edge[0]);
	else {
		// identification des points aux extrémités de shape une fois projetés
		// pour orienter la projection et distinguer les projections utiles de celles parasites
		Utils::Math::Point extremaFirst, extremaLast;
		if(shape.ShapeType()==TopAbs_EDGE) {
			Utils::Math::Point extrema1, extrema2;
			BRepAdaptor_Curve brepCurve(TopoDS::Edge(shape));
			gp_Pnt res = brepCurve.Value(brepCurve.FirstParameter());
			extrema1.setXYZ(res.X(), res.Y(), res.Z());
            GeomProjectImplementation gpi;
			extremaFirst = gpi.project(surface, extrema1).first;
			res = brepCurve.Value(brepCurve.LastParameter());
			extrema2.setXYZ(res.X(), res.Y(), res.Z());
            extremaLast = gpi.project(surface, extrema2).first;
		}
		else {
			throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, newCurveByTopoDS_ShapeProjectionOnSurface n'est pas prévu pour chose qu'une TopAbs_EDGE", TkUtil::Charset::UTF_8));
        }

		return newOCCCompositeCurve(v_ds_edge, extremaFirst, extremaLast);
	}
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

