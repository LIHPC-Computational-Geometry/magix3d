/*----------------------------------------------------------------------------*/
/** \file PropertyBox.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 18/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Geom/PropertyBox.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
PropertyBox::PropertyBox(const Utils::Math::Point& pnt,
                         const double& dx, const double& dy, const double& dz)
: GeomProperty(), m_pnt(pnt),m_x(dx),m_y(dy),m_z(dz)
{}
/*----------------------------------------------------------------------------*/
PropertyBox::~PropertyBox()
{}
/*----------------------------------------------------------------------------*/
GeomProperty::type PropertyBox::getType() const
{
    return GeomProperty::BOX;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point PropertyBox::getPnt() const
{
    return m_pnt;
}
/*----------------------------------------------------------------------------*/
double PropertyBox::getX() const
{
    return m_x;
}
/*----------------------------------------------------------------------------*/
double PropertyBox::getY() const
{
    return m_y;
}
/*----------------------------------------------------------------------------*/
double PropertyBox::getZ() const
{
    return m_z;
}
/*----------------------------------------------------------------------------*/
void PropertyBox::setPnt(Utils::Math::Point& pnt)
{
    m_pnt = pnt;
}
/*----------------------------------------------------------------------------*/
void PropertyBox::setX(const double& dx)
{
    if(dx>0)
        m_x = dx;
}
/*----------------------------------------------------------------------------*/
void PropertyBox::setY(const double& dy)
{
    if(dy>0)
        m_y = dy;
}
/*----------------------------------------------------------------------------*/
void PropertyBox::setZ(const double& dz)
{
    if(dz>0)
        m_z = dz;
}
/*----------------------------------------------------------------------------*/
void PropertyBox::addDescription(Utils::SerializedRepresentation& geomProprietes) const
{
	// type associée à cette propriété
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Type", std::string("boîte")));

	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("X min", m_pnt.getX()));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Y min", m_pnt.getY()));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Z min", m_pnt.getZ()));

	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Longueur en X", m_x));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Longueur en Y", m_y));
	geomProprietes.addProperty (
	            Utils::SerializedRepresentation::Property ("Longueur en Z", m_z));

}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
