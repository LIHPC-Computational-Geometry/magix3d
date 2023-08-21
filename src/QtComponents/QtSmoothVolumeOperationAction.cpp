/**
 * \file        QtSmoothVolumeOperationAction.cpp
 * \author      Eric Brière de l'Isle
 * \date        8/4/2016
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Mesh/MeshManagerIfc.h"
#include "Smoothing/VolumicSmoothing.h"
#include "Group/GroupManagerIfc.h"
#include "QtComponents/QtSmoothVolumeOperationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/MemoryError.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Mesh;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtSmoothVolumeOperationPanel
// ===========================================================================

QtSmoothVolumeOperationPanel::QtSmoothVolumeOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (parent, mainWindow, action, helpURL, helpTag),
	  _namePanel (0), _smoothDefaultCheckBox (0), _nbIterationsTextField(0),
	  _methodComboBox(0)
{
	QVBoxLayout* layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (layout);

	// Nom opération :
	{
		QLabel*	label	= new QLabel (panelName.c_str ( ), this);
		layout->addWidget (label);
	}

	// Nom groupe :
	{
		_namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe", mainWindow, 3,
				QtMgx3DGroupNamePanel::CONSULTATION, "");
		layout->addWidget (_namePanel);
		//_namePanel->autoUpdate ( );
	}

	// Lisser avec options ?
	{
		_smoothDefaultCheckBox	= new QCheckBox (QString::fromUtf8("Lissage avec options par défaut"), this);
		_smoothDefaultCheckBox->setCheckState(Qt::Checked);
		connect (_smoothDefaultCheckBox, SIGNAL (stateChanged (int)), this,
				SLOT (smoothDefaultCallback ( )));
		layout->addWidget (_smoothDefaultCheckBox);
	}

	// nombre d'itérations
	{
		QHBoxLayout*	hlayout	= new QHBoxLayout ( );
		hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
		hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
		layout->addLayout (hlayout);
		QLabel*	label	= new QLabel (QString::fromUtf8("Nombre d'itérations :"), this);
		hlayout->addWidget (label);
		_nbIterationsTextField	= new QtIntTextField (10, this);
		hlayout->addWidget (_nbIterationsTextField);
	}

	// méthode de lissage
	{
		QHBoxLayout*	hlayout	= new QHBoxLayout ( );
		hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
		hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
		layout->addLayout (hlayout);
		QLabel*	label	= new QLabel (QString::fromUtf8("Méthode de lissage :"), this);
		hlayout->addWidget (label);
		_methodComboBox	= new QComboBox (this);
		_methodComboBox->addItem("laplace");
		_methodComboBox->addItem("tipton");
		_methodComboBox->addItem("jun");
		_methodComboBox->addItem("condition number");
		_methodComboBox->addItem("inverse mean ratio");
		_methodComboBox->setCurrentIndex(0);
		hlayout->addWidget (_methodComboBox);
	}

	layout->addStretch (10);

	smoothDefaultCallback ( );
}	// QtSmoothVolumeOperationPanel::QtSmoothVolumeOperationPanel


QtSmoothVolumeOperationPanel::QtSmoothVolumeOperationPanel (
									const QtSmoothVolumeOperationPanel& cao)
	: QtMgx3DOperationPanel (
				0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _namePanel (0), _smoothDefaultCheckBox (0), _nbIterationsTextField(0),
	  _methodComboBox(0)
{
	MGX_FORBIDDEN ("QtSmoothVolumeOperationPanel copy constructor is not allowed.");
}	// QtSmoothVolumeOperationPanel::QtSmoothVolumeOperationPanel (const QtSmoothVolumeOperationPanel&)


QtSmoothVolumeOperationPanel& QtSmoothVolumeOperationPanel::operator = (
										const QtSmoothVolumeOperationPanel&)
{
	MGX_FORBIDDEN ("QtSmoothVolumeOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtSmoothVolumeOperationPanel::QtSmoothVolumeOperationPanel (const QtSmoothVolumeOperationPanel&)


QtSmoothVolumeOperationPanel::~QtSmoothVolumeOperationPanel ( )
{
}	// QtSmoothVolumeOperationPanel::~QtSmoothVolumeOperationPanel


void QtSmoothVolumeOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_nbIterationsTextField)
	_namePanel->autoUpdate ( );
	_nbIterationsTextField->setValue (10);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtSmoothVolumeOperationPanel::reset


string QtSmoothVolumeOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtSmoothVolumeOperationPanel::getGroupName



void QtSmoothVolumeOperationPanel::autoUpdate ( )
{
//	CHECK_NULL_PTR_ERROR (_facesPanel)

	if (0 != _namePanel){
		_namePanel->autoUpdate ( );
		//_namePanel->setPolicy(QtMgx3DGroupNamePanel::CREATION, "");
	}
	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtSmoothVolumeOperationPanel::autoUpdate


bool QtSmoothVolumeOperationPanel::smoothDefault ( ) const
{
	CHECK_NULL_PTR_ERROR (_smoothDefaultCheckBox)
	return Qt::Checked == _smoothDefaultCheckBox->checkState ( ) ? true : false;
}	// QtSmoothVolumeOperationPanel::smoothDefault


void QtSmoothVolumeOperationPanel::smoothDefaultCallback ( )
{
	if (smoothDefault()){
		_nbIterationsTextField->setEnabled(false);
		_methodComboBox->setEnabled(false);
	}
	else {
		_nbIterationsTextField->setEnabled(true);
		_methodComboBox->setEnabled(true);
	}
}	// QtSmoothVolumeOperationPanel::smoothDefaultCallback

uint QtSmoothVolumeOperationPanel::getNbIterations ( ) const
{
	CHECK_NULL_PTR_ERROR (_nbIterationsTextField)
	return _nbIterationsTextField->getValue ( );
}	// QtSmoothVolumeOperationPanel::getNbIteration

VolumicSmoothing::eVolumicMethod
QtSmoothVolumeOperationPanel::getMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_methodComboBox);
	return (VolumicSmoothing::eVolumicMethod)_methodComboBox->currentIndex ( );
}	// QtSmoothVolumeOperationPanel::getMethod


// ===========================================================================
//                  LA CLASSE QtSmoothVolumeOperationAction
// ===========================================================================

QtSmoothVolumeOperationAction::QtSmoothVolumeOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DMeshOperationAction (icon, text, mainWindow, tooltip)
{
	QtSmoothVolumeOperationPanel*	operationPanel	=
			new QtSmoothVolumeOperationPanel (
				&getOperationPanelParent ( ), text.toStdString ( ),
				mainWindow, this,
				QtMgx3DApplication::HelpSystem::instance ( ).meshSmoothVolumeOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).meshSmoothVolumeOperationTag
				);
		if ((0 != operationPanel) && (getOperationPanel ( ) != operationPanel))
			setOperationPanel (operationPanel);
}	// QtSmoothVolumeOperationAction::QtSmoothVolumeOperationAction


QtSmoothVolumeOperationAction::QtSmoothVolumeOperationAction (
										const QtSmoothVolumeOperationAction&)
	: QtMgx3DMeshOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtSmoothVolumeOperationAction copy constructor is not allowed.")
}	// QtSmoothVolumeOperationAction::QtSmoothVolumeOperationAction


QtSmoothVolumeOperationAction& QtSmoothVolumeOperationAction::operator = (
										const QtSmoothVolumeOperationAction&)
{
	MGX_FORBIDDEN ("QtSmoothVolumeOperationAction assignment operator is not allowed.")
	return *this;
}	// QtSmoothVolumeOperationAction::operator =


QtSmoothVolumeOperationAction::~QtSmoothVolumeOperationAction ( )
{
}	// QtSmoothVolumeOperationAction::~QtSmoothVolumeOperationAction


QtSmoothVolumeOperationPanel* QtSmoothVolumeOperationAction::getSmoothPanel ( )
{
	return dynamic_cast<QtSmoothVolumeOperationPanel*>(getOperationPanel ( ));
}	// QtSmoothVolumeOperationAction::getSmoothPanel


void QtSmoothVolumeOperationAction::executeOperation ( )
{
	CHECK_NULL_PTR_ERROR (getSmoothPanel ( ))

	// Validation paramétrage :
	QtMgx3DMeshOperationAction::executeOperation ( );

	QtSmoothVolumeOperationPanel*	panel	= getSmoothPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string		groupName	= panel->getGroupName ( );

	if (true == getSmoothPanel ( )->smoothDefault ( )){
		Mesh::VolumicSmoothing lissage;
		getContext ( ).getGroupManager().addSmoothing(groupName, lissage);
	}
	else
	{
		Mesh::VolumicSmoothing lissage(panel->getNbIterations(),
				panel->getMethod());
		getContext ( ).getGroupManager().addSmoothing(groupName, lissage);
	}
}	// QtSmoothVolumeOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
