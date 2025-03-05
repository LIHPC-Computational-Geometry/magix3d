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
#include <set>
#include <list>
#include <functional>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include "Internal/InternalEntity.h"
#include "Utils/Point.h"
#include "Geom/GeomProperty.h"
#include "Geom/GeomEntityVisitor.h"
#include "Topo/TopoEntity.h"
/*----------------------------------------------------------------------------*/
#include <gmds/math/Triangle.h>
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
    /** \brief  Constructeur. Une entité délègue un certain nombre de calculs
     *          géométrique à un objet de type ComputationalProperty.
     *
     *          Une fois une propriété associée à une entité, la mort de
     *          l'entité entrainera celle des propriétés attachées.
     */
    GeomEntity(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
            GeomProperty* gprop);

public:
    virtual void apply(std::function<void(const TopoDS_Shape&)> const& lambda) const = 0;
    virtual void applyAndReturn(std::function<TopoDS_Shape(const TopoDS_Shape&)> const& lambda) = 0;
    virtual void accept(GeomEntityVisitor& visitor) const = 0 ;

    /*------------------------------------------------------------------------*/
    /** \brief  Crée une copie (avec allocation mémoire, appel à new) de l'objet
     *          courant.
     */
    virtual GeomEntity* clone(Internal::Context&){
        throw TkUtil::Exception (TkUtil::UTF8String ("GeomEntity::clone pas implementee.", TkUtil::Charset::UTF_8));
    };

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

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Calcule l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    virtual double computeArea() const =0;

    /*------------------------------------------------------------------------*/
    /** \brief  Retourne l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    virtual double getArea() const;

    /** \brief  Stocke l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    virtual void setArea(double area)
    {m_computedArea = area;}

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur le sommet. P est modifié
     *  \param P le point à projeter
     */
    virtual uint project(Utils::Math::Point& P) const = 0;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur le sommet, le résultat est le point P2.
     */
    virtual uint project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const = 0;

    /*------------------------------------------------------------------------*/
    /** \brief  Calcul de la boite englobante orientée selon les axes Ox,Oy,Oz
     *
     *  \param pmin Les coordonnées min de la boite englobante
     *  \param pmax Les coordonnées max de la boite englobante
     */
    virtual void computeBoundingBox(Utils::Math::Point& pmin, Utils::Math::Point& pmax) const = 0;

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

private:
#ifndef SWIG
    template<typename T>
    void buildSerializedRepresentation(Utils::SerializedRepresentation& description, const std::string& title, 
                                                const std::set<T*, decltype(&Utils::Entity::compareEntity)> elements) const;
#endif

public:
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

private:
    /// Propriétés géométriques (qui peut être spécifique, PropertyBox par exemple)
    GeomProperty* m_geomProp;

    /// liens vers la topologie
    std::vector<Topo::TopoEntity* > m_topo_entities;

    /// pour savoir si computeArea doit être appelé
    mutable bool m_computedAreaIsUpToDate;

    /// résultat de la commande computeArea, qui peut être longue
    mutable double m_computedArea;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_GEOMENTITY_H_ */
/*----------------------------------------------------------------------------*/
