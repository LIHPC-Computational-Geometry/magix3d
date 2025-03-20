/*----------------------------------------------------------------------------*/
/** \file CommandExportBlocksForCGNS.cpp
 *
 *  \author legoff
 *
 *  \date 17/04/2014
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Utils/Common.h"
#include "Mesh/CommandExportBlocksForCGNS.h"
#include "Geom/GeomManager.h"
#include "Geom/ExportVTKImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
    namespace Mesh {
/*----------------------------------------------------------------------------*/
        CommandExportBlocksForCGNS::
        CommandExportBlocksForCGNS(Internal::Context& context, const std::string& fileName)
                : Internal::CommandInternal(context, "Export VTK total (géométrie)")
                , m_impl(context,fileName)
        {
        }

/*----------------------------------------------------------------------------*/
        CommandExportBlocksForCGNS::~CommandExportBlocksForCGNS()
        {
        }
/*----------------------------------------------------------------------------*/
        void CommandExportBlocksForCGNS::
        internalExecute()
        {
            // écriture du VTK
            m_impl.perform(&getInfoCommand());
        }
/*----------------------------------------------------------------------------*/
    } // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
