//
// Created by rochec on 24/02/2026.
//
/*----------------------------------------------------------------------------*/
#include "Topo/EdgeMeshingPropertyDoubleExponential.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyDoubleExponential::
EdgeMeshingPropertyDoubleExponential(int nb, double r1, double sp1, double r2, double sp2, bool isDirect)
    : CoEdgeMeshingProperty(nb, bigeometrique, isDirect)
      , m_r1(r1)
      , m_sp1(sp1)
      , m_r2(r2)
      , m_sp2(sp2)
      , m_length(0.0)
{
    m_A4 = 0.0;
    if (m_r1 <= 0.0) {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyDoubleExponential, la première raison doit être positive et non : " <<
                m_r1;
        throw TkUtil::Exception(messErr);
    }
    if (m_r2 <= 0.0) {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyDoubleExponential, la deuxième raison doit être positive et non : " <<
                m_r2;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp1 < 0.0) {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr <<
                "EdgeMeshingPropertyDoubleExponential, la longueur du premier bras doit être positive et non : "
                << m_sp1;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp2 < 0.0) {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr <<
                "EdgeMeshingPropertyDoubleExponential, la longueur du dernier bras doit être positive et non : "
                << m_sp2;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp1 == 0 && m_sp2 == 0) {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr <<
                "EdgeMeshingPropertyDoubleExponential, une seule des extrémités peut avoir une taille nulle de spécifiée";
        throw TkUtil::Exception(messErr);
    }

    if (nb < 1)
        throw TkUtil::Exception(TkUtil::UTF8String(
            "EdgeMeshingPropertyDoubleExponential, le nombre de bras doit être au moins de 1",
            TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyDoubleExponential::
EdgeMeshingPropertyDoubleExponential(const EdgeMeshingPropertyDoubleExponential &prop)
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
EdgeMeshingPropertyDoubleExponential::EdgeMeshingPropertyDoubleExponential(const CoEdgeMeshingProperty &prop)
    : CoEdgeMeshingProperty(prop.getNbEdges(), bigeometrique, prop.getDirect())
      , m_r1(0.)
      , m_sp1(0.)
      , m_r2(0.)
      , m_sp2(0.)
      , m_length(0.)
{
    const auto *bg = reinterpret_cast<const EdgeMeshingPropertyDoubleExponential *>(&prop);
    if (!bg)
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr <<
                "EdgeMeshingPropertyDoubleExponential, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyDoubleExponential";
        throw TkUtil::Exception(messErr);
    }

    m_r1 = bg->m_r1;
    m_sp1 = bg->m_sp1;
    m_r2 = bg->m_r2;
    m_sp2 = bg->m_sp2;
    m_length = bg->m_length;
}

/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyDoubleExponential::operator ==(const CoEdgeMeshingProperty &cedp) const
{
    const auto *props = dynamic_cast<const EdgeMeshingPropertyDoubleExponential *>(&cedp);
    if (!props)
        return false;

    if ((m_r1 != props->m_r1) || (m_r2 != props->m_r2) ||
        (m_sp1 != props->m_sp1) || (m_sp2 != props->m_sp2))
        return false;

    return CoEdgeMeshingProperty::operator ==(cedp);
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyDoubleExponential::initCoeff(const double length)
{
    m_length = length;
    m_A4 = computeParamExpo();
    m_dernierIndice = 0;
}

/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyDoubleExponential::initCoeff()
{
    TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
    messErr <<
            "Erreur interne: EdgeMeshingPropertyDoubleExponential, initCoeff ne peut se faire sans une longueur";
    throw TkUtil::Exception(messErr);
}

/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyDoubleExponential::
nextCoeff()
{
    m_dernierIndice += 1;
    double refParameter(0.0);
    double intermediateParameter(0.0);
    if (m_sens)
    {
        refParameter = static_cast<double>(m_dernierIndice) / static_cast<double>(m_nb_edges);
        intermediateParameter = computeIntermediateParameter(refParameter);
    }
    else
    {
        refParameter = static_cast<double>(m_nb_edges-m_dernierIndice) / static_cast<double>(m_nb_edges);
        intermediateParameter = 1.0 - computeIntermediateParameter(refParameter);
    }
    return intermediateParameter;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertyDoubleExponential::
getScriptCommand() const
{
    TkUtil::UTF8String o(TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertyDoubleExponential("
            << static_cast<long>(m_nb_edges) << ","
            << Utils::Math::MgxNumeric::userRepresentation(m_r1) << ","
            << Utils::Math::MgxNumeric::userRepresentation(m_sp1) << ","
            << Utils::Math::MgxNumeric::userRepresentation(m_r2) << ","
            << Utils::Math::MgxNumeric::userRepresentation(m_sp2);
    if (isPolarCut())
        o << ", " << getPolarCenter().getScriptCommand();
    if (!getDirect())
        o << ", False";
    o << ")";
    return o;
}

/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyDoubleExponential::
addDescription(Utils::SerializedRepresentation &topoProprietes) const
{
    topoProprietes.addProperty(
        Utils::SerializedRepresentation::Property("Longueur premier bras", m_sp1));
    topoProprietes.addProperty(
        Utils::SerializedRepresentation::Property("Raison 1", m_r1));
    topoProprietes.addProperty(
        Utils::SerializedRepresentation::Property("Longueur dernier bras", m_sp2));
    topoProprietes.addProperty(
        Utils::SerializedRepresentation::Property("Raison 2", m_r2));

    if (getDirect())
        topoProprietes.addProperty(
            Utils::SerializedRepresentation::Property("Sens", std::string("direct")));
    else
        topoProprietes.addProperty(
            Utils::SerializedRepresentation::Property("Sens", std::string("inverse")));
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyDoubleExponential::
computeParamExpo() const
{
    double A4 = 1.0;
    const double A1 = m_length-m_sp2;
    const double A3 = (static_cast<double>(m_nb_edges)-1.0)/static_cast<double>(m_nb_edges);
    // we normalize the tolerence by a characteristic length
    const double eps = m_length*1e-9;
    constexpr int maxIter = 100;
    int iter = 0;
    double F = 1.0;

    std::cout << "length: " << m_length << ", eps: " << eps << std::endl;
    std::cout << "arm 1: " << m_sp1 << std::endl;

    while ( fabs(F) > eps && iter < maxIter)
    {
        const double b =  ((1.0/m_nb_edges)-A3)/(1.0-A3);
        const double u  = std::exp(A4*b) - 1.0;
        const double v  = std::exp(A4)-1.0;
        F = - m_sp1 + m_length*(A1+(1.0-A1)*(u/v));
        const double up = b*std::exp(A4*b) ;
        const double vp = std::exp(A4);
        const double Fp = m_length*(1.0-A1)*(up*v-u*vp)/(v*v) ;
        std::cout << "iter: " << iter << ", Fp: " << Fp << std::endl;

        if (Fp != 0.0)
        {
            A4 = A4 - F/Fp;
        }

        iter++;
    }
    std::cout << "nbr iter: " << iter << std::endl;
    std::cout << "F: " << F << std::endl;
    std::cout << "paramExpo: " << A4 << std::endl;

    return A4;
}
/*----------------------------------------------------------------------------*/
double
EdgeMeshingPropertyDoubleExponential::computeIntermediateParameter(const double refParameter) const
{
    double A1 = m_length-m_sp2;
    double A3 = (static_cast<double>(m_nb_edges)-1.0)/static_cast<double>(m_nb_edges) ;
    double intermediateParameter(0.0);
    /*
    if (refParameter < 0.5)
    {
        intermediateParameter = A1*(std::exp( m_r1*refParameter/A3) -1.0 )/(std::exp(m_r1)-1.0);
    }
    else
    {*/
        double eNum = std::exp( m_A4*( refParameter -  A3)/(1.0-A3) );
        intermediateParameter = A1 + (1.0-A1)* (eNum - 1.0)/(std::exp(m_A4)-1.0);
    //}
    return intermediateParameter;
}

/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
