/*----------------------------------------------------------------------------*/
/** \file		CommandResult.h
 *	\author		Team Magix3D
 *	\date		02/04/2013
 */
/*----------------------------------------------------------------------------*/

#ifndef UTILS_COMMAND_RESULT_H
#define UTILS_COMMAND_RESULT_H

#include "Utils/CommandResultIfc.h"
#include "Utils/Command.h"
#include <TkUtil/ReferencedObject.h>


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Utils
{


/** \class CommandResult
 * <P>Implémentation de base d'un fourniseur d'accès aux résultats d'une
 * commande qui a été exécutée (<I>CommandResultIfc</I>). Est actualisé lorsque
 * la commande est modifiée (ex : <I>undo</I>/<I>redo</I>, ou commande exécutée
 * dans un autre <I>thread</I>.
 * </P>
 * <P>Hérite de <I>TkUtil::ObjectBase</I> afin d'être informé des modifications
 * de la commande, notamment de son éventuelle destruction.
 * </P>
 */
class CommandResult :
			public Mgx3D::Utils::CommandResultIfc, public TkUtil::ObjectBase
{
	public :

	/**
	 * Constructeur.
	 * \param	Commande représentée.
	 */
	CommandResult (Mgx3D::Utils::Command&);

	/**
	 * Constructeur.
	 * sans commande (cas volontairement en erreur)
	 */
	CommandResult ();

	/**
	 * Destructeur. Se déréférence auprès de la commande associée.
	 */
	virtual ~CommandResult ( );

	/**
	 * Informations générales sur la commande associée.
	 */
	//@{

	/**
	 * \return	Le nom de la commande tel qu'il est vu de l'IHM.
	 * \see		getUniqueName
	 */
	virtual std::string getName ( ) const;

	/**
	 * \return	Le nom unique de la commande.
	 * \see		getName
	 */
	virtual std::string getUniqueName ( ) const;

	/**
	 * \return	Le status de la commande associée.
	 * \except	Lève une exception en l'absence de commande (ex : détruite).
	 * \see		getStrStatus
	 */	
	virtual Mgx3D::Utils::CommandIfc::status getStatus ( ) const;

	/**
	 * \return	Le status de la commande associée sous forme de chaîne de
	 *			caractères.
	 * \except	Lève une exception en l'absence de commande (ex : détruite).
	 * \see		getStatus
	 */
	virtual TkUtil::UTF8String getStrStatus ( ) const;

	/**
	 * \return	L'éventuel message d'erreur de la commande.
	 * \see		getStatus
	 * \see		getStrStatus
	 */
	virtual TkUtil::UTF8String getErrorMessage ( ) const;
	
#ifndef SWIG
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
#endif	// SWIG

	//@}	// Informations générales sur la commande associée.

#ifndef SWIG

	/**
	 * Les callbacks sur les commandes associées.
	 */
	//@{

	/**
	 * Appelé lorsque la commande est modifiée.
	 */
	virtual void observableModified (TkUtil::ReferencedObject* object, unsigned long event);

	/**
	 * Appelé lorsque la commande est détruite. Déréférence cette commande.
	 */
	virtual void observableDeleted (TkUtil::ReferencedObject* object);

	//@}	// Les callbacks sur les commandes associées.

#endif	// SWIG


	protected :

	/**
	 * \return		La commande associée.
	 * \except		Une exception est levée si il n'y a pas de commande
	 *				associée (par exemple si elle est détruite).
	 */
	virtual const Mgx3D::Utils::Command& getCommand ( ) const;
	virtual Mgx3D::Utils::Command& getCommand ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	CommandResult (const CommandResult&);
	CommandResult& operator = (const CommandResult&);

	/** La commande associée. */
	Mgx3D::Utils::Command*			_command;
};	// class CommandResult


/*----------------------------------------------------------------------------*/
}	// namespace Utils
/*----------------------------------------------------------------------------*/
}	// namespace Mgx3D
/*----------------------------------------------------------------------------*/



#endif	// UTILS_COMMAND_RESULT_H
/*----------------------------------------------------------------------------*/

