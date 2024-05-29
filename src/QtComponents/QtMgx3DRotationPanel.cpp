/**
 * \file		QtMgx3DRotationPanel.cpp
 * \author		Charles PIGNEROL
 * \date		22/05/2014
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DRotationPanel.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DEdgePanel.h"
#include "QtComponents/Qt2VerticiesPanel.h"
#include "QtComponents/Qt3VerticiesPanel.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/RotX.h"
#include "Utils/RotY.h"
#include "Utils/RotZ.h"

#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtObjectSignalBlocker.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <QLayout>
#include <QLabel>
#include <QValidator>

#include <iostream>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;


namespace Mgx3D
{

namespace QtComponents
{

QtMgx3DRotationPanel::QtMgx3DRotationPanel (
		QWidget* parent, const string& title, bool editable,
		const vector< pair<string, double> >& predefinedValues,
		unsigned short defaultValue,
		QtMgx3DMainWindow* mainWindow)
	: QWidget (parent), 
	  _anglePanel (0), _axisButtonGroup (0),
	  _xAxisCheckBox (0), _yAxisCheckBox (0), _zAxisCheckBox (0),
	  _anyAxisCheckBox (0),
	  _anyAxisDefinitionModeComboBox (0), _anyAxisDefinitionParentWidget (0),
	  _anyAxisDefinitionWidgets ( )
{
	setEnabled (editable);
	QVBoxLayout*	layout	= new QVBoxLayout (this);

	QLabel*	label	= 0;
	if (false == title.empty ( ))
	{
		label	= new QLabel (title.c_str ( ), this);
		layout->addWidget (label);
	}	// if (false == title.empty ( ))

	// Angle :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	hlayout->setSpacing (5);
	hlayout->setContentsMargins (0, 0, 0, 0);
	layout->addLayout (hlayout);
	_anglePanel		= new QtAnglePanel (
				this, "Magix 3D", 4 + MgxNumeric::mgxAngleDecimalNum,
				MgxNumeric::mgxAngleDecimalNum, 90., "degrés");
	for (vector< pair<string, double> >::const_iterator
		itpv = predefinedValues.begin ( ); predefinedValues.end ( ) != itpv;
		itpv++)
		_anglePanel->addPredefinedValue ((*itpv).first, (*itpv).second);
	if (defaultValue < predefinedValues.size ( ))
		_anglePanel->setPredefinedValue (predefinedValues [defaultValue].first);
	hlayout->addWidget (_anglePanel);
	hlayout->addStretch (1);
	connect (_anglePanel, SIGNAL (angleModified (double)), this,
	         SLOT (rotationModifiedCallback ( )));

	// Axe :
	QVBoxLayout*	vlayout	= new QVBoxLayout ( );
	layout->addLayout (vlayout);
	_axisButtonGroup	= new QButtonGroup (this);
	_axisButtonGroup->setExclusive (true);
	_xAxisCheckBox	= new QCheckBox ("Axe des X", this);
	_xAxisCheckBox->setCheckState (Qt::Checked);
	_axisButtonGroup->addButton (_xAxisCheckBox);
	vlayout->addWidget (_xAxisCheckBox);
	_yAxisCheckBox	= new QCheckBox ("Axe des Y", this);
	_axisButtonGroup->addButton (_yAxisCheckBox);
	vlayout->addWidget (_yAxisCheckBox);
	_zAxisCheckBox	= new QCheckBox ("Axe des Z", this);
	_axisButtonGroup->addButton (_zAxisCheckBox);
	vlayout->addWidget (_zAxisCheckBox);
	_anyAxisCheckBox	= new QCheckBox ("Autre axe", this);
	_axisButtonGroup->addButton (_anyAxisCheckBox);
	hlayout	= new QHBoxLayout (0);
	vlayout->addLayout (hlayout);
	hlayout->addWidget (_anyAxisCheckBox);
	_anyAxisDefinitionModeComboBox	= new QComboBox (this);
	_anyAxisDefinitionModeComboBox->addItem (QString::fromUtf8("Par sélection d'une courbe/arête"));
	_anyAxisDefinitionModeComboBox->addItem (QString::fromUtf8("Par sélection de 2 points/sommets"));
	_anyAxisDefinitionModeComboBox->addItem (QString::fromUtf8("Par sélection de 3 points/sommets"));
	connect (_anyAxisDefinitionModeComboBox, SIGNAL (currentIndexChanged(int)),
	         this, SLOT (axeModeCallback ( )));
	connect (_anyAxisDefinitionModeComboBox, SIGNAL (currentIndexChanged(int)),
	         this, SLOT (axeModifiedCallback ( )));
	hlayout->addWidget (_anyAxisDefinitionModeComboBox);
	hlayout->addStretch (2);
	connect (_axisButtonGroup, SIGNAL (buttonClicked (int)), this,
	         SLOT (axeModifiedCallback ( )));
	connect (_axisButtonGroup, SIGNAL (buttonClicked (int)), this,
	         SLOT (rotationModifiedCallback ( )));
	// Définition de l'axe de rotation à partir d'un segment/d'une arête :
	_anyAxisDefinitionParentWidget	= new QWidget (this);
	vlayout->addWidget (_anyAxisDefinitionParentWidget);
	hlayout	= new QHBoxLayout (0);
	_anyAxisDefinitionParentWidget->setLayout (hlayout);
	QtMgx3DEdgePanel*	edgePanel	=
		new QtMgx3DEdgePanel (_anyAxisDefinitionParentWidget,
				"", true, "", mainWindow, FilterEntity::AllEdges);
	hlayout->addWidget (edgePanel);
	connect (edgePanel, SIGNAL (selectionModified (QString)), this,
	         SLOT (rotationModifiedCallback ( )));
	edgePanel->hide ( );
	_anyAxisDefinitionWidgets.push_back (edgePanel);
	// Définition de l'axe de rotation à partir de 2 points :
	FilterEntity::objectType	types	= (FilterEntity::objectType)(
						FilterEntity::GeomVertex | FilterEntity::TopoVertex);
	Qt2VerticiesPanel*	verticesPanel	=
		new Qt2VerticiesPanel (_anyAxisDefinitionParentWidget,
				"Magix 3D", *mainWindow, types);
	hlayout->addWidget (verticesPanel);
	connect (verticesPanel, SIGNAL (pointAddedToSelection (QString)), this,
	         SLOT (rotationModifiedCallback ( )));
	connect (verticesPanel, SIGNAL (pointRemovedFromSelection (QString)), this,
	         SLOT (rotationModifiedCallback ( )));
	verticesPanel->hide ( );
	_anyAxisDefinitionWidgets.push_back (verticesPanel);
	// Définition de la rotation à partir de 3 points :
	Qt3VerticiesPanel*	vertices3Panel	=
		new Qt3VerticiesPanel (_anyAxisDefinitionParentWidget,
				"Magix 3D", *mainWindow, types, true);
	hlayout->addWidget (vertices3Panel);
	connect (vertices3Panel, SIGNAL (pointAddedToSelection (QString)), this,
	         SLOT (rotationModifiedCallback ( )));
	connect (vertices3Panel, SIGNAL (pointRemovedFromSelection (QString)), this,
	         SLOT (rotationModifiedCallback ( )));
	vertices3Panel->hide ( );
	_anyAxisDefinitionWidgets.push_back (vertices3Panel);

	axeModeCallback ( );
	axeModifiedCallback ( );
}	// QtMgx3DRotationPanel::QtMgx3DRotationPanel


QtMgx3DRotationPanel::QtMgx3DRotationPanel (const QtMgx3DRotationPanel&)
	: QWidget (0), _anglePanel (0), _axisButtonGroup (0),
	  _xAxisCheckBox (0), _yAxisCheckBox (0), _zAxisCheckBox (0),
	  _anyAxisCheckBox (0), _anyAxisDefinitionModeComboBox (0),
	  _anyAxisDefinitionParentWidget (0), _anyAxisDefinitionWidgets ( )
{
	MGX_FORBIDDEN ("QtMgx3DRotationPanel copy constructor is forbidden.")
}	// QtMgx3DRotationPanel::QtMgx3DRotationPanel (const QtMgx3DRotationPanel&)


QtMgx3DRotationPanel& QtMgx3DRotationPanel::operator = (
													const QtMgx3DRotationPanel&)
{
	MGX_FORBIDDEN ("QtMgx3DRotationPanel assignment operator is forbidden.")
	return *this;
}	// QtMgx3DRotationPanel::operator =


QtMgx3DRotationPanel::~QtMgx3DRotationPanel ( )
{
}	// QtMgx3DRotationPanel::~QtMgx3DRotationPanel


Math::Rotation QtMgx3DRotationPanel::getRotation ( ) const
{
	CHECK_NULL_PTR_ERROR (_axisButtonGroup)
	CHECK_NULL_PTR_ERROR (_xAxisCheckBox)
	CHECK_NULL_PTR_ERROR (_yAxisCheckBox)
	CHECK_NULL_PTR_ERROR (_zAxisCheckBox)
	CHECK_NULL_PTR_ERROR (_anglePanel)
	CHECK_NULL_PTR_ERROR (_anyAxisDefinitionModeComboBox)
	const double	angle	= _anglePanel->getAngle ( );

	if (_xAxisCheckBox == _axisButtonGroup->checkedButton ( ))
		return Math::RotX (angle);
	else if (_yAxisCheckBox == _axisButtonGroup->checkedButton ( ))
		return Math::RotY (angle);
	else if (_zAxisCheckBox == _axisButtonGroup->checkedButton ( ))
		return Math::RotZ (angle);

	// Axe de rotation quelconque :
	const int	current	= _anyAxisDefinitionModeComboBox->currentIndex ( );
	if (current >= _anyAxisDefinitionWidgets.size ( ))
	{
		INTERNAL_ERROR (exc, "Plus de modes de saisie que de panneaux.", "QtMgx3DRotationPanel::getRotation")
		throw exc;
	}	// if (current >= _anyAxisDefinitionWidgets.size ( ))
	QtMgx3DEdgePanel*	edgePanel	=
			dynamic_cast<QtMgx3DEdgePanel*>(_anyAxisDefinitionWidgets [current]);
	Qt2VerticiesPanel*	verticesPanel	=
			dynamic_cast<Qt2VerticiesPanel*>(_anyAxisDefinitionWidgets [current]);
	Qt3VerticiesPanel*	vertices3Panel	=
			dynamic_cast<Qt3VerticiesPanel*>(_anyAxisDefinitionWidgets [current]);
	Math::Point	p1, p2, p3;
	Math::Rotation	rotation (Math::Rotation::X, 0.);
	if (0 != edgePanel)
	{
		edgePanel->getPoints (p1, p2);
		rotation	= Rotation (p1, p2, angle);
	}
	else if (0 != verticesPanel)
	{
		verticesPanel->getPoints (p1, p2);
		rotation	= Rotation (p1, p2, angle);
	}
	else if (0 != vertices3Panel)
	{
		// ordre des points pour vertices3Panel : P1, P2, Centre
		vertices3Panel->getPoints (p1, p2, p3);
		rotation	= Rotation (p3, p1, p2);
	}
	else
	{
		INTERNAL_ERROR (exc, "Pas de panneau correspondant au mode de sélection courant de l'axe de rotation.", "QtMgx3DRotationPanel::getRotation")
		throw exc;
	}

//	Math::Rotation	rotation (p1, p2, angle);

	return rotation;
}	// QtMgx3DRotationPanel::getRotation


void QtMgx3DRotationPanel::reset ( )
{
	for (vector<QWidget*>::iterator it = _anyAxisDefinitionWidgets.begin ( );
	     _anyAxisDefinitionWidgets.end ( ) != it; it++)
	{
		QtMgx3DEdgePanel*	edgePanel	= dynamic_cast<QtMgx3DEdgePanel*>(*it);
		Qt2VerticiesPanel*	verticesPanel	= dynamic_cast<Qt2VerticiesPanel*>(*it);
		Qt3VerticiesPanel*	vertices3Panel	= dynamic_cast<Qt3VerticiesPanel*>(*it);
		if (0 != edgePanel)
			edgePanel->reset ( );
		else if (0 != verticesPanel)
			verticesPanel->reset ( );
		else if (0 != vertices3Panel)
			vertices3Panel->reset ( );
	}	// for (vector<QWidget*>::iterator it = 
}	// QtMgx3DRotationPanel::reset


void QtMgx3DRotationPanel::stopSelection ( )
{
	for (vector<QWidget*>::iterator it = _anyAxisDefinitionWidgets.begin ( );
	     _anyAxisDefinitionWidgets.end ( ) != it; it++)
	{
		QtMgx3DEdgePanel*	edgePanel	= dynamic_cast<QtMgx3DEdgePanel*>(*it);
		Qt2VerticiesPanel*	verticesPanel	= dynamic_cast<Qt2VerticiesPanel*>(*it);
		Qt3VerticiesPanel*	vertices3Panel	= dynamic_cast<Qt3VerticiesPanel*>(*it);
		if (0 != edgePanel)
			edgePanel->stopSelection ( );
		else if (0 != verticesPanel)
			verticesPanel->stopSelection ( );
		else if (0 != vertices3Panel)
			vertices3Panel->stopSelection ( );
	}	// for (vector<QWidget*>::iterator it = 
}	// QtMgx3DRotationPanel::stopSelection


bool QtMgx3DRotationPanel::actualizeGui (bool removeDestroyed)
{
	bool	modified	= false;

	for (vector<QWidget*>::iterator it = _anyAxisDefinitionWidgets.begin ( );
	     _anyAxisDefinitionWidgets.end ( ) != it; it++)
	{
		QtMgx3DEdgePanel*	edgePanel	= dynamic_cast<QtMgx3DEdgePanel*>(*it);
		Qt2VerticiesPanel*	verticesPanel	= dynamic_cast<Qt2VerticiesPanel*>(*it);
		Qt3VerticiesPanel*	vertices3Panel	= dynamic_cast<Qt3VerticiesPanel*>(*it);
		bool	tmp	= false;
		if (0 != edgePanel)
			tmp	= edgePanel->actualizeGui (removeDestroyed);
		else if (0 != verticesPanel)
			tmp	= verticesPanel->actualizeGui (removeDestroyed);
//		else if (0 != vertices3Panel)
//			tmp	= vertices3Panel->actualizeGui (removeDestroyed);

		if (true == tmp)
			modified	= true;
	}	// for (vector<QWidget*>::iterator it = 

	return modified;
}	// QtMgx3DRotationPanel::actualizeGui


void QtMgx3DRotationPanel::rotationModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	emit rotationModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtMgx3DRotationPanel::rotationModifiedCallback


void QtMgx3DRotationPanel::axeModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_anglePanel)
	CHECK_NULL_PTR_ERROR (_axisButtonGroup)
	CHECK_NULL_PTR_ERROR (_anyAxisDefinitionModeComboBox)
	CHECK_NULL_PTR_ERROR (_anyAxisDefinitionParentWidget)
	const bool	any	= _anyAxisCheckBox == _axisButtonGroup->checkedButton ( ) ?
					  true : false;

	const int	current	= _anyAxisDefinitionModeComboBox->currentIndex ( );
	if (any && current == 2)
		_anglePanel->setEnabled (false);
	else
		_anglePanel->setEnabled (true);
	_anyAxisDefinitionModeComboBox->setEnabled (any);
	_anyAxisDefinitionParentWidget->setEnabled (any);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtMgx3DRotationPanel::axeModifiedCallback


void QtMgx3DRotationPanel::axeModeCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_axisButtonGroup)
	CHECK_NULL_PTR_ERROR (_anyAxisDefinitionModeComboBox)

	for (vector<QWidget*>::iterator it = _anyAxisDefinitionWidgets.begin ( );
	     _anyAxisDefinitionWidgets.end ( ) != it; it++)
	{
		QtMgx3DEdgePanel*	edgePanel	= dynamic_cast<QtMgx3DEdgePanel*>(*it);
		Qt2VerticiesPanel*	verticesPanel	= dynamic_cast<Qt2VerticiesPanel*>(*it);
		Qt3VerticiesPanel*	vertices3Panel	= dynamic_cast<Qt3VerticiesPanel*>(*it);
		if (0 != edgePanel)
			edgePanel->stopSelection ( );
		else if (0 != verticesPanel)
			verticesPanel->stopSelection ( );
		else if (0 != vertices3Panel)
			vertices3Panel->stopSelection ( );

		(*it)->hide ( );
	}	// for (vector<QWidget*>::iterator it =  ...

	const int	current	= _anyAxisDefinitionModeComboBox->currentIndex ( );
	if (current >= _anyAxisDefinitionWidgets.size ( ))
	{
		INTERNAL_ERROR (exc, "Plus de modes de saisie que de panneaux.", "QtMgx3DRotationPanel::axeModeCallback")
		throw exc;
	}	// if (current >= _anyAxisDefinitionWidgets.size ( ))
	_anyAxisDefinitionWidgets [current]->show ( );
//SET_WIDGET_BACKGROUND (_anyAxisDefinitionParentWidget, Qt::red);
//SET_WIDGET_BACKGROUND (_anyAxisDefinitionWidgets [current], Qt::cyan);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtMgx3DRotationPanel::axeModeCallback


}	// namespace QtComponents

}	// namespace Mgx3D
