/*
 * CommandNewSurfaceByOffset.cpp
 *
 *  Created on: 19/09/2019
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/CommandNewSurfaceByOffset.h"
#include "Geom/GeomManager.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {

/*----------------------------------------------------------------------------*/
CommandNewSurfaceByOffset::
CommandNewSurfaceByOffset(Internal::Context& c,
		Surface* base,
		const double& offset,
		const std::string& groupName)
: CommandCreateGeom(c, "Création d'une surface par offset", groupName)
, m_entity(base)
, m_offset(offset)
{
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Nouvelle surface à partir de "<< base->getName()<<" et offset de "<<offset;
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandNewSurfaceByOffset::~CommandNewSurfaceByOffset()
{}
/*----------------------------------------------------------------------------*/
void CommandNewSurfaceByOffset::internalExecute()
{

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandNewSurfaceByOffset::execute pour la commande " << getName ( )
                    		   << " de nom unique " << getUniqueName ( )
							   << std::ios_base::fixed << TkUtil::setprecision (8);

	Surface* surf = EntityFactory(getContext()).newSurfaceByCopyWithOffset(m_entity, m_offset);
	m_createdEntities.push_back(surf);

    // découpage de la surface en entités de dimensions inférieures et stockage dans le manager géométrique
    split(surf);

	addToGroup(surf);

	log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
