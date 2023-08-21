/*----------------------------------------------------------------------------*/
/*
 * \file CommandTransformMesh.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/2/2016
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/CommandTransformMesh.h"
#include "Mesh/MeshManager.h"
#include "Mesh/MeshItf.h"
#include "Mesh/Volume.h"
#include "Mesh/Surface.h"
#include "Mesh/Cloud.h"

#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
// OCC
#include <gp_Trsf.hxx>
#include <gp_GTrsf.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandTransformMesh::
CommandTransformMesh(Internal::Context& c, std::string name)
:Internal::CommandInternal(c, name)
{
}
/*----------------------------------------------------------------------------*/
CommandTransformMesh::
~CommandTransformMesh()
{
}
/*----------------------------------------------------------------------------*/
void CommandTransformMesh::
transform(gp_Trsf* transf)
{
	gmds::IGMesh& mesh = getContext().getLocalMeshManager().getMesh()->getGMDSMesh();

	std::vector<gmds::Node> nodes;
	mesh.getAll(nodes);

	for (uint i=0; i<nodes.size(); i++){
		gmds::math::Point pt = nodes[i].getPoint();
		double x = pt.X();
		double y = pt.Y();
		double z = pt.Z();

		transf->Transforms(x, y, z);

		nodes[i].setPoint(gmds::math::Point(x,y,z));
	}

	// les groupes visibles ont leur display de changé
	std::vector<std::string> visibles = getContext().getGroupManager().getVisibles();

	std::vector<Mesh::Volume*> volumes;
	getContext().getGroupManager().get(visibles, volumes);
	for (uint i=0; i<volumes.size(); i++)
		getInfoCommand().addMeshInfoEntity(volumes[i] ,Internal::InfoCommand::DISPMODIFIED);

	std::vector<Mesh::Surface*> surfaces;
	getContext().getGroupManager().get(visibles, surfaces);
	for (uint i=0; i<surfaces.size(); i++)
		getInfoCommand().addMeshInfoEntity(surfaces[i] ,Internal::InfoCommand::DISPMODIFIED);

	std::vector<Mesh::Cloud*> nuages;
	getContext().getGroupManager().get(visibles, nuages);
	for (uint i=0; i<nuages.size(); i++)
		getInfoCommand().addMeshInfoEntity(nuages[i] ,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandTransformMesh::
transform(gp_GTrsf* transf)
{
	gmds::IGMesh& mesh = getContext().getLocalMeshManager().getMesh()->getGMDSMesh();

	std::vector<gmds::Node> nodes;
	mesh.getAll(nodes);

	for (uint i=0; i<nodes.size(); i++){
		gmds::math::Point pt = nodes[i].getPoint();
		double x = pt.X();
		double y = pt.Y();
		double z = pt.Z();

		transf->Transforms(x, y, z);

		nodes[i].setPoint(gmds::math::Point(x,y,z));
	}

	// les groupes visibles ont leur display de changé
	std::vector<std::string> visibles = getContext().getGroupManager().getVisibles();

	std::vector<Mesh::Volume*> volumes;
	getContext().getGroupManager().get(visibles, volumes);
	for (uint i=0; i<volumes.size(); i++)
		getInfoCommand().addMeshInfoEntity(volumes[i] ,Internal::InfoCommand::DISPMODIFIED);

	std::vector<Mesh::Surface*> surfaces;
	getContext().getGroupManager().get(visibles, surfaces);
	for (uint i=0; i<surfaces.size(); i++)
		getInfoCommand().addMeshInfoEntity(surfaces[i] ,Internal::InfoCommand::DISPMODIFIED);

	std::vector<Mesh::Cloud*> nuages;
	getContext().getGroupManager().get(visibles, nuages);
	for (uint i=0; i<nuages.size(); i++)
		getInfoCommand().addMeshInfoEntity(nuages[i] ,Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandTransformMesh::internalRedo()
{
	// on refait l'opération
	internalExecute();
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
