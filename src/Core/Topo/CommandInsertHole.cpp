/*----------------------------------------------------------------------------*/
/*
 * \file CommandInsertHole.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 21/3/2013
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandInsertHole.h"

#include "Utils/Common.h"

#include "Topo/TopoHelper.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/Edge.h"
#include "Topo/CoFace.h"
#include "Topo/Face.h"
#include "Topo/Block.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_INSERTHOLE
/*----------------------------------------------------------------------------*/
CommandInsertHole::
CommandInsertHole(Internal::Context& c,
		std::vector<CoFace*>& cofaces)
:CommandEditTopo(c, "Insertion d'un trou topologique")
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Insertion d'un trou topologique pour "<<cofaces.size()<<" faces";

	setScriptComments(comments);
    setName(comments);

    m_cofaces.insert(m_cofaces.end(), cofaces.begin(), cofaces.end());
}
/*----------------------------------------------------------------------------*/
CommandInsertHole::
~CommandInsertHole()
{
}
/*----------------------------------------------------------------------------*/
void CommandInsertHole::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandInsertHole::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

#ifdef _DEBUG_INSERTHOLE
    std::cout<<"CommandInsertHole::internalExecute() ..."<<std::endl;
#endif

    // duplique certaines entités topologiques
    duplicate();

#ifdef _DEBUG_INSERTHOLE
    std::cout<<"CommandInsertHole::internalExecute() FIN"<<std::endl;
