/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyGlobalInterpolate.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 14/11/2016
 */
/*----------------------------------------------------------------------------*/
#ifndef EDGEPROPERTYMESHINGGLOBALINTERPOLATE_H_
#define EDGEPROPERTYMESHINGGLOBALINTERPOLATE_H_
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
   se basant sur l'interpolation des sommets de 2 séries d'arêtes
 */
class EdgeMeshingPropertyGlobalInterpolate : public CoEdgeMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    /// Constructeur avec un nombre de bras spécifié et les noms des 2 séries d'arêtes sur lesquelles on interpole
    EdgeMeshingPropertyGlobalInterpolate(int nb,
    		const std::vector<std::string>& firstCoedgesName,
			const std::vector<std::string>& secondCoedgesName);
    /// Pour les pythoneries, cast par opérateur = :
    EdgeMeshingPropertyGlobalInterpolate (const CoEdgeMeshingProperty&);	
    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoEdgeMeshingProperty&) const;

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return "Interpolée Globale";}

#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    virtual CoEdgeMeshingProperty* clone() {return new  EdgeMeshingPropertyGlobalInterpolate(*this);}

    /// initialisation avant appel à nextCoeff
    virtual void initCoeff();

    /// retourne le coefficient suivant pour les noeuds internes
    virtual double nextCoeff();

    /// calcul la position des points en fonction des points des 2 séries d'arêtes de référence
    void getPoints(std::vector<Utils::Math::Point> &points,
    		std::vector<Topo::CoEdge*> first_coedges_ref,
			std::vector<Topo::CoEdge*> second_coedges_ref);

    /*------------------------------------------------------------------------*/
    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const;

    /*------------------------------------------------------------------------*/
    /** Ajoute les informations spécifiques de cette discrétisation à
     *  la représentation textuelle
     */
    virtual void addDescription(Utils::SerializedRepresentation& topoProprietes) const;
#endif

    /// retourne les noms des arêtes de la 1ère série
    std::vector<std::string> getFirstCoEdges() const {return m_firstCoedgesName;}

    /// retourne les noms des arêtes de la 2ème série
    std::vector<std::string> getSecondCoEdges() const {return m_secondCoedgesName;}

    /// change les noms des arêtes de la 1ère série
    void setFirstCoEdges(std::vector<std::string>& names);

    /// change les noms des arêtes de la 2ème série
    void setSecondCoEdges(std::vector<std::string>& names);



private:
    /// calcul une interpolation des points à partir de 2  ensembles de points et de 2 points extrémité
    void getInterpolate(std::vector<Utils::Math::Point> &points_ref,
    		std::vector<Utils::Math::Point> &loc_first_points,
			std::vector<Utils::Math::Point> &loc_second_points);

private:

    /// nom de la 1ère série d'arêtes sur lesquelles on se base
    std::vector<std::string> m_firstCoedgesName;

    /// nom de la 2ème série d'arêtes sur lesquelles on se base
    std::vector<std::string> m_secondCoedgesName;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* EDGEPROPERTYMESHINGGLOBALINTERPOLATE_H_ */
