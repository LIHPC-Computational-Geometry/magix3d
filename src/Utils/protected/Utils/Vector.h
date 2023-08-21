/*----------------------------------------------------------------------------*/
/** \file Point.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/11/2010
 *
 *  source récupéré dans Maille3D/vecteur.h, similaires à ceux de MeshUtils, sans Lima
 */
/*----------------------------------------------------------------------------*/

#ifndef UTILS_VECTOR_H
#define UTILS_VECTOR_H

#include <iostream>
#include <cmath>
#include "Utils/Point.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"

namespace Mgx3D {

namespace Utils {

namespace Math {

class Vector: public Point
{
public:
	Vector (double x0, double y0, double z0)
: Point(x0, y0, z0)
{
}
	Vector (const Vector& p): Point(p)
	{
	}
	Vector (const Point& p): Point(p)
	{
	}
	Vector (): Point()
	{
	}
	Vector (const Point& p1, const Point& p2)
	: Point(p2.getX()-p1.getX(), p2.getY()-p1.getY(), p2.getZ()-p1.getZ())
	{
	}


	Vector operator + (const Vector & v2) const{
		return Vector (this->m_x + v2.m_x, this->m_y + v2.m_y, this->m_z + v2.m_z);
	}

	Vector operator - (const Vector & v2) const{
		return Vector (this->m_x - v2.m_x, this->m_y - v2.m_y, this->m_z - v2.m_z);
	}

	/// Produit vectoriel
	Vector operator * (const Vector & v2) const{
		return Vector (this->m_y * v2.m_z - this->m_z * v2.m_y,
				-this->m_x * v2.m_z + this->m_z * v2.m_x,
				this->m_x * v2.m_y - this->m_y * v2.m_x);
	}

	Vector operator * (double x) const{
		return Vector (this->m_x * x, this->m_y * x, this->m_z * x);
	}

	Vector operator / (double x) const{
		return Vector (this->m_x / x, this->m_y / x, this->m_z / x);
	}

#ifndef SWIG
friend Vector operator * (double x, const Vector & v1);
friend double mixte (const Vector & v1, const Vector & v2, const Vector & v3);
friend std::ostream & operator << (std::ostream & , const Vector &);

/// Script pour commande de création
virtual TkUtil::UTF8String getScriptCommand() const {
	TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
	o << getMgx3DAlias() << ".Vector(" << MgxNumeric::userRepresentation (m_x) << ", " << MgxNumeric::userRepresentation (m_y) << ", " << MgxNumeric::userRepresentation (m_z) << ")";
	return o;
}
#endif

//produit scalaire
virtual double dot(const Vector & v) const
{
	return m_x * v.m_x + m_y * v.m_y + m_z * v.m_z;
}
virtual double abs2() const
{
	return m_x * m_x + m_y * m_y + m_z * m_z;
}
virtual double abs() const
{
	return std::sqrt (m_x * m_x + m_y * m_y + m_z * m_z);
}

};



#ifndef SWIG
inline std::ostream & operator << (std::ostream & o , const Vector & p)
{
	o << " [ " << p.m_x << ", " << p.m_y << ", " << p.m_z << "] ";
	return o;
}

inline Vector operator * (double x, const Vector & v1)
{
	return Vector (v1.m_x * x, v1.m_y * x, v1.m_z * x);
}

inline double
mixte (const Vector & v1, const Vector & v2, const Vector & v3)
{
	return v1.m_x * v2.m_y * v3.m_z + v2.m_x * v3.m_y * v1.m_z
			+ v3.m_x * v1.m_y * v2.m_z
			- v1.m_z * v2.m_y * v3.m_x - v2.m_z * v3.m_y * v1.m_x
			- v3.m_z * v1.m_y * v2.m_x;
}


inline Vector
TangentArc (const Vector & a, const Vector & b)
{
	return b - (a.dot (b)) * a;
}

inline double
AngleSpherique (const Vector & a, const Vector & b, const Vector & c)
{
	Vector T_ba = TangentArc (b, a);
	Vector T_bc = TangentArc (b, c);

	return std::acos (T_ba.dot (T_bc) / T_ba.abs () / T_bc.abs ());
}


// Aire d'un triangle inscrit dans une sphere
inline double
AireTriangle (const Vector & a, const Vector & b, const Vector & c)
{
	return AngleSpherique (c, a, b) + AngleSpherique (a, b, c) +
			AngleSpherique (b, c, a) - M_PI;
}


// Aire Quadrangle Surfacique
inline double
AireQuadrangleS (const Vector & a, const Vector & b, const Vector & c, const
		Vector & d)
{
	return AireTriangle (a, c, b) + AireTriangle (a, d, c);
}

inline double
AireQuadrangle (const Vector & a, const Vector & b, const Vector & c, const
		Vector & d)
{
	// En fait c'est le volume de l'octaedre contenant forme avec le centre de la
	// sphere et le barycentre de la face;
	Vector d0 = a + b + c + d;
	Vector d1 = a - b - c + d;
	Vector d2 = a + b - c - d;

	return -mixte (d0, d1, d2) / 48.0;
}

#endif // ifndef SWIG


} // end namespace Math
} // end namespace Utils
} // end namespace Mgx3D
#endif // ifndef UTILS_VECTOR_H
