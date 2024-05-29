/*----------------------------------------------------------------------------*/
/*
 * PropertyHollowSphere.cpp
 *
 *  Created on: 12 nov. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Geom/PropertyHollowSphere.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
PropertyHollowSphere::PropertyHollowSphere(
        const Utils::Math::Point& center, const double radius_int,
        const double radius_ext,
        const Utils::Portion::Type& type, const double angle)
: GeomProperty(), m_center(center), m_radius_int(radius_int),
  m_radius_ext(radius_ext), m_portion(type),
  m_angle(angle)
{}
/*----------------------------------------------------------------------------*/
PropertyHollowSphere::~PropertyHollowSphere()
{}
/*----------------------------------------------------------------------------*/
GeomProperty::type PropertyHollowSphere::getType() const
{
    return GeomProperty::HOLLOW_SPHERE;
}
/*----------------------------------------------------------------------------*/
Utils::Portion::Type PropertyHollowSphere::getPortionType() const
{
    return m_portion;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point const& PropertyHollowSphere::getCenter() const
{
    return m_center;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowSphere::setCenter(const Utils::Math::Point& c)
{
    m_center = c;
}
/*----------------------------------------------------------------------------*/
double PropertyHollowSphere::getRadiusInt() const
{
    return m_radius_int;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowSphere::setRadiusInt(const double r)
{
    m_radius_int=r;
}
/*----------------------------------------------------------------------------*/
double PropertyHollowSphere::getRadiusExt() const
{
    return m_radius_ext;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowSphere::setRadiusExt(const double r)
{
    m_radius_ext=r;
}
/*----------------------------------------------------------------------------*/
double PropertyHollowSphere::getAngle() const
{
    return m_angle;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowSphere::setAngle(const double a)
{
    m_angle=a;
}
/*----------------------------------------------------------------------------*/
void PropertyHollowSphere::addDescription(Utils::SerializedRepresentation& geomProprietes) const
{
	// type associée à cette propriété
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Type", std::string("sphère creuse")));

	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Centre X", m_center.getX()));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Centre Y", m_center.getY()));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Centre Z", m_center.getZ()));

    geomProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Rayon interne", m_radius_int));

    geomProprietes.addProperty (
                Utils::SerializedRepresentation::Property ("Rayon externe", m_radius_ext));


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
