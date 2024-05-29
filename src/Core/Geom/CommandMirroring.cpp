/*
 * CommandMirroring.cpp
 *
 *  Created on: 12/4/2016
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandMirroring.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomMirrorImplementation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandMirroring::CommandMirroring(Internal::Context& c,
		std::vector<GeomEntity*>& e,
		Utils::Math::Plane* plane) :
		CommandEditGeom(c, "Symétrie")
{
	m_impl = new GeomMirrorImplementation(c, e, plane);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Symétrie plane de ";
	for (uint i=0; i<e.size() && i<5; i++)
		comments << " " << e[i]->getName();
	if (e.size()>5)
		comments << " ... ";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandMirroring::CommandMirroring(Internal::Context& c,
		Geom::CommandGeomCopy* cmd,
		Utils::Math::Plane* plane) :
		CommandEditGeom(c, "Symétrie")
{
	m_impl = new GeomMirrorImplementation(c, cmd, plane);
}
/*----------------------------------------------------------------------------*/
CommandMirroring::CommandMirroring(Internal::Context& c,
		Utils::Math::Plane* plane) :
		CommandEditGeom(c, "Symétrie plane de tout") {
	m_impl = new GeomMirrorImplementation(c, plane);
}
/*----------------------------------------------------------------------------*/
CommandMirroring::~CommandMirroring() {
	if (m_impl)
		delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandMirroring::internalSpecificExecute() {
	m_impl->perform(m_createdEntities);
}
/*----------------------------------------------------------------------------*/
void CommandMirroring::internalSpecificPreExecute() {
	m_impl->prePerform();
}
/*----------------------------------------------------------------------------*/
void CommandMirroring::internalSpecificUndo()
{
    m_impl->performUndo();
}
/*----------------------------------------------------------------------------*/
void CommandMirroring::internalSpecificRedo()
{
    m_impl->performRedo();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
