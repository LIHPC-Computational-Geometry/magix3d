/**
 * \file        QtMgx3DApplication.cpp
 * \author      Charles PIGNEROL
 * \date        29/11/2010
 */

#include "mgx_config.h"
#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/EntitySeizureManager.h"
#include <QtUtil/QtErrorManagement.h>
#include <QtComponents/QtMgx3DPreferencesDialog.h>
#include "Utils/Common.h"
#include "Utils/DisplayProperties.h"

#include "Internal/Context.h"
#include "Internal/InternalPreferences.h"
#include "Internal/Resources.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/NetworkData.h>
#include <PrefsCore/PreferencesHelper.h>
#include <TkUtil/RemoteProcess.h>
#include <TkUtil/ThreadManager.h>
#include <TkUtil/ThreadPool.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/UserData.h>
#include <QtUtil/QtAutoWaitingCursor.h>
#include <QtUtil/QtHelpWindow.h>
#include <PrefsQt/QtPrefsUpdater.h>
#if PY_MAJOR_VERSION >= 3
#	include <QtPython3/QtPython.h>
#	include <QtPython3/QtPythonConsole.h>
#else	// PY_MAJOR_VERSION >= 3
#	include <QtPython/QtPython.h>
#	include <QtPython/QtPythonConsole.h>
#endif	// PY_MAJOR_VERSION >= 3
#include <QtUtil/QtStringHelper.h>
#include <QtUtil/QtValidatedTextField.h>
#include <QwtCharts/QwtChartsManager.h>
#include <PrefsXerces/XMLLoader.h>

#include <QTextCodec>


using namespace std;
using namespace TkUtil;
using namespace Preferences;
using namespace Mgx3D;
using namespace Utils;
using namespace Mgx3D::Internal;


// Faut-il afficher la syntaxe d'utilisation du logiciel ?
static bool	displayHelp		= false;

// Ressources éventuellement récupérées à la ligne de commande et surchargeant les ressources lues en configuration.
unsigned long	graphicalWindowWidth	= (unsigned long)-1;
unsigned long	graphicalWindowHeight	= (unsigned long)-1;
bool			graphicalWindowFixedSize= false;
double			stillFrameRate			= -1.;
double			desiredFrameRate		= -1.;


inline void initQtComponentsResources ( )
{
	// Ne peut être dans un espace de nommage, dixit la doc Qt.
	Q_INIT_RESOURCE (QtComponents);
}	// initQtComponentsResources


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                  LA CLASSE QtMgx3DApplication::HelpSystem
// ===========================================================================

QtMgx3DApplication::HelpSystem*	QtMgx3DApplication::HelpSystem::_instance	= 0;

QtMgx3DApplication::HelpSystem::HelpSystem ( )
:
userManual (""),
docViewer (""),
rootURL (""),
wikiURL (""),
qualifURL (""),
indexURL ("pages/manuel-utilisateur.html"),
historiqueURL ("pages/historique.html"),
tutorialURL ("index.html"),
tutorialTag ("index-tutoriels"),
pythonAPIURL ("pages/api-python.html"),
sheetExplorationURL ("pages/menu-maillage.html"),
sheetExplorationTag ("exploration-feuillets"),
meshQualityOperationURL ("pages/menu-maillage.html"),
meshQualityOperationTag ("evaluation-qualite"),
shortKeyURL ("pages/raccourcis.html"),
selectURL ("pages/selection.html"),
selectTag (""),
operationsURL ("pages/operations.html"),
operationsTag (""),

distanceMeasurementURL ("pages/menu-outils.html"),
distanceMeasurementTag ("mesure-distance"),
angleMeasurementURL ("pages/menu-outils.html"),
angleMeasurementTag ("mesure-angle"),
extremaMeshingEdgeLengthURL ("pages/menu-outils.html"),
extremaMeshingEdgeLengthTag ("mesure-longeur-bras"),

topoInformationURL ("pages/menu-topologie.html"),
topoInformationTag ("info-topologie"),

meshInformationURL ("pages/menu-maillage.html"),
meshInformationTag ("info-maillage"),

vertexOperationURL ("pages/operations-geometriques.html"),
vertexOperationTag ("creation-point"),
segmentOperationURL ("pages/operations-geometriques.html"),
segmentOperationTag ("creation-segment"),
arcCircleOperationURL ("pages/operations-geometriques.html"),
arcCircleOperationTag ("creation-arc-cercle"),
circleOperationURL ("pages/operations-geometriques.html"),
circleOperationTag ("creation-cercle"),
curveByProjectionOperationURL ("pages/operations-geometriques.html"),
curveByProjectionOperationTag ("creation-courbe-par-projection"),
boxOperationURL ("pages/operations-geometriques.html"),
boxOperationTag ("creation-boite"),
sphereOperationURL ("pages/operations-geometriques.html"),
sphereOperationTag ("creation-sphere"),
aiguilleOperationURL ("pages/operations-geometriques.html"),
aiguilleOperationTag ("creation-aiguille"),
cylinderOperationURL ("pages/operations-geometriques.html"),
cylinderOperationTag ("creation-cylindre"),
coneOperationURL ("pages/operations-geometriques.html"),
coneOperationTag ("creation-cone"),
prismOperationURL ("pages/operations-geometriques.html"),
prismOperationTag ("creation-prisme"),
surfaceCreationURL ("pages/operations-geometriques.html"),
surfaceCreationTag ("creation-surface"),
volumeCreationURL ("pages/operations-geometriques.html"),
volumeCreationTag ("creation-volume"),
geomByRevolutionOperationURL ("pages/operations-geometriques.html"),
geomByRevolutionOperationTag ("creation-par-revolution"),
geomCopyOperationURL ("pages/operations-geometriques.html"),
geomCopyOperationTag ("copie-entites-geometriques"),
geomCopyByOffsetURL ("pages/operations-geometriques.html"),
geomCopyByOffsetTag ("copie-surface-distance"),
geomDestructionOperationURL ("pages/operations-geometriques.html"),
geomDestructionOperationTag ("destruction-entites-geometriques"),
geomMirrorOperationURL ("pages/operations-geometriques.html"),
geomMirrorOperationTag ("symetrie-entites-geometriques"),
geomGlueCurvesOperationURL ("pages/operations-geometriques.html"),
geomGlueCurvesOperationTag ("collage-courbes"),
geomGlueSurfacesOperationURL ("pages/operations-geometriques.html"),
geomGlueSurfacesOperationTag ("collage-surfaces"),
geomPlaneCutOperationURL ("pages/operations-geometriques.html"),
geomPlaneCutOperationTag ("coupe-plan-entites-geometriques"),
fuseOperationURL ("pages/operations-geometriques.html"),
fuseOperationTag ("operations-booleennes"),
geomCommon2DOperationURL ("pages/operations-geometriques.html"),
geomCommon2DOperationSurfacesTag ("intersection-surfaces"),
geomCommon2DOperationCourbesTag ("intersection-courbes"),
geomAddToGroupOperationURL("pages/operations-geometriques.html"),
geomAddToGroupOperationTag("ajout-groupe"),

