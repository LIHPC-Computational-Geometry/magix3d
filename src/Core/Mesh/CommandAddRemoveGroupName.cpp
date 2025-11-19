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
	Group::GroupManager& gm = getContext().getGroupManager();

	for (Geom::GeomEntity* ge : m_geom_entities) {
		// archivage des groupes initiaux
        std::vector<Group::GroupEntity*> groups = gm.getGroupsFor(ge);
		m_geom_groups[ge] = Utils::toNames(groups);

		if (m_ope == add){
			// on ajoute le groupe
			Group::GroupEntity* grp = m_group_helper.addToGroup(m_groupName, ge);
			updateMesh(ge, grp->getName(), true);
		}
		else if (m_ope == remove) {
			// on retire le groupe
			Group::GroupEntity* grp = m_group_helper.removeFromGroup(m_groupName, ge);
			updateMesh(ge, grp->getName(), false);
		} else if (m_ope == set){
			// on retire tous les groupes
			for (Group::GroupEntity* grp : groups)
				m_group_helper.removeFromGroup(grp->getName(), ge);

			// on ajoute le groupe
			Group::GroupEntity* grp = m_group_helper.addToGroup(m_groupName, ge);
			updateMesh(ge, grp->getName(), true);
		} else {
			TkUtil::Exception ("[Erreur interne] Opération non prévue");
		}
	}

	for (Topo::TopoEntity* te : m_topo_entities) {
		// pour la topologie, on n'utilise pas le mécanisme de sauvegarde
        std::vector<Group::GroupEntity*> groups = m_group_helper.getGroupsFor(te);
		m_topo_groups[te] = Utils::toNames(groups);

		if (m_ope == add){
			// on ajoute le groupe
			Group::GroupEntity* grp = m_group_helper.addToGroup(m_groupName, te);
			updateMesh(te, grp->getName(), true);
		}
		else if (m_ope == remove) {
			// on retire le groupe
			Group::GroupEntity* grp = m_group_helper.removeFromGroup(m_groupName, te);
			updateMesh(te, grp->getName(), false);
		} else if (m_ope == set){
			// on retire tous les groupes
			for (Group::GroupEntity* grp : groups)
				m_group_helper.removeFromGroup(grp->getName(), te);

			// on ajoute le groupe
			Group::GroupEntity* grp = m_group_helper.addToGroup(m_groupName, te);
			updateMesh(te, grp->getName(), true);
		} else {
			TkUtil::Exception ("[Erreur interne] Opération non prévue");
		}

		getInfoCommand().addTopoInfoEntity(te, Internal::InfoCommand::VISIBILYCHANGED);
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

	Group::GroupManager& gm = getContext().getGroupManager();

	for (Geom::GeomEntity* ge : m_geom_entities) {
		for (Group::GroupEntity* grp : gm.getGroupsFor(ge)) {
			m_group_helper.removeFromGroup(grp->getName(), ge);
			updateMesh(ge, grp->getName(), false);
		}

		// on ajoute tous les groupes archivés
		for (std::string grp_name : m_geom_groups[ge]) {
			m_group_helper.addToGroup(grp_name, ge);
			updateMesh(ge, grp_name, true);
		}
	}

	for (Topo::TopoEntity* te : m_topo_entities){
		for (Group::GroupEntity* grp : m_group_helper.getGroupsFor(te)) {
			m_group_helper.removeFromGroup(grp->getName(), te);
			updateMesh(te, grp->getName(), false);
		}

		// on ajoute tous les groupes archivés
		for (std::string grp_name : m_topo_groups[te]) {
			m_group_helper.addToGroup(grp_name, te);
			updateMesh(te, grp_name, true);
		}
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
updateMesh(Geom::GeomEntity* e, std::string grpName, bool add)
{
    switch (e->getDim())
    {
        case 1: return updateMesh(dynamic_cast<Geom::Curve*>(e), grpName, add);
        case 2: return updateMesh(dynamic_cast<Geom::Surface*>(e), grpName, add);
        case 3: return updateMesh(dynamic_cast<Geom::Volume*>(e), grpName, add);
		default: return; // rien à faire en 0D
    }
}
/*----------------------------------------------------------------------------*/
void CommandAddRemoveGroupName::
updateMesh(Topo::TopoEntity* e, std::string grpName, bool add)
{
    switch (e->getDim())
    {
        case 1: {
            std::vector<Topo::CoEdge*> v = { dynamic_cast<Topo::CoEdge*>(e) };
            return updateMesh(v, grpName, add);
        }
        case 2: {
            std::vector<Topo::CoFace*> v = { dynamic_cast<Topo::CoFace*>(e) };
            return updateMesh(v, grpName, add);
        }
        case 3: {
            std::vector<Topo::Block*> v = { dynamic_cast<Topo::Block*>(e) };
            return updateMesh(v, grpName, add);
        }
		default: return; // rien à faire en 0D
    }
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
                    std::cout<<" "<<mvol->getName()<<" add("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    mvol->add(blk);
                }
                else {
#ifdef _DEBUG_UPDATE
                    std::cout<<" "<<mvol->getName()<<" remove("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    mvol->remove(blk);
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
                   std::cout<<" "<<msurf->getName()<<" add("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    msurf->add(*iter);
                }
                else {
#ifdef _DEBUG_UPDATE
                    std::cout<<" "<<msurf->getName()<<" remove("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    msurf->remove(*iter);
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
                   std::cout<<" "<<mcld->getName()<<" add("<<(*iter)->getName()<<")"<<std::endl;
#endif
                   mcld->add(*iter);
                }
                else {
#ifdef _DEBUG_UPDATE
                    std::cout<<" "<<mcld->getName()<<" remove("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    mcld->remove(*iter);
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
                   std::cout<<" "<<mln->getName()<<" add("<<(*iter)->getName()<<")"<<std::endl;
#endif
                   mln->add(*iter);
                }
                else {
#ifdef _DEBUG_UPDATE
                    std::cout<<" "<<mcld->getName()<<" remove("<<(*iter)->getName()<<")"<<std::endl;
#endif
                    mln->remove(*iter);
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
