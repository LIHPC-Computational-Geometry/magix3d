/*----------------------------------------------------------------------------*/
/*
 * \file CommandCreateSubVolumeBetweenSheets.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2/12/2016
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
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
#include <Mesh/CommandCreateSubVolumeBetweenSheets.h>

#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/ReferencedMutex.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandCreateSubVolumeBetweenSheets::
CommandCreateSubVolumeBetweenSheets(Internal::Context& c, std::vector<Topo::Block*>& blocks,
		Topo::CoEdge* arete, int& pos1, int& pos2, std::string& groupName)
: CommandInternal(c, "Création volume entre 2 feuillets")
, m_blocks(blocks)
, m_edge(arete)
, m_pos1(pos1)
, m_pos2(pos2)
, m_group_name(groupName)
{
//    std::cout<<"Construction de CommandCreateSubVolumeBetweenSheets "<<getUniqueName()<<std::endl;
    if (arete == 0)
        throw TkUtil::Exception (TkUtil::UTF8String ("CommandSheetsSubVolume impossible sans arête", TkUtil::Charset::UTF_8));

    if (arete->isDestroyed())
        throw TkUtil::Exception (TkUtil::UTF8String ("CommandSheetsSubVolume interdit avec arête détruite", TkUtil::Charset::UTF_8));

    if (m_blocks.empty())
    	throw TkUtil::Exception (TkUtil::UTF8String ("CommandSheetsSubVolume sans bloc", TkUtil::Charset::UTF_8));

    if (m_group_name.empty())
    	throw TkUtil::Exception (TkUtil::UTF8String ("CommandSheetsSubVolume sans nom de volume", TkUtil::Charset::UTF_8));

    if (pos1<1 || pos1>pos2 || pos2>arete->getNbMeshingEdges())
    	throw TkUtil::Exception (TkUtil::UTF8String ("Paramètres invalides, il faut que les positions soient ordonnées et entre 1 et nb bras", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
CommandCreateSubVolumeBetweenSheets::
~CommandCreateSubVolumeBetweenSheets()
{
//    std::cout<<"Destruction de CommandCreateSubVolumeBetweenSheets "<<getUniqueName()<<std::endl;
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_SHEETS
void CommandCreateSubVolumeBetweenSheets::
internalExecute()
{
#ifdef _DEBUG_SHEETS
    std::cout<<"CommandSheetsSubVolume::internalExecute()"<<std::endl;
#endif

    // on marque les faces communes associées aux blocs sélectionnés
    std::map<Topo::CoFace*, uint> filtre_coface;

    selectCoFaces(filtre_coface);


    // pour ne prendre qu'une fois une arête, on les marque suivant la position de la coupe
    std::map<Topo::CoEdge*, uint> filtre1_coedge;
    std::map<Topo::CoEdge*, uint> filtre2_coedge;

    computePosCoEdge(filtre_coface, filtre1_coedge, filtre2_coedge);


    // on parcours les blocs et on cherche la direction pour laquelle une arête est marquée
    // on en profitte pour calculer la position dans le bloc
    std::vector<BlockDirPos> bloc_dirPos;

    computePosBlock(filtre1_coedge, filtre2_coedge, bloc_dirPos);


    // on créé un sous-groupe par groupe 3D visible
    createSubVolume(bloc_dirPos);

}
/*----------------------------------------------------------------------------*/
void CommandCreateSubVolumeBetweenSheets::
internalUndo()
{
    TkUtil::AutoReferencedMutex autoMutex (getMutex ( ));

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();
}
/*----------------------------------------------------------------------------*/
void CommandCreateSubVolumeBetweenSheets::
internalRedo()
{
    TkUtil::AutoReferencedMutex autoMutex (getMutex ( ));

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();
}
/*----------------------------------------------------------------------------*/
void CommandCreateSubVolumeBetweenSheets::
selectCoFaces(std::map<Topo::CoFace*, uint>& filtre_coface)
{
	for (uint j=0; j<m_blocks.size(); j++){
		Topo::Block* blk = m_blocks[j];

		if (blk->isStructured() && blk->isMeshed()){

			std::vector<Topo::CoFace* > cofaces;

			blk->getCoFaces(cofaces);

			for (std::vector<Topo::CoFace* >::iterator iter1 = cofaces.begin();
					iter1 != cofaces.end(); ++iter1){
				filtre_coface[*iter1] = 1;
				//std::cout<<"filtre_coface à 1 pour "<<(*iter1)->getName()<<std::endl;
			}

		} // end if (blk->isStructured() && blk->isMeshed())
		else if (!blk->isStructured()){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message <<"Le bloc "<<blk->getName()
    				<<" n'est pas structuré";
			throw TkUtil::Exception(message);
		}
		else {
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message <<"Le bloc "<<blk->getName()
    			    <<" n'est pas maillé";
			throw TkUtil::Exception(message);
		}
	} // end for j
}
/*----------------------------------------------------------------------------*/
void CommandCreateSubVolumeBetweenSheets::
computePosCoEdge(std::map<Topo::CoFace*, uint>& filtre_coface,
            std::map<Topo::CoEdge*, uint> &filtre1_coedge,
            std::map<Topo::CoEdge*, uint> &filtre2_coedge)
{
    // on initialise la recherche avec l'arête de départ
    std::list<Topo::CoEdge*> coedges_reserve;
    coedges_reserve.push_back(m_edge);

    // on en profite pour mémoriser la position
    filtre1_coedge[m_edge] = m_pos1;
    filtre2_coedge[m_edge] = m_pos2;

    // boucle sur la reserve d'arêtes en observation
    do {
        // on prend la dernière arête
        Topo::CoEdge* coedge_dep = coedges_reserve.back();
        coedges_reserve.pop_back();
        uint pos1 = filtre1_coedge[coedge_dep];
        uint pos2 = filtre2_coedge[coedge_dep];

#ifdef _DEBUG_SHEETS
        std::cout<<"### coedge : "<<coedge_dep->getName()<<", pos1 : "<<pos1<<", pos2 : "<<pos2<<std::endl;
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

#ifdef _DEBUG_SHEETS
                std::cout<<"  vertex1 = "<<vertex1->getName()<<std::endl;
                std::cout<<"  vertex2 = "<<vertex2->getName()<<std::endl;
#endif
                // calcul le nombre de bras entre le sommet de départ et un noeud sur une CoEdge
                uint nbMeshingEdges1_edge, nbMeshingEdges2_edge;
                bool sens;
                edge_dep->computeCorrespondingNbMeshingEdges(vertex1, coedge_dep, pos1,
                        nbMeshingEdges1_edge, sens);
                edge_dep->computeCorrespondingNbMeshingEdges(vertex1, coedge_dep, pos2,
                        nbMeshingEdges2_edge, sens);
#ifdef _DEBUG_SHEETS
                std::cout<<"  nbMeshingEdges1_edge (calculé) = "<<nbMeshingEdges1_edge<<std::endl;
                std::cout<<"  nbMeshingEdges2_edge (calculé) = "<<nbMeshingEdges2_edge<<std::endl;
#endif
                // on cherche une arête et non pas un noeud ...
                bool inv_dep = !edge_dep->isSameSense(vertex1, coedge_dep);
                if (inv_dep){
                    nbMeshingEdges1_edge += 1;
                    nbMeshingEdges2_edge += 1;
                }
#ifdef _DEBUG_SHEETS
                std::cout<<"  nbMeshingEdges1_edge (rectifié) = "<<nbMeshingEdges1_edge<<std::endl;
                std::cout<<"  nbMeshingEdges2_edge (rectifié) = "<<nbMeshingEdges2_edge<<std::endl;
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
                    Topo::CoEdge* coedge1_ar;
                    Topo::CoEdge* coedge2_ar;
                    uint nbMeshingEdges1_ar = 0;
                    uint nbMeshingEdges2_ar = 0;
                    bool sens;
                    edge_ar->computeCorrespondingCoEdgeAndNbMeshingEdges(vertex2, nbMeshingEdges1_edge, false,
                            coedge1_ar, nbMeshingEdges1_ar, sens);
                    edge_ar->computeCorrespondingCoEdgeAndNbMeshingEdges(vertex2, nbMeshingEdges2_edge, false,
                            coedge2_ar, nbMeshingEdges2_ar, sens);
#ifdef _DEBUG_SHEETS
                    std::cout<<"  nbMeshingEdges1_ar (calculé) = "<<nbMeshingEdges1_ar<<std::endl;
                    std::cout<<"  nbMeshingEdges2_ar (calculé) = "<<nbMeshingEdges2_ar<<std::endl;
#endif

                    if (coedge1_ar != coedge2_ar){
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    	message <<"La création d'un sous-volume entre 2 feuillet ne peut se faire si les feuillets n'utilisent pas les mêmes arêtes "
                    			<<", ce qui est le cas dans la face "<<coface->getName()<<" avec les arêtes "
								<<coedge1_ar->getName()<<" et "<<coedge2_ar->getName();
                    	throw TkUtil::Exception(message);
                    }

                    if (filtre1_coedge[coedge1_ar] == 0){

                        // on ajoute cette nouvelle arête commune
                        // recherche de la position dans l'arête commune

                        bool inv_ar  = !edge_ar->isSameSense(vertex2, coedge1_ar);
#ifdef _DEBUG_SHEETS
                        std::cout<<"inv_dep: "<<inv_dep<<", inv_ar: "<<inv_ar<<std::endl;
#endif
                        // encore cette histoire de bras et non de noeuds
                        if (inv_ar){
                            nbMeshingEdges1_ar += 1;
                            nbMeshingEdges2_ar += 1;
                        }
#ifdef _DEBUG_SHEETS
                        std::cout<<"  nbMeshingEdges1_ar (rectifié) = "<<nbMeshingEdges1_ar<<std::endl;
                        std::cout<<"  nbMeshingEdges2_ar (rectifié) = "<<nbMeshingEdges2_ar<<std::endl;
#endif

                        coedges_reserve.push_back(coedge1_ar);
#ifdef _DEBUG_SHEETS
                        std::cout<<" ajoute coedge "<<coedge1_ar->getName()<<", pos "
                        		<<nbMeshingEdges1_ar<<" et "<<nbMeshingEdges2_ar<<std::endl;
#endif

                        filtre1_coedge[coedge1_ar] = nbMeshingEdges1_ar;
                        filtre2_coedge[coedge2_ar] = nbMeshingEdges2_ar;
                    } // end if (filtre1_coedge[coedge1_ar] == 0)

                } // end if (0 != edge_ar)

            } // end if (filtre_coface[coface] == 1)

        } // end for iter1

    } while (!coedges_reserve.empty());
}
/*----------------------------------------------------------------------------*/
void CommandCreateSubVolumeBetweenSheets::
computePosBlock(std::map<Topo::CoEdge*, uint> &filtre1_coedge,
		std::map<Topo::CoEdge*, uint> &filtre2_coedge,
        std::vector<BlockDirPos>& bloc_dirPos)
{

    for (std::vector<Topo::Block*>::iterator iter = m_blocks.begin();
            iter != m_blocks.end(); ++iter){

    	Topo::Block* bloc = *iter;

    	Topo::Block::eDirOnBlock dir = Topo::Block::unknown;

    	std::vector<Topo::CoEdge* > iCoedges[3];
    	bloc->getOrientedCoEdges(iCoedges[0], iCoedges[1], iCoedges[2]);

    	// recherche des ratios par arête dans le bloc
    	std::map<Topo::CoEdge*,uint> ratios;
    	bloc->getRatios(ratios);

    	for (uint i=0; i<3; i++){
    		uint pos1_blk = 0;
    		uint pos2_blk = 0;

    		// on restreint le parcours des arêtes à une arête du bloc
    		std::vector<Topo::CoEdge* > coedges_between;
    		uint ind_vtx = 1 << i;
#ifdef _DEBUG_SHEETS
    		std::cout<<"getCoEdgesBetweenVertices entre "<<bloc->getVertex(0)->getName()
                        		<<" et "<<bloc->getVertex(ind_vtx)->getName()<<" d'indice "<<ind_vtx<<std::endl;
#endif
    		Topo::TopoHelper::getCoEdgesBetweenVertices(bloc->getVertex(0), bloc->getVertex(ind_vtx), iCoedges[i], coedges_between);

    		Topo::Vertex* vtx = bloc->getVertex(0);
    		for (std::vector<Topo::CoEdge* >::iterator iter3 = coedges_between.begin();
    				iter3 != coedges_between.end(); ++iter3){
    			Topo::CoEdge* coedge = *iter3;
    			if (filtre1_coedge[coedge] == 0){
    				pos1_blk += coedge->getNbMeshingEdges()/ratios[coedge];
    				pos2_blk += coedge->getNbMeshingEdges()/ratios[coedge];
    			}
    			else {
    				dir = (Topo::Block::eDirOnBlock)i;
    				uint ratio = ratios[coedge];
    				uint dec = ratio-1;
    				if (vtx == coedge->getVertex(0)){
    					pos1_blk += (filtre1_coedge[coedge]+dec)/ratio;
    					pos2_blk += (filtre2_coedge[coedge]+dec)/ratio;
    				}
    				else {
    					pos1_blk += (coedge->getNbMeshingEdges() - filtre1_coedge[coedge] + 1 + dec)/ratio;
    					pos2_blk += (coedge->getNbMeshingEdges() - filtre2_coedge[coedge] + 1 + dec)/ratio;
    				}
    				// on a la direction et la position dans le bloc
					BlockDirPos bdp = {bloc, dir, pos1_blk, pos2_blk};
    				bloc_dirPos.push_back(bdp);

#ifdef _DEBUG_SHEETS
std::cout<<" Bloc "<<bloc->getName()<<" coupé en pos "<<pos1_blk<<" et "<<pos2_blk<<" suivant dir "<<dir<<std::endl;
#endif
    			}

    			vtx = coedge->getOppositeVertex(vtx);

    		} // end for iter
    	} // end for i<3

#ifdef _DEBUG_SHEETS
    	if (dir == Topo::Block::unknown)
    		std::cout<<" Bloc "<<bloc->getName()<<" non touché par la coupe"<<std::endl;
#endif

    } // end for iter = filtre_block.begin();
}
/*----------------------------------------------------------------------------*/
void CommandCreateSubVolumeBetweenSheets::
createSubVolume(std::vector<BlockDirPos>& bloc_dirPos)
{
#ifdef _DEBUG_SHEETS
    std::cout<<"CommandSheetsSubVolume::createSubVolume()"<<std::endl;
#endif

    // création du sous-volume
    Mesh::SubVolume* sv = getContext().getLocalMeshManager().getNewSubVolume(m_group_name, &getInfoCommand());

    // recherche le groupe 3D et le construit si nécessaire
    Group::Group3D* gr = getContext().getLocalGroupManager().getNewGroup3D(m_group_name, &getInfoCommand());
    gr->setLevel(0);
    sv->getDisplayProperties().setDisplayed(gr->isVisible());
    if (!gr->find(sv))
    	gr->add(sv);

    gmds::IGMesh& gmds_mesh = getContext().getLocalMeshManager().getMesh()->getGMDSMesh();

    // parcours les blocs
    for (std::vector<BlockDirPos>::iterator iter1 = bloc_dirPos.begin();
            iter1 != bloc_dirPos.end(); ++iter1){
        Topo::Block* blk = (*iter1).m_bloc;
        Topo::Block::eDirOnBlock dir = (*iter1).m_dir;
        uint pos1 = (*iter1).m_pos1;
        uint pos2 = (*iter1).m_pos2;
        // cas où il faut pemuter les indices
        if (pos1>pos2){
        	uint tmp = pos1;
        	pos1 = pos2;
        	pos2 = tmp;
        }

#ifdef _DEBUG_SHEETS
        std::cout<<" blk "<<blk->getName()<<", dir "<<dir<<", pos "<<pos1<<" et "<<pos2<<std::endl;
#endif

        uint nbBrasI, nbBrasJ, nbBrasK;
        blk->getNbMeshingEdges(nbBrasI, nbBrasJ, nbBrasK);

        if (dir == 0){
        	for (uint pos=pos1; pos<=pos2; pos++)
        		for (uint j=0; j<nbBrasJ; j++)
        			for (uint k=0; k<nbBrasK; k++){
        				gmds::Region r = gmds_mesh.get<gmds::Region>(blk->regions()[pos-1+j*nbBrasI+k*nbBrasI*nbBrasJ]);
        				sv->addRegion(r);

        			}
        } else if (dir == 1) {
        	for (uint pos=pos1; pos<=pos2; pos++)
        		for (uint i=0; i<nbBrasI; i++)
        			for (uint k=0; k<nbBrasK; k++){
        				gmds::Region r = gmds_mesh.get<gmds::Region>(blk->regions()[i+(pos-1)*nbBrasI+k*nbBrasI*nbBrasJ]);
        				sv->addRegion(r);
        			}
        } else if (dir == 2) {
        	for (uint pos=pos1; pos<=pos2; pos++)
        		for (uint i=0; i<nbBrasI; i++)
        			for (uint j=0; j<nbBrasJ; j++){
        				gmds::Region r = gmds_mesh.get<gmds::Region>(blk->regions()[i+j*nbBrasI+(pos-1)*nbBrasI*nbBrasJ]);
        				sv->addRegion(r);
        			}
        }
    } // end for iter1

}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
