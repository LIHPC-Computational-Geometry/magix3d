/**
 * \file        QtTopologyFuse2VerticesAction.cpp
 * \author      Eric Brière de l'Isle
 * \date        28/11/2017
 */

#include "Internal/ContextIfc.h"

#include <QtComponents/QtTopologyFuse2VerticesAction.h>
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
//                        LA CLASSE QtTopologyFuse2VerticesPanel
// ===========================================================================

QtTopologyFuse2VerticesPanel::QtTopologyFuse2VerticesPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).fuse2verticesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).fuse2verticesOperationTag),
	  _vertex1Panel (0), _vertex2Panel (0)
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

	// Le sommet 1 à coller :
	_vertex1Panel	= new QtMgx3DEntityPanel (
			this, "", true, "Sommet 1 :", "", &mainWindow,
			SelectionManagerIfc::D0, FilterEntity::TopoVertex);
//	_vertex1Panel->setMultiSelectMode (true);
	connect (_vertex1Panel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_vertex1Panel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_vertex1Panel);

	// Le sommet 2 à coller :
	_vertex2Panel	= new QtMgx3DEntityPanel (
			this, "", true, "Sommet 2 :", "", &mainWindow,
			SelectionManagerIfc::D0, FilterEntity::TopoVertex);
//	_vertex2Panel->setMultiSelectMode (true);
	connect (_vertex2Panel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_vertex2Panel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_vertex2Panel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_vertex1Panel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_vertex2Panel->getNameTextField ( ))
	_vertex1Panel->getNameTextField (
			)->setLinkedSeizureManagers (0, _vertex2Panel->getNameTextField ( ));
	_vertex2Panel->getNameTextField (
			)->setLinkedSeizureManagers (_vertex1Panel->getNameTextField ( ), 0);
}	// QtTopologyFuse2VerticesPanel::QtTopologyFuse2VerticesPanel


QtTopologyFuse2VerticesPanel::QtTopologyFuse2VerticesPanel (
										const QtTopologyFuse2VerticesPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _vertex1Panel (0), _vertex2Panel (0)
{
	MGX_FORBIDDEN ("QtTopologyFuse2VerticesPanel copy constructor is not allowed.");
}	// QtTopologyFuse2VerticesPanel::QtTopologyFuse2VerticesPanel (const QtTopologyFuse2VerticesPanel&)


QtTopologyFuse2VerticesPanel& QtTopologyFuse2VerticesPanel::operator = (
											const QtTopologyFuse2VerticesPanel&)
{
	MGX_FORBIDDEN ("QtTopologyFuse2VerticesPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyFuse2VerticesPanel::QtTopologyFuse2VerticesPanel (const QtTopologyFuse2VerticesPanel&)


QtTopologyFuse2VerticesPanel::~QtTopologyFuse2VerticesPanel ( )
{
}	// QtTopologyFuse2VerticesPanel::~QtTopologyFuse2VerticesPanel


void QtTopologyFuse2VerticesPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	_vertex1Panel->reset ( );
	_vertex2Panel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyFuse2VerticesPanel::reset


void QtTopologyFuse2VerticesPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)

	_vertex1Panel->stopSelection ( );
	_vertex2Panel->stopSelection ( );
	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_vertex1Panel->setUniqueName ("");
		_vertex2Panel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyFuse2VerticesPanel::cancel


void QtTopologyFuse2VerticesPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)

#ifdef AUTO_UPDATE_OLD_SCHEME
//	if (true == autoUpdateUsesSelection ( ))
//	{
//		BEGIN_QT_TRY_CATCH_BLOCK
//
//		vector<string>	selectedEdges	=
//			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoVertex);
//		if (2 >= selectedEdges.size ( ))
//		{
//			if (1 <= selectedEdges.size ( ))
//				_vertex1Panel->setUniqueName (selectedEdges [0]);
//			if (2 == selectedEdges.size ( ))
//				_vertex2Panel->setUniqueName (selectedEdges [1]);
//		}	// if (2 >= selectedEdges.size ( ))
//
//		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
//	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_vertex1Panel->clearSelection ( );
	_vertex2Panel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_vertex1Panel->actualizeGui (true);
	_vertex2Panel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyFuse2VerticesPanel::autoUpdate


vector<string> QtTopologyFuse2VerticesPanel::getvertex1Names ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	return _vertex1Panel->getUniqueNames ( );
}	// QtTopologyFuse2VerticesPanel::getvertex1Names


