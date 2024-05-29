/*----------------------------------------------------------------------------*/
/*
 * PropertyHollowSphere.h
 *
 *  Created on: 12 nov. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef PROPERTYHOLLOWSPHERE_H_
#define PROPERTYHOLLOWSPHERE_H_
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
 * \class PropertyHollowSphere
 * \brief Décrit les propriétés d'une sphère creuse
 */
class PropertyHollowSphere: public GeomProperty {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param center     le centre de la sphère creuse
     *  \param radius_int le rayon interne de la sphère creuse
     *  \param radius_ext le rayon externe de la sphère creuse
     *  \param angle      l'angle de la portion de sphère creuse
     *  \param portion    le type de portion de sphère creuse créée
     */
    PropertyHollowSphere(const Utils::Math::Point& center,
            const double radius_int,
            const double radius_ext,
            const Utils::Portion::Type& type, const double angle);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~PropertyHollowSphere();

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
    /** \brief  Accesseur et modificateur sur le rayon interne de la sphère
     */
    double getRadiusInt() const;
    void setRadiusInt(const double r);

    /*------------------------------------------------------------------------*/
    /** \brief  Accesseur et modificateur sur le rayon externe de la sphère
     */
    double getRadiusExt() const;
    void setRadiusExt(const double r);

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
    /** rayon interne */
    double m_radius_int;
    /** rayon externe */
    double m_radius_ext;
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
#endif /* PROPERTYHOLLOWSPHERE_H_ */
/*----------------------------------------------------------------------------*/
