/*----------------------------------------------------------------------------*/
/*
 * \file CommandAddMeshModification.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5 déc. 2013
 */
/*----------------------------------------------------------------------------*/
#include "Group/CommandAddMeshModification.h"
#include "Group/GroupEntity.h"
#include "Utils/Command.h"
#include "Mesh/MeshModificationItf.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Group {
/*----------------------------------------------------------------------------*/
CommandAddMeshModification::
CommandAddMeshModification(Internal::Context& c,
		Group::GroupEntity* grp,
		Mesh::MeshModificationItf* modif)
: Internal::CommandInternal(c, ("Ajoute une modification de maillage"))
, m_grp(grp)
, m_new_modif(modif)
{
}
/*----------------------------------------------------------------------------*/
CommandAddMeshModification::~CommandAddMeshModification()
{
	delete m_new_modif;
}
/*----------------------------------------------------------------------------*/
void CommandAddMeshModification::internalExecute()
{
	m_grp->addMeshModif(m_new_modif);
}
/*----------------------------------------------------------------------------*/
void CommandAddMeshModification::internalUndo()
{
	m_grp->popBackMeshModif();
}
/*----------------------------------------------------------------------------*/
void CommandAddMeshModification::internalRedo()
{
	internalExecute();
}
/*----------------------------------------------------------------------------*/
} // end namespace Group
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
