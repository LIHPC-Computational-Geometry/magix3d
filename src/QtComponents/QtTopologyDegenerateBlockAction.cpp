/**
 * \file        QtTopologyDegenerateBlockAction.cpp
 * \author      Charles PIGNEROL
 * \date        07/03/2014
 */

#include "Internal/ContextIfc.h"

#include <QtComponents/QtTopologyDegenerateBlockAction.h>
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
//                        LA CLASSE QtTopologyDegenerateBlockPanel
// ===========================================================================

QtTopologyDegenerateBlockPanel::QtTopologyDegenerateBlockPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).degenerateBlockOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).degenerateBlockOperationTag),
	  _vertex1Panel (0), _vertex2Panel (0),
	  _projectOnFirstCheckbox (0)
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

	// Le vertex 1 à coller :
	SelectionManagerIfc::DIM	ALLOWED_DIMENSIONS	=
		(SelectionManagerIfc::DIM)(
			SelectionManagerIfc::D0 | SelectionManagerIfc::D1 |
			SelectionManagerIfc::D2);
	FilterEntity::objectType	filter	= (FilterEntity::objectType)(
			FilterEntity::TopoVertex | FilterEntity::TopoCoEdge |
			FilterEntity::TopoCoFace);
	_vertex1Panel	= new QtEntityByDimensionSelectorPanel (
			this, mainWindow, "Entité 1 :", ALLOWED_DIMENSIONS,
			filter, SelectionManagerIfc::D0, false);
	connect (_vertex1Panel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_vertex1Panel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_vertex1Panel,
	         SIGNAL (dimensionsModified ( )), this,
	         SLOT (dimensionModifiedCallback ( )));
	layout->addWidget (_vertex1Panel);

	// Le vertex 2 à coller :
	_vertex2Panel	= new QtEntityByDimensionSelectorPanel (
			this, mainWindow, "Entité 2 :", ALLOWED_DIMENSIONS,
			filter, SelectionManagerIfc::D0, false);
	connect (_vertex2Panel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_vertex2Panel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_vertex2Panel,
	         SIGNAL (dimensionsModified ( )), this,
	         SLOT (dimensionModifiedCallback ( )));
	layout->addWidget (_vertex2Panel);

	// Comment doit être effectuée la projection ?
	_projectOnFirstCheckbox	=
						new QCheckBox ("Projeter sur le premier sommet", this);
	_projectOnFirstCheckbox->setChecked (true);
	_projectOnFirstCheckbox->setToolTip (QString::fromUtf8("Cochée la projetion du second (ensemble de) sommet(s) est effectuée sur le premier. Non cochée le(s) sommet(s) résultant(s) est(sont) situé(s) entre le(s) premier(s) et second(s) (ensembles de) sommet(s)."));
	layout->addWidget (_projectOnFirstCheckbox);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_vertex1Panel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_vertex1Panel->getEntitiesPanel ( )->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_vertex2Panel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_vertex2Panel->getEntitiesPanel ( )->getNameTextField ( ))
	_vertex1Panel->getEntitiesPanel ( )->getNameTextField (
			)->setLinkedSeizureManagers (
				0, _vertex2Panel->getEntitiesPanel ( )->getNameTextField ( ));
	_vertex2Panel->getEntitiesPanel ( )->getNameTextField (
			)->setLinkedSeizureManagers (
				_vertex1Panel->getEntitiesPanel ( )->getNameTextField ( ), 0);
}	// QtTopologyDegenerateBlockPanel::QtTopologyDegenerateBlockPanel


