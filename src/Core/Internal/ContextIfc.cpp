/*----------------------------------------------------------------------------*/
/*
 * \file ContextIfc.cpp
 *
 *  \author Franck Ledoux, Eric Brière de l'Isle, Charles Pignerol
 *
 *  \date 31/01/2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
/*----------------------------------------------------------------------------*/
#include "Internal/NameManager.h"

#include <TkUtil/Mutex.h>
#include <TkUtil/Exception.h>
#include <PrefsCore/PreferencesHelper.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/UserData.h>

#include <string.h>
#include <pthread.h>


/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {

/**
 * La liste des contextes instanciés.
 */
static std::vector<ContextIfc*>	contexts;

/**
 * Un mutex pour protéger <I>contexts</I>.
 */
static TkUtil::Mutex			contextsMutex;

pthread_t						ContextIfc::threadId	= (pthread_t)-1;


/*----------------------------------------------------------------------------*/
void ContextIfc::initialize ( )
{
	threadId	= pthread_self ( );
}

/*----------------------------------------------------------------------------*/
void ContextIfc::finalize ( )
{
	threadId	= (pthread_t)-1;
}

/*----------------------------------------------------------------------------*/
void ContextIfc::load (const Preferences::Section& contextSection)
{
	try
	{
		Preferences::Section&	threadingSection	=
									contextSection.getSection ("threading");
		Preferences::Section&	scriptingSection	=
									contextSection.getSection ("scripting");
		Preferences::Section&	optimizingSection	=
									contextSection.getSection ("optimizing");

		Preferences::PreferencesHelper::getBoolean (
								threadingSection, allowThreadedCommandTasks);

		Preferences::PreferencesHelper::getBoolean (
								threadingSection, allowThreadedEdgePreMeshTasks);

		Preferences::PreferencesHelper::getBoolean (
								threadingSection, allowThreadedFacePreMeshTasks);

		Preferences::PreferencesHelper::getBoolean (
								threadingSection, allowThreadedBlockPreMeshTasks);

		Preferences::PreferencesHelper::getBoolean (
								scriptingSection, displayScriptOutputs);

		Preferences::PreferencesHelper::getBoolean (
				                optimizingSection, memorizeEdgePreMesh);
	}
	catch (...)
	{
	}
}

/*----------------------------------------------------------------------------*/
void ContextIfc::saveConfiguration (Preferences::Section& contextSection)
{
	Preferences::Section&	threadingSection	=
				Preferences::PreferencesHelper::getSection (contextSection, "threading");
	Preferences::Section&	scriptingSection	=
				Preferences::PreferencesHelper::getSection (contextSection, "scripting");
	Preferences::Section&	optimizingSection	=
			    Preferences::PreferencesHelper::getSection (contextSection, "optimizing");

	Preferences::PreferencesHelper::updateBoolean (
									threadingSection, allowThreadedCommandTasks);
	Preferences::PreferencesHelper::updateBoolean (
									threadingSection, allowThreadedEdgePreMeshTasks);
	Preferences::PreferencesHelper::updateBoolean (
									threadingSection, allowThreadedFacePreMeshTasks);
	Preferences::PreferencesHelper::updateBoolean (
									threadingSection, allowThreadedBlockPreMeshTasks);
	Preferences::PreferencesHelper::updateBoolean (
									scriptingSection, displayScriptOutputs);
	Preferences::PreferencesHelper::updateBoolean (
			                        optimizingSection, memorizeEdgePreMesh);
}

/*----------------------------------------------------------------------------*/
ContextIfc::ContextIfc(const std::string& name)
: m_name (name),
allowThreadedCommandTasks (
		TkUtil::UTF8String ("allowThreadedCommandTasks", TkUtil::Charset::UTF_8), true,
		TkUtil::UTF8String ("true si une commande est autorisée à être décomposée en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.", TkUtil::Charset::UTF_8)),
allowThreadedEdgePreMeshTasks (
		TkUtil::UTF8String ("allowThreadedEdgePreMeshTasks", TkUtil::Charset::UTF_8), true,
		TkUtil::UTF8String ("true si le prémaillage des arêtes peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.", TkUtil::Charset::UTF_8)),
allowThreadedFacePreMeshTasks (
		TkUtil::UTF8String ("allowThreadedFacePreMeshTasks", TkUtil::Charset::UTF_8), true,
		TkUtil::UTF8String ("true si le prémaillage des faces peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.", TkUtil::Charset::UTF_8)),
allowThreadedBlockPreMeshTasks (
		TkUtil::UTF8String ("allowThreadedBlockPreMeshTasks", TkUtil::Charset::UTF_8), true,
		TkUtil::UTF8String ("true si le prémaillage des blocs peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.", TkUtil::Charset::UTF_8)),
displayScriptOutputs (
		TkUtil::UTF8String ("displayScriptOutputs", TkUtil::Charset::UTF_8), true,
		TkUtil::UTF8String ("true si le programme doit afficher les sorties des commandes script, false dans le cas contraire.", TkUtil::Charset::UTF_8)),
memorizeEdgePreMesh (
		TkUtil::UTF8String ("memorizeEdgePreMesh", TkUtil::Charset::UTF_8), true,
		TkUtil::UTF8String ("true si le programme doit mémoriser le prémaillage des arêtes, false dans le cas contraire.", TkUtil::Charset::UTF_8))

{
	// Enregistrement auprès de la liste des contextes. On en profite pour
	// s'assurer de l'unicité du nom.
	TkUtil::AutoMutex	autoMutex (&contextsMutex);
	for (std::vector<ContextIfc*>::iterator it = contexts.begin ( );
	     contexts.end ( ) != it; it++)
	{
		if ((*it)->getName ( ) == name)
		{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Création d'un contexte de nom \"" << name
			        << "\" : opération impossible, nom déjà utilisé par une "
			        << "autre instance.";
			throw TkUtil::Exception (message);
		}	// if ((*it).getName ( ) == name)
	}	// for (std::vector<ContextIfc*>::iterator it = contexts.begin ( );
	contexts.push_back (this);
}
/*----------------------------------------------------------------------------*/
ContextIfc::ContextIfc (const ContextIfc&)
: m_name ("Bidon"),
allowThreadedCommandTasks (
		TkUtil::UTF8String ("allowThreadedCommandTasks", TkUtil::Charset::UTF_8), true,
		TkUtil::UTF8String ("true si une commande est autorisée à être décomposée en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.", TkUtil::Charset::UTF_8)),
allowThreadedEdgePreMeshTasks (
		TkUtil::UTF8String ("allowThreadedEdgePreMeshTasks", TkUtil::Charset::UTF_8), true,
		TkUtil::UTF8String ("true si le prémaillage des arêtes peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.", TkUtil::Charset::UTF_8)),
allowThreadedFacePreMeshTasks (
		TkUtil::UTF8String ("allowThreadedFacePreMeshTasks", TkUtil::Charset::UTF_8), true,
		TkUtil::UTF8String ("true si le prémaillage des faces peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.", TkUtil::Charset::UTF_8)),
allowThreadedBlockPreMeshTasks (
		TkUtil::UTF8String ("allowThreadedBlockPreMeshTasks", TkUtil::Charset::UTF_8), true,
		TkUtil::UTF8String ("true si le prémaillage des blocs peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.", TkUtil::Charset::UTF_8)),
displayScriptOutputs (
		TkUtil::UTF8String ("displayScriptOutputs", TkUtil::Charset::UTF_8), true,
		TkUtil::UTF8String ("true si le programme doit afficher les sorties des commandes script, false dans le cas contraire.", TkUtil::Charset::UTF_8)),
memorizeEdgePreMesh (
		TkUtil::UTF8String ("memorizeEdgePreMesh", TkUtil::Charset::UTF_8), true,
		TkUtil::UTF8String ("true si le programme doit mémoriser le prémaillage des arêtes, false dans le cas contraire.", TkUtil::Charset::UTF_8))
{
    MGX_FORBIDDEN ("ContextIfc copy constructor is not allowed.");
}	// ContextIfc::ContextIfc
/*----------------------------------------------------------------------------*/
ContextIfc& ContextIfc::operator = (const ContextIfc& c)
{
    MGX_FORBIDDEN ("ContextIfc assignment operator is not allowed.");
	if (&c != this)
	{
	}	// if (&c != this)

	return *this;
}	// ContextIfc::operator =
/*----------------------------------------------------------------------------*/
ContextIfc::~ContextIfc()
{
	// Déréférencement auprès de la liste des contextes :
	TkUtil::AutoMutex	autoMutex (&contextsMutex);
	for (std::vector<ContextIfc*>::iterator it = contexts.begin ( );
	     contexts.end ( ) != it; it++)
	{
		if (this == *it)
		{
			contexts.erase (it);
			break;
		}	// if (this == *it)
	}	// for (std::vector<ContextIfc*>::iterator it = contexts.begin ( ); ...
}
/*----------------------------------------------------------------------------*/
bool ContextIfc::isFinished ( )
{
	throw TkUtil::Exception ("ContextIfc::isFinished should be overloaded.");
}	// ContextIfc::isFinished
/*----------------------------------------------------------------------------*/
void ContextIfc::setFinished (bool b)
{
	throw TkUtil::Exception ("ContextIfc::setFinished should be overloaded.");
}	// ContextIfc::isFinished
/*----------------------------------------------------------------------------*/
std::string ContextIfc::createName (const std::string& base) const
{
	throw TkUtil::Exception ("ContextIfc::createName should be overloaded.");
}	// ContextIfc::createName
/*----------------------------------------------------------------------------*/
const std::string& ContextIfc::getName ( ) const
{
	return m_name;
}	// ContextIfc::getName
/*----------------------------------------------------------------------------*/
void ContextIfc::setName (const std::string& name)
{
	m_name	= name;
}
/*----------------------------------------------------------------------------*/
TkUtil::PythonSession& ContextIfc::getPythonSession ( )
{
	throw TkUtil::Exception ("ContextIfc::getPythonSession should be overloaded.");
}	// ContextIfc::getPythonSession
/*----------------------------------------------------------------------------*/
void ContextIfc::setPythonSession (TkUtil::PythonSession*)
{
	throw TkUtil::Exception ("ContextIfc::getPythonSession should be overloaded.");
}	// ContextIfc::setPythonSession
/*----------------------------------------------------------------------------*/
Geom::GeomManagerIfc& ContextIfc::getGeomManager ( )
{
	throw TkUtil::Exception ("ContextIfc::getGeomManager should be overloaded.");
}	// ContextIfc::getGeomManager
/*----------------------------------------------------------------------------*/
const Geom::GeomManagerIfc& ContextIfc::getGeomManager ( ) const
{
	throw TkUtil::Exception ("ContextIfc::getGeomManager should be overloaded.");
}	// ContextIfc::getGeomManager
/*----------------------------------------------------------------------------*/
void ContextIfc::setGeomManager (Geom::GeomManagerIfc*)
{
	throw TkUtil::Exception ("ContextIfc::getGeomManager should be overloaded.");
}	// ContextIfc::setGeomManager
/*----------------------------------------------------------------------------*/
Topo::TopoManagerIfc& ContextIfc::getTopoManager ( )
{
	throw TkUtil::Exception ("ContextIfc::getTopoManager should be overloaded.");
}	// ContextIfc::getTopoManager
/*----------------------------------------------------------------------------*/
const Topo::TopoManagerIfc& ContextIfc::getTopoManager ( ) const
{
	throw TkUtil::Exception ("ContextIfc::getTopoManager should be overloaded.");
}	// ContextIfc::getTopoManager
/*----------------------------------------------------------------------------*/
Mesh::MeshManagerIfc& ContextIfc::getMeshManager ( )
{
	throw TkUtil::Exception ("ContextIfc::getMeshManager should be overloaded.");
}	// ContextIfc::getMeshManager
/*----------------------------------------------------------------------------*/
const Mesh::MeshManagerIfc& ContextIfc::getMeshManager ( ) const
{
	throw TkUtil::Exception ("ContextIfc::getMeshManager should be overloaded.");
}	// ContextIfc::getMeshManager
/*----------------------------------------------------------------------------*/
Group::GroupManagerIfc& ContextIfc::getGroupManager()
{
    throw TkUtil::Exception ("ContextIfc::getGroupManager should be overloaded.");
}   // ContextIfc::getGroupManager
/*----------------------------------------------------------------------------*/
const Group::GroupManagerIfc& ContextIfc::getGroupManager() const
{
    throw TkUtil::Exception ("ContextIfc::getGroupManager should be overloaded.");
}   // ContextIfc::getGroupManager
/*----------------------------------------------------------------------------*/
Mgx3D::CoordinateSystem::SysCoordManagerIfc& ContextIfc::getSysCoordManager()
{
	throw TkUtil::Exception ("ContextIfc::getSysCoordManager should be overloaded.");
}
/*----------------------------------------------------------------------------*/
const Mgx3D::CoordinateSystem::SysCoordManagerIfc& ContextIfc::getSysCoordManager() const
{
	throw TkUtil::Exception ("ContextIfc::getSysCoordManager should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Structured::StructuredMeshManagerIfc& ContextIfc::getStructuredMeshManager ( )
{
	throw TkUtil::Exception ("ContextIfc::getStructuredMeshManager should be overloaded.");
}	// ContextIfc::getStructuredMeshManager
/*----------------------------------------------------------------------------*/
const Structured::StructuredMeshManagerIfc& ContextIfc::getStructuredMeshManager ( ) const
{
	throw TkUtil::Exception ("ContextIfc::getStructuredMeshManager should be overloaded.");
}	// ContextIfc::getStructuredMeshManager
/*----------------------------------------------------------------------------*/
Internal::M3DCommandManager& ContextIfc::getM3DCommandManager ( )
{
	throw TkUtil::Exception ("ContextIfc::getM3DCommandManager should be overloaded.");
}	// ContextIfc::getM3DCommandManager
/*----------------------------------------------------------------------------*/
Utils::SelectionManagerIfc& ContextIfc::getSelectionManager ( )
{
	throw TkUtil::Exception ("ContextIfc::getSelectionManager should be overloaded.");
}	// ContextIfc::getSelectionManager
/*----------------------------------------------------------------------------*/
const Utils::SelectionManagerIfc& ContextIfc::getSelectionManager ( ) const
{
	throw TkUtil::Exception ("ContextIfc::getSelectionManager should be overloaded.");
}	// ContextIfc::getSelectionManager
/*----------------------------------------------------------------------------*/
void ContextIfc::setSelectionManager (Utils::SelectionManagerIfc*)
{
	throw TkUtil::Exception ("ContextIfc::setSelectionManager should be overloaded.");
}	// ContextIfc::setSelectionManager
/*----------------------------------------------------------------------------*/
Utils::CommandManagerIfc& ContextIfc::getCommandManager ( )
{
	throw TkUtil::Exception ("ContextIfc::getCommandManager should be overloaded.");
}	// ContextIfc::getCommandManager
/*----------------------------------------------------------------------------*/
TkUtil::LogDispatcher& ContextIfc::getLogDispatcher ( )
{
	throw TkUtil::Exception ("ContextIfc::getLogDispatcher should be overloaded.");
}	// ContextIfc::getLogDispatcher
/*----------------------------------------------------------------------------*/
const TkUtil::LogDispatcher& ContextIfc::getLogDispatcher ( ) const
{
	throw TkUtil::Exception ("ContextIfc::getLogDispatcher should be overloaded.");
}	// ContextIfc::getLogDispatcher
/*----------------------------------------------------------------------------*/
TkUtil::LogOutputStream* ContextIfc::getLogStream ( )
{
	throw TkUtil::Exception ("ContextIfc::getLogStream should be overloaded.");
}	// ContextIfc::getLogStream
/*----------------------------------------------------------------------------*/
Internal::ScriptingManager& ContextIfc::getScriptingManager ( )
{
	throw TkUtil::Exception ("ContextIfc::getScriptingManager should be overloaded.");
}	// ContextIfc::getScriptingManager
/*----------------------------------------------------------------------------*/
void ContextIfc::undo()
{
	throw TkUtil::Exception ("ContextIfc::undo should be overloaded.");
}	// ContextIfc::undo
/*----------------------------------------------------------------------------*/
void ContextIfc::redo()
{
	throw TkUtil::Exception ("ContextIfc::redo should be overloaded.");
}	// ContextIfc::redo
/*----------------------------------------------------------------------------*/
std::vector<std::string> ContextIfc::getSelectedEntities ( ) const
{
	throw TkUtil::Exception ("ContextIfc::getSelectedEntities should be overloaded.");
}	// ContextIfc::getSelectedEntities
/*----------------------------------------------------------------------------*/
void ContextIfc::addToSelection (std::vector<std::string>& names)
{
	throw TkUtil::Exception ("ContextIfc::addToSelection should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void ContextIfc::removeFromSelection (std::vector<std::string>& names)
{
	throw TkUtil::Exception ("ContextIfc::removeFromSelection should be overloaded.");
}
/*----------------------------------------------------------------------------*/
std::string ContextIfc::newScriptingFileName ( )
{
	throw TkUtil::Exception ("ContextIfc::newScriptingFileName should be overloaded.");
}	// ContextIfc::newScriptingFileName
/*----------------------------------------------------------------------------*/
bool ContextIfc::isGraphical() const
{
    throw TkUtil::Exception ("ContextIfc::isGraphical should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void ContextIfc::setGraphical(bool gr)
{
    throw TkUtil::Exception ("ContextIfc::setGraphical should be overloaded.");
}
/*----------------------------------------------------------------------------*/
TkUtil::Color ContextIfc::getBackground ( ) const
{
	throw TkUtil::Exception ("ContextIfc::getBackground should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void ContextIfc::setBackground (const TkUtil::Color& bg)
{
	throw TkUtil::Exception ("ContextIfc::setBackground should be overloaded.");
}
/*----------------------------------------------------------------------------*/
int ContextIfc::getRatioDegrad()
{
	throw TkUtil::Exception ("ContextIfc::getRatioDegrad should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void ContextIfc::setRatioDegrad(int ratio)
{
	throw TkUtil::Exception ("ContextIfc::setRatioDegrad should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void ContextIfc::beginImportScript()
{
    throw TkUtil::Exception ("ContextIfc::beginImportScript should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void ContextIfc::endImportScript()
{
    throw TkUtil::Exception ("ContextIfc::endImportScript should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void ContextIfc::savePythonScript (std::string fileName, encodageScripts enc, TkUtil::Charset::CHARSET)
{
	throw TkUtil::Exception ("ContextIfc::savePythonScript should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void ContextIfc::activateShiftingNameId()
{
	throw TkUtil::Exception ("ContextIfc::activateShiftingNameId should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void ContextIfc::unactivateShiftingNameId()
{
	throw TkUtil::Exception ("ContextIfc::unactivateShiftingNameId should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Utils::Landmark::kind ContextIfc::getLandmark() const
{
	throw TkUtil::Exception ("ContextIfc::getLandmark should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* ContextIfc::setLandmark(Utils::Landmark::kind)
{
	throw TkUtil::Exception ("ContextIfc::setLandmark should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Utils::Unit::lengthUnit ContextIfc::getLengthUnit() const
{
	throw TkUtil::Exception ("ContextIfc::getLengthUnit should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* ContextIfc::setLengthUnit(const Utils::Unit::lengthUnit& lu)
{
	throw TkUtil::Exception ("ContextIfc::setLengthUnit should be overloaded.");
}
/*----------------------------------------------------------------------------*/
ContextIfc::geomKernel ContextIfc::getGeomKernel() const
{
	throw TkUtil::Exception ("ContextIfc::getGeomKernel should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Internal::M3DCommandResultIfc* ContextIfc::setMesh2D()
{
	throw TkUtil::Exception ("ContextIfc::setMesh2D should be overloaded.");
}
/*----------------------------------------------------------------------------*/
Internal::ContextIfc::meshDim ContextIfc::getMeshDim() const
{
	throw TkUtil::Exception ("ContextIfc::getMeshDim should be overloaded.");
}
/*----------------------------------------------------------------------------*/
void ContextIfc::clearSession ()
{
	throw TkUtil::Exception ("ContextIfc::clearSession.");
}	// ContextIfc::clearSession
/*----------------------------------------------------------------------------*/
const std::vector<Utils::Entity*> ContextIfc::getAllVisibleEntities ()
{
	throw TkUtil::Exception ("ContextIfc::getAllVisibleEntities.");
}	// ContextIfc::getAllVisibleEntities
/*----------------------------------------------------------------------------*/
void ContextIfc::add (unsigned long,  Utils::Entity*)
{
	throw TkUtil::Exception ("ContextIfc::add.");
}	// ContextIfc::add
/*----------------------------------------------------------------------------*/
void ContextIfc::remove (unsigned long)
{
	throw TkUtil::Exception ("ContextIfc::remove.");
}	// ContextIfc::remove
/*----------------------------------------------------------------------------*/
std::vector<Utils::Entity*> ContextIfc::get (
								const std::vector<unsigned long>&, bool) const
{
	throw TkUtil::Exception ("ContextIfc::get.");
}	// ContextIfc::get
/*----------------------------------------------------------------------------*/
Utils::Entity& ContextIfc::uniqueIdToEntity (unsigned long uid) const
{
	throw TkUtil::Exception ("ContextIfc::uniqueIdToEntity.");
}
/*----------------------------------------------------------------------------*/
Utils::Entity& ContextIfc::nameToEntity (const std::string& name) const
{
	throw TkUtil::Exception ("ContextIfc::nameToEntity.");
}
/*----------------------------------------------------------------------------*/
std::string ContextIfc::getExeName() const
{
	throw TkUtil::Exception ("ContextIfc::getExeName.");
}
/*----------------------------------------------------------------------------*/
void ContextIfc::setExeName(std::string name)
{
	throw TkUtil::Exception ("ContextIfc::setExeName.");
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/

Internal::ContextIfc* getFirstContextIfc ()
{
    TkUtil::AutoMutex   autoMutex (&Internal::contextsMutex);

//std::cout << __FILE__ << ' ' << __LINE__ << " ContextIfc::getContextIfc : recherche du contexte de nom " << name << " ..." << std::endl;

    if (!Internal::contexts.empty())
        return Internal::contexts.front();

//std::cout << __FILE__ << ' ' << __LINE__ << " ContextIfc::getFirstContextIfc : absence de contexte " << std::endl;
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "getFirstContextIfc : absence d'un premier contexte.";
    throw TkUtil::Exception (message);
}   // getFirstContextIfc


Internal::ContextIfc* getContextIfc (const char* name)
{
    TkUtil::AutoMutex	autoMutex (&Internal::contextsMutex);

//std::cout << __FILE__ << ' ' << __LINE__ << " ContextIfc::getContextIfc : recherche du contexte de nom " << name << " ..." << std::endl;
    for (std::vector<Internal::ContextIfc*>::iterator it =
            Internal::contexts.begin ( );
            Internal::contexts.end ( ) != it; it++)
        if ((*it)->getName ( ) == name)
{
//std::cout << __FILE__ << ' ' << __LINE__ << " ContextIfc::getContextIfc. Contexte " << name << " trouvé." << std::endl;
            return *it;
}

//std::cout << __FILE__ << ' ' << __LINE__ << " ContextIfc::getContextIfc : absence de contexte de nom " << name << std::endl;
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "getContextIfc : absence de contexte de nom \"" << name
            << "\" recensé.";
	throw TkUtil::Exception (message);
}	// getContextIfc


/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
