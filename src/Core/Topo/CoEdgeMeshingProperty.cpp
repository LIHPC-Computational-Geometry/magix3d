/*----------------------------------------------------------------------------*/
/*
 * \file CoEdgeMeshingProperty.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 4 janv. 2011
 */
/*----------------------------------------------------------------------------*/
#include <iostream>
/*----------------------------------------------------------------------------*/
#include "Topo/CoEdgeMeshingProperty.h"
#include "Topo/CoEdge.h"
#include "TkUtil/Exception.h"
#include "Topo/TopoHelper.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CoEdgeMeshingProperty::CoEdgeMeshingProperty(int nb, meshLaw law, bool isDirect)
: m_nb_edges(nb)
, m_mesh_law(law)
, m_dernierIndice(0)
, m_sens(isDirect)
, m_nbLayers(0)
, m_side(side_undef)
, m_is_polar(false)
{
    if (nb == 0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur, il n'est pas prévu de ne pas avoir de bras pour une arête", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CoEdgeMeshingProperty::CoEdgeMeshingProperty(int nb, Utils::Math::Point polar_center, meshLaw law, bool isDirect)
: m_nb_edges(nb)
, m_mesh_law(law)
, m_dernierIndice(0)
, m_sens(isDirect)
, m_nbLayers(0)
, m_side(side_undef)
, m_is_polar(true)
, m_polar_center(polar_center)
, m_modification_time()
{
    if (nb == 0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur, il n'est pas prévu de ne pas avoir de bras pour une arête", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CoEdgeMeshingProperty::~CoEdgeMeshingProperty()
{
}
/*----------------------------------------------------------------------------*/
CoEdgeMeshingProperty::CoEdgeMeshingProperty(const CoEdgeMeshingProperty& pm)
: m_nb_edges(pm.m_nb_edges)
, m_mesh_law(pm.m_mesh_law)
, m_dernierIndice(0)
, m_sens(pm.m_sens)
, m_nbLayers(pm.m_nbLayers)
, m_side(pm.m_side)
, m_is_polar(pm.m_is_polar)
, m_polar_center(pm.m_polar_center)
, m_modification_time(pm.m_modification_time)
{
}
/*----------------------------------------------------------------------------*/
#ifndef SWIG
CoEdgeMeshingProperty& CoEdgeMeshingProperty::
operator=(const CoEdgeMeshingProperty& emp)
{
    if (&emp != this){
        m_nb_edges = emp.m_nb_edges;
        m_mesh_law = emp.m_mesh_law;
        m_dernierIndice = 0;
        m_sens = emp.m_sens;
        m_side = emp.m_side;
        m_nbLayers = emp.m_nbLayers;
        m_is_polar = emp.m_is_polar;
        m_polar_center = emp.m_polar_center;
        m_modification_time = emp.m_modification_time;
    }
    return (*this);
}
#endif
/*----------------------------------------------------------------------------*/
bool CoEdgeMeshingProperty::operator == (const CoEdgeMeshingProperty& cedp) const
{
	if ((getMeshLaw ( ) == cedp.getMeshLaw ( )) &&
	    (getNbEdges ( ) == cedp.getNbEdges ( )) &&
	    (getNbNodes ( ) == cedp.getNbNodes ( )) &&
	    (getDirect ( ) == cedp.getDirect ( )) &&
		(isOrthogonal ( ) == cedp.isOrthogonal ( )) &&
		(m_side == cedp.m_side)&&
		(m_nbLayers == cedp.m_nbLayers)&&
		(m_is_polar == cedp.m_is_polar)&&
		(m_polar_center == cedp.m_polar_center) &&
		(m_modification_time == cedp.m_modification_time))
		return true;

	return false;
}
/*----------------------------------------------------------------------------*/
bool CoEdgeMeshingProperty::operator != (const CoEdgeMeshingProperty& cedp) const
{
	return !(*this == cedp);
}
/*----------------------------------------------------------------------------*/
void CoEdgeMeshingProperty::setOrthogonal(int nbLayers, bool sens)
{
	m_nbLayers = nbLayers;
	if (nbLayers)
		m_side = (sens?side_min:side_max);
	else
		m_side = side_undef;
}
/*----------------------------------------------------------------------------*/
uint CoEdgeMeshingProperty::getSide() const
{
	if (m_side == side_min)
		return 0;
	else if (m_side == side_max)
		return 1;
	else
		throw TkUtil::Exception (TkUtil::UTF8String ("CoEdgeMeshingProperty::getSide() ne peut se faire sans initialiser le côté", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String CoEdgeMeshingProperty::
getScriptCommandBegin() const
{
    TkUtil::UTF8String o (TkUtil::Charset::UTF_8);

    o << getScriptCommandRef() <<" = " << getScriptCommand();
    if (m_nbLayers>0)
    	o << "\n" << getScriptCommandRef()<<".setOrthogonal("<<(long)m_nbLayers<<", "<<(m_side==side_min?"True":"False")<<")";
    return o;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String CoEdgeMeshingProperty::
getScriptCommandRef() const
{
	return "emp";
}
/*----------------------------------------------------------------------------*/
void CoEdgeMeshingProperty::
getPointsRef(std::vector<Utils::Math::Point> &points_ref, std::vector<Topo::CoEdge*>& coedges_ref)
{
#ifdef _DEBUG2
	std::cout<<"getPointsRef pour :"<<std::endl;
	for (uint i=0; i<coedges_ref.size(); i++)
		std::cout<<" "<<coedges_ref[i]->getName();
	std::cout<<std::endl;
#endif
	if (coedges_ref.size() == 1)
		coedges_ref[0]->getPoints(points_ref);
	else {
		// il faut identifier une extrémité, ordonner les arêtes et si nécessaire inverser l'ordre des points

        Topo::Vertex* vtx1;
        Topo::Vertex* vtx2;
        // recherche des sommets extrémité
        Topo::TopoHelper::getVerticesTip(coedges_ref, vtx1, vtx2, false);
        // recherche des arêtes ordonnées entre ces 2 sommets
        std::vector<Topo::CoEdge* > coedges_between;
        Topo::TopoHelper::getCoEdgesBetweenVertices(vtx1, vtx2, coedges_ref, coedges_between);
        // pas de ratio (pas de semi-conformité)
        std::map<Topo::CoEdge*,uint> ratios;
        for (uint i=0; i<coedges_between.size(); i++)
        	ratios[coedges_between[i]] = 1;
        // recherche des points
        Topo::TopoHelper::getPoints(vtx1, vtx2, coedges_between, ratios, points_ref);
	} // end else / if (coedges_ref.size() == 1)

#ifdef _DEBUG2
	std::cout<<"points_ref : "<<std::endl;
	for (uint i=0; i<points_ref.size(); i++)
		std::cout<<" "<<points_ref[i]<<std::endl;
#endif

}
/*----------------------------------------------------------------------------*/
void CoEdgeMeshingProperty::adaptDirection(Utils::Math::Point& pt0,
		Utils::Math::Point& pt1, std::vector<Utils::Math::Point>& points)
{
	// on regarde la distance entre les premiers et derniers en inversant ou non les vecteurs cumulés en pt0
	Utils::Math::Vector v1 = pt0-pt1;
	Utils::Math::Vector v2 = points[1]-points[0];
	bool inverse = ((v1+v2).norme2()>(v1-v2).norme2());

#ifdef _DEBUG_INTERPOLATE
	std::cout<<"adaptDirection, inverse : "<<(inverse?"vrai":"faux")<<std::endl;
#endif

	if (inverse){
		// permutation des couples de points pour inverser le sens
		uint sz = points.size();
		uint nb = sz/2;
		for (uint i=0; i<nb; i++){
			Utils::Math::Point tmp = points[i];
			points[i] = points[sz-1-i];
			points[sz-1-i] = tmp;
		}
	}
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
