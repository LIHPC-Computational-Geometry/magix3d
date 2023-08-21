/*
 * \file	MultiTaskedCommand.cpp
 *
 * \author	Charles PIGNEROL
 *
 * \date	23/03/201710
 */
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Utils/Magix3DEvents.h"
#include "Internal/MultiTaskedCommand.h"

#include <TkUtil/MemoryError.h>

#include <sched.h>	// sched_yield


using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace TkUtil;
using namespace std;


static bool operator < (const struct timeval& l, const struct timeval& r)
{
	if (l.tv_sec < r.tv_sec)
		return true;
	if ((l.tv_sec == r.tv_sec) && (l.tv_usec < r.tv_usec))
		return true;

	return false;
}	// operator <

static bool operator > (const struct timeval& l, const struct timeval& r)
{
	return !(l < r);
}	// operator >


namespace Mgx3D
{

namespace Internal
{

// ============================================================================
//                         LA CLASSE MultiTaskedCommand
// ============================================================================

MultiTaskedCommand::MultiTaskedCommand (
							Context& c, const string& name, size_t tasksNum)
	: Internal::CommandInternal (c, name),
	  _step (1), _stepNum (1), _stepName (name), _stepProgression (0.),
	  _tasks ( ),
	  _notificationTime ( ), _modificationTime ( ), _tasksMutex (new Mutex ( ))
{
	gettimeofday (&_modificationTime, NULL);
	gettimeofday (&_notificationTime, NULL);
}	// MultiTaskedCommand::MultiTaskedCommand


MultiTaskedCommand::MultiTaskedCommand (const MultiTaskedCommand& cmd)
	: CommandInternal (cmd),
	  _step (1), _stepNum (1), _stepName (""), _stepProgression (0.), _tasks( ),
	  _notificationTime ( ), _modificationTime ( ), _tasksMutex (0)
{
    MGX_FORBIDDEN("MultiTaskedCommand::MultiTaskedCommand is not allowed.");
}	// MultiTaskedCommand::MultiTaskedCommand


MultiTaskedCommand& MultiTaskedCommand::operator = (const MultiTaskedCommand&)
{
    MGX_FORBIDDEN("MultiTaskedCommand::operator = is not allowed.");
    return *this;
}	// MultiTaskedCommand::operator =


Mutex* MultiTaskedCommand::getTasksMutex ( )
{
	return _tasksMutex;
}	// MultiTaskedCommand::getTasksMutex


MultiTaskedCommand::~MultiTaskedCommand ( )
{
	notifyObserversForDestruction ( );
	unregisterReferences ( );

	clearTasks ( );

	delete _tasksMutex;		_tasksMutex	= 0;
}	// MultiTaskedCommand::~MultiTaskedCommand


double MultiTaskedCommand::getProgression ( ) const
{	// On part ici du principe que pour chaque étape de la commande on va de 0 à
	// 100pc, et que chaque tache d'une étape prend le même temps.
	// Une autre manière de faire, qui requiert une bonne prédictibilité
	// de la progression, serait que la progression de la commande aille de 0 à
	// 100pc, chaque étape couvrant un pourcentage connu de la commande.
	// Méthode à surcharger pour avoir cette finesse. Elle peut utiliser
	// les méthodes setStepProgression/getStepProgression prévue à cet effet.

	return getStepProgression(); // [EB]
}	// MultiTaskedCommand::getProgression


TkUtil::UTF8String MultiTaskedCommand::getStrProgression ( ) const
{
	const Command::status	status		= getStatus ( );
	const double			progress	= getProgression ( );
	const size_t			stepNum		= getStepNum ( );
	TkUtil::UTF8String	str (TkUtil::Charset::UTF_8);
	str << getName ( );
	switch (status)
	{
		case Command::INITED		: str << " (initialisée)";		break;
		case Command::STARTING		: str << " (démarrée)";			break;
		case Command::PROCESSING	:
			if (1 < stepNum)
			{
				str << " (en cours, phase " << (unsigned long)getStep ( ) << "/"
				    << (unsigned long)stepNum << " - " << getStepName ( ) << " "
				    << ios_base::fixed << TkUtil::setprecision (2)
				    << TkUtil::setw (5)
				    << (100. * getStepProgression())
				    << "%)";
			}	// if (1 < stepNum)
			else
			{
				str << " (en cours : " << ios_base::fixed
				    << TkUtil::setprecision (2) << TkUtil::setw (5)
				    << (100. * progress) << "%)";
			}	// else if (1 < stepNum)
			break;
		case Command::DONE			: str << " (achevée, succès)";	break;
		case Command::FAIL			: str << " (achevée, erreur)";	break;
		case Command::CANCELED		: str << " (annulée)";			break;
	}	// Command::getStrProgression 

	return str;
}	// MultiTaskedCommand::getStrProgression


size_t MultiTaskedCommand::getStep ( ) const
{
	return _step;
}	// MultiTaskedCommand::getStep


size_t MultiTaskedCommand::getStepNum ( ) const
{
	return _stepNum;
}	// MultiTaskedCommand::getStepNum


string MultiTaskedCommand::getStepName ( ) const
{
	return _stepName;
}	// MultiTaskedCommand::getStepName


double MultiTaskedCommand::getStepProgression ( ) const
{
	return _stepProgression;
}	// MultiTaskedCommand::getStepProgression


void MultiTaskedCommand::taskCompleted ( )
{
	size_t			completed	= 0, running	= 0, queued	= 0;
	stats (completed, running, queued);

	const size_t	total		= completed + running + queued;
	const double	progression	= 0 == total ?
								  0. : (double)completed / (double)total;
	setStepProgression (progression);
	markModified ( );
}	// MultiTaskedCommand::taskCompleted


void MultiTaskedCommand::notifyObserversForModifications ( )
{
	try
	{
		if (_notificationTime < _modificationTime)
		{
			notifyObserversForModification (COMMAND_PROGRESSION);
			_notificationTime	= _modificationTime;
		}	// if (_notificationTime < _modificationTime)
	}
	catch (const Exception& exc)
	{
		cerr << "ERREUR lors de la notification aux observateurs d'une "
		     << "modification de la commande " << getName ( ) << " :\n"
		     << exc.getFullMessage ( ) << endl;
	}
	catch (...)
	{
		cerr << "ERREUR non documentée lors de la notification aux "
		     << "observateurs d'une modification de la commande "
		     << getName ( ) << "." << endl;
	}
}	// MultiTaskedCommand::notifyObserversForModifications


//bool MultiTaskedCommand::threadingEnabled ( ) const
//{	// ATTENTION : pour pouvoir effectuer des taches élémentaires d'une commande
//	// dans différents threads il faut que cette commande soit elle même lancée
//	// dans un thread (autre que le thread principal == graphique).
//	//std::cout<<"MultiTaskedCommand::threadingEnabled avec threadId = "<<ContextIfc::threadId<<" et pthread_self = "<<pthread_self ( )<<std::endl;
//	return (true==Internal::ContextIfc::allowThreadedCommandTasks.getValue( ))&&
//			(ContextIfc::threadId != pthread_self ( )) ?
//		true : false;
//}	// MultiTaskedCommand::threadingEnabled

bool MultiTaskedCommand::threadingEnabled ( ) const
{	// EB: version qui accepte d'être dans le thread principal
	return (true==getContext ( ).allowThreadedCommandTasks.getValue( ));
}	// MultiTaskedCommand::threadingEnabled

void MultiTaskedCommand::markModified ( )
{
	AutoMutex	autoMutex (getTasksMutex ( ));

	gettimeofday (&_modificationTime, NULL);
}	// MultiTaskedCommand::markModified


void MultiTaskedCommand::setStep (
				size_t step, const string& stepName, double stepProgression)
{
	AutoMutex	autoMutex (getTasksMutex ( ));

	if (step > getStepNum ( ))
		throw Exception (UTF8String ("Numéro d'étape supérieur au nombre d'étapes.", TkUtil::Charset::UTF_8));

	const bool	notify	= step == _step ? false : true;
	_stepName			= stepName;
	_step				= step;
	_stepProgression	= stepProgression;
	if (true == notify)
		markModified ( );
}	// MultiTaskedCommand::setStep


void MultiTaskedCommand::setStepNum (size_t num)
{	// CP : appel hors contexte concurrentiel
	if (1 > num)
		throw Exception (UTF8String ("Nombre d'étapes inférieur à 1.", TkUtil::Charset::UTF_8));

	_stepNum	= num;
}	// MultiTaskedCommand::setStepNum


void MultiTaskedCommand::setStepProgression (double progression)
{
	if ((progression < 0.) || (progression > 1.)){
		TkUtil::UTF8String	errors (TkUtil::Charset::UTF_8);
		errors << "Progression hors domaine ([0..1]) : "<<progression;
		throw Exception (errors);
	}

	AutoMutex	autoMutex (getTasksMutex ( ));

	_stepProgression	= progression;
	markModified ( );
}	// MultiTaskedCommand::setStepProgression


void MultiTaskedCommand::stats (
				size_t& completed, size_t& running, size_t& queued) const
{
	ThreadPool::instance ( ).stats (running, queued);
	completed	= _tasks.size ( ) - running - queued;
}	// MultiTaskedCommand::stats


void MultiTaskedCommand::addTask (MgxThreadedTask* task)
{
	CHECK_NULL_PTR_ERROR (task)
	AutoMutex	autoMutex (getCommandMutex ( ));

	_tasks.push_back (task);
	ThreadPool::instance ( ).addTask (*task);
	markModified ( );
}	// MultiTaskedCommand::addTask


void MultiTaskedCommand::addTasks (
					const vector<Mgx3D::Utils::MgxThreadedTask*>& tasks)
{
	AutoMutex	autoMutex (getCommandMutex ( ));

	_tasks.insert (_tasks.end ( ), tasks.begin ( ), tasks.end ( ));
	vector<ThreadPool::TaskIfc*>	t;
	t.reserve (tasks.size ( ));
	for (vector<Utils::MgxThreadedTask*>::const_iterator it = tasks.begin ( );
	     tasks.end ( ) != it; it++)
		t.push_back (*it);
	ThreadPool::instance ( ).addTasks (t);
	markModified ( );
}	// MultiTaskedCommand::addTasks


void MultiTaskedCommand::waitTasksExecution ( )
{
	ThreadPool::instance ( ).barrier ( );
}	// MultiTaskedCommand::waitTasksExecution


void MultiTaskedCommand::evaluateTasksCompletion ( )
{
	TkUtil::UTF8String	errors (TkUtil::Charset::UTF_8);
	AutoMutex	autoMutex (getCommandMutex ( ));
	for (vector<MgxThreadedTask*>::const_iterator it = _tasks.begin ( );
	     _tasks.end ( ) != it; it++)
	{
		switch ((*it)->getStatus ( ))
		{
			case ThreadPool::TaskIfc::IN_ERROR	:
				if (false == errors.empty ( ))
					errors << "\n";
				errors << (*it)->getMessage ( );
		}	// switch ((*it)->getStatus ( ))
	}	// for (vector<MgxThreadedTask*>::const_iterator it = ...

	if (false == errors.empty ( ))
		throw Exception (errors);
}	// MultiTaskedCommand::evaluateTasksCompletion


void MultiTaskedCommand::clearTasks ( )
{
	MgxThreadedTask::deleteThreadedTasks (_tasks);
}	// MultiTaskedCommand::clearTasks


}	// namespace Internal

}	// namespace Mgx3D
