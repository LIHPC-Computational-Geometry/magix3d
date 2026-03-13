#ifndef MAGIX3D_EDGEMESHINGPROPERTYBIEXPONENTIAL_H
#define MAGIX3D_EDGEMESHINGPROPERTYBIEXPONENTIAL_H
/*----------------------------------------------------------------------------*/
#include "Topo/CoEdgeMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
 * @brief Topological edge discretization property with two exponential
 * progression law on the two different sides of the edge, with target
 * sizes of the first and the last meshing edges.
 */
class EdgeMeshingPropertyBiexponential : public CoEdgeMeshingProperty
{
public:

    /*------------------------------------------------------------------------*/
    /** @brief Constructor
     *
     * @param[in] nb is the number of meshing edges
     * @param[in] sp1 is the target size of first meshing edge
     * @param[in] sp2 is the target size of last meshing edge
     */
    EdgeMeshingPropertyBiexponential(int nb, double sp1, double sp2, bool isDirect=true);
    /// Pour les pythoneries, cast par opérateur = :
    EdgeMeshingPropertyBiexponential (const CoEdgeMeshingProperty&);

    /*------------------------------------------------------------------------*/
    /// Get name of meshing method
    std::string getMeshLawName() const override {return "Bi-Exponential";}

#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    CoEdgeMeshingProperty* clone() override {return new  EdgeMeshingPropertyBiexponential(*this);}

    /// initialisation avant appel à nextCoeff pour le cas où la longueur est utile
    void initCoeff(double length) override;

    /// retourne le coefficient suivant pour les noeuds internes
    double nextCoeff() override;

    /// retourne vrai pour les méthodes qui nécessitent d'avoir la longueur du contour pour l'initialisation
    bool needLengthToInitialize() override {return true;}

    /*------------------------------------------------------------------------*/
    /// Script pour la commande de création Python
    TkUtil::UTF8String getScriptCommand() const override;

    /*------------------------------------------------------------------------*/
    /** Ajoute les informations spécifiques de cette discrétisation à
     *  la représentation textuelle
     */
    void addDescription(Utils::SerializedRepresentation& topoProprietes) const override;

    /*------------------------------------------------------------------------*/
    /// Compare two meshing properties.
    bool operator == (const CoEdgeMeshingProperty&) const override;
#endif



private:

    /// Copy constructor
    EdgeMeshingPropertyBiexponential(const EdgeMeshingPropertyBiexponential&);

    /** @brief Compute exponential parameter corresponding to first mesh edge size
     *
     * @param[in] size is the target first meshing edge size
     * @param[in] length is the topological edge lenght
     * @param[in] nbrEdges is the number of meshing edges
     *
     * @return The value of the exponential parameter that respects the given
     * constraints.
     */
    static double computeParamExpo(double size, double length, uint nbrEdges) ;

    /** @brief Compute the intermediate parameter, which is a parameter
     * between [0,1] that corresponds to the distribution of the
     * exponential law, for parameter @paramExpo at param @refParameter.
     *
     * @param[in] paramExpo is the exponential parameter of the law
     * @param[in] refParameter is a parameter in [0,1]
     *
     * @return The intermediate parameter, which is in [0,1].
     */
    static double computeIntermediateParameter(double paramExpo, double refParameter) ;

    /** @brief Algorithm to find the balance of meshing edges distribution over
     * the topological edge. We split the topological edge in two parts of same
     * length. We compute an exponential law for each sub-topological edge to
     * respect the first meshing edge sizes. Then, we compare the obtained
     * meshing sizes at the junction between the two parts. We distribute the
     * number of meshing edges from one side to the other, until the junction is
     * as regular as possible.
     *
     * @param[inout] n1_min number of mesh edges on the first side
     * @param[inout] n2_min number of mesh edges on the second sie
     */
    void findNbrEdgesBalance(uint &n1_min, uint &n2_min) const;

    /** @brief Given the parameter of an exponential law, and the number of
     * mesh edges over the domain [0,1], it computes the exponential
     * distribution of points.
     *
     * @param[in] paramExpo is the exponential parameter
     * @param[in] length is the physical topological edge length
     * @param[in] nbrEdges is the number of meshing edges
     *
     * @return The exponential distribution of points over [0,1]
     */
    static std::vector<double> computeCoeffs(double paramExpo, double length, uint nbrEdges);

private:
    /// Projected/Physical topological edge length
    double m_length;
    /// Target size of first meshing edge
    double m_sp1;
    /// Target size of last meshing edge
    double m_sp2;
    /// Distribution of points in [0,1]
    std::vector<double> m_coeff;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif //MAGIX3D_EDGEMESHINGPROPERTYBIEXPONENTIAL_H