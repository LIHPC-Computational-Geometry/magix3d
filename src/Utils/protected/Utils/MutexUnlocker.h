/*----------------------------------------------------------------------------*/
/** \file		MutexUnlocker.h
 *  \author		Charles PIGNEROL
 *  \date		28/02/2013
 */
/*----------------------------------------------------------------------------*/

#ifndef MUTEX_UNLOCKER_H
#define MUTEX_UNLOCKER_H

#include <TkUtil/Mutex.h>


namespace TkUtil
{

/**
 * Classe permettant de s'assurer qu'un mutex sera deverrouillé lorsque le
 * bloc d'instruction courrant sera quitté.
 */
class MutexUnlocker
{
	public :

	/**
	 * \param		Eventuel mutex verrouillé dont on suohaite s'assurer du bon
	 *				deverrouillage.
	 */
	MutexUnlocker (TkUtil::Mutex* mutex);

	/**
	 * Deverrouille l'éventuel mutex associé.
	 */
	virtual ~MutexUnlocker ( );

	/**
	 * \param		Nouveau mutex pris en charge, ou 0. Ne deverrouille pas
	 *				l'éventuel mutex actuellement associé.
	 */
	void setMutex (TkUtil::Mutex* mutex);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	MutexUnlocker (const MutexUnlocker&);
	MutexUnlocker& operator = (const MutexUnlocker&);

	/** Eventuel mutex associé. */
	TkUtil::Mutex*			_mutex;
};	// class MutexUnlocker

}	// namespace TkUtil


#endif	// MUTEX_UNLOCKER_H

