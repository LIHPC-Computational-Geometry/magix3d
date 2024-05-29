/*----------------------------------------------------------------------------*/
/*
 * PropertyHollowSpherePart.cpp
 *
 *  Created on: 29 mars 2016
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Geom/PropertyHollowSpherePart.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
PropertyHollowSpherePart::PropertyHollowSpherePart(
		const double radius_int,
		const double radius_ext,
		const double angleY,
		const double angleZ)
: GeomProperty()
, m_radius_int(radius_int)
, m_radius_ext(radius_ext)
, m_angleY(angleY)
, m_angleZ(angleZ)
{}
/*----------------------------------------------------------------------------*/
PropertyHollowSpherePart::~PropertyHollowSpherePart()
{}
/*----------------------------------------------------------------------------*/
GeomProperty::type PropertyHollowSpherePart::getType() const
{
    return GeomProperty::HOLLOW_SPHERE_PART;
}
/*----------------------------------------------------------------------------*/
double PropertyHollowSpherePart::getRadiusInt() const
{
    return m_radius_int;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowSpherePart::setRadiusInt(const double r)
{
    m_radius_int=r;
}
/*----------------------------------------------------------------------------*/
double PropertyHollowSpherePart::getRadiusExt() const
{
    return m_radius_ext;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowSpherePart::setRadiusExt(const double r)
{
    m_radius_ext=r;
}
/*----------------------------------------------------------------------------*/
double PropertyHollowSpherePart::getAngleY() const
{
    return m_angleY;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowSpherePart::setAngleY(const double a)
{
    m_angleY=a;
}
/*----------------------------------------------------------------------------*/
double PropertyHollowSpherePart::getAngleZ() const
{
    return m_angleZ;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowSpherePart::setAngleZ(const double a)
{
    m_angleZ=a;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowSpherePart::addDescription(Utils::SerializedRepresentation& geomProprietes) const
{
	// type associée à cette propriété
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Type", std::string("aiguille creuse")));

    geomProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Rayon interne", m_radius_int));

    geomProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Rayon externe", m_radius_ext));

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
