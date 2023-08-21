/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyBigeometric.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 26/8/2014
 */
/*----------------------------------------------------------------------------*/
#include "Topo/EdgeMeshingPropertyBigeometric.h"
#include <TkUtil/UTF8String.h>
#include "TkUtil/Exception.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_BIGEOM
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyBigeometric::
EdgeMeshingPropertyBigeometric(int nb, double r1, double sp1, double r2, double sp2, bool isDirect)
: CoEdgeMeshingProperty(nb, bigeometrique, isDirect)
, m_r1(r1)
, m_sp1(sp1)
, m_r2(r2)
, m_sp2(sp2)
, m_length(0.0)
{
    if (m_r1 <= 0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBigeometric, la première raison doit être positive et non : "<<m_r1;
        throw TkUtil::Exception(messErr);
    }
    if (m_r2 <= 0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBigeometric, la deuxième raison doit être positive et non : "<<m_r2;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp1 < 0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBigeometric, la longueur du premier bras doit être positive et non : "<<m_sp1;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp2 < 0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBigeometric, la longueur du dernier bras doit être positive et non : "<<m_sp2;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp1 == 0 && m_sp2 == 0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBigeometric, une seule des extrémités peut avoir une taille nulle de spécifiée";
        throw TkUtil::Exception(messErr);
    }

    if (nb<1)
        throw TkUtil::Exception (TkUtil::UTF8String ("EdgeMeshingPropertyBigeometric, le nombre de bras doit être au moins de 1", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyBigeometric::
EdgeMeshingPropertyBigeometric(int nb, double r1, double sp1, double r2, double sp2, Utils::Math::Point polar_center, bool isDirect)
: CoEdgeMeshingProperty(nb, polar_center, bigeometrique, isDirect)
, m_r1(r1)
, m_sp1(sp1)
, m_r2(r2)
, m_sp2(sp2)
, m_length(0.0)
{
    if (m_r1 <= 0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBigeometric, la première raison doit être positive et non : "<<m_r1;
        throw TkUtil::Exception(messErr);
    }
    if (m_r2 <= 0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBigeometric, la deuxième raison doit être positive et non : "<<m_r2;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp1 < 0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBigeometric, la longueur du premier bras doit être positive et non : "<<m_sp1;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp2 < 0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBigeometric, la longueur du dernier bras doit être positive et non : "<<m_sp2;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp1 == 0 && m_sp2 == 0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBigeometric, une seule des extrémités peut avoir une taille nulle de spécifiée";
        throw TkUtil::Exception(messErr);
    }

    if (nb<1)
        throw TkUtil::Exception (TkUtil::UTF8String ("EdgeMeshingPropertyBigeometric, le nombre de bras doit être au moins de 1", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyBigeometric::
EdgeMeshingPropertyBigeometric(const EdgeMeshingPropertyBigeometric& prop)
: CoEdgeMeshingProperty(prop.getNbEdges(), bigeometrique, prop.getDirect())
, m_r1(prop.m_r1)
, m_sp1(prop.m_sp1)
, m_r2(prop.m_r2)
, m_sp2(prop.m_sp2)
, m_length(prop.m_length)
{
	m_coeff.insert(m_coeff.end(), prop.m_coeff.begin(), prop.m_coeff.end());
	m_is_polar = prop.m_is_polar;
	m_polar_center = prop.m_polar_center;
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyBigeometric::EdgeMeshingPropertyBigeometric (const CoEdgeMeshingProperty& prop)
: CoEdgeMeshingProperty (prop.getNbEdges(), bigeometrique, prop.getDirect())
, m_r1(0.)
, m_sp1(0.)
, m_r2(0.)
, m_sp2(0.)
, m_length(0.)
{
std::cout << __FILE__ << ' ' << __LINE__ << " EdgeMeshingPropertyBigeometric::EdgeMeshingPropertyBigeometric (const CoEdgeMeshingProperty&)" << std::endl;
	const EdgeMeshingPropertyBigeometric*	bg = reinterpret_cast<const EdgeMeshingPropertyBigeometric*>(&prop);
	if (0 == bg)
	{
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr << "EdgeMeshingPropertyBigeometric, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyBigeometric";
	        throw TkUtil::Exception(messErr);
	}	// if (0 == bg)

	m_r1		= bg->m_r1;
	m_sp1		= bg->m_sp1;
	m_r2		= bg->m_r2;
	m_sp2		= bg->m_sp2;
	m_length	= bg->m_length;
}
/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyBigeometric::operator == (const CoEdgeMeshingProperty& cedp) const
{
	const EdgeMeshingPropertyBigeometric*	props	=
					dynamic_cast<const EdgeMeshingPropertyBigeometric*> (&cedp);
	if (0 == props)
		return false;

	if ((m_r1 != props->m_r1) || (m_r2 != props->m_r2) ||
	    (m_sp1 != props->m_sp1) || (m_sp2 != props->m_sp2))
		return false;

	return CoEdgeMeshingProperty::operator == (cedp);
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyBigeometric::setNbEdges(const int nb)
{
    CoEdgeMeshingProperty::setNbEdges(nb);
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyBigeometric::initCoeff(double length)
{
#ifdef _DEBUG_BIGEOM
	std::cout<<"EdgeMeshingPropertyBigeometric::initCoeff("<<length<<") avec m_length "<<m_length<<std::endl;
#endif
	m_dernierIndice = 0;
	if (!Utils::Math::MgxNumeric::isNearlyZero(length-m_length)){
		init(length);
		m_length = length;
	}
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyBigeometric::initCoeff()
{
	TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
	messErr << "Erreur interne: EdgeMeshingPropertyBigeometric, initCoeff ne peut se faire sans une longueur";
	throw TkUtil::Exception(messErr);
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyBigeometric::
nextCoeff()
{

#ifdef _DEBUG
    if (m_dernierIndice>=m_coeff.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr<<"EdgeMeshingPropertyBigeometric::nextCoeff est en dehors des limites: dernierIndice : "
                <<(long)m_dernierIndice<<", nb de bras : "<<(long)m_nb_edges<<", coeff.size : "<<m_coeff.size();
        throw TkUtil::Exception(messErr);
    }
#endif

    double coeff = 0.0;

    if (m_sens)
    	coeff = m_coeff[m_dernierIndice];
    else
    	coeff = 1.0-m_coeff[m_nb_edges-2-m_dernierIndice];

    m_dernierIndice+=1;

#ifdef _DEBUG_BIGEOM
    std::cout<<"EdgeMeshingPropertyBigeometric::nextCoeff() return "<<coeff<<std::endl;
#endif

    return coeff;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertyBigeometric::
getScriptCommand() const
{
    TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertyBigeometric("
      << (long)m_nb_edges <<","
      << Utils::Math::MgxNumeric::userRepresentation(m_r1) << ","
      << Utils::Math::MgxNumeric::userRepresentation(m_sp1)<< ","
      << Utils::Math::MgxNumeric::userRepresentation(m_r2) << ","
      << Utils::Math::MgxNumeric::userRepresentation(m_sp2);
    if (isPolarCut())
    	o << ", "<<getPolarCenter().getScriptCommand();
    if (!getDirect())
    	o << ", False";
    o << ")";
    return o;
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyBigeometric::
addDescription(Utils::SerializedRepresentation& topoProprietes) const
{
	topoProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Longueur premier bras", m_sp1));
	topoProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Raison 1", m_r1));
	topoProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Longueur dernier bras", m_sp2));
	topoProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Raison 2", m_r2));

	if (getDirect())
		topoProprietes.addProperty (
				Utils::SerializedRepresentation::Property ("Sens", std::string("direct")));
	else
		topoProprietes.addProperty (
				Utils::SerializedRepresentation::Property ("Sens", std::string("inverse")));

}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyBigeometric::
init(double length)
{
#ifdef _DEBUG_BIGEOM
	std::cout<<"EdgeMeshingPropertyBigeometric::init("<<length<<") avec m_r1 "
			<<m_r1<<", m_sp1 "<<m_sp1<<", m_r2 "<<m_r2<<", m_sp2 "<<m_sp2<<std::endl;
#endif

	m_coeff.clear();

	// utilisation de 2 vecteurs pour stocker les longueurs cumulées des bras
	std::vector<double> lg_bras1;
	std::vector<double> lg_bras2;

	double lg = 0.0;
	if (Utils::Math::MgxNumeric::isNearlyZero(m_sp1))
		lg_bras1.push_back(0.0);
	else if (Utils::Math::MgxNumeric::isNearlyZero(m_r1-1.0))
		for (uint i=0; i<m_nb_edges && lg<length; i++){
			lg = m_sp1*(i+1);
#ifdef _DEBUG_BIGEOM
			std::cout<<"lg_bras1.size(): "<<lg_bras1.size()<<", push_back "<<lg<<std::endl;
#endif
			lg_bras1.push_back(lg);
		}
	else
		for (uint i=0; i<m_nb_edges && lg<length; i++){
			lg = m_sp1*(std::pow(m_r1,(double)i)-1)/(m_r1-1);
#ifdef _DEBUG_BIGEOM
			std::cout<<"lg_bras1.size(): "<<lg_bras1.size()<<", push_back "<<lg<<std::endl;
#endif
			lg_bras1.push_back(lg);
		}

	lg = 0.0;
	if (Utils::Math::MgxNumeric::isNearlyZero(m_sp2))
		lg_bras2.push_back(0.0);
	else if (Utils::Math::MgxNumeric::isNearlyZero(m_r2-1.0))
		for (uint i=0; i<m_nb_edges && lg<length; i++){
			lg = m_sp2*(i+1);
#ifdef _DEBUG_BIGEOM
			std::cout<<"lg_bras2.size(): "<<lg_bras2.size()<<", push_back "<<lg<<std::endl;
#endif
			lg_bras2.push_back(lg);
		}
	else
		for (uint i=0; i<m_nb_edges && lg<length; i++){
			lg = m_sp2*(std::pow(m_r2,(double)i)-1)/(m_r2-1);
#ifdef _DEBUG_BIGEOM
			std::cout<<"lg_bras2.size(): "<<lg_bras2.size()<<", push_back "<<lg<<std::endl;
#endif
			lg_bras2.push_back(lg);
		}

	uint i1 = getLastIndex(lg_bras1, lg_bras2, length);
	uint i2 = getLastIndex(lg_bras2, lg_bras1, length);


#ifdef _DEBUG_BIGEOM
	std::cout<<" i1 = "<<i1<<std::endl;
	std::cout<<" i2 = "<<i2<<std::endl;
#endif

	// indice i2 dans m_coeff
	uint i3 = m_nb_edges-i2;

	// on commence avec une partie de la premiere courbe
	for (uint i=1; i<=i1; i++)
		m_coeff.push_back(lg_bras1[i]);

	// linéaire entre i1 et i3
	double y1 = lg_bras1[i1];
	double y3 = length-lg_bras2[i2];
	double delta_lg_i = (y3-y1)/(i3-i1);
	for (uint i=i1+1; i<i3; i++)
		m_coeff.push_back(y1+(i-i1)*delta_lg_i);

	// on termine avec une partie de la deuxième courbe
	for (uint i=i3; i<m_nb_edges; i++)
		m_coeff.push_back(length-lg_bras2[m_nb_edges-i]);

	if (m_nb_edges-1 != m_coeff.size()){
#ifdef _DEBUG_BIGEOM
		std::cout<<" i3 = "<<i3<<std::endl;
		std::cout<<"m_coeff.size() : "<<m_coeff.size()<<" et  m_nb_edges : "<<m_nb_edges<<std::endl;
#endif
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr << "EdgeMeshingPropertyBigeometric::init a un pb, nombre de bras en erreur";
		throw TkUtil::Exception(messErr);
	}

	// normalisation
	for (uint i=0; i<m_coeff.size(); i++)
		m_coeff[i] = m_coeff[i]/length;


#ifdef _DEBUG_BIGEOM
	std::cout<<"Coeff pour GnuPlot de taille "<<m_coeff.size()<<" (m_nb_edges = "<<m_nb_edges<<"):"<<std::endl;
	std::cout<<"0 0.0"<<std::endl;
	for (uint i=0; i<m_coeff.size(); i++)
		std::cout<<i+1<<" "<<m_coeff[i]*length<<std::endl;
	std::cout<<m_nb_edges<<" "<<length<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
uint EdgeMeshingPropertyBigeometric::
getLastIndex(std::vector<double>& lg_bras1, std::vector<double>& lg_bras2, double& length)
{
#ifdef _DEBUG_BIGEOM
	std::cout<<"getLastIndex ..."<<std::endl;
#endif

	// cas d'un seul point, c'est pour le cas d'une taille de bras non spécifiée
	if (lg_bras1.size() == 1)
		return 0;

	for (uint i=0; i<lg_bras1.size()-1; i++){

		double x1 = i;
		double y1 = lg_bras1[i];

		double x2 = i+1;
		double y2 = lg_bras1[i+1];
#ifdef _DEBUG_BIGEOM2
		std::cout<<" point 1 : " <<x1<<", "<<y1<<std::endl;
		std::cout<<" point 2 : " <<x2<<", "<<y2<<std::endl;
#endif

		bool trouvePtDessus = false;
		for (uint j=0; j<lg_bras2.size() && !trouvePtDessus; j++){

			double x3 = m_nb_edges-j;
			double y3 = length-lg_bras2[j];
#ifdef _DEBUG_BIGEOM2
			std::cout<<" point 3 : " <<x3<<", "<<y3<<std::endl;
#endif
			double prod_vect = (x2-x1)*(y3-y1)-(y2-y1)*(x3-x1);
#ifdef _DEBUG_BIGEOM2
			std::cout<<"  prod_vect = "<<prod_vect<<std::endl;
#endif
			if (prod_vect > 0.0)
				trouvePtDessus = true;

		} // end for j

		if (!trouvePtDessus){
#ifdef _DEBUG_BIGEOM2
			std::cout<<"  return i = "<<i<<std::endl;
#endif
			return i;
		}

	} // end for i

#ifdef _DEBUG_BIGEOM
	std::cout<<"Exception: EdgeMeshingPropertyBigeometric n'est pas adapté aux contraintes données en entrée...."<<std::endl;
#endif

	TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
	messErr << "EdgeMeshingPropertyBigeometric n'est pas adapté aux contraintes données en entrée. Il faut les changer ou utiliser EdgeMeshingPropertyHyperbolic.";
	throw TkUtil::Exception(messErr);

}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
