//
// Created by rochec on 23/02/2026.
//

#ifndef MAGIX3D_EDGEMESHINGPROPERTYEXPONENTIAL_H
#define MAGIX3D_EDGEMESHINGPROPERTYEXPONENTIAL_H
/*----------------------------------------------------------------------------*/
#include "Topo/CoEdgeMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Discretization of a topological co-edge with exponential function
 */
class EdgeMeshingPropertyExponential : public CoEdgeMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    /** Constructeur
     *  \param paramExpo
     *
     */
    EdgeMeshingPropertyExponential(int nb, double paramExpo, bool isDirect=true, bool initWithFirstEdge=false, double meshingEdgeLength=0.0);

    /// Pour les pythoneries, cast par opérateur = :
    EdgeMeshingPropertyExponential(const CoEdgeMeshingProperty&);

    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    bool operator == (const CoEdgeMeshingProperty&) const override;

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    std::string getMeshLawName() const override {return "Exponential";}

    ///
    double getParamExpo() const {return m_paramExpo;}

    bool initWithFirstEdge() const {return m_initWithArm1;}

    /// longueur de la première arête
    double getFirstEdgeLength() const {return m_arm1;}
    void setFirstEdgeLength(const double & arm1) {m_arm1 = arm1;}

#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    CoEdgeMeshingProperty* clone() override {return new  EdgeMeshingPropertyExponential(*this);}

    /// initialisation avant appel à nextCoeff
    void initCoeff() override;

    /// initialisation avant appel à nextCoeff pour le cas où la longueur est utile
    void initCoeff(double length) override;

    /// retourne le coefficient suivant pour les noeuds internes
    double nextCoeff() override;

    /*------------------------------------------------------------------------*/
    /// Script pour la commande de création Python
    TkUtil::UTF8String getScriptCommand() const override;

    /*------------------------------------------------------------------------*/
    /** Ajoute les informations spécifiques de cette discrétisation à
     *  la représentation textuelle
     */
    void addDescription(Utils::SerializedRepresentation& topoProprietes) const override;

#endif

private:

    /// compute exponential parameter corresponding to first mesh edge size
    double computeParamExpo() const;

    ///
    double computeIntermediateParameter(double refParameter) const;

private:

    /// exponential parameter of the exponential progression law
    double m_paramExpo;
    /// projected curved topological co-edge length
    double m_length;
    /// target size of first meshing edge of the topological co-edge
    double m_arm1;
    /// true if we need to compute the exponential parameter with the target size of first meshing edge
    bool m_initWithArm1;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif //MAGIX3D_EDGEMESHINGPROPERTYEXPONENTIAL_H