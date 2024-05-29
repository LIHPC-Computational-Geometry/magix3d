/*----------------------------------------------------------------------------*/
/*
 * ExportIGESImplementation.cpp
 *
 *  Created on: 14 févr. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include <Geom/ExportIGESImplementation.h>
#include <Geom/OCCGeomRepresentation.h>
#include <Geom/Volume.h>
#include <Geom/Surface.h>
#include <Geom/Curve.h>
#include <Geom/Vertex.h>
#include <Utils/Unit.h>
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <IGESControl_Writer.hxx>
#include <IGESControl_Controller.hxx>
#include <Interface_Static.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
    ExportIGESImplementation::
    ExportIGESImplementation(Internal::Context &c, const std::string &n)
            : GeomExport(c, n) {
        IGESControl_Controller::Init();
        init();
        m_writer = new IGESControl_Writer();
    }

/*----------------------------------------------------------------------------*/
    ExportIGESImplementation::
    ExportIGESImplementation(Internal::Context &c,
                             const std::vector<GeomEntity *> &geomEntities,
                             const std::string &n)
            : GeomExport(c, geomEntities, n) {
        IGESControl_Controller::Init();
        init();
        m_writer = new IGESControl_Writer();
    }
/*----------------------------------------------------------------------------*/
void ExportIGESImplementation::init()
{
   switch (m_context.getLengthUnit ( ))
    {
        case Utils::Unit::meter		:
            Interface_Static::SetCVal ("write.iges.unit", "M");
            break;
        case Utils::Unit::centimeter	:
            Interface_Static::SetCVal ("write.iges.unit", "CM");
            break;
        case Utils::Unit::milimeter	:
            Interface_Static::SetCVal ("write.iges.unit", "MM");
            break;
        case Utils::Unit::micron		:
            Interface_Static::SetCVal ("write.iges.unit", "UM");
            break;
        default					:
            ; // le défaut est le MM
    }	// switch (getLengthUnit ( ))

    // n'est pas pris en compte !
    Interface_Static::SetCVal ("write.iges.header.product", "Magix3D"); //MAGIX3D_VERSION
}
/*----------------------------------------------------------------------------*/
ExportIGESImplementation::~ExportIGESImplementation()
{
    if(m_writer)
        delete m_writer;
}
/*----------------------------------------------------------------------------*/
void ExportIGESImplementation::
addGeomEntityToExport(GeomEntity* geomEntity)
{
	std::vector<GeomRepresentation*> reps = geomEntity->getComputationalProperties();
	for (uint i=0; i<reps.size(); i++){
		OCCGeomRepresentation* geom_rep =
		            dynamic_cast<OCCGeomRepresentation*>(reps[i]);
		CHECK_NULL_PTR_ERROR(geom_rep);
		m_writer->AddShape(geom_rep->getShape());
	}
}
/*----------------------------------------------------------------------------*/
void ExportIGESImplementation::write()
{
     m_writer->Write(m_filename.c_str());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
