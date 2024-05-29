/*----------------------------------------------------------------------------*/
/** \file Curve.h
 *
 *  \author Franck Ledoux
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_CURVE_H_
#define MGX3D_GEOM_CURVE_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <list>
#include <sys/types.h>		// CP : uint sur Bull
/*----------------------------------------------------------------------------*/
#include "Geom/GeomEntity.h"
#include "Geom/GeomProperty.h"
#include "Geom/GeomRepresentation.h"
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
class Group1D;
}
namespace Topo {
class CoEdge;
class Vertex;
}
/*----------------------------------------------------------------------------*/
namespace Geom {

class GeomProperty;
/*----------------------------------------------------------------------------*/
/**
 * \class Curve
 * \brief Interface définissant les services que doit fournir toute courbe
 *        géométrique
 */
class Curve: public GeomEntity {

    static const char* typeNameGeomCurve;

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param ctx le contexte
     *  \param prop propriété (nom ...)
     *  \param disp propriétés d'affichage
     *  \param gprop    les propriétés associées à la courbe
     *  \param compProp les propriétés de calcul
     */
#ifndef SWIG
    Curve(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, GeomRepresentation* compProp);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param ctx le contexte
     *  \param prop propriété (nom ...)
     *  \param disp propriétés d'affichage
     *  \param gprop    les propriétés associées à la courbe
     *  \param compProp les propriétés de calcul
     */
#ifndef SWIG
    Curve(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, std::vector<GeomRepresentation*>& compProp);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Crée une copie (avec allocation mémoire, appel à new) de l'objet
     *          courant.
     */
    virtual GeomEntity* clone(Internal::Context&);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~Curve();

    /*------------------------------------------------------------------------*/
    /** \brief  récupère la liste des geom_entity référencés par (*this)
     *
     *  \param entities la liste des entités géométriques que l'on récupère
     */
    virtual void getRefEntities(std::vector<GeomEntity*>& entities);

    /*------------------------------------------------------------------------*/
    /** \brief  récupère la liste des geom_entity référencés par (*this) et la
     *          supprime ceux appartenant à entities.
     */
#ifndef SWIG
    virtual void clearRefEntities(std::list<GeomEntity*>& vertices,
            std::list<GeomEntity*>& curves,
            std::list<GeomEntity*>& surfaces,
            std::list<GeomEntity*>& volumes);
#endif

    virtual bool isEqual(Geom::Curve* curve);

    /*------------------------------------------------------------------------*/
    /** \brief  Décompose l'entité en ses sous-entités.
     */
    virtual void split(std::vector<Vertex* >&  vert);

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité géométrique
     */
    virtual int getDim() const {return 1;}

    /*------------------------------------------------------------------------*/
    /** Renseigne les paramètres pour les courbes composées
     *
     * \param ptStart position du sommet de départ pour ordonner la courbe
     */
    virtual void computeParams(Utils::Math::Point ptStart);

    /// Vérification que computeParams a bien été utilisé pour initialisé le cas composite
    void checkParams() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Calcule l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    virtual double computeArea() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux sommets géométriques incidents
     *
     *  \param vertices les sommets incidents
     */
    virtual void get(std::vector<Vertex*>& vertices) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux courbes géométriques incidentes
     *
     *  \param curves les courbes incidents
     */
    virtual void get(std::vector<Curve*>& curves) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux surfaces géométriques incidentes
     *
     *  \param surfaces les surfaces incidentes
     */
    virtual void get(std::vector<Surface*>& surfaces) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux volumes géométriques incidents
     *
     *  \param volumes les volumes incidents
     */
    virtual void get(std::vector<Volume*>& volumes) const;

    /*------------------------------------------------------------------------*/
    /// retourne la liste des arêtes topologiques communes qui pointent sur cette courbe
    virtual void get(std::vector<Topo::CoEdge*>& coedges);

    /*------------------------------------------------------------------------*/
   /// retourne la liste des sommets topologiques qui pointent sur cette courbe
    virtual void get(std::vector<Topo::Vertex*>& vertices);

    /*------------------------------------------------------------------------*/
    /** \brief Vérifie que la courbe ACurve est contenu dans *this
     *  \param ACurve la courbe contenue
     *  \return true si elle est contenue, false sinon
     */
    virtual bool contains(Curve* ACurve) const;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur la courbe. P est modifié
     *  \param P le point à projeter
     */
    virtual void project(Utils::Math::Point& P) const;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur la courbe, le résultat est le point P2.
     */
    virtual void project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const ;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le point en fonction du paramètre sur la courbe
     * \param p le paramètre curviligne compris entre 0 et 1
     */
    void getPoint(const double& p, Utils::Math::Point& Pt, const bool inO1=false) const ;


