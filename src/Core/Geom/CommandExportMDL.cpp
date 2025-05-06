#ifdef USE_MDLPARSER
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/ExportMDLImplementation.h"
#include "Utils/Common.h"
#include "Geom/CommandExportMDL.h"
#include "Geom/GeomManager.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {

/*----------------------------------------------------------------------------*/
CommandExportMDL::
CommandExportMDL(Internal::Context& c, Internal::ExportMDLImplementation* impl)
: Internal::CommandInternal(c, "Export Mdl (géométrie)")
, m_impl(impl)
{
}
/*----------------------------------------------------------------------------*/
CommandExportMDL::~CommandExportMDL()
{
}
/*----------------------------------------------------------------------------*/
void CommandExportMDL::
internalExecute()
{
    // écriture du Mdl
    m_impl->perform(&getInfoCommand());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
#endif  // USE_MDLPARSER
/*----------------------------------------------------------------------------*/
