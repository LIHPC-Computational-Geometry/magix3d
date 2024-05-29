/*----------------------------------------------------------------------------*/
/*
 * \file CommandAddRemoveGroupName.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22 oct. 2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"
#include "Group/GroupManager.h"
#include "Mesh/CommandAddRemoveGroupName.h"
#include "Internal/Context.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Mesh/MeshManager.h"
#include "Mesh/Cloud.h"
#include "Mesh/Surface.h"
#include "Mesh/Volume.h"

#include <vector>

#include <TkUtil/MemoryError.h>

//#define _DEBUG_UPDATE
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandAddRemoveGroupName::CommandAddRemoveGroupName(Internal::Context& c,
        std::vector<Geom::Volume*>& volumes,
        const std::string& groupName,
        groupOperation ope)
: CommandCreateMesh(c, (std::string("Modifie le groupe ")+groupName),0)
, m_geom_entities(volumes.begin(), volumes.end())
, m_groupName(groupName)
, m_dim(3)
, m_ope(ope)
{
}
/*----------------------------------------------------------------------------*/
CommandAddRemoveGroupName::CommandAddRemoveGroupName(Internal::Context& c,
        std::vector<Geom::Surface*>& surfaces,
        const std::string& groupName,
        groupOperation ope)
: CommandCreateMesh(c, (std::string("Modifie le groupe ")+groupName),0)
, m_geom_entities(surfaces.begin(), surfaces.end())
, m_groupName(groupName)
, m_dim(2)
, m_ope(ope)
{
}
/*----------------------------------------------------------------------------*/
CommandAddRemoveGroupName::CommandAddRemoveGroupName(Internal::Context& c,
        std::vector<Geom::Curve*>& curves,
        const std::string& groupName,
        groupOperation ope)
: CommandCreateMesh(c, (std::string("Modifie le groupe ")+groupName),0)
, m_geom_entities(curves.begin(), curves.end())
, m_groupName(groupName)
, m_dim(1)
, m_ope(ope)
{
}
/*----------------------------------------------------------------------------*/
CommandAddRemoveGroupName::CommandAddRemoveGroupName(Internal::Context& c,
        std::vector<Geom::Vertex*>& vertices,
        const std::string& groupName,
        groupOperation ope)
: CommandCreateMesh(c, (std::string("Modifie le groupe ")+groupName),0)
, m_geom_entities(vertices.begin(), vertices.end())
, m_groupName(groupName)
, m_dim(0)
, m_ope(ope)
{
}
/*----------------------------------------------------------------------------*/
CommandAddRemoveGroupName::CommandAddRemoveGroupName(Internal::Context& c,
		std::vector<Topo::Block*>& blocks,
        const std::string& groupName,
        groupOperation ope)
: CommandCreateMesh(c, (std::string("Modifie le groupe ")+groupName),0)
, m_topo_entities(blocks.begin(), blocks.end())
, m_groupName(groupName)
, m_dim(3)
, m_ope(ope)
{
}
/*----------------------------------------------------------------------------*/
CommandAddRemoveGroupName::CommandAddRemoveGroupName(Internal::Context& c,
        std::vector<Topo::CoFace*>& cofaces,
        const std::string& groupName,
        groupOperation ope)
: CommandCreateMesh(c, (std::string("Modifie le groupe ")+groupName),0)
, m_topo_entities(cofaces.begin(), cofaces.end())
, m_groupName(groupName)
, m_dim(2)
, m_ope(ope)
{
}
/*----------------------------------------------------------------------------*/
CommandAddRemoveGroupName::CommandAddRemoveGroupName(Internal::Context& c,
        std::vector<Topo::CoEdge*>& coedges,
        const std::string& groupName,
        groupOperation ope)
: CommandCreateMesh(c, (std::string("Modifie le groupe ")+groupName),0)
, m_topo_entities(coedges.begin(), coedges.end())
, m_groupName(groupName)
, m_dim(1)
, m_ope(ope)
{
}
/*----------------------------------------------------------------------------*/
CommandAddRemoveGroupName::CommandAddRemoveGroupName(Internal::Context& c,
        std::vector<Topo::Vertex*>& vertices,
        const std::string& groupName,
        groupOperation ope)
