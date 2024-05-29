/*----------------------------------------------------------------------------*/
/** \file PropertyCone.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5/5/2014
 */
/*----------------------------------------------------------------------------*/
#include "Geom/PropertyCone.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
PropertyCone::PropertyCone(
        const double radius1, const double radius2, const double height,
        const Utils::Math::Vector& axis,
        const double angle )
: GeomProperty(), m_radius1(radius1), m_radius2(radius2), m_height(height),
  m_axis(axis), m_angle(angle)
{}
/*----------------------------------------------------------------------------*/
PropertyCone::~PropertyCone()
{}
/*----------------------------------------------------------------------------*/
GeomProperty::type PropertyCone::getType() const
{
    return GeomProperty::CONE;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Vector PropertyCone::getAxis() const
{
    return m_axis;
}
/*----------------------------------------------------------------------------*/
double PropertyCone::getRadius1() const
{
    return m_radius1;
}
/*----------------------------------------------------------------------------*/
double PropertyCone::getRadius2() const
{
    return m_radius2;
}
/*----------------------------------------------------------------------------*/
double PropertyCone::getHeight() const
{
    return m_height;
}
/*----------------------------------------------------------------------------*/
double PropertyCone::getAngle() const
{
    return m_angle;
}
/*----------------------------------------------------------------------------*/
void PropertyCone::setAxis(const Utils::Math::Vector& axis)
{
    m_axis = axis;
}
/*----------------------------------------------------------------------------*/
void PropertyCone::setRadius1(const double radius)
{
    if(radius>0)
        m_radius1 = radius;
}
/*----------------------------------------------------------------------------*/
void PropertyCone::setRadius2(const double radius)
{
    if(radius>0)
        m_radius2 = radius;
}
/*----------------------------------------------------------------------------*/
void PropertyCone::setHeight(const double height)
{
    if(height>0)
        m_height = height;
}
/*----------------------------------------------------------------------------*/
void PropertyCone::setAngle(const double angle)
{
    if(angle>0)
        m_angle = angle;
}
/*----------------------------------------------------------------------------*/
void PropertyCone::addDescription(Utils::SerializedRepresentation& geomProprietes) const
{
	// type associée à cette propriété
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Type", std::string("cône")));

	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Rayon côté sommet", m_radius1));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Rayon côté base", m_radius2));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Hauteur", m_height));


	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Angle", m_angle));

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
