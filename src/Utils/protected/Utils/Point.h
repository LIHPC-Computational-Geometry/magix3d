/*----------------------------------------------------------------------------*/
/** \file Point.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 8/10/2010
 *
 *  source récupéré dans Maille3D/vecteur.h, similaires à ceux de MeshUtils, sans Lima,
 *  décomposé en Point, Vector et Spherical
 */
/*----------------------------------------------------------------------------*/

#ifndef MGX3D_UTILS_POINT_H
#define MGX3D_UTILS_POINT_H

#include <iostream>
#include <cmath>
#include <vector>
#include <TkUtil/UTF8String.h>
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"

/*!
 * \namespace Mgx3D
 *
 * \brief Espace de nom du projet Magix 3D.
 *
 * Toutes les classes et types utilisés dans \b Magix3D sont dans ce namespace.
 */
namespace Mgx3D {

/*!
 * \namespace Mgx3D::Utils
 *
 * \brief Espace de nom des utilitaires communs au projet.
 *
 *
 */
namespace Utils {

/*!
 * \namespace Mgx3D::Utils::Math
 *
 * \brief Espace de nom des utilitaires spécifiquement pour ce qui est mathématique.
 *
 *
 */
namespace Math {

/*----------------------------------------------------------------------------*/
class Spherical;
class Cylindrical;
/*----------------------------------------------------------------------------*/
class Point
{
protected:
	double m_x, m_y, m_z;

public:
	Point (double x0, double y0, double z0):m_x (x0), m_y (y0), m_z (z0)
    {
    }
	Point (const Point& p):m_x (p.m_x), m_y (p.m_y), m_z (p.m_z)
	{
	}
	Point ():m_x (0.0), m_y (0.0), m_z (0.0)
	{
	}

	Point(const Spherical & s);
	Point(const Cylindrical & c);

	Point operator + (const Point & v2) const{
		return Point (this->m_x + v2.m_x, this->m_y + v2.m_y, this->m_z + v2.m_z);
	}

	Point operator - (const Point & v2) const{
		return Point (this->m_x - v2.m_x, this->m_y - v2.m_y, this->m_z - v2.m_z);
	}

	Point operator * (const Point & v2) const{
		return Point (this->m_y * v2.m_z - this->m_z * v2.m_y,
					-this->m_x * v2.m_z + this->m_z * v2.m_x,
					this->m_x * v2.m_y - this->m_y * v2.m_x);
	}

	Point operator * (double x) const{
		return Point (this->m_x * x, this->m_y * x, this->m_z * x);
	}

