/**
 * \file        QtSysCoordTranslationAction.cpp
 * \author		Eric Brière de l'Isle
 * \date		8/6/2018
 */

#include "Internal/ContextIfc.h"
#include "Utils/SelectionManagerIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtSysCoordTranslationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DVectorPanel.h"

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
//                        LA CLASSE QtSysCoordTranslationPanel
// ===========================================================================

QtSysCoordTranslationPanel::QtSysCoordTranslationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).syscoordOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).syscoordTranslationTag),
	  _syscoordPanel (0),  _translationPanel (0), _copyCheckBox(0), _namePanel(0)
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


	// La Translation :
	_translationPanel	=
			new QtMgx3DVectorPanel (this, "", true,
					"dx :", "dy :", "dz :",
					1., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
					0., -DBL_MAX, DBL_MAX, &mainWindow,
					(FilterEntity::objectType)(
							FilterEntity::AllPoints | FilterEntity::AllEdges),
							true);
//	connect (_translationPanel, SIGNAL (TranslationModified ( )), this,
//	         SLOT (parametersModifiedCallback ( )));
	layout->addWidget (_translationPanel);

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

}	// QtSysCoordTranslationPanel::QtSysCoordTranslationPanel


QtSysCoordTranslationPanel::QtSysCoordTranslationPanel (
										const QtSysCoordTranslationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _syscoordPanel (0), _translationPanel (0), _copyCheckBox(0), _namePanel(0)
{
	MGX_FORBIDDEN ("QtSysCoordTranslationPanel copy constructor is not allowed.");
}	// QtSysCoordTranslationPanel::QtSysCoordTranslationPanel (const QtSysCoordTranslationPanel&)


QtSysCoordTranslationPanel& QtSysCoordTranslationPanel::operator = (
											const QtSysCoordTranslationPanel&)
{
	MGX_FORBIDDEN ("QtSysCoordTranslationPanel assignment operator is not allowed.");
	return *this;
}	// QtSysCoordTranslationPanel::QtSysCoordTranslationPanel (const QtSysCoordTranslationPanel&)


QtSysCoordTranslationPanel::~QtSysCoordTranslationPanel ( )
{
	preview (false, true);
}	// QtSysCoordTranslationPanel::~QtSysCoordTranslationPanel


void QtSysCoordTranslationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_syscoordPanel)
	CHECK_NULL_PTR_ERROR (_translationPanel)
	_syscoordPanel->reset ( );
	_translationPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtSysCoordTranslationPanel::reset



string QtSysCoordTranslationPanel::getSysCoordName ( ) const
{
	CHECK_NULL_PTR_ERROR (_syscoordPanel)
	return _syscoordPanel->getUniqueName ( );
}	// QtSysCoordTranslationPanel::getSysCoordName


Math::Vector QtSysCoordTranslationPanel::getTranslation ( ) const
{
	CHECK_NULL_PTR_ERROR (_translationPanel)
	return Vector (_translationPanel->getDx ( ),
			_translationPanel->getDy ( ),
			_translationPanel->getDz ( ));;
}	// QtSysCoordTranslationPanel::getTranslation


bool QtSysCoordTranslationPanel::copyEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_copyCheckBox)
	return _copyCheckBox->isChecked ( );
}

std::string QtSysCoordTranslationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}

void QtSysCoordTranslationPanel::validate ( )
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
		error << "QtSysCoordTranslationPanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtSysCoordTranslationPanel::validate


void QtSysCoordTranslationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_syscoordPanel)
	CHECK_NULL_PTR_ERROR (_translationPanel)
	_syscoordPanel->stopSelection ( );
	_translationPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_syscoordPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtSysCoordTranslationPanel::cancel


void QtSysCoordTranslationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_syscoordPanel)
	CHECK_NULL_PTR_ERROR (_translationPanel)
	_syscoordPanel->clearSelection ( );

	_syscoordPanel->actualizeGui (true);
	_translationPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtSysCoordTranslationPanel::autoUpdate


vector<Entity*> QtSysCoordTranslationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const string	name	= getSysCoordName ( );
	CoordinateSystem::SysCoord*	entity	=
			getContext ( ).getSysCoordManager ( ).getSysCoord (name, false);
	if (0 != entity)
		entities.push_back (entity);


	return entities;
}	// QtSysCoordTranslationPanel::getInvolvedEntities


void QtSysCoordTranslationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _syscoordPanel)
		_syscoordPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtSysCoordTranslationPanel::operationCompleted

void QtSysCoordTranslationPanel::copyCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)

	_namePanel->setEnabled (copyEntities ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}


// ===========================================================================
//                  LA CLASSE QtSysCoordTranslationAction
// ===========================================================================

QtSysCoordTranslationAction::QtSysCoordTranslationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DOperationAction (icon, text, mainWindow, tooltip)
{
	QtSysCoordTranslationPanel*	operationPanel	=
		new QtSysCoordTranslationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtSysCoordTranslationAction::QtSysCoordTranslationAction


QtSysCoordTranslationAction::QtSysCoordTranslationAction (
										const QtSysCoordTranslationAction&)
	: QtMgx3DOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtSysCoordTranslationAction copy constructor is not allowed.")
}	// QtSysCoordTranslationAction::QtSysCoordTranslationAction


QtSysCoordTranslationAction& QtSysCoordTranslationAction::operator = (
										const QtSysCoordTranslationAction&)
{
	MGX_FORBIDDEN ("QtSysCoordTranslationAction assignment operator is not allowed.")
	return *this;
}	// QtSysCoordTranslationAction::operator =


QtSysCoordTranslationAction::~QtSysCoordTranslationAction ( )
{
}	// QtSysCoordTranslationAction::~QtSysCoordTranslationAction


QtSysCoordTranslationPanel*
					QtSysCoordTranslationAction::getTranslationPanel ( )
{
	return dynamic_cast<QtSysCoordTranslationPanel*>(getOperationPanel ( ));
}	// QtSysCoordTranslationAction::getTranslationPanel


void QtSysCoordTranslationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DOperationAction::executeOperation ( );

	QtSysCoordTranslationPanel*	panel	= getTranslationPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	string					entitiy	= panel->getSysCoordName ( );
	Math::Vector			translation	(panel->getTranslation ( ));
	const bool			    copy        = panel->copyEntities();
	const string	      	groupName	= panel->getGroupName ( );

	if (copy)
		cmdResult	= getContext ( ).getSysCoordManager ( ).copyAndTranslate (entitiy, translation, groupName);
	else
		cmdResult	= getContext ( ).getSysCoordManager ( ).translate (entitiy, translation);

	setCommandResult (cmdResult);
}	// QtSysCoordTranslationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