    /*------------------------------------------------------------------------*/
    /** \brief Calcul la tangente à une courbe en un point
     *
     *  \param P1 le point à projeter
     *  \param V2 la tangente recherchée suivant le projeté du point
     */
    void tangent(const Utils::Math::Point& P1, Utils::Math::Vector& V2) const ;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le point a l'intersection avec le plan
     * \param plan_cut le plan
     * \param Pt le résultat
     */
    void getIntersection(gp_Pln& plan_cut, Utils::Math::Point& Pt) const;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le paramètre curviligne du point sur la courbe
     * \param Pt le point sur la courbe
     */
    void getParameter(const Utils::Math::Point& Pt, double& p) const ;
    void getParameters(double& first, double& last) const;

    /*------------------------------------------------------------------------*/
    /** \brief Calcul les positions des points sur une courbe en fonction des paramètres
     * Cette fonction tient compte du cas de la courbe fermée pour laquelle
     * on a des paramètres qui passent par son extrémité
     *
     * \param Pt0 point de départ
     * \param Pt1 point d'arrivée
     * \param nbPt nombre de points à calculer
     * \param l_ratios les ratios pour les paramètres entre Pt0 et Pt1
     * \param l_points les points calculés, en retour
     */
#ifndef SWIG
    void getParametricsPoints(const Utils::Math::Point& Pt0,
            const Utils::Math::Point& Pt1,
            const uint nbPt,
            double* l_ratios,
            std::vector<Utils::Math::Point> &points);
    void getParametricsPointsLoops(const double& alpha0,
            const double& alpha1,
            const uint nbPt,
            double* l_ratios,
            Utils::Math::Point* l_points);
    /// idem avec découpage en angle
    void getPolarParametricsPoints(const Utils::Math::Point& Pt0,
            const Utils::Math::Point& Pt1,
            const uint nbPt,
            double* l_ratios,
            std::vector<Utils::Math::Point> &points,
			const Utils::Math::Point& polar_center);
#endif
    /*------------------------------------------------------------------------*/
    /** \brief  Access to the first end point
     *
     */
    Vertex* firstPoint() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the second end point
     *
     */
    Vertex* secondPoint() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Access to the parameterization interval of curve *this in the
     *			surface ASurf.
     *
     *	\param ASurf the surface we want to compute the curve parameterization
     *	\return ATMin min value of the parameterization
     *	\return ATMax max value of the parameterization
     */
    void bounds(Surface* ASurf, double& ATMin, double& ATMax) const;
    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute s comme surface incidente
     *
     *  \param s un pointeur sur une surface
     */

    /*------------------------------------------------------------------------*/
       /** \brief  Provides 2D data about the curve in the parametric space of the
 	  surface ASurf
 	  *
 	  *	\param  ASurf the surface we want to compute the curve parameterization
 	  *	\param  AT   the curve parameter we want extract data from
 	  *	\return AUV  the 2D (u,v) parameter
 	  *	\return ADT  the 2D 1st derivative to *this in the param. space of ASurf
 	  *	\return ADTT the 2D 2nd derivative to *this in the param. space of ASurf
 	  */
       void parametricData(Surface* ASurf, double& AT,
 					      double AUV[2], double ADT[2], double ADTT[2]) const;

       /*------------------------------------------------------------------------*/
       /** \brief  Provides the orientation of *this.
 	  */
       GeomOrientation orientation() const;

       /*------------------------------------------------------------------------*/
       /** \brief Indicates if *this is a curve internal to a surface. By internal,
        *		we mean that the curve does not belong to a wire enclosing ASurf but
        *		is an imprint inside ASurf
        *	\return a boolean value that means internal(true)/boundary(false)
        */
       bool isInternal() const;
       void getParameter2D(Surface* ASurf, const Utils::Math::Point& Pt, double& p) const;

#ifndef SWIG
    void add(Surface* s);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute v comme sommet incident
     *
     *  \param v un pointeur sur un sommet
     */
#ifndef SWIG
    void add (Vertex* v);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime s de la liste des surfaces incidentes. Si s n'apparait
     *          pas dans la liste cette méthode ne fait rien
     *
     *  \param s un pointeur sur une surface
     */
#ifndef SWIG
    void remove(Surface* s);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime v de la liste des sommets incidents. Si v n'apparait
     *          pas dans la liste cette méthode ne fait rien
     *
     *  \param v un pointeur sur un sommet
     */
#ifndef SWIG
    void remove(Vertex* v);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute à la liste les entités de niveaux inférieurs
     * (sommets), sans tri ni suppression des doublons
     *
     *  \param liste d'entity
     */
#ifndef SWIG
    virtual void addAllDownLevelEntity(std::list<GeomEntity*>& l_entity) const;
#endif

