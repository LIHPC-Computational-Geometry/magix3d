/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Internal/CommandCreator.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
CommandCreator::CommandCreator(const std::string& name, Internal::Context* c)
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
const Internal::Context& CommandCreator::getContext() const
{
    return *(m_context);
}
Internal::Context& CommandCreator::getContext()
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
