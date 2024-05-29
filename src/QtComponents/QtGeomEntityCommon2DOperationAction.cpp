/**
 * \file        QtGeomEntityCommon2DOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        11/09/2018
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeomEntityCommon2DOperationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtGeomEntityCommon2DOperationPanel
// ===========================================================================

QtGeomEntityCommon2DOperationPanel::QtGeomEntityCommon2DOperationPanel (
	QWidget* parent, const string& panelName, QtMgx3DMainWindow& mainWindow, int dimension, 
	QtMgx3DGroupNamePanel::POLICY creationPolicy, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).geomCommon2DOperationURL,
			(dimension==2?QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).geomCommon2DOperationSurfacesTag
					:QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).geomCommon2DOperationCourbesTag)),
	  _dimension (dimension), _copyCheckBox(0), _namePanel (0), _firstEntityPanel (0), _secondEntityPanel (0)
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

	// Nom groupe :
	_namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe entité intersécante", mainWindow, dimension - 1, creationPolicy, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// Les entités à intersequer :
	const FilterEntity::objectType	filter	= 1 == dimension ? FilterEntity::GeomCurve : FilterEntity::GeomSurface;
	const SelectionManagerIfc::DIM	dims	= 1 == dimension ? SelectionManagerIfc::D1 : SelectionManagerIfc::D2;
	_firstEntityPanel			= new QtMgx3DEntityPanel (
							this, "", true, "Entité 1 :", "", &mainWindow, dims, filter);
	_firstEntityPanel->setMultiSelectMode (false);
	connect (_firstEntityPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_firstEntityPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_firstEntityPanel);
	CHECK_NULL_PTR_ERROR (_firstEntityPanel->getNameTextField ( ))
	_secondEntityPanel			= new QtMgx3DEntityPanel (
							this, "", true, "Entité 2 :", "", &mainWindow, dims, filter);
	_secondEntityPanel->setMultiSelectMode (false);
	connect (_secondEntityPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_secondEntityPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_secondEntityPanel);
	CHECK_NULL_PTR_ERROR (_secondEntityPanel->getNameTextField ( ))
	_firstEntityPanel->getNameTextField ( )->setLinkedSeizureManagers (0, _secondEntityPanel->getNameTextField ( ));
	_secondEntityPanel->getNameTextField ( )->setLinkedSeizureManagers (_firstEntityPanel->getNameTextField ( ), 0);

	// Faut il faire une copie des entités avant la transformation ?
	_copyCheckBox	=
			new QCheckBox ("Conserver les entités (copier avant)", this);
	_copyCheckBox->setChecked (true);
	layout->addWidget (_copyCheckBox);

	layout->addStretch (2);
}	// QtGeomEntityCommon2DOperationPanel::QtGeomEntityCommon2DOperationPanel


QtGeomEntityCommon2DOperationPanel::QtGeomEntityCommon2DOperationPanel (const QtGeomEntityCommon2DOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _dimension (-1), _copyCheckBox(0), _namePanel (0), _firstEntityPanel (0), _secondEntityPanel (0)
{
	MGX_FORBIDDEN ("QtGeomEntityCommon2DOperationPanel copy constructor is not allowed.");
}	// QtGeomEntityCommon2DOperationPanel::QtGeomEntityCommon2DOperationPanel (const QtGeomEntityCommon2DOperationPanel&)


QtGeomEntityCommon2DOperationPanel& QtGeomEntityCommon2DOperationPanel::operator = (const QtGeomEntityCommon2DOperationPanel&)
{
	MGX_FORBIDDEN ("QtGeomEntityCommon2DOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtGeomEntityCommon2DOperationPanel::QtGeomEntityCommon2DOperationPanel (const QtGeomEntityCommon2DOperationPanel&)


QtGeomEntityCommon2DOperationPanel::~QtGeomEntityCommon2DOperationPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtGeomEntityCommon2DOperationPanel::~QtGeomEntityCommon2DOperationPanel

bool QtGeomEntityCommon2DOperationPanel::copyEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_copyCheckBox)
	return _copyCheckBox->isChecked ( );
}	// QtGeomEntityCommon2DOperationPanel::copyEntities

string QtGeomEntityCommon2DOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtGeomEntityCommon2DOperationPanel::getGroupName


string QtGeomEntityCommon2DOperationPanel::getFirstGeomEntityName ( ) const
{
	CHECK_NULL_PTR_ERROR (_firstEntityPanel)
	return _firstEntityPanel->getUniqueName ( );
}	// QtGeomEntityCommon2DOperationPanel::getFirstGeomEntityName


string QtGeomEntityCommon2DOperationPanel::getSecondGeomEntityName ( ) const
{
	CHECK_NULL_PTR_ERROR (_secondEntityPanel)
	return _secondEntityPanel->getUniqueName ( );
}	// QtGeomEntityCommon2DOperationPanel::getSecondGeomEntityName


int QtGeomEntityCommon2DOperationPanel::getDimension ( ) const
{
	return _dimension;
}	// QtGeomEntityCommon2DOperationPanel::withTopology


void QtGeomEntityCommon2DOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_firstEntityPanel)
	CHECK_NULL_PTR_ERROR (_secondEntityPanel)
	_namePanel->autoUpdate ( );
	_firstEntityPanel->reset ( );
	_secondEntityPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeomEntityCommon2DOperationPanel::reset


void QtGeomEntityCommon2DOperationPanel::validate ( )
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
		error << "QtGeomEntityCommon2DOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getGeomEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGeomEntityCommon2DOperationPanel::validate


void QtGeomEntityCommon2DOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_firstEntityPanel)
	CHECK_NULL_PTR_ERROR (_secondEntityPanel)
	_firstEntityPanel->stopSelection ( );
	_secondEntityPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_firstEntityPanel->clearSelection ( );
		_secondEntityPanel->clearSelection ( );
		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeomEntityCommon2DOperationPanel::cancel


void QtGeomEntityCommon2DOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_firstEntityPanel)
	CHECK_NULL_PTR_ERROR (_secondEntityPanel)

	_firstEntityPanel->clearSelection ( );
	_secondEntityPanel->clearSelection ( );

	_namePanel->autoUpdate ( );
	_firstEntityPanel->actualizeGui (true);
	_secondEntityPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtGeomEntityCommon2DOperationPanel::autoUpdate


vector<Entity*> QtGeomEntityCommon2DOperationPanel::getInvolvedEntities ( )
{
	CHECK_NULL_PTR_ERROR (_firstEntityPanel)
	CHECK_NULL_PTR_ERROR (_secondEntityPanel)

	vector<Entity*>	entities;

	if (1 == _firstEntityPanel->getNameTextField ( )->getObjectsCount ( ))
		entities.push_back (getContext ( ).getGeomManager ( ).getEntity (_firstEntityPanel->getUniqueName ( ), false));
	if (1 == _secondEntityPanel->getNameTextField ( )->getObjectsCount ( ))
		entities.push_back (getContext ( ).getGeomManager ( ).getEntity (_secondEntityPanel->getUniqueName ( ), false));

	return entities;
}	// QtGeomEntityCommon2DOperationPanel::getInvolvedEntities


void QtGeomEntityCommon2DOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

//	if (0 != _namePanel)
//		_namePanel->autoUpdate ( );
	if (0 != _firstEntityPanel)
	{
		_firstEntityPanel->stopSelection ( );
		_firstEntityPanel->clearSelection ( );
	}
	if (0 != _secondEntityPanel)
	{
		_secondEntityPanel->stopSelection ( );
		_secondEntityPanel->clearSelection ( );
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeomEntityCommon2DOperationPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtGeomEntityCommon2DOperationAction
// ===========================================================================

QtGeomEntityCommon2DOperationAction::QtGeomEntityCommon2DOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	int dimension, const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeomEntityCommon2DOperationPanel*	operationPanel	=
		new QtGeomEntityCommon2DOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, dimension, creationPolicy, this);
	setOperationPanel (operationPanel);
}	// QtGeomEntityCommon2DOperationAction::QtGeomEntityCommon2DOperationAction


QtGeomEntityCommon2DOperationAction::QtGeomEntityCommon2DOperationAction (const QtGeomEntityCommon2DOperationAction&)
	: QtMgx3DGeomOperationAction (QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeomEntityCommon2DOperationAction copy constructor is not allowed.")
}	// QtGeomEntityCommon2DOperationAction::QtGeomEntityCommon2DOperationAction


QtGeomEntityCommon2DOperationAction& QtGeomEntityCommon2DOperationAction::operator = (
										const QtGeomEntityCommon2DOperationAction&)
{
	MGX_FORBIDDEN ("QtGeomEntityCommon2DOperationAction assignment operator is not allowed.")
	return *this;
}	// QtGeomEntityCommon2DOperationAction::operator =


QtGeomEntityCommon2DOperationAction::~QtGeomEntityCommon2DOperationAction ( )
{
}	// QtGeomEntityCommon2DOperationAction::~QtGeomEntityCommon2DOperationAction


QtGeomEntityCommon2DOperationPanel* QtGeomEntityCommon2DOperationAction::getCommon2DPanel ( )
{
	return dynamic_cast<QtGeomEntityCommon2DOperationPanel*>(getOperationPanel ( ));
}	// QtGeomEntityCommon2DOperationAction::getCommon2DPanel


void QtGeomEntityCommon2DOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de copie des entités géométriques :
	QtGeomEntityCommon2DOperationPanel*	panel	= getCommon2DPanel ( );
	CHECK_NULL_PTR_ERROR (panel)

	const string	groupName	= panel->getGroupName ( );
	const string	firstName	= panel->getFirstGeomEntityName ( );
	const string	secondName	= panel->getSecondGeomEntityName ( );
	const bool			copy        = panel->copyEntities();

	M3DCommandResultIfc*	cmdResult	= 0;
	if (copy)
		cmdResult = getContext ( ).getGeomManager ( ).common2DOnCopy (firstName, secondName, groupName);
	else
		cmdResult = getContext ( ).getGeomManager ( ).common2D (firstName, secondName, groupName);
	setCommandResult (cmdResult);
}	// QtGeomEntityCommon2DOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
