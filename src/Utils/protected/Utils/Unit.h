/*----------------------------------------------------------------------------*/
/*
 * \file Unit.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9 juil. 2014
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_UTILS_UNIT_H_
#define MGX3D_UTILS_UNIT_H_

#include <TkUtil/UTF8String.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/

class Unit
{
public:

	/// les différentes unités de longueur supportées
	enum lengthUnit {
		undefined = 100,
		micron = -6,
		milimeter = -3,
		centimeter = -2,
		meter = 0
	};

	/// conversion en une string ("mètre" ...)
	static std::string toString(const lengthUnit& lu);

	/// conversion en une string ("cm" ...)
	static std::string toShortString(const lengthUnit& lu);

	/// retourne l'UTF8String pour génération d'une ligne de commande python
	static TkUtil::UTF8String getScriptCommand(const lengthUnit& lu);

	/// convertion d'une string en une unité de longueur
	static lengthUnit toLengthUnit(const std::string& name);

	/// calcul le ratio pour exprimer des luFrom en luTo (10 pour passer de cm à mm)
	static double computeFactor(const lengthUnit& luFrom, const lengthUnit& luTo);

	/// retourne l'exposant en base 10 de "lu" par rapport au mètre (-2 pour les cm)
	static int toMeterExposantFactor(const lengthUnit& lu);

	/// calcul le ratio pour exprimer des "lu" en mètres (1e-2 pour des cm)
	static double computeMeterFactor(const lengthUnit& lu);

	/// calcul l'unité à partir du ratio par rapport au mètre
	static lengthUnit computeUnit(const double& ratio);

}; // class Unit
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_UTILS_UNIT_H_ */
/*----------------------------------------------------------------------------*/
