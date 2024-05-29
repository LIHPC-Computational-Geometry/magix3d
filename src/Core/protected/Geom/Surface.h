/*----------------------------------------------------------------------------*/
/** \file Surface.h
 *
 *  \author Franck Ledoux
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_SURFACE_H_
#define MGX3D_GEOM_SURFACE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/GeomEntity.h"
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
#include <list>

class TopoDS_Edge;
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
class Group2D;
}

namespace Topo {
class CoFace;
class CoEdge;
class Vertex;
}
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Loop;
class Curve;
class Volume;
class Vertex;
class GeomProperty;
/*----------------------------------------------------------------------------*/
/**
 * \class Surface
 * \brief Interface définissant les services que doit fournir toute surface
 *        géométrique
 */
class Surface: public GeomEntity {

    static const char* typeNameGeomSurface;

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param ctx le contexte
     *  \param prop propriété (nom ...)
     *  \param disp propriétés d'affichage
     *  \param gprop    les propriétés associées à la surface
     *  \param compProp les propriétés de calcul
     */
#ifndef SWIG
    Surface(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, GeomRepresentation* compProp=0);
#endif

    /** \brief  Constructeur
     *
     *  \param ctx le contexte
     *  \param prop propriété (nom ...)
     *  \param disp propriétés d'affichage
     *  \param gprop    les propriétés associées à la surface composite
     *  \param compProp les propriétés de calcul des surfaces
     */
#ifndef SWIG
    Surface(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
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
    virtual ~Surface();
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

    /*------------------------------------------------------------------------*/
    /** \brief  Décompose l'entité en ses sous-entités.
     */
    virtual void split(std::vector<Curve* >& curv,
                       std::vector<Vertex* >&  vert);

    /*------------------------------------------------------------------------*/
    /** \brief  Calcule l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    virtual double computeArea() const;

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la dimension de l'entité géométrique
     */
    virtual int getDim() const {return 2;}

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
    virtual void getTemporary(std::vector<Curve*>& curves) const;

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
    /** \brief Vérifie que la surface ASurf est contenue dans *this
     *  \param ASurf la surface contenue
     *  \return true si elle est contenue, false sinon
     */
    virtual bool contains(Surface* ASurf) const;


    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur la surface. P est modifié
     *  \param P le point à projeter
     */
    virtual void project(Utils::Math::Point& P) const;

    /*------------------------------------------------------------------------*/
    /** \brief Calcul la normale à une surface en un point
     *
     *  \param P1 le point à projeter
     *  \param V2 la normale recherchée suivant le projeté du point
     */
    virtual void normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2) const;

    /*------------------------------------------------------------------------*/
    /** \brief Fournit les parametres [U1,U2]x[V1,V2] de la parametrisation de
     *         la surface
     */
    virtual void getParametricBounds(double& U1,double& U2,double& V1,double& V2) const;
    /*------------------------------------------------------------------------*/
    /** \brief Get the parametrics bounds of this surface
     *
     *  \return AUMin u min value of the paramtric space
     *  \return AUMax u max value of the paramtric space
     *  \return AVMin v min value of the paramtric space
     *  \return AVMax v max value of the paramtric space
     */
    virtual void bounds(double& AUMin, double& AUMax,
    			double& AVMin, double& AVMax) const;

    /*------------------------------------------------------------------------*/
    /** \brief Computes the 3D point AP, the first and second derivative in the
     *			directions u and v at the point of parametric coordinates
     *			(AU, AV)
     *
     *	\param  AU u parameter
     *  \param  AV v parameter
     *  \return AP 3D point in (u,v) on this surface
     *  \return ADU  first derivative in u at point (u,v) on this surface
     *  \return ADV  first derivative in v at point (u,v) on this surface
     *  \return ADUU second derivative in u   at point (u,v) on this surface
     *  \return ADUV second derivative in u,v at point (u,v) on this surface
     *  \return ADVV second derivative in v   at point (u,v) on this surface
     */
    virtual void d2(const double& AU, const double& AV,
    			Utils::Math::Point& AP,
				Utils::Math::Vector& ADU,
				Utils::Math::Vector& ADV,
				Utils::Math::Vector& ADUU,
				Utils::Math::Vector& ADUV,
				Utils::Math::Vector& ADVV) const;
    /*------------------------------------------------------------------------*/
    /** \brief  Provides the orientation of this surface.
	  */
    virtual GeomOrientation orientation() const;

