/*----------------------------------------------------------------------------*/
/*
 * \file CommandCreateMesh.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/CommandCreateMesh.h"
#include "Mesh/MeshItf.h"
#include "Mesh/MeshManager.h"
#include "Mesh/MeshModificationItf.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/MeshModificationByPythonFunction.h"
#include "Mesh/MeshModificationBySepa.h"
#include "Mesh/MeshModificationByProjectionOnP0.h"

#include "Smoothing/SurfacicSmoothing.h"
#include "Smoothing/VolumicSmoothing.h"

#include "Utils/Command.h"
#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
#include "Group/Group2D.h"
#include "Group/Group3D.h"
#include "Topo/TopoHelper.h"
#include "Topo/CoEdge.h"
#include "Topo/CoFace.h"
#include "Topo/CoEdgeMeshingProperty.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/ThreadPool.h>
#include <TkUtil/TraceLog.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_THREAD
/*----------------------------------------------------------------------------*/

/**
 * Tâche effectuant le pré-maillage d'arêtes topologiques.
 */
class EdgePreMesherTask : public Mgx3D::Utils::MgxThreadedTask
{
	public :

	EdgePreMesherTask (Mgx3D::Mesh::CommandCreateMesh* command,
					Mgx3D::Topo::CoEdge* edge);
	virtual ~EdgePreMesherTask ( );


	protected :

	virtual void  execute ( );


	private :

	EdgePreMesherTask (const EdgePreMesherTask&);
	EdgePreMesherTask& operator = (const EdgePreMesherTask&);
	Mgx3D::Topo::CoEdge*			_edge;
};	// class EdgePreMesherTask


EdgePreMesherTask::EdgePreMesherTask (
		Mesh::CommandCreateMesh* command, Topo::CoEdge* edge)
	: Mgx3D::Utils::MgxThreadedTask (*command), _edge (edge)
{
	CHECK_NULL_PTR_ERROR (_edge)
}	// EdgePreMesherTask::EdgePreMesherTask

/*----------------------------------------------------------------------------*/
EdgePreMesherTask::EdgePreMesherTask (const EdgePreMesherTask& epmt)
	: Mgx3D::Utils::MgxThreadedTask (epmt), _edge (0)
{
	MGX_FORBIDDEN ("EdgePreMesherTask::EdgePreMesherTask is not allowed.")
}	// EdgePreMesherTask::EdgePreMesherTask

/*----------------------------------------------------------------------------*/
EdgePreMesherTask& EdgePreMesherTask::operator = (const EdgePreMesherTask&)
{
	MGX_FORBIDDEN ("EdgePreMesherTask::operator = is not allowed.")
	return *this;
}	// EdgePreMesherTask::operator =

/*----------------------------------------------------------------------------*/
EdgePreMesherTask::~EdgePreMesherTask ( )
{
}	// EdgePreMesherTask::~EdgePreMesherTask

/*----------------------------------------------------------------------------*/
void EdgePreMesherTask::execute ( )
{
	Mesh::CommandCreateMesh*	cmdCreateMesh	=
					dynamic_cast<Mesh::CommandCreateMesh*>(getCommand ( ));
	try
	{
		CHECK_NULL_PTR_ERROR (_edge)
		CHECK_NULL_PTR_ERROR (cmdCreateMesh)
		setStatus (TkUtil::ThreadPool::TaskIfc::RUNNING);
		// REM : _command->threadedPreMesh (_edge) pourrait tester
		// régulièrement si data->isCanceled ( ) retourne true ou si sa
		// méthode getStatus ( ) retourne CANCELED.
   		cmdCreateMesh->threadedPreMesh (_edge);
		setStatus (TkUtil::ThreadPool::TaskIfc::COMPLETED);
	}
	catch (const TkUtil::Exception& exc)
	{
		if (0 == cmdCreateMesh)
		{
			setStatus (TkUtil::ThreadPool::TaskIfc::IN_ERROR);
			setMessage ("Absence de commande pour effectuer le pré-maillage.");
		}
		else
		{
			switch (cmdCreateMesh->getStatus ( ))
			{
				case Utils::CommandIfc::FAIL	:
					setStatus (TkUtil::ThreadPool::TaskIfc::IN_ERROR);	break;
				case Utils::CommandIfc::CANCELED	:
					setStatus (TkUtil::ThreadPool::TaskIfc::CANCELED);	break;
				default					:
					setStatus (TkUtil::ThreadPool::TaskIfc::IN_ERROR);
			}	// switch (cmdCreateMesh->getStatus ( ))
			setMessage (exc.getFullMessage ( ));
		}
	}
	catch (...)
	{
		setStatus (TkUtil::ThreadPool::TaskIfc::IN_ERROR);
		setMessage ("Erreur non documentée.");
	}

	try
	{
		if (0 != cmdCreateMesh)
			cmdCreateMesh->taskCompleted ( );
	}
	catch (...)
	{
	}
}	// EdgePreMesherTask::execute

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/**
 * Tâche effectuant le pré-maillage de face topologiques.
 */
class FacePreMesherTask : public Mgx3D::Utils::MgxThreadedTask
{
	public :

	FacePreMesherTask (
			Mgx3D::Mesh::CommandCreateMesh* command, Mgx3D::Topo::CoFace* face);
	virtual ~FacePreMesherTask ( );


	protected :

	virtual void  execute ( );


	private :

	FacePreMesherTask (const FacePreMesherTask&);
	FacePreMesherTask& operator = (const FacePreMesherTask&);
	Mgx3D::Topo::CoFace*			_face;
};	// class FacePreMesherTask


FacePreMesherTask::FacePreMesherTask (
						Mesh::CommandCreateMesh* command, Topo::CoFace* face)
	: Mgx3D::Utils::MgxThreadedTask (*command), _face (face)
{
	CHECK_NULL_PTR_ERROR (_face)
}	// FacePreMesherTask::FacePreMesherTask

/*----------------------------------------------------------------------------*/
FacePreMesherTask::FacePreMesherTask (const FacePreMesherTask& fpmt)
	: Mgx3D::Utils::MgxThreadedTask (fpmt), _face (0)
{
	MGX_FORBIDDEN ("FacePreMesherTask::FacePreMesherTask is not allowed.")
}	// FacePreMesherTask::FacePreMesherTask

/*----------------------------------------------------------------------------*/
FacePreMesherTask& FacePreMesherTask::operator = (const FacePreMesherTask&)
{
	MGX_FORBIDDEN ("FacePreMesherTask::operator = is not allowed.")
	return *this;
}	// FacePreMesherTask::operator =

/*----------------------------------------------------------------------------*/
FacePreMesherTask::~FacePreMesherTask ( )
{
}	// FacePreMesherTask::~FacePreMesherTask

