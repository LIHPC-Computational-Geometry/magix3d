/**
 * \file        QtSysCoordRotationAction.cpp
 * \author		Eric Brière de l'Isle
 * \date		7/6/2018
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtSysCoordRotationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DEntityPanel.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::CoordinateSystem;

namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtSysCoordRotationPanel
// ===========================================================================

QtSysCoordRotationPanel::QtSysCoordRotationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).syscoordOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).syscoordRotationTag),
	  _syscoordPanel (0),  _rotationPanel (0), _copyCheckBox(0), _namePanel(0)
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



	_syscoordPanel	= new QtMgx3DEntityPanel (
				this, "", true, "Repère à modifier :", "", &mainWindow, SelectionManagerIfc::D3,
				FilterEntity::SysCoord);
	_syscoordPanel->setMultiSelectMode (false);


	connect (_syscoordPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_syscoordPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_syscoordPanel);


	// La rotation :
	_rotationPanel	= new QtMgx3DRotationPanel (
			this, "", true, QtAnglePanel::eightthToHalf ( ), 0, &mainWindow);
	connect (_rotationPanel, SIGNAL (rotationModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));
	layout->addWidget (_rotationPanel);

	layout->addStretch (2);

	// Faut il faire une copie des entités avant la transformation ?
	_copyCheckBox	=
			new QCheckBox ("Copier avant transformation", this);
	connect (_copyCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (copyCallback ( )));
	_copyCheckBox->setChecked (false);
	layout->addWidget (_copyCheckBox);

	// Nom groupe (en cas de copie):
	_namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe", mainWindow, 3,
			QtMgx3DGroupNamePanel::CREATION, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	copyCallback();

}	// QtSysCoordRotationPanel::QtSysCoordRotationPanel


QtSysCoordRotationPanel::QtSysCoordRotationPanel (
										const QtSysCoordRotationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _syscoordPanel (0), _rotationPanel (0), _copyCheckBox(0), _namePanel(0)
{
	MGX_FORBIDDEN ("QtSysCoordRotationPanel copy constructor is not allowed.");
}	// QtSysCoordRotationPanel::QtSysCoordRotationPanel (const QtSysCoordRotationPanel&)


QtSysCoordRotationPanel& QtSysCoordRotationPanel::operator = (
											const QtSysCoordRotationPanel&)
{
	MGX_FORBIDDEN ("QtSysCoordRotationPanel assignment operator is not allowed.");
	return *this;
}	// QtSysCoordRotationPanel::QtSysCoordRotationPanel (const QtSysCoordRotationPanel&)


QtSysCoordRotationPanel::~QtSysCoordRotationPanel ( )
{
	preview (false, true);
}	// QtSysCoordRotationPanel::~QtSysCoordRotationPanel


void QtSysCoordRotationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_syscoordPanel)
	CHECK_NULL_PTR_ERROR (_rotationPanel)
	_syscoordPanel->reset ( );
	_rotationPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtSysCoordRotationPanel::reset



string QtSysCoordRotationPanel::getSysCoordName ( ) const
{
	CHECK_NULL_PTR_ERROR (_syscoordPanel)
	return _syscoordPanel->getUniqueName ( );
}	// QtSysCoordRotationPanel::getSysCoordName


Math::Rotation QtSysCoordRotationPanel::getRotation ( ) const
{
	CHECK_NULL_PTR_ERROR (_rotationPanel)
	return _rotationPanel->getRotation ( );
}	// QtSysCoordRotationPanel::getRotation


bool QtSysCoordRotationPanel::copyEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_copyCheckBox)
	return _copyCheckBox->isChecked ( );
}

std::string QtSysCoordRotationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}

void QtSysCoordRotationPanel::validate ( )
{
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
		error << "QtSysCoordRotationPanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtSysCoordRotationPanel::validate


void QtSysCoordRotationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_syscoordPanel)
	CHECK_NULL_PTR_ERROR (_rotationPanel)
	_syscoordPanel->stopSelection ( );
	_rotationPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_syscoordPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtSysCoordRotationPanel::cancel


void QtSysCoordRotationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_syscoordPanel)
	CHECK_NULL_PTR_ERROR (_rotationPanel)
	_syscoordPanel->clearSelection ( );

	_syscoordPanel->actualizeGui (true);
	_rotationPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtSysCoordRotationPanel::autoUpdate


vector<Entity*> QtSysCoordRotationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const string	name	= getSysCoordName ( );
	CoordinateSystem::SysCoord*	entity	=
			getContext ( ).getSysCoordManager ( ).getSysCoord (name, false);
	if (0 != entity)
		entities.push_back (entity);


	return entities;
}	// QtSysCoordRotationPanel::getInvolvedEntities


void QtSysCoordRotationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _syscoordPanel)
		_syscoordPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtSysCoordRotationPanel::operationCompleted


void QtSysCoordRotationPanel::copyCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)

	_namePanel->setEnabled (copyEntities ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}


// ===========================================================================
//                  LA CLASSE QtSysCoordRotationAction
// ===========================================================================

QtSysCoordRotationAction::QtSysCoordRotationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DOperationAction (icon, text, mainWindow, tooltip)
{
	QtSysCoordRotationPanel*	operationPanel	=
		new QtSysCoordRotationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtSysCoordRotationAction::QtSysCoordRotationAction


QtSysCoordRotationAction::QtSysCoordRotationAction (
										const QtSysCoordRotationAction&)
	: QtMgx3DOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtSysCoordRotationAction copy constructor is not allowed.")
}	// QtSysCoordRotationAction::QtSysCoordRotationAction


QtSysCoordRotationAction& QtSysCoordRotationAction::operator = (
										const QtSysCoordRotationAction&)
{
	MGX_FORBIDDEN ("QtSysCoordRotationAction assignment operator is not allowed.")
	return *this;
}	// QtSysCoordRotationAction::operator =


QtSysCoordRotationAction::~QtSysCoordRotationAction ( )
{
}	// QtSysCoordRotationAction::~QtSysCoordRotationAction


QtSysCoordRotationPanel*
					QtSysCoordRotationAction::getRotationPanel ( )
{
	return dynamic_cast<QtSysCoordRotationPanel*>(getOperationPanel ( ));
}	// QtSysCoordRotationAction::getRotationPanel


void QtSysCoordRotationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DOperationAction::executeOperation ( );

	QtSysCoordRotationPanel*	panel	= getRotationPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	string				entitiy	= panel->getSysCoordName ( );
	Math::Rotation		rotation	= panel->getRotation ( );
	const bool			    copy        = panel->copyEntities();
	const string	      	groupName	= panel->getGroupName ( );

	if (copy)
		cmdResult	= getContext ( ).getSysCoordManager ( ).copyAndRotate (entitiy, rotation, groupName);
	else
		cmdResult	= getContext ( ).getSysCoordManager ( ).rotate (entitiy, rotation);

	setCommandResult (cmdResult);
}	// QtSysCoordRotationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
