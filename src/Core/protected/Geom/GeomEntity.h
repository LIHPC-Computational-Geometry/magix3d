/*----------------------------------------------------------------------------*/
/** \file GeomEntity.h
 *
 *  \author Franck Ledoux
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_GEOMENTITY_H_
#define MGX3D_GEOM_GEOMENTITY_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <list>
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include "Internal/InternalEntity.h"
#include "Utils/Plane.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"
#include "Geom/GeomProperty.h"
#include "Topo/TopoEntity.h"
/*----------------------------------------------------------------------------*/
#include <gmds/math/Triangle.h>
/*----------------------------------------------------------------------------*/
#include <TDF_Label.hxx>
#include <TopoDS_Shape.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
class GroupEntity;
}
/*----------------------------------------------------------------------------*/
/*!
 * \namespace Mgx3D::Geom
 *
 * \brief Espace de nom des classes associées à la géométrie
 *
 *
 */
namespace Geom {
/*----------------------------------------------------------------------------*/
class Volume;
class Surface;
class Curve;
class Vertex;
class GeomProperty;
class MementoGeomEntity;
/*----------------------------------------------------------------------------*/
   /** \enum Orientation gives a list of orientation option for geometric
    *        entities.
    */
   typedef enum {
     GEOM_FORWARD,
     GEOM_REVERSED,
     GEOM_INTERNAL,
     GEOM_EXTERNAL
   } GeomOrientation;
/*----------------------------------------------------------------------------*/
/**
 * \class GeomEntity
 * \brief Interface définissant les services que doit fournir toute entité
 *        géométrique. Cette interface mutualise les services communs aux
 *        sommets, courbes, surfaces et volumes géométriques.
 *
 */
class GeomEntity : public Internal::InternalEntity{

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    GeomEntity(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop, const TopoDS_Shape& shape);

    /*------------------------------------------------------------------------*/
    /** Cas d'utilisation de OCAF
     */
    bool useOCAF() const;

    void projectPointOn( Utils::Math::Point& P) const;

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Crée une copie (avec allocation mémoire, appel à new) de l'objet
     *          courant.
     */
    virtual GeomEntity* clone(Internal::Context&)=0;

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomEntity();

    /*------------------------------------------------------------------------*/
    /** \brief  Retourne la boite englobante de l'entité.
	 * \param	En retour, la boite englobante définit comme suit
	 * 			<I>(xmin, xmax, ymin, ymax, zmin, zmax)</I>.
	 */
#ifndef SWIG
	virtual void getBounds (double bounds[6]) const;
#endif	// SWIG

