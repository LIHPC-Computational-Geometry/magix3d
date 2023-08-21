/*----------------------------------------------------------------------------*/
/*
 * \file Context.cpp
 *
 *  \author Franck Ledoux, Eric Brière de l'Isle, Charles Pignerol
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
/*----------------------------------------------------------------------------*/

#include "Internal/Context.h"
#include "Internal/ClientServerProperties.h"
#include "Internal/NameManager.h"
#include "Internal/EntitiesHelper.h"
#include "Internal/CommandChangeLengthUnit.h"
#include "Internal/CommandChangeLandmark.h"
#include "Internal/M3DCommandResult.h"
#include "Internal/InternalPreferences.h"
#include "Internal/CommandComposite.h"
#include "Internal/PythonWriter.h"

#include "Utils/TypeDedicatedNameManager.h"
#include "Utils/Common.h"
#include "Utils/CommandManager.h"
#include "Utils/DefaultGraphicalRepresentationFactory.h"
#include "Utils/GraphicalEntityRepresentation.h"
#include "Utils/Property.h"
#include "Utils/DisplayProperties.h"
#include "Utils/CommandManager.h"
#include "Utils/Entity.h"
#include <Utils/Log.h>

#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/CoFace.h"
#include "Topo/Block.h"
#include "Topo/CommandDestroyTopo.h"

#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/CommandRemove.h"
#include "Geom/EntityFactory.h"

#include "Mesh/CommandDestroyMesh.h"
#include "Mesh/CommandChangeMeshDim.h"

#include "SysCoord/SysCoordManager.h"

extern TkUtil::PythonSession* createMgx3DPythonSession ( );

#include <TkUtil/Mutex.h>
#include <TkUtil/ErrorLog.h>
#include <TkUtil/Exception.h>
#include <TkUtil/Locale.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/OstreamLogOutputStream.h>
#include <TkUtil/TemporaryFile.h>
#include <TkUtil/UserData.h>
#include <TkUtil/File.h>
#include <TkUtil/Process.h>
#include <TkUtil/ThreadManager.h>
#include <TkUtil/ThreadPool.h>

#include <sys/signal.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

//#include <LM.h>	// Not mandatory :)

/*----------------------------------------------------------------------------*/
/**
 * Fonction appelee lorsque le programme est interrompu par le signal 'val'.
 * Effectue une procedure d'urgence avant de quitter.
 */
static void
_sigFunc(int val)
{
    std::cerr << "Signal Caught !!! (Number "<<val<<")."<<std::endl;

#ifdef _DEBUG
    std::cerr << "Waiting for debug... (lancer totalview par exemple ...)"<<std::endl;
    pause();
#endif

    std::cerr << "Exiting Magix3D with error."<<std::endl;

    exit(EXIT_FAILURE);
}

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {

/*----------------------------------------------------------------------------*/
static std::string scriptToMinScript (const std::string& fileName)
{
	TkUtil::File			file (fileName);
	const std::string		extension	= file.getExtension ( );
	std::string::size_type	extLength	=
					0 == extension.length ( ) ? 0 : extension.length ( ) + 1;
	TkUtil::UTF8String	userFileName (TkUtil::Charset::UTF_8);
	userFileName << file.getFullFileName ( ).substr (
							0, file.getFullFileName ( ).length ( ) - extLength);
	userFileName << "_min";
	if (0 != extLength)
		userFileName << '.' << extension;

	return userFileName.ascii ( );
}	// scriptToMinScript
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
static unsigned char toNormalizedUChar (double value)
{
	if ((-1. > value) || (value > 2.))
	{
		TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);
		error << "Context::toNormalizedUChar : la valeur " << value
		      << " n'est pas comprise entre -1 et 1.";
		throw TkUtil::Exception (error);
	}
	if (value < 0.)
		value   += 1;
	else if (value > 1.)
		value   -= 1;

	assert ((value >= 0.) && (value <= 1.) && "normalize failed.");
	return (unsigned char)(value * 255);
}   // Color::toNormalizedUChar

/*----------------------------------------------------------------------------*/
static TkUtil::Color nextColor (
	ContextIfc& context,
	const TkUtil::Color& color, double redInc, double greenInc, double blueInc,
	const TkUtil::Color& farColor, size_t depth)
{
	if (10 == ++depth)	// Sécurité anti-récursion infinie
	{	// Il peut arriver qu'on soit dans un cas de débordement de pile.
		// Exemple vécu : farColor (127, 127, 127) == (0.5, 0.5, 0.5)
		// et sensibité du test de proximité = 50%
		// Dans ce cas on observe des appels récursifs (quasi ?)infinis de
		// nextColor avec une forte consommation mémoire (> 10 Go) et donc du
		// swap.
		// On cherche ici à se mettre à l'abri de ce type de problème.
		static size_t recursionCount	= 0;
		recursionCount++;
		if (100 == recursionCount)	// Informer l'utilisateur
		{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Le dispositif de création automatique de couleurs "
			        << "de Magix 3D a tendance à faire des boucles infinies. "
			        << "Cela peut être du au fait que la tolérance entre la "
			        << "couleur créée et la couleur de fond est trop "
			        << "importante (préférence utilisateur "
				<< "Magix3D.gui.theatre.representations.backgroundRelDifference"
			        << "), et/ou que la couleur de fond n'est pas assez "
					<< "tranchée (les composantes r, g, b doivent être si "
					<< "possible proche de 0. ou de 1. (préférence utilisateur "
					<< "Magix3D.gui.theatre.background). Valeurs courantes : "
				<< InternalPreferences::instance ( )._backgroundRelDifference.getValue ( )
					<< " pour la tolérance, (" << farColor.getDRed ( ) << ", "
					<< farColor.getDGreen ( ) << ", " << farColor.getDBlue ( )
					<< ") pour la couleur de fond.";
			cerr << message << endl;
			context.getLogDispatcher ( ).log (TkUtil::ErrorLog (message));
		}	// if (100 == recursionCount)

		throw TkUtil::Exception ("Activation du mécanisme anti-récursion dans nextColor.");
	}	// if (10 == ++depth)

    double	r	= color.getRed ( ) / 255.;
    double	g	= color.getGreen ( ) / 255.;
    double	b	= color.getBlue ( ) / 255.;

	r	+= redInc;
	g	+= greenInc;
	b	+= blueInc;

	const unsigned char	red		= toNormalizedUChar (r);
	const unsigned char	green	= toNormalizedUChar (g);
	const unsigned char	blue	= toNormalizedUChar (b);

	TkUtil::Color	newColor (red, green, blue);

	try
	{

	if (true == InternalPreferences::instance ( )._useRgbColorComparison.getValue ( ))
	{
		while (true == newColor.rgbNear (farColor,
					InternalPreferences::instance ( )._backgroundRelDifference.getValue ( )))
		{
			newColor	= nextColor (
				context, newColor, redInc, greenInc, blueInc, farColor, depth);
		}
	}	// if (true == InternalPreferences::instance ( )._useRgbColorComparison.getValue ( ))
	else
	{
		while (true == newColor.hsvNear (farColor,
					InternalPreferences::instance ( )._backgroundRelDifference.getValue ( )))
		{
			newColor	= nextColor (
				context, newColor, redInc, greenInc, blueInc, farColor, depth);
		}
	}	// else if (true == InternalPreferences::instance ( )._useRgbColorComparison...

	}
	catch (const TkUtil::Exception& exc)
	{
		if (1 != depth)	// Dispositif anti-récursion infinie ?
			throw;
	}
	catch (...)
	{
		if (1 != depth)	// ???
			throw;
	}

	return newColor;
}	// nextColor

/*----------------------------------------------------------------------------*/
static TkUtil::Color nextColor (
	ContextIfc& context,
	const TkUtil::Color& color, double redInc, double greenInc, double blueInc,
	const TkUtil::Color& farColor)
{
	return nextColor (context, color, redInc, greenInc, blueInc, farColor, 0);
}	// nextColor


/**
 * Le masque à appliquer par défaut aux flux sortants de logs.
 */
TkUtil::Log::TYPE	Context::m_logMask	= TkUtil::Log::PRODUCTION;
TkUtil::ArgumentsMap	Context::m_argumentsMap;
TkUtil::Charset		Context::m_outCharset (TkUtil::Charset::UTF_8);
/** Utilisation d'OCC (sans OCAF) par défaut */
ContextIfc::geomKernel Context::m_geom_kernel = ContextIfc::WITHOCC;


TkUtil::Color	Context::m_initial_geom_displayColor (255, 0, 127);
TkUtil::Color	Context::m_initial_topo_displayColor (0, 1, 1);	// Initialisé dans initialize
TkUtil::Color	Context::m_initial_mesh_displayColor (0, 127, 255);
TkUtil::Color	Context::m_initial_group_displayColor (0, 255, 127);


