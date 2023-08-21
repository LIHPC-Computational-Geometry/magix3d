/**
 * \file        QtTopologyFuse2BlocksAction.cpp
 * \author      Charles PIGNEROL
 * \date        22/06/2015
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Geom/Volume.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyFuse2BlocksAction.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologyFuse2BlocksPanel
// ===========================================================================

QtTopologyFuse2BlocksPanel::QtTopologyFuse2BlocksPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).fuse2blocksOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).fuse2blocksOperationTag),
	  _block1Panel (0), _block2Panel (0)
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

	// Le bloc 1 à fusionner :
	_block1Panel	= new QtMgx3DEntityPanel (
			this, "", true, "Bloc 1 :", "", &mainWindow,
			SelectionManagerIfc::D3, FilterEntity::TopoBlock);
	connect (_block1Panel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_block1Panel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_block1Panel);

	// Le bloc 2 à fusionner :
	_block2Panel	= new QtMgx3DEntityPanel (
			this, "", true, "Bloc 2 :", "", &mainWindow,
			SelectionManagerIfc::D3, FilterEntity::TopoBlock);
	connect (_block2Panel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_block2Panel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_block2Panel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_block1Panel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_block2Panel->getNameTextField ( ))
	_block1Panel->getNameTextField (
			)->setLinkedSeizureManagers (0, _block2Panel->getNameTextField ( ));
	_block2Panel->getNameTextField (
			)->setLinkedSeizureManagers (_block1Panel->getNameTextField ( ), 0);
}	// QtTopologyFuse2BlocksPanel::QtTopologyFuse2BlocksPanel


QtTopologyFuse2BlocksPanel::QtTopologyFuse2BlocksPanel (
										const QtTopologyFuse2BlocksPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _block1Panel (0), _block2Panel (0)
{
	MGX_FORBIDDEN ("QtTopologyFuse2BlocksPanel copy constructor is not allowed.");
}	// QtTopologyFuse2BlocksPanel::QtTopologyFuse2BlocksPanel (const QtTopologyFuse2BlocksPanel&)


QtTopologyFuse2BlocksPanel& QtTopologyFuse2BlocksPanel::operator = (
											const QtTopologyFuse2BlocksPanel&)
{
	MGX_FORBIDDEN ("QtTopologyFuse2BlocksPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyFuse2BlocksPanel::QtTopologyFuse2BlocksPanel (const QtTopologyFuse2BlocksPanel&)


QtTopologyFuse2BlocksPanel::~QtTopologyFuse2BlocksPanel ( )
{
}	// QtTopologyFuse2BlocksPanel::~QtTopologyFuse2BlocksPanel


void QtTopologyFuse2BlocksPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_block1Panel)
	CHECK_NULL_PTR_ERROR (_block2Panel)
	_block1Panel->reset ( );
	_block2Panel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyFuse2BlocksPanel::reset


void QtTopologyFuse2BlocksPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_block1Panel)
	CHECK_NULL_PTR_ERROR (_block2Panel)

	_block1Panel->stopSelection ( );
	_block2Panel->stopSelection ( );
	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_block1Panel->setUniqueName ("");
		_block2Panel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyFuse2BlocksPanel::cancel


void QtTopologyFuse2BlocksPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_block1Panel)
	CHECK_NULL_PTR_ERROR (_block2Panel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedBlocks	= 
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoBlock);
		if (2 >= selectedBlocks.size ( ))
		{
			if (1 <= selectedBlocks.size ( ))
				_block1Panel->setUniqueName (selectedBlocks [0]);
			if (2 == selectedBlocks.size ( ))
				_block2Panel->setUniqueName (selectedBlocks [1]);
		}	// if (2 >= selectedBlocks.size ( ))

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_block1Panel->clearSelection ( );
	_block2Panel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_block1Panel->actualizeGui (true);
	_block2Panel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyFuse2BlocksPanel::autoUpdate


string QtTopologyFuse2BlocksPanel::getBlock1Name ( ) const
{
	CHECK_NULL_PTR_ERROR (_block1Panel)
	return _block1Panel->getUniqueName ( );
}	// QtTopologyFuse2BlocksPanel::getBlock1Name


string QtTopologyFuse2BlocksPanel::getBlock2Name ( ) const
{
	CHECK_NULL_PTR_ERROR (_block2Panel)
	return _block2Panel->getUniqueName ( );
}	// QtTopologyFuse2BlocksPanel::getBlock2Name


vector<Entity*> QtTopologyFuse2BlocksPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const string	name1	= getBlock1Name ( );
	const string	name2	= getBlock2Name ( );
	TopoEntity*		block1	=
				getContext ( ).getTopoManager ( ).getBlock (name1, false);
	TopoEntity*		block2	=
				getContext ( ).getTopoManager ( ).getBlock (name2, false);
	if (0 != block1)
		entities.push_back (block1);
	if (0 != block2)
		entities.push_back (block2);

	return entities;
}	// QtTopologyFuse2BlocksPanel::getInvolvedEntities


void QtTopologyFuse2BlocksPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _block1Panel)
	{
		_block1Panel->stopSelection ( );
		_block1Panel->setUniqueName ("");
	}
	if (0 != _block2Panel)
	{
		_block2Panel->stopSelection ( );	
		_block2Panel->setUniqueName ("");	
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyFuse2BlocksPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyFuse2BlocksAction
// ===========================================================================

QtTopologyFuse2BlocksAction::QtTopologyFuse2BlocksAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyFuse2BlocksPanel*	operationPanel	= 
			new QtTopologyFuse2BlocksPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyFuse2BlocksAction::QtTopologyFuse2BlocksAction


QtTopologyFuse2BlocksAction::QtTopologyFuse2BlocksAction (
										const QtTopologyFuse2BlocksAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyFuse2BlocksAction copy constructor is not allowed.")
}	// QtTopologyFuse2BlocksAction::QtTopologyFuse2BlocksAction


QtTopologyFuse2BlocksAction& QtTopologyFuse2BlocksAction::operator = (
										const QtTopologyFuse2BlocksAction&)
{
	MGX_FORBIDDEN ("QtTopologyFuse2BlocksAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyFuse2BlocksAction::operator =


QtTopologyFuse2BlocksAction::~QtTopologyFuse2BlocksAction ( )
{
}	// QtTopologyFuse2BlocksAction::~QtTopologyFuse2BlocksAction


QtTopologyFuse2BlocksPanel*
					QtTopologyFuse2BlocksAction::getTopologyFuse2BlocksPanel ( )
{
	return dynamic_cast<QtTopologyFuse2BlocksPanel*>(getOperationPanel ( ));
}	// QtTopologyFuse2BlocksAction::getTopologyFuse2BlocksPanel


void QtTopologyFuse2BlocksAction::executeOperation ( )
{
	QtTopologyFuse2BlocksPanel*	panel	= dynamic_cast<QtTopologyFuse2BlocksPanel*>(getTopologyFuse2BlocksPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de fusion des blocs topologiques :
	const string	name1	= panel->getBlock1Name ( );
	const string	name2	= panel->getBlock2Name ( );

	Mgx3D::Internal::M3DCommandResultIfc*	result	= getContext ( ).getTopoManager ( ).fuse2Blocks (name1, name2);
	CHECK_NULL_PTR_ERROR (result)
	setCommandResult (result);
	if (CommandIfc::FAIL == result->getStatus ( ))
		throw Exception (result->getErrorMessage ( ));
	else if (CommandIfc::CANCELED == result->getStatus ( ))
		throw Exception ("Opération annulée");
}	// QtTopologyFuse2BlocksAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
