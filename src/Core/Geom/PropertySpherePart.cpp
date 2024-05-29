/*----------------------------------------------------------------------------*/
/*
 * PropertySpherePart.cpp
 *
 *  Created on: 21 mars 2016
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Geom/PropertySpherePart.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
PropertySpherePart::PropertySpherePart(
		const double radius,
		const double angleY,
		const double angleZ)
: GeomProperty()
, m_radius(radius)
, m_angleY(angleY)
, m_angleZ(angleZ)
{}
/*----------------------------------------------------------------------------*/
PropertySpherePart::~PropertySpherePart()
{}
/*----------------------------------------------------------------------------*/
GeomProperty::type PropertySpherePart::getType() const
{
    return GeomProperty::SPHERE_PART;
}
/*----------------------------------------------------------------------------*/
double PropertySpherePart::getRadius() const
{
    return m_radius;
}
/*----------------------------------------------------------------------------*/
void PropertySpherePart::setRadius(const double r)
{
    m_radius=r;
}
/*----------------------------------------------------------------------------*/
double PropertySpherePart::getAngleY() const
{
    return m_angleY;
}
/*----------------------------------------------------------------------------*/
void PropertySpherePart::setAngleY(const double a)
{
    m_angleY=a;
}
/*----------------------------------------------------------------------------*/
double PropertySpherePart::getAngleZ() const
{
    return m_angleZ;
}
/*----------------------------------------------------------------------------*/
void PropertySpherePart::setAngleZ(const double a)
{
    m_angleZ=a;
}
/*----------------------------------------------------------------------------*/
void PropertySpherePart::addDescription(Utils::SerializedRepresentation& geomProprietes) const
{
	// type associée à cette propriété
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Type", std::string("aiguille pleine")));

	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Rayon", m_radius));

	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Angle Y", m_angleY));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Angle Z", m_angleZ));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
