/*----------------------------------------------------------------------------*/
/*
 * \file InfoCommand.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 14 déc. 2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "TkUtil/Exception.h"
#include "TkUtil/MemoryError.h"
#include "Utils/Common.h"
#include "Internal/InfoCommand.h"
#include "Geom/GeomEntity.h"
#include "Topo/TopoEntity.h"
#include "Topo/CoEdge.h"
#include "Mesh/MeshEntity.h"
#include "Group/GroupEntity.h"
#include <SysCoord/SysCoord.h>
#include "Structured/StructuredMeshEntity.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {

/*----------------------------------------------------------------------------*/
std::string InfoCommand::type2String(const InfoCommand::type &t)
{
    switch (t) {
    case InfoCommand::UNITIALIZED: return "UNITIALIZED";
    case InfoCommand::LENGTHUNITMODIFIED:return "LENGTHUNITMODIFIED";
    case InfoCommand::MESHDIMMODIFIED:return "MESHDIMMODIFIED";
    case InfoCommand::LANDMARKMODIFIED:return "LANDMARKMODIFIED";
    case InfoCommand::OTHERMODIFIED: return "OTHERMODIFIED";
    case InfoCommand::DISPMODIFIED: return "DISPMODIFIED";
    case InfoCommand::VISIBILYCHANGED: return "VISIBILYCHANGED";
    case InfoCommand::CREATED: return "CREATED";
    case InfoCommand::DELETED: return "DELETED";
    case InfoCommand::ENABLE: return "ENABLE";
    case InfoCommand::DISABLE: return "DISABLE";
    case InfoCommand::NONE: return "NONE";
    }
	throw TkUtil::Exception(TkUtil::UTF8String ("InfoCommand::type2String avec type non prévu", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
InfoCommand::InfoCommand()
	: m_context_type (InfoCommand::UNITIALIZED), m_mutex ( )
{
#ifdef _DEBUG2
    std::cout<<"InfoCommand::InfoCommand()"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
InfoCommand::~InfoCommand()
{
//#define _DEBUG_MEMORY
#ifdef _DEBUG_MEMORY
    if (!m_topo_entities_info.empty()){
        std::cout<<"InfoCommand::~InfoCommand() libère les TopoEntity :"<<std::endl;
        for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = m_topo_entities_info.begin();
                iter_tei != m_topo_entities_info.end(); ++iter_tei){
            Topo::TopoEntity* te = iter_tei->first;
            std::cout<<te->getName()<<", unique id "<<te->getUniqueId()
                    <<" , type "<<type2String(iter_tei->second)
                    <<std::endl;
        }
    }
    else
        std::cout<<"InfoCommand::~InfoCommand() ne libère pas de TopoEntity"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
InfoCommand::InfoCommand(const InfoCommand&)
	: m_context_type (InfoCommand::UNITIALIZED), m_mutex ( )
{
    MGX_FORBIDDEN("InfoCommand::InfoCommand is not allowed.");
}
/*----------------------------------------------------------------------------*/
InfoCommand& InfoCommand::operator = (const InfoCommand&)
{
    MGX_FORBIDDEN("InfoCommand::InfoCommand is not allowed.");
    return *this;
}
/*----------------------------------------------------------------------------*/
void InfoCommand::addContextInfo(type t)
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
#ifdef _DEBUG2
    std::cout <<"InfoCommand::addContextInfo("
            <<" t = "<<type2String(t)<<")"<<std::endl;
#endif
	m_context_type	= t;
}
/*----------------------------------------------------------------------------*/
void InfoCommand::getContextInfo(type &t)
{
	t	= m_context_type;
}
/*----------------------------------------------------------------------------*/
void InfoCommand::addGeomInfoEntity(Geom::GeomEntity* entity, type t)
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
#ifdef _DEBUG2
    std::cout <<"InfoCommand::addGeomInfoEntity("<<entity->getName()
            <<" t = "<<type2String(t)<<")"<<std::endl;
#endif
    GeomEntityInfo gei = {entity,t};
    m_geom_entities_info.push_back(gei);
}
/*----------------------------------------------------------------------------*/
void InfoCommand::addSysCoordInfoEntity(CoordinateSystem::SysCoord* entity, type t)
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
#ifdef _DEBUG2
    std::cout <<"InfoCommand::addSysCoordInfoEntity("<<entity->getName()
            <<" t = "<<type2String(t)<<")"<<std::endl;
#endif
    SysCoordEntityInfo rei = {entity,t};
    m_sys_coord_entities_info.push_back(rei);
}
/*----------------------------------------------------------------------------*/
void InfoCommand::addStructuredMeshInfoEntity(Structured::StructuredMeshEntity* entity, type t)
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
#ifdef _DEBUG2
    std::cout <<"InfoCommand::addStructuredMeshInfoEntity("<<entity->getName()
            <<" t = "<<type2String(t)<<")"<<std::endl;
#endif
    StructuredMeshEntityInfo smei = {entity,t};
    m_structured_mesh_entities_info.push_back(smei);
}
/*----------------------------------------------------------------------------*/
bool InfoCommand::addTopoInfoEntity(Topo::TopoEntity* entity, type t)
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
    type old_t = m_topo_entities_info[entity];
