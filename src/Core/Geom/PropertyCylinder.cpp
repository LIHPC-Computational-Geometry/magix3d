/*----------------------------------------------------------------------------*/
/** \file PropertyCylinder.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 09/11/2010
 */
/*----------------------------------------------------------------------------*/
#include "Geom/PropertyCylinder.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
PropertyCylinder::PropertyCylinder(
        const double radius, const double height,
        const Utils::Math::Point& center, const Utils::Math::Vector& axis,
        const double angle )
: GeomProperty(), m_radius(radius), m_height(height),
  m_center(center), m_axis(axis), m_angle(angle)
{}
/*----------------------------------------------------------------------------*/
PropertyCylinder::~PropertyCylinder()
{}
/*----------------------------------------------------------------------------*/
GeomProperty::type PropertyCylinder::getType() const
{
    return GeomProperty::CYLINDER;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point PropertyCylinder::getCenter() const
{
    return m_center;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Vector PropertyCylinder::getAxis() const
{
    return m_axis;
}
/*----------------------------------------------------------------------------*/
double PropertyCylinder::getRadius() const
{
    return m_radius;
}
/*----------------------------------------------------------------------------*/
double PropertyCylinder::getHeight() const
{
    return m_height;
}
/*----------------------------------------------------------------------------*/
double PropertyCylinder::getAngle() const
{
    return m_angle;
}
/*----------------------------------------------------------------------------*/
void PropertyCylinder::setCenter(const Utils::Math::Point& center)
{
    m_center = center;
}
/*----------------------------------------------------------------------------*/
void PropertyCylinder::setAxis(const Utils::Math::Vector& axis)
{
    m_axis = axis;
}
/*----------------------------------------------------------------------------*/
void PropertyCylinder::setRadius(const double radius)
{
    if(radius>0)
        m_radius = radius;
}
/*----------------------------------------------------------------------------*/
void PropertyCylinder::setHeight(const double height)
{
    if(height>0)
        m_height = height;
}
/*----------------------------------------------------------------------------*/
void PropertyCylinder::setAngle(const double angle)
{
    if(angle>0)
        m_angle = angle;
}
/*----------------------------------------------------------------------------*/
void PropertyCylinder::addDescription(Utils::SerializedRepresentation& geomProprietes) const
{
	// type associée à cette propriété
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Type", std::string("cylindre plein")));

	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Rayon", m_radius));
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
