/*----------------------------------------------------------------------------*/
/*
 * \file CommandMeshExplorer.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7 déc. 2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/CommandMeshExplorer.h"
#include "Group/GroupManager.h"
#include "Geom/Volume.h"
#include "Topo/TopoEntity.h"
#include "Topo/Block.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
#include "Group/Group3D.h"
#include "Internal/InfoCommand.h"
#include "Topo/TopoHelper.h"
#include <GMDS/IG/IGMesh.h>
#include "Mesh/MeshItf.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/ReferencedMutex.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandMeshExplorer::
CommandMeshExplorer(Internal::Context& c, CommandMeshExplorer* oldExplo, int inc, Topo::CoEdge* arete)
: CommandInternal(c, "Exploratation du maillage")
, m_explorer_id(c.newExplorerId())
, m_old_explorer(oldExplo)
, m_edge(arete)
{
//    std::cout<<"Construction de CommandMeshExplorer "<<getUniqueName()<<std::endl;
    if (arete == 0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Exploration impossible sans arête", TkUtil::Charset::UTF_8));

    if (arete->isDestroyed())
        throw TkUtil::Exception (TkUtil::UTF8String ("Exploration interdite avec arête détruite", TkUtil::Charset::UTF_8));

    if (m_old_explorer){
        m_pos = m_old_explorer->m_pos + inc;
        // on se bloque sur les extrémités
        if (m_pos < 1)
            m_pos = 1;
        else if (m_pos>=arete->getNbMeshingEdges())
            m_pos = arete->getNbMeshingEdges();
    }
    else
        // on se place au milieu par défaut (pour commencer)
        m_pos = (arete->getNbMeshingEdges()+1)/2;

}
/*----------------------------------------------------------------------------*/
CommandMeshExplorer::
CommandMeshExplorer(Internal::Context& c, CommandMeshExplorer* oldExplo)
: CommandInternal(c, "Fin de l'exploratation du maillage")
, m_explorer_id(0)
, m_old_explorer(oldExplo)
, m_pos(0)
, m_edge(0)
{
//    std::cout<<"Construction de CommandMeshExplorer (final) "<<getUniqueName()<<std::endl;
    if (oldExplo == 0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Terminaison de l'exploration impossible sans lien sur l'explorateur précédent", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandMeshExplorer::
~CommandMeshExplorer()
{
//    std::cout<<"Destruction de CommandMeshExplorer "<<getUniqueName()<<std::endl;
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_EXPLORER
void CommandMeshExplorer::
internalExecute()
{
#ifdef _DEBUG_EXPLORER
    std::cout<<"CommandMeshExplorer::internalExecute()"<<std::endl;
#endif
    // destruction de la construction de l'exploration précédente
    if (m_old_explorer)
        deleteOldSubVolumes();

    // cas de la fin de l'exploration
    if (m_edge == 0)
        return;

    // on marque les blocs et les faces communes internes aux groupes
    // on met à 1 ce qui est maillé conforme et dans le volume
    // on laisse à 0 le reste
    std::map<Topo::Block*, uint> filtre_block;
    std::map<Topo::CoFace*, uint> filtre_coface;

    selectCoFaceAndBlocks(filtre_coface, filtre_block);


    // pour ne prendre qu'une fois une arête, on les marque suivant la position de la coupe
    std::map<Topo::CoEdge*, uint> filtre_coedge;

    computePosCoEdge(filtre_coface, filtre_coedge);


    // on parcours les blocs et on cherche la direction pour laquelle une arête est marquée
    // on en profitte pour calculer la position dans le bloc
    std::vector<BlockDirPos> bloc_dirPos;

    computePosBlock(filtre_block, filtre_coedge, bloc_dirPos);


    // on créé un sous-groupe par groupe 3D visible
    createSubVolume(bloc_dirPos);

}
/*----------------------------------------------------------------------------*/
void CommandMeshExplorer::
internalUndo()
{
    TkUtil::AutoReferencedMutex autoMutex (getMutex ( ));

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();
}
/*----------------------------------------------------------------------------*/
void CommandMeshExplorer::
internalRedo()
{
    TkUtil::AutoReferencedMutex autoMutex (getMutex ( ));

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();
}
/*----------------------------------------------------------------------------*/
void CommandMeshExplorer::
deleteOldSubVolumes()
{
    Internal::InfoCommand& icmd = m_old_explorer->getInfoCommand();

    for (uint i=0; i<icmd.getNbMeshInfoEntity(); i++){
        Mesh::MeshEntity* entity;
        Internal::InfoCommand::type t;
        icmd.getMeshInfoEntity(i, entity, t);
        if (Internal::InfoCommand::CREATED == t)
            getInfoCommand().addMeshInfoEntity(entity, Internal::InfoCommand::DELETED);
    }
}
/*----------------------------------------------------------------------------*/
void CommandMeshExplorer::
selectCoFaceAndBlocks(std::map<Topo::CoFace*, uint>& filtre_coface,
        std::map<Topo::Block*, uint>& filtre_block)
{
    // tous les groupes 3D
    std::vector<Group::Group3D*> grp;
    getContext().getLocalGroupManager().getGroup3D(grp);

    // le nombre de blocs maillés en structuré
    uint nb_blocks_marked = 0;

    for (uint ig=0; ig<grp.size(); ig++){
    	std::vector<Geom::Volume*>& volumes = grp[ig]->getVolumes();
    	for (uint i=0; i<volumes.size(); i++){
    		std::vector<Topo::TopoEntity* >  topos = volumes[i]->getRefTopo();
    		for (uint j=0; j<topos.size(); j++){
    			if (topos[j]->getDim() == 3){
    				Topo::TopoEntity* te = topos[j];
    				Topo::Block* blk = dynamic_cast<Topo::Block*>(te);
    				if (blk == 0)
    					throw TkUtil::Exception (TkUtil::UTF8String ("CommandMeshExplorer a échoué pour récupérer un bloc", TkUtil::Charset::UTF_8));

    				if (blk->isStructured()){ // && blk->isMeshed()
    					filtre_block[blk] = 1;
    					nb_blocks_marked += 1;
#ifdef _DEBUG_EXPLORER
    					std::cout<<"  bloc pris en compte : "<<blk->getName()<<std::endl;
#endif

    					std::vector<Topo::CoFace* > cofaces;

    					blk->getCoFaces(cofaces);

    					for (std::vector<Topo::CoFace* >::iterator iter1 = cofaces.begin();
    							iter1 != cofaces.end(); ++iter1){
    						filtre_coface[*iter1] = 1;
    						//std::cout<<"filtre_coface à 1 pour "<<(*iter1)->getName()<<std::endl;
    					}

    				} // end if (blk->isStructured() && blk->isMeshed())
    			} // end if (topos[j]->getDim() == 3)
    		} // end for j
    	} // end for i

    	std::vector<Topo::Block*>& blocks = grp[ig]->getBlocks();
    	for (uint j=0; j<blocks.size(); j++){
    		Topo::Block* blk = blocks[j];

    		if (blk->isStructured()){ // && blk->isMeshed()
    			filtre_block[blk] = 1;
    			nb_blocks_marked += 1;
#ifdef _DEBUG_EXPLORER
    			std::cout<<"  bloc pris en compte : "<<blk->getName()<<std::endl;
#endif

    			std::vector<Topo::CoFace* > cofaces;

    			blk->getCoFaces(cofaces);

    			for (std::vector<Topo::CoFace* >::iterator iter1 = cofaces.begin();
    					iter1 != cofaces.end(); ++iter1){
    				filtre_coface[*iter1] = 1;
    				//std::cout<<"filtre_coface à 1 pour "<<(*iter1)->getName()<<std::endl;
    			}

    		} // end if (blk->isStructured() && blk->isMeshed())
    	} // end for j
    } // end for ig

    if (nb_blocks_marked == 0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"Il n'y a aucun bloc maillé et structuré de rencontré";
        log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
    }
}
/*----------------------------------------------------------------------------*/
void CommandMeshExplorer::
computePosCoEdge(std::map<Topo::CoFace*, uint>& filtre_coface,
            std::map<Topo::CoEdge*, uint> &filtre_coedge)
{
    // on initialise la recherche avec l'arête de départ
    std::list<Topo::CoEdge*> coedges_reserve;
    coedges_reserve.push_back(m_edge);

    // on en profite pour mémoriser la position
    filtre_coedge[m_edge] = m_pos;

    // boucle sur la reserve d'arêtes en observation
    do {
        // on prend la dernière arête
        Topo::CoEdge* coedge_dep = coedges_reserve.back();
        coedges_reserve.pop_back();
        uint pos = filtre_coedge[coedge_dep];

#ifdef _DEBUG_EXPLORER
        std::cout<<"### coedge : "<<coedge_dep->getName()<<", pos : "<<pos<<std::endl;
#endif
        std::vector<Topo::CoFace* > cofaces;
        coedge_dep->getCoFaces(cofaces);

        for (std::vector<Topo::CoFace* >::iterator iter1 = cofaces.begin();
                iter1 != cofaces.end(); ++iter1){
            Topo::CoFace* coface = *iter1;

            //std::cout<<"observation de "<<(*iter1)->getName()<<std::endl;

            if (filtre_coface[coface] == 1){
                filtre_coface[coface] = 2;

                // recherche de l'arête opposée et de la position pour la coupe

                // recherche du côté dans lequel est cette arête, et le nombre de bras de maillage jusqu'à la coupe
                Topo::Edge* edge_dep = coface->getEdgeContaining(coedge_dep);
                //std::cout<<"edge_dep : "<<*edge_dep;
                uint ind_edge_dep = coface->getIndex(edge_dep);


                // direction de la face qui est coupée
                Topo::CoFace::eDirOnCoFace dirCoFaceSplit = (ind_edge_dep%2?Topo::CoFace::i_dir:Topo::CoFace::j_dir);

                Topo::Vertex* vertex1;
                Topo::Vertex* vertex2;
                if (dirCoFaceSplit == Topo::CoFace::i_dir){
                    vertex1 = coface->getVertex(1);
                    vertex2 = coface->getVertex(0);
                } else {
                    vertex1 = coface->getVertex(1);
                    vertex2 = coface->getVertex(2);
                }

                if (ind_edge_dep>1){
                    Topo::Vertex* vertex_tmp = vertex1;
                    vertex1 = vertex2;
                    vertex2 = vertex_tmp;
                }

#ifdef _DEBUG_EXPLORER
                std::cout<<"  vertex1 = "<<vertex1->getName()<<std::endl;
                std::cout<<"  vertex2 = "<<vertex2->getName()<<std::endl;
#endif
                // calcul le nombre de bras entre le sommet de départ et un noeud sur une CoEdge
                uint nbMeshingEdges_edge;
                bool sens;
                edge_dep->computeCorrespondingNbMeshingEdges(vertex1, coedge_dep, pos,
                        nbMeshingEdges_edge, sens);
#ifdef _DEBUG_EXPLORER
                std::cout<<"  nbMeshingEdges_edge (calculé) = "<<nbMeshingEdges_edge<<std::endl;
#endif
                // on cherche une arête et non pas un noeud ...
                bool inv_dep = !edge_dep->isSameSense(vertex1, coedge_dep);
                if (inv_dep)
                    nbMeshingEdges_edge += 1;

#ifdef _DEBUG_EXPLORER
                std::cout<<"  nbMeshingEdges_edge (rectifié) = "<<nbMeshingEdges_edge<<std::endl;
#endif

                Topo::Edge* edge_ar = 0;
                // si on est sur le côté face à la dégénérescence, on met à 0 l'arête d'arrivée
                if (coface->getNbEdges() == 4){
                    uint ind_edge_ar = (ind_edge_dep+2)%4;
                    edge_ar = coface->getEdge(ind_edge_ar);
                } else {
                    if (ind_edge_dep == 0)
                        edge_ar = coface->getEdge(2);
                    else if (ind_edge_dep == 2)
                        edge_ar = coface->getEdge(0);
                    else
                        edge_ar = 0;
                }

                // réciproque: recherche la CoEdge touchée par le noeud
                if (0 != edge_ar){
                    //std::cout<<"edge_ar : "<<*edge_ar;
                    Topo::CoEdge* coedge_ar;
                    uint nbMeshingEdges_ar = 0;
                    bool sens;
                    edge_ar->computeCorrespondingCoEdgeAndNbMeshingEdges(vertex2, nbMeshingEdges_edge, false,
                            coedge_ar, nbMeshingEdges_ar, sens);
#ifdef _DEBUG_EXPLORER
                    std::cout<<"  nbMeshingEdges_ar (calculé) = "<<nbMeshingEdges_ar<<std::endl;
#endif

                    if (filtre_coedge[coedge_ar] == 0){

                        // on ajoute cette nouvelle arête commune
                        // recherche de la position dans l'arête commune

                        bool inv_ar  = !edge_ar->isSameSense(vertex2, coedge_ar);
#ifdef _DEBUG_EXPLORER
                        std::cout<<"inv_dep: "<<inv_dep<<", inv_ar: "<<inv_ar<<std::endl;
#endif
                        // encore cette histoire de bras et non de noeuds
                        if (inv_ar)
                            nbMeshingEdges_ar += 1;
#ifdef _DEBUG_EXPLORER
                    std::cout<<"  nbMeshingEdges_ar (rectifié) = "<<nbMeshingEdges_ar<<std::endl;
#endif

                        coedges_reserve.push_back(coedge_ar);
#ifdef _DEBUG_EXPLORER
                        std::cout<<" ajoute coedge "<<coedge_ar->getName()<<", pos "<<nbMeshingEdges_ar<<std::endl;
#endif

                        filtre_coedge[coedge_ar] = nbMeshingEdges_ar;
                    } // end if (filtre_coedge[coedge_ar] == 1)

                } // end if (0 != edge_ar)

            } // end if (filtre_coface[coface] == 1)

        } // end for iter1

    } while (!coedges_reserve.empty());
}
/*----------------------------------------------------------------------------*/
void CommandMeshExplorer::
computePosBlock(std::map<Topo::Block*, uint>& filtre_block,
        std::map<Topo::CoEdge*, uint> &filtre_coedge,
        std::vector<BlockDirPos>& bloc_dirPos)
{

    for (std::map<Topo::Block*, uint>::iterator iter = filtre_block.begin();
            iter != filtre_block.end(); ++iter){

        Topo::Block* bloc = iter->first;
        uint marque = iter->second;

        if (marque == 1) {
            Topo::Block::eDirOnBlock dir = Topo::Block::unknown;

            std::vector<Topo::CoEdge* > iCoedges[3];
            bloc->getOrientedCoEdges(iCoedges[0], iCoedges[1], iCoedges[2]);

            // recherche des ratios par arête dans le bloc
            std::map<Topo::CoEdge*,uint> ratios;
            bloc->getRatios(ratios);

            for (uint i=0; i<3; i++){
                uint pos_blk = 0;

                // on restreint le parcours des arêtes à une arête du bloc
                std::vector<Topo::CoEdge* > coedges_between;
                uint ind_vtx = 1 << i;
#ifdef _DEBUG_EXPLORER
                std::cout<<"getCoEdgesBetweenVertices entre "<<bloc->getVertex(0)->getName()
                        <<" et "<<bloc->getVertex(ind_vtx)->getName()<<" d'indice "<<ind_vtx<<std::endl;
#endif
                Topo::TopoHelper::getCoEdgesBetweenVertices(bloc->getVertex(0), bloc->getVertex(ind_vtx), iCoedges[i], coedges_between);

                Topo::Vertex* vtx = bloc->getVertex(0);
                for (std::vector<Topo::CoEdge* >::iterator iter3 = coedges_between.begin();
                        iter3 != coedges_between.end(); ++iter3){
                    Topo::CoEdge* coedge = *iter3;
                    if (filtre_coedge[coedge] == 0){
                        pos_blk += coedge->getNbMeshingEdges()/ratios[coedge];
                    }
                    else {
                        dir = (Topo::Block::eDirOnBlock)i;
                        uint ratio = ratios[coedge];
                        uint dec = ratio-1;
                        if (vtx == coedge->getVertex(0))
                            pos_blk += (filtre_coedge[coedge]+dec)/ratio;
                        else
                            pos_blk += (coedge->getNbMeshingEdges() - filtre_coedge[coedge] + 1 + dec)/ratio;

                        // on a la direction et la position dans le bloc
                        BlockDirPos bdp = {bloc, dir, pos_blk};
                        bloc_dirPos.push_back(bdp);

#ifdef _DEBUG_EXPLORER
                        std::cout<<" Bloc "<<bloc->getName()<<" coupé en pos "<<pos_blk<<" suivant dir "<<dir<<std::endl;
#endif
                    }

                    vtx = coedge->getOppositeVertex(vtx);

                } // end for iter
            } // end for i<3

#ifdef _DEBUG_EXPLORER
            if (dir == Topo::Block::unknown)
                std::cout<<" Bloc "<<bloc->getName()<<" non touché par la coupe"<<std::endl;
#endif
        } // end if (marque == 1)

    } // end for iter = filtre_block.begin();
}
/*----------------------------------------------------------------------------*/
void CommandMeshExplorer::
createSubVolume(std::vector<BlockDirPos>& bloc_dirPos)
{
#ifdef _DEBUG_EXPLORER
    std::cout<<"CommandMeshExplorer::createSubVolume()"<<std::endl;
#endif

    // map pour les sous-volumes créés
    std::map<std::string, Mesh::SubVolume*> corr_subVol;


    gmds::IGMesh& gmds_mesh = getContext().getLocalMeshManager().getMesh()->getGMDSMesh();


    // parcours les blocs et y récupère les noms des groupes
    for (std::vector<BlockDirPos>::iterator iter1 = bloc_dirPos.begin();
            iter1 != bloc_dirPos.end(); ++iter1){
        Topo::Block* blk = (*iter1).m_bloc;
        Topo::Block::eDirOnBlock dir = (*iter1).m_dir;
        uint pos = (*iter1).m_pos;
#ifdef _DEBUG_EXPLORER
        std::cout<<" blk "<<blk->getName()<<", dir "<<dir<<", pos "<<pos<<std::endl;
#endif

        if (pos == 0)
        	pos = 1;

        // on passe les blocs non maillés
        if (!blk->isMeshed())
        	continue;

        std::vector<std::string> groupsName;
        blk->getGroupsName(groupsName, true, true);

        for (std::vector<std::string>::iterator iter2 = groupsName.begin();
                iter2 != groupsName.end(); ++iter2){

//            TkUtil::UTF8String name (*iter2, Charset::UTF_8);
			TkUtil::UTF8String	name (*iter2, TkUtil::Charset::UTF_8);
            name << "_" <<  (long int)m_explorer_id;

            Mesh::SubVolume* sv = corr_subVol[name];
            if (sv == 0){
                sv = new Mesh::SubVolume(
                        getContext(),
                        getContext().newProperty(Utils::Entity::MeshSubVolume, name),
                        getContext().newDisplayProperties(Utils::Entity::MeshSubVolume),
                        0);
                getInfoCommand().addMeshInfoEntity(sv, Internal::InfoCommand::CREATED);
                getContext().newGraphicalRepresentation (*sv);

                // recherche le groupe 3D de base pour connaitre sa visibilité
                Group::Group3D* gr = getContext().getLocalGroupManager().getGroup3D(*iter2, true);
                sv->getDisplayProperties().setDisplayed(gr->isVisible());

                corr_subVol[name] = sv;
            }

#ifdef _DEBUG_EXPLORER
            std::cout<<"Mesh::SubVolume avec groupe "<<sv->getName()<<std::endl;
#endif
            uint nbBrasI, nbBrasJ, nbBrasK;
            blk->getNbMeshingEdges(nbBrasI, nbBrasJ, nbBrasK);

            if (dir == 0){
                for (uint j=0; j<nbBrasJ; j++)
                    for (uint k=0; k<nbBrasK; k++){
                        gmds::Region r = gmds_mesh.get<gmds::Region>(blk->regions()[pos-1+j*nbBrasI+k*nbBrasI*nbBrasJ]);
                        sv->addRegion(r);

                    }
            } else if (dir == 1) {
                for (uint i=0; i<nbBrasI; i++)
                    for (uint k=0; k<nbBrasK; k++){
                        gmds::Region r = gmds_mesh.get<gmds::Region>(blk->regions()[i+(pos-1)*nbBrasI+k*nbBrasI*nbBrasJ]);
                        sv->addRegion(r);
                    }
            } else if (dir == 2) {
                for (uint i=0; i<nbBrasI; i++)
                    for (uint j=0; j<nbBrasJ; j++){
                        gmds::Region r = gmds_mesh.get<gmds::Region>(blk->regions()[i+j*nbBrasI+(pos-1)*nbBrasI*nbBrasJ]);
                        sv->addRegion(r);
                    }
            }

        } // end for iter2

    } // end for iter1

}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
