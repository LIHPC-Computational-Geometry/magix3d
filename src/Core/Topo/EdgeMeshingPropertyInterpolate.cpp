/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyInterpolate.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 27/5/2014
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"
#include "Topo/CoEdge.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/TopoManager.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/Vector.h"
#include "Utils/MgxException.h"
#include "Internal/Context.h"

#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyInterpolate::
EdgeMeshingPropertyInterpolate(int nbBras, const std::vector<std::string>& coedgesName)
: CoEdgeMeshingProperty(nbBras, interpolate, true)
, m_type(with_coedge_list)
, m_coedgesName(coedgesName)
{
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyInterpolate::
EdgeMeshingPropertyInterpolate(int nbBras, const std::string& cofaceName)
: CoEdgeMeshingProperty(nbBras, interpolate, true)
, m_type(with_coface)
, m_cofaceName(cofaceName)
{
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyInterpolate::EdgeMeshingPropertyInterpolate (const CoEdgeMeshingProperty& prop)
: CoEdgeMeshingProperty (prop.getNbEdges(), interpolate, prop.getDirect())
, m_type(with_coedge_list)
, m_cofaceName()
{
	const EdgeMeshingPropertyInterpolate*	p = reinterpret_cast<const EdgeMeshingPropertyInterpolate*>(&prop);
	if (0 == p)
	{
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr << "EdgeMeshingPropertyInterpolate, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyInterpolate";
	        throw TkUtil::Exception(messErr);
	}	// if (0 == p)

	m_type		= p->m_type;
	m_cofaceName	= p->m_cofaceName;
}
/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyInterpolate::operator == (const CoEdgeMeshingProperty& cedp) const
{
	const EdgeMeshingPropertyInterpolate*	props	=
					dynamic_cast<const EdgeMeshingPropertyInterpolate*> (&cedp);
	if (0 == props)
		return false;

	return CoEdgeMeshingProperty::operator == (cedp);
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyInterpolate::initCoeff()
{
	m_dernierIndice = 0;
	if (m_type == with_coedge_list && m_coedgesName.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans EdgeMeshingPropertyInterpolate::initCoeff, la liste des arêtes référencées est vide", TkUtil::Charset::UTF_8));
	else if (m_type == with_coface && m_cofaceName.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans EdgeMeshingPropertyInterpolate::initCoeff, la face référencée est vide", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyInterpolate::nextCoeff()
{
	// comme pour uniforme, cas sans projection ...
    m_dernierIndice+=1;

    return ((double)m_dernierIndice)/((double)m_nb_edges);
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertyInterpolate::
getScriptCommand() const
{
    TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertyInterpolate("
      << (long)m_nb_edges <<", ";
    if (m_type == with_coedge_list){
    	o << "[\"";
    	for (uint i=0; i<m_coedgesName.size(); i++){
    		if (i!=0)
    			o << "\", \"";
    		o << m_coedgesName[i];
    	}
    	o << "\"]";
    }
    else if (m_type == with_coface){
    	o << "\"" << m_cofaceName << "\"";
    }
    o << ")";
    return o;
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyInterpolate::
addDescription(Utils::SerializedRepresentation& topoProprietes) const
{
    TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
	if (m_type == with_coedge_list){

		Utils::SerializedRepresentation edges ("Arêtes de référence", "");
		for (uint i=0; i<m_coedgesName.size(); i++){
			CoEdge* edge = 0;
			try {
				edge = getStdContext()->getLocalTopoManager().getCoEdge(m_coedgesName[i], false);
			}
			catch (Utils::IsDestroyedException& exc){
				edge = 0;
			}
			if (edge)
				edges.addProperty (
						Utils::SerializedRepresentation::Property (m_coedgesName[i], *edge));
			else
				edges.addProperty (
						Utils::SerializedRepresentation::Property (m_coedgesName[i], std::string("Détruite !")));
		}
		topoProprietes.addPropertiesSet(edges);

	}
	else if (m_type == with_coface){
		CoFace* coface = 0;
		try {
			coface = getStdContext()->getLocalTopoManager().getCoFace(m_cofaceName, false);
		}
		catch (Utils::IsDestroyedException& exc){
			coface = 0;
		}
		if (coface){
			Utils::SerializedRepresentation face ("Face de référence", m_cofaceName);
			face.addProperty (
					Utils::SerializedRepresentation::Property (m_cofaceName, *coface));
			topoProprietes.addPropertiesSet(face);
		}
		else
			topoProprietes.addProperty (
					Utils::SerializedRepresentation::Property (m_cofaceName, std::string("Détruite !")));
	}
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_INTERPOLATE
void EdgeMeshingPropertyInterpolate::
getPoints(std::vector<Utils::Math::Point> &points, std::vector<Topo::CoEdge*> coedges_ref)
{
	if (m_type != with_coedge_list)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans EdgeMeshingPropertyInterpolate::getPoints, mauvais type en entrée", TkUtil::Charset::UTF_8));

#ifdef _DEBUG_INTERPOLATE
	std::cout<<"EdgeMeshingPropertyInterpolate::getPoints avec coedges_ref.size() : "<<coedges_ref.size()<<std::endl;
#endif
	Utils::Math::Point pt0 = points[0];
	Utils::Math::Point pt1 = points.back();

	// les points des arêtes de référence
	std::vector<Utils::Math::Point>	points_ref;
	CoEdgeMeshingProperty::getPointsRef(points_ref, coedges_ref);

	// il faut voir si le sens semble bon
	adaptDirection(pt0, pt1, points_ref);

	getInterpolate(points_ref, points);

}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyInterpolate::
getPoints(std::vector<Utils::Math::Point> &points, const Topo::CoEdge* coedge, Topo::CoFace* coface)
{
	if (m_type != with_coface)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans EdgeMeshingPropertyInterpolate::getPoints, mauvais type en entrée", TkUtil::Charset::UTF_8));

	std::vector<Utils::Math::Point>	loc_points_ref;
	getPointsRef(loc_points_ref, coedge, coface);

	// faire l'interpolation
	getInterpolate(loc_points_ref, points);

}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyInterpolate::
getPointsRef(std::vector<Utils::Math::Point> &points_ref, const Topo::CoEdge* coedge, Topo::CoFace* coface)
{
#ifdef _DEBUG_INTERPOLATE
	std::cout<<"EdgeMeshingPropertyInterpolate::getPointsRef avec comme arête "<<coedge->getName()
			<<" et comme coface : "<<*coface<<std::endl;
#endif

	// rechercher l'edge dans laquelle est cette coedge, à partir de la coface.
	Edge* edge_dep = 0;
	try {
		edge_dep = coface->getEdgeContaining(coedge);
	}
	catch (TkUtil::Exception &e){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Erreur avec l'interpolation de l'arête "
				<< coedge->getName()
				<<", elle référence la face "<<coface->getName()
				<<" alors qu'elles ne sont pas en relation.";
		throw TkUtil::Exception (message);
	}

	// déterminer les indices sur l'edge
	uint ind_edge = coface->getIndex(edge_dep);
	//Vertex* v_dep = coface->getVertex(ind_edge);
	Vertex* v_dep = edge_dep->getVertex(0);
	uint nb_bras1, nb_bras2;
	bool sens_dep;
	edge_dep->computeCorrespondingNbMeshingEdges(v_dep, coedge, 0, nb_bras1, sens_dep);
	edge_dep->computeCorrespondingNbMeshingEdges(v_dep, coedge, coedge->getNbMeshingEdges(), nb_bras2, sens_dep);

#ifdef _DEBUG_INTERPOLATE
	std::cout<<"edge_dep = "<<*edge_dep<<std::endl;
	std::cout<<"ind_edge = "<<ind_edge<<std::endl;
	std::cout<<"sens_dep = "<<sens_dep<<std::endl;
	std::cout<<"v_dep = "<<*v_dep<<std::endl;
	std::cout<<"nb_bras1 = "<<nb_bras1<<std::endl;
	std::cout<<"nb_bras2 = "<<nb_bras2<<std::endl;
#endif

	// récupérer les points de l'edge qui fait face
	uint nbPts = coface->getNbVertices();
	uint indP1 = (ind_edge+3) % nbPts;
	uint indP2 = (ind_edge+2) % nbPts;
	if (nbPts == 3 && ind_edge>1){
		indP1--;
		indP2--;
	}
	Edge* edge_ref = coface->getOppositeEdge(edge_dep);

	std::vector<Utils::Math::Point>	loc_points_ref;
	edge_ref->getPoints(loc_points_ref);

#ifdef _DEBUG_INTERPOLATE
	std::cout<<"indP1 = "<<indP1<<std::endl;
	std::cout<<"indP2 = "<<indP2<<std::endl;
	std::cout<<" loc_points_ref :"<<std::endl;
	for (uint i=0; i<loc_points_ref.size(); i++)
		std::cout<<"  loc_points_ref["<<i<<"] : "<<loc_points_ref[i]<<std::endl;
#endif

	// sens de l'arête de départ / face
	bool sens_edge_dep = (edge_dep->getVertex(0) == coface->getVertex(ind_edge));
	// idem avec celle référencée
	bool sens_edge_ref = (edge_ref->getVertex(0) == coface->getVertex(indP1));

	uint nb_bras_edge = edge_ref->getNbMeshingEdges();
	// si les arêtes sont de sens opposé, on prend du côté opposé
	if (sens_edge_dep != sens_edge_ref){
		nb_bras1 = nb_bras_edge-nb_bras1;
		nb_bras2 = nb_bras_edge-nb_bras2;
	}

#ifdef _DEBUG_INTERPOLATE
	std::cout<<"sens_edge_dep = "<<sens_edge_dep<<std::endl;
	std::cout<<"sens_edge_ref = "<<sens_edge_ref<<std::endl;
	std::cout<<"nb_bras1 = "<<nb_bras1<<std::endl;
	std::cout<<"nb_bras2 = "<<nb_bras2<<std::endl;
#endif
	if (nb_bras1<nb_bras2){
		for (uint i=0; i<coedge->getNbMeshingEdges()+1; i++)
			points_ref.push_back(loc_points_ref[nb_bras1+i]);
	}
	else {
		for (uint i=0; i<coedge->getNbMeshingEdges()+1; i++)
			points_ref.push_back(loc_points_ref[nb_bras1-i]);
	}
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyInterpolate::getInterpolate(std::vector<Utils::Math::Point> &points_ref,
		std::vector<Utils::Math::Point> &loc_points)
{
	if (points_ref.size() != loc_points.size()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Erreur interne dans EdgeMeshingPropertyInterpolate::getInterpolate, les nb de pts ne sont pas équivalents, "
				<<" points_ref.size() = "<<(short)points_ref.size()
				<<" != loc_points.size() = "<<(short)loc_points.size();
		throw TkUtil::Exception (message);
	}

	uint nb_pts = loc_points.size();

	// on met de côté les points souhaités aux extrémités
	Utils::Math::Point pt0 = loc_points[0];
	Utils::Math::Point ptN = loc_points.back();

	// translation du contour de référence
	Utils::Math::Vector trans = (loc_points[0] - points_ref[0]);
#ifdef _DEBUG_INTERPOLATE
	std::cout<<"trans : "<<trans<<std::endl;
#endif
	for (uint i=1; i<nb_pts; i++){
		loc_points[i] = points_ref[i]+trans;
#ifdef _DEBUG_INTERPOLATE
		std::cout<<" loc_points["<<i<<"] = "<<points_ref[i]<<"+trans = "<<loc_points[i]<<std::endl;
#endif
	}

	// homothétie (centrée en pt0) pour les avoir de la même longueur
	double ratio = (ptN-loc_points[0]).norme()/(loc_points.back()-loc_points[0]).norme();
#ifdef _DEBUG_INTERPOLATE
	std::cout<<"ratio : "<<ratio<<std::endl;
#endif
	for (uint i=1; i<nb_pts; i++)
		loc_points[i] = pt0+(loc_points[i]-pt0)*ratio;

	// il faut certainement faire une rotation pour coller avec la bonne direction (et arriver sur ptN)
	Utils::Math::Vector v1 = (loc_points.back() - loc_points[0]);
	Utils::Math::Vector v2 = (ptN - pt0);

	// rotation de v1 vers v2
	Utils::Math::Vector v3 = v1*v2;
	if (v3.norme2() > Utils::Math::MgxNumeric::mgxDoubleEpsilon){
		double n1 = v1.norme();
		// translation associée à cette rotation (pour le point extrémité)
		Utils::Math::Vector rotation = (ptN-loc_points.back());

		for (uint i=1; i<nb_pts-1; i++)
			loc_points[i] += rotation*(loc_points[i]-pt0).norme()/n1;
		loc_points.back() = ptN; // pour éviter une succession de déplacements qui conduisent un peu à coté
	}
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyInterpolate::
getTabulations(std::vector<double> &tabulations,
		std::vector<Topo::CoEdge*>& coedges_ref,
		Utils::Math::Point& pt0,
		Utils::Math::Point& pt1)
{
	if (m_type != with_coedge_list)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans EdgeMeshingPropertyInterpolate::getTabulations, mauvais type en entrée", TkUtil::Charset::UTF_8));

#ifdef _DEBUG_INTERPOLATE
	std::cout<<"EdgeMeshingPropertyInterpolate::getTabulations avec coedges_ref.size() : "
			<<coedges_ref.size()<<", de "<<pt0<<" à "<<pt1<<std::endl;
#endif
	// les points de l'arête de référence
	std::vector<Utils::Math::Point>	points_ref;
	CoEdgeMeshingProperty::getPointsRef(points_ref, coedges_ref);

	// il faut voir si le sens semble bon
	// on regarde juste la somme des distances entre les premiers et derniers
	Utils::Math::Point& pt0_ref = points_ref[0];
	Utils::Math::Point& pt1_ref = points_ref.back();
	double dist_direct = (pt0_ref-pt0).norme() + (pt1_ref-pt1).norme();
	double dist_inverse = (pt0_ref-pt1).norme() + (pt1_ref-pt0).norme();
	bool inverse = (dist_inverse<dist_direct);
#ifdef _DEBUG_INTERPOLATE
	std::cout<<"inverse : "<<(inverse?"vrai":"faux")<<std::endl;
#endif

	if (inverse)
		for (uint i=points_ref.size()-1; i>0; i--)
			tabulations.push_back(points_ref[i].length(points_ref[i-1]));
	else
		for (uint i=1; i<points_ref.size(); i++)
			tabulations.push_back(points_ref[i].length(points_ref[i-1]));
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyInterpolate::
getTabulations(std::vector<double> &tabulations,
		const Topo::CoEdge* coedge,
		Topo::CoFace* coface)
{
	if (m_type != with_coface)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans EdgeMeshingPropertyInterpolate::getTabulations, mauvais type en entrée", TkUtil::Charset::UTF_8));

#ifdef _DEBUG_INTERPOLATE
	std::cout<<"EdgeMeshingPropertyInterpolate::getTabulations avec coface : "<<coface->getName()<<std::endl;
	std::cout<<" arête : "<<*coedge<<std::endl;
#endif
	// les points des arêtes de référence
	std::vector<Utils::Math::Point>	points_ref;
	getPointsRef(points_ref, coedge, coface);

	for (uint i=1; i<points_ref.size(); i++){
		tabulations.push_back(points_ref[i].length(points_ref[i-1]));
#ifdef _DEBUG_INTERPOLATE
		std::cout<<" de "<<points_ref[i-1]<<" à "<<points_ref[i] << " tab : "<<tabulations.back()<<std::endl;
#endif
	}
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyInterpolate::setCoEdges(std::vector<std::string>& names)
{
	m_coedgesName.clear();
	m_coedgesName.insert(m_coedgesName.begin(), names.begin(), names.end());
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyInterpolate::setCoFace(std::string& name)
{
	m_cofaceName = name;
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