vector<string> QtTopologyFuse2VerticesPanel::getvertex2Names ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	return _vertex2Panel->getUniqueNames ( );
}	// QtTopologyFuse2VerticesPanel::getvertex2Names


vector<Entity*> QtTopologyFuse2VerticesPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const vector<string>	names1	= getvertex1Names ( );
	const vector<string>	names2	= getvertex2Names ( );
	for (vector<string>::const_iterator it1 = names1.begin ( );
	     names1.end ( ) != it1; it1++)
	{
		TopoEntity*		vertex1	=
			getContext ( ).getTopoManager ( ).getVertex (*it1, false);
		if (0 != vertex1)
			entities.push_back (vertex1);
	}	// for (vector<string>::const_iterator it1 = names1.begin ( ); ...
	for (vector<string>::const_iterator it2 = names2.begin ( );
	     names2.end ( ) != it2; it2++)
	{
		TopoEntity*		vertex2	=
			getContext ( ).getTopoManager ( ).getVertex (*it2, false);
		if (0 != vertex2)
			entities.push_back (vertex2);
	}	// for (vector<string>::const_iterator it2 = names1.begin ( ); ...

	return entities;
}	// QtTopologyFuse2VerticesPanel::getInvolvedEntities


void QtTopologyFuse2VerticesPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _vertex1Panel)
	{
		_vertex1Panel->stopSelection ( );
		_vertex1Panel->setUniqueName ("");
	}
	if (0 != _vertex2Panel)
	{
		_vertex2Panel->stopSelection ( );
		_vertex2Panel->setUniqueName ("");
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyFuse2VerticesPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyFuse2VerticesAction
// ===========================================================================

QtTopologyFuse2VerticesAction::QtTopologyFuse2VerticesAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyFuse2VerticesPanel*	operationPanel	=
			new QtTopologyFuse2VerticesPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyFuse2VerticesAction::QtTopologyFuse2VerticesAction


QtTopologyFuse2VerticesAction::QtTopologyFuse2VerticesAction (
										const QtTopologyFuse2VerticesAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyFuse2VerticesAction copy constructor is not allowed.")
}	// QtTopologyFuse2VerticesAction::QtTopologyFuse2VerticesAction


QtTopologyFuse2VerticesAction& QtTopologyFuse2VerticesAction::operator = (
										const QtTopologyFuse2VerticesAction&)
{
	MGX_FORBIDDEN ("QtTopologyFuse2VerticesAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyFuse2VerticesAction::operator =


QtTopologyFuse2VerticesAction::~QtTopologyFuse2VerticesAction ( )
{
}	// QtTopologyFuse2VerticesAction::~QtTopologyFuse2VerticesAction


QtTopologyFuse2VerticesPanel*
					QtTopologyFuse2VerticesAction::getTopologyFuse2VerticesPanel ( )
{
	return dynamic_cast<QtTopologyFuse2VerticesPanel*>(getOperationPanel ( ));
}	// QtTopologyFuse2VerticesAction::getTopologyFuse2VerticesPanel


void QtTopologyFuse2VerticesAction::executeOperation ( )
{
	QtTopologyFuse2VerticesPanel*	panel	= dynamic_cast<QtTopologyFuse2VerticesPanel*>(getTopologyFuse2VerticesPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de collage des sommets topologiques :
	vector<string>	names1	= panel->getvertex1Names ( );
	vector<string>	names2	= panel->getvertex2Names ( );
	// On s'assure ici de n'avoir que 2 fois 1 unique nom. Si nom non unique on
	// n'en met pas et le noyau renverra une erreur.
	string	name1, name2;
	if (names1.size ( ) == 1)
		name1	= names1 [0];
	if (names2.size ( ) == 1)
		name2	= names2 [0];

	cmdResult	= getContext ( ).getTopoManager ( ).fuse2Vertices (name1, name2);

	setCommandResult (cmdResult);
}	// QtTopologyFuse2VerticesAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
