/*----------------------------------------------------------------------------*/
#ifdef USE_MDLPARSER
/** \file CommandExportMDL.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 28/6/2013
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Geom/CommandExportMDL.h"
#include "Geom/GeomManager.h"
#include "Internal/ExportMDLImplementation.h"
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
