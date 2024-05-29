/*----------------------------------------------------------------------------*/
/*
 * \file CommandWriteCGNS.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 28 nov. 2013
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/CommandWriteCGNS.h"
#include "Mesh/MeshItf.h"
#include "Mesh/MeshManager.h"
#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandWriteCGNS::CommandWriteCGNS(Internal::Context& c, std::string& nom)
: Internal::CommandInternal (c, "Sauvegarde du maillage (cgns)")
, m_file_name(nom)
{
}
/*----------------------------------------------------------------------------*/
void CommandWriteCGNS::internalExecute()
{
	getContext().getLocalMeshManager().getMesh()->writeCGNS(m_file_name);
}
/*----------------------------------------------------------------------------*/
void CommandWriteCGNS::internalUndo()
{
	// rien à faire
}
/*----------------------------------------------------------------------------*/
void CommandWriteCGNS::internalRedo()
{
	// on ne refait pas la sauvegarde
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
