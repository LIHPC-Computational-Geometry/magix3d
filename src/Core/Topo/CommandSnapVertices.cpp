/*----------------------------------------------------------------------------*/
/*
 * \file CommandSnapVertices.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17/9/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSnapVertices.h"

#include "Utils/Common.h"
#include "Topo/Vertex.h"
#include "Topo/TopoHelper.h"
#include "Topo/Block.h"
#include "Topo/BlockTopoProperty.h"
#include "Topo/BlockMeshingProperty.h"
#include "Topo/BlockMeshingPropertyDelaunayTetgen.h"
#include "Topo/BlockMeshingPropertyDirectional.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
//#define _DEBUG_SNAP
/*----------------------------------------------------------------------------*/
CommandSnapVertices::
CommandSnapVertices(Internal::Context& c,
        std::vector<Topo::Vertex* > &vertices1,
        std::vector<Topo::Vertex* > &vertices2,
        bool project_on_first)
:CommandEditTopo(c, "Fusion de sommets")
, m_project_on_first(project_on_first)
, m_common_block(0), m_common_coface(0)
{
#ifdef _DEBUG_SNAP
    std::cout<<"CommandSnapVertices ..."<<std::endl;
#endif

    if (vertices1.size() != vertices2.size())
    	throw TkUtil::Exception (TkUtil::UTF8String ("La fusion entre sommets ne peut se faire, il faut le même nombre de sommets de part et d'autre", TkUtil::Charset::UTF_8));

    // il faut réordonner les sommets pour former les couples que l'on va fusionner
    // et vérifier à l'occasion que l'opération est réalisable, c'est à dire que les sommets
    // appartiennent à un même bloc (ou une coface) et sont à des extrémités reliés par une arête

    // recherche d'un bloc commun à tous les sommets
    uint marque = vertices1.size() + vertices2.size();
    std::map<Topo::Block*, uint> filtre_block;

    for (std::vector<Topo::Vertex* >::iterator iter1 = vertices1.begin();
            iter1 != vertices1.end(); ++iter1){
        std::vector<Block* > blocks;
        (*iter1)->getBlocks(blocks);

        for (std::vector<Topo::Block* >::iterator iter5 = blocks.begin();
                iter5 != blocks.end(); ++iter5){
            filtre_block[*iter5] += 1;
        }
    }

    for (std::vector<Topo::Vertex* >::iterator iter1 = vertices2.begin();
            iter1 != vertices2.end(); ++iter1){
        std::vector<Block* > blocks;
        (*iter1)->getBlocks(blocks);

        for (std::vector<Topo::Block* >::iterator iter5 = blocks.begin();
                iter5 != blocks.end(); ++iter5){
            filtre_block[*iter5] += 1;
            if (filtre_block[*iter5] == marque)
                m_common_block = *iter5;
        }
    }

    if (m_common_block == 0){
    	// on cherche une coface
    	std::map<Topo::CoFace*, uint> filtre_coface;

    	for (std::vector<Topo::Vertex* >::iterator iter1 = vertices1.begin();
    			iter1 != vertices1.end(); ++iter1){
    		std::vector<CoFace* > cofaces;
    		(*iter1)->getCoFaces(cofaces);

    		for (std::vector<Topo::CoFace* >::iterator iter5 = cofaces.begin();
    				iter5 != cofaces.end(); ++iter5){
    			filtre_coface[*iter5] += 1;
    		}
    	}

    	for (std::vector<Topo::Vertex* >::iterator iter1 = vertices2.begin();
    			iter1 != vertices2.end(); ++iter1){
    		std::vector<CoFace* > cofaces;
    		(*iter1)->getCoFaces(cofaces);

    		for (std::vector<Topo::CoFace* >::iterator iter5 = cofaces.begin();
    				iter5 != cofaces.end(); ++iter5){
    			filtre_coface[*iter5] += 1;
                if (filtre_coface[*iter5] == marque)
                    m_common_coface = *iter5;
    		}
    	}
    }

    if (m_common_coface == 0 && m_common_block == 0)
        throw TkUtil::Exception (TkUtil::UTF8String ("La fusion entre sommets ne peut se faire, on ne trouve ni de bloc, ni de face, commun entre tous les sommets", TkUtil::Charset::UTF_8));

#ifdef _DEBUG_SNAP
    std::cout<<"m_common_block = "<<(m_common_block?m_common_block->getName():"")<<std::endl;
    std::cout<<"m_common_coface = "<<(m_common_coface?m_common_coface->getName():"")<<std::endl;
#endif

    // filtre pour marquer les sommets en fonction de leur appartenance aux 2 listes
    std::map<Topo::Vertex*, uint> filtre_vertex;
    for (std::vector<Topo::Vertex* >::iterator iter1 = vertices1.begin();
            iter1 != vertices1.end(); ++iter1)
        filtre_vertex[*iter1] = 1;
    for (std::vector<Topo::Vertex* >::iterator iter1 = vertices2.begin();
            iter1 != vertices2.end(); ++iter1)
    	if (1 == filtre_vertex[*iter1])
    		filtre_vertex[*iter1] = 3; // noeuds dans les 2 listes
    	else
    		filtre_vertex[*iter1] = 2;

    if (m_common_block && m_common_block->isStructured()){

        // la direction suivant laquelle se fait la dégénérescence pour ce bloc
        Block::eDirOnBlock dir_bl = Block::unknown;

        // les 8 sommets du bloc (en dupliquant si bloc dégénéré)
    	std::vector<Vertex* > vertices;
    	m_common_block->getHexaVertices(vertices);

#ifdef _DEBUG_SNAP2
    	std::cout<<"getHexaVertices => ";
    	for (uint i=0; i<vertices.size(); i++)
    		std::cout<<" "<<vertices[i]->getName();
    	std::cout<<std::endl;
#endif


    	// recherche de la direction du bloc suivant laquelle est faite la dégénérescence
    	for (uint i=0; i<3; i++){
    		bool dir_ok = true;
    		for (uint j=0; j<4; j++){
    			// les 2 marques pour 2 noeuds d'une arête du bloc
    			uint m1 = filtre_vertex[vertices[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[i][j][0]]];
    			uint m2 = filtre_vertex[vertices[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[i][j][1]]];
#ifdef _DEBUG_SNAP2
    			std::cout<<"v1 = "<<vertices[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[i][j][0]]->getName()<<", m1 = "<<m1<<std::endl;
    			std::cout<<"v2 = "<<vertices[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[i][j][1]]->getName()<<", m2 = "<<m2<<std::endl;
#endif

    			if (m1 == 0){
    				if (m2 != 0)
    					// si un des 2 sommets n'est pas dans une des listes, alors pas la bonne direction
    					dir_ok = false;
    			} else {
    				if (m2 == 0)
    					// si un des 2 sommets n'est pas dans une des listes, alors pas la bonne direction
    					dir_ok = false;
    				else if (m1 == m2)
    					// s'ils sont dans la même liste, ce n'est pas la bonne direction
    					dir_ok = false;

    				// sinon, ok pour ce couple ...
    			}
    		} // end for j<4
    		if (dir_ok)
    			dir_bl = (Block::eDirOnBlock)i;
    	} // end for i<3

#ifdef _DEBUG_SNAP
    	std::cout<<"dir_bl = "<<dir_bl<<std::endl;
#endif

    	if (dir_bl == Block::unknown){
    		// on va changer la méthode de maillage en non structurée

    		if (vertices1.size() == 1 && vertices2.size() == 1){
    			m_vertices1.push_back(vertices1[0]);
    			m_vertices2.push_back(vertices2[0]);
    		}
    		else {
    			throw TkUtil::Exception (TkUtil::UTF8String ("La fusion entre sommets ne peut se faire, on ne trouve pas de direction dans le bloc concerné", TkUtil::Charset::UTF_8));
    		}

    	} // if (dir_bl == Block::unknown)
    	else {
    		// on réordonne les sommets par couples
    		for (std::vector<Topo::Vertex* >::iterator iter1 = vertices1.begin();
    				iter1 != vertices1.end(); ++iter1){
    			Vertex* som1 = *iter1;
    			Vertex* som2 = 0;
    			for (uint j=0; j<4; j++){
    				if (som1 == vertices[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[dir_bl][j][0]])
    					som2 = vertices[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[dir_bl][j][1]];
    				else if (som1 == vertices[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[dir_bl][j][1]])
    					som2 = vertices[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[dir_bl][j][0]];
    			}
    			if (som2){
    				m_vertices1.push_back(som1);
    				m_vertices2.push_back(som2);
#ifdef _DEBUG_SNAP
    				std::cout<<" fusion de "<<som1->getName()<<" avec "<<som2->getName()<<std::endl;
#endif
    			}
    			else
    				throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandSnapVertices, on ne trouve pas le sommet opposé", TkUtil::Charset::UTF_8));
    		}
    	} //  else / if (dir_bl == Block::unknown)
    } // end if (m_common_block->isStructured())
    else {
    	// pour le cas où il serait passé 2 arêtes avec un sommet commun (celui de la dégénérescence)
    	// on continue comme si il n'y avait eu qu'un sommet par liste
    	if (2 == vertices1.size() &&
    			(3 == filtre_vertex[vertices1[0]] || 3 == filtre_vertex[vertices1[1]])){
    		for (std::vector<Topo::Vertex* >::iterator iter1 = vertices1.begin();
    				iter1 != vertices1.end(); ++iter1)
    			if (3 != filtre_vertex[*iter1])
    				m_vertices1.push_back(*iter1);
    		for (std::vector<Topo::Vertex* >::iterator iter1 = vertices2.begin();
    				iter1 != vertices2.end(); ++iter1)
    			if (3 != filtre_vertex[*iter1])
    				m_vertices2.push_back(*iter1);

    	}
    	if (1 != m_vertices1.size() && 1 != m_vertices2.size()){
    		if (vertices1.size() != 1)
    			throw TkUtil::Exception (TkUtil::UTF8String ("La fusion entre sommets ne peut se faire, dans le cas d'un bloc non structuré, on ne peut le faire que par couple de sommets", TkUtil::Charset::UTF_8));
    		m_vertices1.push_back(vertices1[0]);
    		m_vertices2.push_back(vertices2[0]);
    	}

    } // end else / if (m_common_block->isStructured())

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Fusion de sommets";
	for (uint i=0; i<m_vertices1.size() && i<5; i++)
		comments << " " << m_vertices1[i]->getName();
	if (m_vertices1.size()>5)
		comments << " ... ";
	comments << " avec";
	for (uint i=0; i<m_vertices2.size() && i<5; i++)
		comments << " " << m_vertices2[i]->getName();
	if (m_vertices2.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSnapVertices::
~CommandSnapVertices()
{
}
/*----------------------------------------------------------------------------*/
void CommandSnapVertices::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSnapVertices::execute pour la commande " << getName ( )
                    << " de nom unique " << getUniqueName ( );


    // liste des blocs modifiés par le déplacement des sommets
    std::list<Topo::Block*> l_b;
    // liste des cofaces modifiées ...
    std::list<Topo::CoFace*> l_f;

    std::vector<Topo::Vertex* >::iterator iter1;
    std::vector<Topo::Vertex* >::iterator iter2;
    iter1 = m_vertices1.begin();
    iter2 = m_vertices2.begin();
    for (; iter1 != m_vertices1.end(); ++iter1, ++iter2){
        Vertex* som1 = *iter1;
        Vertex* som2 = *iter2;

        std::vector<Block* > blocks;
        std::vector<CoFace* > cofaces;

        // le som2 bouge, il est fusionné avec le som1
        som2->getBlocks(blocks);
        l_b.insert(l_b.end(), blocks.begin(), blocks.end());

        som2->getCoFaces(cofaces);
        l_f.insert(l_f.end(), cofaces.begin(), cofaces.end());

        // cas où le som1 bouge
        if (!m_project_on_first){
            som1->getBlocks(blocks);
            l_b.insert(l_b.end(), blocks.begin(), blocks.end());

            som1->getCoFaces(cofaces);
            l_f.insert(l_f.end(), cofaces.begin(), cofaces.end());
        }
    }

    l_b.sort(Utils::Entity::compareEntity);
    l_b.unique();
    l_f.sort(Utils::Entity::compareEntity);
    l_f.unique();


    std::vector<CoEdge* > coedges;
    if (m_common_block)
    	m_common_block->getCoEdges(coedges);
    else if (m_common_coface)
    	m_common_coface->getCoEdges(coedges);

    // on marque les arêtes dans le bloc
    std::map<CoEdge*, uint> filtre_coedge;
    for (std::vector<CoEdge* >::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter)
        filtre_coedge[*iter] = 1;

    // liste des cofaces concernées par la fusion
    std::list<CoFace*> cofaces_degenerated;

    // liste des blocs concernées par la fusion
    std::list<Block*> blocks_degenerated;

    // liste des arêtes qui dégénèrent en un sommet
    std::vector<CoEdge*> coedges_degenerated;

    // Remplissage de ces 3 listes: *_degenerated

    iter1 = m_vertices1.begin();
    iter2 = m_vertices2.begin();
    for (; iter1 != m_vertices1.end(); ++iter1, ++iter2){
        Vertex* som1 = *iter1;
        Vertex* som2 = *iter2;

        // recherche d'une arête entre 2 sommets fusionnés
        CoEdge* coedge_between = 0;
        std::vector<CoEdge* > loc_coedges;
        som1->getCoEdges(loc_coedges);
        for (std::vector<CoEdge* >::iterator iter = loc_coedges.begin();
                iter != loc_coedges.end(); ++iter)
            if (filtre_coedge[*iter] == 1 && som2 == (*iter)->getOppositeVertex(som1))
                coedge_between = *iter;

        // on ne trouve pas d'arête, peut-être parce qu'elle est composée
        if(0 == coedge_between)
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandSnapVertices, il n'est pas prévu pour le moment le cas avec plusieurs arêtes communes entre les sommets fusionnés", TkUtil::Charset::UTF_8));
#ifdef _DEBUG_SNAP
        std::cout<<"coedge_between "<<coedge_between->getName()<<std::endl;
#endif

        // on met de côtés les faces communes touchées
        // ainsi que les blocs touchés
        std::vector<CoFace* > loc_cofaces;
        coedge_between->getCoFaces(loc_cofaces);
        for (std::vector<CoFace* >::iterator iter2 = loc_cofaces.begin();
                iter2 != loc_cofaces.end(); ++iter2){
            cofaces_degenerated.push_back(*iter2);
            std::vector<Block* > loc_blocs;
            (*iter2)->getBlocks(loc_blocs);
            blocks_degenerated.insert(blocks_degenerated.end(), loc_blocs.begin(), loc_blocs.end());
        }

        coedges_degenerated.push_back(coedge_between);
    } // end for iter1 = m_vertices1.begin()

    cofaces_degenerated.sort(Utils::Entity::compareEntity);
    cofaces_degenerated.unique();

    blocks_degenerated.sort(Utils::Entity::compareEntity);
    blocks_degenerated.unique();

    // pour chaque bloc structuré non dégénéré, recherche de la face avec le max de sommets

    // filtre pour marquer les sommets en fonction de leur appartenance aux 2 listes
    std::map<Topo::Vertex*, uint> filtre_vertex;
    for (std::vector<Topo::Vertex* >::iterator iter1 = m_vertices1.begin();
            iter1 != m_vertices1.end(); ++iter1)
        filtre_vertex[*iter1] = 1;
    for (std::vector<Topo::Vertex* >::iterator iter1 = m_vertices2.begin();
            iter1 != m_vertices2.end(); ++iter1)
        filtre_vertex[*iter1] = 2;
    // nombre de sommets de m_vertices1 qui sont dans le bloc
    std::map<Topo::Block*, uint> filtre_block;
    // nombre de sommets de m_vertices1 qui sont dans la face commune
    std::map<Topo::CoFace*, uint> filtre_coface;

    for (std::list<Block*>::iterator iter = blocks_degenerated.begin();
            iter != blocks_degenerated.end(); ++iter){
        Block* bloc = *iter;

        // remplissage de filtre_block
        for (uint i=0; i<bloc->getNbVertices(); i++)
            if (filtre_vertex[bloc->getVertex(i)] == 1)
                filtre_block[bloc] += 1;

        // remplissage de filtre_coface
        std::vector<CoFace* > loc_cofaces;
        bloc->getCoFaces(loc_cofaces);
        for (std::vector<CoFace* >::iterator iter2 = loc_cofaces.begin();
                        iter2 != loc_cofaces.end(); ++iter2){
            CoFace* coface = *iter2;
            for (uint i=0; i<coface->getNbVertices(); i++)
                if (filtre_vertex[coface->getVertex(i)] == 1)
                    filtre_coface[coface] += 1;
        }

        // réorientation du bloc
        if (bloc->isStructured() && bloc->getNbFaces() == 6){

            // recherche d'une Face
            Face* face = 0;
            uint nb_vtx_max = 0;

            for (uint i=0; i<bloc->getNbFaces(); i++){
                uint nb_vtx = 0;
                Face* loc_face = bloc->getFace(i);
                for (uint j=0; j<loc_face->getNbVertices(); j++)
                    if (filtre_vertex[loc_face->getVertex(j)] != 0)
                        nb_vtx+=1;
                if (nb_vtx>nb_vtx_max){
#ifdef _DEBUG_SNAP
                    std::cout<<"Face "<<loc_face->getName()<<" avec nb_vtx = "<<nb_vtx<<std::endl;
#endif
                    face = loc_face;
                    nb_vtx_max = nb_vtx;
                }
                else if (nb_vtx>0 && nb_vtx==nb_vtx_max
                        && face->getNbVertices()>loc_face->getNbVertices()){
                    // on privilégie la face la plus dégénérée
                    face = loc_face;
                }
            } // end for i

            if (face){
#ifdef _DEBUG_SNAP
                std::cout<<" permuteToKmaxFace pour bloc "<<bloc->getName()
                            <<" et pour Face "<<face->getName()<<std::endl;
#endif
                bloc->saveBlockTopoProperty(&getInfoCommand());
                bloc->permuteToKmaxFace(bloc->getIndex(face), &getInfoCommand());
            }
        } // end if bloc->isStructured() && getNbFaces() == 6

        if (!bloc->isStructured() && bloc->getNbVertices() == 7 && filtre_block[bloc] == 1){
        	// si c'est l'un des 4 premiers sommets qui est pris dans la fusion,
        	// alors on symétrise ce bloc / arête opposée au 7ème sommet
        	bool symNeeded = false;
        	for (uint i=0;i<4; i++)
        		if (1 == filtre_vertex[bloc->getVertex(i)])
        			symNeeded = true;
        	if (symNeeded) {
#ifdef _DEBUG_SNAP
        		std::cout<<"   il apparait nécessaire de faire une symétrie du bloc "<<bloc->getName()<<std::endl;
#endif
        		bloc->reverseOrder7Vertices();
        	}
        } // end if (!bloc->isStructured() && bloc->getNbVertices() == 7 && filtre_block[bloc] == 1)

    } // end for iter = blocks_degenerated.begin()

    // un filtre sur les arêtes pour savoir si elles étaient
    // initialement reliées à vertices1 ou vertices2

    std::map<Topo::Edge*, uint> filtre_edge;

    // fusion des sommets et des arêtes entre chaques couples
    iter1 = m_vertices1.begin();
    iter2 = m_vertices2.begin();
    std::vector<CoEdge*>::iterator iter3 = coedges_degenerated.begin();
    for (; iter1 != m_vertices1.end(); ++iter1, ++iter2, ++iter3){
        Vertex* som1 = *iter1;
        Vertex* som2 = *iter2;
        CoEdge* coedge_between = *iter3;

        // marque les arêtes
        std::vector<Topo::Edge* > edges;
        som1->getEdges(edges);
        for (std::vector<Topo::Edge* >::iterator iter4 = edges.begin();
                    iter4 != edges.end(); ++iter4)
            filtre_edge[*iter4] = 1;
        som2->getEdges(edges);
        for (std::vector<Topo::Edge* >::iterator iter4 = edges.begin();
                    iter4 != edges.end(); ++iter4)
            filtre_edge[*iter4] = 2;

#ifdef _DEBUG_SNAP
        std::cout<<" merge de "<<som1->getName()<<" avec "<<som2->getName()
                            <<" et libération de l'arête "<<coedge_between->getName()<<std::endl;
#endif

        // faut-il supprimer les associations pour som1 ?
        bool needDeleteAssociation = !areAssociationOK(som1, som2);

        // Suppression des associations vers la géométrie pour les sommets m_vertices2
        // ainsi que pour tout ce qui touche ces sommets
        if (needDeleteAssociation || !m_project_on_first)
        TopoHelper::deleteAllAdjacentTopoEntitiesGeomAssociation(som2);

        // si le som1 bouge, on supprime égallement les associations
        if (!m_project_on_first)
            TopoHelper::deleteAllAdjacentTopoEntitiesGeomAssociation(som1);

        // la fusion des 2 sommets
        som1->merge(som2, &getInfoCommand());

        // libération de l'arête
        coedge_between->free(&getInfoCommand());

    } // end for iter1, iter2, iter3


    // libération des faces et faces communes

    for (std::list<CoFace*>::iterator iter = cofaces_degenerated.begin();
            iter != cofaces_degenerated.end(); ++iter){
        CoFace* coface = *iter;
        if (coface->getNbVertices() == 1){
#ifdef _DEBUG_SNAP
            std::cout<<" cofaces dégénérée avec 1 seul sommet "<<coface->getName()<<std::endl;
#endif
            coface->free(&getInfoCommand());
            MGX_NOT_YET_IMPLEMENTED("CommandSnapVertices avec face commune dégénérée en 1 seul sommet")

        } else if (coface->getNbVertices() == 2){
#ifdef _DEBUG_SNAP
            std::cout<<" cofaces dégénérée avec 2 sommets "<<coface->getName()<<std::endl;
#endif
            Edge* a1 = coface->getEdge(0);
            Edge* a3 = coface->getEdge(1);
#ifdef _DEBUG_SNAP
            std::cout<<" merge entre les arêtes "<<a1->getName()<<" et "
                    <<a3->getName()<<std::endl;
#endif
            if (a1->getNbCoEdges() > 1 || a3->getNbCoEdges() > 1)
                throw TkUtil::Exception (TkUtil::UTF8String ("CommandSnapVertices, merge d'une arête composée non prévu", TkUtil::Charset::UTF_8));

            // la fusion se fait sur les arêtes internes
            if (filtre_edge[a1] == 1 && filtre_edge[a3] == 2)
                a1->getCoEdge(0)->merge(a3->getCoEdge(0), &getInfoCommand());
            else if (filtre_edge[a1] == 2 && filtre_edge[a3] == 1)
                a3->getCoEdge(0)->merge(a1->getCoEdge(0), &getInfoCommand());
            else
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandSnapVertices, le filtre_edge ne nous apprend rien", TkUtil::Charset::UTF_8));

            // il faut supprimer les Faces des blocs
            std::vector<Face* > loc_faces;
            coface->getFaces(loc_faces);
            for (std::vector<Face* >::iterator iter2 = loc_faces.begin();
                    iter2 != loc_faces.end(); ++iter2){
                Face* face = *iter2;
#ifdef _DEBUG_SNAP
                std::cout<<"  face impactée "<<face->getName()<<std::endl;
#endif
                if (face->getNbBlocks()!=1)
                    throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, CommandSnapVertices, une Face n'a pas un unique Block", TkUtil::Charset::UTF_8));

                Block* bloc = face->getBlock(0);
                bloc->saveBlockTopoProperty(&getInfoCommand());
                bloc->getBlockTopoProperty()->getFaceContainer().remove(face, true);
                face->free(&getInfoCommand());
            }

            coface->free(&getInfoCommand());
        }
    } // end for iter = cofaces_degenerated.begin()

    // on supprime les sommets des blocs, et les blocs si nécessaire
    for (std::list<Block*>::iterator iter = blocks_degenerated.begin();
            iter != blocks_degenerated.end(); ++iter){
        Block* bloc = *iter;
        if (bloc->getNbFaces() == 2){
#ifdef _DEBUG_SNAP
            std::cout<<"  bloc "<<bloc->getName()
                     <<" avec plus que 2 faces"<<std::endl;
#endif
            // suppression des cofaces (fusion d'une avec l'autre)
            if (bloc->getFace(0)->getNbCoFaces() != 1 || bloc->getFace(1)->getNbCoFaces() != 1)
                throw TkUtil::Exception (TkUtil::UTF8String ("CommandSnapVertices, cas avec plusieurs faces communes non prévu pour le moment", TkUtil::Charset::UTF_8));
            else {
                CoFace* cf1 = bloc->getFace(0)->getCoFace(0);
                CoFace* cf2 = bloc->getFace(1)->getCoFace(0);

                if (filtre_coface[cf1] > filtre_coface[cf2])
                    cf1->merge(cf2, &getInfoCommand());
                else
                    cf2->merge(cf1, &getInfoCommand());
            }

            // suppression des faces
            std::vector<Face* > loc_faces;
            bloc->getFaces(loc_faces);
            for (std::vector<Face* >::iterator iter2 = loc_faces.begin();
                    iter2 != loc_faces.end(); ++iter2)
                (*iter2)->free(&getInfoCommand());

            // suppression du bloc
            bloc->free(&getInfoCommand());
        }
        else if (bloc->isStructured() && bloc->getNbVertices() == 8 && filtre_block[bloc] == 2){
#ifdef _DEBUG_SNAP
            std::cout<<"  bloc "<<bloc->getName()
                     <<" avec dégénérescence en prisme [cas structuré]"<<std::endl;
#endif
            // cas similaire à Block::degenerateFaceInEdge
            if (bloc->getVertex(4) == bloc->getVertex(5))
                bloc->getBlockTopoProperty()->getVertexContainer().set(5, bloc->getVertex(6));
            bloc->getBlockTopoProperty()->getVertexContainer().resize(6);
        }
        else if (!bloc->isStructured() && bloc->getNbVertices() == 7 && filtre_block[bloc] == 1){
#ifdef _DEBUG_SNAP
            std::cout<<"  bloc "<<bloc->getName()
                     <<" avec dégénérescence en prisme [cas non structuré]"<<std::endl;
            std::cout<<*bloc<<std::endl;
#endif
            if (bloc->getVertex(4) == bloc->getVertex(5))
                bloc->getBlockTopoProperty()->getVertexContainer().set(5, bloc->getVertex(6));
            bloc->getBlockTopoProperty()->getVertexContainer().resize(6);

            // on le déclare structuré
            BlockMeshingProperty* prop = new BlockMeshingPropertyDirectional(BlockMeshingProperty::dir_k);
            bloc->switchBlockMeshingProperty(&getInfoCommand(), prop);
            delete prop;
        }
        else {
#ifdef _DEBUG_SNAP
            std::cout<<"  bloc "<<bloc->getName()
                     <<" avec dégénérescence quelconque"<<std::endl;
#endif
            // on se contente de le déclarer non-structuré, si ce n'est pas le cas
            if (bloc->isStructured()){
                BlockMeshingProperty* prop = new BlockMeshingPropertyDelaunayTetgen();
                bloc->switchBlockMeshingProperty(&getInfoCommand(), prop);
                delete prop;
            }

            // on supprime les doublons de la liste de sommets
            bloc->saveBlockTopoProperty(&getInfoCommand());

            std::vector<Vertex*> loc_vtx;
            for (uint i=0;i<bloc->getNbVertices(); i++){
            	Vertex* vtx = bloc->getVertex(i);
#ifdef _DEBUG_SNAP
            	std::cout<<"    "<<vtx->getName()<<" , filtre_vertex: "<<filtre_vertex[vtx]<<std::endl;
#endif

            	if (4 != filtre_vertex[vtx])
            		loc_vtx.push_back(vtx);
            	if (1 == filtre_vertex[vtx])
            		// pour ne prendre qu'une fois le sommet
            		filtre_vertex[vtx] = 4;
            }
            bloc->getBlockTopoProperty()->getVertexContainer().clear();
            bloc->getBlockTopoProperty()->getVertexContainer().add(loc_vtx);


            // on remet le filtre à l'état d'avant pour le prochain bloc
            for (uint i=0;i<loc_vtx.size(); i++)
            	if (4 == filtre_vertex[loc_vtx[i]])
            		filtre_vertex[loc_vtx[i]] = 1;
        }

    } // end for iter = blocks_degenerated.begin()

    if (!m_project_on_first){
        // recentre le sommet qui sera conservé
        // cette sauvegarde doit se faire après la fusion pour éviter
        // que les sauvegardes de topologies soient déjà positionnées à DISPMODIFIED
        iter1 = m_vertices1.begin();
        iter2 = m_vertices2.begin();
        for (; iter1 != m_vertices1.end(); ++iter1, ++iter2){
            Vertex* som1 = *iter1;
            Vertex* som2 = *iter2;

            som1->saveVertexGeomProperty(&getInfoCommand(), true);
            Utils::Math::Point pt = (som1->getCoord() + som2->getCoord())/2;
            som1->setCoord(pt);

        } // end for iter1
    } //end if (!m_project_on_first)

    // recherche la méthode la plus simple possible pour les cofaces et les blocs modifiés
    updateMeshLaw(l_f, l_b);

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
bool CommandSnapVertices::areAssociationOK(Vertex* som1, Vertex* som2)
{
	Geom::GeomEntity* assos1 = som1->getGeomAssociation();
	Geom::GeomEntity* assos2 = som2->getGeomAssociation();

	if (assos2 == 0) // on va conserver les associations du 1er
		return true;
	if (assos1 == assos2) // on conserve puisque identiques
		return true;
	if (assos1->getDim() == 0 && assos2->getDim() == 1){
		// on va chercher à vérifier que la courbe est reliée au sommet, si oui alors on conserve
		Geom::Vertex* vt1 = dynamic_cast<Geom::Vertex*>(assos1);

		Geom::Curve* crb2 = dynamic_cast<Geom::Curve*>(assos2);

		if (crb2->firstPoint() == vt1 || crb2->secondPoint() == vt1)
			return true;
	}
	return false;
}
/*----------------------------------------------------------------------------*/
void CommandSnapVertices::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewCoedges(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
