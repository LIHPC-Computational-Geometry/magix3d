/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyUniform.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24 janv. 2012
 */
/*----------------------------------------------------------------------------*/
#ifndef EDGEPROPERTYMESHINGUNIFORM_H_
#define EDGEPROPERTYMESHINGUNIFORM_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CoEdgeMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'une arête avec une discrétisation uniforme
 */
class EdgeMeshingPropertyUniform  : public CoEdgeMeshingProperty{
public:

    /// Constructeur avec un nombre de bras spécifié
    EdgeMeshingPropertyUniform(int nb);

    /// Constructeur avec un nombre de bras spécifié et un centre de découpage polaire
    EdgeMeshingPropertyUniform(int nb, Utils::Math::Point polar_center);

    /// Pour les pythoneries, cast par opérateur = :
    EdgeMeshingPropertyUniform (const CoEdgeMeshingProperty&);

    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoEdgeMeshingProperty&) const;

    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return "Uniforme";}

#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    virtual CoEdgeMeshingProperty* clone() {return new EdgeMeshingPropertyUniform(*this);}

    /// retourne le coefficient suivant pour les noeuds internes
    virtual double nextCoeff();

    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const;

    /*------------------------------------------------------------------------*/
    /** Ajoute les informations spécifiques de cette discrétisation à
     *  la représentation textuelle
     */
    virtual void addDescription(Utils::SerializedRepresentation& topoProprietes) const;
#endif
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* EDGEPROPERTYMESHINGUNIFORM_H_ */
