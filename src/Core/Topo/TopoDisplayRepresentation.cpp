/*----------------------------------------------------------------------------*/
/*
 * \file TopoDisplayRepresentation
 *
 *  \author Eric Brière de l'Isle, Franck Ledoux
 *
 *  \date 9/12/2010
 */
/*----------------------------------------------------------------------------*/
#include "Topo/TopoDisplayRepresentation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
TopoDisplayRepresentation::
TopoDisplayRepresentation(const type t)
: Utils::DisplayRepresentation(t, Utils::DisplayRepresentation::DISPLAY_TOPO)
{
    setShrink(0.9);
}
/*----------------------------------------------------------------------------*/
TopoDisplayRepresentation::~TopoDisplayRepresentation()
{
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
