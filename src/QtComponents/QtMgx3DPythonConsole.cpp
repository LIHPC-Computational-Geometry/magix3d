#include "Internal/ContextIfc.h"
#include "Internal/PythonWriter.h"

#include "QtComponents/QtMgx3DPythonConsole.h"
#include "QtComponents/Qt3DGraphicalWidget.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <PythonUtil/PythonInfos.h>
#include <TkUtil/Date.h>
#include <TkUtil/Exception.h>
#include <TkUtil/ErrorLog.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/MachineData.h>
#include <TkUtil/Process.h>
#include <TkUtil/ProcessLog.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UserData.h>
#include <TkUtil/UtilInfos.h>

#include <IQualif.h>

#include <assert.h>
#include <fstream>
#include <memory>

#include <QLayout>
#include <QLabel>
#include <QKeyEvent>

#include <QThread>
#include <QTextCodec>

#include <Standard_Version.hxx>

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


QtMgx3DPythonConsole::QtMgx3DPythonConsole (QWidget* parent, QtMgx3DMainWindow* mainWindow, const string& title)
	: QtPythonConsole (parent, title),
	  _mgxUserScriptingManager (0), _mainWindow (mainWindow), _graphicalWidget (0), _cmdMgrPolicy ((CommandManagerIfc::POLICY)-1),
	  _insertSelectedEntitiesAction (0),
	  _insertSelectedVolumesAction (0), _insertSelectedSurfacesAction (0), _insertSelectedCurvesAction (0), _insertSelectedGeomVerticesAction (0),
	  _insertSelectedBlocksAction (0), _insertSelectedFacesAction (0), _insertSelectedEdgesAction (0), _insertSelectedTopoVerticesAction (0),
	  _insertSelectedMeshVolumesAction (0), _insertSelectedMeshSurfacesAction (0), _insertSelectedLinesAction (0), _insertSelectedCloudsAction (0)
{
	hideResult ("proxy of <Swig Object of type");
	_insertSelectedEntitiesAction		= new QAction (UTF8TOQSTRING (UTF8String ("Toutes entités", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedEntitiesAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::undefined); });
	_insertSelectedVolumesAction		= new QAction (UTF8TOQSTRING (UTF8String ("Volumes Géométriques", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedVolumesAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::GeomVolume); });
	_insertSelectedSurfacesAction		= new QAction (UTF8TOQSTRING (UTF8String ("Surfaces Géométriques", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedSurfacesAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::GeomSurface); });
	_insertSelectedCurvesAction			= new QAction (UTF8TOQSTRING (UTF8String ("Courbes Géométriques", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedCurvesAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::GeomCurve); });
	_insertSelectedGeomVerticesAction	= new QAction (UTF8TOQSTRING (UTF8String ("Sommets Géométriques", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedGeomVerticesAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::GeomVertex); });
	_insertSelectedBlocksAction		= new QAction (UTF8TOQSTRING (UTF8String ("Blocs Topologiques", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedBlocksAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::TopoBlock); });
	_insertSelectedFacesAction		= new QAction (UTF8TOQSTRING (UTF8String ("Faces Topologiques", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedFacesAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::TopoCoFace); });
	_insertSelectedEdgesAction			= new QAction (UTF8TOQSTRING (UTF8String ("Arêtes Topologiques", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedEdgesAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::TopoCoEdge); });
	_insertSelectedTopoVerticesAction	= new QAction (UTF8TOQSTRING (UTF8String ("Sommets Topologiques", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedTopoVerticesAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::TopoVertex); });	
	_insertSelectedMeshVolumesAction		= new QAction (UTF8TOQSTRING (UTF8String ("Volumes de Mailles", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedMeshVolumesAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::MeshVolume); });
	_insertSelectedMeshSurfacesAction		= new QAction (UTF8TOQSTRING (UTF8String ("Surfaces de Mailles", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedMeshSurfacesAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::MeshSurface); });
	_insertSelectedLinesAction			= new QAction (UTF8TOQSTRING (UTF8String ("Lignes de Mailles", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedLinesAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::MeshLine); });
	_insertSelectedCloudsAction	= new QAction (UTF8TOQSTRING (UTF8String ("Nuages de Mailles", Charset::UTF_8)), this);
	QObject::connect (_insertSelectedCloudsAction, &QAction::triggered, this, [this]( ){ insertSelectionCallback (Utils::Entity::MeshCloud); });
}	// QtMgx3DPythonConsole::QtMgx3DPythonConsole


