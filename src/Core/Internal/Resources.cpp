//
// Created by calderans on 29/07/2026.
//

#include "Internal/Resources.h"
#include "Utils/Common.h"

using namespace TkUtil;


namespace Mgx3D
{

namespace Internal
{

Resources*	Resources::_instance	= 0;



Resources::Resources ( )
:	_allowThreadedCommandTasks ("allowThreadedCommandTasks", true, UTF8String ("true si une commande est autorisée à être décomposée en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.")),
	_allowThreadedEdgePreMeshTasks ("allowThreadedEdgePreMeshTasks", true, UTF8String ("true si le prémaillage des arêtes peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.")),
	_allowThreadedFacePreMeshTasks ("allowThreadedFacePreMeshTasks", true, UTF8String ("true si le prémaillage des faces peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.")),
	_allowThreadedBlockPreMeshTasks ("allowThreadedBlockPreMeshTasks", true, UTF8String ("true si le prémaillage des blocs peut être décomposé en plusieurs tâches exécutées parallèlement dans plusieurs threads, false si l'exécution doit être séquentielle.")),
	_memorizeEdgePreMesh ("memorizeEdgePreMesh", true, UTF8String ("true si le programme doit mémoriser le prémaillage des arêtes, false dans le cas contraire.")),
	_scripts ( )
{
	Resources::_instance	= this;
}	// GUIResources::GUIResources







Resources::~Resources ( )
{
	_instance	= 0;
}	// GUIResources::~GUIResources


	Resources& Resources::instance ( )
{
	if (0 == _instance)
		_instance	= new Resources ( );

	return *_instance;
}	// GUIResources::instance


}	// namespace Internal

}	// namespace Mgx3D
