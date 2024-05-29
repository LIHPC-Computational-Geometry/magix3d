/*----------------------------------------------------------------------------*/
/*
 * PropertyHollowCylinder.cpp
 *
 *  Created on: 12 nov. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Geom/PropertyHollowCylinder.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
PropertyHollowCylinder::PropertyHollowCylinder(
        const double radius_int,const double radius_ext, const double height,
        const Utils::Math::Point& center, const Utils::Math::Vector& axis,
        const double angle )
: GeomProperty(), m_radius_int(radius_int), m_radius_ext(radius_ext),
  m_height(height), m_center(center), m_axis(axis), m_angle(angle)
{}
/*----------------------------------------------------------------------------*/
PropertyHollowCylinder::~PropertyHollowCylinder()
{}
/*----------------------------------------------------------------------------*/
GeomProperty::type PropertyHollowCylinder::getType() const
{
    return GeomProperty::HOLLOW_CYLINDER;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point PropertyHollowCylinder::getCenter() const
{
    return m_center;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Vector PropertyHollowCylinder::getAxis() const
{
    return m_axis;
}
/*----------------------------------------------------------------------------*/
double PropertyHollowCylinder::getRadiusInt() const
{
    return m_radius_int;
}
/*----------------------------------------------------------------------------*/
double PropertyHollowCylinder::getRadiusExt() const
{
    return m_radius_ext;
}
/*----------------------------------------------------------------------------*/
double PropertyHollowCylinder::getHeight() const
{
    return m_height;
}
/*----------------------------------------------------------------------------*/
double PropertyHollowCylinder::getAngle() const
{
    return m_angle;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowCylinder::setCenter(const Utils::Math::Point& center)
{
    m_center = center;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowCylinder::setAxis(const Utils::Math::Vector& axis)
{
    m_axis = axis;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowCylinder::setRadiusInt(const double radius)
{
    if(radius>0)
        m_radius_int = radius;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowCylinder::setRadiusExt(const double radius)
{
    if(radius>0)
        m_radius_ext = radius;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowCylinder::setHeight(const double height)
{
    if(height>0)
        m_height = height;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowCylinder::setAngle(const double angle)
{
    if(angle>0)
        m_angle = angle;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowCylinder::addDescription(Utils::SerializedRepresentation& geomProprietes) const
{
	// type associée à cette propriété
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Type", std::string("cylindre creux")));

    geomProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Rayon interne", m_radius_int));
    geomProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Rayon externe", m_radius_ext));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Hauteur", m_height));


	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Angle", m_angle));

	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Centre X", m_center.getX()));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Centre Y", m_center.getY()));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Centre Z", m_center.getZ()));

	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Vecteur X", m_axis.getX()));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Vecteur Y", m_axis.getY()));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Vecteur Z", m_axis.getZ()));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
