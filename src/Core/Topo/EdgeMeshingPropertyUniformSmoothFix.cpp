/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyUniformSmoothFix.cpp
 *
 *  \date 24 janv. 2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/EdgeMeshingPropertyUniformSmoothFix.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
    EdgeMeshingPropertyUniformSmoothFix::
    EdgeMeshingPropertyUniformSmoothFix(int nb)
: CoEdgeMeshingProperty(nb, uniforme_smoothfix, true)
{
}
/*----------------------------------------------------------------------------*/
    EdgeMeshingPropertyUniformSmoothFix::
    EdgeMeshingPropertyUniformSmoothFix(int nb, Utils::Math::Point polar_center)
: CoEdgeMeshingProperty(nb, polar_center, uniforme_smoothfix, true)
{
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyUniformSmoothFix::EdgeMeshingPropertyUniformSmoothFix (const CoEdgeMeshingProperty& prop)
: CoEdgeMeshingProperty (prop.getNbEdges(), uniforme_smoothfix, prop.getDirect())
{
	const EdgeMeshingPropertyUniformSmoothFix*	p = reinterpret_cast<const EdgeMeshingPropertyUniformSmoothFix*>(&prop);
	if (0 == p)
	{
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr << "EdgeMeshingPropertyUniformSmoothFix, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyUniformSmoothFix";
	        throw TkUtil::Exception(messErr);
	}	// if (0 == p)
}

/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyUniformSmoothFix::operator == (const CoEdgeMeshingProperty& cedp) const
{
	const EdgeMeshingPropertyUniformSmoothFix*	props	=
					dynamic_cast<const EdgeMeshingPropertyUniformSmoothFix*> (&cedp);
	if (0 == props)
		return false;

	return CoEdgeMeshingProperty::operator == (cedp);
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyUniformSmoothFix::
nextCoeff()
{
    m_dernierIndice+=1;

#ifdef _DEBUG
    if (m_dernierIndice>m_nb_edges){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr<<"EdgeMeshingPropertyUniformSmoothFix::nextCoeff est en dehors des limites: dernierIndice : "
                <<(long)m_dernierIndice<<", nb de bras : "<<(long)m_nb_edges;
        throw TkUtil::Exception(messErr);
    }
#endif

    return ((double)m_dernierIndice)/((double)m_nb_edges);
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertyUniformSmoothFix::
getScriptCommand() const
{
    TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertyUniformSmoothFix(" << (long)m_nb_edges ;
    if (isPolarCut())
    	o << ", "<<getPolarCenter().getScriptCommand();
    o	<< ")";
    return o;
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyUniformSmoothFix::
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
