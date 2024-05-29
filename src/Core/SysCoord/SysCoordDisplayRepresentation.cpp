/*----------------------------------------------------------------------------*/
/*
 * \file SysCoordDisplayRepresentation
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 23/5/2018
 */
/*----------------------------------------------------------------------------*/
#include "SysCoord/SysCoordDisplayRepresentation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
SysCoordDisplayRepresentation::
SysCoordDisplayRepresentation(const type t)
: Utils::DisplayRepresentation(t, Utils::DisplayRepresentation::DISPLAY_SYSCOORD)
, m_transform( )
{
}
/*----------------------------------------------------------------------------*/
SysCoordDisplayRepresentation::~SysCoordDisplayRepresentation()
{
}
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
