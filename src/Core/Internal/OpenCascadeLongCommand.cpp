/*----------------------------------------------------------------------------*/
/*
 * \file OpenCascadeLongCommand.cpp
 *
 * \author Charles PIGNEROL
 *
 *  \date	21/01/2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/OpenCascadeLongCommand.h"
#include "Utils/Common.h"

#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/TraceLog.h>


namespace Mgx3D
{

namespace Internal
{

OpenCascadeLongCommand::OpenCascadeLongCommand (Internal::Context& c, std::string name)
	: Internal::CommandInternal (c, name)
{

}


OpenCascadeLongCommand::~OpenCascadeLongCommand ( )
{

}


void OpenCascadeLongCommand::
internalExecute ( )
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "OpenCascadeLongCommand::execute pour la commande "
	        << getName ( ) << " de nom unique " << getUniqueName ( );

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

	uint	i = 0;
	while (true)
	{
		double a=0.0;
		for (int j=0; j<10000000; j++)
			a+=1.0;
		double prog = (double)i;
		prog /= 100.0;
		// On "émule" Open Cascade : boite noire incontrôlable
		// et qui n'envoit aucun message :
	}	// for (uint i = 0; i<100; i++)
}	// OpenCascadeLongCommand::internalExecute

void OpenCascadeLongCommand::
internalUndo()
{
}
void OpenCascadeLongCommand::
internalRedo()
{
}


} // end namespace Internal

} // end namespace Mgx3D
