/**
 * \file        QtNumericFieldsFactory.h
 * \author      Charles PIGNEROL
 * \date        09/01/2015
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/QtNumericFieldsFactory.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericServices.h>
#include <iostream>

#include <QDoubleValidator>

using namespace std;
using namespace TkUtil;

using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::QtComponents;


namespace Mgx3D
{

namespace QtComponents
{

// On veut pouvoir faire du copier/coller entre différents champs numériques.
// Ici on évalue la taille la plus grande d'un réel double précision, en
// notation scientifique.
const string			QtNumericFieldsFactory::doubleMaxLength			=
			MgxNumeric::userRepresentation (-NumericServices::doubleMachMax());

// Le nombre de caractères de la partie entière signée et de exposant
// (-x.decimalsE-308) :
const unsigned char	QtNumericFieldsFactory::doubleIntegerPartMaxLength	= 2;	// -x.
const unsigned char	QtNumericFieldsFactory::doubleExponentPartMaxLength	= 5;	// E-308


QtNumericFieldsFactory::QtNumericFieldsFactory ( )
{
	MGX_FORBIDDEN ("QtNumericFieldsFactory default constructor is not allowed.");
}	// QtNumericFieldsFactory::QtNumericFieldsFactory


QtNumericFieldsFactory::QtNumericFieldsFactory (const QtNumericFieldsFactory&)
{
	MGX_FORBIDDEN ("QtNumericFieldsFactory copy constructor is not allowed.");
}	// QtNumericFieldsFactory::QtNumericFieldsFactory


QtNumericFieldsFactory& QtNumericFieldsFactory::operator = (
												const QtNumericFieldsFactory&)
{
	MGX_FORBIDDEN ("QtNumericFieldsFactory assignment operator is not allowed.");
	return *this;
}	// QtNumericFieldsFactory::operator =


QtNumericFieldsFactory::~QtNumericFieldsFactory ( )
{
	MGX_FORBIDDEN ("QtNumericFieldsFactory destructor is not allowed.");
}	// QtNumericFieldsFactory::~QtNumericFieldsFactory


QtDoubleTextField& QtNumericFieldsFactory::createPositionTextField (
						QWidget *parent, bool autoValidation, const char* name)
{
	static const unsigned short	maxLength	= doubleMaxLength.size ( );
	// -x.decimalsE-308
//	static const unsigned short	decimals	= maxLength - 8;
	static const unsigned short	decimals	= MgxNumeric::mgxDoubleScientificNotationCharMax - doubleIntegerPartMaxLength;

	QtDoubleTextField*	textfield	=
					new QtDoubleTextField (parent, autoValidation, name);

	CHECK_NULL_PTR_ERROR (textfield)
/*	textfield->setVisibleColumns (
							MgxNumeric::mgxDoubleScientificNotationCharMax);
	textfield->setNotation (QDoubleValidator::ScientificNotation);*/
	textfield->setDecimals (decimals);
//	textfield->setVisibleColumns (doubleMaxLength.size ( ));
	// +1 : séparateur décimal
	textfield->setVisibleColumns (
		decimals + doubleIntegerPartMaxLength + doubleExponentPartMaxLength + 1);

	return *textfield;
}	// QtNumericFieldsFactory::createPositionTextField


QtDoubleTextField& QtNumericFieldsFactory::createDistanceTextField (
						QWidget *parent, bool autoValidation, const char* name)
{
	static const unsigned short	maxLength	= doubleMaxLength.size ( );
	// -x.decimalsE-308
//	static const unsigned short	decimals	= maxLength - 8;
	static const unsigned short	decimals	= MgxNumeric::mgxDoubleScientificNotationCharMax - doubleIntegerPartMaxLength;

	QtDoubleTextField*	textfield	=
					new QtDoubleTextField (parent, autoValidation, name);

	CHECK_NULL_PTR_ERROR (textfield)
//	textfield->setVisibleColumns (
//							MgxNumeric::mgxDoubleScientificNotationCharMax);
	textfield->setNotation (QDoubleValidator::ScientificNotation);
	textfield->setDecimals (decimals);
//	textfield->setVisibleColumns (doubleMaxLength.size ( ));
	// +1 : séparateur décimal
	textfield->setVisibleColumns (
		decimals + doubleIntegerPartMaxLength + doubleExponentPartMaxLength + 1);

	return *textfield;
}	// QtNumericFieldsFactory::createDistanceTextField


QtDoubleTextField& QtNumericFieldsFactory::createRatioTextField (
						QWidget *parent, bool autoValidation, const char* name)
{
	QtDoubleTextField*	textfield	=
					new QtDoubleTextField (parent, autoValidation, name);

	CHECK_NULL_PTR_ERROR (textfield)
	textfield->setVisibleColumns (
			MgxNumeric::mgxDoubleScientificNotationCharMax);
	textfield->setNotation (QDoubleValidator::ScientificNotation);

	return *textfield;
}	// QtNumericFieldsFactory::createRatioTextField


QtDoubleTextField& QtNumericFieldsFactory::createAngleTextField (
						QWidget *parent, bool autoValidation, const char* name)
{
	QtDoubleTextField*	textfield	=
					new QtDoubleTextField (parent, autoValidation, name);

	CHECK_NULL_PTR_ERROR (textfield)
	textfield->setVisibleColumns (
							4 + MgxNumeric::mgxAngleDecimalNum);
	textfield->setDecimals (MgxNumeric::mgxAngleDecimalNum);
	textfield->setNotation (QDoubleValidator::StandardNotation);

	return *textfield;
}	// QtNumericFieldsFactory::createAngleTextField

}	// namespace QtComponents

}	// namespace Mgx3D