QtTopologyDegenerateBlockPanel::QtTopologyDegenerateBlockPanel (
										const QtTopologyDegenerateBlockPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _vertex1Panel (0), _vertex2Panel (0),
	  _projectOnFirstCheckbox (0)
{
	MGX_FORBIDDEN ("QtTopologyDegenerateBlockPanel copy constructor is not allowed.");
}	// QtTopologyDegenerateBlockPanel::QtTopologyDegenerateBlockPanel (const QtTopologyDegenerateBlockPanel&)


QtTopologyDegenerateBlockPanel& QtTopologyDegenerateBlockPanel::operator = (
											const QtTopologyDegenerateBlockPanel&)
{
	MGX_FORBIDDEN ("QtTopologyDegenerateBlockPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyDegenerateBlockPanel::QtTopologyDegenerateBlockPanel (const QtTopologyDegenerateBlockPanel&)


QtTopologyDegenerateBlockPanel::~QtTopologyDegenerateBlockPanel ( )
{
}	// QtTopologyDegenerateBlockPanel::~QtTopologyDegenerateBlockPanel


void QtTopologyDegenerateBlockPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	_vertex1Panel->reset ( );
	_vertex2Panel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyDegenerateBlockPanel::reset


void QtTopologyDegenerateBlockPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	_vertex1Panel->stopSelection ( );
	_vertex2Panel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_vertex1Panel->clearSelection ( );
		_vertex2Panel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyDegenerateBlockPanel::cancel


void QtTopologyDegenerateBlockPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedTopoEntities	=
			getSelectionManager ( ).getEntitiesNames (
											_vertex1Panel->getAllowedTypes ( ));
		if (2 >= selectedTopoEntities.size ( ))
		{
			vector<string>	names;
			if (1 <= selectedTopoEntities.size ( ))
			{
				names.push_back (selectedTopoEntities [0]);
				_vertex1Panel->setEntitiesNames (names);
			}	// if (1 <= selectedTopoEntities.size ( ))
			if (2 == selectedTopoEntities.size ( ))
			{
				names [0]	= selectedTopoEntities [1];
				_vertex2Panel->setEntitiesNames (names);
			}	// if (2 == selectedTopoEntities.size ( ))
		}	// if (2 >= selectedTopoEntities.size ( ))

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_vertex1Panel->clearSelection ( );
	_vertex2Panel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_vertex1Panel->actualizeGui (true);
	_vertex2Panel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyDegenerateBlockPanel::autoUpdate


string QtTopologyDegenerateBlockPanel::getVertice1Name ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	const vector<string>	names	= _vertex1Panel->getEntitiesNames ( );
	switch (names.size ( ))
	{
		case	0	: return "";
		case	1	: return names [0];
	}	// switch (names.size ( ))

	INTERNAL_ERROR (exc, "Au moins 2 entités topologiques sont sélectionnées dans le premier champ.", "QtTopologyDegenerateBlockPanel::getVertice1Name")
	throw exc;
}	// QtTopologyDegenerateBlockPanel::getVertice1Name


string QtTopologyDegenerateBlockPanel::getVertice2Name ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	const vector<string>	names	= _vertex2Panel->getEntitiesNames ( );
	switch (names.size ( ))
	{
		case	0	: return "";
		case	1	: return names [0];
	}	// switch (names.size ( ))

	INTERNAL_ERROR (exc, "Au moins 2 entités topologiques sont sélectionnées dans le second champ.", "QtTopologyDegenerateBlockPanel::getVertice1Name")
	throw exc;
}	// QtTopologyDegenerateBlockPanel::getVertice2Name


bool QtTopologyDegenerateBlockPanel::projectOnFirst ( ) const
{
	CHECK_NULL_PTR_ERROR (_projectOnFirstCheckbox)
	return Qt::Checked == _projectOnFirstCheckbox->checkState( ) ? true : false;
}	// QtTopologyDegenerateBlockPanel::projectOnFirst


void QtTopologyDegenerateBlockPanel::setDimension (SelectionManagerIfc::DIM dim)
{
	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	if (dim != _vertex1Panel->getDimensions ( ))
	{
		_vertex1Panel->clearSelection ( );
		_vertex2Panel->clearSelection ( );
		_vertex1Panel->setDimensions (dim);
		_vertex2Panel->setDimensions (dim);
	}	// if (dim != _vertex1Panel->getDimensions ( ))
}	// QtTopologyDegenerateBlockPanel::setDimension


vector<Entity*> QtTopologyDegenerateBlockPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const string	vertice1Name	= getVertice1Name ( );
	const string	vertice2Name	= getVertice2Name ( );
	TopoEntity*		vertice1	=
			getContext ( ).getTopoManager ( ).getEntity (vertice1Name, false);
	TopoEntity*		vertice2	=
			getContext ( ).getTopoManager ( ).getEntity (vertice2Name, false);
	if (0 != vertice1)
		entities.push_back (vertice1);
	if (0 != vertice2)
		entities.push_back (vertice2);

	return entities;
}	// QtTopologyDegenerateBlockPanel::getInvolvedEntities


void QtTopologyDegenerateBlockPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _vertex1Panel)
	{
		_vertex1Panel->stopSelection ( );
		_vertex1Panel->clearSelection ( );
	}
	if (0 != _vertex2Panel)
	{
		_vertex2Panel->stopSelection ( );
		_vertex2Panel->clearSelection ( );
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyDegenerateBlockPanel::operationCompleted


void QtTopologyDegenerateBlockPanel::dimensionModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_vertex1Panel)
	CHECK_NULL_PTR_ERROR (_vertex2Panel)
	CHECK_NULL_PTR_ERROR (sender ( ))
	
	if (sender ( ) == _vertex1Panel)
		_vertex2Panel->setDimensions (_vertex1Panel->getDimensions ( ));
	else
		_vertex1Panel->setDimensions (_vertex2Panel->getDimensions ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologyDegenerateBlockPanel::dimensionModifiedCallback


// ===========================================================================
//                  LA CLASSE QtTopologyDegenerateBlockAction
// ===========================================================================

QtTopologyDegenerateBlockAction::QtTopologyDegenerateBlockAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyDegenerateBlockPanel*	operationPanel	=
			new QtTopologyDegenerateBlockPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyDegenerateBlockAction::QtTopologyDegenerateBlockAction


QtTopologyDegenerateBlockAction::QtTopologyDegenerateBlockAction (
										const QtTopologyDegenerateBlockAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyDegenerateBlockAction copy constructor is not allowed.")
}	// QtTopologyDegenerateBlockAction::QtTopologyDegenerateBlockAction


QtTopologyDegenerateBlockAction& QtTopologyDegenerateBlockAction::operator = (
										const QtTopologyDegenerateBlockAction&)
{
	MGX_FORBIDDEN ("QtTopologyDegenerateBlockAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyDegenerateBlockAction::operator =


QtTopologyDegenerateBlockAction::~QtTopologyDegenerateBlockAction ( )
{
}	// QtTopologyDegenerateBlockAction::~QtTopologyDegenerateBlockAction


QtTopologyDegenerateBlockPanel*
					QtTopologyDegenerateBlockAction::getTopologyDegenerateBlockPanel ( )
{
	return dynamic_cast<QtTopologyDegenerateBlockPanel*>(getOperationPanel ( ));
}	// QtTopologyDegenerateBlockAction::getTopologyDegenerateBlockPanel


void QtTopologyDegenerateBlockAction::executeOperation ( )
{
	QtTopologyDegenerateBlockPanel*	panel	= dynamic_cast<QtTopologyDegenerateBlockPanel*>(getTopologyDegenerateBlockPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de collage des vertices topologiques :
	const string	vertice1Name	= panel->getVertice1Name ( );
	const string	vertice2Name	= panel->getVertice2Name ( );

	cmdResult	= getContext ( ).getTopoManager ( ).snapVertices (vertice1Name, vertice2Name, panel->projectOnFirst ( ));
	
	setCommandResult (cmdResult);
}	// QtTopologyDegenerateBlockAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
