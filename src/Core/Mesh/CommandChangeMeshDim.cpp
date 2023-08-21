/*----------------------------------------------------------------------------*/
/*
 * \file CommandChangeMeshDim.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 29/11/2018
 */
/*----------------------------------------------------------------------------*/
#include "Mesh/CommandChangeMeshDim.h"
#include "Mesh/MeshItf.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandChangeMeshDim::
CommandChangeMeshDim(Internal::Context& context, const Internal::ContextIfc::meshDim& dim)
: Internal::CommandInternal (context, "Changement de dimension du maillage en sortie")
, m_new_mesh_dim(dim)
, m_old_mesh_dim(context.m_mesh_dim)
{
	getInfoCommand ( ).addContextInfo (Internal::InfoCommand::MESHDIMMODIFIED);
}
/*----------------------------------------------------------------------------*/
CommandChangeMeshDim::
~CommandChangeMeshDim()
{
}
/*----------------------------------------------------------------------------*/
void CommandChangeMeshDim::
internalExecute()
{
#ifdef _DEBUG2
	std::cout<<"CommandChangeMeshDim::internalExecute => "
	        <<Utils::Unit::toString(m_new_mesh_dim)
	        <<std::endl;
#endif
	getContext().m_mesh_dim = m_new_mesh_dim;
	updateMeshManager();
}
/*----------------------------------------------------------------------------*/
void CommandChangeMeshDim::
internalUndo()
{
	getContext().m_mesh_dim = m_old_mesh_dim;
	updateMeshManager();
}
/*----------------------------------------------------------------------------*/
void CommandChangeMeshDim::
internalRedo()
{
	getContext().m_mesh_dim = m_new_mesh_dim;
	updateMeshManager();
}
/*----------------------------------------------------------------------------*/
void CommandChangeMeshDim::
updateMeshManager()
{
	getContext().getMeshManager().getMesh()->updateMeshDim();
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