vertexCreationURL ("pages/operations-topologiques.html"),
vertexCreationTag ("creation-sommet"),
edgeCreationURL ("pages/operations-topologiques.html"),
edgeCreationTag ("creation-arete"),
faceCreationURL ("pages/operations-topologiques.html"),
faceCreationTag ("creation-face-surface"),
blockCreationURL ("pages/operations-topologiques.html"),
blockCreationTag ("creation-bloc-volume"),
placeVertexOperationURL ("pages/operations-topologiques.html"),
placeVertexOperationTag ("positionnement-sommet"),
projectVerticesOperationURL ("pages/operations-topologiques.html"),
projectVerticesOperationTag ("modif-coord-sommets"),
alignVerticesOperationURL ("pages/operations-topologiques.html"),
alignVerticesOperationTag ("alignement-sommets"),
snapVerticesOperationURL ("pages/operations-topologiques.html"),
snapVerticesOperationTag ("projection-sommets-geometrie"),
degenerateBlockOperationURL ("pages/operations-topologiques.html"),
degenerateBlockOperationTag ("degenerescence-entites"),
fuse2verticesOperationURL ("pages/operations-topologiques.html"),
fuse2verticesOperationTag ("fusion-sommets"),
fuseEdgesOperationURL ("pages/operations-topologiques.html"),
fuseEdgesOperationTag ("raboutage-aretes"),
fuse2EdgesOperationURL ("pages/operations-topologiques.html"),
fuse2EdgesOperationTag ("fusion-aretes"),
fuse2facesOperationURL ("pages/operations-topologiques.html"),
fuse2facesOperationTag ("fusion-faces"),
fuse2blocksOperationURL ("pages/operations-topologiques.html"),
fuse2blocksOperationTag ("fusion-blocs"),
glue2blocksOperationURL ("pages/operations-topologiques.html"),
glue2blocksOperationTag ("collage-blocs"),
associateOperationURL ("pages/operations-topologiques.html"),
associateOperationTag ("association-topo-geo"),
blocksByRevolutionCreationURL ("pages/operations-topologiques.html"),
blocksByRevolutionCreationTag ("creation-bloc-revolution"),
topoCopyOperationURL ("pages/operations-topologiques.html"),
topoCopyOperationTag ("copie-blocs"),
topoBlockExtractionOperationURL ("pages/operations-topologiques.html"),
topoBlockExtractionOperationTag ("extract-blocs"),
topoAddToGroupOperationURL("pages/operations-topologiques.html"),
topoAddToGroupOperationTag("modification-affectation-groupe"),
splitFaceOperationURL ("pages/operations-topologiques.html"),
splitFaceOperationTag ("decoupage-face-arete"),
splitFacesOperationURL ("pages/operations-topologiques.html"),
splitFacesOperationTag ("decoupage-faces-arete"),
extendSplitFacesOperationURL ("pages/operations-topologiques.html"),
extendSplitFacesOperationTag ("prolongation-face-sommet"),
splitBlockOperationURL ("pages/operations-topologiques.html"),
splitBlockOperationTag ("decoupage-blocs-arete"),
splitFaceWithOGridOperationURL ("pages/operations-topologiques.html"),
splitFaceWithOGridOperationTag ("decoupage-face-ogrid"),
splitBlockWithOGridOperationURL ("pages/operations-topologiques.html"),
splitBlockWithOGridOperationTag ("decoupage-blocs-ogrid"),
extendSplitBlockOperationURL ("pages/operations-topologiques.html"),
extendSplitBlockOperationTag ("prolongation-decoupage-blocs"),
insertHoleOperationURL ("pages/operations-topologiques.html"),
insertHoleOperationTag ("insertion-trou"),
topoDestructionOperationURL ("pages/operations-topologiques.html"),
topoDestructionOperationTag ("destruction-entites-topologiques"),
topoTranslationOperationURL ("pages/operations-topologiques.html"),
topoTranslationOperationTag ("translation-entites-topologiques"),
topoRotationOperationURL ("pages/operations-topologiques.html"),
topoRotationOperationTag ("rotation-entites-topologiques"),
topoHomothetyOperationURL ("pages/operations-topologiques.html"),
topoHomothetyOperationTag ("homethetie-entites-topologiques"),
topoMirrorOperationURL ("pages/operations-topologiques.html"),
topoMirrorOperationTag ("symetrie-entites-topologiques"),
edgeCutOperationURL ("pages/operations-topologiques.html"),
edgeCutOperationTag ("decoupage-arete"),
edgeDirectionOperationURL ("pages/operations-topologiques.html"),
edgeDirectionOperationTag ("inversion-discretisation"),
edgesRefinementOperationURL ("pages/operations-topologiques.html"),
edgesRefinementOperationTag ("parametrage-discretisation-aretes"),
facesMeshingPropertyOperationURL ("pages/operations-topologiques.html"),
facesMeshingPropertyOperationTag ("parametrage-discretisation-faces"),
blocsMeshingPropertyOperationURL ("pages/operations-topologiques.html"),
blocsMeshingPropertyOperationTag ("parametrage-discretisation-blocs"),
unrefineBlockOperationURL ("pages/operations-topologiques.html"),
unrefineBlockOperationTag ("deraffinement-blocs"),
associateBlockOperationURL ("pages/operations-topologiques.html"),
associateBlockOperationTag ("association-blocs-volumes"),

meshAllSurfacesOperationURL ("pages/operations-maillage.html"),
meshAllSurfacesOperationTag ("mailler-surfaces"),
meshAllBlocksOperationURL ("pages/operations-maillage.html"),
meshAllBlocksOperationTag ("mailler-blocs"),
meshSheetsVolumeOperationURL ("pages/operations-maillage.html"),
meshSheetsVolumeOperationTag ("extraire-volume-feuillets"),
groupProjectionOperationURL ("pages/operations-maillage.html"),
groupProjectionOperationTag ("projeter-surfaces"),
meshSmoothSurfaceOperationURL ("pages/operations-maillage.html"),
meshSmoothSurfaceOperationTag ("lisser-surfaces"),
meshSmoothVolumeOperationURL ("pages/operations-maillage.html"),
meshSmoothVolumeOperationTag ("lisser-volumes"),

monitorPanelURL ("pages/onglet-moniteur.html"),
monitorPanelTag (""),

sheetOperationURL ("pages/manuel-utilisateur.html"),
sheetOperationTag ("MenuExtractionDeFeuillet"),
representationURL ("pages/manuel-utilisateur.html"),
representationTag ("MenuModificationRepresentation"),

rayURL ("pages/manuel-utilisateur.html"),
rayTag ("PointagesLaser"),
rayIOsTag ("PointagesLaserIOs"),
rayBoundingBoxTag ("PointagesLaserBoiteEnglobante"),
raysParametersTag ("PointagesLaserParametres"),
raysGraphicalParametersTag ("PointagesLaserParametresGraphiques"),
raysTransform ("PointagesLaserTransformation"),

syscoordOperationURL ("pages/operations-reperes.html"),
syscoordCreationTag ("creation-repere"),
syscoordRotationTag ("translation-repere"),
syscoordTranslationTag ("rotation-repere")
{

}	// HelpSystem::HelpSystem

QtMgx3DApplication::HelpSystem::HelpSystem (const HelpSystem&)
{
	MGX_FORBIDDEN ("HelpSystem copy constructor is not allowed.");
}	// HelpSystem::HelpSystem

QtMgx3DApplication::HelpSystem& QtMgx3DApplication::HelpSystem::operator = (const QtMgx3DApplication::HelpSystem&)
{
	MGX_FORBIDDEN ("HelpSystem assignment operator is not allowed.");
	return *this;
}	// HelpSystem::operator =


QtMgx3DApplication::HelpSystem::~HelpSystem ( )
{
	MGX_FORBIDDEN ("HelpSystem destructor is not allowed.");
}	// HelpSystem::~HelpSystem


