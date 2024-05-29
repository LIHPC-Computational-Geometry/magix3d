/*----------------------------------------------------------------------------*/
/*
 * \file NameManager.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17 janv. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/NameManager.h"
#include "Utils/TypeDedicatedNameManager.h"
#include <TkUtil/Exception.h>

#include "Utils/Entity.h"

#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/Edge.h"
#include "Topo/CoFace.h"
#include "Topo/Face.h"
#include "Topo/Block.h"
#include "SysCoord/SysCoord.h"

#include <string>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
NameManager::NameManager()
: m_global_id(0)
, m_explorer_id(0)
, m_is_name_shifting(false)
{
    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Geom::Vertex::getTinyName()));
    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Geom::Curve::getTinyName()));
    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Geom::Surface::getTinyName()));
    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Geom::Volume::getTinyName()));
    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Topo::Vertex::getTinyName()));
    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Topo::CoEdge::getTinyName()));
    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Topo::Edge::getTinyName()));
    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Topo::CoFace::getTinyName()));
    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Topo::Face::getTinyName()));
    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Topo::Block::getTinyName()));
    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(CoordinateSystem::SysCoord::getTinyName()));
    // a priori, les suivant ne servent pas
    // [EB] sauf pour nommer les groupes de mailles pour les objets de base (cf CommandNewTopoOGridOnGeometry)
//    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Mesh::Cloud::getTinyName()));
//    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Mesh::Surface::getTinyName()));
//    m_all_type_name_managers.push_back(new Utils::TypeDedicatedNameManager(Mesh::Volume::getTinyName()));

}
/*----------------------------------------------------------------------------*/
NameManager::~NameManager()
{
    for(unsigned int i=0; i<m_all_type_name_managers.size();i++){
        Utils::TypeDedicatedNameManager* current_elt = m_all_type_name_managers[i];
        if (current_elt!=0)
            delete current_elt;
    }
}
/*----------------------------------------------------------------------------*/
void NameManager::clear()
{
	for(unsigned int i=0; i<m_all_type_name_managers.size();i++){
		Utils::TypeDedicatedNameManager* current_elt = m_all_type_name_managers[i];
		if (current_elt!=0)
			current_elt->setLastId(0);
	}
	m_global_id = 0;
}
/*----------------------------------------------------------------------------*/
Utils::TypeDedicatedNameManager* NameManager::
getTypeDedicatedNameManager(const Utils::Entity::objectType& ot)
{
    uint id = (uint) ot;

#ifdef _DEBUG
    if (id > (uint) Utils::Entity::StructuredMesh) // cf Utils::Entity::objectType
        throw TkUtil::Exception(TkUtil::UTF8String("Erreur interne, getTypeDedicatedNameManager avec indice en dehors de ce qui est prévu", TkUtil::Charset::UTF_8));
#endif

    return m_all_type_name_managers[id];
}
const Utils::TypeDedicatedNameManager* NameManager::
getTypeDedicatedNameManager(const Utils::Entity::objectType& ot) const
{
    uint id = (uint) ot;

#ifdef _DEBUG
    if (id > (uint) Utils::Entity::StructuredMesh) // cf Utils::Entity::objectType
        throw TkUtil::Exception(TkUtil::UTF8String("Erreur interne, getTypeDedicatedNameManager avec indice en dehors de ce qui est prévu", TkUtil::Charset::UTF_8));
#endif

    return m_all_type_name_managers[id];
}
/*----------------------------------------------------------------------------*/
void NameManager::
getInternalStats (std::vector<unsigned long> & vid)
{
    vid.clear();
    vid.push_back(m_global_id);
    vid.push_back(m_explorer_id);
    for (uint i=0; i<m_all_type_name_managers.size(); i++)
        vid.push_back(m_all_type_name_managers[i]->getLastId());

#ifdef _DEBUG2
	std::cout<<"NameManager::getInternalStats(...)"<<std::endl;
	std::cout<<" m_global_id = "<<vid[0]<<std::endl;
	for (uint i=0; i<m_all_type_name_managers.size(); i++)
		std::cout<<" setLastId pour "<<m_all_type_name_managers[i]->getBaseName()<<" = "<<vid[i+2]<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void NameManager::
setInternalStats (std::vector<unsigned long> & vid)
{
    if (vid.empty())
        throw TkUtil::Exception(TkUtil::UTF8String("Erreur interne, setInternalStats ne peut se faire avec un vecteur vide", TkUtil::Charset::UTF_8));

#ifdef _DEBUG2
    std::cout<<"NameManager::setInternalStats(...) avec "<<std::endl;
	std::cout<<" m_global_id = "<<vid[0]<<std::endl;
	for (uint i=0; i<m_all_type_name_managers.size(); i++)
		std::cout<<" setLastId pour "<<m_all_type_name_managers[i]->getBaseName()<<" = "<<vid[i+2]<<std::endl;
#endif


    m_global_id = vid[0];
    m_explorer_id = vid[1];
    for (uint i=0; i<m_all_type_name_managers.size(); i++)
        m_all_type_name_managers[i]->setLastId(vid[i+2]);
}
/*----------------------------------------------------------------------------*/
void NameManager::
activateShiftingId()
{
    // s'il n'y a pas d'entité, on ne fait rien
    if (m_global_id > 0){
        m_is_name_shifting = true;

        for (uint i=0; i<m_all_type_name_managers.size(); i++)
            m_all_type_name_managers[i]->memorizeLastId();
    }
}
/*----------------------------------------------------------------------------*/
void NameManager::
unactivateShiftingId()
{
    m_is_name_shifting = false;
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
