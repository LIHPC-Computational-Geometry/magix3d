/**
 * \file        QtTopologyBlockExtractionOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        11/05/2015
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtTopologyBlockExtractionAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologyBlockExtractionOperationPanel
// ===========================================================================

QtTopologyBlockExtractionOperationPanel::QtTopologyBlockExtractionOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).topoBlockExtractionOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).topoBlockExtractionOperationTag),
	  _namePanel (0), _topoEntitiesPanel (0)
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
	_namePanel	= new QtMgx3DGroupNamePanel (
							this, "Groupe", mainWindow, 0, creationPolicy, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// Les blocs à extraire :
	_topoEntitiesPanel	= new QtMgx3DEntityPanel (
							this, "", true, "Blocs à extraire :", "",
							&mainWindow, SelectionManagerIfc::D3,
							FilterEntity::TopoBlock);
	_topoEntitiesPanel->setMultiSelectMode (true);
	connect (_topoEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_topoEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_topoEntitiesPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getNameTextField ( ))
//	_topoEntitiesPanel->getNameTextField ( )->setLinkedSeizureManagers (
//								0, _geomEntityPanel->getNameTextField ( ));
}	// QtTopologyBlockExtractionOperationPanel::QtTopologyBlockExtractionOperationPanel


QtTopologyBlockExtractionOperationPanel::QtTopologyBlockExtractionOperationPanel (
							const QtTopologyBlockExtractionOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _topoEntitiesPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyBlockExtractionOperationPanel copy constructor is not allowed.");
}	// QtTopologyBlockExtractionOperationPanel::QtTopologyBlockExtractionOperationPanel (const QtTopologyBlockExtractionOperationPanel&)


QtTopologyBlockExtractionOperationPanel& QtTopologyBlockExtractionOperationPanel::operator = (
								const QtTopologyBlockExtractionOperationPanel&)
{
	MGX_FORBIDDEN ("QtTopologyBlockExtractionOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyBlockExtractionOperationPanel::QtTopologyBlockExtractionOperationPanel (const QtTopologyBlockExtractionOperationPanel&)


QtTopologyBlockExtractionOperationPanel::~QtTopologyBlockExtractionOperationPanel ( )
{
	preview (false, true);
}	// QtTopologyBlockExtractionOperationPanel::~QtTopologyBlockExtractionOperationPanel


string QtTopologyBlockExtractionOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtTopologyBlockExtractionOperationPanel::getGroupName


vector<string> QtTopologyBlockExtractionOperationPanel::getBlockNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	return _topoEntitiesPanel->getUniqueNames ( );
}	// QtTopologyBlockExtractionOperationPanel::getBlockNames


void QtTopologyBlockExtractionOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	_topoEntitiesPanel->reset ( );
	_namePanel->autoUpdate ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyBlockExtractionOperationPanel::reset


void QtTopologyBlockExtractionOperationPanel::validate ( )
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
		error << "QtTopologyBlockExtractionOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getTopoEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopologyBlockExtractionOperationPanel::validate


void QtTopologyBlockExtractionOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	_topoEntitiesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_topoEntitiesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyBlockExtractionOperationPanel::cancel


void QtTopologyBlockExtractionOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	_namePanel->autoUpdate ( );

	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		FilterEntity::objectType	filter	= FilterEntity::TopoBlock;
		vector<string>	entities	=
							getSelectionManager ( ).getEntitiesNames (filter);
		CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
		_topoEntitiesPanel->setUniqueNames (entities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_topoEntitiesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_topoEntitiesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyBlockExtractionOperationPanel::autoUpdate


vector<Entity*> QtTopologyBlockExtractionOperationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	names	= getBlockNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*	entity	= getContext ( ).getTopoManager ( ).getEntity (*it);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}

	return entities;
}	// QtTopologyBlockExtractionOperationPanel::getInvolvedEntities


void QtTopologyBlockExtractionOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _topoEntitiesPanel)
		_topoEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyBlockExtractionOperationPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyBlockExtractionOperationAction
// ===========================================================================

QtTopologyBlockExtractionOperationAction::QtTopologyBlockExtractionOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyBlockExtractionOperationPanel*	operationPanel	=
		new QtTopologyBlockExtractionOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			QtMgx3DGroupNamePanel::CREATION, mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyBlockExtractionOperationAction::QtTopologyBlockExtractionOperationAction


QtTopologyBlockExtractionOperationAction::QtTopologyBlockExtractionOperationAction (
								const QtTopologyBlockExtractionOperationAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyBlockExtractionOperationAction copy constructor is not allowed.")
}	// QtTopologyBlockExtractionOperationAction::QtTopologyBlockExtractionOperationAction


QtTopologyBlockExtractionOperationAction& QtTopologyBlockExtractionOperationAction::operator = (
								const QtTopologyBlockExtractionOperationAction&)
{
	MGX_FORBIDDEN ("QtTopologyBlockExtractionOperationAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyBlockExtractionOperationAction::operator =


QtTopologyBlockExtractionOperationAction::~QtTopologyBlockExtractionOperationAction ( )
{
}	// QtTopologyBlockExtractionOperationAction::~QtTopologyBlockExtractionOperationAction


QtTopologyBlockExtractionOperationPanel*
				QtTopologyBlockExtractionOperationAction::getExtractionPanel ( )
{
	return dynamic_cast<QtTopologyBlockExtractionOperationPanel*>(getOperationPanel ( ));
}	// QtTopologyBlockExtractionOperationAction::getExtractionPanel


void QtTopologyBlockExtractionOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres de copie des entités topologiques :
	QtTopologyBlockExtractionOperationPanel*	panel	= getExtractionPanel( );
	CHECK_NULL_PTR_ERROR (panel)
	const string		name		= panel->getGroupName ( );
	vector<string>		entities	= panel->getBlockNames ( );

	cmdResult	= getContext ( ).getTopoManager ( ).extract (entities, name);

	setCommandResult (cmdResult);
}	// QtTopologyBlockExtractionOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
