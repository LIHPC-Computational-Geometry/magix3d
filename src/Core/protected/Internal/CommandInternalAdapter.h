/*----------------------------------------------------------------------------*/
/** \file		CommandInternalAdapter.h
 *  \author     Charles PIGNEROL (j'assume)
 *  \date       02/04/2013
 */
/*----------------------------------------------------------------------------*/

#ifndef COMMAND_INTERNAL_ADAPTER_H
#define COMMAND_INTERNAL_ADAPTER_H

#include "Utils/Command.h"


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Internal {


/**
 * Classe commande instanciable, à des fins internes.
 */
class CommandInternalAdapter : public Mgx3D::Utils::Command
{
    public :

    CommandInternalAdapter (const std::string& name);
	virtual ~CommandInternalAdapter ( );

	/**
	 * \return	CommandIfc::FAIL
	 */
	virtual Mgx3D::Utils::Command::status undo ( );
	virtual Mgx3D::Utils::Command::status redo ( );


	private :

	CommandInternalAdapter (const CommandInternalAdapter&);
	CommandInternalAdapter& operator = (const CommandInternalAdapter&);
};  // class InvalidCommandAdapter

/*----------------------------------------------------------------------------*/
}	// namespace Internal
/*----------------------------------------------------------------------------*/
}	// namespace Mgx3D
/*----------------------------------------------------------------------------*/



#endif	// COMMAND_INTERNAL_ADAPTER_H
/*----------------------------------------------------------------------------*/
