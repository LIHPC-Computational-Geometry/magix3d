/*----------------------------------------------------------------------------*/
#include "Utils/Common.h"
#include "Internal/InfoCommand.h"
#include "Internal/Context.h"
#include "Geom/CommandEditGeom.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomEntity.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Group/GroupManager.h"
#include "Group/GroupEntity.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
/*----------------------------------------------------------------------------*/
using namespace TkUtil;
using namespace Mgx3D::Utils;
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandEditGeom::CommandEditGeom(Internal::Context& c, std::string name,
        const std::string& groupName)
: Geom::CommandCreateGeom(c, name,groupName)
{
    m_impl=0;
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG2
void CommandEditGeom::internalExecute()
{
//    gmds::Timer t1;
    // suppression du contenu pour le cas d'un redo par exemple
    getInfoCommand().clear();

    //pretraitement délégué aux classes filles
    internalSpecificPreExecute();

    // on recupere toutes les entites concernes par l'opération géométrique
    std::map<GeomEntity*, Services::Memento> mementos_by_entity;
    std::set<GeomEntity*> to_mem_entities[4];

    for(unsigned int i=0;i<4;i++) {
        to_mem_entities[i].insert(getRefEntities(i).begin(), getRefEntities(i).end());
        to_mem_entities[i].insert(getAdjEntities(i).begin(), getAdjEntities(i).end());
        for (GeomEntity* e : to_mem_entities[i]) {
            Services::Memento mem = m_memento_service.createMemento(e);
#ifdef _DEBUG2
           std::cerr<<"Memento cree pour "<<e->getName()<<std::endl;
#endif
            mementos_by_entity.insert({e, mem});
        }
    }
//    gmds::Timer t2;
//    std::cerr<<"Preparation pour undo : "<<t2-t1<<std::endl;
    //traitement délégué aux classes filles
    internalSpecificExecute();
//    gmds::Timer t3;
//    std::cerr<<"Exécution spécifique op : "<<t3-t2<<std::endl;

    //MISE A JOUR DES DONNES A TRANSMETTRE AU CONTEXTE:
    //nouvelles entités, entités conservées, entités supprimées,
    //entités déplacées
    std::vector<GeomEntity*>& new_entities = getNewEntities();
    std::vector<GeomEntity*>  mod_entities  = getKeepedEntities();
    std::vector<GeomEntity*>  mov_entities  = getMovedEntities();
    std::vector<GeomEntity*>& rem_entities = getRemovedEntities();
    std::map<GeomEntity*,std::vector<GeomEntity*> >& rep_entities = getReplacedEntities();

    for(int i=0;i<new_entities.size();i++){
        GeomEntity* ge = new_entities[i];
#ifdef _DEBUG2
        std::cerr<<"Ajoute --> "<<ge->getUniqueName()<<std::endl;
#endif
        getInfoCommand ( ).addGeomInfoEntity (ge, Internal::InfoCommand::CREATED);
        getContext().getGeomManager().addEntity(ge);
    }
    for(int i=0;i<mod_entities.size();i++){
        GeomEntity* ge = mod_entities[i];
#ifdef _DEBUG2
        std::cerr<<"Modifie --> "<<ge->getUniqueName()<<std::endl;
#endif
        getInfoCommand ( ).addGeomInfoEntity (mod_entities[i], Internal::InfoCommand::OTHERMODIFIED);
    }
    for(int i=0;i<mov_entities.size();i++){
        GeomEntity* ge = mov_entities[i];
#ifdef _DEBUG2
        std::cerr<<"Deplace --> "<<ge->getUniqueName()<<std::endl;
#endif
        getInfoCommand ( ).addGeomInfoEntity (mov_entities[i], Internal::InfoCommand::DISPMODIFIED);
    }
    for(int i=0;i<rem_entities.size();i++){
        GeomEntity* ge = rem_entities[i];
#ifdef _DEBUG2
        std::cerr<<"Supprime --> "<<ge->getUniqueName()<<std::endl;
#endif
        getInfoCommand ( ).addGeomInfoEntity (rem_entities[i], Internal::InfoCommand::DELETED);
    }

#ifdef _DEBUG2
    for (std::map<GeomEntity*,std::vector<GeomEntity*> >::iterator iter1 = rep_entities.begin();
    		iter1!=rep_entities.end(); ++iter1){
    	std::cerr<<"Remplace "<<iter1->first->getName()<<" par";
    	for (std::vector<GeomEntity*>::iterator iter2 = iter1->second.begin();
    			iter2 != iter1->second.end(); ++iter2)
    		std::cerr<<" "<<(*iter2)->getName();
    	std::cerr<<std::endl;
    }
#endif

    // Sauvegarde des mementos pour le undo/redo
    // Utiliser at en lecture pour avoir une exception si
    // la clef est absente (ne pas utiliser l'opérateur [])
    std::map<GeomEntity*,Services::Memento> keeped_ref;
    for(unsigned int i=0;i<mod_entities.size();i++){
        GeomEntity* e = mod_entities[i];
        if (mementos_by_entity.find(e) == mementos_by_entity.end())
            std::cout << "*** MOD les mementos ne contiennent pas " << e->getName() << std::endl;
        keeped_ref[e] = mementos_by_entity.at(e);
    }
    for(unsigned int i=0;i<mov_entities.size();i++){
        GeomEntity* e = mov_entities[i];
        if (mementos_by_entity.find(e) == mementos_by_entity.end())
            std::cout << "*** MOV les mementos ne contiennent pas " << e->getName() << std::endl;
        keeped_ref[e] = mementos_by_entity.at(e);
    }
    for(unsigned int i=0;i<rem_entities.size();i++){
        GeomEntity* e = rem_entities[i];
        if (mementos_by_entity.find(e) == mementos_by_entity.end())
            std::cout << "*** REM les mementos ne contiennent pas " << e->getName() << std::endl;
        keeped_ref[e] = mementos_by_entity.at(e);
    }
    saveMementos(keeped_ref);
    getInfoCommand().setDestroyAndUpdateConnectivity(rem_entities);

//    gmds::Timer t4;
//    std::cerr<<"Construction info commande + sauvegarde mementos: "<<t4-t3<<std::endl;

    updateGroups();
//    gmds::Timer t5;
//    std::cerr<<"Mise a jour des groupes : "<<t5-t4<<std::endl;

//    std::cout<<"getInfoCommand() à la fin de CommandEditGeom::internalExecute() : "<<getInfoCommand()<<std::endl;

}
/*----------------------------------------------------------------------------*/
void CommandEditGeom::internalUndo()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandEditGeom::internalUndo pour la commande " << getName ( )
                << " de nom unique " << getUniqueName ( );

    AutoReferencedMutex autoMutex (getMutex ( ));

    // permute toutes les propriétés internes avec leur sauvegarde
    m_memento_service.permMementos();

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();

    // Annulation de la commande pour les entités dont l'état n'est pas mémorisé (cas des FcetedSurface)
    internalSpecificUndo();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandEditGeom::internalRedo()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandEditGeom::internalRedo pour la commande " << getName ( )
                << " de nom unique " << getUniqueName ( );

    AutoReferencedMutex autoMutex (getMutex ( ));

    startingOrcompletionLog (true);

    // permute toutes les propriétés internes avec leur sauvegarde
    m_memento_service.permMementos();

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();

    // Rejoue la commande pour les entités dont l'état n'est pas mémorisé (cas des FcetedSurface)
    internalSpecificRedo();

    startingOrcompletionLog (false);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}

