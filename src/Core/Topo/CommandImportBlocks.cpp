//
// Created by calderans on 4/3/25.
//

#include "Topo/CommandImportBlocks.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
    namespace Topo {

/*----------------------------------------------------------------------------*/
        CommandImportBlocks::
        CommandImportBlocks(Internal::Context& c, const std::string& n)
                : CommandCreateTopo(c, "Import (Topo) Mdl"), m_filename(n)
        {
            m_impl = new ImportBlocksImplementation(c, &getInfoCommand(), m_filename);
        }
/*----------------------------------------------------------------------------*/
        CommandImportBlocks::~CommandImportBlocks()
        {
            if(m_impl)
                delete m_impl;
        }
/*----------------------------------------------------------------------------*/
        void CommandImportBlocks::
        internalExecute()
        {
            m_impl->internalExecute();
        }
/*----------------------------------------------------------------------------*/
        void CommandImportBlocks::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
        {
            MGX_FORBIDDEN("getPreviewRepresentation non prévu");
        }
/*----------------------------------------------------------------------------*/
    } // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/