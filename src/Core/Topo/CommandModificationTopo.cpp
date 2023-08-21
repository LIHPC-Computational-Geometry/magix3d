/*----------------------------------------------------------------------------*/
/*
 * \file CommandModificationTopo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 21/3/2013
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandModificationTopo.h"

#include "Geom/GeomModificationBaseClass.h"
#include "Geom/CommandEditGeom.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"

#include "Utils/Common.h"

#include "Topo/TopoHelper.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/CoFace.h"
#include "Topo/Block.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/WarningLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <set>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MAJTOPO
/*----------------------------------------------------------------------------*/
CommandModificationTopo::
CommandModificationTopo(Internal::Context& c, Geom::CommandEditGeom* command_edit_geom)
:CommandEditTopo(c, "Modifications topologiques")
, m_command_edit_geom(command_edit_geom)
{
}
/*----------------------------------------------------------------------------*/
CommandModificationTopo::
~CommandModificationTopo()
{
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandModificationTopo::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );


    // On ne traite pas ici les entités déplacées.
    // On considère que la commande géométrique à fait ce qu'il faut pour la topologie
    // c'est à dire un appel à la commande de transformation topologique équivalente
    // à celle faite pour la géométrie

#ifdef _DEBUG_MAJTOPO
    std::cout<<"CommandModificationTopo::internalExecute() ..."<<std::endl;
#endif

    if (m_command_edit_geom == 0){
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
        err << "Erreur interne, CommandModificationTopo ne peut se faire, m_command_edit_geom est nul !";
        throw TkUtil::Exception(err);
    }

    MAJVertices();

    MAJCoEdges();

    MAJCoFaces();

    MAJBlocks();

    MAJInternalsCoFaces();

#ifdef _DEBUG_MAJTOPO
    std::cout<<"CommandModificationTopo::internalExecute() FIN"<<std::endl;
