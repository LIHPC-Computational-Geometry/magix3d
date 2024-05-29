/*----------------------------------------------------------------------------*/
/** \file CommandChangeDefaultNbMeshingEdges.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 8/11/2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/InfoCommand.h"
#include "Internal/Context.h"
#include "Topo/TopoManager.h"
#include "Topo/CommandChangeDefaultNbMeshingEdges.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandChangeDefaultNbMeshingEdges::CommandChangeDefaultNbMeshingEdges(Internal::Context& c, int nb)
: Internal::CommandInternal (c, "Changement du nombre de bras par défaut")
, m_new_nb_meshing_edges(nb)
{
    if (nb <= 0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le nombre de bras doit être d'au moins 1", TkUtil::Charset::UTF_8));

    m_old_nb_meshing_edges = getTopoManager().getDefaultNbMeshingEdges();
}
/*----------------------------------------------------------------------------*/
CommandChangeDefaultNbMeshingEdges::~CommandChangeDefaultNbMeshingEdges()
{
}
/*----------------------------------------------------------------------------*/
TopoManager& CommandChangeDefaultNbMeshingEdges::getTopoManager()
{
    return getContext().getLocalTopoManager();
}
/*----------------------------------------------------------------------------*/
void CommandChangeDefaultNbMeshingEdges::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandChangeDefaultNbMeshingEdges::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    getTopoManager().m_defaultNbMeshingEdges = m_new_nb_meshing_edges;

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandChangeDefaultNbMeshingEdges::
internalUndo()
{
    TkUtil::AutoReferencedMutex	autoMutex (getMutex ( ));

    getTopoManager().m_defaultNbMeshingEdges = m_old_nb_meshing_edges;

}
/*----------------------------------------------------------------------------*/
void CommandChangeDefaultNbMeshingEdges::
internalRedo()
{
    TkUtil::AutoReferencedMutex	autoMutex (getMutex ( ));

    getTopoManager().m_defaultNbMeshingEdges = m_new_nb_meshing_edges;
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
