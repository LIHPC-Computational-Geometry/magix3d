/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyTabulated.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/2/2015
 */
/*----------------------------------------------------------------------------*/
#ifndef EDGEPROPERTYMESHINGTABULATED_H_
#define EDGEPROPERTYMESHINGTABULATED_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CoEdgeMeshingProperty.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class CoEdge;

/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'une arête avec une discrétisation
   se basant sur une tabulation
 */
class EdgeMeshingPropertyTabulated : public CoEdgeMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    /// Constructeur avec une liste de longueurs d'arêtes
    EdgeMeshingPropertyTabulated(std::vector<double>& tabulation);

    /// Pour les pythoneries, cast par opérateur = :
    EdgeMeshingPropertyTabulated (const CoEdgeMeshingProperty&);

    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoEdgeMeshingProperty&) const;

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return "Tabulée";}

#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    virtual CoEdgeMeshingProperty* clone() {return new  EdgeMeshingPropertyTabulated(*this);}

    /// initialisation avant appel à nextCoeff
    virtual void initCoeff();

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

    /// Retourne les tabulations
    virtual std::vector<double> getTabulations ( ) const { return m_tabulation; }


private:
    /// Tabulation qui sert à déterminer les coeff
    std::vector<double> m_tabulation;

    double m_dernierCoeff;
    double m_somme;
    /// dernier indice utilisé par nextCoeff
    int m_dernierIndice;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* EDGEPROPERTYMESHINGTABULATED_H_ */