QtMgx3DApplication::HelpSystem& QtMgx3DApplication::HelpSystem::instance ( )
{
	if (0 == _instance)
		_instance	= new QtMgx3DApplication::HelpSystem ( );
		
	return *_instance;
}	// QtMgx3DApplication::HelpSystem::instance


void QtMgx3DApplication::HelpSystem::show (std::string path)
{
	Process*	helpProcess	=
			new Process (QtMgx3DApplication::HelpSystem::instance ( ).docViewer);
	CHECK_NULL_PTR_ERROR (helpProcess)
	Process::ProcessOptions&	options	= helpProcess->getOptions ( );
	options.addOption (path);
	helpProcess->execute ( );
}

void QtMgx3DApplication::HelpSystem::showUrl (std::string url)
{
	string rootURL = "file:///" + QtMgx3DApplication::HelpSystem::instance ( ).rootURL;
	UTF8String	fullURL;
	if ((0 != rootURL.size ( )) && ('/' != rootURL [rootURL.size ( ) - 1]))
		fullURL	= rootURL + '/' + url;
	else
		fullURL	= rootURL + url;

	Process*	helpProcess	=
			new Process (QtMgx3DApplication::HelpSystem::instance ( ).docViewer);
	CHECK_NULL_PTR_ERROR (helpProcess)
	Process::ProcessOptions&	options	= helpProcess->getOptions ( );
	options.addOption (fullURL);
	helpProcess->execute ( );
}

void QtMgx3DApplication::HelpSystem::showUrl (std::string url, std::string tag)
{
	showUrl(url + "#" + tag);
}

// ===========================================================================
//                        LA CLASSE QtMgx3DApplication
// ===========================================================================



QtMgx3DApplication*	QtMgx3DApplication::_instance	= 0;
int					QtMgx3DApplication::_argc		= 0;
char**				QtMgx3DApplication::_argv		= 0;
char**				QtMgx3DApplication::_envp		= 0;
string				QtMgx3DApplication::_appIcon;


QtMgx3DApplication::QtMgx3DApplication (int& argc, char* argv[], char* envp[])
	: QApplication (argc, argv)
{
	QtMgx3DApplication::_instance	= this;
}	// QtMgx3DApplication::QtMgx3DApplication


#ifdef QT_4
QtMgx3DApplication::QtMgx3DApplication (const QtMgx3DApplication&)
	: QApplication (0, 0)
{
	MGX_FORBIDDEN ("QtMgx3DApplication copy constructor is not allowed.");
}	// QtMgx3DApplication::QtMgx3DApplication (const QtMgx3DApplication&)
#endif	// QT_4


QtMgx3DApplication& QtMgx3DApplication::operator = (const QtMgx3DApplication&)
{
	MGX_FORBIDDEN ("QtMgx3DApplication assignment operator is not allowed.");
	return *this;
}	// QtMgx3DApplication::QtMgx3DApplication (const QtMgx3DApplication&)


QtMgx3DApplication::~QtMgx3DApplication ( )
{
	_instance	= 0;
	ContextIfc::finalize ( );
	QwtChartsManager::finalize ( );
	QtHelpWindow::finalize ( );
	QtStringHelper::finalize ( );
	ThreadPool::finalize ( );
}	// QtMgx3DApplication::~QtMgx3DApplication


void QtMgx3DApplication::init (int argc, char* argv[], char* envp[])
{
	_argc	= argc;
	_argv	= argv;
	_envp	= envp;
	initQtComponentsResources ( );

	// QTextCodec : pour les flux Qt, notamment pour le QTextStream utilisé par la console python QConsole. En son absence, si locale vaut C alors
	// les caractères accentués des fichiers scripts seront perdus.
	// CP, remarque du 18/09/24 : ne semble plus d'actualité, et perturbe les QFileDialog lorsque les fichiers/répertoires ont des caractères
	// accentués, ceux-ci sont convertis en std::string de manière erronée => on commente l'appel à QTextCodec::setCodecForLocale. Le contenu
	// de la console python semble OK en environnement fr_FR.UTF-8
	// QTextCodec::setCodecForLocale (QTextCodec::codecForName ("ISO 8859-1"));
	// Eviter des messages d'erreur lorsque l'on quitte un champ de saisie numérique vide :
	QtValidatedTextField::automaticValidation	=true;
	QtValidatedTextField::dialogOnError			=false;
	QtValidatedTextField::errorBackground		= QColor (250, 90, 40);
	QtValidatedTextField::errorForeground		= QColor (255, 255, 255);
	QtStringHelper::initialize ( );
	QwtChartsManager::initialize ( );
#ifdef USE_PYTHON_CONSOLE
	// Indispensable en Python 3 avant l'appel à Py_Initialize : initialise le redirector qui
	// redirige les sorties standards python vers la console python.
	QtPython::preInitialize ( );
#endif  // USE_PYTHON_CONSOLE
	QtPython::initialize (Context::getOutCharset ( ));
	Process::initialize (argc, argv, envp);
	ContextIfc::initialize ( );
	try
	{
		Process::setCurrentSoftware ("Magix3D", Version (MAGIX3D_VERSION));
	}
	catch (...)
	{
	}
	try
	{
		ThreadManager::instance ( );
	}
	catch (...)
	{
		ThreadManager::initialize (0);
		ThreadPool::initialize (0);
	}

	UserData	userData;
	Resources::instance ( )._userConfigURL	= userData.getHome ( ) + "/.magix3d";
	Resources::instance ( )._userLastVersionURL = userData.getHome ( ) + "/.magix3d_version";

	parseArgs (argc, argv);
	if (true == displayHelp){
		syntax (argv [0]);
// Une exception est levée par Context::initialize en cas d'argument invalide.
//		std::exit (EXIT_SUCCESS);
	}
	
	if (false == _appIcon.empty ( ))
		setWindowIcon (QIcon (_appIcon.c_str ( )));

	// On essaie d'amméliorer la complétion du binding swig dans la console python :
	QtPythonConsole::enableSwigCompletion	= true;

	// Chargement de la configuration :
	unique_ptr<Section>	config;
	try
	{
		config.reset (getPersistantConfiguration ( ));
	}
	catch (const Exception& exc)
	{
		cout << "Exception lors du chargement de la configuration :\n"
		     << exc.getMessage ( ) << endl;
	}
	catch (...)
	{
		cout << "Exception lors du chargement de la configuration :\n"
		     << "Erreur non documentée." << endl;
	}
	if (0 == config.get ( ))
		config.reset (new Section (Resources::instance ( )._mgx3DRootSectionName));

	// On applique la configuration :
	applyConfiguration (*(config.get ( )));

	// On surcharge éventuellement avec les options de ligne de commande :
	try
	{

	if ((unsigned long)-1 != graphicalWindowWidth)
		Resources::instance ( )._graphicalWindowWidth.setValue (graphicalWindowWidth);
	if ((unsigned long)-1 != graphicalWindowHeight)
		Resources::instance ( )._graphicalWindowHeight.setValue (graphicalWindowHeight);
	if (false != graphicalWindowFixedSize)
		Resources::instance ( )._graphicalWindowFixedSize.setValue (true);
	if (stillFrameRate > 0.)
		Resources::instance ( )._stillFrameRate.setValue (stillFrameRate);
	if (desiredFrameRate > 0.)
		Resources::instance ( )._desiredFrameRate.setValue (desiredFrameRate);

	// On applique la configuration :
//	applyConfiguration (*(config.get ( )));

	}
	catch (const Exception& exc)
	{
		cout << "Exception lors de la mise en application de la configuration :"
		     << "\n" << exc.getMessage ( ) << endl;
	}
	catch (...)
	{
		cout << "Exception lors de la mise en application de la configuration :"
		     << "\nErreur non documentée." << endl;
	}

	// Initialisation de l'aide en ligne :
	QtHelpWindow::initialize (HelpSystem::instance ( ).rootURL, HelpSystem::instance ( ).indexURL, "Magix 3D : Aide en ligne", "Magix 3D");
}	// QtMgx3DApplication::init


