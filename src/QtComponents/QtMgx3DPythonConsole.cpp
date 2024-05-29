#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DPythonConsole.h"
#include "QtComponents/Qt3DGraphicalWidget.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <TkUtil/Exception.h>
#include <TkUtil/ErrorLog.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/ProcessLog.h>
#include <TkUtil/TraceLog.h>

#include <assert.h>
#include <memory>

#include <QLayout>
#include <QLabel>
#include <QKeyEvent>

#include <QThread>
#include <QTextCodec>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;



#define USE_SEQUENTIAL_POLICY                                                \
if (0 != getMainWindow ( ))                                                  \
	cmdMgrPolicy	= getMainWindow ( )->getContext (                        \
				).getCommandManager( ).setPolicy(CommandManagerIfc::SEQUENTIAL);

#define RETURN_TO_PREVIOUS_POLICY                                            \
if (0 != getMainWindow ( ))                                                  \
{                                                                            \
	try                                                                      \
	{                                                                        \
		if ((CommandManagerIfc::POLICY)-1 != cmdMgrPolicy)                  \
			getMainWindow ( )->getContext (                                  \
						).getCommandManager( ).setPolicy(cmdMgrPolicy);     \
		cmdMgrPolicy	= (CommandManagerIfc::POLICY)-1;                     \
	}                                                                        \
	catch (...)                                                              \
	{                                                                        \
	}                                                                        \
}



