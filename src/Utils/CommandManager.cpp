#include "Utils/Common.h"
#include "Utils/CommandManager.h"
#include "Utils/CommandException.h"
#include "Utils/CommandRunner.h"
#include "Utils/Magix3DEvents.h"
#include "Utils/MgxNumeric.h"
#include "Utils/MutexUnlocker.h"
#include "Utils/UndoRedoManager.h"

#include <TkUtil/ErrorLog.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InformationLog.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/InformationLog.h>
#include <TkUtil/LogOutputStream.h>
#include <TkUtil/ThreadManager.h>

#include <sched.h>		// sched_yield
#include <errno.h>		// errno
#include <algorithm>
#include <string.h>
using namespace Mgx3D::Utils::Math;
using namespace TkUtil;
using namespace std;


namespace Mgx3D {

namespace Utils {

// on force le séquentiel comme défaut
unsigned long   			CommandManager::sequentialDuration  = (unsigned long)-1;

CommandManager::POLICY	CommandManager::runningPolicy		= CommandManager::THREADED;


CommandManager::CommandManager (const string& name)
	: ReferencedNamedObject (name),
	  _runners ( ), _runnersMutex (false),
	  _commandObservers ( ), _logStream (0),
	  _undoManager (new UndoRedoManager ("UndoRedoManager")),
	  _commands ( ), _queuedCommands ( ),
	  _policy (runningPolicy),
	  _queuingMutex (false), _sequentialMutex (false), _sequentialCommand (0),
	  _currentStatus (Command::DONE), _currentCommandName ( )
{
}	// CommandManager::CommandManager


CommandManager::CommandManager (const CommandManager&)
	: ReferencedNamedObject ("unamed"),
	  _runners ( ), _runnersMutex (false),
	  _commandObservers ( ), _logStream (0), _undoManager(0),
	  _commands ( ), _queuedCommands ( ),
	  _policy (runningPolicy),
	  _queuingMutex (false), _sequentialMutex (false), _sequentialCommand (0),
	  _currentStatus (Command::FAIL), _currentCommandName ( )
{
	MGX_FORBIDDEN ("CommandManager copy constructor is not allowed.");
}	// CommandManager::CommandManager


CommandManager& CommandManager::operator = (const CommandManager&)
{
	MGX_FORBIDDEN ("CommandManager assignment operator = is not allowed.");
	return *this;
}	// CommandManager::operator =


CommandManager::~CommandManager ( )
{
	AutoMutex	autoMutex (getMutex ( ));

	_commandObservers.clear ( );

	// Pas clean : runners en cours non détruits pour l'instant. CP
	// TODO [CP] : destruction des runners.
	UTF8String	message (Charset::UTF_8);
	message<< "Destruction du gestionnaire de commandes " << getName ( ) << ".";
	MGX_TRACE_LOG_1 (trace1, message)
	log (trace1);

	notifyObserversForDestruction ( );
	unregisterReferences ( );

	delete _undoManager;	_undoManager	= 0;

	MGX_TRACE_LOG_1 (trace3, message)
	log (trace3);
}	// CommandManager::~CommandManager


void CommandManager::clear()
{
	_undoManager->clear();
	_commands.clear(); // est-ce que cette réinitialisation est judicieuse ? / observateurs
}

const string& CommandManager::getName ( ) const
{
	return TkUtil::ReferencedNamedObject::getName ( );
}	// CommandManager::getName


CommandManager::POLICY CommandManager::getPolicy ( ) const
{
	return _policy;
}   // CommandManager::getPolicy


CommandManager::POLICY CommandManager::setPolicy (POLICY policy)
{
	AutoMutex	queuingAutoMutex (&_queuingMutex);

	POLICY	oldPolicy	= _policy;
	_policy	= policy;

	return oldPolicy;
}   // CommandManager::setPolicy


void CommandManager::addCommand (Command* cmd, Command::PLAY_TYPE pt)
{
	// ========================================================================
	// Note CP : attention, pb rencontré lorsque l'on fait des successions de
	// undo/redo très vite à la souris, une l'exécution d'une commande (ou tout
	// au moins de son do/undo) est interrompue par le slot de la commande
	// suivante => on est ici dans CommandManager::addCommand du même thread
	// => pb avec les mutex, voire éventuellement le séquencement des
	// opérations.
	// Ce pb semble remédié par l'inactivation automatique des actions Qt
	// undo/redo avec réactivation selon contexte provoquée par un évènement
	// type Timer (QtMgx3DMainWindow::timerEvent).
	// ========================================================================
	if (0 == cmd)
		throw Exception ("CommandManager::addCommand : NULL command.");
	Command*	command	= dynamic_cast<Command*>(cmd);
	if (0 == command)
		throw Exception ("CommandManager::addCommand : cmd is not an instance of Command.");

	AutoMutex	autoMutex (getMutex ( ));

	// Force-t-on la mise en file d'attente ? Oui si :
	// - pt	= QUEUED
	// - (SEQUENTIAL == policy) && (true == hasRunningCommands ( ))
	// - (SEQUENTIAL == policy) && (true == hasQueuedCommands ( ))
	// _sequentialMutex déjà verrouillé.
	// L'objectif en undo/redo est bien sûr de s'affranchir de ce type de
	// problème, mais également de (re)jouer/défaire les opérations toujours
	// dans le même ordre.
	bool	hasRunning		= hasRunningCommands ( );
	bool	hasQueued		= false;
	try
	{
		_sequentialMutex.lock ( );
	}
	catch (...)
	{
		addToQueue (cmd, pt);
		return;
	}

	try
	{

		if ((Command::QUEUED == pt) ||
				((SEQUENTIAL == getPolicy ( )) &&
						((true == hasRunning) || (true == hasQueuedCommands( ))) ||
						((true == hasRunning) &&
								((Command::UNDO == pt) || Command::REDO == pt)))
		)
		{
			addToQueue (cmd, pt);
			return;
		}

		if (Command::DO == pt)
		{
			_commands.push_back (command);
			//std::cout<<"_commands.size() = "<<_commands.size()<<std::endl;
			registerObservable (command, false);
		}	// if (_commands.end ( ) == find ( ...

		// Quoi qu'il arrive les observateurs de commandes doivent être informés du
		// devenir de la commande confiée :
		for (vector<ObjectBase*>::iterator ito = _commandObservers.begin ( );
				_commandObservers.end ( ) != ito; ito++)
			if (false == (*ito)->isObservableRegistered (command, false))
				(*ito)->registerObservable (command, false);

		UTF8String	message (Charset::UTF_8);
		message << "Ajout de la commande " << command->getName ( )
	        		<< " de nom unique " << command->getUniqueName ( )
	        		<< " au gestionnaire de commandes " << getName ( ) << "."
	        		<< "Type d'exécution à effectuer : "
	        		<< Command::playTypeToString (pt) << ".";
		MGX_TRACE_LOG_1 (trace1, message)
		log (trace1);

		bool	allowed	= false;
		switch (pt)
		{
		case Command::QUEUED	: allowed	= true;	break;
		case Command::DO		:
		case Command::REDO		:
			allowed	= Command::INITED == command->getStatus ( ) ? true : false;
			break;
		case Command::UNDO		:
			allowed	= Command::DONE == command->getStatus ( ) ? true : false;
			break;
		}	// switch (pt)
		if (false == allowed)
		{
			_sequentialMutex.unlock ( );
			UTF8String	message (Charset::UTF_8);
			message << "CommandManager::addCommand : impossibilité d'ajouter et "
					<< "jouer/défaire/rejouer la commande " << command->getName ( )
					<< " de nom unique " << command->getUniqueName ( )
					<< ". Le status actuel de la commande est "
					<< command->getStrStatus ( )
					<< ". Type d'exécution à effectuer : "
					<< Command::playTypeToString (pt)
			<< ".";
			try
			{
				unregisterObservable (command, false);
			}
			catch (...)
			{ }
			throw Exception (message);
		}	// if (false == allowed)

		UTF8String	message2 (Charset::UTF_8);
		message2 << "Enregistrement de la commande " << getName ( )
	        		 << " auprès du gestionnaire " << getName ( ) << " effectué.";
		MGX_TRACE_LOG_1 (trace2, message2)
		log (trace2);

		if ((Command::QUEUED == pt) || (Command::DO == pt))
			notifyObserversForModification (NEW_COMMAND);

		unsigned long	estimatedTime = 0;
		// pas la peine de faire une estimation pour undo (surtout pour éviter d'être bloqué avec commande en erreur)
		if (Command::UNDO != pt)
			estimatedTime = command->getEstimatedDuration (pt);
//unsigned long	estimatedTime	= 999999999999;
		if ((SEQUENTIAL == getPolicy ( )) ||
				(false == command->threadable ( ))   ||
				(estimatedTime < sequentialDuration) ||
				(Command::UNDO == pt)             ||
				(Command::REDO == pt)
		)
			executeSequential (command, pt);
		else
			executeThreaded (command, pt);

		hasRunning	= hasRunningCommands ( );
		hasQueued	= hasQueuedCommands ( );
		_sequentialMutex.unlock ( );

	}
	catch (...)
	{
		_sequentialMutex.unlock ( );
		throw;
	}

	// S'il y a des commandes en file d'attente et aucune en cours de traitement
	// on en lance une :
	if ((false == hasRunning) && (true == hasQueued))
		runQueuedCommand ( );
}	// CommandManager::addCommand


void CommandManager::processQueuedCommands ( )
{
	while ((false == hasRunningCommands ( )) && (true == hasQueuedCommands ( )))
		runQueuedCommand ( );
}	// CommandManager::processQueuedCommands


std::vector<Command*> CommandManager::getCommands ( ) const
{
	AutoMutex	autoMutex (getMutex ( ));

	return _commands;
}	// CommandManager::getCommands


bool CommandManager::hasCommandInError ( ) const
{
	for (std::vector<Command*>::const_iterator it = _commands.begin ( );
		     _commands.end ( ) != it; it++)
		if (Command::FAIL == (*it)->getStatus())
			return true;

	return false;
}


bool CommandManager::hasRunningCommands ( ) const
{	// Attention, seul un seul mutex doit être verrouillé
	{
		AutoMutex	autoMutex (getMutex ( ));
		if ((0 != _sequentialCommand) &&
		    (false == _sequentialCommand->isCompleted ( )))
			return true;
	}

	AutoMutex	runnersAutoMutex (&_runnersMutex);
	if (0 == _runners.size ( ))
		return false;

	for (vector<CommandRunner*>::const_iterator itr = _runners.begin ( );
	     _runners.end ( ) != itr; itr++)
		if (false == (*itr)->getCommand ( )->isCompleted ( ))
			return true;

	return false;
}	// CommandManager::hasRunningCommands


bool CommandManager::hasQueuedCommands ( ) const
{
	AutoMutex	autoMutex (getMutex ( ));

	return 0 == _queuedCommands.size ( ) ? false : true;
}	// CommandManager::hasQueuedCommands


CommandRunner* CommandManager::getCommandRunner (Command* command)
{
	if (0 == command)
		return 0;

	AutoMutex	autoMutex (getMutex ( ));
	AutoMutex	runnersAutoMutex (&_runnersMutex);

	for (vector<CommandRunner*>::iterator itr = _runners.begin ( );
	     _runners.end ( ) != itr; itr++)
		if ((*itr)->getCommand ( ) == command)
			return *itr;

	return 0;
}	// CommandManager::getCommandRunner


void CommandManager::commandRunnerDeleted (CommandRunner* runner)
{
	if (0 == runner)
		return;

	AutoMutex	autoMutex (getMutex ( ));
	AutoMutex	runnersAutoMutex (&_runnersMutex);
	for (vector<CommandRunner*>::iterator itr = _runners.begin ( );
	     _runners.end ( ) != itr; itr++)
		if (*itr == runner)
		{
			_runners.erase (itr);
			break;
		}	// if (*itr == runner)
}	// CommandManager::commandRunnerDeleted


void CommandManager::wait ( )
{
	AutoMutex	autoMutex (getMutex ( ));
	UTF8String	message1 (Charset::UTF_8), message2 (Charset::UTF_8);
	message1 << "Attente des " << (unsigned long)_runners.size ( )
	         << " commandes en cours d'exécution par le gestionnaire "
	         << getName ( ) << " ...";
	MGX_TRACE_LOG_1 (trace1, message1)
	log (trace1);

	while (0 != _runners.size ( ))
	{
		errno	= 0;
		if (0 != sched_yield ( ))
			cout << "CommandManager::wait. Erreur lors de l'appel de "
			     << "sched_yield : " <<  strerror (errno) << endl;
	}	// while (0 != _runners.size ( ))
	message2 << "Toutes les commandes du gestionnaire " << getName ( )
	         << " sont achevées.";
	MGX_TRACE_LOG_1 (trace2, message2)
	log (trace2);
}	// CommandManager::wait


Command::status CommandManager::getStatus ( ) const
{
	return _currentStatus;
}	// CommandManager::getStatus


string CommandManager::getCommandName ( ) const
{
	return _currentCommandName;
}	// CommandManager::getCommandName


const UndoRedoManager& CommandManager::getUndoManager ( ) const
{
	CHECK_NULL_PTR_ERROR (_undoManager)
	return *_undoManager;
}	// CommandManager::getUndoManager


UndoRedoManager& CommandManager::getUndoManager ( )
{
	CHECK_NULL_PTR_ERROR (_undoManager)
	return *_undoManager;
}	// CommandManager::getUndoManager


void CommandManager::setUndoManager (UndoRedoManager* mgr)
{
	if (_undoManager == mgr)
		return;

	AutoMutex	autoMutex (getMutex ( ));

	TkUtil::LogOutputStream*	stream	= 0;
	UndoRedoManager*	urm	= dynamic_cast<UndoRedoManager*>(_undoManager);
	if (0 != urm)
		stream	= urm->getLogStream ( );
	delete _undoManager;
	_undoManager	= mgr;
	urm	= dynamic_cast<UndoRedoManager*>(_undoManager);
	if (0 != urm)
		urm->setLogStream (stream);
}


void CommandManager::setLogStream (TkUtil::LogOutputStream* stream)
{
	AutoMutex	autoMutex (getMutex ( ));
	AutoMutex	runnersAutoMutex (&_runnersMutex);

	_logStream	= stream;
	UndoRedoManager*	urm	= dynamic_cast<UndoRedoManager*>(&getUndoManager());
	if (0 != urm)
		urm->setLogStream (stream);
	for (vector<CommandRunner*>::iterator itr = _runners.begin ( );
	     _runners.end ( ) != itr; itr++)
		(*itr)->setLogStream (stream);
}	// CommandManager::setLogStream


void CommandManager::observableModified (ReferencedObject* object, unsigned long event)
{
	if (UTIL_UNAVAILABLE_EVENT == event)
	{
		observableDeleted (object);
		return;
	}	// if (UTIL_UNAVAILABLE_EVENT == event)

    if ((0 == object) || (COMMAND_STATE != event))
        return;
    Command*	command	= dynamic_cast<Command*>(object);
    if (0 == command)
        return;

    AutoMutex		autoMutex (getMutex ( ));
	Command::status	status	= command->getStatus ( );
	_currentStatus		= status;
	_currentCommandName	= command->getName ( );
    UTF8String	message1 (Charset::UTF_8), message2 (Charset::UTF_8);
    message1 << "La commande " << command->getName ( ) << " de nom unique "
            << command->getUniqueName ( ) << " est modifiée.";
    MGX_TRACE_LOG_1 (trace1, message1)
    log (trace1);
    if (Command::PROCESSING == status)
        return;
	if ((Command::DONE == status) || (Command::CANCELED == status) ||
	    (Command::FAIL == status))
	{
		AutoMutex		runnersAutoMutex (&_runnersMutex);
	    for (vector<CommandRunner*>::iterator itr = _runners.begin ( );
	            _runners.end ( ) != itr; itr++)
		{
	        //		if ((*itr)->getCommand ( ) == command)
	        if (command->getCommandRunner ( ) == *itr)
			{
				AutoMutex	queuingAutoMutex (&_queuingMutex);
				_runners.erase (itr);
				message2 << "Lanceur de commande associé à la commande "
				         << command->getName ( )
				         << " déréférencé auprès du gestionnaire "
				         << getName ( ) << ".";
				MGX_TRACE_LOG_1 (trace2, message2)
				log (trace2);
				break;
			}	// if ((*itr)->getCommand ( ) == command)
		}	// for (vector<CommandRunner*>::iterator itr = ...
	}	// if ((Command::DONE == status) || (Command::CANCELED == status) || ...

    // L'ajouter au gestionnaire de undo/redo ?
    if (Command::DONE == status)
        getUndoManager ( ).store (command);

    // suppression des commandes déjouées
    if (Command::STARTING == status && command->getPlayType() == Command::DO)
        getUndoManager ( ).clearUndone ();

	switch (status)
	{
		case Command::INITED	:
		case Command::DONE		:
		case Command::CANCELED	:
			UTF8String   message_info (Charset::UTF_8);
			message_info << "Commande " << command->getName ( ) << " "
			             << command->getStrPlayType ( ) << " avec succès (en "
				         << MgxNumeric::userRepresentation (command->getTimer ( )) << ").";
			log (InformationLog (message_info));
			break;
	}	// switch (status)
}	// CommandManager::observableModified


void CommandManager::observableDeleted (ReferencedObject* object)
{
	Command*	command	= dynamic_cast<Command*>(object);
	if (0 != command)
	{
		AutoMutex	autoMutex (getMutex ( ));
		AutoMutex	runnersAutoMutex (&_runnersMutex);

		for (vector<CommandRunner*>::iterator itr = _runners.begin ( );
		     _runners.end ( ) != itr; itr++)
		{
			if ((*itr)->getCommand ( ) == command)
			{
				AutoMutex	queuingAutoMutex (&_queuingMutex);
				_runners.erase (itr);
				UTF8String	message (Charset::UTF_8);
				message << "Lanceur de commande associé à la commande "
				        << command->getName ( )
				        << " déréférencé auprès du gestionnaire "
				        << getName ( ) << ".";
				MGX_TRACE_LOG_1 (trace, message)
				log (trace);
				break;
			}	// if ((*itr)->getCommand ( ) == command)
		}	// for (vector<CommandRunner*>::iterator itr = ...

		// On enlève la commande de la liste des commandes prises en charge :
		for (vector<Command*>::iterator itc = _commands.begin ( );
		     _commands.end ( ) != itc; itc++)
			if (*itc == command)
			{
				_commands.erase (itc);
				break;
			}	// if (*itc == command)
	}	// if (0 != command)

	ReferencedNamedObject::observableDeleted (object);
}	// CommandManager::observableDeleted


void CommandManager::addCommandObserver (TkUtil::ObjectBase* observer)
{
	if (0 != observer)
		_commandObservers.push_back (observer);	
}	// CommandManager::addCommandObserver


void CommandManager::log (const Log& l)
{
	AutoMutex	autoMutex (getMutex ( ));

	if (0 != getLogStream ( ))
		getLogStream ( )->log (l);
}	// CommandManager::log


void CommandManager::undo ()
{
    AutoMutex   autoMutex (getMutex ( ));

    TkUtil::UTF8String   message (Charset::UTF_8);
    std::string             name;

    Command*    cmd = getUndoManager ( ).undoableCommand ( );
    if (0 == cmd)
    {
        throw Exception ("CommandManager::undo, Fonction annuler indisponible, absence de commande à annuler.");
    }   // if (0 == cmd)

	Command*	command	= dynamic_cast<Command*>(cmd);

	// sauvegarde des commentaires et de la ligne de commande avant modif
	if (0 != command)
		command->saveScriptCommandAndComments();

    // changement de la trace pour le script
    TkUtil::UTF8String sccmd (Charset::UTF_8);
    sccmd << getContextAlias() << "." << "undo()";
	if (0 != command)
    	command->setScriptCommand(sccmd);

    TkUtil::UTF8String sccmt (Charset::UTF_8);
    sccmt << "Annulation de : "<<cmd->getName();
	if (0 != command)
    	command->setScriptComments(sccmt);

    name    = cmd->getName ( );
    addCommand (cmd, Command::UNDO);
}	// CommandManager::undo


void CommandManager::redo ()
{
    AutoMutex   autoMutex (getMutex ( ));

    TkUtil::UTF8String   message (Charset::UTF_8);
    std::string             name;

    Command*    cmd = getUndoManager ( ).redoableCommand ( );
    if (0 == cmd)
    {
        throw Exception ("CommandManager::redo, Fonction rejouer indisponible, absence de commande à rejouer.");
    }   // if (0 == cmd)

	Command*	command	= dynamic_cast<Command*>(cmd);

    // changement de la trace pour le script
    TkUtil::UTF8String sccmd (Charset::UTF_8);
    sccmd << getContextAlias() << "." << "redo()";
	if (0 != command)
    	command->setScriptCommand(sccmd);

    TkUtil::UTF8String sccmt (Charset::UTF_8);
    sccmt << "Rejeu de : "<<cmd->getName();
	if (0 != command)
    	command->setScriptComments(sccmt);

    name    = cmd->getName ( );
    addCommand (cmd, Command::REDO);
}	// CommandManager::redo


void CommandManager::executeSequential (Command* cmd, Command::PLAY_TYPE pt)
{
	// Magix3D Issue #54 Un thread séquentiel (ex : écriture d'un maillage) ne doit pas s'exécuter en concurrence avec un autre thread (ex : génération d'un maillage) 
	// => on s'assure qu'il n'y a pas d'éventuels autre thread en cours.
	// On rappelle ici que la génération de maillage est threadable afin de ne pas bloquer l'IHM et par là m^eme inquiéter l'utilisateur durant une opération 
	// potentiellement longue.
	ThreadManager::instance ( ).join ( );	// CP Magix3D Issue #54

	MutexUnlocker	unlocker (&_sequentialMutex);
	if (0 == cmd)
		throw CommandException (UTF8String ("CommandManager::executeSequential : NULL command.", Charset::UTF_8), cmd);

	AutoMutex	autoMutex (getMutex ( ));
	if (0 != _sequentialCommand)
		throw CommandException (UTF8String ("CommandManager::executeSequential : une commande est déjà en cours d'exécution.", Charset::UTF_8), cmd);

	Command*	command	= dynamic_cast<Command*>(cmd);
	if (0 == command)
		throw CommandException (UTF8String ("CommandManager::executeSequential : cmd is not an instance of Command.", Charset::UTF_8), cmd);

	unsigned long	estimatedTime	= command->getEstimatedDuration (pt);
	UTF8String	message1 (Charset::UTF_8), message2 (Charset::UTF_8);
	message1 << "Exécution séquentielle de type "
	         << Command::playTypeToString (pt) <<  " de la commande "
	         << command->getName ( ) << " de nom unique "
	         << command->getUniqueName ( );
	message2	= message1;
	message1 << ". Durée estimée : " << estimatedTime << " secondes.";
	MGX_TRACE_LOG_1 (trace1, message1)
	log (trace1);
	if (Command::UNDO == pt)
		getUndoManager ( ).undo ( );
	else if (Command::REDO == pt)
		getUndoManager ( ).redo ( );
	_sequentialCommand	= cmd;
	try
	{
		command->execute (pt);
	}
	catch (const Exception& exc)
	{
		_sequentialCommand	= 0;
		throw CommandException (exc, cmd);
	}
	catch (...)
	{
		_sequentialCommand	= 0;
		throw CommandException (UTF8String ("Erreur non documentée", Charset::UTF_8), cmd);
	}
	message2 << " effectuée. Status de terminaison : " << command->getStrStatus ( ) << ".";
	MGX_TRACE_LOG_1 (trace2, message2)
	log (trace2);

	_sequentialCommand	= 0;
	_sequentialMutex.unlock ( );
	unlocker.setMutex (0);
}	// CommandManager::executeSequential


void CommandManager::executeThreaded (Command* cmd, Command::PLAY_TYPE pt)
{
	MutexUnlocker	unlocker (&_sequentialMutex);
	Command*	command	= dynamic_cast<Command*>(cmd);
	if (0 == command)
		throw CommandException (UTF8String ("CommandManager::executeThreaded : NULL command.", Charset::UTF_8), cmd);

	AutoMutex	autoMutex (getMutex ( ));

	unsigned long	estimatedTime	= command->getEstimatedDuration (pt);
	UTF8String	message1 (Charset::UTF_8), message2 (Charset::UTF_8);
	message1 << "Exécution de type " << Command::playTypeToString (pt)
	         <<  " de la commande " << command->getName ( )
	         << " de nom unique " << command->getUniqueName ( )
	         << " dans un thread dédié. Durée estimée : "
	         << estimatedTime << " secondes.";
	MGX_TRACE_LOG_1 (trace1, message1)
	log (trace1);
	CommandRunner*	runner	= new CommandRunner (this, command, pt);
	command->setCommandRunner (runner);
	runner->setLogStream (getLogStream ( ));
	_runnersMutex.lock ( );
	_runners.push_back (runner);
		if (Command::UNDO == pt)
			getUndoManager ( ).undo ( );
		else if (Command::REDO == pt)
			getUndoManager ( ).redo ( );
	_runnersMutex.unlock ( );
	// Rem CP 14/03/11 : les runners ne se suicident plus, ils sont pris
	// en charge par l'API TkUtil::ThreadManager qui les détruira une fois
	// l'exécution complétement terminée.
	// C'est également TkUtil::ThreadManager qui démarrera la commande
	// (cf. API TkUtil::ThreadManager).
//	runner->startTask ( );
	ThreadManager::instance ( ).addTask (runner);
	message2 << "Lancement de la commande " << command->getName ( )
	         << " de type " << Command::playTypeToString (pt)
	         << " dans un thread dédié effectué.";
	MGX_TRACE_LOG_1 (trace2, message2)
	log (trace2);
}	// CommandManager::executeThreaded


void CommandManager::runQueuedCommand ( )
{
	if (0 != _sequentialMutex.getLockCount ( ))
		return;

	if ((false == hasRunningCommands ( )) && (0 != _queuedCommands.size ( )))
	{
		{
			Command*				cmd	= 0;
			Command::PLAY_TYPE	pt;
			{
				AutoMutex	mutex (&_queuingMutex);
				cmd	= (*(_queuedCommands.begin ( ))).first;
				pt	= (*(_queuedCommands.begin ( ))).second;
				if (Command::QUEUED == pt)
					pt	= Command::DO;
				_queuedCommands.pop_front (  );
				if (0 == cmd)
					return;
			}

			unsigned long	estimatedTime	= cmd->getEstimatedDuration (pt);
			if ((SEQUENTIAL == getPolicy ( )) ||
			    (false == cmd->threadable ( ))       ||
			    (estimatedTime < sequentialDuration) ||
			    (Command::UNDO == pt)             ||
			    (Command::REDO == pt)
				)
			{
				executeSequential (cmd, pt);
			}
			else
				executeThreaded (cmd, pt);
		}	// if (true == _queuingMutex.tryLock ( ))
	}	// if ((false == hasRunningCommands ( )) && ...
}	// CommandManager::runQueuedCommand


void CommandManager::addToQueue (Command* command, Command::PLAY_TYPE pt)
{
	if (0 == command)
	{
		INTERNAL_ERROR (exc, "Commande nulle.", "CommandManager::addToQueue")
		throw exc;
	}	// if (0 == command)

	Command*	cmd	= dynamic_cast<Command*>(command);
	if (Command::QUEUED == pt)
		pt	= Command::DO;
	{
		AutoMutex	queuingAutoMutex (&_queuingMutex);
		_queuedCommands.push_back (
								pair<Command*, Command::PLAY_TYPE>(cmd, pt));
	}

	UTF8String	message (Charset::UTF_8);
	message << "Commande " << command->getName ( )
		    << " de nom unique " << command->getUniqueName ( )
		    << " mise en file d'attente.";
	MGX_TRACE_LOG_1 (trace, message)
	log (trace);
}	// CommandManager::addToQueue

}	// namespace Utils

}	// namespace Mgx3D
