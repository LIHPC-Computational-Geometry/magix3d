/*----------------------------------------------------------------------------*/
/** \file PropertyPrism.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 09/11/2010
 */
/*----------------------------------------------------------------------------*/
#include "Geom/PropertyPrism.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
PropertyPrism::PropertyPrism(GeomEntity* base, const Utils::Math::Vector& axis)
: GeomProperty(), m_base(base), m_axis(axis)
{}
/*----------------------------------------------------------------------------*/
PropertyPrism::~PropertyPrism()
{}
/*----------------------------------------------------------------------------*/
GeomProperty::type PropertyPrism::getType() const
{
    return GeomProperty::PRISM;
}
/*----------------------------------------------------------------------------*/
GeomEntity* PropertyPrism::getBase() const
{
    return m_base;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Vector PropertyPrism::getAxis() const
{
    return m_axis;
}
/*----------------------------------------------------------------------------*/
void PropertyPrism::setAxis(const Utils::Math::Vector& axis)
{
    m_axis = axis;
}
/*----------------------------------------------------------------------------*/
void PropertyPrism::addDescription(Utils::SerializedRepresentation& geomProprietes) const
{
	// type associée à cette propriété
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Type", std::string("prisme")));

	geomProprietes.addProperty (
			Utils::SerializedRepresentation::Property(m_base->getName(), *m_base));

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
