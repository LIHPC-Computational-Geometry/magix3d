/*----------------------------------------------------------------------------*/
#include "Geom/ExportSTEPImplementation.h"
#include "Geom/Volume.h"
#include "Geom/Curve.h"
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
    auto add = [&](const TopoDS_Shape& sh) { m_writer.Transfer(sh, STEPControl_AsIs); };
    geomEntity->apply(add);
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
