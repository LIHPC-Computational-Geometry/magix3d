/*----------------------------------------------------------------------------*/
/*
 * \file GroupEntity.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18/10/2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
#include "Internal/Context.h"
#include "Utils/DisplayProperties.h"
#include <TkUtil/Exception.h>
#include "Mesh/MeshModificationItf.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
GroupEntity::GroupEntity(Internal::Context& ctx,
                       Utils::Property* prop,
                       Utils::DisplayProperties* disp,
					   bool isDefaultGroup,
					   uint level)
: Internal::InternalEntity (ctx, prop, disp)
, m_isDefaultGroup(isDefaultGroup)
, m_level(level)
{
#ifdef _DEBUG2
    std::cout<<"GroupEntity::GroupEntity("<<id<<") de nom "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
GroupEntity::~GroupEntity()
{
#ifdef _DEBUG2
    std::cout<<"GroupEntity::~GroupEntity() de "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void GroupEntity::setDestroyed(bool b)
{
    // cas sans changement
    if (isDestroyed() == b)
        return;

#ifdef _DEBUG2
    std::cout<<"GroupEntity::setDestroyed("<<b<<") de "<<getName()<<std::endl;;
#endif

    Entity::setDestroyed(b);
}
/*----------------------------------------------------------------------------*/
void GroupEntity::getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
    MGX_FORBIDDEN("Pas de représentation des entités Group...");
    throw TkUtil::Exception (TkUtil::UTF8String ("Il n'est pas prévu de représenter les entités de type Groupe", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* GroupEntity::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Mgx3D::Utils::SerializedRepresentation>   description (
    		InternalEntity::getDescription (alsoComputed));

    description->addProperty(Utils::SerializedRepresentation::Property ("Niveau", (long)getLevel()));

    if (!m_meshModif.empty()){
    	Utils::SerializedRepresentation  modification (
    			            "Modification du maillage", "");
    	for (std::vector<Mesh::MeshModificationItf*>::const_iterator iter = m_meshModif.begin();
    			iter != m_meshModif.end(); ++iter)

    		(*iter)->addToDescription(&modification);

    	description->addPropertiesSet(modification);
    }

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
std::vector<Utils::Entity*> GroupEntity::getEntities() const
{
	return std::vector<Utils::Entity*> ( );	// A surcharger
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