QtMgx3DPythonConsole::QtMgx3DPythonConsole (const QtMgx3DPythonConsole&)
	: QtPythonConsole (0, ""),
	  _mgxUserScriptingManager (0), _mainWindow (0), _graphicalWidget (0), _cmdMgrPolicy ((CommandManagerIfc::POLICY)-1),
	  _insertSelectedEntitiesAction (0),
	  _insertSelectedVolumesAction (0), _insertSelectedSurfacesAction (0), _insertSelectedCurvesAction (0), _insertSelectedGeomVerticesAction (0),
	  _insertSelectedBlocksAction (0), _insertSelectedFacesAction (0), _insertSelectedEdgesAction (0), _insertSelectedTopoVerticesAction (0),
	  _insertSelectedMeshVolumesAction (0), _insertSelectedMeshSurfacesAction (0), _insertSelectedLinesAction (0), _insertSelectedCloudsAction (0)
{
	assert (0 && "QtMgx3DPythonConsole copy constructor is forbidden.");
}	// QtMgx3DPythonConsole::QtMgx3DPythonConsole (const QtMgx3DPythonConsole&)


QtMgx3DPythonConsole& QtMgx3DPythonConsole::operator = (const QtMgx3DPythonConsole&)
{
	assert (0 && "QtMgx3DPythonConsole assignment operator is forbidden.");
	return *this;
}	// QtMgx3DPythonConsole::operator =


QtMgx3DPythonConsole::~QtMgx3DPythonConsole ( )
{
	_graphicalWidget	= 0;

	// Contrairement au scripting manager, le user scripting manager relève de la responsabilité de ce panneau :
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
		default							: enableActions	= true;
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
	
	ActionCompletionNotifier actionCompletionNotifier (getAppName ( ), QtMgx3DApplication::getAppIcon ( ), UTF8String ("Instructions python exécutées", Charset::UTF_8), QtMessageBox::URGENCY_NORMAL, Resources::instance ( )._commandNotificationDuration, Resources::instance ( )._commandNotificationDelay);

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

	ActionCompletionNotifier actionCompletionNotifier (getAppName ( ), QtMgx3DApplication::getAppIcon ( ), UTF8String ("Instructions python exécutées", Charset::UTF_8), QtMessageBox::URGENCY_NORMAL, Resources::instance ( )._commandNotificationDuration, Resources::instance ( )._commandNotificationDelay);

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

	ActionCompletionNotifier actionCompletionNotifier (getAppName ( ), QtMgx3DApplication::getAppIcon ( ), UTF8String ("Script exécuté", Charset::UTF_8), QtMessageBox::URGENCY_NORMAL, Resources::instance ( )._commandNotificationDuration, Resources::instance ( )._commandNotificationDelay);
	
	unique_ptr<RenderingManager::DisplayLocker>	displayLocker (0 == _graphicalWidget ? 0 : new RenderingManager::DisplayLocker (_graphicalWidget->getRenderingManager ( )));

	CommandManagerIfc::POLICY	cmdMgrPolicy	= CommandManagerIfc::THREADED;
// Le PythonConsole reposant sur la QConsole ne supporte pas les instructions écrites sur plusieurs lignes ... => on passe par PythonSession :
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


void QtMgx3DPythonConsole::addToHistoric (const UTF8String& command, const UTF8String& comments, const UTF8String& commandOutput, bool statusErr, bool fromKernel)
{
#ifdef MULTITHREADED_APPLICATION
	AutoMutex	mutex (&getMutex ( ));
#endif	// MULTITHREADED_APPLICATION

	LogOutputStream*	logStream	= getLogStream ( );
	if (true == fromKernel)
		setLogStream (0);	// CP 07/24 : éviter une double écriture dans la fenêtre "Historique" :
	QtPythonConsole::addToHistoric (command, comments, commandOutput, statusErr, fromKernel);
	if (true == fromKernel)
		setLogStream (logStream);
}	// QtMgx3DPythonConsole::addToHistoric


ScriptingManager* QtMgx3DPythonConsole::getMgxUserScriptingManager ( )
{
	return _mgxUserScriptingManager;
}	// QtMgx3DPythonConsole::getMgxUserScriptingManager


const ScriptingManager* QtMgx3DPythonConsole::getMgxUserScriptingManager ( ) const
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


void QtMgx3DPythonConsole::saveConsoleScript (const string filePath, Charset charset, bool environment)
{
	UTF8String	text (charset);
	if (false == environment)
		text << "# -*- coding: " << Charset::charset2str (charset.charset ( )) << " -*-" << "\n" << "\n";
	else
	{
		Date			date;
		OperatingSystem	os;
		text << "#!" << PYTHON_INTERPRETER << "\n";
		text << "# -*- coding: " << Charset::charset2str (charset.charset ( )) << " -*-" << "\n" << "\n";
		text << "# Logiciel               : " << Process::getCurrentSoftware ( ) << "\n";
		text << "# Version                : " << Process::getCurrentSoftwareVersion ( ).getVersion ( ) << "\n";
		text << "# Système d'exploitation : " << os.getName ( ) << " " << os.getVersion ( ).getVersion ( ) << "\n";
		text << "# Auteur                 : " << UserData ( ).getName ( ) << "\n";
		text << "# Date                   : " << date.getDate ( ) << " " << date.getTime ( ) << "\n";
		text << "# Version API TkUtil     : " << UtilInfos::getVersion ( ).getVersion ( ) << "\n";
		text << "# Version de Python      : " << PYTHON_VERSION << "\n";
		text << "# Version de Swig        : " << SWIG_VERSION << "\n";
		text << "# Version API PythonUtil : " << PythonInfos::getVersion ( ).getVersion ( ) << "\n";
		text << "# Version API Qualif     : " << Qualif::QualifVersion ( ) << "\n";
		text << "# Version API OCC        : " << OCC_VERSION_COMPLETE << "\n" << "\n";

		// Un avertissement important :
		text << "# ATTENTION : en cas d'utilisation en dehors de l'IHM Magix3D remplacez la ligne :\n"
		     << "# ctx = Mgx3D.getContext(\"session_1\")\n"
		     << "# par :\n"
		     << "# ctx = Mgx3D.getStdContext()\n\n";	     
	}
	
	// Contenu de la console :
	const QString	qtext	= toPlainText ( );
	text << qtext.toStdString ( ) << "\n";
	
	unique_ptr<ofstream>	stream (new ofstream (filePath.c_str( ), ios::trunc));
	if (stream->rdstate ( ) != std::ios_base::goodbit)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur lors de l'ouverture en écriture du script " << filePath << ".";
		throw Exception (error);
	}	// else if (stream->rdstate ( ) == std::ios_base::goodbit)
	switch (charset.charset ( ))
	{
		case Charset::ASCII		: (*stream.get ( )) << text.ascii ( );	break;
		case Charset::UTF_8		: (*stream.get ( )) << text.utf8 ( );	break;
//		case Charset::UTF_16	: (*stream.get ( )).write ((WChar_t*)text.utf16 ( ).utf16 ( ), text.utf16 ( ).length ( ));	break;
		case Charset::ISO_8859	: (*stream.get ( )) << text.iso ( );	break;
		default	:
			UTF8String	error (Charset::UTF_8);
			error << "Erreur lors de l'écriture du script " << filePath << " : charset no supporté (" << Charset::charset2str (charset.charset ( )) << ")";
			throw Exception (error);

	}	// switch (charset.charset ( ))
	if (stream->rdstate ( ) != std::ios_base::goodbit)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur lors de l'écriture du script " << filePath << ".";
		throw Exception (error);
	}	// if (stream->rdstate ( ) != std::ios_base::goodbit)
	stream->flush ( );
	stream->close ( );
	if (stream->rdstate ( ) != std::ios_base::goodbit)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur lors de la fermeture du script " << filePath << ".";
		throw Exception (error);
	}	// if (stream->rdstate ( ) != std::ios_base::goodbit)
}	// QtMgx3DPythonConsole::saveConsoleScript


