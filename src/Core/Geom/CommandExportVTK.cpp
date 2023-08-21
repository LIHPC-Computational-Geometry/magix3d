/*----------------------------------------------------------------------------*/
/** \file CommandExportVTK.cpp
 *
 *  \author legoff
 *
 *  \date 17/04/2014
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Utils/Common.h"
#include <Geom/CommandExportVTK.h>
#include "Geom/GeomManager.h"
#include "Geom/ExportVTKImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandExportVTK::
CommandExportVTK(Internal::Context& context, const std::string& fileName)
: Internal::CommandInternal(context, "Export VTK total (géométrie)")
, m_impl(context,fileName)
{
}
/*----------------------------------------------------------------------------*/
CommandExportVTK::
CommandExportVTK(Internal::Context& context,
		const std::vector<GeomEntity*>& geomEntities,
		const std::string& fileName)
: Internal::CommandInternal(context, "Export VTK sélection (géométrie)")
, m_impl(context,geomEntities,fileName)
{
}
/*----------------------------------------------------------------------------*/
CommandExportVTK::~CommandExportVTK()
{
}
/*----------------------------------------------------------------------------*/
void CommandExportVTK::
internalExecute()
{
    // écriture du VTK
    m_impl.perform(&getInfoCommand());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
