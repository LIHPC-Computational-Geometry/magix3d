/*----------------------------------------------------------------------------*/
/*
 * \file Unit.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9 juil. 2014
 */
/*----------------------------------------------------------------------------*/
#include "Utils/Unit.h"
#include "Utils/Common.h"
#include "TkUtil/Exception.h"
#include "math.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
std::string Unit::toString(const lengthUnit& lu)
{
	if (lu == undefined)
		return "indéfini";
	else if (lu == micron)
		return "micron";
	else if (lu == milimeter)
		return "millimètre";
	else if (lu == centimeter)
		return "centimètre";
	else if (lu == meter)
		return "mètre";
	else
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, toString avec lengthUnit non prévu", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::string Unit::toShortString(const lengthUnit& lu)
{
	if (lu == undefined)
		return "indéfini";
	else if (lu == micron)
		return "um";
	else if (lu == milimeter)
		return "mm";
	else if (lu == centimeter)
		return "cm";
	else if (lu == meter)
		return "m";
	else
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, toShortString avec lengthUnit non prévu", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String Unit::getScriptCommand(const lengthUnit& lu)
{
	TkUtil::UTF8String o (TkUtil::Charset::UTF_8);

	if (lu == undefined)
		throw TkUtil::Exception(TkUtil::UTF8String ("pas de ligne de commande pour le cas indéfini", TkUtil::Charset::UTF_8));
	else if (lu == micron)
		o << getMgx3DAlias() << ".Unit.micron";
	else if (lu == milimeter)
		o << getMgx3DAlias() << ".Unit.milimeter";
	else if (lu == centimeter)
		o << getMgx3DAlias() << ".Unit.centimeter";
	else if (lu == meter)
		o << getMgx3DAlias() << ".Unit.meter";
	else
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, getScriptCommand avec lengthUnit non prévu", TkUtil::Charset::UTF_8));

	return o;
}
/*----------------------------------------------------------------------------*/
Unit::lengthUnit Unit::toLengthUnit(const std::string& name)
{
	if (name == "um" || name == "micron" || name == "UM")
		return micron;
	else if (name == "milimeter" || name == "millimètre" || name == "mm" || name == "MM")
		return milimeter;
	else if (name == "centimeter" || name == "centimètre" || name == "cm" || name == "CM")
		return centimeter;
	else if (name == "meter" || name == "mètre" || name == "m" || name == "M")
		return meter;
	else if (name == "indéfini" || name == "undefined")
		return undefined;
	else
		throw TkUtil::Exception(TkUtil::UTF8String ("Unité de longueur non reconnue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
double Unit::computeFactor(const lengthUnit& luFrom, const lengthUnit& luTo)
{
	double ratio = computeMeterFactor(luFrom)/computeMeterFactor(luTo);
#ifdef _DEBUG
	std::cout<<"Unit::computeFactor("<<toShortString(luFrom)<<", "<<toShortString(luTo)<<") => "<<ratio<<std::endl;
#endif
	return ratio;
}
/*----------------------------------------------------------------------------*/
int Unit::toMeterExposantFactor(const lengthUnit& lu)
{
	if (lu == undefined)
		throw TkUtil::Exception(TkUtil::UTF8String ("Unité de longueur non définie, ne permet pas le calcul du ratio/mètre", TkUtil::Charset::UTF_8));
	else
		return (int)(lu);
}
/*----------------------------------------------------------------------------*/
double Unit::computeMeterFactor(const lengthUnit& lu)
{
	if (lu == undefined)
		throw TkUtil::Exception(TkUtil::UTF8String ("Unité de longueur non définie, ne permet pas le calcul du ratio/mètre", TkUtil::Charset::UTF_8));
	else
		return pow(10,(double)(lu));
}
/*----------------------------------------------------------------------------*/
Unit::lengthUnit Unit::computeUnit(const double& ratio)
{
    if (ratio == 1.0)
        return Unit::meter;
    else if (ratio == 1e-6)
        return Unit::micron;
    else if (ratio == 1e-3)
        return Unit::milimeter;
    else if (ratio == 1e-2)
        return Unit::centimeter;
    else
        return Unit::undefined;
}
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

