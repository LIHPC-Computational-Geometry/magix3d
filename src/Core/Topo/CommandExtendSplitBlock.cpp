/*----------------------------------------------------------------------------*/
/*
 * \file CommandExtendSplitBlock.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/3/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandExtendSplitBlock.h"

#include "Utils/Common.h"
#include "Topo/Block.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandExtendSplitBlock::
CommandExtendSplitBlock(Internal::Context& c, Block* bloc, CoEdge* arete)
:CommandEditTopo(c, std::string("Découpage du bloc ") + bloc->getName()+ " par prolongation ")
, m_bloc(bloc)
, m_arete(arete)
{
}
/*----------------------------------------------------------------------------*/
CommandExtendSplitBlock::
~CommandExtendSplitBlock()
{
}
/*----------------------------------------------------------------------------*/
void CommandExtendSplitBlock::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandExtendSplitBlock::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    if (!m_bloc->isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Le bloc doit être structuré pour être découpé avec cette méthode", TkUtil::Charset::UTF_8));

    // Les blocs créés (non utilisé)
    std::vector<Block* > newBlocks;

    // la modification de la topologie
    m_bloc->extendSplit(m_arete, newBlocks, &getInfoCommand());

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandExtendSplitBlock::
countNbCoEdgesByVertices(std::map<Topo::Vertex*, uint> &nb_coedges_by_vertex)
{
	// stocke pour chacun des sommets le nombre d'arêtes auxquelles il est relié
	std::vector<Topo::Vertex* > all_vertices;
	m_bloc->getAllVertices(all_vertices);
	for (uint i=0; i<all_vertices.size(); i++)
		nb_coedges_by_vertex[all_vertices[i]] = all_vertices[i]->getNbCoEdges();
}
/*----------------------------------------------------------------------------*/
void CommandExtendSplitBlock::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

