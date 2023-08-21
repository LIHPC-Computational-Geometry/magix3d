/*----------------------------------------------------------------------------*/
/*
 * \file DisplayRepresentation.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 21 déc. 2010
 */
/*----------------------------------------------------------------------------*/
#include "Utils/DisplayRepresentation.h"
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
DisplayRepresentation::
DisplayRepresentation(const type t, const display_type dt)
: m_displayType(dt)
, m_type(t)
, m_shrink(1.0)
{
}
/*----------------------------------------------------------------------------*/
DisplayRepresentation::DisplayRepresentation ( )
: m_displayType (DISPLAY_GEOM)
, m_type (SOLID)
, m_shrink (1.0)
{
}	// DisplayRepresentation::DisplayRepresentation
/*----------------------------------------------------------------------------*/
DisplayRepresentation::DisplayRepresentation (const DisplayRepresentation& dr)
: m_displayType (dr.m_displayType)
, m_type (dr.m_type)
, m_shrink (dr.m_shrink)
{
	m_points.insert(m_points.begin(), dr.m_points.begin(), dr.m_points.end());
	m_surfaceDiscretization.insert(m_surfaceDiscretization.begin(), dr.m_surfaceDiscretization.begin(), dr.m_surfaceDiscretization.end());
	m_curveDiscretization.insert(m_curveDiscretization.begin(), dr.m_curveDiscretization.begin(), dr.m_curveDiscretization.end());
	m_isoCurveDiscretization.insert(m_isoCurveDiscretization.begin(), dr.m_isoCurveDiscretization.begin(), dr.m_isoCurveDiscretization.end());
	m_vector.insert(m_vector.begin(), dr.m_vector.begin(), dr.m_vector.end());
}	// DisplayRepresentation::DisplayRepresentation
/*----------------------------------------------------------------------------*/
DisplayRepresentation& DisplayRepresentation::operator = (const DisplayRepresentation& dr)
{
	if (&dr != this)
	{
		m_displayType	= dr.m_displayType;
		m_type		= dr.m_type;
		m_shrink	= dr.m_shrink;
		m_points.insert(m_points.begin(), dr.m_points.begin(), dr.m_points.end());
		m_surfaceDiscretization.insert(m_surfaceDiscretization.begin(), dr.m_surfaceDiscretization.begin(), dr.m_surfaceDiscretization.end());
		m_curveDiscretization.insert(m_curveDiscretization.begin(), dr.m_curveDiscretization.begin(), dr.m_curveDiscretization.end());
		m_isoCurveDiscretization.insert(m_isoCurveDiscretization.begin(), dr.m_isoCurveDiscretization.begin(), dr.m_isoCurveDiscretization.end());
		m_vector.insert(m_vector.begin(), dr.m_vector.begin(), dr.m_vector.end());
	}	// if (&dr != this)

	return *this;
}	// DisplayRepresentation::DisplayRepresentation
/*----------------------------------------------------------------------------*/
DisplayRepresentation::
~DisplayRepresentation()
{
}
/*----------------------------------------------------------------------------*/
DisplayRepresentation::display_type DisplayRepresentation::getDisplayType() const
{
    return m_displayType;
}
/*----------------------------------------------------------------------------*/
void DisplayRepresentation::setDisplayType (DisplayRepresentation::display_type dt)
{
	m_displayType	= dt;
}
/*----------------------------------------------------------------------------*/
void DisplayRepresentation::setType(const int t)
{
    m_type = t;
}
/*----------------------------------------------------------------------------*/
int DisplayRepresentation::getType() const
{
    return m_type;
}
/*----------------------------------------------------------------------------*/
bool DisplayRepresentation::hasRepresentation(const type& rep) const
{
    return ((m_type|rep)==m_type);
}
/*----------------------------------------------------------------------------*/
void DisplayRepresentation::addRepresentation(const type& rep)
{
    m_type = m_type|rep;
}
/*----------------------------------------------------------------------------*/
void DisplayRepresentation::setShrink(const double d)
{
    if(d>0 || d<=1.0)
        m_shrink=d;
    else
        throw TkUtil::Exception("Invalid shrink value");
}
/*----------------------------------------------------------------------------*/
double DisplayRepresentation::getShrink() const
{
    return m_shrink;
}
/*----------------------------------------------------------------------------*/
std::vector<Utils::Math::Point>& DisplayRepresentation::getPoints()
{
    return m_points;
}
/*----------------------------------------------------------------------------*/
std::vector<size_t>& DisplayRepresentation::getSurfaceDiscretization()
{
    return m_surfaceDiscretization;
}
/*----------------------------------------------------------------------------*/
std::vector<size_t>& DisplayRepresentation::getCurveDiscretization()
{
    return m_curveDiscretization;
}
/*----------------------------------------------------------------------------*/
std::vector<size_t>& DisplayRepresentation::getIsoCurveDiscretization()
{
    return m_isoCurveDiscretization;
}
/*----------------------------------------------------------------------------*/
std::vector<Utils::Math::Point>& DisplayRepresentation::getVector()
{
    return m_vector;
}
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
