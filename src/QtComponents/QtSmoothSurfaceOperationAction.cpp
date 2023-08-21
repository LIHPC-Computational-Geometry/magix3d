/**
 * \file        QtSmoothSurfaceOperationAction.cpp
 * \author      Eric Brière de l'Isle
 * \date        4/4/2016
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Internal/Resources.h"
#include "Mesh/MeshManagerIfc.h"
#include "Smoothing/SurfacicSmoothing.h"
#include "Group/GroupManagerIfc.h"
#include "QtComponents/QtSmoothSurfaceOperationAction.h"
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
//                        LA CLASSE QtSmoothSurfaceOperationPanel
// ===========================================================================

QtSmoothSurfaceOperationPanel::QtSmoothSurfaceOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (parent, mainWindow, action, helpURL, helpTag),
	  _namePanel (0), _smoothDefaultCheckBox (0), _nbIterationsTextField(0),
	  _methodComboBox(0), _solverComboBox(0)
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
		_namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe", mainWindow, 2,
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
		_methodComboBox->addItem(QString::fromUtf8("optimisation suivant la normale"));
		_methodComboBox->addItem(QString::fromUtf8("optimisation suivant l'orthogonalité des bras"));
		_methodComboBox->addItem(QString::fromUtf8("optimisation elliptique suivant l'orthogonalité des bras"));
		_methodComboBox->setCurrentIndex(2);
		hlayout->addWidget (_methodComboBox);
	}

	// solveur
	{
		QHBoxLayout*	hlayout	= new QHBoxLayout ( );
		hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
		hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
		layout->addLayout (hlayout);
		QLabel*	label	= new QLabel ("Solveur :", this);
		hlayout->addWidget (label);
		_solverComboBox	= new QComboBox (this);
		_solverComboBox->addItem(QString::fromUtf8("gardient conjugué"));
		_solverComboBox->addItem(QString::fromUtf8("newton"));
		_solverComboBox->setCurrentIndex(0);
		hlayout->addWidget (_solverComboBox);
	}

	layout->addStretch (10);

	smoothDefaultCallback ( );
}	// QtSmoothSurfaceOperationPanel::QtSmoothSurfaceOperationPanel


QtSmoothSurfaceOperationPanel::QtSmoothSurfaceOperationPanel (
									const QtSmoothSurfaceOperationPanel& cao)
	: QtMgx3DOperationPanel (
				0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _namePanel (0), _smoothDefaultCheckBox (0), _nbIterationsTextField(0),
	  _methodComboBox(0), _solverComboBox(0)
{
	MGX_FORBIDDEN ("QtSmoothSurfaceOperationPanel copy constructor is not allowed.");
}	// QtSmoothSurfaceOperationPanel::QtSmoothSurfaceOperationPanel (const QtSmoothSurfaceOperationPanel&)


QtSmoothSurfaceOperationPanel& QtSmoothSurfaceOperationPanel::operator = (
										const QtSmoothSurfaceOperationPanel&)
{
	MGX_FORBIDDEN ("QtSmoothSurfaceOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtSmoothSurfaceOperationPanel::QtSmoothSurfaceOperationPanel (const QtSmoothSurfaceOperationPanel&)


QtSmoothSurfaceOperationPanel::~QtSmoothSurfaceOperationPanel ( )
{
}	// QtSmoothSurfaceOperationPanel::~QtSmoothSurfaceOperationPanel


void QtSmoothSurfaceOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_nbIterationsTextField)
	_namePanel->autoUpdate ( );
	_nbIterationsTextField->setValue (10);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtSmoothSurfaceOperationPanel::reset


string QtSmoothSurfaceOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtSmoothSurfaceOperationPanel::getGroupName



void QtSmoothSurfaceOperationPanel::autoUpdate ( )
{
//	CHECK_NULL_PTR_ERROR (_facesPanel)

	if (0 != _namePanel){
		_namePanel->autoUpdate ( );
		//_namePanel->setPolicy(QtMgx3DGroupNamePanel::CREATION, "");
	}
	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtSmoothSurfaceOperationPanel::autoUpdate


bool QtSmoothSurfaceOperationPanel::smoothDefault ( ) const
{
	CHECK_NULL_PTR_ERROR (_smoothDefaultCheckBox)
	return Qt::Checked == _smoothDefaultCheckBox->checkState ( ) ? true : false;
}	// QtSmoothSurfaceOperationPanel::smoothDefault


void QtSmoothSurfaceOperationPanel::smoothDefaultCallback ( )
{
	if (smoothDefault()){
		_nbIterationsTextField->setEnabled(false);
		_methodComboBox->setEnabled(false);
		_solverComboBox->setEnabled(false);
	}
	else {
		_nbIterationsTextField->setEnabled(true);
		_methodComboBox->setEnabled(true);
		_solverComboBox->setEnabled(true);
	}
}	// QtSmoothSurfaceOperationPanel::smoothDefaultCallback

uint QtSmoothSurfaceOperationPanel::getNbIterations ( ) const
{
	CHECK_NULL_PTR_ERROR (_nbIterationsTextField)
	return _nbIterationsTextField->getValue ( );
}	// QtSmoothSurfaceOperationPanel::getNbIteration

SurfacicSmoothing::eSurfacicMethod
QtSmoothSurfaceOperationPanel::getMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_methodComboBox);
	return (SurfacicSmoothing::eSurfacicMethod)_methodComboBox->currentIndex ( );
}	// QtSmoothSurfaceOperationPanel::getMethod

SurfacicSmoothing::eSolver
QtSmoothSurfaceOperationPanel::getSolver ( ) const
{
	CHECK_NULL_PTR_ERROR (_solverComboBox);
	return (SurfacicSmoothing::eSolver)_solverComboBox->currentIndex ( );
}	// QtSmoothSurfaceOperationPanel::getSolver



// ===========================================================================
//                  LA CLASSE QtSmoothSurfaceOperationAction
// ===========================================================================

QtSmoothSurfaceOperationAction::QtSmoothSurfaceOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DMeshOperationAction (icon, text, mainWindow, tooltip)
{
	QtSmoothSurfaceOperationPanel*	operationPanel	=
			new QtSmoothSurfaceOperationPanel (
				&getOperationPanelParent ( ), text.toStdString ( ),
				mainWindow, this,
				QtMgx3DApplication::HelpSystem::instance ( ).meshSmoothSurfaceOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).meshSmoothSurfaceOperationTag
				);
		if ((0 != operationPanel) && (getOperationPanel ( ) != operationPanel))
			setOperationPanel (operationPanel);
}	// QtSmoothSurfaceOperationAction::QtSmoothSurfaceOperationAction


QtSmoothSurfaceOperationAction::QtSmoothSurfaceOperationAction (
										const QtSmoothSurfaceOperationAction&)
	: QtMgx3DMeshOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtSmoothSurfaceOperationAction copy constructor is not allowed.")
}	// QtSmoothSurfaceOperationAction::QtSmoothSurfaceOperationAction


QtSmoothSurfaceOperationAction& QtSmoothSurfaceOperationAction::operator = (
										const QtSmoothSurfaceOperationAction&)
{
	MGX_FORBIDDEN ("QtSmoothSurfaceOperationAction assignment operator is not allowed.")
	return *this;
}	// QtSmoothSurfaceOperationAction::operator =


QtSmoothSurfaceOperationAction::~QtSmoothSurfaceOperationAction ( )
{
}	// QtSmoothSurfaceOperationAction::~QtSmoothSurfaceOperationAction


QtSmoothSurfaceOperationPanel* QtSmoothSurfaceOperationAction::getSmoothPanel ( )
{
	return dynamic_cast<QtSmoothSurfaceOperationPanel*>(getOperationPanel ( ));
}	// QtSmoothSurfaceOperationAction::getSmoothPanel


void QtSmoothSurfaceOperationAction::executeOperation ( )
{
	CHECK_NULL_PTR_ERROR (getSmoothPanel ( ))

	// Validation paramétrage :
	QtMgx3DMeshOperationAction::executeOperation ( );

	QtSmoothSurfaceOperationPanel*	panel	= getSmoothPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string		groupName	= panel->getGroupName ( );

	if (true == getSmoothPanel ( )->smoothDefault ( )){
		Mesh::SurfacicSmoothing lissage;
		getContext ( ).getGroupManager().addSmoothing(groupName, lissage);
	}
	else
	{
		Mesh::SurfacicSmoothing lissage(panel->getNbIterations(),
				panel->getMethod(),
				panel->getSolver());
		getContext ( ).getGroupManager().addSmoothing(groupName, lissage);
	}
}	// QtSmoothSurfaceOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
