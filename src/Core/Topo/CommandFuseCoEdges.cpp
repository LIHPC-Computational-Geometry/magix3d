/*----------------------------------------------------------------------------*/
/*
 * \file CommandFuseCoEdges.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 31 mai 2012
 */
/*----------------------------------------------------------------------------*/
#include <Topo/CommandFuseCoEdges.h>
#include "Internal/Context.h"
#include "Topo/CoEdge.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/TopoHelper.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class GeomEntity;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandFuseCoEdges::
CommandFuseCoEdges(Internal::Context& c, std::vector<Topo::CoEdge* > &coedges)
:CommandEditTopo(c, "Fusion d'arêtes")
, m_coedges(coedges)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Fusion des arêtes ";
	for (uint i=0; i<coedges.size() && i<5; i++)
		comments << " " << coedges[i]->getName();
	if (coedges.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandFuseCoEdges::
~CommandFuseCoEdges()
{
}
/*----------------------------------------------------------------------------*/
void CommandFuseCoEdges::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandFuseCoEdges::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // il doit y avoir au moins 2 arêtes, sinon aucun intéret
    if (m_coedges.size()<2)
        throw TkUtil::Exception (TkUtil::UTF8String ("Il n'y a rien à faire avec CommandFuseCoEdges", TkUtil::Charset::UTF_8));

    // Vérification que toutes les arêtes pointes sur la même entité géométrique
    Geom::GeomEntity* ge = m_coedges[0]->getGeomAssociation();
    for (std::vector<Topo::CoEdge* >::iterator iter = m_coedges.begin();
            iter != m_coedges.end(); ++iter){
        if (ge != (*iter)->getGeomAssociation())
            throw TkUtil::Exception (TkUtil::UTF8String ("La fusion d'arêtes ne peut se faire car elles ne pointent pas toutes sur la même entité géométrique", TkUtil::Charset::UTF_8));
    }

    // Vérification qu'elles appartiennent aux mêmes arêtes
    std::map<uint, uint> filtre_edges;
    std::vector<Edge* > edges;
    m_coedges[0]->getEdges(edges);
    uint nb_edges = edges.size();
    for (std::vector<Topo::Edge* >::iterator iter2 = edges.begin();
            iter2 != edges.end(); ++iter2)
        filtre_edges[(*iter2)->getUniqueId()] = 1;

    for (std::vector<Topo::CoEdge* >::iterator iter1 = m_coedges.begin();
                iter1 != m_coedges.end(); ++iter1){

        (*iter1)->getEdges(edges);
        if (nb_edges != edges.size())
            throw TkUtil::Exception (TkUtil::UTF8String ("La fusion d'arêtes communes ne peut se faire car elles ne sont pas toutes utilisées par le même nombre arêtes", TkUtil::Charset::UTF_8));

        for (std::vector<Topo::Edge* >::iterator iter2 = edges.begin();
                    iter2 != edges.end(); ++iter2)
            if (filtre_edges[(*iter2)->getUniqueId()] != 1)
                throw TkUtil::Exception (TkUtil::UTF8String ("La fusion d'arêtes communes ne peut se faire car elles ne sont pas toutes utilisées par les mêmes arêtes", TkUtil::Charset::UTF_8));
    }

    // on modifie la dernière arête
    Topo::CoEdge* edge_modif = m_coedges.back();

    // Attention, le saveCoEdgeTopoProperty et le switchCoEdgeMeshingProperty doivent
    // être fait dans cet ordre, car pb avec  change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
    edge_modif->saveCoEdgeTopoProperty(&getInfoCommand());

    // On met une discrétisation uniforme sur l'ensemble, on respecte le nombre total de bras
    uint nb_bras = 0;
    for (std::vector<Topo::CoEdge* >::iterator iter1 = m_coedges.begin();
            iter1 != m_coedges.end(); ++iter1)
        nb_bras += (*iter1)->getNbMeshingEdges();
    EdgeMeshingPropertyUniform emp(nb_bras);
    edge_modif->switchCoEdgeMeshingProperty(&getInfoCommand(), &emp);

    // il faut repérer les sommets aux extrémités de l'ensemble des arêtes
    Topo::Vertex* vtx1 = 0;
    Topo::Vertex* vtx2 = 0;
    TopoHelper::getVerticesTip(m_coedges, vtx1, vtx2, true);

    if (vtx1 == edge_modif->getVertex(0))
        edge_modif->replace(edge_modif->getVertex(1), vtx2, true, false, &getInfoCommand());
    else if (vtx1 == edge_modif->getVertex(1))
        edge_modif->replace(edge_modif->getVertex(0), vtx2, true, false, &getInfoCommand());
    else if (vtx2 == edge_modif->getVertex(0))
        edge_modif->replace(edge_modif->getVertex(1), vtx1, true, false, &getInfoCommand());
    else if (vtx2 == edge_modif->getVertex(1))
        edge_modif->replace(edge_modif->getVertex(0), vtx1, true, false, &getInfoCommand());
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandFuseCoEdges ne trouve pas de sommet extrémité pour l'arête à conserver", TkUtil::Charset::UTF_8));

    // et on supprime les autres, ainsi que leurs sommets (sauf ceux aux extrémités)
    m_coedges.pop_back();
    for (std::vector<Topo::CoEdge* >::iterator iter1 = m_coedges.begin();
            iter1 != m_coedges.end(); ++iter1){
    	std::vector<Topo::Vertex*> vertices;
    	(*iter1)->getVertices(vertices);
    	for (std::vector<Topo::Vertex*>::iterator iter2 = vertices.begin();
    			iter2 != vertices.end(); ++iter2)
    		if (*iter2 != vtx1 && *iter2 != vtx2)
    			(*iter2)->free(&getInfoCommand());

    	(*iter1)->free(&getInfoCommand());
    }

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager (il n'y en a surement aucune)
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandFuseCoEdges::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewCoedges(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
