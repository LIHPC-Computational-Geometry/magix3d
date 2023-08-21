/*----------------------------------------------------------------------------*/
/*
 * \file CommandChangeLandmark.cpp
 *
 *  \author Charle Pignerol
 *
 *  \date 1 sept. 2016
 */
/*----------------------------------------------------------------------------*/
#include "Internal/CommandChangeLandmark.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
CommandChangeLandmark::
CommandChangeLandmark(Internal::Context& context, Utils::Landmark::kind l)
: Internal::CommandInternal (context, "Changement de repère")
, m_new_landmark(l)
, m_old_landmark(context.m_landmark)
{
	getInfoCommand ( ).addContextInfo (InfoCommand::LANDMARKMODIFIED);
#ifdef _DEBUG2
	std::cout<<"CommandChangeLandmark de "
			<<Utils::Landmark::toString(m_old_landmark)
	        <<" vers "<<Utils::Landmark::toString(m_new_landmark)
	        <<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
CommandChangeLandmark::CommandChangeLandmark (const CommandChangeLandmark& ccl)
//	: Internal::CommandInternal (ccl.getContext ( ), "Changement de repère")
	: Internal::CommandInternal (ccl)
, m_new_landmark(ccl.m_new_landmark)
, m_old_landmark(ccl.m_old_landmark)
{
}
/*----------------------------------------------------------------------------*/
CommandChangeLandmark& CommandChangeLandmark::operator = (const CommandChangeLandmark& ccl)
{
	Internal::CommandInternal::operator = (ccl);
	m_new_landmark	= ccl.m_new_landmark;
	m_old_landmark	= ccl.m_old_landmark;
	return *this;
}
/*----------------------------------------------------------------------------*/
CommandChangeLandmark::
~CommandChangeLandmark()
{
}
/*----------------------------------------------------------------------------*/
void CommandChangeLandmark::
internalExecute()
{
#ifdef _DEBUG2
	std::cout<<"CommandChangeLandmark::internalExecute => "
	        <<Internal::ContextIfc::toString(m_new_landmark)
	        <<std::endl;
#endif
std::cout << __FILE__ << ' ' << __LINE__ << " CONTEXT : " << Utils::Landmark::toString (getContext().m_landmark) << " -> " << Utils::Landmark::toString (m_new_landmark) << std::endl;
	getContext().m_landmark = m_new_landmark;
}
/*----------------------------------------------------------------------------*/
void CommandChangeLandmark::
internalUndo()
{
std::cout << __FILE__ << ' ' << __LINE__ << " CONTEXT : " << Utils::Landmark::toString (getContext().m_landmark) << " -> " << Utils::Landmark::toString (m_old_landmark) << std::endl;
	getContext().m_landmark = m_old_landmark;
}
/*----------------------------------------------------------------------------*/
void CommandChangeLandmark::
internalRedo()
{
std::cout << __FILE__ << ' ' << __LINE__ << " CONTEXT : " << Utils::Landmark::toString (getContext().m_landmark) << " -> " << Utils::Landmark::toString (m_new_landmark) << std::endl;
	getContext().m_landmark = m_new_landmark;
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