void QtMgx3DPythonConsole::insertSelectionCallback (Utils::Entity::objectType type)
{
	SelectionManagerIfc*	sm	= 0 == getMainWindow ( ) ? 0 : getMainWindow ( )->getSelectionManager ( );
	if (0 == sm)
		return;

	const vector<Utils::Entity*>	entities		= Entity::undefined == type ? sm->getEntities ( ) : sm->getEntities (type);
	const string					entitiesNames	= Internal::entitiesToPythonList<Entity>(entities);

	insertPlainText (entitiesNames.c_str ( ));
}	// QtMgx3DPythonConsole::insertSelectionCallback


QMenu* QtMgx3DPythonConsole::createPopupMenu ( )
{
	QMenu*	menu	= QtPythonConsole::createPopupMenu ( );
	if (0 != menu)
	{
		menu->addSeparator ( );
		QMenu*	entry	= new QMenu ("Insérer sélection", menu);
		menu->addMenu (entry);
		entry->addAction (&insertSelectedEntitiesAction ( ));
		entry->addSeparator ( );
		entry->addAction (&insertSelectedVolumesAction ( ));
		entry->addAction (&insertSelectedSurfacesAction ( ));
		entry->addAction (&insertSelectedCurvesAction ( ));
		entry->addAction (&insertSelectedGeomVerticesAction ( ));
		entry->addSeparator ( );
		entry->addAction (&insertSelectedBlocksAction ( ));
		entry->addAction (&insertSelectedFacesAction ( ));
		entry->addAction (&insertSelectedEdgesAction ( ));
		entry->addAction (&insertSelectedTopoVerticesAction ( ));
		entry->addSeparator ( );
		entry->addAction (&insertSelectedMeshVolumesAction ( ));
		entry->addAction (&insertSelectedMeshSurfacesAction ( ));
		entry->addAction (&insertSelectedLinesAction ( ));
		entry->addAction (&insertSelectedCloudsAction ( ));
	}	// if (0 != menu)
	
	return menu;
}	// QtMgx3DPythonConsole::createPopupMenu


