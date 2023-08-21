/*----------------------------------------------------------------------------*/
/*
 * PropertySphere.h
 *
 *  Created on: 22 mars 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef PROPERTYSPHERE_H_
#define PROPERTYSPHERE_H_
/*----------------------------------------------------------------------------*/
//inclusion de fichiers de la STL
#include <string>
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Utils/Constants.h"
#include "Geom/GeomProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class PropertySphere
 * \brief Décrit les propriétés d'une sphère
 */
class PropertySphere: public GeomProperty {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param center  le centre de la sphère
     *  \param radius  le rayon de la sphère
     *  \param angle   l'angle de la portion de sphère
     *  \param portion le type de portion de sphère créée
     */
    PropertySphere(const Utils::Math::Point& center, const double radius,
                   const Utils::Portion::Type& type, const double angle);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~PropertySphere();

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le type de propriété
     */
    GeomProperty::type getType() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur le centre de la sphère
     */
    Utils::Math::Point const& getCenter() const;
    void setCenter(const Utils::Math::Point& c);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur le rayon de la sphère
     */
    double getRadius() const;
    void setRadius(const double r);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur l'angle de la sphère
     */
    double getAngle() const;
    void setAngle(const double r);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur sur le type de portion de sphère
     */
    Utils::Portion::Type getPortionType() const;

    /*------------------------------------------------------------------------*/
    /** permet l'affichage des propriétés spécifiques de l'objet */
    virtual void addDescription(Utils::SerializedRepresentation& geomProprietes) const;

protected:

    /** centre */
    Utils::Math::Point m_center;
    /** rayon */
    double m_radius;
    /** type de portion */
    Utils::Portion::Type m_portion;
    /** angle */
    double m_angle;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* PROPERTYSPHERE_H_ */
/*----------------------------------------------------------------------------*/

