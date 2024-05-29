/*----------------------------------------------------------------------------*/
/*
 * \file UndoRedoManagerIfc.cpp
 *
 *  \author Franck Ledoux, Charles Pignerol
 *
 *  \date 09/02/2012
 */
/*----------------------------------------------------------------------------*/
#include "Utils/UndoRedoManagerIfc.h"
#include "Utils/CommandIfc.h"
#include <TkUtil/Exception.h>

using namespace TkUtil;

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
UndoRedoManagerIfc::UndoRedoManagerIfc()
{
}
/*----------------------------------------------------------------------------*/
UndoRedoManagerIfc::~UndoRedoManagerIfc()
{
}
/*----------------------------------------------------------------------------*/
void UndoRedoManagerIfc::clear()
{
	throw Exception ("UndoRedoManagerIfc::clear should be overloaded.");
}
/*----------------------------------------------------------------------------*/
const std::string& UndoRedoManagerIfc::getName ( ) const
{
	throw Exception ("UndoRedoManagerIfc::getName should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void UndoRedoManagerIfc::store(CommandIfc* command)
{
	throw Exception ("UndoRedoManagerIfc::store should be overloaded.");
}
/*----------------------------------------------------------------------------*/
const std::vector<CommandIfc*>& UndoRedoManagerIfc::getDoneCommands ( ) const
{
	throw Exception ("UndoRedoManagerIfc::getDoneCommands should be overloaded.");
}	// UndoRedoManagerIfc::getDoneCommands
/*----------------------------------------------------------------------------*/
const std::vector<CommandIfc*>& UndoRedoManagerIfc::getUndoneCommands ( ) const
{
	throw Exception ("UndoRedoManagerIfc::getUndoneCommands should be overloaded.");
}	// UndoRedoManagerIfc::getUndoneCommands
/*----------------------------------------------------------------------------*/
void UndoRedoManagerIfc::clearUndone()
{
    throw Exception ("UndoRedoManagerIfc::clearUndone should be overloaded.");
}
/*----------------------------------------------------------------------------*/
CommandIfc* UndoRedoManagerIfc::undoableCommand ( )
{
	throw Exception ("UndoRedoManagerIfc::undoableCommand should be overloaded.");
}	// UndoRedoManagerIfc::undoableCommand
/*----------------------------------------------------------------------------*/
std::string UndoRedoManagerIfc::undoableCommandName ( ) const
{
	throw Exception ("UndoRedoManagerIfc::undoableCommandName should be overloaded.");
}	// UndoRedoManagerIfc::undoableCommand
/*----------------------------------------------------------------------------*/
CommandIfc* UndoRedoManagerIfc::undo()
{
	throw Exception ("UndoRedoManagerIfc::undo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
CommandIfc* UndoRedoManagerIfc::redoableCommand ( )
{
	throw Exception ("UndoRedoManagerIfc::redoableCommand should be overloaded.");
}	// UndoRedoManagerIfc::redoableCommand
/*----------------------------------------------------------------------------*/
std::string UndoRedoManagerIfc::redoableCommandName ( ) const
{
	throw Exception ("UndoRedoManagerIfc::redoableCommandName should be overloaded.");
}	// UndoRedoManagerIfc::redoableCommand
/*----------------------------------------------------------------------------*/
CommandIfc* UndoRedoManagerIfc::redo()
{
	throw Exception ("UndoRedoManagerIfc::redo should be overloaded.");
}
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

