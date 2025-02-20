/*----------------------------------------------------------------------------*/
/*
 * ExportSTLImplementation.cpp
 *
 *  Created on: 13/2/2017
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Geom/ExportSTLImplementation.h"
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
    BRep_Builder builder;
    TopoDS_Compound compound;
    builder.MakeCompound(compound);	
    auto add = [&](const TopoDS_Shape& sh) { builder.Add(compound, sh); };
	for (auto ge : m_geomEntities)
	    ge->apply(add);
	StlAPI::Write(compound, m_filename.c_str(), true);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
