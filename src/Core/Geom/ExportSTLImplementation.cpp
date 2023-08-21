/*----------------------------------------------------------------------------*/
/*
 * ExportSTLImplementation.cpp
 *
 *  Created on: 13/2/2017
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include <Geom/ExportSTLImplementation.h>
#include <Geom/OCCGeomRepresentation.h>
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


	GeomRepresentation* rep = m_geomEntities[0]->getComputationalProperty();
	OCCGeomRepresentation* geom_rep =
			            dynamic_cast<OCCGeomRepresentation*>(rep);
	CHECK_NULL_PTR_ERROR(geom_rep);

	 StlAPI_ErrorStatus err = StlAPI::Write(geom_rep->getShape(), m_filename.c_str(), true);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
