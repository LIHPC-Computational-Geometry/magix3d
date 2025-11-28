/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
#include "Group/GroupManager.h"
#include "Group/GroupEntity.h"
#include "SysCoord/SysCoord.h"
#include "SysCoord/CommandDuplicateSysCoord.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
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

	getContext().getSysCoordManager().add(m_new_syscoord);

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
    Group::Group3D* group = getContext().getGroupManager().getNewGroup<Group::Group3D>(use_default_name?"":m_group_name, &getInfoCommand());
    //rep->add(group);
    group->add(rep);
    getInfoCommand().addGroupInfoEntity(group,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

