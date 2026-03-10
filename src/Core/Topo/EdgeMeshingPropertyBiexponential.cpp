//
// Created by rochec on 27/02/2026.
//
#include "Topo/EdgeMeshingPropertyBiexponential.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyBiexponential::
EdgeMeshingPropertyBiexponential(int nb, double r1, double sp1, double r2, double sp2, bool isDirect)
    : CoEdgeMeshingProperty(nb, bigeometrique, isDirect)
      , m_r1(r1)
      , m_sp1(sp1)
      , m_r2(r2)
      , m_sp2(sp2)
      , m_length(0.0)
{
    m_A4 = 0.0;
    if (m_r1 <= 0.0)
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBiexponential, la première raison doit être positive et non : " <<
                m_r1;
        throw TkUtil::Exception(messErr);
    }
    if (m_r2 <= 0.0)
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBiexponential, la deuxième raison doit être positive et non : " <<
                m_r2;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp1 < 0.0)
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr <<
                "EdgeMeshingPropertyBiexponential, la longueur du premier bras doit être positive et non : "
                << m_sp1;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp2 < 0.0)
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr <<
                "EdgeMeshingPropertyBiexponential, la longueur du dernier bras doit être positive et non : "
                << m_sp2;
        throw TkUtil::Exception(messErr);
    }
    if (m_sp1 == 0 && m_sp2 == 0)
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr <<
                "EdgeMeshingPropertyBiexponential, une seule des extrémités peut avoir une taille nulle de spécifiée";
        throw TkUtil::Exception(messErr);
    }

    if (nb < 1)
        throw TkUtil::Exception(TkUtil::UTF8String(
            "EdgeMeshingPropertyBiexponential, le nombre de bras doit être au moins de 1",
            TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyBiexponential::
EdgeMeshingPropertyBiexponential(const EdgeMeshingPropertyBiexponential &prop)
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
EdgeMeshingPropertyBiexponential::EdgeMeshingPropertyBiexponential(const CoEdgeMeshingProperty &prop)
    : CoEdgeMeshingProperty(prop.getNbEdges(), bigeometrique, prop.getDirect())
      , m_r1(0.)
      , m_sp1(0.)
      , m_r2(0.)
      , m_sp2(0.)
      , m_length(0.)
{
    const auto *bg = reinterpret_cast<const EdgeMeshingPropertyBiexponential *>(&prop);
    if (!bg)
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr <<
                "EdgeMeshingPropertyBiexponential, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyBiexponential";
        throw TkUtil::Exception(messErr);
    }

    m_r1 = bg->m_r1;
    m_sp1 = bg->m_sp1;
    m_r2 = bg->m_r2;
    m_sp2 = bg->m_sp2;
    m_length = bg->m_length;
}

/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyBiexponential::operator ==(const CoEdgeMeshingProperty &cedp) const
{
    const auto *props = dynamic_cast<const EdgeMeshingPropertyBiexponential *>(&cedp);
    if (!props)
        return false;

    if ((m_r1 != props->m_r1) || (m_r2 != props->m_r2) ||
        (m_sp1 != props->m_sp1) || (m_sp2 != props->m_sp2))
        return false;

    return CoEdgeMeshingProperty::operator ==(cedp);
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyBiexponential::initCoeff(const double length)
{
    m_length = length;
    uint n1;
    uint n2;
    findNbrEdgesBalance(n1, n2);
    std::cout << "TARGET SIZES: " << m_sp1 << ", " << m_sp2 << std::endl;
    std::cout << "FINAL BALANCE: " << n1 << ", " << n2 << std::endl;
    std::cout << m_sp1 << std::endl;
    const double paramExpo1 = computeParamExpo(m_sp1, m_length/2.0, n1);
    const double paramExpo2 = computeParamExpo(m_sp2, m_length/2.0, n2);

    std::vector<double> coeffs1 = computeCoeffs(paramExpo1, m_length/2.0, n1);
    std::vector<double> coeffs2 = computeCoeffs(paramExpo2, m_length/2.0, n2);

    m_coeff.clear();
    for (int i =0; i < coeffs1.size(); i++)
    {
        m_coeff.push_back(coeffs1[i]/2.0);
    }
    for (int i=coeffs2.size()-2; i >= 0; i--)
    {
        m_coeff.push_back(0.5 + (1.0-coeffs2[i])/2.0);
    }

    m_dernierIndice = 0;
}

/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyBiexponential::initCoeff()
{
    TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
    messErr <<
            "Erreur interne: EdgeMeshingPropertyBiexponential, initCoeff ne peut se faire sans une longueur";
    throw TkUtil::Exception(messErr);
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyBiexponential::
nextCoeff()
{
    m_dernierIndice += 1;
    double intermediateParameter(0.0);
    if (m_sens)
    {
        intermediateParameter = m_coeff[m_dernierIndice];
    }
    else
    {
        intermediateParameter = 1.0-m_coeff[m_nb_edges-m_dernierIndice];
    }
    std::cout << "coeff: " << intermediateParameter << std::endl;
    return intermediateParameter;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertyBiexponential::
getScriptCommand() const
{
    TkUtil::UTF8String o(TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertyBiexponential("
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
void EdgeMeshingPropertyBiexponential::
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
double EdgeMeshingPropertyBiexponential::
computeParamExpo(const double size, const double length, const uint nbrEdges) const
{
    double paramExpo = 0.1;
    // we normalize the tolerence by a characteristic length
    const double eps = length*1e-9;
    constexpr int maxIter = 100;
    int iter = 0;
    double F = 1.0;

    while ( fabs(F) > eps && iter < maxIter)
    {
        const double u  = std::exp(paramExpo/nbrEdges) - 1.0;
        const double v  = std::exp(paramExpo) - 1.0;
        F = - size + length*(u/v);
        const double up = (1.0/nbrEdges)*std::exp(paramExpo/nbrEdges);
        const double vp = std::exp(paramExpo);
        const double Fp = length*(up*v-u*vp)/(v*v);

        if (Fp != 0.0)
        {
            paramExpo = paramExpo - F/Fp;
        }

        iter++;
    }

    return paramExpo;
}
/*----------------------------------------------------------------------------*/
double
EdgeMeshingPropertyBiexponential::computeIntermediateParameter(const double paramExpo, const double refParameter) const
{
    return (std::exp(refParameter*paramExpo)-1.0)/(std::exp(paramExpo)-1.0);
}
/*----------------------------------------------------------------------------*/
std::vector<double>
EdgeMeshingPropertyBiexponential::computeCoeffs(const double paramExpo, const double length, const uint nbrEdges)
{
    std::vector<double> coeffs;
    const double step = 1.0/nbrEdges;
    for (uint i=0; i<nbrEdges+1; i++)
    {
        coeffs.push_back(computeIntermediateParameter(paramExpo, i*step));
    }
    return coeffs;
}
/*----------------------------------------------------------------------------*/
void
EdgeMeshingPropertyBiexponential::findNbrEdgesBalance(uint &n1_min, uint &n2_min)
{
    uint n1 = static_cast<uint>(m_nb_edges/2);
    uint n2 = m_nb_edges-n1;
    n1_min = n1;
    n2_min = n2;

    double paramExpo1 = computeParamExpo(m_sp1, m_length/2.0, n1);
    double paramExpo2 = computeParamExpo(m_sp2, m_length/2.0, n2);

    std::vector<double> coeffs1 = computeCoeffs(paramExpo1, m_length/2.0, n1);
    std::vector<double> coeffs2 = computeCoeffs(paramExpo2, m_length/2.0, n2);

    constexpr double eps(1e-6);
    double minDif(fabs(coeffs1[n1-1]-coeffs2[n2-1]));
    bool minReached(false);

    while ( fabs(coeffs1[n1-1]-coeffs2[n2-1]) > eps
            && !minReached
            && n1 > 1
            && n2 > 1)
    {
        if (coeffs1[n1-1] < coeffs2[n2-1])
        {
            n1 += 1;
            n2 -= 1;
        }
        else
        {
            n1 -= 1;
            n2 += 1;
        }
        paramExpo1 = computeParamExpo(m_sp1, m_length/2.0, n1);
        paramExpo2 = computeParamExpo(m_sp2, m_length/2.0, n2);

        coeffs1 = computeCoeffs(paramExpo1, m_length/2.0, n1);
        coeffs2 = computeCoeffs(paramExpo2, m_length/2.0, n2);


        if (fabs(coeffs1[n1-1]-coeffs2[n2-1]) > minDif)
        {
            minReached = true;
        }
        else
        {
            n1_min = n1;
            n2_min = n2;
            minDif = fabs(coeffs1[n1-1]-coeffs2[n2-1]);
        }
    }

}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/