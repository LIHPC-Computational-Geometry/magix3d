/*----------------------------------------------------------------------------*/
/*
 * \file MeshEntity.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/MeshEntity.h"
#include <TkUtil/MemoryError.h>
#include "Utils/SerializedRepresentation.h"
#include "Internal/Context.h"
#include <memory>           // unique_ptr
#include <sys/types.h>			// CP : uint sur Bull
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
MeshEntity::
MeshEntity(Internal::Context& ctx,
                  Utils::Property* prop,
                  Utils::DisplayProperties* disp)
: Internal::InternalEntity (ctx, prop, disp)
, m_mesh_manager(&(ctx.getMeshManager())), m_bounds_to_be_calculate(true)
, m_nodesValuesNames ( ), m_cellsValuesNames ( )
{
	for (uint i=0; i<6; i++)
		m_bounds[i] = 0.0;
}
/*----------------------------------------------------------------------------*/
MeshEntity::
~MeshEntity()
{
    // on pourrait le supprimer du maillage, mais il risque de ne plus y être ... à suivre
}
/*----------------------------------------------------------------------------*/
void MeshEntity::getBounds (double bounds[6]) const
{
	Entity::getBounds (bounds);	// RAZ
}
/*----------------------------------------------------------------------------*/
Mesh::MeshManagerIfc& MeshEntity::getMeshManager() const
{
	CHECK_NULL_PTR_ERROR (m_mesh_manager)
	return *m_mesh_manager;
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* MeshEntity::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
    		InternalEntity::getDescription (alsoComputed));

    return description.release ( );
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> MeshEntity::
getNodesValuesNames ( ) const
{
//std::vector<std::string>	v;
//v.push_back ("distance");
//return v;
	return m_nodesValuesNames;
}
/*----------------------------------------------------------------------------*/
std::vector<std::string> MeshEntity::
getCellsValuesNames ( ) const
{
//std::vector<std::string>	v;
//v.push_back ("surface");
//return v;
	return m_cellsValuesNames;
}
/*----------------------------------------------------------------------------*/
void MeshEntity::
setNodesValuesNames (const std::vector<std::string>& names)
{
	m_nodesValuesNames	= names;
}
/*----------------------------------------------------------------------------*/
void MeshEntity::
setCellsValuesNames (const std::vector<std::string>& names)
{
	m_cellsValuesNames	= names;
}
/*----------------------------------------------------------------------------*/
void MeshEntity::
setNodesValue (const std::string& name, const std::map<int,double>& values)
{
	m_nodesValues[name] = values;
}
/*----------------------------------------------------------------------------*/
std::map<int,double> MeshEntity::
getNodesValue (const std::string& name)
{
	return m_nodesValues[name];
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