void QtMgx3DApplication::getArgs (int* argc, char*** argv)
{
	if (0 != argc)
		*argc	= _argc;
	if (0 != argv)
		*argv	= _argv;
}	// QtMgx3DApplication::getArgs


char** QtMgx3DApplication::getEnv ( )
{
	return _envp;
}	// QtMgx3DApplication::getEnv


QtMgx3DApplication& QtMgx3DApplication::instance ( )
{
	if (0 != _instance)
		return *_instance;

	throw Exception (UTF8String ("QtMgx3DApplication::instance : classe non instanciée.", Charset::UTF_8));
}	// QtMgx3DApplication::instance


QtMgx3DMainWindow& QtMgx3DApplication::getMainWindow ( )
{
	QWidgetList				list		= instance ( ).topLevelWidgets ( );
    QWidgetList::iterator	it			= list.begin ( );
    QWidget*                widget		= 0;
    QtMgx3DMainWindow*		mw			= 0;

    while ((list.end ( ) != it) && (0 != (widget = *it)))
    {
		QtMgx3DMainWindow*	w	= dynamic_cast<QtMgx3DMainWindow*>(widget);
        if ((0 != mw) && (0 != w))
			throw Exception ("QtMgx3DApplication::getMainWindow : l'application a plusieurs fenêtres principales.");
		else if (0 != w)
			mw	= w;
		
        ++it;
    }   // while ((0 != (widget = it.current ( ))) && (0 == explorer))

	if (0 == mw)
		throw Exception ("QtMgx3DApplication::getMainWindow : l'application n'a pas de fenêtre principale.");
		
    return *mw;
}	// QtMgx3DApplication::getMainWindow


string QtMgx3DApplication::getAppTitle ( )
{
	return Resources::instance ( )._softwareName + std::string(" !"); 
}	// QtMgx3DApplication::getAppTitle


const string& QtMgx3DApplication::getAppIcon ( )
{
	return _appIcon;
}	// QtMgx3DApplication::getAppIcon


bool QtMgx3DApplication::displayUpdatesErrors ( )
{
	return Resources::instance ( )._displayUpdatesErrors;
}	// QtMgx3DApplication::displayUpdatesErrors


void QtMgx3DApplication::processEvents (unsigned long maxtime)
{
	QApplication::processEvents (QEventLoop::AllEvents, maxtime);
}	// QtMgx3DApplication::processEvents


void QtMgx3DApplication::flushEventQueue ( )
{
	QApplication::processEvents ( );
}	// QtMgx3DApplication::flushEventQueue


void QtMgx3DApplication::parseArgs (int& argc, char* argv [])
{
	displayHelp				= Context::getArguments ( ).hasArg ("-help") || Context::getArguments ( ).hasArg ("--help");
	graphicalWindowFixedSize	= Context::getArguments ( ).hasArg ("-graphicalWindowFixedSize");
	if (true == Context::getArguments ( ).hasArg ("-icon"))
		_appIcon	= Context::getArguments ( ).getArgValue ("-icon");
	if (true == Context::getArguments ( ).hasArg ("-defaultConfig"))
		Resources::instance ( )._defaultConfigURL		= Context::getArguments ( ).getArgValue ("-defaultConfig");
	if (true == Context::getArguments ( ).hasArg ("-userConfig"))
		Resources::instance ( )._userConfigURL			= Context::getArguments ( ).getArgValue ("-userConfig");
	if (true == Context::getArguments ( ).hasArg ("-userConfigPatchs"))
		Resources::instance ( )._userConfigPatchsURL		= Context::getArguments ( ).getArgValue ("-userConfigPatchs");
	if (true == Context::getArguments ( ).hasArg ("-helpURL"))
		HelpSystem::instance ( ).rootURL		= Context::getArguments ( ).getArgValue ("-helpURL");
	if (true == Context::getArguments ( ).hasArg ("-historiqueURL"))
		HelpSystem::instance ( ).historiqueURL	= Context::getArguments ( ).getArgValue ("-historiqueURL");
	if (true == Context::getArguments ( ).hasArg ("-wikiURL"))
		HelpSystem::instance ( ).wikiURL	= Context::getArguments ( ).getArgValue ("-wikiURL");
	if (true == Context::getArguments ( ).hasArg ("-qualifURL"))
		HelpSystem::instance ( ).qualifURL		= Context::getArguments ( ).getArgValue ("-qualifURL");
	if (true == Context::getArguments ( ).hasArg ("-userManual"))
		HelpSystem::instance ( ).userManual		= Context::getArguments ( ).getArgValue ("-userManual");
	if (true == Context::getArguments ( ).hasArg ("-docViewer"))
		HelpSystem::instance ( ).docViewer	= Context::getArguments ( ).getArgValue ("-docViewer");
	if (true == Context::getArguments ( ).hasArg ("-graphicalWindowWidth"))
		graphicalWindowWidth		= Context::getArguments ( ).getArgULongValue ("-graphicalWindowWidth");
	else if (true == Context::getArguments ( ).hasArg ("--graphicalWindowWidth"))
		graphicalWindowWidth		= Context::getArguments ( ).getArgULongValue ("--graphicalWindowWidth");
	if (true == Context::getArguments ( ).hasArg ("-graphicalWindowHeight"))
		graphicalWindowHeight		= Context::getArguments ( ).getArgULongValue ("-graphicalWindowHeight");
	else if (true == Context::getArguments ( ).hasArg ("--graphicalWindowHeight"))
		graphicalWindowHeight		= Context::getArguments ( ).getArgULongValue ("--graphicalWindowHeight");
	if ((true == Context::getArguments ( ).hasArg ("-stillFrameRate")) || (true == Context::getArguments ( ).hasArg ("--stillFrameRate")))
	{
		if (true == Context::getArguments ( ).hasArg ("-stillFrameRate"))
			stillFrameRate	= Context::getArguments ( ).getArgDoubleValue ("-stillFrameRate");
		else
			stillFrameRate	= Context::getArguments ( ).getArgDoubleValue ("--stillFrameRate");
		if (stillFrameRate < 0.)
			cout << "Option -stillFrameRate invalide : nombre d'images "
			     << "par seconde négatif (" << stillFrameRate << ")." << endl;
	}	// if (true == Context::getArguments ( ).hasArg ("-stillFrameRate"))
	if ((true == Context::getArguments ( ).hasArg ("-desiredFrameRate")) || (true == Context::getArguments ( ).hasArg ("--desiredFrameRate")))
	{
		if (true == Context::getArguments ( ).hasArg ("-desiredFrameRate"))
			desiredFrameRate	= Context::getArguments ( ).getArgDoubleValue ("-desiredFrameRate");
		else
			desiredFrameRate	= Context::getArguments ( ).getArgDoubleValue ("--desiredFrameRate");
		if (desiredFrameRate < 0.)
			cout << "Option -desiredFrameRate invalide : nombre d'images "
			     << "par seconde négatif (" << desiredFrameRate << ")." << endl;
	}	// if (true == Context::getArguments ( ).hasArg ("-desiredFrameRate"))
	Resources::instance ( )._scripts	= Context::getArguments ( ).getArgValues ("-script", false);
	vector<string>	scripts	= Context::getArguments ( ).getArgValues ("--script", false);
	for (vector<string>::const_iterator its = scripts.begin ( ); scripts.end ( ) != its; its++)
		Resources::instance ( )._scripts.push_back (*its);
}	// QtMgx3DApplication::parseArgs