    /*------------------------------------------------------------------------*/
    /** \brief  MAJ de la propriété. Par défaut, la propriété précédente est
     *          retournée
     */
#ifndef SWIG
    GeomProperty* setGeomProperty(GeomProperty* prop);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  MAJ de l'état interne de l'objet géométrique en fonction des
     *          informations stockées dans le mémento mem.
     */
#ifndef SWIG
    void setFromMemento(MementoGeomEntity& mem);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  récupération de l'état interne de l'objet géométrique sous la
     *          forme du mémento mem passé en argument
     */
#ifndef SWIG
    void createMemento(MementoGeomEntity& mem);
#endif

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  MAJ de l'état interne de l'objet géométrique spécifique au type
     *          d'entités géométriques (classes filles donc). Cette méthode est
     *          appelée par setMemento(...)
     */
#ifndef SWIG
    virtual void setFromSpecificMemento(MementoGeomEntity& mem);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  récupération de l'état interne de l'objet géométrique
     *          spécifique au type d'entités géométriques (classes filles
     *          donc).
     */
#ifndef SWIG
    virtual void createSpecificMemento(MementoGeomEntity& mem);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Calcule l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     *          Calcul qui peut être long. On ne le fait que quand cest nécessaire
     *          Sinon, on renvoit l'aire stockée
     */
    virtual double computeArea() const =0;

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Retourne l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    virtual double getArea() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Calcul de la boite englobante orientée selon les axes Ox,Oy,Oz
     *
     *  \param pmin Les coordonnées min de la boite englobante
     *  \param pmax Les coordonnées max de la boite englobante
     */
    virtual void computeBoundingBox (
							Utils::Math::Point& pmin,Utils::Math::Point& pmax,
                                    double tol = 0.00) const;


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
    /** \brief  Fournit l'accès aux sommets géométriques incidents
     *
     *  \param vertices les sommets incidents
     */
    virtual void get(std::vector<Vertex*>& vertices) const =0;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux courbes géométriques incidentes
     *
     *  \param curves les courbes incidents
     */
    virtual void get(std::vector<Curve*>& curves) const = 0;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux surfaces géométriques incidentes
     *
     *  \param surfaces les surfaces incidentes
     */
    virtual void get(std::vector<Surface*>& surfaces) const =0;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux volumes géométriques incidents
     *
     *  \param volumes les volumes incidents
     */
    virtual void get(std::vector<Volume*>& volumes) const =0;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur la géométrie. P est modifié
     *  \param P le point à projeter
     */
    virtual void project(Utils::Math::Point& P) const =0;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur la géométrie, le résultat est le point P2.
     */
    virtual void project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const =0;

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute e comme entité géométrique incidente. Selon la dimension
     *          de e, cet ajout peut être impossible et donc ne rien faire
     *
     *  \param e un pointeur sur une entité géométrique
     */
#ifndef SWIG
    virtual void add(GeomEntity* e);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute v comme volume incident
     *
     *  \param v un pointeur sur un volume
     */
#ifndef SWIG
    virtual void add(Volume* v);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute s comme surface incidente
     *
     *  \param s un pointeur sur une surface
     */
#ifndef SWIG
    virtual void add(Surface* s);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute c comme courbe incidente
     *
     *  \param c un pointeur sur une courbe
     */
#ifndef SWIG
    virtual void add(Curve* c);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute v comme sommet incident
     *
     *  \param v un pointeur sur un sommet
     */
#ifndef SWIG
    virtual void add(Vertex* v);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime e comme entité géométrique incidente. Selon la
     *          dimension de e, cet suppression peut être impossible et donc
     *          ne rien faire
     *
     *  \param e un pointeur sur une entité géométrique
     */
#ifndef SWIG
    virtual void remove(GeomEntity* e);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime v comme volume incident
     *
     *  \param v un pointeur sur un volume
     */
#ifndef SWIG
    virtual void remove(Volume* v);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime s comme surface incidente
     *
     *  \param s un pointeur sur une surface
     */
#ifndef SWIG
    virtual void remove(Surface* s);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime c comme courbe incidente
     *
     *  \param c un pointeur sur une courbe
     */
#ifndef SWIG
    virtual void remove(Curve* c);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Supprime v comme sommet incident
     *
     *  \param v un pointeur sur un sommet
     */
#ifndef SWIG
    virtual void remove(Vertex* v);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief   retourne un point sur l'objet au centre si possible
     * \author Eric Brière de l'Isle
     */
    virtual Utils::Math::Point getCenteredPosition() const =0;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès à la propriété de l'entité géométrique
     */
#ifndef SWIG
    virtual GeomProperty* getGeomProperty() const {return m_geomProp;}
#endif

	/*------------------------------------------------------------------------*/
	/** \return		Le type d'entité géométrique. Raccourci sur
	 *				<I>getGeomProperty ( )->getType ( )</I>.
	 */
	virtual Mgx3D::Geom::GeomProperty::type getGeomType ( ) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation affichable de l'entité en se basant
     *          sur la représentation passée en paramètre
     *
     *  \param  dr la représentation que l'on demande à afficher
     *  \param	Lève une exception si checkDestroyed vaut true
     */
#ifndef SWIG
    virtual void getRepresentation(
			Mgx3D::Utils::DisplayRepresentation& dr, bool checkDestroyed) const;
#endif

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
    /** \brief  Fournit un résumé textuel de l'entité.
     */
#ifndef SWIG
    virtual std::string getSummary ( ) const;
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation facétisée (point, segments, polygones) de l'entité
     *
     */
    virtual void getFacetedRepresentation(
            std::vector<gmds::math::Triangle  >& AVec) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation facétisée (point, segments, polygones) de l'entité
     *
     */
    virtual void facetedRepresentationForwardOrient(
            GeomEntity* AEntityOrientation,
            std::vector<gmds::math::Triangle >*) const;

