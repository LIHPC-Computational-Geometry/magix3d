/*----------------------------------------------------------------------------*/
/*
 * ExportSTEPImplementation.cpp
 *
 *  Created on: 14 févr. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include <Geom/ExportSTEPImplementation.h>
#include <Geom/OCCGeomRepresentation.h>
#include <Geom/Volume.h>
#include <Geom/Surface.h>
#include <Geom/Curve.h>
#include <Geom/Vertex.h>
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <STEPControl_Writer.hxx>
#include <Interface_Static.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
ExportSTEPImplementation::
ExportSTEPImplementation(Internal::Context& c, const std::string& n)
: GeomExport(c,n)
{
    init();
}
/*----------------------------------------------------------------------------*/
ExportSTEPImplementation::
ExportSTEPImplementation(Internal::Context& c,
		const std::vector<GeomEntity*>& geomEntities,
		const std::string& n)
: GeomExport(c,geomEntities,n)
{
    init();
}
/*----------------------------------------------------------------------------*/
void ExportSTEPImplementation::init()
{
   switch (m_context.getLengthUnit ( ))
    {
        case Utils::Unit::meter		:
            Interface_Static::SetCVal ("write.step.unit", "M");
            break;
        case Utils::Unit::centimeter	:
            Interface_Static::SetCVal ("write.step.unit", "CM");
            break;
        case Utils::Unit::milimeter	:
            Interface_Static::SetCVal ("write.step.unit", "MM");
            break;
        case Utils::Unit::micron		:
            Interface_Static::SetCVal ("write.step.unit", "UM");
            break;
        default					:
            ; // le défaut est le MM
    }	// switch (getLengthUnit ( ))

    // n'est pas pris en compte !
    Interface_Static::SetCVal ("write.step.header.product", "Magix3D"); //MAGIX3D_VERSION
}
/*----------------------------------------------------------------------------*/
ExportSTEPImplementation::~ExportSTEPImplementation()
{}
/*----------------------------------------------------------------------------*/
void ExportSTEPImplementation::
addGeomEntityToExport(GeomEntity* geomEntity)
{
	std::vector<GeomRepresentation*> reps = geomEntity->getComputationalProperties();
	for (uint i=0; i<reps.size(); i++){
		OCCGeomRepresentation* geom_rep =
		            dynamic_cast<OCCGeomRepresentation*>(reps[i]);
		CHECK_NULL_PTR_ERROR(geom_rep);
		m_writer.Transfer(geom_rep->getShape(), STEPControl_AsIs);
	}
}
/*----------------------------------------------------------------------------*/
void ExportSTEPImplementation::write()
{
    m_writer.Write(m_filename.c_str());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
