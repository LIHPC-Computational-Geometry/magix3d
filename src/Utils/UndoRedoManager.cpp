/*----------------------------------------------------------------------------*/
/*
 * \file UndoRedoManager.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Utils/UndoRedoManager.h"
#include "Utils/Magix3DEvents.h"
#include "Utils/Command.h"
#include <TkUtil/MemoryError.h>


using namespace TkUtil;

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
UndoRedoManager::UndoRedoManager(const std::string& name)
	: UndoRedoManagerIfc ( ), ReferencedObject ( ),
	  m_name (name), m_done ( ), m_undone ( ), m_mutex (0), m_logStream (0)
{
	m_mutex	= new Mutex ( );
}
/*----------------------------------------------------------------------------*/
UndoRedoManager::~UndoRedoManager()
{
	AutoMutex	autoMutex (mutex ( ));

	m_done.clear ( );
	m_undone.clear ( );
	notifyObserversForDestruction ( );
	unregisterReferences ( );	// Provoque l'éventuel suicide des commandes.
}
/*----------------------------------------------------------------------------*/
void UndoRedoManager::clear()
{
	m_done.clear ( );
	m_undone.clear ( );
	unregisterReferences ( );	// Provoque l'éventuel suicide des commandes.
}
/*----------------------------------------------------------------------------*/
const std::string& UndoRedoManager::getName ( ) const
{
	return m_name;
}	// UndoRedoManager::getName
/*----------------------------------------------------------------------------*/
void UndoRedoManager::store(CommandIfc* command)
{
	CHECK_NULL_PTR_ERROR (command)
	AutoMutex	autoMutex (mutex ( ));

	if (true == hasCommand (command))
		return;	// Pas d'exception de levée, arrive lors de redo.

	Command*	cmd	= dynamic_cast<Command*>(command);
	if (0 != cmd)
		registerObservable (cmd, true);
    m_done.push_back(command);

	notifyObserversForModification (COMMAND_STACK);
}
/*----------------------------------------------------------------------------*/
void UndoRedoManager::clearUndone()
{
    // suppression de la liste des commandes undone
    std::vector<CommandIfc*>    oldCommands = m_undone;
    m_undone.clear ( );
    for (std::vector<CommandIfc*>::iterator iter = oldCommands.begin();
         iter != oldCommands.end(); ++iter)
    {   // Les commandes se suicideront si elles n'ont pas un observateur
        // qui n'est pas d'accord :
        Command* cmd = dynamic_cast<Command*>(*iter);
        if (0 != cmd)
            unregisterObservable (cmd, true);
//        delete *iter;
    }   // for (std::vector<Command*>::iterator iter = ...
//    m_undone.clear();
}
/*----------------------------------------------------------------------------*/
const std::vector<CommandIfc*>& UndoRedoManager::getDoneCommands ( ) const
{
	return m_done;
}	// UndoRedoManager::getDoneCommands
/*----------------------------------------------------------------------------*/
const std::vector<CommandIfc*>& UndoRedoManager::getUndoneCommands ( ) const
{
	return m_undone;
}	// UndoRedoManager::getUndoneCommands
/*----------------------------------------------------------------------------*/
CommandIfc* UndoRedoManager::undoableCommand ( )
{
	AutoMutex	autoMutex (mutex ( ));

	CommandIfc*	cmd	= 0 == m_done.size( ) ? 0 : m_done [m_done.size( ) - 1];

	//std::cout <<" UndoRedoManager::undoableCommand retourne "<<(cmd?cmd->getName():"(pas de commande)")<<std::endl;

	return cmd;
}	// UndoRedoManager::undoableCommand
/*----------------------------------------------------------------------------*/
std::string UndoRedoManager::undoableCommandName ( ) const
{
	AutoMutex	autoMutex (mutex ( ));

	const CommandIfc*	cmd	= 0 == m_done.size( ) ? 0 : m_done [m_done.size( ) - 1];

	return 0 == cmd ? "" : cmd->getName ( );
}	// UndoRedoManager::undoableCommandName
/*----------------------------------------------------------------------------*/
CommandIfc* UndoRedoManager::undo()
{
	AutoMutex	autoMutex (mutex ( ));

    CommandIfc* c =  m_done.back();
    m_done.pop_back();
    m_undone.push_back(c);

	notifyObserversForModification (COMMAND_STACK);

    return c;
}
/*----------------------------------------------------------------------------*/
CommandIfc* UndoRedoManager::redoableCommand ( )
{
	AutoMutex	autoMutex (mutex ( ));

	CommandIfc*	cmd	= 0 == m_undone.size ( ) ?
						  0 : m_undone [m_undone.size ( ) - 1];

	return cmd;
}	// UndoRedoManager::redoableCommand
/*----------------------------------------------------------------------------*/
std::string UndoRedoManager::redoableCommandName ( ) const
{
	AutoMutex	autoMutex (mutex ( ));

	const CommandIfc*	cmd	= 0 == m_undone.size ( ) ?
						  0 : m_undone [m_undone.size ( ) - 1];

	return 0 == cmd ? "" : cmd->getName ( );
}	// UndoRedoManager::redoableCommand
/*----------------------------------------------------------------------------*/
CommandIfc* UndoRedoManager::redo()
{
	AutoMutex	autoMutex (mutex ( ));

    CommandIfc* c =  m_undone.back();
    m_undone.pop_back();
    m_done.push_back(c);

	notifyObserversForModification (COMMAND_STACK);

    return c;
}
/*----------------------------------------------------------------------------*/
bool UndoRedoManager::hasCommand (CommandIfc* command)
{
	AutoMutex	autoMutex (mutex ( ));

    for (std::vector<CommandIfc*>::iterator itd = m_done.begin ( );
	     itd != m_done.end ( ); itd++)
		if (*itd == command)
			return true;
    for (std::vector<CommandIfc*>::iterator itud = m_undone.begin ( );
	     itud != m_undone.end ( ); itud++)
		if (*itud == command)
			return true;

	return false;
}	// UndoRedoManager::hasCommand
/*----------------------------------------------------------------------------*/
void UndoRedoManager::log (const Log& l)
{
	AutoMutex	autoMutex (mutex ( ));

	if (0 != getLogStream ( ))
		getLogStream ( )->log (l);
}	// UndoRedoManager::log
/*----------------------------------------------------------------------------*/
void UndoRedoManager::setLogStream (TkUtil::LogOutputStream* stream)
{
	AutoMutex	autoMutex (mutex ( ));

	m_logStream	= stream;

//    for (std::vector<Command*>::iterator itd = m_done.begin ( );
//	     itd != m_done.end ( ); itd++)
//		(*itd)->setLogStream (stream);
//    for (std::vector<Command*>::iterator itud = m_undone.begin ( );
//	     itud != m_undone.end ( ); itud++)
//		(*itud)->setLogStream (stream);
}	// UndoRedoManager::setLogStream
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

