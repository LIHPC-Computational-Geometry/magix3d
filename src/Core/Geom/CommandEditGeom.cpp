/*
 * CommandEditGeom.cpp
 *
 *  Created on: 25 janv. 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
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
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>

//#include <GMDSCommon/Timer.h>
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
CommandEditGeom::~CommandEditGeom()
{
    deleteMementos();
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
    std::map<GeomEntity*,MementoGeomEntity> ref;
    std::vector<std::list<GeomEntity*>*> refEntities;

    refEntities.push_back(&(getRefEntities(0))); //sommets
    refEntities.push_back(&(getRefEntities(1))); //courbes
    refEntities.push_back(&(getRefEntities(2))); //surfaces
    refEntities.push_back(&(getRefEntities(3))); //volumes

    // MODIF - LES ENTITES ADJ NE SONT PAS DES ENTITES DE REF
//    refEntities[0]->insert(refEntities[0]->begin(),getAdjEntities(0).begin(),getAdjEntities(0).end());
//    refEntities[1]->insert(refEntities[1]->begin(),getAdjEntities(1).begin(),getAdjEntities(1).end());
//    refEntities[2]->insert(refEntities[2]->begin(),getAdjEntities(2).begin(),getAdjEntities(2).end());
//    refEntities[3]->insert(refEntities[3]->begin(),getAdjEntities(3).begin(),getAdjEntities(3).end());

#ifdef _DEBUG2
    std::cerr<<"nb refs sommets = "<<getRefEntities(0).size()<<std::endl;
    std::cerr<<"nb refs curves  = "<<getRefEntities(1).size()<<std::endl;
    std::cerr<<"nb refs surfaces= "<<getRefEntities(2).size()<<std::endl;
    std::cerr<<"nb refs volumes = "<<getRefEntities(3).size()<<std::endl;
#endif
    for(unsigned int i=0;i<4;i++){
        std::list<GeomEntity*>* refList = refEntities[i];
        std::list<GeomEntity*>::iterator it;
        for(it=refList->begin();it!=refList->end();it++){
            GeomEntity* e= *it;
            MementoGeomEntity mem;
            e->createMemento(mem);
#ifdef _DEBUG2
           std::cerr<<"Memento cree pour "<<e->getName()<<std::endl;
#endif
            ref.insert(std::pair<GeomEntity*,MementoGeomEntity>(e,mem));
//            std::cerr<<"On vient de sauver l'etat de "<<e->getName()<<std::endl;
//            std::cerr<<"\t "<<mem.getGeomRepresentation()<<std::endl;
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
        getContext().getLocalGeomManager().addEntity(ge);
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

    //sauvegarde des mementos pour le undo/redo
    std::map<GeomEntity*,MementoGeomEntity> keeped_ref;
    for(unsigned int i=0;i<mod_entities.size();i++){
        GeomEntity* e = mod_entities[i];
        keeped_ref[e] = ref[e];
    }
    for(unsigned int i=0;i<mov_entities.size();i++){
        GeomEntity* e = mov_entities[i];
        keeped_ref[e] = ref[e];
    }
    for(unsigned int i=0;i<rem_entities.size();i++){
        GeomEntity* e = rem_entities[i];
        keeped_ref[e] = ref[e];
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
    permMementos();

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
    permMementos();

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
saveMementos(std::map<GeomEntity*,MementoGeomEntity> & candidates)
{
    std::map<GeomEntity*,MementoGeomEntity>::iterator it;
    for (it=candidates.begin();it!=candidates.end();it++){

        GeomEntity *e = it->first;

        MementoGeomEntity mem = it->second;
        m_mementos.insert(std::pair<GeomEntity*,MementoGeomEntity>(e,mem));
#ifdef _DEBUG_CANCEL
        std::cout<<"save Memento pour "<<e->getName()<<std::endl;
#endif
    }
}
/*----------------------------------------------------------------------------*/
void CommandEditGeom::permMementos()
{
    std::map<GeomEntity*,MementoGeomEntity>::iterator it =  m_mementos.begin();
    for(;it!=m_mementos.end();it++){
        GeomEntity *e = it->first;
        MementoGeomEntity mem_saved = it->second;
        MementoGeomEntity mem_current;
        e->createMemento(mem_current);
        it->second = mem_current;
        e->setFromMemento(mem_saved);
#ifdef _DEBUG_CANCEL
        std::cout<<"On a permute l'etat pour "<<e->getName()<<std::endl;
//        std::cerr<<"\t "<<mem_saved.getGeomRepresentation()<<std::endl;
#endif
    }

}
/*----------------------------------------------------------------------------*/
void CommandEditGeom::cancelMementos()
{
#ifdef _DEBUG_CANCEL
    std::cout<<"CommandEditGeom::cancelInternalsStats() ..."<<std::endl;
#endif
//    saveMementos();
//    permMementos();
//    deleteMementos();
//
//    getInfoCommand().permCreatedDeleted();
//    getInfoCommand().clear();
}
/*----------------------------------------------------------------------------*/
void CommandEditGeom::deleteMementos()
{
    m_mementos.clear();
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
                grp->add(res);
                res->add(grp);
                //std::cout<<"grp : "<<grp->getInfos()<<std::endl;
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
                grp->add(res);
                res->add(grp);
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
                grp->add(res);
                res->add(grp);
            }
        }
    }
    else if (ge1->getDim() == 0 && ge2->getDim() == 0){
    std::vector<Group::GroupEntity*> grp;
        ge1->getGroups(grp);
        Vertex* res = dynamic_cast<Vertex*>(ge2);
        for (std::vector<Group::GroupEntity*>::iterator iter = grp.begin();
                iter != grp.end(); ++iter){
            Group::Group0D* grp = dynamic_cast<Group::Group0D*>(*iter);
            if (res && grp && !res->find(grp)){
                grp->add(res);
                res->add(grp);
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

    // on utilise le groupe m_group_name (suivant la dimention)
    std::vector<GeomEntity*>& newEntities = getNewEntities();
    for (std::vector<GeomEntity*>::iterator iter = newEntities.begin();
            iter != newEntities.end(); ++iter){
        GeomEntity* ge = *iter;
        if (filtre_ge[ge] == 0){
            bool use_group_name = (ge->getDim() == m_dim_new_group); // && (filtre_ge[ge] == 0)
            addToGroup(ge, !use_group_name);
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
