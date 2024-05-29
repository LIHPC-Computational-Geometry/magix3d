/**
 * \file        QtGeomEntityDestructionAction.cpp
 * \author      Charles PIGNEROL
 * \date        22/01/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeomEntityDestructionAction.h"
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
//                        LA CLASSE QtGeomEntityDestructionPanel
// ===========================================================================

QtGeomEntityDestructionPanel::QtGeomEntityDestructionPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).geomDestructionOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).geomDestructionOperationTag),
	  _geomEntitiesPanel (0), _propagateCheckBox (0), _withTopoCheckBox(0)
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

	// Les entités à détruire :
	FilterEntity::objectType	filter	=
		(FilterEntity::objectType)(FilterEntity::GeomVertex |
			FilterEntity::GeomCurve | FilterEntity::GeomSurface |
			FilterEntity::GeomVolume);
	_geomEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
							this, mainWindow, "Entités géométriques :", 
							SelectionManagerIfc::ALL_DIMENSIONS,
							filter, SelectionManagerIfc::D3, true);
	_geomEntitiesPanel->setMultiSelectMode (true);
	connect (_geomEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_geomEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_geomEntitiesPanel);


	// Faut il propager la destruction aux entités de dimensions inférieures ?
	_propagateCheckBox	=
			new QCheckBox (QString::fromUtf8("Propager aux dimensions inférieures"), this);
	_propagateCheckBox->setChecked (true);
	layout->addWidget (_propagateCheckBox);

	// Faut il détruire la topologie associée ?
	_withTopoCheckBox	=
			new QCheckBox (QString::fromUtf8("Détruire la topologie associée"), this);
	_withTopoCheckBox->setChecked (false);
	layout->addWidget (_withTopoCheckBox);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
											)->setLinkedSeizureManagers (0, 0);
}	// QtGeomEntityDestructionPanel::QtGeomEntityDestructionPanel


QtGeomEntityDestructionPanel::QtGeomEntityDestructionPanel (
										const QtGeomEntityDestructionPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _geomEntitiesPanel (0), _propagateCheckBox (0), _withTopoCheckBox(0)
{
	MGX_FORBIDDEN ("QtGeomEntityDestructionPanel copy constructor is not allowed.");
}	// QtGeomEntityDestructionPanel::QtGeomEntityDestructionPanel (const QtGeomEntityDestructionPanel&)


QtGeomEntityDestructionPanel& QtGeomEntityDestructionPanel::operator = (
											const QtGeomEntityDestructionPanel&)
{
	MGX_FORBIDDEN ("QtGeomEntityDestructionPanel assignment operator is not allowed.");
	return *this;
}	// QtGeomEntityDestructionPanel::QtGeomEntityDestructionPanel (const QtGeomEntityDestructionPanel&)


QtGeomEntityDestructionPanel::~QtGeomEntityDestructionPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtGeomEntityDestructionPanel::~QtGeomEntityDestructionPanel


vector<string> QtGeomEntityDestructionPanel::getGeomEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	return _geomEntitiesPanel->getEntitiesNames ( );
}	// QtGeomEntityDestructionPanel::getGeomEntitiesNames


bool QtGeomEntityDestructionPanel::doPropagate ( ) const
{
	CHECK_NULL_PTR_ERROR (_propagateCheckBox)
	return Qt::Checked == _propagateCheckBox->checkState ( ) ? true : false;
}	// QtGeomEntityDestructionPanel::doPropagate


bool QtGeomEntityDestructionPanel::withTopo ( ) const
{
	CHECK_NULL_PTR_ERROR (_withTopoCheckBox)
	return Qt::Checked == _withTopoCheckBox->checkState ( ) ? true : false;
}	// QtGeomEntityDestructionPanel::withTopo


void QtGeomEntityDestructionPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	_geomEntitiesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeomEntityDestructionPanel::reset


void QtGeomEntityDestructionPanel::validate ( )
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
		error << "QtGeomEntityDestructionPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getGeomEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGeomEntityDestructionPanel::validate


void QtGeomEntityDestructionPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	_geomEntitiesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_geomEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeomEntityDestructionPanel::cancel


void QtGeomEntityDestructionPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		FilterEntity::objectType	filter	=
				(FilterEntity::objectType)(FilterEntity::GeomVertex |
					FilterEntity::GeomCurve | FilterEntity::GeomSurface |
					FilterEntity::GeomVolume);
		vector<string>	geomEntities	=
							getSelectionManager ( ).getEntitiesNames (filter);
		CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
		if (0 != geomEntities.size ( ))
			_geomEntitiesPanel->setEntitiesNames (geomEntities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_geomEntitiesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_geomEntitiesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtGeomEntityDestructionPanel::autoUpdate


vector<Entity*> QtGeomEntityDestructionPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	names	= getGeomEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*	entity	= getContext ( ).getGeomManager ( ).getEntity (*it);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}

	return entities;
}	// QtGeomEntityDestructionPanel::getInvolvedEntities


QtEntityByDimensionSelectorPanel* QtGeomEntityDestructionPanel::
getEntityByDimensionSelectorPanel ( )
{
	return _geomEntitiesPanel;
}   // QtGeomEntityDestructionPanel::getEntityByDimensionSelectorPanel

void QtGeomEntityDestructionPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _geomEntitiesPanel)
		_geomEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeomEntityDestructionPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtGeomEntityDestructionAction
// ===========================================================================

QtGeomEntityDestructionAction::QtGeomEntityDestructionAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeomEntityDestructionPanel*	operationPanel	=
		new QtGeomEntityDestructionPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeomEntityDestructionAction::QtGeomEntityDestructionAction


QtGeomEntityDestructionAction::QtGeomEntityDestructionAction (
										const QtGeomEntityDestructionAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeomEntityDestructionAction copy constructor is not allowed.")
}	// QtGeomEntityDestructionAction::QtGeomEntityDestructionAction


QtGeomEntityDestructionAction& QtGeomEntityDestructionAction::operator = (
										const QtGeomEntityDestructionAction&)
{
	MGX_FORBIDDEN ("QtGeomEntityDestructionAction assignment operator is not allowed.")
	return *this;
}	// QtGeomEntityDestructionAction::operator =


QtGeomEntityDestructionAction::~QtGeomEntityDestructionAction ( )
{
}	// QtGeomEntityDestructionAction::~QtGeomEntityDestructionAction


QtGeomEntityDestructionPanel*
					QtGeomEntityDestructionAction::getDestructionPanel ( )
{
	return dynamic_cast<QtGeomEntityDestructionPanel*>(getOperationPanel ( ));
}	// QtGeomEntityDestructionAction::getDestructionPanel


void QtGeomEntityDestructionAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités géométriques :
	QtGeomEntityDestructionPanel*	panel	= getDestructionPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>				entities	= panel->getGeomEntitiesNames ( );
	bool						propagate	= panel->doPropagate ( );

	if (panel->withTopo ( ))
		cmdResult	= getContext ( ).getGeomManager ( ).destroyWithTopo (entities, propagate);
	else
		cmdResult	= getContext ( ).getGeomManager ( ).destroy (entities, propagate);

	setCommandResult (cmdResult);
}	// QtGeomEntityDestructionAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
