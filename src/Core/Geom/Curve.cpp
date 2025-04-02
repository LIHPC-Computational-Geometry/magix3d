/*----------------------------------------------------------------------------*/
#include <list>
#include <string.h>
#include <sys/types.h>
/*----------------------------------------------------------------------------*/
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/EntityFactory.h"
#include "Geom/OCCHelper.h"
#include "Geom/GeomProjectImplementation.h"

#include "Internal/Context.h"

#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"

#include "Group/Group1D.h"
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
#include <BRepAdaptor_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <BRep_Builder.hxx>
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
        GeomProperty* gprop, TopoDS_Edge& shape)
:GeomEntity(ctx, prop, disp, gprop)
{
	m_occ_edges.push_back(shape);
}
/*----------------------------------------------------------------------------*/
Curve::Curve(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, std::vector<TopoDS_Edge>& shapes)
:GeomEntity(ctx, prop, disp, gprop)
, m_occ_edges(shapes)
{
}
/*----------------------------------------------------------------------------*/
void Curve::apply(std::function<void(const TopoDS_Shape&)> const& lambda) const
{
	for (auto sh : m_occ_edges)
		lambda(sh);
}
/*----------------------------------------------------------------------------*/
void Curve::applyAndReturn(std::function<TopoDS_Shape(const TopoDS_Shape&)> const& lambda)
{
	for (int i=0 ; i<m_occ_edges.size() ; ++i) {
		m_occ_edges[i] = TopoDS::Edge(lambda(m_occ_edges[i]));
	}
}
/*----------------------------------------------------------------------------*/
GeomEntity* Curve::clone(Internal::Context& c)
{
	Curve* newCrv = new Curve(c,
            c.newProperty(this->getType()),
            c.newDisplayProperties(this->getType()),
            new GeomProperty(),
			m_occ_edges);

	newCrv->paramLocFirst = paramLocFirst;
	newCrv->paramLocLast = paramLocLast;
	newCrv->paramImgFirst = paramImgFirst;
	newCrv->paramImgLast = paramImgLast;

	return newCrv;
}
/*----------------------------------------------------------------------------*/
void Curve::computeBoundingBox(Utils::Math::Point& pmin, Utils::Math::Point& pmax) const
{
	if (m_occ_edges.empty())
		return;

    OCCHelper::computeBoundingBox(m_occ_edges[0], pmin, pmax);
	for (uint i=1; i<m_occ_edges.size(); i++){
		Utils::Math::Point p1,p2;
        OCCHelper::computeBoundingBox(m_occ_edges[i], pmin, pmax);
		for (uint j=0; j<3; j++){
			double c1 = pmin.getCoord(j);
			double c2 = p1.getCoord(j);
			if (c2<c1)
				pmin.setCoord(j,c2);
		}
		for (uint j=0; j<3; j++){
			double c1 = pmax.getCoord(j);
			double c2 = p2.getCoord(j);
			if (c2>c1)
				pmin.setCoord(j,c2);
		}
	}
}
/*----------------------------------------------------------------------------*/
bool Curve::isEqual(Geom::Curve* curve)
{
	if (m_occ_edges.size() != curve->m_occ_edges.size())
		return false;

	// geometrical representations must be in the same order
	for (int i=0 ; i<m_occ_edges.size() ; ++i)
	{
		TopoDS_Edge e1 = m_occ_edges[i];
		TopoDS_Edge e2 = curve->m_occ_edges[i];
		if (!OCCHelper::areEquals(e1,e2))
			return false;
	}

    return true;
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
	GeomProjectImplementation gpi;
	for (TopoDS_Shape shOther : ACurve->m_occ_edges)
	{
		//===============================================================
		// on teste les sommets
		//===============================================================
		TopTools_IndexedMapOfShape map_vertices;
		TopExp::MapShapes(shOther, TopAbs_VERTEX, map_vertices);
		for(unsigned int i=1; i<=map_vertices.Extent();i++)
		{
			TopoDS_Shape s_i = map_vertices.FindKey(i);
			TopoDS_Vertex v_i = TopoDS::Vertex(s_i);
			gp_Pnt p_i = BRep_Tool::Pnt(v_i);

			Utils::Math::Point pi(p_i.X(),p_i.Y(),p_i.Z());
			Utils::Math::Point proj_i = gpi.project(this, pi).first;
			if(!Utils::Math::MgxNumeric::isNearlyZero(pi.length(proj_i),tol))
				return false;
		}

		//===============================================================
		// On teste les courbes (1er point au 1/3, 2eme au 2/3)
		//===============================================================
		TopTools_IndexedMapOfShape map_edges;
		TopExp::MapShapes(shOther, TopAbs_EDGE, map_edges);
		for(unsigned int i=1; i<=map_edges.Extent();i++)
		{
			TopoDS_Shape s_i = map_edges.FindKey(i);
			TopoDS_Edge e_i = TopoDS::Edge(s_i);
			BRepAdaptor_Curve curve_adaptor(e_i);
			double umin = curve_adaptor.FirstParameter();
			double umax = curve_adaptor.LastParameter();

			double u1 = umin+(umax-umin)/3.0;

			gp_Pnt pnt1 = curve_adaptor.Value(u1);
			Utils::Math::Point pi1(pnt1.X(),pnt1.Y(),pnt1.Z());

			Utils::Math::Point proj_i1 = gpi.project(this, pi1).first;
			if(!Utils::Math::MgxNumeric::isNearlyZero(pi1.length(proj_i1),tol))
			{
				//            std::cout<<"Point  :"<<pi1<<std::endl;
				return false;
			}

			double u2 = umin+(umax-umin)*2.0/3.0;

			gp_Pnt pnt2 = curve_adaptor.Value(u2);
			Utils::Math::Point pi2(pnt2.X(),pnt2.Y(),pnt2.Z());

			Utils::Math::Point proj_i2 = gpi.project(this, pi2).first;
			if(!Utils::Math::MgxNumeric::isNearlyZero(pi2.length(proj_i2),tol))
			{
				//            std::cout<<"Point  :"<<pi2<<std::endl;
				return false;
			}
		}
	}
    return true;
}
/*----------------------------------------------------------------------------*/
void Curve::
getPoint(const double& p, Utils::Math::Point& Pt, const bool in01) const
{
	if (m_occ_edges.size() == 1)
	{
		OCCHelper::getPoint(m_occ_edges[0], p, Pt, in01);
	}
	else
	{
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
		OCCHelper::getPoint(m_occ_edges[ind], paramLoc, Pt, false);
	}
}
/*----------------------------------------------------------------------------*/
void Curve::tangent(const Utils::Math::Point& P1, Utils::Math::Vector& V2) const
{
	// on va prendre la projection la plus courte pour le cas composé et utiliser la tangente associée
	uint idBest = GeomProjectImplementation().project(this, P1).second;
	OCCHelper::tangent(m_occ_edges[idBest], P1, V2);
}
/*----------------------------------------------------------------------------*/
void Curve::getIntersection(gp_Pln& plan_cut, Utils::Math::Point& Pt) const
{
	if (m_occ_edges.size() == 1)
		OCCHelper::getIntersection(m_occ_edges[0], plan_cut, Pt);
	else
		MGX_NOT_YET_IMPLEMENTED("Intersection avec un plan non implémentée pour le cas composé (utilisé pour discrétisation polaire)");
}
/*----------------------------------------------------------------------------*/
void Curve::getParameter(const Utils::Math::Point& Pt, double& p) const
{
    //std::cout<<setprecision(14)<<"Curve::getParameter pour pt "<<Pt<<std::endl;
	if (m_occ_edges.size() == 1)
	{
		OCCHelper::getParameter(m_occ_edges[0], Pt, p);
	}
	else
	{
		// vérification que computeParams a bien été utilisé
		checkParams();

		// en général on cherche les points aux extrémités... mais ils ne sont pas toujours renseignés
		// c'est le cas d'une arête projetée sur une surface composite
//		if (vertices.size() != 2 && vertices.size() != 1)
//			throw TkUtil::Exception("Erreur interne, Courbe composite avec autre chose que 1 ou 2 sommets");
		if (m_vertices.size() >= 1 && m_vertices[0]->getPoint() == Pt){
			p=0.0;
			return;
		}
		else if (m_vertices.size() == 2 && m_vertices[1]->getPoint() == Pt){
			p=1.0;
			return;
		}

		for (uint ind=0; ind<m_occ_edges.size(); ind++){

			try{
				double paramLoc = 0.0;
				OCCHelper::getParameter(m_occ_edges[ind], Pt, paramLoc);
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
	if (m_occ_edges.size() == 1) {
		OCCHelper::getParameters(m_occ_edges[0], first, last);
	} else {
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
		if (m_occ_edges.size() != 1) {
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr << "[Erreur interne] La courbe "<<getName()<<" est une courbe composée ce qui ne permet pas de trouver les points en fonction d'une paramétrisation";
			throw TkUtil::Exception(messErr);
		}
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
#ifdef _DEBUG2
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
	if (m_occ_edges.size() > 0) {
		TopoDS_Edge e = m_occ_edges[0];
		TopoDS_Vertex v = TopExp::FirstVertex(e);

		int index = -1;
		for(unsigned int i=0;i<m_vertices.size();i++){
			Vertex* vi = m_vertices[i];
			auto occ_vi = vi->getOCCVertex();
			if (OCCHelper::areEquals(v, occ_vi)){
					return vi;
			}
		}
	}
	throw TkUtil::Exception(TkUtil::UTF8String ("Impossible de trouver le premier sommet d'une courbe (au sens OCC)", TkUtil::Charset::UTF_8));
}
/*------------------------------------------------------------------------*/
Vertex* Curve::secondPoint() const
{
	if (m_occ_edges.size() > 0) {
		TopoDS_Edge e =  m_occ_edges[m_occ_edges.size()-1];
		TopoDS_Vertex v = TopExp::LastVertex(e);

		int index = -1;
		for(unsigned int i=0;i<m_vertices.size();i++){
			Vertex* vi = m_vertices[i];
			auto occ_vi = vi->getOCCVertex();
			if (OCCHelper::areEquals(v, occ_vi)){
				return vi;
			}
		}
	}
	throw TkUtil::Exception(TkUtil::UTF8String ("Impossible de trouver le second sommet d'une courbe (au sens OCC)", TkUtil::Charset::UTF_8));
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
double Curve::computeArea() const
{
	double area = 0.0;
	for (auto s : m_occ_edges)
		area += OCCHelper::getLength(s);
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

	return GeomProjectImplementation().project(this, pt).first;
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
	if (m_occ_edges.size() == 1)
		return OCCHelper::isTypeOf(m_occ_edges[0], STANDARD_TYPE(Geom_Line));
	else
		// courbe composée : on dit que ce n'est pas une cercle
		return false;
}
/*----------------------------------------------------------------------------*/
bool Curve::isCircle() const
{
	if (m_occ_edges.size() == 1)
		return OCCHelper::isTypeOf(m_occ_edges[0], STANDARD_TYPE(Geom_Circle));
	else
		// courbe composée : on dit que ce n'est pas une cercle
		return false;
}
/*----------------------------------------------------------------------------*/
bool Curve::isEllipse() const
{
	if (m_occ_edges.size() == 1)
		return OCCHelper::isTypeOf(m_occ_edges[0], STANDARD_TYPE(Geom_Ellipse));
	else
		// courbe composée : on dit que ce n'est pas une ellipse
		return false;
}
/*----------------------------------------------------------------------------*/
bool Curve::isBSpline() const
{
	if (m_occ_edges.size() == 1)
		return OCCHelper::isTypeOf(m_occ_edges[0], STANDARD_TYPE(Geom_BSplineCurve));
	else
		// courbe composée : on dit que ce n'est pas une spline
		return false;
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
	for (uint i=0; i<m_occ_edges.size(); i++){
	    TkUtil::UTF8String precStr (TkUtil::Charset::UTF_8);
		precStr << BRep_Tool::Tolerance(m_occ_edges[i]);
	    propertyGeomDescription.addProperty (
	    	        Utils::SerializedRepresentation::Property ("Précision", precStr.ascii()) );
	}
#endif	// _DEBUG

    // on ajoute des infos du style: c'est une droite, un arc de cercle, une ellipse, une b-spline
    TkUtil::UTF8String typeStr (TkUtil::Charset::UTF_8);
	bool isABSpline = false;
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
    else if (m_occ_edges.size()>1)
    	typeStr<<"composée";
    else
    	typeStr<<"quelconque";

    propertyGeomDescription.addProperty (
    		Utils::SerializedRepresentation::Property ("Type", typeStr));

#ifdef _DEBUG
    if (isABSpline){
		TkUtil::UTF8String	nbStr1 (TkUtil::Charset::UTF_8);
		TkUtil::UTF8String	nbStr2 (TkUtil::Charset::UTF_8);

		// c'est une spline => une seule représentation de type Edge
		TopoDS_Edge edge = m_occ_edges[0];
		Standard_Real first_param, last_param;
		Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);

		Handle(Geom_BSplineCurve) bspline = Handle(Geom_BSplineCurve)::DownCast(curve);

		nbStr1<<(long int)bspline->NbKnots();
		nbStr2<<(long int)bspline->NbPoles();

		propertyGeomDescription.addProperty (
				Utils::SerializedRepresentation::Property ("Nb noeuds", nbStr1.ascii()) );
		propertyGeomDescription.addProperty (
				Utils::SerializedRepresentation::Property ("Nb poles", nbStr2.ascii()) );
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
	if (m_occ_edges.size() == 1)
		return;

	for (uint i=0; i<m_occ_edges.size(); i++){
		double area = OCCHelper::getLength(m_occ_edges[i]);
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
	for (uint i=0; i<m_occ_edges.size(); i++){
		m_occ_edges[i]->getPoint(0.0, ptBegin, true);
		m_occ_edges[i]->getPoint(1.0, ptEnd, true);
		std::cout<<ptBegin<<" "<<ptEnd<<std::endl;
	}
#endif

	// remplissage des paramLocFirst et paramLocLast et tenant compte du sens
	Utils::Math::Point ptPrec;
	TopoDS_Edge edge_0 = m_occ_edges[0];
	OCCHelper::getPoint(edge_0, 0.0, ptPrec, true);
#ifdef _DEBUG_PARAMS
	std::cout<<"longueur ptStart-ptPrec "<<ptPrec.length(ptStart)<<std::endl;
#endif
	if (not (ptPrec.isEpsilonEqual(ptStart, epsilon))){
		OCCHelper::getPoint(edge_0, 1.0, ptPrec, true);
#ifdef _DEBUG_PARAMS
		std::cout<<"longueur ptStart-ptPrec "<<ptPrec.length(ptStart)<<std::endl;
#endif
		if (not (ptPrec.isEpsilonEqual(ptStart, epsilon))){
			throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, Courbe composite où on ne retrouve pas le premier sommet", TkUtil::Charset::UTF_8));
		}
	}
	for (uint i=0; i<m_occ_edges.size(); i++){
		double inverse = false;
		Utils::Math::Point pt1, pt2;
		TopoDS_Edge edge_i = m_occ_edges[i];
		OCCHelper::getPoint(edge_i, 0.0, pt1, true);
		OCCHelper::getPoint(edge_i, 1.0, pt2, true);
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
			OCCHelper::getParameters(edge_i, last, first);
			ptPrec = pt1;
		}
		else {
			ptPrec = pt2;
			OCCHelper::getParameters(edge_i, first, last);
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
	if (m_occ_edges.size() == 1)
		return;

	if (m_occ_edges.size() != paramLocFirst.size()){
		TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
		messErr << "Erreur interne, paramLocFirst non itialisé correctement pour "<<getName();
		throw TkUtil::Exception(messErr);
	}
	if (m_occ_edges.size() != paramLocLast.size())
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, paramLocLast non itialisé correctement", TkUtil::Charset::UTF_8));
	if (m_occ_edges.size() != paramImgFirst.size())
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, paramImgFirst non itialisé correctement", TkUtil::Charset::UTF_8));
	if (m_occ_edges.size() != paramImgLast.size())
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, paramImgLast non itialisé correctement", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
