/**
 * \file        QtTopologySnapVerticesAction.cpp
 * \author      Charles PIGNEROL
 * \date        31/03/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologySnapVerticesAction.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologySnapVerticesPanel
// ===========================================================================

QtTopologySnapVerticesPanel::QtTopologySnapVerticesPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).snapVerticesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).snapVerticesOperationTag),
	  _verticesPanel (0), _allVerticesCheckBox (0)
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

	// Les sommets à projeter :
	_verticesPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Sommets :", "", &mainWindow,
			SelectionManagerIfc::D0, FilterEntity::TopoVertex);
	_verticesPanel->setMultiSelectMode (true);
	connect (_verticesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_verticesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_verticesPanel);

	// Tous les sommets ?
	_allVerticesCheckBox	= new QCheckBox ("Tous les sommets", this);
	connect (_allVerticesCheckBox,
	         SIGNAL (stateChanged (int)), this,
	         SLOT (parametersModifiedCallback ( )));
	layout->addWidget (_allVerticesCheckBox);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_verticesPanel->getNameTextField ( ))
	_verticesPanel->getNameTextField ( )->setLinkedSeizureManagers (0, 0);
}	// QtTopologySnapVerticesPanel::QtTopologySnapVerticesPanel


QtTopologySnapVerticesPanel::QtTopologySnapVerticesPanel (
									const QtTopologySnapVerticesPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _verticesPanel (0), _allVerticesCheckBox (0)
{
	MGX_FORBIDDEN ("QtTopologySnapVerticesPanel copy constructor is not allowed.");
}	// QtTopologySnapVerticesPanel::QtTopologySnapVerticesPanel (const QtTopologySnapVerticesPanel&)


QtTopologySnapVerticesPanel& QtTopologySnapVerticesPanel::operator = (
										const QtTopologySnapVerticesPanel&)
{
	MGX_FORBIDDEN ("QtTopologySnapVerticesPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologySnapVerticesPanel::QtTopologySnapVerticesPanel (const QtTopologySnapVerticesPanel&)


QtTopologySnapVerticesPanel::~QtTopologySnapVerticesPanel ( )
{
}	// QtTopologySnapVerticesPanel::~QtTopologySnapVerticesPanel


void QtTopologySnapVerticesPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_verticesPanel)
	_verticesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologySnapVerticesPanel::reset


void QtTopologySnapVerticesPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_verticesPanel)

	_verticesPanel->stopSelection ( );
	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_verticesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologySnapVerticesPanel::cancel


void QtTopologySnapVerticesPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	CHECK_NULL_PTR_ERROR (_allVerticesCheckBox)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedVertices	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoVertex);
		if (0 != selectedVertices.size ( ))
			_verticesPanel->setUniqueNames (selectedVertices);
		_allVerticesCheckBox->setCheckState (
				0 == getVerticesNames( ).size( ) ? Qt::Checked : Qt::Unchecked);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_verticesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_verticesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologySnapVerticesPanel::autoUpdate


bool QtTopologySnapVerticesPanel::snapAllVertices ( ) const
{
	CHECK_NULL_PTR_ERROR (_allVerticesCheckBox)
	return Qt::Checked == _allVerticesCheckBox->checkState ( ) ? true : false;
}	// QtTopologySnapVerticesPanel::snapAllVertices


vector<string> QtTopologySnapVerticesPanel::getVerticesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	return _verticesPanel->getUniqueNames ( );
}	// QtTopologySnapVerticesPanel::getVerticesNames


vector<Entity*> QtTopologySnapVerticesPanel::getInvolvedEntities ( )
{
	vector<Entity*>			entities;
	const vector<string>	names	= getVerticesNames ( );

	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*		vertex	=
			getContext ( ).getTopoManager ( ).getVertex (*it, false);
		if (0 != vertex)
			entities.push_back (vertex);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtTopologySnapVerticesPanel::getInvolvedEntities


void QtTopologySnapVerticesPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _verticesPanel)
	{
		_verticesPanel->stopSelection ( );
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologySnapVerticesPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologySnapVerticesAction
// ===========================================================================

QtTopologySnapVerticesAction::QtTopologySnapVerticesAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologySnapVerticesPanel*	operationPanel	= 
			new QtTopologySnapVerticesPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologySnapVerticesAction::QtTopologySnapVerticesAction


QtTopologySnapVerticesAction::QtTopologySnapVerticesAction (
										const QtTopologySnapVerticesAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologySnapVerticesAction copy constructor is not allowed.")
}	// QtTopologySnapVerticesAction::QtTopologySnapVerticesAction


QtTopologySnapVerticesAction& QtTopologySnapVerticesAction::operator = (
										const QtTopologySnapVerticesAction&)
{
	MGX_FORBIDDEN ("QtTopologySnapVerticesAction assignment operator is not allowed.")
	return *this;
}	// QtTopologySnapVerticesAction::operator =


QtTopologySnapVerticesAction::~QtTopologySnapVerticesAction ( )
{
}	// QtTopologySnapVerticesAction::~QtTopologySnapVerticesAction


QtTopologySnapVerticesPanel*
					QtTopologySnapVerticesAction::getTopologySnapVerticesPanel ( )
{
	return dynamic_cast<QtTopologySnapVerticesPanel*>(getOperationPanel ( ));
}	// QtTopologySnapVerticesAction::getTopologySnapVerticesPanel


void QtTopologySnapVerticesAction::executeOperation ( )
{
	QtTopologySnapVerticesPanel*	panel	= dynamic_cast<QtTopologySnapVerticesPanel*>(getTopologySnapVerticesPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de projection des sommets topologiques :
	vector<string>	vertices	= panel->getVerticesNames ( );
	bool			allVertices	= panel->snapAllVertices ( );

	if (true == allVertices)
		cmdResult	= getContext ( ).getTopoManager ( ).snapAllProjectedVertices ( );
	else
		cmdResult	= getContext ( ).getTopoManager ( ).snapProjectedVertices (vertices);

	setCommandResult (cmdResult);
}	// QtTopologySnapVerticesAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