     /*------------------------------------------------------------------------*/
     /** \brief Retourne le point de parametre (u,v) dans l'espace de parametre
      *         de la surface. Si (u,v) est choisi hors des bornes [U1,U2] x
      *         [V1,V2] retournées par getParametricBounds, une exception est
      *         levée. La robustesse de cette méthode repose sur OCC
      */
    virtual Utils::Math::Point getPoint(const double u, const double v) const;
    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur la surface, le résultat est le point P2.
     */
    virtual void project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute v comme volume incident
     *
     *  \param v un pointeur sur un volume
     */
#ifndef SWIG
      virtual void add(Volume* v);
//    /*------------------------------------------------------------------------*/
//    /** \brief  Ajoute c comme courbe incidente
//     *
//     *  \param c un pointeur sur une courbe
//     */
//    virtual void add(TopoDS_Edge e);
//    std::vector<TopoDS_Edge> getEdges();

#endif


#ifndef SWIG
    virtual void add(Curve* c);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime v de la liste des volumes incidents. Si v n'apparait
     *          pas dans la liste cette méthode ne fait rien
     *
     *  \param v un pointeur sur un volume
     */
#ifndef SWIG
    virtual void remove(Volume* v);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime c de la liste des courbes incidentes. Si c n'apparait
     *          pas dans la liste cette méthode ne fait rien
     *
     *  \param v un pointeur sur un volume
     */
#ifndef SWIG
    virtual void remove(Curve* c);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute à la liste les entités de niveaux inférieurs
     * (courbes et sommets), sans tri ni suppression des doublons
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
    virtual std::string getTypeName() const {return typeNameGeomSurface;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::GeomSurface;}

    /*------------------------------------------------------------------------*/
   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "Surf";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est une Surface suivant son nom
     */
    static bool isA(const std::string& name);

    /*------------------------------------------------------------------------*/
    /** Ajoute le groupe parmis ceux auxquels appartient la surface */
#ifndef SWIG
    virtual void add(Group::Group2D* grp);
#endif

    /** Retire le groupe parmis ceux auxquels appartient la surface */
#ifndef SWIG
    virtual void remove(Group::Group2D* grp);
#endif

    /** Recherche le groupe parmis ceux auxquels appartient la surface
     * return vrai si trouvé */
#ifndef SWIG
    virtual bool find(Group::Group2D* grp);
#endif

    /// Retourne les noms des groupes auxquels appartient cette entité
    virtual void getGroupsName (std::vector<std::string>& gn) const;

    /// Retourne la liste des groupes auxquels appartient cette entité
    virtual void getGroups(std::vector<Group::GroupEntity*>& grp) const;

    /// Retourne la liste des groupes 2D auxquels appartient cette entité
    virtual const std::vector<Group::Group2D*>& getGroups() const {return m_groups;}

    /// Affecte une nouvelle liste des groupes auxquels appartient cette entité
    virtual void setGroups(std::vector<Group::GroupEntity*>& grp);

    /// Retourne le nombre de groupes
    virtual int getNbGroups() const;

    /*------------------------------------------------------------------------*/
    /// retourne la liste des faces topologiques communes qui pointent sur cette surface
    virtual void get(std::vector<Topo::CoFace*>& cofaces);

    /// retourne la liste des arêtes topologiques communes qui pointent sur cette surface
    virtual void get(std::vector<Topo::CoEdge*>& coedges);

    /// retourne la liste des sommets topologiques qui pointent sur cette surface
    virtual void get(std::vector<Topo::Vertex*>& vertices);

    /*------------------------------------------------------------------------*/
    /** \brief   détruit l'objet
     */
#ifndef SWIG
    virtual void setDestroyed(bool b);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief   indique si la surface est un plan ou pas
     */
    virtual bool isPlanar() const;

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

    /*------------------------------------------------------------------------*/
    /** Dit s'il faut appliquer la transformation géométrique aux entités adjacentes de dimension inférieurs
     */
    virtual bool needLowerDimensionalEntityModification();


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

protected:

    std::vector<Curve*> m_curves;
//    std::vector<TopoDS_Edge> m_occ_edges;
    //std::vector<TopoDS_Edge> m_occ_edges;
    std::vector<Volume*> m_volumes;

    /// Listes des groupes 2D auxquels appartient cette surface
    std::vector<Group::Group2D*> m_groups;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_SURFACE_H_ */
/*----------------------------------------------------------------------------*/

