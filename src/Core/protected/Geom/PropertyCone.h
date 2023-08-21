/*----------------------------------------------------------------------------*/
/** \file PropertyCone.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5/5/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef PROPERTY_CONE_H_
#define PROPERTY_CONE_H_
/*----------------------------------------------------------------------------*/
//inclusion de fichiers de la STL
#include <string>
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Utils/Vector.h"
#include "Geom/GeomProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class PropertyCone
 * \brief Décrit les propriétés d'un cylindre,
 * un cercle comme base et un vecteur comme support
 */
class PropertyCone: public GeomProperty {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param radius le rayon du cylindre
     *  \param height la hauteur du cylindre
     *  \param center le centre de la base du cylindre
     *  \param axis   l'axe du cylindre
     *  \param angle l'angle correspondant à la portion de cylindre (degrés)
     */
	PropertyCone(const double radius1=0,
			const double radius2=0.1,
			const double length = 1,
			const Utils::Math::Vector& axis = Utils::Math::Vector(0,0,1),
			const double angle=0);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~PropertyCone();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le type de propriété
     */
    GeomProperty::type getType() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur le rayon
     */
    double getRadius1() const;
    void setRadius1(const double radius);
    double getRadius2() const;
    void setRadius2(const double radius);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur la hauteur
     */
    double getHeight() const;
    void setHeight(const double height);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur le centre de la base
     */
    Utils::Math::Point getCenter() const;
    void setCenter(const Utils::Math::Point& center);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur l'axe du cylindre
     */
    Utils::Math::Vector getAxis() const;
    void setAxis(const Utils::Math::Vector& axis);


    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur l'angle (en degrés)
     */
    double getAngle() const;
    void setAngle(const double angle);

    /*------------------------------------------------------------------------*/
    /** permet l'affichage des propriétés spécifiques de l'objet */
    virtual void addDescription(Utils::SerializedRepresentation& geomProprietes) const;

protected:

    /** rayon du cylindre côté sommet */
    double m_radius1;
    /** rayon du cylindre côté base */
    double m_radius2;

    /** hauteur du cône */
    double m_height;

    /** vecteur d'alignement du cône */
    Utils::Math::Vector m_axis;

    /** angle de la portion du cône*/
    double m_angle;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* PROPERTY_CONE_H_ */
/*----------------------------------------------------------------------------*/

