/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyGeometric.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24 janv. 2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/EdgeMeshingPropertyGeometric.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_raison
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyGeometric::
EdgeMeshingPropertyGeometric(int nbBras, double raison, bool isDirect, double meshingEdgeLength)
: CoEdgeMeshingProperty(nbBras, geometrique, isDirect)
, m_raison(raison)
, m_arm1(meshingEdgeLength)
, m_somme(0.0)
, m_dernierSommeCoeff(0.0)
, m_dernierCoeff(0.0)
, m_initWithArm1(meshingEdgeLength!=0.0)
{
    if (m_arm1 == 0.0 && m_raison<=0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyGeometric, la raison doit être positive et non : "<<m_raison;
        throw TkUtil::Exception(messErr);
    }
    if (m_arm1 < 0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    	messErr << "EdgeMeshingPropertyGeometric, la longueur du premier bras ne doit pas être négative : "<<m_arm1;
    	throw TkUtil::Exception(messErr);
    }
    if (nbBras<1)
        throw TkUtil::Exception (TkUtil::UTF8String ("EdgeMeshingPropertyGeometric, le nombre de bras doit être au moins de 1", TkUtil::Charset::UTF_8));

    // calcul de la somme à l'aide de la raison
    if (!m_initWithArm1)
    	initSomme();
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyGeometric::
EdgeMeshingPropertyGeometric(int nbBras, double raison, Utils::Math::Point polar_center, bool isDirect, double meshingEdgeLength)
: CoEdgeMeshingProperty(nbBras, polar_center, geometrique, isDirect)
, m_raison(raison)
, m_arm1(meshingEdgeLength)
, m_somme(0.0)
, m_dernierSommeCoeff(0.0)
, m_dernierCoeff(0.0)
, m_initWithArm1(meshingEdgeLength!=0.0)
{
    if (m_arm1 == 0.0 && m_raison<=0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyGeometric, la raison doit être positive et non : "<<m_raison;
        throw TkUtil::Exception(messErr);
    }
    if (m_arm1 < 0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    	messErr << "EdgeMeshingPropertyGeometric, la longueur du premier bras ne doit pas être négative : "<<m_arm1;
    	throw TkUtil::Exception(messErr);
    }
    if (nbBras<1)
        throw TkUtil::Exception (TkUtil::UTF8String ("EdgeMeshingPropertyGeometric, le nombre de bras doit être au moins de 1", TkUtil::Charset::UTF_8));

    // calcul de la somme à l'aide de la raison
    if (!m_initWithArm1)
    	initSomme();
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyGeometric::EdgeMeshingPropertyGeometric (const CoEdgeMeshingProperty& prop)
: CoEdgeMeshingProperty (prop.getNbEdges(), geometrique, prop.getDirect())
, m_raison(0.)
, m_arm1(0.)
, m_somme(0.)
, m_dernierSommeCoeff(0.)
, m_dernierCoeff(0.)
, m_initWithArm1(0.)
{
	const EdgeMeshingPropertyGeometric*	p = reinterpret_cast<const EdgeMeshingPropertyGeometric*>(&prop);
	if (0 == p)
	{
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr << "EdgeMeshingPropertyGeometric, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyGeometric";
	        throw TkUtil::Exception(messErr);
	}	// if (0 == p)

	m_raison			= p->m_raison;
	m_arm1				= p->m_arm1;
	m_somme				= p->m_somme;
	m_dernierSommeCoeff		= p->m_dernierSommeCoeff;
	m_dernierCoeff			= p->m_dernierCoeff;
	m_initWithArm1			= p->m_initWithArm1;
}
/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyGeometric::operator == (const CoEdgeMeshingProperty& cedp) const
{
	const EdgeMeshingPropertyGeometric*	props	=
					dynamic_cast<const EdgeMeshingPropertyGeometric*> (&cedp);
	if (0 == props)
		return false;

	if (m_raison != props->m_raison)
		return false;

	return CoEdgeMeshingProperty::operator == (cedp);
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyGeometric::setNbEdges(const int nb)
{
    CoEdgeMeshingProperty::setNbEdges(nb);
    initSomme();
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyGeometric::initSomme()
{
  // calcul de la somme
  double val = 1.0;
  m_somme = 0.0;
  for (int i=1; i<=m_nb_edges; i++){
    val*=m_raison;
    m_somme += val;
  }
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyGeometric::initCoeff(double length)
{
	if (m_initWithArm1){
		// c'est l'occasion de calculer la raison
		m_raison = computeRaison(length/m_arm1);
		initSomme();
	}
	initCoeff();
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyGeometric::initCoeff()
{
#ifdef _DEBUG_raison
	std::cout<<"EdgeMeshingPropertyGeometric::initCoeff(), m_somme "<<m_somme<<std::endl;
#endif
    if (m_raison==0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur interne: EdgeMeshingPropertyGeometric, la raison n'a pas été initialisée";
        throw TkUtil::Exception(messErr);
    }

    if (m_somme==0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur interne: EdgeMeshingPropertyGeometric, la somme n'a pas été initialisée";
        throw TkUtil::Exception(messErr);
    }

    m_dernierIndice = 0;
    if (m_sens){
    	m_dernierCoeff = 1.0/m_somme;
    	m_dernierSommeCoeff = 0.0;
    }
    else {
    	m_dernierCoeff = 1.0;
    	for (int i=0; i<=m_nb_edges; i++)
    		m_dernierCoeff *= m_raison;
    	m_dernierCoeff /= m_somme;
    	m_dernierSommeCoeff = 0.0;
    }
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyGeometric::
nextCoeff()
{
    m_dernierIndice+=1;

#ifdef _DEBUG
    if (m_dernierIndice>m_nb_edges){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr<<"EdgeMeshingPropertyGeometric::nextCoeff est en dehors des limites: dernierIndice : "
                <<(long)m_dernierIndice<<", nb de bras : "<<(long)m_nb_edges;
        throw TkUtil::Exception(messErr);
    }
#endif
    if (m_sens){
        m_dernierCoeff *= m_raison;
        m_dernierSommeCoeff += m_dernierCoeff;
    }
    else {
        m_dernierCoeff /= m_raison;
        m_dernierSommeCoeff += m_dernierCoeff;
    }

#ifdef _DEBUG_raison
	std::cout<<"EdgeMeshingPropertyGeometric::nextCoeff pour m_dernierIndice="<<m_dernierIndice<<" return "<<m_dernierSommeCoeff<<std::endl;
#endif
    return m_dernierSommeCoeff;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertyGeometric::
getScriptCommand() const
{
    TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertyGeometric("
      << (long)m_nb_edges <<","
      << Utils::Math::MgxNumeric::userRepresentation(m_raison);
    if (isPolarCut())
    	o << ", "<<getPolarCenter().getScriptCommand();
    if (!getDirect())
    	o << ", False";
    if (m_initWithArm1){
    	 if (getDirect())
    		 o << ", True"; // sinon Python bloque
    	o << ","<<Utils::Math::MgxNumeric::userRepresentation(m_arm1);
    }
    o << ")";
    return o;
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyGeometric::
addDescription(Utils::SerializedRepresentation& topoProprietes) const
{
	// on considère que le nombre de bras et le type de maillage sont déjà donnés.

	if (m_initWithArm1){
		topoProprietes.addProperty (
				Utils::SerializedRepresentation::Property ("Longueur 1er bras", m_arm1));
		topoProprietes.addProperty (
				Utils::SerializedRepresentation::Property ("Raison (calculée)", m_raison));
	}
	else {
		topoProprietes.addProperty (
				Utils::SerializedRepresentation::Property ("Raison", m_raison));
	}

	if (getDirect())
		topoProprietes.addProperty (
				Utils::SerializedRepresentation::Property ("Sens", std::string("direct")));
	else
		topoProprietes.addProperty (
				Utils::SerializedRepresentation::Property ("Sens", std::string("inverse")));

}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyGeometric::
computeRaison(const double lg)
{
#ifdef _DEBUG_raison
	std::cout<<"EdgeMeshingPropertyGeometric::computeRaison("<<lg<<")"<<std::endl;
#endif

	// recherche de la raison pour laquelle somme des raison**j de j à m_nb_edges-1 == lg
	double raison = 0.0;

	// on limite la recherche
	double raison1 = 0.25;
	double raison2 = 4.0;
	double lg1 = computePolynome(raison1);
	double lg2 = computePolynome(raison2);

	if ((lg1<lg && lg2<lg) || (lg1>lg && lg2>lg)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyGeometric, on ne peut pas trouver de raison pour la suite géométrique pour lg "
        		<<lg<<" et nombre de bras de "<<(short)m_nb_edges;
        throw TkUtil::Exception(messErr);
	}
	uint iter = 0;
	bool sens = (lg1<lg2);
	while (!Utils::Math::MgxNumeric::isNearlyZero(raison2-raison1) && iter < 100){
		iter ++;
		raison = (raison1+raison2)/2.0;

		double lg_iter = computePolynome(raison);
#ifdef _DEBUG_raison
		std::cout<<"iter "<<iter<<std::endl;
		std::cout<<" raison "<<raison<<", lg_iter "<<lg_iter<<std::endl;
#endif

		if (sens){
			if (lg_iter>lg)
				raison2 = raison;
			else
				raison1 = raison;
		}
		else {
			if (lg_iter<lg)
				raison2 = raison;
			else
				raison1 = raison;
		}
	} // end while

#ifdef _DEBUG_raison
	std::cout<<"EdgeMeshingPropertyGeometric::computeRaison return "<<raison<<", iter "<<iter<<std::endl;
#endif
	return raison;
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyGeometric::
computePolynome(const double raison)
{
	if (Utils::Math::MgxNumeric::isNearlyZero(raison-1.0))
		return m_nb_edges;
	else
		return (1.0-pow(raison, m_nb_edges))/(1.0-raison);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
