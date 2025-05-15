/*----------------------------------------------------------------------------*/
#ifndef UTIL_COMMAND_H_
#define UTIL_COMMAND_H_
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/LogOutputStream.h>
#include <TkUtil/Mutex.h>
#include <TkUtil/ReferencedObject.h>
#include <TkUtil/Timer.h>
/*----------------------------------------------------------------------------*/
#include <map>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Utils {

class CommandRunner;

/*----------------------------------------------------------------------------*/

/** \class Command
 * \brief	Définit le comportement de base de toute commande Magix3D. En l'occurrence une commande doit pouvoir être jouée, défaite, annulée,
 *			rejouée, et ce en environnement multithread.
 *
 *			<P>Cette classe hérite de la classe ReferencedNamedObject afin d'avoir des instances observables, notamment par une IHM. </P>
 *
 *			<P>Une commande est jouée par invocation de sa méthode <I>execute</I>. Cette exécution peut éventuellement être interrompue
 *			par appel de sa méthode <I>cancel</I>, sous réserves que l'exécution soit en cours et que la commande soit interruptible. En fin
 *			d'exécution, le status de la commande est <I>DONE</I> (terminaison avec succès) ou <I>FAIL</I> (terminaison en erreur). La commande
 *			peut alors être défaite (méthode <I>undo</I> puis éventuellement rejouée (méthode <I>redo</I>). A noter que par défaut <I>redo</I>
 *			appelle <I>execute</I> mais qu'il n'y a pas d'obligation en la matière.
 *			</P>
 *
 *			<P>Pour la sauvegarde des commandes jouées et déjouées, la procédure consiste à ce que la classe qui a crée la commande la stocke auprès
 *			du UndoRedoMamager approprié.
 *			</P>
 *
 * 			<P>Il n'appartient pas à cette classe de définir le comportement à adopter en environnement <I>multithread</I>, notamment en ce qui
 *			concerne les commandes interruptibles. Cette tâche revient en effet au gestionnaire d'exécution de commandes. En revanche l'ensemble des
 *			méthodes de cette classe supportent un fonctionnement dans une application multithread.
 * 			</P>
 */
/*----------------------------------------------------------------------------*/
class Command : public TkUtil::ReferencedNamedObject
{
	public:

	/*------------------------------------------------------------------------*/
	/** \brief  Status possibles d'une commande.
	 * <P>INITED : initialisé, en attente d'être exécutée,<BR>
	 *    STARTING : en cours de démarrage,<BR>
	 *    PROCESSING : en cours d'exécution,<BR>
	 *    DONE : exécution achevée sans erreur,<BR>
	 *    CANCELED : interrompue, dans un état stable,<BR>
	 *    FAIL : exécution achevée, en erreur,
	 * </P>
	 */
	enum status
	{
		INITED,
		STARTING,
		PROCESSING,
		DONE,
		CANCELED,
		FAIL
	};

	/**
	 * @return		Le mutex utilisé pour protéger les attributs spécifiques
	 *				à l'instance.
	 */
	virtual TkUtil::Mutex* getCommandMutex ( );
	virtual const TkUtil::Mutex* getCommandMutex ( ) const;

	/**
	 * \return	  Le nom de la commande telle qu'elle est vue de l'IHM.
	 */
	virtual const std::string& getName ( ) const;

	/**
	 * \return	  Le nom unique de la commande.
	 */
	virtual std::string getUniqueName ( ) const;

	/**
	 * \brief		créé un nom unique de commande.
	 */
	virtual std::string createUniqueName ( );

	/**
	 * Destructeur. Se déréférencie auprès des observateurs.
	 */
	virtual ~Command ( );

	/**
	 * Méthodes relatives à l'état de la commande.
	 */
	//@{
	/**
	 * Convertit le status en une chaîne de caractères.
	 */
	static TkUtil::UTF8String statusToString (Command::status);

	/**
	 * \return	le status de la commande.
	 * \see		getProgression
	 * \see		setStatus
	 * \see		getStrStatus
	 * \see		getPlayType
	 * \see		isCompleted
	 */
	 virtual status getStatus ( ) const
	{ return _status; }

	/**
	 * \return	le status de la commande, sous forme de chaîne de caractères.
	 * \see		getStatus
	 * \see		statusToString
	 * \see		getPlayType
	 */
	virtual TkUtil::UTF8String getStrStatus ( ) const;

