/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyGeometric.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24 janv. 2012
 */
/*----------------------------------------------------------------------------*/
#ifndef EDGEPROPERTYMESHINGGEOMETRIC_H_
#define EDGEPROPERTYMESHINGGEOMETRIC_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CoEdgeMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'une arête avec une discrétisation
   ayant une progression géométrique
 */
class EdgeMeshingPropertyGeometric : public CoEdgeMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    /** Constructeur avec un nombre de bras spécifié,
     *  la raison de la progression
     *  et la longueur du premier bras
     *
     *  Si la longueur du premier bras est non nulle, alors le ratio est calculé
     *  à l'aide de la longueur de l'arête (projetée s'il y a lieu)
     */
    EdgeMeshingPropertyGeometric(int nb, double raison, bool isDirect=true, double meshingEdgeLength=0.0);

    /// idem avec un centre pour découpage polaire
    EdgeMeshingPropertyGeometric(int nb, double raison,
    		Utils::Math::Point polar_center, bool isDirect=true, double meshingEdgeLength=0.0);
    /// Pour les pythoneries, cast par opérateur = :
    EdgeMeshingPropertyGeometric (const CoEdgeMeshingProperty&);	
    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoEdgeMeshingProperty&) const;

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return "Geometrique";}

    /// change le nombre de bras demandé pour un côté, cela nécessite de refaire l'initialisation
    virtual void setNbEdges(const int nb);

    /// le ratio entre deux bras successifs
    double getRatio() const {return m_raison;}

    void setRatio(const double & ratio) {m_raison = ratio;}

#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    virtual CoEdgeMeshingProperty* clone() {return new  EdgeMeshingPropertyGeometric(*this);}

    /// initialisation avant appel à nextCoeff
    virtual void initCoeff();

    /// initialisation avant appel à nextCoeff pour le cas où la longueur est utile
    virtual void initCoeff(double length);

    /// retourne le coefficient suivant pour les noeuds internes
    virtual double nextCoeff();

    /// retourne vrai pour les méthodes qui nécessitent d'avoir la longueur du contour pour l'initialisation
    virtual bool needLengthToInitialize() {return m_initWithArm1;}

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
    /// initialisation interne (calcul de la somme des coeff)
    void initSomme();

    /// calcul la raison suivant le ratio longueur total / longueur premier bras
    double computeRaison(const double lg);

    /// calcul du polynome suivant raison
    double computePolynome(const double raison);

    double m_raison;
    double m_arm1;
    double m_somme;
    double m_dernierSommeCoeff;
    double m_dernierCoeff;

    /// vrai si c'est la longueur du premier bras qui permet de déterminer la raison
    bool m_initWithArm1;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* EDGEPROPERTYMESHINGGEOMETRIC_H_ */
