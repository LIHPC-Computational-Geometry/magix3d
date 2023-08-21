/*----------------------------------------------------------------------------*/
/*
 * Rotation.cpp
 *
 *  Created on: 17/1/2014
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Utils/Rotation.h"
#include "Utils/Vector.h"
#include "Utils/MgxNumeric.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
namespace Math {
/*----------------------------------------------------------------------------*/
Rotation::Rotation (const Point& p1, const Point& p2, const double& angle)
:m_pt1(p1), m_pt2(p2), m_angle(angle), m_pythonCommand ( ), m_axis (Rotation::ANY)
{
	if (m_angle<0.0)
		m_angle+=360.0;

	if (m_angle<0.0 || m_angle>360.0)
		throw TkUtil::Exception (TkUtil::UTF8String ("L'angle est en degrés et doit être compris entre 0 et 360", TkUtil::Charset::UTF_8));

	if (Utils::Math::MgxNumeric::isNearlyZero((m_pt1-m_pt2).norme2()))
		throw TkUtil::Exception (TkUtil::UTF8String ("Les 2 points doivent être distincts pour définir un axe", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Rotation::Rotation (Rotation::AXIS axis, const double& angle)
:m_pt1(0., 0., 0.), m_pt2(1., 0., 0.), m_angle(angle), m_pythonCommand ( ), m_axis(axis)
{
	switch (axis)
	{
		case Rotation::X :	
			m_pt2	= Point (1., 0., 0.);
			break;
		case Rotation::Y :	
			m_pt2	= Point (0., 1., 0.);
			break;
		case Rotation::Z :	
			m_pt2	= Point (0., 0., 1.);
			break;
		default	:
			throw TkUtil::Exception (TkUtil::UTF8String ("L'axe de rotation doit être X, Y ou Z.", TkUtil::Charset::UTF_8));
	}

	if (m_angle<0.0)
		m_angle+=360.0;

	if (m_angle<0.0 || m_angle>360.0)
		throw TkUtil::Exception (TkUtil::UTF8String ("L'angle est en degrés et doit être compris entre 0 et 360", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Rotation::Rotation (const Point& pCenter, const Point& pStart, const Point& pEnd)
:m_pt1(pCenter), m_pt2(1., 0., 0.), m_angle(0), m_pythonCommand ( ), m_axis (Rotation::ANY)
{
	Vector v1(pCenter, pStart);
	Vector v2(pCenter, pEnd);

	v1 /= v1.norme();
	v2 /= v2.norme();

	// la normale au plan
	Vector normale = v1*v2;
	if (MgxNumeric::isNearlyZero(normale.norme())){
		TkUtil::UTF8String messErr (TkUtil::Charset::UTF_8);
		messErr << "La rotation n'est pas définie correctement. Les 3 points sont surements alignés";
		throw TkUtil::Exception(messErr);
	}
	m_pt2 = m_pt1 + normale;

	// l'angle
	double ps = v1.dot(v2); // produit scalaire
	m_angle =  acos(ps)*180/M_PI;

}
/*----------------------------------------------------------------------------*/
Rotation& Rotation::operator = (const Rotation& r)
{
	if (&r != this)
	{
		m_pt1			= r.m_pt1;
		m_pt2			= r.m_pt2;
		m_angle			= r.m_angle;
		m_pythonCommand	= r.getScriptCommand ( );
		m_axis			= r.m_axis;
	}

	return *this;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String Rotation::getScriptCommand() const {
	if (false == m_pythonCommand.empty ( ))
		return m_pythonCommand;	// Issu par copie d'une classe dérivée type RotX

        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        o << getMgx3DAlias() << ".Rotation("
          <<m_pt1.getScriptCommand()<<", "<<m_pt2.getScriptCommand()
          <<", "<<Utils::Math::MgxNumeric::userRepresentation (m_angle)
          << ")";
        return o;
    }
/*----------------------------------------------------------------------------*/
std::ostream & operator << (std::ostream & o , const Rotation & r)
{
    o << " [ " << r.m_pt1 << ", " << r.m_pt2 << ", "
    		<< Utils::Math::MgxNumeric::userRepresentation(r.m_angle) << "] ";
    return o;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o , const Rotation & r)
{
    o << " [ " << r.m_pt1 << ", " << r.m_pt2 << ", "
    		<< Utils::Math::MgxNumeric::userRepresentation(r.m_angle) << "] ";
    return o;
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
