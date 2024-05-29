/*----------------------------------------------------------------------------*/
/*
 * EdgeMeshingPropertySpecificSize.cpp
 *
 *  Created on: 15 janv. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include <Topo/EdgeMeshingPropertySpecificSize.h>
#include "Geom/GeomEntity.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertySpecificSize::EdgeMeshingPropertySpecificSize(double size)
: CoEdgeMeshingProperty(1, specific_size, true)
, m_edge_size(size)
{
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertySpecificSize::EdgeMeshingPropertySpecificSize (const CoEdgeMeshingProperty& prop)
: CoEdgeMeshingProperty (prop.getNbEdges(), specific_size, prop.getDirect())
, m_edge_size(0.)
{
	const EdgeMeshingPropertySpecificSize*	p = reinterpret_cast<const EdgeMeshingPropertySpecificSize*>(&prop);
	if (0 == p)
	{
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr << "EdgeMeshingPropertySpecificSize, la propriété transmise en argument n'est pas de type EdgeMeshingPropertySpecificSize";
	        throw TkUtil::Exception(messErr);
	}	// if (0 == p)

	m_edge_size	= p->m_edge_size;
}
/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertySpecificSize::operator == (const CoEdgeMeshingProperty& cedp) const
{
	const EdgeMeshingPropertySpecificSize*	props	=
					dynamic_cast<const EdgeMeshingPropertySpecificSize*> (&cedp);
	if (0 == props)
		return false;

	if (m_edge_size != props->m_edge_size)
		return false;

	return CoEdgeMeshingProperty::operator == (cedp);
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertySpecificSize::update(Geom::GeomEntity* ge)
{
	if (ge == 0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr<<"Il n'est pas possible d'affecter une méthode EdgeMeshingPropertySpecificSize pour une arête "
				<<" car elle n'est pas associée à une entité géométrique";
		throw TkUtil::Exception(messErr);
	}

	if (ge->getDim() != 1){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr<<"Il n'est pas prévu d'associer une méthode EdgeMeshingPropertySpecificSize pour une arête "
				<<" sur autre chose qu'une courbe";
		throw TkUtil::Exception(messErr);
	}

	double lg = ge->getArea();

	m_nb_edges = lg / m_edge_size;
	// au moins une arête
	if (m_nb_edges == 0)
		m_nb_edges = 1;

}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertySpecificSize::
nextCoeff()
{
    m_dernierIndice+=1;

#ifdef _DEBUG
    if (m_dernierIndice>m_nb_edges){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr<<"EdgeMeshingPropertySpecificSize::nextCoeff est en dehors des limites: dernierIndice : "
                <<(long)m_dernierIndice<<", nb de bras : "<<(long)m_nb_edges;
        throw TkUtil::Exception(messErr);
    }
#endif

    return ((double)m_dernierIndice)/((double)m_nb_edges);
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertySpecificSize::
getScriptCommand() const
{
    TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertySpecificSize(" << (double)m_edge_size<< ")";
    return o;
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertySpecificSize::
addDescription(Utils::SerializedRepresentation& topoProprietes) const
{
	// on considère que le nombre de bras et le type de maillage sont déjà donnés.
	topoProprietes.addProperty (
			Utils::SerializedRepresentation::Property ("Taille demandée", m_edge_size));
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
