/*----------------------------------------------------------------------------*/
/*
 * \file CommandFuse2FaceList.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/12/14
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Topo/TopoHelper.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <Topo/CommandFuse2FaceList.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_GLUE
/*----------------------------------------------------------------------------*/
CommandFuse2FaceList::
CommandFuse2FaceList(Internal::Context& c,
		std::vector<Topo::CoFace*> &cofaces1,
		std::vector<Topo::CoFace*> &cofaces2)
:CommandEditTopo(c, "Collage entre 2 listes de faces")
, m_cofaces1(cofaces1.begin(), cofaces1.end())
, m_cofaces2(cofaces2.begin(), cofaces2.end())
{
	if (cofaces1.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Le collage entre 2 listes de faces doit se faire avec une liste non vide", TkUtil::Charset::UTF_8));
	if (cofaces1.size() != cofaces2.size())
		throw TkUtil::Exception (TkUtil::UTF8String ("Le collage entre 2 listes de faces doit se faire avec des listes de même taille", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandFuse2FaceList::
~CommandFuse2FaceList()
{
}
/*----------------------------------------------------------------------------*/
void CommandFuse2FaceList::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandGlue2FaceList::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // filtre sur les sommets topologiques de ces faces à coller
    // à 1 pour les sommets uniquement dans cofaces1
    // à 2 pour les sommets uniquement dans cofaces2
    // à 3 pour les sommets communs à cofaces1 et cofaces2
    std::map<Vertex*,uint> filtre_vtx;

    for (std::vector<CoFace*>::iterator iter1 = m_cofaces1.begin();
    		iter1 != m_cofaces1.end(); ++iter1){
    	std::vector<Vertex*> vertices;
    	(*iter1)->getAllVertices(vertices);
    	for (std::vector<Vertex*>::iterator iter2 = vertices.begin();
    			iter2 != vertices.end(); ++iter2){
    		filtre_vtx[*iter2] = 1;
    	} // end for iter2 = vertices.begin()
    } // end for iter1 = cofaces1.begin()

    for (std::vector<CoFace*>::iterator iter1 = m_cofaces2.begin();
    		iter1 != m_cofaces2.end(); ++iter1){
    	std::vector<Vertex*> vertices;
    	(*iter1)->getAllVertices(vertices);
    	for (std::vector<Vertex*>::iterator iter2 = vertices.begin();
    			iter2 != vertices.end(); ++iter2){
    		if (1 == filtre_vtx[*iter2])
    			filtre_vtx[*iter2] = 3;
    		else if (0 == filtre_vtx[*iter2])
    			filtre_vtx[*iter2] = 2;
    	} // end for iter2 = vertices.begin()
    } // end for iter1 = cofaces1.begin()

    // constitution des 2 listes de sommets dont on cherche à trouver une relation de proximité
    std::vector<Vertex*> vertices1;
    std::vector<Vertex*> vertices2;
    for (std::map<Vertex*,uint>::iterator iter = filtre_vtx.begin();
    		iter != filtre_vtx.end(); ++iter){
    	 if (1 == iter->second)
    		 vertices1.push_back(iter->first);
    	 else if (2 == iter->second)
    		 vertices2.push_back(iter->first);
    } // end for iter = filtre_vtx.begin()

    // constitution de la table de corespondance entre sommets de vertices1 vers vertices2
    // on met à 0 dans le filtre les sommets de vertices1 qui sont sans correspondance
    std::map<Vertex*, Vertex*> vtx1vers2;
    Vertex::findNearlyVertices(vertices1, vertices2, vtx1vers2);

#ifdef _DEBUG_GLUE
    std::cout<<"vtx1vers2: "<<std::endl;
    for (std::map<Vertex*, Vertex*>::iterator iter = vtx1vers2.begin();
    		iter!=vtx1vers2.end(); ++iter)
    	std::cout<<"  "<<iter->first->getName()<<" -> "<<iter->second->getName()<<std::endl;
#endif

    // il faut vérifier que chacun des noeuds a un et un seul noeud en correspondance
    // on utilise le filtre et on met à 4 les sommets de vertices2 vus au travers de vtx1vers2
	for (std::vector<Vertex*>::iterator iter1 = vertices1.begin();
			iter1 != vertices1.end(); ++iter1){
		Vertex* vtx1 = *iter1;
		if (3 != filtre_vtx[vtx1]){

			Vertex* vtx2 = vtx1vers2[vtx1];
			if (0 == vtx2){
				TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
				err << "Le collage ne peut se faire, on ne trouve pas de sommet en correspondance pour "
						<<vtx1->getName();
				throw TkUtil::Exception(err);
			}

			if (2 == filtre_vtx[vtx2]){
				filtre_vtx[vtx2] = 4;
			} else if (4 == filtre_vtx[vtx2]){
				TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
				err << "Le collage ne peut se faire, on trouve 2 sommets en correspondance pour "
						<<vtx1->getName()<<", "<<vtx2->getName()<<" et un autre";
				throw TkUtil::Exception(err);
			} else {
				TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
				err << "Erreur interne pour le collage du sommet "<<vtx1->getName();
				throw TkUtil::Exception(err);
			}
		} // end if (3 != filtre_vtx[vtx1])
	} // end for iter1 = vertices1.begin()


    // pour toutes les cofaces1, si ses sommets ont
    // soit un sommet déjà commun
    // soit un sommet en correspondance
    // recherche la coface parmi coface2 dont tous les noeuds correspondent
    // les noeuds fusionnés sont mis à 3 dans le filtre
    for (std::vector<CoFace*>::iterator iter1 = m_cofaces1.begin();
    		iter1 != m_cofaces1.end(); ++iter1){
    	CoFace* coface1 = *iter1;
    	std::vector<Vertex*> verticesA;
    	std::vector<Vertex*> verticesB;
    	coface1->getAllVertices(verticesA);
#ifdef _DEBUG_GLUE
    	std::cout<<"on observe la face commune "<<coface1->getName();
    	for (uint i=0; i<verticesA.size(); i++)
    		std::cout<<" "<<verticesA[i]->getName();
    	std::cout<<std::endl;
#endif
    	for (std::vector<Vertex*>::iterator iter2 = verticesA.begin();
    			iter2 != verticesA.end(); ++iter2){
    		if (3 == filtre_vtx[*iter2])
    			// cas d'un sommet commun
    			verticesB.push_back(*iter2);
    		else if (1 == filtre_vtx[*iter2]){
    			Vertex* vtx = vtx1vers2[*iter2];
    			if (0 == vtx){
					TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
    				err << "Erreur interne dans CommandGlue2Topo, on ne trouve pas de sommet en corespondance avec le sommet "
    						<<(*iter2)->getName();
    				throw TkUtil::Exception(err);
    			}
    			verticesB.push_back(vtx);
    		}
			else {
				TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
				err << "Erreur interne pour la valeur du filtre de "<<(*iter2)->getName()<<" : "<<(short)filtre_vtx[*iter2];
				throw TkUtil::Exception(err);
			}
    	} // end for iter2 = verticesA.begin()

    	if (verticesA.size() != verticesB.size()){
			TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
    		err << "Erreur interne dans CommandGlue2Topo, on n'a pas le même nombre de sommets pour "
    				<<coface1->getName()
    				<<" "<<verticesA.size()<<" sommets pour coface1 et "
    				<<verticesB.size()<<" pour coface2";
    		throw TkUtil::Exception(err);
    	}

    		// recherche de la coface en face
    		CoFace* coface2 = TopoHelper::getCoface(verticesB, m_cofaces2);
    		if (coface2){
#ifdef _DEBUG_GLUE
    			std::cout<<"collage entre "<<coface1->getName()<<" et "<<coface2->getName()<<std::endl;
    			//    		std::cout<<*coface1;
    			//    		std::cout<<*coface2;
#endif
    			if (coface1 == coface2){
					TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
    				err << "Erreur interne dans CommandGlue2Topo, on ne peut pas coller 2 fois la même face "
    						<<coface1->getName();
    				throw TkUtil::Exception(err);
    			}
    			coface1->fuse(coface2, &getInfoCommand());

    			// les sommets sont désormais communs
    			for (std::vector<Vertex*>::iterator iter2 = verticesA.begin();
    					iter2 != verticesA.end(); ++iter2){
    				filtre_vtx[*iter2] = 3;
    			}
    		} // end if (coface2)
    		else {
				TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
    			err << "Le collage de la face commune "
    					<<coface1->getName()<<" ne peut se faire, on ne trouve pas de face en correspondance";
    			throw TkUtil::Exception(err);
    		}
    } // end for iter1 = cofaces1.begin()

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandFuse2FaceList::
postExecute(bool hasError)
{
    // remet de l'odre dans la mémoire (libération des parties internes)
    if (hasError)
        cancelInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandFuse2FaceList::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