/*----------------------------------------------------------------------------*/
void FacePreMesherTask::execute ( )
{
	Mesh::CommandCreateMesh*	cmdCreateMesh	=
					dynamic_cast<Mesh::CommandCreateMesh*>(getCommand ( ));
	try
	{
		CHECK_NULL_PTR_ERROR (_face)
		CHECK_NULL_PTR_ERROR (cmdCreateMesh)
		setStatus (TkUtil::ThreadPool::TaskIfc::RUNNING);
		// REM : _command->threadedPreMesh (_face) pourrait tester
		// régulièrement si data->isCanceled ( ) retourne true ou si sa
		// méthode getStatus ( ) retourne CANCELED.
   		cmdCreateMesh->threadedPreMesh (_face);
		setStatus (TkUtil::ThreadPool::TaskIfc::COMPLETED);
	}
	catch (const TkUtil::Exception& exc)
	{
		if (0 == cmdCreateMesh)
		{
			setStatus (TkUtil::ThreadPool::TaskIfc::IN_ERROR);
			setMessage ("Absence de commande pour effectuer le pré-maillage.");
		}
		else
		{
			switch (cmdCreateMesh->getStatus ( ))
			{
				case Utils::CommandIfc::FAIL		:
					setStatus (TkUtil::ThreadPool::TaskIfc::IN_ERROR);	break;
				case Utils::CommandIfc::CANCELED	:
					setStatus (TkUtil::ThreadPool::TaskIfc::CANCELED);	break;
				default								:
					setStatus (TkUtil::ThreadPool::TaskIfc::IN_ERROR);
			}	// switch (cmdCreateMesh->getStatus ( ))
			setMessage (exc.getFullMessage ( ));
		}
	}
	catch (...)
	{
		setStatus (TkUtil::ThreadPool::TaskIfc::IN_ERROR);
		setMessage ("Erreur non documentée.");
	}

	try
	{
		if (0 != cmdCreateMesh)
			cmdCreateMesh->taskCompleted ( );
	}
	catch (...)
	{
	}
}	// FacePreMesherTask::execute

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * Tâche effectuant le pré-maillage de blocs topologiques.
 */
class BlockPreMesherTask : public Mgx3D::Utils::MgxThreadedTask
{
	public :

	BlockPreMesherTask (
			Mgx3D::Mesh::CommandCreateMesh* command, Mgx3D::Topo::Block* block);
	virtual ~BlockPreMesherTask ( );


	protected :

	virtual void  execute ( );


	private :

	BlockPreMesherTask (const BlockPreMesherTask&);
	BlockPreMesherTask& operator = (const BlockPreMesherTask&);
	Mgx3D::Topo::Block*				_block;
};	// class BlockPreMesherTask


BlockPreMesherTask::BlockPreMesherTask (
		Mesh::CommandCreateMesh* command, Topo::Block* block)
	: Mgx3D::Utils::MgxThreadedTask (*command), _block (block)
{
	CHECK_NULL_PTR_ERROR (_block)
}	// BlockPreMesherTask::BlockPreMesherTask

/*----------------------------------------------------------------------------*/
BlockPreMesherTask::BlockPreMesherTask (const BlockPreMesherTask& bpmt)
	: Mgx3D::Utils::MgxThreadedTask (bpmt), _block (0)
{
	MGX_FORBIDDEN ("BlockPreMesherTask::BlockPreMesherTask is not allowed.")
}	// BlockPreMesherTask::BlockPreMesherTask

/*----------------------------------------------------------------------------*/
BlockPreMesherTask& BlockPreMesherTask::operator = (const BlockPreMesherTask&)
{
	MGX_FORBIDDEN ("BlockPreMesherTask::operator = is not allowed.")
	return *this;
}	// BlockPreMesherTask::operator =

/*----------------------------------------------------------------------------*/
BlockPreMesherTask::~BlockPreMesherTask ( )
{
}	// BlockPreMesherTask::~BlockPreMesherTask

