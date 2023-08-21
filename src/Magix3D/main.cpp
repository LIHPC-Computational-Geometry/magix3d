#include "Internal/ContextIfc.h"
#include <Utils/Log.h>

#include <QtVtkComponents/QtVtkMgx3DMainWindow.h>
#include <QtVtkComponents/QtVtkMgx3DApplication.h>
#include <QtVtkComponents/QtVtkGraphicalWidget.h>
#include "QtVtkComponents/VTKSelectionManagerDimFilter.h"
#include "QtVtkComponents/VTKRenderingManager.h"
#include "Internal/Context.h"
#include <Utils/ErrorManagement.h>
#include "Utils/CommandManager.h"
#include "Internal/Mgx3DArguments.h"
#include "Internal/ClientServerProperties.h"
#include "Python/M3DPythonSession.h"
#include <TkUtil/ErrorLog.h>
#include <TkUtil/InformationLog.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/ProcessLog.h>
#include <TkUtil/WarningLog.h>
#include <TkUtil/ScriptingLog.h>
#include <TkUtil/TraceLog.h>
#include <QTextCodec>
#if PY_MAJOR_VERSION >= 3
#   include <QtPython3/QtPython.h>
#else   // PY_MAJOR_VERSION >= 3
#   include <QtPython/QtPython.h>
#endif  // PY_MAJOR_VERSION >= 3

#ifdef QT_4
#include <QX11Info>
#endif	// QT_4

#include <fenv.h>

using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::QtComponents;
using namespace Mgx3D::QtVtkComponents;
using namespace TkUtil;
using namespace std;


//----------------------------------------------------------------------------------------------------
/**
 * Instanciation de l'IHM <I>Magix 3D</I>.
 */
int main (int argc, char* argv[], char* envp[])
{
	int	status	= 0;

	BEGIN_TRY_CATCH_BLOCK
	
	// Paramètres client/serveur : inutilisés dans cette version de Magix 3D mais l'instance est a créer.
	ClientServerProperties::setInstance (new ClientServerProperties ( ));


	vector<string>	allowedArgs	= mgx3dAllowedArgs ( );
	
	Context::getArguments ( ).allowsArgs (allowedArgs);
	Context::initialize (argc, argv);

#ifdef QT_4
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#else	// QT_4
	// CP : A confirmer :
	QTextCodec::setCodecForLocale (QTextCodec::codecForName("UTF-8"));
#endif	// QT_4

//	BEGIN_TRY_CATCH_BLOCK

#ifdef _DEBUG
	feenableexcept(FE_DIVBYZERO); // |FE_INVALID
#endif

    // logues version ~ Magix
    maillage_log(argc, argv, "Magix3D");

	// Jouer sur la nature des logs générés par l'application : c'est ici le
	// meilleur endroit, comme ci-dessous :
//	TkUtil::Log::TYPE	logMask	= TkUtil::Log::PRODUCTION|TkUtil::Log::TRACE_1;
//	Mgx3D::Internal::Context::setLogsMask (logMask);

	// durée d'une commande à partir de laquelle il est décidée
	// de l'exécuter dans un thread dédié

	CommandManager::sequentialDuration = 5;

	QtPython::preInitialize ( );
	VTKRenderingManager::initialize (false);
	const string			sessionName ("session_1");
	ContextIfc*	context	= new Internal::Context (sessionName, true);
	CHECK_NULL_PTR_ERROR (context)
	context->setPythonSession (createMgx3DPythonSession ( ));
	QtVtkMgx3DApplication	mgxApplication (argc, argv, envp);
	mgxApplication.init (argc, argv, envp);

    // on renseigne le contexte comme quoi on a un affichage graphique [EB]
    context->setGraphical(true);

    // on met de côté le nom de l'exécutable
    context->setExeName(argv[0]);

	VTKSelectionManagerDimFilter*	selectionManager	=
		new VTKSelectionManagerDimFilter ("VTKSelectionManagerDimFilter", context->getLogStream ( ));
	context->setSelectionManager (selectionManager);
	QtVtkMgx3DMainWindow*	mgxMainWindow	= new QtVtkMgx3DMainWindow (0, 0);
	mgxMainWindow->init (sessionName, context, 0, 0);
	QtVtkGraphicalWidget*	graphicalWidget	= new QtVtkGraphicalWidget (mgxMainWindow, 0, 0);
	VTKRenderingManager*	renderingManager	= new VTKRenderingManager (&graphicalWidget->getVTKWidget ( ), false);
	renderingManager->setSelectionManager (selectionManager);
	graphicalWidget->setRenderingManager (renderingManager);
//		new VTKRenderingManager (&graphicalWidget->getVTKWidget ( ), false));
	selectionManager->setRenderingManager (renderingManager);
	mgxMainWindow->setGraphicalWidget (*graphicalWidget);
#ifdef USE_EXPERIMENTAL_ROOM	
	mgxMainWindow->createRaysPanel ( );
#endif	// USE_EXPERIMENTAL_ROOM
	mgxMainWindow->showReady ( );

	// test les versions pour afficher si nécessaire un lien vers l'historique
	TkUtil::Version actualVersion = QtMgx3DApplication::getVersion();
	TkUtil::Version lastVersion = QtMgx3DApplication::getLastVersion();
	if (lastVersion<actualVersion){
		QtMgx3DApplication::setLastVersion(actualVersion);
		mgxMainWindow->infoNewVersion(lastVersion, actualVersion);
	}


	// Y a-t-il des scripts à exécuter ?
	for (vector<string>::const_iterator itpy =
			Resources::instance ( ).instance ( )._scripts.begin ( );
			Resources::instance ( ).instance ( )._scripts.end ( ) != itpy; itpy++)
		mgxMainWindow->executePythonScript (*itpy);

	status	= mgxApplication.exec ( );

	COMPLETE_TRY_CATCH_BLOCK

	if (true ==hasError)
		cerr << argv [0] << " erreur : " << errorString << endl;

	return true == hasError ? -1 : status;
}
