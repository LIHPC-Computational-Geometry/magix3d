/*----------------------------------------------------------------------------*/
/** \file		CommandResult.cpp
 *  \author		Team Magix3D
 *  \date		02/04/2013
 */
/*----------------------------------------------------------------------------*/

#include "Utils/CommandResult.h"
#include "Utils/Common.h"

#include <TkUtil/Exception.h>

using namespace std;
using namespace Mgx3D::Utils;
using namespace TkUtil;


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Utils
{

/*----------------------------------------------------------------------------*/

CommandResult::CommandResult (Command& cmd)
	: CommandResultIfc ( ), ObjectBase ( ), _command (0)
{
	// La commande informera cette instance de CommandResult à chacune de ses
	// modifications (invocation de observableModified) et lors de sa
	// destruction (invocation de observableDeleted).
	// 2ème argument = false (blockDestruction) : on n'interdit pas à la
	// commande de s'autodétruire. Cette commande s'autodétruira dès lors
	// qu'elle n'aura pour observateur que des instances qui auront invoqué
	// registerObservable (&cmd, false);
	registerObservable (&cmd, false);
	_command	= &cmd;
}	// CommandResult::CommandResult


CommandResult::CommandResult ()
	: CommandResultIfc ( ), ObjectBase ( ),
	  _command (0)
{
}   // CommandResult::CommandResult


CommandResult::CommandResult (const CommandResult&)
	: CommandResultIfc ( ), ObjectBase ( ),
	  _command (0)
{
	MGX_FORBIDDEN ("CommandResult copy constructor is not allowed.")
}	// CommandResult::CommandResult


CommandResult& CommandResult::operator = (const CommandResult&)
{
	MGX_FORBIDDEN ("CommandResult assignment operator is not allowed.")
	return *this;
}	// CommandResult::operator =


CommandResult::~CommandResult ( )
{
	AutoMutex	autoMutex ((Mutex*)getMutex ( ));
	_command	= 0;
	unregisterReferences ( );
}	// CommandResult::~CommandResult


string CommandResult::getName ( ) const
{
	return getCommand ( ).getName ( );
}	// CommandResult::getName


string CommandResult::getUniqueName ( ) const
{
	return getCommand ( ).getUniqueName ( );
}	// CommandResult::getUniqueName


CommandIfc::status CommandResult::getStatus ( ) const
{
	return getCommand ( ).getStatus ( );
}	// CommandResult::getStatus


UTF8String CommandResult::getStrStatus ( ) const
{
	return getCommand ( ).getStrStatus ( );
}	// CommandResult::getStrStatus


UTF8String CommandResult::getErrorMessage ( ) const
{
	return getCommand ( ).getErrorMessage ( ).iso ( );
}	// CommandResult::getErrorMessage


bool CommandResult::isUserNotified ( ) const
{
	return getCommand ( ).isUserNotified ( );
}	// CommandResult::isUserNotified


void CommandResult::setUserNotified (bool notified)
{
	getCommand ( ).setUserNotified (notified);
}	// CommandResult::setUserNotified


void CommandResult::observableModified (ReferencedObject* object, unsigned long event)
{
	ObjectBase::observableModified (object, event);
}	// CommandResult::observableModified


void CommandResult::observableDeleted (ReferencedObject* object)
{
	AutoMutex	autoMutex ((Mutex*)getMutex ( ));

	if (dynamic_cast<Command*>(object) == _command)
	{
		_command	= 0;
	}	// if (dynamic_cast<Command*>(object) == _command)

	ObjectBase::observableDeleted (object);
}	// CommandResult::observableDeleted


const Command& CommandResult::getCommand ( ) const
{
	if (0 == _command)
		throw Exception ("CommandResult::getCommand : absence de commande associée (commande probablement détruite).");

	return *_command;
}	// CommandResult::getCommand


Command& CommandResult::getCommand ( )
{
	if (0 == _command)
		throw Exception ("CommandResult::getCommand : absence de commande associée (commande probablement détruite).");

	return *_command;
}	// CommandResult::getCommand

/*----------------------------------------------------------------------------*/

}	// namespace Utils
/*----------------------------------------------------------------------------*/

}	// namespace Mgx3D
/*----------------------------------------------------------------------------*/

