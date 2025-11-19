/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Mesh/CommandDestroyMesh.h"
#include "Mesh/Volume.h"
#include "Mesh/Surface.h"
#include "Mesh/Line.h"
#include "Mesh/Cloud.h"
#include "Mesh/MeshItf.h"
#include "Mesh/MeshModificationItf.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <gmds/ig/Mesh.h>
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
	Mesh::MeshManager& mm = getContext().getMeshManager();
	for (Mesh::Volume* v : mm.getVolumesObj())
		me.push_back(v);
	for (Mesh::Surface* s : mm.getSurfacesObj())
		me.push_back(s);
	for (Mesh::Line* l : mm.getLinesObj())
		me.push_back(l);
	for (Mesh::Cloud* c : mm.getCloudsObj())
		me.push_back(c);

	for (uint i=0; i<me.size(); i++)
		getInfoCommand().addMeshInfoEntity(me[i], Internal::InfoCommand::DELETED);

	gmds::Mesh& gmds_mesh = getContext().getMeshManager().getMesh()->getGMDSMesh();
	gmds_mesh.clear();

	// commande de destruction de toutes les entités topologiques qui ont pu être construites
	Topo::TopoManager& tm = getContext().getTopoManager();
	for (Topo::Block* b : tm.getBlocksObj())
		b->getMeshingData()->setMeshed(false);
	for (Topo::CoFace* cf : tm.getCoFacesObj())
		cf->getMeshingData()->setMeshed(false);
	for (Topo::CoEdge* ce : tm.getCoEdgesObj())
		ce->getMeshingData()->setMeshed(false);
	for (Topo::Vertex* v : tm.getVerticesObj())
		v->getMeshingData()->setMeshed(false);
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
