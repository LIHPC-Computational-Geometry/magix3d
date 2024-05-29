/*----------------------------------------------------------------------------*/
/** \file       MgxThreadedTaskManager.cpp
 *  \author     Charles PIGNEROL
 *  \date       21/03/2017
 */
/*----------------------------------------------------------------------------*/

#include "Utils/MgxThreadedTaskManager.h"
#include "Utils/Common.h"

#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>

#include <string.h>		// strerror

using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace TkUtil;
using namespace std;

namespace Mgx3D
{

namespace Utils
{

// ============================================================================
//                            LA CLASSE MgxThreadedTask
// ============================================================================

MgxThreadedTask::MgxThreadedTask (CommandIfc& command)
	: ThreadPool::TaskIfc ( ),
	  _mutex (true), _command (&command), _status (CommandIfc::INITED),
	  _message ( )
{
}	// MgxThreadedTask::MgxThreadedTask


MgxThreadedTask::MgxThreadedTask (const MgxThreadedTask& bpmd)
	: ThreadPool::TaskIfc (bpmd),
	  _mutex (true), _command (bpmd._command), _status (bpmd._status),
	  _message (bpmd._message)
{
}	// MgxThreadedTask::MgxThreadedTask


MgxThreadedTask& MgxThreadedTask::operator = (const MgxThreadedTask& bpmd)
{
	if (&bpmd != this)
	{
		ThreadPool::TaskIfc::operator = (bpmd);
		_command		= bpmd._command;
		_status			= bpmd._status;
		_message		= bpmd._message;
	}	// if (&bpmd != this)

	return *this;
}	// MgxThreadedTask::operator =

MgxThreadedTask::~MgxThreadedTask ( )
{
}	// MgxThreadedTask::~MgxThreadedTask


CommandIfc* MgxThreadedTask::getCommand ( )
{
	return _command;
}	// MgxThreadedTask::getCommand


const CommandIfc* MgxThreadedTask::getCommand ( ) const
{
	return _command;
}	// MgxThreadedTask::getCommand


bool MgxThreadedTask::isCanceled ( ) const
{
	return CommandIfc::CANCELED == _command->getStatus( ) ? true : false;
}	// MgxThreadedTask::isCanceled


void MgxThreadedTask::setMessage (const string& message)
{
	AutoMutex autoMutex (&_mutex);
	_message	= message;
}	// MgxThreadedTask::setMessage


void MgxThreadedTask::deleteThreadedTasks (vector<MgxThreadedTask*>& tasks)
{
	for (vector<MgxThreadedTask*>::iterator it = tasks.begin ( );
	     tasks.end ( ) != it; it++)
		delete *it;
	tasks.clear ( );
}	// MgxThreadedTask::deleteThreadedTasks


}	// namespace Utils

}	// namespace Mgx3D
