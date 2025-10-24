#ifndef MGX3D_GEOM_GEOMENTITY_H_
#define MGX3D_GEOM_GEOMENTITY_H_
/*----------------------------------------------------------------------------*/
#include <set>
#include <vector>
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
#include "Services/DescriptionService.h"
/*----------------------------------------------------------------------------*/
#include <gmds/math/Triangle.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
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
class MementoEntity;
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
    virtual void accept(ConstGeomEntityVisitor& visitor) const = 0 ;
    virtual void accept(GeomEntityVisitor& visitor) = 0 ;

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
	virtual void getBounds (double bounds[6]) const;

    /*------------------------------------------------------------------------*/
    /** \brief  MAJ de la propriété. Par défaut, la propriété précédente est
     *          retournée
     */
    GeomProperty* setGeomProperty(GeomProperty* prop);

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

    void forceComputeArea() { m_computedAreaIsUpToDate = false; }

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
    virtual GeomProperty* getGeomProperty() const { return m_geomProp; }

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
    virtual void getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const;

   	/*------------------------------------------------------------------------*/
	/** \brief	Fournit une représentation textuelle de l'entité.
	 * \param	true si l'entité fourni la totalité de sa description, false si
	 * 			elle ne fournit que les informations non calculées (objectif :
	 * 			optimisation)
	 * \return	Description, à détruire par l'appelant.
	 */
    virtual Utils::SerializedRepresentation* getDescription (bool alsoComputed) const;

    /*------------------------------------------------------------------------*/
    /** \brief   détruit l'objet
     */
    virtual void setDestroyed(bool b);

private:
    /// Propriétés géométriques (qui peut être spécifique, PropertyBox par exemple)
    GeomProperty* m_geomProp;

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
