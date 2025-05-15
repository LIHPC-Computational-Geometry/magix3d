/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Mesh/CommandWriteVTK.h"
#include "Mesh/MeshItf.h"
#include "Mesh/MeshManager.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandWriteVTK::CommandWriteVTK(Internal::Context& c, std::string& nom)
: Internal::CommandInternal (c, "Sauvegarde du maillage (vtk)")
, m_file_name(nom)
{
}
/*----------------------------------------------------------------------------*/
void CommandWriteVTK::internalExecute()
{
	getContext().getMeshManager().getMesh()->writeVTK(m_file_name);
}
/*----------------------------------------------------------------------------*/
void CommandWriteVTK::internalUndo()
{
	// rien à faire
}
/*----------------------------------------------------------------------------*/
void CommandWriteVTK::internalRedo()
{
	// on ne refait pas la sauvegarde
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