void QtMgx3DApplication::syntax (const string& exe) const
{
	cout << "Syntaxe :" << endl
	     << exe << "[-h][-defaultConfig fichier][-userConfig fichier]"
	     << "[-outCharsetRef àéèùô][-outCharset charset][-icon fichier]"
	     << "[-graphicalWindowWidth largeur][-graphicalWindowHeight hauteur]"
	     << "[-graphicalWindowFixedSize]"
		 << "[-useOCAF]"
	     << "[-script file1.py][-script file2.py] ... [-script filen.py]"
	     << endl << endl
	     << " -h                                   : affiche ce message" <<endl
	     << " -defaultConfig fichier               : "
			<< "utilise fichier comme fichier de configuration générale"<< endl
	     << " -userConfig fichier                  : "
			<<"utilise fichier comme fichier de configuration personnelle"<<endl
	     << " -outCharsetRef àéèùô                 : "
			<<"transmettre la chaîne \"àéèùô\" en argument pour détection automatique du jeu de caractères utilisé par la console"<<endl
	     << " -outCharset jeu de caractères        : "
			<<"utilise le jeu de caractères transmis pour les sorties standard (UTF-8/UTF-16/ISO-8859/ASCII)"<<endl
	     << "-icon fichier                         : utilise pour icône de l'application celle contenue dans fichier" << endl
	     << "-graphicalWindowWidth largeur         : "
			<< "affecte largeur à la largeur de la fenêtre graphique" << endl
	     << "-graphicalWindowHeight hauteur        : "
			<< "affecte hauteur à la hauteur de la fenêtre graphique" << endl
	     << "-graphicalWindowFixedSize             : "
			<< "la fenêtre graphique ne peut pas être redimensionnée" << endl
		<< " -useOCAF : utilisation d'OCAF pour la gestion du noyau gémétrique."<<endl
		<< " -scripts fichier python               : exécute le fichier "
			<< "python transmis en arguments au lancement de l'application." <<endl
	     << endl << endl;
}	// QtMgx3DApplication::syntax


