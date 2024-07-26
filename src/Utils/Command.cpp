#include "Utils/Common.h"
#include "Utils/Command.h"
#include "Utils/CommandManager.h"
#include "Utils/CommandRunner.h"
#include "Utils/ErrorManagement.h"
#include "Utils/Magix3DEvents.h"
#include "Utils/MgxNumeric.h"

#include <TkUtil/ErrorLog.h>
#include <TkUtil/InformationLog.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NetworkData.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>

#include <time.h>

using namespace Mgx3D::Utils::Math;
using namespace TkUtil;
using namespace std;


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Utils {

/*----------------------------------------------------------------------------*/



// ============================================================================
//                               LA CLASSE Command
// ============================================================================

Mutex	Command::_mutex;


Command::Command (const string& name)
	: CommandIfc ( ), ReferencedNamedObject (name, true),
	  _status (Command::INITED), _isCompleted(false), _playType (Command::QUEUED), _progress (0.), _timer ( ),
	  _completionTasks ( ), _commandRunner (0), _logStream (0),
	  _scriptComments(name, Charset::UTF_8), _savedScriptComments (Charset::UTF_8),
	  _scriptCommand("Script pour la commande non renseigné !!!", Charset::UTF_8),
          _savedScriptCommand (Charset::UTF_8), _errorMessage  (Charset::UTF_8),
	  _isScriptable(true)
{
	setUniqueName (createUniqueName ( ));
}	// Command::Command


Command::Command (const Command& c)
	: CommandIfc ( ), ReferencedNamedObject ("", true),	
	  _status (c._status), _isCompleted(c._isCompleted), _playType (c._playType), _progress (c._progress), _timer ( ),
	  _completionTasks (c._completionTasks), _commandRunner (0), _logStream (0), _isScriptable(true)
{
	INTERNAL_ERROR (exc, "Opération interdite.", "Command::Command")
	throw exc;
}	// Command::Command


Command& Command::operator = (const Command& c)
{
	INTERNAL_ERROR (exc, "Opération interdite.", "Command::Command")
	throw exc;
/* Pour faire plaisir Ã  coverity ces commentaires
	if (&c != this)
	{
	}	// if (&c != this)

	return *this; */
}	// Command::operator =


Command::~Command ( )
{
	UTF8String	message1 (Charset::UTF_8), message2 (Charset::UTF_8);
	message1 << "Destruction de la commande " << getName( ) << " de nom unique " << getUniqueName ( );
	message2	= message1;
	message1 << " en cours.";
	MGX_TRACE_LOG_3 (trace1, message1)
	log (trace1);

	notifyObserversForDestruction ( );
	unregisterReferences ( );

	message2 << " effectué.";
	MGX_TRACE_LOG_3 (trace2, message2)
	log (trace2);
}	// Command::~Command


const string& Command::getName ( ) const
{
	return ReferencedNamedObject::getName ( );
}	// Command::getName


string Command::getUniqueName ( ) const
{
	return ReferencedNamedObject::getUniqueName ( );
}	// Command::getUniqueName


string Command::createUniqueName ( )
{
	AutoMutex		autoMutex (&_mutex);
	UTF8String	name (Charset::UTF_8);

	name << "Command_" << NetworkData ( ).getInetAddress ( ) << "_" << (unsigned long)(this);
// Rem CP :
// Précision clock == 1 seconde => plusieurs cmd ont le même nom unique !
// Il s'agit de temps CPU, donc 2 commandes successives, même espacées dans le temps, ont de bonnes chances d'avoir le même nom.
//	     << (unsigned long)clock ( );

	return name.iso ( );
}	// Command::createUniqueName


Command::status Command::execute (Command::PLAY_TYPE playType)
{
	AutoMutex	automutex (getCommandMutex ( ));

	setPlayType (playType);
	switch (playType)
	{
		case Command::DO		: return execute ( );
		case Command::UNDO		: return undo ( );
		case Command::REDO		: return redo ( );
		case Command::QUEUED	:
			{
				UTF8String	message (Charset::UTF_8);
				message << "Impossible d'exécuter la commande " << getName ( ) << " de nom unique " << getUniqueName ( ) << " : "
				        << playTypeToString (playType) << " n'est pas un type d'exécution.";
				throw Exception (message);
			}
	}	// switch (playType)

	UTF8String	error (Charset::UTF_8);
	error << "Impossible d'exécuter la commande " << getName ( ) << " de nom unique " << getUniqueName ( ) << ". Type d'exécution "
	      << (unsigned long)playType << " inconnu.";
	INTERNAL_ERROR (exc, error, "Command::execute")

	throw exc;
}	// Command::execute


Command::status Command::execute ( )
{
	AutoMutex	automutex (getCommandMutex ( ));

	if (INITED != getStatus ( ))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Exécution de la commande " << getName ( ) << " impossible. Etat courant : " << statusToString (getStatus( )) << ".";
		log (ErrorLog (message));

		throw Exception (message);
	}	// if (INITED != getStatus ( ))

    setStatus (STARTING);
	setStatus (PROCESSING);
	getTimer ( ).start ( );

	return getStatus ( );
}	// Command::execute


