/*----------------------------------------------------------------------------*/
/*
 * \file CommandGlue2Topo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2/3/2014
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandGlue2Topo.h"
#include "Topo/TopoHelper.h"
#include "Geom/Volume.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <map>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_GLUE
/*----------------------------------------------------------------------------*/
CommandGlue2Topo::
CommandGlue2Topo(Internal::Context& c, Geom::Volume* vol_A, Geom::Volume* vol_B)
:CommandEditTopo(c, std::string("Collage entre les 2 topologies des volumes ")
                    + vol_A->getName() + " et " + vol_B->getName())
, m_vol_A(vol_A)
, m_vol_B(vol_B)
{
	if (0 == m_vol_A)
		throw TkUtil::Exception (TkUtil::UTF8String ("Le collage entre 2 topologies doit avoir un premier volume identifié", TkUtil::Charset::UTF_8));
	if (0 == m_vol_B)
		throw TkUtil::Exception (TkUtil::UTF8String ("Le collage entre 2 topologies doit avoir un deuxième volume identifié", TkUtil::Charset::UTF_8));

	// on n'accepte que 2 volume disctincts pour le moment
	if (m_vol_A == m_vol_B)
		throw TkUtil::Exception (TkUtil::UTF8String ("Le collage entre 2 topologies doit se faire avec 2 volumes distincts", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandGlue2Topo::
~CommandGlue2Topo()
{
}
/*----------------------------------------------------------------------------*/
void CommandGlue2Topo::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandGlue2Topo::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // les cofaces au bord associées au premier volume
    std::vector<CoFace*> cofaces1;
    std::set<Topo::CoFace*> cofaces;
    std::vector<Topo::Block*> blocs;
    m_vol_A->get(blocs);
    Topo::TopoHelper::get(blocs, cofaces);

    for (std::set<Topo::CoFace*>::iterator iter = cofaces.begin();
    		iter != cofaces.end(); ++iter)
    	if ((*iter)->getNbBlocks() == 1)
    		cofaces1.push_back(*iter);
#ifdef _DEBUG_GLUE
    std::cout<<"cofaces1: ";
    for (std::vector<CoFace*>::iterator iter = cofaces1.begin();
    		iter!=cofaces1.end(); ++iter){
    	std::cout<<(*iter)->getName()<<" ";
    }
    std::cout<<std::endl;
#endif

    // les cofaces au bord associées au deuxième volume
    std::vector<CoFace*> cofaces2;
    cofaces.clear();
    blocs.clear();
    m_vol_B->get(blocs);
    Topo::TopoHelper::get(blocs, cofaces);

    for (std::set<Topo::CoFace*>::iterator iter = cofaces.begin();
    		iter != cofaces.end(); ++iter)
    	if ((*iter)->getNbBlocks() == 1)
    		cofaces2.push_back(*iter);
#ifdef _DEBUG_GLUE
    std::cout<<"cofaces2: ";
    for (std::vector<CoFace*>::iterator iter = cofaces2.begin();
    		iter!=cofaces2.end(); ++iter){
    	std::cout<<(*iter)->getName()<<" ";
    }
    std::cout<<std::endl;
#endif


    // filtre sur les sommets topologiques de ces faces au bord
    // à 1 pour les sommets uniquement dans cofaces1
    // à 2 pour les sommets uniquement dans cofaces2
    // à 3 pour les sommets communs à cofaces1 et cofaces2
    std::map<Vertex*,uint> filtre_vtx;

    for (std::vector<CoFace*>::iterator iter1 = cofaces1.begin();
    		iter1 != cofaces1.end(); ++iter1){
    	std::vector<Vertex*> vertices;
    	(*iter1)->getAllVertices(vertices);
    	for (std::vector<Vertex*>::iterator iter2 = vertices.begin();
    			iter2 != vertices.end(); ++iter2){
    		filtre_vtx[*iter2] = 1;
    	} // end for iter2 = vertices.begin()
    } // end for iter1 = cofaces1.begin()

    for (std::vector<CoFace*>::iterator iter1 = cofaces2.begin();
    		iter1 != cofaces2.end(); ++iter1){
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
    // recherche bestiale
    for (std::vector<Vertex*>::iterator iter1 = vertices1.begin();
    		iter1 != vertices1.end(); ++iter1){
    	bool trouve = false;
    	for (std::vector<Vertex*>::iterator iter2 = vertices2.begin();
        		iter2 != vertices2.end() && !trouve; ++iter2){

    		if ((*iter1)->getCoord().isEpsilonEqual((*iter2)->getCoord(), Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon)){
    			trouve = true;
    			vtx1vers2[*iter1] = *iter2;
    		}
    	} // end for iter2 = vertices2.begin()

    	if (!trouve)
    		filtre_vtx[*iter1] = 0;
    } // end for iter1 = vertices1.begin()

#ifdef _DEBUG_GLUE
    std::cout<<"vtx1vers2: "<<std::endl;
    for (std::map<Vertex*, Vertex*>::iterator iter = vtx1vers2.begin();
    		iter!=vtx1vers2.end(); ++iter)
    	std::cout<<"  "<<iter->first->getName()<<" -> "<<iter->second->getName()<<std::endl;
#endif

    // pour toutes les cofaces1, si ses sommets ont
    // soit un sommet déjà commun
    // soit un sommet en correspondance
    // recherche la coface parmi coface2 dont tous les noeuds correspondent
    // les noeuds fusionnés sont mis à 3 dans le filtre
    for (std::vector<CoFace*>::iterator iter1 = cofaces1.begin();
    		iter1 != cofaces1.end(); ++iter1){
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
    	bool unSommetSansCorrespondance = false;
    	for (std::vector<Vertex*>::iterator iter2 = verticesA.begin();
    			iter2 != verticesA.end() && !unSommetSansCorrespondance; ++iter2){
    		if (0 == filtre_vtx[*iter2]){
    			// cas d'un sommet sans correspondance, on passe à la coface suivante
#ifdef _DEBUG_GLUE
    			std::cout<<"  coface passée car un des sommets sans correspondance"<<std::endl;
#endif
    			unSommetSansCorrespondance = true;
    		}
    		else if (3 == filtre_vtx[*iter2])
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
    	} // end for iter2 = verticesA.begin()

    	if (!unSommetSansCorrespondance){
    		if (verticesA.size() != verticesB.size()){
				TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
    			err << "Erreur interne dans CommandGlue2Topo, on n'a pas le même nombre de sommets pour "
    					<<coface1->getName()
    					<<" "<<verticesA.size()<<" sommets pour coface1 et "
    					<<verticesB.size()<<" pour coface2";
    			throw TkUtil::Exception(err);
    		}

    		// recherche de la coface en face
    		CoFace* coface2 = TopoHelper::getCoface(verticesB, cofaces2);
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
#ifdef _DEBUG_GLUE
    			std::cout<<"  coface passée car pas de coface correspondante"<<std::endl;
#endif
    		}
    	} // end if (!unSommetSansCorrespondance)
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
void CommandGlue2Topo::
postExecute(bool hasError)
{
    // remet de l'odre dans la mémoire (libération des parties internes)
    if (hasError)
        cancelInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandGlue2Topo::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
