/** \file MultiTaskedCommand.h
 *
 * \author	Charles PIGNEROL
 *
 * \date	23/03/2017
 */

#ifndef MULTI_TASKED_COMMAND_H
#define MULTI_TASKED_COMMAND_H

#include "Internal/CommandInternal.h"
#include "Utils/MgxThreadedTaskManager.h"

#include <sys/time.h>


namespace Mgx3D
{

namespace Internal
{

/**
 * @brief	Commande dont l'exécution peut se dérouler en plusieurs
 * 			<U>étapes</U>, chaque étape pouvant elle-même être décomposée en
 * 			plusieurs <U>taches</U> exécutées de manière concurrente (dans
 * 			différents threads).
 */
class MultiTaskedCommand : public Mgx3D::Internal::CommandInternal
{
	public :

    /** Destructeur. RAS. */
    virtual ~MultiTaskedCommand ( );

	/**
	 * @return	getStepProgression par défaut, à surcharger pour avoir un
	 * 			comportement plus fin.
	 */
	virtual double getProgression ( ) const;

	/** \return	L'étape courrante de la commande (par défaut 1, modifiable via
	 * 			setStep).
	 * \see		getStepNum
	 * \see		setStep
	 * \see		getStepName
	 * \see		getStepProgression
	 */
	virtual size_t getStep ( ) const;

	/**
 	 * @return	Le nombre d'étapes de la commande (par défaut 1, modifiable
 	 * 			via setStepNum).
 	 * @see		setStepNum
 	 */
	virtual size_t getStepNum ( ) const;

	/** \return	Le nom de l'étape courrante de la commande
	 * \see		getStep
	 * \see		setStep
	 * \see		getStepProgression
	 */
	virtual std::string getStepName ( ) const;

	/** \return	La progression (0 à 1.) dans l'étape courrante de la commande
	 * \see		getStep
	 * \see		setStep
	 * \see		getProgression
	 */
	virtual double getStepProgression ( ) const;

	/**
	 * Appelée par le gestionnaire associé lorsqu'une tache est achevée.
	 * Enregistre la date de modification.
	 */
	virtual void taskCompleted ( );

	/**
 	 * Informe les observateurs, si c'est le cas, que la commande est modifiée
 	 * depuis le dernier appel.
	 * @warning	A appeler depuis le thread principal de la commande pour
	 * 			éviter les deadlocks.
	 */
	virtual void notifyObserversForModifications ( );

	/**
	 * @return	<I>true</I> si cette commande peut réaliser des taches dans
	 * 			threads, <I>false</I> dans le cas contraire.
	 */
	virtual bool threadingEnabled ( ) const;


	protected :

    /** Constructeur par défaut
     * \param	Contexte d'utilisation
     * \param	Nom de la commande
     * \param	Nombre de taches qui seront au maximum lancées en concurrence
     * 			(0 : nombre de processeurs accessibles).
     */
    MultiTaskedCommand (Internal::Context& c, const std::string& name,
	                    size_t tasksNum);

    /** Constructeur par copie (interdit) */
    MultiTaskedCommand (const MultiTaskedCommand&);

    /** Opérateur de copie (interdit) */
    MultiTaskedCommand& operator = (const MultiTaskedCommand&);

	/**
	 * @return	Un mutex affecté au caractère "modifié" de la tache, utilisé
	 *			lors des appels des sous-taches type <I>setProgression</I>.
	 */
	virtual TkUtil::Mutex* getTasksMutex ( );

	/**
	 * Marque la tache comme quoi elle est modifiée. Les observateurs en seront
	 * notifiés dès que possible par le thread principal de la tache.
	 */
	virtual void markModified ( );

	/**
	 * @ return		L'état d'avancement de la commande, sous forme de chaîne
	 * 				de chaine de caractères.
	 */
	virtual TkUtil::UTF8String getStrProgression ( ) const;

	/** Nouvelle étape de la commande
	 * \param	Numéro de la nouvelle étape (commence à 1)
	 * \param	Nom de l'étape
	 * \param	Progression dans l'étape (a priori 0 mais ...)
	 * \see		getStep
	 * \see		getStepNum
	 * \see		getStepName
	 * \see		getStepProgression
	 * \see		setStepProgression
	 */
	virtual void setStep (size_t step, const std::string& stepName,
	                      double stepProgression);

	/**
 	 * @param	Le nombre d'étapes de la commande (>= 1)
 	 * @see		getStepNum
 	 */
	virtual void setStepNum (size_t num);

	/** Nouvelle progression dans l'étape courrante de la commande
	 * \param	Progression dans l'étape
	 */
	virtual void setStepProgression (double progression);

	/**
 	 * En retour des statistiques sur l'opération en cours.
 	 * \return	Le nombre de taches effectuées
 	 * \return	Le nombre de taches en cours de traitement
 	 * \return	Le nombre de taches en attente
	 */
	virtual void stats (size_t& completed, size_t& running, size_t& queued) const;

	/**
	 * Ajoute la tache transmise à celles à exécuter.
	 * \param	Tâche à exécuter. <B>ATTENTION</B> : cette tache est
	 * 			automatiquement détruite à la fin, contrairement aux données
	 * 			associées.
	 * \see		addTasks
	 * \see		waitTasksExecution
	 */
	virtual void addTask (Mgx3D::Utils::MgxThreadedTask* task);

	/**
	 * Ajoute les taches transmises à celles à exécuter.
	 * \param	Tâche à exécuter. <B>ATTENTION</B> : cette tache est
	 * 			automatiquement détruite à la fin, contrairement aux données
	 * 			associées.
	 * \see		addTask
	 * \see		waitTasksExecution
	 */
	virtual void addTasks (
					const std::vector<Mgx3D::Utils::MgxThreadedTask*>& tasks);

	/**
	 * Attends que toutes les taches lancées via <I>addTask</I> soient
	 * achevées.
	 * \see		addTask
	 */
	virtual void waitTasksExecution ( );

	/**
	 * Evalue (status) les données associées aux taches réalisées, et lève une
	 * exception en cas d'erreur.
	 */
	virtual void evaluateTasksCompletion ( );

	/**
	 * Détruit les taches en cours d'exécution.
 	 */
	virtual void clearTasks ( );


	private :

	/** L'étape courrante, le nombre d'étapes. */
	size_t												_step, _stepNum;

	/** Le nom de l'étape courrante. */
	std::string											_stepName;

	/** La progression dans l'étape courrante. */
	double												_stepProgression;

	/** Tâches en cours d'exécution.  */
	std::vector<Mgx3D::Utils::MgxThreadedTask*>			_tasks;

	/** Date de la dernière notification de modification aux observateurs.  */
	struct timeval										_notificationTime;

	/** Date de la dernière modification (tache en progression ou achevée, ...).
	 */
	struct timeval										_modificationTime;

	/** Un mutex pour gérer les accès concurrents (état modifié, progression)
 	 * des taches à cette instance. */
	TkUtil::Mutex*										_tasksMutex;
};  	// class MultiTaskedCommand

}	// end namespace Internal

}	// end namespace Mgx3D

#endif	// MULTI_TASKED_COMMAND_H
