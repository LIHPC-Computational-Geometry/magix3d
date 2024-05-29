/*----------------------------------------------------------------------------*/
/*
 * \file CommandDuplicateSysCoord.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6 Juin 2018
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Internal/InfoCommand.h"
#include "Internal/Context.h"
#include "Group/Group3D.h"
#include <SysCoord/CommandDuplicateSysCoord.h>
#include <SysCoord/SysCoordManager.h>

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
CommandDuplicateSysCoord::CommandDuplicateSysCoord(Internal::Context& c,
		SysCoord* syscoord,
		const std::string& groupName)
: Internal::CommandInternal (c, "Copie d'un repère")
, m_init_syscoord(syscoord)
, m_new_syscoord(0)
, m_group_name(groupName)
{

}
/*----------------------------------------------------------------------------*/
CommandDuplicateSysCoord::~CommandDuplicateSysCoord()
{
	if (m_new_syscoord)
		delete m_new_syscoord;
}
/*----------------------------------------------------------------------------*/
void CommandDuplicateSysCoord::internalExecute()
{
	m_new_syscoord = new SysCoord(getContext());

	SysCoordProperty* new_ppty = m_init_syscoord->getProperty()->clone();
	SysCoordProperty* old_ppty = m_new_syscoord->setProperty(new_ppty);
	m_new_syscoord->update();
	delete old_ppty;

	addToGroup (m_new_syscoord, false);

	getContext().getLocalSysCoordManager().add(m_new_syscoord);

	getInfoCommand().addSysCoordInfoEntity(m_new_syscoord, Internal::InfoCommand::CREATED);
}
/*----------------------------------------------------------------------------*/
void CommandDuplicateSysCoord::internalUndo()
{
    TkUtil::AutoReferencedMutex	autoMutex (getMutex ( ));

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();
	m_new_syscoord->update();
}
/*----------------------------------------------------------------------------*/
void CommandDuplicateSysCoord::internalRedo()
{
    TkUtil::AutoReferencedMutex	autoMutex (getMutex ( ));

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();
	m_new_syscoord->update();
}
/*----------------------------------------------------------------------------*/
void CommandDuplicateSysCoord::addToGroup(SysCoord* rep, bool use_default_name)
{
    Group::Group3D* group = getContext().getLocalGroupManager().getNewGroup3D(use_default_name?"":m_group_name, &getInfoCommand());
    //rep->add(group);
    group->add(rep);
    getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

