/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Mesh/CommandModifyMesh.h"
#include "Mesh/MeshItf.h"
#include "Mesh/MeshManager.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandModifyMesh::
CommandModifyMesh(Internal::Context& c, std::string& name)
: Internal::CommandInternal (c, "Lissage du maillage")
{

}
/*----------------------------------------------------------------------------*/
CommandModifyMesh::~CommandModifyMesh()
{

}
/*----------------------------------------------------------------------------*/
void CommandModifyMesh::
internalExecute()
{
	getContext().getMeshManager().getMesh()->smooth();
}
/*----------------------------------------------------------------------------*/
void CommandModifyMesh::
internalUndo()
{
	throw TkUtil::Exception (TkUtil::UTF8String ("CommandModifyMesh::internalUndo pas encore implémenté.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void CommandModifyMesh::
internalRedo()
{
	throw TkUtil::Exception (TkUtil::UTF8String ("CommandModifyMesh::internalRedo pas encore implémenté.", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
