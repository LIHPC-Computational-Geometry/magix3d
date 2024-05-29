/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyHyperbolic.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 29/8/2014
 */
/*----------------------------------------------------------------------------*/
#include "Topo/EdgeMeshingPropertyHyperbolic.h"
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
EdgeMeshingPropertyHyperbolic::
EdgeMeshingPropertyHyperbolic(int nb, double sp1, double sp2, bool isDirect)
: CoEdgeMeshingProperty(nb, hyperbolique, isDirect)
, m_sp1(sp1)
, m_sp2(sp2)
, m_length(0.0)
{
    if (m_sp1 <= 0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyHyperbolic, la longueur du premier bras doit être positive et non : "<<m_sp1;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp2 <= 0.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyHyperbolic, la longueur du dernier bras doit être positive et non : "<<m_sp2;
        throw TkUtil::Exception(messErr);
    }

    if (nb<1)
        throw TkUtil::Exception (TkUtil::UTF8String ("EdgeMeshingPropertyHyperbolic, le nombre de bras doit être au moins de 1", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyHyperbolic::EdgeMeshingPropertyHyperbolic (const CoEdgeMeshingProperty& prop)
: CoEdgeMeshingProperty (prop.getNbEdges(), hyperbolique, prop.getDirect())
, m_sp1(0.)
, m_sp2(0.)
{
	const EdgeMeshingPropertyHyperbolic*	p = reinterpret_cast<const EdgeMeshingPropertyHyperbolic*>(&prop);
	if (0 == p)
	{
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        	messErr << "EdgeMeshingPropertyHyperbolic, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyHyperbolic";
	        throw TkUtil::Exception(messErr);
	}	// if (0 == p)

	m_sp1	= p->m_sp1;
	m_sp2	= p->m_sp2;
}
/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyHyperbolic::operator == (const CoEdgeMeshingProperty& cedp) const
{
	const EdgeMeshingPropertyHyperbolic*	props	=
					dynamic_cast<const EdgeMeshingPropertyHyperbolic*> (&cedp);
	if (0 == props)
		return false;

	if ((m_sp1 != props->m_sp1) || (m_sp2 != props->m_sp2))
		return false;

	return CoEdgeMeshingProperty::operator == (cedp);
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyHyperbolic::setNbEdges(const int nb)
{
    CoEdgeMeshingProperty::setNbEdges(nb);
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyHyperbolic::initCoeff(double length)
{
#ifdef _DEBUG_raison
	std::cout<<"EdgeMeshingPropertyHyperbolic::initCoeff("<<length<<") avec m_length "<<m_length<<std::endl;
#endif
	m_dernierIndice = 0;
	if (!Utils::Math::MgxNumeric::isNearlyZero(length-m_length)){
		init(length);
		m_length = length;
	}
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyHyperbolic::initCoeff()
{
	TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
	messErr << "Erreur interne: EdgeMeshingPropertyHyperbolic, initCoeff ne peut se faire sans une longueur";
	throw TkUtil::Exception(messErr);
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyHyperbolic::nextCoeff()
{
#ifdef _DEBUG
    if (m_dernierIndice>m_coeff.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr<<"EdgeMeshingPropertyHyperbolic::nextCoeff est en dehors des limites: dernierIndice : "
                <<(long)m_dernierIndice<<", nb de bras : "<<(long)m_nb_edges<<", coeff.size : "<<m_coeff.size();
        throw TkUtil::Exception(messErr);
    }
#endif

    double coeff = 0.0;

	if (m_dernierIndice==m_coeff.size()){
		coeff = 1.0;
	}
	else {
		if (m_sens)
			coeff = m_coeff[m_dernierIndice];
		else
			coeff = 1.0-m_coeff[m_nb_edges-2-m_dernierIndice];
	}
    m_dernierIndice+=1;

#ifdef _DEBUG_raison
    std::cout<<"EdgeMeshingPropertyHyperbolic::nextCoeff() pour m_dernierIndice="<<m_dernierIndice<<" return "<<coeff<<std::endl;
#endif

    return coeff;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertyHyperbolic::getScriptCommand() const
{
    TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
	o << getMgx3DAlias() << ".EdgeMeshingPropertyHyperbolic("
			<< (long)m_nb_edges <<","
			<< Utils::Math::MgxNumeric::userRepresentation(m_sp1)<< ","
			<< Utils::Math::MgxNumeric::userRepresentation(m_sp2);
	if (!getDirect())
		o << ", False";
	o << ")";
	return o;
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyHyperbolic::addDescription(Utils::SerializedRepresentation& topoProprietes) const
{
	topoProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Longueur premier bras", m_sp1));
	topoProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Longueur dernier bras", m_sp2));

	if (getDirect())
		topoProprietes.addProperty (
				Utils::SerializedRepresentation::Property ("Sens", std::string("direct")));
	else
		topoProprietes.addProperty (
				Utils::SerializedRepresentation::Property ("Sens", std::string("inverse")));

}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyHyperbolic::init(double length)
{
#ifdef _DEBUG_raison
	std::cout<<"EdgeMeshingPropertyHyperbolic::init("<<length<<") m_sp1 "<<m_sp1<<", m_sp2 "<<m_sp2<<std::endl;
#endif

	m_coeff.clear();

	double A = std::sqrt(m_sp1/m_sp2);

	// on multiplie par length pour normaliser sp1 et sp2
	// et être ainsi en adéquation avec la doc de la version d'icem
	double B = length/(m_nb_edges*std::sqrt(m_sp1*m_sp2));

	// retourne b / sinh(b)/b == B avec b>0
	double b = solveSinhXSurX(B);

#ifdef _DEBUG_raison
	std::cout<<" A = "<< A << std::endl;
	std::cout<<" B = "<< B << std::endl;
	std::cout<<" b = "<< b << std::endl;
#endif

	for (uint i=1; i<m_nb_edges; i++){

		double Ri = (double)i/(double)m_nb_edges - 0.5;
		double Ui = 1.0 + std::tanh(b*Ri)/std::tanh(b/2.0);
		double Si = Ui/(2*A + (1.0-A)*Ui); // version normalisée entre 0 et 1
		m_coeff.push_back(Si);
#ifdef _DEBUG_raison
		std::cout<<"m_coeff["<<i-1<<"] = "<<m_coeff.back()<<std::endl;
#endif
	}
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyHyperbolic::solveSinhXSurX(double B)
{
	// on sait que la solution est >0, la fonction (fortement) croissante et >=1

	if (B<=1.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr << "EdgeMeshingPropertyHyperbolic::solveSinhXSurX, la valeur de B est < 1 : "<<B;
		throw TkUtil::Exception(messErr);
	}


	double b_min = 0.0;
	double val_min = 1.0;

	double b_max = 0.0;
	double val_max = 1.0;

	// recherche d'un b / val_max>B
	do {
		b_max += 1.0;
		val_max = std::sinh(b_max)/b_max;
	} while (val_max<B);

	// recherche dichotomique
	uint iter = 0;
	do {
		iter++;
		double b = (b_min+b_max)/2.0;
		double val = std::sinh(b)/b;

		if (val<B){
			b_min = b;
			val_min = val;
		} else {
			b_max = b;
			val_max = val;
		}

	} while (!Utils::Math::MgxNumeric::isNearlyZero(val_min-val_max) && iter<1000);

	return  (b_min+b_max)/2.0;
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
