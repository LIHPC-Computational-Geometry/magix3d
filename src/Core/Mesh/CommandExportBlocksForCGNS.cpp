/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Mesh/CommandExportBlocksForCGNS.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
    namespace Mesh {
/*----------------------------------------------------------------------------*/
        CommandExportBlocksForCGNS::
        CommandExportBlocksForCGNS(Internal::Context& context, int dim, const std::string& fileName)
                : Internal::CommandInternal(context, "Export CGNS total")
                , m_impl(context,fileName, dim)
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
            // écriture du CGNS
            m_impl.perform(&getInfoCommand());
        }
/*----------------------------------------------------------------------------*/
    } // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
