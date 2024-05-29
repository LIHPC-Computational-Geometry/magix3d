#include "Utils/MutexUnlocker.h"


namespace TkUtil
{

MutexUnlocker::MutexUnlocker (Mutex* mutex)
	: _mutex (mutex)
{
}	// MutexUnlocker::MutexUnlocker


MutexUnlocker::MutexUnlocker (const MutexUnlocker&)
	: _mutex (0)
{
}	// MutexUnlocker::MutexUnlocker


MutexUnlocker& MutexUnlocker::operator = (const MutexUnlocker&)
{
	return *this;
}	// MutexUnlocker::operator =


MutexUnlocker::~MutexUnlocker ( )
{
	if (0 != _mutex)
		_mutex->unlock ( );
}	// MutexUnlocker::~MutexUnlocker


void MutexUnlocker::setMutex (Mutex* mutex)
{
	_mutex	= mutex;
}	// MutexUnlocker::setMutex

}	// namespace TkUtil