namespace Mgx3D
{

namespace QtComponents
{


// ============================================================================
//                          LA CLASSE QtMgx3DPythonConsole
// ============================================================================


QtMgx3DPythonConsole::QtMgx3DPythonConsole (
			QWidget* parent, QtMgx3DMainWindow* mainWindow, const string& title)
	: QtPythonConsole (parent, title),
	  _mgxUserScriptingManager (0), _mainWindow (mainWindow),
	  _graphicalWidget (0), _cmdMgrPolicy ((CommandManagerIfc::POLICY)-1)
{
	hideResult ("proxy of <Swig Object of type");
}	// QtMgx3DPythonConsole::QtMgx3DPythonConsole


QtMgx3DPythonConsole::QtMgx3DPythonConsole (const QtMgx3DPythonConsole&)
	: QtPythonConsole (0, ""),
	  _mgxUserScriptingManager (0), _mainWindow (0), _graphicalWidget (0),
	  _cmdMgrPolicy ((CommandManagerIfc::POLICY)-1)
{
	assert (0 && "QtMgx3DPythonConsole copy constructor is forbidden.");
}	// QtMgx3DPythonConsole::QtMgx3DPythonConsole (const QtMgx3DPythonConsole&)


QtMgx3DPythonConsole& QtMgx3DPythonConsole::operator = (
													const QtMgx3DPythonConsole&)
{
	assert (0 && "QtMgx3DPythonConsole assignment operator is forbidden.");
	return *this;
}	// QtMgx3DPythonConsole::operator =


QtMgx3DPythonConsole::~QtMgx3DPythonConsole ( )
{
	_graphicalWidget	= 0;

	// Contrairement au scripting manager, le user scripting manager relève de
	// la responsabilité de ce panneau :
	delete _mgxUserScriptingManager;	_mgxUserScriptingManager	= 0;
}	// QtMgx3DPythonConsole::~QtMgx3DPythonConsole


void QtMgx3DPythonConsole::setRunningMode (QtPythonConsole::RUNNING_MODE mode)
{
#ifdef MULTITHREADED_APPLICATION
	AutoMutex	mutex (&getMutex ( ));
#endif	// MULTITHREADED_APPLICATION
	bool	modified	= true;
	if (getRunningMode ( ) == mode)
		modified	= false;

	if ((true == modified) && (QtPythonConsole::RM_DEBUG == mode))
		storePolicy ( );

	QtPythonConsole::setRunningMode (mode);

	bool	enableActions	= true;
	switch (getRunningMode ( ))
	{
		case QtPythonConsole::RM_DEBUG	: enableActions	= false;
			break;
		default						: enableActions	= true;
	}	// switch (getRunningMode ( ))

	if ((true == modified) && (QtPythonConsole::RM_DEBUG != mode))
		restorePolicy ( );

	if (0 != getMainWindow ( ))
		getMainWindow ( )->disableActions (!enableActions);
}	//  QtMgx3DPythonConsole::setRunningMode


void QtMgx3DPythonConsole::run ( )
{
#ifdef MULTITHREADED_APPLICATION
	AutoMutex	mutex (&getMutex ( ));
#endif	// MULTITHREADED_APPLICATION
	storePolicy ( );

	try
	{
		QtPythonConsole::run ( );
		if (QtPythonConsole::RM_DEBUG != getRunningMode ( ))
			restorePolicy ( );
	}
	catch (...)
	{
		if (QtPythonConsole::RM_DEBUG != getRunningMode ( ))
			restorePolicy ( );
		throw;
	}
}	// QtMgx3DPythonConsole::run


void QtMgx3DPythonConsole::stop ( )
{
#ifdef MULTITHREADED_APPLICATION
	AutoMutex	mutex (&getMutex ( ));
#endif	// MULTITHREADED_APPLICATION
	try
	{
		QtPythonConsole::stop ( );
	}
	catch (...)
	{
		throw;
	}
}	// QtMgx3DPythonConsole::stop


void QtMgx3DPythonConsole::cont ( )
{
#ifdef MULTITHREADED_APPLICATION
	AutoMutex	mutex (&getMutex ( ));
#endif	// MULTITHREADED_APPLICATION

	QtPythonConsole::cont ( );
}	// QtMgx3DPythonConsole::cont


void QtMgx3DPythonConsole::next ( )
{
#ifdef MULTITHREADED_APPLICATION
	AutoMutex	mutex (&getMutex ( ));
#endif	// MULTITHREADED_APPLICATION

	QtPythonConsole::next ( );
}	// QtMgx3DPythonConsole::next


void QtMgx3DPythonConsole::executeCommand (const string& command)
{
#ifdef MULTITHREADED_APPLICATION
	AutoMutex	mutex (&getMutex ( ));
#endif	// MULTITHREADED_APPLICATION

	storePolicy ( );

	try
	{
		QtPythonConsole::execInstruction (command, true);
		restorePolicy ( );
	}
	catch (...)
	{
		restorePolicy ( );
		throw;
	}
}	// QtMgx3DPythonConsole::executeCommand


#define LOG_SCRIPT_OUTPUTS_STREAM                                           \
	const UTF8String	output (session.getOutText ( ));                    \
	const UTF8String	errorText (session.getErrorText ( ));               \
	if (0 != logMgr)                                                        \
	{                                                                       \
		logMgr->log (ProcessLog (fileName, session.getOutText ( )));        \
		logMgr->log (ProcessLog (fileName, session.getErrorText ( )));      \
	}	// if (0 != logMgr)


void QtMgx3DPythonConsole::executeFile (const std::string& fileName)
{
#ifdef MULTITHREADED_APPLICATION
	AutoMutex	mutex (&getMutex ( ));
#endif	// MULTITHREADED_APPLICATION

	storePolicy ( );

	unique_ptr<RenderingManager::DisplayLocker>	displayLocker (
		0 == _graphicalWidget ?
		0 : new RenderingManager::DisplayLocker (
							_graphicalWidget->getRenderingManager ( )));

	CommandManagerIfc::POLICY	cmdMgrPolicy	= CommandManagerIfc::THREADED;
// Le PythonConsole reposant sur la QConsole ne supporte pas les instructions
// écrites sur plusieurs lignes ... => on passe par PythonSession :
	CHECK_NULL_PTR_ERROR (getMainWindow ( ))

	try
	{
		QtPythonConsole::execFile (fileName);

		restorePolicy ( );
	}
	catch (const Exception& exc)
	{
		restorePolicy ( );

		throw;	// => Ca n'est pas un succès !
	}
	catch (const exception& stdexc)
	{
		restorePolicy ( );

		throw;	// => Ca n'est pas un succès !
	}
	catch (...)
	{
		restorePolicy ( );

		throw;	// => Ca n'est pas un succès !
	}	// catch (...)

}	// QtMgx3DPythonConsole::executeFile


void QtMgx3DPythonConsole::addToHistoric (
	const UTF8String& command, const UTF8String& comments,
	const UTF8String& commandOutput, bool statusErr, bool fromKernel)
{
#ifdef MULTITHREADED_APPLICATION
	AutoMutex	mutex (&getMutex ( ));
#endif	// MULTITHREADED_APPLICATION

	QtPythonConsole::addToHistoric (
					command, comments, commandOutput, statusErr, fromKernel);
}	// QtMgx3DPythonConsole::addToHistoric


ScriptingManager* QtMgx3DPythonConsole::getMgxUserScriptingManager ( )
{
	return _mgxUserScriptingManager;
}	// QtMgx3DPythonConsole::getMgxUserScriptingManager


const ScriptingManager*
				QtMgx3DPythonConsole::getMgxUserScriptingManager ( ) const
{
	return _mgxUserScriptingManager;
}	// QtMgx3DPythonConsole::getMgxUserScriptingManager


void QtMgx3DPythonConsole::setMgxUserScriptingManager (ScriptingManager* scrmng)
{
#ifdef MULTITHREADED_APPLICATION
	AutoMutex	mutex (&getMutex ( ));
#endif	// MULTITHREADED_APPLICATION

	_mgxUserScriptingManager	= scrmng;
}	// QtMgx3DPythonConsole::setMgxUserScriptingManager


QtMgx3DMainWindow* QtMgx3DPythonConsole::getMainWindow ( )
{
	return _mainWindow;
}	// QtMgx3DPythonConsole::getMainWindow


void QtMgx3DPythonConsole::setGraphicalWidget (Qt3DGraphicalWidget* widget)
{
	_graphicalWidget	= widget;
}	// QtMgx3DPythonConsole::setGraphicalWidget


void QtMgx3DPythonConsole::storePolicy ( )
{
	if ((0 != getMainWindow ( )) &&
	    ((CommandManagerIfc::POLICY)-1 == _cmdMgrPolicy))
		_cmdMgrPolicy	= getMainWindow ( )->getContext (
				).getCommandManager( ).setPolicy(CommandManagerIfc::SEQUENTIAL);
}	// QtMgx3DPythonConsole::storePolicy


void QtMgx3DPythonConsole::restorePolicy ( )
{
	if (0 != getMainWindow ( ))
	{
		try
		{
			if ((CommandManagerIfc::POLICY)-1 != _cmdMgrPolicy)
				getMainWindow ( )->getContext (
								).getCommandManager( ).setPolicy(_cmdMgrPolicy);
			_cmdMgrPolicy	= (CommandManagerIfc::POLICY)-1;
		}
		catch (...)
		{
		}
	}	// if (0 != getMainWindow ( ))
}	// QtMgx3DPythonConsole::restorePolicy


}	// namespace QtComponents

}	// namespace Mgx3D