: CommandCreateMesh(c, (std::string("Modifie le groupe ")+groupName),0)
, m_topo_entities(vertices.begin(), vertices.end())
, m_groupName(groupName)
, m_dim(0)
, m_ope(ope)
{
}
/*----------------------------------------------------------------------------*/
CommandAddRemoveGroupName::~CommandAddRemoveGroupName()
{
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::internalExecute()
{
	// archivage des groupes initiaux
	for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
	                iter != m_geom_entities.end(); ++iter){
		(*iter)->getGroupsName(m_geom_groups[*iter]);
	} // iter
	// pour la topologie, on n'utilise pas le mécanisme de sauvegarde
	for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
	                iter != m_topo_entities.end(); ++iter){
		(*iter)->getGroupsName(m_topo_groups[*iter], false, true);
	} // iter

    switch(m_dim){
    case(0):{

        Group::Group0D* grp = getContext().getLocalGroupManager().getNewGroup0D(m_groupName, &getInfoCommand());

        for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
                iter != m_geom_entities.end(); ++iter){
            Geom::Vertex* vtx =  dynamic_cast<Geom::Vertex*>(*iter);
            CHECK_NULL_PTR_ERROR(vtx);
            if (m_ope == add){

                // il faut peut-être enlever le groupe par défaut
                if (vtx->getNbGroups() == 1){
                    std::vector<std::string> gn;
                    vtx->getGroupsName(gn);
                    if (gn[0] == getContext().getLocalGroupManager().getDefaultName(0))
                    	removeGroup("", vtx);
                }

                // on ajoute le groupe
                grp->add(vtx);
                vtx->add(grp);
            }
            else if (m_ope == remove) {
                // on retire le groupe
                grp->remove(vtx);
                vtx->remove(grp);

                // il faut peut-être ajouter au groupe par défaut
                if (vtx->getNbGroups() == 0)
                    addGroup("", vtx);

            } else if (m_ope == set){
            	std::vector<std::string> gn;
            	vtx->getGroupsName(gn);
            	// on retire tous les groupes
            	for (uint i=0; i<gn.size(); i++)
            		removeGroup(gn[i], vtx);

                // on ajoute le groupe
                grp->add(vtx);
                vtx->add(grp);

            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }
        } // end for iter

        for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
                iter != m_topo_entities.end(); ++iter){
            Topo::Vertex* vtx =  dynamic_cast<Topo::Vertex*>(*iter);
            CHECK_NULL_PTR_ERROR(vtx);
            if (m_ope == add){

                // il faut peut-être enlever le groupe par défaut
                if (vtx->getGroupsContainer().getNb() == 1){
                    std::vector<std::string> gn;
                    vtx->getGroupsName(gn, false, true);
                    if (gn[0] == getContext().getLocalGroupManager().getDefaultName(0))
                    	removeGroup("", vtx);
                }

                // on ajoute le groupe
                grp->add(vtx);
                vtx->getGroupsContainer().add(grp);
            }
            else if (m_ope == remove) {
                // on retire le groupe
                grp->remove(vtx);
                vtx->getGroupsContainer().remove(grp, true);

                // il faut peut-être ajouter au groupe par défaut
                if (vtx->getGroupsContainer().getNb() == 0 && vtx->getGeomAssociation() == 0)
                    addGroup("", vtx);

            } else if (m_ope == set){
            	std::vector<std::string> gn;
            	vtx->getGroupsName(gn, false, true);
            	// on retire tous les groupes
            	for (uint i=0; i<gn.size(); i++)
            		removeGroup(gn[i], vtx);

                // on ajoute le groupe
                grp->add(vtx);
                vtx->getGroupsContainer().add(grp);

            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }

    		// le sommet n'est peut-être plus visible / groupe visibles
			getInfoCommand().addTopoInfoEntity(vtx,Internal::InfoCommand::VISIBILYCHANGED);

        } // end for iter

        if (grp->empty())
            getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISABLE);
        else
            getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
    }
    break;
    case(1):{
    	Group::Group1D* grp = getContext().getLocalGroupManager().getNewGroup1D(m_groupName, &getInfoCommand());

    	for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
    			iter != m_geom_entities.end(); ++iter){
    		Geom::Curve* crv =  dynamic_cast<Geom::Curve*>(*iter);
    		CHECK_NULL_PTR_ERROR(crv);
    		if (m_ope == add){

    			// il faut peut-être enlever le groupe par défaut
    			if (crv->getNbGroups() == 1){
    				std::vector<std::string> gn;
    				crv->getGroupsName(gn);
    				if (gn[0] == getContext().getLocalGroupManager().getDefaultName(1))
    					removeGroup("", crv);
    			}

    			// on ajoute le groupe
    			grp->add(crv);
    			crv->add(grp);
    			updateMesh(crv, grp->getName(), true);
    		}
    		else if (m_ope == remove) {
    			// on retire le groupe
    			grp->remove(crv);
    			crv->remove(grp);
    			updateMesh(crv, grp->getName(), false);

    			// il faut peut-être ajouter au groupe par défaut
    			if (crv->getNbGroups() == 0)
    				addGroup("", crv);

    		} else if (m_ope == set){
            	std::vector<std::string> gn;
            	crv->getGroupsName(gn);
            	// on retire tous les groupes
            	for (uint i=0; i<gn.size(); i++)
            		removeGroup(gn[i], crv);

                // on ajoute le groupe
                grp->add(crv);
                crv->add(grp);

            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }
    	} // end for iter

    	for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
    			iter != m_topo_entities.end(); ++iter){
    		Topo::CoEdge* coedge =  dynamic_cast<Topo::CoEdge*>(*iter);
    		CHECK_NULL_PTR_ERROR(coedge);
    		std::vector<Topo::CoEdge*> coedges;
    		coedges.push_back(coedge);
    		if (m_ope == add){

    			// il faut peut-être enlever le groupe par défaut
    			if (coedge->getGroupsContainer().getNb() == 1){
    				std::vector<std::string> gn;
    				coedge->getGroupsName(gn);
    				if (gn[0] == getContext().getLocalGroupManager().getDefaultName(1))
    					removeGroup("", coedge);
    			}

    			// on ajoute le groupe
    			grp->add(coedge);
    			coedge->getGroupsContainer().add(grp);
    			updateMesh(coedges, grp->getName(), true);
    		}
    		else if (m_ope == remove) {
    			// on retire le groupe
    			grp->remove(coedge);
    			coedge->getGroupsContainer().remove(grp, true);
    			updateMesh(coedges, grp->getName(), false);

    			// il faut peut-être ajouter au groupe par défaut
    			if (coedge->getGroupsContainer().getNb() == 0 && coedge->getGeomAssociation() == 0)
    				addGroup("", coedge);

    		} else if (m_ope == set){
            	std::vector<std::string> gn;
            	coedge->getGroupsName(gn);
            	// on retire tous les groupes
            	for (uint i=0; i<gn.size(); i++)
            		removeGroup(gn[i], coedge);

                // on ajoute le groupe
                grp->add(coedge);
                coedge->getGroupsContainer().add(grp);

            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }

    		// l'arête n'est peut-être plus visible / groupe visibles
			getInfoCommand().addTopoInfoEntity(coedge,Internal::InfoCommand::VISIBILYCHANGED);

    	} // end for iter

    	if (grp->empty())
    		getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISABLE);
    	else
    		getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
    }
    break;
    case(2):{
    	Group::Group2D* grp = getContext().getLocalGroupManager().getNewGroup2D(m_groupName, &getInfoCommand());

    	for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
    			iter != m_geom_entities.end(); ++iter){
    		Geom::Surface* surf =  dynamic_cast<Geom::Surface*>(*iter);
    		CHECK_NULL_PTR_ERROR(surf);
    		if (m_ope == add){

    			// il faut peut-être enlever le groupe par défaut
    			if (surf->getNbGroups() == 1){
    				std::vector<std::string> gn;
    				surf->getGroupsName(gn);
    				if (gn[0] == getContext().getLocalGroupManager().getDefaultName(2))
    					removeGroup("", surf);
    			}

    			// on ajoute le groupe
    			grp->add(surf);
    			surf->add(grp);
    			updateMesh(surf, grp->getName(), true);
    		}
    		else if (m_ope == remove) {
    			// on retire le groupe
    			grp->remove(surf);
    			surf->remove(grp);
    			updateMesh(surf, grp->getName(), false);

    			// il faut peut-être ajouter au groupe par défaut
    			if (surf->getNbGroups() == 0)
    				addGroup("", surf);

    		} else if (m_ope == set){
            	std::vector<std::string> gn;
            	surf->getGroupsName(gn);
            	// on retire tous les groupes
            	for (uint i=0; i<gn.size(); i++)
            		removeGroup(gn[i], surf);

                // on ajoute le groupe
                grp->add(surf);
                surf->add(grp);

            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }
    	} // end for iter

    	for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
    			iter != m_topo_entities.end(); ++iter){
    		Topo::CoFace* coface =  dynamic_cast<Topo::CoFace*>(*iter);
    		CHECK_NULL_PTR_ERROR(coface);
    		std::vector<Topo::CoFace*> cofaces;
    		cofaces.push_back(coface);
    		if (m_ope == add){

    			// il faut peut-être enlever le groupe par défaut
    			if (coface->getGroupsContainer().getNb() == 1){
    				std::vector<std::string> gn;
    				coface->getGroupsName(gn);
    				if (gn[0] == getContext().getLocalGroupManager().getDefaultName(2))
    					removeGroup("", coface);
    			}

    			// on ajoute le groupe
    			grp->add(coface);
    			coface->getGroupsContainer().add(grp);
    			updateMesh(cofaces, grp->getName(), true);
    		}
    		else if (m_ope == remove) {
    			// on retire le groupe
    			grp->remove(coface);
    			coface->getGroupsContainer().remove(grp, true);
    			updateMesh(cofaces, grp->getName(), false);

    			// il faut peut-être ajouter au groupe par défaut
    			if (coface->getGroupsContainer().getNb() == 0 && coface->getGeomAssociation() == 0)
    				addGroup("", coface);

    		} else if (m_ope == set){
            	std::vector<std::string> gn;
            	coface->getGroupsName(gn);
            	// on retire tous les groupes
            	for (uint i=0; i<gn.size(); i++)
            		removeGroup(gn[i], coface);

                // on ajoute le groupe
                grp->add(coface);
                coface->getGroupsContainer().add(grp);

            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }

    		// la face n'est peut-être plus visible / groupe visibles
			getInfoCommand().addTopoInfoEntity(coface,Internal::InfoCommand::VISIBILYCHANGED);

    	} // end for iter

    	if (grp->empty())
    		getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISABLE);
    	else
    		getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
    }
    break;
    case(3):{
    	Group::Group3D* grp = getContext().getLocalGroupManager().getNewGroup3D(m_groupName, &getInfoCommand());

    	for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
    			iter != m_geom_entities.end(); ++iter){
    		Geom::Volume* vol =  dynamic_cast<Geom::Volume*>(*iter);
    		CHECK_NULL_PTR_ERROR(vol);
    		if (m_ope == add){

    			// il faut peut-être enlever le groupe par défaut
    			if (vol->getNbGroups() == 1){
    				std::vector<std::string> gn;
    				vol->getGroupsName(gn);
    				if (gn[0] == getContext().getLocalGroupManager().getDefaultName(3))
    					removeGroup("", vol);
    			}

    			// on ajoute le groupe
    			grp->add(vol);
    			vol->add(grp);
    			updateMesh(vol, grp->getName(), true);
    		}
    		else if (m_ope == remove) {
    			// on retire le groupe
    			grp->remove(vol);
    			vol->remove(grp);
    			updateMesh(vol, grp->getName(), false);

    			// il faut peut-être ajouter au groupe par défaut
    			if (vol->getNbGroups() == 0)
    				addGroup("", vol);

    		} else if (m_ope == set){
            	std::vector<std::string> gn;
            	vol->getGroupsName(gn);
            	// on retire tous les groupes
            	for (uint i=0; i<gn.size(); i++)
            		removeGroup(gn[i], vol);

                // on ajoute le groupe
                grp->add(vol);
                vol->add(grp);
                updateMesh(vol, grp->getName(), true);

            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }
    	} // end for iter

    	for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
    			iter != m_topo_entities.end(); ++iter){
    		Topo::Block* bloc =  dynamic_cast<Topo::Block*>(*iter);
    		CHECK_NULL_PTR_ERROR(bloc);
    		std::vector<Topo::Block*> blocs;
    		blocs.push_back(bloc);
    		if (m_ope == add){

    			// il faut peut-être enlever le groupe par défaut
    			if (bloc->getGroupsContainer().getNb() == 1){
    				std::vector<std::string> gn;
    				bloc->getGroupsName(gn);
    				if (gn[0] == getContext().getLocalGroupManager().getDefaultName(3))
    					removeGroup("", bloc);
    			}

    			// on ajoute le groupe
    			grp->add(bloc);
    			bloc->getGroupsContainer().add(grp);
    			updateMesh(blocs, grp->getName(), true);
    		}
    		else if (m_ope == remove) {
    			// on retire le groupe
    			grp->remove(bloc);
    			bloc->getGroupsContainer().remove(grp, true);
    			updateMesh(blocs, grp->getName(), false);

    			// il faut peut-être ajouter au groupe par défaut
    			if (bloc->getGroupsContainer().getNb() == 0 && bloc->getGeomAssociation() == 0)
    				addGroup("", bloc);

    		} else if (m_ope == set){
            	std::vector<std::string> gn;
            	bloc->getGroupsName(gn);
            	// on retire tous les groupes
            	for (uint i=0; i<gn.size(); i++)
            		removeGroup(gn[i], bloc);

                // on ajoute le groupe
                grp->add(bloc);
                bloc->getGroupsContainer().add(grp);
                updateMesh(blocs, grp->getName(), true);

            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }

    		// le bloc n'est peut-être plus visible / groupe visibles
			getInfoCommand().addTopoInfoEntity(bloc,Internal::InfoCommand::VISIBILYCHANGED);

    	} // end for iter

    	if (grp->empty())
    		getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISABLE);
    	else
    		getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
    }
    break;

    default:{
    	TkUtil::Exception ("[Erreur interne] Dimension erronée");
    }
    break;
    }

