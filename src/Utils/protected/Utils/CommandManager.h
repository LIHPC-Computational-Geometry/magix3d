/*----------------------------------------------------------------------------*/
/** \file CommandManager.h
 *
 *  \author Charles PIGNEROL
 *
 *  \date 10/11/2010
 */
/*----------------------------------------------------------------------------*/

#ifndef UTILS_COMMAND_MANAGER_H_
#define UTILS_COMMAND_MANAGER_H_

#include "Utils/CommandManagerIfc.h"
#include "Utils/CommandRunner.h"

#include <TkUtil/ReferencedObject.h>

#include <deque>
#include <string>
#include <vector>

namespace TkUtil {
class LogOutputStream;
}

/*----------------------------------------------------------------------------*/
namespace Mgx3D {


/*----------------------------------------------------------------------------*/
namespace Utils {

class UndoRedoManagerIfc;

/*----------------------------------------------------------------------------*/
/** \class CommandManager
 *  \brief Classe de base gérant un ensemble de commandes Magix 3D.
 *         Ses services fonctionnent en environnement multithread.
 *			<P>Les commandes jouées avec succès sont ajoutées à un service de
 *			<I>undo/redo</I> interne à cette classe, via une instance de la
 *			classe <I>UndoRedoManager</I>.
 *			</P>
 * \warning L'ensemble des méthodes, à l'exception des constructeurs et
 *         destructeurs, sont susceptibles de lever des exceptions de type
 *         <I>TkUtil::Exception</I>.
 */
/*----------------------------------------------------------------------------*/
class CommandManager :
	public Mgx3D::Utils::CommandManagerIfc, public TkUtil::ReferencedNamedObject
{
	public:

	/**
	 * Durée estimée d'une commande, en secondes, à partir de laquelle la
	 * commande est exécutée dans un thread dédié si elle est interruptible.
	 * Cette valeur vaut <I>(unsigned long)-1</I> secondes par défaut, c'est à
	 * dire que cela force une exécution séquentielle.
	 * \see		addCommand
	 * \see		Command::threadable
	 * \see		Command::getEstimatedDuration
	 */
	static unsigned long				sequentialDuration;

	/**
	 * La politique globale d'exécution des commande (il peut y avoir plusieurs
	 * gestionnaire de commandes, avec des politiques différentes, d'autant plus
	 * que les politique peuvent évoluer temporairement, dans le cadre
	 * d'exécution de scripts par exemple).
	 */
	static CommandManagerIfc::POLICY	runningPolicy;

	/**
	 * Constructeur. RAS.
	 * \param		Nom de l'instance.
	 * \see			getName
	 */
	CommandManager (const std::string& name);

	/**
	 * Destructeur. RAS.	
	 */
	virtual ~CommandManager ( );

    /** Réinitialisation
     */
    virtual void clear();

	/**
	 * \name	Le nom unique de l'instance (utile en environnement distribué).
	 */
	virtual const std::string& getName ( ) const;

	/**
	 * Méthodes relatives à la gestion des commandes : ajout, suppression,
	 * exécution.
	 */
	//@{

	/**
	 * \return  La politique d'exécution des commandes en cours.
	 * \see		setPolicy
	 */
	virtual CommandManagerIfc::POLICY getPolicy ( ) const;

	/**
	 * \param	Nouvelle politique d'exécution des commandes en cours.
	 * \return	L'ancienne politique d'exécution des commandes en cours.
	 * \see		getPolicy
	 */
	virtual CommandManagerIfc::POLICY setPolicy (
											CommandManagerIfc::POLICY policy);

	/**
	 * <P>Ajoute une commande à la liste des commandes gérées. Cette commande
	 * est complétement prise en charge par ce gestionnaire qui devient
	 * responsable de sa destruction.</P>
	 * <P>En cas de lancement dans un autre thread, ce lancement est effectué
	 * via une instance de la classe <I>CommandRunner</I>, dont le flux
	 * d'affichage sera celui de cette instance, accessible via
	 * <I>getLogStream</I>.
	 * </P>
	 * <P>Cette commande est ajoutée au gestionnnaire de <I>undo/redo</I> en cas
	 * de succès d'exécution, si le type d'exécution est <I>DO</I> et si elle
	 * n'est pas encore enregistrée auprès du gestionnaire.
	 * </P>
	 * <P>La commande peut être mise en file d'attente si le contexte interdit
	 * son lancement immédiat (exemple : commande à exécution séquentielle alors
	 * qu'une commande est déjà en cours d'exécution).
	 * </P>
	 * <P>En fin d'exécution invoque <I>runQueuedCommand</I> si aucune commande
	 * n'est en cours d'exécution et qu'il y a des commandes en file d'attente.
	 * </P>
	 * \param	Commande prise en charge
	 * \param	Type d'exécution à effectuer sur la commande, ou
	 *			<I>Command::QUEUED</I> s'il faut juste recenser la commande.
	 * \warning	La commande doit être une instance de la classe <I>Command</I>.
	 * \warning	Une commande peut être enregistrée plusieurs fois, ce qui permet
	 *			de l'enregistrer pour une première exécution, puis pour une
	 *			annulation, puis pour un rejeu, ...
	 * \see		getCommands
	 * \see		hasRunningCommands
	 * \see		hasQueuedCommands
	 * \see		processQueuedCommands
	 */
	virtual void addCommand (CommandIfc* command, Command::PLAY_TYPE pt);

	/**
	 * Exécute tant que c'est possible les commandes en file d'attente.
	 * \see		addCommand
	 * \see		hasRunningCommands
	 * \see		hasQueuedCommands
	 * \see		runQueuedCommand
	 */
	virtual void processQueuedCommands ( );

	/**
	 * \return		La liste des commandes (commandes en cours ou terminée, et pouvant être en erreur).
	 */
	virtual std::vector<CommandIfc*> getCommandIfcs ( ) const;
	virtual std::vector<Command*> getCommands ( ) const;

	/**
 	 * \return	<I>true</I> si des commandes sont en cours d'exécution
	 *			(exécution séquentielle ou dans un thread différent),
	 *			<I>false</I> dans le cas contraire.
	 * \see		addCommand
	 * \see		hasQueuedCommands
	 */
	virtual bool hasRunningCommands ( ) const;

	/**
	 * \return	<I>true</I> si des commandes sont en file d'attente,
	 *			<I>false</I> dans le cas contraire.
	 * \see		addCommand
	 * \see		hasRunningCommands
	 */
	virtual bool hasQueuedCommands ( ) const;

	/**
	 * \return	Un pointeur sur l'éventuel lanceur de commande associé à la
	 *			commande transmise en argument.
	 */
	virtual CommandRunner* getCommandRunner (Command* command);

	/**
	 * A appeler lors de la destruction du runner pour déréférencement.
	 */
	virtual void commandRunnerDeleted (CommandRunner* runner);

	/**
	 * Attend que toutes les commandes en cours soient achevées.
	 */
	virtual void wait ( );

	/**
	 * \return		Le status courant, à savoir le status de la dernière
	 *				commande exécutée ou en cours d'exécution.
	 * \see			getCommandName
	 */
	 virtual CommandIfc::status getStatus ( ) const;

	/**
	 * \return		Le nom de la commande courante, à savoir le nom de la
	 *				dernière commande exécutée ou en cours d'exécution.
	 * \see			getStatus
	 */
	virtual std::string getCommandName ( ) const;

	/**
	 * \return true s'il existe une commande en erreur
	 */
	virtual bool hasCommandInError ( ) const;

    //@}	// Méthodes relatives à la gestion des commandes

	/**
	 * Méthodes relatives à la gestion du <I>undo/redo</I>.
	 */
	//@{

	/**
	 * Le gestionnaire de <I>undo/redo</I>.
	 */
	virtual const UndoRedoManagerIfc& getUndoManager ( ) const;
	virtual UndoRedoManagerIfc& getUndoManager ( );
	virtual void setUndoManager (UndoRedoManagerIfc* mgr);

	//@}	// Méthodes relatives à la gestion du <I>undo/redo</I>.

	/**
	 * Méthodes relatives à l'affichage d'informations relatives à l'instance
	 * dans des flux.
	 */
	//@{

	/**
	 * Le flux recevant les demandes d'affichage. Ce flux est transmis aux
	 * éventuelles instances de la classe <I>CommandRunner</I> créés en
	 * vues d'exécuter des commandes dans des threads différents, au
	 * gestionnaire de <I>undo/redo</I> et aux commandes gérées.
	 */
	virtual void setLogStream (TkUtil::LogOutputStream* stream);

	/**
	 * \return		L'éventuel afficheur utilisé.
	 */
	virtual const TkUtil::LogOutputStream* getLogStream ( ) const
	{ return _logStream; }
	virtual TkUtil::LogOutputStream* getLogStream ( )
	{ return _logStream; }

	//@}	// Méthodes relatives à l'affichage d'informations

	/**
	 * Méthodes relatives à la gestion des évènements survenus aux commandes
	 * gérées.
	 */
	//@{

	/**
	 * Appelé lorsqu'une commande est modifiée. Actualise la liste des lanceurs
	 * de taches si nécessaire.
	 */
	virtual void observableModified (
						TkUtil::ReferencedObject* object, unsigned long event);

	/**
	 * Appelé lorsqu'une commande est détruite. Déréférence cette commande.
	 */
	virtual void observableDeleted (TkUtil::ReferencedObject* object);

	/**
	 * Ajoute l'observateur transmis en argument à chaque commande qui sera
	 * confiée au gestionnaire (via <I>addCommand</I>). Utile par exemple pour
	 * une IHM afin d'être notifiée du devenir des commandes confiées et
	 * exécutées par ce gestionnaire.
	 * \warning		L'argument <I>blockDestruction</I> tranmis lors de
	 *				<I>command->registerObservable</I> vaut <I>false</I>.
	 */
	virtual void addCommandObserver (TkUtil::ObjectBase* observer);

	//@}	// Méthodes relatives à la gestion des évènements

	/**
	 *  Déseffectue la dernière commande effectuée
	 *
	 *  Il faut passer par le contexte pour l'utiliser.
	 * Invoque <I>addCommand</I> avec la dernière commande et
	 * <I>CommandIfc::UNDO</I>.
	 */
	virtual void undo();

    /**
     *  Rejoue la dernière commande défaite
     *
     *  Il faut passer par le contexte pour l'utiliser
	 * 
	 *  Il faut passer par le contexte pour l'utiliser.
	 * Invoque <I>addCommand</I> avec la dernière commande et
	 * <I>CommandIfc::REDO</I>.
     */
	virtual void redo();


	protected :

	/**
	 * <P>Execute de manière séquentielle la commande transmise en argument 
	 * selon la manière transmise en second argument.
	 * </P>
	 * <P>Cette méthode s'assure via mutex qu'aucune autre commande ne peut être
	 * jouée pendant son exécution.
	 * </P>
	 * \param		Commande à exécuter
	 * \param		Manière d'exécution
	 * \except		Si une commande est déjà en cours d'exécution séquentielle.
	 * \see			executeThreaded
	 */
	virtual void executeSequential (CommandIfc* command, Command::PLAY_TYPE pt);

	/**
	 * Execute de dans un thread dédié la commande transmise en argument selon
	 * la manière transmise en second argument.
	 * \param		Commande à exécuter
	 * \param		Manière d'exécution
	 * \except		Une exception est levée si une commande est en cours
	 *				d'exécution séquentielle.
	 * \see			executeSequential
	 */
	virtual void executeThreaded (CommandIfc* command, Command::PLAY_TYPE pt);

	/**
	 * Lance de manière séquentielle la commande la plus ancienne de la file
	 * d'attente si les conditions le permettent.
	 * \see			executeSequential
	 */
	virtual void runQueuedCommand ( );

	/**
	 * Met la commande transmise en file d'attente. Si <I>pt</I>
	 * vaut <I>CommandIfc::QUEUED</I> alors la commande ets mise en file
	 * d'attente avec pour valeur <I>CommandIfc::DO</I>.
	 */
	virtual void addToQueue (CommandIfc* command, Command::PLAY_TYPE pt);

	/**
	 * Envoit le <I>log</I> transmis en argument dans le flux de messages
	 * associé a l'instance.
	 * \see		TkUtil::Log
	 */
	virtual void log (const TkUtil::Log& log);


	private :

	/**
	 * Constructeur de copie, opérateur = : interdits.
	 */
	CommandManager (const CommandManager&);
	CommandManager& operator = (const CommandManager&);

	/** Les commandes en cours d'exécution. */
	std::vector <CommandRunner*>		_runners;

	/** Un mutex non récursif pour protéger la liste des commandes en cours
	 * d'exécution. */
	mutable TkUtil::Mutex				_runnersMutex;

	/** Les observateurs de commandes. */
	std::vector <TkUtil::ObjectBase*>	_commandObservers;

	/** L'éventuel afficheur. */
	TkUtil::LogOutputStream*			_logStream;

	/** Gestionnaire (conteneur) pour undo/redo */
	Utils::UndoRedoManagerIfc*        	 _undoManager;

	/** Les commandes prises en charges. On n'utilise pas <I>getObservables</I>
	 * hérité de <I>TkUtil::ObjectBase</I> car cette liste est triée (par
	 * adresse mémoire, type <I>std::map</I>).
	 * Or on a besoin d'une liste triée par ordre d'enregistrement des commandes
	 * au moins au niveau du <I>QtCommandMonitorPanel</I> ...
	 *
	 * NB [EB]: on y trouve les commandes jouées y compris celles en erreur.
	 */
	std::vector<Command*>				_commands;

	/** Les éventuelles commandes en file d'attente. Les dernières soumises sont
	 * ajoutées à la fin. Elle sont dépilées par le début => FIFO. */
	std::deque< std::pair <Command*, CommandIfc::PLAY_TYPE> >	_queuedCommands;

	/** La politique d'exécution des commandes. */
	CommandManagerIfc::POLICY			_policy;

	/** Un mutex non récursif pour gérer les commandes en file d'attente. */
	mutable TkUtil::Mutex				_queuingMutex;

	/** Un mutex non récursif pour gérer les commandes en cours d'exécution
	 *  sequentielle. */
	mutable TkUtil::Mutex				_sequentialMutex;

	/** L'éventuelle commande exécutée séquentiellement. */
	CommandIfc*							_sequentialCommand;

	/** Le dernier status de commande reçu. */
	CommandIfc::status					_currentStatus;

	/** Le nom de la dernière commande dont on a eu une modification de
	 * status. */
	std::string							_currentCommandName;
};
/*----------------------------------------------------------------------------*/

} // end namespace Utils
/*----------------------------------------------------------------------------*/

} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* UTILS_COMMAND_MANAGER_H_ */
/*----------------------------------------------------------------------------*/
