/*----------------------------------------------------------------------------*/
#include "Geom/ExportBREPImplementation.h"
#include "Geom/IncidentGeomEntitiesVisitor.h"
/*----------------------------------------------------------------------------*/
#include <ios>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <BRepTools.hxx>
#include <TopExp.hxx>
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


    GetDownIncidentGeomEntitiesVisitor v;
    for (GeomEntity* ge : m_geomEntities)
        ge->accept(v);

    TopTools_IndexedMapOfShape map;
    TopExp::MapShapes(m_compound, map);
    for (GeomEntity* ge : v.get())
    {
        std::vector<int> brep_indices;
        auto add = [&](const TopoDS_Shape& sh) { brep_indices.push_back(map.FindIndex(sh)); };
        ge->apply(add);
        m_geom_id_2_brep_indices[ge->getName()] = brep_indices;
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
