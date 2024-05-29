/*----------------------------------------------------------------------------*/
/*
 * \file CommandSplitFacesWithOgrid.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/11/2017
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSplitFacesWithOgrid.h"

#include "Utils/Common.h"
#include "Topo/TopoHelper.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"
#include "Group/Group2D.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_SPLIT_OGRID
/*----------------------------------------------------------------------------*/
CommandSplitFacesWithOgrid::
CommandSplitFacesWithOgrid(Internal::Context& c,
        std::vector<Topo::CoFace* > &cofaces,
        std::vector<Topo::CoEdge* > &coedges,
        double ratio_ogrid,
        int nb_bras)
:CommandEditTopo(c, "Découpage des faces structurées en O-grid")
, m_ratio_ogrid(ratio_ogrid)
, m_nb_meshing_edges(nb_bras)
{
    if (ratio_ogrid<=0.0 || ratio_ogrid>=1.0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le ratio doit être dans l'interval ]0 1[", TkUtil::Charset::UTF_8));

    if (nb_bras<=0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le nombre de bras doit être d'au moins 1", TkUtil::Charset::UTF_8));

    // on ne concerve que les blocs structurés et non dégénérés
    for (std::vector<Topo::CoFace* >::iterator iter = cofaces.begin();
            iter != cofaces.end(); ++iter){
        Topo::CoFace* hf = *iter;
        if (hf->isStructured() && hf->getNbVertices() == 4)
        	m_cofaces.push_back(hf);
        else{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Il n'est pas prévu de faire un découpage en o-grid dans une face non structurée ou dégénérée\n";
            message << "("<<hf->getName()<<" n'est pas structurée)";
            throw TkUtil::Exception(message);
        }
    }

    // la validité de la sélection se fera lors de la création des filtres
    m_coedges.insert(m_coedges.end(), coedges.begin(), coedges.end());

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Découpage en O-grid des faces structurées";
	for (uint i=0; i<cofaces.size() && i<5; i++)
		comments << " " << cofaces[i]->getName();
	if (cofaces.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSplitFacesWithOgrid::
~CommandSplitFacesWithOgrid()
{
}
/*----------------------------------------------------------------------------*/
void CommandSplitFacesWithOgrid::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSplitFacesWithOgrid::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"CommandSplitFacesWithOgrid::internalExecute"<<std::endl;
#endif

    // Filtres:  à 1 pour ce qui est sur le bord et à 2 ce qui est à l'intérieur
    std::map<Vertex*, uint> filtre_vertex;
    std::map<CoEdge*, uint> filtre_coedge;
    std::map<CoFace*, uint> filtre_coface;
    // Filtre pour les Edges à 1 si toutes ses coedges sont à conserver comme un ensemble,
    // à 2 si les coedges sont à prendre séparément
    std::map<Edge*,  uint> filtre_edge;

    // Renseigne les filtres
    computeFilters(filtre_vertex, filtre_coedge, filtre_edge, filtre_coface);

    // Sauvegarde les relations topologiques pour permettre le undo/redo
    saveTopoProperty(filtre_vertex, filtre_coedge, filtre_coface);


    // relation entre sommet initial et sommet au centre de l'o-grid
    std::map<Vertex*, Vertex*> corr_vertex;

    // Création des sommets au centre de l'o-grid
    createVertices(filtre_vertex, filtre_coedge, corr_vertex);


    // relation entre sommet initial et arête vers le centre de l'o-grid
    std::map<Vertex*, CoEdge*> corr_vtx_coedge;

    // pour chaques sommets, création d'une arête vers le centre de l'o-grid
    createCoEdges(filtre_vertex, corr_vertex, corr_vtx_coedge);


    // relation entre couple de sommets et arête interne
    std::map<std::pair<Vertex*, Vertex*>, CoEdge*> corr_2vtx_coedge;
    // relation entre couple de sommets et face commune interne
    std::map<std::pair<Vertex*, Vertex*>, CoFace*> corr_2vtx_coface;

    // Relation entre couple de sommets et arête/face interne
    createCoEdgeAndFace(filtre_vertex, corr_vertex, corr_vtx_coedge,
            filtre_coedge, filtre_coface, filtre_edge,
            corr_2vtx_coedge, corr_2vtx_coface);

    // relation entre face commune issues de la sélection et celles au centre
    std::map<CoFace*, CoFace*> corr_coface;

    // Création des cofaces au centre, issues de la sélection
    createCoFace(filtre_vertex, corr_vertex, corr_2vtx_coedge, filtre_coface, corr_coface);


    // Supression des entités initiales qui ne sont plus utilisées: les faces internes et les blocs,
    // réutilisation des projections pour les faces internes
    freeUnused(filtre_vertex, filtre_coedge, filtre_coface,
            corr_vtx_coedge, corr_2vtx_coface, corr_coface);


    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

//    std::cout<<"Dans CommandSplitFacesWithOgrid: "<<std::endl;
//    std::cout<< getInfoCommand() <<std::endl;

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandSplitFacesWithOgrid::
computeFilters(std::map<Vertex*, uint> & filtre_vertex,
            std::map<CoEdge*, uint> & filtre_coedge,
            std::map<Edge*,  uint> & filtre_edge,
            std::map<CoFace*, uint> & filtre_coface)
{
    // premier passage pour compter le nombre de faces (sélectionnées) en relation avec arêtes communes
    // et on marque à 2 tous les sommets, arêtes
    // on met à 3 les arêtes internes ne touchant pas la peau
    for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
            iter1 != m_cofaces.end(); ++iter1){
        CoFace* coface = *iter1;

        // les blocs
        filtre_coface[coface] = 2;

        // les sommets
        std::vector<Vertex* > vertices;
        coface->getAllVertices(vertices);
        for (uint i=0; i<vertices.size(); i++)
            filtre_vertex[vertices[i]] = 2;

        // on incrémente à chaque fois que l'on voit l'arête commune
        std::vector<CoEdge* > coedges;
        coface->getCoEdges(coedges);
        for (uint i=0; i<coedges.size(); i++)
            filtre_coedge[coedges[i]] += 1;

    } // end for iter1


    // on force les coedges sélectionnées pour qu'elles soient traités
    // comme si elles étaient entre 2 cofaces sélectionnées
    std::vector<CoFace* > addedCoFaces;
    for (std::vector<CoEdge* >::iterator iter3 = m_coedges.begin();
            iter3 != m_coedges.end(); ++iter3){
        CoEdge* coedge = *iter3;
        bool erreur = false;

        std::vector<CoFace* > cofaces;
        coedge->getCoFaces(cofaces);
        if (cofaces.size() == 2){
            uint nb_facesAjoutes = 0;
            // dans se cas on ajoute simplement la face voisine
            for (uint i=0; i<cofaces.size(); i++)
                if (filtre_coface[cofaces[i]] == 0){
                    m_cofaces.push_back(cofaces[i]);
                    addedCoFaces.push_back(cofaces[i]);
                    nb_facesAjoutes += 1;
                }
            if (nb_facesAjoutes != 1)
                erreur = true;
        }
        else if (cofaces.size() == 1)
            if (filtre_coedge[coedge] == 1)
            	filtre_coedge[coedge] = 2;
            else
                erreur = true;

        if (erreur){
            // cas d'une coedge qui n'est pas sur le bord du groupe de faces sélectionnées
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"L'arête "<<coedge->getName()<<" n'est pas prise en compte pour le découpage";
            log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
        }
    } // end for iter3

    // on recommence pour les faces ajoutées
    for (std::vector<CoFace* >::iterator iter1 = addedCoFaces.begin();
            iter1 != addedCoFaces.end(); ++iter1){
    	CoFace* coface = *iter1;

        // les cofaces
        filtre_coface[coface] = 2;

        // les sommets
        std::vector<Vertex* > vertices;
        coface->getAllVertices(vertices);
        for (uint i=0; i<vertices.size(); i++)
            filtre_vertex[vertices[i]] = 2;

        // on incrémente à chaque fois que l'on voit l'arête commune
        std::vector<CoEdge* > coedges;
        coface->getCoEdges(coedges);
        for (uint i=0; i<coedges.size(); i++)
            filtre_coedge[coedges[i]] += 1;

    } // end for iter1


    // constitution de la liste de coedges au bord
    std::vector<CoEdge*> coedgesAuBord;
    for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
            iter1 != m_cofaces.end(); ++iter1){

    	CoFace* coface = *iter1;
        std::vector<CoEdge* > coedges;
        coface->getCoEdges(coedges);
        for (uint i=0; i<coedges.size(); i++)
            if (filtre_coedge[coedges[i]] == 1)
                coedgesAuBord.push_back(coedges[i]);
    } // end for iter1

    // pour les arêtes au bord, on marque les sommets et faces
    // à 1 (au bord)
    for (std::vector<CoEdge*>::iterator iter3 = coedgesAuBord.begin();
            iter3 != coedgesAuBord.end(); ++iter3){
        CoEdge* coedge = *iter3;
#ifdef _DEBUG_SPLIT_OGRID
            std::cout<<"CoEdge au bord : "<<coedge->getName()<<std::endl;
#endif

        // les sommets au bord UTILE ?
//        std::vector<CoEdge* > coedges;
//        coface->getCoEdges(coedges);
//        for (uint i=0; i<coedges.size(); i++)
//            filtre_coedge[coedges[i]] = 1;

        // les faces sélectionnées qui touchent le bord
        std::vector<CoFace* > cofaces;
        coedge->getCoFaces(cofaces);
        for (uint i=0; i<cofaces.size(); i++)
        	if (filtre_coface[cofaces[i]] == 2)
        		filtre_coface[cofaces[i]] = 1;
    } // end for iter3

    // filtre pour les arêtes
    for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
            iter1 != m_cofaces.end(); ++iter1){

    	CoFace* coface = *iter1;

        std::vector<Edge* > edges;
        coface->getEdges(edges);

        for (std::vector<Edge* >::iterator iter2 = edges.begin();
                iter2 != edges.end(); ++iter2){

            Edge* edge = *iter2;
            // recherche des nombres d'arêtes communes internes et externes dans cette arête
            uint nb_arete_int = 0;
            uint nb_arete_ext = 0;

            std::vector<CoEdge* > coedges;
            edge->getCoEdges(coedges);

            for (std::vector<CoEdge* >::iterator iter3 = coedges.begin();
                    iter3 != coedges.end(); ++iter3)
                if (filtre_coedge[*iter3] == 1)
                    nb_arete_ext += 1;
                else if (filtre_coedge[*iter3] == 2)
                    nb_arete_int += 1;
#ifdef _DEBUG_SPLIT_OGRID
            std::cout<<"Edge : "<<edge->getName()<<", nb_arete_ext: "<<nb_arete_ext<<", nb_arete_int: "<<nb_arete_int<<std::endl;
#endif

            if (nb_arete_int == 0){
                filtre_edge[edge] = 1;
                std::vector<Vertex* > vertices;
                edge->getVertices(vertices);
                for (uint i=0; i<vertices.size(); i++)
                	filtre_vertex[vertices[i]] = 1;
            }
            else {
                filtre_edge[edge] = 2;

                // pour ces arete particulières, on marques tous les noeuds des aretes au bord
                for (std::vector<CoEdge* >::iterator iter3 = coedges.begin();
                        iter3 != coedges.end(); ++iter3){
                    CoEdge* coedge = *iter3;
                    if (filtre_coedge[coedge] == 1){
                        std::vector<Vertex* > vertices;
                        coedge->getVertices(vertices);
                        for (uint i=0; i<vertices.size(); i++)
                            filtre_vertex[vertices[i]] = 1;
                    }
                }
            }
        }
    }

    	// pour les faces internes, on marque ses arêtes
    for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
    		iter1 != m_cofaces.end(); ++iter1){

    	CoFace* coface = *iter1;

    	if (filtre_coface[coface] == 2){

#ifdef _DEBUG_SPLIT_OGRID
    		std::cout<<"coface interne : "<<coface->getName()<<std::endl;
#endif

    		// les arêtes communes
    		std::vector<CoEdge* > coedges;
    		coface->getCoEdges(coedges);
    		for (uint i=0; i<coedges.size(); i++)
    			filtre_coedge[coedges[i]] = 3;

    	}
    }

    // constitution de la liste de arêtes au bord
    std::vector<Edge*> aretesAuBord;
    for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
    		iter1 != m_cofaces.end(); ++iter1){

    	CoFace* coface = *iter1;
    	std::vector<Edge* > edges;
    	coface->getEdges(edges);
    	for (uint i=0; i<edges.size(); i++)
    		if (filtre_edge[edges[i]] == 1)
    			aretesAuBord.push_back(edges[i]);
    } // end for iter1

    // pour les arêtes au bord, on marque les sommets,
    // à 1 (au bord)
    for (std::vector<Edge*>::iterator iter3 = aretesAuBord.begin();
    		iter3 != aretesAuBord.end(); ++iter3){
    	Edge* edge = *iter3;

    	std::vector<Vertex* > vertices;
    	edge->getVertices(vertices);
    	for (uint i=0; i<vertices.size(); i++)
    		if (filtre_vertex[vertices[i]] == 2)
    			filtre_vertex[vertices[i]] = 1;

    	edge->getAllVertices(vertices);
    	for (uint i=0; i<vertices.size(); i++)
    		if (filtre_vertex[vertices[i]] == 2){
    			// par défaut ce sommet est à ignorer
    			filtre_vertex[vertices[i]] = 5;
    			// recherche si ce sommet est relié à une arête interne (filtre à 2)
    			// si oui, alors on le considère au bord
    			std::vector<CoEdge* > coedges;
    			vertices[i]->getCoEdges(coedges);
    			for (uint j=0; j<coedges.size(); j++)
    				if (filtre_coedge[coedges[j]] == 2)
    					filtre_vertex[vertices[i]] = 1;
    		}

    }

    // on ne touchera pas aux arêtes entièrement à l'intérieur (celles avec les 2 sommets internes)
    for (std::map<CoEdge*, uint>::iterator iter_ce=filtre_coedge.begin();
    		iter_ce!=filtre_coedge.end(); ++iter_ce)
    	if (iter_ce->second == 2
    			&& filtre_vertex[iter_ce->first->getVertex(0)] == 2
				&& filtre_vertex[iter_ce->first->getVertex(1)] == 2)
    		iter_ce->second = 3;


