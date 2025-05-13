/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Mesh/CommandExportBlocksForCGNS.h"
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
