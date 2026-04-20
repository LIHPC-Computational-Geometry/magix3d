/*----------------------------------------------------------------------------*/
#include "Topo/EdgeMeshingPropertyBeta.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyBeta::
EdgeMeshingPropertyBeta(const int nbBras, const double beta, const bool isDirect,
                        const bool initWithFirstEdge, const double meshingEdgeLength)
    : CoEdgeMeshingProperty(nbBras, beta_resserrement, isDirect)
      , m_beta(beta)
      , m_arm1(meshingEdgeLength)
      , m_initWithArm1(initWithFirstEdge)
{
    if (!m_initWithArm1 && (m_beta < 1.00001 || m_beta > 1.01))
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBeta: ";
        messErr << "la valeur de beta doit être comprise entre 1 et 1.01, actuellement : ";
        messErr << m_beta;
        throw TkUtil::Exception(messErr);
    }
    if (m_initWithArm1 && m_arm1 < 0)
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBeta: ";
        messErr << "la longueur du premier bras ne doit pas être négative : ";
        messErr << m_arm1;
        throw TkUtil::Exception(messErr);
    }
    if (nbBras < 1)
    {
        throw TkUtil::Exception(TkUtil::UTF8String(
            "EdgeMeshingPropertyBeta, le nombre de bras doit être au moins de 1", TkUtil::Charset::UTF_8));
    }
}

/*----------------------------------------------------------------------------*/
EdgeMeshingPropertyBeta::EdgeMeshingPropertyBeta(const CoEdgeMeshingProperty &prop)
    : CoEdgeMeshingProperty(prop.getNbEdges(), beta_resserrement, prop.getDirect())
      , m_beta(0.)
      , m_arm1(0.)
      , m_initWithArm1(false)
{
    const EdgeMeshingPropertyBeta *p = reinterpret_cast<const EdgeMeshingPropertyBeta *>(&prop);
    if (nullptr == p)
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBeta, la propriété transmise en argument n'est pas de type EdgeMeshingPropertyBeta";
        throw TkUtil::Exception(messErr);
    }

    m_beta = p->m_beta;
    m_arm1 = p->m_arm1;
    m_initWithArm1 = p->m_initWithArm1;
}

/*----------------------------------------------------------------------------*/
bool EdgeMeshingPropertyBeta::operator ==(const CoEdgeMeshingProperty &cedp) const
{
    const EdgeMeshingPropertyBeta *props =
            dynamic_cast<const EdgeMeshingPropertyBeta *>(&cedp);
    if (nullptr == props)
    {
        return false;
    }

    if (m_beta != props->m_beta)
    {
        return false;
    }

    if (m_arm1 != props->m_arm1)
    {
        return false;
    }

    if (m_initWithArm1 != props->m_initWithArm1)
    {
        return false;
    }

    return CoEdgeMeshingProperty::operator ==(cedp);
}

/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyBeta::setNbEdges(const int nb)
{
    CoEdgeMeshingProperty::setNbEdges(nb);
}

/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyBeta::initCoeff(double length)
{
    if (m_initWithArm1)
    {
        // on calcule le coefficient de resserement
        m_beta = computeBeta(m_arm1);
    }
    initCoeff();
}

/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyBeta::initCoeff()
{
    if (m_beta == 0.0)
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr << "Erreur interne: EdgeMeshingPropertyBeta, le coefficient de resserrement n'a pas été initialisé";
        throw TkUtil::Exception(messErr);
    }

    m_dernierIndice = 0;
}

/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyBeta::
nextCoeff()
{
    m_dernierIndice += 1;

    double feta = 0.0;
    double eta = 0.0;
    if (m_sens)
    {
        eta = static_cast<double>(m_dernierIndice) / static_cast<double>(m_nb_edges);
        feta = resserre(eta, m_beta);
    }
    else
    {
        eta = (static_cast<double>(m_nb_edges) - m_dernierIndice) / static_cast<double>(m_nb_edges);
        feta = 1.0 - resserre(eta, m_beta);
    }

    return feta;
}

/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyBeta::resserre(const double eta, const double beta)
{
    double p = std::log((beta + 1.0) / (beta - 1.0)) * (1.0 - eta);
    return 1.0 + beta * (1.0 - std::exp(p)) / (1.0 + std::exp(p));
}

/*----------------------------------------------------------------------------*/
TkUtil::UTF8String EdgeMeshingPropertyBeta::
getScriptCommand() const
{
    TkUtil::UTF8String o(TkUtil::Charset::UTF_8);
    o << getMgx3DAlias() << ".EdgeMeshingPropertyBeta( ";
    o << static_cast<long>(m_nb_edges) << ", ";
    o << Utils::Math::MgxNumeric::userRepresentation(m_beta);
    o << (getDirect() ? std::string(", True") : std::string(", False"));
    if (m_initWithArm1)
    {
        o << ", True"; // this is for m_initWithArm1
        o << ", " << Utils::Math::MgxNumeric::userRepresentation(m_arm1);
    }
    o << ")";
    return o;
}

/*----------------------------------------------------------------------------*/
void EdgeMeshingPropertyBeta::
addDescription(Utils::SerializedRepresentation &topoProprietes) const
{
    topoProprietes.addProperty(
        Utils::SerializedRepresentation::Property("Beta Resserrement", m_beta));

    if (getDirect())
    {
        topoProprietes.addProperty(
            Utils::SerializedRepresentation::Property("Sens", std::string("direct")));
    }
    else
    {
        topoProprietes.addProperty(
            Utils::SerializedRepresentation::Property("Sens", std::string("inverse")));
    }
}

/*----------------------------------------------------------------------------*/
double EdgeMeshingPropertyBeta::
computeBeta(const double lg) const
{
    // recherche de beta pour laquelle feta=m_arm1
    double beta = 0.0;
    const double eta = (1.0) / static_cast<double>(m_nb_edges);

    // on limite la recherche
    double beta1 = 1.00001;
    double beta2 = 1.01;
    const double lg1 = resserre(eta, beta1);
    const double lg2 = resserre(eta, beta2);

    if ((lg1 < lg && lg2 < lg) || (lg1 > lg && lg2 > lg))
    {
        TkUtil::UTF8String messErr(TkUtil::Charset::UTF_8);
        messErr << "EdgeMeshingPropertyBeta, on ne peut pas trouver de beta pour lg ";
        messErr << lg << " et nombre de bras de " << static_cast<short>(m_nb_edges);
        throw TkUtil::Exception(messErr);
    }
    uint iter = 0;
    const bool sens = (lg1 < lg2);
    while (!Utils::Math::MgxNumeric::isNearlyZero(beta2 - beta1) && iter < 100)
    {
        iter++;
        beta = (beta1 + beta2) / 2.0;
        const double lg_iter = resserre(eta, beta);

        if (sens)
        {
            if (lg_iter > lg)
            {
                beta2 = beta;
            }
            else
            {
                beta1 = beta;
            }
        }
        else
        {
            if (lg_iter < lg)
            {
                beta2 = beta;
            }
            else
            {
                beta1 = beta;
            }
        }
    } // end while

    return beta;
}

/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