#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"## computeFilters => "<<std::endl;
    std::cout<<"filtre_vertex : "<<std::endl;
    for (std::map<Vertex*, uint>::iterator iter=filtre_vertex.begin();
    		iter!=filtre_vertex.end(); ++iter){
    	std::cout<<"  "<<iter->first->getName()<<" : "<<iter->second<<std::endl;
    } // end for iter
    std::cout<<"filtre_coedge : "<<std::endl;
    for (std::map<CoEdge*, uint>::iterator iter=filtre_coedge.begin();
    		iter!=filtre_coedge.end(); ++iter){
    	std::cout<<"  "<<iter->first->getName()<<" : "<<iter->second<<std::endl;
    } // end for iter
    std::cout<<"filtre_edge : "<<std::endl;
    for (std::map<Edge*, uint>::iterator iter=filtre_edge.begin();
    		iter!=filtre_edge.end(); ++iter){
    	std::cout<<"  "<<iter->first->getName()<<" : "<<iter->second<<std::endl;
    } // end for iter
    std::cout<<"filtre_coface : "<<std::endl;
    for (std::map<CoFace*, uint>::iterator iter=filtre_coface.begin();
    		iter!=filtre_coface.end(); ++iter){
    	std::cout<<"  "<<iter->first->getName()<<" : "<<iter->second<<std::endl;
    } // end for iter