/*----------------------------------------------------------------------------*/
Context::Context(const std::string& name, bool withStdOutputs)
: ContextIfc (name)
, m_command_manager (0)
, m_python_session (0) //new TkUtil::PythonSession ( ))
, m_geom_manager (new Geom::GeomManager (createName ("GeomManager"), this))
, m_topo_manager (new Topo::TopoManager (createName ("TopoManager"), this))
, m_mesh_manager (new Mesh::MeshManager (createName ("MeshManager"), this))
, m_group_manager (new Group::GroupManager (createName ("GroupManager"), this))
, m_sys_coord_manager (new CoordinateSystem::SysCoordManager (createName ("GroupManager"), this))
, m_structured_mesh_manager (new Structured::StructuredMeshManager (createName ("StructuredMeshManager"), this))
, m_m3d_command_manager(createName ("M3DCommandManager"), this)
, m_selection_manager (0)
, m_name_manager (new Internal::NameManager())
, m_log_dispatcher (m_logMask)
, m_stdout_log_stream (0)
, m_stderr_log_stream (0)
, m_scripting_manager(0)
, m_geom_displayColor(m_initial_geom_displayColor)
, m_topo_displayColor(m_initial_topo_displayColor)
, m_mesh_displayColor(m_initial_mesh_displayColor)
, m_group_displayColor(m_initial_group_displayColor)
, m_backgroundColor(0, 0, 0)
, m_is_finish(false)
, m_is_graphical(false)
, m_preview_mode (false)
, m_ratio_degrad(1)
, m_landmark(Utils::Landmark::undefined)
, m_length_unit(Utils::Unit::undefined)
, m_mesh_dim(MESH3D)
, m_geomVertexDisplayProperties ( )
, m_geomCurveDisplayProperties ( )
, m_geomSurfaceDisplayProperties ( )
, m_geomVolumeDisplayProperties ( )
, m_topoVertexDisplayProperties ( )
, m_topoEdgeDisplayProperties ( )
, m_topoCoEdgeDisplayProperties ( )
, m_topoFaceDisplayProperties ( )
, m_topoCoFaceDisplayProperties ( )
, m_topoBlockDisplayProperties ( )
, m_meshCloudDisplayProperties ( )
, m_meshLineDisplayProperties ( )
, m_meshSurfaceDisplayProperties ( )
, m_meshSubSurfaceDisplayProperties ( )
, m_meshVolumeDisplayProperties ( )
, m_meshSubVolumeDisplayProperties ( )
, m_structuredMeshDisplayProperties ( )
, m_group0DisplayProperties ( )
, m_group1DisplayProperties ( )
, m_group2DisplayProperties ( )
, m_group3DisplayProperties ( )
, m_sysCoordDisplayProperties ( )
, m_geomVertexMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::GeomVertex))
, m_geomCurveMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::GeomCurve))
, m_geomSurfaceMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::GeomSurface))
, m_geomVolumeMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::GeomVolume))
, m_topoVertexMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::TopoVertex))
, m_topoEdgeMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::TopoEdge))
, m_topoCoEdgeMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::TopoCoEdge))
, m_topoFaceMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::TopoFace))
, m_topoCoFaceMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::TopoCoFace))
, m_topoBlockMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::TopoBlock))
, m_meshCloudMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshCloud))
, m_meshLineMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshLine))
, m_meshSurfaceMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshSurface))
, m_meshSubSurfaceMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshSubSurface))
, m_meshVolumeMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshVolume))
, m_meshSubVolumeMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshSubVolume))
, m_structuredMeshMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::StructuredMesh))
, m_sysCoordMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::SysCoord))
{
	// [EB] mis ici pour être pris en compte dans les scripts Python
	// sinon on se retrouve en multi-thread systématiquement ...
	ContextIfc::ContextIfc::initialize ( );

	char* env = getenv ("CODE");

    clearIdToEntity();

    m_scripting_manager	= new Internal::ScriptingManager (this);
    m_scripting_manager->setCharset (m_outCharset);
    setPythonSession (createMgx3DPythonSession ( ));

	// Pour pouvoir directement utiliser Magix 3D depuis un script python :
	if (false == Utils::GraphicalRepresentationFactoryIfc::hasInstance ( ))
		Mgx3D::Utils::GraphicalRepresentationFactoryIfc::setInstance (
				new Mgx3D::Utils::DefaultGraphicalRepresentationFactory ( ));

	Utils::SelectionManager*	selectionManager	=
		new Utils::SelectionManager (createName ("SelectionManager"), getLogStream ( ));
	m_selection_manager	= selectionManager;
	Utils::CommandManager*	commandManager	=
					new Utils::CommandManager (createName ("CommandManager"));
	m_command_manager	= commandManager;

#ifdef _DEBUG
    // redirection des signaux pour intervention avec debuggeur si nécessaire
    _redirect_signals(_sigFunc);
#endif

    // Log à l'aide de LM
    char logVersion[128];
    sprintf(logVersion, "Magix3D-%s", MAGIX3D_VERSION);
#ifdef _DEBUG2
    std::cout<<"Context::Context("<<name<<")"<<std::endl;
    std::cout<<"logVersion : "<<logVersion<<std::endl;
#endif

    if (withStdOutputs) {
        // cf fichier ReadMe.txt pour choisir le type de trace

        // std::cout va récupérer toutes les logs de type autre que ERROR
        // elles ne seront disponibles en sortie que si le masque du Dispatcher le permet
        TkUtil::Log::TYPE stdoutLogType = TkUtil::Log::ALL_KINDS ^ TkUtil::Log::ERROR;
        TkUtil::Log::TYPE stderrLogType = TkUtil::Log::ERROR;

	TkUtil::UTF8String	charsetMsg (TkUtil::Charset::UTF_8);
	charsetMsg << "Utilisation du jeu de caractères " << getOutCharset ( ).name ( ) << " pour la sortie standard." ;
	TkUtil::ConsoleOutput::cout ( ) << charsetMsg << TkUtil::co_endl;
        m_stdout_log_stream	= new TkUtil::OstreamLogOutputStream (std::cout, getOutCharset ( ));
        m_stdout_log_stream->setMask (stdoutLogType);
        // NB: m_stdout_log_stream sera détruit par le LogDispatcher
        getLogDispatcher().addStream (m_stdout_log_stream);

        // idem avec cerr
        m_stderr_log_stream = new TkUtil::OstreamLogOutputStream (std::cerr, getOutCharset ( ));
        m_stderr_log_stream->setMask (stderrLogType);
        getLogDispatcher().addStream (m_stderr_log_stream);
    }

    // transmission du logDispatcher
    commandManager->setLogStream(&m_log_dispatcher);

	// Les propriétés d'affichage des entités par défaut :
	TkUtil::Color	color	= m_geom_displayColor;
	m_geomVertexDisplayProperties.setCloudColor (color);
	m_geomVertexDisplayProperties.setWireColor (color);
	m_geomVertexDisplayProperties.setSurfacicColor (color);
	m_geomVertexDisplayProperties.setLineWidth (2.0);
	m_geomVertexDisplayProperties.setPointSize (5.0);

	color	= nextColor (*this, color,-.37,-.05,.18, getBackground( ));
	m_geomCurveDisplayProperties.setCloudColor (color);
	m_geomCurveDisplayProperties.setWireColor (color);
	m_geomCurveDisplayProperties.setSurfacicColor (color);
	m_geomCurveDisplayProperties.setLineWidth (2.0);
	m_geomCurveDisplayProperties.setPointSize (5.0);
	color	= nextColor (*this, color,-.37,-.05,.18, getBackground( ));
	m_geomSurfaceDisplayProperties.setCloudColor (color);
	m_geomSurfaceDisplayProperties.setWireColor (color);
	m_geomSurfaceDisplayProperties.setSurfacicColor (color);
	m_geomSurfaceDisplayProperties.setLineWidth (2.0);
	m_geomSurfaceDisplayProperties.setPointSize (5.0);
	color	= nextColor (*this, color,-.37,-.05,.18, getBackground( ));
	m_geomVolumeDisplayProperties.setCloudColor (color);
	m_geomVolumeDisplayProperties.setWireColor (color);
	m_geomVolumeDisplayProperties.setSurfacicColor (color);
	m_geomVolumeDisplayProperties.setLineWidth (2.0);
	m_geomVolumeDisplayProperties.setPointSize (5.0);
	color	= m_topo_displayColor;
	color	= nextColor (*this, color,-.37,-.05,.18,getBackground ( ));
	m_topoVertexDisplayProperties.setCloudColor (m_topo_displayColor);
	m_topoVertexDisplayProperties.setFontColor (m_topo_displayColor);
	m_topoVertexDisplayProperties.setWireColor (m_topo_displayColor);
	m_topoVertexDisplayProperties.setSurfacicColor (m_topo_displayColor);
	m_topoVertexDisplayProperties.setLineWidth (1.0);
	m_topoVertexDisplayProperties.setPointSize (5.0);
	m_topoVertexDisplayProperties.setDisplayed(false);
	m_topoEdgeDisplayProperties.setCloudColor (m_topo_displayColor);
	m_topoEdgeDisplayProperties.setFontColor (m_topo_displayColor);
	m_topoEdgeDisplayProperties.setWireColor (m_topo_displayColor);
	m_topoEdgeDisplayProperties.setSurfacicColor (m_topo_displayColor);
	m_topoEdgeDisplayProperties.setLineWidth (1.0);
	m_topoEdgeDisplayProperties.setPointSize (5.0);
	m_topoEdgeDisplayProperties.setDisplayed(false);
	// on ne représente que les arêtes et faces communes et pas les Face et Edge par défaut
	m_topoEdgeDisplayProperties.setDisplayable(
			Internal::InternalPreferences::instance ( )._displayEdge.getValue ( ));
	m_topoCoEdgeDisplayProperties.setCloudColor (m_topo_displayColor);
	m_topoCoEdgeDisplayProperties.setFontColor (m_topo_displayColor);
	m_topoCoEdgeDisplayProperties.setWireColor (m_topo_displayColor);
	m_topoCoEdgeDisplayProperties.setSurfacicColor (m_topo_displayColor);
	m_topoCoEdgeDisplayProperties.setLineWidth (1.0);
	m_topoCoEdgeDisplayProperties.setPointSize (5.0);
	m_topoCoEdgeDisplayProperties.setDisplayed(false);
	m_topoFaceDisplayProperties.setCloudColor (m_topo_displayColor);
	m_topoFaceDisplayProperties.setFontColor (m_topo_displayColor);
	m_topoFaceDisplayProperties.setWireColor (m_topo_displayColor);
	m_topoFaceDisplayProperties.setSurfacicColor (m_topo_displayColor);
	m_topoFaceDisplayProperties.setLineWidth (1.0);
	m_topoFaceDisplayProperties.setPointSize (5.0);
	m_topoFaceDisplayProperties.setDisplayed(false);
	m_topoFaceDisplayProperties.setDisplayable(
			Internal::InternalPreferences::instance ( )._displayFace.getValue ( ));
	m_topoCoFaceDisplayProperties.setCloudColor (m_topo_displayColor);
	m_topoCoFaceDisplayProperties.setFontColor (m_topo_displayColor);
	m_topoCoFaceDisplayProperties.setWireColor (m_topo_displayColor);
	m_topoCoFaceDisplayProperties.setSurfacicColor (m_topo_displayColor);
	m_topoCoFaceDisplayProperties.setLineWidth (1.0);
	m_topoCoFaceDisplayProperties.setPointSize (5.0);
	m_topoCoFaceDisplayProperties.setDisplayed(false);
	m_topoCoFaceDisplayProperties.setShrinkFactor(0.9);
	m_topoBlockDisplayProperties.setCloudColor (m_topo_displayColor);
	m_topoBlockDisplayProperties.setFontColor (m_topo_displayColor);
	m_topoBlockDisplayProperties.setWireColor (m_topo_displayColor);
	m_topoBlockDisplayProperties.setSurfacicColor (m_topo_displayColor);
	m_topoBlockDisplayProperties.setLineWidth (1.0);
	m_topoBlockDisplayProperties.setPointSize (5.0);
	m_topoBlockDisplayProperties.setDisplayed(false);
	m_topoBlockDisplayProperties.setShrinkFactor(0.9);
	color	= m_mesh_displayColor;
	m_meshCloudDisplayProperties.setCloudColor (color);
	m_meshCloudDisplayProperties.setFontColor (color);
	m_meshCloudDisplayProperties.setWireColor (TkUtil::Color(255, 255, 255)); // les arêtes en blanc (noir: 0,0,0)
	m_meshCloudDisplayProperties.setSurfacicColor (color);
	m_meshCloudDisplayProperties.setLineWidth (0.5);
	m_meshCloudDisplayProperties.setPointSize (3.0);
	m_meshCloudDisplayProperties.setDisplayed(false);
	color	= nextColor (*this, color,.18,-.05,-.37,getBackground ( ));
	m_meshLineDisplayProperties.setCloudColor (color);
	m_meshLineDisplayProperties.setFontColor (color);
	m_meshLineDisplayProperties.setWireColor (TkUtil::Color(255, 255, 255)); // les arêtes en blanc (noir: 0,0,0)
	m_meshLineDisplayProperties.setSurfacicColor (color);
	m_meshLineDisplayProperties.setLineWidth (0.5);
	m_meshLineDisplayProperties.setPointSize (3.0);
	m_meshLineDisplayProperties.setDisplayed(false);
	color	= nextColor (*this, color,.18,-.05,-.37,getBackground ( ));
	m_meshSurfaceDisplayProperties.setCloudColor (color);
	m_meshSurfaceDisplayProperties.setFontColor (color);
	m_meshSurfaceDisplayProperties.setWireColor (TkUtil::Color(255, 255, 255)); // les arêtes en blanc (noir: 0,0,0)
	m_meshSurfaceDisplayProperties.setSurfacicColor (color);
	m_meshSurfaceDisplayProperties.setLineWidth (0.5);
	m_meshSurfaceDisplayProperties.setPointSize (3.0);
	m_meshSurfaceDisplayProperties.setDisplayed(false);
	color	= nextColor (*this, color,.18,-.05,-.37,getBackground ( ));
	m_meshSubSurfaceDisplayProperties.setCloudColor (color);
	m_meshSubSurfaceDisplayProperties.setFontColor (color);
	m_meshSubSurfaceDisplayProperties.setWireColor (TkUtil::Color(255, 255, 255)); // les arêtes en blanc (noir: 0,0,0)
	m_meshSubSurfaceDisplayProperties.setSurfacicColor (color);
	m_meshSubSurfaceDisplayProperties.setLineWidth (0.5);
	m_meshSubSurfaceDisplayProperties.setPointSize (3.0);
	m_meshSubSurfaceDisplayProperties.setDisplayed(false);
	color	= nextColor (*this, color,.18,-.05,-.37,getBackground ( ));
	m_meshVolumeDisplayProperties.setCloudColor (color);
	m_meshVolumeDisplayProperties.setFontColor (color);
	m_meshVolumeDisplayProperties.setWireColor (TkUtil::Color(255, 255, 255)); // les arêtes en blanc (noir: 0,0,0)
	m_meshVolumeDisplayProperties.setSurfacicColor (color);
	m_meshVolumeDisplayProperties.setLineWidth (0.5);
	m_meshVolumeDisplayProperties.setPointSize (3.0);
	m_meshVolumeDisplayProperties.setDisplayed(false);
	color	= nextColor (*this, color,.18,-.05,-.37, getBackground( ));
	m_meshSubVolumeDisplayProperties.setCloudColor (color);
	m_meshSubVolumeDisplayProperties.setFontColor (color);
	m_meshSubVolumeDisplayProperties.setWireColor (TkUtil::Color(255, 255, 255)); // les arêtes en blanc (noir: 0,0,0)
	m_meshSubVolumeDisplayProperties.setSurfacicColor (color);
	m_meshSubVolumeDisplayProperties.setLineWidth (0.5);
	m_meshSubVolumeDisplayProperties.setPointSize (3.0);
	m_meshSubVolumeDisplayProperties.setDisplayed(true);
	color	= nextColor (*this, color,.18,-.05,-.37, getBackground( ));
	m_structuredMeshDisplayProperties.setCloudColor (color);
	m_structuredMeshDisplayProperties.setFontColor (color);
	m_structuredMeshDisplayProperties.setWireColor (TkUtil::Color(255, 255, 255)); // les arêtes en blanc (noir: 0,0,0)
	m_structuredMeshDisplayProperties.setSurfacicColor (color);
	m_structuredMeshDisplayProperties.setLineWidth (0.5);
	m_structuredMeshDisplayProperties.setPointSize (3.0);
	m_structuredMeshDisplayProperties.setDisplayed(false);
	color	= m_group_displayColor;
	m_group0DisplayProperties.setCloudColor (color);
	m_group0DisplayProperties.setWireColor (color);
	m_group0DisplayProperties.setSurfacicColor (color);
	m_group0DisplayProperties.setLineWidth(2.0);
	m_group0DisplayProperties.setPointSize(5.0);
	m_group0DisplayProperties.setDisplayed(false);
	m_group0DisplayProperties.setDisplayable(false);
    color	= nextColor (*this, color,-.05,-.37,.18,getBackground( ));
	m_group1DisplayProperties.setCloudColor (color);
	m_group1DisplayProperties.setWireColor (color);
	m_group1DisplayProperties.setSurfacicColor (color);
	m_group1DisplayProperties.setLineWidth(2.0);
	m_group1DisplayProperties.setPointSize(5.0);
	m_group1DisplayProperties.setDisplayed(false);
	m_group1DisplayProperties.setDisplayable(false);
    color    = nextColor (*this, color,-.05,-.37,.18, getBackground( ));
	m_group2DisplayProperties.setCloudColor (color);
	m_group2DisplayProperties.setWireColor (color);
	m_group2DisplayProperties.setSurfacicColor (color);
	m_group2DisplayProperties.setLineWidth(2.0);
	m_group2DisplayProperties.setPointSize(5.0);
	m_group2DisplayProperties.setDisplayed(false);
	m_group2DisplayProperties.setDisplayable(false);
    color    = nextColor (*this, color,-.05,-.37,.18, getBackground( ));
	m_group3DisplayProperties.setCloudColor (color);
	m_group3DisplayProperties.setWireColor (color);
	m_group3DisplayProperties.setSurfacicColor (color);
	m_group3DisplayProperties.setLineWidth(2.0);
	m_group3DisplayProperties.setPointSize(5.0);
	m_group3DisplayProperties.setDisplayed(false);
	m_group3DisplayProperties.setDisplayable(false);

    // initialisation méthode OCC ou OCAF
    Geom::EntityFactory::initialize(getGeomKernel());
}
/*----------------------------------------------------------------------------*/
Context::Context (const Context&)
: ContextIfc ("Bidon")
, m_command_manager (new Utils::CommandManager(createName ("Bidon")))
, m_python_session (0)
, m_geom_manager (0)
, m_topo_manager (0)
, m_mesh_manager (0)
, m_group_manager (0)
, m_sys_coord_manager (0)
, m_structured_mesh_manager (0)
, m_m3d_command_manager(createName ("M3DCommandManager"), this)
, m_selection_manager (0)
, m_name_manager (new Internal::NameManager())
, m_log_dispatcher ()
, m_stdout_log_stream (0)
, m_stderr_log_stream (0)
, m_scripting_manager(0)
, m_geom_displayColor(m_initial_geom_displayColor)
, m_topo_displayColor(m_initial_topo_displayColor)
, m_mesh_displayColor(m_initial_mesh_displayColor)
, m_group_displayColor(m_initial_group_displayColor)
, m_backgroundColor(0, 0, 0)
, m_is_finish(false)
, m_is_graphical(false)
, m_preview_mode (false)
, m_ratio_degrad(1)
, m_landmark(Utils::Landmark::undefined)
, m_length_unit(Utils::Unit::undefined)
, m_mesh_dim(MESH3D)
, m_geomVertexDisplayProperties ( )
, m_geomCurveDisplayProperties ( )
, m_geomSurfaceDisplayProperties ( )
, m_geomVolumeDisplayProperties ( )
, m_topoVertexDisplayProperties ( )
, m_topoEdgeDisplayProperties ( )
, m_topoCoEdgeDisplayProperties ( )
, m_topoFaceDisplayProperties ( )
, m_topoCoFaceDisplayProperties ( )
, m_topoBlockDisplayProperties ( )
, m_meshCloudDisplayProperties ( )
, m_meshLineDisplayProperties ( )
, m_meshSurfaceDisplayProperties ( )
, m_meshSubSurfaceDisplayProperties ( )
, m_meshVolumeDisplayProperties ( )
, m_structuredMeshDisplayProperties ( )
, m_group0DisplayProperties ( )
, m_group1DisplayProperties ( )
, m_group2DisplayProperties ( )
, m_group3DisplayProperties ( )
, m_sysCoordDisplayProperties ( )
, m_geomVertexMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::GeomVertex))
, m_geomCurveMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::GeomCurve))
, m_geomSurfaceMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::GeomSurface))
, m_geomVolumeMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::GeomVolume))
, m_topoVertexMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::TopoVertex))
, m_topoEdgeMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::TopoEdge))
, m_topoCoEdgeMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::TopoCoEdge))
, m_topoFaceMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::TopoFace))
, m_topoCoFaceMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::TopoCoFace))
, m_topoBlockMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::TopoBlock))
, m_meshCloudMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshCloud))
, m_meshLineMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshLine))
, m_meshSurfaceMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshSurface))
, m_meshSubSurfaceMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshSubSurface))
, m_meshVolumeMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshVolume))
, m_meshSubVolumeMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshSubVolume))
, m_structuredMeshMask  (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::MeshVolume))
, m_sysCoordMask (Utils::GraphicalEntityRepresentation::getDefaultRepresentationMask (Utils::Entity::SysCoord))
{
    MGX_FORBIDDEN ("Context copy constructor is not allowed.");
}	// Context::Context
/*----------------------------------------------------------------------------*/
Context& Context::operator = (const Context& c)
{
    MGX_FORBIDDEN ("Context assignment operator is not allowed.");
	if (&c != this)
	{
	}	// if (&c != this)

	return *this;
}	// Context::operator =
/*----------------------------------------------------------------------------*/
Context::~Context()
{
#ifdef _DEBUG2
    std::cout<<"Context::~Context() name = "<<getName()<<std::endl;
#endif
    m_is_finish = true;

#ifndef _DEBUG
    // permet de gagner un peu de temps
    clearIdToEntity();
#endif

	delete m_selection_manager;		m_selection_manager	= 0;
	delete m_scripting_manager;		m_scripting_manager	= 0;
	if (m_python_session)
	    delete m_python_session;
	m_python_session	= 0;
    delete m_command_manager;		m_command_manager	= 0;
    delete m_name_manager;			m_name_manager		= 0;
    delete m_geom_manager;			m_geom_manager		= 0;
    delete m_topo_manager;			m_topo_manager		= 0;
    delete m_mesh_manager;			m_mesh_manager		= 0;
    delete m_group_manager;			m_group_manager		= 0;
    delete m_sys_coord_manager;     m_sys_coord_manager = 0;

#ifdef _DEBUG2
    if (!Utils::Entity::isIdToEntityEmpty()){
        std::cout<<"Certaines entités n'ont pas été détruites !"<<std::endl;
        //Utils::Entity::showAllEntities(std::cout);
        Utils::Entity::showEntitiesStats(std::cout);
    	throw TkUtil::Exception(TkUtil::UTF8String ("Certaines entités n'ont pas été détruites à la fin du contexte", TkUtil::Charset::UTF_8));
    }
#endif
}
/*----------------------------------------------------------------------------*/
void Context::initialize (int argc, char* argv [])
{
//	for (uint i=0; i<argc; i++)
//		std::cout<<"argv["<<i<<"] = "<<argv[i]<<std::endl;

	m_argumentsMap.parseArgs (argc - 1, (const char**)(argv + 1), false);

	std::string	outCharsetRefString;

	outCharsetRefString	= m_argumentsMap.getArgValue ("-outCharsetRef", std::string ( ), false);
	m_geom_kernel	= true == m_argumentsMap.hasArg ("-useOCAF") ? ContextIfc::WITHOCAF : ContextIfc::WITHOCC;
	std::string	charset;
	if (true == m_argumentsMap.hasArg ("-outCharset"))
		charset	= m_argumentsMap.getArgValue ("-outCharset", std::string ( ), false);
	else if (true == m_argumentsMap.hasArg ("--outCharset"))
		charset	= m_argumentsMap.getArgValue ("--outCharset", std::string ( ), false);

	if ((std::string ("UTF-8") == charset) || (std::string ("UTF8") == charset) || (std::string ("utf-8") == charset) || (std::string ("utf8") == charset))
		m_outCharset	= TkUtil::Charset (TkUtil::Charset::UTF_8);
	else if ((std::string ("UTF-16") == charset) || (std::string ("UTF16") == charset) || (std::string ("utf-16") == charset) || (std::string ("utf16") == charset))
		m_outCharset	= TkUtil::Charset (TkUtil::Charset::UTF_16);
	else if ((std::string ("ISO-8859") == charset) || (std::string ("ISO8859") == charset) || (std::string ("iso-8859") == charset) || (std::string ("iso8859") == charset))
		m_outCharset	= TkUtil::Charset (TkUtil::Charset::ISO_8859);
	else if ((std::string ("ASCII") == charset) || (std::string ("ascii") == charset) || (std::string ("C") == charset) || (std::string ("c") == charset) ||
	         (std::string ("POSIX") == charset) || (std::string ("posix") == charset))
		m_outCharset	= TkUtil::Charset (TkUtil::Charset::ASCII);

	if (TkUtil::Charset::UNKNOWN == Context::m_outCharset.charset ( ))
		Context::m_outCharset	= TkUtil::Locale::detectCharset (outCharsetRefString);

	// Pour les sorties "console" des différentes API (tKutil, Preferences, ...) :
	TkUtil::ConsoleOutput::cout ( ).setCharset (m_outCharset);
	TkUtil::ConsoleOutput::cerr ( ).setCharset (m_outCharset);

	Context::m_initial_topo_displayColor	= TkUtil::Color (
		255*Internal::InternalPreferences::instance ( )._topoColorWithoutProj.getRed ( ),
		255*Internal::InternalPreferences::instance ( )._topoColorWithoutProj.getGreen ( ),
		255*Internal::InternalPreferences::instance ( )._topoColorWithoutProj.getBlue ( )); // bleu, pas de proj
	
	// Aspects C/S :
	std::string	host		= ClientServerProperties::getInstance ( ).getServerHost ( );
	std::string	port		= ClientServerProperties::getInstance ( ).getPort ( );
	std::string	rport		= ClientServerProperties::getInstance ( ).getRenderingPort ( );
	bool		modified	= false;
	if (true == m_argumentsMap.hasArg ("-host"))
		{ host	= m_argumentsMap.getArgValue ("-host", std::string ( ), false);	modified	= true; }
	else if (true == m_argumentsMap.hasArg ("--host"))
		{ host	= m_argumentsMap.getArgValue ("--host", std::string ( ), false);	modified	= true; }
	if (true == m_argumentsMap.hasArg ("-port"))
		{ port	= m_argumentsMap.getArgValue ("-port", std::string ( ), false);	modified	= true; }
	else if (true == m_argumentsMap.hasArg ("--port"))
		{ port	= m_argumentsMap.getArgValue ("--port", std::string ( ), false);	modified	= true; }
	if (true == m_argumentsMap.hasArg ("-renderingPort"))
		{ rport	= m_argumentsMap.getArgValue ("-renderingPort", std::string ( ), false);	modified	= true; }
	else if (true == m_argumentsMap.hasArg ("--renderingPort"))
		{ rport	= m_argumentsMap.getArgValue ("--renderingPort", std::string ( ), false);	modified	= true; }
	if (true == modified)
		ClientServerProperties::getInstance ( ).setServerParameters (host, port, rport);
}
/*----------------------------------------------------------------------------*/
void Context::finalize ( )
{
}
/*----------------------------------------------------------------------------*/
TkUtil::ArgumentsMap& Context::getArguments ( )
{
	return Context::m_argumentsMap;
}
/*----------------------------------------------------------------------------*/
const TkUtil::Charset& Context::getOutCharset ( )
{
	return Context::m_outCharset;
}
/*----------------------------------------------------------------------------*/
TkUtil::PythonSession& Context::getPythonSession ( )
{
	CHECK_NULL_PTR_ERROR (m_python_session)
	return *m_python_session;
}
/*----------------------------------------------------------------------------*/
void Context::setPythonSession (TkUtil::PythonSession* session)
{
	if (m_python_session != session)
	{
		delete m_python_session;
		m_python_session	= session;
	}	// if (m_python_session != session)
}
/*----------------------------------------------------------------------------*/
Mgx3D::Geom::GeomManagerIfc& Context::getGeomManager ( )
{
	CHECK_NULL_PTR_ERROR (m_geom_manager)
	return *m_geom_manager;
}
/*----------------------------------------------------------------------------*/
const Mgx3D::Geom::GeomManagerIfc& Context::getGeomManager ( ) const
{
	CHECK_NULL_PTR_ERROR (m_geom_manager)
	return *m_geom_manager;
}
/*----------------------------------------------------------------------------*/
void Context::setGeomManager (Mgx3D::Geom::GeomManagerIfc* manager)
{
	delete m_geom_manager;
	m_geom_manager	= manager;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Geom::GeomManager& Context::getLocalGeomManager ( )
{
	Geom::GeomManager*	manager	= dynamic_cast<Geom::GeomManager*>(m_geom_manager);
	CHECK_NULL_PTR_ERROR (manager)
	return *manager;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Topo::TopoManagerIfc& Context::getTopoManager ( )
{
	CHECK_NULL_PTR_ERROR (m_topo_manager)
	return *m_topo_manager;
}
/*----------------------------------------------------------------------------*/
const Mgx3D::Topo::TopoManagerIfc& Context::getTopoManager ( ) const
{
	CHECK_NULL_PTR_ERROR (m_topo_manager)
	return *m_topo_manager;
}
/*----------------------------------------------------------------------------*/
void Context::setTopoManager (Mgx3D::Topo::TopoManagerIfc* manager)
{
	delete m_topo_manager;
	m_topo_manager	= manager;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Topo::TopoManager& Context::getLocalTopoManager ( )
{
	Topo::TopoManager*	manager	= dynamic_cast<Topo::TopoManager*>(m_topo_manager);
	CHECK_NULL_PTR_ERROR (manager)
	return *manager;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Mesh::MeshManagerIfc& Context::getMeshManager ( )
{
	CHECK_NULL_PTR_ERROR (m_mesh_manager)
	return *m_mesh_manager;
}
/*----------------------------------------------------------------------------*/
const Mgx3D::Mesh::MeshManagerIfc& Context::getMeshManager ( ) const
{
	CHECK_NULL_PTR_ERROR (m_mesh_manager)
	return *m_mesh_manager;
}
/*----------------------------------------------------------------------------*/
void Context::setMeshManager (Mgx3D::Mesh::MeshManagerIfc* manager)
{
	delete m_mesh_manager;
	m_mesh_manager	= manager;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Mesh::MeshManager& Context::getLocalMeshManager ( )
{
	Mesh::MeshManager*	manager	= dynamic_cast<Mesh::MeshManager*>(m_mesh_manager);
	CHECK_NULL_PTR_ERROR (manager)
	return *manager;
}

/*----------------------------------------------------------------------------*/
Mgx3D::Group::GroupManagerIfc& Context::getGroupManager ( )
{
    CHECK_NULL_PTR_ERROR (m_group_manager)
    return *m_group_manager;
}
/*----------------------------------------------------------------------------*/
const Mgx3D::Group::GroupManagerIfc& Context::getGroupManager ( ) const
{
    CHECK_NULL_PTR_ERROR (m_group_manager)
    return *m_group_manager;
}
/*----------------------------------------------------------------------------*/
void Context::setGroupManager (Mgx3D::Group::GroupManagerIfc* manager)
{
    delete m_group_manager;
    m_group_manager  = manager;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Group::GroupManager& Context::getLocalGroupManager ( )
{
    Group::GroupManager*  manager = dynamic_cast<Group::GroupManager*>(m_group_manager);
    CHECK_NULL_PTR_ERROR (manager)
    return *manager;
}

/*----------------------------------------------------------------------------*/
Mgx3D::CoordinateSystem::SysCoordManagerIfc& Context::getSysCoordManager()
{
    CHECK_NULL_PTR_ERROR (m_sys_coord_manager)
    return *m_sys_coord_manager;
}
/*----------------------------------------------------------------------------*/
const Mgx3D::CoordinateSystem::SysCoordManagerIfc& Context::getSysCoordManager() const
{
    CHECK_NULL_PTR_ERROR (m_sys_coord_manager)
    return *m_sys_coord_manager;
}
/*----------------------------------------------------------------------------*/
void Context::setSysCoordManager (Mgx3D::CoordinateSystem::SysCoordManagerIfc* mgr)
{
    delete m_sys_coord_manager;
    m_sys_coord_manager  = mgr;
}
/*----------------------------------------------------------------------------*/
Mgx3D::CoordinateSystem::SysCoordManager& Context::getLocalSysCoordManager()
{
	CoordinateSystem::SysCoordManager*  manager = dynamic_cast<CoordinateSystem::SysCoordManager*>(m_sys_coord_manager);
    CHECK_NULL_PTR_ERROR (manager)
    return *manager;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Structured::StructuredMeshManagerIfc& Context::getStructuredMeshManager ( )
{
	CHECK_NULL_PTR_ERROR (m_structured_mesh_manager)
	return *m_structured_mesh_manager;
}
/*----------------------------------------------------------------------------*/
const Mgx3D::Structured::StructuredMeshManagerIfc& Context::getStructuredMeshManager ( ) const
{
	CHECK_NULL_PTR_ERROR (m_structured_mesh_manager)
	return *m_structured_mesh_manager;
}
/*----------------------------------------------------------------------------*/
void Context::setStructuredMeshManager (Mgx3D::Structured::StructuredMeshManagerIfc* manager)
{
	delete m_structured_mesh_manager;
	m_structured_mesh_manager	= manager;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Utils::CommandManagerIfc& Context::getCommandManager()
{
	CHECK_NULL_PTR_ERROR (m_command_manager)
	return *m_command_manager;
}
/*----------------------------------------------------------------------------*/
void Context::setCommandManager (Mgx3D::Utils::CommandManagerIfc* mgr)
{
	delete m_command_manager;
	m_command_manager	= mgr;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Utils::CommandManager& Context::getLocalCommandManager()
{
	Utils::CommandManager*  manager = dynamic_cast<Utils::CommandManager*>(m_command_manager);
    CHECK_NULL_PTR_ERROR (manager)
    return *manager;
}
/*----------------------------------------------------------------------------*/
Mgx3D::Utils::SelectionManagerIfc& Context::getSelectionManager ( )
{
	CHECK_NULL_PTR_ERROR (m_selection_manager)
	return *m_selection_manager;
}
/*----------------------------------------------------------------------------*/
const Mgx3D::Utils::SelectionManagerIfc& Context::getSelectionManager ( ) const
{
	CHECK_NULL_PTR_ERROR (m_selection_manager)
	return *m_selection_manager;
}
/*----------------------------------------------------------------------------*/
void Context::setSelectionManager (Mgx3D::Utils::SelectionManagerIfc* mgr)
{
	delete m_selection_manager;
	m_selection_manager	= mgr;
}
/*----------------------------------------------------------------------------*/
TkUtil::OstreamLogOutputStream& Context::getStdLogStream ( )
{
	CHECK_NULL_PTR_ERROR (m_stdout_log_stream)
	return *m_stdout_log_stream;
}	// Context::getStdLogStream
/*----------------------------------------------------------------------------*/
TkUtil::OstreamLogOutputStream& Context::getErrLogStream ( )
{
	CHECK_NULL_PTR_ERROR (m_stderr_log_stream)
	return *m_stderr_log_stream;
}	// Context::getErrLogStream
/*----------------------------------------------------------------------------*/
TkUtil::Log::TYPE Context::getLogsMask ( )
{
	return m_logMask;
}	// Context::getLogsMask
/*----------------------------------------------------------------------------*/
void Context::setLogsMask (TkUtil::Log::TYPE mask)
{
	m_logMask	= mask;
}	// Context::setLogsMask
/*----------------------------------------------------------------------------*/
Internal::ScriptingManager& Context::getScriptingManager()
{
	CHECK_NULL_PTR_ERROR (m_scripting_manager)
	return *m_scripting_manager;
}
/*----------------------------------------------------------------------------*/
void Context::setScriptingManager (Internal::ScriptingManager* manager)
{
	if (manager != m_scripting_manager)
	{
		delete m_scripting_manager;
		m_scripting_manager	= manager;
		m_scripting_manager->setContext (this);
	}	// if (manager != m_scripting_manager)
}
/*----------------------------------------------------------------------------*/
void Context::_redirect_signals(fSignalFunc sig_func)
{
	//std::cout<<"Context::_redirect_signals ..."<<std::endl;
    void (*func)(int) = sig_func;

    //sigset(SIGHUP,func);    // (1)   Hangup
    sigset(SIGABRT,func);    // (6)  Abort
    sigset(SIGBUS ,func);    // (7)  Bus Error.
    sigset(SIGFPE ,func);    // (8)  Floating Exception.
    //sigset(SIGKILL,func);    // (9)  Kill.
    sigset(SIGSEGV,func);    // (11) Segmentation fault.
    sigset(SIGPIPE,func);    // (13) Pipe error.
    //sigset(SIGTERM,func);    // (15) Termination
    sigset(SIGSTKFLT,func);  // (16) Stack fault.
    sigset(SIGSYS ,func);    // (31) Bad argument in system call.
}
/*----------------------------------------------------------------------------*/
std::string Context::createName (const std::string& base) const
{
	TkUtil::UTF8String	name (TkUtil::Charset::UTF_8);
	name << base << "_" << getName ( );

	return name.iso ( );
}	// Context::createName
/*----------------------------------------------------------------------------*/
unsigned long Context::newUniqueId()
{
    return getNameManager().getUniqueId();
}
/*----------------------------------------------------------------------------*/
unsigned long Context::nextUniqueId()
{
    return getNameManager().getNextUniqueId();
}
/*----------------------------------------------------------------------------*/
unsigned long Context::newExplorerId()
{
    return getNameManager().getExplorerId();
}
/*----------------------------------------------------------------------------*/
Utils::Property* Context::newProperty(const Utils::Entity::objectType& ot)
{
    // retourne une propriété avec un nouveau nom unique par type d'entité
    std::string name = getNameManager().getTypeDedicatedNameManager(ot)->getName();
    return new Utils::Property(name);
}
/*----------------------------------------------------------------------------*/
Utils::Property* Context::newProperty(const Utils::Entity::objectType& ot, const std::string& name)
{
    if (ot != Utils::Entity::MeshCloud
            && ot != Utils::Entity::MeshLine
            && ot != Utils::Entity::MeshSurface
            && ot != Utils::Entity::MeshSubSurface
            && ot != Utils::Entity::MeshVolume
            && ot != Utils::Entity::MeshSubVolume
            && ot != Utils::Entity::StructuredMesh
            && ot != Utils::Entity::Group0D
            && ot != Utils::Entity::Group1D
            && ot != Utils::Entity::Group2D
            && ot != Utils::Entity::Group3D)
    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne: Context::newProperty avec un nom spécifique ne peut se faire que pour le maillage et les groupes", TkUtil::Charset::UTF_8));

//    if (ot == Utils::Entity::Group0D
//            || ot == Utils::Entity::Group1D
//            || ot == Utils::Entity::Group2D
//            || ot == Utils::Entity::Group3D){
//
//        // on test que le nom n'est pas similaire à celui des entités avec des noms imposés
//        if (Topo::Block::isA(name)){
//			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
//            message << "Il n'est pas permi d'utiliser un nom ("<<name<<") similaire à celui utilisé pour nommer un bloc topologique";
//            throw TkUtil::Exception (message);
//        }
//        if (Topo::CoFace::isA(name)){
//			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
//            message << "Il n'est pas permi d'utiliser un nom ("<<name<<") similaire à celui utilisé pour nommer une face topologique ";
//            throw TkUtil::Exception (message);
//        }
//        if (Topo::CoEdge::isA(name)){
//			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
//            message << "Il n'est pas permi d'utiliser un nom ("<<name<<") similaire à celui utilisé pour nommer une arête topologique ";
//            throw TkUtil::Exception (message);
//        }
//        if (Topo::Vertex::isA(name)){
//			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
//            message << "Il n'est pas permi d'utiliser un nom ("<<name<<") similaire à celui utilisé pour nommer un sommet topologique ";
//            throw TkUtil::Exception (message);
//        }
//        if (Geom::Volume::isA(name)){
//			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
//            message << "Il n'est pas permi d'utiliser un nom ("<<name<<") similaire à celui utilisé pour nommer un volume géométrique ";
//            throw TkUtil::Exception (message);
//        }
//        if (Geom::Surface::isA(name)){
//			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
//            message << "Il n'est pas permi d'utiliser un nom ("<<name<<") similaire à celui utilisé pour nommer une surface géométrique ";
//            throw TkUtil::Exception (message);
//        }
//        if (Geom::Curve::isA(name)){
//			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
//            message << "Il n'est pas permi d'utiliser un nom ("<<name<<") similaire à celui utilisé pour nommer une courbe géométrique ";
//            throw TkUtil::Exception (message);
//        }
//        if (Geom::Vertex::isA(name)){
//			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
//            message << "Il n'est pas permi d'utiliser un nom ("<<name<<") similaire à celui utilisé pour nommer un sommet géométrique ";
//            throw TkUtil::Exception (message);
//        }
//    } // end if ot == Utils::Entity::Group...

    // TODO [EB] il faudrait tester que le nom n'est pas déjà utilisé ...

    return new Utils::Property(name);
}

/*----------------------------------------------------------------------------*/
Utils::DisplayProperties& Context::globalDisplayProperties (Utils::Entity::objectType ot)
{	
	switch (ot)
	{
		case Utils::Entity::GeomVertex		: return m_geomVertexDisplayProperties;
		case Utils::Entity::GeomCurve		: return m_geomCurveDisplayProperties;
		case Utils::Entity::GeomSurface		: return m_geomSurfaceDisplayProperties;
		case Utils::Entity::GeomVolume		: return m_geomVolumeDisplayProperties;
		case Utils::Entity::TopoVertex		: return m_topoVertexDisplayProperties;
		case Utils::Entity::TopoEdge		: return m_topoEdgeDisplayProperties;
		case Utils::Entity::TopoCoEdge		: return m_topoCoEdgeDisplayProperties;
		case Utils::Entity::TopoFace		: return m_topoFaceDisplayProperties;
		case Utils::Entity::TopoCoFace		: return m_topoCoFaceDisplayProperties;
		case Utils::Entity::TopoBlock		: return m_topoBlockDisplayProperties;
		case Utils::Entity::MeshCloud		: return m_meshCloudDisplayProperties;
		case Utils::Entity::MeshLine		: return m_meshLineDisplayProperties;
		case Utils::Entity::MeshSurface		: return m_meshSurfaceDisplayProperties;
		case Utils::Entity::MeshSubSurface	: return m_meshSubSurfaceDisplayProperties;
		case Utils::Entity::MeshVolume		: return m_meshVolumeDisplayProperties;
		case Utils::Entity::MeshSubVolume	: return m_meshSubVolumeDisplayProperties;
		case Utils::Entity::Group0D		: return m_group0DisplayProperties;
		case Utils::Entity::Group1D		: return m_group1DisplayProperties;
		case Utils::Entity::Group2D		: return m_group2DisplayProperties;
		case Utils::Entity::Group3D		: return m_group3DisplayProperties;
		case Utils::Entity::SysCoord		: return m_sysCoordDisplayProperties;
		case Utils::Entity::StructuredMesh	: return m_structuredMeshDisplayProperties;
	}	// switch (ot)

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "Fonction non implémentéepour le type d'entité "
	        << Utils::Entity::objectTypeToObjectTypeName (ot) << ".";
	INTERNAL_ERROR (exc, message, "Context::globalDisplayProperties")
	throw exc;
}	// Context::globalDisplayProperties


/*----------------------------------------------------------------------------*/
const Utils::DisplayProperties& Context::globalDisplayProperties (Utils::Entity::objectType ot) const
{
	switch (ot)
	{
		case Utils::Entity::GeomVertex		: return m_geomVertexDisplayProperties;
		case Utils::Entity::GeomCurve		: return m_geomCurveDisplayProperties;
		case Utils::Entity::GeomSurface		: return m_geomSurfaceDisplayProperties;
		case Utils::Entity::GeomVolume		: return m_geomVolumeDisplayProperties;
		case Utils::Entity::TopoVertex		: return m_topoVertexDisplayProperties;
		case Utils::Entity::TopoEdge		: return m_topoEdgeDisplayProperties;
		case Utils::Entity::TopoCoEdge		: return m_topoCoEdgeDisplayProperties;
		case Utils::Entity::TopoFace		: return m_topoFaceDisplayProperties;
		case Utils::Entity::TopoCoFace		: return m_topoCoFaceDisplayProperties;
		case Utils::Entity::TopoBlock		: return m_topoBlockDisplayProperties;
		case Utils::Entity::MeshCloud		: return m_meshCloudDisplayProperties;
		case Utils::Entity::MeshLine		: return m_meshLineDisplayProperties;
		case Utils::Entity::MeshSurface		: return m_meshSurfaceDisplayProperties;
		case Utils::Entity::MeshSubSurface	: return m_meshSubSurfaceDisplayProperties;
		case Utils::Entity::MeshVolume		: return m_meshVolumeDisplayProperties;
		case Utils::Entity::MeshSubVolume	: return m_meshSubVolumeDisplayProperties;
		case Utils::Entity::Group0D		: return m_group0DisplayProperties;
		case Utils::Entity::Group1D		: return m_group1DisplayProperties;
		case Utils::Entity::Group2D		: return m_group2DisplayProperties;
		case Utils::Entity::Group3D		: return m_group3DisplayProperties;
		case Utils::Entity::SysCoord		: return m_sysCoordDisplayProperties;
		case Utils::Entity::StructuredMesh	: return m_structuredMeshDisplayProperties;
	}	// switch (ot)

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "Fonction non implémentée pour le type d'entité "
	        << Utils::Entity::objectTypeToObjectTypeName (ot) << ".";
	INTERNAL_ERROR (exc, message, "Context::globalDisplayProperties")
	throw exc;
}	// Context::globalDisplayProperties


/*----------------------------------------------------------------------------*/
unsigned long& Context::globalMask (Utils::Entity::objectType ot)
{
	switch (ot)
	{
		case Utils::Entity::GeomVertex		: return m_geomVertexMask;
		case Utils::Entity::GeomCurve		: return m_geomCurveMask;
		case Utils::Entity::GeomSurface		: return m_geomSurfaceMask;
		case Utils::Entity::GeomVolume		: return m_geomVolumeMask;
		case Utils::Entity::TopoVertex		: return m_topoVertexMask;
		case Utils::Entity::TopoEdge		: return m_topoEdgeMask;
		case Utils::Entity::TopoCoEdge		: return m_topoCoEdgeMask;
		case Utils::Entity::TopoFace		: return m_topoFaceMask;
		case Utils::Entity::TopoCoFace		: return m_topoCoFaceMask;
		case Utils::Entity::TopoBlock		: return m_topoBlockMask;
		case Utils::Entity::MeshCloud		: return m_meshCloudMask;
		case Utils::Entity::MeshLine		: return m_meshLineMask;
		case Utils::Entity::MeshSurface		: return m_meshSurfaceMask;
		case Utils::Entity::MeshSubSurface	: return m_meshSubSurfaceMask;
		case Utils::Entity::MeshVolume		: return m_meshVolumeMask;
		case Utils::Entity::MeshSubVolume	: return m_meshSubVolumeMask;
		case Utils::Entity::SysCoord		: return m_sysCoordMask;
		case Utils::Entity::StructuredMesh	: return m_structuredMeshMask;
		case Utils::Entity::Group0D			:
		case Utils::Entity::Group1D			:
		case Utils::Entity::Group2D			:
		case Utils::Entity::Group3D			:
		{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Fonction non supportée pour le type d'entité "
			        << Utils::Entity::objectTypeToObjectTypeName (ot) << ".";
			INTERNAL_ERROR (exc, message, "Context::globalMask")
			throw exc;
		}
	}	// switch (ot)

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "Fonction non implémentée pour le type d'entité "
	        << Utils::Entity::objectTypeToObjectTypeName (ot) << ".";
	INTERNAL_ERROR (exc, message, "Context::globalMask")
	throw exc;
}	// Context::globalMask


/*----------------------------------------------------------------------------*/
unsigned long Context::globalMask (Utils::Entity::objectType ot) const
{
	switch (ot)
	{
		case Utils::Entity::GeomVertex		: return m_geomVertexMask;
		case Utils::Entity::GeomCurve		: return m_geomCurveMask;
		case Utils::Entity::GeomSurface		: return m_geomSurfaceMask;
		case Utils::Entity::GeomVolume		: return m_geomVolumeMask;
		case Utils::Entity::TopoVertex		: return m_topoVertexMask;
		case Utils::Entity::TopoEdge		: return m_topoEdgeMask;
		case Utils::Entity::TopoCoEdge		: return m_topoCoEdgeMask;
		case Utils::Entity::TopoFace		: return m_topoFaceMask;
		case Utils::Entity::TopoCoFace		: return m_topoCoFaceMask;
		case Utils::Entity::TopoBlock		: return m_topoBlockMask;
		case Utils::Entity::MeshCloud		: return m_meshCloudMask;
		case Utils::Entity::MeshLine		: return m_meshLineMask;
		case Utils::Entity::MeshSurface		: return m_meshSurfaceMask;
		case Utils::Entity::MeshSubSurface	: return m_meshSubSurfaceMask;
		case Utils::Entity::MeshVolume		: return m_meshVolumeMask;
		case Utils::Entity::MeshSubVolume	: return m_meshSubVolumeMask;
		case Utils::Entity::SysCoord		: return m_sysCoordMask;
		case Utils::Entity::StructuredMesh	: return m_structuredMeshMask;
		case Utils::Entity::Group0D			:
		case Utils::Entity::Group1D			:
		case Utils::Entity::Group2D			:
		case Utils::Entity::Group3D			:
		{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Fonction non supportée pour le type d'entité "
			        << Utils::Entity::objectTypeToObjectTypeName (ot) << ".";
			INTERNAL_ERROR (exc, message, "Context::globalMask")
			throw exc;
		}
	}	// switch (ot)

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "Fonction non implémentée pour le type d'entité "
	        << Utils::Entity::objectTypeToObjectTypeName (ot) << ".";
	INTERNAL_ERROR (exc, message, "Context::globalMask")
	throw exc;
}	// Context::globalMask


/*----------------------------------------------------------------------------*/
Utils::DisplayProperties* Context::newDisplayProperties(const Utils::Entity::objectType& ot)
{
	Utils::DisplayProperties*	properties	=
				new Utils::DisplayProperties (globalDisplayProperties (ot));

	switch (ot)
	{
		case Utils::Entity::GeomVertex	:
		case Utils::Entity::GeomCurve	:
		case Utils::Entity::GeomSurface	:
		case Utils::Entity::GeomVolume	:
			properties->setCloudColor (m_geom_displayColor);
			properties->setWireColor (m_geom_displayColor);
			properties->setSurfacicColor (m_geom_displayColor);
			if (false == isPreviewMode ( ))
				m_geom_displayColor	= nextColor (
					*this, m_geom_displayColor, -.37, -.05, .18, getBackground ( ));
			break;
    	case Utils::Entity::TopoFace		:
    	case Utils::Entity::TopoCoFace		:
    	case Utils::Entity::TopoBlock		:
			properties->setShrinkFactor(0.9);
    	case Utils::Entity::TopoVertex		:
    	case Utils::Entity::TopoEdge		:
    	case Utils::Entity::TopoCoEdge		:
			properties->setCloudColor (m_topo_displayColor);
			properties->setWireColor (m_topo_displayColor);
			properties->setSurfacicColor (m_topo_displayColor);
			properties->setFontColor (m_topo_displayColor);
			break;
    	case Utils::Entity::MeshCloud		:
    	case Utils::Entity::MeshLine		:
        case Utils::Entity::MeshSurface		:
        case Utils::Entity::MeshSubSurface	:
        case Utils::Entity::MeshVolume		:
        case Utils::Entity::MeshSubVolume	:
			properties->setCloudColor (m_mesh_displayColor);
//			properties->setWireColor (m_mesh_displayColor);
			properties->setWireColor (TkUtil::Color(255, 255, 255)); 
			properties->setSurfacicColor (m_mesh_displayColor);
			properties->setFontColor (m_mesh_displayColor);
			if (false == isPreviewMode ( ))
 			       	m_mesh_displayColor = nextColor (
					*this, m_mesh_displayColor, .18, -.05, -.37, getBackground ( ));
			break;
		case Utils::Entity::StructuredMesh		:
			properties->setWireColor (TkUtil::Color(255, 255, 255)); 
			properties->setSurfacicColor (m_mesh_displayColor);
			properties->setFontColor (m_mesh_displayColor);
			break;
		case Utils::Entity::Group0D			:
		case Utils::Entity::Group1D			:
		case Utils::Entity::Group2D			:
		case Utils::Entity::Group3D			:
			properties->setCloudColor (m_mesh_displayColor);
			properties->setWireColor (m_mesh_displayColor);
			properties->setSurfacicColor (m_mesh_displayColor);
			if (false == isPreviewMode ( ))
        			m_group_displayColor    = nextColor (
					*this, m_group_displayColor, -.05, -.37, .18, getBackground( ));
		case Utils::Entity::SysCoord        :
			;
	}	// switch (ot)

	return properties;
}
/*----------------------------------------------------------------------------*/
void Context::newGraphicalRepresentation (Utils::Entity& entity)
{
    if (!isGraphical())
        return;

	Utils::DisplayProperties::GraphicalRepresentation*	representation	=
		Utils::GraphicalRepresentationFactoryIfc::getInstance( ).create(entity);
	if (0 != representation)
		entity.getDisplayProperties ( ).setGraphicalRepresentation (representation);
}	// Context::newGraphicalRepresentation
/*----------------------------------------------------------------------------*/
void Context::undo()
{
    getCommandManager().undo();
}
void Context::redo()
{
    getCommandManager().redo();
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> Context::getSelectedEntities ( ) const
{
   return getSelectionManager().getEntitiesNames();
}
/*----------------------------------------------------------------------------*/
void Context::addToSelection (std::vector<std::string>& names)
{
	std::vector<Mgx3D::Utils::Entity*> entities;

	for (std::vector<std::string>::const_iterator iter = names.begin();
			iter != names.end(); ++iter){
		const std::string& name = *iter;
		Utils::Entity& entity = nameToEntity(name);
		entities.push_back(&entity);
	} // end for iter = names.begin()

	getSelectionManager().addToSelection(entities);
}
/*----------------------------------------------------------------------------*/
void Context::removeFromSelection (std::vector<std::string>& names)
{
	std::vector<Mgx3D::Utils::Entity*> entities;

	for (std::vector<std::string>::const_iterator iter = names.begin();
			iter != names.end(); ++iter){
		const std::string& name = *iter;
		Utils::Entity& entity = nameToEntity(name);
		entities.push_back(&entity);
	} // end for iter = names.begin()

	getSelectionManager().removeFromSelection(entities);
}
/*----------------------------------------------------------------------------*/
std::string Context::newScriptingFileName()
{
    TkUtil::UserData ud(true);
    std::string baseName = std::string("/tmp/") + ud.getName() + "/mgx_";
    TkUtil::UTF8String fileName (TkUtil::Charset::UTF_8);

    bool fileExists = true;
    unsigned long id = 0;
    do {
        fileName.clear();
        fileName << baseName << TkUtil::setw (4) << id++ << ".py";
        TkUtil::File file(fileName);
        fileExists = file.exists();
    }
    while (fileExists);

    return fileName;
}
/*----------------------------------------------------------------------------*/
TkUtil::Color Context::getBackground ( ) const
{
	return m_backgroundColor;
}
/*----------------------------------------------------------------------------*/
void Context::setBackground (const TkUtil::Color& color)
{
	m_backgroundColor	= color;
}
/*----------------------------------------------------------------------------*/
void Context::beginImportScript()
{
    // s'il y a des entités de construites, il faut mémoriser les ids actuels
    // pour décaler les noms des entités importées

    getNameManager().activateShiftingId();
}
/*----------------------------------------------------------------------------*/
void Context::endImportScript()
{
    getNameManager().unactivateShiftingId();
}
/*----------------------------------------------------------------------------*/
void Context::savePythonScript (std::string fileName, encodageScripts enc, TkUtil::Charset::CHARSET charset)
{
//	std::cout<<"Context::savePythonScript("<<fileName<<", "
//			<<(withoutEntityName?"true":"false")<<")"<<std::endl;

	ScriptingManager scripting_manager(this);
	scripting_manager.setCharset (charset);
	std::vector<Utils::Command*> commands = getLocalCommandManager().getCommands();

	if (enc == Context::WITHNAMES || enc == Context::WITHIDREF){
		scripting_manager.initPython(fileName);
	}

	if (enc == Context::WITHIDREF) {
		TkUtil::UTF8String   shiftingLine (TkUtil::Charset::UTF_8);
		shiftingLine << getContextAlias() << ".unactivateShiftingNameId()";
		scripting_manager.addComment("Désactivation du décalage automatique des noms en cas d'importation à la suite d'autres commandes");
		scripting_manager.addCommand(shiftingLine);
		scripting_manager.addCommand("");
	}

	if (enc == Context::WITHNAMES)
		for (uint i=0; i<commands.size(); i++){
			Utils::Command* command = commands[i];
			if (command->isCompleted() && command->getStatus() != Utils::CommandIfc::FAIL){
				scripting_manager.addComment(command->getInitialScriptComments());
				scripting_manager.addCommand(command->getInitialScriptCommand());
			}
		}
	else if (enc == Context::WITHIDREF) {
		// cas où l'on va substituer les noms des entités par une référence d'un objet d'une commande précédente
		std::vector<TkUtil::UTF8String> commandsWithIdRef =
				EntitiesHelper::replaceNameByCommandRef(commands);
		for (uint i=0; i<commands.size(); i++){
			Utils::Command* command = commands[i];
			if (command->isCompleted() && command->getStatus() != Utils::CommandIfc::FAIL){
				scripting_manager.addComment(command->getInitialScriptComments());
				scripting_manager.addComment(command->getInitialScriptCommand());
				scripting_manager.addCommand(commandsWithIdRef[i]);
			}
		}
	}
	else if (enc == Context::WITHCOORD) {
		// cas où l'on va substituer les noms des entités par les coordonnées d'un ou plusieurs points pour retrouver celles-ci

		// on fait comme pour le cas WITHNAMES dans un fichier temporaire
		std::string fileNameTmp = getScriptingManager().getPythonWriter()->getFileName();
		fileNameTmp = scriptToMinScript(fileNameTmp);
		scripting_manager.initPython(fileNameTmp);

		TkUtil::UTF8String   shiftingLine (TkUtil::Charset::UTF_8);
		shiftingLine << getContextAlias() << ".unactivateShiftingNameId()";
		scripting_manager.addComment("Désactivation du décalage automatique des noms en cas d'importation à la suite d'autres commandes");
		scripting_manager.addCommand(shiftingLine);
		scripting_manager.addCommand("");

		for (uint i=0; i<commands.size(); i++){
			Utils::Command* command = commands[i];
			if (command->isCompleted() && command->getStatus() != Utils::CommandIfc::FAIL){
				scripting_manager.addComment(command->getInitialScriptComments());
				scripting_manager.addCommand(command->getInitialScriptCommand());
			}
		}

		// appel à la version batch de Magix3D pour faire une exécution du script
		// et utilisation des coordonnées pour retrouver les entités

		TkUtil::UTF8String exeName = getExeName();
		// on en déduit le nom de la version batch
		size_t pos = exeName.find("/Magix3D-");
		if (pos == -1){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "On ne trouve pas le nom de l'exe (Magix3D) dans le path de l'exécutable "<<exeName;
            throw TkUtil::Exception (message);
		}

		TkUtil::UTF8String exeBatch = exeName.substring(0,pos+7);
		exeBatch << "Batch";
		exeBatch << exeName.substring(pos+8, exeName.length());

#ifdef _DEBUG
		std::cout<<"exeBatch : "<<exeBatch<<std::endl;
#endif
		TkUtil::Process* mgxBatch = new TkUtil::Process (exeBatch);
		mgxBatch->getOptions ( ).addOption ("--coordTranslate");
		mgxBatch->getOptions ( ).addOption (fileNameTmp);
		mgxBatch->getOptions ( ).addOption (fileName);
		mgxBatch->execute (true);

	}
	else {
		MGX_FORBIDDEN("Cas pour encodageScripts non prévu");
	}
}
/*----------------------------------------------------------------------------*/
void Context::activateShiftingNameId()
{
	getNameManager().activateShiftingId();
}
/*----------------------------------------------------------------------------*/
void Context::unactivateShiftingNameId()
{
	getNameManager().unactivateShiftingId();
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* Context::setLandmark(Utils::Landmark::kind l)
{
    //creation de la commande
	CommandChangeLandmark* command = new CommandChangeLandmark(*this, l);

	// trace dans le script
	TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
	cmd << getContextAlias() << ".setLandmark(";
	cmd <<Utils::Landmark::getScriptCommand(l)<<")";
	command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* Context::setLengthUnit(const Utils::Unit::lengthUnit& lu)
{
    //creation de la commande
	CommandChangeLengthUnit* command = new CommandChangeLengthUnit(*this, lu);

	// trace dans le script
	TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
	cmd << getContextAlias() << ".setLengthUnit(";
	cmd <<Utils::Unit::getScriptCommand(lu)<<")";
	command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* Context::setMesh2D()
{
    //creation de la commande
	Mesh::CommandChangeMeshDim* command = new Mesh::CommandChangeMeshDim(*this, MESH2D);

	// trace dans le script
	TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
	cmd << getContextAlias() << ".setMesh2D()";
	command->setScriptCommand(cmd);

    // on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

    Internal::M3DCommandResultIfc*  cmdResult   =
            new Internal::M3DCommandResult (*command);
    return cmdResult;
}
/*----------------------------------------------------------------------------*/
const std::vector<Utils::Entity*> Context::getAllVisibleEntities()
{
	std::vector<Utils::Entity*> entities;
	for (std::map<unsigned long, Utils::Entity*>::iterator ite = _entities.begin();
			ite != _entities.end(); ++ite)
		if ((*ite).second->isVisible())
			entities.push_back((*ite).second);
	return entities;
}
/*----------------------------------------------------------------------------*/
void Context::clearSession()
{
	setFinished(true);

	// annulation de la sélection
	m_selection_manager->clearSelection ( );

    //creation de la commande de destruction
	Internal::CommandComposite* command = new Internal::CommandComposite(*this, TkUtil::UTF8String ("Destruction de tout, les commandes précédentes n'ont plus lieu d'être ! ", TkUtil::Charset::UTF_8));

	// trace dans le script
	TkUtil::UTF8String cmd (TkUtil::Charset::UTF_8);
	cmd << getContextAlias() << ".clearSession()";
	command->setScriptCommand(cmd);

	// commande de destruction de toutes les entités maillages qui ont pu être construites
	Mesh::CommandDestroyMesh* commandMesh = new Mesh::CommandDestroyMesh(*this);

	command->addCommand(commandMesh);

	// commande de destruction de toutes les entités topologiques qui ont pu être construites
	std::vector<Topo::TopoEntity*> ve;
	std::vector<Topo::Block*> blocks;
	getLocalTopoManager().getBlocks(blocks);
	for (uint i=0; i<blocks.size(); i++)
		ve.push_back(blocks[i]);
	std::vector<Topo::CoFace*> cofaces;
	getLocalTopoManager().getCoFaces(cofaces);
	for (uint i=0; i<cofaces.size(); i++)
		ve.push_back(cofaces[i]);
	std::vector<Topo::CoEdge*> coedges;
	getLocalTopoManager().getCoEdges(coedges);
	for (uint i=0; i<coedges.size(); i++)
		ve.push_back(coedges[i]);
	std::vector<Topo::Vertex*> vertices;
	getLocalTopoManager().getVertices(vertices);
	for (uint i=0; i<vertices.size(); i++)
		ve.push_back(vertices[i]);

	Topo::CommandDestroyTopo* commandTopo = new Topo::CommandDestroyTopo(*this, ve, true);

	command->addCommand(commandTopo);

	// commande de destruction de toutes les entités géométriques qui ont pu être construites
	std::vector<Geom::GeomEntity*> entities;
	std::vector<Geom::Volume*> volumes = getLocalGeomManager().getVolumesObj();
	for (uint i=0; i<volumes.size(); i++)
		entities.push_back(volumes[i]);
	std::vector<Geom::Surface*> surfaces = getLocalGeomManager().getSurfacesObj();
	for (uint i=0; i<surfaces.size(); i++)
		entities.push_back(surfaces[i]);
	std::vector<Geom::Curve*> curves = getLocalGeomManager().getCurvesObj();
	for (uint i=0; i<curves.size(); i++)
		entities.push_back(curves[i]);
	std::vector<Geom::Vertex*> vtx = getLocalGeomManager().getVerticesObj();
	for (uint i=0; i<vtx.size(); i++)
		entities.push_back(vtx[i]);

	Geom::CommandRemove* commandGeom = new Geom::CommandRemove(*this, entities, true);

	command->addCommand(commandGeom);

	// TODO [EB] Destruction des SysCoord

	// on passe au gestionnaire de commandes qui exécute la commande en // ou non
    // et la stocke dans le gestionnaire de undo-redo si c'est une réussite
    getCommandManager().addCommand(command, Utils::Command::DO);

	m_command_manager->clear();
	m_name_manager->clear();
	m_geom_manager->clear();
	m_topo_manager->clear();
	m_mesh_manager->clear();
	m_group_manager->clear();
	m_sys_coord_manager->clear();

	m_landmark = Utils::Landmark::undefined;
	m_length_unit = Utils::Unit::undefined;

#ifdef _DEBUG2
	Utils::Entity::showEntitiesStats(std::cout);
#endif

	clearIdToEntity ( );

	// Réinitialisation des couleurs attribuées => si on rejoue la même séquence de commandes se sont les mêmes
	// couleurs qui seront attribuées aux entités créés :
	m_geom_displayColor	= m_initial_geom_displayColor;
	m_mesh_displayColor	= m_initial_mesh_displayColor;
	m_group_displayColor	= m_initial_group_displayColor;

	setFinished(false);

	// on ne peut pas retourner un M3DCommandResultIfc, la commande est détruite par m_command_manager->clear()
}
/*----------------------------------------------------------------------------*/
void Context::add(unsigned long id,  Utils::Entity* entity)
{
	TkUtil::AutoMutex	autoMutex (&_entitiesMutex);
	_entities.insert (std::pair<unsigned long, Utils::Entity*>(id, entity));
}
/*----------------------------------------------------------------------------*/
void Context::remove(unsigned long id)
{
	if (!isIdToEntityEmpty()){

		TkUtil::AutoMutex	autoMutex (&_entitiesMutex);
		std::map<unsigned long, Utils::Entity*>::iterator ite =
				_entities.find (id);
		if (_entities.end ( ) != ite){
			_entities.erase (ite);
		}
		else
		{
//			std::cerr << "Erreur dans Context::remove ("
//					<< __FILE__ << ' ' << __LINE__
//					<< ") : entité d'id " << id
//					<< " non enregistré dans la table _entities."
//					<< std::endl;
#ifdef _DEBUG
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Erreur dans Context::remove(" << id
					<< ") non enregistré dans la table _entities.";
			throw TkUtil::Exception (message);
#endif
		}	// else if (_entities.end ( ) != ite)
	}

}
/*----------------------------------------------------------------------------*/
std::vector<Utils::Entity*> Context::get (
					const std::vector<unsigned long>& ids, bool raises) const
{
	std::vector<Utils::Entity*>	entities;

	for (std::vector<unsigned long>::const_iterator it = ids.begin ( );
	     ids.end ( ) != it; it++)
	{
		std::map<unsigned long, Utils::Entity*>::const_iterator ite	=
														_entities.find (*it);
		if (_entities.end ( ) != ite)
			entities.push_back ((*ite).second);
		else
		{
			if (false == raises)
				continue;

			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Entité d'identifiant " << *it <<" absente du contexte.";
			throw TkUtil::Exception (message);
		}
	}

	return entities;
}	// ContextIfc::get
/*----------------------------------------------------------------------------*/
void Context::clearIdToEntity()
{
	TkUtil::AutoMutex	autoMutex (&_entitiesMutex);
#ifdef _DEBUG
	if (0 != _entities.size ( ))
		std::cerr << __FILE__ << ' ' << __LINE__ << " Context::clearIdToEntity : "
		<< "la liste contient " << _entities.size ( ) << " entités."
		<< std::endl;
	//showAllEntities(std::cout);
#endif
	_entities.clear();
}
/*----------------------------------------------------------------------------*/
void Context::showAllEntities(std::ostream & o)
{
	o << "======== Context::showAllEntities() ========"<<std::endl;

	for (std::map<unsigned long, Utils::Entity*>::iterator ite = _entities.begin();
			ite != _entities.end(); ++ite){
		o << ite->second->getName()
        		  <<" (type "<<ite->second->getTypeName()
        		  <<") (uid "<<ite->first<<")";
		if (ite->second->isDestroyed())
			o << " [marquée à DETRUITE]";
		o << std::endl;
	}

	o << "===================== end ================="<<std::endl;
}
/*----------------------------------------------------------------------------*/
void Context::showEntitiesStats(std::ostream & o)
{
	o << "======== Entity::showEntitiesStats() ========"<<std::endl;

	uint nbGeom = 0;
	uint nbTopo = 0;
	uint nbMesh = 0;
	uint nbGroup = 0;
	uint nbReperes = 0;

	for (std::map<unsigned long, Utils::Entity*>::iterator ite = _entities.begin();
			ite != _entities.end(); ++ite){
#ifdef _DEBUG_MEMORY
		std::cout<<"oubli de : "<<ite->second->getName()<<std::endl;
#endif
		nbGeom += (ite->second->isGeomEntity()?1:0);
		nbTopo += (ite->second->isTopoEntity()?1:0);
		nbMesh += (ite->second->isMeshEntity()?1:0);
		nbGroup += (ite->second->isGroupEntity()?1:0);
		nbReperes += (ite->second->isSysCoordEntity()?1:0);
	}
	if (nbGeom)
		o << "nombre d'entités géométriques non détruites: "<<nbGeom<<std::endl;
	if (nbTopo)
		o << "nombre d'entités topologiques non détruites: "<<nbTopo<<std::endl;
	if (nbMesh)
		o << "nombre d'entités de maillage non détruites:  "<<nbMesh<<std::endl;
	if (nbGroup)
		o << "nombre d'entités groupe non détruites:       "<<nbGroup<<std::endl;
	if (nbReperes)
		o << "nombre d'entités repères non détruites:       "<<nbReperes<<std::endl;

	o << "===================== end ================="<<std::endl;
}
/*----------------------------------------------------------------------------*/
bool Context::isIdToEntityEmpty()
{
	return _entities.empty();
}
/*----------------------------------------------------------------------------*/
Utils::Entity& Context::uniqueIdToEntity(unsigned long uid) const
{
	TkUtil::AutoMutex	autoMutex ((TkUtil::Mutex*)&_entitiesMutex);

	std::map<unsigned long, Utils::Entity*>::const_iterator ite	= _entities.find (uid);

	if (_entities.end ( ) == ite)
	{
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Entité d'identifiant unique " << uid
				<< " non enregistrée dans la liste des entités.";
		message << "\n" << "Entités de la liste :";
		for (ite = _entities.begin ( ); ite != _entities.end ( ); ite++)
			message << ' ' << (*ite).first;
		throw TkUtil::Exception (message);
	}	// if (_entities.end ( ) == ite)

	return *((*ite).second);
}	// Context::uniqueIdToEntity
/*----------------------------------------------------------------------------*/
Utils::Entity& Context::nameToEntity (const std::string& name) const
{	// Peu de chance que ce soit très performant ...
	TkUtil::AutoMutex	autoMutex ((TkUtil::Mutex*)&_entitiesMutex);

	Utils::Entity* ge = 0;
	for (std::map<unsigned long, Utils::Entity*>::const_iterator ite	= _entities.begin ( );
			_entities.end ( ) != ite; ite++)
		if (name == (*ite).second->getName ( )){
			if ((*ite).second->isGroupEntity())
				ge = (*ite).second;
			else
				return *((*ite).second);
		}
	// on prend les groupes uniquement si l'on ne trouve rien d'autre
	// => permet de sélectionner les groupes de mailles en priorité
	if (ge)
		return *ge;

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "Entité de nom " << name
			<< " non enregistrée dans la liste des entités.";
	throw TkUtil::Exception (message);
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/

Internal::Context* getStdContext ()
{
	try {
		return dynamic_cast<Internal::Context*>(getFirstContextIfc ());
	}
	catch (const TkUtil::Exception& exc) {
		// si le contexte n'existe pas, on en créé un avec comme nom "python"
		//std::cout << __FILE__ << ' ' << __LINE__ << " Context::getContext. Contexte " << name << " non trouvé. Création d'une instance de la classe Internal::Context." << std::endl;
		char* env = getenv ("CODE");
		if (env == 0 || strlen(env) == 0){
//			LM_set_codename("python");
			//std::cout<<"LM_set_codename(python)"<<std::endl;

		    // logues version ~ Magix
			Utils::maillage_log(0, 0, "Magix3D");
		}
		TkUtil::ThreadManager::initialize (0);
		TkUtil::ThreadPool::initialize (0);

		return new Internal::Context("python");
	}
}	// getStdContext

Internal::Context* getContext (const char* name)
{
	try {
		return dynamic_cast<Internal::Context*>(getContextIfc (name));
	}
	catch (const TkUtil::Exception& exc) {
		// si le contexte n'existe pas, on en créé un avec ce nom
		//std::cout << __FILE__ << ' ' << __LINE__ << " Context::getContext. Contexte " << name << " non trouvé. Création d'une instance de la classe Internal::Context." << std::endl;
		return new Internal::Context(name);
	}
}   // getContex

std::string getVersion()
{
	return MAGIX3D_VERSION;	// GSCC_PROJECT_VERSION;
}

/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
