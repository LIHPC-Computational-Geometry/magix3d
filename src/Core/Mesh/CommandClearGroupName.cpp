/*----------------------------------------------------------------------------*/
/*
 * \file CommandClearGroupName.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15/04/2019
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"
#include "Group/GroupManager.h"
#include "Mesh/CommandClearGroupName.h"
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
CommandClearGroupName::CommandClearGroupName(Internal::Context& c,
        int dim,
        const std::string& groupName)
: CommandCreateMesh(c, (std::string("Vide le groupe ")+groupName),0)
, m_groupName(groupName)
, m_dim(dim)
{
}
/*----------------------------------------------------------------------------*/
CommandClearGroupName::~CommandClearGroupName()
{
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::internalExecute()
{
	m_geom_entities.clear();
	m_topo_entities.clear();

    switch(m_dim){
    case(0):{
    	Group::Group0D* grp = getContext().getLocalGroupManager().getNewGroup0D(m_groupName, &getInfoCommand());

    	std::vector<Geom::Vertex*> geoms = grp->getVertices();
    	for (std::vector<Geom::Vertex*>::iterator iter = geoms.begin();
    			iter != geoms.end(); ++iter){

    		Geom::Vertex* vtx = *iter;
    		m_geom_entities.push_back(vtx);

    		// on retire le groupe
    		grp->remove(vtx);
    		vtx->remove(grp);

    		// il faut peut-être ajouter au groupe par défaut
    		if (vtx->getNbGroups() == 0)
    			addGroup("", vtx);

    	} // end for iter

    	std::vector<Topo::Vertex*> topos = grp->getTopoVertices();
    	for (std::vector<Topo::Vertex*>::iterator iter = topos.begin();
    			iter != topos.end(); ++iter){

    		Topo::Vertex* topo = *iter;
    		m_topo_entities.push_back(topo);

    		// on retire le groupe
    		grp->remove(topo);
    		topo->getGroupsContainer().remove(grp, true);

    		// il faut peut-être ajouter au groupe par défaut
    		if (topo->getGroupsContainer().getNb() == 0 && topo->getGeomAssociation() == 0)
    			addGroup("", topo);

    		// le sommet n'est peut-être plus visible / groupe visibles
    		getInfoCommand().addTopoInfoEntity(topo, Internal::InfoCommand::VISIBILYCHANGED);

    	} // end for iter

    	if (grp->empty())
    		getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISABLE);
    	else
    		throw TkUtil::Exception("Le groupe n'est pas vide");
    }
    break;
    case(1):{
    	Group::Group1D* grp = getContext().getLocalGroupManager().getNewGroup1D(m_groupName, &getInfoCommand());

    	std::vector<Geom::Curve*> geoms = grp->getCurves();
    	for (std::vector<Geom::Curve*>::iterator iter = geoms.begin();
    			iter != geoms.end(); ++iter){

    		Geom::Curve* geom = *iter;
    		m_geom_entities.push_back(geom);

    		// on retire le groupe
    		grp->remove(geom);
    		geom->remove(grp);

    		// il faut peut-être ajouter au groupe par défaut
    		if (geom->getNbGroups() == 0)
    			addGroup("", geom);

    	} // end for iter

    	std::vector<Topo::CoEdge*> topos = grp->getCoEdges();
    	for (std::vector<Topo::CoEdge*>::iterator iter = topos.begin();
    			iter != topos.end(); ++iter){

    		Topo::CoEdge* topo = *iter;
    		m_topo_entities.push_back(topo);

    		// on retire le groupe
    		grp->remove(topo);
    		topo->getGroupsContainer().remove(grp, true);

    		// il faut peut-être ajouter au groupe par défaut
    		if (topo->getGroupsContainer().getNb() == 0 && topo->getGeomAssociation() == 0)
    			addGroup("", topo);

    		// l'arête n'est peut-être plus visible / groupe visibles
    		getInfoCommand().addTopoInfoEntity(topo, Internal::InfoCommand::VISIBILYCHANGED);

    	} // end for iter

    	if (grp->empty())
    		getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISABLE);
    	else
    		throw TkUtil::Exception("Le groupe n'est pas vide");
    }
    break;
    case(2):{
        Group::Group2D* grp = getContext().getLocalGroupManager().getNewGroup2D(m_groupName, &getInfoCommand());

        std::vector<Geom::Surface*> geoms = grp->getSurfaces();
        for (std::vector<Geom::Surface*>::iterator iter = geoms.begin();
                iter != geoms.end(); ++iter){

        	Geom::Surface* geom = *iter;
        	m_geom_entities.push_back(geom);

        	// on retire le groupe
        	grp->remove(geom);
        	geom->remove(grp);

        	// il faut peut-être ajouter au groupe par défaut
        	if (geom->getNbGroups() == 0)
        		addGroup("", geom);

        } // end for iter

        std::vector<Topo::CoFace*> topos = grp->getCoFaces();
        for (std::vector<Topo::CoFace*>::iterator iter = topos.begin();
                        iter != topos.end(); ++iter){

        	Topo::CoFace* topo = *iter;
        	m_topo_entities.push_back(topo);

        	// on retire le groupe
        	grp->remove(topo);
        	topo->getGroupsContainer().remove(grp, true);

        	// il faut peut-être ajouter au groupe par défaut
        	if (topo->getGroupsContainer().getNb() == 0 && topo->getGeomAssociation() == 0)
        		addGroup("", topo);

    		// la face n'est peut-être plus visible / groupe visibles
			getInfoCommand().addTopoInfoEntity(topo, Internal::InfoCommand::VISIBILYCHANGED);

        } // end for iter

        if (grp->empty())
            getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISABLE);
        else
        	throw TkUtil::Exception("Le groupe n'est pas vide");
    }
    break;
    case(3):{
        Group::Group3D* grp = getContext().getLocalGroupManager().getNewGroup3D(m_groupName, &getInfoCommand());

        std::vector<Geom::Volume*> geoms = grp->getVolumes();
        for (std::vector<Geom::Volume*>::iterator iter = geoms.begin();
                iter != geoms.end(); ++iter){

        	Geom::Volume* geom = *iter;
        	m_geom_entities.push_back(geom);

        	// on retire le groupe
        	grp->remove(geom);
        	geom->remove(grp);

        	// il faut peut-être ajouter au groupe par défaut
        	if (geom->getNbGroups() == 0)
        		addGroup("", geom);

        } // end for iter

        std::vector<Topo::Block*> topos = grp->getBlocks();
        for (std::vector<Topo::Block*>::iterator iter = topos.begin();
                        iter != topos.end(); ++iter){

        	Topo::Block* topo = *iter;
        	m_topo_entities.push_back(topo);

        	// on retire le groupe
        	grp->remove(topo);
        	topo->getGroupsContainer().remove(grp, true);

        	// il faut peut-être ajouter au groupe par défaut
        	if (topo->getGroupsContainer().getNb() == 0 && topo->getGeomAssociation() == 0)
        		addGroup("", topo);

    		// le bloc n'est peut-être plus visible / groupe visibles
			getInfoCommand().addTopoInfoEntity(topo, Internal::InfoCommand::VISIBILYCHANGED);

        } // end for iter

        if (grp->empty())
            getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISABLE);
        else
        	throw TkUtil::Exception("Le groupe n'est pas vide");
    }
    break;

    default:{
    	TkUtil::Exception ("[Erreur interne] Dimension erronée");
    }
    break;
    }

