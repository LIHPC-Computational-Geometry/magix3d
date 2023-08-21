/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyBeta.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 21/1/2015
 *
 *  d'après ce qui a été fourni par Thierry Hocquellet et nommé "loi beta de resserrement sur paroi"
 */
/*----------------------------------------------------------------------------*/
#ifndef EDGEPROPERTYMESHINGBETA_H_
#define EDGEPROPERTYMESHINGBETA_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CoEdgeMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'une arête avec une discrétisation
   ayant un resserrement sur paroi
 */
class EdgeMeshingPropertyBeta : public CoEdgeMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    /** Constructeur avec un nombre de bras spécifié et la valeur de beta
     *
     *  \param beta coeff de resserrement , beta=1+epsilon  ; epsilon>  0;
     *  en pratique beta peut varier de 1.01 (peu resséré) à 1.00001 (très resséré)
     */
    EdgeMeshingPropertyBeta(int nb, double beta, bool isDirect=true);
    /// Pour les pythoneries, cast par opérateur = :
    EdgeMeshingPropertyBeta (const CoEdgeMeshingProperty&);

    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoEdgeMeshingProperty&) const;

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return "Beta";}

    /// le ratio entre deux bras successifs
    double getBeta() const {return m_beta;}

#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    virtual CoEdgeMeshingProperty* clone() {return new  EdgeMeshingPropertyBeta(*this);}

    /// retourne le coefficient suivant pour les noeuds internes
    virtual double nextCoeff();

    /*------------------------------------------------------------------------*/
    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const;

    /*------------------------------------------------------------------------*/
    /** Ajoute les informations spécifiques de cette discrétisation à
     *  la représentation textuelle
     */
    virtual void addDescription(Utils::SerializedRepresentation& topoProprietes) const;

#endif

private:
    /** f(eta) : est en sortie de resserre, ça peut être vu comme la
     position du point après resserrement (feta=0 -->  paroi,
     feta=1-->  frontière amont)

             transformation de (0,1) sur (0,1) permettant de resserrer
                        le maillage au voisinage du corps
     */
    double resserre(double eta);

    /// valeur de beta pour le resserrement (de la forme 1+epsilon)
    double m_beta;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* EDGEPROPERTYMESHINGBETA_H_ */
