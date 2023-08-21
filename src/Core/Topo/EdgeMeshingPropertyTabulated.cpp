/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyTabulated.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/2/2015
 */
/*----------------------------------------------------------------------------*/
#include "Topo/EdgeMeshingPropertyTabulated.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Topo/CoEdge.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyTabulated::
EdgeMeshingPropertyTabulated(std::vector<double>& tabulation)
: CoEdgeMeshingProperty(tabulation.size(), tabulated, true)
, m_tabulation(tabulation)
, m_dernierCoeff(0)
, m_somme(0)
, m_dernierIndice(0)
{
#ifdef _DEBUG2
	std::cout<<"EdgeMeshingPropertyTabulated avec "<<tabulation.size()<<" tabulations"<<std::endl;
	for (uint i=0; i<tabulation.size(); i++)
		std::cout<<" "<<tabulation[i];
	std::cout<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyTabulated::EdgeMeshingPropertyTabulated (const CoEdgeMeshingProperty& prop)
: CoEdgeMeshingProperty (prop.getNbEdges(), tabulated, prop.getDirect())
, m_tabulation()
, m_dernierCoeff(0)
, m_somme(0)
, m_dernierIndice(0)
{
	const EdgeMeshingPropertyTabulated*	p = reinterpret_cast<const EdgeMeshingPropertyTabulated*>(&prop);
	if (0 == p)
	{
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr << "EdgeMeshingPropertyTabulated, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyTabulated";
	        throw TkUtil::Exception(messErr);
	}	// if (0 == p)

	m_tabulation	= p->m_tabulation;
	m_dernierCoeff	= p->m_dernierCoeff;
	m_somme		= p->m_somme;
	m_dernierIndice	= p->m_dernierIndice;
}
/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyTabulated::operator == (const CoEdgeMeshingProperty& cedp) const
{
	const EdgeMeshingPropertyTabulated*	props	=
					dynamic_cast<const EdgeMeshingPropertyTabulated*> (&cedp);
	if (0 == props)
		return false;

	return CoEdgeMeshingProperty::operator == (cedp);
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyTabulated::initCoeff()
{
	m_dernierCoeff = 0.0;
	m_somme = 0.0;
	m_dernierIndice = 0;
	for (uint i=0; i<m_tabulation.size(); ++i)
		m_somme += m_tabulation[i];

}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyTabulated::nextCoeff()
{
	m_dernierCoeff+=m_tabulation[m_dernierIndice++];
    return m_dernierCoeff/m_somme;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertyTabulated::
getScriptCommand() const
{
    TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertyTabulated([";
    for (uint i=0; i<m_tabulation.size(); i++){
       	if (i!=0)
       		o<<", ";
       	o<<m_tabulation[i];
    }
      o << "])";
    return o;
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyTabulated::
addDescription(Utils::SerializedRepresentation& topoProprietes) const
{
	// on considère que le nombre de bras et le type de maillage sont déjà donnés.
	topoProprietes.addProperty (
			Utils::SerializedRepresentation::Property ("Tabulation", m_tabulation.size()));
}
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
