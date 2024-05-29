/**
 * \file        QtGeomEntityCopyByOffsetAction.cpp
 * \author      Charles PIGNEROL
 * \date        25/09/2019
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Geom/Surface.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeomEntityCopyByOffsetAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"

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
//                        LA CLASSE QtGeomEntityCopyByOffsetPanel
// ===========================================================================

QtGeomEntityCopyByOffsetPanel::QtGeomEntityCopyByOffsetPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).geomCopyByOffsetURL,
			QtMgx3DApplication::HelpSystem::instance ( ).geomCopyByOffsetTag),
	  _namePanel (0), _geomEntityPanel (0), _offsetTextField (0)
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
	_namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe", mainWindow, 3, QtMgx3DGroupNamePanel::CREATION, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// L'entité à copier :
	FilterEntity::objectType	filter	=
		(FilterEntity::objectType)(FilterEntity::GeomVertex |
			FilterEntity::GeomCurve | FilterEntity::GeomSurface |
			FilterEntity::GeomVolume);
	_geomEntityPanel	= new QtMgx3DEntityPanel (this, "", true, "Surface :", "", &mainWindow, SelectionManagerIfc::D2, FilterEntity::GeomSurface);
	_geomEntityPanel->setMultiSelectMode (false);
	connect (_geomEntityPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_geomEntityPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_geomEntityPanel);

	// Le décallage entre l'entité copiée et sa copie :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	_offsetTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	label	= new QLabel (QString::fromUtf8 ("Distance :"), this);
	hlayout->addWidget (label);
	hlayout->addWidget (_offsetTextField);
	hlayout->addStretch (20);

	layout->addStretch (2);
}	// QtGeomEntityCopyByOffsetPanel::QtGeomEntityCopyByOffsetPanel


QtGeomEntityCopyByOffsetPanel::QtGeomEntityCopyByOffsetPanel (
										const QtGeomEntityCopyByOffsetPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _geomEntityPanel (0), _offsetTextField (0)
{
	MGX_FORBIDDEN ("QtGeomEntityCopyByOffsetPanel copy constructor is not allowed.");
}	// QtGeomEntityCopyByOffsetPanel::QtGeomEntityCopyByOffsetPanel (const QtGeomEntityCopyByOffsetPanel&)


QtGeomEntityCopyByOffsetPanel& QtGeomEntityCopyByOffsetPanel::operator = (
											const QtGeomEntityCopyByOffsetPanel&)
{
	MGX_FORBIDDEN ("QtGeomEntityCopyByOffsetPanel assignment operator is not allowed.");
	return *this;
}	// QtGeomEntityCopyByOffsetPanel::QtGeomEntityCopyByOffsetPanel (const QtGeomEntityCopyByOffsetPanel&)


QtGeomEntityCopyByOffsetPanel::~QtGeomEntityCopyByOffsetPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtGeomEntityCopyByOffsetPanel::~QtGeomEntityCopyByOffsetPanel


string QtGeomEntityCopyByOffsetPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtGeomEntityCopyByOffsetPanel::getGroupName


string QtGeomEntityCopyByOffsetPanel::getGeomEntityName ( ) const
{
	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
	return _geomEntityPanel->getUniqueName ( );
}	// QtGeomEntityCopyByOffsetPanel::getGeomEntityName


double QtGeomEntityCopyByOffsetPanel::getOffset ( ) const
{
	CHECK_NULL_PTR_ERROR (_offsetTextField)
	return _offsetTextField->getValue ( );
}	// QtGeomEntityCopyByOffsetPanel::getOffset


void QtGeomEntityCopyByOffsetPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
	CHECK_NULL_PTR_ERROR (_offsetTextField)
	_namePanel->autoUpdate ( );
	_geomEntityPanel->reset ( );
	_offsetTextField->clear ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeomEntityCopyByOffsetPanel::reset


void QtGeomEntityCopyByOffsetPanel::validate ( )
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
		error << "QtGeomEntityCopyByOffsetPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getGeomEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGeomEntityCopyByOffsetPanel::validate


void QtGeomEntityCopyByOffsetPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
	_geomEntityPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_geomEntityPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeomEntityCopyByOffsetPanel::cancel


void QtGeomEntityCopyByOffsetPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
	CHECK_NULL_PTR_ERROR (_offsetTextField)
	_namePanel->autoUpdate ( );
	_geomEntityPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtGeomEntityCopyByOffsetPanel::autoUpdate


vector<Entity*> QtGeomEntityCopyByOffsetPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const string	name	= getGeomEntityName ( );
	if (false == name.empty ( ))
	{
		Geom::Surface*	surface	= getContext ( ).getGeomManager ( ).getSurface (name);
		if (0 != surface)
			entities.push_back (dynamic_cast<Entity*>(surface));
	}

	return entities;
}	// QtGeomEntityCopyByOffsetPanel::getInvolvedEntities


void QtGeomEntityCopyByOffsetPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _geomEntityPanel)
		_geomEntityPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeomEntityCopyByOffsetPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtGeomEntityCopyByOffsetAction
// ===========================================================================

QtGeomEntityCopyByOffsetAction::QtGeomEntityCopyByOffsetAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeomEntityCopyByOffsetPanel*	operationPanel	=
		new QtGeomEntityCopyByOffsetPanel (&getOperationPanelParent ( ), text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeomEntityCopyByOffsetAction::QtGeomEntityCopyByOffsetAction


QtGeomEntityCopyByOffsetAction::QtGeomEntityCopyByOffsetAction (const QtGeomEntityCopyByOffsetAction&)
	: QtMgx3DGeomOperationAction (QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeomEntityCopyByOffsetAction copy constructor is not allowed.")
}	// QtGeomEntityCopyByOffsetAction::QtGeomEntityCopyByOffsetAction


QtGeomEntityCopyByOffsetAction& QtGeomEntityCopyByOffsetAction::operator = (const QtGeomEntityCopyByOffsetAction&)
{
	MGX_FORBIDDEN ("QtGeomEntityCopyByOffsetAction assignment operator is not allowed.")
	return *this;
}	// QtGeomEntityCopyByOffsetAction::operator =


QtGeomEntityCopyByOffsetAction::~QtGeomEntityCopyByOffsetAction ( )
{
}	// QtGeomEntityCopyByOffsetAction::~QtGeomEntityCopyByOffsetAction


QtGeomEntityCopyByOffsetPanel* QtGeomEntityCopyByOffsetAction::getCopyPanel ( )
{
	return dynamic_cast<QtGeomEntityCopyByOffsetPanel*>(getOperationPanel ( ));
}	// QtGeomEntityCopyByOffsetAction::getCopyPanel


void QtGeomEntityCopyByOffsetAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de copie des entités géométriques :
	QtGeomEntityCopyByOffsetPanel*	panel	= getCopyPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string		groupName	= panel->getGroupName ( );
	const string		entityName	= panel->getGeomEntityName ( );
	const double		offset		= panel->getOffset ( );

	cmdResult	= getContext ( ).getGeomManager ( ).newSurfaceByOffset (entityName, offset, groupName);

	setCommandResult (cmdResult);
}	// QtGeomEntityCopyByOffsetAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
