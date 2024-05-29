/*----------------------------------------------------------------------------*/
/*
 * \file CommandSplitBlocksWithOgrid.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/3/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSplitBlocksWithOgrid.h"

#include "Utils/Common.h"
#include "Topo/TopoHelper.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"
#include "Topo/SetNbMeshingEdgesImplementation.h"
#include "Group/Group3D.h"
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
CommandSplitBlocksWithOgrid::
CommandSplitBlocksWithOgrid(Internal::Context& c,
        std::vector<Topo::Block* > &blocs,
        std::vector<Topo::CoFace* > &cofaces,
        double ratio_ogrid,
        int nb_bras,
		bool create_internal_vertices,
		bool propagate_neighbor_block)
:CommandEditTopo(c, "Découpage des blocs structurés en O-grid")
, m_ratio_ogrid(ratio_ogrid)
, m_nb_meshing_edges(nb_bras)
, m_create_internal_vertices(create_internal_vertices)
, m_propagate_neighbor_block(propagate_neighbor_block)
{
    if (ratio_ogrid<=0.0 || ratio_ogrid>=1.0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le ratio doit être dans l'interval ]0 1[", TkUtil::Charset::UTF_8));

    if (nb_bras<=0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le nombre de bras doit être d'au moins 1", TkUtil::Charset::UTF_8));

    // on ne conserve que les blocs structurés et non dégénérés
    for (std::vector<Topo::Block* >::iterator iter = blocs.begin();
            iter != blocs.end(); ++iter){
        Topo::Block* hb = *iter;
        if (hb->isStructured() && hb->getNbVertices() == 8)
            m_blocs.push_back(hb);
        else{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"Il n'est pas prévu de faire un découpage en o-grid dans un bloc non structuré ou dégénéré\n";
            message << "("<<hb->getName()<<" n'est pas structuré)";
            throw TkUtil::Exception(message);
        }
    }

    // la validité de la sélection se fera lors de la création des filtres
    m_cofaces.insert(m_cofaces.end(), cofaces.begin(), cofaces.end());

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Découpage en O-grid des blocs structurés";
	for (uint i=0; i<blocs.size() && i<5; i++)
		comments << " " << blocs[i]->getName();
	if (blocs.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSplitBlocksWithOgrid::
~CommandSplitBlocksWithOgrid()
{
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocksWithOgrid::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSplitBlocksWithOgrid::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"CommandSplitBlocksWithOgrid::internalExecute"<<std::endl;
#endif

    // Filtres:  à 1 pour ce qui est sur le bord et à 2 ce qui est à l'intérieur
    std::map<Vertex*, uint> filtre_vertex;
    std::map<CoEdge*, uint> filtre_coedge;
    std::map<CoFace*, uint> filtre_coface;
    std::map<Block*,  uint> filtre_bloc;
    // Filtre pour les Faces à 1 si toutes ses cofaces sont à conserver comme un ensemble,
    // à 2 si les cofaces sont à prendre séparément
    std::map<Face*,  uint> filtre_face;

    // Renseigne les filtres
    computeFilters(filtre_vertex, filtre_coedge, filtre_coface, filtre_face, filtre_bloc);

    // Sauvegarde les relations topologiques pour permettre le undo/redo
    saveTopoProperty(filtre_vertex, filtre_coedge, filtre_coface, filtre_bloc);


    // relation entre sommet initial et sommet au centre de l'o-grid
    std::map<Vertex*, Vertex*> corr_vertex;

    // Création des sommets au centre de l'o-grid
    createVertices(filtre_vertex, filtre_coedge, filtre_coface, corr_vertex);


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
            filtre_coedge, filtre_coface, filtre_face, filtre_bloc,
            corr_2vtx_coedge, corr_2vtx_coface);


    // relation entre face commune issues de la sélection et celles au centre
    std::map<CoFace*, CoFace*> corr_coface;

    // Création des cofaces au centre, issues de la sélection
    createCoFace(filtre_vertex, corr_vertex, corr_2vtx_coedge, filtre_coface, filtre_face, filtre_bloc,
            corr_coface);


    // Création des blocs
    createBlock(filtre_vertex, corr_vertex, corr_2vtx_coface, filtre_coface, corr_coface, filtre_face, filtre_bloc);


    // Supression des entités initiales qui ne sont plus utilisées: les faces internes et les blocs,
    // réutilisation des projections pour les faces internes
    // mise à jour des arêtes et faces voisines si nécessaire pour cas m_propagate_neighbor_block==false
    freeUnused(filtre_vertex, filtre_coedge, filtre_coface, filtre_bloc,
            corr_vtx_coedge, corr_2vtx_coedge, corr_2vtx_coface, corr_coface);


    // suppression des entités temporaires
    cleanTemporaryEntities();

    if (!m_propagate_neighbor_block)
    	updateNbMeshingEdges();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

//    std::cout<<"Dans CommandSplitBlocksWithOgrid: "<<std::endl;
//    std::cout<< getInfoCommand() <<std::endl;

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocksWithOgrid::
computeFilters(std::map<Vertex*, uint> & filtre_vertex,
            std::map<CoEdge*, uint> & filtre_coedge,
            std::map<CoFace*, uint> & filtre_coface,
            std::map<Face*,  uint> & filtre_face,
            std::map<Block*,  uint> & filtre_bloc)
{
    // premier passage pour compter le nombre de blocs (sélectionnés) en relation avec faces communes
    // et on marque à 2 tous les sommets, arêtes et blocs
    // on met à 3 les arêtes et faces internes ne touchant pas la peau
    for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
            iter1 != m_blocs.end(); ++iter1){
        Block* bloc = *iter1;

        // les blocs
        filtre_bloc[bloc] = 2;

        // les sommets
        std::vector<Vertex* > vertices;
        bloc->getAllVertices(vertices);
        for (uint i=0; i<vertices.size(); i++)
            filtre_vertex[vertices[i]] = 2;

        // les arêtes communes
        std::vector<CoEdge* > coedges;
        bloc->getCoEdges(coedges);
        for (uint i=0; i<coedges.size(); i++)
            filtre_coedge[coedges[i]] = 2;

        // on incrémente à chaque fois que l'on voit la face commune
        std::vector<CoFace* > cofaces;
        bloc->getCoFaces(cofaces);
        for (uint i=0; i<cofaces.size(); i++)
            filtre_coface[cofaces[i]] += 1;

    } // end for iter1


    // on force les cofaces sélectionnées pour qu'elles soient traités
    // comme si elles étaient entre 2 blocs sélectionnés
    std::vector<Block* > addedBlocks;
    for (std::vector<CoFace* >::iterator iter3 = m_cofaces.begin();
            iter3 != m_cofaces.end(); ++iter3){
        CoFace* coface = *iter3;
        bool erreur = false;

        if (m_propagate_neighbor_block){
        	std::vector<Block* > blocs;
        	coface->getBlocks(blocs);
        	if (blocs.size() == 2){
        		uint nb_blocsAjoutes = 0;
        		// dans se cas on ajoute simplement le bloc voisin
        		for (uint i=0; i<blocs.size(); i++)
        			if (filtre_bloc[blocs[i]] == 0){
        				m_blocs.push_back(blocs[i]);
        				addedBlocks.push_back(blocs[i]);
        				nb_blocsAjoutes += 1;
        			}
        		if (nb_blocsAjoutes != 1)
        			erreur = true;
        	}
        	else if (blocs.size() == 1)
        		if (filtre_coface[coface] == 1)
        			filtre_coface[coface] = 2;
        		else
        			erreur = true;
        }
        else {
        	if (filtre_coface[coface] == 1)
        		filtre_coface[coface] = 2;
        	else
        		erreur = true;
        }

        if (erreur){
            // cas d'une coface qui n'est pas sur le bord du groupe de blocs sélectionnés
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"La face "<<coface->getName()<<" n'est pas prise en compte pour le découpage";
            log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
        }
    } // end for iter3

    //    // on recommence pour les blocs ajoutés
    if (m_propagate_neighbor_block)
    	for (std::vector<Block* >::iterator iter1 = addedBlocks.begin();
    			iter1 != addedBlocks.end(); ++iter1){
    		Block* bloc = *iter1;

    		// les blocs
    		filtre_bloc[bloc] = 2;

    		// les sommets
    		std::vector<Vertex* > vertices;
    		bloc->getAllVertices(vertices);
    		for (uint i=0; i<vertices.size(); i++)
    			filtre_vertex[vertices[i]] = 2;

    		// les arêtes communes
    		std::vector<CoEdge* > coedges;
    		bloc->getCoEdges(coedges);
    		for (uint i=0; i<coedges.size(); i++)
    			filtre_coedge[coedges[i]] = 2;

    		// on incrémente à chaque fois que l'on voit la face commune
    		std::vector<CoFace* > cofaces;
    		bloc->getCoFaces(cofaces);
    		for (uint i=0; i<cofaces.size(); i++)
    			filtre_coface[cofaces[i]] += 1;

    	} // end for iter1


    // constitution de la liste de cofaces au bord
    std::vector<CoFace*> cofacesAuBord;
    for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
            iter1 != m_blocs.end(); ++iter1){

        Block* bloc = *iter1;
        std::vector<CoFace* > cofaces;
        bloc->getCoFaces(cofaces);
        for (uint i=0; i<cofaces.size(); i++)
            if (filtre_coface[cofaces[i]] == 1)
                cofacesAuBord.push_back(cofaces[i]);
    } // end for iter1

    // pour les cofaces au bord, on marque les arêtes et blocs //sommets,
    // à 1 (au bord)
    for (std::vector<CoFace*>::iterator iter3 = cofacesAuBord.begin();
            iter3 != cofacesAuBord.end(); ++iter3){
        CoFace* coface = *iter3;
#ifdef _DEBUG_SPLIT_OGRID
            std::cout<<"CoFace au bord : "<<coface->getName()<<std::endl;
#endif

        // les arêtes communes au bord
        std::vector<CoEdge* > coedges;
        coface->getCoEdges(coedges);
        for (uint i=0; i<coedges.size(); i++)
            filtre_coedge[coedges[i]] = 1;

        if (m_create_internal_vertices){
        	// les sommets des cofaces au bord de la sélection
        	// [EB le 19/12/13] désormais on marque les sommets qui ne sont pas référencés directement par un bloc
        	// puisque l'on utilise getAllVertices ...
        	std::vector<Vertex* > vertices;
        	coface->getAllVertices(vertices);
        	for (uint i=0; i<vertices.size(); i++)
        		if (filtre_vertex[vertices[i]] == 2)
        			filtre_vertex[vertices[i]] = 1;
        }
        // les blocs sélectionnés qui touchent le bord
        std::vector<Block* > blocs;
        coface->getBlocks(blocs);
        for (uint i=0; i<blocs.size(); i++)
        	if (filtre_bloc[blocs[i]] == 2)
        		filtre_bloc[blocs[i]] = 1;
    } // end for iter3

    if (m_create_internal_vertices)
    	// pour les blocs internes, on marque ses arêtes et cofaces à 3
    	for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
    			iter1 != m_blocs.end(); ++iter1){

    		Block* bloc = *iter1;

    		if (filtre_bloc[bloc] == 2){

#ifdef _DEBUG_SPLIT_OGRID
    			std::cout<<"bloc interne : "<<bloc->getName()<<std::endl;
#endif

    			// les arêtes communes
    			std::vector<CoEdge* > coedges;
    			bloc->getCoEdges(coedges);
    			for (uint i=0; i<coedges.size(); i++)
    				filtre_coedge[coedges[i]] = 3;

    			// les faces communes
    			std::vector<CoFace* > cofaces;
    			bloc->getCoFaces(cofaces);
    			for (uint i=0; i<cofaces.size(); i++)
    				filtre_coface[cofaces[i]] = 3;

    		}
    	}

    // filtre pour les faces
    for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
            iter1 != m_blocs.end(); ++iter1){

        Block* bloc = *iter1;

        std::vector<Face* > faces;
        bloc->getFaces(faces);

        for (std::vector<Face* >::iterator iter2 = faces.begin();
                iter2 != faces.end(); ++iter2){

            Face* face = *iter2;
            // recherche des nombres de faces communes internes et externes dans cette face
            uint nb_face_int = 0;
            uint nb_face_ext = 0;

            std::vector<CoFace* > cofaces;
            face->getCoFaces(cofaces);

            for (std::vector<CoFace* >::iterator iter3 = cofaces.begin();
                    iter3 != cofaces.end(); ++iter3)
                if (filtre_coface[*iter3] == 1)
                    nb_face_ext += 1;
                else if (filtre_coface[*iter3] == 2)
                    nb_face_int += 1;
#ifdef _DEBUG_SPLIT_OGRID
            std::cout<<"Face : "<<face->getName()<<", nb_face_ext: "<<nb_face_ext<<", nb_face_int: "<<nb_face_int<<std::endl;
#endif

            if (nb_face_int == 0){ // || nb_face_ext == 0
                filtre_face[face] = 1;
                if (!m_create_internal_vertices){
                	std::vector<Vertex* > vertices;
                	face->getVertices(vertices);
                	for (uint i=0; i<vertices.size(); i++)
                		filtre_vertex[vertices[i]] = 1;
                }
            }
            else {
                filtre_face[face] = 2;

                // pour ces faces particulières, on marques tous les noeuds des cofaces au bord
                for (std::vector<CoFace* >::iterator iter3 = cofaces.begin();
                        iter3 != cofaces.end(); ++iter3){
                    CoFace* coface = *iter3;
                    if (filtre_coface[coface] == 1){
                        std::vector<Vertex* > vertices;
                        coface->getVertices(vertices);
                        for (uint i=0; i<vertices.size(); i++)
                            filtre_vertex[vertices[i]] = 1;
                    }
                }
            }
        }
    }

    if (!m_create_internal_vertices){
    	// pour les blocs internes, on marque ses arêtes et cofaces à 3
    	for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
    			iter1 != m_blocs.end(); ++iter1){

    		Block* bloc = *iter1;

    		if (filtre_bloc[bloc] == 2){

#ifdef _DEBUG_SPLIT_OGRID
    			std::cout<<"bloc interne : "<<bloc->getName()<<std::endl;
#endif

    			// les arêtes communes
    			std::vector<CoEdge* > coedges;
    			bloc->getCoEdges(coedges);
    			for (uint i=0; i<coedges.size(); i++)
    				filtre_coedge[coedges[i]] = 3;

    			// les faces communes
    			std::vector<CoFace* > cofaces;
    			bloc->getCoFaces(cofaces);
    			for (uint i=0; i<cofaces.size(); i++)
    				filtre_coface[cofaces[i]] = 3;

    		}
    	}

    	// constitution de la liste de faces au bord
    	std::vector<Face*> facesAuBord;
    	for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
    			iter1 != m_blocs.end(); ++iter1){

    		Block* bloc = *iter1;
    		std::vector<Face* > faces;
    		bloc->getFaces(faces);
    		for (uint i=0; i<faces.size(); i++)
    			if (filtre_face[faces[i]] == 1)
    				facesAuBord.push_back(faces[i]);
    	} // end for iter1

		// pour les faces au bord, on marque les sommets,
    	// à 1 (au bord)
    	for (std::vector<Face*>::iterator iter3 = facesAuBord.begin();
    			iter3 != facesAuBord.end(); ++iter3){
    		Face* face = *iter3;

    		std::vector<Vertex* > vertices;
    		face->getVertices(vertices);
    		for (uint i=0; i<vertices.size(); i++)
    			if (filtre_vertex[vertices[i]] == 2)
    				filtre_vertex[vertices[i]] = 1;

    		face->getAllVertices(vertices);
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
    } // end if (!m_create_internal_vertices)

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
    std::cout<<"filtre_coface : "<<std::endl;
    for (std::map<CoFace*, uint>::iterator iter=filtre_coface.begin();
    		iter!=filtre_coface.end(); ++iter){
    	std::cout<<"  "<<iter->first->getName()<<" : "<<iter->second<<std::endl;
    } // end for iter
    std::cout<<"filtre_face : "<<std::endl;
    for (std::map<Face*, uint>::iterator iter=filtre_face.begin();
    		iter!=filtre_face.end(); ++iter){
    	std::cout<<"  "<<iter->first->getName()<<" : "<<iter->second<<std::endl;
    } // end for iter
    std::cout<<"filtre_bloc : "<<std::endl;
    for (std::map<Block*, uint>::iterator iter=filtre_bloc.begin();
    		iter!=filtre_bloc.end(); ++iter){
    	std::cout<<"  "<<iter->first->getName()<<" : "<<iter->second<<std::endl;
    } // end for iter
#endif
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocksWithOgrid::saveTopoProperty(std::map<Vertex*, uint> & filtre_vertex,
        std::map<CoEdge*, uint> & filtre_coedge,
        std::map<CoFace*, uint> & filtre_coface,
        std::map<Block*,  uint> & filtre_bloc)
{
    for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
             iter1 != m_blocs.end(); ++iter1){

         Block* bloc = *iter1;

         if (filtre_bloc[bloc] == 1){
             // les sommets
              std::vector<Vertex* > vertices;
              bloc->getVertices(vertices);
              for (uint i=0; i<vertices.size(); i++)
                  vertices[i]->saveVertexTopoProperty(&getInfoCommand());

              std::vector<Face* > faces;
              bloc->getFaces(faces);

              for (std::vector<Face* >::iterator iter2 = faces.begin();
                      iter2 != faces.end(); ++iter2){
                  Face* face = *iter2;

                  face->saveFaceTopoProperty(&getInfoCommand());

                  std::vector<CoFace* > cofaces;
                  face->getCoFaces(cofaces);
                  for (std::vector<CoFace* >::iterator iter3 = cofaces.begin();
                          iter3 != cofaces.end(); ++iter3){
                      CoFace* coface = *iter3;

                      // seules les faces communes internes ne sont pas touchées
                      if (filtre_coface[coface] != 3)
                          coface->saveCoFaceTopoProperty(&getInfoCommand());

                      if (!m_propagate_neighbor_block){
                    	  // propage aux faces reliées
                    	  std::vector<Face* > otherFaces;
                    	  coface->getFaces(otherFaces);

                    	  for (std::vector<Face* >::iterator iter6 = otherFaces.begin();
                    			  iter6 != otherFaces.end(); ++iter6)
                    		  (*iter6)->saveFaceTopoProperty(&getInfoCommand());
                      }

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
                              if (filtre_coedge[coedge] != 3){
                                  coedge->saveCoEdgeTopoProperty(&getInfoCommand());
                                  //std::cout<<"saveCoEdgeTopoProperty pour "<<coedge->getName()<<std::endl;

                                  if (!m_propagate_neighbor_block){
                                	  // propage aux arêtes reliées
                                	  std::vector<Edge* > otherEdges;
                                	  coedge->getEdges(otherEdges);

                                	  for (std::vector<Edge* >::iterator iter6 = otherEdges.begin();
                                			  iter6 != otherEdges.end(); ++iter6)
                                		  (*iter6)->saveEdgeTopoProperty(&getInfoCommand());

                                  }

                              }
                          } // end for iter5
                      } // end for iter4
                  } // end for iter3
              } // end for iter2

         } // end if (filtre_bloc[bloc] == 1)
     } // end for iter1

}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocksWithOgrid::
createVertices(std::map<Vertex*, uint> & filtre_vertex,
        std::map<CoEdge*, uint> & filtre_coedge,
        std::map<CoFace*, uint> & filtre_coface,
        std::map<Vertex*, Vertex*> & corr_vertex)
{
#ifdef _DEBUG_SPLIT_OGRID
	std::cout<<"CommandSplitBlocksWithOgrid::createVertices(...)"<<std::endl;
#endif
    for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
            iter1 != m_blocs.end(); ++iter1){
        Block* bloc = *iter1;

        // les sommets extrémités du bloc
        std::vector<Vertex* > vertices;
        bloc->getVertices(vertices);

        // recherche du barycentre du bloc
        Utils::Math::Point barycentre_blk;
        for (uint i=0; i<vertices.size(); i++)
            barycentre_blk += vertices[i]->getCoord();
        barycentre_blk /= (double)vertices.size();

        bloc->getAllVertices(vertices);

        // création des sommets pour le bloc central
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
                // cas où le sommet n'a pas été construit pour un autre bloc

                // recherche des nombres de faces communes internes et externes liées à ce sommet
                uint nb_face_int = 0;
                uint nb_face_ext = 0;
                // on conserve les cofaces internes
                std::vector<CoFace* > cofaces_int;

                std::vector<CoFace* > cofaces;
                sommet->getCoFaces(cofaces);

                for (std::vector<CoFace* >::iterator iter3 = cofaces.begin();
                        iter3 != cofaces.end(); ++iter3){
#ifdef _DEBUG_SPLIT_OGRID
                	std::cout<<"  coface = "<<(*iter3)->getName()<<", filtre à "<<filtre_coface[*iter3]<<std::endl;
#endif
                    if (filtre_coface[*iter3] == 1)
                        nb_face_ext += 1;
                    else if (filtre_coface[*iter3] == 2){
                        cofaces_int.push_back(*iter3);
                        nb_face_int += 1;
                    }
                }
#ifdef _DEBUG_SPLIT_OGRID
                std::cout<<"nb_face_int = "<<nb_face_int<<std::endl;
                std::cout<<"nb_face_ext = "<<nb_face_ext<<std::endl;
#endif

                if (nb_face_int == 0){
                    // il n'est relié qu'à un seul bloc sélectionné
                    Utils::Math::Point pt = (barycentre_blk + (sommet->getCoord() - barycentre_blk)*m_ratio_ogrid);
                    newVtx = new Topo::Vertex(getContext(), pt);
                    getInfoCommand().addTopoInfoEntity(newVtx, Internal::InfoCommand::CREATED);
                    corr_vertex[sommet] = newVtx;
                }
                else if ((nb_face_int >= 3)
                        || (nb_face_int == 2 && nb_face_ext == 1)
//                        || (nb_face_int == 3 && nb_face_ext == 2)
                        ){
                    // il est relié à au moins 3 blocs sélectionnés
                    // ou un seul bloc mais avec 2 faces de sélectionnées
                    // ou relié à 2 blocs mais avec 2 faces de sélectionnées
                    // on recherche le sommet marqué à 1 ou plus en partant de ce sommet
                    // et en utilisant seulement des arêtes marquées à 2
                    // (arêtes internes à la sélection, entre bord et sélection)
                    Vertex* vtx_opp;
                    std::map<CoEdge*, uint> filtre_vu;
                    Vertex* vtx_prec = sommet;

                    // les coedges utilisées
                    std::vector<CoEdge*> coedges_int;

                    do {
#ifdef _DEBUG_SPLIT_OGRID
                    	std::cout<<"vtx_prec : "<<vtx_prec->getName()<<std::endl;
#endif
                        std::vector<CoEdge* > coedges;
                        vtx_prec->getCoEdges(coedges);
                        CoEdge* coedge = 0;
                        for (uint i=0; i<coedges.size(); i++)
                            if (filtre_vu[coedges[i]] == 0 && filtre_coedge[coedges[i]] == 2)
                                coedge = coedges[i];

                        if (0 == coedge)
                            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitBlocksWithOgrid::createVertices, on ne trouve pas de sommet opposé au sommet marqué à 4", TkUtil::Charset::UTF_8));

                        // pour ne pas revenir en arrière
                        filtre_vu[coedge] = 1;
                        coedges_int.push_back(coedge);
#ifdef _DEBUG_SPLIT_OGRID
                        std::cout<<"coedge : "<<coedge->getName()
                        		<<", filtre_vu = "<<filtre_vu[coedge]
                        		<<", filtre_coedge = "<<filtre_coedge[coedge]
                        		<<std::endl;
#endif
                        vtx_opp = coedge->getOppositeVertex(vtx_prec);

                        vtx_prec = vtx_opp;
                    } while (filtre_vertex[vtx_opp] == 0);

#ifdef _DEBUG_SPLIT_OGRID
                    std::cout<<"vtx_opp : "<<vtx_opp->getName()<<std::endl;
#endif
                    Utils::Math::Point barycentre_edge = (sommet->getCoord() + vtx_opp->getCoord())/2;

                    Utils::Math::Point pt = (barycentre_edge + (sommet->getCoord() - barycentre_edge)*m_ratio_ogrid);
                    newVtx = new Topo::Vertex(getContext(), pt);
                    getInfoCommand().addTopoInfoEntity(newVtx, Internal::InfoCommand::CREATED);
                    corr_vertex[sommet] = newVtx;

                    // utilisation de la projection de[s] coedges vues si possible
                    if (coedges_int.size() == 1 && coedges_int[0]->getGeomAssociation()){
                        newVtx->setGeomAssociation(coedges_int[0]->getGeomAssociation());
                        filtre_vertex[newVtx] = 4;
                    }

                }
                else if (nb_face_int == 1){
                    // il est relié à 2 blocs sélectionnés
                    CoFace* coface = cofaces_int[0];

                    // calcul le barycentre de la CoFace
                    std::vector<Vertex* > vertices_cf;
                    coface->getVertices(vertices_cf);
                    Utils::Math::Point barycentre_cf;
                    for (uint i=0; i<vertices_cf.size(); i++)
                        barycentre_cf += vertices_cf[i]->getCoord();
                    barycentre_cf /= (double)vertices_cf.size();

                    Utils::Math::Point pt = (barycentre_cf + (sommet->getCoord() - barycentre_cf)*m_ratio_ogrid);
                    newVtx = new Topo::Vertex(getContext(), pt);
                    getInfoCommand().addTopoInfoEntity(newVtx, Internal::InfoCommand::CREATED);
                    corr_vertex[sommet] = newVtx;
                }
                else if (nb_face_int == 2){
                    // il est relié à 3 blocs sélectionnés
                    // on place le sommet nouveau à l'intérieur du bloc entre les 2 cofaces

                    // recherche du bloc commun aux deux cofaces
                    Block* bloc_commun = 0;
                    std::map<Block*, uint> filtre_vu;

                    std::vector<Block* > blocs;
                    cofaces_int[0]->getBlocks(blocs);
                    for (std::vector<Block* >::iterator iter11 = blocs.begin();
                            iter11 != blocs.end(); ++iter11)
                        filtre_vu[*iter11] = 1;

                    cofaces_int[1]->getBlocks(blocs);
                    for (std::vector<Block* >::iterator iter11 = blocs.begin();
                            iter11 != blocs.end(); ++iter11)
                        if (filtre_vu[*iter11] == 1)
                            bloc_commun = *iter11;

                    if (0 == bloc_commun)
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitBlocksWithOgrid::createVertices, on ne trouve pas de bloc commun aux 2 cofaces internes", TkUtil::Charset::UTF_8));

                    Utils::Math::Point barycentre_blk2;
                    std::vector<Vertex* > vertices2;
                    bloc_commun->getVertices(vertices2);

                    for (uint i=0; i<vertices2.size(); i++)
                        barycentre_blk2 += vertices2[i]->getCoord();
                    barycentre_blk2 /= (double)vertices2.size();

                    Utils::Math::Point pt = (barycentre_blk2 + (sommet->getCoord() - barycentre_blk2)*m_ratio_ogrid);
                    newVtx = new Topo::Vertex(getContext(), pt);
                    getInfoCommand().addTopoInfoEntity(newVtx, Internal::InfoCommand::CREATED);
                    corr_vertex[sommet] = newVtx;

                }
                else {
                    // dans tous les autres cas, on ne fait rien pour le sommet
                }

            } // end else if (0 == newVtx)
        } // end for iter6
    } // end for iter1
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocksWithOgrid::
createCoEdges(std::map<Vertex*, uint> & filtre_vertex,
        std::map<Vertex*, Vertex*> & corr_vertex,
        std::map<Vertex*, CoEdge*> & corr_vtx_coedge)
{
#ifdef _DEBUG_SPLIT_OGRID
	std::cout<<"CommandSplitBlocksWithOgrid::createCoEdges(...)"<<std::endl;
#endif
    // il ne faut pas parcourir la map, au risque que l'ordre de création dépende de l'adresse
    // donc on le fait à partir des blocs
    for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
            iter1 != m_blocs.end(); ++iter1){

        Block* bloc = *iter1;

        std::vector<Vertex* > vertices;
        bloc->getAllVertices(vertices);

        for (std::vector<Vertex* >::iterator iter6 = vertices.begin();
                iter6 != vertices.end(); ++iter6){
            CoEdge* newCoEdge = corr_vtx_coedge[*iter6];

            if (0 == newCoEdge){
                // cas où l'arête n'a pas été construite pour un autre bloc
                // pas d'arête pour les sommets internes
                if (filtre_vertex[*iter6] != 2){
                    Vertex* newVtx = corr_vertex[*iter6];
                    if (0 == newVtx)
                        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitBlocksWithOgrid::createCoEdges, on ne retrouve pas de sommet opposé au sommet", TkUtil::Charset::UTF_8));

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
void CommandSplitBlocksWithOgrid::
createCoEdgeAndFace(std::map<Vertex*, uint> & filtre_vertex,
        std::map<Vertex*, Vertex*>  & corr_vertex,
        std::map<Vertex*, CoEdge*> & corr_vtx_coedge,
        std::map<CoEdge*, uint> & filtre_coedge,
        std::map<CoFace*, uint> & filtre_coface,
        std::map<Face*,  uint> & filtre_face,
        std::map<Block*,  uint> & filtre_bloc,
        std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge,
        std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface)
{
    // pour chaque arêtes sur le bord de la sélection (avec une marque de 1 ou 2)
    // création de l'arête interne à l'ogrid
    // et de la face commune entre ces deux arêtes
#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"createCoEdgeAndFace ..."<<std::endl;
#endif
    for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
            iter1 != m_blocs.end(); ++iter1){

        Block* bloc = *iter1;
#ifdef _DEBUG_SPLIT_OGRID
        std::cout<<"bloc: "<<bloc->getName()<<std::endl;
#endif

        // on ne fait rien sur les blocs internes
        if (filtre_bloc[bloc] == 2)
            continue;

        // on procède par Face
        // et si cette Face est composée de Face commune interne et externe
        // alors on procède par face commune
        std::vector<Face* > faces;
        bloc->getFaces(faces);

        for (std::vector<Face* >::iterator iter2 = faces.begin();
                iter2 != faces.end(); ++iter2){

            Face* face = *iter2;
#ifdef _DEBUG_SPLIT_OGRID
            std::cout<<"face: "<<face->getName()<<", filtre à "<<filtre_face[face]<<std::endl;
#endif
            if (filtre_face[face] == 1){
                // les arêtes suivant les différentes directions
                std::vector<CoEdge* > iCoedges;
                std::vector<CoEdge* > jCoedges;
                face->getOrientedCoEdges(iCoedges, jCoedges);

                // boucle sur les arêtes
                for (uint i=0; i<4; i++){

                    // couple de sommets qui déterminent une arête du bloc
                    Vertex* vtx0 = face->getVertex(TopoHelper::tabIndVtxByEdgeOnFace[i][0]);
                    Vertex* vtx1 = face->getVertex(TopoHelper::tabIndVtxByEdgeOnFace[i][1]);

                    // groupsName pour la face (on prend ceux de la première coface, si elle est entre 2 blocs)
                    std::vector<std::string> groupsName;

                    // Création des arêtes et coface en fonction d'un couple de sommets
                    createCoEdgeAndFace(vtx0, vtx1,
                            iCoedges, jCoedges,
                            corr_vertex, corr_vtx_coedge, filtre_vertex, filtre_coedge,
                            groupsName,
                            corr_2vtx_coedge, corr_2vtx_coface);

                } // end for i<4
            } // end if (filtre_face[face] == 1)
            else {
                std::vector<CoFace* > cofaces;
                face->getCoFaces(cofaces);

                // boucle sur les CoFaces
                for (std::vector<CoFace* >::iterator iter3 = cofaces.begin();
                        iter3 != cofaces.end(); ++iter3){

                    CoFace* coface = *iter3;
#ifdef _DEBUG_SPLIT_OGRID
                    std::cout<<"coface: "<<coface->getName()<<std::endl;
#endif

                    // groupsName pour la face (on prend ceux de la première coface, si elle est entre 2 blocs)
                    std::vector<std::string> groupsName;

                    // les arêtes suivant les différentes directions
                    std::vector<CoEdge* > iCoedges;
                    std::vector<CoEdge* > jCoedges;
                    coface->getOrientedCoEdges(iCoedges, jCoedges);

                    // boucle sur les arêtes
                    for (uint i=0; i<4; i++){

                        // couple de sommets qui déterminent une arête du bloc
                        Vertex* vtx0 = coface->getVertex(TopoHelper::tabIndVtxByEdgeOnFace[i][0]);
                        Vertex* vtx1 = coface->getVertex(TopoHelper::tabIndVtxByEdgeOnFace[i][1]);

                        // Création des arêtes et coface en fonction d'un couple de sommets
                        createCoEdgeAndFace(vtx0, vtx1,
                                iCoedges, jCoedges,
                                corr_vertex, corr_vtx_coedge, filtre_vertex, filtre_coedge,
                                groupsName,
                                corr_2vtx_coedge, corr_2vtx_coface);

                    } // end for i<4

                } // end for iter3
            } // end else / if (filtre_face[face] == 1)
        } // end for iter2
    } // end for iter1
}
/*----------------------------------------------------------------------------*/
std::vector<Vertex*> CommandSplitBlocksWithOgrid::
getSelectedVertices(Vertex* vtx0,
        Vertex*vtx1,
        std::vector<CoEdge* > & iCoedges,
        std::vector<CoEdge* > & jCoedges,
        std::map<Vertex*, uint> & filtre_vertex)
{
    // recherche des arêtes entre les 2 sommets
    std::vector<CoEdge* > coedges_between;
    if (!TopoHelper::getCoEdgesBetweenVertices(vtx0, vtx1, iCoedges, coedges_between))
        if (!TopoHelper::getCoEdgesBetweenVertices(vtx0, vtx1, jCoedges, coedges_between))
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitBlocksWithOgrid::getSelectedVertices, on ne retrouve pas de groupes d'arêtes entre 2 sommets", TkUtil::Charset::UTF_8));

    if (coedges_between.empty())
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitBlocksWithOgrid::getSelectedVertices, aucune arête !!!", TkUtil::Charset::UTF_8));

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
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitBlocksWithOgrid::getSelectedVertices, aucune arête voisine de vtx_prec", TkUtil::Charset::UTF_8));

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
void CommandSplitBlocksWithOgrid::
createCoEdgeAndFace(Vertex* vtx0,
        Vertex*vtx1,
        std::vector<CoEdge* > & iCoedges,
        std::vector<CoEdge* > & jCoedges,
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

    // recherche des sommets marqués (sélectionnés)
    std::vector<Vertex*> vertices = getSelectedVertices(vtx0, vtx1, iCoedges, jCoedges, filtre_vertex);

    // pour chaques couples de sommets
    for (uint i=0; i<vertices.size()-1; i++) {
        Vertex* som0 = vertices[i];
        Vertex* som1 = vertices[i+1];

        // cas des arêtes internes avec les 2 sommets internes
        if (filtre_vertex[som0] == 2 && filtre_vertex[som1] == 2)
            continue;

        // on ordonne les sommets suivant leur id
        TopoHelper::sortVertices(som0, som1);

        // recherche des arêtes entre les 2 sommets
        std::vector<CoEdge* > coedges_between;
        if (!TopoHelper::getCoEdgesBetweenVertices(som0, som1, iCoedges, coedges_between))
            if (!TopoHelper::getCoEdgesBetweenVertices(som0, som1, jCoedges, coedges_between))
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitBlocksWithOgrid::createCoEdgeAndFace, on ne retrouve pas de groupes d'arêtes entre 2 sommets", TkUtil::Charset::UTF_8));

        if (coedges_between.empty())
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitBlocksWithOgrid::createCoEdgeAndFace, aucune arête !!!", TkUtil::Charset::UTF_8));

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
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitBlocksWithOgrid::createCoEdgeAndFace, pas de sommets internes pour la nouvelle arête interne", TkUtil::Charset::UTF_8));
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
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans CommandSplitBlocksWithOgrid::createCoEdgeAndFace, pas d'arêtes entre sommets externes et internes", TkUtil::Charset::UTF_8));

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

    } // end for i<vertices.size()-1
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocksWithOgrid::
createCoFace(std::map<Vertex*, uint> & filtre_vertex,
           std::map<Vertex*, Vertex*>  & corr_vertex,
           std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge,
           std::map<CoFace*, uint> & filtre_coface,
           std::map<Face*,  uint> & filtre_face,
           std::map<Block*,  uint> & filtre_bloc,
           std::map<CoFace*, CoFace*> & corr_coface)
{
#ifdef _DEBUG_SPLIT_OGRID
        std::cout<<"createCoFace ... "<<std::endl;
#endif
    // pour chaque face de bloc au bord de la sélection, création d'une face commune
    // pour les faces de bloc à l'intérieur de la sélection création d'autant de face communes qu'avant
    for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
            iter1 != m_blocs.end(); ++iter1){

        Block* bloc = *iter1;
#ifdef _DEBUG_SPLIT_OGRID
        std::cout<<"bloc: "<<bloc->getName()<<std::endl;
#endif
        // on ne fait rien sur les blocs internes
        if (filtre_bloc[bloc] == 2)
            continue;

        std::vector<Face* > faces;
        bloc->getFaces(faces);

        for (std::vector<Face* >::iterator iter2 = faces.begin();
                iter2 != faces.end(); ++iter2){
            Face* face = *iter2;
#ifdef _DEBUG_SPLIT_OGRID
            std::cout<<"face: "<<face->getName()<<std::endl;
#endif

            // les arêtes suivant les différentes directions
            std::vector<CoEdge* > iCoedges;
            std::vector<CoEdge* > jCoedges;
            face->getOrientedCoEdges(iCoedges, jCoedges);

            if (face->getNbCoFaces() == 1){

                CoFace* coface = face->getCoFace(0);
#ifdef _DEBUG_SPLIT_OGRID
                std::cout<<"coface (unique pour la face): "<<coface->getName()<<std::endl;
#endif
                CoFace* newCoface = corr_coface[coface];

                // la face commune est-elle déjà créée ?
                if (0 == newCoface){
                    if (filtre_coface[coface] == 3)
                        corr_coface[coface] = coface;
                    else {
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
                                        iCoedges, jCoedges,
                                        corr_vertex,
                                        filtre_vertex,
                                        corr_2vtx_coedge);

                            edges.push_back(newEdge);
                        } // end for i<4

                        // création de la face commune en rapport avec la Face
                        newCoface = new Topo::CoFace(getContext(), edges[0], edges[1], edges[2], edges[3]);
                        getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);
                        corr_coface[coface] = newCoface;

                    } // end else / if (filtre_coface[coface] == 3)
                } // end if (0 == newCoface)

            } // end if (face->getNbCoFaces() == 1)
            else {
                // cas où chaques cofaces est associée à une autre coface
                if (filtre_face[face] == 2){
                    std::vector<CoFace* > cofaces;
                    face->getCoFaces(cofaces);

                    for (std::vector<CoFace* >::iterator iter3 = cofaces.begin();
                            iter3 != cofaces.end(); ++iter3){

                        CoFace* coface = *iter3;
#ifdef _DEBUG_SPLIT_OGRID
                        std::cout<<"coface: "<<coface->getName()<<std::endl;
#endif
                        CoFace* newCoface = corr_coface[coface];

                        // la face commune est-elle déjà créée ?
                        if (0 == newCoface){
                            if (filtre_coface[coface] == 3)
                                corr_coface[coface] = coface;
                            else {
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
                                                iCoedges, jCoedges,
                                                corr_vertex,
                                                filtre_vertex,
                                                corr_2vtx_coedge);

                                    edges.push_back(newEdge);
                                } // end for i<4

                                // création de la face commune en rapport avec la Face
                                newCoface = new Topo::CoFace(getContext(), edges[0], edges[1], edges[2], edges[3]);
                                getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);
                                corr_coface[coface] = newCoface;

                            } // end else / if (filtre_coface[coface] == 3)
                        } // end if (0 == newCoface)

                    } // end for iter3
                }
                else if (filtre_face[face] == 1) {
                    // cas ou c'est toute la face qui donne une unique coface
#ifdef _DEBUG_SPLIT_OGRID
                    std::cout<<"    face avec de multiples ("<<face->getNbCoFaces()
                                <<") cofaces, mais pas décomposée."<<std::endl;
#endif

                    // on ne teste pas que l'on a déjà fait cette opération pour la face,
                    // car les faces ne sont référencées qu'une fois (par un bloc)

                    std::vector<Edge* > edges;
                    // boucle sur les 4 côtés
                    for (uint i=0; i<4; i++){

                        Edge* newEdge;
                        Vertex* som0 = face->getVertex(i);
                        Vertex* som1 = face->getVertex((i+1)%4);

                        newEdge = createEdge(
                                som0,
                                som1,
                                iCoedges, jCoedges,
                                corr_vertex,
                                filtre_vertex,
                                corr_2vtx_coedge);

                        edges.push_back(newEdge);
                    } // end for i<4

                    // création de la face commune en rapport avec la Face
                    CoFace* newCoface;
                    newCoface = new Topo::CoFace(getContext(), edges[0], edges[1], edges[2], edges[3]);
                    getInfoCommand().addTopoInfoEntity(newCoface, Internal::InfoCommand::CREATED);

                    std::vector<CoFace* > cofaces;
                    face->getCoFaces(cofaces);
                    for (std::vector<CoFace* >::iterator iter3 = cofaces.begin();
                            iter3 != cofaces.end(); ++iter3)
                        corr_coface[*iter3] = newCoface;

                } // end else if (filtre_face[face] == 2)
            } // end else / if (face->getNbCoFaces() == 1)

        } // end for iter2
    } // end for iter1
}
/*----------------------------------------------------------------------------*/
Edge* CommandSplitBlocksWithOgrid::
createEdge(Vertex* vtx0,
        Vertex*vtx1,
        std::vector<CoEdge* > & iCoedges,
        std::vector<CoEdge* > & jCoedges,
        std::map<Vertex*, Vertex*>  & corr_vertex,
        std::map<Vertex*, uint> & filtre_vertex,
        std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge)
{
#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"createEdge("<<(vtx0?vtx0->getName():"0")<<","<<(vtx1?vtx1->getName():"0")<<")"<<std::endl;
#endif
    // on ordonne les sommets suivant leur id
    TopoHelper::sortVertices(vtx0, vtx1);

    std::vector<Vertex*> vertices = getSelectedVertices(vtx0, vtx1, iCoedges, jCoedges, filtre_vertex);

    // pour chaques couples de sommets, on recherche l'arête correspondante
    std::vector<CoEdge* > newCoedges;

    for (uint i=0; i<vertices.size()-1; i++) {
        Vertex* som0 = vertices[i];
        Vertex* som1 = vertices[i+1];

        // on ordonne les sommets suivant leur id
        TopoHelper::sortVertices(som0, som1);

        CoEdge* newCoedge = corr_2vtx_coedge[std::pair<Vertex*, Vertex*>(som0, som1)];
        if (0 == newCoedge){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        	message <<"Erreur interne dans CommandSplitBlocksWithOgrid::createEdge, pas d'arêtes interne face à 2 sommets ("
        			<< som0->getName()<<","<< som1->getName()<<")";
        	throw TkUtil::Exception(message);
        }
        newCoedges.push_back(newCoedge);
    } // end for i<vertices.size()-1

    // création de l'arête interne
    Edge* newEdge;
    if (newCoedges.size() == 1)
        newEdge = new Topo::Edge(getContext(),newCoedges[0]);
    else
        newEdge = new Topo::Edge(getContext(), corr_vertex[vtx0], corr_vertex[vtx1], newCoedges);

    getInfoCommand().addTopoInfoEntity(newEdge, Internal::InfoCommand::CREATED);

    return newEdge;
}
/*----------------------------------------------------------------------------*/
Face* CommandSplitBlocksWithOgrid::
createFace(Vertex* vtx0,
        Vertex*vtx1,
        std::vector<CoEdge* > & iCoedges,
        std::vector<CoEdge* > & jCoedges,
        std::map<Vertex*, Vertex*>  & corr_vertex,
        std::map<Vertex*, uint> & filtre_vertex,
        std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface)
{
#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"createFace("<<(vtx0?vtx0->getName():"0")<<","<<(vtx1?vtx1->getName():"0")<<")"<<std::endl;
#endif
    // on ordonne les sommets suivant leur id
    TopoHelper::sortVertices(vtx0, vtx1);

    CoFace* newCoface = corr_2vtx_coface[std::pair<Vertex*, Vertex*>(vtx0, vtx1)];
    Face* newFace = 0;

    if (0 == newCoface){
        std::vector<Vertex*> vertices = getSelectedVertices(vtx0, vtx1, iCoedges, jCoedges, filtre_vertex);

        // pour chaques couples de sommets, on recherche la face correspondante
        std::vector<CoFace* > newCofaces;

        for (uint i=0; i<vertices.size()-1; i++) {
            Vertex* som0 = vertices[i];
            Vertex* som1 = vertices[i+1];

            // on ordonne les sommets suivant leur id
            TopoHelper::sortVertices(som0, som1);
            newCoface = corr_2vtx_coface[std::pair<Vertex*, Vertex*>(som0, som1)];

            if (0 == newCoface){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message <<"CommandSplitBlocksWithOgrid::createFace ne trouve pas de face pour les sommets "
                        << som0->getName()<<","<< som1->getName();
                throw TkUtil::Exception(message);
            }

            newCofaces.push_back(newCoface);
        } /// end for i

        std::vector<Vertex* > newVertices;
        newVertices.push_back(vtx0);
        newVertices.push_back(vtx1);
        newVertices.push_back(corr_vertex[vtx1]);
        newVertices.push_back(corr_vertex[vtx0]);

        newFace = new Topo::Face(getContext(), newCofaces, newVertices, true);
        getInfoCommand().addTopoInfoEntity(newFace, Internal::InfoCommand::CREATED);
#ifdef _DEBUG_SPLIT_OGRID
        std::cout<<" => création de "<<newFace->getName()<<" avec "<<newCofaces.size()<<" cofaces"<<std::endl;
#endif

    } // end if (0 == newCoface)
    else {

        newFace = new Topo::Face(getContext(),newCoface);
        getInfoCommand().addTopoInfoEntity(newFace, Internal::InfoCommand::CREATED);
#ifdef _DEBUG_SPLIT_OGRID
        std::cout<<" => création de "<<newFace->getName()<<" avec une coface"<<std::endl;
#endif

    } // end else / if (0 == newCoface)

    return newFace;
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocksWithOgrid::
createBlock(std::map<Vertex*, uint> & filtre_vertex,
        std::map<Vertex*, Vertex*>  & corr_vertex,
        std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface,
        std::map<CoFace*, uint> & filtre_coface,
        std::map<CoFace*, CoFace*> & corr_coface,
        std::map<Face*,  uint> & filtre_face,
        std::map<Block*,  uint> & filtre_bloc)
{
#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"createBlock ..."<<std::endl;
#endif
    // création des blocs
    // un par coface ou par face au bord de la sélection suivant filtre sur les faces
    for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
            iter1 != m_blocs.end(); ++iter1){

        Block* bloc = *iter1;
#ifdef _DEBUG_SPLIT_OGRID
        std::cout<<"bloc: "<<bloc->getName()<<std::endl;
#endif
        // rien pour les blocs internes
        if (filtre_bloc[bloc] == 2)
            continue;

        std::vector<Face* > faces;
        bloc->getFaces(faces);

        for (std::vector<Face* >::iterator iter2 = faces.begin();
                iter2 != faces.end(); ++iter2){
            Face* face = *iter2;
#ifdef _DEBUG_SPLIT_OGRID
            std::cout<<"face: "<<face->getName()<<std::endl;
#endif

            // les arêtes suivant les différentes directions
            std::vector<CoEdge* > iCoedges;
            std::vector<CoEdge* > jCoedges;
            face->getOrientedCoEdges(iCoedges, jCoedges);

            if (filtre_face[face] == 2 || face->getNbCoFaces() == 1){
                // cas de la face qui donne un bloc par coface

                std::vector<CoFace* > cofaces;
                face->getCoFaces(cofaces);

                for (std::vector<CoFace* >::iterator iter3 = cofaces.begin();
                        iter3 != cofaces.end(); ++iter3){

                    CoFace* coface = *iter3;
#ifdef _DEBUG_SPLIT_OGRID
                    std::cout<<"coface: "<<coface->getName()<<std::endl;
#endif

                    if (filtre_coface[coface] == 1){
                        std::vector<Face* > newFaces;
                        std::vector<Vertex* > newVertices;

                        newVertices.push_back(coface->getVertex(0));
                        newVertices.push_back(corr_vertex[newVertices[0]]);
                        newVertices.push_back(coface->getVertex(1));
                        newVertices.push_back(corr_vertex[newVertices[2]]);
                        newVertices.push_back(coface->getVertex(3));
                        newVertices.push_back(corr_vertex[newVertices[4]]);
                        newVertices.push_back(coface->getVertex(2));
                        newVertices.push_back(corr_vertex[newVertices[6]]);

                        if (cofaces.size() == 1)
                            newFaces.push_back(face);
                        else {
                            // création d'une face avec juste cette coface
                            newFaces.push_back(new Topo::Face(getContext(), coface));
                            getInfoCommand().addTopoInfoEntity(newFaces.back(), Internal::InfoCommand::CREATED);
#ifdef _DEBUG_SPLIT_OGRID
                            std::cout<<" => création de "<<newFaces.back()->getName()<<std::endl;
#endif
                        }

                        // la face opposée
                        newFaces.push_back(new Topo::Face(getContext(), corr_coface[coface]));
                        getInfoCommand().addTopoInfoEntity(newFaces.back(), Internal::InfoCommand::CREATED);
#ifdef _DEBUG_SPLIT_OGRID
                        std::cout<<" => création de "<<newFaces.back()->getName()<<" comme face opposée"<<std::endl;
#endif

                        // les faces issues des arêtes de la face
                        newFaces.push_back(createFace(newVertices[0], newVertices[4],
                                iCoedges, jCoedges,
                                corr_vertex,
                                filtre_vertex,
                                corr_2vtx_coface));
                        newFaces.push_back(createFace(newVertices[2], newVertices[6],
                                iCoedges, jCoedges,
                                corr_vertex,
                                filtre_vertex,
                                corr_2vtx_coface));
                        newFaces.push_back(createFace(newVertices[0], newVertices[2],
                                iCoedges, jCoedges,
                                corr_vertex,
                                filtre_vertex,
                                corr_2vtx_coface));
                        newFaces.push_back(createFace(newVertices[4], newVertices[6],
                                iCoedges, jCoedges,
                                corr_vertex,
                                filtre_vertex,
                                corr_2vtx_coface));

                        // création du bloc
                        Block* newBlock;
                        newBlock = new Topo::Block(getContext(), newFaces, newVertices, true);
                        getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);

                        // met les mêmes groupes que pour le bloc découpé
                        for (uint i=0; i<bloc->getGroupsContainer().size(); i++){
                        	Group::Group3D* gr = bloc->getGroupsContainer().get(i);
                        	gr->add(newBlock);
                        	newBlock->getGroupsContainer().add(gr);
                        }

                        // copie le lien sur la géométrie
                        newBlock->setGeomAssociation(bloc->getGeomAssociation());

                        if (bloc->getMeshLaw() == BlockMeshingProperty::directional)
                        	newBlock->selectBasicMeshLaw(&getInfoCommand());

                    } // end if filtre_coface[...] == 1
                } // end iter3
            } // end if (filtre_face[face] == 2 || face->getNbCoFaces() == 1)
            else if (filtre_face[face] == 1 && filtre_coface[face->getCoFace(0)] == 1){
                // cas de la face qui ne donne qu'un bloc (bien qu'il y ait plusieurs cofaces)
#ifdef _DEBUG_SPLIT_OGRID
                std::cout<<"face non décomposée"<<std::endl;
#endif
                std::vector<Face* > newFaces;
                std::vector<Vertex* > newVertices;

                newVertices.push_back(face->getVertex(0));
                newVertices.push_back(corr_vertex[newVertices[0]]);
                newVertices.push_back(face->getVertex(1));
                newVertices.push_back(corr_vertex[newVertices[2]]);
                newVertices.push_back(face->getVertex(3));
                newVertices.push_back(corr_vertex[newVertices[4]]);
                newVertices.push_back(face->getVertex(2));
                newVertices.push_back(corr_vertex[newVertices[6]]);

                // la face de départ
                newFaces.push_back(face);

                // la face opposée
                newFaces.push_back(new Topo::Face(getContext(), corr_coface[face->getCoFace(0)]));
                getInfoCommand().addTopoInfoEntity(newFaces.back(), Internal::InfoCommand::CREATED);
#ifdef _DEBUG_SPLIT_OGRID
                std::cout<<" => création de "<<newFaces.back()->getName()<<" comme face opposée"<<std::endl;
#endif

                // les faces issues des arêtes de la face
                newFaces.push_back(createFace(newVertices[0], newVertices[4],
                        iCoedges, jCoedges,
                        corr_vertex,
                        filtre_vertex,
                        corr_2vtx_coface));
                newFaces.push_back(createFace(newVertices[2], newVertices[6],
                        iCoedges, jCoedges,
                        corr_vertex,
                        filtre_vertex,
                        corr_2vtx_coface));
                newFaces.push_back(createFace(newVertices[0], newVertices[2],
                        iCoedges, jCoedges,
                        corr_vertex,
                        filtre_vertex,
                        corr_2vtx_coface));
                newFaces.push_back(createFace(newVertices[4], newVertices[6],
                        iCoedges, jCoedges,
                        corr_vertex,
                        filtre_vertex,
                        corr_2vtx_coface));

                // création du bloc
                Block* newBlock;
                newBlock = new Topo::Block(getContext(), newFaces, newVertices, true);
                getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);

                // met les mêmes groupes que pour le bloc découpé
                for (uint i=0; i<bloc->getGroupsContainer().size(); i++){
                	Group::Group3D* gr = bloc->getGroupsContainer().get(i);
                	gr->add(newBlock);
                	newBlock->getGroupsContainer().add(gr);
                }

                // copie le lien sur la géométrie
                newBlock->setGeomAssociation(bloc->getGeomAssociation());

                if (bloc->getMeshLaw() == BlockMeshingProperty::directional)
                	newBlock->selectBasicMeshLaw(&getInfoCommand());

            } // end else if (filtre_face[face] == 1)
        else {
#ifdef _DEBUG_SPLIT_OGRID
            std::cout<<"  face qui ne donne pas de bloc, elle contient les cofaces: ";
            std::vector<CoFace* > cofaces;
            face->getCoFaces(cofaces);
            for (std::vector<CoFace* >::iterator iter3 = cofaces.begin();
                    iter3 != cofaces.end(); ++iter3)
                std::cout<<(*iter3)->getName();
#endif
            }
        } // end iter2

        // le bloc central
        std::vector<Face* > newFaces;
        std::vector<Vertex* > newVertices;

        for (uint i=0; i<8; i++)
            newVertices.push_back(corr_vertex[bloc->getVertex(i)]);

        for (uint i=0; i<6; i++)
            newFaces.push_back(createFace(faces[i], corr_vertex, corr_coface));

        Block* newBlock;
        newBlock = new Topo::Block(getContext(), newFaces, newVertices, true);
        getInfoCommand().addTopoInfoEntity(newBlock, Internal::InfoCommand::CREATED);

        // met les mêmes groupes que pour le bloc découpé
        for (uint i=0; i<bloc->getGroupsContainer().size(); i++){
        	Group::Group3D* gr = bloc->getGroupsContainer().get(i);
        	gr->add(newBlock);
        	newBlock->getGroupsContainer().add(gr);
        }

        // copie le lien sur la géométrie
        newBlock->setGeomAssociation(bloc->getGeomAssociation());

        if (bloc->getMeshLaw() == BlockMeshingProperty::directional)
        	newBlock->selectBasicMeshLaw(&getInfoCommand());

    } // end iter1
}
/*----------------------------------------------------------------------------*/
Face* CommandSplitBlocksWithOgrid::
createFace(Face* face,
        std::map<Vertex*, Vertex*>  & corr_vertex,
        std::map<CoFace*, CoFace*> & corr_coface)
{
#ifdef _DEBUG_SPLIT_OGRID
    std::cout<<"createFace("<<face->getName()<<")"<<std::endl;
#endif

    std::vector<CoFace* > cofaces;
    face->getCoFaces(cofaces);

    Face* newFace = 0;

    if (cofaces.size() == 1){
        CoFace* coface = face->getCoFace(0);
#ifdef _DEBUG_SPLIT_OGRID
        std::cout<<"coface (unique): "<<coface->getName()<<std::endl;
#endif
        CoFace* newCoface = corr_coface[coface];
        if (0 == newCoface){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message <<"CommandSplitBlocksWithOgrid::createFace ne trouve pas de coface pour coface "
                    << coface->getName();
            throw TkUtil::Exception(message);
        }

        // on conserve la face initiale si la face commune est identique
        if (newCoface == coface)
            newFace = face;
        else {
            newFace = new Topo::Face(getContext(), newCoface);
            getInfoCommand().addTopoInfoEntity(newFace, Internal::InfoCommand::CREATED);
#ifdef _DEBUG_SPLIT_OGRID
            std::cout<<" => création de "<<newFace->getName()<<" avec 1 seule coface"<<std::endl;
#endif
        }
    }
    else {

        std::vector<CoFace* > newCofaces;
        for (std::vector<CoFace* >::iterator iter3 = cofaces.begin();
                iter3 != cofaces.end(); ++iter3){
            CoFace* coface = *iter3;
#ifdef _DEBUG_SPLIT_OGRID
            std::cout<<"coface: "<<coface->getName()<<std::endl;
#endif
            CoFace* newCoface = corr_coface[coface];
            if (0 == newCoface){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message <<"CommandSplitBlocksWithOgrid::createFace ne trouve pas de coface pour coface "
                        << coface->getName();
                throw TkUtil::Exception(message);
            }
            // on évite de mettre la même 2 fois de suite (ce qui apparait avec filtre_face à 1)
            if (newCofaces.empty() || newCoface != newCofaces.back())
                newCofaces.push_back(newCoface);
        } // end for iter3

        std::vector<Vertex* > vertices;
        face->getVertices(vertices);
        std::vector<Vertex* > newVertices;

        for (std::vector<Vertex* >::iterator iter6 = vertices.begin();
                iter6 != vertices.end(); ++iter6){
            Vertex* sommet = *iter6;
            Vertex* newVtx = corr_vertex[sommet];

            if (0 == newVtx){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message <<"CommandSplitBlocksWithOgrid::createFace ne trouve pas de sommet pour le sommet "
                        << sommet->getName();
                throw TkUtil::Exception(message);
            }

            newVertices.push_back(newVtx);
        } // end for iter6

        newFace = new Topo::Face(getContext(), newCofaces, newVertices, true);
        getInfoCommand().addTopoInfoEntity(newFace, Internal::InfoCommand::CREATED);
#ifdef _DEBUG_SPLIT_OGRID
        std::cout<<" => création de "<<newFace->getName()<<" avec "<<newCofaces.size()<<" cofaces"<<std::endl;
        for (uint i=0; i<newCofaces.size(); i++)
        	std::cout<<" newCofaces["<<i<<"]: "<<*newCofaces[i];
        for (uint i=0; i<newVertices.size(); i++)
        	std::cout<<" newVertices["<<i<<"]: "<<*newVertices[i]<<std::endl;
        std::cout<<" newFace : "<<*newFace<<std::endl;
#endif

    } // end else / if (cofaces.size() == 1)

    return newFace;
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocksWithOgrid::
freeUnused(std::map<Vertex*, uint> & filtre_vertex,
        std::map<CoEdge*, uint> & filtre_coedge,
		std::map<CoFace*, uint> & filtre_coface,
		std::map<Block*,  uint> & filtre_bloc,
		std::map<Vertex*, CoEdge*> & corr_vtx_coedge,
		std::map<std::pair<Vertex*, Vertex*>, CoEdge*> & corr_2vtx_coedge,
		std::map<std::pair<Vertex*, Vertex*>, CoFace*> & corr_2vtx_coface,
		std::map<CoFace*, CoFace*> & corr_coface)
{
#ifdef _DEBUG_SPLIT_OGRID
	std::cout<<"freeUnused ..."<<std::endl;
#endif

	/// liste des arêtes communes à détruire (ou à remplacer par... pour MAJ des arêtes)
	std::list<CoEdge*> coedgesToBeDeleted;

	// filtre sur les edges pour stocker d'une part celles mises à jour (marque à 2)
	// celles à mettre à jour par la suite (marque à 1)
	std::map<Edge*,  uint>  filtre_edge;

	// supression des blocs, des faces et cofaces internes (reprise des projections pour ces dernières)
	for (std::vector<Block* >::iterator iter1 = m_blocs.begin();
			iter1 != m_blocs.end(); ++iter1){

		Block* bloc = *iter1;
#ifdef _DEBUG_SPLIT_OGRID
		std::cout<<"bloc: "<<bloc->getName()<<std::endl;
#endif
		// les blocs au centre sont conservés
		if (filtre_bloc[bloc] == 2)
			continue;

		std::vector<Face* > faces;
		bloc->getFaces(faces);

		for (std::vector<Face* >::iterator iter2 = faces.begin();
				iter2 != faces.end(); ++iter2){
			Face* face = *iter2;
#ifdef _DEBUG_SPLIT_OGRID
			std::cout<<"face: "<<face->getName()<<std::endl;
#endif
			std::vector<CoFace* > cofaces;
			face->getCoFaces(cofaces);

			bool faceToBeDeleted = false;
			for (std::vector<CoFace* >::iterator iter3 = cofaces.begin();
					iter3 != cofaces.end(); ++iter3){

				CoFace* coface = *iter3;
#ifdef _DEBUG_SPLIT_OGRID
				std::cout<<"coface: "<<coface->getName()<<std::endl;
#endif

				if (filtre_coface[coface] == 2){
					faceToBeDeleted = true;

					// reprise des projections
					Geom::GeomEntity* ga = coface->getGeomAssociation();
					if (ga){
						// pour la coface centrale
						CoFace* newCoface = corr_coface[coface];
						if (newCoface && newCoface != coface) {
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

					if (!coface->getGroupsContainer().empty()) {
						// pour la coface centrale
						CoFace* newCoface = corr_coface[coface];
						if (newCoface && newCoface != coface) {
							// met les mêmes groupes que pour la face découpée
							for (uint i=0; i<coface->getGroupsContainer().size(); i++){
								Group::Group2D* gr = coface->getGroupsContainer().get(i);
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
								if (newCoface){
									for (uint i=0; i<coface->getGroupsContainer().size(); i++){
										Group::Group2D* gr = coface->getGroupsContainer().get(i);
										gr->add(newCoface);
										newCoface->getGroupsContainer().add(gr);
									}
								}
							}
						}
					} // end if (!coface->getGroupsContainer().empty())

					std::vector<Edge* > edges;
					coface->getEdges(edges);
					for (std::vector<Edge* >::iterator iter4 = edges.begin();
							iter4 != edges.end(); ++iter4){
						Edge* edge = *iter4;
						bool edgeToBeDeleted = true;

						filtre_edge[edge] = 2;

						std::vector<CoEdge* > loc_coedges;
						edge->getCoEdges(loc_coedges);

						// recherche si l'une des arêtes est reliée à une coface qui n'est pas détruite
						std::vector<CoFace* > neighborCofaces;
						edge->getCoFaces(neighborCofaces);
#ifdef _DEBUG_SPLIT_OGRID
						std::cout<<"  observation des faces reliées à "<<edge->getName()<<" : "<<std::endl;
						for (std::vector<CoFace* >::iterator iter6 = neighborCofaces.begin();
								iter6 != neighborCofaces.end(); ++iter6)
							std::cout<<" "<<(*iter6)->getName();
						std::cout<<std::endl;
#endif

						bool faceToBeDeleted = false;
						for (std::vector<CoFace* >::iterator iter6 = neighborCofaces.begin();
								iter6 != neighborCofaces.end(); ++iter6)
							if (filtre_coface[*iter6] == 0)
								edgeToBeDeleted = false;

						if (!edgeToBeDeleted){
#ifdef _DEBUG_SPLIT_OGRID
							std::cout<<"  conservation et mise à jour de "<<edge->getName()<<std::endl;
#endif
							//edgesToBeUpdated.push_back(edge);
							filtre_edge[edge] = 1;
						}

						// suppression des arêtes initiales entre le bord et le centre de la sélection
						for (std::vector<CoEdge* >::iterator iter5 = loc_coedges.begin();
								iter5 != loc_coedges.end(); ++iter5){
							CoEdge* coedge = *iter5;
							if (filtre_coedge[coedge] == 2){

								if (edgeToBeDeleted){

									// marque les edges qui partagent une arête commune
									updateNeighBorFilter(edge, filtre_edge);

#ifdef _DEBUG_SPLIT_OGRID
									std::cout<<"  enregistrée à détruire pour "<<coedge->getName()<<std::endl;
#endif
									coedgesToBeDeleted.push_back(coedge);
									//coedge->free(&getInfoCommand());
								}
							}
						} // end for iter5

						if (edge->getNbCoFaces() == 1){

#ifdef _DEBUG_SPLIT_OGRID
							std::cout<<"  destruction de "<<edge->getName()<<std::endl;
#endif
							edge->free(&getInfoCommand());
						}
					} // end for iter4

					bool cofaceToBeReplaced=false;
					std::vector<Block* > blocs;
					coface->getBlocks(blocs);
					for (uint i=0; i<blocs.size(); i++)
						if (filtre_bloc[blocs[i]] == 0)
							cofaceToBeReplaced = true;

					if (cofaceToBeReplaced && !m_propagate_neighbor_block){
						// recherche de la face qu'il faut mettre à jour
						Face* faceToUpdate = 0;

						std::vector<Face* > faces;
						coface->getFaces(faces);
						if (faces.size() != 2){
							TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
							message <<"Erreur interne, CommandSplitBlocksWithOgrid n'est pas prévu pour le cas avec une seule face pour face commune "
									<< coface->getName();
							throw TkUtil::Exception(message);
						}
						for (uint i=0; i<faces.size(); i++)
							if (faces[i] != face)
								faceToUpdate = faces[i];
						CHECK_NULL_PTR_ERROR(faceToUpdate);

						// pour les faces entre les bords de la face initiale et celle au centre
						for (uint i=0; i<4; i++){
							Vertex* vtx0 = coface->getVertex(i);
							Vertex* vtx1 = coface->getVertex((i+1)%4);
							TopoHelper::sortVertices(vtx0, vtx1);
							CoFace* newCoface = corr_2vtx_coface[std::pair<Vertex*, Vertex*>(vtx0, vtx1)];
							if (newCoface){
								faceToUpdate->addCoFace(newCoface);
								newCoface->addFace(faceToUpdate);
							}
						} // end for i
						CoFace* newCoface = corr_coface[coface];
						if (newCoface){
							faceToUpdate->addCoFace(newCoface);
							newCoface->addFace(faceToUpdate);
						}

					} // if (cofaceToBeReplaced)

					// la face commune doit être détruite
#ifdef _DEBUG_SPLIT_OGRID
					std::cout<<"  destruction de "<<coface->getName()<<std::endl;
#endif
					coface->free(&getInfoCommand());

				} // end if (filtre_coface[coface] == 2)
				else {
					// recherche si l'une des cofaces est référencée par plus de 2 faces
					if (coface->getNbFaces()>2)
						faceToBeDeleted = true;
				}
			} // end for iter3

			if (faceToBeDeleted){
#ifdef _DEBUG_SPLIT_OGRID
				std::cout<<"  destruction de "<<face->getName()<<std::endl;
#endif
				face->free(&getInfoCommand());
			}

		} // end for iter2

		// suppression du bloc
#ifdef _DEBUG_SPLIT_OGRID
		std::cout<<"  destruction de "<<bloc->getName()<<std::endl;
#endif
		bloc->free(&getInfoCommand());

	} // end for iter1


	// destruction des coedges
	coedgesToBeDeleted.sort(Utils::Entity::compareEntity);
	coedgesToBeDeleted.unique();
	for (std::list<CoEdge*>::iterator iter4 = coedgesToBeDeleted.begin();
			iter4 != coedgesToBeDeleted.end(); ++iter4){
		CoEdge* coedge = *iter4;
#ifdef _DEBUG_SPLIT_OGRID
		std::cout<<"  coedge à détruire : "<<coedge->getName()<<std::endl;
#endif
		// recherche si elle est connectée à des edges à mettre à jour
		std::vector<Edge* > edges;
		coedge->getEdges(edges);
		std::vector<Edge* > edgesToBeUpdated;
		for (uint i=0; i<edges.size(); i++)
			if (filtre_edge[edges[i]] == 1)
				edgesToBeUpdated.push_back(edges[i]);

		if (!edgesToBeUpdated.empty()){
			// recherche de ce par quoi elle est remplacée
			std::vector<CoEdge* > newCoEdges;

			std::vector<Vertex* > vertices;
			coedge->getVertices(vertices);

			if (vertices.size() != 2)
				throw TkUtil::Exception("Errer interne dans CommandSplitBlocksWithOgrid::freeUnused, une arête commune n'a pas 2 sommets");

			Vertex* som0 = vertices[0];
			Vertex* som1 = vertices[1];
			// on ordonne les sommets suivant leur id
			TopoHelper::sortVertices(som0, som1);

			CoEdge* newCoedge = 0;
			newCoedge = corr_vtx_coedge[vertices[0]];
			if (newCoedge)
				newCoEdges.push_back(newCoedge);

			newCoedge = corr_2vtx_coedge[std::pair<Vertex*, Vertex*>(som0, som1)];
			if (newCoedge)
				newCoEdges.push_back(newCoedge);


			newCoedge = corr_vtx_coedge[vertices[1]];
			if (newCoedge)
				newCoEdges.push_back(newCoedge);


			// mise à jour des arêtes avant destruction
			for (uint i=0; i<edgesToBeUpdated.size(); i++)
				edgesToBeUpdated[i]->replace(coedge, newCoEdges, &getInfoCommand());
		}

		coedge->free(&getInfoCommand());
	}

	// suppression des sommets et arêtes inutilisées vers le centre de l'ogrid
	for (std::map<Vertex*, CoEdge*>::iterator iter = corr_vtx_coedge.begin();
			iter != corr_vtx_coedge.end(); ++iter){
		CoEdge* coedge = iter->second;
		Vertex* vtx = iter->first;
#ifdef _DEBUG_SPLIT_OGRID
		std::cout<<"corr_vtx_coedge["<<vtx->getName()<<"] -> "<<(coedge?coedge->getName():"0")<<std::endl;
#endif

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
void CommandSplitBlocksWithOgrid::
updateNeighBorFilter(Edge* edge, std::map<Edge*,  uint> & filtre_edge)
{
	std::vector<CoEdge* > coedges;
	edge->getCoEdges(coedges);
	for (std::vector<CoEdge* >::iterator iter1 = coedges.begin();
			iter1 != coedges.end(); ++iter1){
		CoEdge* coedge = *iter1;

		std::vector<Edge* > edges;
		coedge->getEdges(edges);
		for (std::vector<Edge* >::iterator iter2 = edges.begin();
				iter2 != edges.end(); ++iter2){
			Edge* edge = *iter2;

		if (filtre_edge[edge] == 0)
			filtre_edge[edge] = 1;
		}
	}
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocksWithOgrid::
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
void CommandSplitBlocksWithOgrid::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
void CommandSplitBlocksWithOgrid::updateNbMeshingEdges()
{
	SetNbMeshingEdgesImplementation implementation(&getInfoCommand());

    Internal::InfoCommand& icmd = getInfoCommand();
    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>& topo_entities_info = icmd.getTopoInfoEntity();
    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
            iter_tei != topo_entities_info.end(); ++iter_tei){
        Topo::TopoEntity* te = iter_tei->first;
        Internal::InfoCommand::type& t = iter_tei->second;

        if (t == Internal::InfoCommand::CREATED){
        	if (te->getType() == Utils::Entity::TopoCoEdge)
        		implementation.addDelta(dynamic_cast<CoEdge*>(te), 0);
        	else if (te->getType() == Utils::Entity::TopoCoFace)
        		implementation.addTaggedCoFace(dynamic_cast<CoFace*>(te));
        }
    }

	 implementation.execute();
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
