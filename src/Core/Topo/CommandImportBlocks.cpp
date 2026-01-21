//
// Created by calderans on 4/3/25.
//

#include "Topo/CommandImportBlocks.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
    namespace Topo {

/*----------------------------------------------------------------------------*/
        CommandImportBlocks::
        CommandImportBlocks(Internal::Context& c, const std::string& n, bool withGeom)
                : CommandCreateTopo(c, "Import (Topo) Mdl"), m_filename(n)
        {
            m_impl = new ImportBlocksImplementation(c, &getInfoCommand(), m_filename, withGeom);
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

            if(!m_impl->getWarning().empty())
                m_warning_to_pop_up = "L'association géométrique pour les entités "+m_impl->getWarning()+"n'a pas pu etre effectuée.";

            for(auto te : getInfoCommand().getTopoInfoEntity()) {
                if (te.first->getType() == Utils::Entity::TopoVertex) {
                    getTopoManager().add((Vertex *) te.first);
                } else if (te.first->getType() == Utils::Entity::TopoCoEdge) {
                    getTopoManager().add((CoEdge *) te.first);
                }else if (te.first->getType() == Utils::Entity::TopoEdge) {
                    getTopoManager().add((Edge *) te.first);
                }else if (te.first->getType() == Utils::Entity::TopoCoFace) {
                    getTopoManager().add((CoFace *) te.first);
                }else if (te.first->getType() == Utils::Entity::TopoFace) {
                    getTopoManager().add((Face *) te.first);
                } else if (te.first->getType() == Utils::Entity::TopoBlock) {
                    getTopoManager().add((Block *) te.first);
                }
            }
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