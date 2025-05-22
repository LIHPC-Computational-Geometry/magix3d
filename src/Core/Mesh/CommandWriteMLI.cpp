/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Mesh/CommandWriteMLI.h"
#include "Mesh/MeshItf.h"
#include "Mesh/MeshManager.h"
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
	getContext().getMeshManager().getMesh()->writeMli(m_file_name);
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