void QtMgx3DApplication::applyConfiguration (const Section& mainSection)
{
	try
	{	// Théâtre
		Section&	guiSection		= mainSection.getSection ("gui");
		Section&	theatreSection	= guiSection.getSection ("theatre");
		PreferencesHelper::getUnsignedLong (theatreSection, Resources::instance ( )._updateRefreshRate);
		PreferencesHelper::getBoolean (theatreSection, Resources::instance ( )._useDisplayList);
		PreferencesHelper::getBoolean (theatreSection, Resources::instance ( )._xyzCancelRoll);
		PreferencesHelper::getDouble (theatreSection, Resources::instance ( )._stillFrameRate);
		PreferencesHelper::getDouble (theatreSection, Resources::instance ( )._desiredFrameRate);
		PreferencesHelper::getColor (theatreSection, Resources::instance ( )._background);
		PreferencesHelper::getBoolean (theatreSection, Resources::instance ( )._multipleLighting);
		PreferencesHelper::getBoolean (theatreSection, Resources::instance ( )._displayTrihedron);
		PreferencesHelper::getBoolean (theatreSection, Resources::instance ( )._displayFocalPoint);
		PreferencesHelper::getUnsignedLong (theatreSection, Resources::instance ( )._graphicalWindowWidth);
		PreferencesHelper::getUnsignedLong (theatreSection, Resources::instance ( )._graphicalWindowHeight);
		PreferencesHelper::getBoolean (theatreSection, Resources::instance ( )._graphicalWindowFixedSize);
		Section&	representationsSection	= theatreSection.getSection ("representations");
		Section&	geomSection	= representationsSection.getSection ("geomEntities");
		Section&	topoSection	= representationsSection.getSection ("topoEntities");
		Section&	meshSection	= representationsSection.getSection ("meshEntities");
		PreferencesHelper::getString (representationsSection, Resources::instance ( )._fontFamily);
		DisplayProperties::_defaultFontFamily	= fontNameToInt (Resources::instance ( )._fontFamily.getValue ( ));
		PreferencesHelper::getUnsignedLong (representationsSection, Resources::instance ( )._fontSize);
		DisplayProperties::_defaultFontSize	= Resources::instance ( )._fontSize.getValue ( );
		PreferencesHelper::getBoolean (representationsSection, Resources::instance ( )._fontBold);
		DisplayProperties::_defaultFontBold	= Resources::instance ( )._fontBold.getValue ( );
		PreferencesHelper::getBoolean (representationsSection, Resources::instance ( )._fontItalic);
		DisplayProperties::_defaultFontItalic	= Resources::instance ( )._fontItalic.getValue ( );
		PreferencesHelper::getBoolean (geomSection, Resources::instance ( )._geomDestroyOnHide);
		PreferencesHelper::getBoolean (topoSection, Resources::instance ( )._topoDestroyOnHide);
		PreferencesHelper::getBoolean (meshSection, Resources::instance ( )._meshDestroyOnHide);

		Internal::InternalPreferences::instance ( ).loadPreferences(representationsSection);
	}
	catch (...)	
	{
	}

	try
	{	// Interacteur
		Section&	guiSection	= mainSection.getSection ("gui");
		Section&	intSection	= guiSection.getSection ("interactor");
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._mouseUpZoom);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._zoomOnWheel);
		PreferencesHelper::getColor (intSection, Resources::instance ( )._selectionColor);
		PreferencesHelper::getDouble (intSection, Resources::instance ( )._selectionPointSize);
		PreferencesHelper::getDouble (intSection, Resources::instance ( )._selectionLineWidth);
		PreferencesHelper::getString (intSection, Resources::instance ( )._nextSelectionKey);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._selectionOnTop);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._pickOnLeftButtonDown);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._pickOnRightButtonDown);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._pickOnLeftButtonUp);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._pickOnRightButtonUp);
		PreferencesHelper::getDouble (intSection, Resources::instance ( )._marginBoundingBoxPercentage);
		PreferencesHelper::getColor (intSection, Resources::instance ( )._highlightColor);
		PreferencesHelper::getDouble (intSection, Resources::instance ( )._highLightWidth);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._highLightBoundinBox);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._useHighLightColor);
		PreferencesHelper::getColor (intSection, Resources::instance ( )._previewColor);
		PreferencesHelper::getDouble (intSection, Resources::instance ( )._previewWidth);
		PreferencesHelper::getDouble (intSection, Resources::instance ( )._previewPointSize);
	}
	catch (...)
	{
	}

	try
	{	// Panneaux
		Section&	guiSection		= mainSection.getSection ("gui");
		Section&	panelsSection	= guiSection.getSection ("panels");
		Section&	operationSection= panelsSection.getSection("operation");
		PreferencesHelper::getLong (guiSection, Resources::instance ( )._margin);
		PreferencesHelper::getLong (guiSection, Resources::instance ( )._spacing);
		PreferencesHelper::getColor (operationSection, Resources::instance ( )._IDSeizureBackgroundColor);
		PreferencesHelper::getBoolean (operationSection, Resources::instance ( )._prevalidateSeizure);
		PreferencesHelper::getBoolean (operationSection, Resources::instance ( )._cancelClearEntities);
		PreferencesHelper::getBoolean (operationSection, Resources::instance ( )._autoUpdateUsesSelection);
		PreferencesHelper::getBoolean (operationSection, Resources::instance ( )._showDialogOnCommandError);
		PreferencesHelper::getBoolean (operationSection, Resources::instance ( )._showAmodalDialogOnCommandError);
		PreferencesHelper::getUnsignedLong (operationSection, Resources::instance ( )._commandNotificationDelay);
		EntitySeizureManager::interactiveModeBackground.setRedF (Resources::instance ( )._IDSeizureBackgroundColor.getRed ( ));
		EntitySeizureManager::interactiveModeBackground.setGreenF (Resources::instance ( )._IDSeizureBackgroundColor.getGreen ( ));
		EntitySeizureManager::interactiveModeBackground.setBlueF (Resources::instance ( )._IDSeizureBackgroundColor.getBlue ( ));

		Section&	selectionSection	= panelsSection.getSection("selection");
		PreferencesHelper::getUnsignedLong (selectionSection, Resources::instance ( )._maxIndividualProperties);
		PreferencesHelper::getUnsignedLong (selectionSection, Resources::instance ( )._maxCommonProperties);

		Section&	groupsSection	= panelsSection.getSection("groups");
		PreferencesHelper::getBoolean (groupsSection, Resources::instance ( )._automaticSort);
		PreferencesHelper::getString (groupsSection, Resources::instance ( )._sortType);
		PreferencesHelper::getBoolean (groupsSection, Resources::instance ( )._multiLevelGroupsView);
	}
	catch (...)
	{
	}

	// Vérification de la cohérence : au plus un des 4 booléens suivants peut
	// être vrai.
	int	trueCount	= 0;
	if (true == Resources::instance ( )._pickOnLeftButtonDown.getValue ( ))
		trueCount	+= 1;
	if (true == Resources::instance ( )._pickOnRightButtonDown.getValue ( ))
		trueCount	+= 1;
	if (true == Resources::instance ( )._pickOnLeftButtonUp.getValue ( ))
		trueCount	+= 1;
	if (true == Resources::instance ( )._pickOnRightButtonUp.getValue ( ))
		trueCount	+= 1;
	if (1 < trueCount)
	{
		try
		{

		cerr << "\a\a\aParmi les 4 booléens suivants en configuration, au plus "
		     << "un d'entre-eux peut être vrai. Au moins deux le sont. "
		     << "Affectation de false à l'ensemble de ces booléens.\n"
		     << "Booléens incriminés : " << Resources::instance ( )._pickOnLeftButtonDown.getName ( )
		     << ", " << Resources::instance ( )._pickOnRightButtonDown.getName ( )
		     << ", " << Resources::instance ( )._pickOnLeftButtonUp.getName ( )
		     << ", " << Resources::instance ( )._pickOnRightButtonUp.getName ( ) << endl;
			Resources::instance ( )._pickOnLeftButtonDown.setValue (false);
			Resources::instance ( )._pickOnRightButtonDown.setValue (false);
			Resources::instance ( )._pickOnLeftButtonUp.setValue (false);
			Resources::instance ( )._pickOnRightButtonUp.setValue (false);
		}
		catch (...)
		{ }
	}	// if (1 < trueCount)

	try
	{	// Logs
		Section&	guiSection	= mainSection.getSection ("gui");
		Section&	intSection	= guiSection.getSection ("logs");
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._logInformations);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._logErrors);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._logWarnings);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._logTaskOutputs);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._logTraces_1);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._logTraces_2);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._logTraces_3);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._logTraces_4);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._logTraces_5);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._logDate);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._logTime);
		PreferencesHelper::getBoolean (intSection, Resources::instance ( )._logThreadID);
		// Actualisation du contexte :
		Context::setLogsMask (getLogsMask ( ));
	}
	catch (...)
	{
	}

	try
	{	// Scripting
		Section&	guiSection			= mainSection.getSection ("gui");
		Section&	guiScriptingSection	= guiSection.getSection ("scripting");
		PreferencesHelper::getBoolean (guiSection, Resources::instance ( )._saveGuiState);
		PreferencesHelper::getBoolean (guiSection, Resources::instance ( )._loadGuiState);
		PreferencesHelper::getUnsignedLong (guiScriptingSection, Resources::instance ( )._recentScriptCapacity);
		PreferencesHelper::getString (guiScriptingSection, Resources::instance ( )._scriptsCharset);
	}
	catch (...)	
	{
	}

	try
	{
		// Contexte :
		Section&	contextSection	= mainSection.getSection ("context");
		Preferences::Section&	threadingSection	= Preferences::PreferencesHelper::getSection (contextSection, "threading");
		Preferences::Section&	scriptingSection	= Preferences::PreferencesHelper::getSection (contextSection, "scripting");
		Preferences::Section&	optimizingSection	= Preferences::PreferencesHelper::getSection (contextSection, "optimizing");
		PreferencesHelper::getBoolean (threadingSection, Resources::instance ( )._allowThreadedCommandTasks);
		PreferencesHelper::getBoolean (threadingSection, Resources::instance ( )._allowThreadedEdgePreMeshTasks);
		PreferencesHelper::getBoolean (threadingSection, Resources::instance ( )._allowThreadedFacePreMeshTasks);
		PreferencesHelper::getBoolean (threadingSection, Resources::instance ( )._allowThreadedBlockPreMeshTasks);
		PreferencesHelper::getBoolean (scriptingSection, Resources::instance ( )._displayScriptOutputs);
		PreferencesHelper::getBoolean (optimizingSection, Resources::instance ( )._memorizeEdgePreMesh);
	}
	catch (...)
	{
	}
}	// QtMgx3DApplication::applyConfiguration


void QtMgx3DApplication::editConfiguration (QWidget* parent, const UTF8String& title)
{
	Section*	section	= new Section (Resources::instance ( )._mgx3DRootSectionName);
	CHECK_NULL_PTR_ERROR (section)
	instance ( ).saveConfiguration (*section);
	QtMgx3DPreferencesDialog	dialog (
			parent, section, Resources::instance ( )._userConfigURL, true, true, true, title,
			UTF8String ("Magix 3D", Charset::UTF_8));
	dialog.exec ( );
}	// QtMgx3DApplication::editConfiguration


