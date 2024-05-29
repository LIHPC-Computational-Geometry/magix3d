/*----------------------------------------------------------------------------*/
/*
 * \file CommandNewLongCommand.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 /*  \date 7/1/2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/CommandNewLongCommand.h"
#include "Utils/Common.h"
#include "Internal/InfoCommand.h"

#include <TkUtil/Exception.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/ReferencedMutex.h>

#include <stdexcept>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
CommandNewLongCommand::CommandNewLongCommand(
										Internal::Context& c, std::string name)
: Internal::CommandInternal (c, name)
{

}
/*----------------------------------------------------------------------------*/
CommandNewLongCommand::~CommandNewLongCommand()
{

}
/*----------------------------------------------------------------------------*/
void CommandNewLongCommand::
internalExecute ( )
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewLongCommand::execute pour la commande " << getName ( )
                            << " de nom unique " << getUniqueName ( );

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));

    // exécution
	for (uint i = 0; i<100; i++)
	{
		double a=0.0;
		for (int j=0; j<2*10000000; j++)
			a+=1.0;
//		std::cout<<"Longue Commande, "<<i<<" / 100"<<std::endl;
		double prog = (double)i;
		prog /= 100.0;
		setProgression(prog);

		// On donne l'opportunité d'arrêter "proprement" l'exécution :
		if ((Command::CANCELED == getStatus ( )) &&
			(true == cancelable ( )))
			break;
		// Interruption possible via API pthread :
		pthread_testcancel ( );

//		if (50 == i)
//			throw TkUtil::Exception ("Erreur dans l'algo.");
	}	// for (uint i = 0; i<100; i++)
    std::cout<<"Longue Commande, terminée"<<std::endl;
}
/*----------------------------------------------------------------------------*/
void CommandNewLongCommand::internalUndo()
{
}
/*----------------------------------------------------------------------------*/
void CommandNewLongCommand::internalRedo()
{
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