	/**
	 * \return	<I>true</I> si la commande est achevée (succès ou non), <I>false</I> dans le cas contraire.
	 * \see		getStatus
	 */
	virtual bool isCompleted ( ) const
	{ return _isCompleted; }

	/**
	 * \return	L'état d'avancement de la commande, compris entre 0 et 1.
	 * \see		getStatus
	 * \see		setProgression
	 */
	virtual double getProgression ( ) const
	{ return _progress; }

	/**
	 * \return	L'état d'avancement de la commande, sous forme de chaîne de caractères.
	 * \see		getProgression
	 */
	virtual TkUtil::UTF8String getStrProgression ( ) const;

	/** Accès au message d'erreur
	 * \see getStatus
	 */
	virtual const TkUtil::UTF8String& getErrorMessage ( ) const
	{ return _errorMessage; }

	/**
	 * \return	<I>true</I> si l'utilisateur a été informé du résultat de la commande,
	 *			<I>false</I> dans le cas contraire.
	 * \see		setUserNotified
	 */
	virtual bool isUserNotified ( ) const;

	/**
	 * \return	<I>true</I> si l'utilisateur a été informé du résultat de la commande,
	 *			<I>false</I> dans le cas contraire.
	 * \see		setUserNotified
	 */
	virtual void setUserNotified (bool notified);
	//@}

    /*------------------------------------------------------------------------*/
	/**
	 * Méthodes relatives à l'exécution d'une commande.
	 */
	//@{
	/**
	 * La manière de jouer une commande :<BR>
	 * <OL>
	 * <LI><B>QUEUED</B> : en file d'attente (non jouée),
	 * <LI><B>DO</B> : première exécution de la commande,
	 * <LI><B>UNDO</B> : annulation de la commande,
	 * <LI><B>REDO</B> : rejeu de la commande,
	 * </OL>
	 * \see		getPlayType
	 */
	enum PLAY_TYPE { QUEUED, DO, UNDO, REDO };

	/**
	 * \return	La manière dont a été jouée la commande.
	 * \see		setPlayType
	 * \see		getStrPlayType
	 * \see		getStatus
	 */
	virtual PLAY_TYPE getPlayType ( ) const
	{ return _playType; }

	/**
	 * \return	La manière dont a été jouée la commande, sous forme de chaine de
	 *			caractères.
	 * \see		getPlayType
	 * \see		playTypeToString
	 * \see		getStatus
	 */
	virtual TkUtil::UTF8String getStrPlayType ( ) const;

	/**
	 * \brief	joue/défait/rejoue la commande, selon la valeur reçu en argument. Invoque préalablement setPlayType (playType).
	 * \see		execute
	 * \see		undo
	 * \see		redo
	 * \see		setPlayType
	 * \see		getPlayType
	 */
	virtual status execute (Command::PLAY_TYPE playType);

    /** \brief  execute la commande
	 *
	 * <P>Par défaut cette méthode lève une exception si déjà jouée. Dans le cas contraire affecte le status <I>PROCESSING</I> et lance le chronomètre.
	 * </P>
	 * <P>
	 * <B>Les classes dérivées doivent invoquer <I>atCompletion</I> en fin de traitement et en l'absence d'erreur</B>, par exemple juste après avoir 
	 * invoqué <I>setStatus (DONE)</I>. Elles peuvent utiliser la méthode <I>startingOrcompletionLog</I> pour envoyer dans le flux de logs des
	 * messages annonçant le début et la terminaison de la commande.
	 * </P>
	 * \warning	N'écrit rien dans le flux de logs.
	 * \see		cancel
	 * \see		atCompletion
	 * \see		startingOrcompletionLog
	 * \see		getTimer
     */
    virtual status execute ( );

	/*------------------------------------------------------------------------*/
	/** \brief  déjoue la commande
	 * <P>Si le status est <I>DONE</I>, réaffecte le status <I>INITED</I> et
	 * réinitialise le chronomètre. C'est à la classe spécialisée que revient
	 * alors de faire le traitement.
	 * Dans le cas contraire cette méthode lève une exception.
	 * </P>
	 * \warning	N'écrit rien dans le flux de logs.
	 * \see		execute
	 * \see		getTimer
	 * \see		getPlayType
	 */
	virtual status undo ( )=0;

