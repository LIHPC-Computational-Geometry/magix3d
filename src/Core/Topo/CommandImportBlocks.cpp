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


            for(auto te : getInfoCommand().getTopoInfoEntity()){
                if(te.first->getDim() == 0){
                    getTopoManager().add((Vertex*)te.first);
                }else if(te.first->getDim() == 1){
                    getTopoManager().add((CoEdge*)te.first);
                }else if(te.first->getDim() == 2){
                    getTopoManager().add((CoFace*)te.first);
                }else if(te.first->getDim() == 3){
                    getTopoManager().add((Block*)te.first);
                }
            }

        }
/*----------------------------------------------------------------------------*/
    } // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/