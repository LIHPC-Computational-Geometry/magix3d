/*----------------------------------------------------------------------------*/
#include "Geom/CommandExportBREP.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandExportBREP::
CommandExportBREP(Internal::Context& context, const std::string& fileName)
: Internal::CommandInternal(context, "Export BREP total (géométrie)")
, m_impl(context,fileName)
{
}
/*----------------------------------------------------------------------------*/
CommandExportBREP::
CommandExportBREP(Internal::Context& context,
		const std::vector<GeomEntity*>& geomEntities,
		const std::string& fileName)
: Internal::CommandInternal(context, "Export BREP sélection (géométrie)")
, m_impl(context,geomEntities,fileName)
{
}
/*----------------------------------------------------------------------------*/
CommandExportBREP::~CommandExportBREP()
{
}
/*----------------------------------------------------------------------------*/
void CommandExportBREP::
internalExecute()
{
    // écriture du BREP
    m_impl.perform(&getInfoCommand());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
