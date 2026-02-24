//
// Created by rochec on 23/02/2026.
//
#include "Topo/EdgeMeshingPropertyExponential.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyExponential::
EdgeMeshingPropertyExponential(int nbBras,
                               double paramExpo,
                               bool isDirect,
                               bool initWithFirstEdge,
                               double meshingEdgeLength)
    : CoEdgeMeshingProperty(nbBras, exponential, isDirect),
      m_paramExpo(paramExpo),
      m_arm1(meshingEdgeLength),
      m_initWithArm1(initWithFirstEdge),
      m_length(0.0)
{
    if (nbBras < 1)
    {
        throw TkUtil::Exception (TkUtil::UTF8String ("EdgeMeshingPropertyExponential: must have at least one meshing edge.", TkUtil::Charset::UTF_8));
    }
    if (m_paramExpo == 0.0)
    {
        throw TkUtil::Exception(TkUtil::UTF8String("EdgeMeshingPropertyExponential: exponential parameter must be different than 0.", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyExponential::EdgeMeshingPropertyExponential(const CoEdgeMeshingProperty &prop)
    : CoEdgeMeshingProperty(prop.getNbEdges(), exponential, prop.getDirect()),
      m_paramExpo(0.),
      m_arm1(0.),
      m_initWithArm1(false)
{
    const auto *p = reinterpret_cast<const EdgeMeshingPropertyExponential *>(&prop);
    if (!p)
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr <<
                "EdgeMeshingPropertyBeta, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyExponential";
        throw TkUtil::Exception(messErr);
    }

    m_paramExpo = p->m_paramExpo;
    m_arm1 = p->m_arm1;
    m_initWithArm1 = p->m_initWithArm1;
    m_length = p->m_length;
}
/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyExponential::operator == (const CoEdgeMeshingProperty& cedp) const
{
    const auto *props = dynamic_cast<const EdgeMeshingPropertyExponential *>(&cedp);
    if (!props)
        return false;

    if (m_paramExpo != props->m_paramExpo)
        return false;

    if (m_arm1 != props->m_arm1)
        return false;

    if (m_initWithArm1 != props->m_initWithArm1)
        return false;

    return CoEdgeMeshingProperty::operator == (cedp);
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyExponential::initCoeff(const double length)
{
    std::cout << __FILE__ << ", length: " << length << std::endl;
    if (m_initWithArm1)
    {
        std::cout << "compute Param Expo..." << std::endl;
        m_paramExpo = computeParamExpo();
    }
    m_length = length;
    initCoeff();
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyExponential::initCoeff()
{
    m_dernierIndice = 0;
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyExponential::
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
TkUtil::UTF8String EdgeMeshingPropertyExponential::
getScriptCommand() const
{
    TkUtil::UTF8String o(TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertyExponential("
            << static_cast<long>(m_nb_edges) << ","
            << Utils::Math::MgxNumeric::userRepresentation(m_paramExpo);
    if (m_initWithArm1)
    {
        o << (getDirect() ? std::string(", True") : std::string(", False"));
        o << ", True"; // this is for m_initWithArm1
        o << ", " << Utils::Math::MgxNumeric::userRepresentation(m_arm1);
    }
    else if (!getDirect())
    {
        o << ", False";
    }
    o << ")";
    return o;
}
/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyExponential::
addDescription(Utils::SerializedRepresentation& topoProprietes) const
{
    topoProprietes.addProperty(
        Utils::SerializedRepresentation::Property("Exponential", m_paramExpo));

    if (getDirect())
        topoProprietes.addProperty(
            Utils::SerializedRepresentation::Property("Sens", std::string("direct")));
    else
        topoProprietes.addProperty(
            Utils::SerializedRepresentation::Property("Sens", std::string("inverse")));
}
/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyExponential::
computeParamExpo() const
{
    double paramExpo = 0.1;
    // we normalize the tolerence by a characteristic length
    const double eps = m_length*1e-9;
    constexpr int maxIter = 100;
    int iter = 0;
    double F = 1.0;

    std::cout << "length: " << m_length << ", eps: " << eps << std::endl;
    std::cout << "arm 1: " << m_arm1 << std::endl;

    while ( fabs(F) > eps && iter < maxIter)
    {
        const double u  = std::exp(paramExpo/m_nb_edges) - 1.0;
        const double v  = std::exp(paramExpo) - 1.0 ;
        F = - m_arm1 + m_length*(u/v);
        const double up = (1.0/m_nb_edges)*std::exp(paramExpo/m_nb_edges) ;
        const double vp = std::exp(paramExpo) ;
        const double Fp = m_length*(up*v-u*vp)/(v*v) ;
        std::cout << "iter: " << iter << ", Fp: " << Fp << std::endl;

        if (Fp != 0.0)
        {
            paramExpo = paramExpo - F/Fp;
        }

        iter++;
    }
    std::cout << "nbr iter: " << iter << std::endl;
    std::cout << "F: " << F << std::endl;
    std::cout << "paramExpo: " << paramExpo << std::endl;

    return paramExpo;
}
/*----------------------------------------------------------------------------*/
double
EdgeMeshingPropertyExponential::computeIntermediateParameter(const double refParameter) const
{
    return (std::exp(refParameter*m_paramExpo)-1.0)/(std::exp(m_paramExpo)-1.0);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/