/*----------------------------------------------------------------------------*/
/*
 * Plane.h
 *
 *  Created on: 24 juin 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_UTILS_PLANE_H
#define MGX3D_UTILS_PLANE_H
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <cmath>
#include <TkUtil/UTF8String.h>
#include "Utils/Common.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
/*!
 * \namespace Mgx3D
 *
 * \brief Espace de nom du projet Magix 3D.
 *
 * Toutes les classes et types utilisés dans \b Magix3D sont dans ce namespace.
 */
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
/*!
 * \namespace Mgx3D::Utils
 *
 * \brief Espace de nom des utilitaires communs au projet.
 *
 *
 */
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
/*!
 * \namespace Mgx3D::Utils::Math
 *
 * \brief Espace de nom des utilitaires spécifiquement pour ce qui est mathématique.
 *
 *
 */
/*----------------------------------------------------------------------------*/
namespace Math {
/*----------------------------------------------------------------------------*/
class Plane
{
protected:
    Point m_point;
    Vector m_normal;

public:

    enum XYZ {XY,YZ,XZ};


    /*------------------------------------------------------------------------*/
    /**  Constructeur par défaut définissant le plan XY
     */
    Plane ():m_point(Point(0,0,0)), m_normal(Vector(0,0,1)) {}

    /*------------------------------------------------------------------------*/
    /**  Constructeur
     *   Définition d'un plan à partir d'un point et d'un vecteur normal
     *
     *  @param p un point
     *  @param v un vecteur, qui définit la normale au plan
     */
    Plane (Point& p, Vector& v):m_point(p), m_normal(v) {}

    /*------------------------------------------------------------------------*/
    /**  Constructeur
     *   Définition d'un plan à partir de trois points
     *
     *  @param p1 un premier point
     *  @param p2 un second point
     *  @param p3 un plan point
     */
    Plane (Point& p1, Point& p2, Point& p3);
    /*------------------------------------------------------------------------*/
    /**  Constructeur
     *   Définition d'un plan comme étant parallèle à XY, YZ ou ZY. Le déplacement
     *   est indiqué par un décalage
     *
     *  @param type le type de plan (XY, XZ ou YZ)
     *  @param offset le décalage par rapport au plan initial de type @type
     */
    Plane(const XYZ& type, const double offset=0);

    /*------------------------------------------------------------------------*/
    /**  Constructeur par copie
     *  @param p3 le plan à copier
     */
    Plane (const Plane& p):m_point(p.m_point), m_normal(p.m_normal)  {}



#ifndef SWIG

    friend std::ostream & operator << (std::ostream & , const Plane &);
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Plane &);

    /// Script pour commande de création
    virtual TkUtil::UTF8String getScriptCommand() const {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        o << getMgx3DAlias() << ".Plane(" << m_point.getScriptCommand()
          << ", " << m_normal.getScriptCommand() << ")";
        return o;
    }
#endif

    virtual Point getPoint () const
    {
        return m_point;
    }

    virtual Vector getNormal () const
    {
        return m_normal;
    }


};
/*----------------------------------------------------------------------------*/
#ifndef SWIG
/*----------------------------------------------------------------------------*/
inline std::ostream & operator << (std::ostream & o , const Plane & p)
{
    o << " [ " << p.m_point << ", " << p.m_normal << "] ";
    return o;
}
/*----------------------------------------------------------------------------*/
inline TkUtil::UTF8String & operator << (TkUtil::UTF8String & o , const Plane & p)
{
    o << " [ " << p.m_point << ", " << p.m_normal << "] ";
    return o;
}

/*----------------------------------------------------------------------------*/
#endif // ifndef SWIG
/*----------------------------------------------------------------------------*/
} // end namespace Math
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif // ifndef MGX3D_UTILS_PLANE_H
/*----------------------------------------------------------------------------*/

