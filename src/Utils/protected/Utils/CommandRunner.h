#ifndef COMMAND_RUNNER_H
#define COMMAND_RUNNER_H

#include "Utils/Command.h"

#include <TkUtil/Threads.h>


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Utils {

class CommandManager;


/**
 * \brief		Classe exécutant des commandes Magix 3D dans des threads
 *				différents, qualifiée de <I>lanceur de commande</I>.
 */
class CommandRunner : public TkUtil::JoinableThread
{
	public :

	/**
	 * Constructeur.
	 * \param		Gestionnaire de commandes associé
	 * \param		Commande à exécuter.
	 * \param		Type d'exécution à effectuer.
	 * \warning		La commande n'est pas adoptée, et, de ce fait, n'est pas
	 *				détruite avec l'instance de <I>CommandRunner</I>.
	 *				En cas de destruction de la commande il est nécessaire
	 *				d'en informer l'instance de <I>CommandRunner</I> via un
	 *				appel à <I>setCommand (0)</I>.
	 * \see			setCommand
	 */
	CommandRunner (Mgx3D::Utils::CommandManager* manager,
	               Mgx3D::Utils::Command* command, Command::PLAY_TYPE pt);

	/**
	 * Destructeur.
	 * <B>Ne détruit pas la commande gérée.</B>
	 */
	virtual ~CommandRunner ( );

	/**
	 * Exécute la commande.
	 */
	virtual void execute ( );

	/**
	 * Arrête le thread si cela est possible.
	 * @see			API JoinableThread::cancel.
	 */
	virtual void cancel ( );

	/**
	 * \return		La commande à exécuter, s'il y en a une.
	 * \see			setCommand
	 */
	Mgx3D::Utils::Command* getCommand ( )
	{ return _command; }
	const Mgx3D::Utils::Command* getCommand ( ) const
	{ return _command; }

	/**
	 * \param		Nouvelle commande à exécuter, ou 0.
	 * \warning		Une exception est levée en cas de commande en cours
	 *				d'exécution.
	 * \see			getCommand
	 */
	virtual void setCommand (Mgx3D::Utils::Command* command);

	/**
	 * \return		Le type d'exécution de la commande.
	 */
	virtual Command::PLAY_TYPE getPlayType ( ) const
	{ return _playType; }

	/**
	 * \param		Gestionnaire de message à utiliser pour afficher des
	 *				informations sur le déroulement de la commande, ou 0.
	 * \see			log
	 * \see			getLogStream
	 */
	virtual void setLogStream (TkUtil::LogOutputStream* stream)
	{ _logStream	= stream; }

	/**
	 * \return		Le gestionnaire de message utilisé pour afficher des
	 *				informations sur le déroulement de la commande, ou 0.
	 * \see			log
	 * \see			setLogStream
	 */
	virtual TkUtil::LogOutputStream* getLogStream ( )
	{ return _logStream; }


	protected :

	/**
	 * Appellé lorsque le thread est annulé. Doit libérer autant que possible
	 * les ressources allouées par la tache.
	 */
	virtual void cancelledCleanup ( );

	/**
	 * Envoit le <I>log</I> transmis en argument dans le flux de messages
	 * associé a l'instance, ou, à défaut, dans <I>cout</I>.
	 * \see		TkUtil::Log
	 */
	virtual void log (const TkUtil::Log& log);

	/**
	 * \return		Le verrou associé à l'instance.
	 */
	virtual TkUtil::Mutex* getMutex ( )
	{ return _mutex; }


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	CommandRunner (const CommandRunner&);
	CommandRunner& operator = (const CommandRunner&);

	/** Appelé lorsqu'un <I>lanceur</I> est interrompu. */
	static void runnerAborted (int notused);


	/** La commande à exécuter. */
	Mgx3D::Utils::Command*			_command;

	/** Le gestionnaire de commandes associé. */
	Mgx3D::Utils::CommandManager*	_manager;

	/** Infos sur la commande à exécuter. */
	std::string						_cmdName, _cmdUniqueName;

	/** Mutex protégeant certaines opérations. */
	TkUtil::Mutex*					_mutex;

	/** Le type d'exécution effectué. */
	Command::PLAY_TYPE				_playType;

	/** Le gestionnaire de log utilisé pour afficher des messages sur le
	 * déroulement de la tâche. */
	TkUtil::LogOutputStream*		_logStream;
};	// class CommandRunner


} // end namespace Utils
/*----------------------------------------------------------------------------*/

} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/


#endif	// COMMAND_RUNNER_H
