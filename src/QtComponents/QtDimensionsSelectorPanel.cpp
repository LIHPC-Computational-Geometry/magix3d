/**
 * \file        QtDimensionsSelectorPanel.cpp
 * \author      Charles PIGNEROL
 * \date        21/11/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtDimensionsSelectorPanel.h"
#include <QtUtil/QtErrorManagement.h>

#include <TkUtil/MemoryError.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtObjectSignalBlocker.h>

#include <QHBoxLayout>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::QtComponents;


namespace Mgx3D
{

namespace QtComponents
{

// ===========================================================================
//                        LA CLASSE QtDimensionsSelectorPanel
// ===========================================================================


QtDimensionsSelectorPanel::QtDimensionsSelectorPanel (
	QWidget* parent, SelectionManagerIfc::DIM allowedDimensions,
	SelectionManagerIfc::DIM defaultDimensions, bool allowMultipleDimensions)
	: QWidget (parent), _buttonGroup (0),
	  _d0CheckBox (0), _d1CheckBox (0), _d2CheckBox (0), _d3CheckBox (0)
{
	_buttonGroup	= new QButtonGroup (this);
	_buttonGroup->setExclusive (!allowMultipleDimensions);

	QVBoxLayout*	layout	= new QVBoxLayout (this);

	if (0 != (allowedDimensions & SelectionManagerIfc::D0))
	{
		_d0CheckBox	= new QCheckBox ("Dim0", this);
		layout->addWidget (_d0CheckBox);
		_d0CheckBox->setToolTip(QString::fromUtf8("Entités de dimension 0: points, sommets"));
		if (0 != (defaultDimensions & SelectionManagerIfc::D0))
			_d0CheckBox->setCheckState (Qt::Checked);
		connect (_d0CheckBox, SIGNAL (stateChanged (int)), this,
		         SLOT (dimensionsModifiedCallback ( )));
		_buttonGroup->addButton (_d0CheckBox);
	}	// if (0 != (allowedDimensions & SelectionManagerIfc::D0))
	if (0 != (allowedDimensions & SelectionManagerIfc::D1))
	{
		_d1CheckBox	= new QCheckBox ("Dim1", this);
		layout->addWidget (_d1CheckBox);
		_d1CheckBox->setToolTip(QString::fromUtf8("Entités de dimension 1: courbes, arêtes"));
		connect (_d1CheckBox, SIGNAL (stateChanged (int)), this,
		         SLOT (dimensionsModifiedCallback ( )));
		if (0 != (defaultDimensions & SelectionManagerIfc::D1))
			_d1CheckBox->setCheckState (Qt::Checked);
		_buttonGroup->addButton (_d1CheckBox);
	}	// if (0 != (allowedDimensions & SelectionManagerIfc::D1))
	if (0 != (allowedDimensions & SelectionManagerIfc::D2))
	{
		_d2CheckBox	= new QCheckBox ("Dim2", this);
		layout->addWidget (_d2CheckBox);
		_d2CheckBox->setToolTip(QString::fromUtf8("Entités de dimension 2: surfaces, faces"));
		connect (_d2CheckBox, SIGNAL (stateChanged (int)), this,
		         SLOT (dimensionsModifiedCallback ( )));
		if (0 != (defaultDimensions & SelectionManagerIfc::D2))
			_d2CheckBox->setCheckState (Qt::Checked);
		_buttonGroup->addButton (_d2CheckBox);
	}	// if (0 != (allowedDimensions & SelectionManagerIfc::D2))
	if (0 != (allowedDimensions & SelectionManagerIfc::D3))
	{
		_d3CheckBox	= new QCheckBox ("Dim3", this);
		layout->addWidget (_d3CheckBox);
		_d3CheckBox->setToolTip(QString::fromUtf8("Entités de dimension 3: volumes, blocs"));
		connect (_d3CheckBox, SIGNAL (stateChanged (int)), this,
		         SLOT (dimensionsModifiedCallback ( )));
		if (0 != (defaultDimensions & SelectionManagerIfc::D3))
			_d3CheckBox->setCheckState (Qt::Checked);
		_buttonGroup->addButton (_d3CheckBox);
	}	// if (0 != (allowedDimensions & SelectionManagerIfc::D3))
}	// QtDimensionsSelectorPanel::QtDimensionsSelectorPanel


QtDimensionsSelectorPanel::QtDimensionsSelectorPanel (
											const QtDimensionsSelectorPanel&)
	: QWidget (0), _buttonGroup (0),
	  _d0CheckBox (0), _d1CheckBox (0), _d2CheckBox (0), _d3CheckBox (0)
{
	MGX_FORBIDDEN ("QtDimensionsSelectorPanel copy constructor is not allowed.");
}	// QtDimensionsSelectorPanel::QtDimensionsSelectorPanel (const QtDimensionsSelectorPanel&)


QtDimensionsSelectorPanel& QtDimensionsSelectorPanel::operator = (
											const QtDimensionsSelectorPanel&)
{
	MGX_FORBIDDEN ("QtDimensionsSelectorPanel assignment operator is not allowed.");
	return *this;
}	// QtDimensionsSelectorPanel::QtDimensionsSelectorPanel (const QtDimensionsSelectorPanel&)


QtDimensionsSelectorPanel::~QtDimensionsSelectorPanel ( )
{
}	// QtDimensionsSelectorPanel::~QtDimensionsSelectorPanel


bool QtDimensionsSelectorPanel::allowMultipleDimensions ( ) const
{
	CHECK_NULL_PTR_ERROR (_buttonGroup)
	return !_buttonGroup->exclusive ( );
}	// QtDimensionsSelectorPanel::allowMultipleDimensions


SelectionManagerIfc::DIM QtDimensionsSelectorPanel::getDimensions ( ) const
{
	SelectionManagerIfc::DIM	dimensions	= SelectionManagerIfc::NO_DIM;

	if ((0 != _d0CheckBox) && (Qt::Checked == _d0CheckBox->checkState ( )) &&
	    (true == _d0CheckBox->isEnabled ( )))
		dimensions	=
			(SelectionManagerIfc::DIM)(dimensions | SelectionManagerIfc::D0);
	if ((0 != _d1CheckBox) && (Qt::Checked == _d1CheckBox->checkState ( )) &&
	    (true == _d1CheckBox->isEnabled ( )))
		dimensions	=
			(SelectionManagerIfc::DIM)(dimensions | SelectionManagerIfc::D1);
	if ((0 != _d2CheckBox) && (Qt::Checked == _d2CheckBox->checkState ( )) &&
	    (true == _d2CheckBox->isEnabled ( )))
		dimensions	=
			(SelectionManagerIfc::DIM)(dimensions | SelectionManagerIfc::D2);
	if ((0 != _d3CheckBox) && (Qt::Checked == _d3CheckBox->checkState ( )) &&
	    (true == _d3CheckBox->isEnabled ( )))
		dimensions	=
			(SelectionManagerIfc::DIM)(dimensions | SelectionManagerIfc::D3);

	return dimensions;
}	// QtDimensionsSelectorPanel::getDimensions


void QtDimensionsSelectorPanel::setDimensions (SelectionManagerIfc::DIM dims)
{
	QtObjectSignalBlocker	blocker0 (_d0CheckBox), blocker1 (_d1CheckBox),
							blocker2 (_d2CheckBox), blocker3 (_d3CheckBox);

	if (0 != _d0CheckBox)
		_d0CheckBox->setCheckState (
			0 != (SelectionManagerIfc::D0 & dims) ? Qt::Checked:Qt::Unchecked);
	if (0 != _d1CheckBox)
		_d1CheckBox->setCheckState (
			0 != (SelectionManagerIfc::D1 & dims) ? Qt::Checked:Qt::Unchecked);
	if (0 != _d2CheckBox)
		_d2CheckBox->setCheckState (
			0 != (SelectionManagerIfc::D2 & dims) ? Qt::Checked:Qt::Unchecked);
	if (0 != _d3CheckBox)
		_d3CheckBox->setCheckState (
			0 != (SelectionManagerIfc::D3 & dims) ? Qt::Checked:Qt::Unchecked);
}	// QtDimensionsSelectorPanel::setDimensions


SelectionManagerIfc::DIM
					QtDimensionsSelectorPanel::getAllowedDimensions ( ) const
{
	SelectionManagerIfc::DIM	dimensions	= SelectionManagerIfc::NO_DIM;

	if ((0 != _d0CheckBox) && (true == _d0CheckBox->isEnabled ( )))
		dimensions	=
			(SelectionManagerIfc::DIM)(dimensions | SelectionManagerIfc::D0);
	if ((0 != _d1CheckBox) && (true == _d1CheckBox->isEnabled ( )))
		dimensions	=
			(SelectionManagerIfc::DIM)(dimensions | SelectionManagerIfc::D1);
	if ((0 != _d2CheckBox) && (true == _d2CheckBox->isEnabled ( )))
		dimensions	=
			(SelectionManagerIfc::DIM)(dimensions | SelectionManagerIfc::D2);
	if ((0 != _d3CheckBox) && (true == _d3CheckBox->isEnabled ( )))
		dimensions	=
			(SelectionManagerIfc::DIM)(dimensions | SelectionManagerIfc::D3);

	return dimensions;
}	// QtDimensionsSelectorPanel::getAllowedDimensions


void QtDimensionsSelectorPanel::setAllowedDimensions (
											SelectionManagerIfc::DIM dimensions)
{
	if (0 != _d0CheckBox)
		_d0CheckBox->setEnabled (
				0 != (dimensions & SelectionManagerIfc::D0) ? true : false);
	if (0 != _d1CheckBox)
		_d1CheckBox->setEnabled (
				0 != (dimensions & SelectionManagerIfc::D1) ? true : false);
	if (0 != _d2CheckBox)
		_d2CheckBox->setEnabled (
				0 != (dimensions & SelectionManagerIfc::D2) ? true : false);
	if (0 != _d3CheckBox)
		_d3CheckBox->setEnabled (
				0 != (dimensions & SelectionManagerIfc::D3) ? true : false);
}	// QtDimensionsSelectorPanel::setAllowedDimensions


void QtDimensionsSelectorPanel::dimensionsModifiedCallback ( )
{
	emit (dimensionsModified ( ));
}	// QtDimensionsSelectorPanel::dimensionsModifiedCallback



// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
