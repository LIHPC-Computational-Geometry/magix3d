/*----------------------------------------------------------------------------*/
/** \file PropertyCylinder.h
 *
 *  \author Franck Ledoux
 *
 *  \date 09/11/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef PROPERTY_CYLINDER_H_
#define PROPERTY_CYLINDER_H_
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
 * \class PropertyCylinder
 * \brief Décrit les propriétés d'un cylindre,
 * un cercle comme base et un vecteur comme support
 */
class PropertyCylinder: public GeomProperty {

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
    PropertyCylinder(const double radius=1,
                     const double height = 1,
                     const Utils::Math::Point& center = Utils::Math::Point(0,0,0),
                     const Utils::Math::Vector& axis = Utils::Math::Vector(0,0,1),
                     const double angle=0);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~PropertyCylinder();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le type de propriété
     */
    GeomProperty::type getType() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur le rayon
     */
    double getRadius() const;
    void setRadius(const double radius);

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

    /** rayon du cylindre */
    double m_radius;

    /** hauteur du cylindre */
    double m_height;

    /** point du centre de la base */
    Utils::Math::Point m_center;

    /** vecteur d'alignement du cylindre */
    Utils::Math::Vector m_axis;

    /** angle de la portion du cylindre*/
    double m_angle;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* PROPERTY_CYLINDER_H_ */
/*----------------------------------------------------------------------------*/

