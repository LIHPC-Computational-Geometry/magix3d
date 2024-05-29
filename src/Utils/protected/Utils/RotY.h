/*----------------------------------------------------------------------------*/
/*
 * RotY.h
 *
 *  Created on: 20/1/2014
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_UTILS_ROTY_H
#define MGX3D_UTILS_ROTY_H
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
/** Classe pour définir une rotation suivant l'axe des Y
 */
/*----------------------------------------------------------------------------*/
class RotY : public Rotation
{
public:
	RotY (const double& angle):Rotation(Rotation::Y, angle) {}

	RotY (const RotX& r):Rotation(r) {}

	RotY operator = (const RotY& r)
	{ Rotation::operator = (r); return *this; }

#ifndef SWIG
    /// Script pour commande de création
    virtual TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        o << getMgx3DAlias() << ".RotY("
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
#endif // ifndef MGX3D_UTILS_ROTY_H
/*----------------------------------------------------------------------------*/

