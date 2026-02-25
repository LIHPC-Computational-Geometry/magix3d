//
// Created by rochec on 24/02/2026.
//

#ifndef MAGIX3D_EDGEMESHINGPROPERTYDOUBLEEXPONENTIAL_H
#define MAGIX3D_EDGEMESHINGPROPERTYDOUBLEEXPONENTIAL_H
/*----------------------------------------------------------------------------*/
#include "Topo/CoEdgeMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'une arête avec une discrétisation
   ayant deux progressions géométriques et deux longueurs de bras (une à chaque extrémitées)

 */
class EdgeMeshingPropertyDoubleExponential : public CoEdgeMeshingProperty
{
public:

    /*------------------------------------------------------------------------*/
    /** Constructeur avec un nombre de bras spécifié,
     *  les raisons des progressions
     *  et les longueurs des premiers bras
     *
     *  La longueur à l'une des extrémités peut être nulle, on termine alors par une discrétisation uniforme
     */
    EdgeMeshingPropertyDoubleExponential(int nb, double r1, double sp1, double r2, double sp2, bool isDirect=true);
    /// Pour les pythoneries, cast par opérateur = :
    EdgeMeshingPropertyDoubleExponential (const CoEdgeMeshingProperty&);

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return "Double Exponential";}

#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    CoEdgeMeshingProperty* clone() override {return new  EdgeMeshingPropertyDoubleExponential(*this);}

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

    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoEdgeMeshingProperty&) const;
#endif

    /*------------------------------------------------------------------------*/
    /// Retournent raisons et longueurs caractérisant la discrétisation.
    virtual double getRatio1 ( ) const { return m_r1; }
    virtual double getLength1 ( ) const { return m_sp1; }
    virtual double getRatio2 ( ) const { return m_r2; }
    virtual double getLength2 ( ) const { return m_sp2; }



private:

    /// Constructeur de copie
    EdgeMeshingPropertyDoubleExponential(const EdgeMeshingPropertyDoubleExponential&);

    /// compute exponential parameter corresponding to first mesh edge size
    double computeParamExpo() const;

    ///
    double computeIntermediateParameter(double refParameter) const;

    /// raison géométrique à la première extrémité
    double m_r1;
    /// longueur du premier bras
    double m_sp1;
    /// raison géométrique à la deuxième extrémité
    double m_r2;
    /// longueur du dernier bras
    double m_sp2;
    double m_A4;

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

#endif //MAGIX3D_EDGEMESHINGPROPERTYDOUBLEEXPONENTIAL_H