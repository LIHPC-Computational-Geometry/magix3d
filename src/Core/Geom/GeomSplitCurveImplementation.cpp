/*----------------------------------------------------------------------------*/
/*
 * GeomSplitCurveImplementation.cpp
 *
 *  Created on: 12/4/2018
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Geom/GeomSplitCurveImplementation.h"
#include "Geom/Curve.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomSplitCurveImplementation::
GeomSplitCurveImplementation(Internal::Context& c, Curve* crv, const Point& pt)
:GeomModificationBaseClass(c)
, m_entity_param(crv)
, m_pt(pt)
{
}
/*----------------------------------------------------------------------------*/
GeomSplitCurveImplementation::~GeomSplitCurveImplementation()
{
}
/*----------------------------------------------------------------------------*/
void GeomSplitCurveImplementation::prePerform()
{
    // Mise à jour des connectivés de références
	std::vector<GeomEntity*> entities_param;
	entities_param.push_back(m_entity_param);
    init(entities_param);
}
/*----------------------------------------------------------------------------*/
void GeomSplitCurveImplementation::perform(std::vector<GeomEntity*>& res)
{
	GeomRepresentation* gr = m_entity_param->getComputationalProperty();
	OCCGeomRepresentation* occ_gr = dynamic_cast<OCCGeomRepresentation*>(gr);

	if (occ_gr){
		MGX_NOT_YET_IMPLEMENTED("découpage d'une courbe OCC à faire")

		// peut être utiliser la section avec plan orthogonal à la tangente à la courbe
		// La construction d'une courbe à l'aide de Geom_TrimmedCurve ne permettra pas
		// facilement de réutiliser les vertex
	}
	else {
		throw TkUtil::Exception (TkUtil::UTF8String ("type de GeomRepresentation non pris en charge", TkUtil::Charset::UTF_8));
	}
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
