/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Group/GroupManager.h"
#include "Group/GroupEntity.h"
#include "Mesh/CommandAddRemoveGroupName.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Mesh/MeshManager.h"
#include "Mesh/Cloud.h"
#include "Mesh/Surface.h"
#include "Mesh/Volume.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
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
, m_group_helper(getInfoCommand(), c.getGroupManager())
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
, m_group_helper(getInfoCommand(), c.getGroupManager())
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
, m_group_helper(getInfoCommand(), c.getGroupManager())
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
, m_group_helper(getInfoCommand(), c.getGroupManager())
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
, m_group_helper(getInfoCommand(), c.getGroupManager())
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
, m_group_helper(getInfoCommand(), c.getGroupManager())
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
, m_group_helper(getInfoCommand(), c.getGroupManager())
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
, m_group_helper(getInfoCommand(), c.getGroupManager())
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
        for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
                iter != m_geom_entities.end(); ++iter){
            Geom::Vertex* vtx =  dynamic_cast<Geom::Vertex*>(*iter);
            CHECK_NULL_PTR_ERROR(vtx);
            if (m_ope == add){
                // on ajoute le groupe
				m_group_helper.addToGroup(m_groupName, vtx);
            }
            else if (m_ope == remove) {
                // on retire le groupe
				m_group_helper.removeFromGroup(m_groupName, vtx);
            } else if (m_ope == set){
            	// on retire tous les groupes
            	std::vector<std::string> gn;
            	vtx->getGroupsName(gn);
            	for (uint i=0; i<gn.size(); i++)
            		m_group_helper.removeFromGroup(gn[i], vtx);

                // on ajoute le groupe
				m_group_helper.addToGroup(m_groupName, vtx);
            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }
        } // end for iter

        for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
                iter != m_topo_entities.end(); ++iter){
            Topo::Vertex* vtx =  dynamic_cast<Topo::Vertex*>(*iter);
            CHECK_NULL_PTR_ERROR(vtx);
            if (m_ope == add){
                // on ajoute le groupe
				m_group_helper.addToGroup(m_groupName, vtx);
            }
            else if (m_ope == remove) {
                // on retire le groupe
				m_group_helper.removeFromGroup(m_groupName, vtx);
            } else if (m_ope == set){
            	// on retire tous les groupes
            	std::vector<std::string> gn;
            	vtx->getGroupsName(gn, false, true);
            	for (uint i=0; i<gn.size(); i++)
            		m_group_helper.removeFromGroup(gn[i], vtx);

                // on ajoute le groupe
				m_group_helper.addToGroup(m_groupName, vtx);
            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }

    		// le sommet n'est peut-être plus visible / groupe visibles
			getInfoCommand().addTopoInfoEntity(vtx,Internal::InfoCommand::VISIBILYCHANGED);

        } // end for iter
    }
    break;
    case(1):{
    	for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
    			iter != m_geom_entities.end(); ++iter){
    		Geom::Curve* crv =  dynamic_cast<Geom::Curve*>(*iter);
    		CHECK_NULL_PTR_ERROR(crv);
    		if (m_ope == add){
    			// on ajoute le groupe
				Group::Group1D* grp = m_group_helper.addToGroup<Group::Group1D>(m_groupName, crv);
    			updateMesh(crv, grp->getName(), true);
    		}
    		else if (m_ope == remove) {
    			// on retire le groupe
				Group::Group1D* grp = m_group_helper.removeFromGroup<Group::Group1D>(m_groupName, crv);
    			updateMesh(crv, grp->getName(), false);
    		} else if (m_ope == set){
            	// on retire tous les groupes
            	std::vector<std::string> gn;
            	crv->getGroupsName(gn);
            	for (uint i=0; i<gn.size(); i++)
            		m_group_helper.removeFromGroup(gn[i], crv);

                // on ajoute le groupe
				m_group_helper.addToGroup(m_groupName, crv);
            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }
    	} // end for iter

    	for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
    			iter != m_topo_entities.end(); ++iter){
    		Topo::CoEdge* coedge =  dynamic_cast<Topo::CoEdge*>(*iter);
    		CHECK_NULL_PTR_ERROR(coedge);
    		std::vector<Topo::CoEdge*> coedges = {coedge};
    		if (m_ope == add){
    			// on ajoute le groupe
				Group::Group1D* grp = m_group_helper.addToGroup<Group::Group1D>(m_groupName, coedge);
    			updateMesh(coedges, grp->getName(), true);
    		}
    		else if (m_ope == remove) {
    			// on retire le groupe
				Group::Group1D* grp = m_group_helper.removeFromGroup<Group::Group1D>(m_groupName, coedge);
    			updateMesh(coedges, grp->getName(), false);
    		} else if (m_ope == set){
            	// on retire tous les groupes
            	std::vector<std::string> gn;
            	coedge->getGroupsName(gn);
            	for (uint i=0; i<gn.size(); i++)
            		m_group_helper.removeFromGroup(gn[i], coedge);

                // on ajoute le groupe
				m_group_helper.addToGroup(m_groupName, coedge);
            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }

    		// l'arête n'est peut-être plus visible / groupe visibles
			getInfoCommand().addTopoInfoEntity(coedge,Internal::InfoCommand::VISIBILYCHANGED);

    	} // end for iter
    }
    break;
    case(2):{
    	for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
    			iter != m_geom_entities.end(); ++iter){
    		Geom::Surface* surf =  dynamic_cast<Geom::Surface*>(*iter);
    		CHECK_NULL_PTR_ERROR(surf);
    		if (m_ope == add){
    			// on ajoute le groupe
				Group::Group2D* grp = m_group_helper.addToGroup<Group::Group2D>(m_groupName, surf);
    			updateMesh(surf, grp->getName(), true);
    		}
    		else if (m_ope == remove) {
    			// on retire le groupe
				Group::Group2D* grp = m_group_helper.removeFromGroup<Group::Group2D>(m_groupName, surf);
    			updateMesh(surf, grp->getName(), false);
    		} else if (m_ope == set){
            	// on retire tous les groupes
            	std::vector<std::string> gn;
            	surf->getGroupsName(gn);
            	for (uint i=0; i<gn.size(); i++)
            		m_group_helper.removeFromGroup(gn[i], surf);

                // on ajoute le groupe
				m_group_helper.addToGroup(m_groupName, surf);
            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }
    	} // end for iter

    	for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
    			iter != m_topo_entities.end(); ++iter){
    		Topo::CoFace* coface =  dynamic_cast<Topo::CoFace*>(*iter);
    		CHECK_NULL_PTR_ERROR(coface);
    		std::vector<Topo::CoFace*> cofaces = {coface};
    		if (m_ope == add){
    			// on ajoute le groupe
				Group::Group2D* grp = m_group_helper.addToGroup<Group::Group2D>(m_groupName, coface);
    			updateMesh(cofaces, grp->getName(), true);
    		}
    		else if (m_ope == remove) {
    			// on retire le groupe
				Group::Group2D* grp = m_group_helper.removeFromGroup<Group::Group2D>(m_groupName, coface);
    			updateMesh(cofaces, grp->getName(), false);
    		} else if (m_ope == set){
            	// on retire tous les groupes
            	std::vector<std::string> gn;
            	coface->getGroupsName(gn);
            	for (uint i=0; i<gn.size(); i++)
            		m_group_helper.removeFromGroup(gn[i], coface);

                // on ajoute le groupe
				m_group_helper.addToGroup(m_groupName, coface);
            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }

    		// la face n'est peut-être plus visible / groupe visibles
			getInfoCommand().addTopoInfoEntity(coface,Internal::InfoCommand::VISIBILYCHANGED);

    	} // end for iter
    }
    break;
    case(3):{
    	for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
    			iter != m_geom_entities.end(); ++iter){
    		Geom::Volume* vol =  dynamic_cast<Geom::Volume*>(*iter);
    		CHECK_NULL_PTR_ERROR(vol);
    		if (m_ope == add){
    			// on ajoute le groupe
				Group::Group3D* grp = m_group_helper.addToGroup<Group::Group3D>(m_groupName, vol);
    			updateMesh(vol, grp->getName(), true);
    		}
    		else if (m_ope == remove) {
    			// on retire le groupe
				Group::Group3D* grp = m_group_helper.removeFromGroup<Group::Group3D>(m_groupName, vol);
    			updateMesh(vol, grp->getName(), false);
    		} else if (m_ope == set){
            	// on retire tous les groupes
            	std::vector<std::string> gn;
            	vol->getGroupsName(gn);
            	for (uint i=0; i<gn.size(); i++)
            		m_group_helper.removeFromGroup(gn[i], vol);

                // on ajoute le groupe
				Group::Group3D* grp = m_group_helper.addToGroup<Group::Group3D>(m_groupName, vol);
                updateMesh(vol, grp->getName(), true);
            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }
    	} // end for iter

    	for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
    			iter != m_topo_entities.end(); ++iter){
    		Topo::Block* bloc =  dynamic_cast<Topo::Block*>(*iter);
    		CHECK_NULL_PTR_ERROR(bloc);
    		std::vector<Topo::Block*> blocs = {bloc};
    		if (m_ope == add){
    			// on ajoute le groupe
				Group::Group3D* grp = m_group_helper.addToGroup<Group::Group3D>(m_groupName, bloc);
    			updateMesh(blocs, grp->getName(), true);
    		}
    		else if (m_ope == remove) {
    			// on retire le groupe
				Group::Group3D* grp = m_group_helper.removeFromGroup<Group::Group3D>(m_groupName, bloc);
    			updateMesh(blocs, grp->getName(), false);
    		} else if (m_ope == set){
            	// on retire tous les groupes
            	std::vector<std::string> gn;
            	bloc->getGroupsName(gn);
            	for (uint i=0; i<gn.size(); i++)
            		m_group_helper.removeFromGroup(gn[i], bloc);

                // on ajoute le groupe
				Group::Group3D* grp = m_group_helper.addToGroup<Group::Group3D>(m_groupName, bloc);
                updateMesh(blocs, grp->getName(), true);
            } else {
            	TkUtil::Exception ("[Erreur interne] Opération non prévue");
            }

    		// le bloc n'est peut-être plus visible / groupe visibles
			getInfoCommand().addTopoInfoEntity(bloc,Internal::InfoCommand::VISIBILYCHANGED);

    	} // end for iter
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
	// on commence par remettre les anciens groupes
    getInfoCommand().permCreatedDeleted();

    // suppression de ce qui a été ajouté
    deleteCreatedMeshEntities();

    // permute toutes les propriétés internes avec leur sauvegarde
    permInternalsStats();

	switch(m_dim){
	case(0):{
		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Vertex* vtx =  dynamic_cast<Geom::Vertex*>(*iter);
			CHECK_NULL_PTR_ERROR(vtx);

			std::vector<std::string> gnOld;
			vtx->getGroupsName(gnOld);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++) {
				m_group_helper.removeFromGroup(gnOld[i], vtx);
			}

			std::vector<std::string>& gnNew = m_geom_groups[vtx];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++) {
				m_group_helper.addToGroup(gnNew[i], vtx);
			}
		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::Vertex* te =  dynamic_cast<Topo::Vertex*>(*iter);
			CHECK_NULL_PTR_ERROR(te);

			std::vector<std::string> gnOld;
			te->getGroupsName(gnOld, false, true);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++) {
				m_group_helper.removeFromGroup(gnOld[i], te);
			}

			std::vector<std::string>& gnNew = m_topo_groups[te];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++) {
				m_group_helper.addToGroup(gnNew[i], te);
			}
		} // end for iter
	}
	break;
	case(1):{
		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Curve* crv =  dynamic_cast<Geom::Curve*>(*iter);
			CHECK_NULL_PTR_ERROR(crv);

			std::vector<std::string> gnOld;
			crv->getGroupsName(gnOld);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++) {
				Group::Group1D* grp = m_group_helper.removeFromGroup<Group::Group1D>(gnOld[i], crv);
				updateMesh(crv, grp->getName(), false);
			}

			std::vector<std::string>& gnNew = m_geom_groups[crv];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++) {
				Group::Group1D* grp = m_group_helper.addToGroup<Group::Group1D>(gnNew[i], crv);
				updateMesh(crv, grp->getName(), true);
			}
		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::CoEdge* coedge =  dynamic_cast<Topo::CoEdge*>(*iter);
			CHECK_NULL_PTR_ERROR(coedge);
    		std::vector<Topo::CoEdge*> coedges = {coedge};

			std::vector<std::string> gnOld;
			coedge->getGroupsName(gnOld, false, true);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++) {
				Group::Group1D* grp = m_group_helper.removeFromGroup<Group::Group1D>(gnOld[i], coedge);
				updateMesh(coedges, grp->getName(), false);
			}

			std::vector<std::string>& gnNew = m_topo_groups[coedge];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++) {
				Group::Group1D* grp = m_group_helper.addToGroup<Group::Group1D>(gnNew[i], coedge);
				updateMesh(coedges, grp->getName(), true);
			}
		} // end for iter
	}
	break;
	case(2):{
		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Surface* srf =  dynamic_cast<Geom::Surface*>(*iter);
			CHECK_NULL_PTR_ERROR(srf);

			std::vector<std::string> gnOld;
			srf->getGroupsName(gnOld);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++) {
				Group::Group2D* grp = m_group_helper.removeFromGroup<Group::Group2D>(gnOld[i], srf);
				updateMesh(srf, grp->getName(), false);
			}

			std::vector<std::string>& gnNew = m_geom_groups[srf];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++) {
				Group::Group2D* grp = m_group_helper.addToGroup<Group::Group2D>(gnNew[i], srf);
				updateMesh(srf, grp->getName(), true);
			}
		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::CoFace* coface =  dynamic_cast<Topo::CoFace*>(*iter);
			CHECK_NULL_PTR_ERROR(coface);
    		std::vector<Topo::CoFace*> cofaces = {coface};

			std::vector<std::string> gnOld;
			coface->getGroupsName(gnOld, false, true);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++) {
				Group::Group2D* grp = m_group_helper.removeFromGroup<Group::Group2D>(gnOld[i], coface);
				updateMesh(cofaces, grp->getName(), false);
			}

			std::vector<std::string>& gnNew = m_topo_groups[coface];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++) {
				Group::Group2D* grp = m_group_helper.addToGroup<Group::Group2D>(gnNew[i], coface);
				updateMesh(cofaces, grp->getName(), true);
			}
		} // end for iter
	}
	break;
	case(3):{
		for (std::vector<Geom::GeomEntity*>::iterator iter = m_geom_entities.begin();
				iter != m_geom_entities.end(); ++iter){
			Geom::Volume* vol =  dynamic_cast<Geom::Volume*>(*iter);
			CHECK_NULL_PTR_ERROR(vol);

			std::vector<std::string> gnOld;
			vol->getGroupsName(gnOld);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++) {
				Group::Group3D* grp = m_group_helper.removeFromGroup<Group::Group3D>(gnOld[i], vol);
				updateMesh(vol, grp->getName(), false);
			}

			std::vector<std::string>& gnNew = m_geom_groups[vol];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++) {
				Group::Group3D* grp = m_group_helper.addToGroup<Group::Group3D>(gnNew[i], vol);
				updateMesh(vol, grp->getName(), true);
			}
		} // end for iter

		for (std::vector<Topo::TopoEntity*>::iterator iter = m_topo_entities.begin();
				iter != m_topo_entities.end(); ++iter){
			Topo::Block* bloc =  dynamic_cast<Topo::Block*>(*iter);
			CHECK_NULL_PTR_ERROR(bloc);
    		std::vector<Topo::Block*> blocs = {bloc};

			std::vector<std::string> gnOld;
			bloc->getGroupsName(gnOld, false, true);
			// on retire tous les groupes actuellement présents
			for (uint i=0; i<gnOld.size(); i++) {
				Group::Group3D* grp = m_group_helper.removeFromGroup<Group::Group3D>(gnOld[i], bloc);
				updateMesh(blocs, grp->getName(), false);
			}

			std::vector<std::string>& gnNew = m_topo_groups[bloc];
			// on ajoute tous les groupes archivés
			for (uint i=0; i<gnNew.size(); i++) {
				Group::Group3D* grp = m_group_helper.addToGroup<Group::Group3D>(gnNew[i], bloc);
				updateMesh(blocs, grp->getName(), true);
			}
		} // end for iter
	}
	break;

	default:{
		TkUtil::Exception ("[Erreur interne] Dimension erronée");
	}
	break;
	}
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
	Topo::TopoManager& tm = getContext().getTopoManager();
	std::vector<Topo::Block*> blocs = tm.getFilteredRefTopos<Topo::Block>(vol);
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
    for (Topo::Block* blk : blocs){
        if (blk->isMeshed())
            is_meshed = true;
    }

    if (is_meshed){
        // recherche du Mesh::Volume
        bool isNewVolume = false;
        try {
#ifdef _DEBUG_UPDATE
            std::cout<<"recherche getVolume "<<grpName<<std::endl;
#endif
            getContext().getMeshManager().getVolume(grpName);
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
        Mesh::Volume* mvol = getContext().getMeshManager().getVolume(grpName);

        for (Topo::Block* blk : blocs)
            if (blk->isMeshed()){
                if (add){
#ifdef _DEBUG_UPDATE
                    std::cout<<" "<<mvol->getName()<<" addBlock("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    mvol->addBlock(blk);
                }
                else {
#ifdef _DEBUG_UPDATE
                    std::cout<<" "<<mvol->getName()<<" removeBlock("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    mvol->removeBlock(blk);
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
	Topo::TopoManager& tm = getContext().getTopoManager();
	std::vector<Topo::CoFace*> cofaces = tm.getFilteredRefTopos<Topo::CoFace>(surf);
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
            getContext().getMeshManager().getSurface(grpName);
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
        Mesh::Surface* msurf = getContext().getMeshManager().getSurface(grpName);

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
	Topo::TopoManager& tm = getContext().getTopoManager();
	std::vector<Topo::CoEdge*> coedges = tm.getFilteredRefTopos<Topo::CoEdge>(crv);
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
            getContext().getMeshManager().getCloud(grpName);
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
        Mesh::Cloud* mcld = getContext().getMeshManager().getCloud(grpName);

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
            getContext().getMeshManager().getLine(grpName);
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
        Mesh::Line* mln = getContext().getMeshManager().getLine(grpName);

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
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