/*----------------------------------------------------------------------------*/
//#define _DEBUG_CANCEL
void CommandEditGeom::
saveMementos(std::map<GeomEntity*,Services::Memento> & candidates)
{
    for (const auto& pair : candidates) {
        m_memento_service.saveMemento(pair.first, pair.second);
#ifdef _DEBUG_CANCEL
        std::cout<<"save Memento pour "<<e->getName()<<std::endl;
#endif
    }
}
/*----------------------------------------------------------------------------*/
void CommandEditGeom::copyGroups(GeomEntity* ge1, GeomEntity* ge2)
{
    if (ge1->getDim() == 3 && ge2->getDim() == 3){
    std::vector<Group::GroupEntity*> grp;
        ge1->getGroups(grp);
        Volume* res = dynamic_cast<Volume*>(ge2);
        for (std::vector<Group::GroupEntity*>::iterator iter = grp.begin();
                iter != grp.end(); ++iter){
            Group::Group3D* grp = dynamic_cast<Group::Group3D*>(*iter);
            if (res && grp && !res->find(grp)){
                m_group_helper.addToGroup(grp->getName(), res);
            }
        }
    }
    else if (ge1->getDim() == 2 && ge2->getDim() == 2){
    std::vector<Group::GroupEntity*> grp;
        ge1->getGroups(grp);
        Surface* res = dynamic_cast<Surface*>(ge2);
        for (std::vector<Group::GroupEntity*>::iterator iter = grp.begin();
                iter != grp.end(); ++iter){
            Group::Group2D* grp = dynamic_cast<Group::Group2D*>(*iter);
            if (res && grp && !res->find(grp)){
                m_group_helper.addToGroup(grp->getName(), res);
            }
        }
    }
    else if (ge1->getDim() == 1 && ge2->getDim() == 1){
    std::vector<Group::GroupEntity*> grp;
        ge1->getGroups(grp);
        Curve* res = dynamic_cast<Curve*>(ge2);
        for (std::vector<Group::GroupEntity*>::iterator iter = grp.begin();
                iter != grp.end(); ++iter){
            Group::Group1D* grp = dynamic_cast<Group::Group1D*>(*iter);
            if (res && grp && !res->find(grp)){
                m_group_helper.addToGroup(grp->getName(), res);
            }
        }
    }
    else if (ge1->getDim() == 0 && ge2->getDim() == 0){
        Vertex* res = dynamic_cast<Vertex*>(ge2);
        for (Group::Group0D* grp : res->getGroups()) {
            if (res && grp && !res->find(grp)){
                m_group_helper.addToGroup(grp->getName(), res);
            }
        }
    }
    else {
        throw TkUtil::Exception(TkUtil::UTF8String ("Copie des groupes pour une dimension non prévue", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void CommandEditGeom::updateGroups()
{
#ifdef _DEBUG2
    std::cout<<"CommandEditGeom::updateGroups() m_dim_new_group = "<<m_dim_new_group<<std::endl;
#endif
    std::map<GeomEntity*,std::vector<GeomEntity*> >& replacedEntities = getReplacedEntities();

    // filtre pour identifier les entités nouvelles qui ne sont pas référencées dans replacedEntities
    std::map<GeomEntity*, uint > filtre_ge;

    for (std::map<GeomEntity*,std::vector<GeomEntity*> >::iterator iter = replacedEntities.begin();
            iter != replacedEntities.end(); ++iter){
        GeomEntity* ge = (*iter).first;
        std::vector<GeomEntity*>& new_entities = (*iter).second;

        // on copie les groupes de la première entité
        for(unsigned int i=0;i<new_entities.size();i++){
            copyGroups(ge, new_entities[i]);
            filtre_ge[new_entities[i]] = 1;
#ifdef _DEBUG2
            std::cout<<"  copyGroups de "<<ge->getName()<<" vers "<<new_entities[i]->getName()<<std::endl;
#endif
        }
    }

    // on utilise le groupe m_group_name (suivant la dimension)
    std::vector<GeomEntity*>& newEntities = getNewEntities();
    for (std::vector<GeomEntity*>::iterator iter = newEntities.begin();
            iter != newEntities.end(); ++iter){
        GeomEntity* ge = *iter;
        if (filtre_ge[ge] == 0){
            std::string group_name = (ge->getDim() == m_dim_new_group ? m_group_name : ""); // && (filtre_ge[ge] == 0)
            m_group_helper.addToGroup(group_name, ge);
#ifdef _DEBUG2
            std::cout<<"  addToGroup de "<<ge->getName()<<" avec groupe: "<<(use_group_name?m_group_name:"")<<std::endl;
#endif
            filtre_ge[ge] = 1;
        }
   }

}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*>& CommandEditGeom::getRemovedEntities()
{
	if (m_impl == 0)
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, getRemovedEntities avec m_impl vide", TkUtil::Charset::UTF_8));
    return m_impl->getRemovedEntities();
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*>& CommandEditGeom::getNewEntities()
{
	if (m_impl == 0)
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, getNewEntities avec m_impl vide", TkUtil::Charset::UTF_8));
    return m_impl->getNewEntities();
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*> CommandEditGeom::getKeepedEntities()
{
	if (m_impl == 0)
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, getKeepedEntities avec m_impl vide", TkUtil::Charset::UTF_8));
    return m_impl->getKeepedEntities();
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*>& CommandEditGeom::getMovedEntities()
{
	if (m_impl == 0)
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, getMovedEntities avec m_impl vide", TkUtil::Charset::UTF_8));
    return m_impl->getMovedEntities();
}
/*----------------------------------------------------------------------------*/
std::list<GeomEntity*>& CommandEditGeom::getRefEntities(const int dim)
{
	if (m_impl == 0)
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, getRefEntities avec m_impl vide", TkUtil::Charset::UTF_8));
    return m_impl->getRefEntities(dim);
}
/*----------------------------------------------------------------------------*/
std::list<GeomEntity*>& CommandEditGeom::getAdjEntities(const int dim)
{
	if (m_impl == 0)
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, getAdjEntities avec m_impl vide", TkUtil::Charset::UTF_8));
    return m_impl->getAdjEntities(dim);
}
/*----------------------------------------------------------------------------*/
std::map<GeomEntity*,std::vector<GeomEntity*> >& CommandEditGeom::
getReplacedEntities()
{
	if (m_impl == 0)
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, getReplacedEntities avec m_impl vide", TkUtil::Charset::UTF_8));
    return m_impl->getReplacedEntities();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
