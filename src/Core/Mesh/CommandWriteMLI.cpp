/*----------------------------------------------------------------------------*/
/*
 * \file CommandWriteMLI.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 28 nov. 2013
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/CommandWriteMLI.h"
#include "Mesh/MeshItf.h"
#include "Mesh/MeshManager.h"
#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandWriteMLI::CommandWriteMLI(Internal::Context& c, std::string& nom)
: Internal::CommandInternal (c, "Sauvegarde du maillage (mli)")
, m_file_name(nom)
{
}
/*----------------------------------------------------------------------------*/
void CommandWriteMLI::internalExecute()
{
	getContext().getLocalMeshManager().getMesh()->writeMli(m_file_name);
}
/*----------------------------------------------------------------------------*/
void CommandWriteMLI::internalUndo()
{
	// rien à faire
}
/*----------------------------------------------------------------------------*/
void CommandWriteMLI::internalRedo()
{
	// on ne refait pas la sauvegarde
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
