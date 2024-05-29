/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyHyperbolic.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 29/8/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef EDGEPROPERTYMESHINGHYPERBOLIC_H_
#define EDGEPROPERTYMESHINGHYPERBOLIC_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CoEdgeMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation hyperbolique d'une arête dont on
   renseigne la longueur des bras aux extrémitées
 */
class EdgeMeshingPropertyHyperbolic : public CoEdgeMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    /** Constructeur avec un nombre de bras spécifié,
     *  et les longueurs des premiers bras
     */
    EdgeMeshingPropertyHyperbolic(int nb, double sp1, double sp2, bool isDirect=true);
    /// Pour les pythoneries, cast par opérateur = :
    EdgeMeshingPropertyHyperbolic (const CoEdgeMeshingProperty&);

    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoEdgeMeshingProperty&) const;

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return "Hyperbolique";}

    /// change le nombre de bras demandé pour un côté, cela nécessite de refaire l'initialisation
    virtual void setNbEdges(const int nb);

#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    virtual CoEdgeMeshingProperty* clone() {return new  EdgeMeshingPropertyHyperbolic(*this);}

    /// initialisation avant appel à nextCoeff
    virtual void initCoeff();

    /// initialisation avant appel à nextCoeff pour le cas où la longueur est utile
    virtual void initCoeff(double length);

    /// retourne le coefficient suivant pour les noeuds internes
    virtual double nextCoeff();

    /// retourne vrai pour les méthodes qui nécessitent d'avoir la longueur du contour pour l'initialisation
    virtual bool needLengthToInitialize() {return true;}

    /*------------------------------------------------------------------------*/
    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const;

    /*------------------------------------------------------------------------*/
    /** Ajoute les informations spécifiques de cette discrétisation à
     *  la représentation textuelle
     */
    virtual void addDescription(Utils::SerializedRepresentation& topoProprietes) const;


#endif

	/*------------------------------------------------------------------------*/
	/// Retournent les longueurs des premiers et derniers bras.
	virtual double getLength1 ( ) const { return m_sp1; }
	virtual double getLength2 ( ) const { return m_sp2; }


private:

    /// Initialisation du tableau de coeff en fonctio de la longueur de l'arête (projetée ou non)
    void init(double length);

    /// Résolution du système sinh(x)/x = B
    double solveSinhXSurX(double B);

    /// longueur du premier bras
    double m_sp1;
    /// longueur du dernier bras
    double m_sp2;

    /// liste des coefficients
    std::vector<double> m_coeff;

    /// longueur de l'arête pour laquelle ont été calculés les coefficients
    double m_length;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* EDGEPROPERTYMESHINGHYPERBOLIC_H_ */
