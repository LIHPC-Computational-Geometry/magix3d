/*----------------------------------------------------------------------------*/
/*
 * \file CommandExtendSplitFace.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15/5/2014
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandExtendSplitFace.h"

#include "Utils/Common.h"
#include "Topo/TopoHelper.h"
#include "Topo/CoFace.h"
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
CommandExtendSplitFace::
CommandExtendSplitFace(Internal::Context& c, CoFace*  coface, Vertex* sommet)
:CommandEditTopo(c, "Découpage de la face " + coface->getName() + " par prolongation")
, m_coface(coface)
, m_sommet(sommet)
{
    if (!coface->isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Le découpage d'une face n'est possible que si elle est structurée", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandExtendSplitFace::
~CommandExtendSplitFace()
{
}
/*----------------------------------------------------------------------------*/
void CommandExtendSplitFace::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandExtendSplitFace::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // vérification que le sommet n'est pas à une extrémité de la coface, sinon cela ne ferait rien
    std::vector<Vertex* > vertices;
    m_coface->getVertices(vertices);
    for (std::vector<Vertex* >::iterator iter = vertices.begin(); iter != vertices.end(); ++iter)
    	if (m_sommet == *iter)
    		throw TkUtil::Exception (TkUtil::UTF8String ("La face ne peut être découpée avec un sommet au sommet de la face (en prendre un sur un côté)", TkUtil::Charset::UTF_8));


    // recherche de l'arête de la face qui contient ce sommet
    std::vector<CoEdge* > coedges;
    m_coface->getCoEdges(coedges);

    CoEdge* coedge = 0;
    for (std::vector<Topo::CoEdge* >::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter)
    	if ((*iter)->find(m_sommet))
    		coedge = *iter;

    if (coedge == 0)
    	throw TkUtil::Exception (TkUtil::UTF8String ("La face ne peut être découpée avec ce sommet, on ne trouve pas d'arête contenant ce sommet", TkUtil::Charset::UTF_8));

    // les Edges qui coupent la face en deux (inutilisé)
    std::vector<Edge* > splitingEdges;
    std::vector<CoFace* > cofaces;
    cofaces.push_back(m_coface);

    if (coedge->getVertex(0) == m_sommet)
    	TopoHelper::splitFaces(cofaces, coedge, 0, 0, false, false, splitingEdges, &getInfoCommand());
    else if (coedge->getVertex(1) == m_sommet)
    	TopoHelper::splitFaces(cofaces, coedge, 1, 0, false, false, splitingEdges, &getInfoCommand());
    else
    	throw TkUtil::Exception (TkUtil::UTF8String ("La face ne peut être découpée avec ce sommet, on ne trouve retrouve pas ce sommet dans l'arête", TkUtil::Charset::UTF_8));

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandExtendSplitFace::
countNbCoEdgesByVertices(std::map<Topo::Vertex*, uint> &nb_coedges_by_vertex)
{
	// stocke pour chacun des sommets le nombre d'arêtes auxquelles il est relié
	std::vector<Topo::Vertex* > all_vertices;
	m_coface->getAllVertices(all_vertices);
	for (uint i=0; i<all_vertices.size(); i++)
		nb_coedges_by_vertex[all_vertices[i]] = all_vertices[i]->getNbCoEdges();
}
/*----------------------------------------------------------------------------*/
void CommandExtendSplitFace::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
