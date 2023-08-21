/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyGlobalInterpolate.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 14/11/2016
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Topo/EdgeMeshingPropertyGlobalInterpolate.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Topo/CoEdge.h"
#include "Topo/TopoHelper.h"
#include "Mesh/MeshImplementation.h"
#include "Internal/Context.h"
#include "Utils/MgxException.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyGlobalInterpolate::
EdgeMeshingPropertyGlobalInterpolate(int nbBras,
		const std::vector<std::string>& firstCoedgesName,
		const std::vector<std::string>& secondCoedgesName)
: CoEdgeMeshingProperty(nbBras, globalinterpolate, true)
, m_firstCoedgesName(firstCoedgesName)
, m_secondCoedgesName(secondCoedgesName)
{
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyGlobalInterpolate::EdgeMeshingPropertyGlobalInterpolate (const CoEdgeMeshingProperty& prop)
: CoEdgeMeshingProperty (prop.getNbEdges(), globalinterpolate, prop.getDirect())
, m_firstCoedgesName()
, m_secondCoedgesName()
{
	const EdgeMeshingPropertyGlobalInterpolate*	p = reinterpret_cast<const EdgeMeshingPropertyGlobalInterpolate*>(&prop);
	if (0 == p)
	{
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr << "EdgeMeshingPropertyGlobalInterpolate, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyGlobalInterpolate";
	        throw TkUtil::Exception(messErr);
	}	// if (0 == p)

	m_firstCoedgesName	= p->m_firstCoedgesName;
	m_secondCoedgesName	= p->m_secondCoedgesName;
}
/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyGlobalInterpolate::operator == (const CoEdgeMeshingProperty& cedp) const
{
	const EdgeMeshingPropertyGlobalInterpolate*	props	=
					dynamic_cast<const EdgeMeshingPropertyGlobalInterpolate*> (&cedp);
	if (0 == props)
		return false;

	return CoEdgeMeshingProperty::operator == (cedp);
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyGlobalInterpolate::initCoeff()
{
	m_dernierIndice = 0;
	if (m_firstCoedgesName.empty() || m_secondCoedgesName.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans EdgeMeshingPropertyGlobalInterpolate::initCoeff, une des listes des arêtes référencées est vide", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyGlobalInterpolate::nextCoeff()
{
	// comme pour uniforme, cas sans projection ...
    m_dernierIndice+=1;

    return ((double)m_dernierIndice)/((double)m_nb_edges);
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertyGlobalInterpolate::
getScriptCommand() const
{
    TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertyGlobalInterpolate("
      << (long)m_nb_edges <<", [\"";
    for (uint i=0; i<m_firstCoedgesName.size(); i++){
    	if (i!=0)
    		o << "\", \"";
    	o << m_firstCoedgesName[i];
    }
    o << "\"], [\"";
    for (uint i=0; i<m_secondCoedgesName.size(); i++){
    	if (i!=0)
    		o << "\", \"";
    	o << m_secondCoedgesName[i];
    }
    o << "\"])";
    return o;
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyGlobalInterpolate::
addDescription(Utils::SerializedRepresentation& topoProprietes) const
{

	Utils::SerializedRepresentation edges1 ("Arêtes de référence 1", "");
	for (uint i=0; i<m_firstCoedgesName.size(); i++){
		CoEdge* edge = 0;
		try {
			edge = getStdContext()->getLocalTopoManager().getCoEdge(m_firstCoedgesName[i], false);
		}
		catch (Utils::IsDestroyedException& exc){
			edge = 0;
		}
		if (edge)
			edges1.addProperty (
					Utils::SerializedRepresentation::Property (m_firstCoedgesName[i], *edge));
		else
			edges1.addProperty (
					Utils::SerializedRepresentation::Property (m_firstCoedgesName[i], std::string("Détruite !")));
	}
	topoProprietes.addPropertiesSet(edges1);

	Utils::SerializedRepresentation edges2 ("Arêtes de référence 2", "");
	for (uint i=0; i<m_secondCoedgesName.size(); i++){
		CoEdge* edge = 0;
		try {
			edge = getStdContext()->getLocalTopoManager().getCoEdge(m_secondCoedgesName[i], false);
		}
		catch (Utils::IsDestroyedException& exc){
			edge = 0;
		}
		if (edge)
			edges2.addProperty (
					Utils::SerializedRepresentation::Property (m_secondCoedgesName[i], *edge));
		else
			edges2.addProperty (
					Utils::SerializedRepresentation::Property (m_secondCoedgesName[i], std::string("Détruite !")));
	}
	topoProprietes.addPropertiesSet(edges2);


}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_INTERPOLATE
void EdgeMeshingPropertyGlobalInterpolate::
getPoints(std::vector<Utils::Math::Point> &points,
		std::vector<Topo::CoEdge*> first_coedges_ref,
		std::vector<Topo::CoEdge*> second_coedges_ref)
{
#ifdef _DEBUG_INTERPOLATE
	std::cout<<"EdgeMeshingPropertyGlobalInterpolate::getPoints avec size() : "
			<<first_coedges_ref.size()<<" et "<<second_coedges_ref.size()<<std::endl;
#endif
	Utils::Math::Point pt0 = points[0];
	Utils::Math::Point pt1 = points.back();

	// les points des arêtes de référence
	std::vector<Utils::Math::Point>	first_points_ref;
	getPointsRef(first_points_ref, first_coedges_ref);
	std::vector<Utils::Math::Point>	second_points_ref;
	getPointsRef(second_points_ref, second_coedges_ref);

	// il faut voir si le sens semble bon
	adaptDirection(pt0, pt1, first_points_ref);
	adaptDirection(pt0, pt1, second_points_ref);

	getInterpolate(first_points_ref, second_points_ref, points);

}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyGlobalInterpolate::getInterpolate(std::vector<Utils::Math::Point> &first_points_ref,
		std::vector<Utils::Math::Point> &second_points_ref,
		std::vector<Utils::Math::Point> &points)
{
	if (first_points_ref.size() != points.size() || second_points_ref.size() != points.size())
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans EdgeMeshingPropertyGlobalInterpolate::getInterpolate, les nb de pts ne sont pas équivalents", TkUtil::Charset::UTF_8));

	uint sz = points.size();

	Utils::Math::Point* l_points = new Utils::Math::Point[sz*3];

	for (uint i=0; i<sz; i++){
		l_points[i] = first_points_ref[i];
		l_points[i+sz] = Utils::Math::Point();
		l_points[i+2*sz] = second_points_ref[i];
	}

	l_points[sz] = points[0];
	l_points[2*sz-1] = points.back();

	Mesh::MeshItf*            meshItf  = getStdContext()->getMeshManager().getMesh();
	Mesh::MeshImplementation* meshImpl = dynamic_cast<Mesh::MeshImplementation*> (meshItf);

	meshImpl->discretiseTransfinie (sz-1, 3-1, l_points);

	for (uint i=0; i<sz; i++)
		points[i] = l_points[sz+i];

	delete [] l_points;
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyGlobalInterpolate::setFirstCoEdges(std::vector<std::string>& names)
{
	m_firstCoedgesName.clear();
	m_firstCoedgesName.insert(m_firstCoedgesName.begin(), names.begin(), names.end());
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyGlobalInterpolate::setSecondCoEdges(std::vector<std::string>& names)
{
	m_secondCoedgesName.clear();
	m_secondCoedgesName.insert(m_secondCoedgesName.begin(), names.begin(), names.end());
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
