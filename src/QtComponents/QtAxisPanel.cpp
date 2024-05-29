#include "Internal/ContextIfc.h"

#include "QtComponents/QtAxisPanel.h"
#include "QtComponents/QtNumericFieldsFactory.h"

#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <TkUtil/Exception.h>

#include <assert.h>

#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QValidator> 
#include <QColorDialog>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::QtComponents;


QtAxisPanel::QtAxisPanel (QWidget* parent, const string& appTitle,
			const string& title, const AxisAttributes& parameters)
	: QtGroupBox (title.c_str ( ), parent),
	  _appTitle (appTitle), _titleTextField (0), _minTextField (0),
	  _maxTextField (0), _formatTextField (0), _visibleCheckBox (0),
	  _labelsCheckBox (0), 
	  _ticksCheckBox (0), _minorTicksCheckBox (0), 
	  _linesCheckBox (0), _color (parameters.color)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	setMargin (0);
	setSpacing (0);
	setContentsMargins (0, 0, 0, 0);
	layout->setSpacing (0);

	// 1ere ligne : titre, min, max :
	QtGroupBox*		groupBox	= new QtGroupBox (this);
	QHBoxLayout*	hLayout		= new QHBoxLayout (groupBox);
	layout->addWidget (groupBox);
	groupBox->setLayout (hLayout);
	groupBox->setMargin (0);
	groupBox->setSpacing (QtConfiguration::spacing);
	QLabel*	label	= new QLabel ("Titre :", groupBox);
	label->setMinimumSize (label->sizeHint ( ));
	hLayout->addWidget (label);
	_titleTextField	= new QtTextField (groupBox);
	_titleTextField->setText (parameters.title);
	_titleTextField->setMinimumSize (_titleTextField->sizeHint ( ));
	hLayout->addWidget (_titleTextField);
	label	= new QLabel ("  Min :", groupBox);
	label->setMinimumSize (label->sizeHint ( ));
	hLayout->addWidget (label);
	_minTextField	= &QtNumericFieldsFactory::createPositionTextField (groupBox);
	_minTextField->setText (QString::number (parameters.min));
//	_minTextField->setValidator (new QDoubleValidator (_minTextField));
//	_minTextField->setVisibleColumns ()
//	_minTextField->setMaxLength ();
	_minTextField->setFixedSize (_minTextField->sizeHint ( ));
	hLayout->addWidget (_minTextField);
	label	= new QLabel ("  Max :", groupBox);
	label->setMinimumSize (label->sizeHint ( ));
	hLayout->addWidget (label);
	_maxTextField	= &QtNumericFieldsFactory::createPositionTextField (groupBox);
	_maxTextField->setText (QString::number (parameters.max));
//	_maxTextField->setValidator (new QDoubleValidator (_maxTextField));
//	_maxTextField->setVisibleColumns ();
//	_maxTextField->setMaxLength ();
	_maxTextField->setFixedSize (_maxTextField->sizeHint ( ));
	hLayout->addWidget (_maxTextField);
	groupBox->adjustSize ( );

	// Seconde ligne : Format + couleur :
	groupBox	= new QtGroupBox (this);
	hLayout		= new QHBoxLayout (groupBox);
	layout->addWidget (groupBox);
	groupBox->setLayout (hLayout);
	groupBox->setMargin (0);
	groupBox->setSpacing (QtConfiguration::spacing);
	label	= new QLabel ("Format :", groupBox);
	label->setMinimumSize (label->sizeHint ( ));
	hLayout->addWidget (label);
	_formatTextField	= new QtTextField (groupBox);
	_formatTextField->setText (parameters.format);
	_formatTextField->setMinimumSize (_formatTextField->sizeHint ( ));
	hLayout->addWidget (_formatTextField);
	QPushButton*	colorButton	= new QPushButton ("Couleur ...", groupBox);
	colorButton->setFixedSize (colorButton->sizeHint ( ));
	connect (colorButton, SIGNAL(clicked ( )), this,
	         SLOT (setColorCallback ( )));
	hLayout->addWidget (colorButton);
	groupBox->adjustSize ( );

	// Troisieme ligne : visibilites de l'axe, du titre et des valeurs :
	groupBox	= new QtGroupBox (this);
	hLayout		= new QHBoxLayout (groupBox);
	layout->addWidget (groupBox);
	groupBox->setLayout (hLayout);
	groupBox->setMargin (0);
	groupBox->setSpacing (QtConfiguration::spacing);
	_visibleCheckBox	= new QCheckBox ("Visible", groupBox);
	hLayout->addWidget (_visibleCheckBox);
	_visibleCheckBox->setChecked (parameters.visible);
	_labelsCheckBox	= new QCheckBox (QString::fromUtf8("Intitulés"), groupBox);
	hLayout->addWidget (_labelsCheckBox);
	_labelsCheckBox->setChecked (parameters.displayLabels);
	groupBox->adjustSize ( );
	_ticksCheckBox	= new QCheckBox ("Graduations", groupBox);
	hLayout->addWidget (_ticksCheckBox);
	_ticksCheckBox->setChecked (parameters.displayTicks);
	_minorTicksCheckBox	= new QCheckBox ("Graduations mineures", groupBox);
	hLayout->addWidget (_minorTicksCheckBox);
	_minorTicksCheckBox->setChecked (parameters.displayMinorTicks);
	_linesCheckBox	= new QCheckBox ("Lignes", groupBox);
	hLayout->addWidget (_linesCheckBox);
	_linesCheckBox->setChecked (parameters.displayLines);
	groupBox->adjustSize ( );

	adjustSize ( );
}	// QtAxisPanel::QtAxisPanel


