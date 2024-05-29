/*----------------------------------------------------------------------------*/
/*
 * \file CommandCreator.cpp
 *
 *  \author Franck Ledoux, Eric Brière de l'Isle
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/CommandCreator.h"
#include "TkUtil/LogOutputStream.h"
#include <TkUtil/Exception.h>
#include "Internal/ContextIfc.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
CommandCreator::CommandCreator(const std::string& name, Internal::ContextIfc* c)
: m_name (name), m_context(c)
{
    if (c==0)
        throw TkUtil::Exception ("Context non initialisé pour CommandCreator");
}
/*----------------------------------------------------------------------------*/
CommandCreator::~CommandCreator()
{}
/*----------------------------------------------------------------------------*/
const std::string& CommandCreator::getName ( ) const
{
    return m_name;
}
/*----------------------------------------------------------------------------*/
Utils::CommandManagerIfc& CommandCreator::getCommandManager()
{
    return m_context->getCommandManager();
}
/*----------------------------------------------------------------------------*/
const Internal::ContextIfc& CommandCreator::getContext() const
{
    return *(m_context);
}
Internal::ContextIfc& CommandCreator::getContext()
{
    return *(m_context);
}
/*----------------------------------------------------------------------------*/
Internal::Context& CommandCreator::getLocalContext()
{
	Internal::Context*	context = dynamic_cast<Internal::Context*>(m_context);
	if (0 == context)
		throw TkUtil::Exception ("CommandCreator::getLocalContext : no local context.");

	return *context;
}
const Internal::Context& CommandCreator::getLocalContext() const
{
    Internal::Context*  context = dynamic_cast<Internal::Context*>(m_context);
    if (0 == context)
        throw TkUtil::Exception ("CommandCreator::getLocalContext : no local context.");

    return *context;
}
/*----------------------------------------------------------------------------*/
void CommandCreator::log (const TkUtil::Log& l)
{
    // AutoMutex   autoMutex (mutex ( ));

        m_context->getLogDispatcher().log (l);

}   // CommandCreator::log
/*----------------------------------------------------------------------------*/
TkUtil::LogOutputStream* CommandCreator::getLogStream ( )
{
    return (m_context?m_context->getLogStream():0);
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
