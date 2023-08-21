/**
 * \file		MgxNumeric.cpp
 * \author		Charles PIGNEROL
 * \date		18/10/2013
 */

#include "Utils/MgxNumeric.h"
#include "Utils/Common.h"

#include <TkUtil/NumericConversions.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>

#include <limits>
#include <cmath>
#include <iomanip>
#include <iostream>


using namespace TkUtil;
using namespace std;


namespace Mgx3D
{

namespace Utils
{

namespace Math
{
// mgxGeomDoubleEpsilon= 1.E-5 ok pour Job34
// 1e-6; pour Curve::contains initiallement

double      MgxNumeric::mgxGeomDoubleEpsilon= 1.E-5;
double      MgxNumeric::mgxParameterEpsilon= 1.E-2;

double      MgxNumeric::mgxGeomDoubleEpsilonSquare= MgxNumeric::mgxGeomDoubleEpsilon*MgxNumeric::mgxGeomDoubleEpsilon;

double		MgxNumeric::mgxDoubleEpsilon	= 1.E-13;
double		MgxNumeric::mgxTopoDoubleEpsilon	= 1.E-5; // pour détecter les boites englobantes avec longueur nulle, boite calculée par OCC
size_t		MgxNumeric::mgxDoubleScientificNotationCharMax			= 16;
size_t		MgxNumeric::mgxDoubleFixedNotationCharMax				= 5;


float		MgxNumeric::mgxFloatEpsilon	= 1.E-5;
size_t		MgxNumeric::mgxFloatScientificNotationCharMax			= 10;

unsigned short	MgxNumeric::mgxAngleDecimalNum						= 3;


MgxNumeric::MgxNumeric ( )
{
	MGX_FORBIDDEN ("MgxNumeric constructor is not allowed.")
}	// MgxNumeric::MgxNumeric


MgxNumeric::MgxNumeric (const MgxNumeric&)
{
	MGX_FORBIDDEN ("MgxNumeric copy constructor is not allowed.")
}	// MgxNumeric::MgxNumeric


MgxNumeric& MgxNumeric::operator = (const MgxNumeric&)
{
	MGX_FORBIDDEN ("MgxNumeric assignment operator is not allowed.")
    return *this;
}	// MgxNumeric::operator =


MgxNumeric::~MgxNumeric ( )
{
	MGX_FORBIDDEN ("MgxNumeric destructor is not allowed.")
}	// MgxNumeric::~MgxNumeric


string MgxNumeric::userRepresentation (double d)
{
	try
	{
		 return NumericConversions::userRepresentation (
			d,mgxDoubleScientificNotationCharMax,mgxDoubleFixedNotationCharMax);
	}
	catch (...)
	{
		if (true == NumericServices::isValid (d))
			throw;	// => Quelle erreur ???

		// Ce peut être inf ou nan
		if ((NumericServices::doubleMachInfinity() == d) ||
		    (-NumericServices::doubleMachInfinity() == d))
			return "inf";
		return "nan";
	}
}	// MgxNumeric::userRepresentation


string MgxNumeric::userRepresentation (const Timer& timer)
{
	UTF8String	str (Charset::UTF_8);

	if (10 < timer.duration ( ))
		str << (unsigned long)timer.duration ( ) << " secondes";
	else if (1e6 < timer.microduration ( ))	// 1 sec < d < 10 sec
	{
		double	d	= (double)timer.microduration ( ) / 1.e6;
		str << NumericConversions::shortestRepresentation (d, 3, 3)
		    << " secondes";
	}
	else if (1000 < timer.microduration ( ))	// 1e3 à 1e6 microsecondes
	{
		double	d	= (double)timer.microduration ( ) / 1.e3;
		str << NumericConversions::shortestRepresentation (d, 3, 3)
		    << " millisecondes";
	}
	else	// < 1e3 microsecondes
		str << timer.microduration ( ) << " microsecondes";

	return str.utf8 ( );
}	// MgxNumeric::userRepresentation


}	// namespace Math

}	// namespace Utils

}	// namespace Mgx3D
