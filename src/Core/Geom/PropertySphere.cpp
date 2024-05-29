/*----------------------------------------------------------------------------*/
/*
 * PropertySphere.cpp
 *
 *  Created on: 22 mars 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Geom/PropertySphere.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
PropertySphere::PropertySphere(
        const Utils::Math::Point& center, const double radius,
        const Utils::Portion::Type& type, const double angle)
: GeomProperty(), m_center(center), m_radius(radius), m_portion(type),
  m_angle(angle)
{}
/*----------------------------------------------------------------------------*/
PropertySphere::~PropertySphere()
{}
/*----------------------------------------------------------------------------*/
GeomProperty::type PropertySphere::getType() const
{
    return GeomProperty::SPHERE;
}
/*----------------------------------------------------------------------------*/
Utils::Portion::Type PropertySphere::getPortionType() const
{
    return m_portion;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point const& PropertySphere::getCenter() const
{
    return m_center;
}
/*----------------------------------------------------------------------------*/
void PropertySphere::setCenter(const Utils::Math::Point& c)
{
    m_center = c;
}
/*----------------------------------------------------------------------------*/
double PropertySphere::getRadius() const
{
    return m_radius;
}
/*----------------------------------------------------------------------------*/
void PropertySphere::setRadius(const double r)
{
    m_radius=r;
}
/*----------------------------------------------------------------------------*/
double PropertySphere::getAngle() const
{
    return m_angle;
}
/*----------------------------------------------------------------------------*/
void PropertySphere::setAngle(const double a)
{
    m_angle=a;
}
/*----------------------------------------------------------------------------*/
void PropertySphere::addDescription(Utils::SerializedRepresentation& geomProprietes) const
{
	// type associée à cette propriété
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Type", std::string("sphère pleine")));

	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Centre X", m_center.getX()));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Centre Y", m_center.getY()));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Centre Z", m_center.getZ()));

	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Rayon", m_radius));


	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Angle", m_angle));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Portion", Utils::Portion::getName(m_portion)));

}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
