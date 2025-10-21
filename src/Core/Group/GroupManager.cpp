/*----------------------------------------------------------------------------*/
// pythonerie à mettre au début (pour permettre ifndef Py_PYTHON_H dans GroupManager.h)
#include <set>
/*----------------------------------------------------------------------------*/
#include "Group/GroupManager.h"
#include "Group/GroupEntity.h"
#include "Group/CommandAddMeshModification.h"

#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
#include "Internal/InternalPreferences.h"
#include "Internal/M3DCommandResult.h"

#include "Utils/Entity.h"
#include "Utils/Rotation.h"
#include "Utils/DisplayProperties.h"
#include "Utils/CommandManager.h"

#include "Geom/GeomEntity.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/IncidentGeomEntitiesVisitor.h"

#include "Topo/TopoEntity.h"
#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
#include "Topo/TopoHelper.h"

#include "Mesh/MeshEntity.h"
#include "Mesh/MeshManager.h"
#include "Mesh/Volume.h"
#include "Mesh/Surface.h"
#include "Mesh/Line.h"
#include "Mesh/Cloud.h"
#include "Mesh/MeshModificationByPythonFunction.h"
#include "Mesh/MeshModificationBySepa.h"
#include "Mesh/MeshModificationByProjectionOnP0.h"
#include "Mesh/CommandClearGroupName.h"
#include "Mesh/CommandChangeGroupName.h"
#include "SysCoord/SysCoord.h"

#include "Smoothing/SurfacicSmoothing.h"
#include "Smoothing/VolumicSmoothing.h"

