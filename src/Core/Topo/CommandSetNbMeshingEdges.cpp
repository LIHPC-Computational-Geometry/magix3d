/*----------------------------------------------------------------------------*/
/*
 * \file CommandSetNbMeshingEdges.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/11/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSetNbMeshingEdges.h"
#include "Topo/SetNbMeshingEdgesImplementation.h"
#include "Topo/CoFace.h"
#include "Topo/CoEdge.h"

#include "Utils/Common.h"
#include "Topo/TopoHelper.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandSetNbMeshingEdges::
CommandSetNbMeshingEdges(Internal::Context& c, CoEdge* ed, int nb, std::vector<CoEdge*>& frozed_coedges)
:CommandEditTopo(c, std::string("Changement de discrétisation pour ")
                    + ed->getName() + " avec propagation")
, m_coedge(ed)
, m_nb_meshing_edges(nb)
, m_frozed_coedges(frozed_coedges)
{
    if (m_nb_meshing_edges<=0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le nombre de bras doit être au moins de 1", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandSetNbMeshingEdges::
~CommandSetNbMeshingEdges()
{
}
/*----------------------------------------------------------------------------*/
void CommandSetNbMeshingEdges::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSetNbMeshingEdges::execute pour la commande " << getName ( )
                    << " de nom unique " << getUniqueName ( );

    SetNbMeshingEdgesImplementation implementation(&getInfoCommand());

    implementation.addFrozen(m_frozed_coedges);
    implementation.addDelta(m_coedge, m_nb_meshing_edges-m_coedge->getNbMeshingEdges());

    implementation.execute();


    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandSetNbMeshingEdges::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
