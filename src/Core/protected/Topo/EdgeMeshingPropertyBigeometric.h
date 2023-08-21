/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyBigeometric.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 26/8/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef EDGEPROPERTYMESHINGBIGEOMETRIC_H_
#define EDGEPROPERTYMESHINGBIGEOMETRIC_H_
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
class EdgeMeshingPropertyBigeometric : public CoEdgeMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    /** Constructeur avec un nombre de bras spécifié,
     *  les raisons des progressions
     *  et les longueurs des premiers bras
     *
     *  La longueur à l'une des extrémités peut être nulle, on termine alors par une discrétisation uniforme
     */
    EdgeMeshingPropertyBigeometric(int nb, double r1, double sp1, double r2, double sp2, bool isDirect=true);
    EdgeMeshingPropertyBigeometric(int nb, double r1, double sp1, double r2, double sp2, Utils::Math::Point polar_center, bool isDirect=true);
    /// Pour les pythoneries, cast par opérateur = :
    EdgeMeshingPropertyBigeometric (const CoEdgeMeshingProperty&);

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return "Bigeometrique";}

    /// change le nombre de bras demandé pour un côté, cela nécessite de refaire l'initialisation
    virtual void setNbEdges(const int nb);

#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    virtual CoEdgeMeshingProperty* clone() {return new  EdgeMeshingPropertyBigeometric(*this);}

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
    EdgeMeshingPropertyBigeometric(const EdgeMeshingPropertyBigeometric&);

    /// Initialisation du tableau de coeff en fonction de la longueur de l'arête (projetée ou non)
    void init(double length);

    /** Recherche du dernier indice du vecteur formé par les couples de points (i,lg_bras1[i]) (i+1,lg_bras1[i+1])
     *  avant que tous les points du second vecteur soient en dessous de la droite passant par ce vecteur
     *  Les points du second vecteur sont retournés, c'est à dire que l'on inverse le sens de parcours
     *  ainsi que l'ordonnée
     *  on prend donc les points en (m_nb_edges-i, length-lg_bras2[i])
     */
    uint getLastIndex(std::vector<double>& lg_bras1, std::vector<double>& lg_bras2, double& length);

    /// raison géométrique à la première extrémité
    double m_r1;
    /// longueur du premier bras
    double m_sp1;
    /// raison géométrique à la deuxième extrémité
    double m_r2;
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

#endif /* EDGEPROPERTYMESHINGBIGEOMETRIC_H_ */
