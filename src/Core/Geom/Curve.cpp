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
#include "Group/Group1D.h"
#include "Geom/EntityFactory.h"
#include "Geom/MementoGeomEntity.h"
#include "Internal/Context.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/FacetedCurve.h"

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
#include <BRepAdaptor_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
const char* Curve::typeNameGeomCurve = "GeomCurve";
/*----------------------------------------------------------------------------*/
Curve::Curve(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
        GeomProperty* gprop, GeomRepresentation* compProp)
:GeomEntity(ctx, prop, disp, gprop,compProp)
{
}
/*----------------------------------------------------------------------------*/
Curve::Curve(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, std::vector<GeomRepresentation*>& compProp)
:GeomEntity(ctx, prop, disp, gprop,compProp)
{
}
/*----------------------------------------------------------------------------*/
GeomEntity* Curve::clone(Internal::Context& c)
{
	std::vector<GeomRepresentation*> newGeomRep;
	std::vector<GeomRepresentation*> oldGeomRep = this->getComputationalProperties();

	for (uint i=0; i<oldGeomRep.size(); i++)
		newGeomRep.push_back(oldGeomRep[i]->clone());
	Curve* newCrv = new Curve(c,
            c.newProperty(this->getType()),
            c.newDisplayProperties(this->getType()),
            new GeomProperty(),
			newGeomRep);

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
    OCCGeomRepresentation* rep1 = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
    OCCGeomRepresentation* rep2 = dynamic_cast<OCCGeomRepresentation*>(curve->getComputationalProperty());
    CHECK_NULL_PTR_ERROR(rep1);
    CHECK_NULL_PTR_ERROR(rep2);
    TopoDS_Shape sh1 = rep1->getShape();
    TopoDS_Shape sh2 = rep2->getShape();

    if(sh1.ShapeType()==TopAbs_EDGE && sh2.ShapeType()==TopAbs_EDGE)
    {
        TopoDS_Edge e1 = TopoDS::Edge(rep1->getShape());
        TopoDS_Edge e2 = TopoDS::Edge(rep2->getShape());
        return OCCGeomRepresentation::areEquals(e1,e2);
    }
    else if(sh1.ShapeType()==TopAbs_WIRE && sh2.ShapeType()==TopAbs_WIRE)
    {
        TopoDS_Wire w1 = TopoDS::Wire(rep1->getShape());
        TopoDS_Wire w2 = TopoDS::Wire(rep2->getShape());
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
//    if(getName()=="Surf0025" && ASurf->getName()=="Surf0076"){
//        std::cout<<"ENCORE DU DEBUG"<<std::endl;
//    }

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
    OCCGeomRepresentation* rep =
            dynamic_cast<OCCGeomRepresentation*>(ACurve->getComputationalProperty());
	CHECK_NULL_PTR_ERROR(rep);
    TopoDS_Shape shOther = rep->getShape();

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
	if (getComputationalProperties().size() == 1)
		getComputationalProperty()->project(P,this);
	else {
		// on va prendre la projection la plus courte pour le cas composé
		Utils::Math::Point pInit = P;

		std::vector<GeomRepresentation*> reps = getComputationalProperties();
		reps[0]->project(P,this);
		Utils::Math::Point pBest = P;
		double norme2 = (P-pInit).norme2();
		//std::cout<<"pBest : "<<pBest<<", norme2 : "<<norme2<<std::endl;
		for (uint i=1; i<reps.size(); i++){
			P = pInit;
			reps[i]->project(P,this);
			double dist = (P-pInit).norme2();
			//std::cout<<"P : "<<P<<", dist : "<<dist<<std::endl;
			if (dist<norme2){
				//std::cout<<"pBest = P"<<std::endl;
				norme2 = dist;
				pBest = P;
			}
		}
		P = pBest;
	}
}
/*----------------------------------------------------------------------------*/
void Curve::project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const
{
	if (getComputationalProperties().size() == 1)
		getComputationalProperty()->project(P1,P2,this);
	else {
		// on va prendre la projection la plus courte pour le cas composé

		std::vector<GeomRepresentation*> reps = getComputationalProperties();
		reps[0]->project(P1,P2,this);
		Utils::Math::Point pBest = P2;
		double norme2 = (P2-P1).norme2();
		for (uint i=1; i<reps.size(); i++){
			reps[i]->project(P1,P2,this);
			double dist = (P2-P1).norme2();
			if (dist<norme2){
				norme2 = dist;
				pBest = P2;
			}
		}
		P2 = pBest;
	}
}
/*----------------------------------------------------------------------------*/
void Curve::
getPoint(const double& p, Utils::Math::Point& Pt, const bool in01) const
{
	if (getComputationalProperties().size() == 1)
		getComputationalProperty()->getPoint(p, Pt, in01);
	else {
		// vérification que computeParams a bien été utilisé
		checkParams();

		// on cherche la section paramètrée correspondante
		if (p<0.0 || p>1.0)
	    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, le paramètre doit être dans l'interval [0,1] pour les courbes composées", TkUtil::Charset::UTF_8));

		uint ind = 0;
		for (; ind<paramImgLast.size() && p>paramImgLast[ind]; ind++)
			;
		if (ind>=paramImgLast.size())
			throw TkUtil::Exception("Erreur interne, l'indice est en dehors des bornes");

		double ratio = (p-paramImgFirst[ind])/(paramImgLast[ind]-paramImgFirst[ind]);

		double paramLoc = paramLocFirst[ind]+ratio*(paramLocLast[ind]-paramLocFirst[ind]);

		std::vector<GeomRepresentation*> reps = getComputationalProperties();
		reps[ind]->getPoint(paramLoc, Pt, false);
	}
}
/*----------------------------------------------------------------------------*/
void Curve::tangent(const Utils::Math::Point& P1, Utils::Math::Vector& V2) const
{
	if (getComputationalProperties().size() == 1)
		getComputationalProperty()->tangent(P1, V2);
	else {
		// on va prendre la projection la plus courte pour le cas composé et utiliser la tangente associée
		Utils::Math::Point P2;
		std::vector<GeomRepresentation*> reps = getComputationalProperties();
		reps[0]->project(P1,P2,this);
		Utils::Math::Point pBest = P2;
		uint idBest=0;
		double norme2 = (P2-P1).norme2();
		for (uint i=1; i<reps.size(); i++){
			reps[i]->project(P1,P2,this);
			double dist = (P2-P1).norme2();
			if (dist<norme2){
				norme2 = dist;
				pBest = P2;
				idBest = i;
			}
		}
		P2 = pBest;
		reps[idBest]->tangent(P1, V2);
	}
}
/*----------------------------------------------------------------------------*/
void Curve::getIntersection(gp_Pln& plan_cut, Utils::Math::Point& Pt) const
{
	if (getComputationalProperties().size() == 1)
		getComputationalProperty()->getIntersection(plan_cut, Pt);
	else {
		MGX_NOT_YET_IMPLEMENTED("intersection avec un plan non implémentée pour le cas composée (utilisé pour discrétisation polaire)");
	}

}
/*----------------------------------------------------------------------------*/
void Curve::getParameter(const Utils::Math::Point& Pt, double& p) const
{
    //std::cout<<setprecision(14)<<"Curve::getParameter pour pt "<<Pt<<std::endl;
	if (getComputationalProperties().size() == 1)
		getComputationalProperty()->getParameter(Pt, p, this);
	else {
		// vérification que computeParams a bien été utilisé
		checkParams();

		// en général on cherche les points aux extrémités... mais ils ne sont pas toujours renseignés
		// c'est le cas d'une arête projetée sur une surface composite
		std::vector<Vertex*> vertices;
		get(vertices);
//		if (vertices.size() != 2 && vertices.size() != 1)
//			throw TkUtil::Exception("Erreur interne, Courbe composite avec autre chose que 1 ou 2 sommets");
		if (vertices.size() >= 1 && vertices[0]->getPoint() == Pt){
			p=0.0;
			return;
		}
		else if (vertices.size() == 2 && vertices[1]->getPoint() == Pt){
			p=1.0;
			return;
		}

		std::vector<GeomRepresentation*> reps = getComputationalProperties();
		for (uint ind=0; ind<reps.size(); ind++){

			try{
				double paramLoc = 0.0;
				reps[ind]->getParameter(Pt, paramLoc, this);
				//std::cout<<" paramLoc "<<paramLoc<<std::endl;

				double ratio = (paramLoc-paramLocFirst[ind])/(paramLocLast[ind]-paramLocFirst[ind]);
				p = paramImgFirst[ind]+ratio*(paramImgLast[ind]-paramImgFirst[ind]);
				//std::cout<<" p "<<p<<std::endl;
				return;
			}
			catch(TkUtil::Exception &e){

			}
		}

    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, le point n'a pas permis de trouver un paramètre sur l'une des courbes", TkUtil::Charset::UTF_8));
	}
    //std::cout<<"  => p = "<<p<<std::endl;
}
/*----------------------------------------------------------------------------*/
void Curve::getParameters(double& first, double& last) const
{
	if (getComputationalProperties().size() == 1)
		getComputationalProperty()->getParameters(first, last);
	else {
		// vérification que computeParams a bien été utilisé
		checkParams();
		first = 0.0;
		last = 1.0;
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
        getComputationalProperty()->getParameters(first, last);


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
#ifdef _DEBUG2
    std::cout<<"Curve::getParametricsPointsLoops pour "<<nbPt<<" points avec la courbe "
            << getName()<<", qui possède "<<m_vertices.size()<<" sommets."<<std::endl;
    if (m_vertices.size() == 2){
        std::cout<<" m_vertices[0] : "<<setprecision(14)<<m_vertices[0]->gedouble()<<std::endl;
        std::cout<<" m_vertices[1] : "<<setprecision(14)<<m_vertices[1]->gedouble()<<std::endl;
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
	OCCGeomRepresentation* this_rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
	CHECK_NULL_PTR_ERROR(this_rep);
	TopoDS_Shape this_shape = this_rep->getShape();

	TopoDS_Edge e = TopoDS::Edge(this_shape);
	TopoDS_Vertex v = TopExp::FirstVertex(e);

	int index = -1;
	for(unsigned int i=0;i<m_vertices.size();i++){
		Vertex* vi = m_vertices[i];
		OCCGeomRepresentation* vi_rep = dynamic_cast<OCCGeomRepresentation*>(vi->getComputationalProperty());
		CHECK_NULL_PTR_ERROR(vi_rep);
		TopoDS_Shape vi_shape = vi_rep->getShape();

		TopoDS_Vertex occ_vi = TopoDS::Vertex(vi_shape);
		if (OCCGeomRepresentation::areEquals(v, occ_vi)){
			return vi;
		}
	}
	throw TkUtil::Exception(TkUtil::UTF8String ("Impossible de trouver le premier sommet d'une courbe (au sens OCC)", TkUtil::Charset::UTF_8));
}
/*------------------------------------------------------------------------*/
Vertex* Curve::secondPoint() const
{
	OCCGeomRepresentation* this_rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
	CHECK_NULL_PTR_ERROR(this_rep);
	TopoDS_Shape this_shape = this_rep->getShape();

	TopoDS_Edge e = TopoDS::Edge(this_shape);
	TopoDS_Vertex v = TopExp::LastVertex(e);

	int index = -1;
	for(unsigned int i=0;i<m_vertices.size();i++){
		Vertex* vi = m_vertices[i];
		OCCGeomRepresentation* vi_rep = dynamic_cast<OCCGeomRepresentation*>(vi->getComputationalProperty());
		CHECK_NULL_PTR_ERROR(vi_rep);
		TopoDS_Shape vi_shape = vi_rep->getShape();

		TopoDS_Vertex occ_vi = TopoDS::Vertex(vi_shape);
		if (OCCGeomRepresentation::areEquals(v, occ_vi)){
			return vi;
		}
	}
	throw TkUtil::Exception(TkUtil::UTF8String ("Impossible de trouver le second sommet d'une courbe (au sens OCC)", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void Curve::bounds(Surface* ASurf, double& ATMin, double& ATMax) const
{
	OCCGeomRepresentation* this_rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
	OCCGeomRepresentation* surf_rep = dynamic_cast<OCCGeomRepresentation*>(ASurf->getComputationalProperty());
	CHECK_NULL_PTR_ERROR(this_rep);
	CHECK_NULL_PTR_ERROR(surf_rep);
	TopoDS_Shape this_shape = this_rep->getShape();
	TopoDS_Shape surf_shape = surf_rep->getShape();

	TopoDS_Edge e = TopoDS::Edge(this_shape);
	TopoDS_Face f = TopoDS::Face(surf_shape);

	BRep_Tool::CurveOnSurface(e, f, ATMin, ATMax);

}
/*----------------------------------------------------------------------------*/
void Curve::
getParameter2D(Surface* ASurf, const Utils::Math::Point& Pt, double& p) const
{
	OCCGeomRepresentation* this_rep =
			dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
	OCCGeomRepresentation* surf_rep =
			dynamic_cast<OCCGeomRepresentation*>(ASurf->getComputationalProperty());
	CHECK_NULL_PTR_ERROR(this_rep);
	CHECK_NULL_PTR_ERROR(surf_rep);
	TopoDS_Shape this_shape = this_rep->getShape();
	TopoDS_Shape surf_shape = surf_rep->getShape();

	TopoDS_Edge e = TopoDS::Edge(this_shape);
	TopoDS_Face f = TopoDS::Face(surf_shape);

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
	OCCGeomRepresentation* this_rep =
			dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
	OCCGeomRepresentation* surf_rep =
			dynamic_cast<OCCGeomRepresentation*>(ASurf->getComputationalProperty());
	CHECK_NULL_PTR_ERROR(this_rep);
	CHECK_NULL_PTR_ERROR(surf_rep);
	TopoDS_Shape this_shape = this_rep->getShape();
	TopoDS_Shape surf_shape = surf_rep->getShape();

	TopoDS_Edge e = TopoDS::Edge(this_shape);
	TopoDS_Face f = TopoDS::Face(surf_shape);

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
	OCCGeomRepresentation* this_rep =
			dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
	CHECK_NULL_PTR_ERROR(this_rep);
	TopoDS_Shape this_shape = this_rep->getShape();
	TopoDS_Edge e = TopoDS::Edge(this_shape);

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
	OCCGeomRepresentation* this_rep =
			dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
	CHECK_NULL_PTR_ERROR(this_rep);
	TopoDS_Shape this_shape = this_rep->getShape();
	TopoDS_Edge e = TopoDS::Edge(this_shape);

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
	// identification des sommets aux extrémités (ceux vus qu'une unique fois)

	std::vector<GeomRepresentation*> reps = getComputationalProperties();
	if (reps.size() == 1)
		reps[0]->split(vert,this);
	else {
		//std::cout<<"Curve::split avec reps.size() = "<<reps.size()<<std::endl;

		std::vector<TopoDS_Vertex> vtx;
		for (uint i=0; i<reps.size(); i++){
			OCCGeomRepresentation* occ_rep = dynamic_cast<OCCGeomRepresentation*>(reps[i]);
			CHECK_NULL_PTR_ERROR(occ_rep);

			TopExp_Explorer e;
			for(e.Init(occ_rep->getShape(), TopAbs_VERTEX); e.More(); e.Next()){
				TopoDS_Vertex V = TopoDS::Vertex(e.Current());
				vtx.push_back(V);
			}
		}
		//std::cout<<"vtx.size() = "<<vtx.size()<<std::endl;

		TopoDS_Vertex Vdep;
		TopoDS_Vertex Vfin;

		for (uint i=0; i<vtx.size()-1; i++){
			TopoDS_Vertex V1 = vtx[i];
			for (uint j=i+1; j<vtx.size(); j++){
				TopoDS_Vertex V2 = vtx[j];
				if ((!V1.IsNull()) && (!V2.IsNull()) && OCCGeomRepresentation::areEquals(V1,V2)){
					 vtx[i].Nullify();
					 vtx[j].Nullify();
				}
			}
		} // end for i<vtx.size()

		for (uint i=0; i<vtx.size(); i++){
			TopoDS_Vertex V1 = vtx[i];
			if (!V1.IsNull()){
				if (Vdep.IsNull())
					Vdep = V1;
				else if (Vfin.IsNull())
					Vfin = V1;
				else {
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
					messErr << "La courbe "<<getName()<<" est composée de plusieurs parties et on trouve plus de 2 sommets comme extrémité";
					throw TkUtil::Exception(messErr);
				}

			}
		}

		if (!Vdep.IsNull()){
			// création du nouveau sommet
			Vertex* v = EntityFactory(getContext()).newOCCVertex(Vdep);
			// on crée le lien C->V
			add(v);
			// on crée le lien V->C
			v->add(this);
		}

		if (!Vfin.IsNull()){
			Vertex* v = EntityFactory(getContext()).newOCCVertex(Vfin);
			add(v);
			v->add(this);
		}

	} // end else / if (reps.size() == 1)
}
/*----------------------------------------------------------------------------*/
double Curve::computeArea() const
{
	double area = 0.0;
	std::vector<GeomRepresentation*> reps = getComputationalProperties();
	for (uint i=0; i<reps.size(); i++)
		area += reps[i]->computeCurveArea();

	return area;
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
	// on dit que ce n'est pas linéaire dès que c'est composé
	if (getComputationalProperties().size()>1)
		return false;

     OCCGeomRepresentation* rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());

    if(rep){
    	TopoDS_Shape sh = rep->getShape();


    	if(sh.ShapeType()== TopAbs_EDGE){
    		TopoDS_Edge edge = TopoDS::Edge(sh);

    		Standard_Real first_param, last_param;
    		Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);

    		if (curve.IsNull())
    			return false;
    		if(curve->DynamicType()==STANDARD_TYPE(Geom_Line))
    			return true;

    	}
    	else  if(sh.ShapeType()== TopAbs_WIRE){

    		TopTools_IndexedMapOfShape  mapE;
    		TopExp::MapShapes(sh,TopAbs_EDGE, mapE);
    		for(int i=1;i<=mapE.Extent();i++){
    			TopoDS_Edge edge = TopoDS::Edge(mapE.FindKey(i));
    			Standard_Real first_param, last_param;
    			Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);
    			if(curve.IsNull() || curve->DynamicType()!=STANDARD_TYPE(Geom_Line))
    				return false;
    		}
    		return true;
    	}
    }

    return false;
}
/*----------------------------------------------------------------------------*/
bool Curve::isCircle() const
{
	// on dit que ce n'est pas circulaire dès que c'est composé
	if (getComputationalProperties().size()>1)
		return false;

    OCCGeomRepresentation* rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());

    if(rep){
    	TopoDS_Shape sh = rep->getShape();


    	if(sh.ShapeType()!= TopAbs_EDGE)
    	{
    		return false;
    		//on peut avoir un wire formant un cercle mais cela ne sera pas
    		//considéré comme un cercle
    	}


    	TopoDS_Edge edge = TopoDS::Edge(sh);

    	Standard_Real first_param, last_param;
    	Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);
    	if (curve.IsNull())
    		return false;


    	if(curve->DynamicType()==STANDARD_TYPE(Geom_Circle))
    		return true;
    }

    return false;
}
/*----------------------------------------------------------------------------*/
bool Curve::isEllipse() const
{
	// on dit que ce n'est pas circulaire dès que c'est composé
	if (getComputationalProperties().size()>1)
		return false;

    OCCGeomRepresentation* rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());

    if(rep){
    	TopoDS_Shape sh = rep->getShape();


    	if(sh.ShapeType()!= TopAbs_EDGE)
    	{
    		return false;
    		//on peut avoir un wire formant une ellipse mais cela ne sera pas
    		//considéré comme un cercle
    	}

    	TopoDS_Edge edge = TopoDS::Edge(sh);

    	Standard_Real first_param, last_param;
    	Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);
    	if (curve.IsNull())
    		return false;


    	if(curve->DynamicType()==STANDARD_TYPE(Geom_Ellipse))
    		return true;
    }

    return false;
}
/*----------------------------------------------------------------------------*/
bool Curve::isBSpline() const
{
	// on dit que ce n'est pas, dès que c'est composé
	if (getComputationalProperties().size()>1)
		return false;

	OCCGeomRepresentation* rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());

	if(rep){

		TopoDS_Shape sh = rep->getShape();


		if(sh.ShapeType()!= TopAbs_EDGE)
		{
			return false;
		}

		TopoDS_Edge edge = TopoDS::Edge(sh);

		Standard_Real first_param, last_param;
		Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);
		if (curve.IsNull())
			return false;


		if(curve->DynamicType()==STANDARD_TYPE(Geom_BSplineCurve))
			return true;
	}

    return false;
}
/*----------------------------------------------------------------------------*/
bool Curve::isWire() const
{
	// on dit que ce n'est pas, dès que c'est composé
	if (getComputationalProperties().size()>1)
		return false;

	OCCGeomRepresentation* rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());

	if(rep){

		TopoDS_Shape sh = rep->getShape();

		if(sh.ShapeType()== TopAbs_WIRE)
			return true;
	}

    return false;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Curve::getCenter() const
{
    //std::cout<<"Curve::getCenter()  pour "<<getName()<<std::endl;

    OCCGeomRepresentation* rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());

    if(rep==0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne (pas de shape OCC), avec Curve::getCenter", TkUtil::Charset::UTF_8));

    TopoDS_Shape sh = rep->getShape();


    if(sh.ShapeType()!= TopAbs_EDGE)
    {
        throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne (pas de shape OCC), avec Curve::getCenter", TkUtil::Charset::UTF_8));

        //on peut avoir un wire !!!
    }


    TopoDS_Edge edge = TopoDS::Edge(sh);

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

    // précision OpenCascade
	std::vector<GeomRepresentation*> reps = getComputationalProperties();
	for (uint i=0; i<reps.size(); i++){
	    TkUtil::UTF8String precStr (TkUtil::Charset::UTF_8);
		precStr << reps[i]->getPrecision();
	    propertyGeomDescription.addProperty (
	    	        Utils::SerializedRepresentation::Property ("Précision", precStr.ascii()) );
	}

	// recherche des infos pour le cas facétisé
	bool isFaceted = false;
	uint nbNodes = 0;
	if (reps.size() == 1){
		FacetedCurve* fc = dynamic_cast<FacetedCurve*>(reps[0]);
		if (fc){
			isFaceted = true;
			nbNodes = fc->getNbNodes();
		}
	}

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
    else if (getComputationalProperties().size()>1)
    	typeStr<<"composée";
    else if (isFaceted)
    	typeStr<<"facétisée";
    else
    	typeStr<<"quelconque";

    propertyGeomDescription.addProperty (
    		Utils::SerializedRepresentation::Property ("Type", typeStr));

    if (isFaceted){
		TkUtil::UTF8String	nbStr (TkUtil::Charset::UTF_8);
    	nbStr<<(long int)nbNodes;

    	propertyGeomDescription.addProperty (
    			Utils::SerializedRepresentation::Property ("Nb noeuds", nbStr.ascii()) );
    }

#ifdef _DEBUG
    if (isABSpline){
		TkUtil::UTF8String	nbStr1 (TkUtil::Charset::UTF_8);
		TkUtil::UTF8String	nbStr2 (TkUtil::Charset::UTF_8);

		OCCGeomRepresentation* rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
		CHECK_NULL_PTR_ERROR(rep);
		TopoDS_Shape sh = rep->getShape();

		if (sh.ShapeType()==TopAbs_EDGE){
			Standard_Real first_param, last_param;
			TopoDS_Edge edge = TopoDS::Edge(sh);
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

		OCCGeomRepresentation* rep = dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
		CHECK_NULL_PTR_ERROR(rep);
		TopoDS_Shape sh = rep->getShape();

		if (sh.ShapeType()==TopAbs_WIRE){
			TopoDS_Wire aWire= TopoDS::Wire(sh);
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
void Curve::checkParams() const
{
	std::vector<GeomRepresentation*> reps = getComputationalProperties();
	if (reps.size() == 1)
		return;

	if (reps.size() != paramLocFirst.size()){
		TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
		messErr << "Erreur interne, paramLocFirst non itialisé correctement pour "<<getName();
		throw TkUtil::Exception(messErr);
	}
	if (reps.size() != paramLocLast.size())
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, paramLocLast non itialisé correctement", TkUtil::Charset::UTF_8));
	if (reps.size() != paramImgFirst.size())
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, paramImgFirst non itialisé correctement", TkUtil::Charset::UTF_8));
	if (reps.size() != paramImgLast.size())
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, paramImgLast non itialisé correctement", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
bool Curve::needLowerDimensionalEntityModification()
{
	std::vector<GeomRepresentation*> reps = getComputationalProperties();
	if (reps.size() == 1) {
		FacetedCurve *fc = dynamic_cast<FacetedCurve *>(reps[0]);
		if (fc)
			return false;
	}
	return true;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