	Point operator / (double x) const{
		return Point (this->m_x / x, this->m_y / x, this->m_z / x);
	}

#ifndef SWIG
friend Point operator * (double x, const Point & v1);

friend std::ostream & operator << (std::ostream & , const Point &);
friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Point &);

/// Script pour commande de création
virtual TkUtil::UTF8String getScriptCommand() const {
	TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
	o << getMgx3DAlias() << ".Point(" << MgxNumeric::userRepresentation (m_x) << ", " << MgxNumeric::userRepresentation (m_y) << ", " << MgxNumeric::userRepresentation (m_z) << ")";
	return o;
}
#endif
/*----------------------------------------------------------------------------*/
virtual Point & operator += (const Point & v1) {
	m_x += v1.m_x;
	m_y += v1.m_y;
	m_z += v1.m_z;
	return *this;
}
/*----------------------------------------------------------------------------*/
virtual Point & operator *= (const double & x) {
	m_x *= x;
	m_y *= x;
	m_z *= x;
	return *this;
}
/*----------------------------------------------------------------------------*/
virtual Point & operator /= (const double & x) {
	m_x /= x;
	m_y /= x;
	m_z /= x;
	return *this;
}

/*----------------------------------------------------------------------------*/
virtual double getCoord (int i) const {
	if (i == 0)
		return m_x;
	else if (i == 1)
		return m_y;
	else
		return m_z;
}

/*----------------------------------------------------------------------------*/
virtual std::vector<double> getCoords() const {
	std::vector<double> coords;
	coords.push_back(m_x);
	coords.push_back(m_y);
	coords.push_back(m_z);
	return coords;
}
/*----------------------------------------------------------------------------*/
virtual void setCoord (int i, double d) {
	if (i == 0)
		m_x = d;
	else if (i == 1)
		m_y = d;
	else if (i == 2)
		m_z = d;
}

/*----------------------------------------------------------------------------*/
virtual double getX () const
{
	return m_x;
}
virtual double getY () const
{
	return m_y;
}
virtual double getZ () const
{
	return m_z;
}

/*----------------------------------------------------------------------------*/
virtual void setX (double v)
{
	m_x = v;
}
/*----------------------------------------------------------------------------*/
virtual void setY (double v)
{
	m_y = v;
}
/*----------------------------------------------------------------------------*/
virtual void setZ (double v)
{
	m_z = v;
}
/*----------------------------------------------------------------------------*/
virtual void setXYZ (double x, double y, double z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

/*----------------------------------------------------------------------------*/
inline double norme2 () const
{
	return m_x * m_x + m_y * m_y + m_z * m_z;
}
/*----------------------------------------------------------------------------*/
inline double norme () const
{
	return std::sqrt (m_x * m_x + m_y * m_y + m_z * m_z);
}

/*----------------------------------------------------------------------------*/
virtual double length (const Point & v) const
{
    double xx = m_x-v.m_x;
    double yy = m_y-v.m_y;
    double zz = m_z-v.m_z;

    return std::sqrt(xx*xx+yy*yy+zz*zz);
}
/*----------------------------------------------------------------------------*/
virtual double length2 (const Point & v) const
{
    double xx = m_x-v.m_x;
    double yy = m_y-v.m_y;
    double zz = m_z-v.m_z;

    return xx*xx+yy*yy+zz*zz;
}

/*----------------------------------------------------------------------------*/
virtual bool isEpsilonEqual (const Point & v, const double & tol) const
{
    double sq_dist = length2(v);

    return Mgx3D::Utils::Math::MgxNumeric::isNearlyZero(sq_dist,tol*tol);
}

/*----------------------------------------------------------------------------*/
virtual bool operator == (const Point & v) const
{
    return Mgx3D::Utils::Math::MgxNumeric::isNearlyZero(length(v));
}

/*----------------------------------------------------------------------------*/
virtual double dot (const Point & v2) const
{
	return (m_x * v2.m_x + m_y * v2.m_y + m_z * v2.m_z);
}

};
/*----------------------------------------------------------------------------*/
#ifndef SWIG
/*----------------------------------------------------------------------------*/
inline std::ostream & operator << (std::ostream & o , const Point & p)
{
	//o << " [ " << p.m_x << ", " << p.m_y << ", " << p.m_z << "] ";
	o << " [ " << MgxNumeric::userRepresentation (p.m_x) << ", " << MgxNumeric::userRepresentation (p.m_y) << ", " << MgxNumeric::userRepresentation (p.m_z) << "] ";
	return o;
}
/*----------------------------------------------------------------------------*/
inline TkUtil::UTF8String & operator << (TkUtil::UTF8String & o , const Point & p)
{
	o << " [ " << MgxNumeric::userRepresentation (p.m_x) << ", " << MgxNumeric::userRepresentation (p.m_y) << ", " << MgxNumeric::userRepresentation (p.m_z) << "] ";
	return o;
}
/*----------------------------------------------------------------------------*/
inline Point operator * (double x, const Point & v1)
{
	return Point (v1.m_x * x, v1.m_y * x, v1.m_z * x);
}
/*----------------------------------------------------------------------------*/
inline double scaMul (Point  u, Point  v)	// Produit scalaire
{
	return (u.getX() * v.getX() + u.getY() * v.getY() + u.getZ() * v.getZ());
}
/*----------------------------------------------------------------------------*/

extern "C"
{
/*----------------------------------------------------------------------------*/

/** Détermination du point qui minimise la somme des carrés des distances aux 3 droites

 *   En paramètre: les 3 paires de points qui déterminent les 3 droites
 *   dont on cherche à minimiser la distance avec le point recherché.
 *   Les droites ne sont pas parallèles.
 */
Utils::Math::Point minDist3Droites(Utils::Math::Point & p1x, Utils::Math::Point & p2x,
		Utils::Math::Point & p1y, Utils::Math::Point & p2y,
		Utils::Math::Point & p1z, Utils::Math::Point & p2z);

/*----------------------------------------------------------------------------*/
/** retourne le point à l'intersection des 3 plans suivant les 3 droites (p1x,p2x), ...
    et pour les trois paramètres lx, ly et lz qui donnent chacun la position du point
    intersection entre plan et droite
    (p1x dans le plan x=0, p2x dans le plan x=1 ...)
    modofie dist et y met la somme des carrés des distances aux droites
 */
Utils::Math::Point intersection3Plans(Utils::Math::Point & p1x, Utils::Math::Point & p2x,
		Utils::Math::Point & p1y, Utils::Math::Point & p2y,
		Utils::Math::Point & p1z, Utils::Math::Point & p2z,
		double l1, double l2, double l3, double & dist);

/*----------------------------------------------------------------------------*/
/** retourne l'indice du point le plus proche parmi un vecteur de points
 *  par rapport à la distance au point pt
 */
uint findNearestPoint(std::vector<Utils::Math::Point>& points, const Utils::Math::Point& pt);

/*----------------------------------------------------------------------------*/
/** calcul du point de Bézier par interpolation linéaire
 * */
Utils::Math::Point bezier4(const Utils::Math::Point& pt1,
		const Utils::Math::Point& pt2,
		const Utils::Math::Point& pt3,
		const Utils::Math::Point& pt4,
		const double & param);
Utils::Math::Point bezier3(const Utils::Math::Point& pt1,
		const Utils::Math::Point& pt2,
		const Utils::Math::Point& pt3,
		const double & param);
Utils::Math::Point bezier2(const Utils::Math::Point& pt1,
		const Utils::Math::Point& pt2,
		const double & param);

/*----------------------------------------------------------------------------*/
} // end extern "C"
/*----------------------------------------------------------------------------*/


#endif // ifndef SWIG



/*----------------------------------------------------------------------------*/
} // end namespace Math
} // end namespace Utils
} // end namespace Mgx3D
#endif // ifndef MGX3D_UTILS_POINT_H
