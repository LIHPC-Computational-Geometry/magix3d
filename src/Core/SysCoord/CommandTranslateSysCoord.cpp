/*----------------------------------------------------------------------------*/
/*
 * \file CommandTranslateSysCoord.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6 Juin 2018
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Utils/Vector.h"
#include "Internal/InfoCommand.h"
#include "Internal/Context.h"
#include <SysCoord/CommandTranslateSysCoord.h>
#include <SysCoord/CommandDuplicateSysCoord.h>
#include <SysCoord/SysCoordManager.h>

/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
CommandTranslateSysCoord::CommandTranslateSysCoord(Internal::Context& c,
		SysCoord* syscoord, const Vector& dp)
: CommandEditSysCoord (c, "Translation d'un repère")
, m_syscoord(syscoord)
, m_command(0)
, m_translation(dp)
{
}
/*----------------------------------------------------------------------------*/
CommandTranslateSysCoord::CommandTranslateSysCoord(Internal::Context& c,
		CommandDuplicateSysCoord* cmd, const Vector& dp)
: CommandEditSysCoord (c, "Translation d'un repère")
, m_syscoord(0)
, m_command(cmd)
, m_translation(dp)
{
}
/*----------------------------------------------------------------------------*/
CommandTranslateSysCoord::~CommandTranslateSysCoord()
{
}
/*----------------------------------------------------------------------------*/
void CommandTranslateSysCoord::internalExecute()
{
	if (m_syscoord == 0 && m_command)
		m_syscoord = m_command->getNewSysCoord();

	CHECK_NULL_PTR_ERROR(m_syscoord)

	m_syscoord->saveProperty(&getInfoCommand());

	m_syscoord->getProperty()->setCenter(m_syscoord->getProperty()->getCenter()+m_translation);

	m_syscoord->saveInternals(this);
}
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