#endif
}
/*----------------------------------------------------------------------------*/
void CommandSplitFacesWithOgrid::saveTopoProperty(std::map<Vertex*, uint> & filtre_vertex,
        std::map<CoEdge*, uint> & filtre_coedge,
        std::map<CoFace*, uint> & filtre_coface)
{
    for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
             iter1 != m_cofaces.end(); ++iter1){

    	CoFace* coface = *iter1;

    	// seules les faces communes internes ne sont pas touchées
         if (filtre_coface[coface] != 3){
             // les sommets
              std::vector<Vertex* > vertices;
              coface->getVertices(vertices);
              for (uint i=0; i<vertices.size(); i++)
                  vertices[i]->saveVertexTopoProperty(&getInfoCommand());

              std::vector<Edge* > edges;
              coface->getEdges(edges);
              for (std::vector<Edge* >::iterator iter4 = edges.begin();
            		  iter4 != edges.end(); ++iter4){
            	  Edge* edge = *iter4;

            	  edge->saveEdgeTopoProperty(&getInfoCommand());

            	  std::vector<CoEdge* > coedges;
            	  edge->getCoEdges(coedges);
            	  for (std::vector<CoEdge* >::iterator iter5 = coedges.begin();
            			  iter5 != coedges.end(); ++iter5){
            		  CoEdge* coedge = *iter5;

            		  // seules les arêtes communes internes ne sont pas touchées
					  if (filtre_coedge[coedge] != 3)
						  coedge->saveCoEdgeTopoProperty(&getInfoCommand());
					  //std::cout<<"saveCoEdgeTopoProperty pour "<<coedge->getName()<<std::endl;
            	  } // end for iter5
              } // end for iter4
         } // end if filtre ...
     } // end for iter1

}
/*----------------------------------------------------------------------------*/
void CommandSplitFacesWithOgrid::
createVertices(std::map<Vertex*, uint> & filtre_vertex,
        std::map<CoEdge*, uint> & filtre_coedge,
        std::map<Vertex*, Vertex*> & corr_vertex)
{
#ifdef _DEBUG_SPLIT_OGRID
	std::cout<<"CommandSplitFacesWithOgrid::createVertices(...)"<<std::endl;
#endif
    for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
            iter1 != m_cofaces.end(); ++iter1){
        CoFace* coface = *iter1;

        // les sommets extrémités de la face
        std::vector<Vertex* > vertices;
        coface->getVertices(vertices);

        // recherche du barycentre de la face
        Utils::Math::Point barycentre_cf;
        for (uint i=0; i<vertices.size(); i++)
            barycentre_cf += vertices[i]->getCoord();
        barycentre_cf /= (double)vertices.size();

        coface->getAllVertices(vertices);

        // création des sommets pour la face centrale
        for (std::vector<Vertex* >::iterator iter6 = vertices.begin();
                iter6 != vertices.end(); ++iter6){
            Vertex* sommet = *iter6;
            Vertex* newVtx = corr_vertex[sommet];

#ifdef _DEBUG_SPLIT_OGRID
            std::cout<<"sommet = "<<sommet->getName()<<", filtre à "<<filtre_vertex[sommet]<<std::endl;
#endif
            if (filtre_vertex[sommet] == 2){
                // les sommets internes ne bougent pas
                corr_vertex[sommet] = sommet;
            }
            else if (0 == newVtx){
                // cas où le sommet n'a pas été construit pour une autre face

                // recherche des nombres d'arêtes communes internes et externes liées à ce sommet
                uint nb_arete_int = 0;
                uint nb_arete_ext = 0;
                // on conserve les arêtes internes
                std::vector<CoEdge* > coedges_int;

                std::vector<CoEdge* > coedges;
                sommet->getCoEdges(coedges);

                for (std::vector<CoEdge* >::iterator iter3 = coedges.begin();
                        iter3 != coedges.end(); ++iter3){
#ifdef _DEBUG_SPLIT_OGRID
                	std::cout<<"  coedge = "<<(*iter3)->getName()<<", filtre à "<<filtre_coedge[*iter3]<<std::endl;
#endif
                    if (filtre_coedge[*iter3] == 1)
                        nb_arete_ext += 1;
                    else if (filtre_coedge[*iter3] == 2){
                    	coedges_int.push_back(*iter3);
                        nb_arete_int += 1;
                    }
                }
#ifdef _DEBUG_SPLIT_OGRID
                std::cout<<"nb_arete_int = "<<nb_arete_int<<std::endl;
                std::cout<<"nb_arete_ext = "<<nb_arete_ext<<std::endl;
#endif

                if (nb_arete_int == 0){
#ifdef _DEBUG_SPLIT_OGRID
                	std::cout<<"il n'est relié à aucune arête sélectionnée"<<std::endl;
#endif
                    Utils::Math::Point pt = (barycentre_cf + (sommet->getCoord() - barycentre_cf)*m_ratio_ogrid);
                    newVtx = new Topo::Vertex(getContext(), pt);
                    getInfoCommand().addTopoInfoEntity(newVtx, Internal::InfoCommand::CREATED);
                    corr_vertex[sommet] = newVtx;
                }
//                else if ((nb_arete_int >= 3)
//                        || (nb_arete_int == 2 && nb_arete_ext == 1)
////                        || (nb_face_int == 3 && nb_face_ext == 2)
//                        ){
//                    // il est relié à au moins 3 faces sélectionnées
//                    // ou une seule face mais avec 2 arêtes de sélectionnées
//                    // ou relié à 2 faces mais avec 2 arêtes de sélectionnées
//                    // on recherche le sommet marqué à 1 ou plus en partant de ce sommet
//                    // et en utilisant seulement des arêtes marquées à 2
//                    // (arêtes internes à la sélection, entre bord et sélection)
//                    Vertex* vtx_opp;
//                    std::map<CoEdge*, uint> filtre_vu;
//                    Vertex* vtx_prec = sommet;
//
//                    // les coedges utilisées
//                    std::vector<CoEdge*> coedges_int;
//
//                    do {
//#ifdef _DEBUG_SPLIT_OGRID
//                    	std::cout<<"vtx_prec : "<<vtx_prec->getName()<<std::endl;
//#endif
//                        std::vector<CoEdge* > coedges;
//                        vtx_prec->getCoEdges(coedges);
//                        CoEdge* coedge = 0;
//                        for (uint i=0; i<coedges.size(); i++)
//                            if (filtre_vu[coedges[i]] == 0 && filtre_coedge[coedges[i]] == 2)
//                                coedge = coedges[i];
//
//                        if (0 == coedge)
//                            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitFacesWithOgrid::createVertices, on ne trouve pas de sommet opposé au sommet marqué à 4");
//
//                        // pour ne pas revenir en arrière
//                        filtre_vu[coedge] = 1;
//                        coedges_int.push_back(coedge);
//#ifdef _DEBUG_SPLIT_OGRID
//                        std::cout<<"coedge : "<<coedge->getName()
//                        		<<", filtre_vu = "<<filtre_vu[coedge]
//                        		<<", filtre_coedge = "<<filtre_coedge[coedge]
//                        		<<std::endl;
//#endif
//                        vtx_opp = coedge->getOppositeVertex(vtx_prec);
//
//                        vtx_prec = vtx_opp;
//                    } while (filtre_vertex[vtx_opp] == 0);
//
//#ifdef _DEBUG_SPLIT_OGRID
//                    std::cout<<"vtx_opp : "<<vtx_opp->getName()<<std::endl;
//#endif
//                    Utils::Math::Point barycentre_edge = (sommet->getCoord() + vtx_opp->getCoord())/2;
//
//                    Utils::Math::Point pt = (barycentre_edge + (sommet->getCoord() - barycentre_edge)*m_ratio_ogrid);
//                    newVtx = new Topo::Vertex(getContext(), pt);
//                    getInfoCommand().addTopoInfoEntity(newVtx, Internal::InfoCommand::CREATED);
//                    corr_vertex[sommet] = newVtx;
//
//                    // utilisation de la projection de[s] coedges vues si possible
//                    if (coedges_int.size() == 1 && coedges_int[0]->getGeomAssociation()){
//                        newVtx->setGeomAssociation(coedges_int[0]->getGeomAssociation());
//                        filtre_vertex[newVtx] = 4;
//                    }
//
//                }
                else if (nb_arete_int == 1){
#ifdef _DEBUG_SPLIT_OGRID
                	std::cout<<"il n'est relié qu'à une seule arête sélectionnée"<<std::endl;
#endif
                    CoEdge* coedge = coedges_int[0];

                    // calcul le barycentre de la CoEdge
                    std::vector<Vertex* > vertices_ce;
                    coedge->getVertices(vertices_ce);
                    Utils::Math::Point barycentre_ce = (vertices_ce[0]->getCoord()+vertices_ce[1]->getCoord())/2;

                    Utils::Math::Point pt = (barycentre_ce + (sommet->getCoord() - barycentre_ce)*m_ratio_ogrid);
                    newVtx = new Topo::Vertex(getContext(), pt);
                    getInfoCommand().addTopoInfoEntity(newVtx, Internal::InfoCommand::CREATED);
                    corr_vertex[sommet] = newVtx;
                    // utilisation de la projection de la coedge vue si possible
                    if (coedges_int[0]->getGeomAssociation()){
                    	newVtx->setGeomAssociation(coedges_int[0]->getGeomAssociation());
                    	filtre_vertex[newVtx] = 4;
                    }

                }
//                else if (nb_arete_int == 2){
//                    // il est relié à 3 faces sélectionnées
//                    // on place le sommet nouveau à l'intérieur de la face entre les 2 coedges
//
//                    // recherche du bloc commun aux deux cofaces
//                    CoFace* coface_commune = 0;
//                    std::map<CoFace*, uint> filtre_vu;
//
//                    std::vector<CoFace* > cofaces;
//                    coedges_int[0]->getCoFaces(cofaces);
//                    for (std::vector<CoFace* >::iterator iter11 = cofaces.begin();
//                            iter11 != cofaces.end(); ++iter11)
//                        filtre_vu[*iter11] = 1;
//
//                    coedges_int[1]->getCoFaces(cofaces);
//                    for (std::vector<CoFace* >::iterator iter11 = cofaces.begin();
//                            iter11 != cofaces.end(); ++iter11)
//                        if (filtre_vu[*iter11] == 1)
//                            coface_commune = *iter11;
//
//                    if (0 == coface_commune)
//                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitFacesWithOgrid::createVertices, on ne trouve pas de face commune aux 2 coedges internes");
//
//                    Utils::Math::Point barycentre_cf2;
//                    std::vector<Vertex* > vertices2;
//                    coface_commune->getVertices(vertices2);
//
//                    for (uint i=0; i<vertices2.size(); i++)
//                        barycentre_cf2 += vertices2[i]->getCoord();
//                    barycentre_cf2 /= (double)vertices2.size();
//
//                    Utils::Math::Point pt = (barycentre_cf2 + (sommet->getCoord() - barycentre_cf2)*m_ratio_ogrid);
//                    newVtx = new Topo::Vertex(getContext(), pt);
//                    getInfoCommand().addTopoInfoEntity(newVtx, Internal::InfoCommand::CREATED);
//                    corr_vertex[sommet] = newVtx;
//
//                }
                else {
                    // dans tous les autres cas, on ne fait rien pour le sommet
                }

            } // end else if (0 == newVtx)
        } // end for iter6
    } // end for iter1
}
/*----------------------------------------------------------------------------*/
void CommandSplitFacesWithOgrid::
createCoEdges(std::map<Vertex*, uint> & filtre_vertex,
        std::map<Vertex*, Vertex*> & corr_vertex,
        std::map<Vertex*, CoEdge*> & corr_vtx_coedge)
{
#ifdef _DEBUG_SPLIT_OGRID
	std::cout<<"CommandSplitFacesWithOgrid::createCoEdges(...)"<<std::endl;
#endif
    // il ne faut pas parcourir la map, au risque que l'ordre de création dépende de l'adresse
    // donc on le fait à partir des cofaces
    for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
            iter1 != m_cofaces.end(); ++iter1){

    	CoFace* coface = *iter1;

        std::vector<Vertex* > vertices;
        coface->getAllVertices(vertices);

        for (std::vector<Vertex* >::iterator iter6 = vertices.begin();
                iter6 != vertices.end(); ++iter6){
            CoEdge* newCoEdge = corr_vtx_coedge[*iter6];

            if (0 == newCoEdge){
                // cas où l'arête n'a pas été construite pour un autre bloc
                // pas d'arête pour les sommets internes
                if (filtre_vertex[*iter6] != 2){
                    Vertex* newVtx = corr_vertex[*iter6];
                    if (0 == newVtx)
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitFacesWithOgrid::createCoEdges, on ne retrouve pas de sommet opposé au sommet", TkUtil::Charset::UTF_8));

                    (*iter6)->saveVertexTopoProperty(&getInfoCommand());
                    EdgeMeshingPropertyUniform emp(m_nb_meshing_edges);
                    newCoEdge = new Topo::CoEdge(getContext(), &emp, *iter6, newVtx);
                    getInfoCommand().addTopoInfoEntity(newCoEdge, Internal::InfoCommand::CREATED);
                    corr_vtx_coedge[*iter6] = newCoEdge;
                }
            } // end if (0 == newCoEdge)
        } // end for iter6
    } // end for iter1
}
/*----------------------------------------------------------------------------*/
void CommandSplitFacesWithOgrid::
createCoEdgeAndFace(std::map<Vertex*, uint> & filtre_vertex,
        std::map<Vertex*, Vertex*>  & corr_vertex,
        std::map<Vertex*, CoEdge*> & corr_vtx_coedge,
        std::map<CoEdge*, uint> & filtre_coedge,
        std::map<CoFace*, uint> & filtre_coface,
        std::map<Edge*,  uint> & filtre_edge,
        std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge,
        std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface)
{
    // pour chaque arêtes sur le bord de la sélection (avec une marque de 1 ou 2)
    // création de l'arête interne à l'ogrid
    // et de la face commune entre ces deux arêtes
#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"createCoEdgeAndFace ..."<<std::endl;
#endif
    for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
            iter1 != m_cofaces.end(); ++iter1){

    	CoFace* coface = *iter1;
#ifdef _DEBUG_SPLIT_OGRID
        std::cout<<"coface: "<<coface->getName()<<std::endl;
#endif

        // on ne fait rien sur les faces internes
        if (filtre_coface[coface] == 2)
            continue;

        // on procède par Edge
        // et si cette Edge est composée de Edge commune interne et externe
        // alors on procède par arête commune
        std::vector<Edge* > edges;
        coface->getEdges(edges);

        for (std::vector<Edge* >::iterator iter2 = edges.begin();
                iter2 != edges.end(); ++iter2){

        	Edge* edge = *iter2;
#ifdef _DEBUG_SPLIT_OGRID
        	std::cout<<"arête: "<<edge->getName()<<", filtre à "<<filtre_edge[edge]<<std::endl;
#endif
        	Vertex* vtx0 = edge->getVertex(0);
        	Vertex* vtx1 = edge->getVertex(1);

        	std::vector<std::string> groupsName;

        	// Création des arêtes et coface en fonction d'un couple de sommets
        	createCoEdgeAndFace(vtx0, vtx1, edge,
					corr_vertex, corr_vtx_coedge, filtre_vertex, filtre_coedge,
					groupsName,
					corr_2vtx_coedge,
					corr_2vtx_coface);

        } // end for iter2
    } // end for iter1
}
/*----------------------------------------------------------------------------*/
std::vector<Vertex*> CommandSplitFacesWithOgrid::
getSelectedVertices(Vertex* vtx0,
        Vertex*vtx1,
        std::vector<CoEdge* > & coedges,
        std::map<Vertex*, uint> & filtre_vertex)
{
    // recherche des arêtes entre les 2 sommets
    std::vector<CoEdge* > coedges_between;
    if (!TopoHelper::getCoEdgesBetweenVertices(vtx0, vtx1, coedges, coedges_between))
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitFacesWithOgrid::getSelectedVertices, on ne retrouve pas de groupes d'arêtes entre 2 sommets", TkUtil::Charset::UTF_8));

    if (coedges_between.empty())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitFacesWithOgrid::getSelectedVertices, aucune arête !!!", TkUtil::Charset::UTF_8));

    // création de la liste des sommets en tenant compte des sommets
    // interne au bord, on ne prend que les sommets marqués

    Vertex* vtx_opp;
    std::map<CoEdge*, uint> filtre_vu;
    Vertex* vtx_prec = vtx0;
    // on se déplace suivant coedges_between, pour cela on marque à 1 ces arêtes puis à 2 lorsqu'on les a utlisé
    for (uint i=0; i<coedges_between.size(); i++)
        filtre_vu[coedges_between[i]] = 1;

    std::vector<Vertex*> vertices;
    vertices.push_back(vtx0);

    do {
        // recherche de l'arête marquée à 1 parmis celles liées au sommet vtx_prec
        std::vector<CoEdge* > coedge_vois;
        vtx_prec->getCoEdges(coedge_vois);
        CoEdge* coedge = 0;

        for (uint i=0; i<coedge_vois.size(); i++)
            if (filtre_vu[coedge_vois[i]] == 1)
                coedge = coedge_vois[i];

        if (0 == coedge)
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitFacesWithOgrid::getSelectedVertices, aucune arête voisine de vtx_prec", TkUtil::Charset::UTF_8));

        filtre_vu[coedge] = 2;
        vtx_opp = coedge->getOppositeVertex(vtx_prec);

        if (filtre_vertex[vtx_opp] == 1 || filtre_vertex[vtx_opp] == 2)
            vertices.push_back(vtx_opp);

        vtx_prec = vtx_opp;
    } while (vtx_opp != vtx1);

#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"getSelectedVertices("<<vtx0->getName()<<","<<vtx1->getName()<<")"<<std::endl;
    std::cout<<"On trouve pour le contour les sommets marqués: ";
    for (uint i=0; i<vertices.size(); i++)
        std::cout<<" "<<vertices[i]->getName();
    std::cout<<std::endl;
#endif

    return vertices;
}
/*----------------------------------------------------------------------------*/
void CommandSplitFacesWithOgrid::
createCoEdgeAndFace(Vertex* vtx0,
        Vertex* vtx1,
		Edge* edge,
        std::map<Vertex*, Vertex*>  & corr_vertex,
        std::map<Vertex*, CoEdge*> & corr_vtx_coedge,
        std::map<Vertex*, uint> & filtre_vertex,
        std::map<CoEdge*, uint> & filtre_coedge,
        std::vector<std::string> & groupsName,
        std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge,
        std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface)
{
#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"createCoEdgeAndFace("<<(vtx0?vtx0->getName():"0")<<","<<(vtx1?vtx1->getName():"0")<<")"<<std::endl;
#endif

    // pour reprendre ce qu'il y avait de codé dans CommandSplitBlocksWithOgrid
    Vertex* som0 = vtx0;
    Vertex* som1 = vtx1;

    // on ordonne les sommets suivant leur id
    TopoHelper::sortVertices(som0, som1);

    // recherche des arêtes entre les 2 sommets
    std::vector<CoEdge* > coedges_between;
    edge->getCoEdges(coedges_between);

    // si l'une des arêtes est interne (sans toucher le bord), on passe
    if (filtre_coedge[coedges_between[0]] == 3)
    	return;

#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"=> createCoEdgeAndFace pour ("<<som0->getName()<<","<<som1->getName()<<")"<<std::endl;
#endif

    CoEdge* newCoEdge = 0;
    CoFace* newCoface = 0;

    // l'arête est-elle déjà créée ?
    newCoEdge = corr_2vtx_coedge[std::pair<Vertex*, Vertex*>(som0, som1)];
    if (0 == newCoEdge){

    	// les 2 nouveaux sommets internes
    	Vertex* newVtx0 = corr_vertex[som0];
    	Vertex* newVtx1 = corr_vertex[som1];
    	if (0 == newVtx0 || 0 == newVtx1){
    		std::cerr<<"som0 : "<< som0->getName()<<" corr_vertex => "<<(newVtx0?newVtx0->getName():"vide")<<std::endl;
    		std::cerr<<"som1 : "<< som1->getName()<<" corr_vertex => "<<(newVtx1?newVtx1->getName():"vide")<<std::endl;
    		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitFacesWithOgrid::createCoEdgeAndFace, pas de sommets internes pour la nouvelle arête interne", TkUtil::Charset::UTF_8));
    	}
#ifdef _DEBUG_SPLIT_OGRID
    	std::cout<<"newVtx0 : "<<newVtx0->getName()<<", newVtx1 : "<<newVtx1->getName()<<std::endl;
#endif

    	// memorise si la/les coedge[s] sont au bord
    	bool isCoedgesAuBord  = false;

    	// s'il n'y a qu'une arête commune, on se permet de reprendre la discrétisation,
    	// sinon on reprend juste le nombre total de bras
    	if (coedges_between.size() == 1){
#ifdef _DEBUG_SPLIT_OGRID
    		std::cout<<"une seule arête ..."<<std::endl;
#endif
    		newCoEdge = new Topo::CoEdge(getContext(), coedges_between[0]->getMeshingProperty(), newVtx0, newVtx1);

    		isCoedgesAuBord = (filtre_coedge[coedges_between[0]] == 1);

    	} else if (coedges_between.size() > 1){
    		uint nb_bras = 0;
    		for (std::vector<CoEdge* >::iterator iter5 = coedges_between.begin();
    				iter5 != coedges_between.end(); ++iter5){
    			nb_bras += (*iter5)->getNbMeshingEdges();
    			if (filtre_coedge[*iter5] == 1)
    				isCoedgesAuBord = true;
    		}

#ifdef _DEBUG_SPLIT_OGRID
std::cout<<"plusieurs arêtes, nb_bras = "<<nb_bras<<std::endl;
#endif
EdgeMeshingPropertyUniform emp(nb_bras);
newCoEdge = new Topo::CoEdge(getContext(), &emp, newVtx0, newVtx1);
    	}
    	CHECK_NULL_PTR_ERROR(newCoEdge);
#ifdef _DEBUG_SPLIT_OGRID
    	std::cout<<"newCoEdge : "<<newCoEdge->getName()<<std::endl;
#endif

    	getInfoCommand().addTopoInfoEntity(newCoEdge, Internal::InfoCommand::CREATED);
    	corr_2vtx_coedge[std::pair<Vertex*, Vertex*>(som0, som1)] = newCoEdge;

    	// création d'une face commune seulement si les 2 sommets sont sur le bord
    	// ainsi que la coedge
    	if (filtre_vertex[som0] == 1 && filtre_vertex[som1] == 1 && isCoedgesAuBord){
    		// recherche des 2 arêtes entre les sommets externes et ceux internes
    		CoEdge* newCoEdge0 = corr_vtx_coedge[som0];
    		CoEdge* newCoEdge1 = corr_vtx_coedge[som1];
    		if (0 == newCoEdge0 || 0 == newCoEdge1)
    			throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitFacesWithOgrid::createCoEdgeAndFace, pas d'arêtes entre sommets externes et internes", TkUtil::Charset::UTF_8));

    		// création des 4 Edges
    		Edge* edge0 = new Topo::Edge(getContext(), newCoEdge0);
    		Edge* edge1 = new Topo::Edge(getContext(), newCoEdge);
    		Edge* edge2 = new Topo::Edge(getContext(), newCoEdge1);
    		Edge* edge3 = new Topo::Edge(getContext(), som0, som1, coedges_between);
    		getInfoCommand().addTopoInfoEntity(edge0, Internal::InfoCommand::CREATED);
    		getInfoCommand().addTopoInfoEntity(edge1, Internal::InfoCommand::CREATED);
    		getInfoCommand().addTopoInfoEntity(edge2, Internal::InfoCommand::CREATED);
    		getInfoCommand().addTopoInfoEntity(edge3, Internal::InfoCommand::CREATED);

    		// création de la face commune
			newCoface = new Topo::CoFace(getContext(), edge0, edge1, edge2, edge3);
			getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);
			corr_2vtx_coface[std::pair<Vertex*, Vertex*>(som0, som1)] = newCoface;
#ifdef _DEBUG_SPLIT_OGRID
			std::cout<<"newCoface : "<<newCoface->getName()<<std::endl;
#endif

			// cas d'une interpolation vers une coface, il faut mettre à jour
			if (newCoEdge->getMeshingProperty()->getMeshLaw() == CoEdgeMeshingProperty::interpolate){
				EdgeMeshingPropertyInterpolate* empi = dynamic_cast<EdgeMeshingPropertyInterpolate*>(newCoEdge->getMeshingProperty());
				CHECK_NULL_PTR_ERROR(empi);
				std::string coface_name = newCoface->getName();
				if (empi->getType() == EdgeMeshingPropertyInterpolate::with_coface)
					empi->setCoFace(coface_name);
			}

    	} // end if (filtre_vertex[som0] == 1 && filtre_vertex[som1] == 1)
    } // end if (0 == newCoEdge)
