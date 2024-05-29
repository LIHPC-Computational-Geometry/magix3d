/**
 * \file        QtMeshSheetOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        30/01/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Mesh/MeshManagerIfc.h"
#include "QtComponents/QtMeshSheetOperationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/MemoryError.h>
#include <QtUtil/QtConfiguration.h>
#include <TkUtil/UTF8String.h>

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
//                        LA CLASSE QtMeshSheetOperationPanel
// ===========================================================================

QtMeshSheetOperationPanel::QtMeshSheetOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			const string& edgeName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (parent, mainWindow, action, helpURL, helpTag),
	  _namePanel (0), _edgePanel (0), _verticalLayout (0)
{
	_verticalLayout	= new QVBoxLayout (this);
	setLayout (_verticalLayout);

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	_verticalLayout->addWidget (label);

	// Définition du feuillet :
	QtGroupBox*	groupBox	= new QtGroupBox (QString::fromUtf8("Paramètres du feuillet"), this);
	_verticalLayout->addWidget (groupBox);
	_verticalLayout->setSpacing (5);
	_verticalLayout->setContentsMargins (0, 10, 0, 0);
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	_edgePanel	= new QtMgx3DEdgePanel (
		groupBox, "Arête définissant le feuillet", true, edgeName,
		&mainWindow, FilterEntity::TopoCoEdge);
	_edgePanel->layout ( )->setSpacing (5);
	connect (&_edgePanel->getEntityTextField ( ),
	         SIGNAL (selectionModified (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
	groupBox->setMargin (0);
	groupBox->layout ( )->setSpacing (5);
	vlayout->addWidget (_edgePanel);
	vlayout->addStretch (1000);
}	// QtMeshSheetOperationPanel::QtMeshSheetOperationPanel


QtMeshSheetOperationPanel::QtMeshSheetOperationPanel (
										const QtMeshSheetOperationPanel& cao)
	: QtMgx3DOperationPanel (
				0, *new QtMgx3DMainWindow (0,"Invalid main window"), 0, "", ""),
	  _namePanel (0), _edgePanel (0), _verticalLayout (0)
{
	MGX_FORBIDDEN ("QtMeshSheetOperationPanel copy constructor is not allowed.");
}	// QtMeshSheetOperationPanel::QtMeshSheetOperationPanel (const QtMeshSheetOperationPanel&)


QtMeshSheetOperationPanel& QtMeshSheetOperationPanel::operator = (
											const QtMeshSheetOperationPanel&)
{
	MGX_FORBIDDEN ("QtMeshSheetOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtMeshSheetOperationPanel::QtMeshSheetOperationPanel (const QtMeshSheetOperationPanel&)


QtMeshSheetOperationPanel::~QtMeshSheetOperationPanel ( )
{
}	// QtMeshSheetOperationPanel::~QtMeshSheetOperationPanel


string QtMeshSheetOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtMeshSheetOperationPanel::getGroupName


string QtMeshSheetOperationPanel::getEdgeUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
	return _edgePanel->getUniqueName ( );
}	// QtMeshSheetOperationPanel::getEdgeUniqueName


void QtMeshSheetOperationPanel::setEdgeUniqueName (const string& name)
{
	CHECK_NULL_PTR_ERROR (_edgePanel)	
	_edgePanel->setUniqueName (name);
}	// QtMeshSheetOperationPanel::setEdgeUniqueName


void QtMeshSheetOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	_namePanel->autoUpdate ( );
	_edgePanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtMeshSheetOperationPanel::reset


void QtMeshSheetOperationPanel::validate ( )
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
		error << "QtMeshSheetOperationPanel::validate : erreur non documentée.";
	}
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtMeshSheetOperationPanel::validate


void QtMeshSheetOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_edgePanel)
	_edgePanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		setEdgeUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtMeshSheetOperationPanel::cancel


void QtMeshSheetOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		//Récupération des éventuelles arêtes sélectionnées :
		vector<string>	selectedEdges	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
		if (1 == selectedEdges.size ( ))
		    setEdgeUniqueName (selectedEdges [0]);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_edgePanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	_edgePanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtMeshSheetOperationPanel::autoUpdate


vector<Entity*> QtMeshSheetOperationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const string	edgeName	= getEdgeUniqueName ( );
	Entity*			edge		=
				getContext ( ).getTopoManager ( ).getCoEdge (edgeName, false);
	if (0 != edge)
		entities.push_back (edge);

	return entities;
}	// QtMeshSheetOperationPanel::getInvolvedEntities


QVBoxLayout& QtMeshSheetOperationPanel::getVLayout ( )
{
	CHECK_NULL_PTR_ERROR (_verticalLayout)
	return *_verticalLayout;
}	// QtMeshSheetOperationPanel::getVLayout


void QtMeshSheetOperationPanel::applyCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (getMgx3DOperationAction ( ))
	getMgx3DOperationAction ( )->executeOperation ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : exécution d'une opération")

	const bool succeeded	= !hasError;
	RE_BEGIN_QT_TRY_CATCH_BLOCK

	if (true == succeeded)
	{	// Ne pas effacer la sélection, on perdrait le support de progression
//				getSelectionManager ( ).clearSelection ( );
		operationCompleted ( );
	}	// if (true == succeeded)

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : exécution d'une opération")
}	// QtMeshSheetOperationPanel::applyCallback


// ===========================================================================
//                  LA CLASSE QtMeshSheetOperationAction
// ===========================================================================

QtMeshSheetOperationAction::QtMeshSheetOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy,
	bool isCommand, bool createPanel)
	: QtMgx3DMeshOperationAction (icon, text, mainWindow, tooltip),
	  _meshExplorer (), _isCommand (isCommand)
{
	if (true == createPanel)
	{
		QtMeshSheetOperationPanel*	operationPanel	=
			new QtMeshSheetOperationPanel (
				&getOperationPanelParent ( ), text.toStdString ( ),
				creationPolicy, "", mainWindow, this,
				QtMgx3DApplication::HelpSystem::instance ( ).sheetOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).sheetOperationTag
				);
		if ((0 != operationPanel) && (getOperationPanel ( ) != operationPanel))
			setOperationPanel (operationPanel);
	}	// if (true == createPanel)
}	// QtMeshSheetOperationAction::QtMeshSheetOperationAction


QtMeshSheetOperationAction::QtMeshSheetOperationAction (const QtMeshSheetOperationAction&)
	: QtMgx3DMeshOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), ""),
	  _meshExplorer (), _isCommand (false)
{
	MGX_FORBIDDEN ("QtMeshSheetOperationAction copy constructor is not allowed.")
}	// QtMeshSheetOperationAction::QtMeshSheetOperationAction


QtMeshSheetOperationAction& QtMeshSheetOperationAction::operator = (const QtMeshSheetOperationAction&)
{
	MGX_FORBIDDEN ("QtMeshSheetOperationAction assignment operator is not allowed.")
	return *this;
}	// QtMeshSheetOperationAction::operator =


QtMeshSheetOperationAction::~QtMeshSheetOperationAction ( )
{
}	// QtMeshSheetOperationAction::~QtMeshSheetOperationAction


QtMeshSheetOperationPanel* QtMeshSheetOperationAction::getMeshSheetPanel ( )
{
	return dynamic_cast<QtMeshSheetOperationPanel*>(getOperationPanel ( ));
}	// QtMeshSheetOperationAction::getMeshSheetPanel


void QtMeshSheetOperationAction::executeOperation ( )
{
	// Destruction de l'éventuel feuillet en cours :
	releaseMeshExplorer ( );

	// Validation paramétrage :
	QtMgx3DMeshOperationAction::executeOperation ( );

	// Récupération des paramètres de création du feuillet :
	QtMeshSheetOperationPanel*	panel	= getMeshSheetPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string	edgeName	= panel->getEdgeUniqueName ( );

	setMeshExplorer (getContext ( ).getMeshManager ( ).newExplorer (0, 1, edgeName, isCommand ( )));

	// Si c'est une commande Magix 3D elle est prise en charge par le
	// gestionnaire de undo/redo, et on la libère donc de ce contexte.
	// Dans le cas contraire (ex : explorateur), on la conserve pour
	// être en mesure de faire évoluer le feuillet.
	if (true == isCommand ( ))
		releaseMeshExplorer ( );
}	// QtMeshSheetOperationAction::executeOperation


bool QtMeshSheetOperationAction::isCommand ( ) const
{
	return _isCommand;
}	// QtMeshSheetOperationAction::isCommand


CommandMeshExplorer* QtMeshSheetOperationAction::getMeshExplorer ( )
{
	return _meshExplorer.get ( );
}	// QtMeshSheetOperationAction::getMeshExplorer


CommandMeshExplorer* QtMeshSheetOperationAction::releaseMeshExplorer ( )
{
	return _meshExplorer.release ( );
}	// QtMeshSheetOperationAction::releaseMeshExplorer


void QtMeshSheetOperationAction::setMeshExplorer (CommandMeshExplorer* explorer)
{
	if (_meshExplorer.get ( ) == explorer)
		return;

	_meshExplorer.reset (explorer);
}	// QtMeshSheetOperationAction::setMeshExplorer


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
