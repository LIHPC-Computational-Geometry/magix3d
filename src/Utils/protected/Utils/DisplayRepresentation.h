/*----------------------------------------------------------------------------*/
/** \file DisplayRepresentation.h
 *
 *  \author Franck Ledoux
 *
 *  \date 30/11/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef DISPLAY_REPRESENTATION_H_
#define DISPLAY_REPRESENTATION_H_
/*----------------------------------------------------------------------------*/
#include <string>
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
/** \class DisplayRepresentation
 *  \brief La classe abstraite DisplayRepresentation décrit une représentation
 *         de ce quoi doit être affichée pour représenter graphiquement une
 *         entité.
 *
 *         La représentation est donnée par un tableau de points sur lequel
 *         sont construit les éléments (arêtes ou triangles). Les éléments sont
 *         décrit par un tableau d'indice sur les points. Pour la représentation
 *         filaire les indices i et i+1 avec i pair définissent une arête. Pour
 *         la représentation solide, les indices i, i+1, i+2 avec i%3=0
 *         représentent un triangle.
 */
/*----------------------------------------------------------------------------*/
class DisplayRepresentation{
public:

	/**
	 * Constructeur par défaut, de copie et opérateur = : pour la sérialisation
	 * (mode client/serveur).
	 * \warning		<B>Ne pas utiliser directement ces constructeurs, utiliser
	 *			ceux des classes dérivées prévus à cet effet.</B>
	 */
	DisplayRepresentation ( );
	DisplayRepresentation (const DisplayRepresentation&);
	DisplayRepresentation& operator = (const DisplayRepresentation&);

    /*------------------------------------------------------------------------*/
    /** \brief  Type d'affichage
     */
    enum display_type{DISPLAY_GEOM = 1, DISPLAY_TOPO, DISPLAY_MESH, DISPLAY_SYSCOORD, DISPLAY_STRUCTURED_MESH};

    /*------------------------------------------------------------------------*/
    /** \brief  Type de représentation:
     * solide (pleine) ou filaire, voir avec des isocurves en plus
     *
     * Avec possibilité (pour la topologie) de voir le contour projeté suivant 
     * le maillage ou de voir l'association vers l'entité géométrique.
     */
    enum type {SOLID    = 1,
               WIRE     = 1 << 1,
               ISOCURVE = 1 << 2,
               SHOWMESHSHAPE = 1 << 3,
               SHOWASSOCIATION = 1 << 4,
               SHOWDISCRETISATIONTYPE = 1 <<5,
               MINIMUMWIRE = 1 <<6,      // autorise de ne mettre qu'un wire pour la représentation
			   SHOWTRIHEDRON = 1 <<7
    };

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    DisplayRepresentation(const type t, const display_type dt);

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~DisplayRepresentation();

    /*------------------------------------------------------------------------*/
    /// Type de display \see display_type
    display_type getDisplayType() const;

    /*------------------------------------------------------------------------*/
    /** \param		Type de display
	 *	\warning	<B>Ne pas utiliser, fonction prévue pour la sérialisation
	 *				(mode client/serveur).</B>
	 */
	void setDisplayType (display_type dt);

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit le type de la représentation géométrique.
     *          Le type retourné peut être l'un des énumérés de base ou une
     *          une union dessus (d'où le type de retour entier). Ainsi pour
     *          tester si on a renseigné le type SURFACE|CURVE, on pourra tester
     *          (getType()|CURVE)==CURVE pour savoir que la représentation de la
     *          courbe est demandée.
     */
    int getType() const;

    /*------------------------------------------------------------------------*/
    /** \brief  permet de changer le type de représentation
     */
    void setType(const int t);

    /*------------------------------------------------------------------------*/
    /** \brief  indique si le type de représentation rep fournie en argument
     *          est disponible
     *
     *  \param rep le type de représentation que l'on veut avoir
     */
    bool hasRepresentation(const type& rep) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute la gestion de la représentation rep
     *
     *  \param rep le nouveau type de représentation à gérer.
     */
    void addRepresentation(const type& rep);

    /*------------------------------------------------------------------------*/
    /** \brief  mise à jour du ratio de la représentation
     */
    virtual void setShrink(const double d);

    /*------------------------------------------------------------------------*/
    /** \brief  récupération du ratio de la représentation
     */
    double getShrink() const;

    /*------------------------------------------------------------------------*/
    /** \brief  récupère les points de la discrétisation
     */
    std::vector<Utils::Math::Point>& getPoints();

    /*------------------------------------------------------------------------*/
    /** \brief  récupère les éléments discrétisant la surface
     */
    std::vector<size_t>& getSurfaceDiscretization();

    /*------------------------------------------------------------------------*/
    /** \brief  récupère les éléments discrétisant les courbes
     */
    std::vector<size_t>& getCurveDiscretization();

    /*------------------------------------------------------------------------*/
    /** \brief  récupère les éléments discrétisant les courbes
     */
    std::vector<size_t>& getIsoCurveDiscretization();

    /*------------------------------------------------------------------------*/
    /** \brief  récupère le vecteur associé à la représentation
     */
    std::vector<Utils::Math::Point>& getVector();

protected:

    display_type m_displayType;

    /** type de discretisation */
    int m_type;

    /** shrink associée à la représentation à utiliser dans l'interval ]0 , 1]*/
    double m_shrink;

    /** Points portant la discrétisation de l'objet */
    mutable std::vector<Utils::Math::Point> m_points;

    /** Element utilisées pour la réprésentation sous forme d'indices des
     *  surfaces*/
    std::vector<size_t>   m_surfaceDiscretization;

    /** Element utilisées pour la réprésentation sous forme d'indices des
     *  courbes*/
    std::vector<size_t>   m_curveDiscretization;

    /** Element utilisées pour la réprésentation sous forme d'indices des
     *  iso-courbes*/
    std::vector<size_t>   m_isoCurveDiscretization;

    /** Couple de points pour définir un vecteur (projection de la topologie par exemple)
     */
    mutable std::vector<Utils::Math::Point> m_vector;


};
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* DISPLAY_REPRESENTATION_H_ */
/*----------------------------------------------------------------------------*/