    /*------------------------------------------------------------------------*/
    /** \brief   retourne un point sur l'objet au centre si possible
     * \author Eric Brière de l'Isle
     */
    virtual Utils::Math::Point getCenteredPosition() const;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameGeomCurve;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::GeomCurve;}

    /*------------------------------------------------------------------------*/
   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "Crb";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est une Curve suivant son nom
     */
    static bool isA(const std::string& name);

    /*------------------------------------------------------------------------*/
    /** Ajoute le groupe parmis ceux auxquels appartient la courbe */
#ifndef SWIG
    void add(Group::Group1D* grp);
#endif

    /** Retire le groupe parmis ceux auxquels appartient la courbe */
#ifndef SWIG
    void remove(Group::Group1D* grp);
#endif

    /** Recherche le groupe parmis ceux auxquels appartient la courbe
     * return vrai si trouvé */
#ifndef SWIG
    bool find(Group::Group1D* grp);
#endif

    /// Retourne les noms des groupes auxquels appartient cette entité
    virtual void getGroupsName (std::vector<std::string>& gn) const;

    /// Retourne la liste des groupes auxquels appartient cette entité
    virtual void getGroups(std::vector<Group::GroupEntity*>& grp) const;

    /// Affecte une nouvelle liste des groupes auxquels appartient cette entité
    virtual void setGroups(std::vector<Group::GroupEntity*>& grp);

    /// Retourne le nombre de groupes
    virtual int getNbGroups() const;

/*------------------------------------------------------------------------*/
    /** \brief   détruit l'objet
     */
#ifndef SWIG
    virtual void setDestroyed(bool b);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief   indique si la courbe est un segment ou pas
     */
    bool isLinear() const;

    /*------------------------------------------------------------------------*/
    /** \brief   indique si la courbe est un cercle ou pas
     */
    bool isCircle() const;

    /*------------------------------------------------------------------------*/
    /** \brief   indique si la courbe est une ellipse ou pas
     */
    bool isEllipse() const;

    /*------------------------------------------------------------------------*/
    /** \brief   indique si la courbe est une BSpline ou pas
     */
    bool isBSpline() const;

    /*------------------------------------------------------------------------*/
    /** \brief   indique si la courbe est un wire (courbe composée) ou pas
     */
    bool isWire() const;

    /*------------------------------------------------------------------------*/
    /** \brief   retourne la position du centre dans d'une ellipse ou d'un cercle
     */
    Utils::Math::Point getCenter() const;

	/*------------------------------------------------------------------------*/
	/** \brief	Fournit une représentation textuelle de l'entité.
	 * \param	true si l'entité fourni la totalité de sa description, false si
	 * 			elle ne fournit que les informations non calculées (objectif :
	 * 			optimisation)
	 * \return	Description, à détruire par l'appelant.
	 */
#ifndef SWIG
    virtual Mgx3D::Utils::SerializedRepresentation* getDescription (
													bool alsoComputed) const;
#endif


protected:

    /*------------------------------------------------------------------------*/
    /** \brief  MAJ de l'état interne de l'objet géométrique spécifique au type
     *          d'entités géométriques (classes filles donc). Cette méthode est
     *          appelée par setMemento(...)
     */
    virtual void setFromSpecificMemento(MementoGeomEntity& mem);

    /*------------------------------------------------------------------------*/
    /** \brief  récupération de l'état interne de l'objet géométrique
     *          spécifique au type d'entités géométriques (classes filles
     *          donc).
     */
    virtual void createSpecificMemento(MementoGeomEntity& mem);

    /*------------------------------------------------------------------------*/
    /** Dit s'il faut appliquer la transformation géométrique aux entités adjacentes de dimension inférieurs
     */
    virtual bool needLowerDimensionalEntityModification();


        protected:
    std::vector<Surface*> m_surfaces;
    std::vector<Vertex*> m_vertices;
    /// Listes des groupes 1D auxquels appartient cette courbe
    std::vector<Group::Group1D*> m_groups;

private:
    /// premier paramètre local à une des composantes
    std::vector<double> paramLocFirst;
    /// dernier paramètre local à une des composantes
    std::vector<double> paramLocLast;
    /// premier paramètre image pour une des composantes, dans [0,1]
    std::vector<double> paramImgFirst;
    /// dernier paramètre image pour une des composantes, dans [0,1]
    std::vector<double> paramImgLast;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_CURVE_H_ */
/*----------------------------------------------------------------------------*/