#ifdef _DEBUG2
    std::cout<<"Après : CommandAddRemoveGroupName::internalExecute"<<std::endl;
    std::cout<< getInfoCommand() <<std::endl;
#endif

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::internalUndo()
{

	switch(m_dim){
	case(0):{
		Group::Group0D* grp = getContext().getLocalGroupManager().getNewGroup0D(m_groupName, &getInfoCommand());

		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Vertex* vtx =  dynamic_cast<Geom::Vertex*>(*iter);
			CHECK_NULL_PTR_ERROR(vtx);

			std::vector<std::string> gnOld;
			vtx->getGroupsName(gnOld);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++)
				removeGroup(gnOld[i], vtx);

			std::vector<std::string>& gnNew = m_geom_groups[vtx];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++)
				addGroup(gnNew[i], vtx);
		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::Vertex* te =  dynamic_cast<Topo::Vertex*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			std::vector<std::string> gnOld;
			te->getGroupsName(gnOld, false, true);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++)
				removeGroup(gnOld[i], te);

			std::vector<std::string>& gnNew = m_topo_groups[te];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++)
				addGroup(gnNew[i], te);
		} // end for iter
	}
	break;
	case(1):{
		Group::Group1D* grp = getContext().getLocalGroupManager().getNewGroup1D(m_groupName, &getInfoCommand());

		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Curve* crv =  dynamic_cast<Geom::Curve*>(*iter);
			CHECK_NULL_PTR_ERROR(crv);

			std::vector<std::string> gnOld;
			crv->getGroupsName(gnOld);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++)
				removeGroup(gnOld[i], crv);

			std::vector<std::string>& gnNew = m_geom_groups[crv];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++)
				addGroup(gnNew[i], crv);
		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::CoEdge* te =  dynamic_cast<Topo::CoEdge*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			std::vector<std::string> gnOld;
			te->getGroupsName(gnOld, false, true);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++)
				removeGroup(gnOld[i], te);

			std::vector<std::string>& gnNew = m_topo_groups[te];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++)
				addGroup(gnNew[i], te);
		} // end for iter
	}
	break;
	case(2):{
		Group::Group2D* grp = getContext().getLocalGroupManager().getNewGroup2D(m_groupName, &getInfoCommand());

		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Surface* srf =  dynamic_cast<Geom::Surface*>(*iter);
			CHECK_NULL_PTR_ERROR(srf);

			std::vector<std::string> gnOld;
			srf->getGroupsName(gnOld);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++)
				removeGroup(gnOld[i], srf);

			std::vector<std::string>& gnNew = m_geom_groups[srf];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++)
				addGroup(gnNew[i], srf);
		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::CoFace* te =  dynamic_cast<Topo::CoFace*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			std::vector<std::string> gnOld;
			te->getGroupsName(gnOld, false, true);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++)
				removeGroup(gnOld[i], te);

			std::vector<std::string>& gnNew = m_topo_groups[te];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++)
				addGroup(gnNew[i], te);
		} // end for iter
	}
	break;
	case(3):{
		Group::Group3D* grp = getContext().getLocalGroupManager().getNewGroup3D(m_groupName, &getInfoCommand());

		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Volume* vol =  dynamic_cast<Geom::Volume*>(*iter);
			CHECK_NULL_PTR_ERROR(vol);

			std::vector<std::string> gnOld;
			vol->getGroupsName(gnOld);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++)
				removeGroup(gnOld[i], vol);

			std::vector<std::string>& gnNew = m_geom_groups[vol];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++)
				addGroup(gnNew[i], vol);
		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::Block* te =  dynamic_cast<Topo::Block*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			std::vector<std::string> gnOld;
			te->getGroupsName(gnOld, false, true);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++)
				removeGroup(gnOld[i], te);

			std::vector<std::string>& gnNew = m_topo_groups[te];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++)
				addGroup(gnNew[i], te);
		} // end for iter
	}
	break;

	default:{
		TkUtil::Exception ("[Erreur interne] Dimension erronée");
	}
	break;
	}

    getInfoCommand().permCreatedDeleted();

    // suppression de ce qui a été ajouté
    deleteCreatedMeshEntities();

    // permute toutes les propriétés internes avec leur sauvegarde
    permInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::internalRedo()
{
    // supprime toutes les propriétés internes sauvegardées
    deleteInternalsStats();

    getInfoCommand().clear();

    // refait les opérations
    execute();
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
updateMesh(Geom::Volume* vol, std::string grpName, bool add)
{
    // récupération de la liste des blocs
    std::vector<Topo::Block*>  blocs;
    vol->get(blocs);
    updateMesh(blocs, grpName, add);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
updateMesh(std::vector<Topo::Block*>& blocs, std::string grpName, bool add)
{
#ifdef _DEBUG_UPDATE
    std::cout<<"updateMesh"<<std::endl;
#endif

    // recherche si l'un d'entre eux est maillé
    bool is_meshed = false;
    for (std::vector<Topo::Block*>::iterator iter = blocs.begin();
            iter != blocs.end(); ++iter){
        if ((*iter)->isMeshed())
            is_meshed = true;
    }

    if (is_meshed){
        // recherche du Mesh::Volume
        bool isNewVolume = false;
        try {
#ifdef _DEBUG_UPDATE
            std::cout<<"recherche getVolume "<<grpName<<std::endl;
#endif
            getContext().getLocalMeshManager().getVolume(grpName);
#ifdef _DEBUG_UPDATE
            std::cout<<"addModifiedVolume "<<grpName<<std::endl;
#endif
            addModifiedVolume(grpName);
        } catch (...) {
#ifdef _DEBUG_UPDATE
            std::cout<<"addNewVolume "<<grpName<<std::endl;
#endif
            addNewVolume(grpName);
            isNewVolume = true;
        }
        Mesh::Volume* mvol = getContext().getLocalMeshManager().getVolume(grpName);

        for (std::vector<Topo::Block*>::iterator iter = blocs.begin();
                iter != blocs.end(); ++iter)
            if ((*iter)->isMeshed()){
                if (add){
#ifdef _DEBUG_UPDATE
                    std::cout<<" "<<mvol->getName()<<" addBlock("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    mvol->addBlock(*iter);
                }
                else {
#ifdef _DEBUG_UPDATE
                    std::cout<<" "<<mvol->getName()<<" removeBlock("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    mvol->removeBlock(*iter);
                }
            }
        if (isNewVolume)
            getContext().newGraphicalRepresentation (*mvol);
    } // end if (is_meshed)
#ifdef _DEBUG_UPDATE
    std::cout<<"end updateMesh"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
updateMesh(Geom::Surface* surf, std::string grpName, bool add)
{
	// récupération de la liste des CoFaces
	std::vector<Topo::CoFace*>  cofaces;
	surf->get(cofaces);
	updateMesh(cofaces, grpName, add);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
updateMesh(std::vector<Topo::CoFace*>& cofaces, std::string grpName, bool add)
{
#ifdef _DEBUG_UPDATE
    std::cout<<"updateMesh"<<std::endl;
#endif

    // recherche si l'une d'entre elles est maillée
    bool is_meshed = false;
    for (std::vector<Topo::CoFace*>::iterator iter = cofaces.begin();
            iter != cofaces.end(); ++iter){
        if ((*iter)->isMeshed())
            is_meshed = true;
    }

    if (is_meshed){
        // recherche de la Mesh::Surface
        try {
#ifdef _DEBUG_UPDATE
            std::cout<<"recherche getSurface "<<grpName<<std::endl;
#endif
            getContext().getLocalMeshManager().getSurface(grpName);
#ifdef _DEBUG_UPDATE
            std::cout<<"addModifiedSurface "<<grpName<<std::endl;
#endif
            addModifiedSurface(grpName);
        } catch (...) {
#ifdef _DEBUG_UPDATE
            std::cout<<"addNewSurface "<<grpName<<std::endl;
#endif
            addNewSurface(grpName);
        }
        Mesh::Surface* msurf = getContext().getLocalMeshManager().getSurface(grpName);

        for (std::vector<Topo::CoFace*>::iterator iter = cofaces.begin();
                iter != cofaces.end(); ++iter)
            if ((*iter)->isMeshed()){
                if (add){
#ifdef _DEBUG_UPDATE
                   std::cout<<" "<<msurf->getName()<<" addCoFace("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    msurf->addCoFace(*iter);
                }
                else {
#ifdef _DEBUG_UPDATE
                    std::cout<<" "<<msurf->getName()<<" removeCoFace("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    msurf->removeCoFace(*iter);
                }
            }
    } // end if (is_meshed)
#ifdef _DEBUG_UPDATE
    std::cout<<"end updateMesh"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
updateMesh(Geom::Curve* crv, std::string grpName, bool add)
{
    // récupération de la liste des CoEdges
    std::vector<Topo::CoEdge*>  coedges;
    crv->get(coedges);
    updateMesh(coedges, grpName, add);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
updateMesh(std::vector<Topo::CoEdge*>& coedges, std::string grpName, bool add)
{
#ifdef _DEBUG_UPDATE
    std::cout<<"updateMesh"<<std::endl;
#endif

    // recherche si l'une d'entre elles est maillée
    bool is_meshed = false;
    for (std::vector<Topo::CoEdge*>::iterator iter = coedges.begin();
            iter != coedges.end(); ++iter){
        if ((*iter)->isMeshed())
            is_meshed = true;
    }

    if (is_meshed){
        // recherche du Mesh::Cloud
        try {
#ifdef _DEBUG_UPDATE
            std::cout<<"recherche getCloud "<<grpName<<std::endl;
#endif
            getContext().getLocalMeshManager().getCloud(grpName);
#ifdef _DEBUG_UPDATE
            std::cout<<"addModifiedCloud "<<grpName<<std::endl;
#endif
            addModifiedCloud(grpName);
        } catch (...) {
#ifdef _DEBUG_UPDATE
            std::cout<<"addNewCloud "<<grpName<<std::endl;
#endif
            addNewCloud(grpName);
        }
        Mesh::Cloud* mcld = getContext().getLocalMeshManager().getCloud(grpName);

        for (std::vector<Topo::CoEdge*>::iterator iter = coedges.begin();
                iter != coedges.end(); ++iter)
            if ((*iter)->isMeshed()){
                if (add){
#ifdef _DEBUG_UPDATE
                   std::cout<<" "<<mcld->getName()<<" addCoEdge("<<(*iter)->getName()<<")"<<std::endl;
#endif
                   mcld->addCoEdge(*iter);
                }
                else {
#ifdef _DEBUG_UPDATE
                    std::cout<<" "<<mcld->getName()<<" removeCoEdge("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    mcld->removeCoEdge(*iter);
                }
            }

        // recherche du Mesh::Line
        try {
#ifdef _DEBUG_UPDATE
            std::cout<<"recherche getLine "<<grpName<<std::endl;
#endif
            getContext().getLocalMeshManager().getLine(grpName);
#ifdef _DEBUG_UPDATE
            std::cout<<"addModifiedLine "<<grpName<<std::endl;
#endif
            addModifiedLine(grpName);
        } catch (...) {
#ifdef _DEBUG_UPDATE
            std::cout<<"addNewLine "<<grpName<<std::endl;
#endif
            addNewLine(grpName);
        }
        Mesh::Line* mln = getContext().getLocalMeshManager().getLine(grpName);

        for (std::vector<Topo::CoEdge*>::iterator iter = coedges.begin();
                iter != coedges.end(); ++iter)
            if ((*iter)->isMeshed()){
                if (add){
#ifdef _DEBUG_UPDATE
                   std::cout<<" "<<mln->getName()<<" addCoEdge("<<(*iter)->getName()<<")"<<std::endl;
#endif
                   mln->addCoEdge(*iter);
                }
                else {
#ifdef _DEBUG_UPDATE
                    std::cout<<" "<<mcld->getName()<<" removeCoEdge("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    mln->removeCoEdge(*iter);
                }
            }


    } // end if (is_meshed)
#ifdef _DEBUG_UPDATE
    std::cout<<"end updateMesh"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
addGroup(std::string ng, Geom::Vertex* vtx)
{
	Group::Group0D* grp = getContext().getLocalGroupManager().getNewGroup0D(ng, &getInfoCommand());
	grp->add(vtx);
	vtx->add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
addGroup(std::string ng, Topo::Vertex* vtx)
{
	Group::Group0D* grp = getContext().getLocalGroupManager().getNewGroup0D(ng, &getInfoCommand());
	grp->add(vtx);
	vtx->getGroupsContainer().add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
removeGroup(std::string ng, Geom::Vertex* vtx)
{
	Group::Group0D* grp = getContext().getLocalGroupManager().getGroup0D(ng, &getInfoCommand());
	grp->remove(vtx);
	vtx->remove(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
removeGroup(std::string ng, Topo::Vertex* vtx)
{
	Group::Group0D* grp = getContext().getLocalGroupManager().getGroup0D(ng, &getInfoCommand());
	grp->remove(vtx);
	vtx->getGroupsContainer().remove(grp, true);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
addGroup(std::string ng, Geom::Curve* crv)
{
	Group::Group1D* grp = getContext().getLocalGroupManager().getNewGroup1D(ng, &getInfoCommand());
	grp->add(crv);
	crv->add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	updateMesh(crv, grp->getName(), true);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
addGroup(std::string ng, Topo::CoEdge* coedge)
{
	Group::Group1D* grp = getContext().getLocalGroupManager().getNewGroup1D(ng, &getInfoCommand());
	grp->add(coedge);
	coedge->getGroupsContainer().add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	std::vector<Topo::CoEdge*>  coedges;
	coedges.push_back(coedge);
	updateMesh(coedges, grp->getName(), true);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
removeGroup(std::string ng, Geom::Curve* crv)
{
	Group::Group1D* grp = getContext().getLocalGroupManager().getGroup1D(ng, &getInfoCommand());
	grp->remove(crv);
	crv->remove(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	updateMesh(crv, grp->getName(), false);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
removeGroup(std::string ng, Topo::CoEdge* coedge)
{
	Group::Group1D* grp = getContext().getLocalGroupManager().getGroup1D(ng, &getInfoCommand());
	grp->remove(coedge);
	coedge->getGroupsContainer().remove(grp, true);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	std::vector<Topo::CoEdge*>  coedges;
	coedges.push_back(coedge);
	updateMesh(coedges, grp->getName(), false);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
addGroup(std::string ng, Geom::Surface* srf)
{
	Group::Group2D* grp = getContext().getLocalGroupManager().getNewGroup2D(ng, &getInfoCommand());
	grp->add(srf);
	srf->add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	updateMesh(srf, grp->getName(), true);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
addGroup(std::string ng, Topo::CoFace* coface)
{
	Group::Group2D* grp = getContext().getLocalGroupManager().getNewGroup2D(ng, &getInfoCommand());
	grp->add(coface);
	coface->getGroupsContainer().add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	std::vector<Topo::CoFace*>  cofaces;
	cofaces.push_back(coface);
	updateMesh(cofaces, grp->getName(), true);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
removeGroup(std::string ng, Geom::Surface* srf)
{
	Group::Group2D* grp = getContext().getLocalGroupManager().getGroup2D(ng, &getInfoCommand());
	grp->remove(srf);
	srf->remove(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	updateMesh(srf, grp->getName(), false);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
removeGroup(std::string ng, Topo::CoFace* coface)
{
	Group::Group2D* grp = getContext().getLocalGroupManager().getGroup2D(ng, &getInfoCommand());
	grp->remove(coface);
	coface->getGroupsContainer().remove(grp, true);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	std::vector<Topo::CoFace*>  cofaces;
	cofaces.push_back(coface);
	updateMesh(cofaces, grp->getName(), false);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
addGroup(std::string ng, Geom::Volume* vol)
{
	Group::Group3D* grp = getContext().getLocalGroupManager().getNewGroup3D(ng, &getInfoCommand());
	grp->add(vol);
	vol->add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	updateMesh(vol, grp->getName(), true);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
addGroup(std::string ng, Topo::Block* bloc)
{
	Group::Group3D* grp = getContext().getLocalGroupManager().getNewGroup3D(ng, &getInfoCommand());
	grp->add(bloc);
	bloc->getGroupsContainer().add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	std::vector<Topo::Block*>  blocs;
	blocs.push_back(bloc);
	updateMesh(blocs, grp->getName(), true);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
removeGroup(std::string ng, Geom::Volume* vol)
{
	Group::Group3D* grp = getContext().getLocalGroupManager().getGroup3D(ng, &getInfoCommand());
	grp->remove(vol);
	vol->remove(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	updateMesh(vol, grp->getName(), false);
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
removeGroup(std::string ng, Topo::Block* bloc)
{
	Group::Group3D* grp = getContext().getLocalGroupManager().getGroup3D(ng, &getInfoCommand());
	grp->remove(bloc);
	bloc->getGroupsContainer().remove(grp, true);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	std::vector<Topo::Block*>  blocs;
	blocs.push_back(bloc);
	updateMesh(blocs, grp->getName(), false);
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
