/*----------------------------------------------------------------------------*/
/** \file		CommandException.cpp
 *  \author		Charles PIGNEROL
 *  \date		15/10/19
 */
/*----------------------------------------------------------------------------*/

#include "Utils/CommandException.h"

using namespace TkUtil;


namespace Mgx3D 
{

namespace Utils 
{

CommandException::CommandException (const UTF8String& msg, CommandIfc* cmd)
	: TkUtil::Exception (msg), _command (cmd)
{
}	// CommandException::CommandException


CommandException::CommandException (const CommandException& ce)
	: Exception (ce.getFullMessage ( )), _command (ce._command)
{
}	// CommandException::CommandException


CommandException::CommandException (const Exception& e, CommandIfc* cmd)
	: TkUtil::Exception (e.getFullMessage ( )), _command (cmd)
{
}	// CommandException::CommandException


CommandException& CommandException::operator = (const CommandException& ce)
{
	if (&ce != this)
	{
		Exception::operator = (ce);
		_command	= ce._command;
	}

	return *this;
}	// CommandException::operator =


CommandException::~CommandException ( )
{
}	// CommandException::~CommandException
	
	
const CommandIfc* CommandException::getCommand ( ) const
{
	return _command;
}	// CommandException::getCommand


CommandIfc* CommandException::getCommand ( )
{
	return _command;
}	 // CommandException::getCommand
	

}	// namespace Utils

}	// namespace Mgx3D