#ifdef _DEBUG2
    std::cout<<"Après : CommandClearGroupName::internalExecute"<<std::endl;
    std::cout<< getInfoCommand() <<std::endl;
#endif

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::internalUndo()
{

	switch(m_dim){
	case(0):{
		Group::Group0D* grp = getContext().getLocalGroupManager().getNewGroup0D(m_groupName, &getInfoCommand());

		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Vertex* vtx =  dynamic_cast<Geom::Vertex*>(*iter);
			CHECK_NULL_PTR_ERROR(vtx);

			addGroup(m_groupName, vtx);

		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::Vertex* te =  dynamic_cast<Topo::Vertex*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			addGroup(m_groupName, te);

		} // end for iter
	}
	break;
	case(1):{
		Group::Group1D* grp = getContext().getLocalGroupManager().getNewGroup1D(m_groupName, &getInfoCommand());

		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Curve* crv =  dynamic_cast<Geom::Curve*>(*iter);
			CHECK_NULL_PTR_ERROR(crv);

			addGroup(m_groupName, crv);

		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::CoEdge* te =  dynamic_cast<Topo::CoEdge*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			addGroup(m_groupName, te);

		} // end for iter
	}
	break;
	case(2):{
		Group::Group2D* grp = getContext().getLocalGroupManager().getNewGroup2D(m_groupName, &getInfoCommand());

		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Surface* srf =  dynamic_cast<Geom::Surface*>(*iter);
			CHECK_NULL_PTR_ERROR(srf);

			addGroup(m_groupName, srf);

		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::CoFace* te =  dynamic_cast<Topo::CoFace*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			addGroup(m_groupName, te);

		} // end for iter
	}
	break;
	case(3):{
		Group::Group3D* grp = getContext().getLocalGroupManager().getNewGroup3D(m_groupName, &getInfoCommand());

		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Volume* vol =  dynamic_cast<Geom::Volume*>(*iter);
			CHECK_NULL_PTR_ERROR(vol);

			addGroup(m_groupName, vol);

		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::Block* te =  dynamic_cast<Topo::Block*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			addGroup(m_groupName, te);

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
void CommandClearGroupName::internalRedo()
{
	switch(m_dim){
	case(0):{
		Group::Group0D* grp = getContext().getLocalGroupManager().getNewGroup0D(m_groupName, &getInfoCommand());

		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Vertex* vtx =  dynamic_cast<Geom::Vertex*>(*iter);
			CHECK_NULL_PTR_ERROR(vtx);

			removeGroup(m_groupName, vtx);

		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::Vertex* te =  dynamic_cast<Topo::Vertex*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			removeGroup(m_groupName, te);

		} // end for iter
	}
	break;
	case(1):{
		Group::Group1D* grp = getContext().getLocalGroupManager().getNewGroup1D(m_groupName, &getInfoCommand());

		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Curve* crv =  dynamic_cast<Geom::Curve*>(*iter);
			CHECK_NULL_PTR_ERROR(crv);

			removeGroup(m_groupName, crv);

		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::CoEdge* te =  dynamic_cast<Topo::CoEdge*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			removeGroup(m_groupName, te);

		} // end for iter
	}
	break;
	case(2):{
		Group::Group2D* grp = getContext().getLocalGroupManager().getNewGroup2D(m_groupName, &getInfoCommand());

		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Surface* srf =  dynamic_cast<Geom::Surface*>(*iter);
			CHECK_NULL_PTR_ERROR(srf);

			removeGroup(m_groupName, srf);

		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::CoFace* te =  dynamic_cast<Topo::CoFace*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			removeGroup(m_groupName, te);

		} // end for iter
	}
	break;
	case(3):{
		Group::Group3D* grp = getContext().getLocalGroupManager().getNewGroup3D(m_groupName, &getInfoCommand());

		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Volume* vol =  dynamic_cast<Geom::Volume*>(*iter);
			CHECK_NULL_PTR_ERROR(vol);

			removeGroup(m_groupName, vol);

		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::Block* te =  dynamic_cast<Topo::Block*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			removeGroup(m_groupName, te);

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
void CommandClearGroupName::
updateMesh(Geom::Volume* vol, std::string grpName, bool add)
{
    // récupération de la liste des blocs
    std::vector<Topo::Block*>  blocs;
    vol->get(blocs);
    updateMesh(blocs, grpName, add);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
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
void CommandClearGroupName::
updateMesh(Geom::Surface* surf, std::string grpName, bool add)
{
	// récupération de la liste des CoFaces
	std::vector<Topo::CoFace*>  cofaces;
	surf->get(cofaces);
	updateMesh(cofaces, grpName, add);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
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
void CommandClearGroupName::
updateMesh(Geom::Curve* crv, std::string grpName, bool add)
{
    // récupération de la liste des CoEdges
    std::vector<Topo::CoEdge*>  coedges;
    crv->get(coedges);
    updateMesh(coedges, grpName, add);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
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
void CommandClearGroupName::
addGroup(std::string ng, Geom::Vertex* vtx)
{
	Group::Group0D* grp = getContext().getLocalGroupManager().getNewGroup0D(ng, &getInfoCommand());
	grp->add(vtx);
	vtx->add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
addGroup(std::string ng, Topo::Vertex* vtx)
{
	Group::Group0D* grp = getContext().getLocalGroupManager().getNewGroup0D(ng, &getInfoCommand());
	grp->add(vtx);
	vtx->getGroupsContainer().add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
removeGroup(std::string ng, Geom::Vertex* vtx)
{
	Group::Group0D* grp = getContext().getLocalGroupManager().getGroup0D(ng, &getInfoCommand());
	grp->remove(vtx);
	vtx->remove(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
removeGroup(std::string ng, Topo::Vertex* vtx)
{
	Group::Group0D* grp = getContext().getLocalGroupManager().getGroup0D(ng, &getInfoCommand());
	grp->remove(vtx);
	vtx->getGroupsContainer().remove(grp, true);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
addGroup(std::string ng, Geom::Curve* crv)
{
	Group::Group1D* grp = getContext().getLocalGroupManager().getNewGroup1D(ng, &getInfoCommand());
	grp->add(crv);
	crv->add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	updateMesh(crv, grp->getName(), true);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
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
void CommandClearGroupName::
removeGroup(std::string ng, Geom::Curve* crv)
{
	Group::Group1D* grp = getContext().getLocalGroupManager().getGroup1D(ng, &getInfoCommand());
	grp->remove(crv);
	crv->remove(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	updateMesh(crv, grp->getName(), false);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
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
void CommandClearGroupName::
addGroup(std::string ng, Geom::Surface* srf)
{
	Group::Group2D* grp = getContext().getLocalGroupManager().getNewGroup2D(ng, &getInfoCommand());
	grp->add(srf);
	srf->add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	updateMesh(srf, grp->getName(), true);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
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
void CommandClearGroupName::
removeGroup(std::string ng, Geom::Surface* srf)
{
	Group::Group2D* grp = getContext().getLocalGroupManager().getGroup2D(ng, &getInfoCommand());
	grp->remove(srf);
	srf->remove(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	updateMesh(srf, grp->getName(), false);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
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
void CommandClearGroupName::
addGroup(std::string ng, Geom::Volume* vol)
{
	Group::Group3D* grp = getContext().getLocalGroupManager().getNewGroup3D(ng, &getInfoCommand());
	grp->add(vol);
	vol->add(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	updateMesh(vol, grp->getName(), true);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
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
void CommandClearGroupName::
removeGroup(std::string ng, Geom::Volume* vol)
{
	Group::Group3D* grp = getContext().getLocalGroupManager().getGroup3D(ng, &getInfoCommand());
	grp->remove(vol);
	vol->remove(grp);
	getInfoCommand().addGroupInfoEntity(grp,Internal::InfoCommand::DISPMODIFIED);
	updateMesh(vol, grp->getName(), false);
}
/*----------------------------------------------------------------------------*/
void CommandClearGroupName::
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
