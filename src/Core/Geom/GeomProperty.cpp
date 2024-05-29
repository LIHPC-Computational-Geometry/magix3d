/*----------------------------------------------------------------------------*/
/** \file GeomProperty.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 18/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Geom/GeomProperty.h"
#include "Geom/PropertyBox.h"
#include "Geom/PropertyCylinder.h"
#include "Geom/PropertyHollowCylinder.h"
#include "Geom/PropertySphere.h"
#include "Geom/PropertyHollowSphere.h"
#include "Geom/PropertyCone.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomProperty::GeomProperty()
{}
/*----------------------------------------------------------------------------*/
GeomProperty::~GeomProperty()
{}
/*----------------------------------------------------------------------------*/
GeomProperty::type GeomProperty::getType() const
{
    return GeomProperty::DEFAULT;
}
/*----------------------------------------------------------------------------*/
std::string GeomProperty::typeToString(type& t)
{
	if (t == DEFAULT)
		return "indéfini";
	else if (t == BOX)
		return "boîte";
	else if (t == CYLINDER)
		return "cylindre";
	else if (t == CONE)
		return "cône";
	else if (t == SPHERE)
		return "sphère pleine";
	else if (t == PRISM)
		return "prisme";
	else if (t == REVOL)
		return "objet de révolution";
	else if (t == HOLLOW_CYLINDER)
		return "cylindre creux";
	else if (t == HOLLOW_SPHERE)
		return "sphère creuse";
	else if (t == SPHERE_PART)
		return "aiguille pleine";
	else if (t == HOLLOW_SPHERE_PART)
		return "aiguille creuse";
	else
		return "cas non prévu";
}
/*----------------------------------------------------------------------------*/
PropertyBox* GeomProperty::toPropertyBox()
{
    return dynamic_cast<PropertyBox*>(this);
}
/*----------------------------------------------------------------------------*/
PropertyCylinder* GeomProperty::toPropertyCylinder()
{
    return dynamic_cast<PropertyCylinder*>(this);
}
/*----------------------------------------------------------------------------*/
PropertyHollowCylinder* GeomProperty::toPropertyHollowCylinder()
{
    return dynamic_cast<PropertyHollowCylinder*>(this);
}
/*----------------------------------------------------------------------------*/
PropertyCone* GeomProperty::toPropertyCone()
{
    return dynamic_cast<PropertyCone*>(this);
}
/*----------------------------------------------------------------------------*/
PropertySphere* GeomProperty::toPropertySphere()
{
    return dynamic_cast<PropertySphere*>(this);
}
/*----------------------------------------------------------------------------*/
PropertyHollowSphere* GeomProperty::toPropertyHollowSphere()
{
    return dynamic_cast<PropertyHollowSphere*>(this);
}
/*----------------------------------------------------------------------------*/
void GeomProperty::addDescription(Utils::SerializedRepresentation& geomProprietes) const
{
	// type associée à cette propriété
	geomProprietes.addProperty (
			Utils::SerializedRepresentation::Property ("Type", std::string("quelconque")));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
