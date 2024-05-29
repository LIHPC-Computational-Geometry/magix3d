/*----------------------------------------------------------------------------*/
/** \file		CommandManagerIfc.h
 *  \author		Charles PIGNEROL
 *  \date		06/02/2012
 */
/*----------------------------------------------------------------------------*/

#ifndef UTILS_COMMAND_MANAGER_IFC_H_
#define UTILS_COMMAND_MANAGER_IFC_H_

#include "Utils/CommandIfc.h"

#include <TkUtil/ReferencedObject.h>

#include <string>
#include <vector>


/*----------------------------------------------------------------------------*/
namespace Mgx3D {


/*----------------------------------------------------------------------------*/
namespace Utils {

class UndoRedoManagerIfc;

/*----------------------------------------------------------------------------*/
/** \class CommandManagerIfc
 *  \brief Interface de gestionnaire de commandes Magix 3D.
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
class CommandManagerIfc
{
	public:

	/**
	 * Politique de fonctionnement du gestionnaire de commande :<BR>
	 * <UL>
	 * <LI><I>THREADED</I> : lancement possible de commandes dans des threads
	 * dédiés,
	 * <LI>SEQUENTIAL</I> : les tâches sont lancées séquentiellement. Si une
	 * commande est soumise alors qu'une autre est en cours d'exécution, cette
	 * nouvelle commande est mise en file d'attente. Les commandes en file
	 * d'attente sont lancées une à une, dans leur ordre de soumission, lorsque
	 * plus aucune commande n'est en cours d'exécution.
	 * </UL>
	 */
	enum POLICY {THREADED, SEQUENTIAL};

	/**
	 * Destructeur. RAS.	
	 */
	virtual ~CommandManagerIfc ( );

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
	 * \return	La politique d'exécution des commandes en cours.
	 * \see		setPolicy
	 */
	virtual POLICY getPolicy ( ) const;

	/**
	 * \param	Nouvelle politique d'exécution des commandes en cours.
	 * \return	L'ancienne politique d'exécution des commandes en cours.
	 * \see		getPolicy
	 */
	virtual POLICY setPolicy (POLICY policy);

	/**
	 * <P>Ajoute une commande à la liste des commandes gérées. Cette commande
	 * est complétement prise en charge par ce gestionnaire qui devient
	 * responsable de sa destruction.</P>
	 * </P>
	 * <P>Cette commande est ajoutée au gestionnnaire de <I>undo/redo</I> en cas
	 * de succès d'exécution, si le type d'exécution est <I>DO</I> et si elle
	 * n'est pas encore enregistrée auprès du gestionnaire.
	 * </P>
	 * \param	Commande prise en charge
	 * \param	Type d'exécution à effectuer sur la commande, ou
	 *			<I>Command::QUEUED</I> s'il faut juste recenser la commande.
	 * \warning	Une commande peut être enregistrée plusieurs fois, ce qui permet
	 *			de l'enregistrer pour une première exécution, puis pour une
	 *			annulation, puis pour un rejeu, ...
	 * \see		getCommands
	 * \see		hasRunningCommands
	 * \see		hasQueuedCommands
	 * \see		processQueuedCommands
	 */
	virtual void addCommand (Utils::CommandIfc* command, CommandIfc::PLAY_TYPE pt);

	/**
	 * Exécute si possible les commandes en file d'attente.
	 * \see		addCommand
	 * \see		hasRunningCommands
	 * \see		hasQueuedCommands
	 */
	virtual void processQueuedCommands ( );

	/**
	 * \return		La liste des commandes.
	 */
	virtual std::vector<Utils::CommandIfc*> getCommandIfcs ( ) const;

    /**
     *  Déseffectue la dernière commande effectuée dans son contexte.
     */
    virtual void undo ( );

    /**
     *  Rejoue la dernière commande défaite dans son contexte.
     */
    virtual void redo ( );

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

	//@}

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
	//@}

	/**
	 * Ajoute l'observateur transmis en argument à chaque commande qui sera
	 * confiée au gestionnaire (via <I>addCommand</I>). Utile par exemple pour
	 * une IHM afin d'être notifiée du devenir des commandes confiées et
	 * exécutées par ce gestionnaire.
	 * \warning		L'argument <I>blockDestruction</I> tranmis lors de
	 *				<I>command->registerObservable</I> vaut <I>false</I>.
	 */
	virtual void addCommandObserver (TkUtil::ObjectBase* observer);


	protected :

	/**
	 * Constructeur. RAS.
	 */
	CommandManagerIfc ( );


	private :

	/**
	 * Constructeur de copie, opérateur = : interdits.
	 */
	CommandManagerIfc (const CommandManagerIfc&);
	CommandManagerIfc& operator = (const CommandManagerIfc&);
};
/*----------------------------------------------------------------------------*/

} // end namespace Utils
/*----------------------------------------------------------------------------*/

} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* UTILS_COMMAND_MANAGER_IFC_H_ */
/*----------------------------------------------------------------------------*/