//Command::status Command::undo ( )
//{
//	AutoMutex	automutex (getCommandMutex ( ));
//	UTF8String	message1, message2;
//	message1 << "Annulation de la commande " << getName ( ) << " de nom unique "
//	         << getUniqueName ( );
//	message2	= message1;
//	message1 << " en cours.";
//	MGX_TRACE_LOG_3 (trace1, message1)
//	log (trace1);
//
//	if (DONE != getStatus ( ))
//	{
//		UTF8String	message;
//		message << "Réversion de la commande " << getName ( )
//		        << " impossible. Etat courant : "
//		        << statusToString (getStatus( )) << ".";
//		throw Exception (message);
//	}	// if (DONE != getStatus ( ))
//
//	setStatus (Command::INITED);
//	getTimer ( ).reset ( );
//
//	message2 << " effectué.";
//	MGX_TRACE_LOG_3 (trace2, message2)
//	log (trace2);
//
//	return getStatus ( );
//}	// Command::undo


//Command::status Command::redo ( )
//{
//	UTF8String	message1, message2;
//	message1 << "Rejeu de la commande " << getName ( ) << " de nom unique " << getUniqueName ( );
//	message2	= message1;
//	message1 << " en cours.";
//	MGX_TRACE_LOG_3 (trace1, message1)
//	log (trace1);
//
//	execute ( );
//
//	message2 << " effectué.";
//	MGX_TRACE_LOG_3 (trace2, message2)
//	log (trace2);
//
//	return getStatus ( );
//}	// Command::redo


void Command::cancel ( )
{
	// Hors mutex  on affecte CANCELED à _status afin de pouvoir interrompre l'exécution dans un autre thread.
	// Puis on prend le mutex et on attend qu'il soit libéré afin de faire un setStatus (CANCELED) dans les règles de l'art.
	// TODO [CP] : envisager une méthode non virtuelle protégée type "setCanceled" qui forcerait le status Ã  devenir CANCELED et retournerait le status initial.
	Command::status	currentStatus	= getStatus ( );
	_status	= Command::CANCELED;

	// On prend le mutex : L'exécution dans un autre thread est interrompue.
	AutoMutex	automutex (getCommandMutex ( ));

	// On réaffecte le status initial afin de gérer au mieux les différentes transitions entre états :
	_status	= currentStatus;
	if (false == cancelable ( ))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Annulation de la commande " << getName ( ) << " de nom unique " << getUniqueName ( ) << " impossible : commande non interruptible.";
		log (ErrorLog (message));
		throw Exception (message);
	}	// if (false == cancelable ( ))

	switch (currentStatus)
	{
		case DONE		:
		case FAIL		:
			{
				UTF8String	message (Charset::UTF_8);
				message << "Annulation de la commande " << getName ( ) << " de nom unique " << getUniqueName ( ) << " sans effet. Etat courant : " << statusToString (getStatus ( )) << ".";
				log (InformationLog (message));
			}
			break;
		case INITED		:
        case STARTING   :
		case PROCESSING	:
			{
				setStatus (Command::CANCELED);
				getTimer ( ).stop ( );
				UTF8String	message (Charset::UTF_8);
				message << "Annulation de la commande " << getName ( ) << " de nom unique " << getUniqueName ( ) << " au bout de " << MgxNumeric::userRepresentation (getTimer ( )) << ".";
				log (InformationLog (message));
			}
			break;
		default			:
			{
				UTF8String	message (Charset::UTF_8);
				message << "Annulation de la commande " << getName ( ) << " de nom unique " << getUniqueName ( ) << " impossible. Etat courant : " << statusToString (getStatus ( )) << ".";
				log (ErrorLog (message));
				throw Exception (message);
			}
	}	// switch (currentStatus)
}	// Command::cancel


