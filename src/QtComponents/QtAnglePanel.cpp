/**
 * \file		QtAnglePanel.cpp
 * \author		Charles PIGNEROL
 * \date		17/12/2012
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"
#include "QtComponents/QtAnglePanel.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "Utils/MgxNumeric.h"
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtObjectSignalBlocker.h>

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/NumericServices.h>

#include <assert.h>
#include <cmath>
#include <memory>

#include <QGridLayout>
#include <QLabel>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ============================================================================
//                          LA CLASSE QtAnglePanel
// ============================================================================


QtAnglePanel::QtAnglePanel (
		QWidget* parent, const string& title,
		unsigned short visibleColumns, unsigned short decimals,
		double angle, const string& unity, bool allowCustomizedValues)
	: QWidget (parent), 
	  _appName (title), _partLabel(0), _predefinedValuesComboBox (0), _angleTextField (0),
	  _predefinedValues ( ), _allowCustomizedValues (allowCustomizedValues)
{
	QGridLayout*	layout	= new QGridLayout (this);
	setLayout (layout);
	layout->setContentsMargins (
				Resources::instance ( )._margin.getValue ( ),
				Resources::instance ( )._margin.getValue ( ),
				Resources::instance ( )._margin.getValue ( ),
				Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));

	// Les valeurs prédéfinies :
	_partLabel	= new QLabel ("Portion", this);
	layout->addWidget (_partLabel, 0, 0);
	_predefinedValuesComboBox	= new QComboBox (this);
	layout->addWidget (_predefinedValuesComboBox, 0, 1);
	connect (_predefinedValuesComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (predefinedValueCallback (int)));

	// Les valeurs personnalisées :
	QLabel* label	= new QLabel ("Angle", this);
	layout->addWidget (label, 1, 0);
	_angleTextField	= &QtNumericFieldsFactory::createAngleTextField (this);
	_angleTextField->setValue (angle);
	layout->addWidget (_angleTextField, 1, 1);
//	_angleTextField->setNotation (QDoubleValidator::StandardNotation);
//	_angleTextField->setDecimals (decimals);
//	_angleTextField->setVisibleColumns (visibleColumns);
	connect (_angleTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (angleModifiedCallback ( )));
	label	= new QLabel (unity.c_str ( ), this);
	layout->addWidget (label, 1, 2);
	if (true == allowCustomizedValues)
	{
		QtObjectSignalBlocker	blocker (_predefinedValuesComboBox);
		pair <string, double>	customValue ("Personnalisé", angle);
		_predefinedValues.push_back (customValue);
		_predefinedValuesComboBox->addItem (QString::fromUtf8(customValue.first.c_str ( )));
		_angleTextField->setValue (customValue.second);
	}	// if (true == allowCustomizedValues)
	else
		_angleTextField->setValue (angle);
}	// QtAnglePanel::QtAnglePanel


QtAnglePanel::QtAnglePanel (const QtAnglePanel&)
	: QWidget (0),
	  _partLabel(0), _predefinedValuesComboBox (0), _angleTextField (0),
	  _predefinedValues ( ), _allowCustomizedValues (false)
{
	assert (0 && "QtAnglePanel copy constructor is forbidden.");
}	// QtAnglePanel::QtAnglePanel (const QtAnglePanel&)


QtAnglePanel& QtAnglePanel::operator = (const QtAnglePanel&)
{
	assert (0 && "QtAnglePanel assignment operator is forbidden.");
	return *this;
}	// QtAnglePanel::operator =


QtAnglePanel::~QtAnglePanel ( )
{
}	// QtAnglePanel::~QtAnglePanel


void QtAnglePanel::hidePartComboBox()
{
	_partLabel->hide();
	_predefinedValuesComboBox->hide();
}

double QtAnglePanel::getAngle ( ) const
{
	CHECK_NULL_PTR_ERROR (_predefinedValuesComboBox)
	CHECK_NULL_PTR_ERROR (_angleTextField)
	const int	index	= _predefinedValuesComboBox->currentIndex ( );

	if (0 == index)
		return _angleTextField->getValue ( );

	if ((index < 0) || (index >= _predefinedValues.size ( )))
	{
		TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);
		error << "Index invalide : " << (long)index
		      << ". Domaine possible : 0 - "
		      << (unsigned long)(_predefinedValues.size ( ) - 1);
		INTERNAL_ERROR (exc, error, "QtAnglePanel::getAngle")
		throw exc;
	}	// if ((index < 0) || (index >= _predefinedValues.size ( )))

	return _predefinedValues [index].second;
}	// QtAnglePanel::getAngle


//unsigned short QtAnglePanel::getCircleFraction ( ) const
//{
//	const double	angle	= getAngle ( );
//	const double	fraction=
//		std::fabs (angle - 360.) < NumericServices::doubleMachEpsilon ?
//		1. : 360. / angle;
//	const string	txt		= NumericConversions::toStr (fraction);
//
//	unsigned long	ul		= NumericConversions::strToULong (txt);
//	if (NumericServices::unsignedShortMachMax >= ul)
//		return (unsigned short)ul;
//
//	TkUtil::UTF8String	err (TkUtil::Charset::UTF_8);
//	error << angle << " est une portion trop grande (maximum admissible : "
//	      << NumericServices::unsignedShortMachMax << ")";
//	throw Exception (error);
//}	// QtAnglePanel::getCircleFraction


Utils::Portion::Type QtAnglePanel::getPortion ( ) const
{
	const double	angle	= getAngle ( );

	if (true == Utils::Math::MgxNumeric::isNearlyZero (
						angle - Utils::Portion::HUITIEME,
						Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon))
		return Utils::Portion::HUITIEME;
	if (true == Utils::Math::MgxNumeric::isNearlyZero (
						angle - Utils::Portion::QUART,
						Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon))
		return Utils::Portion::QUART;
	if (true == Utils::Math::MgxNumeric::isNearlyZero (
						angle - Utils::Portion::TIERS,
						Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon))
		return Utils::Portion::TIERS;
	if (true == Utils::Math::MgxNumeric::isNearlyZero (
						angle - Utils::Portion::DEMI,
						Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon))
		return Utils::Portion::DEMI;
	if (true == Utils::Math::MgxNumeric::isNearlyZero (
						angle - Utils::Portion::TROIS_QUARTS,
						Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon))
		return Utils::Portion::TROIS_QUARTS;
	if (true == Utils::Math::MgxNumeric::isNearlyZero (
						angle - Utils::Portion::ENTIER,
						Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon))
		return Utils::Portion::ENTIER;
	if (true == Utils::Math::MgxNumeric::isNearlyZero (
						angle - Utils::Portion::CINQUIEME,
						Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon))
		return Utils::Portion::CINQUIEME;
	if (true == Utils::Math::MgxNumeric::isNearlyZero (
						angle - Utils::Portion::SIXIEME,
						Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon))
		return Utils::Portion::SIXIEME;

	TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);
	error << angle << " n'est pas une portion d'angle au sens Magix 3D.";
	throw Exception (error);
}	// QtAnglePanel::getPortion


void QtAnglePanel::setRange (double min, double max)
{
	CHECK_NULL_PTR_ERROR (_angleTextField)
	_angleTextField->setRange (min, max);
}	// QtAnglePanel::setRange


void QtAnglePanel::addPredefinedValue (const string& name, double value)
{
	CHECK_NULL_PTR_ERROR (_predefinedValuesComboBox)
	_predefinedValues.push_back (pair <string, double> (name, value));
	_predefinedValuesComboBox->addItem (QString::fromUtf8(name.c_str ( )));
}	// QtAnglePanel::addPredefinedValue


void QtAnglePanel::setPredefinedValue (const string& name)
{
	CHECK_NULL_PTR_ERROR (_predefinedValuesComboBox)
	size_t	index	= 0;
	for (vector< pair <string, double> >::const_iterator it =
			_predefinedValues.begin ( );
			_predefinedValues.end ( ) != it; it++, index++)
	{
		if ((*it).first == name)
		{
			_predefinedValuesComboBox->setCurrentIndex (index);
			return;
		}	// if ((*it).first == name)
	}	// for (vector< pair <string, double> >::const_iterator ...

	TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);
	error << "QtAnglePanel::setPredefinedValue. Absence de valeur prédéfinie "
	      << "de nom \"" << name << "\".";
	throw Exception (error);
}	// QtAnglePanel::setPredefinedValue


void QtAnglePanel::predefinedValueCallback (int index)
{
	CHECK_NULL_PTR_ERROR (_angleTextField)
	CHECK_NULL_PTR_ERROR (_predefinedValuesComboBox)
	bool	custom	= (true  == _allowCustomizedValues) && (0 == index) ?
					  true : false;
	_angleTextField->setEnabled (custom);
	try
	{
		if ((index < 0) || (index >= _predefinedValues.size ( )))
		{
			TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);
			error << "Index invalide : " << (long)index
			      << ". Domaine possible : 0 - "
			      << (unsigned long)(_predefinedValues.size ( ) - 1);
			INTERNAL_ERROR (exc, error, "QtAnglePanel::getAngle")
			throw exc;
		}	// if ((index < 0) || (index >= _predefinedValues.size ( )))
		_angleTextField->setValue (_predefinedValues [index].second);
	}
	catch (const Exception& exc)
	{
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Angle invalide : " << exc.getFullMessage ( );
		QtMessageBox::displayErrorMessage (this, _appName, message);
	}
	catch (exception& e)
	{
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Angle invalide : " << e.what ( );
		QtMessageBox::displayErrorMessage (this, _appName, message);
	}
	catch (...)
	{
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "Angle invalide : erreur non documentée.";
		QtMessageBox::displayErrorMessage (this, _appName, message);
	}

	try
	{
		emit angleModified (getAngle ( ));
	}
	catch (...)
	{
	}
}	// QtAnglePanel::predefinedValueCallback


void QtAnglePanel::angleModifiedCallback ( )
{
	try
	{
		emit angleModified (getAngle ( ));
	}
	catch (...)
	{
	}
}	// QtAnglePanel::angleModifiedCallback


const vector< pair<string, double> >& QtAnglePanel::eightthToWhole ( )
{
	static vector< pair<string, double> >	values;

	if (0 == values.size ( ))
	{
		values.push_back (pair<string, double>("Entier", 360.));
		values.push_back (pair<string, double>("Demi", 180.));
		values.push_back (pair<string, double>("Tiers", 120.));
		values.push_back (pair<string, double>("Quart", 90.));
		values.push_back (pair<string, double>("Cinquième", 72.));
		values.push_back (pair<string, double>("Sixième", 60.));
		values.push_back (pair<string, double>("Huitième", 45.));
	}	// if (0 == values.size ( ))

	return values;
}	// QtAnglePanel::eightthToWhole


const vector< pair<string, double> >& QtAnglePanel::eightthToHalf ( )
{
	static vector< pair<string, double> >	values;

	if (0 == values.size ( ))
	{
		values.push_back (pair<string, double>("Demi", 180.));
		values.push_back (pair<string, double>("Tiers", 120.));
		values.push_back (pair<string, double>("Quart", 90.));
		values.push_back (pair<string, double>("Sixième", 60.));
		values.push_back (pair<string, double>("Huitième", 45.));
	}	// if (0 == values.size ( ))

	return values;
}	// QtAnglePanel::eightthToHalf


}	// namespace QtComponents

}	// namespace Mgx3D

