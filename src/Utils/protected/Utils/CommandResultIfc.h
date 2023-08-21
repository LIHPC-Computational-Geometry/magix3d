/*----------------------------------------------------------------------------*/
/** \file		CommandResultIfc.h
 *	\author		Team Magix3D
 *	\date		02/04/2013
 */
/*----------------------------------------------------------------------------*/

#ifndef UTILS_COMMAND_RESULT_IFC_H
#define UTILS_COMMAND_RESULT_IFC_H

#include "Utils/CommandIfc.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Utils
{


/** \class CommandResultIfc
 * \brief	Fournit l'accès aux résultats d'une commande qui a été exécutée.
 *			Doit être actualisé lorsque la commande est modifiée
 *			(ex : <I>undo</I>/<I>redo</I>, ou commande exécutée dans un autre
 *			<I>thread</I>.
 */
class CommandResultIfc
{
	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~CommandResultIfc ( );

	/**
	 * Informations générales sur la commande associée.
	 */
	//@{

	/**
	 * \return	Le nom de la commande tel qu'il est vu de l'IHM.
	 * \see		getUniqueName
	 */
	virtual std::string getName ( ) const = 0;

	/**
	 * \return	Le nom unique de la commande.
	 * \see		getName
	 */
	virtual std::string getUniqueName ( ) const = 0;

	/**
	 * \return	Le status de la commande associée.
	 * \except	Lève une exception en l'absence de commande (ex : détruite).
	 * \see		getStrStatus
	 */	
	virtual Mgx3D::Utils::CommandIfc::status getStatus ( ) const = 0;

	/**
	 * \return	Le status de la commande associée sous forme de chaîne de
	 *			caractères.
	 * \except	Lève une exception en l'absence de commande (ex : détruite).
	 * \see		getStatus
	 * \see		getErrorMessage
	 */	
	virtual TkUtil::UTF8String getStrStatus ( ) const = 0;

	/**
	 * \return	L'éventuel message d'erreur de la commande.
	 * \see		getStatus
	 * \see		getStrStatus
	 */
	virtual TkUtil::UTF8String getErrorMessage ( ) const = 0;

#ifndef SWIG
	/**
	 * \return	<I>true</I> si l'utilisateur a été informé du résultat de la commande,
	 *			<I>false</I> dans le cas contraire.
	 * \see		setUserNotified
	 */
	virtual bool isUserNotified ( ) const = 0;

	/**
	 * \return	<I>true</I> si l'utilisateur a été informé du résultat de la commande,
	 *			<I>false</I> dans le cas contraire.
	 * \see		setUserNotified
	 */
	virtual void setUserNotified (bool notified) = 0;
#endif	// SWIG

	//@}	// Informations générales sur la commande associée.


	protected :

	/**
	 * Constructeur.
	 */
	CommandResultIfc ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	CommandResultIfc (const CommandResultIfc&);
	CommandResultIfc& operator = (const CommandResultIfc&);
};	// class CommandResultIfc


/*----------------------------------------------------------------------------*/
}	// namespace Utils
/*----------------------------------------------------------------------------*/
}	// namespace Mgx3D
/*----------------------------------------------------------------------------*/



#endif	// UTILS_COMMAND_RESULT_IFC_H
/*----------------------------------------------------------------------------*/

