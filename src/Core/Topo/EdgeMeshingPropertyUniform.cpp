/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyUniform.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24 janv. 2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/EdgeMeshingPropertyUniform.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyUniform::
EdgeMeshingPropertyUniform(int nb)
: CoEdgeMeshingProperty(nb, uniforme, true)
{
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyUniform::
EdgeMeshingPropertyUniform(int nb, Utils::Math::Point polar_center)
: CoEdgeMeshingProperty(nb, polar_center, uniforme, true)
{
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyUniform::EdgeMeshingPropertyUniform (const CoEdgeMeshingProperty& prop)
: CoEdgeMeshingProperty (prop.getNbEdges(), uniforme, prop.getDirect())
{
	const EdgeMeshingPropertyUniform*	p = reinterpret_cast<const EdgeMeshingPropertyUniform*>(&prop);
	if (0 == p)
	{
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr << "EdgeMeshingPropertyUniform, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyUniform";
	        throw TkUtil::Exception(messErr);
	}	// if (0 == p)
}
/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyUniform::operator == (const CoEdgeMeshingProperty& cedp) const
{
	const EdgeMeshingPropertyUniform*	props	=
					dynamic_cast<const EdgeMeshingPropertyUniform*> (&cedp);
	if (0 == props)
		return false;

	return CoEdgeMeshingProperty::operator == (cedp);
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyUniform::
nextCoeff()
{
    m_dernierIndice+=1;

#ifdef _DEBUG
    if (m_dernierIndice>m_nb_edges){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr<<"EdgeMeshingPropertyUniform::nextCoeff est en dehors des limites: dernierIndice : "
                <<(long)m_dernierIndice<<", nb de bras : "<<(long)m_nb_edges;
        throw TkUtil::Exception(messErr);
    }
#endif

    return ((double)m_dernierIndice)/((double)m_nb_edges);
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertyUniform::
getScriptCommand() const
{
    TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertyUniform(" << (long)m_nb_edges ;
    if (isPolarCut())
    	o << ", "<<getPolarCenter().getScriptCommand();
    o	<< ")";
    return o;
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyUniform::
addDescription(Utils::SerializedRepresentation& topoProprietes) const
{
	// on considère que le nombre de bras et le type de maillage sont déjà donnés.
	// rien à ajouter
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
