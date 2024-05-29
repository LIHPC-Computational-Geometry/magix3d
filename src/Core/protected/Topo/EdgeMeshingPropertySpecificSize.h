/*----------------------------------------------------------------------------*/
/*
 * EdgeMeshingPropertySpecificSize.h
 *
 *  Created on: 15 janv. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef EDGEMESHINGPROPERTYSPECIFICSIZE_H_
#define EDGEMESHINGPROPERTYSPECIFICSIZE_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CoEdgeMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'une arête avec une discrétisation
   où est spécifiée la taille cible des segments qui discrétiseront l'arête
   topologique
 */
class EdgeMeshingPropertySpecificSize : public CoEdgeMeshingProperty
{
public:

    /// Constructeur avec une taille cible
    EdgeMeshingPropertySpecificSize(double size);

    /// Pour les pythoneries, cast par opérateur = :
    EdgeMeshingPropertySpecificSize (const CoEdgeMeshingProperty&);

    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoEdgeMeshingProperty&) const;

    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return "Taille Specifique";}

#ifndef SWIG
    /// retourne le coefficient suivant pour les noeuds internes
    virtual double nextCoeff();

    /// Retourne vrai s'il est nécessaire de faire une initialisation à l'aide de la géométrie
    virtual bool needGeomUpdate() const {return true;}

    /// Initialisation à l'aide de la géométrie
    virtual void update(Geom::GeomEntity* ge);

    /** Création d'un clone, on copie toutes les informations */
    virtual CoEdgeMeshingProperty* clone() {return new  EdgeMeshingPropertySpecificSize(*this);}

    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const;

    /*------------------------------------------------------------------------*/
    /** Ajoute les informations spécifiques de cette discrétisation à
     *  la représentation textuelle
     */
    virtual void addDescription(Utils::SerializedRepresentation& topoProprietes) const;

#endif

	/**
	 * \return	La taille des bras.
	 */
	virtual double getEdgeSize ( ) const { return m_edge_size; }

protected:

    double m_edge_size;
};

/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* EDGEMESHINGPROPERTYSPECIFICSIZE_H_ */
