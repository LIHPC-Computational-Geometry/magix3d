/*----------------------------------------------------------------------------*/
/*
 * \file M3DCommandManager.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 20 déc. 2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/M3DCommandManager.h"
#include "Internal/M3DCommandResult.h"
#include "Internal/CommandNewLongCommand.h"
#include "Internal/OpenCascadeLongCommand.h"

#include "Utils/CommandManager.h"

#include <TkUtil/Exception.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
M3DCommandManager::M3DCommandManager(const std::string& name, Internal::Context* c)
:Internal::CommandCreator(name, c)
{
}
/*----------------------------------------------------------------------------*/
M3DCommandResultIfc* M3DCommandManager::newLongCommand()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "M3DCommandManager::newLongCommand ()";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

    CommandNewLongCommand *command = new CommandNewLongCommand(getLocalContext(), "Création d'une longue commande");

    // trace dans le script
    TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
    cmd << getContextAlias() << "." << "getM3DCommandManager(). newLongCommand()";
    command->setScriptCommand(cmd);

    getCommandManager().addCommand(command, Utils::Command::DO);

	M3DCommandResultIfc*	cmdResult	= new M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
M3DCommandResultIfc* M3DCommandManager::newOpenCascadeLongCommand()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "M3DCommandManager::openCascadeLongCommand ()";
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

    OpenCascadeLongCommand *command = new OpenCascadeLongCommand(getLocalContext(), "Création d'une longue commande Open Cascade");

    getCommandManager ( ).addCommand(command, Utils::Command::DO);

	M3DCommandResultIfc*	cmdResult	= new M3DCommandResult (*command);
	return cmdResult;
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
