/*----------------------------------------------------------------------------*/
/*
 * \file Time.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 12 juin 2017
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_UTILS_TIME_H_
#define MGX3D_UTILS_TIME_H_

#include <sys/time.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
/**
   @brief Gestion d'une heure
 */
class Time {
public:
	Time() : _modificationTime (new timeval ( ))
 	 {
		update();
 	 }

	Time(const Time& t): _modificationTime(new timeval ( ))
	{
		_modificationTime->tv_sec = t._modificationTime->tv_sec;
		_modificationTime->tv_usec = t._modificationTime->tv_usec;
	}

	~Time()
	{
		delete _modificationTime;
	}

	/// réactualise l'heure
	void update()
	{
		gettimeofday (_modificationTime, NULL);
	}

	/// comparaison
	bool operator == (const Time& r) const
	{
		return ((_modificationTime->tv_sec == r._modificationTime->tv_sec)
				&& (_modificationTime->tv_usec == r._modificationTime->tv_usec));
	}


	/// compare 2 heures
	bool operator < (const Time& r) const
	{
		if (_modificationTime->tv_sec < r._modificationTime->tv_sec)
			return true;
		if ((_modificationTime->tv_sec == r._modificationTime->tv_sec)
				&& (_modificationTime->tv_usec < r._modificationTime->tv_usec))
			return true;

		return false;
	}

	/// affecte un nouveau horaire
	void set (const Time& r) const
		{
			_modificationTime->tv_sec = r._modificationTime->tv_sec;
			_modificationTime->tv_usec = r._modificationTime->tv_usec;
		}

	/// affecte un nouveau horaire
	Time& operator = (const Time& r)
	{
		_modificationTime->tv_sec = r._modificationTime->tv_sec;
		_modificationTime->tv_usec = r._modificationTime->tv_usec;
		return *this;
	}

protected:

private:
	mutable struct timeval*	 _modificationTime;
};
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_UTILS_TIME_H_ */