#ifdef _DEBUG_SPLIT_OGRID
    else {
#ifdef _DEBUG_SPLIT_OGRID
    	std::cout<<"déjà créée: newCoEdge : "<<newCoEdge->getName()<<std::endl;
#endif
    }
#endif

}
/*----------------------------------------------------------------------------*/
Edge* CommandSplitFacesWithOgrid::
createEdge(Vertex* vtx0,
        Vertex* vtx1,
        std::map<Vertex*, Vertex*>  & corr_vertex,
        std::map<Vertex*, uint> & filtre_vertex,
        std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge)
{
#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"createEdge("<<(vtx0?vtx0->getName():"0")<<","<<(vtx1?vtx1->getName():"0")<<")"<<std::endl;
#endif
    // on ordonne les sommets suivant leur id
    TopoHelper::sortVertices(vtx0, vtx1);


    CoEdge* newCoedge = corr_2vtx_coedge[std::pair<Vertex*, Vertex*>(vtx0, vtx1)];
    if (0 == newCoedge){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message <<"Erreur interne dans CommandSplitFacesWithOgrid::createEdge, pas d'arêtes interne face à 2 sommets ("
    			<< vtx0->getName()<<","<< vtx1->getName()<<")";
    	throw TkUtil::Exception(message);
    }

    // création de l'arête interne
    Edge* newEdge;
    newEdge = new Topo::Edge(getContext(),newCoedge);

    getInfoCommand().addTopoInfoEntity(newEdge, Internal::InfoCommand::CREATED);

    return newEdge;
}
/*----------------------------------------------------------------------------*/
void CommandSplitFacesWithOgrid::
createCoFace(std::map<Vertex*, uint> & filtre_vertex,
           std::map<Vertex*, Vertex*>  & corr_vertex,
           std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge,
           std::map<CoFace*, uint> & filtre_coface,
           std::map<CoFace*, CoFace*> & corr_coface)
{
#ifdef _DEBUG_SPLIT_OGRID
	std::cout<<"createCoFace ... "<<std::endl;
#endif

	for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
			iter1 != m_cofaces.end(); ++iter1){

		CoFace* coface = *iter1;

		if (filtre_coface[coface] != 3){
			std::vector<Edge* > edges;
			// boucle sur les 4 côtés
			for (uint i=0; i<4; i++){

				Edge* newEdge;
				Vertex* som0 = coface->getVertex(i);
				Vertex* som1 = coface->getVertex((i+1)%4);

				if (filtre_vertex[som0] == 2 && filtre_vertex[som1] == 2)
					// on conserve l'arête lorsqu'elle est interne
					newEdge = coface->getEdge(i);
				else
					newEdge = createEdge(
							som0,
							som1,
							corr_vertex,
							filtre_vertex,
							corr_2vtx_coedge);

				edges.push_back(newEdge);
			} // end for i<4

			// création de la face commune en rapport avec la Face
			CoFace* newCoface = new Topo::CoFace(getContext(), edges[0], edges[1], edges[2], edges[3]);
			getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);
			corr_coface[coface] = newCoface;

		} // end if (filtre_coface[coface] != 3)

	} // end for iter1
}
/*----------------------------------------------------------------------------*/
void CommandSplitFacesWithOgrid::
freeUnused(std::map<Vertex*, uint> & filtre_vertex,
        std::map<CoEdge*, uint> & filtre_coedge,
        std::map<CoFace*, uint> & filtre_coface,
        std::map<Vertex*, CoEdge*> & corr_vtx_coedge,
        std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface,
        std::map<CoFace*, CoFace*> & corr_coface)
{
#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"freeUnused ..."<<std::endl;
#endif
    // supression des faces et cofaces internes (reprise des projections pour ces dernières)
    for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
            iter1 != m_cofaces.end(); ++iter1){

        CoFace* coface = *iter1;
#ifdef _DEBUG_SPLIT_OGRID
        std::cout<<"coface: "<<coface->getName()<<std::endl;
#endif
        // les cofaces au centre sont conservés
        if (filtre_coface[coface] == 2)
            continue;

        // reprise des projections
        Geom::GeomEntity* ga = coface->getGeomAssociation();
        if (ga){
            // pour la coface centrale
            CoFace* newCoface = corr_coface[coface];
            if (newCoface) {
                newCoface->setGeomAssociation(ga);

                // pour les arêtes et sommets au bord de cette coface centrale
                std::vector<CoEdge* > coedges;
                newCoface->getCoEdges(coedges);
                for (std::vector<CoEdge* >::iterator iter5 = coedges.begin();
                        iter5 != coedges.end(); ++iter5){
#ifdef _DEBUG_SPLIT_OGRID
                	std::cout<<"coedge: "<<(*iter5)->getName()<<std::endl;
                	std::cout<<"  vtx0: "<<(*iter5)->getVertex(0)->getName()<<", filtre à "<<filtre_vertex[(*iter5)->getVertex(0)]<<std::endl;
                	std::cout<<"  vtx1: "<<(*iter5)->getVertex(1)->getName()<<", filtre à "<<filtre_vertex[(*iter5)->getVertex(1)]<<std::endl;
                	if ((*iter5)->getVertex(0)->getGeomAssociation() == (*iter5)->getVertex(1)->getGeomAssociation())
                		std::cout<<"  associés à la même entité"<<std::endl;
                	else
                		std::cout<<"  associés à des entités différentes"<<std::endl;
#endif
                    if ((filtre_vertex[(*iter5)->getVertex(0)] == 4
                            || filtre_vertex[(*iter5)->getVertex(1)] == 4)
                            && (*iter5)->getVertex(0)->getGeomAssociation() == (*iter5)->getVertex(1)->getGeomAssociation())
                        (*iter5)->setGeomAssociation((*iter5)->getVertex(0)->getGeomAssociation());
                    else
                        (*iter5)->setGeomAssociation(ga);

                    for (uint i=0; i<2; i++)
                        if ((*iter5)->getVertex(i)->getGeomAssociation() == 0)
                            (*iter5)->getVertex(i)->setGeomAssociation(ga);
                } // end for iter5

                // pour les arêtes entre les sommets de la face initiale et celle créée au centre
                for (uint i=0; i<4; i++){
                    CoEdge* newCoedge = corr_vtx_coedge[coface->getVertex(i)];
                    if (newCoedge){
#ifdef _DEBUG_SPLIT_OGRID
                    	std::cout<<"newCoedge: "<<newCoedge->getName()<<std::endl;
                    	std::cout<<"  vtx0: "<<newCoedge->getVertex(0)->getName()<<", filtre à "<<filtre_vertex[newCoedge->getVertex(0)]<<std::endl;
                    	std::cout<<"  vtx1: "<<newCoedge->getVertex(1)->getName()<<", filtre à "<<filtre_vertex[newCoedge->getVertex(1)]<<std::endl;
#endif
                        if (filtre_vertex[newCoedge->getVertex(0)] == 4)
                            newCoedge->setGeomAssociation(newCoedge->getVertex(0)->getGeomAssociation());
                        else if (filtre_vertex[newCoedge->getVertex(1)] == 4)
                            newCoedge->setGeomAssociation(newCoedge->getVertex(1)->getGeomAssociation());
                        else
                            newCoedge->setGeomAssociation(ga);
                    }
                } // end for i

                // pour les faces entre les bords de la face initiale et celle au centre
                for (uint i=0; i<4; i++){
                    Vertex* vtx0 = coface->getVertex(i);
                    Vertex* vtx1 = coface->getVertex((i+1)%4);

                    // on ordonne les sommets suivant leur id
                    TopoHelper::sortVertices(vtx0, vtx1);

                    newCoface = corr_2vtx_coface[std::pair<Vertex*, Vertex*>(vtx0, vtx1)];
                    if (newCoface)
                        newCoface->setGeomAssociation(ga);

                } // end for i
            } // end if (newCoface)
        } // end if (ga)

        // récupération des groupes pour les cofaces
        Utils::Container<Group::Group2D>& groups = coface->getGroupsContainer();
        // pour la coface centrale
        CoFace* newCoface = corr_coface[coface];
        if (newCoface) {
            newCoface->setGeomAssociation(ga);

            for (uint i=0; i<groups.size(); i++){
            	Group::Group2D* gr = groups.get(i);
            	gr->add(newCoface);
            	newCoface->getGroupsContainer().add(gr);
            }

            // pour les faces entre les bords de la face initiale et celle au centre
            for (uint i=0; i<4; i++){
                Vertex* vtx0 = coface->getVertex(i);
                Vertex* vtx1 = coface->getVertex((i+1)%4);

                // on ordonne les sommets suivant leur id
                TopoHelper::sortVertices(vtx0, vtx1);

                newCoface = corr_2vtx_coface[std::pair<Vertex*, Vertex*>(vtx0, vtx1)];
                if (newCoface)
                	for (uint i=0; i<groups.size(); i++){
                		Group::Group2D* gr = groups.get(i);
                		gr->add(newCoface);
                		newCoface->getGroupsContainer().add(gr);
                	}
            } // end for i
        } // end if (newCoface)



        std::vector<Edge* > edges;
        coface->getEdges(edges);
        for (std::vector<Edge* >::iterator iter4 = edges.begin();
        		iter4 != edges.end(); ++iter4){
        	Edge* edge = *iter4;

        	std::vector<CoEdge* > coedges;
        	edge->getCoEdges(coedges);

        	// suppression des arêtes initiales entre le bord et le centre de la sélection
        	for (std::vector<CoEdge* >::iterator iter5 = coedges.begin();
        			iter5 != coedges.end(); ++iter5){
        		CoEdge* coedge = *iter5;
        		if (filtre_coedge[coedge] == 2){
        			coedge->free(&getInfoCommand());
#ifdef _DEBUG_SPLIT_OGRID
        			std::cout<<"  destruction de "<<coedge->getName()<<std::endl;
#endif
        		}
        	} // end for iter5

        	if (edge->getNbCoFaces() == 1)
        		edge->free(&getInfoCommand());
        } // end for iter4

        // la face commune doit être détruite
        coface->free(&getInfoCommand());

    } // end for iter1

    // suppression des sommets et arêtes inutilisées vers le centre de l'ogrid
    for (std::map<Vertex*, CoEdge*>::iterator iter = corr_vtx_coedge.begin();
    		iter != corr_vtx_coedge.end(); ++iter){
    	CoEdge* coedge = iter->second;
    	Vertex* vtx = iter->first;

    	if (coedge && !coedge->isDestroyed() && coedge->getNbEdges() == 0){
    		Vertex* newVtx = coedge->getOppositeVertex(vtx);
    		if (newVtx->getNbCoEdges() == 1){
#ifdef _DEBUG_SPLIT_OGRID
    			std::cout<<"  sommet à détruire : "<<newVtx->getName()<<std::endl;
#endif
    			newVtx->free(&getInfoCommand());
    		}
#ifdef _DEBUG_SPLIT_OGRID
    		std::cout<<"  coedge à détruire : "<<coedge->getName()<<std::endl;
#endif
    		coedge->free(&getInfoCommand());
    	}

    } // end for iter


}
/*----------------------------------------------------------------------------*/
void CommandSplitFacesWithOgrid::
countNbCoEdgesByVertices(std::map<Topo::Vertex*, uint> &nb_coedges_by_vertex)
{
	// stocke pour chacun des sommets le nombre d'arêtes auxquelles il est relié
	for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
	            iter1 != m_cofaces.end(); ++iter1){
		CoFace* coface = *iter1;
		std::vector<Topo::Vertex* > all_vertices;
		coface->getAllVertices(all_vertices);
		for (uint i=0; i<all_vertices.size(); i++)
			nb_coedges_by_vertex[all_vertices[i]] = all_vertices[i]->getNbCoEdges();
	}
}
/*----------------------------------------------------------------------------*/
void CommandSplitFacesWithOgrid::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
