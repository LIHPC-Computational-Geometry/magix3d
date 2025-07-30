#ifndef MGX3D_GEOM_CURVE_H_
#define MGX3D_GEOM_CURVE_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <list>
#include <sys/types.h>		// CP : uint sur Bull
/*----------------------------------------------------------------------------*/
#include "Geom/GeomEntity.h"
#include "Geom/GeomProperty.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Edge.hxx>
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

    friend class MementoManager;
    static const char* typeNameGeomCurve;

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param ctx le contexte
     *  \param prop propriété (nom ...)
     *  \param disp propriétés d'affichage
     *  \param gprop les propriétés associées à la courbe
     *  \param shape la shape OCC
     */
#ifndef SWIG
    Curve(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, TopoDS_Edge& shape);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param ctx le contexte
     *  \param prop propriété (nom ...)
     *  \param disp propriétés d'affichage
     *  \param gprop les propriétés associées à la courbe
     *  \param shapes les shapes OCC
     */
#ifndef SWIG
    Curve(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, std::vector<TopoDS_Edge>& shapes);
#endif
    const std::vector<TopoDS_Edge>& getOCCEdges() const { return m_occ_edges; }

    virtual void apply(std::function<void(const TopoDS_Shape&)> const& lambda) const;
    virtual void applyAndReturn(std::function<TopoDS_Shape(const TopoDS_Shape&)> const& lambda);
    virtual void accept(ConstGeomEntityVisitor& v) const { v.visit(this); }
    virtual void accept(GeomEntityVisitor& v) { v.visit(this); }

    /*------------------------------------------------------------------------*/
    /** \brief  Crée une copie (avec allocation mémoire, appel à new) de l'objet
     *          courant.
     */
    virtual GeomEntity* clone(Internal::Context&);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~Curve() = default;

    virtual bool isEqual(Geom::Curve* curve);

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité géométrique
     */
    virtual int getDim() const {return 1;}

    /*------------------------------------------------------------------------*/
    /** \brief  Calcule l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    virtual double computeArea() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Calcul de la boite englobante orientée selon les axes Ox,Oy,Oz
     *
     *  \param pmin Les coordonnées min de la boite englobante
     *  \param pmax Les coordonnées max de la boite englobante
     */
    virtual void computeBoundingBox(Utils::Math::Point& pmin, Utils::Math::Point& pmax) const;

    /*------------------------------------------------------------------------*/
    /// retourne la liste des arêtes topologiques communes qui pointent sur cette courbe
    virtual void get(std::vector<Topo::CoEdge*>& coedges);

    /*------------------------------------------------------------------------*/
   /// retourne la liste des sommets topologiques qui pointent sur cette courbe
    virtual void get(std::vector<Topo::Vertex*>& vertices);

    /*------------------------------------------------------------------------*/
    /** \brief Donne le point en fonction du paramètre sur la courbe
     * \param p le paramètre curviligne
     * \param in01 Si le paramétre est compris entre 0 et 1
     */
    void getPoint(const double& p, Utils::Math::Point& Pt, const bool in01=false) const;

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
    /** \brief  Ajoute s comme surface incidente
     *
     *  \param s un pointeur sur une surface
     */

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

    /// Retourne la liste des groupes auxquels appartient cette entité
    virtual const std::vector<Group::Group1D*>& getGroups() const {return m_groups;}

    /// Affecte une nouvelle liste des groupes auxquels appartient cette entité
    virtual void setGroups(std::vector<Group::GroupEntity*>& grp);

    /// Retourne le nombre de groupes
    virtual int getNbGroups() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Return the surfaces incident to this curve
     */
    const std::vector<Surface*>& getSurfaces() const { return m_surfaces; }

    /*------------------------------------------------------------------------*/
    /** \brief  Return the vertices incident to this curve
     */
    const std::vector<Vertex*>& getVertices() const { return m_vertices; }
    
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

private:
    std::vector<Surface*> m_surfaces;
    std::vector<Vertex*> m_vertices;
    /// Listes des groupes 1D auxquels appartient cette courbe
    std::vector<Group::Group1D*> m_groups;
    /// représentation open cascade
    std::vector<TopoDS_Edge> m_occ_edges;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_CURVE_H_ */
/*----------------------------------------------------------------------------*/
