/*----------------------------------------------------------------------------*/
/*
 * \file CommandSplitBlocks.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 14/5/2014
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSplitBlocks.h"

#include "Utils/Common.h"
#include "Topo/Block.h"
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
CommandSplitBlocks::
CommandSplitBlocks(Internal::Context& c, std::vector<Topo::Block* > &blocs, CoEdge* arete, double ratio)
:CommandEditTopo(c, std::string("Découpage de plusieurs blocs"))
, m_arete(arete)
, m_ratio(ratio)
, m_allBlocksMustBeCut(false)
{
	validateRatio();
	initBlocks(blocs, true);
	initCommentsBlocks(arete);

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Découpage suivant "<<m_arete->getName()<<" des blocs";
	for (uint i=0; i<blocs.size() && i<5; i++)
		comments << " " << blocs[i]->getName();
	if (blocs.size()>5)
		comments << " ... ";
	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSplitBlocks::
CommandSplitBlocks(Internal::Context& c, std::vector<Topo::Block* > &blocs, CoEdge* arete, const Point& pt)
:CommandEditTopo(c, std::string("Découpage de plusieurs blocs"))
, m_arete(arete)
, m_ratio(0)
, m_allBlocksMustBeCut(false)
{
	m_ratio = arete->computeRatio(pt);
	validateRatio();
	initBlocks(blocs, true);
	initCommentsBlocks(arete);
}
/*----------------------------------------------------------------------------*/
CommandSplitBlocks::
CommandSplitBlocks(Internal::Context& c, CoEdge* arete, double ratio)
:CommandEditTopo(c, std::string("Découpage de tous les blocs"))
, m_arete(arete)
, m_ratio(ratio)
, m_allBlocksMustBeCut(false)
{
	std::vector<Topo::Block* > blocs;
	c.getLocalTopoManager().getBlocks(blocs, true);
	validateRatio();
	initBlocks(blocs, false);
	initCommentsAllBlocks(arete);
}
/*----------------------------------------------------------------------------*/
CommandSplitBlocks::
CommandSplitBlocks(Internal::Context& c, CoEdge* arete, const Point& pt)
:CommandEditTopo(c, std::string("Découpage de tous les blocs"))
, m_arete(arete)
, m_ratio(0.0)
, m_allBlocksMustBeCut(false)
{
	m_ratio = arete->computeRatio(pt);
	std::vector<Topo::Block* > blocs;
	c.getLocalTopoManager().getBlocks(blocs, true);

	validateRatio();
	initBlocks(blocs, false);
	initCommentsAllBlocks(arete);
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocks::validateRatio()
{
	if (m_ratio<0.0 || m_ratio>1.0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le ratio doit être dans l'interval [0 1] (compris)", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocks::initBlocks(std::vector<Topo::Block* > &blocs, bool exceptionIfUnstructured)
{
	// on ne concerve que les blocs structurés
	for (std::vector<Topo::Block* >::iterator iter = blocs.begin();
			iter != blocs.end(); ++iter){
		Topo::Block* hb = *iter;
		if (hb->isStructured())
			m_blocs.push_back(hb);
		else if (exceptionIfUnstructured){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message <<"Il n'est pas prévu de faire un découpage dans un bloc non structuré\n";
			message << "("<<hb->getName()<<" n'est pas structuré)";
			throw TkUtil::Exception(message);
		}
	}
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocks::initCommentsBlocks(CoEdge* arete)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);

	comments << "Découpage des blocs";
	for (uint i=0; i<m_blocs.size() && i<5; i++)
		comments << " " << m_blocs[i]->getName();
	if (m_blocs.size()>5)
		comments << " ... ";
	comments << ", suivant l'arête " <<arete->getName();

	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocks::initCommentsAllBlocks(CoEdge* arete)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);

	comments << "Découpage de tous les blocs suivant l'arête " <<arete->getName();

	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSplitBlocks::
~CommandSplitBlocks()
{
#ifdef _DEBUG_MEMORY
    std::cout<<"CommandSplitBlocks::~CommandSplitBlocks() pour la commande "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocks::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSplitBlocks::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // cas où la commande doit être détuite, il faut en créer une autre ...
    if (m_isPreview)
    	throw TkUtil::Exception (TkUtil::UTF8String ("La commande CommandSplitBlocks ne peut être exécuté, elle est en mode preview", TkUtil::Charset::UTF_8));


    uint nb_blocs_dep = m_blocs.size();
    uint nb_blocs_split = 0;

    std::set<CoEdge*> filtre_coedges;
    std::set<Block*> filtre_blocs;

    filtre_coedges.insert(m_arete);
    //std::cout<<"  filtre_coedges.insert("<<m_arete->getName()<<")\n";

    try {
    	do {
    		CoEdge* arete = 0;
    		Block* bloc = 0;

    		// recherche d'un bloc suivant avec une arête
    		findBlockUnmarkedWithCoEdgeMarked(filtre_blocs, filtre_coedges, bloc, arete);

    		// Les blocs créés
    		std::vector<Block* > newBlocks;

    		if (nb_blocs_split)
    			bloc->extendSplit(arete, newBlocks, &getInfoCommand());
    		else
    			bloc->split(m_arete, m_ratio, newBlocks, &getInfoCommand());

    		nb_blocs_split+=1;
    		filtre_blocs.insert(bloc);

    		// marquer les aretes entre les 2 séries de blocs
    		if (newBlocks.size() != 2){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    			message <<"La commande CommandSplitBlocks ne donne pas deux blocs après découpage du bloc "<<bloc->getName();
    			throw TkUtil::Exception(message);
    		}

    		std::vector<CoEdge*> newCoedges = TopoHelper::getCommonCoEdges(newBlocks[0], newBlocks[1]);

    		for (std::vector<CoEdge*>::iterator iter=newCoedges.begin(); iter!=newCoedges.end(); ++iter){
    			//std::cout<<"  filtre_coedges.insert("<<(*iter)->getName()<<")\n";
    			filtre_coedges.insert(*iter);
    		}

    	} while (nb_blocs_dep != nb_blocs_split);
    }
    catch (const FindBlockException& exc){
    	if (m_allBlocksMustBeCut)
    		throw TkUtil::Exception(exc);
    }
    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocks::
findBlockUnmarkedWithCoEdgeMarked(std::set<Block*>& filtre_blocs,
		std::set<CoEdge*>& filtre_coedges, Block* &bloc, CoEdge* &arete)
{
	//std::cout<<"findBlockUnmarkedWithCoEdgeMarked ..."<<std::endl;
	for (std::vector<Block*>::iterator iter1 = m_blocs.begin(); iter1 != m_blocs.end(); ++iter1){
		// le bloc est-il marqué ?
		//std::cout<<" bloc "<<(*iter1)->getName()<<std::endl;
		if (filtre_blocs.find(*iter1) == filtre_blocs.end()){
			Block* bl = *iter1;
			//std::cout<<"   pas encore vu, on recherche une arête ..."<<std::endl;
			// possède-t-il une arête marquée ?
			std::vector<CoEdge* > coedges;
			bl->getCoEdges(coedges);
			for (std::vector<CoEdge*>::iterator iter2 = coedges.begin(); iter2 != coedges.end(); ++iter2){
				if (filtre_coedges.find(*iter2) != filtre_coedges.end()){
					arete = *iter2;
					bloc = bl;
					return;
				}
			}
		}
	}
	throw FindBlockException(TkUtil::UTF8String ( "On ne trouve pas de couple arête-bloc pour propager la découpe", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocks::
countNbCoEdgesByVertices(std::map<Topo::Vertex*, uint> &nb_coedges_by_vertex)
{
	// stocke pour chacun des sommets le nombre d'arêtes auxquelles il est relié
	for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
	            iter1 != m_blocs.end(); ++iter1){
		Block* bloc = *iter1;
		std::vector<Topo::Vertex* > all_vertices;
		bloc->getAllVertices(all_vertices);
		for (uint i=0; i<all_vertices.size(); i++)
			nb_coedges_by_vertex[all_vertices[i]] = all_vertices[i]->getNbCoEdges();
	}
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocks::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
