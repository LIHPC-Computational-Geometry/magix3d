/*----------------------------------------------------------------------------*/
/** \file		CommandException.h
 *  \author		Charles PIGNEROL
 *  \date		15/10/19
 */
/*----------------------------------------------------------------------------*/

#ifndef UTILS_COMMAND_EXCEPTION_H
#define UTILS_COMMAND_EXCEPTION_H

#include "Utils/CommandIfc.h"


namespace Mgx3D 
{

namespace Utils 
{
/**
 * Classe d'exception associée à l'exécution d'une commande. Permet l'accès à 
 * ladite commande.
 * \warning		<B>Ne pas utiliser la commande associée à l'exception après
 * 				destruction de la commande (pas de comptage de référence ou autre
 * 				système de déréférencement).</B>
 */
class CommandException : public TkUtil::Exception
{
	public :
	
	CommandException (const TkUtil::UTF8String& msg, Mgx3D::Utils::CommandIfc* cmd);
	CommandException (const CommandException& ce);
	CommandException (const Exception& e, Mgx3D::Utils::CommandIfc* cmd);
	CommandException& operator = (const CommandException& ce);
	virtual ~CommandException ( );
	const Mgx3D::Utils::CommandIfc* getCommand ( ) const;
	Mgx3D::Utils::CommandIfc* getCommand ( );
	
	
	private :
	
	Mgx3D::Utils::CommandIfc*	_command;
};	// class CommandException

}	// namespace Utils

}	// namespace Mgx3D

#endif	// UTILS_COMMAND_EXCEPTION_H