#endif

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::MAJVertices()
{
#ifdef _DEBUG_MAJTOPO
	std::cout<<"CommandModificationTopo::MAJVertices()"<<std::endl;
#endif
    // récupération de tous les sommets géométriques
    std::list<Geom::GeomEntity*>& geom_vtx = m_command_edit_geom->getRefEntities(0);

#ifdef _DEBUG_MAJTOPO_2
    // on afiche la liste des sommets détruits et leurs coordonnées
    for (std::list<Geom::GeomEntity*>::iterator iter = geom_vtx.begin();
                iter != geom_vtx.end(); ++iter){
        Geom::Vertex* vtx = dynamic_cast<Geom::Vertex*>(*iter);
        if (vtx)
            std::cout<<"  "<<vtx->getName()<<(vtx->isDestroyed()?" détruit":" conservé")<<", position "<<vtx->getCoord()<<std::endl;
    }
    // les nouveaux sommets
    std::vector<Geom::GeomEntity*>& new_entities = m_command_edit_geom->getNewEntities();
    for (std::vector<Geom::GeomEntity*>::iterator iter = new_entities.begin();
                iter != new_entities.end(); ++iter){
        Geom::Vertex* vtx = dynamic_cast<Geom::Vertex*>(*iter);
        if (vtx)
            std::cout<<"  "<<vtx->getName()<<" sommet nouveau, position "<<vtx->getCoord()<<std::endl;
    }
#endif

    for (std::list<Geom::GeomEntity*>::iterator iter = geom_vtx.begin();
            iter != geom_vtx.end(); ++iter){

#ifdef _DEBUG_MAJTOPO
        std::cout<<"  "<<(*iter)->getName()<<((*iter)->isDestroyed()?" [DETRUIT]":" conservé")<<std::endl;
#endif
        if ((*iter)->isDestroyed()){
            // recherche du sommet par lequel il est remplacé
            std::vector<Geom::GeomEntity*> newEntities = m_command_edit_geom->getReplacedEntities()[*iter];
#ifdef _DEBUG_MAJTOPO
            std::cout<<"   "<<(*iter)->getName()<<" est remplacée par "<<newEntities.size()<<" sommet[s]:";
            for (uint i=0; i<newEntities.size(); i++)
                std::cout<<" "<<newEntities[i]->getName();
            std::cout<<std::endl;
#endif

//            if (newEntities.size() != 1){
//				    TkUtil::UTF_8   err (Charset::UTF_8);
//                err << "Erreur interne, un sommet détruit n'est pas remplacé par une autre unique entité ("
//                    << (*iter)->getName()<<")";
//                throw TkUtil::Exception(err);
//            }

            // récupération du sommet topologique à détruire
            Topo::Vertex* vtx1 = getVertex(*iter, false);
            if (vtx1 == 0)
            	continue;

            if (newEntities.size() == 1){
                // récupération du sommet topologique conservé
                Topo::Vertex* vtx2 = getVertex(newEntities[0], false);

                if (vtx2 == 0){
                	getInfoCommand().addTopoInfoEntity(vtx1, Internal::InfoCommand::DISPMODIFIED);
                	vtx1->saveTopoProperty();
                	vtx1->setGeomAssociation(newEntities[0]);
                }
                else {
#ifdef _DEBUG_MAJTOPO
                	std::cout<<"Vertex::merge, "<<vtx1->getName()<<" est remplacé par "<<vtx2->getName()<<std::endl;
#endif
                	vtx2->merge(vtx1, &getInfoCommand());
                }
            }
            else {
            	getInfoCommand().addTopoInfoEntity(vtx1, Internal::InfoCommand::DISPMODIFIED);
                vtx1->saveTopoProperty();
                vtx1->setGeomAssociation(0);
            }

        } // end if (*iter)->isDestroyed()

    } // end for iter = geom_vtx.begin()
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::MAJCoEdges()
{
#ifdef _DEBUG_MAJTOPO
	std::cout<<"CommandModificationTopo::MAJCoEdges()"<<std::endl;
#endif
    // récupération de toutes les courbes géométriques
    std::list<Geom::GeomEntity*> geom_crv = m_command_edit_geom->getRefEntities(1);

    for (std::list<Geom::GeomEntity*>::iterator iter = geom_crv.begin();
            iter != geom_crv.end(); ++iter){

#ifdef _DEBUG_MAJTOPO
        std::cout<<"  "<<(*iter)->getName()<<((*iter)->isDestroyed()?" [DETRUITE]":" conservée")<<std::endl;
#endif
        if ((*iter)->isDestroyed()){
            // recherche de[s] la[es] courbe[s] par laquelle elle est remplacée
            std::vector<Geom::GeomEntity*> newEntities = m_command_edit_geom->getReplacedEntities()[*iter];

#ifdef _DEBUG_MAJTOPO
            std::cout<<"   "<<(*iter)->getName()<<" est remplacée par "<<newEntities.size()<<" courbe[s]:";
            for (uint i=0; i<newEntities.size(); i++)
                std::cout<<" "<<newEntities[i]->getName();
            std::cout<<std::endl;
#endif

            if (!newEntities.empty()) {

                // cas d'une courbe remplacée par plusieurs courbes

                // récupération des arêtes communes topologiques du côté de la courbe qui disparait
                std::vector<Topo::CoEdge*> coedges1 = getCoEdges(*iter);

                // tolérance en fonction de la longueur de la courbe
                double tol;
                // on évite de demander la longueur à l'entité détruite (car OCC retourne parfois 0)
                if (newEntities.size()>=1)
                	tol = newEntities[0]->getArea()*Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon;
                else
                	tol = Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon;

                // on traite à part le cas des courbes à un seul sommet (cercle complet)
                // cas où la courbe est remplacée par une autre
                if (newEntities.size() == 1){
                	std::vector<Geom::Vertex*> geom_vertices;
                	newEntities[0]->get(geom_vertices);
                	if (geom_vertices.size() == 1){

                		std::vector<Topo::Vertex*> all_vertices = getAllVertices(*iter);
                		Topo::Vertex* vtx0 = TopoHelper::getVertexAtPosition(all_vertices, geom_vertices[0]->getCoord(), tol);
#ifdef _DEBUG_MAJTOPO
                		std::cout<<"newEntities[0] : "<<newEntities[0]->getName()<<std::endl;
                		std::cout<<" vtx0 "<<(vtx0?vtx0->getName():"0")<<std::endl;
#endif
                		if (vtx0 != 0){

                			// remplace le sommet géométrique et fait la fusion des sommets topologiques si nécessaire
                			Topo::Vertex* vtx0new = getVertex(geom_vertices[0], false);
                			if (vtx0new){
#ifdef _DEBUG_MAJTOPO
                				std::cout<<"Vertex::merge, "<<vtx0->getName()<<" est remplacé par "<<vtx0new->getName()<<std::endl;
#endif
                				vtx0new->merge(vtx0, &getInfoCommand());
                				vtx0 = vtx0new;
                			}
                			else {
                				getInfoCommand().addTopoInfoEntity(vtx0, Internal::InfoCommand::DISPMODIFIED);
                				vtx0->saveTopoProperty();
                				vtx0->setGeomAssociation(geom_vertices[0]);
                			}
                		}

                		// récupération des arêtes communes topologiques du côté de la courbe à conserver
                		std::vector<Topo::CoEdge*> coedges2 = getCoEdges(newEntities[0]);

                		// on recolle les coedges uniquement pour le cas avec autant d'arêtes
                		if (coedges1.size() == coedges2.size()){
                			// Pour chacun des sommets des arêtes, on fait la fusion avec le sommet proche de l'autre groupe
                			std::vector<Topo::Vertex*> all_vertices2 = getAllVertices(newEntities[0]);

                			std::map<Vertex*, uint> filtre_vertices;

                			for (uint i=0; i<all_vertices.size(); i++){
                				Topo::Vertex* vtx0 = all_vertices[i];
                				Topo::Vertex* vtx0new = TopoHelper::getVertexAtPosition(all_vertices2, vtx0->getCoord(), tol);

                				if (vtx0new){
#ifdef _DEBUG_MAJTOPO
                					std::cout<<"Vertex::merge, "<<vtx0->getName()<<" est remplacé par "<<vtx0new->getName()<<std::endl;
#endif
                					// avant le merge des sommets, on cherche les arêtes reliées qui sont équivalentes (ayant 2 sommets égaux)
                					std::vector<Topo::CoEdge*> coedges_loc1, coedges_loc2;
                					vtx0->getCoEdges(coedges_loc1);
                					vtx0new->getCoEdges(coedges_loc2);

                					// fusion des sommets
                					vtx0new->merge(vtx0, &getInfoCommand());
                					filtre_vertices[vtx0new] = 2;

                					for (uint j=0; j<coedges_loc1.size(); j++){
                						Topo::CoEdge* co1 = coedges_loc1[j];
                						Topo::Vertex* vtx1 = co1->getOppositeVertex(vtx0new);

                						// recherche d'une arête dans coedges_loc2 qui possède également vtx1
                						Topo::CoEdge* co2 = 0;
                						for (uint k=0; k<coedges_loc2.size(); k++)
                							if (coedges_loc2[k]->find(vtx1))
                								co2 = coedges_loc2[k];

                						if (co2){
#ifdef _DEBUG_MAJTOPO
                							std::cout<<"CoEdge::merge, [autour du sommet] "<<co2->getName()<<" est remplacé par "<<co1->getName()<<std::endl;
                							std::cout<<*co2<<std::endl;
                							std::cout<<*co1<<std::endl;
#endif
                							co2->merge(co1, &getInfoCommand());
                						}
                					}
                				}
                			}

                		} // end if (coedges1.size() == coedges2.size())
                		else if (coedges1.size() != 0 && coedges2.size() == 0){
                			// on reporte les associations s'il y a pas de topo sur la courbe conservée

                			for (uint i=0; i<coedges1.size(); i++){
                				getInfoCommand().addTopoInfoEntity(coedges1[i], Internal::InfoCommand::DISPMODIFIED);
                				coedges1[i]->saveTopoProperty();
                				coedges1[i]->setGeomAssociation(newEntities[0]);
                			}
                			for (uint i=0; i<all_vertices.size(); i++){
                				getInfoCommand().addTopoInfoEntity(all_vertices[i], Internal::InfoCommand::DISPMODIFIED);
                				all_vertices[i]->saveTopoProperty();
                				all_vertices[i]->setGeomAssociation(newEntities[0]);
                			}

                		} // end else if (coedges1.size() != 0 && coedges2.size() == 0)

                	} // end if (geom_vertices.size() == 1)
                } // end if (newEntities.size() == 1)



                for (uint i=0; i<newEntities.size(); i++){
                    std::vector<Geom::Vertex*> geom_vertices;
                    newEntities[i]->get(geom_vertices);
                    if (geom_vertices.size() != 2){
                    	continue;
                    }
                    std::vector<Topo::Vertex*> all_vertices = getAllVertices(*iter);

                    // recherche des sommets suivant les positions géométriques des extrémités de la courbe
                    Topo::Vertex* vtx0 = TopoHelper::getVertexAtPosition(all_vertices, geom_vertices[0]->getCoord(), tol);
                    Topo::Vertex* vtx1 = TopoHelper::getVertexAtPosition(all_vertices, geom_vertices[1]->getCoord(), tol);
#ifdef _DEBUG_MAJTOPO
                    std::cout<<"newEntities["<<i<<"] : "<<newEntities[i]->getName()<<std::endl;
                    std::cout<<" vtx0 "<<(vtx0?vtx0->getName():"0")<<std::endl;
                    std::cout<<" vtx1 "<<(vtx1?vtx1->getName():"0")<<std::endl;
#endif

                    // dans le cas où la topologie n'est pas coupée, alors on ne fait rien ici
                    if (vtx1 == 0 || vtx0 == 0){
                    	continue;
                    }

                    // remplace le sommet géométrique et fait la fusion des sommets topologiques si nécessaire
                    Topo::Vertex* vtx0new = getVertex(geom_vertices[0], false);
                    if (vtx0new){
#ifdef _DEBUG_MAJTOPO
                        std::cout<<"Vertex::merge, "<<vtx0->getName()<<" est remplacé par "<<vtx0new->getName()<<std::endl;
#endif
                        vtx0new->merge(vtx0, &getInfoCommand());
                        vtx0 = vtx0new;
                    }
                    else {
                    	getInfoCommand().addTopoInfoEntity(vtx0, Internal::InfoCommand::DISPMODIFIED);
                        vtx0->saveTopoProperty();
                        vtx0->setGeomAssociation(geom_vertices[0]);
                    }

                    Topo::Vertex* vtx1new = getVertex(geom_vertices[1], false);
                    if (vtx1new){
#ifdef _DEBUG_MAJTOPO
                        std::cout<<"Vertex::merge, "<<vtx1->getName()<<" est remplacé par "<<vtx1new->getName()<<std::endl;
#endif
                        vtx1new->merge(vtx1, &getInfoCommand());
                        vtx1 = vtx1new;
                    }
                    else {
                    	getInfoCommand().addTopoInfoEntity(vtx1, Internal::InfoCommand::DISPMODIFIED);
                        vtx1->saveTopoProperty();
                        vtx1->setGeomAssociation(geom_vertices[1]);
                    }

                    // récupération des arêtes communes topologiques du côté de la courbe à conserver
                    std::vector<Topo::CoEdge*> coedges2 = getCoEdges(newEntities[i]);

                    // on sélectionne les coedges entre les 2 sommets, ce qui fait un tri
                    std::vector<Topo::CoEdge*> coedges1sorted;
                    TopoHelper::getCoEdgesBetweenVertices(vtx0, vtx1, coedges1, coedges1sorted);

                    // on identifie les sommets internes à la sélection d'arêtes
                    std::vector<Topo::Vertex*> innerVertices;
                    TopoHelper::getInnerVertices(coedges1sorted, innerVertices);

                    if (coedges2.size() == 0){
                        // cas d'une nouvelle courbe
                        for (uint j=0; j<coedges1sorted.size(); j++){
                        	getInfoCommand().addTopoInfoEntity(coedges1sorted[j], Internal::InfoCommand::DISPMODIFIED);
                            coedges1sorted[j]->saveTopoProperty();
                            coedges1sorted[j]->setGeomAssociation(newEntities[i]);
                        }

                        for (uint j=0; j<innerVertices.size(); j++){
                        	getInfoCommand().addTopoInfoEntity(innerVertices[j], Internal::InfoCommand::DISPMODIFIED);
                        	innerVertices[j]->saveTopoProperty();
                        	innerVertices[j]->setGeomAssociation(newEntities[i]);
                        }

                    }
                    else {
                        // cas d'une courbe ancienne, il faut coller les 2 topologies

                        std::vector<Topo::CoEdge*> coedges2sorted;
                        TopoHelper::getCoEdgesBetweenVertices(vtx0, vtx1, coedges2, coedges2sorted);

                        uint nb_bras1_tot = 0;
                        uint nb_bras2_tot = 0;
                        for (uint idx1=0; idx1<coedges1sorted.size(); idx1++)
                        	nb_bras1_tot += coedges1sorted[idx1]->getNbMeshingEdges();
                        for (uint idx1=0; idx1<coedges2sorted.size(); idx1++)
                        	nb_bras2_tot += coedges2sorted[idx1]->getNbMeshingEdges();

                        uint sz1 = coedges1sorted.size();
                        uint sz2 = coedges2sorted.size();
                        // on découpe les arêtes de la deuxième liste en fonction des sommets de la première liste d'arêtes
                        if (sz1>1)
                            coedges2sorted = splitCoEdges(coedges1sorted, coedges2sorted, vtx0, vtx1);
                        // réciproque
                        if (sz2>1)
                            coedges1sorted = splitCoEdges(coedges2sorted, coedges1sorted, vtx0, vtx1);

                        // merge des arêtes
                        if (coedges1sorted.size() != coedges2sorted.size()){
							TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
                            err << "Erreur interne, les listes après découpage ne sont pas de même taille ("
                                    << coedges1sorted.size()<<" != "<<coedges2sorted.size()<<")";
                            throw TkUtil::Exception(err);
                        }


                        if (nb_bras1_tot<=nb_bras2_tot)
                        	for (uint i=0; i<coedges1sorted.size(); i++){
                        		coedges2sorted[i]->merge(coedges1sorted[i], &getInfoCommand());
#ifdef _DEBUG_MAJTOPO
                        		std::cout<<"CoEdge::merge, "<<coedges1sorted[i]->getName()<<" est remplacée par "<<coedges2sorted[i]->getName()<<std::endl;
#endif
                        		if (coedges2sorted[i]->getGeomAssociation() && coedges2sorted[i]->getGeomAssociation()->isDestroyed()
                        				&& coedges1sorted[i]->getGeomAssociation()){
                        			getInfoCommand().addTopoInfoEntity(coedges2sorted[i], Internal::InfoCommand::DISPMODIFIED);
                        			coedges2sorted[i]->saveTopoProperty();
                        			coedges2sorted[i]->setGeomAssociation(coedges1sorted[i]->getGeomAssociation());
                        		}
                        	}
                        else
                        	for (uint i=0; i<coedges2sorted.size(); i++){
                        		coedges1sorted[i]->merge(coedges2sorted[i], &getInfoCommand());
#ifdef _DEBUG_MAJTOPO
                        		std::cout<<"CoEdge::merge, "<<coedges2sorted[i]->getName()<<" est remplacée par "<<coedges1sorted[i]->getName()<<std::endl;
#endif
                        		if (coedges1sorted[i]->getGeomAssociation() && coedges1sorted[i]->getGeomAssociation()->isDestroyed()
                        				&& coedges2sorted[i]->getGeomAssociation()){
                        			getInfoCommand().addTopoInfoEntity(coedges1sorted[i], Internal::InfoCommand::DISPMODIFIED);
                        			coedges1sorted[i]->saveTopoProperty();
                        			coedges1sorted[i]->setGeomAssociation(coedges2sorted[i]->getGeomAssociation());
                        		}
                        	}

                        // tri des coedges dans les edges
                        std::set<Topo::Edge*> all_edges;
                        std::vector<Topo::Edge*> edges;
                        for (uint i=0; i<coedges1sorted.size(); i++){
                            coedges1sorted[i]->getEdges(edges);
                            all_edges.insert(edges.begin(), edges.end());
                        }
                        for (uint i=0; i<coedges2sorted.size(); i++){
                            coedges2sorted[i]->getEdges(edges);
                            all_edges.insert(edges.begin(), edges.end());
                        }

                        for (std::set<Topo::Edge*>::iterator iter2 = all_edges.begin();
                                iter2 != all_edges.end(); ++iter2)
                            (*iter2)->sortCoEdges();

                    } // end else / if coedges2.size() == 0

                } // end for i<newEntities.size()

            } // end if (!newEntities.empty())


            // On laisse l'utilisateur détruire ce qui ne sert plus au niveau topologique
            // on annule les associations vers la géométrie qui disparait
            std::vector<Topo::Vertex*> vertices = getVertices(*iter);
            std::vector<Topo::CoEdge*> coedges = getCoEdges(*iter);

#ifdef _DEBUG_MAJTOPO
            std::cout<<"  Sommets concernés:";
            for (uint i=0; i<vertices.size(); i++)
            	std::cout<<" "<<vertices[i]->getName();
            std::cout<<std::endl;
            std::cout<<"  Arêtes concernées:";
            for (uint i=0; i<coedges.size(); i++){
            	std::cout<<" "<<coedges[i]->getName();
            	if (coedges[i]->getGeomAssociation() && coedges[i]->getGeomAssociation()->isDestroyed())
            		std::cout<<" (-> 0) ";
            }
            std::cout<<std::endl;
#endif
            for (uint i=0; i<vertices.size(); i++){
            	if (vertices[i]->getGeomAssociation() && vertices[i]->getGeomAssociation()->isDestroyed()){
            		getInfoCommand().addTopoInfoEntity(vertices[i], Internal::InfoCommand::DISPMODIFIED);
            		vertices[i]->saveTopoProperty();
            		vertices[i]->setGeomAssociation(0);
            	}
            }
            for (uint i=0; i<coedges.size(); i++){
            	if (coedges[i]->getGeomAssociation() && coedges[i]->getGeomAssociation()->isDestroyed()){
            		getInfoCommand().addTopoInfoEntity(coedges[i], Internal::InfoCommand::DISPMODIFIED);
            		coedges[i]->saveTopoProperty();
            		coedges[i]->setGeomAssociation(0);
            	}
            }

        } // end if (*iter)->isDestroyed()

    } // end for iter = geom_crv.begin()
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::updateGeomAssociation(std::vector<CoFace*>& coface, Geom::GeomEntity* oldGE, Geom::GeomEntity* newGE)
{
    for (uint i=0; i<coface.size(); i++){
#ifdef _DEBUG_MAJTOPO
        std::cout<<"   on change l'association pour "<<*coface[i]<<std::endl;
#endif
        getInfoCommand().addTopoInfoEntity(coface[i], Internal::InfoCommand::DISPMODIFIED);
        //coface[i]->saveCoFaceTopoProperty(&getInfoCommand());
        coface[i]->saveTopoProperty();
        coface[i]->setGeomAssociation(newGE);

        // on fait de même avec les arêtes et sommets internes
        std::vector<CoEdge*> coedges;
        coface[i]->getCoEdges(coedges);
        for (uint j=0; j<coedges.size(); j++){
#ifdef _DEBUG_MAJTOPO
        	std::cout<<"   on traite le cas de "<<*coedges[j]<<std::endl;
#endif
            std::vector<Vertex*> vertices;
            coedges[j]->getVertices(vertices);
            for (uint k=0; k<vertices.size(); k++)
                if (vertices[k]->getGeomAssociation() == oldGE){
                    getInfoCommand().addTopoInfoEntity(vertices[k], Internal::InfoCommand::DISPMODIFIED);
                    vertices[k]->saveTopoProperty();
                    vertices[k]->setGeomAssociation(newGE);
                }
            if (coedges[j]->getGeomAssociation() == oldGE){
                getInfoCommand().addTopoInfoEntity(coedges[j], Internal::InfoCommand::DISPMODIFIED);
                coedges[j]->saveTopoProperty();
                coedges[j]->setGeomAssociation(newGE);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::MAJCoFaces()
{
#ifdef _DEBUG_MAJTOPO
	std::cout<<"CommandModificationTopo::MAJCoFaces()"<<std::endl;
#endif
    // récupération de toutes les surfaces géométriques
    std::list<Geom::GeomEntity*> geom_surf = m_command_edit_geom->getRefEntities(2);

    for (std::list<Geom::GeomEntity*>::iterator iter = geom_surf.begin();
            iter != geom_surf.end(); ++iter){

#ifdef _DEBUG_MAJTOPO
        std::cout<<"  "<<(*iter)->getName()<<((*iter)->isDestroyed()?" [DETRUITE]":" conservée")<<std::endl;
#endif
        if ((*iter)->isDestroyed()){
            // recherche de[s] la[es] surface[s] par laquelle elle est remplacée
            std::vector<Geom::GeomEntity*> newEntities = m_command_edit_geom->getReplacedEntities()[*iter];

#ifdef _DEBUG_MAJTOPO
            std::cout<<"   "<<(*iter)->getName()<<" est remplacée par "<<newEntities.size()<<" surface[s]:";
            for (uint i=0; i<newEntities.size(); i++)
                std::cout<<" "<<newEntities[i]->getName();
            std::cout<<std::endl;
#endif

            // récupération des faces communes topologiques sur la surface à détruire
            std::vector<Topo::CoFace*> coface1 = getCoFaces(*iter, false);

            // si les cofaces sont toutes détruites, on n'a plus rien à faire
            if (coface1.empty())
                continue;

            // on recherche s'il y a des arêtes topologiques internes à cette surface
            // et on fait la fusion avec les sommets internes proches
            fuseInternalCoEdgesAndVertices(*iter, newEntities);


            if (newEntities.empty()){
                // cas d'une surface qui disparait
                // elle est interne à un volume conservé (sinon les cofaces seraient détruites)
                // NB: on n'a plus la relation surface vers volume (car surface détruite)

                for (uint j=0; j<coface1.size(); j++){
                    // suppression de l'association vers cette surface
                	//getInfoCommand().addTopoInfoEntity(coface1[j], Internal::InfoCommand::DISPMODIFIED);
                	coface1[j]->saveCoFaceTopoProperty(&getInfoCommand());
                    coface1[j]->saveTopoProperty();
                    coface1[j]->setGeomAssociation(0);

                    // fusion des CoFaces équivalentes
                    std::vector<Topo::CoFace*> cofaces = getSameCoFaces(coface1[j]);
                    if (cofaces.size() == 1){
                        merge (cofaces[0], coface1[j]);
                    }
                }

            } // end if(newEntities.empty())
            else if (newEntities.size() == 1) {

                std::vector<Topo::CoFace*> coface2 = getCoFaces(newEntities[0], false);

                // une surface a pris la place d'une autre sans topologie
                if (coface2.empty()){
#ifdef _DEBUG_MAJTOPO
                    std::cout<<"  cas d'une géométrie remplacée par une autre, sans topologie"<<std::endl;
#endif

                    // on réaffecte la topologie de la face à détruire
                    updateGeomAssociation(coface1, *iter, newEntities[0]);

                }
                else {
#ifdef _DEBUG_MAJTOPO
                    std::cout<<"  cas d'une géométrie remplacée par une autre, avec topologie"<<std::endl;
#endif
                    // même chose que pour cas d'une surface qui disparait
                    // sauf que l'on connait la surface d'en face
                    // mais on n'en fait rien
                    uint nb_merged_surface = 0;
                    for (uint j=0; j<coface1.size(); j++){
                        // fusion des CoFaces équivalentes
                        std::vector<Topo::CoFace*> cofaces = getSameCoFaces(coface1[j]);
                        if (cofaces.size() == 1){
                            merge (cofaces[0], coface1[j]);
                            nb_merged_surface += 1;
                        }
                    }
                    if (nb_merged_surface == 0 && coface2.size() == 1 && !coface2[0]->isStructured()){
#ifdef _DEBUG_MAJTOPO
                    	std::cout<<"  cas non structuré, on remplace cette face par celles en face"<<std::endl;
#endif
                    	Face* face2 = coface2[0]->getFace(0);
                    	CHECK_NULL_PTR_ERROR(face2);

                    	// suprime la coface et les relations
                    	coface2[0]->free(&getInfoCommand());

                    	face2->addCoFaces(coface1);

                    	updateGeomAssociation(coface1, 0, newEntities[0]);

                    }
                }

            } // end else if (newEntities.size() == 1)
            else {
#ifdef _DEBUG_MAJTOPO
                std::cout<<"  cas d'une géométrie remplacée par plusieurs autres"<<std::endl;
#endif
                // cas d'une surface qui est remplacée par plusieurs surfaces
                for (uint i=0; i<newEntities.size(); i++){

                    std::vector<Topo::CoFace*> coface2 = getCoFaces(newEntities[i], false);

                    // une surface a pris la place d'une autre sans topologie
                    if (coface2.empty()){
                        // les arêtes du contour de la surface i
                        std::vector<CoEdge*> coedges = getCoEdges(newEntities[i], *iter);

                        // recherche de la topologie en se limitant à la surface i
                        std::vector<Topo::CoFace*> coface1_surf = getInnerCoFaces(coface1, coedges);

                        // on réaffecte la sous-topologie de la face à détruire
                        for (uint j=0; j<coface1_surf.size(); j++){
                        	//getInfoCommand().addTopoInfoEntity(coface1_surf[j], Internal::InfoCommand::DISPMODIFIED);
                        	coface1_surf[j]->saveCoFaceTopoProperty(&getInfoCommand());
                            coface1_surf[j]->saveTopoProperty();
                            coface1_surf[j]->setGeomAssociation(newEntities[i]);
                        }
                        // idem avec les arêtes internes
                        associateInnerCoEdges(coface1_surf, newEntities[i]);
                    }

                } // end for i<newEntities.size()

                for (uint i=0; i<newEntities.size(); i++){
                    // fusion des faces si possible
                    for (uint j=0; j<coface1.size(); j++){
                        std::vector<Topo::CoFace*> cofaces = getSameCoFaces(coface1[j]);
                        if (cofaces.size() == 1){
                            merge (cofaces[0], coface1[j]);
                        }
                    }

                } // end for i<newEntities.size()

            } // end else / if (newEntities.empty())

            // supppression des relation vers les entités détruites
            std::vector<Topo::Vertex*> vertices = getVertices(*iter);
            std::vector<Topo::CoEdge*> coedges = getCoEdges(*iter);

            for (uint i=0; i<vertices.size(); i++){
            	if (vertices[i]->getGeomAssociation() && vertices[i]->getGeomAssociation()->isDestroyed()){
            		getInfoCommand().addTopoInfoEntity(vertices[i], Internal::InfoCommand::DISPMODIFIED);
            		vertices[i]->saveTopoProperty();
            		vertices[i]->setGeomAssociation(0);
            	}
            }
            for (uint i=0; i<coedges.size(); i++){
            	if (coedges[i]->getGeomAssociation() && coedges[i]->getGeomAssociation()->isDestroyed()){
            		getInfoCommand().addTopoInfoEntity(coedges[i], Internal::InfoCommand::DISPMODIFIED);
            		coedges[i]->saveTopoProperty();
            		coedges[i]->setGeomAssociation(0);
            	}
            }

            for (uint i=0; i<coface1.size(); i++)
            	if (coface1[i]->getGeomAssociation() && coface1[i]->getGeomAssociation()->isDestroyed()){
            		getInfoCommand().addTopoInfoEntity(coface1[i], Internal::InfoCommand::DISPMODIFIED);
            		coface1[i]->saveTopoProperty();
            		coface1[i]->setGeomAssociation(0);
            	}

        } // end if (*iter)->isDestroyed()

    } // end for iter = geom_surf.begin()

}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::MAJBlocks()
{
#ifdef _DEBUG_MAJTOPO
	std::cout<<"CommandModificationTopo::MAJBlocks()"<<std::endl;
#endif
    // récupération de tous les volumes géométriques
    std::list<Geom::GeomEntity*> geom_vol = m_command_edit_geom->getRefEntities(3);

    for (std::list<Geom::GeomEntity*>::iterator iter = geom_vol.begin();
            iter != geom_vol.end(); ++iter){

#ifdef _DEBUG_MAJTOPO
        std::cout<<"  "<<(*iter)->getName()<<((*iter)->isDestroyed()?" [DETRUIT]":" conservé")<<std::endl;
#endif
        if ((*iter)->isDestroyed()){
        	// recherche le volume par lequel il est remplacé
        	std::vector<Geom::GeomEntity*> newEntities = m_command_edit_geom->getReplacedEntities()[*iter];

#ifdef _DEBUG_MAJTOPO
        	std::cout<<"   "<<(*iter)->getName()<<" est remplacée par "<<newEntities.size()<<" volume[s]:";
        	for (uint i=0; i<newEntities.size(); i++)
        		std::cout<<" "<<newEntities[i]->getName();
        	std::cout<<std::endl;
#endif

        	// récupération des blocs topologiques à détruire
        	std::vector<Topo::Block*> blocs1 = getBlocks(*iter);

        	if (newEntities.size() == 1) {
        		for (uint i=0; i<blocs1.size(); i++){
        			//getInfoCommand().addTopoInfoEntity(blocs1[i], Internal::InfoCommand::DISPMODIFIED);
        			blocs1[i]->saveBlockTopoProperty(&getInfoCommand());
        			blocs1[i]->saveTopoProperty();
        			blocs1[i]->setGeomAssociation(newEntities[0]);
        		}
        	}
        	else if (newEntities.size() > 1){
                // cas d'un volume qui est remplacé par plusieurs volumes

        		// il se peut qu'il y ait des surfaces internes qui ne soient pas associées à la topologie
        		// on tente d'y remédier pour permettre l'association des blocs
        		associateInnerCoFaces(newEntities, blocs1);


        		// Met à jout l'association vers la géométrie pour chacun des nouveaux volumes
        		// On utilise pour cela les relations entre les surfaces et les cofaces
        		for (uint i=0; i<newEntities.size(); i++){

                	// les cofaces du contour du volume i
                	std::vector<CoFace*> cofaces = getCoFaces(newEntities[i], false);

                	// recherche de la topologie en se limitant au volume i
                	std::vector<Topo::Block*> bloc1_vol = getInnerBlocks(blocs1, cofaces);

                	// on réaffecte la sous-topologie de la face à détruire
                	for (uint j=0; j<bloc1_vol.size(); j++){
            			getInfoCommand().addTopoInfoEntity(bloc1_vol[j], Internal::InfoCommand::DISPMODIFIED);
                		bloc1_vol[j]->saveTopoProperty();
                		bloc1_vol[j]->setGeomAssociation(newEntities[i]);
                	}

                } // end for i<newEntities.size()

        		// recherche s'il existe des blocs liés à de la géométrie détruite
        		bool existeBlocOrphelin = false;
        		 for (uint i=0; i<blocs1.size(); i++)
        			 if (blocs1[i]->getGeomAssociation() && blocs1[i]->getGeomAssociation()->isDestroyed())
        				 existeBlocOrphelin = true;

        		 // si oui, on recherche le plus gros volume parmis les newEntities pour lui affecter les blocs
        		 if (existeBlocOrphelin){
#ifdef _DEBUG_MAJTOPO
        			 std::cout<<"   existeBlocOrphelin ..."<<std::endl;
#endif

        			 double vol = newEntities[0]->getArea();
        			 Geom::GeomEntity* biggest = newEntities[0];
        			 for (uint i=1; i<newEntities.size(); i++){
        				 double vol2 = newEntities[i]->getArea();
        				 if (vol2>vol){
        					 vol = vol2;
        					 biggest = newEntities[i];
        				 }
        			 }
#ifdef _DEBUG_MAJTOPO
        			 std::cout<<"   biggest : "<<(biggest?biggest->getName():"vide")<<std::endl;
#endif

        			 for (uint i=0; i<blocs1.size(); i++)
        				 if (blocs1[i]->getGeomAssociation() && blocs1[i]->getGeomAssociation()->isDestroyed()){
        					 getInfoCommand().addTopoInfoEntity(blocs1[i], Internal::InfoCommand::DISPMODIFIED);
        					 blocs1[i]->saveTopoProperty();
        					 blocs1[i]->setGeomAssociation(biggest);
        				 }

        		 } // end if (existeBlocOrphelin)

        	} // end else if (newEntities.size() > 1)

            // supppression des relation vers les entités détruites
            for (uint i=0; i<blocs1.size(); i++)
            	if (blocs1[i]->getGeomAssociation() && blocs1[i]->getGeomAssociation()->isDestroyed()){
        			getInfoCommand().addTopoInfoEntity(blocs1[i], Internal::InfoCommand::DISPMODIFIED);
            		blocs1[i]->saveTopoProperty();
            		blocs1[i]->setGeomAssociation(0);
            	}

        } // end if ((*iter)->isDestroyed())
    } // end for iter = geom_vol.begin()
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::
MAJInternalsCoFaces()
{
#ifdef _DEBUG_MAJTOPO
    std::cout<<"CommandModificationTopo::MAJInternalsCoFaces()"<<std::endl;
#endif
    // on parcours les cofaces modifiées à la recherche de celles non structurées, associées à rien et entre 2 blocs
    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>& tei = getInfoCommand().getTopoInfoEntity();
    std::list<Topo::Block*> l_blocks;
    std::map<Topo::CoFace*, uint> filtre_cofaces;
    std::vector<Topo::CoFace*> v_cofaces_to_delete;
    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::const_iterator iter = tei.begin();
         iter != tei.end(); ++iter) {
        if (iter->first->getDim() == 2 && iter->second == Internal::InfoCommand::DISPMODIFIED){
            Topo::CoFace* ob = dynamic_cast<Topo::CoFace*>(iter->first);
            if (ob && !ob->isStructured() && ob->getGeomAssociation() == 0){
                std::vector<Block*> blocks;
                ob->getBlocks(blocks);
                if (blocks.size() ==2 && blocks[0]->getGeomAssociation() == blocks[1]->getGeomAssociation()) {
//                    std::cout << "coface trouvée : " << ob->getName() << std::endl;
//                    std::cout << " blocks[0]->getGeomAssociation():"<<(blocks[0]->getGeomAssociation()?blocks[0]->getGeomAssociation()->getName():"vide")<<std::endl;
//                    std::cout << " blocks[1]->getGeomAssociation():"<<(blocks[1]->getGeomAssociation()?blocks[1]->getGeomAssociation()->getName():"vide")<<std::endl;
                    if (blocks[0]->isStructured() == false)
                        l_blocks.push_back(blocks[0]);
                    if (blocks[1]->isStructured() == false)
                        l_blocks.push_back(blocks[1]);
                    if (l_blocks.size() == 2) {
                        filtre_cofaces[ob] = 1;
                        v_cofaces_to_delete.push_back(ob);
                    }
                }
            }
        }
    }

    if (l_blocks.empty())
        return;

    // on constitue la liste des blocs associés à ces faces à supprimer
    l_blocks.sort(Utils::Entity::compareEntity);
    l_blocks.unique();
    std::vector<Topo::Block*> v_blocks;
    v_blocks.insert(v_blocks.end(),l_blocks.begin(),l_blocks.end());

    // vérification que tout est bien associé au même volume
    Geom::GeomEntity* vol = v_blocks[0]->getGeomAssociation();

    for (uint i=0; i<v_blocks.size(); i++)
        if (v_blocks[i]->getGeomAssociation() != vol){
            TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "Des blocs ("<< v_blocks[0]->getName() << " et "<< v_blocks[i]->getName()
                    <<") sont identifiés pour être fusionnés mais ne sont pas associés au même volume";
            log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
            return;
        }

    std::list<Topo::Face*> l_faces;
    for (uint i=0; i<v_blocks.size(); i++){
        std::vector<Face*> faces;
        v_blocks[i]->getFaces(faces);
        for (uint j=0; j<faces.size(); j++) {
            std::vector<CoFace*> cofaces;
            faces[j]->getCoFaces(cofaces);
            if (cofaces.size() != 1){
                TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message << "Une face ("<<faces[j]->getName()
                        <<") contient plusieurs cofaces ce qui n'est pas vraiment compatible avec le cas non-structuré";
                log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
                return;
            }
            if (filtre_cofaces[cofaces[0]] == 0) {
                l_faces.push_back(faces[j]);
                filtre_cofaces[cofaces[0]] = 1;
            }
        }
    }

    // on constitue la liste des faces à mettre dans un même bloc
    l_faces.sort(Utils::Entity::compareEntity);
    l_faces.unique();
    std::vector<Topo::Face*> v_faces;
    v_faces.insert(v_faces.end(),l_faces.begin(),l_faces.end());

    // constitution de la liste des sommets
    std::vector<Topo::Vertex*> v_vertices;
    std::map<Topo::Vertex*, uint> filtre_vertices;
    for (uint i=0; i<v_faces.size(); i++){
        v_faces[i]->saveFaceTopoProperty(&getInfoCommand());
        std::vector<Vertex*> vertices;
        v_faces[i]->getVertices(vertices);
        for (uint j=0; j<vertices.size(); j++)
            if (filtre_vertices[vertices[j]] == 0){
                v_vertices.push_back(vertices[j]);
                filtre_vertices[vertices[j]] = 1;
            }
    }

    // création du nouveau bloc non-structuré
    Topo::Block* bloc = new Topo::Block(getContext(), v_faces, v_vertices, false);
    getInfoCommand().addTopoInfoEntity(bloc, Internal::InfoCommand::CREATED);
    bloc->setGeomAssociation(vol);

    // destruction des blocs
    for (uint i=0; i<v_blocks.size(); i++)
        v_blocks[i]->free(&getInfoCommand());
    // destruction des cofaces et des faces
    for (uint i=0; i<v_cofaces_to_delete.size(); i++){
        Topo::CoFace* coface = v_cofaces_to_delete[i];
        std::vector<Face*> faces;
        coface->getFaces(faces);
        for (uint j=0; j<faces.size(); j++)
            faces[j]->free(&getInfoCommand());
        coface->free(&getInfoCommand());
    }
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::
postExecute(bool hasError)
{
    // remet de l'odre dans la mémoire (libération des parties internes)
    if (hasError){
		//std::cout<<"CommandModificationTopo::postExecute, cas en erreur ..."<<std::endl;
        cancelInternalsStats();
    }
}
/*----------------------------------------------------------------------------*/
Topo::Vertex* CommandModificationTopo::
getVertex(Geom::GeomEntity* ge, bool exceptionIfNotFound)
{
    const std::vector<Topo::TopoEntity* >& topos = ge->getRefTopo();

    if (ge->getDim()!=0){
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
        err << "Erreur interne, ce n'est pas un sommet géométrique ("
            << ge->getName()<<")";
        throw TkUtil::Exception(err);
    }

    if (exceptionIfNotFound && topos.size() != 1){
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
        err << "Erreur interne, un sommet géométrique n'a pas une unique référence topologique ("
            << ge->getName()<<")";
        throw TkUtil::Exception(err);
    }

    if (topos.empty())
    	return 0;

    Topo::TopoEntity* topo = topos[0];

    if (topo->getDim()!=0){
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
        err << "Erreur interne, ce n'est pas un sommet topologique qui pointe sur un sommet géométrique ("
            << ge->getName()<<" -> "<<topo->getName()<<")";
        throw TkUtil::Exception(err);
    }

    return dynamic_cast<Topo::Vertex*>(topo);
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::Vertex*> CommandModificationTopo::
getVertices(Geom::GeomEntity* ge)
{
    const std::vector<Topo::TopoEntity* >& topos = ge->getRefTopo();

    std::vector<Topo::Vertex*>  vertices;

    for (uint i=0; i<topos.size(); i++){
        // on ne s'intéresse qu'aux sommets
        if (topos[i]->getDim()==0){
            vertices.push_back(dynamic_cast<Topo::Vertex*>(topos[i]));
        }
    }

    return vertices;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::Vertex*> CommandModificationTopo::
getAllVertices(Geom::GeomEntity* ge)
{
    const std::vector<Topo::TopoEntity* >& topos = ge->getRefTopo();
#ifdef _DEBUG_MAJTOPO
    std::cout<<"getAllVertices("<<ge->getName()<<") topos.size() = "<<topos.size()<<std::endl;
#endif

    std::list<Topo::Vertex*> l_v;

    if (ge->getDim() == 1){
    	for (uint i=0; i<topos.size(); i++){
    		// on ne s'intéresse qu'aux arêtes de cette courbe
    		if (topos[i]->getDim()==1){
    			Topo::CoEdge* coedge = dynamic_cast<Topo::CoEdge*>(topos[i]);
    			if (coedge){
    				const std::vector<Vertex* > & local_vertices = coedge->getVertices();
    				l_v.insert(l_v.end(), local_vertices.begin(), local_vertices.end());
    			}
    		}
    	}
    	// on ajoute les sommets aux extrémités
    	std::vector<Geom::Vertex*> vertices;
    	ge->get(vertices);
    	for (uint j=0; j<vertices.size();j++){
    		const std::vector<Topo::TopoEntity* >& topos_som = vertices[j]->getRefTopo();
    		for (uint i=0; i<topos_som.size(); i++){
    			// on ne s'intéresse qu'aux sommets
    			if (topos_som[i]->getDim()==0){
    				Topo::Vertex* vtx = dynamic_cast<Topo::Vertex*>(topos_som[i]);
    				if (vtx)
    					l_v.push_back(vtx);
    			}
    		}
    	}
    } // end if (ge->getDim() == 1)
    else if (ge->getDim() == 2){
    	for (uint i=0; i<topos.size(); i++){
    		// on ne s'intéresse qu'aux sommets de cette surface
    		if (topos[i]->getDim()==0){
    			Topo::Vertex* vertex = dynamic_cast<Topo::Vertex*>(topos[i]);
    			l_v.push_back(vertex);
    		}
    	}
    } // end else if (ge->getDim() == 2)

    std::vector<Topo::Vertex*>  vertices;

    l_v.sort(Utils::Entity::compareEntity);
    l_v.unique();

    vertices.insert(vertices.end(),l_v.begin(),l_v.end());

#ifdef _DEBUG_MAJTOPO
    std::cout<<"getAllVertices("<<ge->getName()<<") => ";
    for (uint i=0; i<vertices.size(); i++)
    	std::cout<<" "<<vertices[i]->getName();
    std::cout<<std::endl;
#endif

    return vertices;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::CoEdge*> CommandModificationTopo::
getCoEdges(Geom::GeomEntity* ge, Geom::GeomEntity* ge_init)
{

    std::vector<Topo::CoEdge*>  coedges;

    if (ge->getDim() == 1){
        const std::vector<Topo::TopoEntity* >& topos = ge->getRefTopo();
        for (uint i=0; i<topos.size(); i++){
            // on ne s'intéresse qu'aux coedges, on néglige les sommets
            if (topos[i]->getDim()==1 && !topos[i]->isDestroyed()){
                coedges.push_back(dynamic_cast<Topo::CoEdge*>(topos[i]));
            }
        }
    }
    else if (ge->getDim() == 2){
    	bool contour_troue = false;
        std::vector<Geom::Curve*> curves;
        ge->get(curves);
        for (std::vector<Geom::Curve*>::iterator iter = curves.begin();
                iter != curves.end(); ++iter){
            const std::vector<Topo::TopoEntity* >& topos = (*iter)->getRefTopo();
            uint nb_coedge = 0;
            for (uint i=0; i<topos.size(); i++){
                // on ne s'intéresse qu'aux coedges, on néglige les sommets
                if (topos[i]->getDim()==1 && !topos[i]->isDestroyed()){
                    coedges.push_back(dynamic_cast<Topo::CoEdge*>(topos[i]));
                    nb_coedge+=1;
                }
            }
            // cas où les courbes du contour n'ont aucune coedge d'associé
            if (nb_coedge == 0)
            	contour_troue = true;
        } // end for iter = curves ...

        if (contour_troue && ge_init){
        	// on cherche parmis les arêtes des cofaces (de la surface initiale),
        	// celles qui pointeraient sur une courbe
        	// similaire à celles pour lesquelles on n'a rien trouvé
#ifdef _DEBUG_MAJTOPO
        	std::cout<<"CommandModificationTopo::getCoEdges("<<ge->getName()<<") avec cas d'un contour troué"<<std::endl;
#endif
        	std::vector<Topo::CoFace*> cofaces = getCoFaces(ge_init, false);
        	std::vector<Topo::CoEdge*> all_coedges;
        	TopoHelper::getCoEdges(cofaces, all_coedges);

        	std::list<Geom::Curve*> courbes_interessantes;

        	// on marque les filtre de la surface considérée
        	std::map<Geom::Curve*, uint> filtre_curve;
        	// on marque les coedges qui sont intéressantes
        	std::map<Topo::CoEdge*, uint> filtre_coedge;

        	for (std::vector<Geom::Curve*>::iterator iter = curves.begin();
        			iter != curves.end(); ++iter)
        		filtre_curve[*iter] = 1;

        	for (std::vector<Topo::CoEdge*>::iterator iter = all_coedges.begin();
        			iter != all_coedges.end(); ++iter){
        		Topo::CoEdge* coedge = *iter;
        		if (coedge->getGeomAssociation() && coedge->getGeomAssociation()->getDim() == 1){
        			Geom::Curve* curve = dynamic_cast<Geom::Curve*>(coedge->getGeomAssociation());
        			if (filtre_curve[curve] == 0){
        				courbes_interessantes.push_back(curve);
        				filtre_coedge[coedge] = 1;
        			}
        		}
        	}

        	// on recommence la recherche à partir de la surface
        	for (std::vector<Geom::Curve*>::iterator iter = curves.begin();
        			iter != curves.end(); ++iter){
        		const std::vector<Topo::TopoEntity* >& topos = (*iter)->getRefTopo();
        		uint nb_coedge = 0;
        		for (uint i=0; i<topos.size(); i++){
        			if (topos[i]->getDim()==1 && !topos[i]->isDestroyed()){
        				nb_coedge+=1;
        			}
        		}
        		// cas où les courbes du contour n'ont aucune coedge d'associé
        		if (nb_coedge == 0){
        			// on recherche si cette courbe est contenue par l'une de celles ditent intéressante

        			for (std::list<Geom::Curve*>::iterator iter2 = courbes_interessantes.begin();
        					iter2 != courbes_interessantes.end(); ++iter2){
        				if ((*iter2)->contains(*iter)){
        					// on va récupérer les coedges de cette courbe
        					const std::vector<Topo::TopoEntity* >& topos = (*iter2)->getRefTopo();
        					for (std::vector<Topo::TopoEntity* >::const_iterator iter3 = topos.begin();
        							iter3 != topos.end(); ++iter3)
        						if ((*iter3)->getDim() == 1){
        							Topo::CoEdge* coedge = dynamic_cast<Topo::CoEdge*>(*iter3);
        							if (filtre_coedge[coedge] == 1){
        								filtre_coedge[coedge] = 0; // pour ne la prendre qu'une fois
        								coedges.push_back(coedge);
        							}
        						}
        				}
        			}
        		}
        	} // end for iter = curves ...

        } // if contour_troue
    }
    else {
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
        err << "Erreur interne, ce n'est pas une courbe ou surface géométrique ("
                << ge->getName()<<")";
        throw TkUtil::Exception(err);
    }

    return coedges;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::CoFace*> CommandModificationTopo::
getCoFaces(Geom::GeomEntity* ge, bool exceptionIfNotFound)
{
    const std::vector<Topo::TopoEntity* >& topos = ge->getRefTopo();

    if (exceptionIfNotFound && topos.size() == 0){
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
        err << "Erreur interne, un[e] surface/volume géométrique n'a pas de références topologiques ("
            << ge->getName()<<")";
        throw TkUtil::Exception(err);
    }

    std::vector<Topo::CoFace*>  cofaces;

    if (ge->getDim()==2){
        for (uint i=0; i<topos.size(); i++){
            // on ne s'intéresse qu'aux CoFaces, on néglige les sommets et les arêtes
            if (topos[i]->getDim()==2 && !topos[i]->isDestroyed()){
                cofaces.push_back(dynamic_cast<Topo::CoFace*>(topos[i]));
            }
        }
    }
    else if (ge->getDim()==3){
        std::vector<Geom::Surface*> surfaces;
         ge->get(surfaces);
         for (std::vector<Geom::Surface*>::iterator iter = surfaces.begin();
                 iter != surfaces.end(); ++iter){
             const std::vector<Topo::TopoEntity* >& topos = (*iter)->getRefTopo();
             for (uint i=0; i<topos.size(); i++){
                 // on ne s'intéresse qu'aux CoFaces, on néglige les sommets et les arêtes
                 if (topos[i]->getDim()==2 && !topos[i]->isDestroyed()){
                	 cofaces.push_back(dynamic_cast<Topo::CoFace*>(topos[i]));
                 }
             }
         }
    }
    else {
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
    	err << "Erreur interne, ce n'est pas une surface ou un volume géométrique ("
    			<< ge->getName()<<")";
    	throw TkUtil::Exception(err);
    }

#ifdef _DEBUG_MAJTOPO
	std::cout<<"getCoFaces("<<ge->getName()<<", "<<exceptionIfNotFound<<") retourne :";
	for (uint i=0; i<cofaces.size(); i++)
		std::cout<<" "<<cofaces[i]->getName();
	std::cout<<std::endl;
#endif

    return cofaces;
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::
fuseInternalCoEdgesAndVertices(Geom::GeomEntity* ge, std::vector<Geom::GeomEntity*>& newEntities)
{
#ifdef _DEBUG_MAJTOPO
    std::cout<<"fuseInternalCoEdgesAndVertices() pour "<<ge->getName()<<std::endl;
#endif
    // on recherche s'il y a des arêtes et des sommets topologiques internes à cette surface

    const std::vector<Topo::TopoEntity* >& topos = ge->getRefTopo();

    if (ge->getDim()!=2){
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
        err << "Erreur interne, ce n'est pas une surface géométrique ("
            << ge->getName()<<")";
        throw TkUtil::Exception(err);
    }

    // si la fusion se fait avec une autre surface sans topo, alors on ne fait rien
    // (on ne supprime pas les associations)
    if (newEntities.size() == 1){
    	const std::vector<Topo::TopoEntity* >& topos2 = newEntities[0]->getRefTopo();
    	if (topos2.empty())
    		return;
    }

    // on marque à 1 ce qui est interne, à 2 ce qui est sur le bord
    // comme la surface est détruite, on n'a plus la relation vers les courbes
    // on considère que c'est interne à partir du moment où la projection se fait sur la surface
    std::map<CoEdge*, uint> filtre_coedges;
    std::map<Vertex*, uint> filtre_vertices;

#ifdef _DEBUG_MAJTOPO2
    std::cout<<" Relations topologiques : "<<std::endl;
#endif
    for (uint i=0; i<topos.size(); i++){
#ifdef _DEBUG_MAJTOPO2
    	std::cout<<"   "<<topos[i]->getName()<<std::endl;
#endif
    	if (topos[i]->getDim()==2){
            CoFace* coface = dynamic_cast<Topo::CoFace*>(topos[i]);
            CHECK_NULL_PTR_ERROR(coface);
            std::vector<CoEdge*> coedges;
            coface->getCoEdges(coedges);

            for (std::vector<CoEdge*>::iterator iter1 = coedges.begin();
                    iter1 != coedges.end(); ++iter1){
#ifdef _DEBUG_MAJTOPO2
            	std::cout<<"      "<<(*iter1)->getName()<<" en relation avec "<<((*iter1)->getGeomAssociation() == 0?"rien":(*iter1)->getGeomAssociation()->getName())<<std::endl;
#endif
                filtre_coedges[*iter1] = ((*iter1)->getGeomAssociation() == ge?1:2);
                std::vector<Vertex*> vertices;
                (*iter1)->getVertices(vertices);
                for (std::vector<Vertex*>::iterator iter2 = vertices.begin();
                        iter2 != vertices.end(); ++iter2){
#ifdef _DEBUG_MAJTOPO2
                	std::cout<<"         "<<(*iter2)->getName()<<" en relation avec "<<((*iter2)->getGeomAssociation() == 0?"rien":(*iter2)->getGeomAssociation()->getName())<<std::endl;
#endif
                    filtre_vertices[*iter2] = ((*iter2)->getGeomAssociation() == ge?1:2);
                }
            }
        }
        else if (topos[i]->getDim()==1){
            CoEdge* coedge = dynamic_cast<Topo::CoEdge*>(topos[i]);
            CHECK_NULL_PTR_ERROR(coedge);
            filtre_coedges[coedge] = (coedge->getGeomAssociation() == ge?1:2);
            std::vector<Vertex*> vertices;
            coedge->getVertices(vertices);
            for (std::vector<Vertex*>::iterator iter2 = vertices.begin();
                    iter2 != vertices.end(); ++iter2){
#ifdef _DEBUG_MAJTOPO2
            	std::cout<<"      "<<(*iter2)->getName()<<" en relation avec "<<((*iter2)->getGeomAssociation() == 0?"rien":(*iter2)->getGeomAssociation()->getName())<<std::endl;
#endif
                filtre_vertices[*iter2] = ((*iter2)->getGeomAssociation() == ge?1:2);
            }
        }
        else if (topos[i]->getDim()==0){
            Vertex* vertex = dynamic_cast<Topo::Vertex*>(topos[i]);
            CHECK_NULL_PTR_ERROR(vertex);
            filtre_vertices[vertex] = (vertex->getGeomAssociation() == ge?1:2);
#ifdef _DEBUG_MAJTOPO2
            std::cout<<"    "<<vertex->getName()<<" en relation avec "<<(vertex->getGeomAssociation() == 0?"rien":vertex->getGeomAssociation()->getName())<<std::endl;
#endif
        }
    } // end for i<topos.size()


    // constitution des listes de sommets et arêtes internes
    std::vector<Topo::CoEdge*>  internalCoedges;
    std::vector<Topo::Vertex*>  internalVertices;
    uint nbBorderVertices = 0;
    for (std::map<CoEdge*, uint>::iterator iter = filtre_coedges.begin();
            iter != filtre_coedges.end(); ++iter)
        if ((*iter).second == 1)
            internalCoedges.push_back((*iter).first);
    for (std::map<Vertex*, uint>::iterator iter = filtre_vertices.begin();
            iter != filtre_vertices.end(); ++iter)
        if ((*iter).second == 1)
            internalVertices.push_back((*iter).first);
        else if ((*iter).second == 2)
        	nbBorderVertices++;

#ifdef _DEBUG_MAJTOPO
    std::cout<<"fuseInternalCoEdgesAndVertices pour "<<ge->getName()<<" trouve :"<<std::endl;
    std::cout<<"internalCoedges :";
    for (uint i=0; i<internalCoedges.size(); i++)
        std::cout<<" "<<internalCoedges[i]->getName();
    std::cout<<std::endl;
    std::cout<<"internalVertices :";
    for (uint i=0; i<internalVertices.size(); i++)
        std::cout<<" "<<internalVertices[i]->getName();
    std::cout<<std::endl;
    std::cout<<"nb de sommets au bord : "<<nbBorderVertices<<std::endl;
#endif

    // s'il n'y a pas de sommet au bord, il faut essayer de faire une fusion au plus proche
    // cas d'une surface d'une sphère avec une autre du même type
    if (!internalVertices.empty() && nbBorderVertices == 0 && newEntities.size() == 1){

    	// Pour chacun des sommets, on fait la fusion avec le sommet proche de l'autre groupe
    	std::vector<Topo::Vertex*> all_vertices2 = getAllVertices(newEntities[0]);

#ifdef _DEBUG_MAJTOPO
    	std::cout<<"Fusion des sommets au plus proche entre les 2 surfaces, "<<internalVertices.size()
    			 <<" à coller avec "<<all_vertices2.size()<<" sommets"<<std::endl;
#endif
   	std::map<Vertex*, uint> filtre_vertices;
    	double tol = ge->getArea()*Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon;
    	for (uint i=0; i<internalVertices.size(); i++){
    		Topo::Vertex* vtx0 = internalVertices[i];
    		Topo::Vertex* vtx0new = TopoHelper::getVertexAtPosition(all_vertices2, vtx0->getCoord(), tol);

    		if (vtx0new){
#ifdef _DEBUG_MAJTOPO
    			std::cout<<"Vertex::merge, "<<vtx0->getName()<<" est remplacé par "<<vtx0new->getName()<<std::endl;
#endif
    			// fusion des sommets
    			vtx0new->merge(vtx0, &getInfoCommand());
    			filtre_vertices[vtx0new] = 2;
    		}
    	}
    } // end if (nbBorderVertices == 0)

    if (!internalVertices.empty() && nbBorderVertices){
        // les sommets au bord sont déjà fusionnés, et marqués à 2
        // on parcours les arêtes internes et pour celles reliées au bord on recherche un sommet non vu
        // que l'on tente de relier avec un sommet voisin géométriquement
        // on recherche ce sommet en passant par les arêtes reliées au sommet au bord
        // (ou simplement marqué comme fusionné avec voisin)

        uint nbInternalVerticesFused = 0;
        uint nbInternalVerticesFused_loc = 0;

        do {
            nbInternalVerticesFused_loc = 0;

            for (uint i=0; i<internalCoedges.size(); i++){
                CoEdge* coedge = internalCoedges[i];
                if (filtre_coedges[coedge] == 1 && coedge->getNbVertices() == 2
                        && ((filtre_vertices[coedge->getVertex(0)] == 1 && filtre_vertices[coedge->getVertex(1)] == 2)
                            || (filtre_vertices[coedge->getVertex(0)] == 2 && filtre_vertices[coedge->getVertex(1)] == 1))){
                    Vertex* vtx1;
                    Vertex* vtxBord;
                    if (filtre_vertices[coedge->getVertex(0)] == 1 && filtre_vertices[coedge->getVertex(1)] == 2){
                        vtx1 = coedge->getVertex(0);
                        vtxBord = coedge->getVertex(1);
                    } else {
                        vtx1 = coedge->getVertex(1);
                        vtxBord = coedge->getVertex(0);
                    }
                    // pour ne pas reprendre cette arête
                    filtre_coedges[coedge] = 2;

                    // tous les sommets reliés à vtxBord par une arête
                    std::vector<Vertex*> vertices = TopoHelper::getVerticesNeighbour(vtxBord);

                    // on filtre les sommets (on ne veut pas retrouver vtx1 ni les sommets détruits)
                    std::vector<Vertex*> vertices2;
                    for (std::vector<Topo::Vertex*>::iterator iter = vertices.begin(); iter != vertices.end(); ++iter)
                        if (*iter != vtx1 && !(*iter)->isDestroyed())
                            vertices2.push_back(*iter);

                    Vertex* vtx2 = TopoHelper::getVertexAtPosition(vertices2, vtx1->getCoord(), Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon);
                    if (vtx2){
#ifdef _DEBUG_MAJTOPO
                        std::cout<<"Vertex::merge, "<<vtx1->getName()<<" est remplacé par "<<vtx2->getName()<<std::endl;
#endif
                        vtx2->merge(vtx1, &getInfoCommand());
                        filtre_vertices[vtx1] = 2;
                        nbInternalVerticesFused_loc += 1;
                    }
                }
            }

            nbInternalVerticesFused += nbInternalVerticesFused_loc;

            // on boucle jusqu'à avoir traité tous les sommets, mais on évite les boucles infinies
            // NB: on ne trouve pas forcément des sommets pour la fusion,
            // par exemple dans le cas d'une surface qui est au bord d'un volume mais pas entre 2
        } while (internalVertices.size() != nbInternalVerticesFused && nbInternalVerticesFused_loc != 0);

    } // end if (!internalVertices.empty())

    // recherche des arêtes équivalentes (2 sommets identiques) parmis les arêtes internes
    for (uint i=0; i<internalCoedges.size(); i++){
        std::vector<CoEdge*> coedges = getSameCoEdges(internalCoedges[i]);

        if (coedges.size() == 1){
            if (internalCoedges[i]->getGeomAssociation() && !internalCoedges[i]->getGeomAssociation()->isDestroyed()
                    && (!coedges[0]->getGeomAssociation() || (coedges[0]->getGeomAssociation() && coedges[0]->getGeomAssociation()->isDestroyed() ))){
            	getInfoCommand().addTopoInfoEntity(coedges[0], Internal::InfoCommand::DISPMODIFIED);
            	coedges[0]->saveTopoProperty();
                coedges[0]->setGeomAssociation(internalCoedges[i]->getGeomAssociation());
#ifdef _DEBUG_MAJTOPO
                std::cout<<"Récupération de l'association vers "<<internalCoedges[i]->getGeomAssociation()->getName()<<std::endl;
#endif
            }

#ifdef _DEBUG_MAJTOPO
            std::cout<<"CoEdge::merge, [arêtes internes] "<<internalCoedges[i]->getName()<<" est remplacé par "<<coedges[0]->getName()<<std::endl;
#endif
            coedges[0]->merge(internalCoedges[i], &getInfoCommand());
        }
        else if (coedges.size() == 0){
        	// cas où l'on cherche si cette arête ne serait pas à projeter sur une nouvelle courbe
//        	std::vector<Geom::GeomEntity*> newEntities = m_command_edit_geom->getReplacedEntities()[ge];

        	if (newEntities.size() >= 2){
        		// recherche des courbes communes aux différentes surfaces
        		std::map<Geom::Curve*, uint> filtre_curves;
        		for (uint k=0; k<newEntities.size(); k++){
            		std::vector<Geom::Curve*> curves;
        			newEntities[k]->get(curves);

        			for (uint j=0; j<curves.size(); ++j){
        				filtre_curves[curves[j]] += 1;
        			}
        		}

        		for (std::map<Geom::Curve*, uint>::iterator iter = filtre_curves.begin();
        				iter != filtre_curves.end(); ++iter){
        			if (iter->second >= 2){
        				Geom::Curve* curve = iter->first;
#ifdef _DEBUG_MAJTOPO2
        				std::cout<<" cas avec la courbe "<<curve->getName()
        						<<" potentiellement intéressante pour arête "<<internalCoedges[i]->getName()<<std::endl;
#endif

        				// recherche si les sommets extrémités sont compatibles avec ceux de l'arête
        				std::vector<Geom::Vertex*> vertices;
        				curve->get(vertices);
        				// les sommets géométriques aux extrémités de la topologie
        				Geom::Vertex* vtx1 = 0;
        				Geom::Vertex* vtx2 = 0;
        				if (internalCoedges[i]->getVertex(0)->getGeomAssociation())
        					vtx1 = dynamic_cast<Geom::Vertex*>(internalCoedges[i]->getVertex(0)->getGeomAssociation());
          				if (internalCoedges[i]->getVertex(1)->getGeomAssociation())
            					vtx2 = dynamic_cast<Geom::Vertex*>(internalCoedges[i]->getVertex(1)->getGeomAssociation());
          				if (vertices.size() == 2 &&
          						((vertices[0] == vtx1 && vertices[1] == vtx2)
          								|| (vertices[0] == vtx2 && vertices[1] == vtx1))){
          					getInfoCommand().addTopoInfoEntity(internalCoedges[i], Internal::InfoCommand::DISPMODIFIED);
          					internalCoedges[i]->saveTopoProperty();
          					internalCoedges[i]->setGeomAssociation(curve);
          				}
        			} // end if (iter->second == newEntities.size())
        		} // end for iter = filtre_curves.begin()
        	} // end if (newEntities.size() >= 2)
        } // end else if (coedges.size() == 0)

        // si l'arête pointe encore sur quelque chose de détruit, on supprime l'association
        if (internalCoedges[i]->getGeomAssociation() && internalCoedges[i]->getGeomAssociation()->isDestroyed()){
        	getInfoCommand().addTopoInfoEntity(internalCoedges[i], Internal::InfoCommand::DISPMODIFIED);
        	internalCoedges[i]->saveTopoProperty();
        	internalCoedges[i]->setGeomAssociation(0);
        }
    } // end for i<internalCoedges.size()
#ifdef _DEBUG_MAJTOPO
    std::cout<<"fuseInternalCoEdgesAndVertices FIN"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::Block*> CommandModificationTopo::
getBlocks(Geom::GeomEntity* ge)
{
    const std::vector<Topo::TopoEntity* >& topos = ge->getRefTopo();

    if (ge->getDim()!=3){
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
        err << "Erreur interne, ce n'est pas un volume géométrique ("
            << ge->getName()<<")";
        throw TkUtil::Exception(err);
    }

    std::vector<Topo::Block*>  blocks;

    for (uint i=0; i<topos.size(); i++){
        // on ne s'intéresse qu'aux Block, on néglige les autres entités
        if (topos[i]->getDim()==3 && !topos[i]->isDestroyed()){
            blocks.push_back(dynamic_cast<Topo::Block*>(topos[i]));
        }
    }

    return blocks;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::CoEdge*> CommandModificationTopo::
getSameCoEdges(Topo::CoEdge* coedge)
{
    std::vector<Topo::CoEdge*> coedges_trouvees;

    if (0 == coedge)
        return coedges_trouvees;

    std::map<Vertex*, uint> filtre_vertices;

    std::vector<Vertex*> vertices;

    coedge->getVertices(vertices);

    for (uint i=0; i<vertices.size(); i++)
        filtre_vertices[vertices[i]] = 1;

    if (vertices.empty())
        return coedges_trouvees;

    // recherche parmi les coedges du premier vertex venu
    std::vector<Topo::CoEdge*> coedges;
    vertices[0]->getCoEdges(coedges);

    for (uint i=0; i<coedges.size(); i++){
        coedges[i]->getVertices(vertices);
        uint nbVertexMarques = 0;
        for (uint j=0; j<vertices.size(); j++)
            nbVertexMarques += filtre_vertices[vertices[j]];

        if (nbVertexMarques == vertices.size() && coedges[i] != coedge)
            coedges_trouvees.push_back(coedges[i]);
    }

#ifdef _DEBUG_MAJTOPO
    std::cout<<"getSameCoEdges("<<coedge->getName()<<") =>";
    if (coedges_trouvees.empty())
        std::cout<<" rien"<<std::endl;
    else {
        for (uint i=0; i<coedges_trouvees.size(); i++)
            std::cout<<" "<<coedges_trouvees[i]->getName();
        std::cout<<std::endl;
    }
#endif

    return coedges_trouvees;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::CoFace*> CommandModificationTopo::
getSameCoFaces(Topo::CoFace* coface)
{
   std::vector<Topo::CoFace*> cofaces_trouvees;

    if (0 == coface)
        return cofaces_trouvees;
#ifdef _DEBUG_MAJTOPO
    std::cout<<"getSameCoFaces avec coface = "<<coface->getName()<<std::endl;
#endif

    std::map<CoEdge*, uint> filtre_coedges;

    std::vector<CoEdge* > coedges;

    coface->getCoEdges(coedges);

    for (uint i=0; i<coedges.size(); i++)
        filtre_coedges[coedges[i]] = 1;

    if (coedges.empty())
        return cofaces_trouvees;

    // recherche parmi les cofaces de la première coedge venue
    std::vector<Topo::CoFace*> cofaces;
    coedges[0]->getCoFaces(cofaces);
#ifdef _DEBUG_MAJTOPO
    std::cout<<"coedges[0] = "<<coedges[0]->getName()<<std::endl;
#endif

    for (uint i=0; i<cofaces.size(); i++){
        cofaces[i]->getCoEdges(coedges);
        uint nbCoedgesMarques = 0;
        for (uint j=0; j<coedges.size(); j++)
            nbCoedgesMarques += filtre_coedges[coedges[j]];

        if (nbCoedgesMarques == coedges.size() && cofaces[i] != coface)
            cofaces_trouvees.push_back(cofaces[i]);
#ifdef _DEBUG_MAJTOPO
        std::cout<<"  cofaces["<<i<<"] = "<<cofaces[i]->getName()<<", nbCoedgesMarques = "<<nbCoedgesMarques<<std::endl;
#endif
    }

#ifdef _DEBUG_MAJTOPO
    std::cout<<"getSameCoFaces("<<coface->getName()<<") =>";
    if (cofaces_trouvees.empty())
        std::cout<<" rien"<<std::endl;
    else {
        for (uint i=0; i<cofaces_trouvees.size(); i++)
            std::cout<<" "<<cofaces_trouvees[i]->getName();
        std::cout<<std::endl;
    }
#endif

    return cofaces_trouvees;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::CoEdge*> CommandModificationTopo::
splitCoEdges(std::vector<Topo::CoEdge*>& coedges1,
        std::vector<Topo::CoEdge*>& coedges2,
        Topo::Vertex* vtx0, Topo::Vertex* vtx1)
{
#ifdef _DEBUG_MAJTOPO
    std::cout<<"CommandModificationTopo::splitCoEdges() avec :"<<std::endl;
    std::cout<<"coedges1:";
    for (uint i=0; i<coedges1.size(); i++)
        std::cout<<" "<<coedges1[i]->getName();
    std::cout<<std::endl;
    std::cout<<"coedges2:";
    for (uint i=0; i<coedges2.size(); i++)
        std::cout<<" "<<coedges2[i]->getName();
    std::cout<<std::endl;
    std::cout<<"vtx0: "<<vtx0->getName()<<std::endl;
    std::cout<<"vtx1: "<<vtx1->getName()<<std::endl;

    std::cout<<" En détail: "<<std::endl;
    std::cout<<"coedges1: "<<std::endl;
    for (uint i=0; i<coedges1.size(); i++)
        std::cout<<*coedges1[i]<<std::endl;
    std::cout<<"coedges2: "<<std::endl;
    for (uint i=0; i<coedges2.size(); i++)
        std::cout<<*coedges2[i]<<std::endl;
#endif

    // pour le cas avec semi-conformité, on calcul un coeff par côté
    // ce coeff multiplicateur permet de se ramener à des discrétisations équivalentes
    uint coeff1 = 1;
    uint coeff2 = 1;
    uint nb_bras1_tot = 0;
    uint nb_bras2_tot = 0;
    for (uint idx1=0; idx1<coedges1.size(); idx1++)
    	nb_bras1_tot += coedges1[idx1]->getNbMeshingEdges();
    for (uint idx1=0; idx1<coedges2.size(); idx1++)
    	nb_bras2_tot += coedges2[idx1]->getNbMeshingEdges();

    if (nb_bras1_tot == 0 || nb_bras2_tot == 0){
		TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
        err << "Erreur interne, CommandModificationTopo::splitCoEdges ne peut se faire, nombre de bras nul !";
        throw TkUtil::Exception(err);
    }

    if (nb_bras1_tot>nb_bras2_tot)
    	coeff2 = nb_bras1_tot/nb_bras2_tot;
    else if (nb_bras1_tot<nb_bras2_tot)
    	coeff1 = nb_bras2_tot/nb_bras1_tot;
#ifdef _DEBUG_MAJTOPO
    std::cout<<"coeff1: "<<coeff1<<std::endl;
    std::cout<<"coeff2: "<<coeff2<<std::endl;
#endif

    // les nouvelles arêtes après découpage ou les anciennes sinon
    std::vector<Topo::CoEdge*> coedges2new;

    // le sommet précédent
    Topo::Vertex* vtx_prec1 = vtx0;

    // index pour coedges2, indépendant de idx1
    uint idx2 = 0;

    for (uint idx1=0; idx1<coedges1.size(); idx1++){
        // le sommet actuel pour le découpage
        Topo::Vertex* vtxI1 = coedges1[idx1]->getOppositeVertex(vtx_prec1);
#ifdef _DEBUG_MAJTOPO
        std::cout<<"vtx_prec1 : "<<vtx_prec1->getName()<<std::endl;
        std::cout<<"idx1 : "<<idx1<<std::endl;
        std::cout<<"vtxI1: "<<vtxI1->getName()<<std::endl;
#endif

        // nombre de bras (longueur côté 1) pour positionner le sommet dans l'arête du côté coedges2
        uint nb_bras1 = coeff1*coedges1[idx1]->getNbMeshingEdges();
#ifdef _DEBUG_MAJTOPO
        std::cout<<"nb_bras1: "<<nb_bras1<<std::endl;
#endif

        // longueur côté 2
        uint nb_bras2 = 0;

        // pour avoir le sens des arêtes dans coedges2
        Topo::Vertex* vtx_prec2 = vtx_prec1;

        Topo::Vertex* vtxI2 = 0;
        while(nb_bras2 + coeff2*coedges2[idx2]->getNbMeshingEdges() < nb_bras1) {
            coedges2new.push_back(coedges2[idx2]); // on passe l'arête donc on la conserve
            vtxI2 = coedges2[idx2]->getOppositeVertex(vtx_prec2);
            nb_bras2 += coeff2*coedges2[idx2]->getNbMeshingEdges();
            idx2 += 1;
            vtx_prec2 = vtxI2;
        }
#ifdef _DEBUG_MAJTOPO
        std::cout<<"nb_bras2 : "<<nb_bras2<<std::endl;
        std::cout<<"idx2 : "<<idx2<<std::endl;
#endif
        vtxI2 = coedges2[idx2]->getOppositeVertex(vtx_prec2);
#ifdef _DEBUG_MAJTOPO
        std::cout<<"vtxI2: "<<vtxI2->getName()<<std::endl;
#endif

        // cas avec simple fusion des sommets, qui peuvent être les mêmes
        if (nb_bras2 + coeff2*coedges2[idx2]->getNbMeshingEdges() == nb_bras1){
#ifdef _DEBUG_MAJTOPO
            std::cout<<"Vertex::merge, "<<vtxI1->getName()<<" est remplacé par "<<vtxI2->getName()<<std::endl;
#endif
            vtxI2->merge(vtxI1, &getInfoCommand());
            coedges2new.push_back(coedges2[idx2]);

            // MAJ de la projection
            if (vtxI1!=vtxI2 && vtxI1->getGeomAssociation() && !vtxI1->getGeomAssociation()->isDestroyed()){
            	getInfoCommand().addTopoInfoEntity(vtxI2, Internal::InfoCommand::DISPMODIFIED);
                vtxI2->saveTopoProperty();
                vtxI2->setGeomAssociation(vtxI1->getGeomAssociation());
            }
            vtxI1 = vtxI2;
            idx2 += 1;
        }
        else {
            // cas du découpage

            // sens normal dans la coedge2 ?
            bool sens2 = (vtx_prec2 == coedges2[idx2]->getVertex(0));

            uint nbMeshingEdges = (sens2 ? nb_bras1 - nb_bras2
                    :coedges2[idx2]->getNbMeshingEdges()*coeff2 - nb_bras1 + nb_bras2)/coeff2;
#ifdef _DEBUG_MAJTOPO
            std::cout<<"nbMeshingEdges : "<<nbMeshingEdges<<std::endl;
            std::cout<<"découpage de "<<coedges2[idx2]->getName()<<std::endl;
            std::cout<<"sens2 = "<<sens2<<std::endl;
#endif

            // découpe sans tri des coedges
            std::vector<CoEdge*> newCoEdges = coedges2[idx2]->split(vtxI1, nbMeshingEdges, &getInfoCommand(), false);

            if (sens2){
                // une nouvelle arête
                coedges2new.push_back(newCoEdges[0]);
                // la deuxième est à utiliser pour positionner le sommet suivant
                coedges2[idx2] = newCoEdges[1];
            }
            else {
                coedges2new.push_back(newCoEdges[1]);
                coedges2[idx2] = newCoEdges[0];
            }

            // MAJ de la projection du sommet
            if (vtxI1->getGeomAssociation() && vtxI1->getGeomAssociation()->isDestroyed()
                    && coedges2[idx2]->getGeomAssociation()){
            	getInfoCommand().addTopoInfoEntity(vtxI1, Internal::InfoCommand::DISPMODIFIED);
                vtxI1->saveTopoProperty();
                vtxI1->setGeomAssociation(coedges2[idx2]->getGeomAssociation());
            }


#ifdef _DEBUG_MAJTOPO
            std::cout<<"Création des arêtes  : "<<*newCoEdges[0]<<std::endl;
            std::cout<<"et de                : "<<*newCoEdges[1]<<std::endl;
#endif
        } // end else / if (nb_bras2 + coedges2[idx2]->getNbMeshingEdges() == nb_bras1)

        vtx_prec1 = vtxI1;
    } // end for i<coedges1.size()


    return coedges2new;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::CoEdge*> CommandModificationTopo::
getInnerCoEdges(std::vector<Topo::CoEdge*>& coedges,
		std::vector<Vertex*>& vertices)
{
#ifdef _DEBUG_MAJTOPO
    std::cout<<"getInnerCoEdges :"<<std::endl;
    std::cout<<" coedges :";
    for (uint i=0; i<coedges.size(); i++)
        std::cout<<" "<<coedges[i]->getName();
    std::cout<<std::endl;
    std::cout<<" vertices :";
    for (uint i=0; i<vertices.size(); i++)
        std::cout<<" "<<vertices[i]->getName();
    std::cout<<std::endl;
#endif

    std::vector<CoEdge* > innerCoEdges;

	if (vertices.size() != 2){
#ifdef _DEBUG_MAJTOPO
		std::cout<<"  getInnerCoEdges => rien"<<std::endl;
#endif
		return innerCoEdges;
	}


	TopoHelper::getCoEdgesBetweenVertices(vertices[0], vertices[1],
			coedges, innerCoEdges);

#ifdef _DEBUG_MAJTOPO
            	std::cout<<"  getInnerCoEdges => ";
            	for (uint j=0; j<innerCoEdges.size(); j++)
            		std::cout<<" "<<innerCoEdges[j]->getName();
            	std::cout<<std::endl;
#endif

	return innerCoEdges;
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::CoFace*> CommandModificationTopo::
getInnerCoFaces(std::vector<Topo::CoFace*>& cofaces, std::vector<CoEdge*>& coedges)
{
#ifdef _DEBUG_MAJTOPO
    std::cout<<"getInnerCoFaces :"<<std::endl;
    std::cout<<" cofaces :";
    for (uint i=0; i<cofaces.size(); i++)
        std::cout<<" "<<cofaces[i]->getName();
    std::cout<<std::endl;
    std::cout<<" coedges :";
    for (uint i=0; i<coedges.size(); i++)
        std::cout<<" "<<coedges[i]->getName();
    std::cout<<std::endl;
#endif

    // les arêtes sont marquées à:
    // 1 <-> au bord
    // 2 <-> connectée à une coface sélectionnée
    // 3 <-> connectée à deux cofaces sélectionnées
    std::map<CoEdge*, uint> filtre_coedges;

    // les faces sont marquées à:
    // 1 <-> acceptables
    // 2 <-> sélectionnées dans sous-ensemble
    std::map<CoFace*, uint> filtre_cofaces;

    // on marque les arêtes au bord
    for (uint i=0; i<coedges.size(); i++)
        filtre_coedges[coedges[i]] = 1;

    // on marque les cofaces auxquelles on se limite
    for (uint i=0; i<cofaces.size(); i++)
        filtre_cofaces[cofaces[i]] = 1;

    // le sous-ensemble que l'on cherche à constituer
    std::vector<Topo::CoFace*> innerCoFaces;

    if (coedges.empty())
    	return innerCoFaces;

    // on part d'une coface de la liste
    // et on cherche un ensemble avec uniquement des cofaces reliées au bord ou entre elles
    for (uint i=0; i<cofaces.size(); i++)
        if (filtre_cofaces[cofaces[i]] == 1) {
            std::vector<Topo::CoEdge*> innerCoEdges;
            // on compte le nombre de coedges interne pour lesquelles il y a bien deux cofaces internes
            uint nbInnerCoEdgesMarqued = 0;

            CoFace* coface = cofaces[i];
            innerCoFaces.push_back(coface);
            filtre_cofaces[coface] = 2;
            //std::cout<<"coface = "<<coface->getName()<<std::endl;

            // analyse des arêtes de la face: au bord ou interne
            std::vector<Topo::CoEdge*> loc_coedges;
            coface->getCoEdges(loc_coedges);
            for (uint j=0; j<loc_coedges.size(); j++)
                if (filtre_coedges[loc_coedges[j]]==0){
                    // cas d'une arête interne nouvelle (reliée à une seule coface)
                    innerCoEdges.push_back(loc_coedges[j]);
                    filtre_coedges[loc_coedges[j]] = 2;
                } else if (filtre_coedges[loc_coedges[j]]==2){
                    // cas d'une arête interne reliée à 2 cofaces
                    filtre_coedges[loc_coedges[j]] = 3;
                    nbInnerCoEdgesMarqued += 1;
                }
            //std::cout<<" nbInnerCoEdgesMarqued = "<<nbInnerCoEdgesMarqued<<std::endl;
            //std::cout<<" innerCoEdges.size() = "<<innerCoEdges.size()<<std::endl;

            // boucle tant qu'il y a des arêtes internes reliées à une seule face interne
            do {
                // détection de blocage, cas de faces à l'extérieur du groupe de coedges
                bool bloque = !innerCoEdges.empty();
                for (uint k=0; k<innerCoEdges.size(); k++)
                    if (filtre_coedges[innerCoEdges[k]] == 2){
                        //std::cout<<"innerCoEdges[k] (marque à 2) = "<<innerCoEdges[k]->getName()<<std::endl;
                        // recherche d'une coface marquée à 1
                        std::vector<Topo::CoFace*> loc_cofaces;
                        innerCoEdges[k]->getCoFaces(loc_cofaces);

                        for (uint l=0; l<loc_cofaces.size(); l++)
                            if (filtre_cofaces[loc_cofaces[l]] == 1){
                                bloque = false;

                                CoFace* coface = loc_cofaces[l];
                                //std::cout<<"coface = "<<coface->getName()<<std::endl;
                                innerCoFaces.push_back(coface);
                                filtre_cofaces[coface] = 2;

                                // analyse des arêtes de la face: au bord ou interne
                                std::vector<Topo::CoEdge*> loc_coedges;
                                coface->getCoEdges(loc_coedges);
                                for (uint j=0; j<loc_coedges.size(); j++)
                                    if (filtre_coedges[loc_coedges[j]]==0){
                                        // cas d'une arête interne nouvelle (reliée à une seule coface)
                                        innerCoEdges.push_back(loc_coedges[j]);
                                        filtre_coedges[loc_coedges[j]] = 2;
                                    } else if (filtre_coedges[loc_coedges[j]]==2){
                                        // cas d'une arête interne reliée à 2 cofaces
                                        filtre_coedges[loc_coedges[j]] = 3;
                                        nbInnerCoEdgesMarqued += 1;
                                    }
                                //std::cout<<" nbInnerCoEdgesMarqued = "<<nbInnerCoEdgesMarqued<<std::endl;
                                //std::cout<<" innerCoEdges.size() = "<<innerCoEdges.size()<<std::endl;
                           }
                    }

                if (bloque){
                    //std::cout<<"on est bloqué ..."<<std::endl;
                    // on recommence
                    innerCoEdges.clear();
                    innerCoFaces.clear();
                    nbInnerCoEdgesMarqued = 0;
                }

            } while (innerCoEdges.size() != nbInnerCoEdgesMarqued);

            // cas où l'on sort avec quelque chose
            if (!innerCoFaces.empty()){
#ifdef _DEBUG_MAJTOPO
            	std::cout<<"  getInnerCoFaces => ";
            	for (uint j=0; j<innerCoFaces.size(); j++)
            		std::cout<<" "<<innerCoFaces[j]->getName();
            	std::cout<<std::endl;
#endif

                return innerCoFaces;
            }

    } // end for i<cofaces.size()

#ifdef _DEBUG_MAJTOPO
    std::cout<<"  getInnerCoFaces => rien"<<std::endl;
#endif
    return innerCoFaces; // vide ...
}
/*----------------------------------------------------------------------------*/
std::vector<Topo::Block*> CommandModificationTopo::getInnerBlocks(std::vector<Topo::Block*>& blocks,
		std::vector<CoFace*>& cofaces)
{
#ifdef _DEBUG_MAJTOPO
    std::cout<<"getInnerBlocks :"<<std::endl;
    std::cout<<" blocks :";
    for (uint i=0; i<blocks.size(); i++)
        std::cout<<" "<<blocks[i]->getName();
    std::cout<<std::endl;
    std::cout<<" cofaces :";
    for (uint i=0; i<cofaces.size(); i++)
        std::cout<<" "<<cofaces[i]->getName();
    std::cout<<std::endl;
#endif

    // retranscription de getInnerCoFaces, pour la dimension supérieur

    // les faces sont marquées à:
    // 1 <-> au bord
    // 2 <-> connectée à un bloc sélectionné
    // 3 <-> connectée à deux blocs sélectionnés
    std::map<CoFace*, uint> filtre_cofaces;

    // les blocs sont marqués à:
    // 1 <-> acceptables
    // 2 <-> sélectionnées dans sous-ensemble
    std::map<Block*, uint> filtre_blocks;

    // on marque les faces au bord
    for (uint i=0; i<cofaces.size(); i++)
    	filtre_cofaces[cofaces[i]] = 1;

    // on marque les blocs auxquels on se limite
    for (uint i=0; i<blocks.size(); i++)
    	filtre_blocks[blocks[i]] = 1;

    // le sous-ensemble que l'on cherche à constituer
    std::vector<Topo::Block*> innerBlocks;

    // on part d'un bloc de la liste
    // et on cherche un ensemble avec uniquement des blocs reliés au bord ou entre eux
    for (uint i=0; i<blocks.size(); i++)
        if (filtre_blocks[blocks[i]] == 1) {
            std::vector<Topo::CoFace*> innerCoFaces;
            // on compte le nombre de cofaces internes pour lesquelles il y a bien deux blocs internes
            uint nbInnerCoFacesMarqued = 0;

            Block* bloc = blocks[i];
            innerBlocks.push_back(bloc);
            filtre_blocks[bloc] = 2;
            //std::cout<<"bloc = "<<bloc->getName()<<std::endl;

            // analyse des cofaces du bloc: au bord ou interne
            std::vector<Topo::CoFace*> loc_cofaces;
            bloc->getCoFaces(loc_cofaces);
            for (uint j=0; j<loc_cofaces.size(); j++)
                if (filtre_cofaces[loc_cofaces[j]]==0){
                    // cas d'une face interne nouvelle (reliée à un seul bloc)
                    innerCoFaces.push_back(loc_cofaces[j]);
                    filtre_cofaces[loc_cofaces[j]] = 2;
                } else if (filtre_cofaces[loc_cofaces[j]]==2){
                    // cas d'une face interne reliée à 2 blocs
                	filtre_cofaces[loc_cofaces[j]] = 3;
                    nbInnerCoFacesMarqued += 1;
                }
            //std::cout<<" nbInnerCoFacesMarqued = "<<nbInnerCoFacesMarqued<<std::endl;
            //std::cout<<" innerCoFaces.size() = "<<innerCoFaces.size()<<std::endl;

            // boucle tant qu'il y a des arêtes internes reliées à une seule face interne
            do {
                // détection de blocage, cas de blocs à l'extérieur du groupe de cofaces
                bool bloque = !innerCoFaces.empty();
                for (uint k=0; k<innerCoFaces.size(); k++)
                    if (filtre_cofaces[innerCoFaces[k]] == 2){
                        //std::cout<<"innerCoFaces[k] (marque à 2) = "<<innerCoFaces[k]->getName()<<std::endl;
                        // recherche d'un bloc marqué à 1
                        std::vector<Topo::Block*> loc_blocks;
                        innerCoFaces[k]->getBlocks(loc_blocks);

                        for (uint l=0; l<loc_blocks.size(); l++)
                            if (filtre_blocks[loc_blocks[l]] == 1){
                                bloque = false;

                                Block* bloc = loc_blocks[l];
                                //std::cout<<"bloc = "<<bloc->getName()<<std::endl;
                                innerBlocks.push_back(bloc);
                                filtre_blocks[bloc] = 2;

                                // analyse des faces du bloc: au bord ou interne
                                std::vector<Topo::CoFace*> loc_cofaces;
                                bloc->getCoFaces(loc_cofaces);
                                for (uint j=0; j<loc_cofaces.size(); j++)
                                    if (filtre_cofaces[loc_cofaces[j]]==0){
                                        // cas d'une face interne nouvelle (reliée à un seul bloc)
                                        innerCoFaces.push_back(loc_cofaces[j]);
                                        filtre_cofaces[loc_cofaces[j]] = 2;
                                    } else if (filtre_cofaces[loc_cofaces[j]]==2){
                                        // cas d'une face interne reliée à 2 blocs
                                    	filtre_cofaces[loc_cofaces[j]] = 3;
                                        nbInnerCoFacesMarqued += 1;
                                    }
                                //std::cout<<" nbInnerCoFacesMarqued = "<<nbInnerCoFacesMarqued<<std::endl;
                                //std::cout<<" innerCoFaces.size() = "<<innerCoFaces.size()<<std::endl;
                           }
                    }

                if (bloque){
                    //std::cout<<"on est bloqué ..."<<std::endl;
                    // on recommence
                	innerCoFaces.clear();
                    innerBlocks.clear();
                    nbInnerCoFacesMarqued = 0;
                }

            } while (innerCoFaces.size() != nbInnerCoFacesMarqued);

            // cas où l'on sort avec quelque chose
            if (!innerBlocks.empty()){
                //std::cout<<"on a quelque chose ... de taille "<<innerBlocks.size()<<std::endl;
#ifdef _DEBUG_MAJTOPO
            	std::cout<<"  getInnerBlocks => ";
            	for (uint j=0; j<innerBlocks.size(); j++)
            		std::cout<<" "<<innerBlocks[j]->getName();
            	std::cout<<std::endl;
#endif

                return innerBlocks;
            }

    } // end for i<cofaces.size()

#ifdef _DEBUG_MAJTOPO
    std::cout<<"  getInnerBlocks => rien"<<std::endl;
#endif
	 return innerBlocks;
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::merge(CoFace* cf1, CoFace* cf2)
{
#ifdef _DEBUG_MAJTOPO
    std::cout<<"CommandModificationTopo::merge avec : "<<std::endl;
    std::cout<<"cf1 : "<<*cf1;
    std::cout<<"cf2 : "<<*cf2;
#endif

    // nombre debras par face et par direction
    uint nbI1 = 0, nbJ1 = 0, nbI2 = 0, nbJ2 = 0;
    if(cf1->isStructured())
    	cf1->getNbMeshingEdges(nbI1, nbJ1);
    if(cf2->isStructured())
     	cf2->getNbMeshingEdges(nbI2, nbJ2);

    if (!cf1->isStructured() && cf2->isStructured() || nbI1*nbJ1 < nbI2*nbJ2){
#ifdef _DEBUG_MAJTOPO
        std::cout<<"CoFace::merge, "<<cf1->getName()<<" est remplacée par "<<cf2->getName()<<std::endl;
#endif
        // récupération de l'association si possible et nécessaire
        if (cf1->getGeomAssociation() && !cf1->getGeomAssociation()->isDestroyed()
                && (!cf2->getGeomAssociation() || (cf2->getGeomAssociation() && cf2->getGeomAssociation()->isDestroyed() ))){
        	//getInfoCommand().addTopoInfoEntity(cf2, Internal::InfoCommand::DISPMODIFIED);
        	cf2->saveCoFaceTopoProperty(&getInfoCommand());
            cf2->saveTopoProperty();
            cf2->setGeomAssociation(cf1->getGeomAssociation());
#ifdef _DEBUG_MAJTOPO
            std::cout<<"Récupération de l'association vers "<<cf1->getGeomAssociation()->getName()<<std::endl;
#endif
        }

        // mise à jour des ratios s'il y en a
        if (cf1->isStructured() && cf2->isStructured())
        	MAJRatios(cf1, cf2, nbI1, nbJ1, nbI2, nbJ2);

        // fusion des cofaces
        cf2->merge(cf1, &getInfoCommand());
    }
    else {
#ifdef _DEBUG_MAJTOPO
        std::cout<<"CoFace::merge, "<<cf2->getName()<<" est remplacée par "<<cf1->getName()<<std::endl;
#endif
        // récupération de l'association si possible et nécessaire
        if (cf2->getGeomAssociation() && !cf2->getGeomAssociation()->isDestroyed()
                && (!cf1->getGeomAssociation() || (cf1->getGeomAssociation() && cf1->getGeomAssociation()->isDestroyed() ))){
        	//getInfoCommand().addTopoInfoEntity(cf1, Internal::InfoCommand::DISPMODIFIED);
        	cf1->saveCoFaceTopoProperty(&getInfoCommand());
        	cf1->saveTopoProperty();
            cf1->setGeomAssociation(cf2->getGeomAssociation());
#ifdef _DEBUG_MAJTOPO
            std::cout<<"Récupération de l'association vers "<<cf2->getGeomAssociation()->getName()<<std::endl;
#endif
        }

        // mise à jour des ratios s'il y en a
        if (cf1->isStructured() && cf2->isStructured())
        	MAJRatios(cf2, cf1, nbI2, nbJ2, nbI1, nbJ1);

        // fusion des cofaces
        cf1->merge(cf2, &getInfoCommand());
    }
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::MAJRatios(CoFace* cf1, CoFace* cf2, uint nbI1, uint nbJ1, uint nbI2, uint nbJ2)
{
    Face* face1 = 0;
    Face* face2 = 0;
    if (1 == cf1->getNbFaces())
    	face1 = cf1->getFace(0);
    if (1 == cf2->getNbFaces())
    	face2 = cf2->getFace(0);

    if (face1 && face2){
    	// il faut trouver les correspondances au niveau des directions entre les 2 cofaces

    	// les 2 sommets pour la direction I de la coface supprimée
    	Vertex* vtx1 = cf1->getVertex(1);
    	Vertex* vtx2 = cf1->getVertex(2);
    	CoFace::eDirOnCoFace dir1 = cf2->getDir(vtx1, vtx2);
    	bool sameDir = (dir1 == CoFace::i_dir);
    	if (sameDir){
    		uint ratioI = nbI2/nbI1;
    		if (ratioI>1){
#ifdef _DEBUG_MAJTOPO
    			std::cout<<" cas avec direction équivalente, ratioI = "<<ratioI<<std::endl;
#endif
    			face1->saveFaceMeshingProperty(&getInfoCommand());
    			face1->setRatio(cf2, ratioI, CoFace::i_dir);
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            	message<<"Attribution d'un ratio entre les faces "<<cf1->getName()<<" et "<<cf2->getName();
            	getContext().getLogStream()->log(TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));
    		}
    		uint ratioJ = nbJ2/nbJ1;
    		if (ratioJ>1){
#ifdef _DEBUG_MAJTOPO
    			std::cout<<" cas avec direction équivalente, ratioJ = "<<ratioJ<<std::endl;
#endif
    			face1->saveFaceMeshingProperty(&getInfoCommand());
    			face1->setRatio(cf2, ratioJ, CoFace::j_dir);
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            	message<<"Attribution d'un ratio entre les faces "<<cf1->getName()<<" et "<<cf2->getName();
            	getContext().getLogStream()->log(TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));
    		}
    	}
    	else {
    		uint ratioI = nbI2/nbJ1;
    		if (ratioI>1){
#ifdef _DEBUG_MAJTOPO
    			std::cout<<" cas avec direction inversée, ratioI = "<<ratioI<<std::endl;
#endif
    			face1->saveFaceMeshingProperty(&getInfoCommand());
    			face1->setRatio(cf2, ratioI, CoFace::i_dir);
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            	message<<"Attribution d'un ratio entre les faces "<<cf1->getName()<<" et "<<cf2->getName();
            	getContext().getLogStream()->log(TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));
    		}
    		uint ratioJ = nbJ2/nbI1;
    		if (ratioJ>1){
#ifdef _DEBUG_MAJTOPO
    			std::cout<<" cas avec direction inversée, ratioJ = "<<ratioJ<<std::endl;
#endif
    			face1->saveFaceMeshingProperty(&getInfoCommand());
    			face1->setRatio(cf2, ratioJ, CoFace::j_dir);
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            	message<<"Attribution d'un ratio entre les faces "<<cf1->getName()<<" et "<<cf2->getName();
            	getContext().getLogStream()->log(TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));
    		}
    	}
    } // end if (face1 && face2)
#ifdef _DEBUG_MAJTOPO
    else
    	std::cout<<" cas sans face1 && face2 !!!"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::associateInnerCoFaces(std::vector<Geom::GeomEntity*>& geoms,
    		std::vector<Topo::Block*>& blocks)
{
#ifdef _DEBUG_MAJTOPO
    std::cout<<"associateInnerCoFaces :"<<std::endl;
    std::cout<<" geoms :";
    for (uint i=0; i<geoms.size(); i++)
        std::cout<<" "<<geoms[i]->getName();
    std::cout<<std::endl;
    std::cout<<" blocks :";
    for (uint i=0; i<blocks.size(); i++)
        std::cout<<" "<<blocks[i]->getName();
    std::cout<<std::endl;
#endif

    std::map<Geom::Surface*, uint> filtre_surface;
    std::map<Topo::CoFace*, uint> filtre_cofaces;

    for (uint k=0; k<geoms.size(); k++){
    	std::vector<Geom::Surface*> surfaces;
    	geoms[k]->get(surfaces);

    	for (uint j=0; j<surfaces.size(); ++j)
    		filtre_surface[surfaces[j]] += 1;
    }

    for (uint k=0; k<blocks.size(); k++){
    	std::vector<Topo::CoFace*> cofaces;
    	blocks[k]->getCoFaces(cofaces);

    	for (uint j=0; j<cofaces.size(); ++j)
    		filtre_cofaces[cofaces[j]] += 1;
    }

    std::vector<Topo::CoFace*> inner_cofaces;
    for (std::map<Topo::CoFace*, uint>::iterator iter = filtre_cofaces.begin();
    		iter != filtre_cofaces.end(); ++iter)
    	if (iter->second >= 2)
    		inner_cofaces.push_back(iter->first);


    // on ne s'intéresse qu'aux surfaces internes, donc celles vues au moins 2 fois
    for (std::map<Geom::Surface*, uint>::iterator iter = filtre_surface.begin();
    		iter != filtre_surface.end(); ++iter)
    	if (iter->second >= 2 && iter->first->getRefTopo().empty()){
    		Geom::Surface* surface = iter->first;

    		std::vector<Topo::CoEdge*> coedges = getCoEdges(surface);
    		std::vector<Topo::CoFace*> coface1_surf = getInnerCoFaces(inner_cofaces, coedges);

    		for (uint j=0; j<coface1_surf.size(); j++){
    			getInfoCommand().addTopoInfoEntity(coface1_surf[j], Internal::InfoCommand::DISPMODIFIED);
    			coface1_surf[j]->saveTopoProperty();
    			coface1_surf[j]->setGeomAssociation(surface);
    			// TODO [EB] cf pour les CoEdge et Vertex internes
    		}

    } // end for iter = filtre_surface.begin()
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::associateInnerCoEdges(std::vector<Topo::CoFace*>& cofaces,
		Geom::GeomEntity* ge)
{
#ifdef _DEBUG_MAJTOPO
    std::cout<<"associateInnerCoEdges :"<<std::endl;
    std::cout<<" cofaces :";
    for (uint i=0; i<cofaces.size(); i++)
        std::cout<<" "<<cofaces[i]->getName();
    std::cout<<std::endl;
    std::cout<<" ge :"<<ge->getName()<<std::endl;
#endif

    std::map<Topo::CoEdge*, uint> filtre_coedges;

    for (uint k=0; k<cofaces.size(); k++){
    	std::vector<Topo::CoEdge*> coedges;
    	cofaces[k]->getCoEdges(coedges);

    	for (uint j=0; j<coedges.size(); ++j)
    		filtre_coedges[coedges[j]] += 1;
    }

    for (std::map<Topo::CoEdge*, uint>::iterator iter = filtre_coedges.begin();
    		iter != filtre_coedges.end(); ++iter)
    	if (iter->second >= 2){
    		Topo::CoEdge* coedge = iter->first;
    		getInfoCommand().addTopoInfoEntity(coedge, Internal::InfoCommand::DISPMODIFIED);
    		coedge->saveTopoProperty();
    		coedge->setGeomAssociation(ge);
    	}
}
/*----------------------------------------------------------------------------*/
void CommandModificationTopo::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewCoedges(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
