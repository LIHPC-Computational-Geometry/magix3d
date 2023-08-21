/*----------------------------------------------------------------------------*/
/** \file       MgxThreadedTaskManager.h
 *  \author     Charles PIGNEROL
 *  \date       21/03/2017
 *  \brief		Tâches élémentaires Magix 3D pouvant être exécutées en
 *				parallèle. Une tache élémentaire est ici une composante de
 *				commande.
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX_THREADED_TASK_MANAGER_H
#define MGX_THREADED_TASK_MANAGER_H

#include "Utils/CommandIfc.h"

#include <TkUtil/Mutex.h>
#include <TkUtil/ThreadPool.h>
#include <string>

namespace Mgx3D
{

namespace Utils
{

/**
 * Tâche de base Magix 3D (sous-élément de commande) exécutée en parallèle dans
 * différents threads.
 * Permet de récupérer des infos sur une tache joignable exécutée dans un autre
 * thread. Une commande est associée aux données, et permet par exemple de
 * savoir si la commande est annulée (via <I>isCanceled ( )</I>, et donc si la
 * tache doit s'interrompre.
 * \warning	<B>A ALLOUER DANS LE TAS car utilisé dans différents threads.</B>
 */
class MgxThreadedTask : public TkUtil::ThreadPool::TaskIfc
{
	public :

	/**
	 * \param	Commande associée.
	 */
	MgxThreadedTask (Utils::CommandIfc& command);

	/**
	 * Constructeur de copie, opérateur = : RAS.
	 */
	MgxThreadedTask (const MgxThreadedTask&);
	MgxThreadedTask& operator = (const MgxThreadedTask&);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~MgxThreadedTask ( );

	/**
	 * \return	L'éventuelle commande associée.
	 */
	virtual Utils::CommandIfc* getCommand ( );
	virtual const Utils::CommandIfc* getCommand ( ) const;

	/**
	 * \return	<I>true</I> si la commande associée est annulée, <I>false</I>
	 * 			dans le cas contraire.
	 */
	inline bool isCanceled ( ) const;

	/**
	 * \return	Message associé à l'exécution de la tache (erreur, ...)
	 * \see		setMessage
	 */
	virtual const std::string& getMessage ( ) const
	{ return _message; }

	/**
	 * \param	Message à associer à l'exécution de la tache (erreur, ...)
	 * \see		setMessage
	 */
	virtual void setMessage (const std::string& message);

	/**
	 * Détruit les instances de MgxThreadedTask puis vide le vecteur.
	 */
	static void deleteThreadedTasks (std::vector<MgxThreadedTask*>& tasks);


	private :

	TkUtil::Mutex						_mutex;
	Utils::CommandIfc*					_command;
	Mgx3D::Utils::CommandIfc::status	_status;
	std::string							_message;
};	// MgxThreadedTask


}	// namespace Utils

}	// namespace Mgx3D

#endif	// MGX_THREADED_TASK_MANAGER_H

