/*----------------------------------------------------------------------------*/
/*
 * \file CommandDestroyMesh.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 23 sept. 2014
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/CommandDestroyMesh.h"
#include "Mesh/Volume.h"
#include "Mesh/Surface.h"
#include "Mesh/Line.h"
#include "Mesh/Cloud.h"
#include "Mesh/MeshItf.h"
#include "Mesh/MeshModificationItf.h"

#include "TkUtil/Exception.h"

#include <GMDS/IG/IGMesh.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandDestroyMesh::CommandDestroyMesh(Internal::Context& c)
: Internal::CommandInternal (c, "Desruction du maillage")
{
}
/*----------------------------------------------------------------------------*/
CommandDestroyMesh::~CommandDestroyMesh()
{
}
/*----------------------------------------------------------------------------*/
void CommandDestroyMesh::internalExecute()
{
	std::vector<Mesh::MeshEntity*> me;
	std::vector<Mesh::Volume*> mvol;
	getContext().getLocalMeshManager().getVolumes(mvol);
	for (uint i=0; i<mvol.size(); i++)
		me.push_back(mvol[i]);
	std::vector<Mesh::Surface*> msurf;
	getContext().getLocalMeshManager().getSurfaces(msurf);
	for (uint i=0; i<msurf.size(); i++)
		me.push_back(msurf[i]);
	std::vector<Mesh::Line*> mline;
	getContext().getLocalMeshManager().getLines(mline);
	for (uint i=0; i<mline.size(); i++)
		me.push_back(mline[i]);
	std::vector<Mesh::Cloud*> mcloud;
	getContext().getLocalMeshManager().getClouds(mcloud);
	for (uint i=0; i<mcloud.size(); i++)
		me.push_back(mcloud[i]);

	for (uint i=0; i<me.size(); i++)
		getInfoCommand().addMeshInfoEntity(me[i], Internal::InfoCommand::DELETED);

	gmds::IGMesh& gmds_mesh = getContext().getMeshManager().getMesh()->getGMDSMesh();
	gmds_mesh.clear();

	// commande de destruction de toutes les entités topologiques qui ont pu être construites
	std::vector<Topo::Block*> blocks;
	getContext().getLocalTopoManager().getBlocks(blocks);
	for (uint i=0; i<blocks.size(); i++)
		blocks[i]->getMeshingData()->setMeshed(false);
	std::vector<Topo::CoFace*> cofaces;
	getContext().getLocalTopoManager().getCoFaces(cofaces);
	for (uint i=0; i<cofaces.size(); i++)
		cofaces[i]->getMeshingData()->setMeshed(false);
	std::vector<Topo::CoEdge*> coedges;
	getContext().getLocalTopoManager().getCoEdges(coedges);
	for (uint i=0; i<coedges.size(); i++)
		coedges[i]->getMeshingData()->setMeshed(false);
	std::vector<Topo::Vertex*> vertices;
	getContext().getLocalTopoManager().getVertices(vertices);
	for (uint i=0; i<vertices.size(); i++)
		vertices[i]->getMeshingData()->setMeshed(false);
}
/*----------------------------------------------------------------------------*/
void CommandDestroyMesh::internalUndo()
{
	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur, la commande de destruction du maillage n'est pas annulable", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandDestroyMesh::internalRedo()
{
	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur, la commande de destruction du maillage n'est pas rejouable", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
