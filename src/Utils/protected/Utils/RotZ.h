/*----------------------------------------------------------------------------*/
/*
 * RotZ.h
 *
 *  Created on: 20/1/2014
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_UTILS_ROTZ_H
#define MGX3D_UTILS_ROTZ_H
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <TkUtil/UTF8String.h>
#include "Utils/Common.h"
#include "Utils/Point.h"
#include "Utils/Rotation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
namespace Math {
/*----------------------------------------------------------------------------*/
/** Classe pour définir une rotation suivant l'axe des Z
 */
/*----------------------------------------------------------------------------*/
class RotZ : public Rotation
{
public:
	RotZ (const double& angle):Rotation(Rotation::Z, angle) {}

	RotZ (const RotX& r):Rotation(r) {}

	RotZ operator = (const RotZ& r)
	{ Rotation::operator = (r); return *this; }

#ifndef SWIG
    /// Script pour commande de création
    virtual TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        o << getMgx3DAlias() << ".RotZ("
          <<Utils::Math::MgxNumeric::userRepresentation (getAngle())
          << ")";
        return o;
    }

#endif
};
/*----------------------------------------------------------------------------*/
} // end namespace Math
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif // ifndef MGX3D_UTILS_ROTZ_H
/*----------------------------------------------------------------------------*/

