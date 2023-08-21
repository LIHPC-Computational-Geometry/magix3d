/*----------------------------------------------------------------------------*/
/*
 * Rotation.h
 *
 *  Created on: 17/1/2014
 *  Modifié le 28/8/2017
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_UTILS_ROTATION_H
#define MGX3D_UTILS_ROTATION_H
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <cmath>
#include <TkUtil/UTF8String.h>
#include "Utils/Common.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
namespace Math {
/*----------------------------------------------------------------------------*/
/** Classe pour définir une rotation
 *
 * On se limite dans un premier temps à une rotation défini suivant un axe et un angle (en degrés)
 * Il est possible de renseigner la rotation à partir de 3 points (non alignés)
 *
 */
/*----------------------------------------------------------------------------*/
class Rotation
{
public:

	/// Axe de rotation :
	enum AXIS { X, Y, Z, ANY };

	/// Rotation suivant un axe défini par 2 points et un angle en degrés
	Rotation (const Point& p1, const Point& p2, const double& angle);

	/** Rotation suivant un axe du repère cartésien et un angle en degrés
	*
	* Une exception est levée si axis est différent de X, Y ou Z.
	*/
	Rotation (AXIS axis, const double& angle);

	/** Rotation définie par 3 points. un point fixe, un de départ et un d'arrivée
	 *  L'axe est orthogonal au plan défini par les 3 points.
	 *  L'angle permet de passer d'une direction définie vers le point de départ à une direction vers le point d'arrivée
	 */
	Rotation (const Point& pCenter, const Point& pStart, const Point& pEnd);


	Rotation (const Rotation& r):m_pt1(r.m_pt1), m_pt2(r.m_pt2), m_angle(r.m_angle), m_pythonCommand(r.getScriptCommand ( )), m_axis(r.m_axis)  {}
	Rotation& operator = (const Rotation& r);

#ifndef SWIG

    friend std::ostream & operator << (std::ostream & , const Rotation &);
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const Rotation &);

    /// Script pour commande de création
    virtual TkUtil::UTF8String getScriptCommand() const;

#endif

    virtual Point getAxis1 () const
    {
        return m_pt1;
    }
    virtual Point getAxis2 () const
    {
        return m_pt2;
    }

	virtual AXIS getAxis ( ) const
	{ return m_axis; }

    virtual double getAngle () const
    {
        return m_angle;
    }

private:
    /// premier qui défini l'axe de rotation
    Point m_pt1;
    /// deuxième point
    Point m_pt2;
    /// angle en degré
    double m_angle;
	/** La chaine de caractères décrivant la commande python => permet de
	* propager cette description lors de copies de classes dérivées type
	* RotX (constructeur de copie, opérateur = par exemple).
	*/
	TkUtil::UTF8String	m_pythonCommand;
	/// L'axe de rotation
	AXIS	m_axis;
};
/*----------------------------------------------------------------------------*/
} // end namespace Math
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif // ifndef MGX3D_UTILS_ROTATION_H
/*----------------------------------------------------------------------------*/

