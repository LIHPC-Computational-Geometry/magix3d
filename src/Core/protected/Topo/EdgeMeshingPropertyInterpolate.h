/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingPropertyInterpolate.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 27/5/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef EDGEPROPERTYMESHINGINTERPOLATE_H_
#define EDGEPROPERTYMESHINGINTERPOLATE_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CoEdgeMeshingProperty.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class CoEdge;
class CoFace;

/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'une arête avec une discrétisation
   se basant sur l'interpolation des sommets d'une autre arête
 */
class EdgeMeshingPropertyInterpolate : public CoEdgeMeshingProperty {
public:

	/** Type d'interpolation
	 */
	typedef enum{
		with_coedge_list,   // cas initialement développé en 3D mais limité à une arête vers N arêtes
		with_coface         // équivalent à ce qui est fait avec Magix 2D
	} interpolateType;

	/// retourne le type d'interpolation
	interpolateType getType() const {return m_type;}

    /*------------------------------------------------------------------------*/
    /// Constructeur avec un nombre de bras spécifié et les noms des arêtes sur lesquelles on interpole
    EdgeMeshingPropertyInterpolate(int nb, const std::vector<std::string>& coedgesName);

    /*------------------------------------------------------------------------*/
    /// Constructeur avec un nombre de bras spécifié et le nom d'une face commune
    EdgeMeshingPropertyInterpolate(int nb, const std::string& cofaceName);

    /// Pour les pythoneries, cast par opérateur = :
    EdgeMeshingPropertyInterpolate (const CoEdgeMeshingProperty&);

    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoEdgeMeshingProperty&) const;

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const {return "Interpolée";}

#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    virtual CoEdgeMeshingProperty* clone() {return new  EdgeMeshingPropertyInterpolate(*this);}

    /// initialisation avant appel à nextCoeff
    virtual void initCoeff();

    /// retourne le coefficient suivant pour les noeuds internes
    virtual double nextCoeff();

    /// calcul la position des points en fonction des points des arêtes de référence
    void getPoints(std::vector<Utils::Math::Point> &points, std::vector<Topo::CoEdge*> coedges_ref);

    /// calcul la position des points en fonction des points des arêtes en face de l'arête via la coface
     void getPoints(std::vector<Utils::Math::Point> &points, const Topo::CoEdge* coedge, Topo::CoFace* coface);

    /// calcul les distances entre les points
    void getTabulations(std::vector<double> &tabulations,
    		std::vector<Topo::CoEdge*>& coedges_ref,
    		Utils::Math::Point& pt0,
    		Utils::Math::Point& pt1);

    /// calcul les distances entre les points
    void getTabulations(std::vector<double> &tabulations,
    		const Topo::CoEdge* coedge,
			Topo::CoFace* coface);

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
    /// retourne les noms des arêtes
    std::vector<std::string> getCoEdges() const {return m_coedgesName;}

    /// change les noms des arêtes
    void setCoEdges(std::vector<std::string>& names);

    /// retourne le nom de la coface
    std::string getCoFace() const {return m_cofaceName;}

    /// change le nom de la coface
    void setCoFace(std::string& name);


private:
    /*------------------------------------------------------------------------*/
    /// recherche la position des points des arêtes de référence en partant d'une coedge et d'une coface
    virtual void getPointsRef(std::vector<Utils::Math::Point> &points_ref,
    		const Topo::CoEdge* coedge,
			Topo::CoFace* coface);

    /*------------------------------------------------------------------------*/
    /// calcul une interpolation des points à partir d'un ensemble de points et de 2 points extrémité
    void getInterpolate(std::vector<Utils::Math::Point> &points_ref, std::vector<Utils::Math::Point> &loc_points);


private:

    /// type d'interpolation
    interpolateType m_type;

    /// nom des arêtes sur lesquelles on se base
    std::vector<std::string> m_coedgesName;

    /// nom de la face commune pour retrouver les arêtes en face
    std::string m_cofaceName;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* EDGEPROPERTYMESHINGINTERPOLATE_H_ */
