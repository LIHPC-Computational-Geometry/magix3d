/*----------------------------------------------------------------------------*/
/** \file Curve.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 03/11/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <string.h>
#include <sys/types.h>
/*----------------------------------------------------------------------------*/
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCHelper.h"
#include "Group/Group1D.h"
#include "Geom/EntityFactory.h"
#include "Geom/MementoGeomEntity.h"
#include "Internal/Context.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
#include "Utils/MgxException.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <BRep_Tool.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Pln.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

#include <GeomAPI_IntCS.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
const char* Curve::typeNameGeomCurve = "GeomCurve";
/*----------------------------------------------------------------------------*/
Curve::Curve(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
        GeomProperty* gprop, const TopoDS_Shape& shape)
:GeomEntity(ctx, prop, disp, gprop, shape)
{
}
/*----------------------------------------------------------------------------*/
GeomEntity* Curve::clone(Internal::Context& c)
{
	Curve* newCrv = new Curve(c,
            c.newProperty(this->getType()),
            c.newDisplayProperties(this->getType()),
            new GeomProperty(),
			m_shape);

	newCrv->paramLocFirst = paramLocFirst;
	newCrv->paramLocLast = paramLocLast;
	newCrv->paramImgFirst = paramImgFirst;
	newCrv->paramImgLast = paramImgLast;

	return newCrv;
}
/*----------------------------------------------------------------------------*/
Curve::~Curve()
{}
/*----------------------------------------------------------------------------*/
void Curve::setFromSpecificMemento(MementoGeomEntity& mem)
{
    m_surfaces = mem.getSurfaces();
    m_vertices = mem.getVertices();
    m_groups  = mem.getGroups1D();

    // pour le cas des courbes composites, on force la mise à jour des paramètres
    if (!m_vertices.empty())
    	computeParams(m_vertices[0]->getPoint());
}
/*----------------------------------------------------------------------------*/
void Curve::createSpecificMemento(MementoGeomEntity& mem)
{
    mem.setSurfaces(m_surfaces);
    mem.setVertices(m_vertices);
    mem.setGroups1D(m_groups);
}
/*----------------------------------------------------------------------------*/
void Curve::getRefEntities(std::vector<GeomEntity*>& entities)
{
    entities.clear();
    entities.insert(entities.end(),m_surfaces.begin(),m_surfaces.end());
    entities.insert(entities.end(),m_vertices.begin(),m_vertices.end());
}
/*----------------------------------------------------------------------------*/
void Curve::clearRefEntities(std::list<GeomEntity*>& vertices,
        std::list<GeomEntity*>& curves,
        std::list<GeomEntity*>& surfaces,
        std::list<GeomEntity*>& volumes)
{
    std::vector<Surface*> toRemoveS;
    std::vector<Vertex*> toRemoveV;
    for(unsigned int i=0;i<m_surfaces.size();i++){
        GeomEntity *e = m_surfaces[i];
        std::list<GeomEntity*>::iterator res = std::find(surfaces.begin(),surfaces.end(),e);
        if(res!=surfaces.end())
            toRemoveS.push_back(dynamic_cast<Surface*>(e));
    }

    for(unsigned int i=0;i<m_vertices.size();i++){
        GeomEntity *e = m_vertices[i];
        std::list<GeomEntity*>::iterator res = std::find(vertices.begin(),vertices.end(),e);
        if(res!=vertices.end())
            toRemoveV.push_back(dynamic_cast<Vertex*>(e));
    }

    for(unsigned int i=0;i<toRemoveS.size();i++)
        remove(toRemoveS[i]);
    for(unsigned int i=0;i<toRemoveV.size();i++)
        remove(toRemoveV[i]);
}
/*----------------------------------------------------------------------------*/
bool Curve::isEqual(Geom::Curve* curve)
{
    TopoDS_Shape sh1 = m_shape;
    TopoDS_Shape sh2 = curve->getShape();

    if(sh1.ShapeType()==TopAbs_EDGE && sh2.ShapeType()==TopAbs_EDGE)
    {
        TopoDS_Edge e1 = TopoDS::Edge(sh1);
        TopoDS_Edge e2 = TopoDS::Edge(sh2);
        return OCCGeomRepresentation::areEquals(e1,e2);
    }
    else if(sh1.ShapeType()==TopAbs_WIRE && sh2.ShapeType()==TopAbs_WIRE)
    {
        TopoDS_Wire w1 = TopoDS::Wire(sh1);
        TopoDS_Wire w2 = TopoDS::Wire(sh2);
        return OCCGeomRepresentation::areEquals(w1,w2);
    }

    return false;

}
/*----------------------------------------------------------------------------*/
void Curve::get(std::vector<Vertex*>& vertices) const
{
    vertices.clear();
    vertices.insert(vertices.end(),m_vertices.begin(),m_vertices.end());
}
/*----------------------------------------------------------------------------*/
void Curve::get(std::vector<Curve*>& curves) const
{
    std::list<Curve*> l;
    curves.clear();
    std::vector<Vertex*>::const_iterator it;
    for(it = m_vertices.begin();it!=m_vertices.end();it++){
        Vertex* v = *it;
        std::vector<Curve*> local_curves;
        v->get(local_curves);
        l.insert(l.end(),local_curves.begin(),local_curves.end());
    }

    l.sort(Utils::Entity::compareEntity);
    l.unique();
    l.remove(const_cast<Curve*>(this));
    curves.insert(curves.end(),l.begin(),l.end());
}
/*----------------------------------------------------------------------------*/
void Curve::get(std::vector<Surface*>& surfaces) const
{
    surfaces.clear();
    surfaces.insert(surfaces.end(),m_surfaces.begin(),m_surfaces.end());
}
/*----------------------------------------------------------------------------*/
void Curve::get(std::vector<Volume*>& volumes) const
{
    std::list<Volume*> l;
    volumes.clear();
    std::vector<Surface*>::const_iterator it;
    for(it = m_surfaces.begin();it!=m_surfaces.end();it++){
        Surface* s = *it;
        std::vector<Volume*> local_volumes;
        s->get(local_volumes);
        l.insert(l.end(),local_volumes.begin(),local_volumes.end());
    }
    l.sort(Utils::Entity::compareEntity);
    l.unique();

    volumes.insert(volumes.end(),l.begin(),l.end());
}
/*----------------------------------------------------------------------------*/
void Curve::get(std::vector<Topo::CoEdge*>& coedges)
{
	const std::vector<Topo::TopoEntity* >& topos = getRefTopo();

	for (std::vector<Topo::TopoEntity* >::const_iterator iter = topos.begin();
			iter != topos.end(); ++iter)
		if ((*iter)->getDim() == 1){
			Topo::CoEdge* coedge = dynamic_cast<Topo::CoEdge*>(*iter);
			if (coedge)
				coedges.push_back(coedge);
		}
}
/*----------------------------------------------------------------------------*/
void Curve::get(std::vector<Topo::Vertex*>& vertices)
{
	const std::vector<Topo::TopoEntity* >& topos = getRefTopo();

	for (std::vector<Topo::TopoEntity* >::const_iterator iter = topos.begin();
			iter != topos.end(); ++iter)
		if ((*iter)->getDim() == 0){
			Topo::Vertex* vertex = dynamic_cast<Topo::Vertex*>(*iter);
			if (vertex)
				vertices.push_back(vertex);
		}
}
/*----------------------------------------------------------------------------*/
bool Curve::contains(Curve* ACurve) const
{
    double my_bounds[6];
    double bounds[6];

    getBounds(my_bounds);
    ACurve->getBounds(bounds);
    double local_tolX = 0.5*(bounds[1]-bounds[0]);
    double local_tolY = 0.5*(bounds[3]-bounds[2]);
    double local_tolZ = 0.5*(bounds[5]-bounds[4]);
    if(    ( bounds[0] < my_bounds[0] - local_tolX)||//minX
            ( bounds[1] > my_bounds[1] + local_tolX)||//maxX
            ( bounds[2] < my_bounds[2] - local_tolY)||//minY
            ( bounds[3] > my_bounds[3] + local_tolY)||//maxY
            ( bounds[4] < my_bounds[4] - local_tolZ)||//minZ
            ( bounds[5] > my_bounds[5] + local_tolZ) )//maxZ
    {
        return false;
    }

    // détermine un ordre de dimension pour la courbe à partir de sa boite englobante
    double length = 0.0;
    for (uint i=0; i<3; i++)
    	if (length<my_bounds[i*2+1]-my_bounds[i*2])
    		length=my_bounds[i*2+1]-my_bounds[i*2];

    //std::cout<<" length pour "<<getName()<<" : "<<length<<std::endl;

    double tol = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon*length;

    //===============================================================
    // EN FAIT IL FAUT TRAVAILLER AVEC LA SHAPES OCC, CAR LA SHAPE
    // TESTEE PEUT NE PAS ENCORE ETRE CONNECTEE TOPOLOGIQUEMENT
    // AVEC DES ENTITES M3D
    //===============================================================
    TopoDS_Shape shOther = ACurve->getShape();

    //===============================================================
    // on teste les sommets
    //===============================================================
    TopTools_IndexedMapOfShape map_vertices;
    TopExp::MapShapes(shOther,TopAbs_VERTEX, map_vertices);
    for(unsigned int i=1; i<=map_vertices.Extent();i++){
        TopoDS_Shape s_i = map_vertices.FindKey(i);
        TopoDS_Vertex v_i = TopoDS::Vertex(s_i);
        gp_Pnt p_i = BRep_Tool::Pnt(v_i);

        Utils::Math::Point pi(p_i.X(),p_i.Y(),p_i.Z());
        Utils::Math::Point proj_i;

        project(pi,proj_i);
        if(!Utils::Math::MgxNumeric::isNearlyZero(pi.length(proj_i),tol))
            return false;

    }

    //===============================================================
    // On teste les courbes (1er point au 1/3, 2eme au 2/3)
    //===============================================================
    TopTools_IndexedMapOfShape map_edges;
    TopExp::MapShapes(shOther,TopAbs_EDGE, map_edges);
    for(unsigned int i=1; i<=map_edges.Extent();i++){
        TopoDS_Shape s_i = map_edges.FindKey(i);
        TopoDS_Edge e_i = TopoDS::Edge(s_i);
        BRepAdaptor_Curve curve_adaptor(e_i);
        double umin = curve_adaptor.FirstParameter();
        double umax = curve_adaptor.LastParameter();

        double u1 = umin+(umax-umin)/3.0;

        gp_Pnt pnt1 = curve_adaptor.Value(u1);
        Utils::Math::Point pi1(pnt1.X(),pnt1.Y(),pnt1.Z());

        Utils::Math::Point proj_i1;
        project(pi1,proj_i1);
        if(!Utils::Math::MgxNumeric::isNearlyZero(pi1.length(proj_i1),tol))
        {
            //            std::cout<<"Point  :"<<pi1<<std::endl;
            return false;
        }

        double u2 = umin+(umax-umin)*2.0/3.0;

        gp_Pnt pnt2 = curve_adaptor.Value(u2);
        Utils::Math::Point pi2(pnt2.X(),pnt2.Y(),pnt2.Z());

        Utils::Math::Point proj_i2;
        project(pi2,proj_i2);
        if(!Utils::Math::MgxNumeric::isNearlyZero(pi2.length(proj_i2),tol))
        {
            //            std::cout<<"Point  :"<<pi2<<std::endl;
            return false;
        }

    }
    return true;
}
/*----------------------------------------------------------------------------*/
void Curve::project(Utils::Math::Point& P) const
{
    projectPointOn(P);
}
/*----------------------------------------------------------------------------*/
void Curve::project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const
{
    P2=P1;
    projectPointOn(P2);
}
/*----------------------------------------------------------------------------*/
void Curve::
getPoint(const double& p, Utils::Math::Point& Pt, const bool in01) const
{
    gp_Pnt res;
    if(m_shape.ShapeType()==TopAbs_EDGE){
        BRepAdaptor_Curve brepCurve(TopoDS::Edge(m_shape));
        if(in01)
        {
            double f= brepCurve.FirstParameter();
            double l= brepCurve.LastParameter();

            //p dans [0,1] a positionner dans [f,l]
            if(f<l)
                res = brepCurve.Value(f+p*(l-f));
            else
                res = brepCurve.Value(l+p*(f-l));
        }
        else
            res = brepCurve.Value(p);
    }
    else{
    	MGX_FORBIDDEN("cas d'un wire");
//        //cas du wire, on est entre 0 et 1 et on gère nous même
//        TopoDS_Wire w = TopoDS::Wire(m_shape);
//        //====================================================================
//        // Calcul de la longueur de la courbe
//        //====================================================================
//        BRepCheck_Analyzer anaAna(w, Standard_False);
//        GProp_GProps pb;
//        BRepGProp::LinearProperties(w,pb);
//        double total_length = pb.Mass();
//
//        //====================================================================
//        // on recupere toutes les courbes formant le wire
//        //====================================================================
//        TopTools_IndexedMapOfShape  mapE;
//        TopExp::MapShapes(w,TopAbs_EDGE, mapE);
//
//        //====================================================================
//        // on recupere le point de depart et de fin (qui peuvent etre
//        // confondus)
//        //====================================================================
//        std::vector<Vertex*> m3d_vertices;
//        crb->get(m3d_vertices);
//        Vertex* first_vertex  = m3d_vertices.front();
//        OCCGeomRepresentation* occ_rep =
//                dynamic_cast<OCCGeomRepresentation*>
//                (first_vertex->getComputationalProperty());
//        CHECK_NULL_PTR_ERROR(occ_rep);
//        TopoDS_Vertex v1 = TopoDS::Vertex(occ_rep->getShape());
//
//        Vertex* second_vertex = m3d_vertices.back();
//        occ_rep = dynamic_cast<OCCGeomRepresentation*>
//                (second_vertex->getComputationalProperty());
//        CHECK_NULL_PTR_ERROR(occ_rep);
//        TopoDS_Vertex v2 = TopoDS::Vertex(occ_rep->getShape());
//
//
//        //====================================================================
//        // En fonction du paramètre p, on cherche le tronçon, ou la courbe,
//        // dont le paramétrage contient p
//        //====================================================================
//        TopoDS_Vertex v_from = v1;
//        TopoDS_Vertex v_to;
//
//        // On cherche la courbe incidente a v_from et appartenant à w.
//        // Pour le moment, le cas  cyclique n'est pas géré, car on ne sait
//        // alors pas quelle courbe choisir pour démarrer?
//        if(OCCGeomRepresentation::areEquals(v1,v2))
//            throw TkUtil::Exception("Incapacité à déterminer un point à partir d'un paramètre pour une courbe composite cyclique");
//
//        TopoDS_Edge current_edge;
//        bool edge_found=false;
//        for(int i=1; i<=mapE.Extent() && !edge_found;i++)
//        {
//            TopoDS_Edge ce = TopoDS::Edge(mapE.FindKey(i));
//            TopTools_IndexedMapOfShape  mapV;
//            TopExp::MapShapes(ce,TopAbs_VERTEX, mapV);
//            TopoDS_Vertex ce_v1, ce_v2;
//            if(mapV.Extent()>1){
//                ce_v1 = TopoDS::Vertex(mapV.FindKey(1));
//                ce_v2 = TopoDS::Vertex(mapV.FindKey(2));
//                gp_Pnt edge_pnt1 = BRep_Tool::Pnt(ce_v1);
//                gp_Pnt edge_pnt2 = BRep_Tool::Pnt(ce_v2);
//                if(OCCGeomRepresentation::areEquals(ce_v1,v_from)){
//                    edge_found = true;
//                    current_edge = ce;
//                    v_to = ce_v2;
//                }
//                else if(OCCGeomRepresentation::areEquals(ce_v2,v_from)){
//                    edge_found = true;
//                    current_edge = ce;
//                    v_to = ce_v1;
//                }
//            }
//        }
//        if(!edge_found)
//             throw TkUtil::Exception("Problème de topologie OCC (Wire,Edge,Vertex) dans OCCGeomRepresentation::getPoint");
//
//      //  std::cout<<"EDGE choisie "<<current_edge<<std::endl;
//        //ici on a la premiere courbe, le sommet de départ, le sommet suivant.
//        // On peut calculer les paramètres
//        double param_global_from, param_local_from;
//        double param_global_to  , param_local_to  ;
//        double prev_length=0;
//        BRepCheck_Analyzer anaEdge(current_edge, Standard_False);
//        GProp_GProps pb_edge;
//        BRepGProp::LinearProperties(current_edge,pb_edge);
//        double current_length = pb_edge.Mass();
//
//        param_global_from=0; //premier sommet de la première courbe
//        //On déduit le paramètre param_global_to
//        param_global_to = (prev_length+current_length)/total_length;
//        while(p>param_global_to){
//            v_from = v_to; // on change de sommet de départ
//            //puis de courbe courante
//            bool edge_found=false;
//            prev_length+=current_length;
//            for(int i=1; i<=mapE.Extent() && !edge_found;i++)
//            {
//                TopoDS_Edge ce = TopoDS::Edge(mapE.FindKey(i));
//                TopTools_IndexedMapOfShape  mapV;
//                TopExp::MapShapes(ce,TopAbs_VERTEX, mapV);
//                TopoDS_Vertex ce_v1, ce_v2;
//                if(mapV.Extent()>1){
//                    ce_v1 = TopoDS::Vertex(mapV.FindKey(1));
//                    ce_v2 = TopoDS::Vertex(mapV.FindKey(2));
//                    if(OCCGeomRepresentation::areEquals(ce_v1,v_from) &&
//                            ce!=current_edge)
//                    {
//                        edge_found = true;
//                        current_edge = ce;
//                        v_to = ce_v2;
//                    }
//                    else if(OCCGeomRepresentation::areEquals(ce_v2,v_from) &&
//                            ce!=current_edge)
//                    {
//                        edge_found = true;
//                        current_edge = ce;
//                        v_to = ce_v1;
//                    }
//                }
//            }
//            //prev_length, current_edge, v_from et v_to sont maintenant initialisés
//            BRepCheck_Analyzer anaEdge(current_edge, Standard_False);
//            GProp_GProps pb_edge;
//            BRepGProp::LinearProperties(current_edge,pb_edge);
//            current_length = pb_edge.Mass();
//
//            param_global_from=prev_length/total_length;;
//            param_global_to = (prev_length+current_length)/total_length;
//        }
//        //====================================================================
//        // On est maintenant sur la bonne courbe
//        //====================================================================
//        // On récupère le paramétrage local
//        Handle(Geom_Curve) brepCurve = BRep_Tool::Curve(current_edge,
//                         param_local_from,//paramètre local à la courbe courante pour v_from
//                         param_local_to); //paramètre local à la courbe courante pour v_to
//
//        double t= (p-param_global_from)/(param_global_to-param_global_from);
//        double local_param;
//        if(param_local_from<param_local_to){
//            gp_Pnt first_curve_pnt = brepCurve->Value(param_local_from);
//            gp_Pnt from_pnt =BRep_Tool::Pnt(v_from);
//            if(from_pnt.Distance(first_curve_pnt)<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)
//                //courbe locale parcourue dans le même sens que la courbe composite
//                local_param = (1-t)*param_local_from + t*param_local_to;
//            else//courbe locale parcourue dans le sens inverse
//                local_param = (1-t)*param_local_to + t*param_local_from;
//        }
//        else{//sens inverse de la parametrisation OCC
//            local_param = (1-t)*param_local_to + t*param_local_from;
//        }
//        res = brepCurve->Value(local_param);
////        std::cout<<"Pour p="<<p<<" -> ("<<res.X()<<", "<<res.Y()<<", "<<res.Z()<<")"
////                <<" entre ("<<first_vertex->getX()<<", "<<first_vertex->getY()<<", "<<first_vertex->getZ()<<") et "
////                <<" ("<<second_vertex->getX()<<", "<<second_vertex->getY()<<", "<<second_vertex->getZ()<<")"
////                <<std::endl;
    }

    Pt.setXYZ(res.X(), res.Y(), res.Z());
}
/*----------------------------------------------------------------------------*/
void Curve::tangent(const Utils::Math::Point& P1, Utils::Math::Vector& V2) const
{
	TopoDS_Edge edge = TopoDS::Edge(m_shape);
	double first, last;
	Handle_Geom_Curve curv = BRep_Tool::Curve(edge, first, last);
	gp_Pnt pnt(P1.getX(),P1.getY(),P1.getZ());

    GeomAPI_ProjectPointOnCurve proj(pnt,curv, first, last);

    proj.Perform(pnt);

    // on va aussi observer les points aux extrémités
    gp_Pnt pnt_first = curv->Value(first);
    gp_Pnt pnt_last  = curv->Value(last);
    if(proj.NbPoints()!=0){
    	Standard_Real p = proj.LowerDistanceParameter();

        gp_Vec V1;
        gp_Pnt Pt;

        curv->D1(p, Pt, V1);
        V2.setXYZ(V1.X(), V1.Y(), V1.Z());
    }
    else {
    	throw  Utils::BadNormalException("Erreur interne, pas de point projeté sur la courbe pour obtenir la tangente");
    }
}
/*----------------------------------------------------------------------------*/
void Curve::getIntersection(gp_Pln& plan_cut, Utils::Math::Point& Pt) const
{
#ifdef _DEBUG_INTERSECTION
	std::cout<<"OCCGeomRepresentation::getIntersection ..."<<std::endl;
#endif
	BRepBuilderAPI_MakeFace mkF(plan_cut);
	TopoDS_Face wf = mkF.Face();
	Handle_Geom_Surface surf = BRep_Tool::Surface(wf);

	TopoDS_Edge edge = TopoDS::Edge(m_shape);
	double first, last;
	Handle_Geom_Curve curv = BRep_Tool::Curve(edge, first, last);

	bool isPeriodic = curv->IsPeriodic();
	Standard_Real period = 0;
	if (isPeriodic)
		period = curv->Period();

	 GeomAPI_IntCS Intersector(curv, surf);
	 uint pt_found = 0;
	 if (Intersector.IsDone()){
		 if (Intersector.NbPoints() == 1){
			 gp_Pnt pt = Intersector.Point (1);
			 for (uint j=0; j<3; j++)
				 Pt.setCoord(j,pt.Coord(j+1));
			 pt_found = 1;
		 }
		 else if (Intersector.NbPoints() > 1){
			 for (uint i=1; i<=Intersector.NbPoints(); i++){
				 gp_Pnt pt = Intersector.Point (i);
				 Quantity_Parameter U, V, W;
				 Intersector.Parameters(i, U, V, W);
#ifdef _DEBUG_INTERSECTION
				 std::cout<<" i = "<<i<<" W = "<<W<<" dans ["<<first<<" "<<last<<"] ?"<<std::endl;
				 std::cout<<"  isPeriodic = "<<isPeriodic<<", period = "<<period<<std::endl;
#endif
				 if ((W>=first && W<=last) || (isPeriodic && W+period>=first && W+period<=last)){
#ifdef _DEBUG_INTERSECTION
					 std::cout<<"  point accepté"<<std::endl;
#endif
					 for (uint j=0; j<3; j++)
						 Pt.setCoord(j,pt.Coord(j+1));
					 pt_found+=1;
				 }
			 }
		 }
	 }
	 else {
     	throw TkUtil::Exception(TkUtil::UTF8String ("Problème OCC lors de l'intersection entre courbe et un plan", TkUtil::Charset::UTF_8));
	 }
	 if (pt_found != 1) {
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		 message << "Problème OCC lors de l'intersection entre courbe et un plan, on obtient ";
		 message << (short)Intersector.NbPoints();
		 message <<" points dont "<<(short)pt_found<<" pourraient convenir\n";
		 for (uint i=1; i<=Intersector.NbPoints(); i++){
			 gp_Pnt pt = Intersector.Point (i);
			 message <<" i = "<<(short)i<<" "<<pt.X()<<","<<pt.Y()<<","<<pt.Z()<<"\n";
		 }
		 message<<" plan de coupe passe par point "<<plan_cut.Location().X()
				 <<", "<<plan_cut.Location().Y()
				 <<", "<<plan_cut.Location().Z()
				 <<" et a pour normale "<<plan_cut.Axis().Direction().X()
				 <<", "<<plan_cut.Axis().Direction().Y()
				 <<", "<<plan_cut.Axis().Direction().Z()
				 <<"\n";

		 throw TkUtil::Exception(message);
	 }
}
/*----------------------------------------------------------------------------*/
void Curve::getParameter(const Utils::Math::Point& Pt, double& p) const
{
    if(m_shape.ShapeType()==TopAbs_EDGE){
        getParameterOnTopoDSEdge(TopoDS::Edge(m_shape), Pt,p);
    }
    else if(m_shape.ShapeType()==TopAbs_WIRE)
    {
        //====================================================================
        // On recherche l'arête du wire sur laquelle le point P se projete car
        // le plus proche. On ne peut toutefois pas faire ce parcours dans n'
        // importe quel ordre. On va partir du point intial de la courbe C et
        // aller vers le point final en récoltant diverses informations
        // nécessaires pour la suite des calculs
        //====================================================================
        TopoDS_Wire w = TopoDS::Wire(m_shape);

        //====================================================================
        // Calcul de la longueur totale de la courbe
        //====================================================================
        BRepCheck_Analyzer anaAna(w, Standard_False);
        GProp_GProps pb;
        BRepGProp::LinearProperties(w,pb);
        double wire_length = pb.Mass();

        //====================================================================
        // on recupere toutes les courbes formant le wire
        //====================================================================
        TopTools_IndexedMapOfShape  mapE;
        TopExp::MapShapes(w,TopAbs_EDGE, mapE);

        //====================================================================
        // on recupere le point de depart et de fin (qui peuvent etre
        // confondus)
        //====================================================================
        std::vector<Vertex*> m3d_vertices;
        get(m3d_vertices);

        //récuperation du sommet initial
        Vertex* first_vertex  = m3d_vertices.front();
        TopoDS_Vertex v1 = TopoDS::Vertex(first_vertex->getShape());

        //récuperation du sommet final
        Vertex* second_vertex = m3d_vertices.back();
        TopoDS_Vertex v2 = TopoDS::Vertex(second_vertex->getShape());

        //====================================================================
        // On recherce maintenant l'arête sur laquelle se projète P en partant
        // de v_from.
        //====================================================================
        TopoDS_Vertex v_from = v1;
        TopoDS_Vertex v_to;
        // On cherche la courbe incidente a v_from et appartenant à w.
        // Pour le moment, le cas  cyclique n'est pas géré, car on ne sait
        // alors pas quelle courbe choisir pour démarrer?
        if(OCCHelper::areEquals(v1,v2))
         	throw TkUtil::Exception(TkUtil::UTF8String ("Incapacité à déterminer un point à partir d'un paramètre pour une courbe composite cyclique", TkUtil::Charset::UTF_8));


        //On commence par récupérer la première arête
        TopoDS_Edge current_edge;
        bool edge_found=false;

        for(int i=1; i<=mapE.Extent() && !edge_found;i++)
        {
            TopoDS_Edge ce = TopoDS::Edge(mapE.FindKey(i));
            TopTools_IndexedMapOfShape  mapV;
            TopExp::MapShapes(ce,TopAbs_VERTEX, mapV);
            TopoDS_Vertex ce_v1, ce_v2;
            if(mapV.Extent()>1){
                ce_v1 = TopoDS::Vertex(mapV.FindKey(1));
                ce_v2 = TopoDS::Vertex(mapV.FindKey(2));
//                gp_Pnt edge_pnt1 = BRep_Tool::Pnt(ce_v1);
//                gp_Pnt edge_pnt2 = BRep_Tool::Pnt(ce_v2);
//                std::cout<<"Courbe OCC  de ("<<edge_pnt1.X()<<", "<<edge_pnt1.Y()<<", "<<edge_pnt1.Z()<<") "
//                        <<" a ("<<edge_pnt2.X()<<", "<<edge_pnt2.Y()<<", "<<edge_pnt2.Z()<<") "
//                        <<std::endl;
                if(OCCHelper::areEquals(ce_v1,v_from)){
//                    std::cout<<"\t POINT 1 RECHERCHE"<<std::endl;
                    edge_found = true;
                    current_edge = ce;
                    v_to = ce_v2;
                }
                else if(OCCHelper::areEquals(ce_v2,v_from)){
 //                   std::cout<<"\t POINT 2 RECHERCHE"<<std::endl;
                    edge_found = true;
                    current_edge = ce;
                    v_to = ce_v1;
                }
            }

        }
        if(!edge_found)
        	throw TkUtil::Exception(TkUtil::UTF8String ("Problème de topologie OCC (Wire,Edge,Vertex) dans OCCGeomRepresentation::getPoint", TkUtil::Charset::UTF_8));

        //on a maintenant la courbe de départ orientée dans le sens
        //de parcours défini de v_from à v_to
        double param_global_from, param_global_to  ;
        double prev_length=0;
        BRepCheck_Analyzer anaEdge(current_edge, Standard_False);
        GProp_GProps pb_edge;
        BRepGProp::LinearProperties(current_edge,pb_edge);
        double current_length = pb_edge.Mass();


        param_global_from=0; //premier sommet de la première courbe
        //On déduit le paramètre param_global_to
        param_global_to = (prev_length+current_length)/wire_length;

        //maintenant on parcourt tant que l'arête contenant P n'a
        //pas été trouvée
        double local_param_of_P;
        bool found_edge_containing_P=false;
        double local_param_of_P_trial=1000;
        try{
            local_param_of_P_trial =
                    getParameterOnTopoDSEdge(current_edge,Pt,local_param_of_P);
        }
        catch(TkUtil::Exception &e) {
            local_param_of_P_trial = 1000;
        }
//        std::cout<<"* from "<<param_global_from<<" to "<<param_global_to<<std::endl;

        while( local_param_of_P_trial > Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon )
        {
            v_from = v_to; // on change de sommet de départ
            //puis de courbe courante
            bool edge_found=false;

            prev_length+=current_length;
            for(int i=1; i<=mapE.Extent() && !edge_found;i++)
            {
                TopoDS_Edge ce = TopoDS::Edge(mapE.FindKey(i));
                TopTools_IndexedMapOfShape  mapV;
                TopExp::MapShapes(ce,TopAbs_VERTEX, mapV);
                TopoDS_Vertex ce_v1, ce_v2;
                if(mapV.Extent()>1){
                    ce_v1 = TopoDS::Vertex(mapV.FindKey(1));
                    ce_v2 = TopoDS::Vertex(mapV.FindKey(2));
//                    gp_Pnt edge_pnt1 = BRep_Tool::Pnt(ce_v1);
//                    gp_Pnt edge_pnt2 = BRep_Tool::Pnt(ce_v2);
//                    std::cout<<"Courbe OCC  de ("<<edge_pnt1.X()<<", "<<edge_pnt1.Y()<<", "<<edge_pnt1.Z()<<") "
//                                         <<" a ("<<edge_pnt2.X()<<", "<<edge_pnt2.Y()<<", "<<edge_pnt2.Z()<<") "
//                                         <<std::endl;
                    if(OCCHelper::areEquals(ce_v1,v_from) &&
                            ce!=current_edge)
                    {
//                        std::cout<<"\t POINT 1 RECHERCHE"<<std::endl;

                        edge_found = true;
                        current_edge = ce;
                        v_to = ce_v2;
                    }
                    else if(OCCHelper::areEquals(ce_v2,v_from) &&
                            ce!=current_edge)
                    {
  //                      std::cout<<"\t POINT 2 RECHERCHE"<<std::endl;
                        edge_found = true;
                        current_edge = ce;
                        v_to = ce_v1;
                    }
                }
            }
            //prev_length, current_edge, v_from et v_to sont maintenant initialisés
            BRepCheck_Analyzer anaEdge(current_edge, Standard_False);
            GProp_GProps pb_edge;
            BRepGProp::LinearProperties(current_edge,pb_edge);
            current_length = pb_edge.Mass();
//            std::cout<<"prev = "<<prev_length<<std::endl;
//            std::cout<<"curr = "<<current_length<<std::endl;
//            std::cout<<"total= "<<wire_length<<std::endl;
            param_global_from=prev_length/wire_length;
            param_global_to = (prev_length+current_length)/wire_length;
//            std::cout<<"- from "<<param_global_from<<" to "<<param_global_to<<std::endl;
            try{
                local_param_of_P_trial =
                        getParameterOnTopoDSEdge(current_edge,Pt,local_param_of_P);
            }
            catch(TkUtil::Exception &e) {
                local_param_of_P_trial = 1000;
            }

        }
        //====================================================================
        // on est sur l'arête contenant le point P
        // on a deja les coordonnées globales de v_from et v_to ainsi que le
        // paramètre local de P dans current_edge.
        // on récupère maintenant le paramétrage local à la courbe
        //====================================================================
        double first_local_param, last_local_param;
        //Parametre locaux de la courbe
        Handle(Geom_Curve) brepCurve = BRep_Tool::Curve(current_edge,
                first_local_param, // param. local à la courbe pour v_from
                last_local_param); // param. local à la courbe pour v_to

        //====================================================================
        double t= (local_param_of_P-first_local_param)/(last_local_param-first_local_param);
        double global_param;
        if(first_local_param<last_local_param){
            gp_Pnt first_curve_pnt = brepCurve->Value(first_local_param);
            gp_Pnt from_pnt =BRep_Tool::Pnt(v_from);
            if(from_pnt.Distance(first_curve_pnt)<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)
                //courbe locale parcourue dans le même sens que la courbe composite
                global_param = (1-t)*param_global_from + t*param_global_to;
            else//courbe locale parcourue dans le sens inverse
                global_param = (1-t)*param_global_to + t*param_global_from;
        }
        else{//sens inverse de la parametrisation OCC
            global_param = (1-t)*param_global_to + t*param_global_from;
        }
        p = global_param;
//        std::cout<<"Parametre : "<<p<<" pour "<<Pt<<std::endl;
//        gp_Pnt from_pnt =BRep_Tool::Pnt(v_from);
//        std::cout<<"\t entre ("<<from_pnt.X()<<", "<<from_pnt.Y()<<", "<<from_pnt.Z()<<")";
//        gp_Pnt to_pnt =BRep_Tool::Pnt(v_to);
//        std::cout<<" et ("<<to_pnt.X()<<", "<<to_pnt.Y()<<", "<<to_pnt.Z()<<")"<<std::endl;
//        std::cout<<"\t de param local ("<<first_local_param<<", "<<last_local_param<<")"<<std::endl;
//        std::cout<<"\t de param global("<<param_global_from<<", "<<param_global_to<<")"<<std::endl;
//        gp_Pnt first_curve_pnt = brepCurve->Value(first_local_param);
//        if(from_pnt.Distance(first_curve_pnt)<Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)
//            std::cout<<"\t avec ORDRE DIRECT"<<std::endl;
//        else
//            std::cout<<"\t avec ORDRE INVERSE"<<std::endl;
//        std::cout<<"\t t = "<<t<<std::endl;
    }
    else
    	throw TkUtil::Exception(TkUtil::UTF8String ("Le paramétrage d'un point ne peut être demandé que sur une courbe!!", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_GETPARAMETER
double Curve::getParameterOnTopoDSEdge(
        const TopoDS_Edge& edge, const Utils::Math::Point& Pt, double& p)
{
    double distance = 0;
    gp_Pnt pnt(Pt.getX(),Pt.getY(),Pt.getZ());
    Standard_Real first, last;
    Handle_Geom_Curve curv = BRep_Tool::Curve(edge,first, last);
#ifdef _DEBUG_GETPARAMETER
    std::cout<<"OCCGeomRepresentation::getParameterOnTopoDSEdge pour "<<Pt<<std::endl;
    std::cout<<"first = "<<first<<std::endl;
    std::cout<<"last  = "<<last<<std::endl;
#endif

    GeomAPI_ProjectPointOnCurve proj(pnt,curv, first, last);

    proj.Perform(pnt);

    // on va aussi observer les points aux extrémités
    gp_Pnt pnt_first = curv->Value(first);
    gp_Pnt pnt_last  = curv->Value(last);
#ifdef _DEBUG_GETPARAMETER
    std::cout<<" => NbPoints = "<<proj.NbPoints()<<std::endl;
    std::cout<<"first = ("<<pnt_first.X()<<", "<<pnt_first.Y()<<", "<<pnt_first.Z()<<") pour param "<<first<<std::endl;
    std::cout<<"last = ("<<pnt_last.X()<<", "<<pnt_last.Y()<<", "<<pnt_last.Z()<<") pour param "<<last<<std::endl;
#endif
    if(proj.NbPoints()!=0){
        p = proj.LowerDistanceParameter();
        distance = proj.LowerDistance();
        gp_Pnt nearest_pt = proj.NearestPoint();

        double dist2 = pnt.Distance(pnt_first);
#ifdef _DEBUG_GETPARAMETER
        std::cout<<"dist2 = "<<dist2<<" pour pnt_first"<<std::endl;
#endif
        if (dist2<distance){
        	p=first;
        	nearest_pt = pnt_first;
        	distance = dist2;
        }
        dist2 = pnt.Distance(pnt_last);
#ifdef _DEBUG_GETPARAMETER
        std::cout<<"dist2 = "<<dist2<<" pour pnt_last"<<std::endl;
#endif
        if (dist2<distance){
        	p=last;
        	nearest_pt = pnt_last;
        	distance = dist2;
        }

#ifdef _DEBUG_GETPARAMETER
       for (uint i=1; i<=proj.NbPoints(); i++){
        	std::cout<<"i: "<<i<<", Parameter: "<<proj.Parameter(i)<<", Distance: "<<proj.Distance(i)<<std::endl;
        }
    	std::cout<<"retenu : distance "<<distance<<", param "<<p<<", nearest_pt "<<nearest_pt.X()<<", "<<nearest_pt.Y()<<", "<<nearest_pt.Z()<<std::endl;
#endif
    }
    else{
        distance = 0; // si sur un des points extrémités
        Utils::Math::Point pfirst(pnt_first.X(),pnt_first.Y(),pnt_first.Z());
        Utils::Math::Point plast(pnt_last.X(),pnt_last.Y(),pnt_last.Z());

        // calcul une tolérance relative à la dimension de la courbe (distance entre les 2 extrémités pour faire rapide)
        double length = (pfirst-plast).norme();
#ifdef _DEBUG_GETPARAMETER
        std::cout<<"length = "<<length<<std::endl;
#endif
       double tol = Utils::Math::MgxNumeric::mgxParameterEpsilon;
        if (!Utils::Math::MgxNumeric::isNearlyZero(length))
        	tol*=length;

        if(Pt.isEpsilonEqual(pfirst, tol))
            p =first;
        else if(Pt.isEpsilonEqual(plast, tol))
            p =last;
        else{
        	/*std::cout<<"==================="<<std::endl;
            std::cout<<"On cherche les parametres de "<<Pt<<std::endl;
            std::cout<<"first: "<<first<<" -> "<<pfirst<<", sq. dist: "<<Pt.length2(pfirst)<<std::endl;
            std::cout<<"last : "<<first<<" -> "<<plast<<", sq. dist: "<<Pt.length2(plast)<<std::endl;
            std::cout<<"avec epsilon = "<<tol<<std::endl;
            std::cout<<"==================="<<std::endl;

            std::cout<<"OCCGeomRepresentation::getParameter() pour "<<C->getName()<<std::endl;
            std::cout<<"OCCGeomRepresentation::getParameter() pour "<<C->getName()<<" (uid "<<C->getUniqueId()<<")"<<std::endl;
            std::cout<<" que l'on trouve dans les volumes:";
            std::vector<Volume*> volumes;
            C->get(volumes);
            for (uint i=0; i<volumes.size(); i++)
            	std::cout<<" "<<volumes[i]->getName();
            std::cout<<std::endl;
            std::cout<<" que l'on trouve dans les surfaces:";
            std::vector<Surface*> surfaces;
            C->get(surfaces);
            for (uint i=0; i<surfaces.size(); i++)
            	std::cout<<" "<<surfaces[i]->getName();
            std::cout<<std::endl;
            std::cout<<" qui est référencée par topo:";
            std::vector<Topo::TopoEntity* > topos = C->getRefTopo();
            for (uint i=0; i<topos.size(); i++)
            	std::cout<<" "<<topos[i]->getName();
            std::cout<<std::endl;
            std::cout<<"Pt = ("<<pnt.X()<<", "<<pnt.Y()<<", "<<pnt.Z()<<")\n";
            std::cout<<"p = "<<p<<std::endl;
            std::cout<<"first = ("<<pnt_first.X()<<", "<<pnt_first.Y()<<", "<<pnt_first.Z()<<")\n";
            std::cout<<"last = ("<<pnt_last.X()<<", "<<pnt_last.Y()<<", "<<pnt_last.Z()<<")\n";
*/
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Echec de la récupération d'un paramétrage d'un point sur une courbe!!\n";
            message <<"===================\n";
            message<<"On cherche le parametre de "<<Pt<<"\n";
            message<<"first: "<<first<<" -> "<<pfirst<<", dist: "<<Pt.length(pfirst)<<"\n";
            message<<"last : "<<first<<" -> "<<plast<<", dist: "<<Pt.length(plast)<<"\n";
            message<<"avec epsilon = "<<tol<<"\n";
            message<<"===================\n";
            throw TkUtil::Exception(message);
        }
    }

    return distance;
}
/*----------------------------------------------------------------------------*/
void Curve::getParameters(double& first, double& last) const
{
    if(m_shape.ShapeType()==TopAbs_EDGE){
        BRepAdaptor_Curve curve_adaptor(TopoDS::Edge(m_shape));
        first = curve_adaptor.FirstParameter();
        last= curve_adaptor.LastParameter();

//        if(first!=0){
//            double step=0;
//            if(first<0){
//                step=-first; first=0; last+=step;
//            }
//            else{ //first>0
//                step=first; first=0; last-=step;
//            }
//        }
    }
    else
    { //cas du wire!!
        first = 0;
        last  = 1;
    }
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_GETPARAMETRICSPOINTS
void Curve::getParametricsPoints(const Utils::Math::Point& Pt0,
        const Utils::Math::Point& Pt1,
        const uint nbPt,
        double* l_ratios,
        std::vector<Utils::Math::Point> &points)
{
	uint init_size = points.size();
#ifdef _DEBUG_GETPARAMETRICSPOINTS
    std::cout<<"Curve::getParametricsPoints pour "<<nbPt<<" points avec la courbe "
            << getName()<<", qui possède "<<m_vertices.size()<<" sommets."<<std::endl;
    if (m_vertices.size() == 2){
        std::cout<<" m_vertices[0] : "<<setprecision(14)<<m_vertices[0]->getCoord()<<std::endl;
        std::cout<<" m_vertices[1] : "<<setprecision(14)<<m_vertices[1]->getCoord()<<std::endl;
    }
#endif

    // sommes-nous dans le cas avec une interpolation qui passe de part et d'autre du sommet
    // de cette courbe fermée ?
    bool passeParSommet = false;
    // calcul du ratio à partir duquel on passe d'un côté du sommet à l'autre (si c'est le cas)
    double ratioSeuil = 0;


    double paramPt0 = 0;
    double paramPt1 = 1.0;
    double first = 0;
    double last = 0;
	getParameter(Pt0, paramPt0);
	getParameter(Pt1, paramPt1);

#ifdef _DEBUG_GETPARAMETRICSPOINTS
    std::cout<<"paramPt0 = "<<paramPt0<<std::endl;
    std::cout<<"paramPt1 = "<<paramPt1<<std::endl;
#endif

    if (m_vertices.size() == 1){
        getParameters(first, last);

#ifdef _DEBUG_GETPARAMETRICSPOINTS
        std::cout<<"first = "<<first<<std::endl;
        std::cout<<"last = "<<last<<std::endl;
#endif

        if (paramPt0 == paramPt1){
            // cas où l'on discrétise toute la courbe, mais les points extrémités sont confondus
            paramPt0 = first;
            paramPt1 = last;
        }
        else if (paramPt0 < paramPt1) {
            if (paramPt0 - first + last - paramPt1 < paramPt1 - paramPt0){
                passeParSommet = true;
                ratioSeuil = (paramPt0 - first) / (paramPt0 - first + last - paramPt1);
            }
        }
        else {
            if (paramPt1 - first + last - paramPt0 < paramPt0 - paramPt1){
            	if (Utils::Math::MgxNumeric::isNearlyZero(paramPt1 - first + last - paramPt0)){
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
            		messErr << "[Erreur interne] La courbe "<<getName()<<" ne permet pas de trouver les points en fonction d'une paramétrisation";
            		throw TkUtil::Exception(messErr);
            	}

                passeParSommet = true;
                ratioSeuil = (last - paramPt0) / (paramPt1 - first + last - paramPt0);
            }
        }
    }

#ifdef _DEBUG_GETPARAMETRICSPOINTS
    std::cout<<"passeParSommet = "<<(passeParSommet?"vrai":"faux")<<std::endl;
    std::cout<<"ratioSeuil = "<<ratioSeuil<<std::endl;
#endif

    if (passeParSommet) {
    	if (paramPt0 < paramPt1){
    		for (uint i=0; i<nbPt; i++) {
    			double paramVtx;
    			if (l_ratios[i] <= ratioSeuil)
    				paramVtx = paramPt0 + (first-paramPt0)*l_ratios[i]/ratioSeuil;
    			else
    				paramVtx = last + (paramPt1-last)*(l_ratios[i]-ratioSeuil)/(1-ratioSeuil);
    			Utils::Math::Point PtI;
    			getPoint(paramVtx, PtI);
    			points.push_back(PtI);
    		}
    	}
    	else {
    		for (uint i=0; i<nbPt; i++) {
                double paramVtx;
                if (l_ratios[i] <= ratioSeuil)
                    paramVtx = paramPt0 + (last-paramPt0)*l_ratios[i]/ratioSeuil;
                else
                    paramVtx = first + (paramPt1-first)*(l_ratios[i]-ratioSeuil)/(1-ratioSeuil);

                Utils::Math::Point PtI;
                getPoint(paramVtx, PtI);
                points.push_back(PtI);
            }
        }
    }
    else {
        for (uint i=0; i<nbPt; i++){
            double paramVtx = paramPt0 + (paramPt1-paramPt0)*l_ratios[i];
            Utils::Math::Point PtI;
            getPoint(paramVtx, PtI);
            points.push_back(PtI);
#ifdef _DEBUG_GETPARAMETRICSPOINTS
            std::cout<<"paramVtx (pour i = "<<i<<") = "<<paramVtx<<" => PtI "<<PtI<<std::endl;
#endif
        }
#ifndef NOT_USE_OPTIM_DISCR
        // vérification du respect de l_ratios
        double* lgi = new double[nbPt+1]; // longueurs des bras
        double* ecarts = new double[nbPt];
        double ecart_max = 0.0;
        double ecart_max_prec = 0.0;
        double ecart_tol = 0.001;
        uint nb_iter=0;
        const uint nb_iter_max = 10;
        // ratios locaux à une itération
        double* ratios = new double[nbPt+1];
        for (uint i=0; i<nbPt; i++)
        	ratios[i] = l_ratios[i];
        ratios[nbPt] = 1.0;

        do {
        	lgi[0] = Pt0.length(points[init_size]);
        	for (uint i=1; i<nbPt; i++)
        		lgi[i] = points[i-1+init_size].length(points[i+init_size]) + lgi[i-1];
        	lgi[nbPt] = points[nbPt-1+init_size].length(Pt1) + lgi[nbPt-1];

#ifdef _DEBUG_GETPARAMETRICSPOINTS
        	std::cout<<"nb_iter = "<<nb_iter<<", points.size() = "<<points.size()<<", init_size = "<<init_size<<std::endl;
        	for (uint i=0; i<nbPt+1; i++)
        		std::cout<<" lgi["<<i<<"] = "<<lgi[i]
						 <<std::endl;
#endif
        	ecart_max_prec = ecart_max;
        	ecart_max = 0.0;
        	for (uint i=0; i<nbPt; i++){
        		ecarts[i] = (l_ratios[i]*lgi[nbPt]-lgi[i]);
        		double ecart = std::abs(ecarts[i]);
        		if (ecart>ecart_max)
        			ecart_max = ecart;
        	}
        	if (nb_iter == 0){
        		ecart_max_prec = ecart_max;
        		ecart_tol *= lgi[nbPt];
        	}

#ifdef _DEBUG_GETPARAMETRICSPOINTS
        	for (uint i=0; i<nbPt; i++)
        		std::cout<<" ratios["<<i<<"] = "<<ratios[i]
						 <<" ecarts["<<i<<"] = "<<ecarts[i]
						 <<std::endl;
        	std::cout<<"ecart_max = "<<ecart_max<<std::endl;
        	std::cout<<"ecart_tol = "<<ecart_tol<<std::endl;
#endif
        	if (ecart_max>ecart_tol){
        		// on recommece ...
        		for (uint i=0; i<nbPt; i++){
        			ratios[i] += ecarts[i]*(ratios[i]-ratios[i+1])/(lgi[i]-lgi[i+1]);
        			double paramVtx = paramPt0 + (paramPt1-paramPt0)*ratios[i];
#ifdef _DEBUG_GETPARAMETRICSPOINTS
        			std::cout<<"paramVtx (pour i = "<<i<<") = "<<paramVtx<<std::endl;
#endif
        			getPoint(paramVtx, points[i+init_size]);
        		}
        	}
        	nb_iter += 1;
        } while (ecart_max>ecart_tol && nb_iter<nb_iter_max && ecart_max_prec >= ecart_max);
        delete [] lgi;
        delete [] ecarts;
        delete [] ratios;
#endif
    } // end else / if (passeParSommet)

//   for (uint i=0; i<nbPt; i++)
//       std::cout<<" ratio : "<<l_ratios[i]<<" => Point "<<l_points[i]<<std::endl;
}
/*----------------------------------------------------------------------------*/
void Curve::getParametricsPointsLoops(const double& alpha0,
        const double& alpha1,
        const uint nbPt,
        double* l_ratios,
        Utils::Math::Point* l_points)
{
#ifdef _DEBUG
    std::cout<<"Curve::getParametricsPointsLoops pour "<<nbPt<<" points avec la courbe "
            << getName()<<", qui possède "<<m_vertices.size()<<" sommets."<<std::endl;
    if (m_vertices.size() == 2){
        std::cout<<" m_vertices[0] : "<<setprecision(14)<<m_vertices[0]->getCoord()<<std::endl;
        std::cout<<" m_vertices[1] : "<<setprecision(14)<<m_vertices[1]->getCoord()<<std::endl;
    }
#endif
//
//    // sommes-nous dans le cas avec une interpolation qui passe de part et d'autre du sommet
//    // de cette courbe fermée ?
//    bool passeParSommet = false;
//    // calcul du ratio à partir duquel on passe d'un côté du sommet à l'autre (si c'est le cas)
//    double ratioSeuil = 0;
//

//    double paramPt0 = 0;
//    double paramPt1 = 0;
//    getParameter(Pt0, paramPt0);
//    getParameter(Pt1, paramPt1);
//    double first = 0;
//    double last = 0;
//
//    if (m_vertices.size() == 1){
//        getComputationalProperty()->getParameters(first, last);
//
////        std::cout<<"paramPt0 = "<<paramPt0<<std::endl;
////        std::cout<<"paramPt1 = "<<paramPt1<<std::endl;
////        std::cout<<"first = "<<first<<std::endl;
////        std::cout<<"last = "<<last<<std::endl;
//
//        if (paramPt0 < paramPt1) {
//            if (paramPt0 - first + last - paramPt1 < paramPt1 - paramPt0){
//                passeParSommet = true;
//                ratioSeuil = (paramPt0 - first) / (paramPt0 - first + last - paramPt1);
//            }
//        }
//        else {
//            if (paramPt1 - first + last - paramPt0 < paramPt0 - paramPt1){
//                passeParSommet = true;
//                ratioSeuil = (last - paramPt0) / (paramPt1 - first + last - paramPt0);
//// [EB]               ratioSeuil = (paramPt1 - first) / (paramPt1 - first + last - paramPt0);
//            }
//        }
//    }
//
//    if (passeParSommet) {
//        std::cout<<" On passe par le sommet unique de la courbe fermée, ratioSeuil = "<<ratioSeuil<<std::endl;
//        if (paramPt0 < paramPt1){
//            for (uint i=0; i<nbPt; i++) {
//                double paramVtx;
//                if (l_ratios[i] <= ratioSeuil)
//                    paramVtx = paramPt0 + (first-paramPt0)*l_ratios[i]/ratioSeuil;
//                else
//                    paramVtx = last + (paramPt1-last)*(l_ratios[i]-ratioSeuil)/(1-ratioSeuil);
//
//                getPoint(paramVtx, l_points[i]);
//            }
//        }
//        else {
//            for (uint i=0; i<nbPt; i++) {
//                double paramVtx;
//                if (l_ratios[i] <= ratioSeuil)
//                    paramVtx = paramPt0 + (last-paramPt0)*l_ratios[i]/ratioSeuil;
//// [EB]               paramVtx = paramPt1 + (first-paramPt1)*l_ratios[i]/ratioSeuil;
//                else
//                    paramVtx = first + (paramPt1-first)*(l_ratios[i]-ratioSeuil)/(1-ratioSeuil);
//// [EB]               paramVtx = last + (paramPt0-last)*(l_ratios[i]-ratioSeuil)/(1-ratioSeuil);
//
//                getPoint(paramVtx, l_points[i]);
//            }
//        }
//    }
//    else {
        for (uint i=0; i<nbPt; i++){
            double paramVtx = alpha0 + (alpha1-alpha0)*l_ratios[i];
            getPoint(paramVtx, l_points[i]);
        }
//    }

//   for (uint i=0; i<nbPt; i++)
//       std::cout<<" ratio : "<<l_ratios[i]<<" => Point "<<l_points[i]<<std::endl;
}
/*------------------------------------------------------------------------*/
//#define _DEBUG_GETPOLARPARAMETRICSPOINTS
void Curve::getPolarParametricsPoints(const Utils::Math::Point& Pt0,
		const Utils::Math::Point& Pt1,
		const uint nbPt,
		double* l_ratios,
		std::vector<Utils::Math::Point> &points,
		const Utils::Math::Point& polar_center)
{
#ifdef _DEBUG_GETPOLARPARAMETRICSPOINTS
    std::cout<<"Curve::getPolarParametricsPoints pour "<<nbPt<<" points avec la courbe "
            << getName()<<", qui possède "<<m_vertices.size()<<" sommets."<<std::endl;
    if (m_vertices.size() == 2){
        std::cout<<" m_vertices[0] : "<<setprecision(14)<<m_vertices[0]->getCoord()<<std::endl;
        std::cout<<" m_vertices[1] : "<<setprecision(14)<<m_vertices[1]->getCoord()<<std::endl;
    }
    std::cout<<"Pt0 = "<<Pt0<<std::endl;
    std::cout<<"Pt1 = "<<Pt1<<std::endl;
    std::cout<<"polar_center = "<<polar_center<<std::endl;
#endif

    // sommes-nous dans le cas avec une interpolation qui passe de part et d'autre du sommet
    // de cette courbe fermée ?
    bool passeParSommet = false;

    double paramPt0 = 0;
    double paramPt1 = 1.0;
    double first = 0;
    double last = 0;
	getParameter(Pt0, paramPt0);
	getParameter(Pt1, paramPt1);

#ifdef _DEBUG_GETPOLARPARAMETRICSPOINTS
    std::cout<<"paramPt0 = "<<paramPt0<<std::endl;
    std::cout<<"paramPt1 = "<<paramPt1<<std::endl;
#endif

    if (m_vertices.size() == 1){
    	MGX_NOT_YET_IMPLEMENTED("Cas avec courbe complète et découpage polaire")
    }

    // direction normale au plan de la courbe
    // on utilise un sommet sur la courbe
    Utils::Math::Point PtI;
    double paramPtI = paramPt0 + (paramPt1-paramPt0)/3.0;
    getPoint(paramPtI, PtI);
#ifdef _DEBUG_GETPOLARPARAMETRICSPOINTS
    std::cout<<"PtI = "<<PtI<<std::endl;
#endif

    Utils::Math::Vector vPt0 = Pt0-polar_center;
    vPt0 /= vPt0.norme();
    Utils::Math::Vector vPt1 = Pt1-polar_center;
    vPt1 /= vPt1.norme();
    Utils::Math::Vector vPtI = PtI-polar_center;
    vPtI /= vPtI.norme();

    gp_Pnt pCenter(polar_center.getX(), polar_center.getY(), polar_center.getZ());

    // normale au plan de la courbe
    Utils::Math::Vector normaleCrb = vPt0*vPtI;
    Utils::Math::Vector normaleCrb2 = vPt1*vPtI;
    // vérification que les 2 normales sont colinéaires, sinon c'est que les points ne sont pas coplanaires
    if (!Utils::Math::MgxNumeric::isNearlyZero((normaleCrb*normaleCrb2).norme(), Utils::Math::MgxNumeric::mgxDoubleEpsilon*10.0)){ // on relache légèrement la précision...
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr << "La courbe "<<getName()<<" ne peut se discrétiser avec une méthode polaire si le centre ("<<polar_center
				<<") n'est pas dans le même plan que la courbe, avec erreur de "<<(normaleCrb*normaleCrb2).norme()<<" et tolérance de "<<Utils::Math::MgxNumeric::mgxDoubleEpsilon*10.0;
		throw TkUtil::Exception(messErr);
    }
#ifdef _DEBUG_GETPOLARPARAMETRICSPOINTS
    std::cout<<"normaleCrb = "<<normaleCrb<<std::endl;
    std::cout<<"normaleCrb2 = "<<normaleCrb2<<std::endl;
#endif
    gp_Dir dir_normale(normaleCrb.getX(), normaleCrb.getY(), normaleCrb.getZ());
    gp_Ax1 axis_normaleCrb(pCenter,dir_normale);

    // normale au plan qui passe par Pt0 et le centre (avec précédente normale tangente)
    Utils::Math::Vector normalePlan = normaleCrb*vPt0;
    gp_Dir dir_normalePlan(normalePlan.getX(), normalePlan.getY(), normalePlan.getZ());
    //gp_Ax1 axis_normalePlan(pCenter,dir_normale2);

    // calcul de l'angle de rotation pour passer de Pt0 à Pt1
    double anglePt0Pt1 = atan2((vPt0*vPt1).norme(), vPt0.dot(vPt1));

#ifdef _DEBUG_GETPOLARPARAMETRICSPOINTS
    std::cout<<"anglePt0Pt1 = "<<anglePt0Pt1*180/M_PI<<std::endl;
#endif

    for (uint i=0; i<nbPt; i++){
    	// construction du plan pour l'intersection
    	gp_Pln plan_cut(pCenter, dir_normalePlan);

    	// rotation du plan
    	plan_cut.Rotate(axis_normaleCrb, anglePt0Pt1*l_ratios[i]);

    	// intersection entre plan et courbe
    	Utils::Math::Point ptIntersection;
    	getIntersection(plan_cut, ptIntersection);
    	points.push_back(ptIntersection);

    }
}
/*------------------------------------------------------------------------*/
Vertex* Curve::firstPoint() const
{
	TopoDS_Shape this_shape = m_shape;
	TopoDS_Edge e = TopoDS::Edge(this_shape);
	TopoDS_Vertex v = TopExp::FirstVertex(e);

	int index = -1;
	for(unsigned int i=0;i<m_vertices.size();i++){
		Vertex* vi = m_vertices[i];
		TopoDS_Shape vi_shape = vi->getShape();

		TopoDS_Vertex occ_vi = TopoDS::Vertex(vi_shape);
		if (OCCHelper::areEquals(v, occ_vi)){
			return vi;
		}
	}
	throw TkUtil::Exception(TkUtil::UTF8String ("Impossible de trouver le premier sommet d'une courbe (au sens OCC)", TkUtil::Charset::UTF_8));
}
/*------------------------------------------------------------------------*/
Vertex* Curve::secondPoint() const
{
	TopoDS_Edge e = TopoDS::Edge(m_shape);
	TopoDS_Vertex v = TopExp::LastVertex(e);

	int index = -1;
	for(unsigned int i=0;i<m_vertices.size();i++){
		Vertex* vi = m_vertices[i];
		TopoDS_Shape vi_shape = vi->getShape();

		TopoDS_Vertex occ_vi = TopoDS::Vertex(vi_shape);
		if (OCCHelper::areEquals(v, occ_vi)){
			return vi;
		}
	}
	throw TkUtil::Exception(TkUtil::UTF8String ("Impossible de trouver le second sommet d'une courbe (au sens OCC)", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void Curve::bounds(Surface* ASurf, double& ATMin, double& ATMax) const
{
	TopoDS_Edge e = TopoDS::Edge(m_shape);
	TopoDS_Face f = TopoDS::Face(ASurf->getShape());

	BRep_Tool::CurveOnSurface(e, f, ATMin, ATMax);
}
/*----------------------------------------------------------------------------*/
void Curve::
getParameter2D(Surface* ASurf, const Utils::Math::Point& Pt, double& p) const
{
	TopoDS_Edge e = TopoDS::Edge(m_shape);
	TopoDS_Face f = TopoDS::Face(ASurf->getShape());

	//WE GET THE 2D CURVE
	double t_min, t_max;
	Handle_Geom2d_Curve parametric_curve = BRep_Tool::CurveOnSurface(e, f, t_min, t_max);

	//WE PROJECT THE POINT ON THE SURFACE
	Handle_Geom_Surface surf_3d = BRep_Tool::Surface(f);
	gp_Pnt to_project(Pt.getX(),Pt.getY(),Pt.getZ());
	GeomAPI_ProjectPointOnSurf surf_proj(to_project,surf_3d);
	//WE GET THE U, V parameters of the point in the surface
	double u,v;
	surf_proj.Parameters(1,u,v);

 	gp_Pnt2d point_2D(u,v);
	Geom2dAPI_ProjectPointOnCurve proj2d(point_2D, parametric_curve);
	p = proj2d.Parameter(1);
}
/*----------------------------------------------------------------------------*/
void Curve::parametricData(Surface* ASurf, double& AT,  double AUV[2],
		double ADT[2],
		double ADTT[2]) const
{
	TopoDS_Edge e = TopoDS::Edge(m_shape);
	TopoDS_Face f = TopoDS::Face(ASurf->getShape());

	double t_min, t_max;
	Handle_Geom2d_Curve parametric_curve = BRep_Tool::CurveOnSurface(e, f, t_min, t_max);

	// We get the 2D coordinate of the point with parameter AT
	gp_Pnt2d pnt = parametric_curve->Value(AT);
	AUV[0] = pnt.X();
	AUV[1] = pnt.Y();
	// We get the first derivative in the point with parameter AT
	gp_Vec2d dt = parametric_curve->DN(AT,1);
	ADT[0] = dt.X();
	ADT[1] = dt.Y();
	// We get the second derivative in the point with parameter AT
	gp_Vec2d dtt = parametric_curve->DN(AT,2);
	ADTT[0] = dtt.X();
	ADTT[1] = dtt.Y();
}
/*------------------------------------------------------------------------*/
GeomOrientation Curve::orientation() const
{
	TopoDS_Edge e = TopoDS::Edge(m_shape);

	GeomOrientation value;
	switch(e.Orientation())
	{
	case(TopAbs_FORWARD):
			 value = GEOM_FORWARD;
	break;
	case(TopAbs_REVERSED):
			  value = GEOM_REVERSED;
	break;
	case(TopAbs_INTERNAL):
			  value = GEOM_INTERNAL;
	break;
	case(TopAbs_EXTERNAL):
			  value = GEOM_EXTERNAL;
	break;
	};
	return value;
}
/*------------------------------------------------------------------------*/
bool Curve::isInternal() const
{
	TopoDS_Edge e = TopoDS::Edge(m_shape);
	return (e.Orientation()==TopAbs_INTERNAL);
}
/*----------------------------------------------------------------------------*/
void Curve::add(Surface* s)
{
	m_surfaces.push_back(s);
}
/*----------------------------------------------------------------------------*/
void Curve::remove(Surface* s)
{
    std::vector<Surface*>::iterator it= m_surfaces.begin();
    while(it!=m_surfaces.end() && *it!=s)
        it++;

    if(it!=m_surfaces.end())
        m_surfaces.erase(it);
}
/*----------------------------------------------------------------------------*/
void Curve::add(Vertex* v)
{
   m_vertices.push_back(v);
}
/*----------------------------------------------------------------------------*/
void Curve::remove(Vertex* v)
{
    std::vector<Vertex*>::iterator it= m_vertices.begin();
    while(it!=m_vertices.end() && *it!=v)
        it++;

    if(it!=m_vertices.end())
        m_vertices.erase(it);
}
/*----------------------------------------------------------------------------*/
void Curve::split(std::vector<Vertex* >&  vert)
{
#ifdef _DEBUG
	std::cout<<"Curve::split avec vert.size () = "<<vert.size ()<<std::endl;
#endif

	if (useOCAF()){
		m_rootLabel = m_label;
	}

    /* on va explorer la courbe OCC stockée en attribut et créer les entités de
     * dimension directement inférieure, c'est-à-dire les sommets
     */
	Vertex* v = 0;
	TopExp_Explorer e;
    for(e.Init(m_shape, TopAbs_VERTEX); e.More(); e.Next())
    {

        TopoDS_Vertex V = TopoDS::Vertex(e.Current());
        // on évite de mettre 2 fois le même sommet [EB]
        bool are_same = false;
        if (v){
        	TopoDS_Vertex Vprec = TopoDS::Vertex(v->getShape());
        	if (Vprec.IsSame(V))
        		are_same = true;
        }

        if (!are_same){
        	// création du nouveau sommet
        	v = EntityFactory(getContext()).newOCCVertex(V);

        	// on crée le lien C->V
        	this->add(v);
        	// on crée le lien V->C
        	v->add(this);
        }
    }

    // on renseigne la fonction appelante
    this->get(vert);

    if (useOCAF())
     	m_rootLabel = EntityFactory::getOCAFRootLabel();
}
/*----------------------------------------------------------------------------*/
double Curve::computeArea() const
{
	// pour éviter un plantage dans LinearProperties pour le cas d'une arête dégénérée
	if(m_shape.ShapeType()==TopAbs_EDGE){
		TopoDS_Edge e = TopoDS::Edge(m_shape);
		if (BRep_Tool::Degenerated(e))
			return 0.0;
	}

    //Fonctionne aussi bien pour une topoDS_Edge qu'un TopoDS_Wire
    BRepCheck_Analyzer anaAna(m_shape, Standard_False);

    GProp_GProps pb;
    BRepGProp::LinearProperties(m_shape,pb);

    double res = pb.Mass();
    return res;
}
/*----------------------------------------------------------------------------*/
void Curve::addAllDownLevelEntity(std::list<GeomEntity*>& l_entity) const
{
    std::vector<Vertex*>::const_iterator it;
    for(it = m_vertices.begin();it!=m_vertices.end();it++){
        l_entity.push_back(*it);
    }
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Curve::
getCenteredPosition() const
{
    Utils::Math::Point pt;
    if (m_vertices.empty())
    	return pt;

    std::vector<Vertex*>::const_iterator it;
    for(it = m_vertices.begin();it!=m_vertices.end();it++){

        pt += (*it)->getCenteredPosition();
    }

    pt /= (double)m_vertices.size();

    project(pt);
    return pt;
}
/*----------------------------------------------------------------------------*/
bool Curve::isA(const std::string& name)
{
    return (name.compare(0,getTinyName().size(),getTinyName()) == 0);
}
/*----------------------------------------------------------------------------*/
void Curve::add(Group::Group1D* grp)
{
    //std::cout<<"Curve::add("<<grp->getName()<<") à "<<getName()<<std::endl;
    m_groups.push_back(grp);
}
/*----------------------------------------------------------------------------*/
void Curve::remove(Group::Group1D* grp)
{
    uint i = 0;
    for (; i<m_groups.size() && grp != m_groups[i]; ++i)
        ;

    if (i!=m_groups.size())
        m_groups.erase(m_groups.begin()+i);
    else
        throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne (pas de groupe), avec Curve::remove", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
bool Curve::find(Group::Group1D* grp)
{
    uint i = 0;
    for (; i<m_groups.size() && grp != m_groups[i]; ++i)
        ;

    return (i!=m_groups.size());
}
/*----------------------------------------------------------------------------*/
void Curve::getGroupsName (std::vector<std::string>& gn) const
{
    gn.clear();
    for (uint i = 0; i<m_groups.size(); ++i)
        gn.push_back(m_groups[i]->getName());
}
/*----------------------------------------------------------------------------*/
void Curve::getGroups(std::vector<Group::GroupEntity*>& grp) const
{
    grp.insert(grp.end(), m_groups.begin(), m_groups.end());
}
/*----------------------------------------------------------------------------*/
void Curve::setGroups(std::vector<Group::GroupEntity*>& grp)
{
	m_groups.clear();
	for (std::vector<Group::GroupEntity*>::iterator iter = grp.begin(); iter != grp.end(); iter++){
		Group::Group1D* g1 = dynamic_cast<Group::Group1D*>(*iter);
		if (g1 == 0)
			throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne avec conversion en groupe local dans setGroups", TkUtil::Charset::UTF_8));
		m_groups.push_back(g1);
	}
}
/*----------------------------------------------------------------------------*/
int Curve::getNbGroups() const
{
    return m_groups.size();
}
/*----------------------------------------------------------------------------*/
void Curve::setDestroyed(bool b)
{
    if (isDestroyed() == b)
        return;

    // supprime la relation du groupe vers la courbe en cas de destruction
    if (b)
        for (uint i = 0; i<m_groups.size(); ++i)
            m_groups[i]->remove(this);
    else
        // et inversement en cas de ressurection
        for (uint i = 0; i<m_groups.size(); ++i)
            m_groups[i]->add(this);

    Entity::setDestroyed(b);
}
/*----------------------------------------------------------------------------*/
bool Curve::isLinear() const
{
	if(m_shape.ShapeType()== TopAbs_EDGE){
		TopoDS_Edge edge = TopoDS::Edge(m_shape);

		Standard_Real first_param, last_param;
		Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);

		if (curve.IsNull())
			return false;
		if(curve->DynamicType()==STANDARD_TYPE(Geom_Line))
			return true;

	}
	else  if(m_shape.ShapeType()== TopAbs_WIRE){

		TopTools_IndexedMapOfShape  mapE;
		TopExp::MapShapes(m_shape,TopAbs_EDGE, mapE);
		for(int i=1;i<=mapE.Extent();i++){
			TopoDS_Edge edge = TopoDS::Edge(mapE.FindKey(i));
			Standard_Real first_param, last_param;
			Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);
			if(curve.IsNull() || curve->DynamicType()!=STANDARD_TYPE(Geom_Line))
				return false;
		}
		return true;
	}

    return false;
}
/*----------------------------------------------------------------------------*/
bool Curve::isCircle() const
{
	if(m_shape.ShapeType()!= TopAbs_EDGE)
	{
		return false;
		//on peut avoir un wire formant un cercle mais cela ne sera pas
		//considéré comme un cercle
	}


	TopoDS_Edge edge = TopoDS::Edge(m_shape);

	Standard_Real first_param, last_param;
	Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);
	if (curve.IsNull())
		return false;


	if(curve->DynamicType()==STANDARD_TYPE(Geom_Circle))
		return true;

    return false;
}
/*----------------------------------------------------------------------------*/
bool Curve::isEllipse() const
{
	if(m_shape.ShapeType()!= TopAbs_EDGE)
	{
		return false;
		//on peut avoir un wire formant une ellipse mais cela ne sera pas
		//considéré comme un cercle
	}

	TopoDS_Edge edge = TopoDS::Edge(m_shape);

	Standard_Real first_param, last_param;
	Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);
	if (curve.IsNull())
		return false;


	if(curve->DynamicType()==STANDARD_TYPE(Geom_Ellipse))
		return true;

    return false;
}
/*----------------------------------------------------------------------------*/
bool Curve::isBSpline() const
{
	if(m_shape.ShapeType()!= TopAbs_EDGE)
	{
		return false;
	}

	TopoDS_Edge edge = TopoDS::Edge(m_shape);

	Standard_Real first_param, last_param;
	Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);
	if (curve.IsNull())
		return false;


	if(curve->DynamicType()==STANDARD_TYPE(Geom_BSplineCurve))
		return true;

    return false;
}
/*----------------------------------------------------------------------------*/
bool Curve::isWire() const
{
	if (m_shape.ShapeType()== TopAbs_WIRE)
		return true;
	else
	    return false;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Curve::getCenter() const
{
    //std::cout<<"Curve::getCenter()  pour "<<getName()<<std::endl;

    if(m_shape.ShapeType()!= TopAbs_EDGE)
    {
        throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne (pas de shape OCC), avec Curve::getCenter", TkUtil::Charset::UTF_8));
        //on peut avoir un wire !!!
    }

    TopoDS_Edge edge = TopoDS::Edge(m_shape);

    Standard_Real first_param, last_param;
    Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);
    if (curve.IsNull())
    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, curve non reconnue, avec Curve::getCenter", TkUtil::Charset::UTF_8));

    if (curve->DynamicType()==STANDARD_TYPE(Geom_Ellipse)){
        Handle(Geom_Ellipse) ellipse = Handle(Geom_Ellipse)::DownCast(curve);
        gp_Elips elips = ellipse->Elips();
        const gp_Pnt& loc = elips.Location();

        return Utils::Math::Point(loc.X(), loc.Y(), loc.Z());

    } else if (curve->DynamicType()==STANDARD_TYPE(Geom_Circle)){
        Handle(Geom_Circle) circle = Handle(Geom_Circle)::DownCast(curve);
        gp_Circ circ = circle->Circ();
        const gp_Ax2& ax = circ.Position();
        const gp_Pnt& loc = ax.Location();

        return Utils::Math::Point(loc.X(), loc.Y(), loc.Z());
    }
    else
        throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, type non prévu, avec Curve::getCenter", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Curve::getDescription (bool alsoComputed) const
{
	std::unique_ptr<Utils::SerializedRepresentation>	description	(
			GeomEntity::getDescription (alsoComputed));
	CHECK_NULL_PTR_ERROR (description.get ( ))

    Utils::SerializedRepresentation  propertyGeomDescription (
                                                "Propriétés géométriques", "");

	if (true == alsoComputed)
	{
		//recuperation de la longueur
		TkUtil::UTF8String volStr (TkUtil::Charset::UTF_8);
		volStr<<getArea();

		propertyGeomDescription.addProperty (
	        Utils::SerializedRepresentation::Property ("Longueur", volStr.ascii()) );
	}
	
#ifdef _DEBUG		// Issue#111
    // précision OpenCascade
	TkUtil::UTF8String precStr (TkUtil::Charset::UTF_8);
	precStr << getPrecision();
	propertyGeomDescription.addProperty (
				Utils::SerializedRepresentation::Property ("Précision", precStr.ascii()) );
#endif	// _DEBUG

    // on ajoute des infos du style: c'est une droite, un arc de cercle, une ellipse, une b-spline
    TkUtil::UTF8String typeStr (TkUtil::Charset::UTF_8);
	bool isABSpline = false;
	bool isAWire = false;
    if (isLinear())
    	typeStr<<"segment";
    else if (isCircle())
    	typeStr<<"cercle";
    else if (isEllipse())
    	typeStr<<"ellipse";
    else if (isBSpline()){
    	isABSpline = true;
    	typeStr<<"b-spline";
    }
    else if (isWire()){
    	typeStr<<"wire";
    	isAWire = true;
    }
   	typeStr<<"quelconque";

    propertyGeomDescription.addProperty (
    		Utils::SerializedRepresentation::Property ("Type", typeStr));

#ifdef _DEBUG
    if (isABSpline){
		TkUtil::UTF8String	nbStr1 (TkUtil::Charset::UTF_8);
		TkUtil::UTF8String	nbStr2 (TkUtil::Charset::UTF_8);

		if (m_shape.ShapeType()==TopAbs_EDGE){
			Standard_Real first_param, last_param;
			TopoDS_Edge edge = TopoDS::Edge(m_shape);
			Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);

			Handle(Geom_BSplineCurve) bspline = Handle(Geom_BSplineCurve)::DownCast(curve);

			nbStr1<<(long int)bspline->NbKnots();
			nbStr2<<(long int)bspline->NbPoles();

	    	propertyGeomDescription.addProperty (
	    			Utils::SerializedRepresentation::Property ("Nb noeuds", nbStr1.ascii()) );
	    	propertyGeomDescription.addProperty (
	    			Utils::SerializedRepresentation::Property ("Nb poles", nbStr2.ascii()) );
		}
    }

    if (isAWire){
		TkUtil::UTF8String	nbStr1 (TkUtil::Charset::UTF_8);
		TkUtil::UTF8String	nbStr2 (TkUtil::Charset::UTF_8);

		if (m_shape.ShapeType()==TopAbs_WIRE){
			TopoDS_Wire aWire= TopoDS::Wire(m_shape);
			TopExp_Explorer ex;
			uint nb_edges = 0;
			for (ex.Init(aWire,TopAbs_EDGE); ex.More(); ex.Next()){ // pas trouvé de "size"
				nb_edges++;
			}
			nbStr1<<(long int)nb_edges;
			nbStr2<<(long int)m_vertices.size();
	    	propertyGeomDescription.addProperty (
	    			Utils::SerializedRepresentation::Property ("Nb edges", nbStr1.ascii()) );
	    	propertyGeomDescription.addProperty (
	    			Utils::SerializedRepresentation::Property ("Nb vertices", nbStr2.ascii()) );
		}
    }

    // affichage des paramètres extrémas
    double first, last;
    getParameters(first, last);
    TkUtil::UTF8String param1 (TkUtil::Charset::UTF_8);
    param1<<first;
    TkUtil::UTF8String param2 (TkUtil::Charset::UTF_8);
    param2<<last;
    propertyGeomDescription.addProperty (
        		Utils::SerializedRepresentation::Property ("Param first", param1.ascii()) );
    propertyGeomDescription.addProperty (
        		Utils::SerializedRepresentation::Property ("Param last", param2.ascii()) );
#endif

    description->addPropertiesSet (propertyGeomDescription);

	return description.release ( );
}
/*----------------------------------------------------------------------------*/
void Curve::computeParams(Utils::Math::Point ptStart)
{
//#define _DEBUG_PARAMS

	paramImgFirst.clear();
	paramImgLast.clear();
	paramLocFirst.clear();
	paramLocLast.clear();

	// NB, on ne fait rien si la courbe n'est pas composite

	// on renseigne les paramLocFirst et autres

	// pour cela on commence par calculer les longueurs des différentes parties
	std::vector<double> areasLoc;
	double areaTot = 0.0;
	std::vector<GeomRepresentation*> reps = getComputationalProperties();
	if (reps.size() == 1)
		return;

	for (uint i=0; i<reps.size(); i++){
		double area = reps[i]->computeCurveArea();
		areasLoc.push_back(area);
		areaTot+=area;
	}

	// calcul des paramImgFirst et paramImgLast
	double areaI = 0.0;
	double paramI = 0.0;
	for (uint i=0; i<areasLoc.size(); i++){
		paramImgFirst.push_back(paramI);
		areaI += areasLoc[i];
		paramI = areaI/areaTot;
		paramImgLast.push_back(paramI);
	}

	// epsilon relatif à la longueur totale
	double epsilon = areaTot*Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon*10;

#ifdef _DEBUG_PARAMS
	std::cout<<"Curve::computeParams ptStart :"<<ptStart<<std::endl;
	std::cout<<"epsilon loc = "<<epsilon<<std::endl;
	std::cout<<"areasLoc:";
	for (uint i=0; i<areasLoc.size(); i++)
		std::cout<<" "<<areasLoc[i];
	std::cout<<std::endl;
	std::cout<<"paramImgFirst:";
	for (uint i=0; i<paramImgFirst.size(); i++)
		std::cout<<" "<<paramImgFirst[i];
	std::cout<<std::endl;
	std::cout<<"paramImgLast:";
	for (uint i=0; i<paramImgLast.size(); i++)
		std::cout<<" "<<paramImgLast[i];
	std::cout<<std::endl;
	std::cout<<"couples de points: "<<std::endl;
	Utils::Math::Point ptBegin, ptEnd;
	for (uint i=0; i<reps.size(); i++){
		reps[i]->getPoint(0.0, ptBegin, true);
		reps[i]->getPoint(1.0, ptEnd, true);
		std::cout<<ptBegin<<" "<<ptEnd<<std::endl;
	}
#endif

	// remplissage des paramLocFirst et paramLocLast et tenant compte du sens
	Utils::Math::Point ptPrec;
	reps[0]->getPoint(0.0, ptPrec, true);
#ifdef _DEBUG_PARAMS
	std::cout<<"longueur ptStart-ptPrec "<<ptPrec.length(ptStart)<<std::endl;
#endif
	if (not (ptPrec.isEpsilonEqual(ptStart, epsilon))){
		reps[0]->getPoint(1.0, ptPrec, true);
#ifdef _DEBUG_PARAMS
		std::cout<<"longueur ptStart-ptPrec "<<ptPrec.length(ptStart)<<std::endl;
#endif
		if (not (ptPrec.isEpsilonEqual(ptStart, epsilon))){
			throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, Courbe composite où on ne retrouve pas le premier sommet", TkUtil::Charset::UTF_8));
		}
	}
	for (uint i=0; i<reps.size(); i++){
		double inverse = false;
		Utils::Math::Point pt1, pt2;
		reps[i]->getPoint(0.0, pt1, true);
		reps[i]->getPoint(1.0, pt2, true);
#ifdef _DEBUG_PARAMS
		std::cout<<"i="<<i<<" ptPrec: "<<ptPrec<<std::endl;
		std::cout<<"longueur pt1-ptPrec "<<ptPrec.length(pt1)<<" pt2-ptPrec "<<ptPrec.length(pt2)<<std::endl;
#endif
		if (not (pt1.isEpsilonEqual(ptPrec, epsilon))){
			inverse = true;
			if (not (pt2.isEpsilonEqual(ptPrec, epsilon))){
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, Courbe composite où on ne retrouve pas le sommet précédent", TkUtil::Charset::UTF_8));
			}
		}

		double first, last;
		if (inverse){
			reps[i]->getParameters(last, first);
			ptPrec = pt1;
		}
		else {
			ptPrec = pt2;
			reps[i]->getParameters(first, last);
		}
		paramLocFirst.push_back(first);
		paramLocLast.push_back(last);
	}
#ifdef _DEBUG_PARAMS
	std::cout<<"paramLocFirst:";
	for (uint i=0; i<paramLocFirst.size(); i++)
		std::cout<<" "<<paramLocFirst[i];
	std::cout<<std::endl;
	std::cout<<"paramLocLast:";
	for (uint i=0; i<paramLocLast.size(); i++)
		std::cout<<" "<<paramLocLast[i];
	std::cout<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