#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
GroupManager::GroupManager(const std::string& name, Internal::Context* c)
:Internal::CommandCreator(name, c)
, m_visibilityMask(Utils::FilterEntity::NoneEntity)
{
}
/*----------------------------------------------------------------------------*/
GroupManager::~GroupManager()
{
//	std::cout<<"GroupManager::~GroupManager()"<<std::endl;
	clear();
}
/*----------------------------------------------------------------------------*/
void GroupManager::clear()
{
    // destruction des entités référencées par le manager
    for (std::vector<GroupEntity*>::const_iterator iter = m_groups.begin();
            iter != m_groups.end(); ++iter)
        delete *iter;
    m_groups.clear();
}
/*------------------------------------------------------------------------*/
/** Vide un groupe suivant son nom et une dimension */
Internal::M3DCommandResult* GroupManager::clearGroup(int dim, const std::string& groupName)
{
	Mesh::CommandClearGroupName* command =
			new Mesh::CommandClearGroupName(getContext(), dim, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().clearGroup ("<<(short)dim<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResult*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
bool GroupManager::getPropagate()
{
	return (true==Internal::InternalPreferences::instance ( )._propagateDownEntityShow.getValue( ));
}
/*----------------------------------------------------------------------------*/
std::string GroupManager::getInfos(const std::string& name, int dim) const
{
    return getGroup(name, dim)->getInfos();
}
/*----------------------------------------------------------------------------*/
std::string GroupManager::getDefaultName(int dim) const
{
//    return "Hors Groupe";
    switch(dim){
    case(3):{
        return "Hors_Groupe_3D";
    }
    break;
    case(2):{
        return "Hors_Groupe_2D";
    }
    break;
    case(1):{
        return "Hors_Groupe_1D";
    }
    break;
    case(0):{
        return "Hors_Groupe_0D";
    }
    break;
    default:
        throw TkUtil::Exception (TkUtil::UTF8String ("dimension non prévue pour GroupManager::getDefaultName", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResult*
GroupManager::changeGroupName(const std::string& oldName, const std::string& newName, int dim)
{
	Mesh::CommandChangeGroupName* command =
			new Mesh::CommandChangeGroupName(getContext(), oldName, newName, dim);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().changeGroupName (\""<<oldName<<"\", \""<<newName<<"\", "<<(short)dim<<")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResult*  cmdResult   =
    		new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
template <typename T, typename = std::enable_if_t<std::is_base_of<GroupEntity, T>::value>>
T* GroupManager::getGroup(const std::string& gr_name, const bool exceptionIfNotFound) const
{
    std::string name(gr_name.empty()?getDefaultName(T::DIM):gr_name);

    T* gr = 0;
    for (GroupEntity* g : m_groups)
        if (g->getName() == name)
            if (T* casted = dynamic_cast<T*>(g))
                return casted;

    if (exceptionIfNotFound){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr <<"On ne trouve pas "<<name<<" dans le GroupManager";
        throw TkUtil::Exception(messErr);
    }

    return gr;
}
/*----------------------------------------------------------------------------*/
template <typename T, typename = std::enable_if_t<std::is_base_of<GroupEntity, T>::value>>
T* GroupManager::getNewGroup(const std::string& gr_name, Internal::InfoCommand* icmd)
{
    T* gr = getGroup<T>(gr_name, false);

    if (gr == 0){
        std::string name(gr_name.empty()?getDefaultName(T::DIM):gr_name);
        gr = new T(getContext(), name, gr_name.empty());
        m_groups.push_back(gr);
        if (icmd)
            icmd->addGroupInfoEntity(gr,Internal::InfoCommand::CREATED);
    }
    else if (gr->isDestroyed()) {
        gr->setDestroyed(false);
        if (icmd)
            icmd->addGroupInfoEntity(gr,Internal::InfoCommand::ENABLE);
    }

    return gr;
}
/*----------------------------------------------------------------------------*/
template <typename T, typename = std::enable_if_t<std::is_base_of<GroupEntity, T>::value>>
std::vector<T*> GroupManager::getGroups(const bool onlyLive) const
{
    std::vector<T*> groups;
    for (GroupEntity* g : m_groups)
        if (T* casted = dynamic_cast<T*>(g))
            if (!onlyLive || !casted->isDestroyed())
                groups.push_back(casted);
    return groups;
}
/*----------------------------------------------------------------------------*/
std::vector<GroupEntity*> GroupManager::getGroups(Internal::SelectionManager::DIM dimensions, const bool onlyLive) const
{
    std::vector<GroupEntity*> groups;
	for (int i = 0; i < 4; i++){
		const Internal::SelectionManager::DIM dim = Internal::SelectionManager::dimensionToDimensions(i);
		if (0 == (dimensions&dim))
			continue;
		switch (i)
		{
			case 0 :
				for (Group0D* g : getGroups<Group0D>(onlyLive))
					groups.push_back (g);
    			break;
			case 1 :
				for (Group1D* g : getGroups<Group1D>(onlyLive))
					groups.push_back (g);
    			break;
			case 2 :
				for (Group2D* g : getGroups<Group2D>(onlyLive))
					groups.push_back (g);
    			break;
			case 3 :
				for (Group3D* g : getGroups<Group3D>(onlyLive))
					groups.push_back (g);
    			break;
		}
	}
    return groups;
}
/*----------------------------------------------------------------------------*/
void GroupManager::getAddedShownAndHidden(
        const std::vector<Group::GroupEntity*>& groupAddedShown,
        const std::vector<Group::GroupEntity*>& groupAddedHidden,
        std::vector<Geom::GeomEntity*>& geomAddedShown,
        std::vector<Geom::GeomEntity*>& geomAddedHidden,
        std::vector<Topo::TopoEntity*>& topoAddedShown,
        std::vector<Topo::TopoEntity*>& topoAddedHidden,
        std::vector<Mesh::MeshEntity*>& meshAddedShown,
        std::vector<Mesh::MeshEntity*>& meshAddedHidden,
		std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedShown,
		std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedHidden)
{
//#define _DEBUG_ADDED
#ifdef _DEBUG_ADDED
    std::cout<<"GroupManager::getAddedShownAndHidden, visibilityMask = "<<m_visibilityMask
            <<" == "<<Utils::FilterEntity::filterToObjectTypeName((Utils::FilterEntity::objectType)m_visibilityMask)<<std::endl;
    std::cout<<" groupAddedShown :";
    if (groupAddedShown.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<groupAddedShown.size(); i++)
        std::cout<<" "<<groupAddedShown[i]->getName();
    std::cout<<std::endl;
    std::cout<<" groupAddedHidden :";
    if (groupAddedHidden.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<groupAddedHidden.size(); i++)
        std::cout<<" "<<groupAddedHidden[i]->getName();
    std::cout<<std::endl;
#endif

    // réinitialisation
    geomAddedShown.clear();
    geomAddedHidden.clear();
    topoAddedShown.clear();
    topoAddedHidden.clear();
    meshAddedShown.clear();
    meshAddedHidden.clear();
    sysCoordAddedShown.clear();
	sysCoordAddedHidden.clear();

    // cas sans rien de modifié dans la visibilité
    if (groupAddedShown.empty() && groupAddedHidden.empty())
        return;

    // utilisation de filtres (masques) pour la géométrie et la topologie
    // c'est à 1 lorsque c'est visible initialement
    // c'est à 2 lorsque c'est à rendre invisible
    // c'est à 4 lorsque c'est à rendre visible

    std::map<Geom::GeomEntity*, uint> filtre_geom;
    std::map<Topo::TopoEntity*, uint> filtre_topo;
    std::map<CoordinateSystem::SysCoord*, uint> filtre_rep;

    // parcours tous les groupes initiaux pour repérer les entités visibles initiallement
    addMarkAllGroups(m_visibilityMask, filtre_geom, filtre_topo, filtre_rep, 1);

    // mise à jour de la visibilités des groupes
    for (std::vector<Group::GroupEntity*>::const_iterator iter = groupAddedHidden.begin();
            iter != groupAddedHidden.end(); ++iter)
        (*iter)->getDisplayProperties().setDisplayed(false); //m_isVisible = false;

    for (std::vector<Group::GroupEntity*>::const_iterator iter = groupAddedShown.begin();
            iter != groupAddedShown.end(); ++iter)
        (*iter)->getDisplayProperties().setDisplayed(true); //m_isVisible = true;

    if (groupAddedHidden.empty()){
        // parcours les groupes à rendre visible
        addMark(groupAddedShown, m_visibilityMask, filtre_geom, filtre_topo, filtre_rep, 4);
    }
    else {
        // parcours les groupes à masquer
        addMark(groupAddedHidden, m_visibilityMask, filtre_geom, filtre_topo, filtre_rep, 2);

        // parcours tous les groupes pour repérer les entités visibles à la fin
        addMarkAllGroups(m_visibilityMask, filtre_geom, filtre_topo, filtre_rep, 4);
    }


    // ajoute les groupes du maillages
    addMeshGroups(groupAddedShown, m_visibilityMask, meshAddedShown);
    addMeshGroups(groupAddedHidden, m_visibilityMask, meshAddedHidden);


#ifdef _DEBUG2
    std::cout<<"Remplissage de geomAddedShown et geomAddedHidden\n";
#endif
    // parcours du filtre géométrique pour remplir les vecteurs d'entités
    for (std::map<Geom::GeomEntity*, uint>::iterator iter = filtre_geom.begin();
            iter != filtre_geom.end(); ++iter){
        Geom::GeomEntity* ge = iter->first;
        bool initVisible = iter->second & 1;
        bool addedHidden = iter->second & 2;
        bool addedShown  = iter->second & 4;
#ifdef _DEBUG2
        std::cout<<"ge = "<<ge->getName()<<", marque = "<<iter->second
                <<", initVisible = "<<(initVisible?"vrai":"faux")
                <<", addedHidden = "<<(addedHidden?"vrai":"faux")
                <<", addedShown = "<<(addedShown?"vrai":"faux")
                <<std::endl;
#endif

        if (addedShown && !initVisible)
            geomAddedShown.push_back(ge);
        else if (addedHidden && !addedShown && initVisible)
            geomAddedHidden.push_back(ge);
    }

    // parcours du filtre topologique pour remplir les vecteurs d'entités
    for (std::map<Topo::TopoEntity*, uint>::iterator iter = filtre_topo.begin();
            iter != filtre_topo.end(); ++iter){
        Topo::TopoEntity* te = iter->first;
        bool initVisible = iter->second & 1;
        bool addedHidden = iter->second & 2;
        bool addedShown  = iter->second & 4;

        if (addedShown && !initVisible)
            topoAddedShown.push_back(te);
        else if (addedHidden && !addedShown && initVisible)
            topoAddedHidden.push_back(te);
    }

    // parcours du filtre des repères pour remplir les vecteurs d'entités
    for (std::map<CoordinateSystem::SysCoord*, uint>::iterator iter = filtre_rep.begin();
            iter != filtre_rep.end(); ++iter){
    	CoordinateSystem::SysCoord* rep = iter->first;
        bool initVisible = iter->second & 1;
        bool addedHidden = iter->second & 2;
        bool addedShown  = iter->second & 4;

        if (addedShown && !initVisible)
            sysCoordAddedShown.push_back(rep);
        else if (addedHidden && !addedShown && initVisible)
        	sysCoordAddedHidden.push_back(rep);
    }


#ifdef _DEBUG_ADDED
    std::cout<<"  ====>>>>> "<<std::endl;
    std::cout<<" geomAddedShown :";
    if (geomAddedShown.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<geomAddedShown.size(); i++)
        std::cout<<" "<<geomAddedShown[i]->getName();
    std::cout<<std::endl;
    std::cout<<" geomAddedHidden :";
    if (geomAddedHidden.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<geomAddedHidden.size(); i++)
        std::cout<<" "<<geomAddedHidden[i]->getName();
    std::cout<<std::endl;

    std::cout<<" topoAddedShown :";
    if (topoAddedShown.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<topoAddedShown.size(); i++)
        std::cout<<" "<<topoAddedShown[i]->getName();
    std::cout<<std::endl;
    std::cout<<" topoAddedHidden :";
    if (topoAddedHidden.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<topoAddedHidden.size(); i++)
        std::cout<<" "<<topoAddedHidden[i]->getName();
    std::cout<<std::endl;

    std::cout<<" meshAddedShown :";
    if (meshAddedShown.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<meshAddedShown.size(); i++)
        std::cout<<" "<<meshAddedShown[i]->getName();
    std::cout<<std::endl;
    std::cout<<" meshAddedHidden :";
    if (meshAddedHidden.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<meshAddedHidden.size(); i++)
        std::cout<<" "<<meshAddedHidden[i]->getName();
    std::cout<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void GroupManager::getAddedShownAndHidden(
        const Utils::FilterEntity::objectType newVisibilityMask,
        std::vector<Geom::GeomEntity*>& geomAddedShown,
        std::vector<Geom::GeomEntity*>& geomAddedHidden,
        std::vector<Topo::TopoEntity*>& topoAddedShown,
        std::vector<Topo::TopoEntity*>& topoAddedHidden,
        std::vector<Mesh::MeshEntity*>& meshAddedShown,
        std::vector<Mesh::MeshEntity*>& meshAddedHidden,
		std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedShown,
		std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedHidden)
{
#ifdef _DEBUG_ADDED
    std::cout<<"GroupManager::getAddedShownAndHidden, oldVisibilityMask = "<<m_visibilityMask
             <<", newVisibilityMask = "<<newVisibilityMask
             <<std::endl;
    std::cout<<"oldVisibilityMask = "
             <<Utils::FilterEntity::filterToObjectTypeName((Utils::FilterEntity::objectType)m_visibilityMask)
             <<std::endl;
    std::cout<<"newVisibilityMask = "
             <<Utils::FilterEntity::filterToObjectTypeName((Utils::FilterEntity::objectType)newVisibilityMask)
             <<std::endl;
#endif

    // réinitialisation
    geomAddedShown.clear();
    geomAddedHidden.clear();
    topoAddedShown.clear();
    topoAddedHidden.clear();
    meshAddedShown.clear();
    meshAddedHidden.clear();
    sysCoordAddedShown.clear();
	sysCoordAddedHidden.clear();

    // masque de changement
    Utils::FilterEntity::objectType changeMask = (Utils::FilterEntity::objectType)(m_visibilityMask ^ newVisibilityMask);

    // masque de ce qui est à rendre visible
    Utils::FilterEntity::objectType shownMask = (Utils::FilterEntity::objectType)(newVisibilityMask & changeMask);

    // masque de ce qui est à cacher
    Utils::FilterEntity::objectType hiddenMask = (Utils::FilterEntity::objectType)(m_visibilityMask & changeMask);

    // cas sans rien de modifié dans la visibilité
    if (changeMask == 0)
        return;

    // utilisation de filtres (masques) pour la géométrie et la topologie
    // c'est à 2 lorsque c'est à rendre invisible
    // c'est à 4 lorsque c'est à rendre visible

    std::map<Geom::GeomEntity*, uint> filtre_geom;
    std::map<Topo::TopoEntity*, uint> filtre_topo;
    std::map<CoordinateSystem::SysCoord*, uint> filtre_rep;

    // parcours tous les groupes pour repérer les entités à rendre invisible
    addMarkAllGroups(hiddenMask, filtre_geom, filtre_topo, filtre_rep, 2);

    // parcours tous les groupes pour repérer les entités à rendre visible
    addMarkAllGroups(shownMask, filtre_geom, filtre_topo, filtre_rep, 4);

    // ajoute les groupes du maillages
    addMeshGroups(hiddenMask, meshAddedHidden);
    addMeshGroups(shownMask, meshAddedShown);


    // parcours du filtre géométrique pour remplir les vecteurs d'entités
    for (std::map<Geom::GeomEntity*, uint>::iterator iter = filtre_geom.begin();
            iter != filtre_geom.end(); ++iter){
        Geom::GeomEntity* ge = iter->first;
        bool addedHidden = iter->second & 2;
        bool addedShown  = iter->second & 4;

        if (addedShown)
            geomAddedShown.push_back(ge);
        else if (addedHidden)
            geomAddedHidden.push_back(ge);
    }

    // parcours du filtre topologique pour remplir les vecteurs d'entités
    for (std::map<Topo::TopoEntity*, uint>::iterator iter = filtre_topo.begin();
            iter != filtre_topo.end(); ++iter){
        Topo::TopoEntity* te = iter->first;
        bool addedHidden = iter->second & 2;
        bool addedShown  = iter->second & 4;

        if (addedShown)
            topoAddedShown.push_back(te);
        else if (addedHidden)
            topoAddedHidden.push_back(te);
    }

    // parcours du filtre des repères pour remplir les vecteurs d'entités
    for (std::map<CoordinateSystem::SysCoord*, uint>::iterator iter = filtre_rep.begin();
            iter != filtre_rep.end(); ++iter){
    	CoordinateSystem::SysCoord* rep = iter->first;
        bool addedHidden = iter->second & 2;
        bool addedShown  = iter->second & 4;

        if (addedShown)
            sysCoordAddedShown.push_back(rep);
        else if (addedHidden)
        	sysCoordAddedHidden.push_back(rep);
    }

    // on stocke le masque
    m_visibilityMask = newVisibilityMask;

#ifdef _DEBUG_ADDED
    std::cout<<"  ====>>>>> "<<std::endl;
    std::cout<<" geomAddedShown :";
    if (geomAddedShown.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<geomAddedShown.size(); i++)
        std::cout<<" "<<geomAddedShown[i]->getName();
    std::cout<<std::endl;
    std::cout<<" geomAddedHidden :";
    if (geomAddedHidden.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<geomAddedHidden.size(); i++)
        std::cout<<" "<<geomAddedHidden[i]->getName();
    std::cout<<std::endl;

    std::cout<<" topoAddedShown :";
    if (topoAddedShown.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<topoAddedShown.size(); i++)
        std::cout<<" "<<topoAddedShown[i]->getName();
    std::cout<<std::endl;
    std::cout<<" topoAddedHidden :";
    if (topoAddedHidden.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<topoAddedHidden.size(); i++)
        std::cout<<" "<<topoAddedHidden[i]->getName();
    std::cout<<std::endl;

    std::cout<<" meshAddedShown :";
    if (meshAddedShown.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<meshAddedShown.size(); i++)
        std::cout<<" "<<meshAddedShown[i]->getName();
    std::cout<<std::endl;
    std::cout<<" meshAddedHidden :";
    if (meshAddedHidden.empty())
        std::cout<<" (vide)";
    for (uint i=0; i<meshAddedHidden.size(); i++)
        std::cout<<" "<<meshAddedHidden[i]->getName();
    std::cout<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void GroupManager::getShownEntities(
		const Utils::FilterEntity::objectType visibilityMask,
		std::vector<Utils::Entity*>& entities)
{
#ifdef _DEBUG2
    std::cout<<"GroupManager::getShownEntities(Mask = "
            <<Utils::FilterEntity::filterToObjectTypeName((Utils::FilterEntity::objectType)visibilityMask)
            <<")"<<std::endl;
#endif
	entities.clear ( );

    // utilisation de filtres (masques) pour la géométrie et la topologie
    // c'est à 1 lorsque c'est visible suivant le masque

    std::map<Geom::GeomEntity*, uint> filtre_geom;
    std::map<Topo::TopoEntity*, uint> filtre_topo;
    std::map<CoordinateSystem::SysCoord*, uint> filtre_rep;

    // parcours tous les groupes initiaux pour repérer les entités visibles
    addMarkAllGroups(visibilityMask, filtre_geom, filtre_topo, filtre_rep, 1);

    // parcours du filtre géométrique pour remplir les vecteurs d'entités
    for (std::map<Geom::GeomEntity*, uint>::iterator iter = filtre_geom.begin();
            iter != filtre_geom.end(); ++iter){
        Geom::GeomEntity* ge = iter->first;
        bool initVisible = iter->second & 1;
        if (initVisible)
            entities.push_back(ge);
    }

    // parcours du filtre topologique pour remplir les vecteurs d'entités
    for (std::map<Topo::TopoEntity*, uint>::iterator iter = filtre_topo.begin();
            iter != filtre_topo.end(); ++iter){
        Topo::TopoEntity* te = iter->first;
        bool initVisible = iter->second & 1;
        if (initVisible)
            entities.push_back(te);
    }

    // parcours du filtre des repères pour remplir les vecteurs d'entités
    for (std::map<CoordinateSystem::SysCoord*, uint>::iterator iter = filtre_rep.begin();
            iter != filtre_rep.end(); ++iter){
    	CoordinateSystem::SysCoord* rep = iter->first;
        bool initVisible = iter->second & 1;
        if (initVisible)
        	entities.push_back(rep);
    }

    // recherche les groupes de mailles visibles
    std::vector<Mesh::MeshEntity*> meshShown;
    addMeshGroups(visibilityMask, meshShown);
    entities.insert(entities.end(), meshShown.begin(), meshShown.end());
}
/*----------------------------------------------------------------------------*/
void GroupManager::updateDisplayProperties(Internal::InfoCommand* icmd)
{
#ifdef _DEBUG_ADDED
    std::cout<<"GroupManager::updateDisplayProperties, visibilityMask = "<<m_visibilityMask
             <<std::endl;
    std::cout<<"visibilityMask = "
             <<Utils::FilterEntity::filterToObjectTypeName((Utils::FilterEntity::objectType)m_visibilityMask)
             <<std::endl;
#endif

    // parcours des groupes pour en déduire la plus grande dimension utilisée
    uint dim = 0;
    std::map<Group::GroupEntity*, Internal::InfoCommand::type>& group_entities_info = icmd->getGroupInfoEntity();
    for (std::map<Group::GroupEntity*, Internal::InfoCommand::type>::iterator iter_grp = group_entities_info.begin();
                    iter_grp != group_entities_info.end(); ++iter_grp){
        Group::GroupEntity* grp = iter_grp->first;
        //Internal::InfoCommand::type& t = iter_grp->second;
#ifdef _DEBUG_ADDED
        std::cout<<"   grp = "<<grp->getName()<<" dim = "<<grp->getDim()<<std::endl;
#endif
        if (grp->getDim() > dim)
        	dim = grp->getDim();
    }
#ifdef _DEBUG_ADDED
    std::cout<<" dim (max) = "<<dim<<std::endl;
#endif
    // les groupes de la plus grande dimension rencontrée sont rendus visibles
    for (std::map<Group::GroupEntity*, Internal::InfoCommand::type>::iterator iter_grp = group_entities_info.begin();
    		iter_grp != group_entities_info.end(); ++iter_grp){
    	Group::GroupEntity* grp = iter_grp->first;
    	Internal::InfoCommand::type& t = iter_grp->second;
#ifdef _DEBUG_ADDED
    	std::cout<<" grp = "<<grp->getName()<<", t = "<<Internal::InfoCommand::type2String(t)<<std::endl;
#endif
    	if (grp->getDim() == dim && t == Internal::InfoCommand::CREATED){
    		grp->getDisplayProperties ( ).setDisplayed(true);
#ifdef _DEBUG_ADDED
    		std::cout<<"  setDisplayed(true)"<<std::endl;
#endif
    	}
    }


    // utilisation de filtres (masques) pour la géométrie et la topologie
    // c'est à 1 lorsque c'est visible
    std::map<Geom::GeomEntity*, uint> filtre_geom;
    std::map<Topo::TopoEntity*, uint> filtre_topo;
    std::map<CoordinateSystem::SysCoord*, uint> filtre_rep;

    // parcours tous les groupes initiaux pour repérer les entités visibles
    addMarkAllGroups(m_visibilityMask, filtre_geom, filtre_topo, filtre_rep, 1);

    // on parcours les entités nouvelles/modifiées
    for (uint i=0; i<icmd->getNbGeomInfoEntity(); i++) {
        Geom::GeomEntity* ge = 0;
        Internal::InfoCommand::type t;
        icmd->getGeomInfoEntity(i, ge, t);

        // on passe les entités non représentables et celles détruites
        if (ge->getDisplayProperties ( ).isDisplayable()
                && Internal::InfoCommand::DELETED != t)
            ge->getDisplayProperties ( ).setDisplayed(filtre_geom[ge] == 1);
    } // end for i<icmd->getNbGeomInfoEntity()

    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>& topo_entities_info = icmd->getTopoInfoEntity();

    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
            iter_tei != topo_entities_info.end(); ++iter_tei){
        Topo::TopoEntity* te = (*iter_tei).first;
        Internal::InfoCommand::type t = iter_tei->second;

        // on passe les entités non représentables et celles détruites
        if (te->getDisplayProperties ( ).isDisplayable()
                && Internal::InfoCommand::DELETED != t){
        	if (Internal::InfoCommand::DISPMODIFIED == t
        			|| Internal::InfoCommand::VISIBILYCHANGED == t){
        		bool old_displayed = te->getDisplayProperties ( ).isDisplayed();
        		bool new_displayed = filtre_topo[te] == 1;
#ifdef _DEBUG_ADDED
        		std::cout<<" te = "<<te->getName()<<", t = "<<Internal::InfoCommand::type2String(t)<<", old_displayed "<<old_displayed<<", new_displayed "<<new_displayed<<std::endl;
#endif
        		if (new_displayed != old_displayed){
        			// Comme c'est ici que l'on sait si l'entité doit être
        			// visible ou non, on le met dans la propriété pour
        			// passer le paramètre à la vue
        			te->getDisplayProperties ( ).setDisplayed(new_displayed);
        			iter_tei->second = Internal::InfoCommand::VISIBILYCHANGED;
        		}
        	}
        	else {
        		// pour celles créées
        		te->getDisplayProperties ( ).setDisplayed(filtre_topo[te] == 1);
#ifdef _DEBUG_ADDED
        		std::cout<<" te = "<<te->getName()<<", t = "<<Internal::InfoCommand::type2String(t)<<", filtre à "<<filtre_topo[te]<<std::endl;
#endif
        	}
        }
    } // end for iter_tei

    for (uint i=0; i<icmd->getNbSysCoordInfoEntity(); i++) {
    	CoordinateSystem::SysCoord* rep = 0;
        Internal::InfoCommand::type t;
        icmd->getSysCoordInfoEntity(i, rep, t);

        // on passe les entités non représentables et celles détruites
        if (rep->getDisplayProperties ( ).isDisplayable()
                && Internal::InfoCommand::DELETED != t)
            rep->getDisplayProperties ( ).setDisplayed(filtre_rep[rep] == 1);
    } // end for i<icmd->getNbSysCoordInfoEntity()


    // la visibilité des groupes de mailles
    std::vector<Mesh::MeshEntity*> meshShown;
    addMeshGroups(m_visibilityMask, meshShown);
    for (uint i=0; i<meshShown.size(); i++)
        meshShown[i]->getDisplayProperties ( ).setDisplayed(true);

}
/*----------------------------------------------------------------------------*/
void GroupManager::updateDeletedGroups(Internal::InfoCommand* icmd)
{
#ifdef _DEBUG2
	std::cout<<"GroupManager::updateDeletedGroups ..."<<std::endl;
#endif

    for (GroupEntity* grp : m_groups) {
        if (grp->empty() && !grp->isDestroyed()) {
            grp->setDestroyed(true);
            icmd->addGroupInfoEntity(grp,Internal::InfoCommand::DELETED);
        }
        else if (!grp->empty() && grp->isDestroyed()) {
            grp->setDestroyed(false);
            icmd->addGroupInfoEntity(grp,Internal::InfoCommand::NONE);
        }
    }
}
/*----------------------------------------------------------------------------*/
void GroupManager::deleteEntity(GroupEntity* ge)
{
	// pour éviter le cas où le groupe aurait déjà était détruit,
	// on teste la présence dans l'une des listes
	// cela peut arriver pour le cas d'un groupe détruit en cours de route et réutilisé
	// cela pose problème avec le clearSession
    bool found = false;
	if (!m_groups.empty()) {
		// on le retire de la liste
	    std::vector<GroupEntity*>::iterator it = m_groups.begin();
	    while (it != m_groups.end() && *it != ge)
	        it++;
	    if (it != m_groups.end()) {
	    	found = true;
	    	m_groups.erase(it);
	    }
	}

    if (found)
		delete ge;
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMarkAllGroups(const Utils::FilterEntity::objectType visibilityMask,
        std::map<Geom::GeomEntity*, uint>& filtre_geom,
        std::map<Topo::TopoEntity*, uint>& filtre_topo,
		std::map<CoordinateSystem::SysCoord*, uint>& filtre_rep,
        uint mark)
{
#ifdef _DEBUG_ADDED
	std::cout<<"GroupManager::addMarkAllGroups(visibilityMask = "<<visibilityMask<<", mark = "<<mark<<")"<<std::endl;
#endif
    // utilisation d'un filtre pour ne pas parcourir inutilement les même entités
    std::map<Utils::Entity*, bool> filtre_vu;

    for (GroupEntity* e : m_groups) {
        if (e->isVisible()) {
            switch (e->getDim()) {
            case 3: {
                Group3D* grp = dynamic_cast<Group3D*>(e);
                addMark(grp, visibilityMask, filtre_vu, filtre_geom, filtre_topo, filtre_rep, mark);
            }
            break;
            case 2: {
                Group2D* grp = dynamic_cast<Group2D*>(e);
                addMark(grp, visibilityMask, filtre_vu, filtre_geom, filtre_topo, mark);
            }
            break;
            case 1: {
                Group1D* grp = dynamic_cast<Group1D*>(e);
                addMark(grp, visibilityMask, filtre_vu, filtre_geom, filtre_topo, mark);
            }
            break;
            case 0: {
                Group0D* grp = dynamic_cast<Group0D*>(e);
                addMark(grp, visibilityMask, filtre_vu, filtre_geom, filtre_topo, mark);
            }
            break;
            default:
                MGX_NOT_YET_IMPLEMENTED("GroupManager::addMark pour cette dimension de groupe");
            }
        }
    } // end for iter = groups.begin()
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMark(const std::vector<Group::GroupEntity*>& groups,
                const Utils::FilterEntity::objectType visibilityMask,
                std::map<Geom::GeomEntity*, uint>& filtre_geom,
                std::map<Topo::TopoEntity*, uint>& filtre_topo,
				std::map<CoordinateSystem::SysCoord*, uint>& filtre_rep,
                uint mark)
{
    // utilisation d'un filtre pour ne pas parcourir inutilement les même entités
    std::map<Utils::Entity*, bool> filtre_vu;

    for (std::vector<Group::GroupEntity*>::const_iterator iter = groups.begin();
            iter != groups.end(); ++iter){
        switch ((*iter)->getDim()){
        case 3: {
            Group3D* grp = dynamic_cast<Group3D*>(*iter);
            addMark(grp, visibilityMask, filtre_vu, filtre_geom, filtre_topo, filtre_rep, mark);
        }
        break;
        case 2: {
            Group2D* grp = dynamic_cast<Group2D*>(*iter);
            addMark(grp, visibilityMask, filtre_vu, filtre_geom, filtre_topo, mark);
        }
        break;
        case 1: {
            Group1D* grp = dynamic_cast<Group1D*>(*iter);
            addMark(grp, visibilityMask, filtre_vu, filtre_geom, filtre_topo, mark);
        }
        break;
        case 0: {
            Group0D* grp = dynamic_cast<Group0D*>(*iter);
            addMark(grp, visibilityMask, filtre_vu, filtre_geom, filtre_topo, mark);
        }
        break;
        default:
            MGX_NOT_YET_IMPLEMENTED("GroupManager::addMark pour cette dimension de groupe");
        }
    } // end for iter = groups.begin()
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMeshGroups(const std::vector<Group::GroupEntity*>& groups,
        const Utils::FilterEntity::objectType visibilityMask,
        std::vector<Mesh::MeshEntity*>& meshAdded)
{
    Mesh::MeshManager& mmng = getContext().getMeshManager();

    for (std::vector<Group::GroupEntity*>::const_iterator iter = groups.begin();
            iter != groups.end(); ++iter){
        switch ((*iter)->getDim()){
        case 3: {
            if (visibilityMask & Utils::FilterEntity::MeshVolume){
                Mesh::MeshEntity* me = mmng.getVolume((*iter)->getName(), false);
                if (me)
                    meshAdded.push_back(me);
            }
        }
        break;
        case 2: {
            if (visibilityMask & Utils::FilterEntity::MeshSurface){
                Mesh::MeshEntity* me = mmng.getSurface((*iter)->getName(), false);
                if (me)
                    meshAdded.push_back(me);
            }
        }
        break;
        case 1:
            if (visibilityMask & Utils::FilterEntity::MeshLine){
                Mesh::MeshEntity* me = mmng.getLine((*iter)->getName(), false);
                if (me)
                    meshAdded.push_back(me);
            }
            if (visibilityMask & Utils::FilterEntity::MeshCloud){
                Mesh::MeshEntity* me = mmng.getCloud((*iter)->getName(), false);
                if (me)
                    meshAdded.push_back(me);
            }
        break;
        case 0: {
            if (visibilityMask & Utils::FilterEntity::MeshCloud){
                Mesh::MeshEntity* me = mmng.getCloud((*iter)->getName(), false);
                if (me)
                    meshAdded.push_back(me);
            }
        }
        break;
        default:
            MGX_NOT_YET_IMPLEMENTED("GroupManager::addMark pour cette dimension de groupe");
        }
    } // end for iter = groups.begin()
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMeshGroups(const Utils::FilterEntity::objectType visibilityMask,
        std::vector<Mesh::MeshEntity*>& meshAdded)
{
#ifdef _DEBUG_ADDED
	std::cout<<"GroupManager::addMeshGroups avec visibilityMask = "
			<<Utils::FilterEntity::filterToObjectTypeName((Utils::FilterEntity::objectType)visibilityMask)
			<<std::endl;
#endif

    Mesh::MeshManager& mmng = getContext().getMeshManager();

    if (visibilityMask & Utils::FilterEntity::MeshVolume)
        for (Group3D* g : getGroups<Group3D>()){
            Mesh::MeshEntity* me = mmng.getVolume(g->getName(), false);
            if (me && g->isVisible())
                meshAdded.push_back(me);
        }

    if (visibilityMask & Utils::FilterEntity::MeshSurface)
        for (Group2D* g : getGroups<Group2D>()){
            Mesh::MeshEntity* me = mmng.getSurface(g->getName(), false);
            if (me && g->isVisible())
                meshAdded.push_back(me);
        }

    if (visibilityMask & Utils::FilterEntity::MeshLine)
        for (Group1D* g : getGroups<Group1D>()){
            Mesh::MeshEntity* me = mmng.getLine(g->getName(), false);
            if (me && g->isVisible())
                meshAdded.push_back(me);
        }

    if (visibilityMask & Utils::FilterEntity::MeshCloud){
        for (Group0D* g : getGroups<Group0D>()){
            Mesh::MeshEntity* me = mmng.getCloud(g->getName(), false);
            if (me && g->isVisible())
                meshAdded.push_back(me);
        }
        for (Group1D* g : getGroups<Group1D>()){
            Mesh::MeshEntity* me = mmng.getCloud(g->getName(), false);
            if (me && g->isVisible())
                meshAdded.push_back(me);
        }
    }
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMark(Group3D* grp,
        const Utils::FilterEntity::objectType visibilityMask,
        std::map<Utils::Entity*, bool>& filtre_vu,
        std::map<Geom::GeomEntity*, uint>& filtre_geom,
        std::map<Topo::TopoEntity*, uint>& filtre_topo,
		std::map<CoordinateSystem::SysCoord*, uint>& filtre_rep,
        uint mark)
{
	bool propagate = getPropagate();

    for (Geom::Volume* vol : grp->getFilteredEntities<Geom::Volume>()) {
        // on n'observe les entités qu'une fois
        if (filtre_vu[vol] == false){
            filtre_vu[vol] = true;

            Geom::GetDownIncidentGeomEntitiesVisitor v;
            vol->accept(v);

            // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
            if (visibilityMask & Utils::FilterEntity::GeomVolume)
                filtre_geom[vol] = filtre_geom[vol] | mark;

            // faut-il continuer avec les surfaces ?
            if ((visibilityMask & Utils::FilterEntity::GeomSurface) && propagate){
                for (auto s : v.getSurfaces())
                    filtre_geom[s] = filtre_geom[s] | mark;
            } // end if (visibilityMask & Utils::FilterEntity::GeomSurface)

            // faut-il continuer avec les courbes ?
            if ((visibilityMask & Utils::FilterEntity::GeomCurve) && propagate){
                for (auto c : v.getCurves())
                    filtre_geom[c] = filtre_geom[c] | mark;
            } // end if (visibilityMask & Utils::FilterEntity::GeomCurve)

            // faut-il continuer avec les sommets ?
            if ((visibilityMask & Utils::FilterEntity::GeomVertex) && propagate){
                for (auto vert : v.getVertices())
                    filtre_geom[vert] = filtre_geom[vert] | mark;
            } // end if (visibilityMask & Utils::FilterEntity::GeomVertex)

            // faut-il propager à la topologie ?
            if (visibilityMask >= Utils::FilterEntity::TopoBlock){
                Topo::TopoManager& topo_manager = getContext().getTopoManager();
                for (Topo::Block* blk : topo_manager.getFilteredRefTopos<Topo::Block>(vol))
                    addMark(blk, visibilityMask, filtre_vu, filtre_topo, mark);
            }
        } // end if (filtre_vu[vol] == false)
    }
    
    if (visibilityMask >= Utils::FilterEntity::TopoBlock){
    	for (Topo::Block* blk : grp->getFilteredEntities<Topo::Block>())
    		addMark(blk, visibilityMask, filtre_vu, filtre_topo, mark);
    }

    if (visibilityMask >= Utils::FilterEntity::SysCoord){
    	for (CoordinateSystem::SysCoord* sc : grp->getFilteredEntities<CoordinateSystem::SysCoord>())
    		addMark(sc, visibilityMask, filtre_vu, filtre_rep, mark);
    }
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMark(Group2D* grp,
        const Utils::FilterEntity::objectType visibilityMask,
        std::map<Utils::Entity*, bool>& filtre_vu,
        std::map<Geom::GeomEntity*, uint>& filtre_geom,
        std::map<Topo::TopoEntity*, uint>& filtre_topo,
        uint mark)
{
	bool propagate = getPropagate();

    for (Geom::Surface* surf : grp->getFilteredEntities<Geom::Surface>()) {
        // on n'observe les entités qu'une fois
        if (filtre_vu[surf] == false){
            filtre_vu[surf] = true;

            Geom::GetDownIncidentGeomEntitiesVisitor v;
            surf->accept(v);

            // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
            if (visibilityMask & Utils::FilterEntity::GeomSurface)
                filtre_geom[surf] = filtre_geom[surf] | mark;

            // faut-il continuer avec les courbes ?
            if ((visibilityMask & Utils::FilterEntity::GeomCurve) && propagate){
                 for (auto c : v.getCurves())
                    filtre_geom[c] = filtre_geom[c] | mark;
            } // end if (visibilityMask & Utils::FilterEntity::GeomCurve)

            // faut-il continuer avec les sommets ?
            if ((visibilityMask & Utils::FilterEntity::GeomVertex) && propagate){
                for (auto vert : v.getVertices())
                    filtre_geom[vert] = filtre_geom[vert] | mark;
            } // end if (visibilityMask & Utils::FilterEntity::GeomVertex)

            // faut-il propager à la topologie ?
            if (visibilityMask >= Utils::FilterEntity::TopoCoFace){
                Topo::TopoManager& topo_manager = getContext().getTopoManager();
                for (Topo::CoFace* coface : topo_manager.getFilteredRefTopos<Topo::CoFace>(surf))
                    addMark(coface, visibilityMask, filtre_vu, filtre_topo, mark);
            } // end if (visibilityMask >= Utils::FilterEntity::TopoCoFace)

        } // end if (filtre_vu[surf] == false)
    }

    if (visibilityMask >= Utils::FilterEntity::TopoCoFace){
    	for (Topo::CoFace* cf : grp->getFilteredEntities<Topo::CoFace>())
    		addMark(cf, visibilityMask, filtre_vu, filtre_topo, mark);
    }
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMark(Group1D* grp,
        const Utils::FilterEntity::objectType visibilityMask,
        std::map<Utils::Entity*, bool>& filtre_vu,
        std::map<Geom::GeomEntity*, uint>& filtre_geom,
        std::map<Topo::TopoEntity*, uint>& filtre_topo,
        uint mark)
{
	bool propagate = getPropagate();

    for (Geom::Curve* c : grp->getFilteredEntities<Geom::Curve>()) {
        // on n'observe les entités qu'une fois
        if (filtre_vu[c] == false){
            filtre_vu[c] = true;

            // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
            if (visibilityMask & Utils::FilterEntity::GeomCurve)
                filtre_geom[c] = filtre_geom[c] | mark;

            // faut-il continuer avec les sommets ?
            if ((visibilityMask & Utils::FilterEntity::GeomVertex) && propagate){
                for (auto vert : c->getVertices())
                    filtre_geom[vert] = filtre_geom[vert] | mark;
            } // end if (visibilityMask & Utils::FilterEntity::GeomVertex)

            // faut-il propager à la topologie ?
            if (visibilityMask >= Utils::FilterEntity::TopoCoEdge){
                Topo::TopoManager& topo_manager = getContext().getTopoManager();
                for (Topo::CoEdge* coedge : topo_manager.getFilteredRefTopos<Topo::CoEdge>(c))
                    addMark(coedge, visibilityMask, filtre_vu, filtre_topo, mark);
            } // end if (visibilityMask >= Utils::FilterEntity::TopoCoEdge)

        } // end if (filtre_vu[c] == false)
    }

    if (visibilityMask >= Utils::FilterEntity::TopoCoEdge){
    	for (Topo::CoEdge* ce : grp->getFilteredEntities<Topo::CoEdge>())
    		addMark(ce, visibilityMask, filtre_vu, filtre_topo, mark);
    }
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMark(Group0D* grp,
        const Utils::FilterEntity::objectType visibilityMask,
        std::map<Utils::Entity*, bool>& filtre_vu,
        std::map<Geom::GeomEntity*, uint>& filtre_geom,
        std::map<Topo::TopoEntity*, uint>& filtre_topo,
        uint mark)
{
    // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
    if (visibilityMask & Utils::FilterEntity::GeomVertex){
        for (Geom::Vertex* vert : grp->getFilteredEntities<Geom::Vertex>()){
            // on n'observe les entités qu'une fois
            if (filtre_vu[vert] == false){
                filtre_vu[vert] = true;

                filtre_geom[vert] = filtre_geom[vert] | mark;

                // faut-il propager à la topologie ?
                if (visibilityMask >= Utils::FilterEntity::TopoVertex){
                    Topo::TopoManager& topo_manager = getContext().getTopoManager();
                    for (Topo::Vertex* vertex : topo_manager.getFilteredRefTopos<Topo::Vertex>(vert))
                        addMark(vertex, visibilityMask, filtre_vu, filtre_topo, mark);
                } // end if (visibilityMask >= Utils::FilterEntity::TopoVertex)

            } // end if (filtre_vu[vert] == false)
        }
    }
    if (visibilityMask >= Utils::FilterEntity::TopoVertex){
    	for (Topo::Vertex* vert : grp->getFilteredEntities<Topo::Vertex>())
    		addMark(vert, visibilityMask, filtre_vu, filtre_topo, mark);
    }
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMark(Topo::Block* blk,
        const Utils::FilterEntity::objectType visibilityMask,
        std::map<Utils::Entity*, bool>& filtre_vu,
        std::map<Topo::TopoEntity*, uint>& filtre_topo,
        uint mark)
{
	bool propagate = getPropagate();

    if (filtre_vu[blk] == false){
        filtre_vu[blk] = true;

        // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
        if (visibilityMask & Utils::FilterEntity::TopoBlock)
            filtre_topo[blk] = filtre_topo[blk] | mark;

        // faut-il continuer avec les entités de niveau inférieur ?
        if (visibilityMask >= Utils::FilterEntity::TopoFace && propagate) {
            const std::vector<Topo::Face* >& faces = blk->getFaces();

            for (std::vector<Topo::Face* >::const_iterator iter = faces.begin();
                    iter != faces.end(); ++iter)
                addMark(*iter, visibilityMask, filtre_vu, filtre_topo, mark);
        }

    } // end if (filtre_vu[blk] == false)
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMark(Topo::Face* face,
        const Utils::FilterEntity::objectType visibilityMask,
        std::map<Utils::Entity*, bool>& filtre_vu,
        std::map<Topo::TopoEntity*, uint>& filtre_topo,
        uint mark)
{
	bool propagate = getPropagate();

    if (filtre_vu[face] == false){
        filtre_vu[face] = true;

        // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
        if (visibilityMask & Utils::FilterEntity::TopoFace)
            filtre_topo[face] = filtre_topo[face] | mark;

        // faut-il continuer avec les entités de niveau inférieur ?
        if (visibilityMask >= Utils::FilterEntity::TopoCoFace && propagate) {
            const std::vector<Topo::CoFace* >& cofaces = face->getCoFaces();

            for (std::vector<Topo::CoFace* >::const_iterator iter = cofaces.begin();
                    iter != cofaces.end(); ++iter)
                addMark(*iter, visibilityMask, filtre_vu, filtre_topo, mark);
        }
    }
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMark(Topo::CoFace* coface,
        const Utils::FilterEntity::objectType visibilityMask,
        std::map<Utils::Entity*, bool>& filtre_vu,
        std::map<Topo::TopoEntity*, uint>& filtre_topo,
        uint mark)
{
	bool propagate = getPropagate();

    if (filtre_vu[coface] == false){
        filtre_vu[coface] = true;

        // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
        if (visibilityMask & Utils::FilterEntity::TopoCoFace)
            filtre_topo[coface] = filtre_topo[coface] | mark;

        // faut-il continuer avec les entités de niveau inférieur ?
        if (visibilityMask >= Utils::FilterEntity::TopoEdge && propagate) {
            const std::vector<Topo::Edge* >& edges = coface->getEdges();

            for (std::vector<Topo::Edge* >::const_iterator iter = edges.begin();
                    iter != edges.end(); ++iter)
                addMark(*iter, visibilityMask, filtre_vu, filtre_topo, mark);
        }
    }
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMark(Topo::Edge* edge,
        const Utils::FilterEntity::objectType visibilityMask,
        std::map<Utils::Entity*, bool>& filtre_vu,
        std::map<Topo::TopoEntity*, uint>& filtre_topo,
        uint mark)
{
	bool propagate = getPropagate();

    if (filtre_vu[edge] == false){
        filtre_vu[edge] = true;

        // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
        if (visibilityMask & Utils::FilterEntity::TopoEdge)
            filtre_topo[edge] = filtre_topo[edge] | mark;

        // faut-il continuer avec les entités de niveau inférieur ?
        if (visibilityMask >= Utils::FilterEntity::TopoCoEdge && propagate) {
            const std::vector<Topo::CoEdge* >& coedges = edge->getCoEdges();

            for (std::vector<Topo::CoEdge* >::const_iterator iter = coedges.begin();
                    iter != coedges.end(); ++iter)
                addMark(*iter, visibilityMask, filtre_vu, filtre_topo, mark);
        }
    }
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMark(Topo::CoEdge* coedge,
        const Utils::FilterEntity::objectType visibilityMask,
        std::map<Utils::Entity*, bool>& filtre_vu,
        std::map<Topo::TopoEntity*, uint>& filtre_topo,
        uint mark)
{
	bool propagate = getPropagate();

    if (filtre_vu[coedge] == false){
        filtre_vu[coedge] = true;

        // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
        if (visibilityMask & Utils::FilterEntity::TopoCoEdge)
            filtre_topo[coedge] = filtre_topo[coedge] | mark;

        // faut-il continuer avec les entités de niveau inférieur ?
        if (visibilityMask >= Utils::FilterEntity::TopoVertex && propagate) {
            const std::vector<Topo::Vertex* >& vertices = coedge->getVertices();

            for (std::vector<Topo::Vertex* >::const_iterator iter = vertices.begin();
                    iter != vertices.end(); ++iter)
                addMark(*iter, visibilityMask, filtre_vu, filtre_topo, mark);
        }
    }
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMark(Topo::Vertex* vertex,
        const Utils::FilterEntity::objectType visibilityMask,
        std::map<Utils::Entity*, bool>& filtre_vu,
        std::map<Topo::TopoEntity*, uint>& filtre_topo,
        uint mark)
{
    if (filtre_vu[vertex] == false){
        filtre_vu[vertex] = true;

        // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
        if (visibilityMask & Utils::FilterEntity::TopoVertex)
            filtre_topo[vertex] = filtre_topo[vertex] | mark;
    }
}
/*----------------------------------------------------------------------------*/
void GroupManager::addMark(CoordinateSystem::SysCoord* rep,
            const Utils::FilterEntity::objectType visibilityMask,
            std::map<Utils::Entity*, bool>& filtre_vu,
            std::map<CoordinateSystem::SysCoord*, uint>& filtre_rep,
            uint mark)
{
    if (filtre_vu[rep] == false){
        filtre_vu[rep] = true;

        // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
        if (visibilityMask & Utils::FilterEntity::SysCoord)
        	filtre_rep[rep] = filtre_rep[rep] | mark;
    } // end if (filtre_vu[rep] == false)

}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<GroupEntity*>& vg, std::vector<Geom::Volume*>& volumes)
{
    std::set<Geom::Volume*> initGeomEntities;
    for (GroupEntity* g : vg) {
        std::vector<Geom::Volume*> loc_volumes = g->getFilteredEntities<Geom::Volume>();
        initGeomEntities.insert(loc_volumes.begin(), loc_volumes.end());
    }

    for (Geom::Volume* v : initGeomEntities)
    	volumes.push_back(v);
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<GroupEntity*>& vg, std::vector<Geom::Surface*>& surfaces)
{
    std::set<Geom::Surface*> initGeomEntities;
    for (GroupEntity* g : vg) {
    	Group3D* gr3d = dynamic_cast<Group3D*>(g);
    	if (gr3d) {
    		for (Geom::Volume* volume : gr3d->getFilteredEntities<Geom::Volume>()){
    			auto loc_surfaces = volume->getSurfaces();
    			initGeomEntities.insert(loc_surfaces.begin(), loc_surfaces.end());
    		}
    	} else {
    		Group2D* gr2d = dynamic_cast<Group2D*>(g);
    		if (gr2d){
    			auto loc_surfaces = gr2d->getFilteredEntities<Geom::Surface>();
    			initGeomEntities.insert(loc_surfaces.begin(), loc_surfaces.end());
    		}
    	}
    }

    for (Geom::Surface* s : initGeomEntities)
    	surfaces.push_back(s);
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<GroupEntity*>& vg, std::vector<Geom::Curve*>& curves)
{
    std::set<Geom::Curve*> initGeomEntities;
    for (GroupEntity* g : vg) {
    	Group3D* gr3d = dynamic_cast<Group3D*>(g);
    	if (gr3d){
    		for (auto volume : gr3d->getFilteredEntities<Geom::Volume>()){
                for (auto surface : volume->getSurfaces()){
                    auto loc_curves = surface->getCurves();
                    initGeomEntities.insert(loc_curves.begin(), loc_curves.end());
                }
    		}
    	} else {
            Group2D* gr2d = dynamic_cast<Group2D*>(g);
            if (gr2d){
                for (auto surface : gr2d->getFilteredEntities<Geom::Surface>()){
                	auto loc_curves = surface->getCurves();
                	initGeomEntities.insert(loc_curves.begin(), loc_curves.end());
                }
            } else {
                Group1D* gr1d = dynamic_cast<Group1D*>(g);
                if (gr1d){
                	auto loc_curves = gr1d->getFilteredEntities<Geom::Curve>();
                	initGeomEntities.insert(loc_curves.begin(), loc_curves.end());
                }
            }
    	}
    }

    for (Geom::Curve* c : initGeomEntities)
    	curves.push_back(c);
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<GroupEntity*>& vg, std::vector<Geom::Vertex*>& vertices)
{
    std::set<Geom::Vertex*> initGeomEntities;
    for (GroupEntity* g : vg) {
    	Group3D* gr3d = dynamic_cast<Group3D*>(g);
    	if (gr3d){
    		for (auto volume : gr3d->getFilteredEntities<Geom::Volume>()){
                for (auto surface : volume->getSurfaces())
                    for (auto curve : surface->getCurves()) {
    			        auto loc_vertices = curve->getVertices();
    			        initGeomEntities.insert(loc_vertices.begin(), loc_vertices.end());
                    }
    		}
    	} else {
            Group2D* gr2d = dynamic_cast<Group2D*>(g);
            if (gr2d){
                for (auto surface : gr2d->getFilteredEntities<Geom::Surface>()){
                    for (auto curve : surface->getCurves()){
                	    auto loc_vertices = curve->getVertices();
                	    initGeomEntities.insert(loc_vertices.begin(), loc_vertices.end());
                    }
                }
            } else {
            	Group1D* gr1d = dynamic_cast<Group1D*>(g);
            	if (gr1d){
            		for (auto curve : gr1d->getFilteredEntities<Geom::Curve>()){
            			auto loc_vertices = curve->getVertices();
            			initGeomEntities.insert(loc_vertices.begin(), loc_vertices.end());
            		}
            	} else {
            		Group0D* gr0d = dynamic_cast<Group0D*>(g);
            		if (gr0d){
            			auto loc_vertices = gr0d->getFilteredEntities<Geom::Vertex>();
            			initGeomEntities.insert(loc_vertices.begin(), loc_vertices.end());
            		}
            	}
            }
    	}
    }

    for (Geom::Vertex* v : initGeomEntities)
    	vertices.push_back(v);
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<GroupEntity*>& vg, std::vector<Topo::Block*>& blocks)
{
	std::set<Topo::Block*> initTopoEntities;

    for (GroupEntity* g : vg) {
		Group3D* gr3d = dynamic_cast<Group3D*>(g);
		if (gr3d){
            auto ge = gr3d->getFilteredEntities<Geom::Volume>();
			Topo::TopoHelper::get(ge, initTopoEntities);
			auto te = gr3d->getFilteredEntities<Topo::Block>();
			initTopoEntities.insert(te.begin(), te.end());
		}
	}

	for (Topo::Block* b : initTopoEntities)
		blocks.push_back(b);
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<GroupEntity*>& vg, std::vector<Topo::CoFace*>& cofaces)
{
	bool propagate = getPropagate();
	std::set<Topo::CoFace*> initTopoEntities;
    for (GroupEntity* g : vg) {
		Group3D* gr3d = dynamic_cast<Group3D*>(g);
		if (gr3d){
            auto ge = gr3d->getFilteredEntities<Geom::Volume>();
			Topo::TopoHelper::get(ge, initTopoEntities, propagate);
            auto te = gr3d->getFilteredEntities<Topo::Block>();
			Topo::TopoHelper::get(te, initTopoEntities);
		}
		else {
			Group2D* gr2d = dynamic_cast<Group2D*>(g);
			if (gr2d){
                auto ge = gr2d->getFilteredEntities<Geom::Surface>();
				Topo::TopoHelper::get(ge, initTopoEntities);
				auto te = gr2d->getFilteredEntities<Topo::CoFace>();
				initTopoEntities.insert(te.begin(), te.end());
			}
		}
	}

	for (Topo::CoFace* cf : initTopoEntities)
		cofaces.push_back(cf);
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<GroupEntity*>& vg, std::vector<Topo::CoEdge*>& coedges)
{
	bool propagate = getPropagate();
	std::set<Topo::CoEdge*> initTopoEntities;
    for (GroupEntity* g : vg) {
		Group3D* gr3d = dynamic_cast<Group3D*>(g);
		if (gr3d){
            auto ge = gr3d->getFilteredEntities<Geom::Volume>();
			Topo::TopoHelper::get(ge, initTopoEntities, propagate);
            auto te = gr3d->getFilteredEntities<Topo::Block>();
			Topo::TopoHelper::get(te, initTopoEntities);
		}
		else {
			Group2D* gr2d = dynamic_cast<Group2D*>(g);
			if (gr2d){
                auto ge = gr2d->getFilteredEntities<Geom::Surface>();
				Topo::TopoHelper::get(ge, initTopoEntities, propagate);
                auto te = gr2d->getFilteredEntities<Topo::CoFace>();
				Topo::TopoHelper::get(te, initTopoEntities);
			}
			else {
				Group1D* gr1d = dynamic_cast<Group1D*>(g);
				if (gr1d){
                    auto ge = gr1d->getFilteredEntities<Geom::Curve>();
					Topo::TopoHelper::get(ge, initTopoEntities);
					auto te = gr1d->getFilteredEntities<Topo::CoEdge>();
					initTopoEntities.insert(te.begin(), te.end());
				}
			}
		}
	}

	for (Topo::CoEdge* ce : initTopoEntities)
		coedges.push_back(ce);
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<GroupEntity*>& vg, std::vector<Topo::Vertex*>& vertices)
{
	bool propagate = getPropagate();
	std::set<Topo::Vertex*> initTopoEntities;
    for (GroupEntity* g : vg) {
		Group3D* gr3d = dynamic_cast<Group3D*>(g);
		if (gr3d){
            auto ge = gr3d->getFilteredEntities<Geom::Volume>();
			Topo::TopoHelper::get(ge, initTopoEntities, propagate);
            auto te = gr3d->getFilteredEntities<Topo::Block>();
			Topo::TopoHelper::get(te, initTopoEntities);
		}
		else {
			Group2D* gr2d = dynamic_cast<Group2D*>(g);
			if (gr2d){
                auto ge = gr2d->getFilteredEntities<Geom::Surface>();
				Topo::TopoHelper::get(ge, initTopoEntities, propagate);
                auto te = gr2d->getFilteredEntities<Topo::CoFace>();
				Topo::TopoHelper::get(te, initTopoEntities);
			}
			else {
				Group1D* gr1d = dynamic_cast<Group1D*>(g);
				if (gr1d){
                    auto ge = gr1d->getFilteredEntities<Geom::Curve>();
					Topo::TopoHelper::get(ge, initTopoEntities, propagate);
                    auto te = gr1d->getFilteredEntities<Topo::CoEdge>();
					Topo::TopoHelper::get(te, initTopoEntities);
				}
				else {
					Group0D* gr0d = dynamic_cast<Group0D*>(g);
					if (gr0d){
                        auto ge = gr0d->getFilteredEntities<Geom::Vertex>();
						Topo::TopoHelper::get(ge, initTopoEntities);
						auto tv = gr0d->getFilteredEntities<Topo::Vertex>();
						initTopoEntities.insert(tv.begin(), tv.end());
					}

				}
			}
		}
	}

	for (Topo::Vertex* v : initTopoEntities)
		vertices.push_back(v);
}

/*------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<GroupEntity*>& vg, std::vector<Mesh::Volume*>& volumes)
{
	Mesh::MeshManager& mmng = getContext().getMeshManager();

    for (auto vn : vg){
		Mesh::Volume* me = mmng.getVolume(vn->getName(), false);

		if (me)
			volumes.push_back(me);
	}
}
/*------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<GroupEntity*>& vg, std::vector<Mesh::Surface*>& surfaces)
{
	Mesh::MeshManager& mmng = getContext().getMeshManager();

    for (auto vn : vg){
		Mesh::Surface* me = mmng.getSurface(vn->getName(), false);

		if (me)
			surfaces.push_back(me);
	}
}
/*------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<GroupEntity*>& vg, std::vector<Mesh::Line*>& lines)
{
	Mesh::MeshManager& mmng = getContext().getMeshManager();

    for (auto vn : vg){
		Mesh::Line* me = mmng.getLine(vn->getName(), false);

		if (me)
			lines.push_back(me);
	}
}
/*------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<GroupEntity*>& vg, std::vector<Mesh::Cloud*>& clouds)
{
	Mesh::MeshManager& mmng = getContext().getMeshManager();

    for (auto vn : vg){
		Mesh::Cloud* me = mmng.getCloud(vn->getName(), false);

		if (me)
			clouds.push_back(me);
	}
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getGeomVolumes(const std::string& g, const int d)
{
	std::vector<std::string> result;
    std::vector<GroupEntity*> groups = { getGroup(g, d) };
	std::vector<Geom::Volume*> geom_entities;
	get(groups, geom_entities);

	for (auto e : geom_entities)
		result.push_back(e->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getGeomSurfaces(const std::string& g, const int d)
{
	std::vector<std::string> result;
    std::vector<GroupEntity*> groups = { getGroup(g, d) };
	std::vector<Geom::Surface*> geom_entities;
	get(groups, geom_entities);

	for (auto e : geom_entities)
		result.push_back(e->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getGeomCurves(const std::string& g, const int d)
{
	std::vector<std::string> result;
    std::vector<GroupEntity*> groups = { getGroup(g, d) };
	std::vector<Geom::Curve*> geom_entities;
	get(groups, geom_entities);

	for (auto e : geom_entities)
		result.push_back(e->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getGeomVertices(const std::string& g, const int d)
{
	std::vector<std::string> result;
    std::vector<GroupEntity*> groups = { getGroup(g, d) };
	std::vector<Geom::Vertex*> geom_entities;
	get(groups, geom_entities);

	for (auto e : geom_entities)
		result.push_back(e->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getTopoBlocks(const std::string& g, const int d)
{
	std::vector<std::string> result;
    std::vector<GroupEntity*> groups = { getGroup(g, d) };
	std::vector<Topo::Block*> topo_entities;
	get(groups, topo_entities);

	for (auto e : topo_entities)
		result.push_back(e->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getTopoFaces(const std::string& g, const int d)
{
	std::vector<std::string> result;
    std::vector<GroupEntity*> groups = { getGroup(g, d) };
	std::vector<Topo::CoFace*> topo_entities;
	get(groups, topo_entities);

	for (auto e : topo_entities)
		result.push_back(e->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getTopoEdges(const std::string& g, const int d)
{
	std::vector<std::string> result;
    std::vector<GroupEntity*> groups = { getGroup(g, d) };
	std::vector<Topo::CoEdge*> topo_entities;
	get(groups, topo_entities);

	for (auto e : topo_entities)
		result.push_back(e->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getTopoVertices(const std::string& g, const int d)
{
	std::vector<std::string> result;
    std::vector<GroupEntity*> groups = { getGroup(g, d) };
	std::vector<Topo::Vertex*> topo_entities;
	get(groups, topo_entities);

	for (auto e : topo_entities)
		result.push_back(e->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<GroupEntity*> GroupManager::getVisibles() const
{
	std::vector<GroupEntity*> visibles;
	for (GroupEntity* g : m_groups)
		if (g->isVisible() && !g->isDestroyed())
			visibles.push_back(g);
	return visibles;
}
/*----------------------------------------------------------------------------*/
void GroupManager::addProjectionOnPX0(const std::string& nom)
{
	// recherche du groupe 2D
	Group2D* gr2d = getGroup<Group2D>(nom, true);

	// création de l'objet qui va modifier le maillage
	Mesh::MeshModificationItf* modif = new Mesh::MeshModificationByProjectionOnP0(Mesh::MeshModificationByProjectionOnP0::X);

	CommandAddMeshModification* command = new CommandAddMeshModification(getContext(), gr2d, modif);
	TkUtil::UTF8String	cmt (TkUtil::Charset::UTF_8);
	cmt<<"Projette les noeuds du groupe "<<nom<<" sur le plan X=0";
	command->setScriptComments(cmt);

	// trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().addProjectionOnPX0 (\""
        << nom<<"\")";
    command->setScriptCommand(cmd);

    getContext().getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void GroupManager::addProjectionOnPY0(const std::string& nom)
{
	// recherche du groupe 2D
	Group2D* gr2d = getGroup<Group2D>(nom, true);

	// création de l'objet qui va modifier le maillage
	Mesh::MeshModificationItf* modif = new Mesh::MeshModificationByProjectionOnP0(Mesh::MeshModificationByProjectionOnP0::Y);

	CommandAddMeshModification* command = new CommandAddMeshModification(getContext(), gr2d, modif);
	TkUtil::UTF8String	cmt (TkUtil::Charset::UTF_8);
	cmt<<"Projette les noeuds du groupe "<<nom<<" sur le plan Y=0";
	command->setScriptComments(cmt);

	// trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().addProjectionOnPY0 (\""
        << nom<<"\")";
    command->setScriptCommand(cmd);

    getContext().getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void GroupManager::addProjectionOnPZ0(const std::string& nom)
{
	// recherche du groupe 2D
	Group2D* gr2d = getGroup<Group2D>(nom, true);

	// création de l'objet qui va modifier le maillage
	Mesh::MeshModificationItf* modif = new Mesh::MeshModificationByProjectionOnP0(Mesh::MeshModificationByProjectionOnP0::Z);

	CommandAddMeshModification* command = new CommandAddMeshModification(getContext(), gr2d, modif);
	TkUtil::UTF8String	cmt (TkUtil::Charset::UTF_8);
	cmt<<"Projette les noeuds du groupe "<<nom<<" sur le plan Z=0";
	command->setScriptComments(cmt);

	// trace dans le script
	TkUtil::UTF8String	cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().addProjectionOnPZ0 (\""
        << nom<<"\")";
    command->setScriptCommand(cmd);

    getContext().getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void GroupManager::addCartesianPerturbation(const std::string& nom, PyObject* py_obj)
{
    PyObject* pyName = PyObject_Str(py_obj);
#ifdef _DEBUG2
	std::cout<<"GroupManager::addCartesianPerturbation("<<nom<<", "<<PyUnicode_AsUTF8(pyName)<<")"<<std::endl;
#endif
	// recherche du groupe 2D ou 3D
	Group2D* gr2d = getGroup<Group2D>(nom, false);
	Group3D* gr3d = getGroup<Group3D>(nom, false);

	if (gr2d==0 && gr3d==0){
		TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
		messErr <<"On ne trouve pas "<<nom<<" dans le GroupManager parmis les groupes 2D et 3D";
		throw TkUtil::Exception(messErr);
	}

	GroupEntity* grp = gr3d;
	if (gr2d && !gr2d->isDestroyed())
		grp = gr2d;

	// création de l'objet qui va modifier le maillage
	Mesh::MeshModificationItf* modif = new Mesh::MeshModificationByPythonFunction(py_obj,
			Mesh::MeshModificationByPythonFunction::cartesian);
	CommandAddMeshModification* command = new CommandAddMeshModification(getContext(), grp, modif);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().addCartesianPerturbation (\""
        << nom<<"\", "<<PyUnicode_AsUTF8(pyName)<<")";
    command->setScriptCommand(cmd);

    getContext().getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void GroupManager::addPolarPerturbation(const std::string& nom, PyObject* py_obj)
{
    PyObject* pyName = PyObject_Str(py_obj);
#ifdef _DEBUG2
	std::cout<<"GroupManager::addPolarPerturbation("<<nom<<", "<<PyUnicode_AsUTF8(pyName)<<")"<<std::endl;
#endif
	// recherche du groupe 2D ou 3D
	Group2D* gr2d = getGroup<Group2D>(nom, false);
	Group3D* gr3d = getGroup<Group3D>(nom, false);

	if (gr2d==0 && gr3d==0){
		TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
		messErr <<"On ne trouve pas "<<nom<<" dans le GroupManager parmis les groupes 2D et 3D";
		throw TkUtil::Exception(messErr);
	}

	GroupEntity* grp = gr3d;
	if (gr2d)
		grp = gr2d;

	// création de l'objet qui va modifier le maillage
	Mesh::MeshModificationItf* modif = new Mesh::MeshModificationByPythonFunction(py_obj,
			Mesh::MeshModificationByPythonFunction::polar);
	CommandAddMeshModification* command = new CommandAddMeshModification(getContext(), grp, modif);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().addPolarPerturbation (\""
        << nom<<"\", "<<PyUnicode_AsUTF8(pyName)<<")";
    command->setScriptCommand(cmd);

    getContext().getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void GroupManager::addSmoothing(const std::string& nom, Mesh::SurfacicSmoothing& sm)
{
#ifdef _DEBUG2
	std::cout<<"GroupManager::addSmoothing("<<nom<<", SurfacicSmoothing)"<<std::endl;
#endif
	// recherche du groupe 2D
	Group2D* gr2d = getGroup<Group2D>(nom, true);

	CommandAddMeshModification* command = new CommandAddMeshModification(getContext(), gr2d, sm.clone());

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().addSmoothing (\""
        << nom<<"\","<<sm.getScriptCommand()<<")";
    command->setScriptCommand(cmd);

    getContext().getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void GroupManager::addSmoothing(const std::string& nom, Mesh::VolumicSmoothing& sm)
{
#ifdef _DEBUG2
	std::cout<<"GroupManager::addSmoothing("<<nom<<", VolumicSmoothing)"<<std::endl;
#endif
	// recherche du groupe 3D
	Group3D* gr3d = getGroup<Group3D>(nom, true);

	CommandAddMeshModification* command = new CommandAddMeshModification(getContext(), gr3d, sm.clone());

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().addSmoothing (\""
        << nom<<"\","<<sm.getScriptCommand()<<")";
    command->setScriptCommand(cmd);

    getContext().getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void GroupManager::addSepa(const std::string& nom, Mesh::MeshModificationBySepa& ASepa)
{
#ifdef _DEBUG2
	std::cout<<"GroupManager::addSepa("<<nom<<", MeshModificationBySepa)"<<std::endl;
#endif
	// recherche du groupe 2D
	Group2D* gr2d = getGroup<Group2D>(nom, true);

	CommandAddMeshModification* command = new CommandAddMeshModification(getContext(), gr2d, ASepa.clone());

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().addSepa (\""
        << nom<<"\","<<ASepa.getScriptCommand()<<")";
    command->setScriptCommand(cmd);

    getContext().getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void GroupManager::setLevel(std::vector<std::string>& vg, int dim, int level)
{
    for (std::string n : vg)
        getGroup(n, dim, true)->setLevel(level);
}
/*----------------------------------------------------------------------------*/
GroupEntity* GroupManager::getGroup(const std::string& name, const int dim, const bool exceptionIfNotFound) const
{
    switch(dim){
    case(3): return getGroup<Group3D>(name, exceptionIfNotFound);
    case(2): return getGroup<Group2D>(name, exceptionIfNotFound);
    case(1): return getGroup<Group1D>(name, exceptionIfNotFound);
    case(0): return getGroup<Group0D>(name, exceptionIfNotFound);
    default: throw TkUtil::Exception (TkUtil::UTF8String ("dimension non prévue pour GroupManager::getGroup", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
template Group0D* GroupManager::getGroup<Group0D>(const std::string&, const bool) const;
template Group1D* GroupManager::getGroup<Group1D>(const std::string&, const bool) const;
template Group2D* GroupManager::getGroup<Group2D>(const std::string&, const bool) const;
template Group3D* GroupManager::getGroup<Group3D>(const std::string&, const bool) const;
/*----------------------------------------------------------------------------*/
template Group0D* GroupManager::getNewGroup<Group0D>(const std::string&, Internal::InfoCommand*);
template Group1D* GroupManager::getNewGroup<Group1D>(const std::string&, Internal::InfoCommand*);
template Group2D* GroupManager::getNewGroup<Group2D>(const std::string&, Internal::InfoCommand*);
template Group3D* GroupManager::getNewGroup<Group3D>(const std::string&, Internal::InfoCommand*);
/*----------------------------------------------------------------------------*/
template std::vector<Group0D*> GroupManager::getGroups<Group0D>(const bool) const;
template std::vector<Group1D*> GroupManager::getGroups<Group1D>(const bool) const;
template std::vector<Group2D*> GroupManager::getGroups<Group2D>(const bool) const;
template std::vector<Group3D*> GroupManager::getGroups<Group3D>(const bool) const;
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
