QAction& QtMgx3DPythonConsole::insertSelectedEntitiesAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedEntitiesAction)
	return *_insertSelectedEntitiesAction;
}	// QtMgx3DPythonConsole::insertSelectedEntitiesAction


QAction& QtMgx3DPythonConsole::insertSelectedVolumesAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedVolumesAction)
	return *_insertSelectedVolumesAction;
}	// QtMgx3DPythonConsole::insertSelectedVolumesAction


QAction& QtMgx3DPythonConsole::insertSelectedSurfacesAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedSurfacesAction)
	return *_insertSelectedSurfacesAction;
}	// QtMgx3DPythonConsole::insertSelectedSurfacesAction


QAction& QtMgx3DPythonConsole::insertSelectedCurvesAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedCurvesAction)
	return *_insertSelectedCurvesAction;
}	// QtMgx3DPythonConsole::insertSelectedCurvesAction


QAction& QtMgx3DPythonConsole::insertSelectedGeomVerticesAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedGeomVerticesAction)
	return *_insertSelectedGeomVerticesAction;
}	// QtMgx3DPythonConsole::insertSelectedGeomVerticesAction


QAction& QtMgx3DPythonConsole::insertSelectedBlocksAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedBlocksAction)
	return *_insertSelectedBlocksAction;
}	// QtMgx3DPythonConsole::insertSelectedBlocksAction


QAction& QtMgx3DPythonConsole::insertSelectedFacesAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedFacesAction)
	return *_insertSelectedFacesAction;
}	// QtMgx3DPythonConsole::insertSelectedFacesAction


QAction& QtMgx3DPythonConsole::insertSelectedEdgesAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedEdgesAction)
	return *_insertSelectedEdgesAction;
}	// QtMgx3DPythonConsole::insertSelectedEdgesAction


QAction& QtMgx3DPythonConsole::insertSelectedTopoVerticesAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedTopoVerticesAction)
	return *_insertSelectedTopoVerticesAction;
}	// QtMgx3DPythonConsole::insertSelectedTopoVerticesAction


QAction& QtMgx3DPythonConsole::insertSelectedMeshVolumesAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedMeshVolumesAction)
	return *_insertSelectedMeshVolumesAction;
}	// QtMgx3DPythonConsole::insertSelectedMeshVolumesAction


QAction& QtMgx3DPythonConsole::insertSelectedMeshSurfacesAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedMeshSurfacesAction)
	return *_insertSelectedMeshSurfacesAction;
}	// QtMgx3DPythonConsole::insertSelectedMeshSurfacesAction


QAction& QtMgx3DPythonConsole::insertSelectedLinesAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedLinesAction)
	return *_insertSelectedLinesAction;
}	// QtMgx3DPythonConsole::insertSelectedLinesAction


QAction& QtMgx3DPythonConsole::insertSelectedCloudsAction ( )
{
	CHECK_NULL_PTR_ERROR (_insertSelectedCloudsAction)
	return *_insertSelectedCloudsAction;
}	// QtMgx3DPythonConsole::insertSelectedCloudsAction


void QtMgx3DPythonConsole::storePolicy ( )
{
	if ((0 != getMainWindow ( )) && ((CommandManagerIfc::POLICY)-1 == _cmdMgrPolicy))
		_cmdMgrPolicy	= getMainWindow ( )->getContext ( ).getCommandManager( ).setPolicy(CommandManagerIfc::SEQUENTIAL);
}	// QtMgx3DPythonConsole::storePolicy


void QtMgx3DPythonConsole::restorePolicy ( )
{
	if (0 != getMainWindow ( ))
	{
		try
		{
			if ((CommandManagerIfc::POLICY)-1 != _cmdMgrPolicy)
				getMainWindow ( )->getContext ( ).getCommandManager( ).setPolicy(_cmdMgrPolicy);
			_cmdMgrPolicy	= (CommandManagerIfc::POLICY)-1;
		}
		catch (...)
		{
		}
	}	// if (0 != getMainWindow ( ))
}	// QtMgx3DPythonConsole::restorePolicy


}	// namespace QtComponents

}	// namespace Mgx3D