/*----------------------------------------------------------------------------*/
void BlockPreMesherTask::execute ( )
{
	Mesh::CommandCreateMesh*	cmdCreateMesh	=
					dynamic_cast<Mesh::CommandCreateMesh*>(getCommand ( ));
	try
	{
		CHECK_NULL_PTR_ERROR (_block)
		CHECK_NULL_PTR_ERROR (cmdCreateMesh)
		setStatus (TkUtil::ThreadPool::TaskIfc::RUNNING);
		// REM : _command->threadedPreMesh (_block) pourrait tester
		// régulièrement si data->isCanceled ( ) retourne true ou si sa
		// méthode getStatus ( ) retourne CANCELED.
   		cmdCreateMesh->threadedPreMesh (_block);
		setStatus (TkUtil::ThreadPool::TaskIfc::COMPLETED);
	}
	catch (const TkUtil::Exception& exc)
	{
		if (0 == cmdCreateMesh)
		{
			setStatus (TkUtil::ThreadPool::TaskIfc::IN_ERROR);
			setMessage ("Absence de commande pour effectuer le pré-maillage.");
		}
		else
		{
			switch (cmdCreateMesh->getStatus ( ))
			{
				case Utils::CommandIfc::FAIL		:
					setStatus (TkUtil::ThreadPool::TaskIfc::IN_ERROR);	break;
				case Utils::CommandIfc::CANCELED	:
					setStatus (TkUtil::ThreadPool::TaskIfc::CANCELED);	break;
				default								:
					setStatus (TkUtil::ThreadPool::TaskIfc::IN_ERROR);
			}	// switch (cmdCreateMesh->getStatus ( ))
			setMessage (exc.getFullMessage ( ));
		}
	}
	catch (...)
	{
		setStatus (TkUtil::ThreadPool::TaskIfc::IN_ERROR);
		setMessage ("Erreur non documentée.");
	}

	try
	{
		if (0 != cmdCreateMesh)
			cmdCreateMesh->taskCompleted ( );
	}
	catch (...)
	{
	}
}	// BlockPreMesherTask::execute
/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
CommandCreateMesh::CommandCreateMesh(Internal::Context& c, std::string name, size_t tasksNum)
: Internal::MultiTaskedCommand (c, name, tasksNum)
, m_strategy(getContext().getMeshManager().getStrategy())
{
}
/*----------------------------------------------------------------------------*/
CommandCreateMesh::~CommandCreateMesh()
{
    deleteInternalsStats();
    deleteCreatedMeshGroups();
}
/*----------------------------------------------------------------------------*/
MeshManager& CommandCreateMesh::getMeshManager()
{
    return getContext().getLocalMeshManager();
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
postExecute(bool hasError)
{
    // remet de l'odre dans la mémoire (libération des parties internes)
    if (hasError)
        cancelInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::internalUndo()
{
    // NB: en cas de GRANDCHALLENGE, il ne semble pas utile de permettre l'undo

    TkUtil::AutoReferencedMutex autoMutex (getMutex ( ));

    //std::cout <<"CommandCreateMesh::internalUndo() avec InfoCommand:"<<getInfoCommand()<<std::endl;

    // les entités détruites sont dites créées et inversement
    getInfoCommand().permCreatedDeleted();

    // suppression de ce qui a été ajouté au maillage
    deleteCreatedMeshEntities();

    // permute toutes les propriétés internes avec leur sauvegarde
    permInternalsStats();
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::internalRedo()
{
    // suppression (delete) des groupes de mailles (Mesh::Volume Surface ...)
    deleteCreatedMeshGroups();

    // supprime toutes les propriétés internes sauvegardées
    deleteInternalsStats();

    getInfoCommand().clear();

    // refait le maillage en partant de rien
    execute();
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::permInternalsStats()
{
#define PERM_PROPERTY(T,L) \
    for (std::vector <T>::iterator iter = L.begin(); iter != L.end(); ++iter) \
        (*iter).m_property = (*iter).m_entity->setProperty((*iter).m_property);

    PERM_PROPERTY (VertexPropertyInfo,      m_vertex_property_info);
    PERM_PROPERTY (CoEdgePropertyInfo,      m_coedge_property_info);
    PERM_PROPERTY (CoFacePropertyInfo,      m_coface_property_info);
    PERM_PROPERTY (CoFaceMeshingPropertyInfo, m_coface_mesh_property_info);
    PERM_PROPERTY (BlockPropertyInfo,       m_block_property_info);
    PERM_PROPERTY (BlockMeshingPropertyInfo, m_block_mesh_property_info);

    PERM_PROPERTY (VolumePropertyInfo,      m_volume_property_info);
    PERM_PROPERTY (SurfacePropertyInfo,     m_surface_property_info);
    PERM_PROPERTY (CloudPropertyInfo,       m_cloud_property_info);

#undef PERM_PROPERTY
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::mesh(std::vector<Topo::Block* >& blocs)
{
    getContext().getMeshManager().getMesh()->mesh(this, blocs);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::mesh(Topo::Block* & bloc)
{
    getContext().getMeshManager().getMesh()->mesh(this, bloc);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::preMesh(const std::vector<Topo::Block*>& blocks)
{
	if (true == threadingEnabled ( )
			&& true==getContext ( ).allowThreadedBlockPreMeshTasks.getValue( ))
	{
#ifdef _DEBUG_THREAD
		std::cout << "CommandCreateMesh::preMesh. Lancement du pré-maillage des blocs dans des threads. NB_BLOCKS=" << blocks.size ( ) << std::endl;
#endif
		clearTasks ( );
		for (std::vector<Topo::Block*>::const_iterator it = blocks.begin ( );
		     blocks.end ( ) != it; it++)
		{
			if (Command::CANCELED == getStatus ( ))
				break;
			if ((*it)->getMeshLaw ( ) <= Topo::BlockMeshingProperty::transfinite)
			{
				BlockPreMesherTask*	task	= new BlockPreMesherTask(this, *it);
				addTask (task);
			}	// if ((*it)->getMeshLaw ( ) <= Topo::BlockMeshingProperty::transfinite)
		}	// for (std::vector<Topo::Block*>::const_iterator it = blocks.begin ( );

		waitTasksExecution ( );
		evaluateTasksCompletion ( );
		clearTasks ( );
#ifdef _DEBUG_THREAD
		std::cout << "CommandCreateMesh::preMesh. Achèvement avec succès du pré-maillage des blocs dans des threads." << std::endl;
#endif
	}
	else
	{
#ifdef _DEBUG_THREAD
		std::cout << "CommandCreateMesh::preMesh. Exécution séquentielle du pré-maillage des blocs." << std::endl;
#endif
		for (std::vector<Topo::Block*>::const_iterator it = blocks.begin ( );
		     blocks.end ( ) != it; it++)
		{
			if (Command::CANCELED == getStatus ( ))
				break;
			if ((*it)->getMeshLaw ( ) <= Topo::BlockMeshingProperty::transfinite)
				preMesh (*it);
		}
	}
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::preMesh(Topo::Block* bloc)
{
    getContext().getMeshManager().getMesh()->preMesh(this, bloc);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::threadedPreMesh(Topo::Block* bloc)
{
	preMesh (bloc);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::mesh(std::vector<Topo::CoFace* >& faces)
{
    getContext().getMeshManager().getMesh()->mesh(this, faces);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::mesh(Topo::CoFace*  face)
{
    getContext().getMeshManager().getMesh()->mesh(this, face);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::preMesh(const std::vector<Topo::CoFace*>& faces)
{
    double nbTot = (double)faces.size();
    double nbFaits = 0;

    if (true == threadingEnabled ( )
			&& true==getContext ( ).allowThreadedFacePreMeshTasks.getValue( ))
	{
#ifdef _DEBUG_THREAD
		std::cout << "CommandCreateMesh::preMesh. Lancement du pré-maillage des faces dans des threads. NB_FACES=" << faces.size ( ) << std::endl;
#endif
		clearTasks ( );
		for (std::vector<Topo::CoFace*>::const_iterator it = faces.begin ( );
		     faces.end ( ) != it; it++)
		{
			if (Command::CANCELED == getStatus ( ))
				break;
			if ((*it)->getMeshLaw ( ) <= Topo::CoFaceMeshingProperty::transfinite)
			{
				nbFaits += 1.0;
				FacePreMesherTask*	task	= new FacePreMesherTask (this, *it);
				addTask (task);
			}	// if ((*it)->getMeshLaw ( ) <= Topo::CoFaceMeshingProperty::transfinite)
		}	// for (std::vector<Topo::CoFace*>::const_iterator it = faces.begin ( );

		waitTasksExecution ( );
		evaluateTasksCompletion ( );
		clearTasks ( );
	
#ifdef _DEBUG_THREAD
		std::cout << "CommandCreateMesh::preMesh. Achèvement avec succès du pré-maillage des faces dans des threads." << std::endl;
#endif
	}
	else
	{
#ifdef _DEBUG_THREAD
		std::cout << "CommandCreateMesh::preMesh. Exécution séquentielle du pré-maillage des faces." << std::endl;
#endif
		for (std::vector<Topo::CoFace*>::const_iterator it = faces.begin ( );
		     faces.end ( ) != it; it++)
		{
			if (Command::CANCELED == getStatus ( ))
				break;
			if ((*it)->getMeshLaw ( ) <= Topo::CoFaceMeshingProperty::transfinite){
				preMesh (*it);
				nbFaits += 1.0;
				setStepProgression(nbFaits/nbTot);
			}
		}
	}
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::preMesh(Topo::CoFace*  face)
{
    getContext().getMeshManager().getMesh()->preMesh(this, face);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::threadedPreMesh(Topo::CoFace*  face)
{
	preMesh (face);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::mesh(Topo::CoEdge* arete)
{
    getContext().getMeshManager().getMesh()->mesh(this, arete);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::threadedPreMesh (Topo::CoEdge* coedge)
{
	preMesh (coedge);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::preMesh(const std::vector<Topo::CoEdge*>& aretes)
{
    double nbTot = (double)aretes.size();
    double nbFaits = 0;
    uint nbMTThreads = 0; // nombre de taches indépendantes

	if (true == threadingEnabled ( )
			&& true==getContext ( ).allowThreadedEdgePreMeshTasks.getValue( ))
	{
#ifdef _DEBUG_THREAD
		std::cout << "CommandCreateMesh::preMesh. Lancement du pré-maillage des arêtes dans des threads. NB_EDGE=" << aretes.size ( ) << std::endl;
#endif
		clearTasks ( );
		for (std::vector<Topo::CoEdge*>::const_iterator it = aretes.begin ( );
				aretes.end ( ) != it; it++)
		{
			if (Command::CANCELED == getStatus ( ))
				break;
			Geom::GeomEntity* ge = (*it)->getGeomAssociation();
			bool treadSafe = true;
//			// [EB] pas de multi-threading pour les constructions des points des arêtes
//			//      qui font appel à la construction d'une courbe temporaire via OCC.
//			//      Création d'entité qui ne peut être faite dans des threads concurrents
//			if (ge && ge->getType() == Utils::Entity::GeomSurface)
//				treadSafe = false;
			// [EB] pas de multi-threading pour les constructions des points des arêtes
			// pour les arêtes interpolées
			if ((*it)->getMeshingProperty()->getMeshLaw() == Topo::CoEdgeMeshingProperty::interpolate
					|| (*it)->getMeshingProperty()->getMeshLaw() == Topo::CoEdgeMeshingProperty::globalinterpolate)
				treadSafe = false;
			if (treadSafe){
				nbFaits += 1.0;
				EdgePreMesherTask*	task	= new EdgePreMesherTask (this, *it);
				// une seule tache à fois qui utilise les projection de segment sur surface OCC
				if (ge && ge->getType() == Utils::Entity::GeomSurface)
					task->setConcurrencyFlag(1);
				else
					nbMTThreads+=1;
				addTask (task);
#ifdef _DEBUG_THREAD
				std::cout << " addTask pour "<<(*it)->getName()<<std::endl;
#endif
			}
		}	// for (std::vector<Topo::CoEdge*>::const_iterator it = aretes.begin ( );

		waitTasksExecution ( );
		evaluateTasksCompletion ( );
		clearTasks ( );

#ifdef _DEBUG_THREAD
		std::cout << "CommandCreateMesh::preMesh. Achèvement avec succès du pré-maillage des arêtes dans des threads." << std::endl;
#endif
	} // if (true == threadingEnabled ( ))


#ifdef _DEBUG_THREAD
	std::cout << "CommandCreateMesh::preMesh. Exécution séquentielle du pré-maillage des arêtes (restantes)." << std::endl;
#endif
	for (std::vector<Topo::CoEdge*>::const_iterator it = aretes.begin ( );
			aretes.end ( ) != it; it++)
	{
		if (Command::CANCELED == getStatus ( ))
			break;
		if (!(*it)->isPreMeshed() && !(*it)->isMeshed()){
#ifdef _DEBUG_THREAD
			std::cout << " preMesh séquetiel de "<<(*it)->getName()<<std::endl;
#endif
			preMesh (*it);
			nbFaits += 1.0;
			setStepProgression(nbFaits/nbTot);
	    	notifyObserversForModifications();
		}
	}
#ifdef _DEBUG_THREAD
			std::cout << "  nombre de taches réellement indépendantes "<<nbMTThreads<<" sur "<<aretes.size()<<std::endl;
#endif
}

/*----------------------------------------------------------------------------*/
void CommandCreateMesh::preMesh(Topo::CoEdge* arete)
{
    getContext().getMeshManager().getMesh()->preMesh(this, arete);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::mesh(Topo::Vertex* sommet)
{
    getContext().getMeshManager().getMesh()->mesh(this, sommet);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addCreatedNode(gmds::TCellID id)
{
    if (m_strategy == MeshManager::MODIFIABLE)
        m_created_nodes.push_back(id);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addCreatedEdge(gmds::TCellID id)
{
    if (m_strategy == MeshManager::MODIFIABLE)
        m_created_edges.push_back(id);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addCreatedFace(gmds::TCellID id)
{
    if (m_strategy == MeshManager::MODIFIABLE)
        m_created_faces.push_back(id);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addCreatedRegion(gmds::TCellID id)
{
    if (m_strategy == MeshManager::MODIFIABLE)
        m_created_regions.push_back(id);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addNewCloud(const std::string& name)
{
    Cloud* cl = new Cloud(getContext(),
            getContext().newProperty(Utils::Entity::MeshCloud, name),
            getContext().newDisplayProperties(Utils::Entity::MeshCloud));
	CHECK_NULL_PTR_ERROR (cl)
	getContext ( ).newGraphicalRepresentation (*cl);

    getInfoCommand().addMeshInfoEntity(cl, Internal::InfoCommand::CREATED);
    getMeshManager().add(cl);
    m_created_clouds.add(cl);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addNewLine(const std::string& name)
{
    Line* ln = new Line(getContext(),
            getContext().newProperty(Utils::Entity::MeshLine, name),
            getContext().newDisplayProperties(Utils::Entity::MeshLine));
	CHECK_NULL_PTR_ERROR (ln)
	getContext ( ).newGraphicalRepresentation (*ln);

    getInfoCommand().addMeshInfoEntity(ln, Internal::InfoCommand::CREATED);
    getMeshManager().add(ln);
    m_created_lines.add(ln);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addNewSurface(const std::string& name)
{
    Surface* sf = new Surface(getContext(),
            getContext().newProperty(Utils::Entity::MeshSurface, name),
            getContext().newDisplayProperties(Utils::Entity::MeshSurface));
	CHECK_NULL_PTR_ERROR (sf)
	getContext ( ).newGraphicalRepresentation (*sf);

    getInfoCommand().addMeshInfoEntity(sf, Internal::InfoCommand::CREATED);
    getMeshManager().add(sf);
    m_created_surfaces.add(sf);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addNewVolume(const std::string& name)
{
    Volume* vo = new Volume(getContext(),
            getContext().newProperty(Utils::Entity::MeshVolume, name),
            getContext().newDisplayProperties(Utils::Entity::MeshVolume));
	CHECK_NULL_PTR_ERROR (vo)
	//getContext ( ).newGraphicalRepresentation (*vo); // retiré car on souhaite la présence des blocs pour utiliser useGMDSOptimization

    getInfoCommand().addMeshInfoEntity(vo, Internal::InfoCommand::CREATED);
    getMeshManager().add(vo);
    m_created_volumes.add(vo);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addModifiedCloud(const std::string& name)
{
    Cloud* cl = getMeshManager().getCloud(name);
    // on ressuscite le nuage si nécessaire
    if (cl->isDestroyed()){
        getInfoCommand().addMeshInfoEntity(cl, Internal::InfoCommand::CREATED);
        cl->setDestroyed(false);
        cl->clearCoEdges();
    }
    else
        getInfoCommand().addMeshInfoEntity(cl, Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addModifiedLine(const std::string& name)
{
    Line* ln = getMeshManager().getLine(name);
    // on ressuscite la ligne si nécessaire
    if (ln->isDestroyed()){
        getInfoCommand().addMeshInfoEntity(ln, Internal::InfoCommand::CREATED);
        ln->setDestroyed(false);
        ln->clearCoEdges();
    }
    else
        getInfoCommand().addMeshInfoEntity(ln, Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addModifiedSurface(const std::string& name)
{
    Surface* sf = getMeshManager().getSurface(name);
    // on ressuscite la surface si nécessaire
    if (sf->isDestroyed()){
        getInfoCommand().addMeshInfoEntity(sf, Internal::InfoCommand::CREATED);
        sf->setDestroyed(false);
        sf->clearCoFaces();
    }
    else
        getInfoCommand().addMeshInfoEntity(sf, Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addModifiedVolume(const std::string& name)
{
    //std::cout<<"CommandCreateMesh::addModifiedVolume("<<name<<", "<<bl->getName()<<")"<<std::endl;
    Volume* vo = getMeshManager().getVolume(name);
    // on ressuscite le volume si nécessaire
    if (vo->isDestroyed()){
        //std::cout<<"  on ressuscite le volume..."<<std::endl;
        getInfoCommand().addMeshInfoEntity(vo, Internal::InfoCommand::CREATED);
        vo->setDestroyed(false);
        vo->clearBlocks();
    }
    else {
        vo->saveMeshVolumeTopoProperty(&getInfoCommand());
    }
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
addVertexInfoMeshingData(Topo::Vertex* te, Topo::VertexMeshingData* tp)
{
    VertexPropertyInfo ti = {te,tp};
    m_vertex_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
addCoEdgeInfoMeshingData(Topo::CoEdge* te, Topo::CoEdgeMeshingData* tp)
{
    CoEdgePropertyInfo ti = {te,tp};
    m_coedge_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
addCoFaceInfoMeshingData(Topo::CoFace* te, Topo::CoFaceMeshingData* tp)
{
    CoFacePropertyInfo ti = {te,tp};
    m_coface_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::addCoFaceInfoMeshingProperty(Topo::CoFace* te, Topo::CoFaceMeshingProperty* tp)
{
    CoFaceMeshingPropertyInfo ti = {te,tp};
    m_coface_mesh_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
addBlockInfoMeshingData(Topo::Block* te, Topo::BlockMeshingData* tp)
{
    BlockPropertyInfo ti = {te,tp};
    m_block_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
addBlockInfoMeshingProperty(Topo::Block* te, Topo::BlockMeshingProperty* tp)
{
	BlockMeshingPropertyInfo ti = {te,tp};
	m_block_mesh_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
addVolumeInfoTopoProperty(Mesh::Volume* te, Topo::MeshVolumeTopoProperty* tp)
{
    VolumePropertyInfo ti = {te,tp};
    m_volume_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
addSurfaceInfoTopoProperty(Mesh::Surface* te, Topo::MeshSurfaceTopoProperty* tp)
{
    SurfacePropertyInfo ti = {te,tp};
    m_surface_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
addLineInfoTopoProperty(Mesh::Line* te, Topo::MeshLineTopoProperty* tp)
{
    LinePropertyInfo ti = {te,tp};
    m_line_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
addCloudInfoTopoProperty(Mesh::Cloud* te, Topo::MeshCloudTopoProperty* tp)
{
    CloudPropertyInfo ti = {te,tp};
    m_cloud_property_info.push_back(ti);
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
saveInternalsStats()
{
    Internal::InfoCommand& icmd = getInfoCommand();

    std::map<Topo::TopoEntity*, Internal::InfoCommand::type>&  topo_entities_info = icmd.getTopoInfoEntity();
    for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator iter_tei = topo_entities_info.begin();
            iter_tei != topo_entities_info.end(); ++iter_tei){
        Topo::TopoEntity* te = iter_tei->first;
        te->saveInternals(this);
    }

    std::vector <Internal::InfoCommand::MeshEntityInfo>&  mesh_entities_info = icmd.getMeshInfoEntities();
    for (std::vector <Internal::InfoCommand::MeshEntityInfo>::iterator iter_mei = mesh_entities_info.begin();
            iter_mei != mesh_entities_info.end(); ++iter_mei){
        Mesh::MeshEntity* me = (*iter_mei).m_mesh_entity;
        me->saveInternals(this);
    }
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
cancelInternalsStats()
{
    saveInternalsStats();
    permInternalsStats();
    deleteInternalsStats();
    deleteCreatedMeshEntities();
    deleteCreatedMeshGroups();

    getInfoCommand().clear();
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
deleteInternalsStats()
{
#define DELETE_PROPERTY(T,L) \
    for (std::vector <T>::iterator iter = L.begin(); iter != L.end(); ++iter) \
        delete (*iter).m_property; \
    L.clear();

    DELETE_PROPERTY (VertexPropertyInfo,      m_vertex_property_info);
    DELETE_PROPERTY (CoEdgePropertyInfo,      m_coedge_property_info);
    DELETE_PROPERTY (CoFacePropertyInfo,      m_coface_property_info);
    DELETE_PROPERTY (CoFaceMeshingPropertyInfo, m_coface_mesh_property_info);
    DELETE_PROPERTY (BlockPropertyInfo,       m_block_property_info);
    DELETE_PROPERTY (BlockMeshingPropertyInfo, m_block_mesh_property_info);

    DELETE_PROPERTY (VolumePropertyInfo,      m_volume_property_info);
    DELETE_PROPERTY (SurfacePropertyInfo,     m_surface_property_info);
    DELETE_PROPERTY (CloudPropertyInfo,       m_cloud_property_info);

#undef DELETE_PROPERTY
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
deleteCreatedMeshEntities()
{
    // suppression des entités créées suivant la stategie, ou suppression du maillage
    if (m_strategy == MeshManager::MODIFIABLE){

        // si tous les noeuds sont à détruire, autant vider le maillage
        if (m_created_nodes.size() == getMeshManager().getMesh()->getNbNodes()){
            getMeshManager().getMesh()->deleteMesh();
        }
        else {
            // destruction de ce qui a été créé
            getMeshManager().getMesh()->undoCreatedMesh(this);
        }

        m_created_nodes.clear();
        m_created_edges.clear();
        m_created_faces.clear();
        m_created_regions.clear();

    } else if (m_strategy == MeshManager::GRANDCHALLENGE){

        getMeshManager().getMesh()->deleteMesh();

    } else {
        MGX_NOT_YET_IMPLEMENTED("CommandCreateMesh::deleteCreatedMeshEntities(), pour une autre stratégie");
    }

}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
deleteCreatedMeshGroups()
{
    // suppression des entités créées suivant la stategie
    if (m_strategy == MeshManager::MODIFIABLE){

        // désenregistrement du MeshManager des groupes créés
        for (std::vector<Cloud*>::const_iterator iter = m_created_clouds.get().begin();
                iter != m_created_clouds.get().end(); ++iter)
            getMeshManager().remove(*iter);

        for (std::vector<Line*>::const_iterator iter = m_created_lines.get().begin();
                iter != m_created_lines.get().end(); ++iter)
            getMeshManager().remove(*iter);

        for (std::vector<Surface*>::const_iterator iter = m_created_surfaces.get().begin();
                iter != m_created_surfaces.get().end(); ++iter)
            getMeshManager().remove(*iter);

        for (std::vector<Volume*>::const_iterator iter = m_created_volumes.get().begin();
                iter != m_created_volumes.get().end(); ++iter)
            getMeshManager().remove(*iter);

        m_created_clouds.deleteAndClear();
        m_created_lines.deleteAndClear();
        m_created_surfaces.deleteAndClear();
        m_created_volumes.deleteAndClear();

    }
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
meshAndModify(std::list<Topo::CoFace*>& list_cofaces)
{
#ifdef _DEBUG2
	std::cout<<"CommandCreateMesh::meshAndModify avec "<<list_cofaces.size()<<" cofaces"<<std::endl;
#endif

	// recherche des groupes 2D, parmis les cofaces en entrée, qui ont une modification
	std::list<Group::Group2D*> list_grp;

	// filtre pour identifier les cofaces à mailler (marque à 1)
	std::map<Topo::CoFace*, uint> filtre_cofaces;

	// filtre sur les noeuds gmds pour les perturbations:
	// 0 pour les nouveaux,
	// 1 pour les anciens (ceux créés avant cette commande)
	std::map<gmds::TCellID, uint> filtre_nodes_pert;

	for (std::list<Topo::CoFace*>::iterator iter1 = list_cofaces.begin();
			iter1 != list_cofaces.end(); ++iter1){
		Topo::CoFace* coface = *iter1;
		filtre_cofaces[coface] = 1;

		if (coface->getGeomAssociation() && coface->getGeomAssociation()->getDim() == 2){
			Geom::Surface* surface = dynamic_cast<Geom::Surface*>(coface->getGeomAssociation());
			CHECK_NULL_PTR_ERROR(surface);
			const std::vector<Group::Group2D*>& grps = surface->getGroups();

			for (std::vector<Group::Group2D*>::const_iterator iter2 = grps.begin();
					iter2 != grps.end(); ++iter2){
				Group::Group2D* grp = *iter2;
				if (grp->getNbMeshModif() != 0)
					list_grp.push_back(grp);
			} // end for iter2
		} // end if getGeomAssociation

		// les groupes depuis les faces
		Utils::Container<Group::Group2D>& grp2d = coface->getGroupsContainer();
		for (uint i=0; i<grp2d.getNb(); i++){
			Group::Group2D* grp = grp2d.get(i);
			if (grp->getNbMeshModif() != 0)
				list_grp.push_back(grp);
		}

	} // end for iter1

	list_grp.sort();
	list_grp.unique();

#ifdef _DEBUG2
	std::cout<<" => list_grp avec "<<list_grp.size()<<" groupes 2D"<<std::endl;
#endif

	gmds::IGMesh& gmds_mesh = getMeshManager().getMesh()->getGMDSMesh();
	// les noeuds déjà dans gmds
	std::vector<gmds::Node> nodes;

	// on marque les noeuds déjà présents
	//std::cout<<"Nb Nodes : "<<gmds_mesh.getNbNodes()<<std::endl;
	gmds::IGMesh::node_iterator iter1 = gmds_mesh.nodes_begin();

	for (;!iter1.isDone();iter1.next()){
		//std::cout<<"Node "<<iter1.value().getID()<<" fixé "<<std::endl;
		filtre_nodes_pert[iter1.value().getID()] = 1;
	}

	// application de la modif pour chacun des groupes 2D
	for (std::list<Group::Group2D*>::iterator iter1 = list_grp.begin();
			iter1 != list_grp.end(); ++iter1){
		Group::Group2D* grp = *iter1;

		// liste des cofaces associé au groupe
		std::list<Topo::CoFace*> cofaces_grp;

		std::vector<Geom::Surface*> surfaces = grp->getSurfaces();
		for (std::vector<Geom::Surface*>::iterator iter2 = surfaces.begin();
				iter2 != surfaces.end(); ++iter2){
			std::vector<Topo::CoFace*> cofaces;
			(*iter2)->get(cofaces);
			cofaces_grp.insert(cofaces_grp.end(), cofaces.begin(), cofaces.end());
		} // end for iter2

		std::vector<Topo::CoFace*>& cofaces = grp->getCoFaces();
		cofaces_grp.insert(cofaces_grp.end(), cofaces.begin(), cofaces.end());

		cofaces_grp.sort(Utils::Entity::compareEntity);
		cofaces_grp.unique();

		std::vector<Topo::CoFace*> cofaces_to_be_meshed;

		for (std::list<Topo::CoFace*>::iterator iter2 = cofaces_grp.begin();
				iter2 != cofaces_grp.end(); ++iter2){
			Topo::CoFace* coface = *iter2;

			if (!coface->isMeshed() && filtre_cofaces[coface] == 1){
#ifdef _DEBUG2
				std::cout<<"  on doit mailler "<<coface->getName()<<std::endl;
#endif
				std::vector<Topo::Vertex*> vertices;
				coface->getVertices(vertices);
				for (uint i=0; i<vertices.size(); i++){
					if (!vertices[i]->isMeshed())
						mesh (vertices[i]);
				}

				std::vector<Topo::CoEdge*> aretes;
				coface->getCoEdges(aretes);
				for (uint i=0; i<aretes.size(); i++){
					if (!aretes[i]->isPreMeshed() && !aretes[i]->isMeshed())
						preMesh (aretes[i]);
					if (!aretes[i]->isMeshed())
						mesh (aretes[i]);
				}
				cofaces_to_be_meshed.push_back(coface);
			}
		} // end for iter2

		// utilisation du multithreading si possible
		preMesh(cofaces_to_be_meshed);

		for (uint i=0; i<cofaces_to_be_meshed.size(); i++)
			mesh(cofaces_to_be_meshed[i]);

#ifdef _DEBUG2
		std::cout<<grp->getName()<<" avec "<<cofaces_grp.size()<<" cofaces"<<std::endl;
#endif

		for (uint i=0; i<grp->getNbMeshModif(); i++){
			Mesh::MeshModificationItf* modif = grp->getMeshModif(i);

			MeshModificationBySepa* sepa           = dynamic_cast<MeshModificationBySepa*>(modif);
			MeshModificationByProjectionOnP0* proj = dynamic_cast<MeshModificationByProjectionOnP0*>(modif);
			MeshModificationByPythonFunction* pert = dynamic_cast<MeshModificationByPythonFunction*>(modif);
			SurfacicSmoothing* lissageSurf         = dynamic_cast<SurfacicSmoothing*>(modif);

			if(sepa) {
				Mesh::Surface* msurf = getMeshManager().getSurface(grp->getName(), true);
				std::vector<gmds::Node > gmdsNodes;
				msurf->getGMDSNodes(gmdsNodes);
				std::vector<gmds::Face > gmdsFaces;
				msurf->getGMDSFaces(gmdsFaces);

				// applique la perturbation uniquement aux noeuds nouveaux (non marqués à 1)
				sepa->applyModification(gmdsNodes,gmdsFaces,getContext());

				// this will be used for displaying deltas
				std::vector<std::string> names;
				names.push_back("SepaDelta");
				msurf->setNodesValuesNames(names);
				std::map<int,double> value;//(gmdsNodes.size());

				std::map<gmds::Node,double> deltas = sepa->getDeltas();

				for(unsigned int iNode=0; iNode<gmdsNodes.size(); iNode++) {
					value[gmdsNodes[iNode].getID()] = deltas[gmdsNodes[iNode]];
				}

				msurf->setNodesValue("SepaDelta",value);

			}
			else if (pert){

				// récupération des noeuds associés à ce groupe
				Mesh::Surface* msurf = getMeshManager().getSurface(grp->getName(), true);
				std::vector<gmds::Node > gmdsNodes;
				msurf->getGMDSNodes(gmdsNodes);

				// applique la perturbation uniquement aux noeuds nouveaux (non marqués à 1)
				pert->applyModification(gmdsNodes, filtre_nodes_pert, 1);

			}
			else if (proj){

				// récupération des noeuds associés à ce groupe
				Mesh::Surface* msurf = getMeshManager().getSurface(grp->getName(), true);
				std::vector<gmds::Node > gmdsNodes;
				msurf->getGMDSNodes(gmdsNodes);

				// applique la perturbation aux noeuds
				proj->applyModification(gmdsNodes);

			}
			else if (lissageSurf){


				// le lissage dépend de la surface sur laquelle se fait la projection
				for (std::vector<Geom::Surface*>::iterator iter2 = surfaces.begin();
						iter2 != surfaces.end(); ++iter2){
					Geom::Surface* surface = *iter2;
					std::vector<Topo::CoFace*> cofaces;
					surface->get(cofaces);

					// marque par coface pour savoir si elle est dans le même sens (normale) que les autres
					std::map<Topo::CoFace*, bool> isCoFaceInverted;
					Topo::TopoHelper::computeInvertedDir(cofaces, isCoFaceInverted);
					std::map<gmds::TCellID, bool> isPolyInverted;

					// ensemble des cofaces d'une surface du groupe qui sont maillées à la fin
					std::vector<Topo::CoFace*> meshed_cofaces;

					for (std::vector<Topo::CoFace*>::iterator iter3 = cofaces.begin();
							iter3 != cofaces.end(); ++iter3){
						Topo::CoFace* coface = *iter3;
						if (coface->isMeshed())
							meshed_cofaces.push_back(coface);
					} // end for iter3

					// la liste des polygones dans ces cofaces
					std::vector<gmds::Face> polygones;
					// la liste des noeuds dans ces cofaces
					std::vector<gmds::Node> nodes;

					// filtre sur les noeuds gmds pour les lissages:
					// 1 pour les nouveaux,
					// 2 pour les noeuds au bord d'une surface de maillage
					std::map<gmds::TCellID, uint> filtre_nodes_lisse;

					for (std::vector<Topo::CoFace*>::iterator iter3 = meshed_cofaces.begin();
							iter3 != meshed_cofaces.end(); ++iter3){
						Topo::CoFace* coface = *iter3;
						bool isInverted = isCoFaceInverted[coface];
						std::vector<gmds::TCellID>& l_nds = coface->nodes();
						std::vector<gmds::TCellID>& l_poly = coface->faces();

                        for(unsigned int i_poly=0;i_poly<l_poly.size();i_poly++){
                            polygones.push_back(gmds_mesh.get<gmds::Face>(l_poly[i_poly]));
                            isPolyInverted[l_poly[i_poly]] = isInverted;
                        }

						for (std::vector<gmds::TCellID>::iterator iter4 = l_nds.begin();
								iter4 != l_nds.end(); ++iter4)
							if (filtre_nodes_lisse[*iter4] == 0){
								filtre_nodes_lisse[*iter4] = 1;
								nodes.push_back(gmds_mesh.get<gmds::Node>(*iter4));
						}
					}


					std::vector<Topo::CoEdge*> border_meshed_coedges;
					border_meshed_coedges = Topo::TopoHelper::getBorder(meshed_cofaces);

					// on marque les noeuds des arêtes au bord à 2 pour ne pas les déplacer
					for (std::vector<Topo::CoEdge*>::iterator iter3 = border_meshed_coedges.begin();
							iter3 != border_meshed_coedges.end(); ++iter3){
						std::vector<gmds::TCellID>& nodes = (*iter3)->nodes();
						for (std::vector<gmds::TCellID>::iterator iter4 = nodes.begin();
								iter4 != nodes.end(); ++iter4)
							filtre_nodes_lisse[*iter4] = 2;
					} // end for iter3
#ifdef _DEBUG2
					{
						// stats sur le nombre de noeuds à 1 et ceux à 2
						uint mq1 = 0;
						uint mq2 = 0;
						for (std::map<gmds::TCellID, uint>::iterator iter=filtre_nodes_lisse.begin();
								iter!=filtre_nodes_lisse.end(); ++iter)
							if (iter->second == 1)
								mq1++;
							else if (iter->second == 2)
								mq2++;
						std::cout<<"CommandCreateMesh::meshAndModify, filtre_nodes_lisse avec "<<mq1<<" noeuds à bouger et "<<mq2<<" figés"<<std::endl;

					}
#endif
					// applique le lissage uniquement aux noeuds internes à la surface (non marqués à 2)
					lissageSurf->applyModification(nodes, polygones, filtre_nodes_lisse, isPolyInverted, 2, surface);

				} // end for iter2


			}
			else
				throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (meshAndModify), Type de modification de maillage inconnu", TkUtil::Charset::UTF_8));

		} // end for i<grp->getNbMeshModif()
	} // end for iter1
}
/*----------------------------------------------------------------------------*/
void CommandCreateMesh::
modify(std::vector<Topo::Block*>& list_blocks)
{
	// recherche des groupes 3D, parmis les blocs en entrée, qui ont une modification
	std::list<Group::Group3D*> list_grp;

	for (std::vector<Topo::Block*>::iterator iter1 = list_blocks.begin();
			iter1 != list_blocks.end(); ++iter1){
		Topo::Block* block = *iter1;

		if (block->getGeomAssociation() && block->getGeomAssociation()->getDim() == 3){
			Geom::Volume* volume = dynamic_cast<Geom::Volume*>(block->getGeomAssociation());
			CHECK_NULL_PTR_ERROR(volume);
			const std::vector<Group::Group3D*>& grps = volume->getGroups();

			for (std::vector<Group::Group3D*>::const_iterator iter2 = grps.begin();
					iter2 != grps.end(); ++iter2){
				Group::Group3D* grp = *iter2;
				if (grp->getNbMeshModif() != 0)
					list_grp.push_back(grp);
			} // end for iter2
		} // end if getGeomAssociation
		else {
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message <<"Le bloc "<<block->getName()<<" n'est associé à aucun volume";
			getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
		}

		// les groupes depuis les blocs
		Utils::Container<Group::Group3D>& grp3d = block->getGroupsContainer();
		for (uint i=0; i<grp3d.getNb(); i++){
			Group::Group3D* grp = grp3d.get(i);
			if (grp->getNbMeshModif() != 0)
				list_grp.push_back(grp);
		}

	} // end for iter1

	list_grp.sort();
	list_grp.unique();

#ifdef _DEBUG2
	std::cout<<" => list_grp avec "<<list_grp.size()<<" groupes 3D"<<std::endl;
#endif

	gmds::IGMesh& gmds_mesh = getMeshManager().getMesh()->getGMDSMesh();

	// application de la modif pour chacun des groupes 3D
	for (std::list<Group::Group3D*>::iterator iter1 = list_grp.begin();
			iter1 != list_grp.end(); ++iter1){
		Group::Group3D* grp = *iter1;

		// liste des blocs associés au groupe
		std::list<Topo::Block*> blocks_grp;

		std::vector<Geom::Volume*> volumes = grp->getVolumes();
		for (std::vector<Geom::Volume*>::iterator iter2 = volumes.begin();
				iter2 != volumes.end(); ++iter2){
			std::vector<Topo::Block*> blocks;
			(*iter2)->get(blocks);
			blocks_grp.insert(blocks_grp.end(), blocks.begin(), blocks.end());
		} // end for iter2

		blocks_grp.sort(Utils::Entity::compareEntity);
		blocks_grp.unique();

		for (std::list<Topo::Block*>::iterator iter2 = blocks_grp.begin();
				iter2 != blocks_grp.end(); ++iter2){
			Topo::Block* block = *iter2;

			if (!block->isMeshed()){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
				messErr <<"Le bloc "<<block->getName()<<" n'est pas maillé ce qui pose problème pour permettre la perturbation sur le groupe.\n";
				messErr <<"Il est nécessaire de mailler tous les blocs perturbés en une fois";
				throw TkUtil::Exception(messErr);
			}
		} // end for iter2

#ifdef _DEBUG2
		std::cout<<grp->getName()<<" avec "<<blocks_grp.size()<<" blocs"<<std::endl;
#endif

		for (uint i=0; i<grp->getNbMeshModif(); i++){
			Mesh::MeshModificationItf* modif = grp->getMeshModif(i);

			VolumicSmoothing* lissageVol = dynamic_cast<VolumicSmoothing*>(modif);
			MeshModificationByPythonFunction* pert = dynamic_cast<MeshModificationByPythonFunction*>(modif);

			if (lissageVol){

				// le lissage dépend du volume sur lequel se fait la projection
				for (std::vector<Geom::Volume*>::iterator iter2 = volumes.begin();
						iter2 != volumes.end(); ++iter2){
					Geom::Volume* volume = *iter2;
					std::vector<Topo::Block*> blocks;
					volume->get(blocks);

					// ensemble des blocs d'un volume du groupe qui sont maillés à la fin
					std::vector<Topo::Block*> meshed_blocks;

					for (std::vector<Topo::Block*>::iterator iter3 = blocks.begin();
							iter3 != blocks.end(); ++iter3){
						Topo::Block* block = *iter3;
						if (block->isMeshed())
							meshed_blocks.push_back(block);
					} // end for iter3

					// la liste des polygedres dans ces blocs
					std::vector<gmds::Region> polygedres;
					// la liste des noeuds dans ces blocs
					std::vector<gmds::Node> nodes;

					// filtre sur les noeuds gmds pour les lissages:
					// 1 pour les nouveaux,
					// 2 pour les noeuds au bord d'un volume de maillage
					std::map<gmds::TCellID, uint> filtre_nodes_lisse;

					for (std::vector<Topo::Block*>::iterator iter3 = meshed_blocks.begin();
							iter3 != meshed_blocks.end(); ++iter3){
						Topo::Block* block = *iter3;
						std::vector<gmds::TCellID>& l_nds = block->nodes();
						std::vector<gmds::TCellID>& l_poly = block->regions();

						for(unsigned int i_poly=0;i_poly<l_poly.size();i_poly++){
						    polygedres.push_back(gmds_mesh.get<gmds::Region>(l_poly[i_poly]));
						}


						for (std::vector<gmds::TCellID>::iterator iter4 = l_nds.begin();
								iter4 != l_nds.end(); ++iter4)
							if (filtre_nodes_lisse[*iter4] == 0){
								filtre_nodes_lisse[*iter4] = 1;
								nodes.push_back(gmds_mesh.get<gmds::Node>(*iter4));
						}
					}


					std::vector<Topo::CoFace*> border_meshed_cofaces;
					border_meshed_cofaces = Topo::TopoHelper::getBorder(meshed_blocks);

					// on marque les noeuds des arêtes au bord à 2 pour ne pas les déplacer
					for (std::vector<Topo::CoFace*>::iterator iter3 = border_meshed_cofaces.begin();
							iter3 != border_meshed_cofaces.end(); ++iter3){
						std::vector<gmds::TCellID>& nodes = (*iter3)->nodes();
						for (std::vector<gmds::TCellID>::iterator iter4 = nodes.begin();
								iter4 != nodes.end(); ++iter4)
							filtre_nodes_lisse[*iter4] = 2;
					} // end for iter3

					// applique le lissage uniquement aux noeuds internes au volume (non marqués à 2)
					lissageVol->applyModification(nodes, polygedres, filtre_nodes_lisse, 2, volume);

				} // end for iter2

			}
			else if (pert){

				// filtre sur les noeuds gmds pour les perturbations:
				// 0 pour les nouveaux,
				// 1 pour les anciens (ceux créés avant cette commande)
				std::map<gmds::TCellID, uint> filtre_nodes_pert;
				// actuellement on bouge tous les noeuds ...

				// récupération des noeuds associés à ce groupe
				Mesh::Volume* mvol = getMeshManager().getVolume(grp->getName(), true);
				std::vector<gmds::Node > gmdsNodes;
				mvol->getGMDSNodes(gmdsNodes);

				// applique la perturbation uniquement aux noeuds nouveaux (non marqués à 1)
				pert->applyModification(gmdsNodes, filtre_nodes_pert, 1);

			}
			else
				throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (modify), Type de modification de maillage inconnu", TkUtil::Charset::UTF_8));

		} // end for i<grp->getNbMeshModif()
	} // end for iter1
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
