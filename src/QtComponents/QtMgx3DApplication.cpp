/**
 * \file        QtMgx3DApplication.cpp
 * \author      Charles PIGNEROL
 * \date        29/11/2010
 */

#include "mgx_config.h"

#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/EntitySeizureManager.h"
#include "QtComponents/QtMgx3DPreferencesDialog.h"
#include "Utils/Common.h"
#include "Utils/DisplayProperties.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/NetworkData.h>
#include <TkUtil/RemoteProcess.h>
#include <TkUtil/ThreadManager.h>
#include <TkUtil/ThreadPool.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/UserData.h>

#include <QtUtil/QtAutoWaitingCursor.h>
#include <QtUtil/QtErrorManagement.h>
#include <QtUtil/QtHelpWindow.h>
#include <QtUtil/QtStringHelper.h>
#include <QtUtil/QtValidatedTextField.h>

#include <PrefsCore/PreferencesHelper.h>
#include <PrefsQt/QtPrefsUpdater.h>
#include <QtPython3/QtPython.h>
#include <QtPython3/QtPythonConsole.h>
#include <QwtCharts/QwtChartsManager.h>
#include <PrefsXerces/XMLLoader.h>

#include <QTextCodec>

#include "QtComponents/GUIResources.h"

using namespace std;
using namespace TkUtil;
using namespace Preferences;
using namespace Mgx3D;
using namespace Utils;


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
	Controller::finalize ( );
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
	// REM : QtPython::preInitialize ( ) doit avoir été appelé auparavant, et avant Py_Initialize. C'est actuellement fait en début de main.
	QtPython::initialize (Controller::getOutCharset ( ));
	Process::initialize (argc, argv, envp);
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
	GUIResources::instance ( )._userConfigURL	= userData.getHome ( ) + "/.magix3d";
	GUIResources::instance ( )._userLastVersionURL = userData.getHome ( ) + "/.magix3d_version";

	parseArgs (argc, argv);
	if (true == displayHelp){
		syntax (argv [0]);
// Une exception est levée par Controller::initialize en cas d'argument invalide.
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
		config.reset (new Section (GUIResources::instance ( )._mgx3DRootSectionName));

	// On applique la configuration :
	applyConfiguration (*(config.get ( )));

	// On surcharge éventuellement avec les options de ligne de commande :
	try
	{

	if ((unsigned long)-1 != graphicalWindowWidth)
		GUIResources::instance ( )._graphicalWindowWidth.setValue (graphicalWindowWidth);
	if ((unsigned long)-1 != graphicalWindowHeight)
		GUIResources::instance ( )._graphicalWindowHeight.setValue (graphicalWindowHeight);
	if (false != graphicalWindowFixedSize)
		GUIResources::instance ( )._graphicalWindowFixedSize.setValue (true);
	if (stillFrameRate > 0.)
		GUIResources::instance ( )._stillFrameRate.setValue (stillFrameRate);
	if (desiredFrameRate > 0.)
		GUIResources::instance ( )._desiredFrameRate.setValue (desiredFrameRate);

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
	return GUIResources::instance ( )._softwareName + std::string(" !");
}	// QtMgx3DApplication::getAppTitle


const string& QtMgx3DApplication::getAppIcon ( )
{
	return _appIcon;
}	// QtMgx3DApplication::getAppIcon


bool QtMgx3DApplication::displayUpdatesErrors ( )
{
	return GUIResources::instance ( )._displayUpdatesErrors;
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
	displayHelp				= Controller::getArguments ( ).hasArg ("-help") || Controller::getArguments ( ).hasArg ("--help");
	graphicalWindowFixedSize	= Controller::getArguments ( ).hasArg ("-graphicalWindowFixedSize");
	if (true == Controller::getArguments ( ).hasArg ("-icon"))
		_appIcon	= Controller::getArguments ( ).getArgValue ("-icon");
	if (true == Controller::getArguments ( ).hasArg ("-defaultConfig"))
		GUIResources::instance ( )._defaultConfigURL		= Controller::getArguments ( ).getArgValue ("-defaultConfig");
	if (true == Controller::getArguments ( ).hasArg ("-userConfig"))
		GUIResources::instance ( )._userConfigURL			= Controller::getArguments ( ).getArgValue ("-userConfig");
	if (true == Controller::getArguments ( ).hasArg ("-userConfigPatchs"))
		GUIResources::instance ( )._userConfigPatchsURL		= Controller::getArguments ( ).getArgValue ("-userConfigPatchs");
	if (true == Controller::getArguments ( ).hasArg ("-helpURL"))
		HelpSystem::instance ( ).rootURL		= Controller::getArguments ( ).getArgValue ("-helpURL");
	if (true == Controller::getArguments ( ).hasArg ("-historiqueURL"))
		HelpSystem::instance ( ).historiqueURL	= Controller::getArguments ( ).getArgValue ("-historiqueURL");
	if (true == Controller::getArguments ( ).hasArg ("-wikiURL"))
		HelpSystem::instance ( ).wikiURL	= Controller::getArguments ( ).getArgValue ("-wikiURL");
	if (true == Controller::getArguments ( ).hasArg ("-qualifURL"))
		HelpSystem::instance ( ).qualifURL		= Controller::getArguments ( ).getArgValue ("-qualifURL");
	if (true == Controller::getArguments ( ).hasArg ("-userManual"))
		HelpSystem::instance ( ).userManual		= Controller::getArguments ( ).getArgValue ("-userManual");
	if (true == Controller::getArguments ( ).hasArg ("-docViewer"))
		HelpSystem::instance ( ).docViewer	= Controller::getArguments ( ).getArgValue ("-docViewer");
	if (true == Controller::getArguments ( ).hasArg ("-graphicalWindowWidth"))
		graphicalWindowWidth		= Controller::getArguments ( ).getArgULongValue ("-graphicalWindowWidth");
	else if (true == Controller::getArguments ( ).hasArg ("--graphicalWindowWidth"))
		graphicalWindowWidth		= Controller::getArguments ( ).getArgULongValue ("--graphicalWindowWidth");
	if (true == Controller::getArguments ( ).hasArg ("-graphicalWindowHeight"))
		graphicalWindowHeight		= Controller::getArguments ( ).getArgULongValue ("-graphicalWindowHeight");
	else if (true == Controller::getArguments ( ).hasArg ("--graphicalWindowHeight"))
		graphicalWindowHeight		= Controller::getArguments ( ).getArgULongValue ("--graphicalWindowHeight");
	if ((true == Controller::getArguments ( ).hasArg ("-stillFrameRate")) || (true == Controller::getArguments ( ).hasArg ("--stillFrameRate")))
	{
		if (true == Controller::getArguments ( ).hasArg ("-stillFrameRate"))
			stillFrameRate	= Controller::getArguments ( ).getArgDoubleValue ("-stillFrameRate");
		else
			stillFrameRate	= Controller::getArguments ( ).getArgDoubleValue ("--stillFrameRate");
		if (stillFrameRate < 0.)
			cout << "Option -stillFrameRate invalide : nombre d'images "
			     << "par seconde négatif (" << stillFrameRate << ")." << endl;
	}	// if (true == Controller::getArguments ( ).hasArg ("-stillFrameRate"))
	if ((true == Controller::getArguments ( ).hasArg ("-desiredFrameRate")) || (true == Controller::getArguments ( ).hasArg ("--desiredFrameRate")))
	{
		if (true == Controller::getArguments ( ).hasArg ("-desiredFrameRate"))
			desiredFrameRate	= Controller::getArguments ( ).getArgDoubleValue ("-desiredFrameRate");
		else
			desiredFrameRate	= Controller::getArguments ( ).getArgDoubleValue ("--desiredFrameRate");
		if (desiredFrameRate < 0.)
			cout << "Option -desiredFrameRate invalide : nombre d'images "
			     << "par seconde négatif (" << desiredFrameRate << ")." << endl;
	}	// if (true == Controller::getArguments ( ).hasArg ("-desiredFrameRate"))
	GUIResources::instance ( )._scripts	= Controller::getArguments ( ).getArgValues ("-script", false);
	vector<string>	scripts	= Controller::getArguments ( ).getArgValues ("--script", false);
	for (vector<string>::const_iterator its = scripts.begin ( ); scripts.end ( ) != its; its++)
		GUIResources::instance ( )._scripts.push_back (*its);
}	// QtMgx3DApplication::parseArgs


void QtMgx3DApplication::syntax (const string& exe) const
{
	cout << "Syntaxe :" << endl
	     << exe << "[-h][-defaultConfig fichier][-userConfig fichier]"
	     << "[-outCharsetRef àéèùô][-outCharset charset][-icon fichier]"
	     << "[-graphicalWindowWidth largeur][-graphicalWindowHeight hauteur]"
	     << "[-graphicalWindowFixedSize]"
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
		PreferencesHelper::getUnsignedLong (theatreSection, GUIResources::instance ( )._updateRefreshRate);
		PreferencesHelper::getBoolean (theatreSection, GUIResources::instance ( )._useDisplayList);
		PreferencesHelper::getBoolean (theatreSection, GUIResources::instance ( )._xyzCancelRoll);
		PreferencesHelper::getDouble (theatreSection, GUIResources::instance ( )._stillFrameRate);
		PreferencesHelper::getDouble (theatreSection, GUIResources::instance ( )._desiredFrameRate);
		PreferencesHelper::getColor (theatreSection, GUIResources::instance ( )._background);
		PreferencesHelper::getBoolean (theatreSection, GUIResources::instance ( )._multipleLighting);
		PreferencesHelper::getBoolean (theatreSection, GUIResources::instance ( )._displayTrihedron);
		PreferencesHelper::getBoolean (theatreSection, GUIResources::instance ( )._displayFocalPoint);
		PreferencesHelper::getUnsignedLong (theatreSection, GUIResources::instance ( )._graphicalWindowWidth);
		PreferencesHelper::getUnsignedLong (theatreSection, GUIResources::instance ( )._graphicalWindowHeight);
		PreferencesHelper::getBoolean (theatreSection, GUIResources::instance ( )._graphicalWindowFixedSize);
		Section&	representationsSection	= theatreSection.getSection ("representations");
		Section&	geomSection	= representationsSection.getSection ("geomEntities");
		Section&	topoSection	= representationsSection.getSection ("topoEntities");
		Section&	meshSection	= representationsSection.getSection ("meshEntities");
		PreferencesHelper::getString (representationsSection, GUIResources::instance ( )._fontFamily);
		DisplayProperties::_defaultFontFamily	= fontNameToInt (GUIResources::instance ( )._fontFamily.getValue ( ));
		PreferencesHelper::getUnsignedLong (representationsSection, GUIResources::instance ( )._fontSize);
		DisplayProperties::_defaultFontSize	= GUIResources::instance ( )._fontSize.getValue ( );
		PreferencesHelper::getBoolean (representationsSection, GUIResources::instance ( )._fontBold);
		DisplayProperties::_defaultFontBold	= GUIResources::instance ( )._fontBold.getValue ( );
		PreferencesHelper::getBoolean (representationsSection, GUIResources::instance ( )._fontItalic);
		DisplayProperties::_defaultFontItalic	= GUIResources::instance ( )._fontItalic.getValue ( );
		PreferencesHelper::getBoolean (geomSection, GUIResources::instance ( )._geomDestroyOnHide);
		PreferencesHelper::getBoolean (topoSection, GUIResources::instance ( )._topoDestroyOnHide);
		PreferencesHelper::getBoolean (meshSection, GUIResources::instance ( )._meshDestroyOnHide);

		Internal::InternalPreferences::instance ( ).loadPreferences(representationsSection);
	}
	catch (...)	
	{
	}

	try
	{	// Interacteur
		Section&	guiSection	= mainSection.getSection ("gui");
		Section&	intSection	= guiSection.getSection ("interactor");
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._mouseUpZoom);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._zoomOnWheel);
		PreferencesHelper::getColor (intSection, GUIResources::instance ( )._selectionColor);
		PreferencesHelper::getDouble (intSection, GUIResources::instance ( )._selectionPointSize);
		PreferencesHelper::getDouble (intSection, GUIResources::instance ( )._selectionLineWidth);
		PreferencesHelper::getString (intSection, GUIResources::instance ( )._nextSelectionKey);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._selectionOnTop);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._pickOnLeftButtonDown);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._pickOnRightButtonDown);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._pickOnLeftButtonUp);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._pickOnRightButtonUp);
		PreferencesHelper::getDouble (intSection, GUIResources::instance ( )._marginBoundingBoxPercentage);
		PreferencesHelper::getColor (intSection, GUIResources::instance ( )._highlightColor);
		PreferencesHelper::getDouble (intSection, GUIResources::instance ( )._highLightWidth);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._highLightBoundinBox);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._useHighLightColor);
		PreferencesHelper::getColor (intSection, GUIResources::instance ( )._previewColor);
		PreferencesHelper::getDouble (intSection, GUIResources::instance ( )._previewWidth);
		PreferencesHelper::getDouble (intSection, GUIResources::instance ( )._previewPointSize);
	}
	catch (...)
	{
	}

	try
	{	// Panneaux
		Section&	guiSection		= mainSection.getSection ("gui");
		Section&	panelsSection	= guiSection.getSection ("panels");
		Section&	operationSection= panelsSection.getSection("operation");
		PreferencesHelper::getLong (guiSection, GUIResources::instance ( )._margin);
		PreferencesHelper::getLong (guiSection, GUIResources::instance ( )._spacing);
		PreferencesHelper::getColor (operationSection, GUIResources::instance ( )._IDSeizureBackgroundColor);
		PreferencesHelper::getBoolean (operationSection, GUIResources::instance ( )._prevalidateSeizure);
		PreferencesHelper::getBoolean (operationSection, GUIResources::instance ( )._cancelClearEntities);
		PreferencesHelper::getBoolean (operationSection, GUIResources::instance ( )._autoUpdateUsesSelection);
		PreferencesHelper::getBoolean (operationSection, GUIResources::instance ( )._showDialogOnCommandError);
		PreferencesHelper::getBoolean (operationSection, GUIResources::instance ( )._showAmodalDialogOnCommandError);
		PreferencesHelper::getUnsignedLong (operationSection, GUIResources::instance ( )._commandNotificationDelay);
		PreferencesHelper::getUnsignedLong (operationSection, GUIResources::instance ( )._commandNotificationDuration);
		EntitySeizureManager::interactiveModeBackground.setRedF (GUIResources::instance ( )._IDSeizureBackgroundColor.getRed ( ));
		EntitySeizureManager::interactiveModeBackground.setGreenF (GUIResources::instance ( )._IDSeizureBackgroundColor.getGreen ( ));
		EntitySeizureManager::interactiveModeBackground.setBlueF (GUIResources::instance ( )._IDSeizureBackgroundColor.getBlue ( ));

		Section&	selectionSection	= panelsSection.getSection("selection");
		PreferencesHelper::getUnsignedLong (selectionSection, GUIResources::instance ( )._maxIndividualProperties);
		PreferencesHelper::getUnsignedLong (selectionSection, GUIResources::instance ( )._maxCommonProperties);

		Section&	groupsSection	= panelsSection.getSection("groups");
		PreferencesHelper::getBoolean (groupsSection, GUIResources::instance ( )._automaticSort);
		PreferencesHelper::getString (groupsSection, GUIResources::instance ( )._sortType);
		PreferencesHelper::getBoolean (groupsSection, GUIResources::instance ( )._multiLevelGroupsView);
	}
	catch (...)
	{
	}

	// Vérification de la cohérence : au plus un des 4 booléens suivants peut
	// être vrai.
	int	trueCount	= 0;
	if (true == GUIResources::instance ( )._pickOnLeftButtonDown.getValue ( ))
		trueCount	+= 1;
	if (true == GUIResources::instance ( )._pickOnRightButtonDown.getValue ( ))
		trueCount	+= 1;
	if (true == GUIResources::instance ( )._pickOnLeftButtonUp.getValue ( ))
		trueCount	+= 1;
	if (true == GUIResources::instance ( )._pickOnRightButtonUp.getValue ( ))
		trueCount	+= 1;
	if (1 < trueCount)
	{
		try
		{

		cerr << "\a\a\aParmi les 4 booléens suivants en configuration, au plus "
		     << "un d'entre-eux peut être vrai. Au moins deux le sont. "
		     << "Affectation de false à l'ensemble de ces booléens.\n"
		     << "Booléens incriminés : " << GUIResources::instance ( )._pickOnLeftButtonDown.getName ( )
		     << ", " << GUIResources::instance ( )._pickOnRightButtonDown.getName ( )
		     << ", " << GUIResources::instance ( )._pickOnLeftButtonUp.getName ( )
		     << ", " << GUIResources::instance ( )._pickOnRightButtonUp.getName ( ) << endl;
			GUIResources::instance ( )._pickOnLeftButtonDown.setValue (false);
			GUIResources::instance ( )._pickOnRightButtonDown.setValue (false);
			GUIResources::instance ( )._pickOnLeftButtonUp.setValue (false);
			GUIResources::instance ( )._pickOnRightButtonUp.setValue (false);
		}
		catch (...)
		{ }
	}	// if (1 < trueCount)

	try
	{	// Logs
		Section&	guiSection	= mainSection.getSection ("gui");
		Section&	intSection	= guiSection.getSection ("logs");
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._logInformations);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._logErrors);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._logWarnings);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._logTaskOutputs);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._logTraces_1);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._logTraces_2);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._logTraces_3);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._logTraces_4);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._logTraces_5);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._logDate);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._logTime);
		PreferencesHelper::getBoolean (intSection, GUIResources::instance ( )._logThreadID);
		// Actualisation du Controllere :
		Controller::setLogsMask (getLogsMask ( ));
	}
	catch (...)
	{
	}

	try
	{	// Scripting
		Section&	guiSection			= mainSection.getSection ("gui");
		Section&	guiScriptingSection	= guiSection.getSection ("scripting");
		PreferencesHelper::getBoolean (guiSection, GUIResources::instance ( )._saveGuiState);
		PreferencesHelper::getBoolean (guiSection, GUIResources::instance ( )._loadGuiState);
		PreferencesHelper::getBoolean (guiSection, GUIResources::instance ( )._confirmQuitAction);
		PreferencesHelper::getUnsignedLong (guiSection, GUIResources::instance ( )._recentScriptCapacity);
		PreferencesHelper::getString (guiScriptingSection, GUIResources::instance ( )._scriptsCharset);
	}
	catch (...)	
	{
	}

	try
	{
		// Controllere :
		Section&	ControllerSection	= mainSection.getSection ("Controller");
		Preferences::Section&	threadingSection	= Preferences::PreferencesHelper::getSection (ControllerSection, "threading");
		Preferences::Section&	scriptingSection	= Preferences::PreferencesHelper::getSection (ControllerSection, "scripting");
		Preferences::Section&	optimizingSection	= Preferences::PreferencesHelper::getSection (ControllerSection, "optimizing");
		PreferencesHelper::getBoolean (threadingSection, GUIResources::instance ( )._allowThreadedCommandTasks);
		PreferencesHelper::getBoolean (threadingSection, GUIResources::instance ( )._allowThreadedEdgePreMeshTasks);
		PreferencesHelper::getBoolean (threadingSection, GUIResources::instance ( )._allowThreadedFacePreMeshTasks);
		PreferencesHelper::getBoolean (threadingSection, GUIResources::instance ( )._allowThreadedBlockPreMeshTasks);
		PreferencesHelper::getBoolean (scriptingSection, GUIResources::instance ( )._displayScriptOutputs);
		PreferencesHelper::getBoolean (optimizingSection, GUIResources::instance ( )._memorizeEdgePreMesh);
	}
	catch (...)
	{
	}
}	// QtMgx3DApplication::applyConfiguration


