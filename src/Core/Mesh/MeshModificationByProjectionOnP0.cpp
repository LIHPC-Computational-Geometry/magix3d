/*----------------------------------------------------------------------------*/
/*
 * \file MeshModificationByProjectionOnP0.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 20/3/2018
 */
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshImplementation.h"
#include "Mesh/MeshManager.h"
#include "Utils/SerializedRepresentation.h"
#include "Utils/MgxNumeric.h"
/*----------------------------------------------------------------------------*/
#include <Mesh/MeshModificationByProjectionOnP0.h>
/*----------------------------------------------------------------------------*/
#include "GMDS/IG/Node.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
MeshModificationByProjectionOnP0::
MeshModificationByProjectionOnP0(eTypeCoord typeCoord)
 : MeshModificationItf()
, m_typeCoord(typeCoord)
{
}
/*----------------------------------------------------------------------------*/
MeshModificationByProjectionOnP0::
~MeshModificationByProjectionOnP0()
{
}
/*----------------------------------------------------------------------------*/
void MeshModificationByProjectionOnP0::
addToDescription (Mgx3D::Utils::SerializedRepresentation* description) const
{
	std::string strTypeCoord;
	if (m_typeCoord == X)
		strTypeCoord = "X";
	else if (m_typeCoord == Y)
		strTypeCoord = "Y";
	else if (m_typeCoord == Z)
		strTypeCoord = "Z";
	else
		strTypeCoord = "inconnu";

	description->addProperty (
			Utils::SerializedRepresentation::Property (
					"Mise à 0", strTypeCoord));
}
/*----------------------------------------------------------------------------*/
void MeshModificationByProjectionOnP0::
applyModification(std::vector<gmds::Node >& gmdsNodes)
{
#ifdef _DEBUG2
	std::cout<<"MeshModificationByProjectionOnP0::applyModification pour "<<gmdsNodes.size()<<" noeuds"<<std::endl;
#endif

	for (std::vector<gmds::Node >::iterator iter = gmdsNodes.begin();
			iter != gmdsNodes.end(); ++iter){
		gmds::Node nd = *iter;

		if (m_typeCoord == X)
			nd.setX(0.0);
		else if (m_typeCoord == Y)
			nd.setY(0.0);
		else if (m_typeCoord == Z)
			nd.setZ(0.0);

	} // end for iter

}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
