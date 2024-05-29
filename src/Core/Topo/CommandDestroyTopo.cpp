/*----------------------------------------------------------------------------*/
/*
 * \file CommandDestroyTopo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 31 mai 2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Topo/CommandDestroyTopo.h"
#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"
#include "Topo/EdgeMeshingPropertyGlobalInterpolate.h"
#include "Topo/TopoHelper.h"

#include "Geom/CommandEditGeom.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <string>
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandDestroyTopo::
CommandDestroyTopo(Internal::Context& c, std::vector<Topo::TopoEntity*> topos, bool propagate)
:CommandEditTopo(c, "Destructions d'entités topologiques")
, m_topo_entities(topos)
, m_propagate(propagate)
, m_command_edit_geom(0)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Destruction des entités topologiques";
	for (uint i=0; i<topos.size() && i<5; i++)
		comments << " " << topos[i]->getName();
	if (topos.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandDestroyTopo::
CommandDestroyTopo(Internal::Context& c, Geom::CommandEditGeom* command_geom, bool propagate)
:CommandEditTopo(c, "Destructions d'entités topologiques à partir d'une commande géométrique")
, m_propagate(propagate)
, m_command_edit_geom(command_geom)
{
}
/*----------------------------------------------------------------------------*/
CommandDestroyTopo::
~CommandDestroyTopo()
{
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::
internalExecute()
{
	// récupération si nécessaire de la liste des entités topologiques à partir de
	// la liste des entités géométriques détruites
	findTopoEntities();

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandDestroyTopo::execute pour la commande " << getName ( );

    for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
    		iter != m_topo_entities.end(); ++iter)
    	switch((*iter)->getDim()){
    	case(3):
			{
    		Block* bl = dynamic_cast<Block*>(*iter);
    		CHECK_NULL_PTR_ERROR(bl);
    		destroy (bl);
			}
    	break;
    	case(2):
			{
    		CoFace* cf = dynamic_cast<CoFace*>(*iter);
    		CHECK_NULL_PTR_ERROR(cf);
    		destroy (cf);
			}
    	break;
    	case(1):
			{
    		CoEdge* ce = dynamic_cast<CoEdge*>(*iter);
    		CHECK_NULL_PTR_ERROR(ce);
    		destroy (ce);
			}
    	break;
    	case(0):
				{
	    		Vertex* vtx = dynamic_cast<Vertex*>(*iter);
	    		CHECK_NULL_PTR_ERROR(vtx);
	    		destroy (vtx);
				}
    	break;
    	default:
    		throw TkUtil::Exception (TkUtil::UTF8String ("dimension non prévue pour CommandDestroyTopo", TkUtil::Charset::UTF_8));
    	}

    // libération mémoire, les filtres ne servent plus
    m_filtre_blocks.clear();
    m_filtre_faces.clear();
    m_filtre_cofaces.clear();
    m_filtre_edges.clear();
    m_filtre_coedges.clear();
    m_filtre_vertices.clear();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));


    // vérification des liaisons pour les arêtes avec méthode interpolée
    checkInterpolates();
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::findTopoEntities()
{
	if (m_command_edit_geom){

		std::vector<Geom::GeomEntity*> entities = m_command_edit_geom->getRemovedEntities();

		m_topo_entities = Topo::TopoHelper::getTopoEntities(entities);

	} // if (m_command_edit_geom)
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::destroy(Block* bloc)
{
	CHECK_NULL_PTR_ERROR(bloc);

	// marque le bloc et les faces, ainsi que les entités de niveau inférieur si propagation demandée
	mark(bloc);

	// parcours les entités pour déterminer si elles dépendent encore
	// de quelque chose qui n'est pas marqué
	// si ce n'est pas le cas, alors elles sont à détruire
	destroyAllFreeEntities();
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::destroy(CoFace* coface)
{
	CHECK_NULL_PTR_ERROR(coface);

	// marque la coface et ses edges, ainsi que les entités de niveau inférieur si propagation demandée
	mark(coface);

	// marque égallement les entités de niveau supérieur
	std::vector<Block*> blocks;
	coface->getBlocks(blocks);
	for (std::vector<Block*>::iterator iter = blocks.begin();
			iter != blocks.end(); ++iter)
		mark(*iter);

	// parcours les entités pour déterminer si elles dépendent encore
	// de quelque chose qui n'est pas marqué
	// si ce n'est pas le cas, alors elles sont à détruire
	destroyAllFreeEntities();
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::destroy(CoEdge* coedge)
{
	CHECK_NULL_PTR_ERROR(coedge);

	// marque la coedge, ainsi que les entités de niveau inférieur si propagation demandée
	mark(coedge);

	// marque égallement les entités de niveau supérieur
	std::vector<CoFace*> cofaces;
	coedge->getCoFaces(cofaces);
	for (std::vector<CoFace*>::iterator iter1 = cofaces.begin();
			iter1 != cofaces.end(); ++iter1){
		CoFace* coface = *iter1;
		mark(coface);

		std::vector<Block*> blocks;
		coface->getBlocks(blocks);
		for (std::vector<Block*>::iterator iter2 = blocks.begin();
				iter2 != blocks.end(); ++iter2)
			mark(*iter2);
	} // end for iter1

	// parcours les entités pour déterminer si elles dépendent encore
	// de quelque chose qui n'est pas marqué
	// si ce n'est pas le cas, alors elles sont à détruire
	destroyAllFreeEntities();
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::destroy(Vertex* vertex)
{
	CHECK_NULL_PTR_ERROR(vertex);

	// marque la coedge, ainsi que les entités de niveau inférieur si propagation demandée
	mark(vertex);

	// marque égallement les entités de niveau supérieur
	std::vector<CoEdge*> coedges;
	vertex->getCoEdges(coedges);
	for (std::vector<CoEdge*>::iterator iter0 = coedges.begin();
				iter0 != coedges.end(); ++iter0){
		CoEdge* coedge = *iter0;
		mark(coedge);

		std::vector<CoFace*> cofaces;
		coedge->getCoFaces(cofaces);
		for (std::vector<CoFace*>::iterator iter1 = cofaces.begin();
				iter1 != cofaces.end(); ++iter1){
			CoFace* coface = *iter1;
			mark(coface);

			std::vector<Block*> blocks;
			coface->getBlocks(blocks);
			for (std::vector<Block*>::iterator iter2 = blocks.begin();
					iter2 != blocks.end(); ++iter2)
				mark(*iter2);
		} // end for iter1
	} // end for iter0
	// parcours les entités pour déterminer si elles dépendent encore
	// de quelque chose qui n'est pas marqué
	// si ce n'est pas le cas, alors elles sont à détruire
	destroyAllFreeEntities();
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::mark(Block* bloc)
{
	if (m_filtre_blocks.count(bloc))
		return;

	m_filtre_blocks.insert(bloc);

	std::vector<Face*> faces;
	bloc->getFaces(faces);

	for (std::vector<Face*>::iterator iter = faces.begin();
			iter != faces.end(); ++iter)
		mark(*iter);
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::mark(Face* face)
{
	m_filtre_faces.insert(face);

	if (m_propagate){
		std::vector<CoFace*> cofaces;
		face->getCoFaces(cofaces);

		for (std::vector<CoFace*>::iterator iter = cofaces.begin();
				iter != cofaces.end(); ++iter)
			mark(*iter);
	}
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::mark(CoFace* coface)
{
	if (m_filtre_cofaces.count(coface))
		return;

	m_filtre_cofaces.insert(coface);

	std::vector<Edge*> edges;
	coface->getEdges(edges);

	for (std::vector<Edge*>::iterator iter = edges.begin();
			iter != edges.end(); ++iter)
		mark(*iter);
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::mark(Edge* edge)
{
	m_filtre_edges.insert(edge);

	std::vector<CoEdge*> coedges;
	edge->getCoEdges(coedges);

	if (m_propagate){
		for (std::vector<CoEdge*>::iterator iter = coedges.begin();
				iter != coedges.end(); ++iter)
			mark(*iter);
	}
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::mark(CoEdge* coedge)
{
	m_filtre_coedges.insert(coedge);

	std::vector<Vertex*> vertices;
	coedge->getVertices(vertices);

	if (m_propagate){
		for (std::vector<Vertex*>::iterator iter = vertices.begin();
				iter != vertices.end(); ++iter)
			mark(*iter);
	}
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::mark(Vertex* vertex)
{
	m_filtre_vertices.insert(vertex);
}
/*----------------------------------------------------------------------------*/
bool CommandDestroyTopo::dependUnmarqued(Face* face)
{
	std::vector<Block*> blocs;
	face->getBlocks(blocs);
	for (std::vector<Block*>::iterator iter = blocs.begin();
			iter != blocs.end(); ++iter)
		if (m_filtre_blocks.count(*iter) == 0)
			return true;

	return false;
}
/*----------------------------------------------------------------------------*/
bool CommandDestroyTopo::dependUnmarqued(CoFace* coface)
{
	std::vector<Face*> faces;
	coface->getFaces(faces);
	for (std::vector<Face*>::iterator iter = faces.begin();
			iter != faces.end(); ++iter)
		if (m_filtre_faces.count(*iter) == 0)
			return true;

	return false;
}
/*----------------------------------------------------------------------------*/
bool CommandDestroyTopo::dependUnmarqued(Edge* edge)
{
	std::vector<CoFace*> cofaces;
	edge->getCoFaces(cofaces);
//	if (cofaces.size()>1)
//		std::cout<<"CommandDestroyTopo::dependUnmarqued, trouve que "<<edge->getName()<<" est relié à "<<cofaces.size()<<" cofaces"<<std::endl;

	for (std::vector<CoFace*>::iterator iter = cofaces.begin();
			iter != cofaces.end(); ++iter)
		if (m_filtre_cofaces.count(*iter) == 0)
			return true;

	return false;
}
/*----------------------------------------------------------------------------*/
bool CommandDestroyTopo::dependUnmarqued(CoEdge* coedge)
{
	std::vector<Edge*> edges;
	coedge->getEdges(edges);
	for (std::vector<Edge*>::iterator iter = edges.begin();
			iter != edges.end(); ++iter)
		if (m_filtre_edges.count(*iter) == 0)
			return true;

	return false;
}
/*----------------------------------------------------------------------------*/
bool CommandDestroyTopo::dependUnmarqued(Vertex* vertex)
{
	std::vector<CoEdge*> coedges;
	vertex->getCoEdges(coedges);
	for (std::vector<CoEdge*>::iterator iter = coedges.begin();
			iter != coedges.end(); ++iter)
		if (m_filtre_coedges.count(*iter) == 0)
			return true;

	return false;
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::destroyAllFreeEntities()
{
	// listes des entités qui sont finallement conservées
	// on mofifie les filtres pour éviter de détruire inutilement une entité de niveau inférieur
	std::vector<CoEdge*> coedges_conservated;
	std::vector<Edge*> edges_conservated;
	std::vector<CoFace*> cofaces_conservated;


	for (std::set<Block*>::iterator iter = m_filtre_blocks.begin();
		                iter != m_filtre_blocks.end(); ++iter)
		(*iter)->free(&getInfoCommand());

	for (std::set<Face*>::iterator iter = m_filtre_faces.begin();
		                iter != m_filtre_faces.end(); ++iter)
		if (!dependUnmarqued(*iter))
			(*iter)->free(&getInfoCommand());

	for (std::set<CoFace*>::iterator iter = m_filtre_cofaces.begin();
		                iter != m_filtre_cofaces.end(); ++iter)
		if (!dependUnmarqued(*iter))
			(*iter)->free(&getInfoCommand());
		else
			cofaces_conservated.push_back(*iter);

	for (std::vector<CoFace*>::iterator iter = cofaces_conservated.begin();
			iter != cofaces_conservated.end(); ++iter)
		m_filtre_cofaces.erase(*iter);


	for (std::set<Edge*>::iterator iter = m_filtre_edges.begin();
		                iter != m_filtre_edges.end(); ++iter)
		if (!dependUnmarqued(*iter))
			(*iter)->free(&getInfoCommand());
		else
			edges_conservated.push_back(*iter);

	for (std::vector<Edge*>::iterator iter = edges_conservated.begin();
			iter != edges_conservated.end(); ++iter)
		m_filtre_edges.erase(*iter);


	for (std::set<CoEdge*>::iterator iter = m_filtre_coedges.begin();
		                iter != m_filtre_coedges.end(); ++iter)
		if (!dependUnmarqued(*iter))
			(*iter)->free(&getInfoCommand());
		else
			coedges_conservated.push_back(*iter);

	for (std::vector<CoEdge*>::iterator iter = coedges_conservated.begin();
			iter != coedges_conservated.end(); ++iter)
		m_filtre_coedges.erase(*iter);


	for (std::set<Vertex*>::iterator iter = m_filtre_vertices.begin();
		                iter != m_filtre_vertices.end(); ++iter)
		if (!dependUnmarqued(*iter))
			(*iter)->free(&getInfoCommand());
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationDeletedTopo(dr);
}
/*----------------------------------------------------------------------------*/
void CommandDestroyTopo::checkInterpolates()
{
	// problèmes identifiés
	TkUtil::UTF8String	warnings (TkUtil::Charset::UTF_8);

	// liste des noms des arêtes et faces détruites
//	std::set<std::string> deletedCoEdges; plante avec icpc sur set::insert !!!
//	std::set<std::string> deletedCoFaces;
	std::map<std::string, uint> deletedCoEdges;
	std::map<std::string, uint> deletedCoFaces;
	Internal::InfoCommand& ifc = getInfoCommand();
	std::map<Topo::TopoEntity*, Internal::InfoCommand::type> tei = ifc.getTopoInfoEntity();

	for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::const_iterator iter = tei.begin();
			iter != tei.end(); ++iter){
		//std::cout<<" observation pour "<<iter->first->getName()<<" de type "<<Internal::InfoCommand::type2String(iter->second)<<std::endl;
		if (iter->first->getType() ==  Utils::Entity::TopoCoEdge && iter->second == Internal::InfoCommand::DELETED)
			deletedCoEdges[iter->first->getName()] = 1;
		else if (iter->first->getType() ==  Utils::Entity::TopoCoFace && iter->second == Internal::InfoCommand::DELETED)
			deletedCoFaces[iter->first->getName()] = 1;
	}
	// parcours des arêtes existantes dans le manager
	std::vector<Topo::CoEdge*> coedges;
	getContext().getTopoManager().getCoEdges(coedges);

	for (std::vector<Topo::CoEdge*>::iterator iter=coedges.begin();
				iter!=coedges.end(); ++iter){
		Topo::CoEdge* coedge = *iter;
		if (coedge->getMeshingProperty()->getMeshLaw() == Topo::CoEdgeMeshingProperty::interpolate){
			EdgeMeshingPropertyInterpolate* empi = dynamic_cast<EdgeMeshingPropertyInterpolate*>(coedge->getMeshingProperty());
			CHECK_NULL_PTR_ERROR(empi);

			if (empi->getType() == EdgeMeshingPropertyInterpolate::with_coedge_list){
				std::vector<std::string> coedgesNames = empi->getCoEdges();
				for (uint i=0; i<coedgesNames.size(); i++)
					if (deletedCoEdges[coedgesNames[i]] == 1)
						warnings << "ATTENTION, l'arête "<<coedge->getName()<<" référence "<<coedgesNames[i]<<" qui est désormais détruite";

			}
			else if (empi->getType() == EdgeMeshingPropertyInterpolate::with_coface) {
				std::string cofaceName = empi->getCoFace();
				if (deletedCoFaces[cofaceName] == 1)
					warnings << "ATTENTION, l'arête "<<coedge->getName()<<" référence "<<cofaceName<<" qui est désormais détruite";
			}
		} // end if (getMeshLaw() == interpolate)
		else if (coedge->getMeshingProperty()->getMeshLaw() == Topo::CoEdgeMeshingProperty::globalinterpolate){
			EdgeMeshingPropertyGlobalInterpolate* interpol = dynamic_cast<EdgeMeshingPropertyGlobalInterpolate*>(coedge->getMeshingProperty());
			CHECK_NULL_PTR_ERROR(interpol);

			std::vector<std::string> first_coedges_names = interpol->getFirstCoEdges();
			std::vector<std::string> second_coedges_names = interpol->getSecondCoEdges();
			for (uint i=0; i<first_coedges_names.size(); i++)
				if (deletedCoEdges[first_coedges_names[i]] == 1)
					warnings << "ATTENTION, l'arête "<<coedge->getName()<<" référence "<<first_coedges_names[i]<<" qui est désormais détruite";

			for (uint i=0; i<second_coedges_names.size(); i++)
				if (deletedCoEdges[second_coedges_names[i]] == 1)
					warnings << "ATTENTION, l'arête "<<coedge->getName()<<" référence "<<second_coedges_names[i]<<" qui est désormais détruite";

		} // else if (getMeshLaw() == globalinterpolate)
	}
	if (!warnings.empty())
		log (TkUtil::TraceLog (warnings, TkUtil::Log::WARNING));

}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
