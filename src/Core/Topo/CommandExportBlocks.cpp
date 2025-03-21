//
// Created by calderans on 3/17/25.
//

#include "Topo/CommandExportBlocks.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandExportBlocks::
CommandExportBlocks(Internal::Context &context, const std::string &fileName)
: Internal::CommandInternal(context, "Export des Blocs"), m_impl(context, fileName)
{}
/*----------------------------------------------------------------------------*/
CommandExportBlocks::~CommandExportBlocks()
{}
/*----------------------------------------------------------------------------*/
void CommandExportBlocks::
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