#include "Internal/ContextIfc.h"

#include "QtComponents/QtLandmarkPanel.h"

#include <QtUtil/QtConfiguration.h>
#include <TkUtil/Exception.h>

#include <assert.h>

#include <QLayout>


using namespace std;
using namespace TkUtil;


QtLandmarkPanel::QtLandmarkPanel (QWidget* parent, const string& title,
						const string& xTitle, const string& yTitle,
						const string& zTitle,
						const LandmarkAttributes& parameters)
	: QtGroupBox (title.c_str ( ), parent), 
	  _xPanel (0), _yPanel (0), _zPanel (0), _autoAdjustCheckBox (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	setMargin (0);	// QtConfiguration::margin);
	setSpacing (0);	// QtConfiguration::spacing);
	setContentsMargins (0, 0, 0, 0);
	layout->setSpacing (0);
	_xPanel	= new QtAxisPanel (this, title, xTitle, parameters.axis1);
	_xPanel->setMinimumSize (_xPanel->sizeHint ( ));
	layout->addWidget (_xPanel);
	_yPanel	= new QtAxisPanel (this, title, yTitle, parameters.axis2);
	_yPanel->setMinimumSize (_yPanel->sizeHint ( ));
	layout->addWidget (_yPanel);
	_zPanel	= new QtAxisPanel (this, title, zTitle, parameters.axis3);
	_zPanel->setMinimumSize (_zPanel->sizeHint ( ));
	layout->addWidget (_zPanel);
	_autoAdjustCheckBox	= new QCheckBox ("Ajustement automatique", this);
	_autoAdjustCheckBox->setChecked (parameters.autoAdjust);
	_autoAdjustCheckBox->setFixedSize (_autoAdjustCheckBox->sizeHint ( ));
	layout->addWidget (_autoAdjustCheckBox);

	adjustSize ( );
}	// QtLandmarkPanel::QtLandmarkPanel


QtLandmarkPanel::QtLandmarkPanel (const QtLandmarkPanel&)
	: QtGroupBox (0)
{
	assert (0 && "QtLandmarkPanel copy constructor is forbidden.");
}	// QtLandmarkPanel::QtLandmarkPanel (const QtLandmarkPanel&)


QtLandmarkPanel& QtLandmarkPanel::operator = (const QtLandmarkPanel&)
{
	assert (0 && "QtLandmarkPanel assignment operator is forbidden.");
	return *this;
}	// QtLandmarkPanel::operator =


QtLandmarkPanel::~QtLandmarkPanel ( )
{
}	// QtLandmarkPanel::~QtLandmarkPanel


LandmarkAttributes QtLandmarkPanel::getParameters ( ) const
{
	LandmarkAttributes	parameters;

	parameters.axis1		= xParameters ( );
	parameters.axis2		= yParameters ( );
	parameters.axis3		= zParameters ( );
	parameters.autoAdjust	= autoAdjust ( );

	return parameters;
}	// QtLandmarkPanel::xParameters


AxisAttributes QtLandmarkPanel::xParameters ( ) const
{
	assert ((0 != _xPanel) && "QtLandmarkPanel::xParameters : null x panel.");
	return _xPanel->getParameters ( );
}	// QtLandmarkPanel::xParameters


AxisAttributes QtLandmarkPanel::yParameters ( ) const
{
	assert ((0 != _yPanel) && "QtLandmarkPanel::yParameters : null y panel.");
	return _yPanel->getParameters ( );
}	// QtLandmarkPanel::yParameters


AxisAttributes QtLandmarkPanel::zParameters ( ) const
{
	assert ((0 != _zPanel) && "QtLandmarkPanel::zParameters : null z panel.");
	return _zPanel->getParameters ( );
}	// QtLandmarkPanel::zParameters


bool QtLandmarkPanel::autoAdjust ( ) const
{
	assert ((0 != _autoAdjustCheckBox) && "QtLandmarkPanel::autoAdjust : null autoadjust checkbox.");
	return _autoAdjustCheckBox->isChecked ( );
}	// QtLandmarkPanel::autoAdjust



