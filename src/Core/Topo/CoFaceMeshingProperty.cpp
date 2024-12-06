/*----------------------------------------------------------------------------*/
/*
 * \file CoFaceMeshingProperty.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 11 oct. 2013
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CoFaceMeshingProperty.h"
#include "Topo/CoFace.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"

#include "Utils/MgxNumeric.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"

#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
bool CoFaceMeshingProperty::operator == (const CoFaceMeshingProperty& mp) const
{
	if (getMeshLaw() != mp.getMeshLaw())
		return false;

	return true;
}
/*----------------------------------------------------------------------------*/
bool CoFaceMeshingProperty::operator != (const CoFaceMeshingProperty& mp) const
{
	return !(*this == mp);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