CommandRunner* Command::getCommandRunner ( ) const
{
	return _commandRunner;
}	// Command::getCommandRunner


void Command::setCommandRunner (CommandRunner* runner)
{
	_commandRunner	= runner;
}	// Command::setCommandRunner


void Command::cancelledCleanup ( )
{
	_status			= Command::FAIL;
	_isCompleted	= true;

	try
	{
		emergencyCleanup ( );
	}
	catch (...)
	{
	}
}	// Command::cancelledCleanup


void Command::addCompletionTask (CommandTask task, void* clientData)
{
	AutoMutex	automutex (getCommandMutex ( ));

	_completionTasks.insert (pair<CommandTask, void*>(task, clientData));
}	// Command::addCompletionTask


void Command::notifyObserversForModification (unsigned long event)
{
	AutoMutex	automutex (getCommandMutex ( ));

	// Les observateurs locaux :
	ReferencedNamedObject::notifyObserversForModification (event);
}	// Command::notifyObserversForModification


void Command::setStatus (Command::status status)
{
	AutoMutex	automutex (getCommandMutex ( ));

	UTF8String	message (Charset::UTF_8);
	message << "Changement de status de la commande " << getName ( ) << " de nom unique " << getUniqueName ( ) << " : " << statusToString (getStatus ( )) << " -> ";

	bool changeStatus = (_status != status);
	_status	= status;
	switch (_status)
	{
		case Command::INITED		:
		case Command::STARTING      :
			_isCompleted	= false;
			_progress		= 0.;
			break;
		case Command::PROCESSING	:
			_isCompleted	= false;
			break;
		case Command::DONE			: _progress = 1.;
		case Command::CANCELED		:
		case Command::FAIL			:
			_isCompleted	= true;
			getTimer ( ).stop ( );
	}	// switch (_status)

	// On prévient les observateurs avant l'appel à log () qui peut lever une exception ...
    if (changeStatus || _status == Command::PROCESSING)
	{
		setUserNotified (false);			// CP 07/24 : une commande change plusieurs fois de status => autant de notifications à faire
        notifyObserversForModification (COMMAND_STATE);
	}

	message << statusToString (status) << ", chronomètre = " << MgxNumeric::userRepresentation (getTimer ( )) << ".";
    MGX_TRACE_LOG_1 (trace1, message)
	log (trace1);
}	// Command::setStatus


void Command::setProgression (double progress)
{
	AutoMutex	automutex (getCommandMutex ( ));

	if ((0. > progress) || (1. < progress))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Erreur dans la valeur de progression de la commande " << getName ( ) << " (" << progress << "). Domaine de validité : (0., 1.).";
		log (ErrorLog (message));

		throw Exception (message);
	}	// if ((0. > progress) || (1. < progress))

	getTimer ( ).stop ( );	// Temps intermédiaire, chrono non arrêté.
	UTF8String	message (Charset::UTF_8);
	message << "Progression de la commande " << getName ( ) << " de nom unique " << getUniqueName ( ) << " : " << ios_base::fixed 
	        << IN_UTIL setprecision (2) << progress << ", chronomètre = " << MgxNumeric::userRepresentation (getTimer ( )) << ".";
	MGX_TRACE_LOG_1 (trace1, message)
	log (trace1);

	_progress	= progress;
	notifyObserversForModification (COMMAND_PROGRESSION);
}	// Command::setProgression


UTF8String Command::getStrProgression ( ) const
{
	const Command::status	status		= getStatus ( );
	const double			progress	= getProgression ( );
	UTF8String	str (Charset::UTF_8);
	str << getName ( );
	switch (status)
	{
		case Command::INITED		: str << " (initialisée)";		break;
		case Command::STARTING		: str << " (démarrée)";			break;
		case Command::PROCESSING	: str << " (en cours : " << ios_base::fixed << TkUtil::setprecision (2) << TkUtil::setw (5) << (100. * progress) << "%)";
			break;
		case Command::DONE			: str << " (achevée, succès)";	break;
		case Command::FAIL			: str << " (achevée, erreur)";	break;
		case Command::CANCELED		: str << " (annulée)";			break;
	}	// Command::getStrProgression 

	return str;
}	// Command::getStrProgression