#ifdef _DEBUG2
    std::cout <<"InfoCommand::addTopoInfoEntity("<<entity->getName()
            <<" t = "<<type2String(old_t)<<" => "<<type2String(t)<<")"<<std::endl;
#endif
    // les types de modifs sont ordonnées volontairement
    if (old_t < t){
        // on ne doit rien faire (pas d'affichage) d'une entité temporaire (le temps de faire la commande)
        if (old_t == CREATED && t == DELETED)
            m_topo_entities_info[entity] = NONE;
        else
            m_topo_entities_info[entity] = t;

        if (entity->getType() == Utils::Entity::TopoCoEdge && t == DISPMODIFIED){
        	Topo::CoEdge* coedge = dynamic_cast<Topo::CoEdge*>(entity);
        	CHECK_NULL_PTR_ERROR(coedge);
        	coedge->getMeshingProperty()->updateModificationTime();
        }

        return true;
    }
    else
        return false;
}
/*----------------------------------------------------------------------------*/
bool InfoCommand::addMeshInfoEntity(Mesh::MeshEntity* entity, type t)
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
#ifdef _DEBUG2
    std::cout<<"addMeshInfoEntity("<<entity->getName()<<","
            <<type2String(t)
            <<"), m_mesh_entities_info de taille "<<m_mesh_entities_info.size()
            <<std::endl;
#endif

   Mesh::MeshEntity* me(entity);

    // on évite d'ajouter une entité avec DISPMODIFIED lorsqu'elle est en cours de création
    // ce qui arrive avec les sous-volumes d'un même matériau
    if (t == DISPMODIFIED){
        // recherche si l'entité y est déjà
        std::vector <MeshEntityInfo>::iterator iter_found;
        bool found = false;
        for (std::vector <MeshEntityInfo>::iterator iter = m_mesh_entities_info.begin();
                iter != m_mesh_entities_info.end() && !found;
                ++iter)
            if (entity == iter->m_mesh_entity){
                found = true;
                iter_found = iter;
            }
        if (found){
#ifdef _DEBUG2
            std::cout<<" déjà présente et de type "<<type2String(iter_found->m_type)<<std::endl;
#endif
            if (t > iter_found->m_type){
            	iter_found->m_type = t;
            	return true;
            }
            else
            	return false;
        }
    }

    MeshEntityInfo mei = {me,t};
    m_mesh_entities_info.push_back(mei);

#ifdef _DEBUG2
    std::cout<<" nouvelle entité"<<std::endl;
#endif

    return true;
}
/*----------------------------------------------------------------------------*/
void InfoCommand::addGroupInfoEntity(Group::GroupEntity* entity, type t)
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
#ifdef _DEBUG2
    std::cout<<"addGroupInfoEntity("<<entity->getName()<<" (id "<<entity->getUniqueId()<<"),"
            <<type2String(m_group_entities_info[entity])<<" => "<<type2String(t)
            <<")"<<std::endl;
