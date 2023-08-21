#include "Utils/Common.h"
#include "Utils/CommandRunner.h"
#include "Utils/CommandManager.h"
#include "Utils/ErrorManagement.h"
#include "Utils/MgxNumeric.h"

#include <TkUtil/ErrorLog.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/TraceLog.h>

#include <assert.h>
#include <stdio.h>

#include <vector>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;

/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Utils {


/** Les lanceurs de commandes, et le verrou les protégeants. */
static vector<CommandRunner*>	runners;
static Mutex					runnersMutex;


CommandRunner::CommandRunner (
			CommandManager* manager, Command* command, Command::PLAY_TYPE pt)
	: JoinableThread ( ),
	  _command (0), _manager (manager),
	  _cmdName (0 == command ? string ("NULL") : command->getName ( )),
	  _cmdUniqueName(0 == command ? string("NULL") : command->getUniqueName( )),
	  _mutex ( ), _playType (pt),
	  _logStream (0)
{
	_mutex	= new Mutex ( );
	setCommand (command);

	AutoMutex	runnersAutoMutex (&runnersMutex);
	runners.push_back (this);

	UTF8String	message (Charset::UTF_8);
	message << "Création d'un lanceur de commande pour la commande "
	        << _cmdName << " de nom unique " << _cmdUniqueName
	        << ", type d'exécution " << Command::playTypeToString (pt) << ".";
	MGX_TRACE_LOG_1 (trace1, message)
	log (trace1);
}	// CommandRunner::CommandRunner


CommandRunner::CommandRunner (const CommandRunner& cr)
	: JoinableThread ( ),
	  _command (0), _manager (0), _cmdName ( ), _cmdUniqueName ( ),
	  _mutex ( ), _playType (cr._playType), _logStream (0)
{
	MGX_FORBIDDEN ("CommandRunner copy constructor is not allowed.");
}	// CommandRunner::CommandRunner


CommandRunner& CommandRunner::operator = (const CommandRunner& cr)
{
	MGX_FORBIDDEN ("CommandRunner assignment operator is not allowed.");
	if (&cr != this)
	{
	}	// if (&cr != this)

	return *this;
}	// CommandRunner::CommandRunner


CommandRunner::~CommandRunner ( )
{
	AutoMutex	runnersAutoMutex (&runnersMutex);
	for (vector<CommandRunner*>::iterator itr = runners.begin ( );
	     runners.end ( ) != itr; itr++)
		if (*itr == this)
		{
			runners.erase (itr);
			break;
		}
	if (0 != _manager)
	{
		_manager->commandRunnerDeleted (this);
		_manager	= 0;
	}	// if (0 != _manager)

	// ATTENTION : à ce stade la commande est peut être détruite.
	UTF8String	message;
	message << "Destruction du lanceur de commande associé à la commande "
	        << _cmdName << " de nom unique " << _cmdUniqueName
	        << ", type d'exécution "
	        << Command::playTypeToString (getPlayType ( ))
	        << ".";
	MGX_TRACE_LOG_1 (trace1, message)
	log (trace1);

	_command	= 0;
	delete _mutex;	_mutex	= 0;
}	// CommandRunner::~CommandRunner


void CommandRunner::execute ( )
{
	string			commandName, commandUniqueName;
	// Attention, on est dans un autre thread que le thread principal, toute
	// exception levée ici serait fatale ...
	// On récupère donc toute exception levée, bien qu'il ne devrait pas y en
	// avoir.
	if (0 != getCommand ( ))
	{
		// _cmdName : on a déjà l'info, mais peut être a t-elle changée ?
		_cmdName		= getCommand ( )->getName ( );
		_cmdUniqueName	= getCommand ( )->getUniqueName ( );
		UTF8String	message1 (Charset::UTF_8), message2 (Charset::UTF_8);
		message1 << "Exécution de la commande " << _cmdName
		         << " de nom unique " << _cmdUniqueName
	             << ", type d'exécution "
		         << Command::playTypeToString (getPlayType ( ))
		         << " dans un thread dédié.";
		MGX_TRACE_LOG_1 (trace1, message1)
		log (trace1);
		Command::status	status	= Command::INITED;
		Timer			timer;

//		BEGIN_TRY_CATCH_BLOCK
		bool	hasError = false;
		string	errorString;
		try
		{

			timer.start ( );

			status	= getCommand ( )->execute (getPlayType ( ));

		}
		catch (const Exception& exc)
		{
			hasError	= true;
			errorString	= exc.getFullMessage ( );
		}
		catch (...)
		{
			hasError	= true;
			errorString	= "erreur non documentée.";
printf ("%s %d COMMAND INTERRUPTED.\n", __FILE__, __LINE__);
		}
//		COMPLETE_TRY_CATCH_BLOCK

		timer.stop ( );

		if (true == hasError)
		{
			UTF8String	error (Charset::UTF_8);
			error << "ERREUR INTERNE lors de l'exécution de la commande "
			      << _cmdName << " (nom unique : " << _cmdUniqueName
			      << ") dans le thread " << (unsigned long)getId ( ) << " : "
			      << errorString << "." << "\n"
			      << "Merci d'informer l'équipe de développement de ce "
			      << "logiciel de cette anomalie récupérée en ligne "
			      << (unsigned long)__LINE__ << " du fichier " << __FILE__
			      << ".";
			log (ErrorLog (error));
		}	// if (true == hasError)
		else
		{
			message2 << "Succès de l'exécution de la commande "
			         << _cmdName << " (nom unique : " << _cmdUniqueName
			         << ") dans le thread " << (unsigned long)getId ( ) << ", "
			         << "\n"
			         << ", type d'exécution "
			         << Command::playTypeToString (getPlayType ( )) << ". "
			         << "Status d'exécution de la commande : "
			         << Command::statusToString (status)
				     << ", temps d'exécution : "
				     << MgxNumeric::userRepresentation (timer) << ".";
			log (TkUtil::TraceLog (message2, TkUtil::Log::TRACE_1));
		}	// else if (true == hasError)
	}	// if (0 != getCommand ( ))
}	// CommandRunner::execute


void CommandRunner::cancel ( )
{
	JoinableThread::cancel ( );

//	join ( );
	cancelledCleanup ( );
}	// CommandRunner::cancel


void CommandRunner::setCommand (Mgx3D::Utils::Command* command)
{
	assert (0 != getMutex ( ));
	AutoMutex	autoMutex (getMutex ( ));
	UTF8String	message1 (Charset::UTF_8);
	message1 << "Remplacement de la commande "
	         << (0 == getCommand ( ) ?
							string ("NULL") : getCommand ( )->getName ( ))
	         << " de nom unique "
	         << (0 == getCommand ( ) ?
							string ("NULL") : getCommand ( )->getUniqueName ( ))
	         << " par la commande "
	         << (0 == command ? string ("NULL") : command->getName ( ))
	         << " de nom unique "
	         << (0 == command ? string ("NULL") : command->getUniqueName ( ))
	         << " dans un lanceur de commande.";
	MGX_TRACE_LOG_1 (trace1, message1)
	log (trace1);

	if ((0 != _command) && (Command::PROCESSING == _command->getStatus ( )))
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur lors du remplacement de la commande "
		      << _command->getName ( ) << " de nom unique "
		      << _command->getUniqueName ( ) << " par la commande "
		      << (0 == command ? string ("NULL") : command->getName ( ))
		      << " de nom unique "
		      << (0 == command ? string ("NULL") : command->getUniqueName ( ))
	          <<" dans un lanceur de commande : commande en cours d'exécution.";
		log (ErrorLog (error));

		throw Exception (error.iso ( ));
	}	// if ((0 != _command) && (Command::PROCESSING == ...

	UTF8String	message2 (Charset::UTF_8);
	if (0 == _command)
		message2 << "Affectation de la commande ";
	else
		message2 << "Remplacement de la commande "
		         << _command->getName ( ) << " de nom unique "
		         << _command->getUniqueName ( ) << " par la commande ";
	message2 << (0 == command ? string ("NULL") : command->getName ( ))
	         << " de nom unique "
	         << (0 == command ? string ("NULL") : command->getUniqueName ( ))
	         << " dans un lanceur de commande.";
	if (0 == _command)
        log (TkUtil::TraceLog (message2, TkUtil::Log::TRACE_1));
	else
	{
		MGX_TRACE_LOG_1 (trace1, message2);
		log (trace1);
	}

	_command	= command;
}	// CommandRunner::setCommand


void CommandRunner::cancelledCleanup ( )
{
	try
	{
		// On est ici dans une situation très inconfortable. L'utilisateur a
		// demandé l'annulation de la commande.
		if (0 != getCommand ( ))
			getCommand ( )->cancelledCleanup ( );

		if (0 != getMutex ( ))
			getMutex ( )->unlock ( );

		runnersMutex.unlock ( );
	}
	catch (...)
	{
	}
}	// CommandRunner::cancelledCleanup


void CommandRunner::log (const TkUtil::Log& l)
{
	AutoMutex	autoMutex (getMutex ( ));

	if (0 != getLogStream ( ))
		getLogStream ( )->log (l);
}	// CommandRunner::log


} // end namespace Utils
/*----------------------------------------------------------------------------*/
    
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

