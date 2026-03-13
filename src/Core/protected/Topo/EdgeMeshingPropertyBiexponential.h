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
 *
 *  0                              l/2                              l
 *  o-------------------------------x-------------------------------o
 *
 * Considering the topological edge above, we split it at l/2, where l
 * is the projected topological edge length. We distribute equally the
 * number of mesh edges on both side, and compute on each side, an
 * exponential law [1] to get the requested target first mesh size, on
 * each side of the edge. Then, we compare the size of the meshing edges
 * on both side of l/2 (where a meshing node is placed). If the size
 * difference is too important, then we re-distribute the number of
 * meshing edges from one side to another, until the difference is as
 * small as possible.
 *
 * [1] THOMPSON, Joe F., SONI, Bharat K., et WEATHERILL, Nigel P. (ed.).
 *     Handbook of grid generation. CRC press, 1998.
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
    /// Get name of meshing method.
    std::string getMeshLawName() const override {return "Bi-Exponential";}

#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    CoEdgeMeshingProperty* clone() override {return new  EdgeMeshingPropertyBiexponential(*this);}

    /// Initialization before call to nextCoeff function.
    void initCoeff(double length) override;

    /// Return the next coefficient.
    double nextCoeff() override;

    /// Return true if the discretization needs the topological edge length information.
    bool needLengthToInitialize() override {return true;}

    /*------------------------------------------------------------------------*/
    /// Python command script.
    TkUtil::UTF8String getScriptCommand() const override;

    /*------------------------------------------------------------------------*/
    /** Add discretization specific information to textual representation.
     */
    void addDescription(Utils::SerializedRepresentation& topoProprietes) const override;

    /*------------------------------------------------------------------------*/
    /// Compare two meshing properties.
    bool operator == (const CoEdgeMeshingProperty&) const override;

#endif

    /*------------------------------------------------------------------------*/
    /// Return target sizes of first and last meshing edges.
    virtual double getLength1() const { return m_sp1; }
    virtual double getLength2() const { return m_sp2; }

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
     * @param[in] nbrEdges is the number of meshing edges
     *
     * @return The exponential distribution of points over [0,1]
     */
    static std::vector<double> computeCoeffs(double paramExpo, uint nbrEdges);

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