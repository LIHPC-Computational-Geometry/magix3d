/*----------------------------------------------------------------------------*/
#include "Geom/ExportBREPImplementation.h"
/*----------------------------------------------------------------------------*/
#include <ios>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
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
    auto add = [&](const TopoDS_Shape& sh) { m_builder.Add(m_compound, sh); };
    geomEntity->apply(add);
}
/*----------------------------------------------------------------------------*/
void ExportBREPImplementation::write()
{
    std::ofstream file(m_filename, std::ios::out);
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
