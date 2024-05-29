/**
 * \file        QtGroupProjectionOnPlaneAction.cpp
 * \author      Charles PIGNEROL
 * \date        09/04/2018
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Group/Group2D.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGroupProjectionOnPlaneAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QBoxLayout>
#include <QRadioButton>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Group;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtGroupProjectionOnPlanePanel
// ===========================================================================

QtGroupProjectionOnPlanePanel::QtGroupProjectionOnPlanePanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).groupProjectionOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).groupProjectionOperationTag),
	  _groupPanel (0), _planeButtonGroup (0)
{
//	SET_WIDGET_BACKGROUND (this, Qt::yellow)
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins (0, 0, 0, 0);

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Le groupe à projeter :
	_groupPanel	= new QtMgx3DGroupNamePanel (
			this, "Groupe", mainWindow, 2, QtMgx3DGroupNamePanel::CONSULTATION, "");
	addValidatedField (*_groupPanel);
	layout->addWidget (_groupPanel);

	// La projection à effectuer :
	_planeButtonGroup	= new QButtonGroup (this);
	_planeButtonGroup->setExclusive (true);
	QHBoxLayout*	planeLayout	= new QHBoxLayout ( );
	planeLayout->setContentsMargins  (0, 0, 0, 0);
	layout->addLayout (planeLayout);
	label	= new QLabel ("Plan : ", this);
	planeLayout->addWidget (label);
	QRadioButton*	radioButton	= new QRadioButton ("xOy", this);
	planeLayout->addWidget (radioButton);
	radioButton->setChecked (true);
	_planeButtonGroup->addButton (radioButton, (int)Math::Plane::XY);
	radioButton	= new QRadioButton ("yOz", this);
	planeLayout->addWidget (radioButton);
	_planeButtonGroup->addButton (radioButton, (int)Math::Plane::YZ);
	radioButton	= new QRadioButton ("xOz", this);
	planeLayout->addWidget (radioButton);
	_planeButtonGroup->addButton (radioButton, (int)Math::Plane::XZ);

	layout->addStretch (50.);
}	// QtGroupProjectionOnPlanePanel::QtGroupProjectionOnPlanePanel


QtGroupProjectionOnPlanePanel::QtGroupProjectionOnPlanePanel (const QtGroupProjectionOnPlanePanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _groupPanel (0), _planeButtonGroup (0)
{
	MGX_FORBIDDEN ("QtGroupProjectionOnPlanePanel copy constructor is not allowed.");
}	// QtGroupProjectionOnPlanePanel::QtGroupProjectionOnPlanePanel (const QtGroupProjectionOnPlanePanel&)


QtGroupProjectionOnPlanePanel& QtGroupProjectionOnPlanePanel::operator = (const QtGroupProjectionOnPlanePanel&)
{
	MGX_FORBIDDEN ("QtGroupProjectionOnPlanePanel assignment operator is not allowed.");
	return *this;
}	// QtGroupProjectionOnPlanePanel::QtGroupProjectionOnPlanePanel (const QtGroupProjectionOnPlanePanel&)


QtGroupProjectionOnPlanePanel::~QtGroupProjectionOnPlanePanel ( )
{
}	// QtGroupProjectionOnPlanePanel::~QtGroupProjectionOnPlanePanel


string QtGroupProjectionOnPlanePanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_groupPanel)
	return _groupPanel->getGroupName ( );
}	// QtGroupProjectionOnPlanePanel::getGroupName


Math::Plane::XYZ QtGroupProjectionOnPlanePanel::getProjection ( ) const
{
	CHECK_NULL_PTR_ERROR (_planeButtonGroup)
	return (Math::Plane::XYZ)_planeButtonGroup->checkedId ( );
}	// QtGroupProjectionOnPlanePanel::getProjection


void QtGroupProjectionOnPlanePanel::validate ( )
{
// CP : suite discussion EBL/FL, il est convenu que la validation des
// paramètres de l'opération est effectuée par le "noyau" et qu'un mauvais
// paramétrage est remonté sous forme d'exception à la fonction appelante, donc
// avant exécution de la commande.
// Les validations des valeurs des paramètres sont donc ici commentées.
	UTF8String	error (Charset::UTF_8);

	try
	{
		QtMgx3DOperationPanel::validate ( );
	}
	catch (const Exception& exc)
	{
		error << exc.getFullMessage ( );
	}
	catch (...)
	{
		error << "QtGroupProjectionOnPlanePanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getMeshEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGroupProjectionOnPlanePanel::validate


void QtGroupProjectionOnPlanePanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_groupPanel)
	_groupPanel->autoUpdate ( );
}	// QtGroupProjectionOnPlanePanel::autoUpdate


void QtGroupProjectionOnPlanePanel::reset ( )
{
	CHECK_NULL_PTR_ERROR (_groupPanel)
	_groupPanel->autoUpdate ( );

	QtMgx3DOperationPanel::reset ( );
}	// QtGroupProjectionOnPlanePanel::reset


vector<Entity*> QtGroupProjectionOnPlanePanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	names;
	const string		groupName	= getGroupName ( );
	if (false == groupName.empty ( ))
	{
		Group2D*	group	= getContext ( ).getGroupManager ( ).getGroup2D (groupName, false);
		if (0 != group)
			entities.push_back (group);
	}	// if (false == groupName.empty ( ))

	return entities;
}	// QtGroupProjectionOnPlanePanel::getInvolvedEntities


void QtGroupProjectionOnPlanePanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGroupProjectionOnPlanePanel::operationCompleted



// ===========================================================================
//                  LA CLASSE QtGroupProjectionOnPlaneAction
// ===========================================================================

QtGroupProjectionOnPlaneAction::QtGroupProjectionOnPlaneAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DMeshOperationAction (icon, text, mainWindow, tooltip)
{
	QtGroupProjectionOnPlanePanel*	operationPanel	=
		new QtGroupProjectionOnPlanePanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGroupProjectionOnPlaneAction::QtGroupProjectionOnPlaneAction


QtGroupProjectionOnPlaneAction::QtGroupProjectionOnPlaneAction (const QtGroupProjectionOnPlaneAction&)
	: QtMgx3DMeshOperationAction (QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGroupProjectionOnPlaneAction copy constructor is not allowed.")
}	// QtGroupProjectionOnPlaneAction::QtGroupProjectionOnPlaneAction


QtGroupProjectionOnPlaneAction& QtGroupProjectionOnPlaneAction::operator = (const QtGroupProjectionOnPlaneAction&)
{
	MGX_FORBIDDEN ("QtGroupProjectionOnPlaneAction assignment operator is not allowed.")
	return *this;
}	// QtGroupProjectionOnPlaneAction::operator =


QtGroupProjectionOnPlaneAction::~QtGroupProjectionOnPlaneAction ( )
{
}	// QtGroupProjectionOnPlaneAction::~QtGroupProjectionOnPlaneAction


QtGroupProjectionOnPlanePanel* QtGroupProjectionOnPlaneAction::getProjectionPanel ( )
{
	return dynamic_cast<QtGroupProjectionOnPlanePanel*>(getOperationPanel ( ));
}	// QtGroupProjectionOnPlaneAction::getProjectionPanel


void QtGroupProjectionOnPlaneAction::executeOperation ( )
{
	// Validation paramétrage :
	QtMgx3DMeshOperationAction::executeOperation ( );

	// Récupération des paramètres de la projection :
	QtGroupProjectionOnPlanePanel*	panel	= getProjectionPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const Math::Plane::XYZ	plane		= panel->getProjection ( );
	const string	      	groupName	= panel->getGroupName ( );

	switch (plane)
	{
		case Math::Plane::XY	:
			getContext ( ).getGroupManager ( ).addProjectionOnPZ0 (groupName);
			break;
		case Math::Plane::YZ	:
			getContext ( ).getGroupManager ( ).addProjectionOnPX0 (groupName);
			break;
		case Math::Plane::XZ	:
			getContext ( ).getGroupManager ( ).addProjectionOnPY0 (groupName);
			break;
		default			:
			throw Exception ("Erreur interne en QtGroupProjectionOnPlaneAction::executeOperation : plan inconnu.");
	}	// switch (plane)
}	// QtGroupProjectionOnPlaneAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
