/*----------------------------------------------------------------------------*/
/*
 * \file SetNbMeshingEdgesImplementation.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 11/1/2019
 */
/*----------------------------------------------------------------------------*/
#include "Topo/SetNbMeshingEdgesImplementation.h"
#include "Topo/CoFace.h"
#include "Topo/CoEdge.h"
#include "Topo/Edge.h"

#include "Utils/Common.h"
#include "Topo/TopoHelper.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
//#define _DEBUG_SETNBMESHINGEDGES
/*----------------------------------------------------------------------------*/
SetNbMeshingEdgesImplementation::
SetNbMeshingEdgesImplementation(Internal::InfoCommand* icmd)
: m_icmd(icmd)
{
}
/*----------------------------------------------------------------------------*/
void SetNbMeshingEdgesImplementation::addFrozen(std::vector<CoEdge*>& frozed_coedges)
{
    // on marque comme vues les arêtes figées
    for (std::vector<CoEdge*>::iterator iter = frozed_coedges.begin();
    		iter != frozed_coedges.end(); ++iter)
    	m_coedges_vue[*iter] = true;
}
/*----------------------------------------------------------------------------*/
void SetNbMeshingEdgesImplementation::addDelta(CoEdge* ed, int nb)
{
    // initialisation: on impose un déséquilibre pour une arête
    m_coedge_delta[ed] = nb;

    // on mémorise cette arête pour démarrer les recherches
    m_coedges.push_back(ed);

    m_coedges_vue[ed] = true;

#ifdef _DEBUG_SETNBMESHINGEDGES
    std::cout<<"coedge_delta["<<ed->getName()<<"] = "<<m_coedge_delta[ed]<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void SetNbMeshingEdgesImplementation::addTaggedCoFace(CoFace* coface)
{
	if (coface)
		m_cofaces_vue[coface] = true;
	else
		throw TkUtil::Exception ("Erreur interne, dans SetNbMeshingEdgesImplementation::addTaggedCoFace");
}
/*----------------------------------------------------------------------------*/
SetNbMeshingEdgesImplementation::
~SetNbMeshingEdgesImplementation()
{
}
/*----------------------------------------------------------------------------*/
void SetNbMeshingEdgesImplementation::
execute()
{

#ifdef _DEBUG_SETNBMESHINGEDGES
    std::cout<<"### SetNbMeshingEdgesImplementation execute\n";
#endif

    // propage le delta aux arêtes voisines si l'arête est dite entière
    for (uint i=0; i<m_coedges.size(); i++)
    	propagateWholeCoedges(m_coedges[i]);

#ifdef _DEBUG_SETNBMESHINGEDGES
    std::cout<<"en sortie du 1er propagateWholeCoedges: m_coface_invalide.size() = "<<m_coface_invalide.size()<<std::endl;
#endif

    // boucle tant qu'il y a des cofaces repérées comme invalides
    while(!m_coface_invalide.empty()) {
    	// repart avec le dernier
    	CoFace* coface = m_coface_invalide.back().m_coface;
    	CoEdge* coedge_dep = m_coface_invalide.back().m_coedge;
    	m_coface_invalide.pop_back();

#ifdef _DEBUG_SETNBMESHINGEDGES
    std::cout<<"### redémarage avec "<<coface->getName()<<" et "<<coedge_dep->getName()<<std::endl;
#endif

    	if (m_cofaces_vue[coface])
    		continue;

    	// déséquilibre dans cette face
    	uint cote_dep = coface->getIndex(coedge_dep);
    	int delta = computeDelta(coface, cote_dep);

    	if (delta){
    		// recherche d'une arête en face non marquée

    		CoEdge* coedge_opp = findOppositeCoEdge(coedge_dep, cote_dep, coface);

        	if (coedge_opp){
        		m_coedge_delta[coedge_opp] = delta;
        		propagateWholeCoedges(coedge_opp);
        	}
        	else {
        		// on espère que le cas en sens contraire va arriver ...
#ifdef _DEBUG
        		std::cout<<" cas où on espère que le cas en sens contraire va arriver ..."<<coface->getName()<<" et "<<coedge_dep->getName()<<std::endl;
#endif
        	}
    	}
    	else {
    		// pas de delta, on arête ici
    		m_cofaces_vue[coface] = true;
    	}

    } // end while !m_coface_invalide.empty()

    // applique les deltas
    applyDeltas();
}
/*----------------------------------------------------------------------------*/
void SetNbMeshingEdgesImplementation::
propagateWholeCoedges(CoEdge* coedge)
{
	// on utilise une liste de coedges parallèles entre elles, mais avec uniquement des cofaces
	// ayant une unique coedge sur le côté qui fait face à l'arête traitée
	std::vector<CoEdge*> whole_coedges;
	whole_coedges.push_back(coedge);
	//m_coedges_vue[coedge] = true;

	for (uint i=0; i<whole_coedges.size(); i++){
		CoEdge* coedge_dep = whole_coedges[i];

		// ajoute à la liste les arêtes parallèles "entières",
		// et met de côté celles correspondant à une arête composée (que l'on traitera après)
		addParalelCoEdges(coedge_dep, whole_coedges);

	} // end for iter1=whole_coedges.begin()
}
/*----------------------------------------------------------------------------*/
void SetNbMeshingEdgesImplementation::
addParalelCoEdges(CoEdge* coedge_dep, std::vector<CoEdge*>& whole_coedges)
{
#ifdef _DEBUG_SETNBMESHINGEDGES
	std::cout<<"addParalelCoEdges("<<coedge_dep->getName()<<", whole_coedges.size() "<<whole_coedges.size()<<")"<<std::endl;
#endif

	// les faces structurées reliées à l'arête
	std::vector<CoFace* > cofaces;
	getStructuredCoFaces(coedge_dep, cofaces);

    for (std::vector<CoFace*>::iterator iter = cofaces.begin();
            iter != cofaces.end(); ++iter){
    	CoFace* coface = *iter;
#ifdef _DEBUG_SETNBMESHINGEDGES
    	std::cout<<"- coface = "<<coface->getName()<<std::endl;
#endif
   	if (m_cofaces_vue[coface])
    		continue;

    	// coté touché par le déséquilibre
    	uint cote_dep = coface->getIndex(coedge_dep);
    	uint cote_opp = (cote_dep+2)%4;
    	// cas d'une face dégénérée en triangle
    	if (cote_opp == 3 && coface->getNbEdges() == 3)
    		continue;

    	// déséquilibre dans cette face
		int delta = computeDelta(coface, cote_dep);

		Edge* edge_opp = coface->getEdge(cote_opp);
    	if (edge_opp->getNbCoEdges() == 1){
    		CoEdge* coedge_opp = edge_opp->getCoEdge(0);

    		if (m_coedges_vue[coedge_opp]){
    			// reste à vérifier que la coface est équilibrée
    			if (delta){
    				Edge* edge_dep = coface->getEdge(cote_dep);
    				if (edge_dep->getNbCoEdges() == 1){
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    					message << "La modification a échouée, elle est bloquée sur la face commune "<< coface->getName();
    					message<<"\n avec une première arête "<<coedge_dep->getName()<<" et delta de "<<(short)m_coedge_delta[coedge_dep];
    					message<<"\n et une deuxième arête "<<coedge_opp->getName()<<" et delta de "<<(short)m_coedge_delta[coedge_opp];
    					message<<"\nIl y a peut-être trop de contraintes.";
    					throw TkUtil::Exception (message);
    				}
    				else {
    					// cas où on peut espérer repartir dans l'autre direction
    					CoFace_CoEdge cc = {coface, coedge_opp};
    					m_coface_invalide.push_back(cc);
#ifdef _DEBUG_SETNBMESHINGEDGES
    					std::cout<<" on ajoute à coface_invalide la coface "<<coface->getName()
    							<<", cote_opp "<<cote_opp<<", delta = "<<delta
    							<<", coedge_opp "<<coedge_opp->getName()<<std::endl;
#endif
    				}
    			} // if delta
    			else {
    				// face ok
    				m_cofaces_vue[coface] = true;
#ifdef _DEBUG_SETNBMESHINGEDGES
    				std::cout<<" coface ok"<<std::endl;
#endif
    			} // else / if delta
    		} // if m_coedges_vue
    		else {
#ifdef _DEBUG_SETNBMESHINGEDGES
    			std::cout<<" on ajoute à whole_coedges la coedge "<<coedge_opp->getName()
    					 <<", cote_opp "<<cote_opp<<", delta = "<<delta<<std::endl;
#endif
    			whole_coedges.push_back(coedge_opp);
    			m_coedge_delta[coedge_opp] = delta;
    			m_coedges_vue[coedge_opp] = true;
    			m_cofaces_vue[coface] = true;
    		} // else / if m_coedges_vue

    	} // if edge_opp->getNbCoEdges() == 1
    	else {
    		// cas avec un choix à faire sur l'arête qui devra être déséquilibrée
    		// on reporte cela à plus tard
			CoFace_CoEdge cc = {coface, coedge_dep};
			m_coface_invalide.push_back(cc);
#ifdef _DEBUG_SETNBMESHINGEDGES
			std::cout<<" on ajoute à coface_invalide la coface "<<coface->getName()
					 <<", cote_dep "<<cote_dep<<", delta = "<<delta
					 <<", coedge_dep "<<coedge_dep->getName()<<std::endl;
#endif
    	} // else / if edge_opp->getNbCoEdges() == 1
    } // end for iter
}
/*----------------------------------------------------------------------------*/
CoEdge* SetNbMeshingEdgesImplementation::
findOppositeCoEdge(CoEdge* coedge_dep, uint cote_dep, CoFace* coface)
{
	CoEdge* coedge_opp = 0;

	uint cote_opp = (cote_dep+2)%4;
	Edge* edge_dep = coface->getEdge(cote_dep);
	Edge* edge_opp = coface->getEdge(cote_opp);

	if (coface->getNbVertices() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Erreur interne, cas avec face "<< coface->getName()
				<<" avec autre chose que 4 côtés pas encore pris en compte.";
		throw TkUtil::Exception (message);
	}

	// recherche des arêtes triées côté départ
	Vertex* vtx1_dep = coface->getVertex(cote_dep);
	Vertex* vtx2_dep = coface->getVertex((cote_dep+1)%4);
	std::vector<CoEdge*> coedges_dep;
	edge_dep->getCoEdges(coedges_dep);
	std::vector<CoEdge*> coedges_dep_sort;
	TopoHelper::getCoEdgesBetweenVertices(vtx1_dep, vtx2_dep, coedges_dep, coedges_dep_sort);

	// idem côté opposé
	Vertex* vtx1_opp = coface->getVertex((cote_dep+3)%4);
	Vertex* vtx2_opp = coface->getVertex((cote_dep+2)%4);
	std::vector<CoEdge*> coedges_opp;
	edge_opp->getCoEdges(coedges_opp);
	std::vector<CoEdge*> coedges_opp_sort;
	TopoHelper::getCoEdgesBetweenVertices(vtx1_opp, vtx2_opp, coedges_opp, coedges_opp_sort);

	// Recherche les indices des noeuds extrémités de l'arête coedge_dep dans edge_dep
	uint ind_nd1_dep = 0;
	uint ind_nd2_dep = 0;
	uint i=0;
	for (i=0; i<coedges_dep_sort.size() && coedge_dep!=coedges_dep_sort[i]; i++)
		ind_nd1_dep += coedges_dep_sort[i]->getNbMeshingEdges();
	if (coedge_dep!=coedges_dep_sort[i]){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Erreur interne, on ne retrouve pas l'arête commune "<< coedge_dep->getName()
								<<" dans l'arête "<<edge_dep->getName();
		throw TkUtil::Exception (message);
	}
	ind_nd2_dep = ind_nd1_dep + coedges_dep_sort[i]->getNbMeshingEdges();

	// Recherche la liste des arêtes communes dont les noeuds ont des indices compris entre
	// les indices de ceux de l'arête précédente
	// on se limite aux arêtes non vues
	std::vector<CoEdge*> coedges_inner;
	uint ind_nd1_opp = 0;
	for (i=0; i<coedges_opp_sort.size(); i++){
		uint ind_nd2_opp = ind_nd1_opp+coedges_opp_sort[i]->getNbMeshingEdges();

		if (ind_nd1_opp>=ind_nd1_dep && ind_nd2_opp<=ind_nd2_dep && !m_coedges_vue[coedges_opp_sort[i]])
			coedges_inner.push_back(coedges_opp_sort[i]);

		// indice pour l'arête suivante
		ind_nd1_opp = ind_nd2_opp;
	}

	// on prend la première des arêtes possibles
	if (!coedges_inner.empty())
		coedge_opp = coedges_inner.front();

#ifdef _DEBUG_SETNBMESHINGEDGES
	std::cout<<" findOppositeCoEdge("<<coedge_dep->getName()
    		 <<", "<<coface->getName()<<") => "
    		 <<(coedge_opp?coedge_opp->getName():"rien")<<std::endl;
#endif

	return  coedge_opp;
}
/*----------------------------------------------------------------------------*/
int SetNbMeshingEdgesImplementation::
computeDelta(CoFace* coface, uint cote)
{
    //std::cout<<"computeDelta ...\n";

    uint cote_opp = (cote+2)%4;

    if (cote_opp == 3 && coface->getNbEdges() == 3)
        return 0;
    else {
        std::vector<CoEdge* > coedges;

        int nb_init = 0;
        coface->getEdge(cote)->getCoEdges(coedges);
        for (std::vector<CoEdge* >::iterator iter = coedges.begin();
                iter != coedges.end(); ++iter){
            nb_init += (*iter)->getNbMeshingEdges() + m_coedge_delta[*iter];
            //std::cout<<"nb_init += "<<(*iter)->getNbMeshingEdges()<<" + "<<m_coedge_delta[*iter]<<std::endl;
        }

        int nb_opp = 0;
        coface->getEdge(cote_opp)->getCoEdges(coedges);
        for (std::vector<CoEdge* >::iterator iter = coedges.begin();
                iter != coedges.end(); ++iter){
            nb_opp += (*iter)->getNbMeshingEdges() + m_coedge_delta[*iter];
            //std::cout<<"nb_opp += "<<(*iter)->getNbMeshingEdges()<<" + "<<m_coedge_delta[*iter]<<std::endl;
        }

#ifdef _DEBUG_SETNBMESHINGEDGES
        std::cout<<"  computeDelta pour coface "<<coface->getName()<<" => "<<nb_init - nb_opp<<std::endl;
#endif
        return (nb_init - nb_opp);
    }
}
/*----------------------------------------------------------------------------*/
void SetNbMeshingEdgesImplementation::
getStructuredCoFaces(CoEdge* coedge, std::vector<CoFace* >& cofaces)
{
    std::vector<CoFace* > all_cofaces;
    coedge->getCoFaces(all_cofaces);

    for (std::vector<CoFace* >::iterator iter = all_cofaces.begin();
            iter != all_cofaces.end(); ++iter)
        if ((*iter)->isStructured())
            cofaces.push_back(*iter);
}
/*----------------------------------------------------------------------------*/
void SetNbMeshingEdgesImplementation::
applyDeltas()
{
#ifdef _DEBUG_SETNBMESHINGEDGES
	std::cout<<"applyDeltas() ..."<<std::endl;
#endif
    // on parcours toutes les arêtes rencontrées et l'on modifie la discrétisation
    // en fonction de ce qui a été demandé
    for (std::map<CoEdge*, int>::iterator iter = m_coedge_delta.begin();
            iter != m_coedge_delta.end(); ++iter){
        CoEdge* coedge = iter->first;
        int delta = iter->second;
        if (delta != 0){
            CoEdgeMeshingProperty* mp = coedge->getMeshingProperty();
            CoEdgeMeshingProperty* new_mp = mp->clone();

#ifdef _DEBUG_SETNBMESHINGEDGES
            std::cout<<"setNbEdges("<<new_mp->getNbEdges()<<" -> "<<new_mp->getNbEdges()+delta<<") pour "<<coedge->getName()<<std::endl;
#endif

            new_mp->setNbEdges(new_mp->getNbEdges()+delta);

            coedge->switchCoEdgeMeshingProperty(m_icmd, new_mp);
            delete new_mp;

        } // end if (delta != 0)
    } // end for iter = coedge_delta.begin()
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
