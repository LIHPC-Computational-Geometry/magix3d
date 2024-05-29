/*----------------------------------------------------------------------------*/
/*
 * \file CommandSplitBlock.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/3/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSplitBlock.h"

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
CommandSplitBlock::
CommandSplitBlock(Internal::Context& c, Block* bloc, CoEdge* arete, double ratio)
:CommandEditTopo(c, std::string("Découpage du bloc ") + bloc->getName())
, m_bloc(bloc)
, m_arete(arete)
, m_ratio(ratio)
{
#ifdef _DEBUG_MEMORY
    std::cout<<"CommandSplitBlock::CommandSplitBlock("
    		<<bloc->getName()<<", "
    		<<arete->getName()<<", "
    		<<ratio
    		<<") pour la commande "<<getName()<<std::endl;
#endif
    if (ratio<0.0 || ratio>1.0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le ratio doit être dans l'interval [0 1]", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandSplitBlock::
CommandSplitBlock(Internal::Context& c, Block* bloc, CoEdge* arete, const Point& pt)
:CommandEditTopo(c, std::string("Découpage du bloc ") + bloc->getName())
, m_bloc(bloc)
, m_arete(arete)
, m_ratio(0)
{
	m_ratio = arete->computeRatio(pt);

#ifdef _DEBUG_MEMORY
    std::cout<<"CommandSplitBlock::CommandSplitBlock("
    		<<bloc->getName()<<", "
    		<<arete->getName()<<", "
    		<<m_ratio
    		<<") pour la commande "<<getName()<<std::endl;
#endif
    if (m_ratio<0.0 || m_ratio>1.0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le ratio doit être dans l'interval [0 1]", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandSplitBlock::
~CommandSplitBlock()
{
#ifdef _DEBUG_MEMORY
    std::cout<<"CommandSplitBlock::~CommandSplitBlock() pour la commande "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlock::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSplitBlock::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // cas où la commande doit être détuite, il faut en créer une autre ...
    if (m_isPreview)
    	throw TkUtil::Exception (TkUtil::UTF8String ("La commande CommandSplitBlock ne peut être exécuté, elle est en mode preview", TkUtil::Charset::UTF_8));

    if (!m_bloc->isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Le bloc doit être structuré pour être découpé avec cette méthode", TkUtil::Charset::UTF_8));

    /// Les blocs créés (non utilisé)
    std::vector<Block* > newBlocks;

    // la modification de la topologie
    m_bloc->split(m_arete, m_ratio, newBlocks, &getInfoCommand());

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlock::
countNbCoEdgesByVertices(std::map<Topo::Vertex*, uint> &nb_coedges_by_vertex)
{
	// stocke pour chacun des sommets le nombre d'arêtes auxquelles il est relié
	std::vector<Topo::Vertex* > all_vertices;
	m_bloc->getAllVertices(all_vertices);
	for (uint i=0; i<all_vertices.size(); i++)
		nb_coedges_by_vertex[all_vertices[i]] = all_vertices[i]->getNbCoEdges();
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlock::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
