/**
 * \file        QtTopologyFuseEdgesAction.cpp
 * \author      Charles PIGNEROL
 * \date        25/03/2014
 */

#include "Internal/ContextIfc.h"

#include <QtComponents/QtTopologyFuseEdgesAction.h>
#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
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
//                        LA CLASSE QtTopologyFuseEdgesPanel
// ===========================================================================

QtTopologyFuseEdgesPanel::QtTopologyFuseEdgesPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).fuseEdgesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).fuseEdgesOperationTag),
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

	// Les arêtes à coller :
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
}	// QtTopologyFuseEdgesPanel::QtTopologyFuseEdgesPanel


QtTopologyFuseEdgesPanel::QtTopologyFuseEdgesPanel (
										const QtTopologyFuseEdgesPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _edgesPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyFuseEdgesPanel copy constructor is not allowed.");
}	// QtTopologyFuseEdgesPanel::QtTopologyFuseEdgesPanel (const QtTopologyFuseEdgesPanel&)


QtTopologyFuseEdgesPanel& QtTopologyFuseEdgesPanel::operator = (
											const QtTopologyFuseEdgesPanel&)
{
	MGX_FORBIDDEN ("QtTopologyFuseEdgesPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyFuseEdgesPanel::QtTopologyFuseEdgesPanel (const QtTopologyFuseEdgesPanel&)


QtTopologyFuseEdgesPanel::~QtTopologyFuseEdgesPanel ( )
{
}	// QtTopologyFuseEdgesPanel::~QtTopologyFuseEdgesPanel


void QtTopologyFuseEdgesPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgesPanel)
	_edgesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyFuseEdgesPanel::reset


void QtTopologyFuseEdgesPanel::cancel ( )
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
}	// QtTopologyFuseEdgesPanel::cancel


void QtTopologyFuseEdgesPanel::autoUpdate ( )
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
	_edgesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_edgesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyFuseEdgesPanel::autoUpdate


vector<string> QtTopologyFuseEdgesPanel::getEdgesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgesPanel)
	return _edgesPanel->getUniqueNames ( );
}	// QtTopologyFuseEdgesPanel::getEdgesNames


vector<Entity*> QtTopologyFuseEdgesPanel::getInvolvedEntities ( )
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
}	// QtTopologyFuseEdgesPanel::getInvolvedEntities


void QtTopologyFuseEdgesPanel::operationCompleted ( )
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
}	// QtTopologyFuseEdgesPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyFuseEdgesAction
// ===========================================================================

QtTopologyFuseEdgesAction::QtTopologyFuseEdgesAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyFuseEdgesPanel*	operationPanel	= 
			new QtTopologyFuseEdgesPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyFuseEdgesAction::QtTopologyFuseEdgesAction


QtTopologyFuseEdgesAction::QtTopologyFuseEdgesAction (
										const QtTopologyFuseEdgesAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyFuseEdgesAction copy constructor is not allowed.")
}	// QtTopologyFuseEdgesAction::QtTopologyFuseEdgesAction


QtTopologyFuseEdgesAction& QtTopologyFuseEdgesAction::operator = (
										const QtTopologyFuseEdgesAction&)
{
	MGX_FORBIDDEN ("QtTopologyFuseEdgesAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyFuseEdgesAction::operator =


QtTopologyFuseEdgesAction::~QtTopologyFuseEdgesAction ( )
{
}	// QtTopologyFuseEdgesAction::~QtTopologyFuseEdgesAction


QtTopologyFuseEdgesPanel*
					QtTopologyFuseEdgesAction::getTopologyFuseEdgesPanel ( )
{
	return dynamic_cast<QtTopologyFuseEdgesPanel*>(getOperationPanel ( ));
}	// QtTopologyFuseEdgesAction::getTopologyFuseEdgesPanel


void QtTopologyFuseEdgesAction::executeOperation ( )
{
	QtTopologyFuseEdgesPanel*	panel	= dynamic_cast<QtTopologyFuseEdgesPanel*>(getTopologyFuseEdgesPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de collage des arêtes topologiques :
	vector<string>	edges	= panel->getEdgesNames ( );

	Mgx3D::Internal::M3DCommandResultIfc*	result	= getContext ( ).getTopoManager ( ).fuseEdges (edges);
	CHECK_NULL_PTR_ERROR (result)
	setCommandResult (result);
	if (CommandIfc::FAIL == result->getStatus ( ))
		throw Exception (result->getErrorMessage ( ));
	else if (CommandIfc::CANCELED == result->getStatus ( ))
		throw Exception ("Opération annulée");
}	// QtTopologyFuseEdgesAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
