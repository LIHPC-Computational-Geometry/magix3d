/*----------------------------------------------------------------------------*/
/*
 * \file FacetedCurve.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 29/3/2018
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/FacetedCurve.h"
#include "Geom/FacetedHelper.h"
#include "Mesh/MeshImplementation.h"
#include "Utils/MgxNumeric.h"

#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>

#include <GMDS/Utils/Exception.h>
// OCC
#include <gp_Trsf.hxx>
#include <gp_GTrsf.hxx>
#include <gp_GTrsf.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <GeomAPI_IntCS.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
FacetedCurve::FacetedCurve(Internal::Context& c, uint gmds_id, std::vector<gmds::Node>& nodes)
:m_context(c), m_gmds_id(gmds_id), m_length(0.0)
{
	m_nodes.insert(m_nodes.end(), nodes.begin(), nodes.end());
	updateLength();
}
/*----------------------------------------------------------------------------*/
FacetedCurve::FacetedCurve(const FacetedCurve& rep)
:m_context(rep.m_context), m_gmds_id(rep.m_gmds_id), m_length(rep.m_length)
{
	m_nodes.insert(m_nodes.end(), rep.m_nodes.begin(), rep.m_nodes.end());
	m_lengths.insert(m_lengths.end(), rep.m_lengths.begin(), rep.m_lengths.end());

}
/*----------------------------------------------------------------------------*/
FacetedCurve::~FacetedCurve()
{
}
/*----------------------------------------------------------------------------*/
GeomRepresentation* FacetedCurve::clone() const
{
    //std::cout<<"FacetedCurve::clone()..."<<std::endl;
	GeomRepresentation* e= new FacetedCurve(*this);
	return e;
}
/*----------------------------------------------------------------------------*/
double FacetedCurve::computeVolumeArea()
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedCurve::computeVolumeArea non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
double FacetedCurve::computeSurfaceArea()
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedCurve::computeSurfaceArea non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
double FacetedCurve::computeCurveArea()
{
	return m_length;
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::updateLength()
{
	m_lengths.clear();
	m_length = 0.0;
	const size_t	nodesNum	= m_nodes.size ( );
	for (size_t id = 0; id < nodesNum-1; id++){

		Utils::Math::Vector v1(
				m_nodes[id+1].X()-m_nodes[id].X(),
				m_nodes[id+1].Y()-m_nodes[id].Y(),
				m_nodes[id+1].Z()-m_nodes[id].Z());
		m_lengths.push_back(m_length);
		m_length += v1.abs();
	}
	m_lengths.push_back(m_length);
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::computeBoundingBox(Utils::Math::Point& pmin,Utils::Math::Point& pmax, double tol) const
{
	FacetedHelper::computeBoundingBox(m_nodes, pmin, pmax);
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::split(std::vector<Surface* >& surf,
           std::vector<Curve*>& curv,
           std::vector<Vertex* >&  vert,
           Volume* owner)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedCurve::split non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::split(std::vector<Curve*>& curv,
           std::vector<Vertex* >&  vert,
           Surface* owner)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedCurve::split non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::split( std::vector<Vertex* >&  vert,
            Curve* owner)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedCurve::split non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::projectAndParam(const Utils::Math::Point& Pt, Utils::Math::Point& PtProj, double& length)
{
	// recherche du point le plus proche
	size_t indiceMin = 0;
	gmds::Node& node0 = m_nodes[0];
	Utils::Math::Point Pt0 = Utils::Math::Point(node0.X(), node0.Y(), node0.Z());
	double distMin = (Pt-Pt0).norme();

	for (size_t i=1; i<m_nodes.size(); i++){
		gmds::Node& nodeI = m_nodes[i];
		Utils::Math::Point PtI = Utils::Math::Point(nodeI.X(), nodeI.Y(), nodeI.Z());
		double dist = (Pt-PtI).norme();
		if (dist<distMin){
			distMin = dist;
			indiceMin = i;
		}
	}

	// est-on plutôt avant ou après le noeud trouvé (ou entre les deux)
	bool isAfter = false;
	bool isBefore = false;

	// regarde s'il est avant
	if (indiceMin>0){
		gmds::Node& node1 = m_nodes[indiceMin];
		gmds::Node& node2 = m_nodes[indiceMin-1];
		Utils::Math::Point Pt1 = Utils::Math::Point(node1.X(), node1.Y(), node1.Z());
		Utils::Math::Point Pt2 = Utils::Math::Point(node2.X(), node2.Y(), node2.Z());

		isBefore = ((Pt2-Pt1).dot(Pt-Pt1)>0);
#ifdef _DEBUG_PROJECT
		std::cout<<"Pt2-Pt1 = "<<Pt2-Pt1<<std::endl;
		std::cout<<"Pt-Pt1 = "<<Pt-Pt1<<std::endl;
		std::cout<<"(Pt2-Pt1).dot(Pt-Pt1) = "<<(Pt2-Pt1).dot(Pt-Pt1)<<std::endl;
		std::cout<<"isBefore = "<<isBefore<<std::endl;
#endif
	}

	// regarde s'il est après
	if (indiceMin<m_nodes.size()-1){
		gmds::Node& node1 = m_nodes[indiceMin];
		gmds::Node& node2 = m_nodes[indiceMin+1];
		Utils::Math::Point Pt1 = Utils::Math::Point(node1.X(), node1.Y(), node1.Z());
		Utils::Math::Point Pt2 = Utils::Math::Point(node2.X(), node2.Y(), node2.Z());

		isAfter = ((Pt2-Pt1).dot(Pt-Pt1)>0);
#ifdef _DEBUG_PROJECT
		std::cout<<"Pt2-Pt1 = "<<Pt2-Pt1<<std::endl;
		std::cout<<"Pt-Pt1 = "<<Pt-Pt1<<std::endl;
		std::cout<<"(Pt2-Pt1).dot(Pt-Pt1) = "<<(Pt2-Pt1).dot(Pt-Pt1)<<std::endl;
		std::cout<<"isAfter = "<<isAfter<<std::endl;
#endif
	}

#ifdef _DEBUG_PROJECT
	std::cout<<"FacetedCurve::project("<<Pt<<")"<<std::endl;
	std::cout<<"indiceMin = "<<indiceMin<<std::endl;
	std::cout<<"isBefore = "<<isBefore<<std::endl;
	std::cout<<"isAfter = "<<isAfter<<std::endl;
#endif

	if ((isAfter && isBefore) || (!isAfter && !isBefore)){
		// on se place sur le noeud le plus proche
		gmds::Node& nodeI = m_nodes[indiceMin];
		PtProj = Utils::Math::Point(nodeI.X(), nodeI.Y(), nodeI.Z());
		length = m_lengths[indiceMin];
#ifdef _DEBUG_PROJECT
		std::cout<<"on prend le noeud => Pt : "<<Pt<<std::endl;
#endif
	}
	else {
		if (isBefore){
			indiceMin--;
		}

		if (indiceMin == m_nodes.size()-1)
			throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne FacetedCurve::projectAndParam avec indiceMin au max", TkUtil::Charset::UTF_8));

		gmds::Node& node1 = m_nodes[indiceMin];
		gmds::Node& node2 = m_nodes[indiceMin+1];
		Utils::Math::Point Pt1 = Utils::Math::Point(node1.X(), node1.Y(), node1.Z());
		Utils::Math::Point Pt2 = Utils::Math::Point(node2.X(), node2.Y(), node2.Z());

		Utils::Math::Vector V1 = Pt2-Pt1;
#ifdef _DEBUG_PROJECT
		std::cout<<"V1.norme() = "<<V1.norme()<<std::endl;
		std::cout<<"(Pt-Pt1).norme() = "<<(Pt-Pt1).norme()<<std::endl;
#endif
		V1/=V1.norme();
		double proj = V1.dot(Pt-Pt1);

		PtProj = Pt1 + V1*proj;
#ifdef _DEBUG_PROJECT
		std::cout<<"on trouve proj = "<<proj<<" => PtProj : "<<PtProj<<std::endl;
#endif
		length = m_lengths[indiceMin]+proj;
	}

}
/*----------------------------------------------------------------------------*/
void FacetedCurve::project( Utils::Math::Point& Pt, const Curve* C)
{
	double length;
	Utils::Math::Point PtProj;
	projectAndParam(Pt, PtProj, length);
	Pt = PtProj;
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::project(const Utils::Math::Point& P1, Utils::Math::Point& P2, const Curve* C)
{
	double length;
	projectAndParam(P1, P2, length);
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::project( Utils::Math::Point& P, const Surface* S)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("project Pt, Pt, Surface) non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::project(const Utils::Math::Point& P1, Utils::Math::Point& P2, const Surface* S)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("project Pt, Pt, Surface) non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2, const Surface* S)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedCurve::normal non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::getPoint(const double& p, Utils::Math::Point& Pt,
        const bool in01)
{
	double param = p;
	if (in01)
		param *= m_length;

	if (param<0.0 || param>m_length){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr << "Erreur avec FacetedCurve::getPoint, le paramètre doit être entre 0 et "<<m_length<<" mais actuellement on a "<<param;
		throw TkUtil::Exception(messErr);
	}

	// cas aux extrémités
	if (Utils::Math::MgxNumeric::isNearlyZero(param, getPrecision())){
		gmds::Node& node = m_nodes[0];
		Pt = Utils::Math::Point(node.X(), node.Y(), node.Z());
	}
	else if (Utils::Math::MgxNumeric::isNearlyZero(param - m_length, getPrecision())){
		gmds::Node& node = m_nodes[m_nodes.size()-1];
		Pt = Utils::Math::Point(node.X(), node.Y(), node.Z());
	}
	else {
		// recherche de la section dans laquelle il faut trouver la position par interpolation
		bool trouve = false;
		size_t indice = 0;
		double longTot = 0.0;
		double longI = 0.0;
		do {
			longI = m_lengths[indice+1]; // longueur totale jusqu'à I+1
			if (longI<param){
				longTot=longI;
				indice++;
			}
			else
				trouve = true;

		} while (!trouve);
		double reste = param-longTot;
		longI = m_lengths[indice+1] - m_lengths[indice]; // longueur du bras
		Pt = Utils::Math::Point(
				m_nodes[indice].X() + reste/longI*(m_nodes[indice+1].X()-m_nodes[indice].X()),
				m_nodes[indice].Y() + reste/longI*(m_nodes[indice+1].Y()-m_nodes[indice].Y()),
				m_nodes[indice].Z() + reste/longI*(m_nodes[indice+1].Z()-m_nodes[indice].Z()));
	}
#ifdef _DEBUG2
	std::cout<<"FacetedCurve::getPoint param "<<p<<(in01?" entre 0 et 1":" quelconque")<<" => "<<Pt<<std::endl;
	std::cout<<" m_nodes[0] : "<<m_nodes[0].X()<<", "<<m_nodes[0].Y()<<", "<<m_nodes[0].Z()<<std::endl;
	std::cout<<" m_nodes[dernier] : "<<m_nodes[m_nodes.size()-1].X()<<", "<<m_nodes[m_nodes.size()-1].Y()<<", "<<m_nodes[m_nodes.size()-1].Z()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::getIntersection(gp_Pln& plan_cut, Utils::Math::Point& Pt)
{
	const gp_Dir& dir = plan_cut.Axis().Direction();
	const gp_Pnt& pt = plan_cut.Location();

	// recherche le changement de signe pour le produit scalaire entre le vecteur du centre du plan avec un noeud
	// avec le vecteur normal au plan

	gmds::Node& node0 = m_nodes[0];
	const gp_Pnt pt0(node0.X(), node0.Y(), node0.Z());

	double dot0 = dir.Dot(gp_Vec(pt, pt0));

	size_t indice = 0;
	bool trouve = false;
	do {
		gmds::Node& nodeI = m_nodes[indice+1];
		const gp_Pnt ptI(nodeI.X(), nodeI.Y(), nodeI.Z());
		double dotI = dir.Dot(gp_Vec(pt, ptI));

		if (dot0*dotI<0){
			trouve = true;
		}
		else {
			indice++;
		}
	}while (!trouve && indice+1<m_nodes.size());

	if (indice+1 == m_nodes.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr << "Erreur avec FacetedCurve::getIntersection, le plan n'a pas permis de trouver 2 noeuds de part et d'autre ";
		throw TkUtil::Exception(messErr);
	}
	else {
		//std::cout<<"getIntersection : indice = "<<indice<<std::endl;

		gmds::Node& nodeI1 = m_nodes[indice];
		gmds::Node& nodeI2 = m_nodes[indice+1];

		const gp_Pnt Pt1(nodeI1.X(), nodeI1.Y(), nodeI1.Z());
		const gp_Pnt Pt2(nodeI2.X(), nodeI2.Y(), nodeI2.Z());

		TopoDS_Edge edge_shape = BRepBuilderAPI_MakeEdge(Pt1,Pt2);
		double first, last;
		Handle_Geom_Curve curv = BRep_Tool::Curve(edge_shape, first, last);


		BRepBuilderAPI_MakeFace mkF(plan_cut);
		TopoDS_Face wf = mkF.Face();
		Handle_Geom_Surface surf = BRep_Tool::Surface(wf);


		GeomAPI_IntCS Intersector(curv, surf);
		uint pt_found = 0;
		if (Intersector.IsDone() && Intersector.NbPoints() == 1){
			gp_Pnt pt = Intersector.Point (1);
			for (uint j=0; j<3; j++)
				Pt.setCoord(j,pt.Coord(j+1));
			pt_found = 1;
		}
		else {
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr << "Erreur avec FacetedCurve::getIntersection, l'intersection entre plan et segment ne trouve pas un unique point";
			throw TkUtil::Exception(messErr);
		}
	}

}
/*----------------------------------------------------------------------------*/
void FacetedCurve::getParameter(const Utils::Math::Point& Pt, double& p, const Curve* C)
{
	gmds::Node& node0 = m_nodes[0];
	gmds::Node& nodeN = m_nodes[m_nodes.size()-1];
	Utils::Math::Point Pt0 = Utils::Math::Point(node0.X(), node0.Y(), node0.Z());
	Utils::Math::Point Pt1 = Utils::Math::Point(nodeN.X(), nodeN.Y(), nodeN.Z());
	p=0;
	if (Pt == Pt0)
		p = 0;
	else if (Pt == Pt1)
		p = m_length;
	else {
		Utils::Math::Point PtProj;
		projectAndParam(Pt, PtProj, p);
	}
#ifdef _DEBUG2
	std::cout<<"FacetedCurve::getParameter param "<<Pt<<" => p = "<<p<<std::endl;
	std::cout<<" m_nodes[0] : "<<m_nodes[0].X()<<", "<<m_nodes[0].Y()<<", "<<m_nodes[0].Z()<<std::endl;
	std::cout<<" m_nodes[dernier] : "<<m_nodes[m_nodes.size()-1].X()<<", "<<m_nodes[m_nodes.size()-1].Y()<<", "<<m_nodes[m_nodes.size()-1].Z()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::getParameters(double& first, double& last)
{
	// paramétrisation suivant la longueur
	first = 0;
	last = m_length;
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::translate(const Utils::Math::Vector& V)
{
    //std::cout<<"FacetedCurve::translate()..."<<std::endl;

    // création de l'opérateur de translation via OCC
    gp_Trsf transf;
    gp_Vec vec(V.getX(),V.getY(),V.getZ());
    transf.SetTranslation(vec);

	FacetedHelper::transform(m_nodes, &transf);
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::scale(const double F, const Utils::Math::Point& center)
{
	// création de l'opérateur d'homothétie via OCC
	gp_Trsf transf;
	transf.SetScale(gp_Pnt(center.getX(), center.getY(), center.getZ()), F);

	FacetedHelper::transform(m_nodes, &transf);
	updateLength();
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::scale(const double factorX,
            const double factorY,
            const double factorZ)
{
    // création de l'opérateur d'homothétie via OCC
    gp_GTrsf transf;
    transf.SetValue(1,1, factorX);
    transf.SetValue(2,2, factorY);
    transf.SetValue(3,3, factorZ);

	FacetedHelper::transform(m_nodes, &transf);
    updateLength();
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::rotate(const Utils::Math::Point& P1,
        const Utils::Math::Point& P2, double Angle)
{
    // création de l'opérateur de rotation via OCC
    gp_Trsf transf;

    gp_Pnt p1(P1.getX(),P1.getY(),P1.getZ());
    gp_Dir dir( P2.getX()-P1.getX(),
                P2.getY()-P1.getY(),
                P2.getZ()-P2.getZ());
    gp_Ax1 axis(p1,dir);
    transf.SetRotation(axis, Angle);

    FacetedHelper::transform(m_nodes, &transf);
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::mirror(const Utils::Math::Plane& plane)
{
    // création de l'opérateur d'homothétie via OCC
    gp_Trsf transf;
    Utils::Math::Point plane_pnt = plane.getPoint();
    Utils::Math::Vector plane_vec = plane.getNormal();
    transf.SetMirror(gp_Ax2(gp_Pnt(plane_pnt.getX(), plane_pnt.getY(), plane_pnt.getZ()),
    		gp_Dir(plane_vec.getX(), plane_vec.getY(), plane_vec.getZ())));

    FacetedHelper::transform(m_nodes, &transf);
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::buildDisplayRepresentation(Utils::DisplayRepresentation& dr,
                                const GeomEntity*) const
{
    std::vector<Utils::Math::Point>& points = dr.getPoints();
    std::vector<size_t>& indicesFilaire = dr.getCurveDiscretization();

    points.clear();
    indicesFilaire.clear();

    for (std::vector<gmds::Node>::const_iterator itn = m_nodes.begin ( );
    		itn != m_nodes.end ( ); itn++)
    	points.push_back (Utils::Math::Point (
    			(*itn).X(),
				(*itn).Y(),
				(*itn).Z()));

    if (dr.hasRepresentation(Utils::DisplayRepresentation::WIRE)){
    	const size_t	nodesNum	= m_nodes.size ( );
    	for (size_t id = 0; id < nodesNum-1; id++){
    		indicesFilaire.push_back(id);
    		indicesFilaire.push_back(id+1);
       	}	// for (size_t id = 0; id < nodesNum; id++)
    }
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::getFacetedRepresentation(
        std::vector<gmds::math::Triangle >& AVec,
        const GeomEntity* caller) const
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedCurve::getFacetedRepresentation non prévu", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::facetedRepresentationForwardOrient(
		const GeomEntity* ACaller,
		const GeomEntity* AEntityOrientation,
		std::vector<gmds::math::Triangle>*) const
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedCurve::facetedRepresentationForwardOrient non prévu", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedCurve::tangent(const Utils::Math::Point& P1, Utils::Math::Vector& V2)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedCurve::tangent non prévu", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
double FacetedCurve::getPrecision()
{
	return Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon;
}
/*----------------------------------------------------------------------------*/
size_t FacetedCurve::getNodeIdNear(const Utils::Math::Point& Pt)
{
	// recherche de l'indice du noeud le plus proche
	size_t indiceMin = 0;
	gmds::Node& node0 = m_nodes[0];
	Utils::Math::Point Pt0 = Utils::Math::Point(node0.X(), node0.Y(), node0.Z());
	double distMin = (Pt-Pt0).norme();

	for (size_t i=1; i<m_nodes.size(); i++){
		gmds::Node& nodeI = m_nodes[i];
		Utils::Math::Point PtI = Utils::Math::Point(nodeI.X(), nodeI.Y(), nodeI.Z());
		double dist = (Pt-PtI).norme();
		if (dist<distMin){
			distMin = dist;
			indiceMin = i;
		}
	}

	return indiceMin;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
