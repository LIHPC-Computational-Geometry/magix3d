/*----------------------------------------------------------------------------*/
/*
 * \file Bounds.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17 sept. 2013
 */
/*----------------------------------------------------------------------------*/
#include "Utils/Bounds.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
Bounds::Bounds(double x, double y, double z)
: m_x_min(x)
, m_x_max(x)
, m_y_min(y)
, m_y_max(y)
, m_z_min(z)
, m_z_max(z)
{
}
/*----------------------------------------------------------------------------*/
Bounds::Bounds(Math::Point pt)
: m_x_min(pt.getX())
, m_x_max(pt.getX())
, m_y_min(pt.getY())
, m_y_max(pt.getY())
, m_z_min(pt.getZ())
, m_z_max(pt.getZ())
{
}
/*----------------------------------------------------------------------------*/
void Bounds::add(double x, double y, double z)
{
    if (x>m_x_max)
        m_x_max = x;
    else if (x<m_x_min)
        m_x_min = x;

    if (y>m_y_max)
        m_y_max = y;
    else if (y<m_y_min)
        m_y_min = y;

    if (z>m_z_max)
        m_z_max = z;
    else if (z<m_z_min)
        m_z_min = z;
}
/*----------------------------------------------------------------------------*/
void Bounds::add(Math::Point pt)
{
    add(pt.getX(), pt.getY(), pt.getZ());
}
/*----------------------------------------------------------------------------*/
void Bounds::get(double bounds[6]) const
{
    bounds[0] = m_x_min;
    bounds[1] = m_x_max;
    bounds[2] = m_y_min;
    bounds[3] = m_y_max;
    bounds[4] = m_z_min;
    bounds[5] = m_z_max;
}
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