#endif
    type old_t = m_group_entities_info[entity];
    if (old_t < t)
        m_group_entities_info[entity] = t;
}
/*----------------------------------------------------------------------------*/
void InfoCommand::getGeomInfoEntity(uint ind, Geom::GeomEntity* &entity, type &t)
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
    if (ind >= m_geom_entities_info.size())
        throw TkUtil::Exception("InfoCommand::getGeomInfoEntity avec indice en dehors des bornes");

    GeomEntityInfo &gei = m_geom_entities_info[ind];

    entity = gei.m_geom_entity;
    t = gei.m_type;
}
/*----------------------------------------------------------------------------*/
void InfoCommand::getSysCoordInfoEntity(uint ind, CoordinateSystem::SysCoord* &entity, type &t)
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
    if (ind >= m_sys_coord_entities_info.size())
        throw TkUtil::Exception("InfoCommand::getSysCoordInfoEntity avec indice en dehors des bornes");

    SysCoordEntityInfo &rei = m_sys_coord_entities_info[ind];

    entity = rei.m_sys_coord_entity;
    t = rei.m_type;
}
/*----------------------------------------------------------------------------*/
std::vector<InfoCommand::TopoEntityInfo> InfoCommand::getSortedTopoInfoEntity()
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
	std::list<TopoEntityInfo> l_tie;

	for (std::map<Topo::TopoEntity*, InfoCommand::type>::iterator iter_tei = m_topo_entities_info.begin();
			iter_tei != m_topo_entities_info.end(); ++iter_tei){
		TopoEntityInfo tei = {iter_tei->first, iter_tei->second};
		l_tie.push_back(tei);
	}

	l_tie.sort(InfoCommand::compareTopoEntityInfo);

	std::vector<TopoEntityInfo> res;
	res.insert(res.end(), l_tie.begin(), l_tie.end());
	return res;
}
/*----------------------------------------------------------------------------*/
void InfoCommand::getMeshInfoEntity(uint ind, Mesh::MeshEntity* &entity, type &t)
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
    if (ind >= m_mesh_entities_info.size())
        throw TkUtil::Exception("InfoCommand::getMeshInfoEntity avec indice en dehors des bornes");

    MeshEntityInfo &mei = m_mesh_entities_info[ind];

    entity = mei.m_mesh_entity;
    t = mei.m_type;
}
/*----------------------------------------------------------------------------*/
void InfoCommand::getStructuredMeshInfoEntity(uint ind, Structured::StructuredMeshEntity* &entity, type &t)
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
    if (ind >= m_structured_mesh_entities_info.size())
        throw TkUtil::Exception("InfoCommand::getStructuredMeshInfoEntity avec indice en dehors des bornes");

    StructuredMeshEntityInfo &smei = m_structured_mesh_entities_info[ind];

    entity = smei.m_mesh_entity;
    t = smei.m_type;
}
/*----------------------------------------------------------------------------*/
void InfoCommand::permCreatedDeleted()
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
#ifdef _DEBUG2
    std::cout <<"InfoCommand::permCreatedDeleted()"<<std::endl;
