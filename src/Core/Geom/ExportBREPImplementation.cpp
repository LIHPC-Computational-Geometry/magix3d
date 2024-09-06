/*----------------------------------------------------------------------------*/
/*
 * ExportBREPImplementation.cpp
 *
 *  Created on: 23 aout 2024
 *      Author: lelandaisb
 */
/*----------------------------------------------------------------------------*/
#include "Geom/ExportBREPImplementation.h"
#include "Geom/OCCGeomRepresentation.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <TkUtil/MemoryError.h>
#include <BRepTools.hxx>
#include <Interface_Static.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ExportBREPImplementation::
ExportBREPImplementation(Internal::Context& c, const std::string& n)
: GeomExport(c, n)
{
    m_builder.MakeCompound(m_compound);
}
/*----------------------------------------------------------------------------*/
ExportBREPImplementation::
ExportBREPImplementation(Internal::Context& c,
		const std::vector<GeomEntity*>& geomEntities,
		const std::string& n)
: GeomExport(c, geomEntities, n)
{
    m_builder.MakeCompound(m_compound);
}
/*----------------------------------------------------------------------------*/
ExportBREPImplementation::~ExportBREPImplementation()
{}
/*----------------------------------------------------------------------------*/
void ExportBREPImplementation::
addGeomEntityToExport(GeomEntity* geomEntity)
{
	std::vector<GeomRepresentation*> reps = geomEntity->getComputationalProperties();
	for (uint i=0; i<reps.size(); i++){
		OCCGeomRepresentation* geom_rep =
		            dynamic_cast<OCCGeomRepresentation*>(reps[i]);
		CHECK_NULL_PTR_ERROR(geom_rep);
        m_builder.Add(m_compound, geom_rep->getShape());
	}
}
/*----------------------------------------------------------------------------*/
void ExportBREPImplementation::write()
{
    std::ofstream file(m_filename, ios::out);
    if (file.bad()) {
        throw TkUtil::Exception (TkUtil::UTF8String ("Impossible d'écrire ce fichier BREP", TkUtil::Charset::UTF_8));
    }
    BRepTools::Write(m_compound, file);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