void QtMgx3DApplication::editConfiguration (QWidget* parent, const UTF8String& title)
{
	Section*	section	= new Section (GUIResources::instance ( )._mgx3DRootSectionName);
	CHECK_NULL_PTR_ERROR (section)
	instance ( ).saveConfiguration (*section);
	QtMgx3DPreferencesDialog	dialog (
			parent, section, GUIResources::instance ( )._userConfigURL, true, true, true, title,
			UTF8String ("Magix 3D", Charset::UTF_8));
	dialog.exec ( );
}	// QtMgx3DApplication::editConfiguration


Section* QtMgx3DApplication::getPersistantConfiguration ( )
{
	unique_ptr<Section>	config;

	if ((0 != GUIResources::instance ( )._defaultConfigURL.length ( )) || (0 != GUIResources::instance ( )._userConfigURL.length ( )))
	{
		try
		{
			if (0 != GUIResources::instance ( )._defaultConfigURL.length ( ))
			    config.reset (XMLLoader::load (GUIResources::instance ( )._defaultConfigURL));
		}
		catch (const Exception& exc)
		{
			cout << "Exception lors du chargement de la configuration depuis "
			     << "le fichier " << GUIResources::instance ( )._defaultConfigURL << " :\n"
			     << exc.getMessage ( ) << endl;
		}
		catch (...)
		{
			cout << "Exception lors du chargement de la configuration depuis "
			     << "le fichier " << GUIResources::instance ( )._defaultConfigURL << " :\n"
			     << "Erreur non documentée." << endl;
		}
		Section*	s	= config.release ( );
		try
		{
			if (0 != GUIResources::instance ( )._userConfigURL.length ( ))
			{
				// On effectue une actualisation préalable :
				if (0 != GUIResources::instance ( )._userConfigPatchsURL.length ( ))
				{
					bool	isExtendedMode	= false;
					string	extension ("undefined extension");
					UTF8String	Controller (Charset::UTF_8);
					Controller << "Magix3D propose d'effectuer une mise à jour automatique de certaines préférences utilisateur jugées importantes.\n";
					Controller << "- spotTolerance, influant sur la qualité des tâches laser affichées,\n";
					Controller << "- updateRefreshRate, influant sur la rapidité des affichages des entités\n";
					UTF8String	modifications (Charset::UTF_8);
					size_t			count	=
						QtPrefsUpdater::update (
							GUIResources::instance ( )._userConfigURL, GUIResources::instance ( )._userConfigPatchsURL, isExtendedMode?extension.c_str ( ):"Magix3D",
							Controller.format (100), true, modifications);
					if (0 != count)
						QtMessageBox::displayInformationMessage (0, isExtendedMode?extension.c_str ( ):"Magix3D", modifications);
				}	// if (0 != GUIResources::instance ( )._userConfigPatchsURL.length ( ))

				Section*	s	= config.release ( );
			    config.reset (XMLLoader::load (GUIResources::instance ( )._userConfigURL, s));
			}
		}
		catch (const Exception& exc)
		{
			cout << "Exception lors du chargement de la configuration depuis "
			     << "le fichier " << GUIResources::instance ( )._userConfigURL << " :\n"
			     << exc.getMessage ( ) << endl;
		}
		catch (...)
		{
			cout << "Exception lors du chargement de la configuration depuis "
			     << "le fichier " << GUIResources::instance ( )._userConfigURL << " :\n"
			     << "Erreur non documentée." << endl;
		}
		if (0 == config.get ( )) // Exception levée car pas de fichier perso ???
			config.reset (s);
		s	= 0;
	}	// if ((0 != GUIResources::instance ( )._defaultConfigURL.length ( )) || ...

	return config.release ( );
}	// QtMgx3DApplication::getPersistantConfiguration