QtAxisPanel::QtAxisPanel (const QtAxisPanel&)
	: QtGroupBox (0), _color (0, 0, 0)
{
	assert (0 && "QtAxisPanel copy constructor is not allowed.");
}	// QtAxisPanel::QtAxisPanel


QtAxisPanel& QtAxisPanel::operator = (const QtAxisPanel&)
{
	assert (0 && "QtAxisPanel operator = is not allowed.");
	return *this;
}	// QtAxisPanel::QtAxisPanel


QtAxisPanel::~QtAxisPanel ( )
{
}	// QtAxisPanel::~QtAxisPanel


AxisAttributes QtAxisPanel::getParameters ( ) const
{
	AxisAttributes	parameters;

	parameters.visible				= isVisible ( );
	parameters.min					= getMin ( );
	parameters.max					= getMax ( );
	parameters.title				= getTitle ( );
	parameters.color				= getColor ( );
	parameters.displayLabels		= displayLabels ( );
	parameters.format				= getDisplayFormat ( );
	parameters.displayTicks			= displayTicks ( );
	parameters.displayMinorTicks	= displayMinorTicks ( );
	parameters.displayLines			= displayLines ( );
	
	return parameters;
}	// QtAxisPanel::getParameters


bool QtAxisPanel::isVisible ( ) const
{
	assert ((0 != _visibleCheckBox) && "QtAxisPanel::isVisible : null visible checkbox.");
	return _visibleCheckBox->isChecked ( );
}	// QtAxisPanel::isVisible


double QtAxisPanel::getMin ( ) const
{
	assert ((0 != _minTextField) && "QtAxisPanel::getMin : null min textfield.");
	bool	ok	= true;
	return _minTextField->text ( ).toDouble (&ok);
}	// QtAxisPanel::getMin


double QtAxisPanel::getMax ( ) const
{
	assert ((0 != _maxTextField) && "QtAxisPanel::getMax : null max textfield.");
	bool	ok	= true;
	return _maxTextField->text ( ).toDouble (&ok);
}	// QtAxisPanel::getMax


string QtAxisPanel::getTitle ( ) const
{
	assert ((0 != _titleTextField) && "QtAxisPanel::getTitle : null title textfield.");
	return _titleTextField->text ( ).toStdString ( );
}	// QtAxisPanel::getTitle


bool QtAxisPanel::displayLabels ( ) const
{
	assert ((0 != _labelsCheckBox) && "QtAxisPanel::displayLabels : null values checkbox.");
	return _labelsCheckBox->isChecked ( );
}	// QtAxisPanel::displayLabels


string QtAxisPanel::getDisplayFormat ( ) const
{
	assert ((0 != _formatTextField) && "QtAxisPanel::getDisplayFormat : null format textfield.");
	return _formatTextField->text ( ).toStdString ( );
}	// QtAxisPanel::getDisplayFormat


bool QtAxisPanel::displayTicks ( ) const
{
	assert ((0 != _ticksCheckBox) && "QtAxisPanel::displayTicks : null ticks checkbox.");
	return _ticksCheckBox->isChecked ( );
}	// QtAxisPanel::displayTicks


bool QtAxisPanel::displayMinorTicks ( ) const
{
	assert ((0 != _minorTicksCheckBox) && "QtAxisPanel::displayMinorTicks : null ticks checkbox.");
	return _minorTicksCheckBox->isChecked ( );
}	// QtAxisPanel::displayMinorTicks


bool QtAxisPanel::displayLines ( ) const
{
	assert ((0 != _linesCheckBox) && "QtAxisPanel::displayLines : null ticks checkbox.");
	return _linesCheckBox->isChecked ( );
}	// QtAxisPanel::displayLines



void QtAxisPanel::setColorCallback ( )
{
	QColor	defaultColor (_color.getRed ( ), 
			              _color.getGreen ( ),
			              _color.getBlue ( ));
	QColor	color	= QColorDialog::getColor (defaultColor, this, QString::fromUtf8 ("Couleur des axes"), QColorDialog::DontUseNativeDialog);
	if (false == color.isValid ( ))
		return;

	if (defaultColor == color)
		return;

	_color.setRed (color.red ( ));
	_color.setGreen (color.green ( ));
	_color.setBlue (color.blue ( ));
}	// QtAxisPanel::setColorCallback

