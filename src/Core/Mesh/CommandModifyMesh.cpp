/*----------------------------------------------------------------------------*/
/*
 * \file CommandModifyMesh.cpp
 *
 *  \author Nicolas Le Goff
 *
 *  \date 20 mar. 2014
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/CommandModifyMesh.h"
#include "Mesh/MeshItf.h"
#include "Mesh/MeshManager.h"

/// TkUtil
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>

#include "Utils/Command.h"
#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
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
	getContext().getLocalMeshManager().getMesh()->smooth();
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
