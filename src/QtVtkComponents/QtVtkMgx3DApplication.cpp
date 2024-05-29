/**
 * \file        QtVtkMgx3DApplication.cpp
 * \author      Charles PIGNEROL
 * \date        24/11/2011
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "QtVtkComponents/QtVtkMgx3DApplication.h"
#include "QtVtkComponents/VTKConfiguration.h"

#include <Utils/Common.h>

#include <QtVtk/QtVtk.h>
#ifdef USE_EXPERIMENTAL_ROOM
#include <ExperimentalRoom/ExperimentalRoom.h>
#include <ExperimentalRoom/ExperimentalRoomDistribution.h>
#include <ExperimentalRoom/QtVtkExperimentalRoomPanel.h>
#endif	// USE_EXPERIMENTAL_ROOM
#include <QSurfaceFormat>
#include <PrefsCore/PreferencesHelper.h>

#include <vtkOpenGLRenderWindow.h>
#include <vtkMapper.h>
#include <vtkTextProperty.h>

#include <mgx_config.h>
using namespace std;
using namespace Preferences;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtVtkComponents
{


// ===========================================================================
//                        LA CLASSE QtVtkMgx3DApplication
// ===========================================================================
//


QtVtkMgx3DApplication::QtVtkMgx3DApplication (int& argc, char* argv[], char* envp[])
	: QtComponents::QtMgx3DApplication (argc, argv, envp)
	,
_raysReleaseIntermediateMemory ("releaseIntermediateMemory", false,"Faut il restituer la mémoire inutilisée dans les calculs des pointages lasers et diagnostics (calculs intermédiaires) ?\nOccupe moins de mémoire mais perte en temps de calcul."),
_raysLineWidth ("lineWidth", 0.,"Epaisseur du trait des faces des pointages laser"),
_raysOpacity ("raysOpacity", 1.,"Opacité ([0..1]) des faces des pointages laser, 1 par défaut"),
_raysSpotOpacity ("raysSpotOpacity", 1.,"Opacité ([0..1]) des taches des pointages laser, 1 par défaut"),
_spotTolerance ("spotTolerance", 1.e-6,"Tolérance dans les calculs d'intersection pointage laser/maille, 1e-6 par défaut"),
_spotRepresentationCheat ("spotRepresentationCheat", true,"Faut il essayer de limiter les moirés lors des représentations des taches ?"),
_spotRepresentationCheatFactor ("spotRepresentationCheatFactor", 0.991,"Pourcentage de proximité de la tache à la surface de projection, 0.991 par défaut"),
_hiddenCellsElimination ("hiddenCellsElimination", true,"Faut il supprimer les taches cachées ?"),
_offScreenWindowWidth ("offScreenWindowWidth", 1024, "Largeur de la fenêtre offscreen utilisée pour supprimer les taches cachées"),
_offScreenWindowHeight ("offScreenWindowHeight", 768, "Hauteur de la fenêtre offscreen utilisée pour supprimer les taches cachées"),
_raysFileCharset ("raysFileCharset", "ISO8859", "Jeu de caractères utilisé par défaut lors de l'enregistrement des fichiers lasers/diagnostics (ISO8859, UTF8).")
{
#ifdef VTK_8
    // Gestion des paramètres Open GL via Qt.
	// En son absence, par défaut, les polygones sont transparents.
	// Permet de choisir la version d'Open GL (et extensions, ...)
//	QSurfaceFormat::setDefaultFormat (QVTKOpenGLWidget::defaultFormat ( ));
	QSurfaceFormat fmt;
	fmt.setRenderableType(QSurfaceFormat::OpenGL);
	fmt.setVersion(3, 2);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	fmt.setRedBufferSize(8);
	fmt.setGreenBufferSize(8);
	fmt.setBlueBufferSize(8);
	fmt.setDepthBufferSize(24);
	fmt.setStencilBufferSize(8);
	fmt.setAlphaBufferSize(0);
//fmt.setAlphaBufferSize(1);
	fmt.setStereo(false);
	fmt.setSamples(vtkOpenGLRenderWindow::GetGlobalMaximumNumberOfMultiSamples());
//fmt.setSamples(0);
	QSurfaceFormat::setDefaultFormat (fmt);

#else	// VTK 8
    // Code issu de QVTKOpenGLWindow::defaultFormat ( ) de VTK 8.2.0
    // Paramètres à donner à Qt pour initialiser Open GL. Evite que les maillages ne soient transparents.
    // OK avec VTK 7.1.1 QVTKWidget/backend OpenGL
    QSurfaceFormat fmt;
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    fmt.setVersion(3, 2);
//fmt.setVersion(2, 0);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    fmt.setRedBufferSize(8);
    fmt.setGreenBufferSize(8);
    fmt.setBlueBufferSize(8);
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setAlphaBufferSize(0);
    fmt.setStereo(false);
    fmt.setSamples(0);//vtkOpenGLRenderWindow::GetGlobalMaximumNumberOfMultiSamples());
    QSurfaceFormat::setDefaultFormat (fmt);
#endif	// VTK_8
}	// QtVtkMgx3DApplication::QtVtkMgx3DApplication


#ifdef QT_4
QtVtkMgx3DApplication::QtVtkMgx3DApplication (const QtVtkMgx3DApplication&)
	: QtComponents::QtMgx3DApplication (*new int [1], 0, 0)
	,
_raysReleaseIntermediateMemory ("releaseIntermediateMemory", false,"Faut il restituer la mémoire inutilisée dans les calculs des pointages lasers et diagnostics (calculs intermédiaires) ?\nOccupe moins de mémoire mais perte en temps de calcul."),
_raysLineWidth ("lineWidth", 0.,"Epaisseur du trait des faces des pointages laser"),
_raysOpacity ("raysOpacity", 1.,"Opacité ([0..1]) des faces des pointages laser, 1 par défaut"),
_raysSpotOpacity ("raysSpotOpacity", 1.,"Opacité ([0..1]) des taches des pointages laser, 1 par défaut"),
_spotTolerance ("spotTolerance", 1.e-6,"Tolérance dans les calculs d'intersection pointage laser/maille, 1e-6 par défaut"),
_spotRepresentationCheat ("spotRepresentationCheat", true,"Faut il essayer de limiter les moirés lors des représentations des taches ?"),
_spotRepresentationCheatFactor ("spotRepresentationCheatFactor", 0.991,"Pourcentage de proximité de la tache à la surface de projection, 0.991 par défaut"),
_hiddenCellsElimination ("hiddenCellsElimination", true,"Faut il supprimer les taches cachées ?"),
_offScreenWindowWidth ("offScreenWindowWidth", 1024, "Largeur de la fenêtre offscreen utilisée pour supprimer les taches cachées"),
_offScreenWindowHeight ("offScreenWindowHeight", 768, "Hauteur de la fenêtre offscreen utilisée pour supprimer les taches cachées"),
_raysFileCharset ("raysFileCharset", "ISO8859", "Jeu de caractères utilisé par défaut lors de l'enregistrement des fichiers lasers/diagnostics (ISO8859, UTF8).")
{
	MGX_FORBIDDEN ("QtVtkMgx3DApplication copy constructor is not allowed.");
}	// QtVtkMgx3DApplication::QtVtkMgx3DApplication (const QtVtkMgx3DApplication&)
#endif	// QT_4


QtVtkMgx3DApplication& QtVtkMgx3DApplication::operator = (const QtVtkMgx3DApplication&)
{
	MGX_FORBIDDEN ("QtVtkMgx3DApplication assignment operator is not allowed.");
	return *this;
}	// QtVtkMgx3DApplication::QtVtkMgx3DApplication (const QtVtkMgx3DApplication&)


QtVtkMgx3DApplication::~QtVtkMgx3DApplication ( )
{
#ifdef USE_EXPERIMENTAL_ROOM
	ExperimentalRoom::finalize ( );
#endif	// USE_EXPERIMENTAL_ROOM
	QtVtk::finalize ( );
}	// QtVtkMgx3DApplication::~QtVtkMgx3DApplication


QtVtkMgx3DApplication& QtVtkMgx3DApplication::instance ( )
{
	return dynamic_cast<QtVtkMgx3DApplication&>(QtMgx3DApplication::instance ( ));
}	// QtVtkMgx3DApplication::instance


void QtVtkMgx3DApplication::init (int argc, char* argv[], char* envp[])
{
	QtVtk::initialize ( );
	
#ifdef USE_EXPERIMENTAL_ROOM
	ExperimentalRoom::initialize ( );
	QtMgx3DApplication::HelpSystem::instance ( ).rayURL	= LASERS_HELP_URL;

	// Rayons bleus, par défaut :
	ExperimentalRayGraphicalProperties::defaultRed		= 0;
	ExperimentalRayGraphicalProperties::defaultGreen	= 170;
	ExperimentalRayGraphicalProperties::defaultBlue		= 255;
	ExperimentRay::characteristicPointPrefix			= USER_TEAM;
#endif	// USE_EXPERIMENTAL_ROOM

	// Finesse lors de la projection de la découpe cylindre/laser sur le rayon.
	// Si pas assez fin le résultat est assez dégueulasse :
//	vtkClippedCylinderSource::projectionTolerance	= 1E-10;
	QtMgx3DApplication::init (argc, argv, envp);

	// Pas de display lists :
#if	VTK_MAJOR_VERSION < 8
	vtkMapper::SetGlobalImmediateModeRendering (!Resources::instance ( )._useDisplayList);
#endif	// VTK_MAJOR_VERSION < 8

	// ATTENTION : appel vital pour un bon affichage des bras.
    // En son absence les segments de droites, coplanaires avec des surfaces à
    // afficher, ne sont que partiellement dessinés => il est difficile 
    // d'apprécier les limites des mailles.
//	vtkMapper::SetResolveCoincidentTopologyToPolygonOffset ( );
}	// QtVtkMgx3DApplication::init


void QtVtkMgx3DApplication::applyConfiguration (const Section& mainSection)
{
	try
	{
		QtMgx3DApplication::applyConfiguration (mainSection);
	}
	catch (...)	
	{
	}

	try
	{
		Section&	guiSection		= mainSection.getSection ("gui");
		Section&	theatreSection	= guiSection.getSection ("theatre");
		Section&	vtkSection		= theatreSection.getSection ("vtk");
		VTKConfiguration::applyConfiguration (vtkSection);
	}
	catch (...)	
	{
	}

#ifdef USE_EXPERIMENTAL_ROOM
	try
	{
		Section&	guiSection	= mainSection.getSection ("gui");
		Section&	raysSection	= guiSection.getSection ("rays");
		PreferencesHelper::getBoolean (raysSection, _raysReleaseIntermediateMemory);
		QtVtkExperimentalRoomPanel::_releaseIntermediateMemory	= _raysReleaseIntermediateMemory.getValue ( );
		PreferencesHelper::getDouble (raysSection, _raysLineWidth);
		ExperimentalRayGraphicalProperties::defaultLineWidth	= _raysLineWidth.getValue ( );
		PreferencesHelper::getDouble (raysSection, _raysOpacity);
		ExperimentalRayGraphicalProperties::defaultOpacity	= _raysOpacity.getValue ( );
		PreferencesHelper::getDouble (raysSection, _raysSpotOpacity);
		ExperimentalRayGraphicalProperties::defaultSpotOpacity	= _raysSpotOpacity.getValue ( );
		PreferencesHelper::getDouble (raysSection, _spotTolerance);
		ExperimentalRayGraphicalProperties::defaultSpotOpacity	= _raysSpotOpacity.getValue ( );
		ExperimentalRayGraphicalProperties::defaultSpotTolerance	= _spotTolerance.getValue ( );
		PreferencesHelper::getBoolean (raysSection, _spotRepresentationCheat);
		ExperimentalRayGraphicalProperties::spotRepresentationCheat	= _spotRepresentationCheat.getValue ( );
		PreferencesHelper::getDouble (raysSection, _spotRepresentationCheatFactor);
		ExperimentalRayGraphicalProperties::spotRepresentationCheatFactor	= _spotRepresentationCheatFactor.getValue ( );
		PreferencesHelper::getBoolean (raysSection, _hiddenCellsElimination);
		ExperimentalRayGraphicalProperties::defaultHiddenCellsElimination	= _hiddenCellsElimination.getValue ( );
		PreferencesHelper::getUnsignedLong (raysSection, _offScreenWindowWidth);
		ExperimentalRayGraphicalProperties::defaultWindowWidth	= _offScreenWindowWidth.getValue ( );
		PreferencesHelper::getUnsignedLong (raysSection,_offScreenWindowHeight);
		ExperimentalRayGraphicalProperties::defaultWindowHeight	= _offScreenWindowHeight.getValue ( );
		PreferencesHelper::getString (raysSection, _raysFileCharset);
	}
	catch (...)
	{
	}
#endif	// USE_EXPERIMENTAL_ROOM
}	// QtVtkMgx3DApplication::applyConfiguration


void QtVtkMgx3DApplication::saveConfiguration (Section& mainSection)
{
	QtMgx3DApplication::saveConfiguration (mainSection);
	Section&	guiSection	= PreferencesHelper::getSection (mainSection,"gui");
	Section&	theatreSection	= PreferencesHelper::getSection (guiSection,"theatre");
	Section&	vtkSection	= PreferencesHelper::getSection (theatreSection,"vtk");
	VTKConfiguration::saveConfiguration (vtkSection);

#ifdef USE_EXPERIMENTAL_ROOM
	// Pointages laser :
	Section&	raysSection	= PreferencesHelper::getSection (guiSection,"rays");
	PreferencesHelper::updateBoolean (raysSection, _raysReleaseIntermediateMemory);
	PreferencesHelper::updateDouble (raysSection, _raysLineWidth);
	PreferencesHelper::updateDouble (raysSection, _raysOpacity);
	PreferencesHelper::updateDouble (raysSection, _raysSpotOpacity);
	PreferencesHelper::updateDouble (raysSection, _spotTolerance);
	PreferencesHelper::updateBoolean (raysSection, _spotRepresentationCheat);
	PreferencesHelper::updateDouble (raysSection, _spotRepresentationCheatFactor);
	PreferencesHelper::updateBoolean (raysSection, _hiddenCellsElimination);
	PreferencesHelper::updateUnsignedLong (raysSection, _offScreenWindowWidth);
	PreferencesHelper::updateUnsignedLong (raysSection, _offScreenWindowHeight);
	PreferencesHelper::updateString (raysSection, _raysFileCharset);
#endif	// USE_EXPERIMENTAL_ROOM
}	// QtVtkMgx3DApplication::saveConfiguration


unsigned short QtVtkMgx3DApplication::fontNameToInt (const string& name)
{
	int	font	= vtkTextProperty::GetFontFamilyFromString (name.c_str ( ));
	if (VTK_UNKNOWN_FONT == font)
		cout << "Police de caractère invalide : " << name << endl;

	return VTK_UNKNOWN_FONT == font ? 0 : font;
}	// QtVtkMgx3DApplication::saveConfiguration


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtVtkComponents

}	// namespace Mgx3D