#endif

    // destruction des données temporaires
    clearMemory();

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandInsertHole::
postExecute(bool hasError)
{
    // remet de l'odre dans la mémoire (libération des parties internes)
    if (hasError)
        cancelInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandInsertHole::
duplicate()
{
	// convention pour les filtres
	// à 1 ou 2 suivant le côté du trou
	// à 3 si c'est commun
	// à 4 si c'est commun et à dupliquer

	// nombre de blocs de part et d'autre du trou
	uint nb_blocs_touches = 0;

	// filtre pour cofaces (à 4) et blocs (à 1)
	for (std::vector<CoFace*>::iterator iter_cf=m_cofaces.begin();
			iter_cf!=m_cofaces.end(); ++iter_cf){
		CoFace* coface = *iter_cf;

		// les cofaces sélectionnées sont à dupliquer
		m_filtre_coface[coface] = 4;

		std::vector<Block* > blocks;
		coface->getBlocks(blocks);

		if (blocks.size() != 2){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Insertion d'un trou impossible avec face "<<coface->getName()<<", elle n'est pas entre 2 blocs";
            throw TkUtil::Exception(message);
		}

		// on marque tous les blocs à 1 dans un premier temps
		for (std::vector<Block*>::iterator iter_bl=blocks.begin();
					iter_bl!=blocks.end(); ++iter_bl){
			if (m_filtre_block[*iter_bl] == 0){
				nb_blocs_touches++;
				m_filtre_block[*iter_bl] = 1;
			}
		}
	} // end for iter_cf

#ifdef _DEBUG_INSERTHOLE
    std::cout<<"nb_blocs_touches = "<<nb_blocs_touches<<std::endl;
#endif

	// pour chacunes des cofaces à dupliquer,
    // on cherche à savoir si ses arêtes sont à dupliquer ou non
    // si une arête est entre 2 cofaces à dupliquer, alors on la duplique
    // sinon on ne la duplique que s'il n'y a pas de liaison topo entre les blocs
    // de part et d'autre en tournant autour de l'arête et sans passer par la coface traitée

	for (std::vector<CoFace*>::iterator iter_cf=m_cofaces.begin();
			iter_cf!=m_cofaces.end(); ++iter_cf){
		CoFace* coface = *iter_cf;

		std::vector<CoEdge* > coedges;
		coface->getCoEdges(coedges, false);

		for (std::vector<CoEdge*>::iterator iter_ce=coedges.begin();
				iter_ce!=coedges.end(); ++iter_ce){
			CoEdge* coedge = *iter_ce;
			if (m_filtre_coedge[coedge] == 0){

				std::vector<Topo::CoFace*> cofaces;
				coedge->getCoFaces(cofaces);
				// on compte le nb de cofaces marqués à dupliquer
				uint nb_coface_marquees = 0;
				for (std::vector<CoFace*>::iterator iter=cofaces.begin();
							iter!=cofaces.end(); ++iter)
					if (m_filtre_coface[*iter] == 4)
						nb_coface_marquees++;

				if (nb_coface_marquees == 2){
#ifdef _DEBUG_INSERTHOLE
					std::cout<<"CoEdge à dupliquer (entre 2 cofaces): "<<coedge->getName()<<std::endl;
#endif
					m_filtre_coedge[coedge] = 4;


				} else {
					// ok s'il n'y a une liaison via les blocs et cofaces de l'arête en s'interdisant la coface actuelle
					bool is_ok = findCoFace_Block_relation(coedge, coface);

					if (is_ok){
#ifdef _DEBUG_INSERTHOLE
						std::cout<<"CoEdge commune et conservée: "<<coedge->getName()<<std::endl;
#endif
						m_filtre_coedge[coedge] = 3;
					}
					else {
#ifdef _DEBUG_INSERTHOLE
						std::cout<<"CoEdge à dupliquer (sans passage): "<<coedge->getName()<<std::endl;
#endif
						m_filtre_coedge[coedge] = 4;
					}

				} // end else / if (nb_coface_marquees == 2)

				if (m_filtre_coedge[coedge] == 4){
					// on marque tous les blocs en contact à 1
					std::vector<Block* > blocks;
					coedge->getBlocks(blocks);
					for (std::vector<Block*>::iterator iter_bl=blocks.begin();
							iter_bl!=blocks.end(); ++iter_bl){
						if (m_filtre_block[*iter_bl] == 0){
							nb_blocs_touches++;
							m_filtre_block[*iter_bl] = 1;
						}
					}
				} // end if if (m_filtre_coedge[coedge] == 4)

			} // end if (m_filtre_coedge[coedge] == 0)

		} // end for iter_ce

	} // end for iter_cf

    // un sommet est dupliqué dès lors qu'il est relié à une arête à dupliquée et à aucune arête
    // commune non dupliquée
	std::vector<Vertex*> duplicatedVertices;
	for (std::vector<CoFace*>::iterator iter_cf=m_cofaces.begin();
			iter_cf!=m_cofaces.end(); ++iter_cf){
		CoFace* coface = *iter_cf;

		std::vector<Vertex*> vertices;
		coface->getVertices(vertices);

		for (std::vector<Vertex*>::iterator iter_vt=vertices.begin();
				iter_vt!=vertices.end(); ++iter_vt){
			Vertex* vertex = *iter_vt;
			if (m_filtre_vertex[vertex] == 0){

				std::vector<CoEdge* > coedges;
				vertex->getCoEdges(coedges);

				// a-t-on trouvé une arête marquée à 3 ?
				bool haveCoEdgeConservated = false;

				for (std::vector<CoEdge*>::iterator iter_ce=coedges.begin();
						iter_ce!=coedges.end(); ++iter_ce){
					CoEdge* coedge = *iter_ce;
					if (m_filtre_coedge[coedge] == 3)
						haveCoEdgeConservated = true;
				}
				if (haveCoEdgeConservated){
#ifdef _DEBUG_INSERTHOLE
					std::cout<<"Sommet commun conservé: "<<vertex->getName()<<std::endl;
#endif
					m_filtre_vertex[vertex] = 3;
				}
				else {
					m_filtre_vertex[vertex] = 4;
#ifdef _DEBUG_INSERTHOLE
					std::cout<<"Sommet commun dupliqué: "<<vertex->getName()<<std::endl;
#endif
					duplicatedVertices.push_back(vertex);

					// on marque tous les blocs en contact à 1
					std::vector<Block* > blocks;
					vertex->getBlocks(blocks);
					for (std::vector<Block*>::iterator iter_bl=blocks.begin();
							iter_bl!=blocks.end(); ++iter_bl){
						if (m_filtre_block[*iter_bl] == 0){
							nb_blocs_touches++;
							m_filtre_block[*iter_bl] = 1;
						}
					}
				}

			} // end if (m_filtre_vertex[vertex] == 0
		} // end for iter_vt
	} // end for iter_cf

	// création de 2 groupes de blocs de part et d'autre de l'interface constituée des cofaces
	std::vector<Block*> blocs1 = getConnectedBlocks();
	std::vector<Block*> blocs2 = getConnectedBlocks();
	std::vector<Block*> blocs3 = getConnectedBlocks(); // doit être vide, histoire de voir s'il en reste

#ifdef _DEBUG_INSERTHOLE
	std::cout<<"Nombre de blocs dans 1er groupe : "<<blocs1.size()<<std::endl;
	std::cout<<"Nombre de blocs dans 2ème groupe : "<<blocs2.size()<<std::endl;
	std::cout<<"blocs2:";
	for (uint i=0; i<blocs2.size(); i++)
		std::cout<<" "<<blocs2[i]->getName();
	std::cout<<std::endl;
#endif

	if (blocs1.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur avec commande de création d'un trou, on ne trouve pas de blocs pour le premier côté du trou", TkUtil::Charset::UTF_8));

	if (blocs2.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur avec commande de création d'un trou, on ne trouve pas de blocs pour le deuxième côté du trou", TkUtil::Charset::UTF_8));

	if (!blocs3.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur avec commande de création d'un trou, il reste des blocs autour du trou (non reliés avec les 2 autres groupes)", TkUtil::Charset::UTF_8));

	// on met à 1 d'un côté du trou puis à 2 de l'autre
	for (std::vector<Block*>::iterator iter=blocs1.begin();
			iter!=blocs1.end(); ++iter)
		m_filtre_block[*iter] = 1;

	for (std::vector<Block*>::iterator iter=blocs2.begin();
			iter!=blocs2.end(); ++iter)
		m_filtre_block[*iter] = 2;

	// duplique les Edges qui apparaissent dans plusieurs cofaces alors que l'arête commune
	// va être dupliquée. Et cela sans dupliquer les sommets ni les arêtes,
	for (std::vector<CoFace*>::iterator iter_cf=m_cofaces.begin();
			iter_cf!=m_cofaces.end(); ++iter_cf){
		std::vector<CoEdge*> coedges;
		(*iter_cf)->getCoEdges(coedges);
		for (std::vector<CoEdge*>::iterator iter_ce=coedges.begin();
				iter_ce!= coedges.end(); ++iter_ce){
			std::vector<Edge*> edges;
			(*iter_ce)->getEdges(edges);
			for (std::vector<Edge*>::iterator iter_ed=edges.begin();
					iter_ed!=edges.end(); ++iter_ed){
				separate(*iter_ed);
			} // end for iter_ed
		} // end for iter_ce
	} // end for iter_cf


	// par convention, on ne change rien du côté marqué à 1, on fait un copie pour le côté à 2

	// on duplique les sommets
	for (std::vector<Vertex*>::iterator iter_vtx = duplicatedVertices.begin();
			iter_vtx!=duplicatedVertices.end(); ++iter_vtx){
		Vertex* old_vtx = *iter_vtx;
		Vertex* new_vtx = duplicate(old_vtx);
	} // end for iter_vtx

	// duplique les cofaces, ce qui duplique si nécessaire les entités de niveaux inférieurs
	for (std::vector<CoFace*>::iterator iter_cf=m_cofaces.begin();
			iter_cf!=m_cofaces.end(); ++iter_cf){
		CoFace* old_coface = *iter_cf;
		CoFace* new_coface = duplicate(old_coface);

		// mise à jour dans les faces des blocs du côté marqué à 2
		std::vector<Face*> faces;
		old_coface->getFaces(faces);
		if (faces.size()!=2)
			throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandInsertHole, face commune reliée à autre chose que 2 faces", TkUtil::Charset::UTF_8));

		// recherche celle avec bloc dans groupe 2
		Face* face = 0;
		for (uint i=0; i<faces.size(); i++){
			std::vector<Block*> blocs;
			faces[i]->getBlocks(blocs);
			if (blocs.size()!=1)
				throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandInsertHole, face reliée à autre chose qu'un unique bloc", TkUtil::Charset::UTF_8));
			if (m_filtre_block[blocs[0]] == 2)
				face = faces[i];
		}
		if (face == 0)
			throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandInsertHole, pas de face de trouvée du côté à dupliquer", TkUtil::Charset::UTF_8));
#ifdef _DEBUG_INSERTHOLE
		std::cout<<"Remplace "<<old_coface->getName()<<" par "<<new_coface->getName()<<" dans "<<face->getName()<<std::endl;
#endif
		face->saveFaceTopoProperty(&getInfoCommand());
		face->replace(old_coface, new_coface, &getInfoCommand());

	} // end for iter_cf


	// duplique les arêtes des blocs2 et propage
	for (std::vector<Block*>::iterator iter_bl=blocs2.begin();
			iter_bl!=blocs2.end(); ++iter_bl){
		Block* bloc = *iter_bl;
		std::vector<CoEdge*> coedges;
		bloc->getCoEdges(coedges);

		for (std::vector<CoEdge*>::iterator iter_ce=coedges.begin();
				iter_ce!= coedges.end(); ++iter_ce){
			CoEdge* old_ce = *iter_ce;
			CoEdge* new_ce = duplicate(old_ce);
			if (old_ce != new_ce){
				bloc->replace(old_ce, new_ce, &getInfoCommand());
#ifdef _DEBUG_INSERTHOLE
				std::cout<<"Remplace "<<old_ce->getName()<<" par "<<new_ce->getName()<<" dans "<<bloc->getName()<<std::endl;
#endif
			}
		}
	}


	// duplique les sommets des blocs2 et propage aux entités de niveaux inférieurs
	for (std::vector<Block*>::iterator iter_bl=blocs2.begin();
			iter_bl!=blocs2.end(); ++iter_bl){
		Block* bloc = *iter_bl;
		std::vector<Vertex*> vertices;
		bloc->getVertices(vertices);

		for (std::vector<Vertex*>::iterator iter_vtx=vertices.begin();
				iter_vtx!= vertices.end(); ++iter_vtx){
			Vertex* old_vtx = *iter_vtx;
			Vertex* new_vtx = duplicate(old_vtx);
			if (old_vtx != new_vtx){
				bloc->saveBlockTopoProperty(&getInfoCommand());
				bloc->replace(old_vtx, new_vtx, false, true, &getInfoCommand());
#ifdef _DEBUG_INSERTHOLE
				std::cout<<"Remplace "<<old_vtx->getName()<<" par "<<new_vtx->getName()<<" dans "<<bloc->getName()<<std::endl;
#endif
			}
		}
	}


	// on déplace les sommets pour mieux percevoir le trou
	for (std::vector<Vertex*>::iterator iter_vtx = duplicatedVertices.begin();
			iter_vtx!=duplicatedVertices.end(); ++iter_vtx){
		Vertex* old_vtx = *iter_vtx;
		Vertex* new_vtx = duplicate(old_vtx);

		// translation du sommet en direction du barycentre moyen des blocs autours
		translate(old_vtx);
		translate(new_vtx);

	} // end for iter_vtx

}
/*----------------------------------------------------------------------------*/
void CommandInsertHole::translate(Vertex* vtx)
{
#ifdef _DEBUG_INSERTHOLE
	std::cout<<"translate("<<vtx->getName()<<")"<<std::endl;
#endif
	// utilisation d'un vecteur entre le sommet et la moyenne des barycentres des blocs voisins du groupe 2
	Utils::Math::Point bary_moy;
	std::vector<Block* > blocks;
	vtx->getBlocks(blocks);
	for (std::vector<Block*>::iterator iter_bl=blocks.begin();
			iter_bl!=blocks.end(); ++iter_bl){
		bary_moy += (*iter_bl)->getBarycentre();
	}
	if (blocks.size()==0)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur avec commande de création d'un trou, on ne trouve pas de blocs reliés à un sommet donné à déplacer", TkUtil::Charset::UTF_8));

	bary_moy/=blocks.size();

	// calcul du déplacement
	Utils::Math::Point deplacement=(bary_moy-vtx->getCoord())/10.0;

	vtx->saveVertexGeomProperty(&getInfoCommand(), true);

	Utils::Math::Point newPt = vtx->getCoord()+deplacement;
	vtx->setCoord(newPt);
}
/*----------------------------------------------------------------------------*/
std::vector<Block*> CommandInsertHole::getConnectedBlocks()
{
	// m_filtre_block à 1 pour les blocs autorisés et non vus
	// à 2 dès qu'ils sont mis dans la liste
	// à 3 lorsqu'ils sont traités pour rechercher leur voisinage

	std::vector<Block*> blocs;

	// recherche d'un premier bloc marqué à 1
	Block* bl_dep = getBlock(1);
	if (bl_dep){
		blocs.push_back(bl_dep);
		m_filtre_block[bl_dep] = 2;
	}

	while(bl_dep) {
		m_filtre_block[bl_dep] = 3;
		// récupération des blocs voisins suivant voisinage par coface

		std::vector<Topo::CoFace*> cofaces;
		bl_dep->getCoFaces(cofaces);

		for (std::vector<Topo::CoFace*>::iterator iter_cf=cofaces.begin();
				iter_cf!=cofaces.end();++iter_cf)
			if (m_filtre_coface[*iter_cf] != 4){
				std::vector<Block* > blocks_vois;
				(*iter_cf)->getBlocks(blocks_vois);

				for (std::vector<Block*>::iterator iter_bl=blocks_vois.begin();
						iter_bl!=blocks_vois.end(); ++iter_bl)
					if (m_filtre_block[*iter_bl] == 1){
						blocs.push_back(*iter_bl);
						m_filtre_block[*iter_bl] = 2;
					}
			}

		// recherche d'un bloc dans la liste
		bl_dep = getBlock(blocs, 2);
	}

	return blocs;
}
/*----------------------------------------------------------------------------*/
Block* CommandInsertHole::getBlock(uint mark)
{
	Block* bloc = 0;
	std::map<Block*, uint>::iterator iter_bl;
	for (iter_bl = m_filtre_block.begin();
			iter_bl!=m_filtre_block.end(); ++iter_bl){
		if (iter_bl->second == mark){
			Block* bloc2 = iter_bl->first;
			if (bloc == 0 || bloc2->getUniqueId()<bloc->getUniqueId())
				bloc = bloc2;
		}
	}
	return bloc;
}
/*----------------------------------------------------------------------------*/
Block* CommandInsertHole::getBlock(std::vector<Block*>& blocs, uint mark)
{
	for (std::vector<Block*>::iterator iter_bl=blocs.begin();
			iter_bl!=blocs.end(); ++iter_bl)
		if (m_filtre_block[*iter_bl] == mark)
			return *iter_bl;
	return 0;
}
/*----------------------------------------------------------------------------*/
Vertex* CommandInsertHole::
duplicate(Vertex* ve)
{
	if (m_filtre_vertex[ve]!=4)
		return ve;

    Vertex* new_vtx = m_corr_vertex[ve];
    if (new_vtx == 0){

        new_vtx = new Vertex(getContext(), ve->getCoord());

        m_corr_vertex[ve] = new_vtx;

        getInfoCommand().addTopoInfoEntity(new_vtx, Internal::InfoCommand::CREATED);
    }
    return new_vtx;
}
/*----------------------------------------------------------------------------*/
CoEdge* CommandInsertHole::
duplicate(CoEdge* ce)
{
//#ifdef _DEBUG_INSERTHOLE
//	std::cout<<"duplicate("<<ce->getName()<<")"<<std::endl;
//#endif

	if (m_filtre_coedge[ce]!=4)
		return ce;

    CoEdge* new_coedge = m_corr_coedge[ce];
    if (new_coedge == 0){
        Vertex* new_vtx1 = duplicate(ce->getVertex(0));
        Vertex* new_vtx2 = duplicate(ce->getVertex(1));

        // pour le cas où le sommet n'est pas dupliqué, on va le modifier: lui ajouter une arête
        new_vtx1->saveVertexTopoProperty(&getInfoCommand());
        new_vtx2->saveVertexTopoProperty(&getInfoCommand());

        CoEdgeMeshingProperty* cemp = ce->getMeshingProperty()->clone();
        new_coedge = new CoEdge(getContext(), cemp, new_vtx1, new_vtx2);

        m_corr_coedge[ce] = new_coedge;

        getInfoCommand().addTopoInfoEntity(new_coedge, Internal::InfoCommand::CREATED);
    }
    return new_coedge;
}
/*----------------------------------------------------------------------------*/
Edge* CommandInsertHole::
duplicate(Edge* ed)
{
   Edge* new_edge = m_corr_edge[ed];

    if (new_edge == 0){

    	if (ed->getNbCoFaces() > 1){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
             message <<"Erreur interne, dans  CommandInsertHole::duplicate,"<<ed->getName()<<" appartient à plusieurs cofaces";
             throw TkUtil::Exception(message);
    	}

    	Vertex* new_vtx1 = duplicate(ed->getVertex(0));
    	Vertex* new_vtx2 = duplicate(ed->getVertex(1));

    	const std::vector<CoEdge*> & coedges = ed->getCoEdges();
    	std::vector<CoEdge*> new_coedges;
    	for (std::vector<CoEdge*>::const_iterator iter = coedges.begin();
    			iter != coedges.end(); ++iter)
    		new_coedges.push_back(duplicate(*iter));

    	new_edge = new Edge(getContext(), new_vtx1, new_vtx2, new_coedges);

    	// reprise des ratios
    	for (std::vector<CoEdge*>::const_iterator iter = coedges.begin();
    			iter != coedges.end(); ++iter){
    		uint ratio = ed->getRatio(*iter);
    		if (ratio > 1)
    			new_edge->setRatio(duplicate(*iter), ratio);
    	}
    	getInfoCommand().addTopoInfoEntity(new_edge, Internal::InfoCommand::CREATED);

    	m_corr_edge[ed] = new_edge;
    }
    return new_edge;
}
/*----------------------------------------------------------------------------*/
void CommandInsertHole::
separate(Edge* ed)
{
#ifdef _DEBUG_INSERTHOLE
    std::cout<<" separate pour "<<ed->getName()<<std::endl;
#endif
	std::vector<CoFace*> cofaces;
	ed->getCoFaces(cofaces);

	for (uint i=0; i<ed->getNbCoEdges(); i++)
		ed->getCoEdge(i)->saveCoEdgeTopoProperty(&getInfoCommand());

	for (uint i=1; i<cofaces.size(); i++){
		CoFace* coface = cofaces[i];
		Edge* new_edge = ed->clone();
#ifdef _DEBUG_INSERTHOLE
    std::cout<<"  => création par clone de "<<new_edge->getName()<<std::endl;
#endif
    	getInfoCommand().addTopoInfoEntity(new_edge, Internal::InfoCommand::CREATED);

		coface->replace(ed,new_edge,&getInfoCommand());
    }
}
/*----------------------------------------------------------------------------*/
CoFace* CommandInsertHole::
duplicate(CoFace* cf)
{
	if (m_filtre_coface[cf]!=4)
		return cf;

	CoFace* new_coface = m_corr_coface[cf];

    if (new_coface == 0){

        const std::vector<Vertex*> & vertices = cf->getVertices();
        std::vector<Vertex*> new_vertices;
        for (std::vector<Vertex*>::const_iterator iter = vertices.begin();
                iter != vertices.end(); ++iter)
            new_vertices.push_back(duplicate(*iter));

        const std::vector<Edge*> & edges = cf->getEdges();
        std::vector<Edge*> new_edges;
        for (std::vector<Edge*>::const_iterator iter = edges.begin();
                iter != edges.end(); ++iter)
            new_edges.push_back(duplicate(*iter));

        new_coface = new CoFace(getContext(), new_edges, new_vertices, cf->isStructured(), cf->hasHole());

        // copie des propriétés
        CoFaceMeshingProperty* new_ppty = cf->getCoFaceMeshingProperty()->clone();
        CoFaceMeshingProperty* old_ppty = new_coface->setProperty(new_ppty);
        delete old_ppty;

        m_corr_coface[cf] = new_coface;

        getInfoCommand().addTopoInfoEntity(new_coface, Internal::InfoCommand::CREATED);
    }

    return new_coface;
}
/*----------------------------------------------------------------------------*/
bool CommandInsertHole::findCoFace_Block_relation(CoEdge* coedge, CoFace* coface)
{
	// on se limite aux cofaces et blocs en relation avec cette arête
    std::map<Block*, uint> filtre_block;
    std::map<CoFace*, uint> filtre_coface;

    std::vector<Topo::CoFace*> cofaces;
    std::vector<Block* > blocks;
    coedge->getCoFaces(cofaces);
    coedge->getBlocks(blocks);

    for (std::vector<Topo::CoFace*>::iterator iter=cofaces.begin(); iter != cofaces.end(); ++iter)
    	filtre_coface[*iter] = 1;

    for (std::vector<Topo::Block*>::iterator iter=blocks.begin(); iter != blocks.end(); ++iter)
    	filtre_block[*iter] = 1;

    // on s'interdit la coface
    filtre_coface[coface] = 2;

    blocks.clear();
    coface->getBlocks(blocks);
	if (blocks.size()!=2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "CommandInsertHole::findCoFace_Block_relation pour " << coedge->getName()
		    	<< " et "<<coface->getName()<<", pb car on n'a pas 2 blocs voisins !";
		throw TkUtil::Exception(message);
	}
	Block* bl_dep = blocks[0];
	Block* bl_fin = blocks[1];

	// on part de bl_dep, on cherche à arriver à bl_fin en ne passant que par les blocs et cofaces marquées à 1
	while (bl_dep){
		// pour ne pas revenir sur ce bloc
		filtre_block[bl_dep] = 2;

		// une (unique il me semble) coface dans bl_dep et marquée à 1
		CoFace* coface_suiv = 0;
		cofaces.clear();
		bl_dep->getCoFaces(cofaces);
		for (std::vector<Topo::CoFace*>::iterator iter=cofaces.begin();
				iter != cofaces.end() && coface_suiv==0; ++iter)
			if (filtre_coface[*iter] == 1)
				coface_suiv = *iter;

		if (coface_suiv == 0)
			return false;

		// pour ne pas faire demi-tour
		filtre_coface[coface_suiv] = 2;

		// recherche du bloc suivant
		blocks.clear();
		coface_suiv->getBlocks(blocks);

		Block* bl_suiv = 0;
		for (std::vector<Topo::Block*>::iterator iter=blocks.begin(); iter != blocks.end(); ++iter)
			if (filtre_block[*iter] == 1)
				bl_suiv = *iter;
		if (bl_suiv == bl_fin)
			return true;

		// on continue avec ce bloc, ou on s'arête
		bl_dep = bl_suiv;
	}

	return false;
}
/*----------------------------------------------------------------------------*/
void CommandInsertHole::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewCoedges(dr);
}
/*----------------------------------------------------------------------------*/
void CommandInsertHole::clearMemory()
{
	m_corr_vertex.clear();
	m_corr_coedge.clear();
	m_corr_edge.clear();
	m_corr_coface.clear();
	m_corr_face.clear();
	m_corr_block.clear();

	m_filtre_block.clear();
	m_filtre_coface.clear();
	m_filtre_coedge.clear();
	m_filtre_vertex.clear();
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
