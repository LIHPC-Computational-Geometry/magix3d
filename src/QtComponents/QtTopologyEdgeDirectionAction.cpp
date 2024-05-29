/**
 * \file        QtTopologyEdgeDirectionAction.cpp
 * \author      Charles PIGNEROL
 * \date        18/12/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyEdgeDirectionAction.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologyEdgeDirectionPanel
// ===========================================================================

QtTopologyEdgeDirectionPanel::QtTopologyEdgeDirectionPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).edgeDirectionOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).edgeDirectionOperationTag),
	  _edgesPanel (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);

	// Le nom du panneau :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Les arêtes à modifier :
	_edgesPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Arêtes :", "", &mainWindow,
			SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	_edgesPanel->setMultiSelectMode (true);
	connect (_edgesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edgesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_edgesPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_edgesPanel->getNameTextField ( ))
	_edgesPanel->getNameTextField ( )->setLinkedSeizureManagers (0, 0);
}	// QtTopologyEdgeDirectionPanel::QtTopologyEdgeDirectionPanel


QtTopologyEdgeDirectionPanel::QtTopologyEdgeDirectionPanel (
										const QtTopologyEdgeDirectionPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _edgesPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyEdgeDirectionPanel copy constructor is not allowed.");
}	// QtTopologyEdgeDirectionPanel::QtTopologyEdgeDirectionPanel (const QtTopologyEdgeDirectionPanel&)


QtTopologyEdgeDirectionPanel& QtTopologyEdgeDirectionPanel::operator = (
											const QtTopologyEdgeDirectionPanel&)
{
	MGX_FORBIDDEN ("QtTopologyEdgeDirectionPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyEdgeDirectionPanel::QtTopologyEdgeDirectionPanel (const QtTopologyEdgeDirectionPanel&)


QtTopologyEdgeDirectionPanel::~QtTopologyEdgeDirectionPanel ( )
{
}	// QtTopologyEdgeDirectionPanel::~QtTopologyEdgeDirectionPanel


void QtTopologyEdgeDirectionPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgesPanel)
	_edgesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyEdgeDirectionPanel::reset


void QtTopologyEdgeDirectionPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_edgesPanel)

	_edgesPanel->stopSelection ( );
	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_edgesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyEdgeDirectionPanel::cancel


void QtTopologyEdgeDirectionPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_edgesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedEdges	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
		if (0 != selectedEdges.size ( ))
			_edgesPanel->setUniqueNames (selectedEdges);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_edgesPanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	_edgesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyEdgeDirectionPanel::autoUpdate


vector<string> QtTopologyEdgeDirectionPanel::getEdgesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgesPanel)
	return _edgesPanel->getUniqueNames ( );
}	// QtTopologyEdgeDirectionPanel::getEdgesNames


vector<Entity*> QtTopologyEdgeDirectionPanel::getInvolvedEntities ( )
{
	vector<Entity*>			entities;
	const vector<string>	names	= getEdgesNames ( );

	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*		edge	=
			getContext ( ).getTopoManager ( ).getCoEdge (*it, false);
		if (0 != edge)
			entities.push_back (edge);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtTopologyEdgeDirectionPanel::getInvolvedEntities


void QtTopologyEdgeDirectionPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _edgesPanel)
	{
		_edgesPanel->stopSelection ( );
		_edgesPanel->setUniqueName ("");
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyEdgeDirectionPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyEdgeDirectionAction
// ===========================================================================

QtTopologyEdgeDirectionAction::QtTopologyEdgeDirectionAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyEdgeDirectionPanel*	operationPanel	= 
			new QtTopologyEdgeDirectionPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyEdgeDirectionAction::QtTopologyEdgeDirectionAction


QtTopologyEdgeDirectionAction::QtTopologyEdgeDirectionAction (
										const QtTopologyEdgeDirectionAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyEdgeDirectionAction copy constructor is not allowed.")
}	// QtTopologyEdgeDirectionAction::QtTopologyEdgeDirectionAction


QtTopologyEdgeDirectionAction& QtTopologyEdgeDirectionAction::operator = (
										const QtTopologyEdgeDirectionAction&)
{
	MGX_FORBIDDEN ("QtTopologyEdgeDirectionAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyEdgeDirectionAction::operator =


QtTopologyEdgeDirectionAction::~QtTopologyEdgeDirectionAction ( )
{
}	// QtTopologyEdgeDirectionAction::~QtTopologyEdgeDirectionAction


QtTopologyEdgeDirectionPanel*
					QtTopologyEdgeDirectionAction::getTopologyEdgeDirectionPanel ( )
{
	return dynamic_cast<QtTopologyEdgeDirectionPanel*>(getOperationPanel ( ));
}	// QtTopologyEdgeDirectionAction::getTopologyEdgeDirectionPanel


void QtTopologyEdgeDirectionAction::executeOperation ( )
{
	QtTopologyEdgeDirectionPanel*	panel	= dynamic_cast<QtTopologyEdgeDirectionPanel*>(getTopologyEdgeDirectionPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de modification des arêtes topologiques :
	vector<string>	edges	= panel->getEdgesNames ( );

	cmdResult	= getContext ( ).getTopoManager ( ).reverseDirection (edges);

	setCommandResult (cmdResult);
}	// QtTopologyEdgeDirectionAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
