/*----------------------------------------------------------------------------*/
/*
 * \file GroupManager.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18/10/2012
 */
/*----------------------------------------------------------------------------*/
// pythonerie à mettre au début (pour permettre ifndef Py_PYTHON_H dans GroupManagerIfc.h)
#include <Python.h>
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <set>
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Group/GroupManager.h"
#include "Group/GroupEntity.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"
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
#include "Mesh/CommandAddRemoveGroupName.h"
#include "Mesh/MeshModificationBySepa.h"
#include "Mesh/MeshModificationByProjectionOnP0.h"
#include "Mesh/CommandClearGroupName.h"

#include "SysCoord/SysCoord.h"

#include "Smoothing/SurfacicSmoothing.h"
#include "Smoothing/VolumicSmoothing.h"

#include <TkUtil/Exception.h>
#include <TkUtil/TraceLog.h>
#if PY_MAJOR_VERSION >= 3
#define PyString_AsString PyUnicode_AsUTF8
#endif	// # PY_MAJOR_VERSION < 3
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
GroupManager::GroupManager(const std::string& name, Internal::ContextIfc* c)
:Group::GroupManagerIfc(name, c)
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
    for (std::vector<Group3D*>::const_iterator iter = m_group3D.begin();
            iter != m_group3D.end(); ++iter)
        delete *iter;
    m_group3D.clear();

    for (std::vector<Group2D*>::const_iterator iter = m_group2D.begin();
            iter != m_group2D.end(); ++iter)
        delete *iter;
    m_group2D.clear();

    for (std::vector<Group1D*>::const_iterator iter = m_group1D.begin();
            iter != m_group1D.end(); ++iter)
        delete *iter;
    m_group1D.clear();

    for (std::vector<Group0D*>::const_iterator iter = m_group0D.begin();
            iter != m_group0D.end(); ++iter)
        delete *iter;
    m_group0D.clear();
}
/*------------------------------------------------------------------------*/
/** Vide un groupe suivant son nom et une dimension */
Internal::M3DCommandResultIfc* GroupManager::clearGroup(int dim, const std::string& groupName)
{
	Mesh::CommandClearGroupName* command =
			new Mesh::CommandClearGroupName(getLocalContext(), dim, groupName);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().clearGroup ("<<(short)dim<<", \""<<groupName<<"\")";
    command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
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
    switch(dim){
    case(3):{
        Group3D* gr = getGroup3D(name);
        return gr->getInfos();
    }
    break;
    case(2):{
        Group2D* gr = getGroup2D(name);
        return gr->getInfos();
    }
    break;
    case(1):{
        Group1D* gr = getGroup1D(name);
        return gr->getInfos();
    }
    break;
    case(0):{
        Group0D* gr = getGroup0D(name);
        return gr->getInfos();
    }
    break;
    default:
        throw TkUtil::Exception (TkUtil::UTF8String ("dimension non prévue pour GroupManager::getInfos", TkUtil::Charset::UTF_8));
    }
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
Group3D* GroupManager::getGroup3D(const std::string& gr_name, const bool exceptionIfNotFound) const
{
    std::string name(gr_name.empty()?getDefaultName(3):gr_name);

    Group3D* gr = 0;
    for (std::vector<Group3D*>::const_iterator iter = m_group3D.begin();
            iter != m_group3D.end(); ++iter)
        if ((*iter)->getName() == name)
            gr = (*iter);

    if (exceptionIfNotFound && gr == 0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr <<"On ne trouve pas "<<name<<" dans le GroupManager / 3D";
        throw TkUtil::Exception(messErr);
    }

    return gr;
}
/*----------------------------------------------------------------------------*/
Group3D* GroupManager::getNewGroup3D(const std::string& gr_name, Internal::InfoCommand* icmd)
{
    std::string name(gr_name.empty()?getDefaultName(3):gr_name);

    Group3D* gr = 0;
    for (std::vector<Group3D*>::const_iterator iter = m_group3D.begin();
            iter != m_group3D.end(); ++iter)
        if ((*iter)->getName() == name)
            gr = (*iter);

    if (gr == 0){
        gr = new Group3D(getLocalContext(), name, gr_name.empty());
        m_group3D.push_back(gr);
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
Group2D* GroupManager::getGroup2D(const std::string& gr_name, const bool exceptionIfNotFound) const
{
    std::string name(gr_name.empty()?getDefaultName(2):gr_name);

    Group2D* gr = 0;
    for (std::vector<Group2D*>::const_iterator iter = m_group2D.begin();
            iter != m_group2D.end(); ++iter)
        if ((*iter)->getName() == name)
            gr = (*iter);

    if (exceptionIfNotFound && gr == 0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr <<"On ne trouve pas "<<name<<" dans le GroupManager / 2D";
        throw TkUtil::Exception(messErr);
    }

    return gr;
}
/*----------------------------------------------------------------------------*/
Group2D* GroupManager::getNewGroup2D(const std::string& gr_name, Internal::InfoCommand* icmd)
{
    std::string name(gr_name.empty()?getDefaultName(2):gr_name);

    Group2D* gr = 0;
    for (std::vector<Group2D*>::const_iterator iter = m_group2D.begin();
            iter != m_group2D.end(); ++iter)
        if ((*iter)->getName() == name)
            gr = (*iter);

    if (gr == 0){
        gr = new Group2D(getLocalContext(), name, gr_name.empty());
        m_group2D.push_back(gr);
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
Group1D* GroupManager::getGroup1D(const std::string& gr_name, const bool exceptionIfNotFound) const
{
    std::string name(gr_name.empty()?getDefaultName(1):gr_name);

    Group1D* gr = 0;
    for (std::vector<Group1D*>::const_iterator iter = m_group1D.begin();
            iter != m_group1D.end(); ++iter)
        if ((*iter)->getName() == name)
            gr = (*iter);

    if (exceptionIfNotFound && gr == 0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr <<"On ne trouve pas "<<name<<" dans le GroupManager / 1D";
        throw TkUtil::Exception(messErr);
    }

    return gr;
}
/*----------------------------------------------------------------------------*/
Group1D* GroupManager::getNewGroup1D(const std::string& gr_name, Internal::InfoCommand* icmd)
{
    std::string name(gr_name.empty()?getDefaultName(1):gr_name);

    Group1D* gr = 0;
    for (std::vector<Group1D*>::const_iterator iter = m_group1D.begin();
            iter != m_group1D.end(); ++iter)
        if ((*iter)->getName() == name)
            gr = (*iter);

    if (gr == 0){
        gr = new Group1D(getLocalContext(), name, gr_name.empty());
        m_group1D.push_back(gr);
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
Group0D* GroupManager::getGroup0D(const std::string& gr_name, const bool exceptionIfNotFound) const
{
    std::string name(gr_name.empty()?getDefaultName(0):gr_name);

    Group0D* gr = 0;
    for (std::vector<Group0D*>::const_iterator iter = m_group0D.begin();
            iter != m_group0D.end(); ++iter)
        if ((*iter)->getName() == name)
            gr = (*iter);

    if (exceptionIfNotFound && gr == 0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr <<"On ne trouve pas "<<name<<" dans le GroupManager / 0D";
        throw TkUtil::Exception(messErr);
    }

    return gr;
}
/*----------------------------------------------------------------------------*/
void GroupManager::getGroups(std::vector<GroupEntity*>& grp, Utils::SelectionManagerIfc::DIM dimensions, const bool onlyLive) const
{
	for (int i = 0; i < 4; i++){
		const Utils::SelectionManagerIfc::DIM dim = Utils::SelectionManagerIfc::dimensionToDimensions(i);
		if (0 == (dimensions&dim))
			continue;
		switch (i)
		{
			case 0 : {
				std::vector<Group0D*>	groups;
				getGroup0D (groups, onlyLive);
				for (std::vector<Group0D*>::iterator it=groups.begin( ); groups.end() != it; it++)
					grp.push_back (*it);
			}
			break;
			case 1 : {
				std::vector<Group1D*>	groups;
				getGroup1D (groups, onlyLive);
				for (std::vector<Group1D*>::iterator it=groups.begin( ); groups.end() != it; it++)
					grp.push_back (*it);
			}
			break;
			case 2 : {
				std::vector<Group2D*>	groups;
				getGroup2D (groups, onlyLive);
				for (std::vector<Group2D*>::iterator it=groups.begin( ); groups.end() != it; it++)
					grp.push_back (*it);
			}
			break;
			case 3 : {
				std::vector<Group3D*>	groups;
				getGroup3D (groups, onlyLive);
				for (std::vector<Group3D*>::iterator it=groups.begin( ); groups.end() != it; it++)
					grp.push_back (*it);
			}
			break;
		}
	}
}
/*----------------------------------------------------------------------------*/
Group0D* GroupManager::getNewGroup0D(const std::string& gr_name, Internal::InfoCommand* icmd)
{
    std::string name(gr_name.empty()?getDefaultName(0):gr_name);

    Group0D* gr = 0;
    for (std::vector<Group0D*>::const_iterator iter = m_group0D.begin();
            iter != m_group0D.end(); ++iter)
        if ((*iter)->getName() == name)
            gr = (*iter);

    if (gr == 0){
        gr = new Group0D(getLocalContext(), name, gr_name.empty());
        m_group0D.push_back(gr);
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
void GroupManager::getGroup3D(std::vector<Group3D*>& grp, const bool onlyLive) const
{
    grp.clear();
    if (onlyLive){
        for (std::vector<Group3D*>::const_iterator iter = m_group3D.begin();
                iter != m_group3D.end(); ++iter)
            if (!(*iter)->isDestroyed())
                grp.push_back(*iter);
    }
    else
        grp.insert(grp.end(), m_group3D.begin(), m_group3D.end());
}
/*----------------------------------------------------------------------------*/
void GroupManager::getGroup2D(std::vector<Group2D*>& grp, const bool onlyLive) const
{
    grp.clear();
    if (onlyLive){
        for (std::vector<Group2D*>::const_iterator iter = m_group2D.begin();
                iter != m_group2D.end(); ++iter)
            if (!(*iter)->isDestroyed())
                grp.push_back(*iter);
    }
    else
        grp.insert(grp.end(), m_group2D.begin(), m_group2D.end());
}
/*----------------------------------------------------------------------------*/
void GroupManager::getGroup1D(std::vector<Group1D*>& grp, const bool onlyLive) const
{
    grp.clear();
    if (onlyLive){
        for (std::vector<Group1D*>::const_iterator iter = m_group1D.begin();
                iter != m_group1D.end(); ++iter)
            if (!(*iter)->isDestroyed())
                grp.push_back(*iter);
    }
    else
        grp.insert(grp.end(), m_group1D.begin(), m_group1D.end());
}
/*----------------------------------------------------------------------------*/
void GroupManager::getGroup0D(std::vector<Group0D*>& grp, const bool onlyLive) const
{
    grp.clear();
    if (onlyLive){
        for (std::vector<Group0D*>::const_iterator iter = m_group0D.begin();
                iter != m_group0D.end(); ++iter)
            if (!(*iter)->isDestroyed())
                grp.push_back(*iter);
    }
    else
        grp.insert(grp.end(), m_group0D.begin(), m_group0D.end());
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

    // on parcours tous les groupes
    for (std::vector<Group0D*>::iterator iter = m_group0D.begin();
            iter != m_group0D.end(); ++iter)
        if ((*iter)->empty() && !(*iter)->isDestroyed()){
//        	if ((*iter)->isDefaultGroup()){
//        		icmd->addGroupInfoEntity(*iter,Internal::InfoCommand::DISABLE);
//        	}
//        	else {
        		(*iter)->setDestroyed(true);
        		icmd->addGroupInfoEntity(*iter,Internal::InfoCommand::DELETED);
//        	}
        }
        else if (!(*iter)->empty() && (*iter)->isDestroyed()){
            (*iter)->setDestroyed(false);
            icmd->addGroupInfoEntity(*iter,Internal::InfoCommand::NONE);
        }
    for (std::vector<Group1D*>::iterator iter = m_group1D.begin();
            iter != m_group1D.end(); ++iter)
        if ((*iter)->empty() && !(*iter)->isDestroyed()){
//        	if ((*iter)->isDefaultGroup()){
//        		icmd->addGroupInfoEntity(*iter,Internal::InfoCommand::DISABLE);
//        	}
//        	else {
        		(*iter)->setDestroyed(true);
        		icmd->addGroupInfoEntity(*iter,Internal::InfoCommand::DELETED);
//        	}
        }
        else if (!(*iter)->empty() && (*iter)->isDestroyed()){
            (*iter)->setDestroyed(false);
            icmd->addGroupInfoEntity(*iter,Internal::InfoCommand::NONE);
        }
    for (std::vector<Group2D*>::iterator iter = m_group2D.begin();
            iter != m_group2D.end(); ++iter)
        if ((*iter)->empty() && !(*iter)->isDestroyed()){
//        	if ((*iter)->isDefaultGroup()){
//        		icmd->addGroupInfoEntity(*iter,Internal::InfoCommand::DISABLE);
//        	}
//        	else {
        		(*iter)->setDestroyed(true);
        		icmd->addGroupInfoEntity(*iter,Internal::InfoCommand::DELETED);
//        	}
        }
        else if (!(*iter)->empty() && (*iter)->isDestroyed()){
            (*iter)->setDestroyed(false);
            icmd->addGroupInfoEntity(*iter,Internal::InfoCommand::NONE);
        }
    for (std::vector<Group3D*>::iterator iter = m_group3D.begin();
            iter != m_group3D.end(); ++iter)
        if ((*iter)->empty() && !(*iter)->isDestroyed()){
//        	if ((*iter)->isDefaultGroup()){
//        		icmd->addGroupInfoEntity(*iter,Internal::InfoCommand::DISABLE);
//        	}
//        	else {
        		(*iter)->setDestroyed(true);
        		icmd->addGroupInfoEntity(*iter,Internal::InfoCommand::DELETED);
//        	}
        }
        else if (!(*iter)->empty() && (*iter)->isDestroyed()){
            (*iter)->setDestroyed(false);
            icmd->addGroupInfoEntity(*iter,Internal::InfoCommand::NONE);
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

	if (!found && !m_group3D.empty()){
		// on le retire de la liste
	    std::vector<Group3D*>::iterator it= m_group3D.begin();
	    while(it!=m_group3D.end() && *it!=ge)
	        it++;
	    if(it!=m_group3D.end()){
	    	found = true;
	    	m_group3D.erase(it);
	    }
	}
	if (!found && !m_group2D.empty()){
		// on le retire de la liste
	    std::vector<Group2D*>::iterator it= m_group2D.begin();
	    while(it!=m_group2D.end() && *it!=ge)
	        it++;
	    if(it!=m_group2D.end()){
	    	found = true;
	    	m_group2D.erase(it);
	    }
	}
	if (!found && !m_group1D.empty()){
		// on le retire de la liste
	    std::vector<Group1D*>::iterator it= m_group1D.begin();
	    while(it!=m_group1D.end() && *it!=ge)
	        it++;
	    if(it!=m_group1D.end()){
	    	found = true;
	    	m_group1D.erase(it);
	    }
	}
	if (!found && !m_group0D.empty()){
		// on le retire de la liste
	    std::vector<Group0D*>::iterator it= m_group0D.begin();
	    while(it!=m_group0D.end() && *it!=ge)
	        it++;
	    if(it!=m_group0D.end()){
	    	found = true;
	    	m_group0D.erase(it);
	    }
	}

	//std::cout<<"delete de "<<ge->getName()<<" (uid "<< ge->getUniqueId()<<") found = "<<(found?"true":"false")<<std::endl;

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

    for (std::vector<Group::Group3D*>::iterator iter = m_group3D.begin();
            iter != m_group3D.end(); ++iter)
        if ((*iter)->isVisible())
            addMark(*iter, visibilityMask, filtre_vu, filtre_geom, filtre_topo, filtre_rep, mark);

    for (std::vector<Group::Group2D*>::iterator iter = m_group2D.begin();
            iter != m_group2D.end(); ++iter){
//#ifdef _DEBUG_ADDED
//    	std::cout<<" groupe 2D "<<(*iter)->getName()<<", isVisible = "<<(*iter)->isVisible()<<std::endl;
//#endif
    	if ((*iter)->isVisible())
            addMark(*iter, visibilityMask, filtre_vu, filtre_geom, filtre_topo, mark);
    }

    for (std::vector<Group::Group1D*>::iterator iter = m_group1D.begin();
            iter != m_group1D.end(); ++iter)
        if ((*iter)->isVisible())
            addMark(*iter, visibilityMask, filtre_vu, filtre_geom, filtre_topo, mark);

    for (std::vector<Group::Group0D*>::iterator iter = m_group0D.begin();
            iter != m_group0D.end(); ++iter)
        if ((*iter)->isVisible())
            addMark(*iter, visibilityMask, filtre_vu, filtre_geom, filtre_topo, mark);
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
    Mesh::MeshManager& mmng = getLocalContext().getLocalMeshManager();

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

    Mesh::MeshManager& mmng = getLocalContext().getLocalMeshManager();

    if (visibilityMask & Utils::FilterEntity::MeshVolume)
        for (std::vector<Group::Group3D*>::iterator iter = m_group3D.begin();
                iter != m_group3D.end(); ++iter){
            Mesh::MeshEntity* me = mmng.getVolume((*iter)->getName(), false);
            if (me && (*iter)->isVisible())
                meshAdded.push_back(me);
        }

    if (visibilityMask & Utils::FilterEntity::MeshSurface)
        for (std::vector<Group::Group2D*>::iterator iter = m_group2D.begin();
                iter != m_group2D.end(); ++iter){
            Mesh::MeshEntity* me = mmng.getSurface((*iter)->getName(), false);
            if (me && (*iter)->isVisible())
                meshAdded.push_back(me);
        }

    if (visibilityMask & Utils::FilterEntity::MeshLine)
        for (std::vector<Group::Group1D*>::iterator iter = m_group1D.begin();
                iter != m_group1D.end(); ++iter){
            Mesh::MeshEntity* me = mmng.getLine((*iter)->getName(), false);
            if (me && (*iter)->isVisible())
                meshAdded.push_back(me);
        }

    if (visibilityMask & Utils::FilterEntity::MeshCloud){
        for (std::vector<Group::Group0D*>::iterator iter = m_group0D.begin();
                iter != m_group0D.end(); ++iter){
            Mesh::MeshEntity* me = mmng.getCloud((*iter)->getName(), false);
            if (me && (*iter)->isVisible())
                meshAdded.push_back(me);
        }
        for (std::vector<Group::Group1D*>::iterator iter = m_group1D.begin();
                iter != m_group1D.end(); ++iter){
            Mesh::MeshEntity* me = mmng.getCloud((*iter)->getName(), false);
            if (me && (*iter)->isVisible())
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

    std::vector<Geom::Volume*>& volumes = grp->getVolumes();

    for (std::vector<Geom::Volume*>::iterator iter3 = volumes.begin();
            iter3 != volumes.end(); ++iter3)

        // on n'observe les entités qu'une fois
        if (filtre_vu[*iter3] == false){
            filtre_vu[*iter3] = true;

            // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
            if (visibilityMask & Utils::FilterEntity::GeomVolume)
                filtre_geom[*iter3] = filtre_geom[*iter3] | mark;

            // faut-il continuer avec les surfaces ?
            if ((visibilityMask & Utils::FilterEntity::GeomSurface) && propagate){
                std::vector<Geom::Surface*> surfaces;
                (*iter3)->get(surfaces);

                for (std::vector<Geom::Surface*>::iterator iter2 = surfaces.begin();
                        iter2 != surfaces.end(); ++iter2)
                    filtre_geom[*iter2] = filtre_geom[*iter2] | mark;
            } // end if (visibilityMask & Utils::FilterEntity::GeomSurface)

            // faut-il continuer avec les courbes ?
            if ((visibilityMask & Utils::FilterEntity::GeomCurve) && propagate){
                std::vector<Geom::Curve*> curves;
                (*iter3)->get(curves);

                for (std::vector<Geom::Curve*>::iterator iter2 = curves.begin();
                        iter2 != curves.end(); ++iter2)
                    filtre_geom[*iter2] = filtre_geom[*iter2] | mark;
            } // end if (visibilityMask & Utils::FilterEntity::GeomCurve)

            // faut-il continuer avec les sommets ?
            if ((visibilityMask & Utils::FilterEntity::GeomVertex) && propagate){
                std::vector<Geom::Vertex*> vertices;
                (*iter3)->get(vertices);

                for (std::vector<Geom::Vertex*>::iterator iter2 = vertices.begin();
                        iter2 != vertices.end(); ++iter2)
                    filtre_geom[*iter2] = filtre_geom[*iter2] | mark;
            } // end if (visibilityMask & Utils::FilterEntity::GeomVertex)

            // faut-il propager à la topologie ?
            if (visibilityMask >= Utils::FilterEntity::TopoBlock){
                const std::vector<Topo::TopoEntity* >& topo = (*iter3)->getRefTopo();

                for (std::vector<Topo::TopoEntity* >::const_iterator iter2 = topo.begin();
                        iter2 != topo.end(); ++iter2){
                    if ((*iter2)->getDim() == 3){
                        Topo::TopoEntity* te = *iter2;
                        Topo::Block* blk = dynamic_cast<Topo::Block*>(te);
                        addMark(blk, visibilityMask, filtre_vu, filtre_topo, mark);
                    }
                }
            } // end if (visibilityMask >= Utils::FilterEntity::TopoBlock)

        } // end if (filtre_vu[*iter3] == false)

    if (visibilityMask >= Utils::FilterEntity::TopoBlock){
    	std::vector<Topo::Block*>& blocs = grp->getBlocks();

    	for (std::vector<Topo::Block*>::iterator iter1=blocs.begin();
    			iter1!=blocs.end(); ++iter1)
    		addMark(*iter1, visibilityMask, filtre_vu, filtre_topo, mark);
    }

    if (visibilityMask >= Utils::FilterEntity::SysCoord){
    	std::vector<CoordinateSystem::SysCoord*>& rep = grp->getSysCoord();

    	for (std::vector<CoordinateSystem::SysCoord*>::iterator iter1=rep.begin();
    			iter1!=rep.end(); ++iter1)
    		addMark(*iter1, visibilityMask, filtre_vu, filtre_rep, mark);
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

    std::vector<Geom::Surface*>& surfaces = grp->getSurfaces();

    for (std::vector<Geom::Surface*>::iterator iter3 = surfaces.begin();
            iter3 != surfaces.end(); ++iter3)

        // on n'observe les entités qu'une fois
        if (filtre_vu[*iter3] == false){
            filtre_vu[*iter3] = true;

            // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
            if (visibilityMask & Utils::FilterEntity::GeomSurface)
                filtre_geom[*iter3] = filtre_geom[*iter3] | mark;

            // faut-il continuer avec les courbes ?
            if ((visibilityMask & Utils::FilterEntity::GeomCurve) && propagate){
                std::vector<Geom::Curve*> curves;
                (*iter3)->get(curves);

                for (std::vector<Geom::Curve*>::iterator iter2 = curves.begin();
                        iter2 != curves.end(); ++iter2)
                    filtre_geom[*iter2] = filtre_geom[*iter2] | mark;
            } // end if (visibilityMask & Utils::FilterEntity::GeomCurve)

            // faut-il continuer avec les sommets ?
            if ((visibilityMask & Utils::FilterEntity::GeomVertex) && propagate){
                std::vector<Geom::Vertex*> vertices;
                (*iter3)->get(vertices);

                for (std::vector<Geom::Vertex*>::iterator iter2 = vertices.begin();
                        iter2 != vertices.end(); ++iter2)
                    filtre_geom[*iter2] = filtre_geom[*iter2] | mark;
            } // end if (visibilityMask & Utils::FilterEntity::GeomVertex)

            // faut-il propager à la topologie ?
            if (visibilityMask >= Utils::FilterEntity::TopoCoFace){
                const std::vector<Topo::TopoEntity* >& topo = (*iter3)->getRefTopo();

                for (std::vector<Topo::TopoEntity* >::const_iterator iter2 = topo.begin();
                        iter2 != topo.end(); ++iter2){
                    if ((*iter2)->getDim() == 2){
                        Topo::TopoEntity* te = *iter2;
                        Topo::CoFace* coface = dynamic_cast<Topo::CoFace*>(te);
                        addMark(coface, visibilityMask, filtre_vu, filtre_topo, mark);
                    }
                }
            } // end if (visibilityMask >= Utils::FilterEntity::TopoCoFace)

        } // end if (filtre_vu[*iter3] == false)


    if (visibilityMask >= Utils::FilterEntity::TopoCoFace){
    	std::vector<Topo::CoFace*>& cofaces = grp->getCoFaces();

    	for (std::vector<Topo::CoFace*>::iterator iter1=cofaces.begin();
    			iter1!=cofaces.end(); ++iter1)
    		addMark(*iter1, visibilityMask, filtre_vu, filtre_topo, mark);
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

    std::vector<Geom::Curve*>& curves = grp->getCurves();

    for (std::vector<Geom::Curve*>::iterator iter3 = curves.begin();
            iter3 != curves.end(); ++iter3)

        // on n'observe les entités qu'une fois
        if (filtre_vu[*iter3] == false){
            filtre_vu[*iter3] = true;

            // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
            if (visibilityMask & Utils::FilterEntity::GeomCurve)
                filtre_geom[*iter3] = filtre_geom[*iter3] | mark;

            // faut-il continuer avec les sommets ?
            if ((visibilityMask & Utils::FilterEntity::GeomVertex) && propagate){
                std::vector<Geom::Vertex*> vertices;
                (*iter3)->get(vertices);

                for (std::vector<Geom::Vertex*>::iterator iter2 = vertices.begin();
                        iter2 != vertices.end(); ++iter2)
                    filtre_geom[*iter2] = filtre_geom[*iter2] | mark;
            } // end if (visibilityMask & Utils::FilterEntity::GeomVertex)

            // faut-il propager à la topologie ?
            if (visibilityMask >= Utils::FilterEntity::TopoCoEdge){
                const std::vector<Topo::TopoEntity* >& topo = (*iter3)->getRefTopo();

                for (std::vector<Topo::TopoEntity* >::const_iterator iter2 = topo.begin();
                        iter2 != topo.end(); ++iter2){
                    if ((*iter2)->getDim() == 1){
                        Topo::TopoEntity* te = *iter2;
                        Topo::CoEdge* coedge = dynamic_cast<Topo::CoEdge*>(te);
                        addMark(coedge, visibilityMask, filtre_vu, filtre_topo, mark);
                    }
                }
            } // end if (visibilityMask >= Utils::FilterEntity::TopoCoEdge)

        } // end if (filtre_vu[*iter3] == false)

    if (visibilityMask >= Utils::FilterEntity::TopoCoEdge){
    	std::vector<Topo::CoEdge*>& coedges = grp->getCoEdges();

    	for (std::vector<Topo::CoEdge*>::iterator iter1=coedges.begin();
    			iter1!=coedges.end(); ++iter1)
    		addMark(*iter1, visibilityMask, filtre_vu, filtre_topo, mark);
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

    std::vector<Geom::Vertex*>& vertices = grp->getVertices();

    // on tient compte du masque pour savoir si on s'interesse à la visibilité de ce type d'entité
    if (visibilityMask & Utils::FilterEntity::GeomVertex)
        for (std::vector<Geom::Vertex*>::iterator iter3 = vertices.begin();
                iter3 != vertices.end(); ++iter3)

            // on n'observe les entités qu'une fois
            if (filtre_vu[*iter3] == false){
                filtre_vu[*iter3] = true;

                filtre_geom[*iter3] = filtre_geom[*iter3] | mark;

                // faut-il propager à la topologie ?
                if (visibilityMask >= Utils::FilterEntity::TopoVertex){
                    const std::vector<Topo::TopoEntity* >& topo = (*iter3)->getRefTopo();

                    for (std::vector<Topo::TopoEntity* >::const_iterator iter2 = topo.begin();
                            iter2 != topo.end(); ++iter2){
                        if ((*iter2)->getDim() == 0){
                            Topo::TopoEntity* te = *iter2;
                            Topo::Vertex* vertex = dynamic_cast<Topo::Vertex*>(te);
                            addMark(vertex, visibilityMask, filtre_vu, filtre_topo, mark);
                        }
                    }
                } // end if (visibilityMask >= Utils::FilterEntity::TopoVertex)

            } // end if (filtre_vu[*iter3] == false)

    if (visibilityMask >= Utils::FilterEntity::TopoVertex){
    	std::vector<Topo::Vertex*>& vertices = grp->getTopoVertices();

    	for (std::vector<Topo::Vertex*>::iterator iter1=vertices.begin();
    			iter1!=vertices.end(); ++iter1)
    		addMark(*iter1, visibilityMask, filtre_vu, filtre_topo, mark);
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
std::vector<std::string> GroupManager::getGeomEntities(const std::vector<std::string>& vg)
{
    std::set<Geom::GeomEntity*> initGeomEntities;

    for (uint i=0; i<vg.size(); i++){
        Group3D* gr3d = getGroup3D(vg[i], false);
        if (gr3d){
            std::vector<Geom::Volume*> volumes = gr3d->getVolumes();
            initGeomEntities.insert(volumes.begin(), volumes.end());
        } else {
            Group2D* gr2d = getGroup2D(vg[i], false);
            if (gr2d){
                std::vector<Geom::Surface*> surfaces = gr2d->getSurfaces();
                initGeomEntities.insert(surfaces.begin(), surfaces.end());
            } else {
                Group1D* gr1d = getGroup1D(vg[i], false);
                if (gr1d){
                    std::vector<Geom::Curve*> curves = gr1d->getCurves();
                    initGeomEntities.insert(curves.begin(), curves.end());
                } else {
                    Group0D* gr0d = getGroup0D(vg[i], false);
                    if (gr0d){
                        std::vector<Geom::Vertex*> vertices = gr0d->getVertices();
                        initGeomEntities.insert(vertices.begin(), vertices.end());
                    }
                }
            }
        }
    } // end for i<vg.size()

    std::vector<std::string> geomEntities;
    for (std::set<Geom::GeomEntity*>::iterator iter = initGeomEntities.begin();
    		iter != initGeomEntities.end(); ++iter)
    	geomEntities.push_back((*iter)->getName());

    return geomEntities;
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<std::string>& vg, std::vector<Geom::Volume*>& volumes)
{
    std::set<Geom::Volume*> initGeomEntities;

    for (uint i=0; i<vg.size(); i++){
        Group3D* gr3d = getGroup3D(vg[i], false);
        if (gr3d){
        	std::vector<Geom::Volume*> loc_volumes = gr3d->getVolumes();
        	initGeomEntities.insert(loc_volumes.begin(), loc_volumes.end());
        }
    } // end for i

    for (std::set<Geom::Volume*>::iterator iter = initGeomEntities.begin();
    		iter != initGeomEntities.end(); ++iter)
    	volumes.push_back((*iter));
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<std::string>& vg, std::vector<Geom::Surface*>& surfaces)
{
    std::set<Geom::Surface*> initGeomEntities;
    for (uint i=0; i<vg.size(); i++){
    	Group3D* gr3d = getGroup3D(vg[i], false);
    	if (gr3d){
    		std::vector<Geom::Volume*> volumes = gr3d->getVolumes();
    		for (uint j=0; j<volumes.size(); j++){
    			std::vector<Geom::Surface*> loc_surfaces;
    			volumes[j]->get(loc_surfaces);
    			initGeomEntities.insert(loc_surfaces.begin(), loc_surfaces.end());
    		}
    	} else {
    		Group2D* gr2d = getGroup2D(vg[i], false);
    		if (gr2d){
    			std::vector<Geom::Surface*> loc_surfaces = gr2d->getSurfaces();
    			initGeomEntities.insert(loc_surfaces.begin(), loc_surfaces.end());
    		}
    	}
    }

    for (std::set<Geom::Surface*>::iterator iter = initGeomEntities.begin();
    		iter != initGeomEntities.end(); ++iter)
    	surfaces.push_back((*iter));
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<std::string>& vg, std::vector<Geom::Curve*>& curves)
{
    std::set<Geom::Curve*> initGeomEntities;
    for (uint i=0; i<vg.size(); i++){
    	Group3D* gr3d = getGroup3D(vg[i], false);
    	if (gr3d){
    		std::vector<Geom::Volume*> volumes = gr3d->getVolumes();
    		for (uint j=0; j<volumes.size(); j++){
    			std::vector<Geom::Curve*> loc_curves;
    			volumes[j]->get(loc_curves);
    			initGeomEntities.insert(loc_curves.begin(), loc_curves.end());
    		}
    	} else {
            Group2D* gr2d = getGroup2D(vg[i], false);
            if (gr2d){
                std::vector<Geom::Surface*> surfaces = gr2d->getSurfaces();
                for (uint j=0; j<surfaces.size(); j++){
                	std::vector<Geom::Curve*> loc_curves;
                	surfaces[j]->get(loc_curves);
                	initGeomEntities.insert(loc_curves.begin(), loc_curves.end());
                }
            } else {
                Group1D* gr1d = getGroup1D(vg[i], false);
                if (gr1d){
                	std::vector<Geom::Curve*> loc_curves = gr1d->getCurves();
                	initGeomEntities.insert(loc_curves.begin(), loc_curves.end());
                }
            }
    	}
    }

    for (std::set<Geom::Curve*>::iterator iter = initGeomEntities.begin();
    		iter != initGeomEntities.end(); ++iter)
    	curves.push_back((*iter));
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<std::string>& vg, std::vector<Geom::Vertex*>& vertices)
{
    std::set<Geom::Vertex*> initGeomEntities;
    for (uint i=0; i<vg.size(); i++){
    	Group3D* gr3d = getGroup3D(vg[i], false);
    	if (gr3d){
    		std::vector<Geom::Volume*> volumes = gr3d->getVolumes();
    		for (uint j=0; j<volumes.size(); j++){
    			std::vector<Geom::Vertex*> loc_vertices;
    			volumes[j]->get(loc_vertices);
    			initGeomEntities.insert(loc_vertices.begin(), loc_vertices.end());
    		}
    	} else {
            Group2D* gr2d = getGroup2D(vg[i], false);
            if (gr2d){
                std::vector<Geom::Surface*> surfaces = gr2d->getSurfaces();
                for (uint j=0; j<surfaces.size(); j++){
                	std::vector<Geom::Vertex*> loc_vertices;
                	surfaces[j]->get(loc_vertices);
                	initGeomEntities.insert(loc_vertices.begin(), loc_vertices.end());
                }
            } else {
            	Group1D* gr1d = getGroup1D(vg[i], false);
            	if (gr1d){
            		std::vector<Geom::Curve*> curves = gr1d->getCurves();
            		for (uint j=0; j<curves.size(); j++){
            			std::vector<Geom::Vertex*> loc_vertices;
            			curves[j]->get(loc_vertices);
            			initGeomEntities.insert(loc_vertices.begin(), loc_vertices.end());
            		}
            	} else {
            		Group0D* gr0d = getGroup0D(vg[i], false);
            		if (gr0d){
            			std::vector<Geom::Vertex*> loc_vertices = gr0d->getVertices();
            			initGeomEntities.insert(loc_vertices.begin(), loc_vertices.end());
            		}
            	}
            }
    	}
    }

    for (std::set<Geom::Vertex*>::iterator iter = initGeomEntities.begin();
    		iter != initGeomEntities.end(); ++iter)
    	vertices.push_back((*iter));
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<std::string>& vg, std::vector<Topo::Block*>& blocks)
{
	std::set<Topo::Block*> initTopoEntities;

	for (uint i=0; i<vg.size(); i++){
		Group3D* gr3d = getGroup3D(vg[i], false);
		if (gr3d){
			Topo::TopoHelper::get(gr3d->getVolumes(), initTopoEntities);
			std::vector<Topo::Block*>& te = gr3d->getBlocks();
			initTopoEntities.insert(te.begin(), te.end());
		}
	}

	for (std::set<Topo::Block*>::iterator iter = initTopoEntities.begin();
			iter != initTopoEntities.end(); ++iter)
		blocks.push_back((*iter));
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<std::string>& vg, std::vector<Topo::CoFace*>& cofaces)
{
	bool propagate = getPropagate();

	std::set<Topo::CoFace*> initTopoEntities;

	for (uint i=0; i<vg.size(); i++){
		Group3D* gr3d = getGroup3D(vg[i], false);
		if (gr3d){
			Topo::TopoHelper::get(gr3d->getVolumes(), initTopoEntities, propagate);
			Topo::TopoHelper::get(gr3d->getBlocks(), initTopoEntities);
		}
		else {
			Group2D* gr2d = getGroup2D(vg[i], false);
			if (gr2d){
				Topo::TopoHelper::get(gr2d->getSurfaces(), initTopoEntities);
				std::vector<Topo::CoFace*>& te = gr2d->getCoFaces();
				initTopoEntities.insert(te.begin(), te.end());
			}
		}
	}

	for (std::set<Topo::CoFace*>::iterator iter = initTopoEntities.begin();
			iter != initTopoEntities.end(); ++iter)
		cofaces.push_back((*iter));
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<std::string>& vg, std::vector<Topo::CoEdge*>& coedges)
{
	bool propagate = getPropagate();

	std::set<Topo::CoEdge*> initTopoEntities;

	for (uint i=0; i<vg.size(); i++){
		Group3D* gr3d = getGroup3D(vg[i], false);
		if (gr3d){
			Topo::TopoHelper::get(gr3d->getVolumes(), initTopoEntities, propagate);
			Topo::TopoHelper::get(gr3d->getBlocks(), initTopoEntities);
		}
		else {
			Group2D* gr2d = getGroup2D(vg[i], false);
			if (gr2d){
				Topo::TopoHelper::get(gr2d->getSurfaces(), initTopoEntities, propagate);
				Topo::TopoHelper::get(gr2d->getCoFaces(), initTopoEntities);
			}
			else {
				Group1D* gr1d = getGroup1D(vg[i], false);
				if (gr1d){
					Topo::TopoHelper::get(gr1d->getCurves(), initTopoEntities);
					std::vector<Topo::CoEdge*>& te = gr1d->getCoEdges();
					initTopoEntities.insert(te.begin(), te.end());
				}
			}
		}
	}

	for (std::set<Topo::CoEdge*>::iterator iter = initTopoEntities.begin();
			iter != initTopoEntities.end(); ++iter)
		coedges.push_back((*iter));
}
/*----------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<std::string>& vg, std::vector<Topo::Vertex*>& vertices)
{
	bool propagate = getPropagate();

	std::set<Topo::Vertex*> initTopoEntities;

	for (uint i=0; i<vg.size(); i++){
		Group3D* gr3d = getGroup3D(vg[i], false);
		if (gr3d){
			Topo::TopoHelper::get(gr3d->getVolumes(), initTopoEntities, propagate);
			Topo::TopoHelper::get(gr3d->getBlocks(), initTopoEntities);
		}
		else {
			Group2D* gr2d = getGroup2D(vg[i], false);
			if (gr2d){
				Topo::TopoHelper::get(gr2d->getSurfaces(), initTopoEntities, propagate);
				Topo::TopoHelper::get(gr2d->getCoFaces(), initTopoEntities);
			}
			else {
				Group1D* gr1d = getGroup1D(vg[i], false);
				if (gr1d){
					Topo::TopoHelper::get(gr1d->getCurves(), initTopoEntities, propagate);
					Topo::TopoHelper::get(gr1d->getCoEdges(), initTopoEntities);
				}
				else {
					Group0D* gr0d = getGroup0D(vg[i], false);
					if (gr0d){
						Topo::TopoHelper::get(gr0d->getVertices(), initTopoEntities);
						std::vector<Topo::Vertex*>& te = gr0d->getTopoVertices();
						initTopoEntities.insert(te.begin(), te.end());
					}

				}
			}
		}
	}

	for (std::set<Topo::Vertex*>::iterator iter = initTopoEntities.begin();
			iter != initTopoEntities.end(); ++iter)
		vertices.push_back((*iter));
}

/*------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<std::string>& vg, std::vector<Mesh::Volume*>& volumes)
{
	Mesh::MeshManager& mmng = getLocalContext().getLocalMeshManager();

	for (uint i=0; i<vg.size(); i++){
		Mesh::Volume* me = mmng.getVolume(vg[i], false);

		if (me)
			volumes.push_back(me);
	}
}
/*------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<std::string>& vg, std::vector<Mesh::Surface*>& surfaces)
{
	Mesh::MeshManager& mmng = getLocalContext().getLocalMeshManager();

	for (uint i=0; i<vg.size(); i++){
		Mesh::Surface* me = mmng.getSurface(vg[i], false);

		if (me)
			surfaces.push_back(me);
	}
}
/*------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<std::string>& vg, std::vector<Mesh::Line*>& lines)
{
	Mesh::MeshManager& mmng = getLocalContext().getLocalMeshManager();

	for (uint i=0; i<vg.size(); i++){
		Mesh::Line* me = mmng.getLine(vg[i], false);

		if (me)
			lines.push_back(me);
	}
}
/*------------------------------------------------------------------------*/
void GroupManager::get(const std::vector<std::string>& vg, std::vector<Mesh::Cloud*>& clouds)
{
	Mesh::MeshManager& mmng = getLocalContext().getLocalMeshManager();

	for (uint i=0; i<vg.size(); i++){
		Mesh::Cloud* me = mmng.getCloud(vg[i], false);

		if (me)
			clouds.push_back(me);
	}
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getGeomVolumes(const std::vector<std::string>& vg)
{
	std::vector<std::string> result;
	std::vector<Geom::Volume*> geom_entities;
	get(vg, geom_entities);

	for (std::vector<Geom::Volume*>::iterator iter=geom_entities.begin();
			iter!=geom_entities.end(); ++iter)
		result.push_back((*iter)->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getGeomSurfaces(const std::vector<std::string>& vg)
{
	std::vector<std::string> result;
	std::vector<Geom::Surface*> geom_entities;
	get(vg, geom_entities);

	for (std::vector<Geom::Surface*>::iterator iter=geom_entities.begin();
			iter!=geom_entities.end(); ++iter)
		result.push_back((*iter)->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getGeomCurves(const std::vector<std::string>& vg)
{
	std::vector<std::string> result;
	std::vector<Geom::Curve*> geom_entities;
	get(vg, geom_entities);

	for (std::vector<Geom::Curve*>::iterator iter=geom_entities.begin();
			iter!=geom_entities.end(); ++iter)
		result.push_back((*iter)->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getGeomVertices(const std::vector<std::string>& vg)
{
	std::vector<std::string> result;
	std::vector<Geom::Vertex*> geom_entities;
	get(vg, geom_entities);

	for (std::vector<Geom::Vertex*>::iterator iter=geom_entities.begin();
			iter!=geom_entities.end(); ++iter)
		result.push_back((*iter)->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getTopoBlocks(const std::vector<std::string>& vg)
{
	std::vector<std::string> result;
	std::vector<Topo::Block*> topo_entities;
	get(vg, topo_entities);

	for (std::vector<Topo::Block*>::iterator iter=topo_entities.begin();
			iter!=topo_entities.end(); ++iter)
		result.push_back((*iter)->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getTopoFaces(const std::vector<std::string>& vg)
{
	std::vector<std::string> result;
	std::vector<Topo::CoFace*> topo_entities;
	get(vg, topo_entities);

	for (std::vector<Topo::CoFace*>::iterator iter=topo_entities.begin();
			iter!=topo_entities.end(); ++iter)
		result.push_back((*iter)->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getTopoEdges(const std::vector<std::string>& vg)
{
	std::vector<std::string> result;
	std::vector<Topo::CoEdge*> topo_entities;
	get(vg, topo_entities);

	for (std::vector<Topo::CoEdge*>::iterator iter=topo_entities.begin();
			iter!=topo_entities.end(); ++iter)
		result.push_back((*iter)->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getTopoVertices(const std::vector<std::string>& vg)
{
	std::vector<std::string> result;
	std::vector<Topo::Vertex*> topo_entities;
	get(vg, topo_entities);

	for (std::vector<Topo::Vertex*>::iterator iter=topo_entities.begin();
			iter!=topo_entities.end(); ++iter)
		result.push_back((*iter)->getName());

	return result;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> GroupManager::getVisibles() const
{
	std::vector<std::string> visibles;

	for (std::vector<Group3D*>::const_iterator iter = m_group3D.begin();
			iter != m_group3D.end(); ++iter)
		if ((*iter)->isVisible() && !(*iter)->isDestroyed())
			visibles.push_back((*iter)->getName());

	for (std::vector<Group2D*>::const_iterator iter = m_group2D.begin();
			iter != m_group2D.end(); ++iter)
		if ((*iter)->isVisible() && !(*iter)->isDestroyed())
			visibles.push_back((*iter)->getName());

	for (std::vector<Group1D*>::const_iterator iter = m_group1D.begin();
			iter != m_group1D.end(); ++iter)
		if ((*iter)->isVisible() && !(*iter)->isDestroyed())
			visibles.push_back((*iter)->getName());

	for (std::vector<Group0D*>::const_iterator iter = m_group0D.begin();
			iter != m_group0D.end(); ++iter)
		if ((*iter)->isVisible() && !(*iter)->isDestroyed())
			visibles.push_back((*iter)->getName());

	return visibles;
}
/*----------------------------------------------------------------------------*/
void GroupManager::addProjectionOnPX0(const std::string& nom)
{
	// recherche du groupe 2D
	Group2D* gr2d = getGroup2D(nom, true);

	// création de l'objet qui va modifier le maillage
	Mesh::MeshModificationItf* modif = new Mesh::MeshModificationByProjectionOnP0(Mesh::MeshModificationByProjectionOnP0::X);

	CommandAddMeshModification* command = new CommandAddMeshModification(getLocalContext(), gr2d, modif);
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
	Group2D* gr2d = getGroup2D(nom, true);

	// création de l'objet qui va modifier le maillage
	Mesh::MeshModificationItf* modif = new Mesh::MeshModificationByProjectionOnP0(Mesh::MeshModificationByProjectionOnP0::Y);

	CommandAddMeshModification* command = new CommandAddMeshModification(getLocalContext(), gr2d, modif);
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
	Group2D* gr2d = getGroup2D(nom, true);

	// création de l'objet qui va modifier le maillage
	Mesh::MeshModificationItf* modif = new Mesh::MeshModificationByProjectionOnP0(Mesh::MeshModificationByProjectionOnP0::Z);

	CommandAddMeshModification* command = new CommandAddMeshModification(getLocalContext(), gr2d, modif);
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
	PyObject* pyName = PyObject_GetAttrString(py_obj, "func_name");
#ifdef _DEBUG2
	std::cout<<"GroupManager::addCartesianPerturbation("<<nom<<", "<<PyString_AsString(pyName)<<")"<<std::endl;
#endif
	// recherche du groupe 2D ou 3D
	Group2D* gr2d = getGroup2D(nom, false);
	Group3D* gr3d = getGroup3D(nom, false);

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
	CommandAddMeshModification* command = new CommandAddMeshModification(getLocalContext(), grp, modif);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().addCartesianPerturbation (\""
        << nom<<"\", "<<PyString_AsString(pyName)<<")";
    command->setScriptCommand(cmd);

    getContext().getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void GroupManager::addPolarPerturbation(const std::string& nom, PyObject* py_obj)
{
	PyObject* pyName = PyObject_GetAttrString(py_obj, "func_name");
#ifdef _DEBUG2
	std::cout<<"GroupManager::addPolarPerturbation("<<nom<<", "<<PyString_AsString(pyName)<<")"<<std::endl;
#endif
	// recherche du groupe 2D ou 3D
	Group2D* gr2d = getGroup2D(nom, false);
	Group3D* gr3d = getGroup3D(nom, false);

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
	CommandAddMeshModification* command = new CommandAddMeshModification(getLocalContext(), grp, modif);

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().addPolarPerturbation (\""
        << nom<<"\", "<<PyString_AsString(pyName)<<")";
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
	Group2D* gr2d = getGroup2D(nom, true);

	CommandAddMeshModification* command = new CommandAddMeshModification(getLocalContext(), gr2d, sm.clone());

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
	Group3D* gr3d = getGroup3D(nom, true);

	CommandAddMeshModification* command = new CommandAddMeshModification(getLocalContext(), gr3d, sm.clone());

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
	Group2D* gr2d = getGroup2D(nom, true);

	CommandAddMeshModification* command = new CommandAddMeshModification(getLocalContext(), gr2d, ASepa.clone());

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getGroupManager().addSepa (\""
        << nom<<"\","<<ASepa.getScriptCommand()<<")";
    command->setScriptCommand(cmd);

    getContext().getCommandManager().addCommand(command, Utils::Command::DO);
}
/*----------------------------------------------------------------------------*/
void GroupManager::addToGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{
    // récupération des entités géométriques associées
    std::vector<std::string> geom_entity_names = getGeomEntities(ve);

    getContext().getGeomManager().addToGroup(geom_entity_names, dim, groupName);
}
/*----------------------------------------------------------------------------*/
void GroupManager::removeFromGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{
    // récupération des entités géométriques associées
    std::vector<std::string> geom_entity_names = getGeomEntities(ve);

    getContext().getGeomManager().removeFromGroup(geom_entity_names, dim, groupName);
}
/*----------------------------------------------------------------------------*/
void GroupManager::setGroup(std::vector<std::string>& ve, int dim, const std::string& groupName)
{
    // récupération des entités géométriques associées
    std::vector<std::string> geom_entity_names = getGeomEntities(ve);

    getContext().getGeomManager().setGroup(geom_entity_names, dim, groupName);
}
/*----------------------------------------------------------------------------*/
void GroupManager::setLevel(std::vector<std::string>& vg, int dim, int level)
{
	   switch(dim){
	    case(3):{
	        for (uint i=0; i<vg.size(); i++){
	            Group3D* gr = getGroup3D(vg[i], true);
	            gr->setLevel(level);
	        } // end for i
	    }
	    break;
	    case(2):{
	        for (uint i=0; i<vg.size(); i++){
	            Group2D* gr = getGroup2D(vg[i], true);
	            gr->setLevel(level);
	        } // end for i
	    }
	    break;
	    case(1):{
	        for (uint i=0; i<vg.size(); i++){
	            Group1D* gr = getGroup1D(vg[i], true);
	            gr->setLevel(level);
	        } // end for i
	    }
	    break;
	    case(0):{
	        for (uint i=0; i<vg.size(); i++){
	            Group0D* gr = getGroup0D(vg[i], true);
	            gr->setLevel(level);
	        } // end for i
	    }
	    break;
	    default:
	        throw TkUtil::Exception (TkUtil::UTF8String ("dimension non prévue pour GroupManager::getInfos", TkUtil::Charset::UTF_8));
	    }

}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
