#endif

   // pour chaque type d'entité, on change la marque et on permute
    for (std::vector <GeomEntityInfo>::iterator iter = m_geom_entities_info.begin();
            iter != m_geom_entities_info.end(); ++iter) {

        GeomEntityInfo &gei = *iter;
        if (gei.m_type == CREATED){
            gei.m_geom_entity->setDestroyed(true);
            gei.m_type = DELETED;
        }
        else if (gei.m_type == DELETED){
            gei.m_geom_entity->setDestroyed(false);
            gei.m_type = CREATED;
        }
    } // end for ( ... iter ...)

    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = m_topo_entities_info.begin();
            iter_tei != m_topo_entities_info.end(); ++iter_tei){
        Topo::TopoEntity* te = iter_tei->first;
        Internal::InfoCommand::type& t = iter_tei->second;
#ifdef _DEBUG2
        std::cout <<"InfoCommand::permCreatedDeleted pour Topo "<<te->getName()
                  <<" avec t = "<<type2String(t)<<std::endl;
#endif

        if (t == CREATED){
            te->setDestroyed(true);
            t = DELETED;
        }
        else if (t == DELETED){
            te->setDestroyed(false);
            t = CREATED;
        }
    } // end for ( ... iter ...)

    for (std::vector <MeshEntityInfo>::iterator iter = m_mesh_entities_info.begin();
            iter != m_mesh_entities_info.end(); ++iter) {

        MeshEntityInfo &mei = *iter;
#ifdef _DEBUG2
        std::cout <<"InfoCommand::permCreatedDeleted pour Mesh "<<mei.m_mesh_entity->getName()
                  <<" avec t = "<<type2String(mei.m_type)<<std::endl;
#endif
        if (mei.m_type == CREATED){
            mei.m_mesh_entity->setDestroyed(true);
            mei.m_type = DELETED;
        }
        else if (mei.m_type == DELETED){
            mei.m_mesh_entity->setDestroyed(false);
            mei.m_type = CREATED;
        }
    } // end for ( ... iter ...)

    for (std::vector <StructuredMeshEntityInfo>::iterator iter = m_structured_mesh_entities_info.begin();
            iter != m_structured_mesh_entities_info.end(); ++iter) {

        StructuredMeshEntityInfo &smei = *iter;
#ifdef _DEBUG2
        std::cout <<"InfoCommand::permCreatedDeleted pour StructuredMesh "<<smei.m_mesh_entity->getName()
                  <<" avec t = "<<type2String(smei.m_type)<<std::endl;
#endif
        if (smei.m_type == CREATED){
            smei.m_mesh_entity->setDestroyed(true);
            smei.m_type = DELETED;
        }
        else if (smei.m_type == DELETED){
            smei.m_mesh_entity->setDestroyed(false);
            smei.m_type = CREATED;
        }
    } // end for ( ... iter ...)

    for (std::map<Group::GroupEntity*, type>::iterator iter_grp = m_group_entities_info.begin();
    		iter_grp != m_group_entities_info.end(); ++iter_grp){
    	Group::GroupEntity* grp = iter_grp->first;
    	Internal::InfoCommand::type& t = iter_grp->second;
#ifdef _DEBUG2
    	std::cout <<"InfoCommand::permCreatedDeleted pour Group "<<grp->getName()
                        		  <<" (id "<<grp->getUniqueId()
                        		  <<") avec t = "<<type2String(t)<<std::endl;
#endif

    	if (t == CREATED){
    		grp->setDestroyed(true);
    		t = DELETED;
    	}
    	else if (t == DELETED){
    		grp->setDestroyed(false);
    		t = CREATED;
    	}
    	else if (t == ENABLE){
    		grp->setDestroyed(true);
    		t = DISABLE;
    	}
    	else if (t == DISABLE){
    		grp->setDestroyed(false);
    		t = ENABLE;
    	}
    } // end for ( ... iter ...)

     for (std::vector <SysCoordEntityInfo>::iterator iter = m_sys_coord_entities_info.begin();
             iter != m_sys_coord_entities_info.end(); ++iter) {

    	 SysCoordEntityInfo &scei = *iter;
         if (scei.m_type == CREATED){
        	 scei.m_sys_coord_entity->setDestroyed(true);
        	 scei.m_type = DELETED;
         }
         else if (scei.m_type == DELETED){
        	 scei.m_sys_coord_entity->setDestroyed(false);
        	 scei.m_type = CREATED;
         }
     } // end for ( ... iter ...)


}
/*----------------------------------------------------------------------------*/
void InfoCommand::setDestroyAndUpdateConnectivity
(std::vector<Geom::GeomEntity*>& entitiesToRemove)
{
	TkUtil::AutoMutex	autoMutex (&m_mutex);
    for (std::vector <Geom::GeomEntity*>::iterator iter = entitiesToRemove.begin();
         iter != entitiesToRemove.end(); ++iter)
        (*iter)->setDestroyed(true);

    for (std::vector <Geom::GeomEntity*>::iterator iter = entitiesToRemove.begin();
         iter != entitiesToRemove.end(); ++iter)
    {
        Geom::GeomEntity* entity = *iter;
        std::vector<Geom::GeomEntity*> ref_entities;
        entity->getRefEntities(ref_entities);
        for(unsigned int i=0;i<ref_entities.size();i++){
            Geom::GeomEntity* ref_entity = ref_entities[i];
            if(!ref_entity->isDestroyed())
                ref_entity->remove(entity);
        }
    } // end for ( ... iter ...)

}
/*----------------------------------------------------------------------------*/
void InfoCommand::clear()
{
	//std::cout<<"InfoCommand::clear()..."<<std::endl;
	TkUtil::AutoMutex	autoMutex (&m_mutex);
    m_geom_entities_info.clear();
    m_topo_entities_info.clear();
    m_mesh_entities_info.clear();
    m_group_entities_info.clear();
    m_sys_coord_entities_info.clear();
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const InfoCommand & ic)
{
    o<< "InfoCommand GeomEntities: "<<"\n";
    const std::vector<InfoCommand::GeomEntityInfo> geomIE = ic.getGeomInfoEntities();
    for (std::vector <InfoCommand::GeomEntityInfo>::const_iterator iter = geomIE.begin();
            iter != geomIE.end(); ++iter)
        o << "  "<<iter->m_geom_entity->getName()<<" : "
          <<InfoCommand::type2String(iter->m_type)
          <<" (uid "<<(long)iter->m_geom_entity->getUniqueId()<<")"
          <<(iter->m_geom_entity->isDestroyed()?" [marquée à DETRUITE]":"")
          <<"\n";

    o<< "InfoCommand TopoEntities: "<<"\n";
    const std::map<Topo::TopoEntity*, InfoCommand::type> topoIE = ic.getTopoInfoEntity();
    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::const_iterator iter = topoIE.begin();
            iter != topoIE.end(); ++iter)
        o << "  "<<iter->first->getName()<<" : "
          <<InfoCommand::type2String(iter->second)
          <<" (uid "<<(long)iter->first->getUniqueId()<<")"
          <<(iter->first->isDestroyed()?" [marquée à DETRUITE]":"")
          <<"\n";

    o<< "InfoCommand MeshEntities: "<<"\n";
    const std::vector<InfoCommand::MeshEntityInfo> meshIE = ic.getMeshInfoEntities();
    for (std::vector <InfoCommand::MeshEntityInfo>::const_iterator iter = meshIE.begin();
            iter != meshIE.end(); ++iter)
        o << "  "<<iter->m_mesh_entity->getName()<<" : "
          <<InfoCommand::type2String(iter->m_type)
          <<" (uid "<<(long)iter->m_mesh_entity->getUniqueId()<<")"
          <<(iter->m_mesh_entity->isDestroyed()?" [marquée à DETRUITE]":"")
          <<"\n";

    o<< "InfoCommand GroupEntities: "<<"\n";
    const std::map<Group::GroupEntity*, InfoCommand::type> groupIE = ic.getGroupInfoEntity();
    for (std::map<Group::GroupEntity*, InfoCommand::type>::const_iterator iter = groupIE.begin();
            iter != groupIE.end(); ++iter)
        o << "  "<<iter->first->getName()<<" : "
          <<InfoCommand::type2String(iter->second)
          <<" (uid "<<(long)iter->first->getUniqueId()<<")"
          <<(iter->first->isDestroyed()?" [marquée à DETRUITE]":"")
          <<"\n";

    o<< "InfoCommand StructuredMeshEntities: "<<"\n";
    const std::vector<InfoCommand::StructuredMeshEntityInfo> smeshIE = ic.getStructuredMeshInfoEntities();
    for (std::vector <InfoCommand::StructuredMeshEntityInfo>::const_iterator iter = smeshIE.begin();
            iter != smeshIE.end(); ++iter)
        o << "  "<<iter->m_mesh_entity->getName()<<" : "
          <<InfoCommand::type2String(iter->m_type)
          <<" (uid "<<(long)iter->m_mesh_entity->getUniqueId()<<")"
          <<(iter->m_mesh_entity->isDestroyed()?" [marquée à DETRUITE]":"")
          <<"\n";

    return o;
}
/*----------------------------------------------------------------------------*/
std::ostream & operator << (std::ostream & o, const InfoCommand & ic)
{
    TkUtil::UTF8String us (TkUtil::Charset::UTF_8);
    us << ic;
    o << us;
    return o;
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
