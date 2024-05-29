/*----------------------------------------------------------------------------*/
/** \file		CommandInternalAdapter.cpp
 *  \author		Charles PIGNEROL (j'assume)
 *  \date		02/04/2013
 */
/*----------------------------------------------------------------------------*/

#include "Internal/CommandInternalAdapter.h"
#include "Utils/Common.h"


using namespace Mgx3D::Utils;
using namespace std;


namespace Mgx3D
{

namespace Internal
{

CommandInternalAdapter::CommandInternalAdapter (const string& name)
	: Command (name)
{
}	// CommandInternalAdapter::CommandInternalAdapter


CommandInternalAdapter::CommandInternalAdapter (const CommandInternalAdapter&)
	: Command ("invalid")
{
	MGX_FORBIDDEN("CommandInternalAdapter copy constructor is not allowed.");
}	// CommandInternalAdapter::CommandInternalAdapter


CommandInternalAdapter& CommandInternalAdapter::operator = (
										const CommandInternalAdapter&)
{
	MGX_FORBIDDEN("CommandInternalAdapter assignment operator is not allowed.");
	return *this;
}	// CommandInternalAdapter::operator =


CommandInternalAdapter::~CommandInternalAdapter ( )
{
	notifyObserversForDestruction ( );
	unregisterReferences ( );
}	// CommandInternalAdapter::~CommandInternalAdapter


Command::status CommandInternalAdapter::undo ( )
{
	return CommandIfc::FAIL;
}	// CommandInternalAdapter::undo


Command::status CommandInternalAdapter::redo ( )
{
	return CommandIfc::FAIL;
}	// CommandInternalAdapter::redo

}	// namespace Internal

}	// namespace Mgx3D
