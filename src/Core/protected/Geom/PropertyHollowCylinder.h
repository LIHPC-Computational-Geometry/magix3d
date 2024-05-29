/*----------------------------------------------------------------------------*/
/*
 * PropertyHollowCylinder.h
 *
 *  Created on: 12 nov. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef PROPERTYHOLLOWCYLINDER_H_
#define PROPERTYHOLLOWCYLINDER_H_
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
 * \class PropertyHollowCylinder
 * \brief Décrit les propriétés d'un cylindre creux,
 * un cercle comme base et un vecteur comme support
 */
class PropertyHollowCylinder: public GeomProperty {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param radius_int le rayon interne du cylindre creux
     *  \param radius_ext le rayon externe du cylindre creux
     *  \param height la hauteur du cylindre creux
     *  \param center le centre de la base du cylindre creux
     *  \param axis   l'axe du cylindre creux
     *  \param angle l'angle correspondant à la portion de cylindre creux (degrés)
     */
    PropertyHollowCylinder(const double radius_int=0.5,
            const double radius_ext=1,
            const double height = 1,
            const Utils::Math::Point& center = Utils::Math::Point(0,0,0),
            const Utils::Math::Vector& axis = Utils::Math::Vector(0,0,1),
            const double angle=0);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~PropertyHollowCylinder();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le type de propriété
     */
    GeomProperty::type getType() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur le rayon interne
     */
    double getRadiusInt() const;
    void setRadiusInt(const double radius);
    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur le rayon externe
     */
    double getRadiusExt() const;
    void setRadiusExt(const double radius);

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

    /** rayon interne du cylindre creux*/
    double m_radius_int;
    /** rayon externe du cylindre creux*/
    double m_radius_ext;

    /** hauteur du cylindre creux */
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
#endif /* PROPERTYHOLLOWCYLINDER_H_ */
/*----------------------------------------------------------------------------*/