	/*------------------------------------------------------------------------*/
	/** \brief  rejoue la commande
	 * Appelle <I>execute</I> par défaut.
	 * \warning	N'écrit rien dans le flux de logs.
	 * \see		getPlayType
	 */
	virtual status redo ( )=0;
	//@}

	/**
	 * Méthodes relatives au caractère "interruptible" des commandes.
	 * Une commande est dite <I>interruptible</I> si son exécution peut être stoppée de manière volontaire par l'utilisateur. Ce caractère
	 * interruptible prend tout son sens lorsque la commande est exécutée dans un thread dédié, permettant ainsi à l'utilisateur d'y mettre fin via
	 * une commande depuis le thread principal.
	 */
	//@{
	/**
	 * <P>Interrompt la commande en cours et passe de ce fait son status à <I>CANCELED</I>. Arrête le chronomètre.
	 * Ne fait rien si le status est DONE ou FAIL.
	 * </P>
	 * <P>A l'issue de cet appel l'état de la commande doit être stable si son status devient <I>CANCELED</I>. Cela veut dire que cette commande peut 
	 * être défaite (<I>undo</I>) ou exécutée à nouveau (<I>execute</I>). En cas d'interruption menant à un état instable le status doit alors être
	 * positionné à <I>FAIL</I>.
	 * </P>
	 * \see		cancelable
	 * \see		getTimer
	 */
	virtual void cancel ( );

	/**
	 * \return		<I>true</I> si la commande peut être interrompue par appel de la méthode <I>cancel</I>, <I>false</I> dans le cas
	 * 				contraire. Retourne <I>false</I> par défaut.
	 * @see			threadable
	 */
	virtual bool cancelable ( ) const
	{ return false; }

	/**
	 * \return		<I>true</I> si la commande a vocation à être lancée dans un autre <I>thread</I>, <I>false</I> dans le cas contraire.
	 * 				Retourne <I>false</I> par défaut.
	 * @see			cancelable
	 */
	virtual bool threadable ( ) const
	{ return false; }

	/**
	 * \return		Eventuelle instance de la classe <I>CommandRunner</I> exécutant cette commande dans un autre thread. Mécanisme
	 *				interne, à ne pas utiliser.
	 * \see		setCommandRunner
	 */
	virtual CommandRunner* getCommandRunner ( ) const;

	/**
	 * Instance de la classe <I>CommandRunner</I> exécutant cette commande dans un autre thread. Mécanisme interne, à ne pas utiliser.
	 * \see		getCommandRunner
	 */
	virtual void setCommandRunner (CommandRunner* runner);

	/**
	 * Appellé lorsque le thread est annulé. Doit libérer les ressources de manière thread safe.
	 */
	virtual void cancelledCleanup ( );

	/**
	 * Méthode potentiellement appelée par un gestionnaire multi-thread
	 * lorsqu'une tâche de la commande est achevée. Ne fait rien par défaut.
	 */
	virtual void taskCompleted ( );

	/**
	 * Méthode potentiellement appelée par un gestionnaire multi-thread pour
	 * demander l'éventuelle notification de modifications aux observateurs
	 * de la commande.
	 * @warning	Cette méthode doit être appelée depuis le thread principal
	 * 			de la commande en vue d'éviter des dead-locks.
	 */
	virtual void notifyObserversForModifications ( );
	//@}

	/**
	 * Méthodes relatives au temps nécessaire à l'exécution d'une commande.
	 */
	//@{
	/**
	 * \return		Le chronomètre de la commande.
	 */
	virtual const TkUtil::Timer& getTimer ( ) const
	{ return _timer; }
	virtual TkUtil::Timer& getTimer ( )
	{ return _timer; }
	//@}

	/**
	 * Méthodes relatives aux traitements à effectuer en fin de commande.
	 */
	//@{
	/**
	 * \return		Une chaîne de caractère représentant le type d'exécution
	 *				reçu en argument. Pour les messages destinés à
	 *				l'utilisateur.
	 */
	static TkUtil::UTF8String playTypeToString (PLAY_TYPE pt);

