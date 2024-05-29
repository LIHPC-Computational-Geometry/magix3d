#include "Utils/CommandIfc.h"
#include "Utils/ErrorManagement.h"
#include "Utils/Magix3DEvents.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/UTF8String.h>

#include <time.h>

using namespace TkUtil;
using namespace std;


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Utils {

/*----------------------------------------------------------------------------*/



CommandIfc::CommandIfc ( )
	: _commandMutex (0), _userNotified (false)
{
	_commandMutex	= new Mutex ( );
}	// CommandIfc::CommandIfc


CommandIfc::CommandIfc (const CommandIfc& c)
	: _commandMutex (0), _userNotified (c._userNotified)
{
	INTERNAL_ERROR (exc, "Opération interdite.", "CommandIfc::CommandIfc")
	throw exc;
}	// CommandIfc::CommandIfc


CommandIfc& CommandIfc::operator = (const CommandIfc& c)
{
	INTERNAL_ERROR (exc, "Opération interdite.", "CommandIfc::CommandIfc")
	throw exc;
/* Commentaires pour coverity
	if (&c != this)
	{
	}	// if (&c != this)

	return *this;
*/
}	// CommandIfc::operator =


CommandIfc::~CommandIfc ( )
{
	delete _commandMutex;		_commandMutex	= 0;
}	// CommandIfc::~CommandIfc


const string& CommandIfc::getName ( ) const
{
	throw Exception ("CommandIfc::getName should be overloaded.");
}	// CommandIfc::getName


string CommandIfc::getUniqueName ( ) const
{
	throw Exception ("CommandIfc::getUniqueName should be overloaded.");
}	// CommandIfc::getUniqueName


TkUtil::Mutex* CommandIfc::getCommandMutex ( )
{
	return _commandMutex;
}	// CommandIfc::getCommandMutex


const TkUtil::Mutex* CommandIfc::getCommandMutex ( ) const
{
	return _commandMutex;
}	// CommandIfc::getCommandMutex


UTF8String CommandIfc::statusToString (CommandIfc::status s)
{   
	switch (s) 
	{
		case CommandIfc::INITED			: return UTF8String ("initialisé", Charset::UTF_8);
        case CommandIfc::STARTING       : return UTF8String ("démarre", Charset::UTF_8);
		case CommandIfc::PROCESSING		: return UTF8String ("en cours de traitement", Charset::UTF_8);
		case CommandIfc::DONE			: return UTF8String ("achevé", Charset::UTF_8);
		case CommandIfc::CANCELED		: return UTF8String ("annulé", Charset::UTF_8);
		case CommandIfc::FAIL			: return UTF8String ("en erreur", Charset::UTF_8);
	}   // switch (s)

	return UTF8String ("inconnu", Charset::UTF_8);
}   // CommandIfc::statusToString


CommandIfc::status CommandIfc::getStatus ( ) const
{
	throw Exception ("CommandIfc::getStatus should be overloaded.");
}	// CommandIfc::getStatus


UTF8String CommandIfc::getStrStatus ( ) const
{
	return CommandIfc::statusToString (getStatus ( ));
}	// CommandIfc::getStrStatus


bool CommandIfc::isCompleted ( ) const
{
	throw Exception ("CommandIfc::isCompleted should be overloaded.");
}	// CommandIfc::isCompleted


const UTF8String& CommandIfc::getErrorMessage ( ) const
{
	throw Exception ("CommandIfc::getErrorMessage should be overloaded.");
}	// CommandIfc::getErrorMessage


bool CommandIfc::isUserNotified ( ) const
{
	return _userNotified;
}	// CommandIfc::isUserNotified


void CommandIfc::setUserNotified (bool notified)
{
	_userNotified	= notified;
}	// CommandIfc::setUserNotified


double CommandIfc::getProgression ( ) const
{
	throw Exception ("CommandIfc::getProgression should be overloaded.");
}	// CommandIfc::getProgression


UTF8String CommandIfc::getStrProgression ( ) const
{
	throw Exception ("CommandIfc::getStrProgression should be overloaded.");
}	// CommandIfc::getStrProgression


CommandIfc::PLAY_TYPE CommandIfc::getPlayType ( ) const
{
	throw Exception ("CommandIfc::getPlayType should be overloaded.");
}	// CommandIfc::getPlayType


UTF8String CommandIfc::getStrPlayType ( ) const
{
	return CommandIfc::playTypeToString (getPlayType ( ));
}	// CommandIfc::getStrPlayType


CommandIfc::status CommandIfc::execute (CommandIfc::PLAY_TYPE playType)
{
	throw Exception ("CommandIfc::execute should be overloaded.");
}	// CommandIfc::execute


CommandIfc::status CommandIfc::execute ( )
{
	throw Exception ("CommandIfc::execute should be overloaded.");
}	// CommandIfc::execute


CommandIfc::status CommandIfc::undo ( )
{
	throw Exception ("CommandIfc::undo should be overloaded.");
}	// CommandIfc::undo


CommandIfc::status CommandIfc::redo ( )
{
	throw Exception ("CommandIfc::redo should be overloaded.");
}	// CommandIfc::redo


void CommandIfc::cancel ( )
{
	throw Exception ("CommandIfc::cancel should be overloaded.");
}	// CommandIfc::cancel


void CommandIfc::cancelledCleanup ( )
{
	throw Exception ("CommandIfc::cancelledCleanup should be overloaded.");
}	// CommandIfc::cancelledCleanup


void CommandIfc::taskCompleted ( )
{
}	// CommandIfc::taskCompleted


void CommandIfc::notifyObserversForModifications ( )
{
}	// CommandIfc::notifyObserversForModifications


UTF8String CommandIfc::playTypeToString (CommandIfc::PLAY_TYPE pt)
{
	switch (pt)
	{
		case CommandIfc::QUEUED		: return UTF8String ("mise en file d'attente", Charset::UTF_8);
		case CommandIfc::DO			: return UTF8String ("exécutée", Charset::UTF_8);
		case CommandIfc::UNDO		: return UTF8String ("défaite", Charset::UTF_8);
		case CommandIfc::REDO		: return UTF8String ("rejouée", Charset::UTF_8);
	}	// switch (pt)

	UTF8String	message (Charset::UTF_8);
	message << "Type d'exécution de commande inconnu : " << (unsigned long)pt << ".";
	INTERNAL_ERROR (exc, message, "CommandIfc::playTypeToString")
	throw exc;
}	// CommandIfc::playTypeToString


const Timer& CommandIfc::getTimer ( ) const
{
	throw Exception ("CommandIfc::getTimer should be overloaded.");
}	// CommandIfc::getTimer


Timer& CommandIfc::getTimer ( )
{
	throw Exception ("CommandIfc::getTimer should be overloaded.");
}	// CommandIfc::getTimer


/*----------------------------------------------------------------------------*/
} // end namespace Utils

/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
