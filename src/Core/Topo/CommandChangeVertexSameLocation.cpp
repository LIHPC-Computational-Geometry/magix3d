/*----------------------------------------------------------------------------*/
/*
 * \file CommandChangeVertexSameLocation.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24/11/2017
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandChangeVertexSameLocation.h"

#include "Utils/Common.h"
#include "Topo/Vertex.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandChangeVertexSameLocation::
CommandChangeVertexSameLocation(Internal::Context& c,
		Vertex* vtx,
		Vertex* target)
:CommandEditTopo(c, "Modification de la position d'un sommet topologique")
, m_vertex(vtx)
, m_target(target)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Modification de la position de "<<vtx->getName()<<" pour qu'il ait la même position que "<<target->getName();

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandChangeVertexSameLocation::
~CommandChangeVertexSameLocation()
{
}
/*----------------------------------------------------------------------------*/
void CommandChangeVertexSameLocation::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandChangeVertexSameLocation::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // liste des blocs modifiés
    std::vector<Topo::Block*> blocks;
    // liste des cofaces modifiées
    std::vector<Topo::CoFace*> cofaces;

    m_vertex->saveVertexGeomProperty(&getInfoCommand(), true);
    m_vertex->setCoord(m_target->getCoord());
    m_vertex->getBlocks(blocks);

    m_vertex->getCoFaces(cofaces);

    std::sort(blocks.begin(), blocks.end(), Utils::Entity::compareEntity);
    auto lastblocks = std::unique(blocks.begin(), blocks.end());
    blocks.erase(lastblocks, blocks.end());

    std::sort(cofaces.begin(), cofaces.end(), Utils::Entity::compareEntity);
    auto lastcofaces = std::unique(cofaces.begin(), cofaces.end());
    cofaces.erase(lastcofaces, cofaces.end());

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandChangeVertexSameLocation::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