void QtMgx3DApplication::setPersistantConfiguration (const Section& mainSection)
{
	if (0 != GUIResources::instance ( )._userConfigURL.length ( ))
	{
		XMLLoader::ENCODING	encoding	= XMLLoader::UTF_8;
	    XMLLoader::save (mainSection, GUIResources::instance ( )._userConfigURL, encoding);
	}	// if (0 != _userConfigURL.length ( ))
}	// QtMgx3DApplication::setPersistantConfiguration


void QtMgx3DApplication::updateConfiguration (Section& section, NamedValue& value)
{
	Section&	guiSection	= PreferencesHelper::getSection (section,"gui");
	if  (GUIResources::instance ( )._loadGuiState.getName ( ) == value.getName ( ))
		PreferencesHelper::updateBoolean (guiSection, GUIResources::instance ( )._loadGuiState);
	else if  (GUIResources::instance ( )._saveGuiState.getName ( ) == value.getName ( ))
		PreferencesHelper::updateBoolean (guiSection, GUIResources::instance ( )._saveGuiState);
	else
	{
		UTF8String	message (Charset::UTF_8);
		message << "Actualisation de la préférence utilisateur " << value.getName ( ) << " non prévue.";
		throw Exception (message);
	}
}	// QtMgx3DApplication::updateConfiguration


