/**
 * \file        QtTopologyFuse2EdgesAction.cpp
 * \author      Charles PIGNEROL
 * \date        28/06/2016
 */

#include "Internal/ContextIfc.h"

#include <QtComponents/QtTopologyFuse2EdgesAction.h>
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


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologyFuse2EdgesPanel
// ===========================================================================

QtTopologyFuse2EdgesPanel::QtTopologyFuse2EdgesPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).fuse2EdgesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).fuse2EdgesOperationTag),
	  _edges1Panel (0), _edges2Panel (0)
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

	// L'arête 1 à coller :
	_edges1Panel	= new QtMgx3DEntityPanel (
			this, "", true, "Arêtes 1 :", "", &mainWindow,
			SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	_edges1Panel->setMultiSelectMode (true);
	connect (_edges1Panel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edges1Panel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_edges1Panel);

	// L'arête 2 à coller :
	_edges2Panel	= new QtMgx3DEntityPanel (
			this, "", true, "Arêtes 2 :", "", &mainWindow,
			SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	_edges2Panel->setMultiSelectMode (true);
	connect (_edges2Panel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edges2Panel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_edges2Panel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_edges1Panel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_edges2Panel->getNameTextField ( ))
	_edges1Panel->getNameTextField (
			)->setLinkedSeizureManagers (0, _edges2Panel->getNameTextField ( ));
	_edges2Panel->getNameTextField (
			)->setLinkedSeizureManagers (_edges1Panel->getNameTextField ( ), 0);
}	// QtTopologyFuse2EdgesPanel::QtTopologyFuse2EdgesPanel


QtTopologyFuse2EdgesPanel::QtTopologyFuse2EdgesPanel (
										const QtTopologyFuse2EdgesPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _edges1Panel (0), _edges2Panel (0)
{
	MGX_FORBIDDEN ("QtTopologyFuse2EdgesPanel copy constructor is not allowed.");
}	// QtTopologyFuse2EdgesPanel::QtTopologyFuse2EdgesPanel (const QtTopologyFuse2EdgesPanel&)


QtTopologyFuse2EdgesPanel& QtTopologyFuse2EdgesPanel::operator = (
											const QtTopologyFuse2EdgesPanel&)
{
	MGX_FORBIDDEN ("QtTopologyFuse2EdgesPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyFuse2EdgesPanel::QtTopologyFuse2EdgesPanel (const QtTopologyFuse2EdgesPanel&)


QtTopologyFuse2EdgesPanel::~QtTopologyFuse2EdgesPanel ( )
{
}	// QtTopologyFuse2EdgesPanel::~QtTopologyFuse2EdgesPanel


void QtTopologyFuse2EdgesPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edges1Panel)
	CHECK_NULL_PTR_ERROR (_edges2Panel)
	_edges1Panel->reset ( );
	_edges2Panel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyFuse2EdgesPanel::reset


void QtTopologyFuse2EdgesPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_edges1Panel)
	CHECK_NULL_PTR_ERROR (_edges2Panel)

	_edges1Panel->stopSelection ( );
	_edges2Panel->stopSelection ( );
	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_edges1Panel->setUniqueName ("");
		_edges2Panel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyFuse2EdgesPanel::cancel


void QtTopologyFuse2EdgesPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_edges1Panel)
	CHECK_NULL_PTR_ERROR (_edges2Panel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedEdges	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
		if (2 >= selectedEdges.size ( ))
		{
			if (1 <= selectedEdges.size ( ))
				_edges1Panel->setUniqueName (selectedEdges [0]);
			if (2 == selectedEdges.size ( ))
				_edges2Panel->setUniqueName (selectedEdges [1]);
		}	// if (2 >= selectedEdges.size ( ))

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_edges1Panel->clearSelection ( );
	_edges2Panel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_edges1Panel->actualizeGui (true);
	_edges2Panel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyFuse2EdgesPanel::autoUpdate


vector<string> QtTopologyFuse2EdgesPanel::getEdges1Names ( ) const
{
	CHECK_NULL_PTR_ERROR (_edges1Panel)
	return _edges1Panel->getUniqueNames ( );
}	// QtTopologyFuse2EdgesPanel::getEdges1Names


vector<string> QtTopologyFuse2EdgesPanel::getEdges2Names ( ) const
{
	CHECK_NULL_PTR_ERROR (_edges2Panel)
	return _edges2Panel->getUniqueNames ( );
}	// QtTopologyFuse2EdgesPanel::getEdges2Names


vector<Entity*> QtTopologyFuse2EdgesPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const vector<string>	names1	= getEdges1Names ( );
	const vector<string>	names2	= getEdges2Names ( );
	for (vector<string>::const_iterator it1 = names1.begin ( );
	     names1.end ( ) != it1; it1++)
	{
		TopoEntity*		edge1	=
			getContext ( ).getTopoManager ( ).getCoEdge (*it1, false);
		if (0 != edge1)
			entities.push_back (edge1);
	}	// for (vector<string>::const_iterator it1 = names1.begin ( ); ...
	for (vector<string>::const_iterator it2 = names2.begin ( );
	     names2.end ( ) != it2; it2++)
	{
		TopoEntity*		edge2	=
			getContext ( ).getTopoManager ( ).getCoEdge (*it2, false);
		if (0 != edge2)
			entities.push_back (edge2);
	}	// for (vector<string>::const_iterator it2 = names1.begin ( ); ...

	return entities;
}	// QtTopologyFuse2EdgesPanel::getInvolvedEntities


void QtTopologyFuse2EdgesPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _edges1Panel)
	{
		_edges1Panel->stopSelection ( );
		_edges1Panel->setUniqueName ("");
	}
	if (0 != _edges2Panel)
	{
		_edges2Panel->stopSelection ( );	
		_edges2Panel->setUniqueName ("");	
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyFuse2EdgesPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyFuse2EdgesAction
// ===========================================================================

QtTopologyFuse2EdgesAction::QtTopologyFuse2EdgesAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyFuse2EdgesPanel*	operationPanel	=
			new QtTopologyFuse2EdgesPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyFuse2EdgesAction::QtTopologyFuse2EdgesAction


QtTopologyFuse2EdgesAction::QtTopologyFuse2EdgesAction (
										const QtTopologyFuse2EdgesAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyFuse2EdgesAction copy constructor is not allowed.")
}	// QtTopologyFuse2EdgesAction::QtTopologyFuse2EdgesAction


QtTopologyFuse2EdgesAction& QtTopologyFuse2EdgesAction::operator = (
										const QtTopologyFuse2EdgesAction&)
{
	MGX_FORBIDDEN ("QtTopologyFuse2EdgesAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyFuse2EdgesAction::operator =


QtTopologyFuse2EdgesAction::~QtTopologyFuse2EdgesAction ( )
{
}	// QtTopologyFuse2EdgesAction::~QtTopologyFuse2EdgesAction


QtTopologyFuse2EdgesPanel*
					QtTopologyFuse2EdgesAction::getTopologyFuse2EdgesPanel ( )
{
	return dynamic_cast<QtTopologyFuse2EdgesPanel*>(getOperationPanel ( ));
}	// QtTopologyFuse2EdgesAction::getTopologyFuse2EdgesPanel


void QtTopologyFuse2EdgesAction::executeOperation ( )
{
	QtTopologyFuse2EdgesPanel*	panel	= dynamic_cast<QtTopologyFuse2EdgesPanel*>(getTopologyFuse2EdgesPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de collage des arêtes topologiques :
	vector<string>	names1	= panel->getEdges1Names ( );
	vector<string>	names2	= panel->getEdges2Names ( );

	Mgx3D::Internal::M3DCommandResultIfc*	result	= 0;
	if ((names1.size ( ) == 1) && (names2.size ( ) == 1))
		result	= getContext ( ).getTopoManager ( ).fuse2Edges (names1[0], names2[0]);
	else
		result	= getContext ( ).getTopoManager ( ).fuse2EdgeList (names1, names2);
	CHECK_NULL_PTR_ERROR (result)
	setCommandResult (result);
	if (CommandIfc::FAIL == result->getStatus ( ))
		throw Exception (result->getErrorMessage ( ));
	else if (CommandIfc::CANCELED == result->getStatus ( ))
		throw Exception ("Opération annulée");
}	// QtTopologyFuse2EdgesAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
