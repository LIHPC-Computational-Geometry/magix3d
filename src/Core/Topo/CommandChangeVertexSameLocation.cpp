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
    std::list<Topo::Block*> l_b;
    // liste des cofaces modifiées
    std::list<Topo::CoFace*> l_f;


    m_vertex->saveVertexGeomProperty(&getInfoCommand(), true);

    m_vertex->setCoord(m_target->getCoord());

    std::vector<Block* > blocks;
    m_vertex->getBlocks(blocks);
    l_b.insert(l_b.end(), blocks.begin(), blocks.end());

    std::vector<CoFace* > cofaces;
    m_vertex->getCoFaces(cofaces);
    l_f.insert(l_f.end(), cofaces.begin(), cofaces.end());

    l_b.sort(Utils::Entity::compareEntity);
    l_b.unique();
    l_f.sort(Utils::Entity::compareEntity);
    l_f.unique();

    // recherche la méthode la plus simple possible
    updateMeshLaw(l_f, l_b);

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