    /*------------------------------------------------------------------------*/
    /** Ajoute une relation vers la topologie
     *  Il est vérifié que la relation n'y ait pas déjà
     * */
#ifndef SWIG
    virtual void addRefTopo(Topo::TopoEntity* te);
#endif

    /** Enlève une relation vers la topologie
     *  Il est vérifié que la relation y ait déjà
     * */
#ifndef SWIG
    virtual void removeRefTopo(Topo::TopoEntity* te);
#endif

    /** Retourne la liste des entités topologiques référencées */
#ifndef SWIG
    virtual void getRefTopo(std::vector<Topo::TopoEntity* >& vte) const;
    virtual const std::vector<Topo::TopoEntity* >& getRefTopo() const {return m_topo_entities;}
#endif

    /*------------------------------------------------------------------------*/
    /// Retourne les noms des groupes auxquels appartient cette entité
    virtual void getGroupsName (std::vector<std::string>& gn) const;

    /// Retourne la liste des groupes auxquels appartient cette entité
    virtual void getGroups(std::vector<Group::GroupEntity*>& grp) const;

    /// Affecte une nouvelle liste des groupes auxquels appartient cette entité
    virtual void setGroups(std::vector<Group::GroupEntity*>& grp);

    /// Retourne le nombre de groupes
    virtual int getNbGroups() const;

    /// Accès à la shape OCC sous-jacente.
    TopoDS_Shape& getShape();

    /*------------------------------------------------------------------------*/
    /** \brief Effectue la translation de l'entité géométrique associée de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param V le vecteur de translation à appliquer
     */
    void translate(const Utils::Math::Vector& V);

    /*------------------------------------------------------------------------*/
    /** \brief Effectue l'homothétié de l'entité géométrique associée de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param F le facteur d'homothétie
     *  \param center le centre d'homothétie
     */
    void scale(const double F, const Utils::Math::Point& center);
    void scale(const double factorX,
            const double factorY,
            const double factorZ,
            const Utils::Math::Point& center);

    /*------------------------------------------------------------------------*/
    /** \brief Effectue la rotation de l'entité géométrique associée de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param P1    premier point definissant l'axe de rotation
     *  \param P2    second point definissant l'axe de rotation
     *  \param Angle    angle de rotation en degré (compris entre ]0,360])
     */
    void rotate(const Utils::Math::Point& P1,
            const Utils::Math::Point& P2, double Angle);

    /*------------------------------------------------------------------------*/
    /** \brief Effectue la symétrie de l'entité géométrique associée de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param plane le plan de symétrie
     */
    void mirror(const Utils::Math::Plane& plane);

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit la précision
     *
     *  \return un double (en général de l'ordre de e-5)
     */
   double getPrecision();

private:

    /// Propriétés géométriques (qui peut être spécifique, PropertyBox par exemple)
    GeomProperty* m_geomProp;

    /// liens vers la topologie
    std::vector<Topo::TopoEntity* > m_topo_entities;

    /// pour savoir si computeArea doit être appelé
    mutable bool m_computedAreaIsUpToDate;

    /// résultat de la commande computeArea, qui peut être longue
    mutable double m_computedArea;

protected:

    TopoDS_Shape m_shape;

    /// label pour OCAF
    TDF_Label m_label;

    /// le label pour ajouter les construction suivantes
    static TDF_Label m_rootLabel;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_GEOMENTITY_H_ */
/*----------------------------------------------------------------------------*/
