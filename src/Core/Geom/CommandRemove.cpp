/*----------------------------------------------------------------------------*/
/*
 * CommandRemove.cpp
 *
 *  Created on: 10 janv. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandRemove.h"
#include "Geom/GeomManager.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandRemove::
CommandRemove(Internal::Context& c, std::vector<GeomEntity*>& es,
            const bool propagateDown)
: CommandEditGeom(c, "Destruction"), m_entities(es), m_propagate(propagateDown)
{
    m_impl = new GeomRemoveImplementation(c, m_entities,m_propagate);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Destruction de ";
	for (uint i=0; i<m_entities.size() && i<5; i++)
		comments << " " << m_entities[i]->getName();
	if (m_entities.size()>5)
		comments << " ... ";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandRemove::~CommandRemove()
{
    if(m_impl)
        delete m_impl;
}

/*----------------------------------------------------------------------------*/
void CommandRemove::internalSpecificPreExecute()
{
    m_impl->prePerform();
}/*----------------------------------------------------------------------------*/
void CommandRemove::
internalSpecificExecute()
{
    if(m_entities.empty())
        return;

    m_impl->perform(m_createdEntities);

}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
