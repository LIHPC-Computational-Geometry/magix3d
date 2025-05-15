/*----------------------------------------------------------------------------*/
#include "Geom/CommandExportIGES.h"
#include "Geom/ExportIGESImplementation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandExportIGES::
CommandExportIGES(Internal::Context& context, const std::string& fileName)
: Internal::CommandInternal(context, "Export IGES total (géométrie)"), m_impl(context,fileName)
{
}
/*----------------------------------------------------------------------------*/
CommandExportIGES::CommandExportIGES(Internal::Context& context,
    		const std::vector<GeomEntity*>& geomEntities,
    		const std::string &fileName)
: Internal::CommandInternal(context, "Export IGES sélection (géométrie)"),
  m_impl(context,geomEntities,fileName)
{
}
/*----------------------------------------------------------------------------*/
CommandExportIGES::~CommandExportIGES()
{
}
/*----------------------------------------------------------------------------*/
void CommandExportIGES::
internalExecute()
{
    // écriture du IGES
    m_impl.perform(&getInfoCommand());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