Section* QtMgx3DApplication::getPersistantConfiguration ( )
{
	unique_ptr<Section>	config;

	if ((0 != Resources::instance ( )._defaultConfigURL.length ( )) || (0 != Resources::instance ( )._userConfigURL.length ( )))
	{
		try
		{
			if (0 != Resources::instance ( )._defaultConfigURL.length ( ))
			    config.reset (XMLLoader::load (Resources::instance ( )._defaultConfigURL));
		}
		catch (const Exception& exc)
		{
			cout << "Exception lors du chargement de la configuration depuis "
			     << "le fichier " << Resources::instance ( )._defaultConfigURL << " :\n"
			     << exc.getMessage ( ) << endl;
		}
		catch (...)
		{
			cout << "Exception lors du chargement de la configuration depuis "
			     << "le fichier " << Resources::instance ( )._defaultConfigURL << " :\n"
			     << "Erreur non documentée." << endl;
		}
		Section*	s	= config.release ( );
		try
		{
			if (0 != Resources::instance ( )._userConfigURL.length ( ))
			{
				// On effectue une actualisation préalable :
				if (0 != Resources::instance ( )._userConfigPatchsURL.length ( ))
				{
					bool	isExtendedMode	= false;
					string	extension ("undefined extension");
					UTF8String	context (Charset::UTF_8);
					context << "Magix3D propose d'effectuer une mise à jour automatique de certaines préférences utilisateur jugées importantes";
					context << ", l'une d'entre-elles (spotTolerance) influant sur la qualité des tâches laser affichées.";
					UTF8String	modifications (Charset::UTF_8);
					size_t			count	=
						QtPrefsUpdater::update (
							Resources::instance ( )._userConfigURL, Resources::instance ( )._userConfigPatchsURL, isExtendedMode?extension.c_str ( ):"Magix3D",
							context.format (100), true, modifications);
					if (0 != count)
						QtMessageBox::displayInformationMessage (0, isExtendedMode?extension.c_str ( ):"Magix3D", modifications);
				}	// if (0 != Resources::instance ( )._userConfigPatchsURL.length ( ))

				Section*	s	= config.release ( );
			    config.reset (XMLLoader::load (Resources::instance ( )._userConfigURL, s));
			}
		}
		catch (const Exception& exc)
		{
			cout << "Exception lors du chargement de la configuration depuis "
			     << "le fichier " << Resources::instance ( )._userConfigURL << " :\n"
			     << exc.getMessage ( ) << endl;
		}
		catch (...)
		{
			cout << "Exception lors du chargement de la configuration depuis "
			     << "le fichier " << Resources::instance ( )._userConfigURL << " :\n"
			     << "Erreur non documentée." << endl;
		}
		if (0 == config.get ( )) // Exception levée car pas de fichier perso ???
			config.reset (s);
		s	= 0;
	}	// if ((0 != Resources::instance ( )._defaultConfigURL.length ( )) || ...

	return config.release ( );
}	// QtMgx3DApplication::getPersistantConfiguration


void QtMgx3DApplication::setPersistantConfiguration (const Section& mainSection)
{
	if (0 != Resources::instance ( )._userConfigURL.length ( ))
	{
		XMLLoader::ENCODING	encoding	= XMLLoader::UTF_16;
		if (true == mainSection.isIso ( ))
			encoding	= XMLLoader::ISO_8859 ;
		else if (true == mainSection.isAscii ( ))
			encoding	= XMLLoader::UTF_8;
	    XMLLoader::save (mainSection, Resources::instance ( )._userConfigURL, encoding);
	}	// if (0 != _userConfigURL.length ( ))
}	// QtMgx3DApplication::setPersistantConfiguration


void QtMgx3DApplication::updateConfiguration (Section& section, NamedValue& value)
{
	Section&	guiSection	= PreferencesHelper::getSection (section,"gui");
	if  (Resources::instance ( )._loadGuiState.getName ( ) == value.getName ( ))
		PreferencesHelper::updateBoolean (guiSection, Resources::instance ( )._loadGuiState);
	else if  (Resources::instance ( )._saveGuiState.getName ( ) == value.getName ( ))
		PreferencesHelper::updateBoolean (guiSection, Resources::instance ( )._saveGuiState);
	else
	{
		UTF8String	message (Charset::UTF_8);
		message << "Actualisation de la préférence utilisateur "
		        << value.getName ( ) << " non prévue.";
		throw Exception (message);
	}
}	// QtMgx3DApplication::updateConfiguration


