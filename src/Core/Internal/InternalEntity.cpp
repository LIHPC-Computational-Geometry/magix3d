/*----------------------------------------------------------------------------*/
/*
 * \file InternalEntity.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2 mars 2015
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/InternalEntity.h"
#include "Internal/Context.h"
#include "Utils/SerializedRepresentation.h"
#include "TkUtil/MemoryError.h"
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
InternalEntity::InternalEntity(Internal::Context& ctx,
		Utils::Property* prop,
		Utils::DisplayProperties* disp)
: Entity(ctx.newUniqueId(), prop, disp, ctx.getLogStream())
, m_context(ctx)
{
	m_context.add(getUniqueId(), this);
#ifdef _DEBUG2
	std::cout<<"Construction de "<<getName()<<", id = "<<getUniqueId()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
InternalEntity::~InternalEntity()
{
#ifdef _DEBUG2
	std::cout<<"Destruction de "<<getName()<<", id = "<<getUniqueId()<<std::endl;
#endif
	try {
		m_context.remove(getUniqueId());
	}
	catch (const TkUtil::Exception& exc) {
		std::cerr<<exc.getMessage()<<std::endl;
	}
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* InternalEntity::
getDescription (bool alsoComputed) const
{
	std::unique_ptr<Utils::SerializedRepresentation>	description	(
										Entity::getDescription (alsoComputed));
	CHECK_NULL_PTR_ERROR (description.get ( ));

#ifdef _DEBUG2
	Utils::SerializedRepresentation  relationsDescription (
                                                "Relations vers contexte", "");

	relationsDescription.addProperty (
			Utils::SerializedRepresentation::Property ("Nom du contexte", getContext().getName()));

	description->addPropertiesSet (relationsDescription);
#endif

	return description.release ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