	/**
	 * Le temps estimé pour exécuter une commande. Peut permettre à un
	 * gestionnaire de prendre la décision de lancer séquentiellement une
	 * commande dont le temps d'exécution est instantané, ou de lancer dans un
	 * thread une commande interruptible dont le temps d'exécution est élevé.
	 * Par convention cette fonction retourne 0 pour une commande "instantanée",
	 * et (unsigned long)-1 pour une commande interruptible à lancer dans un
	 * thread dédié.
	 * \param		Type d'exécution visée, sachant qu'une annulation voire un
	 *				rejeu peuvent être quasi-instantanés, ce qui n'est pas
	 *				forcément le cas de (la première) exécution d'une commande.
	 * \return		Le temps estimé, en seconde, pour exécuter la commande.
	 */
	virtual unsigned long getEstimatedDuration (PLAY_TYPE playType = DO)
	{ return 0; }

	/**
	 * <P>
	 * Tâche éventuellement effectuée par la commande à un moment donné (exemple : en fin de commande, remise en service d'une partie des menus de l'IHM).
	 * </P>
	 */
	typedef void (*CommandTask)(void* clientData);

	/**
	 * Ajoute une tâche à effectuer en fin de commande.
	 * \see		atCompletion
	 */
	void addCompletionTask (CommandTask task, void* clientData);
	//@}


    /**
     * Méthodes relatives à l'affichage d'informations relatives à l'instance dans des flux (texte/ihm/...).
     */
    //@{
    /**
     * \param       Gestionnaire de message à utiliser pour afficher des informations sur le déroulement de la commande, ou 0.
     * \see         log
     * \see         getLogStream
     */
    virtual void setLogStream (TkUtil::LogOutputStream* stream);

    /**
     * \return      Le gestionnaire de message utilisé pour afficher des informations sur le déroulement de la commande, ou 0.
     *
     *              Attention, cette fonction est surchargée dans CommandInternal
     * \see         log
     * \see         setLogStream Internal::CommandInternal
     */
    virtual TkUtil::LogOutputStream* getLogStream ( )
    { return _logStream; }
    //@}


	/** Accesseur sur le commentaire pour le script
	 */
	TkUtil::UTF8String getScriptComments() const
	{return _scriptComments;}

	/** Modificateur du commentaire pour le script
	 */
	virtual void setScriptComments(TkUtil::UTF8String& cmt)
	{_scriptComments = cmt;}


	/** Accesseur sur la commande sous forme de script
     */
    virtual TkUtil::UTF8String getScriptCommand() const
	{return _scriptCommand;}

    /** Modificateur pour la commande sous forme de script
     */
    virtual void setScriptCommand(TkUtil::UTF8String& cmd)
	{_scriptCommand = cmd;}


    /** Sauvegarde les commentaires et la ligne de commande */
    virtual void saveScriptCommandAndComments();

	/** Accesseur sur la commande sous forme de script dans sa version initiale
     */
    virtual TkUtil::UTF8String getInitialScriptCommand() const;

	/** Accesseur sur le commentaire pour le script dans sa version initiale
     */
    virtual TkUtil::UTF8String getInitialScriptComments() const;


    /// Accesseur sur le fait de devoir tenir compte de la commande dans le script
    virtual bool isScriptable()
    {return _isScriptable;}

    /// Modificateur sur le fait de devoir tenir compte de la commande dans le script
    virtual void setScriptable(bool sc)
    {_isScriptable = sc;}

	protected :

	/**
	 * Constructeur par défaut (classe abstraite).
	 * \param		Nom de la commande. Il s'agit ici d'un nom pour l'IHM, qui n'a pas vocation à être unique. Le <I>nom unique</I> de
	 *				l'instance est créé automatiquement via un appel à <I>createUniqueName</I>.
	 * \see			createUniqueName
	 */
	Command (const std::string& name);

	/**
	 * Appelle observableModified (this) de tous les objets qu'il référence, en local et sur le réseau.
	 */
	virtual void notifyObserversForModification (unsigned long event);

	/**
	 * <P>Affecte le status de la commande.
	 * Modifie la valeur de progression et le chronomètre si <I>status</I> vaut <I>INITED</I> (progression nulle, chronomètre réinitialisé),
	 * <I>DONE</I> (progression égale à 1., chronomètre arrêté), ou <I>CANCELED</I> ou <I>FAIL</I> (chronomètre arrêté).<BR>
	 * Informe les observateurs du changement d'état (évènement COMMAND_STATE).
	 * >/P>
	 * \see		setProgress
	 */
	virtual void setStatus (Command::status status);

