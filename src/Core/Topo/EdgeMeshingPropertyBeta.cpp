/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyBeta.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24 janv. 2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/EdgeMeshingPropertyBeta.h"
#include <TkUtil/UTF8String.h>
#include "TkUtil/Exception.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyBeta::
EdgeMeshingPropertyBeta(int nb, double beta, bool isDirect)
: CoEdgeMeshingProperty(nb, beta_resserrement, isDirect)
, m_beta(beta)
{
    if (m_beta<=1.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBeta, la valeur de beta doit être légèrement supérieur à 1, actuellement : "<<m_beta;
        throw TkUtil::Exception(messErr);
    }
    if (m_beta>2.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    	messErr << "EdgeMeshingPropertyBeta, la longueur de beta n'a pas été testé jusque là "<<m_beta;
    	throw TkUtil::Exception(messErr);
    }
    if (nb<1)
        throw TkUtil::Exception (TkUtil::UTF8String ("EdgeMeshingPropertyBeta, le nombre de bras doit être au moins de 1", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyBeta::EdgeMeshingPropertyBeta (const CoEdgeMeshingProperty& prop)
: CoEdgeMeshingProperty (prop.getNbEdges(), beta_resserrement, prop.getDirect())
, m_beta(0.)
{
	const EdgeMeshingPropertyBeta*	p = reinterpret_cast<const EdgeMeshingPropertyBeta*>(&prop);
	if (0 == p)
	{
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr << "EdgeMeshingPropertyBeta, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyBeta";
	        throw TkUtil::Exception(messErr);
	}	// if (0 == p)

	m_beta		= p->m_beta;
}
/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyBeta::operator == (const CoEdgeMeshingProperty& cedp) const
{
	const EdgeMeshingPropertyBeta*	props	=
					dynamic_cast<const EdgeMeshingPropertyBeta*> (&cedp);
	if (0 == props)
		return false;

	if (m_beta != props->m_beta)
		return false;

	return CoEdgeMeshingProperty::operator == (cedp);
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyBeta::
nextCoeff()
{
    m_dernierIndice+=1;

#ifdef _DEBUG
    if (m_dernierIndice>m_nb_edges){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr<<"EdgeMeshingPropertyBeta::nextCoeff est en dehors des limites: dernierIndice : "
                <<(long)m_dernierIndice<<", nb de bras : "<<(long)m_nb_edges;
        throw TkUtil::Exception(messErr);
    }
#endif

    double feta=0.0;
    double eta = 0.0;
    if (m_sens){
    	eta = ((double)m_dernierIndice)/((double)m_nb_edges);
    	feta=resserre(eta);
    }
    else {
    	eta = ((double)m_nb_edges-m_dernierIndice)/((double)m_nb_edges);
    	feta = 1.0-resserre(eta);
    }

#ifdef _DEBUG2
	std::cout<<"EdgeMeshingPropertyBeta::nextCoeff (m_beta="<<m_beta<<") return "<<feta<<" pour eta = "<<eta<<std::endl;
#endif
    return feta;
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyBeta::resserre(double eta)
{
	double ratio = (m_beta + 1.0) / (m_beta - 1.0);
	double zlog = std::log(ratio);
	double puiss  =  zlog * (1.0 - eta);
	puiss  =  std::exp(puiss);
	double rapp   =  (1.0 - puiss) / (1.0 + puiss);
	double  feta   =  1.0 + m_beta * rapp;
	return feta;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertyBeta::
getScriptCommand() const
{
    TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertyBeta("
      << (long)m_nb_edges <<","
      << Utils::Math::MgxNumeric::userRepresentation(m_beta);
    if (!getDirect())
    	o << ", False";
    o << ")";
    return o;
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyBeta::
addDescription(Utils::SerializedRepresentation& topoProprietes) const
{
	topoProprietes.addProperty (
			Utils::SerializedRepresentation::Property ("Beta Resserrement", m_beta));

	if (getDirect())
		topoProprietes.addProperty (
				Utils::SerializedRepresentation::Property ("Sens", std::string("direct")));
	else
		topoProprietes.addProperty (
				Utils::SerializedRepresentation::Property ("Sens", std::string("inverse")));
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
