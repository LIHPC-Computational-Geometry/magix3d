/*----------------------------------------------------------------------------*/
/*
 * \file CommandFuse2Edges.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 27/6/2016
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <Topo/CommandFuse2Edges.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandFuse2Edges::
CommandFuse2Edges(Internal::Context& c, CoEdge* edge_A, CoEdge* edge_B)
:CommandEditTopo(c, std::string("Fusion entre arêtes ")
                    + edge_A->getName() + " et " + edge_B->getName())
, m_edge_A(edge_A)
, m_edge_B(edge_B)
{
}
/*----------------------------------------------------------------------------*/
CommandFuse2Edges::
~CommandFuse2Edges()
{
}
/*----------------------------------------------------------------------------*/
void CommandFuse2Edges::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandGlue2Edges::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // marque les coedges et cofaces à conserver de préférence
    // lors des collages automatiques autours
    std::map<CoEdge*,uint> keep_coedges;
    std::map<CoFace*,uint> keep_cofaces;

    for (uint i=0; i<m_edge_A->getNbVertices(); i++){
    	Vertex* vtx = m_edge_A->getVertex(i);
    	if (!m_edge_B->find(vtx)){
    		std::vector<CoEdge*> loc_coedges;
    		vtx->getCoEdges(loc_coedges);
    		for (uint j=0; j<loc_coedges.size(); j++)
    			keep_coedges[loc_coedges[j]] = 1;
    	}
    }

    std::vector<CoFace*> loc_cofaces;
    m_edge_A->getCoFaces(loc_cofaces);
    for (uint i=0; i<loc_cofaces.size(); i++)
    	keep_cofaces[loc_cofaces[i]] = 1;

    // fusion avec recherche des sommets les plus proche possible
    m_edge_A->fuse(m_edge_B, &getInfoCommand());

    std::vector<Vertex*> vertices;
    m_edge_A->getVertices(vertices);

    // fusion des arêtes autour
    for (uint i=0; i<vertices.size(); i++)
    	mergeCoEdges(vertices[i], keep_coedges);

    // fusion des faces autour des 2 sommets
    for (uint i=0; i<vertices.size(); i++)
    	mergeCoFaces(vertices[i], keep_cofaces);

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandFuse2Edges::
postExecute(bool hasError)
{
    // remet de l'odre dans la mémoire (libération des parties internes)
    if (hasError)
        cancelInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandFuse2Edges::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
