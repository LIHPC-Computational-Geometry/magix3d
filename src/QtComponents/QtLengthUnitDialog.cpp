/**
 * \file        QtLengthUnitDialog.cpp
 * \author      Charles PIGNEROL
 * \date        30/09/2020
 */

#include "Utils/Common.h"
#include "QtComponents/QtLengthUnitDialog.h"
#include <QtUtil/QtErrorManagement.h>

#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <QtUtil/QtStringHelper.h>

#include <QBoxLayout>
#include <QLabel>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtLengthUnitDialog
// ===========================================================================

QtLengthUnitDialog::QtLengthUnitDialog (QWidget* parent, const string& appTitle, Unit::lengthUnit suggested)
	: QDialog (parent),
	  _buttonGroup (0), _umRadioButton (0), _mmRadioButton (0), _cmRadioButton (0), _mRadioButton (0), _undefinedRadioButton (0), _closurePanel (0)
{
	setModal(true);
	setWindowModality(Qt::WindowModal);
	setWindowTitle(appTitle.c_str());

	QBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(QtConfiguration::margin);
	layout->setSizeConstraint(QLayout::SetMinimumSize);
	setLayout(layout);

	_buttonGroup = new QButtonGroup(this);
	_buttonGroup->setExclusive(true);
	QLabel *label = new QLabel("Unité de longueur", this);
	layout->addWidget(label);
	layout->addSpacing(20);

	_mRadioButton = new QRadioButton("m", this);
	_buttonGroup->addButton(_mRadioButton, Unit::meter);
	layout->addWidget(_mRadioButton);
	_cmRadioButton = new QRadioButton("cm", this);
	_buttonGroup->addButton(_cmRadioButton, Unit::centimeter);
	layout->addWidget(_cmRadioButton);
	_mmRadioButton = new QRadioButton("mm", this);
	_buttonGroup->addButton(_mmRadioButton, Unit::milimeter);
	layout->addWidget(_mmRadioButton);
	QString name;
	name           = QtStringHelper::muMin() + "m";
	_umRadioButton = new QRadioButton(name, this);
	_buttonGroup->addButton(_umRadioButton, Unit::micron);
	layout->addWidget(_umRadioButton);
	_undefinedRadioButton = new QRadioButton("indéfini", this);
	_buttonGroup->addButton(_undefinedRadioButton, Unit::undefined);
	_undefinedRadioButton->setEnabled (false);
	layout->addWidget(_undefinedRadioButton);

	switch (suggested)
	{
		case Unit::meter        : _mRadioButton->setChecked (true); break;
		case Unit::centimeter   : _cmRadioButton->setChecked (true); break;
		case Unit::milimeter    : _mmRadioButton->setChecked (true); break;
		case Unit::micron       : _umRadioButton->setChecked (true); break;
		default                 : _undefinedRadioButton->setChecked (true);
	}   // switch (suggested)

	_closurePanel	= new QtDlgClosurePanel (this, false, "OK", "", "Annuler");
	layout->addWidget (_closurePanel);
	_closurePanel->setMinimumSize (_closurePanel->sizeHint ( ));
	connect (_closurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this, SLOT(accept ( )));
	connect (_closurePanel->getCancelButton ( ), SIGNAL(clicked ( )), this, SLOT(reject ( )));
	_closurePanel->getApplyButton ( )->setAutoDefault (false);
	_closurePanel->getApplyButton ( )->setDefault (false);
	_closurePanel->getCancelButton ( )->setAutoDefault (false);
	_closurePanel->getCancelButton ( )->setDefault (false);

	layout->activate ( );

}	// QtLengthUnitDialog::QtLengthUnitDialog


QtLengthUnitDialog::QtLengthUnitDialog (const QtLengthUnitDialog&)
	: QDialog (0), _buttonGroup (0), _umRadioButton (0), _mmRadioButton (0), _cmRadioButton (0), _mRadioButton (0), _undefinedRadioButton (0), _closurePanel (0)
{
	MGX_FORBIDDEN ("QtLengthUnitDialog copy constructor is not allowed.");
}	// QtLengthUnitDialog::QtLengthUnitDialog (const QtLengthUnitDialog&)


QtLengthUnitDialog& QtLengthUnitDialog::operator = (const QtLengthUnitDialog&)
{
	MGX_FORBIDDEN ("QtLengthUnitDialog assignment operator is not allowed.");
	return *this;
}	// QtLengthUnitDialog::QtLengthUnitDialog (const QtLengthUnitDialog&)


QtLengthUnitDialog::~QtLengthUnitDialog ( )
{
}	// QtLengthUnitDialog::~QtLengthUnitDialog


Unit::lengthUnit QtLengthUnitDialog::lengthUnit ( ) const
{
	CHECK_NULL_PTR_ERROR (_buttonGroup)
	return (Unit::lengthUnit)_buttonGroup->checkedId ( );
}   // QtLengthUnitDialog::lengthUnit


}	// namespace QtComponents

}	// namespace Mgx3D