void QtMgx3DApplication::saveConfiguration (Section& mainSection)
{
	Section&	ControllerSection	= PreferencesHelper::getSection (mainSection,"Controller");
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
	PreferencesHelper::updateBoolean (guiSection, GUIResources::instance ( )._saveGuiState);
	PreferencesHelper::updateBoolean (guiSection, GUIResources::instance ( )._loadGuiState);
	PreferencesHelper::updateBoolean (guiSection, GUIResources::instance ( )._confirmQuitAction);

	// Théâtre :
	PreferencesHelper::updateUnsignedLong (theatreSection, GUIResources::instance ( )._updateRefreshRate);
	PreferencesHelper::updateBoolean (theatreSection, GUIResources::instance ( )._useDisplayList);
	PreferencesHelper::updateBoolean (theatreSection, GUIResources::instance ( )._xyzCancelRoll);
	PreferencesHelper::updateDouble (theatreSection, GUIResources::instance ( )._stillFrameRate);
	PreferencesHelper::updateDouble (theatreSection, GUIResources::instance ( )._desiredFrameRate);
	PreferencesHelper::updateColor (theatreSection, GUIResources::instance ( )._background);
	PreferencesHelper::updateBoolean (theatreSection, GUIResources::instance ( )._multipleLighting);
	PreferencesHelper::updateBoolean (theatreSection, GUIResources::instance ( )._displayTrihedron);
	PreferencesHelper::updateBoolean (theatreSection, GUIResources::instance ( )._displayFocalPoint);
	PreferencesHelper::updateUnsignedLong (theatreSection, GUIResources::instance ( )._graphicalWindowWidth);
	PreferencesHelper::updateUnsignedLong (theatreSection, GUIResources::instance ( )._graphicalWindowHeight);
	PreferencesHelper::updateBoolean (theatreSection, GUIResources::instance ( )._graphicalWindowFixedSize);
	Section&	representationsSection	= PreferencesHelper::getSection (theatreSection, "representations");
	Section&	geomSection	= PreferencesHelper::getSection (representationsSection, "geomEntities");
	Section&	topoSection	= PreferencesHelper::getSection (representationsSection, "topoEntities");
	Section&	meshSection	= PreferencesHelper::getSection (representationsSection, "meshEntities");
	PreferencesHelper::updateString (representationsSection, GUIResources::instance ( )._fontFamily);
	PreferencesHelper::updateUnsignedLong (representationsSection, GUIResources::instance ( )._fontSize);
	PreferencesHelper::updateBoolean (representationsSection, GUIResources::instance ( )._fontBold);
	PreferencesHelper::updateBoolean (representationsSection, GUIResources::instance ( )._fontItalic);
	PreferencesHelper::updateBoolean (geomSection, GUIResources::instance ( )._geomDestroyOnHide);
	PreferencesHelper::updateBoolean (topoSection, GUIResources::instance ( )._topoDestroyOnHide);
	PreferencesHelper::updateBoolean (meshSection, GUIResources::instance ( )._meshDestroyOnHide);
	Internal::InternalPreferences::instance ( ).savePreferences(representationsSection);

	// Interacteur :
	PreferencesHelper::updateBoolean (intSection, GUIResources::instance ( )._mouseUpZoom);
	PreferencesHelper::updateBoolean (intSection, GUIResources::instance ( )._zoomOnWheel);
	PreferencesHelper::updateColor (intSection, GUIResources::instance ( )._selectionColor);
	PreferencesHelper::updateDouble (intSection, GUIResources::instance ( )._selectionPointSize);
	PreferencesHelper::updateDouble (intSection, GUIResources::instance ( )._selectionLineWidth);
	PreferencesHelper::updateString (intSection, GUIResources::instance ( )._nextSelectionKey);
	PreferencesHelper::updateBoolean (intSection, GUIResources::instance ( )._selectionOnTop);
	PreferencesHelper::updateBoolean (intSection, GUIResources::instance ( )._pickOnLeftButtonDown);
	PreferencesHelper::updateBoolean (intSection, GUIResources::instance ( )._pickOnRightButtonDown);
	PreferencesHelper::updateBoolean (intSection, GUIResources::instance ( )._pickOnLeftButtonUp);
	PreferencesHelper::updateBoolean (intSection, GUIResources::instance ( )._pickOnRightButtonUp);
	PreferencesHelper::updateDouble (intSection, GUIResources::instance ( )._marginBoundingBoxPercentage);
	PreferencesHelper::updateColor (intSection, GUIResources::instance ( )._highlightColor);
	PreferencesHelper::updateDouble (intSection, GUIResources::instance ( )._highLightWidth);
	PreferencesHelper::updateBoolean (intSection, GUIResources::instance ( )._highLightBoundinBox);
	PreferencesHelper::updateBoolean (intSection, GUIResources::instance ( )._useHighLightColor);
	PreferencesHelper::updateColor (intSection, GUIResources::instance ( )._previewColor);
	PreferencesHelper::updateDouble (intSection, GUIResources::instance ( )._previewWidth);
	PreferencesHelper::updateDouble (intSection, GUIResources::instance ( )._previewPointSize);

	// Panels :
	PreferencesHelper::updateLong (guiSection, GUIResources::instance ( )._margin);
	PreferencesHelper::updateLong (guiSection, GUIResources::instance ( )._spacing);
	PreferencesHelper::updateColor (operationSection,GUIResources::instance ( )._IDSeizureBackgroundColor);
	PreferencesHelper::updateBoolean (operationSection, GUIResources::instance ( )._prevalidateSeizure);
	PreferencesHelper::updateBoolean (operationSection, GUIResources::instance ( )._cancelClearEntities);
	PreferencesHelper::updateBoolean (operationSection, GUIResources::instance ( )._autoUpdateUsesSelection);
	PreferencesHelper::updateBoolean (operationSection, GUIResources::instance ( )._showDialogOnCommandError);
	PreferencesHelper::updateBoolean (operationSection, GUIResources::instance ( )._showAmodalDialogOnCommandError);
	PreferencesHelper::updateUnsignedLong (operationSection, GUIResources::instance ( )._commandNotificationDelay);
	PreferencesHelper::updateUnsignedLong (operationSection, GUIResources::instance ( )._commandNotificationDuration);
	PreferencesHelper::updateUnsignedLong (selectionSection, GUIResources::instance ( )._maxIndividualProperties);
	PreferencesHelper::updateUnsignedLong (selectionSection, GUIResources::instance ( )._maxCommonProperties);
	PreferencesHelper::updateBoolean (groupsSection, GUIResources::instance ( )._automaticSort);
	PreferencesHelper::updateString (groupsSection, GUIResources::instance ( )._sortType);
	PreferencesHelper::updateBoolean (groupsSection, GUIResources::instance ( )._multiLevelGroupsView);

	// Logs :
	PreferencesHelper::updateBoolean (logsSection, GUIResources::instance ( )._logInformations);
	PreferencesHelper::updateBoolean (logsSection, GUIResources::instance ( )._logWarnings);
	PreferencesHelper::updateBoolean (logsSection, GUIResources::instance ( )._logErrors);
	PreferencesHelper::updateBoolean (logsSection, GUIResources::instance ( )._logTaskOutputs);
	PreferencesHelper::updateBoolean (logsSection, GUIResources::instance ( )._logTraces_1);
	PreferencesHelper::updateBoolean (logsSection, GUIResources::instance ( )._logTraces_2);
	PreferencesHelper::updateBoolean (logsSection, GUIResources::instance ( )._logTraces_3);
	PreferencesHelper::updateBoolean (logsSection, GUIResources::instance ( )._logTraces_4);
	PreferencesHelper::updateBoolean (logsSection, GUIResources::instance ( )._logTraces_5);
	PreferencesHelper::updateBoolean (logsSection, GUIResources::instance ( )._logDate);
	PreferencesHelper::updateBoolean (logsSection, GUIResources::instance ( )._logTime);
	PreferencesHelper::updateBoolean (logsSection, GUIResources::instance ( )._logThreadID);

	// Scripting :
	PreferencesHelper::updateUnsignedLong (guiScriptingSection, GUIResources::instance ( )._recentScriptCapacity);
	PreferencesHelper::updateString (guiScriptingSection, GUIResources::instance ( )._scriptsCharset);

	// Controllere :
	Preferences::Section&	threadingSection	= PreferencesHelper::getSection (ControllerSection, "threading");
	Preferences::Section&	scriptingSection	= PreferencesHelper::getSection (ControllerSection, "scripting");
	Preferences::Section&	optimizingSection	= PreferencesHelper::getSection (ControllerSection, "optimizing");
	PreferencesHelper::updateBoolean (threadingSection, GUIResources::instance ( )._allowThreadedCommandTasks);
	PreferencesHelper::updateBoolean (threadingSection, GUIResources::instance ( )._allowThreadedEdgePreMeshTasks);
	PreferencesHelper::updateBoolean (threadingSection, GUIResources::instance ( )._allowThreadedFacePreMeshTasks);
	PreferencesHelper::updateBoolean (threadingSection, GUIResources::instance ( )._allowThreadedBlockPreMeshTasks);
	PreferencesHelper::updateBoolean (scriptingSection, GUIResources::instance ( )._displayScriptOutputs);
	PreferencesHelper::updateBoolean (optimizingSection, GUIResources::instance ( )._memorizeEdgePreMesh);
}	// QtMgx3DApplication::saveConfiguration