	/**
	 * Avertit les observateurs de la progression (évènement COMMAND_PROGRESSION).
	 * \param		Etat d'avancement. Doit être compris entre 0 et 1.
	 * \warning		N'appelle pas <I>setStatus</I>, même si <I>progress</I> vaut 1.
	 * \see			setStatus
	 */
	virtual void setProgression (double progress);

	/**
	 * \param		Manière dont est jouée la commande.
	 * \see			getPlayType
	 * \see			getStatus
	 * \see			execute (PLAY_TYPE)
	 * \warning		<B>notifyObserversForModification n'est pas invoqué.</B>
	 */
	virtual void setPlayType (PLAY_TYPE pt);

	/**
	 * <P>Effectue les tâches de terminaison.
	 * <B>A appeler par execute, dans les classes dérivées, en cas de succès</B>.
	 * </P>
	 * \see		addCompletionTask
	 */
	virtual void atCompletion ( );

	/**
	 * Envoit le <I>log</I> transmis en argument dans le flux de messages associé a l'instance, ou, à défaut, dans <I>cout</I>.
	 * \see		TkUtil::Log
	 * \see		startingOrcompletionLog
	 */
	virtual void log (const TkUtil::Log& log);

	/**
	 * Envoit dans le flux de log un log annonçant le début ou la fin de l'exécution de la commande.
	 * \see		<I>true</I> si c'est le début de la commande, <I>false</I> si c'est la fin.
	 */
	virtual void startingOrcompletionLog (bool beginning);

	/** Stockage du message d'erreur et trace dans le flux */
	virtual void setErrorMessage(const TkUtil::UTF8String& msg);

	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	Command (const Command&);
	Command& operator = (const Command&);

	/** Le status d'exécution de la commande. */
	status							_status;

	/** La commande est-elle achevée ? */
	bool							_isCompleted;

	/** Manière dont est jouée la commande. */
	PLAY_TYPE						_playType;

	/** L'avancement de la commande (de 0. à 1. inclus). */
	double							_progress;

	/** Le temps d'exécution de la commande. */
	TkUtil::Timer					_timer;

	/** Les tâches à exécuter en fin de commande. */
	std::map <CommandTask, void*>	_completionTasks;

	/** Eventuelle instance de la classe <I>CommandRunner</I> exécutant cette commande dans un autre thread. Mécanisme interne, à ne pas utiliser. */
	CommandRunner*					_commandRunner;

	/** Le gestionnaire de log utilisé pour afficher des messages sur le déroulement de la tâche. */
    TkUtil::LogOutputStream*        _logStream;

	/** Mutex pour les opération concurrentes de cette classe telle que la création d'un nom unique. */
	static TkUtil::Mutex			_mutex;

	/** Commentaire associé à la commande lors de son exécution.
	 * Ce qui tient compte du undo et redo
	 */
	TkUtil::UTF8String           _scriptComments;

	/** Commentaire initial associé à la commande lors de son exécution.
	 * Ce que l'on a avant undo et redo
	 */
	TkUtil::UTF8String           _savedScriptComments;

	/** String de construction de cette commande par un manager dans son context
	 * TODO [CP] peut-être à mettre sous forme de vector<string>
	 */
	TkUtil::UTF8String           _scriptCommand;

	/** String initiale de construction de cette commande par un manager dans son context
	 */
	TkUtil::UTF8String           _savedScriptCommand;

	/** Message retourné par la commande en cas d'erreur */
	TkUtil::UTF8String           _errorMessage;

	/** Vrai s'il y a lieu de mettre une trace dans le script */
	bool _isScriptable;

	/** Un mutex pour protéger les attributs de la commande. */
	mutable TkUtil::Mutex*				_commandMutex;

	/** L'utilisateur a t'il déjà été informé du résultat de la commande ? */
	mutable bool						_userNotified;
};

/*----------------------------------------------------------------------------*/
} // end namespace Utils

/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D

/*----------------------------------------------------------------------------*/

#endif /* UTIL_COMMAND_H_ */
/*----------------------------------------------------------------------------*/
