/*----------------------------------------------------------------------------*/
/*
 * \file GeomDisplayRepresentation
 *
 *  \author Franck Ledoux
 *
 *  \date 30/11/2010
 */
/*----------------------------------------------------------------------------*/
#include "Geom/GeomDisplayRepresentation.h"
#include "TkUtil/Exception.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomDisplayRepresentation::
GeomDisplayRepresentation(const type t, const double d)
: Utils::DisplayRepresentation(t,
        Utils::DisplayRepresentation::DISPLAY_GEOM)
, m_deflection(d)
, m_nb_pts(1001)
{
}
/*----------------------------------------------------------------------------*/
GeomDisplayRepresentation::~GeomDisplayRepresentation()
{
}
/*----------------------------------------------------------------------------*/
void GeomDisplayRepresentation::setDeflection(const double d)
{
    m_deflection=d;
}
/*----------------------------------------------------------------------------*/
double GeomDisplayRepresentation::getDeflection() const
{
    return m_deflection;
}
/*----------------------------------------------------------------------------*/
int GeomDisplayRepresentation::getNbPts() const
{
    return m_nb_pts;
}
/*----------------------------------------------------------------------------*/
void GeomDisplayRepresentation::setNbPts(const int nb)
{
	m_nb_pts=nb;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
