#include "Utils/Common.h"
#include "Utils/CommandManagerIfc.h"
#include "Utils/Magix3DEvents.h"
#include "Utils/UndoRedoManagerIfc.h"

#include <TkUtil/ErrorLog.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/ThreadManager.h>

#include <errno.h>		// errno
#include <algorithm>
#include <string.h>
using namespace TkUtil;
using namespace std;


namespace Mgx3D {

namespace Utils {


CommandManagerIfc::CommandManagerIfc ( )
{
}	// CommandManagerIfc::CommandManagerIfc


CommandManagerIfc::CommandManagerIfc (const CommandManagerIfc&)
{
	MGX_FORBIDDEN ("CommandManagerIfc copy constructor is not allowed.");
}	// CommandManagerIfc::CommandManagerIfc


CommandManagerIfc& CommandManagerIfc::operator = (const CommandManagerIfc&)
{
	MGX_FORBIDDEN ("CommandManagerIfc assignment operator = is not allowed.");
	return *this;
}	// CommandManagerIfc::operator =


CommandManagerIfc::~CommandManagerIfc ( )
{
}	// CommandManagerIfc::~CommandManagerIfc


void CommandManagerIfc::clear ( )
{
	throw Exception ("CommandManagerIfc::clear should be overloaded.");
}	// CommandManagerIfc::clear


const string& CommandManagerIfc::getName ( ) const
{
	throw Exception ("CommandManagerIfc::getName should be overloaded.");
}	// CommandManagerIfc::getName


CommandManagerIfc::POLICY CommandManagerIfc::getPolicy ( ) const
{
	throw Exception ("CommandManagerIfc::getPolicy should be overloaded.");
}	// CommandManagerIfc::getPolicy


CommandManagerIfc::POLICY CommandManagerIfc::setPolicy (POLICY)
{
	throw Exception ("CommandManagerIfc::setPolicy should be overloaded.");
}	// CommandManagerIfc::setPolicy


void CommandManagerIfc::addCommand (
							CommandIfc* command, CommandIfc::PLAY_TYPE pt)
{
	throw Exception ("CommandManagerIfc::addCommand should be overloaded.");
}	// CommandManagerIfc::addCommand


void CommandManagerIfc::processQueuedCommands ( )
{
	throw Exception ("CommandManagerIfc::processQueuedCommands should be overloaded.");
}	// CommandManagerIfc::processQueuedCommands


std::vector<CommandIfc*> CommandManagerIfc::getCommandIfcs ( ) const
{
	throw Exception ("CommandManagerIfc::getCommandIfcs should be overloaded.");
}	// CommandManagerIfc::getCommands


bool CommandManagerIfc::hasRunningCommands ( ) const
{
	throw Exception ("CommandManagerIfc::hasRunningCommands should be overloaded.");
}	// CommandManagerIfc::hasRunningCommands


bool CommandManagerIfc::hasQueuedCommands ( ) const
{
	throw Exception ("CommandManagerIfc::hasQueuedCommands should be overloaded.");
}	// CommandManagerIfc::hasQueuedCommands


void CommandManagerIfc::wait ( )
{
	throw Exception ("CommandManagerIfc::wait should be overloaded.");
}	// CommandManagerIfc::wait


CommandIfc::status CommandManagerIfc::getStatus ( ) const
{
	throw Exception ("CommandManagerIfc::getStatus should be overloaded.");
}	// CommandManagerIfc::getStatus

bool CommandManagerIfc::hasCommandInError ( ) const
{
	throw Exception ("CommandManagerIfc::hasCommandInError should be overloaded.");
}	// CommandManagerIfc::hasCommandInError


string CommandManagerIfc::getCommandName ( ) const
{
	throw Exception ("CommandManagerIfc::getCommandName should be overloaded.");
}	// CommandManagerIfc::getCommandName


void CommandManagerIfc::undo ( )
{
	throw Exception ("CommandManagerIfc::undo should be overloaded.");
}	// CommandManagerIfc::undo


void CommandManagerIfc::redo ( )
{
	throw Exception ("CommandManagerIfc::redo should be overloaded.");
}	// CommandManagerIfc::redo


const UndoRedoManagerIfc& CommandManagerIfc::getUndoManager ( ) const
{
	throw Exception ("CommandManagerIfc::getUndoManager should be overloaded.");
}	// CommandManagerIfc::getUndoManager


UndoRedoManagerIfc& CommandManagerIfc::getUndoManager ( )
{
	throw Exception ("CommandManagerIfc::getUndoManager should be overloaded.");
}	// CommandManagerIfc::getUndoManager


void CommandManagerIfc::setUndoManager (UndoRedoManagerIfc*)
{
	throw Exception ("CommandManagerIfc::setUndoManager should be overloaded.");
}	// CommandManagerIfc::setUndoManager


void CommandManagerIfc::addCommandObserver (TkUtil::ObjectBase* observer)
{
	throw Exception ("CommandManagerIfc::addCommandObserver should be overloaded.");
}	// CommandManagerIfc::addCommandObserver

}	// namespace Utils

}	// namespace Mgx3D
