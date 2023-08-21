/*----------------------------------------------------------------------------*/
/*
 * \file CommandFuse2Vertices.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 13/1/2014
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandFuse2Vertices.h"

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
CommandFuse2Vertices::
CommandFuse2Vertices(Internal::Context& c, Vertex* vtx_A, Vertex* vtx_B)
:CommandEditTopo(c, std::string("Fusion entre les 2 sommets ")
                    + vtx_A->getName() + " et " + vtx_B->getName())
, m_vtx_A(vtx_A)
, m_vtx_B(vtx_B)
{
	// vérification qu'il n'y à pas une arête commune entre les 2 sommets
	// ce qui arrive lorsque l'on souhaite faire de la dégénérescence (snapVertices)
    CoEdge* coedge_between = 0;
    std::vector<CoEdge* > loc_coedges;
    vtx_A->getCoEdges(loc_coedges);
    for (std::vector<CoEdge* >::iterator iter = loc_coedges.begin();
            iter != loc_coedges.end(); ++iter)
        if (vtx_B == (*iter)->getOppositeVertex(vtx_A))
            coedge_between = *iter;
    if (coedge_between){
    	throw TkUtil::Exception(TkUtil::UTF8String(
    		"La fusion entre 2 sommets est prévue pour coller 2 topologies et pas pour effectuer une dégénescence d'un bloc",
    		TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
CommandFuse2Vertices::
~CommandFuse2Vertices()
{
}
/*----------------------------------------------------------------------------*/
void CommandFuse2Vertices::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandFuse2Vertices::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // marque les coedges et cofaces à conserver de préférence
    // lors des collages automatiques autours
    std::map<CoEdge*,uint> keep_coedges;
    std::map<CoFace*,uint> keep_cofaces;

    std::vector<CoEdge*> loc_coedges;
    m_vtx_A->getCoEdges(loc_coedges);
    for (uint j=0; j<loc_coedges.size(); j++)
    	keep_coedges[loc_coedges[j]] = 1;

    std::vector<CoFace*> loc_cofaces;
    m_vtx_A->getCoFaces(loc_cofaces);
    for (uint i=0; i<loc_cofaces.size(); i++)
    	keep_cofaces[loc_cofaces[i]] = 1;

    // fusion des 2 sommets
    m_vtx_A->merge(m_vtx_B, &getInfoCommand());

    // fusion des arêtes autour si possible
    mergeCoEdges(m_vtx_A, keep_coedges);

    // fusion des faces autour si possible
    mergeCoFaces(m_vtx_A, keep_cofaces);

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandFuse2Vertices::
postExecute(bool hasError)
{
    // remet de l'odre dans la mémoire (libération des parties internes)
    if (hasError)
        cancelInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandFuse2Vertices::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