unsigned short QtMgx3DApplication::fontNameToInt (const string& name)
{
	return 0;
}	// QtMgx3DApplication::fontNameToInt


Log::TYPE QtMgx3DApplication::getLogsMask ( )
{
	Log::TYPE	mask	= Log::NONE;

	if (true == GUIResources::instance ( )._logInformations)
		mask	|= Log::INFORMATION;
	if (true == GUIResources::instance ( )._logWarnings)
		mask	|= Log::WARNING;
	if (true == GUIResources::instance ( )._logErrors)
		mask	|= Log::ERROR;
	if (true == GUIResources::instance ( )._logTaskOutputs)
		mask	|= Log::PROCESS;
	if (true == GUIResources::instance ( )._logTraces_1)
		mask	|= Log::TRACE_1;
	if (true == GUIResources::instance ( )._logTraces_2)
		mask	|= Log::TRACE_2;
	if (true == GUIResources::instance ( )._logTraces_3)
		mask	|= Log::TRACE_3;
	if (true == GUIResources::instance ( )._logTraces_4)
		mask	|= Log::TRACE_4;
	if (true == GUIResources::instance ( )._logTraces_5)
		mask	|= Log::TRACE_5;

	return mask;
}	// QtMgx3DApplication::getLogsMask


TkUtil::Version QtMgx3DApplication::getLastVersion ( )
{
	// ouverture du fichier avec la chaine de caractère de la dernière version
	std::ifstream ifile(GUIResources::instance ( )._userLastVersionURL);
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
	std::ofstream ofile(GUIResources::instance ( )._userLastVersionURL);
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
