/*----------------------------------------------------------------------------*/
/*
 * CommandExportSTEP.cpp
 *
 *  Created on: 14 févr. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include <Geom/CommandExportSTEP.h>
#include "Geom/GeomManager.h"
#include "Geom/ExportSTEPImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandExportSTEP::
CommandExportSTEP(Internal::Context& context, const std::string& fileName)
: Internal::CommandInternal(context, "Export STEP total (géométrie)")
, m_impl(context,fileName)
{
}
/*----------------------------------------------------------------------------*/
CommandExportSTEP::
CommandExportSTEP(Internal::Context& context,
		const std::vector<GeomEntity*>& geomEntities,
		const std::string& fileName)
: Internal::CommandInternal(context, "Export STEP sélection (géométrie)")
, m_impl(context,geomEntities,fileName)
{
}
/*----------------------------------------------------------------------------*/
CommandExportSTEP::~CommandExportSTEP()
{
}
/*----------------------------------------------------------------------------*/
void CommandExportSTEP::
internalExecute()
{
    // écriture du STEP
    m_impl.perform(&getInfoCommand());
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