void QtMgx3DApplication::saveConfiguration (Section& mainSection)
{
	Section&	contextSection	= PreferencesHelper::getSection (mainSection,"context");
	Section&	guiSection		= PreferencesHelper::getSection (mainSection,"gui");
	Section&	theatreSection	= PreferencesHelper::getSection (guiSection,"theatre");
	Section&	intSection		= PreferencesHelper::getSection (guiSection,"interactor");
	Section&	panelsSection	= PreferencesHelper::getSection (guiSection,"panels");
	Section&	operationSection= PreferencesHelper::getSection (panelsSection, "operation");
	Section&	selectionSection= PreferencesHelper::getSection (panelsSection, "selection");
	Section&	groupsSection	= PreferencesHelper::getSection (panelsSection, "groups");
	Section&	logsSection		= PreferencesHelper::getSection (guiSection,"logs");
	Section&	guiScriptingSection	= PreferencesHelper::getSection (guiSection,"scripting");

	// GUI :
	PreferencesHelper::updateBoolean (guiSection, Resources::instance ( )._saveGuiState);
	PreferencesHelper::updateBoolean (guiSection, Resources::instance ( )._loadGuiState);

	// Théâtre :
	PreferencesHelper::updateUnsignedLong (theatreSection, Resources::instance ( )._updateRefreshRate);
	PreferencesHelper::updateBoolean (theatreSection, Resources::instance ( )._useDisplayList);
	PreferencesHelper::updateBoolean (theatreSection, Resources::instance ( )._xyzCancelRoll);
	PreferencesHelper::updateDouble (theatreSection, Resources::instance ( )._stillFrameRate);
	PreferencesHelper::updateDouble (theatreSection, Resources::instance ( )._desiredFrameRate);
	PreferencesHelper::updateColor (theatreSection, Resources::instance ( )._background);
	PreferencesHelper::updateBoolean (theatreSection, Resources::instance ( )._multipleLighting);
	PreferencesHelper::updateBoolean (theatreSection, Resources::instance ( )._displayTrihedron);
	PreferencesHelper::updateBoolean (theatreSection, Resources::instance ( )._displayFocalPoint);
	PreferencesHelper::updateUnsignedLong (theatreSection, Resources::instance ( )._graphicalWindowWidth);
	PreferencesHelper::updateUnsignedLong (theatreSection, Resources::instance ( )._graphicalWindowHeight);
	PreferencesHelper::updateBoolean (theatreSection, Resources::instance ( )._graphicalWindowFixedSize);
	Section&	representationsSection	= PreferencesHelper::getSection (theatreSection, "representations");
	Section&	geomSection	= PreferencesHelper::getSection (representationsSection, "geomEntities");
	Section&	topoSection	= PreferencesHelper::getSection (representationsSection, "topoEntities");
	Section&	meshSection	= PreferencesHelper::getSection (representationsSection, "meshEntities");
	PreferencesHelper::updateString (representationsSection, Resources::instance ( )._fontFamily);
	PreferencesHelper::updateUnsignedLong (representationsSection, Resources::instance ( )._fontSize);
	PreferencesHelper::updateBoolean (representationsSection, Resources::instance ( )._fontBold);
	PreferencesHelper::updateBoolean (representationsSection, Resources::instance ( )._fontItalic);
	PreferencesHelper::updateBoolean (geomSection, Resources::instance ( )._geomDestroyOnHide);
	PreferencesHelper::updateBoolean (topoSection, Resources::instance ( )._topoDestroyOnHide);
	PreferencesHelper::updateBoolean (meshSection, Resources::instance ( )._meshDestroyOnHide);
	Internal::InternalPreferences::instance ( ).savePreferences(representationsSection);

	// Interacteur :
	PreferencesHelper::updateBoolean (intSection, Resources::instance ( )._mouseUpZoom);
	PreferencesHelper::updateBoolean (intSection, Resources::instance ( )._zoomOnWheel);
	PreferencesHelper::updateColor (intSection, Resources::instance ( )._selectionColor);
	PreferencesHelper::updateDouble (intSection, Resources::instance ( )._selectionPointSize);
	PreferencesHelper::updateDouble (intSection, Resources::instance ( )._selectionLineWidth);
	PreferencesHelper::updateString (intSection, Resources::instance ( )._nextSelectionKey);
	PreferencesHelper::updateBoolean (intSection, Resources::instance ( )._selectionOnTop);
	PreferencesHelper::updateBoolean (intSection, Resources::instance ( )._pickOnLeftButtonDown);
	PreferencesHelper::updateBoolean (intSection, Resources::instance ( )._pickOnRightButtonDown);
	PreferencesHelper::updateBoolean (intSection, Resources::instance ( )._pickOnLeftButtonUp);
	PreferencesHelper::updateBoolean (intSection, Resources::instance ( )._pickOnRightButtonUp);
	PreferencesHelper::updateDouble (intSection, Resources::instance ( )._marginBoundingBoxPercentage);
	PreferencesHelper::updateColor (intSection, Resources::instance ( )._highlightColor);
	PreferencesHelper::updateDouble (intSection, Resources::instance ( )._highLightWidth);
	PreferencesHelper::updateBoolean (intSection, Resources::instance ( )._highLightBoundinBox);
	PreferencesHelper::updateBoolean (intSection, Resources::instance ( )._useHighLightColor);
	PreferencesHelper::updateColor (intSection, Resources::instance ( )._previewColor);
	PreferencesHelper::updateDouble (intSection, Resources::instance ( )._previewWidth);
	PreferencesHelper::updateDouble (intSection, Resources::instance ( )._previewPointSize);

	// Panels :
	PreferencesHelper::updateLong (guiSection, Resources::instance ( )._margin);
	PreferencesHelper::updateLong (guiSection, Resources::instance ( )._spacing);
	PreferencesHelper::updateColor (operationSection,Resources::instance ( )._IDSeizureBackgroundColor);
	PreferencesHelper::updateBoolean (operationSection, Resources::instance ( )._prevalidateSeizure);
	PreferencesHelper::updateBoolean (operationSection, Resources::instance ( )._cancelClearEntities);
	PreferencesHelper::updateBoolean (operationSection, Resources::instance ( )._autoUpdateUsesSelection);
	PreferencesHelper::updateBoolean (operationSection, Resources::instance ( )._showDialogOnCommandError);
	PreferencesHelper::updateBoolean (operationSection, Resources::instance ( )._showAmodalDialogOnCommandError);
	PreferencesHelper::updateUnsignedLong (operationSection, Resources::instance ( )._commandNotificationDelay);
	PreferencesHelper::updateUnsignedLong (selectionSection, Resources::instance ( )._maxIndividualProperties);
	PreferencesHelper::updateUnsignedLong (selectionSection, Resources::instance ( )._maxCommonProperties);
	PreferencesHelper::updateBoolean (groupsSection, Resources::instance ( )._automaticSort);
	PreferencesHelper::updateString (groupsSection, Resources::instance ( )._sortType);
	PreferencesHelper::updateBoolean (groupsSection, Resources::instance ( )._multiLevelGroupsView);

	// Logs :
	PreferencesHelper::updateBoolean (logsSection, Resources::instance ( )._logInformations);
	PreferencesHelper::updateBoolean (logsSection, Resources::instance ( )._logWarnings);
	PreferencesHelper::updateBoolean (logsSection, Resources::instance ( )._logErrors);
	PreferencesHelper::updateBoolean (logsSection, Resources::instance ( )._logTaskOutputs);
	PreferencesHelper::updateBoolean (logsSection, Resources::instance ( )._logTraces_1);
	PreferencesHelper::updateBoolean (logsSection, Resources::instance ( )._logTraces_2);
	PreferencesHelper::updateBoolean (logsSection, Resources::instance ( )._logTraces_3);
	PreferencesHelper::updateBoolean (logsSection, Resources::instance ( )._logTraces_4);
	PreferencesHelper::updateBoolean (logsSection, Resources::instance ( )._logTraces_5);
	PreferencesHelper::updateBoolean (logsSection, Resources::instance ( )._logDate);
	PreferencesHelper::updateBoolean (logsSection, Resources::instance ( )._logTime);
	PreferencesHelper::updateBoolean (logsSection, Resources::instance ( )._logThreadID);

	// Scripting :
	PreferencesHelper::updateUnsignedLong (guiScriptingSection, Resources::instance ( )._recentScriptCapacity);
	PreferencesHelper::updateString (guiScriptingSection, Resources::instance ( )._scriptsCharset);

	// Contexte :
	Preferences::Section&	threadingSection	= PreferencesHelper::getSection (contextSection, "threading");
	Preferences::Section&	scriptingSection	= PreferencesHelper::getSection (contextSection, "scripting");
	Preferences::Section&	optimizingSection	= PreferencesHelper::getSection (contextSection, "optimizing");
	PreferencesHelper::updateBoolean (threadingSection, Resources::instance ( )._allowThreadedCommandTasks);
	PreferencesHelper::updateBoolean (threadingSection, Resources::instance ( )._allowThreadedEdgePreMeshTasks);
	PreferencesHelper::updateBoolean (threadingSection, Resources::instance ( )._allowThreadedFacePreMeshTasks);
	PreferencesHelper::updateBoolean (threadingSection, Resources::instance ( )._allowThreadedBlockPreMeshTasks);
	PreferencesHelper::updateBoolean (scriptingSection, Resources::instance ( )._displayScriptOutputs);
	PreferencesHelper::updateBoolean (optimizingSection, Resources::instance ( )._memorizeEdgePreMesh);
}	// QtMgx3DApplication::saveConfiguration


unsigned short QtMgx3DApplication::fontNameToInt (const string& name)
{
	return 0;
}	// QtMgx3DApplication::fontNameToInt


Log::TYPE QtMgx3DApplication::getLogsMask ( )
{
	Log::TYPE	mask	= Log::NONE;

	if (true == Resources::instance ( )._logInformations)
		mask	|= Log::INFORMATION;
	if (true == Resources::instance ( )._logWarnings)
		mask	|= Log::WARNING;
	if (true == Resources::instance ( )._logErrors)
		mask	|= Log::ERROR;
	if (true == Resources::instance ( )._logTaskOutputs)
		mask	|= Log::PROCESS;
	if (true == Resources::instance ( )._logTraces_1)
		mask	|= Log::TRACE_1;
	if (true == Resources::instance ( )._logTraces_2)
		mask	|= Log::TRACE_2;
	if (true == Resources::instance ( )._logTraces_3)
		mask	|= Log::TRACE_3;
	if (true == Resources::instance ( )._logTraces_4)
		mask	|= Log::TRACE_4;
	if (true == Resources::instance ( )._logTraces_5)
		mask	|= Log::TRACE_5;

	return mask;
}	// QtMgx3DApplication::getLogsMask


TkUtil::Version QtMgx3DApplication::getLastVersion ( )
{
	// ouverture du fichier avec la chaine de caractère de la dernière version
	std::ifstream ifile(Resources::instance ( )._userLastVersionURL);
	if (true == ifile.eof ( ))
		return TkUtil::Version("0.0.0");
	std::string numero;
	std::getline(ifile, numero);
	if (numero == "")
		return TkUtil::Version("0.0.0");
	return TkUtil::Version(numero);
}

void QtMgx3DApplication::setLastVersion (const TkUtil::Version& version)
{
	std::ofstream ofile(Resources::instance ( )._userLastVersionURL);
	ofile << version.getVersion();
	ofile.close();
}

TkUtil::Version QtMgx3DApplication::getVersion ( )
{
    if (std::string("dev") == MAGIX3D_VERSION)	// GSCC_PROJECT_VERSION)
        return TkUtil::Version("0.0.0");
    else
    	return TkUtil::Version (MAGIX3D_VERSION);	// GSCC_PROJECT_VERSION);
}

// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