void Command::setPlayType (Command::PLAY_TYPE pt)
{
	AutoMutex	automutex (getCommandMutex ( ));

	UTF8String	message (Charset::UTF_8);
	message << "Changement de type de jeu de la commande " << getName ( ) << " de nom unique " << getUniqueName ( ) << " : "
	        << playTypeToString (getPlayType ( )) << " -> " << playTypeToString (pt) << ".";

	_playType	= pt;
    MGX_TRACE_LOG_5 (trace1, message)
	log (trace1);
}	// Command::setPlayType


void Command::atCompletion ( )
{
	AutoMutex	autoMutex (&_mutex);

	UTF8String	message1 (Charset::UTF_8), message2 (Charset::UTF_8);
	message1 << "Commande " << getName ( ) << " de nom unique " << getUniqueName ( ) << " terminée. Exécution des fonctions de fin de tache ";
	message2	= message1;
	message1 << " en cours.";
	MGX_TRACE_LOG_3 (trace1, message1)
	log (trace1);

	for (map<CommandTask, void*>::iterator it = _completionTasks.begin ( ); _completionTasks.end ( ) != it; it++)
	{
		BEGIN_TRY_CATCH_BLOCK

		Command::CommandTask	f	= (*it).first;
		void*					cd	= (*it).second;
		f (cd);

		COMPLETE_TRY_CATCH_BLOCK
	}	// for (map<CommandTask, void*>::iterator it = ...

	message2 << " effectuée.";
	MGX_TRACE_LOG_3 (trace2, message2)
	log (trace2);
}	// Command::atCompletion


void Command::setLogStream (LogOutputStream* stream)
{
	AutoMutex	automutex (getCommandMutex ( ));

    _logStream  = stream;
}   // Command::setLogStream

void Command::log (const Log& l)
{
	AutoMutex	automutex (getCommandMutex ( ));

	if (0 != getLogStream ( ))
		getLogStream ( )->log (l);
}	// Command::log


void Command::startingOrcompletionLog (bool beginning)
{
	AutoMutex	automutex (&_mutex);

	UTF8String	message (Charset::UTF_8);
	message << (true == beginning ? "Lancement" : "Terminaison") << " de la commande " << getName ( ) << " de nom unique " << getUniqueName ( ) << ".";
	if (false == beginning)
		message << " Status de terminaison : " << statusToString (getStatus ( )) << ".";

	MGX_TRACE_LOG_1 (trace1, message)
	log (trace1);
}	// Command::startingOrcompletionLog


void Command::setErrorMessage(const TkUtil::UTF8String& msg)
{
    _errorMessage = msg;

    UTF8String   message (Charset::UTF_8);
    message << "Erreur de la commande " << getName ( ) << " avec le message : " << msg;
    log (ErrorLog (message));
}


void Command::saveScriptCommandAndComments()
{
	// CP correctif bogue undo/redo du 16/02/24.
	// Dans la version actuelle une commande n'est jamais modifiée, donc _savedScriptCommand doit être invariant.
	// Par ce test on évite le bogue suivant où _savedScriptCommand fini par valoir "ctx.redo ( )" :
	// ctx.getTopoManager ( ).newBoxWithTopo (Mgx3D.Point (0, 0, 0), Mgx3D.Point (1, 1, 1), 10, 10, 10)
	// ctx.undo ( )
	// ctx.redo ( )
	// ctx.undo ( )
	// ctx.redo ( )
	// Sans ce tests, le script mimimum généré contient alors pour unique instruction "ctx.redo ( )", alors qu'il devrait
	// bien sûr contenir "ctx.getTopoManager ( ).newBoxWithTopo (Mgx3D.Point (0, 0, 0), Mgx3D.Point (1, 1, 1), 10, 10, 10)".
	if (true == _savedScriptCommand.empty ( ))
	{
		_savedScriptCommand = _scriptCommand;
		_savedScriptComments = _scriptComments;
	}	// if (true == _savedScriptCommand.empty ( ))
}

TkUtil::UTF8String Command::getInitialScriptCommand() const
{
	if (_savedScriptCommand.empty())
		return _scriptCommand;
	else
		return _savedScriptCommand;
}

TkUtil::UTF8String Command::getInitialScriptComments() const
{
	if (_savedScriptComments.empty())
		return _scriptComments;
	else
		return _savedScriptComments;
}


/*----------------------------------------------------------------------------*/
} // end namespace Utils

/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
