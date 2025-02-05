/*----------------------------------------------------------------------------*/
/*
 * ExportSTLImplementation.cpp
 *
 *  Created on: 13/2/2017
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Geom/ExportSTLImplementation.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/GeomEntity.h"
#include "Utils/Entity.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <StlAPI.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ExportSTLImplementation::
ExportSTLImplementation(Internal::Context& c,
		GeomEntity* geomEntity,
		const std::string& n)
: GeomExport(c,geomEntity,n)
{
	if (geomEntity->getType() != Utils::Entity::GeomSurface){
		throw TkUtil::Exception (TkUtil::UTF8String ("ExportSTLImplementation ne peut se faire actuellement que pour une surface", TkUtil::Charset::UTF_8));
	}
}
/*----------------------------------------------------------------------------*/
ExportSTLImplementation::~ExportSTLImplementation()
{}
/*----------------------------------------------------------------------------*/
void ExportSTLImplementation::write()
{
	if (m_geomEntities.size() != 1)
		throw TkUtil::Exception (TkUtil::UTF8String ("ExportSTLImplementation ne peut fonctionner qu'avec une unique entité", TkUtil::Charset::UTF_8));


	std::vector<GeomRepresentation*> reps = m_geomEntities[0]->getComputationalProperties();
	if (reps.size() == 1) {
		OCCGeomRepresentation* gr = dynamic_cast<OCCGeomRepresentation*>(reps[0]);
		CHECK_NULL_PTR_ERROR(gr);
		StlAPI::Write(gr->getShape(), m_filename.c_str(), true);
	} else if (reps.size() > 1) {
		TopoDS_Compound compound = combineShapes(reps);
		StlAPI::Write(compound, m_filename.c_str(), true);
	}
}

/*----------------------------------------------------------------------------*/
TopoDS_Compound ExportSTLImplementation::combineShapes(const std::vector<GeomRepresentation*>& reps)
{
    BRep_Builder builder;
    TopoDS_Compound compound;
    builder.MakeCompound(compound);

    for (GeomRepresentation* rep : reps) {
		OCCGeomRepresentation* gr = dynamic_cast<OCCGeomRepresentation*>(rep);
		CHECK_NULL_PTR_ERROR(gr);
        builder.Add(compound, gr->getShape());
    }

    return compound;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
