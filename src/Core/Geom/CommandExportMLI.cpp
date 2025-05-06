/*----------------------------------------------------------------------------*/
#include "Geom/CommandExportMLI.h"
#include "Geom/ExportMLIImplementation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandExportMLI::
CommandExportMLI(Internal::Context& context, const std::string& fileName)
: Internal::CommandInternal(context, "Export Mli total (géométrie)")
, m_impl(context,fileName)
{
}
/*----------------------------------------------------------------------------*/
CommandExportMLI::
CommandExportMLI(Internal::Context& context,
		const std::vector<GeomEntity*>& geomEntities,
		const std::string& fileName)
: Internal::CommandInternal(context, "Export Mli sélection (géométrie)")
, m_impl(context,geomEntities,fileName)
{
}
/*----------------------------------------------------------------------------*/
CommandExportMLI::~CommandExportMLI()
{
}
/*----------------------------------------------------------------------------*/
void CommandExportMLI::
internalExecute()
{
    // écriture du Mli
    m_impl.perform(&getInfoCommand());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
