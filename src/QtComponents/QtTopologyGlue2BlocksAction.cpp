/**
 * \file        QtTopologyGlue2BlocksAction.cpp
 * \author      Charles PIGNEROL
 * \date        09/12/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Geom/Volume.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyGlue2BlocksAction.h"

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
//                        LA CLASSE QtTopologyGlue2BlocksPanel
// ===========================================================================

QtTopologyGlue2BlocksPanel::QtTopologyGlue2BlocksPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).glue2blocksOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).glue2blocksOperationTag),
	  _block1Panel (0), _block2Panel (0), _methodCheckBox (0)
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

	// Le bloc 1 à coller :
	_block1Panel	= new QtMgx3DEntityPanel (
			this, "", true, "Bloc 1 :", "", &mainWindow,
			SelectionManagerIfc::D3, FilterEntity::TopoBlock);
	connect (_block1Panel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_block1Panel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_block1Panel);

	// Le bloc 2 à coller :
	_block2Panel	= new QtMgx3DEntityPanel (
			this, "", true, "Bloc 2 :", "", &mainWindow,
			SelectionManagerIfc::D3, FilterEntity::TopoBlock);
	connect (_block2Panel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_block2Panel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_block2Panel);

	// La méthode de collage :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	_methodCheckBox	= new QCheckBox (QString::fromUtf8("Blocs associés à des volumes"), this);
	_methodCheckBox->setCheckState (Qt::Unchecked);
	connect (_methodCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (methodChangedCallback ( )));
	_methodCheckBox->setToolTip (QString::fromUtf8("Cochée le collage porte sur les blocs topologiques des volumes géométriques sélectionnés. Non coché il porte sur les faces communes des blocs topologiques sélectionnés."));
	hlayout->addWidget (_methodCheckBox);
	hlayout->addStretch (2);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_block1Panel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_block2Panel->getNameTextField ( ))
	_block1Panel->getNameTextField (
			)->setLinkedSeizureManagers (0, _block2Panel->getNameTextField ( ));
	_block2Panel->getNameTextField (
			)->setLinkedSeizureManagers (_block1Panel->getNameTextField ( ), 0);

	methodChangedCallback ( );
}	// QtTopologyGlue2BlocksPanel::QtTopologyGlue2BlocksPanel


QtTopologyGlue2BlocksPanel::QtTopologyGlue2BlocksPanel (
										const QtTopologyGlue2BlocksPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _block1Panel (0), _block2Panel (0), _methodCheckBox (0)
{
	MGX_FORBIDDEN ("QtTopologyGlue2BlocksPanel copy constructor is not allowed.");
}	// QtTopologyGlue2BlocksPanel::QtTopologyGlue2BlocksPanel (const QtTopologyGlue2BlocksPanel&)


QtTopologyGlue2BlocksPanel& QtTopologyGlue2BlocksPanel::operator = (
											const QtTopologyGlue2BlocksPanel&)
{
	MGX_FORBIDDEN ("QtTopologyGlue2BlocksPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyGlue2BlocksPanel::QtTopologyGlue2BlocksPanel (const QtTopologyGlue2BlocksPanel&)


QtTopologyGlue2BlocksPanel::~QtTopologyGlue2BlocksPanel ( )
{
}	// QtTopologyGlue2BlocksPanel::~QtTopologyGlue2BlocksPanel


void QtTopologyGlue2BlocksPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_block1Panel)
	CHECK_NULL_PTR_ERROR (_block2Panel)
	_block1Panel->reset ( );
	_block2Panel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyGlue2BlocksPanel::reset


void QtTopologyGlue2BlocksPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );	// => highlight (false)

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
}	// QtTopologyGlue2BlocksPanel::cancel


void QtTopologyGlue2BlocksPanel::autoUpdate ( )
{
	highlight (false);	// car champs de texte modifiés

	CHECK_NULL_PTR_ERROR (_block1Panel)
	CHECK_NULL_PTR_ERROR (_block2Panel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedBlocks;
		switch (getGlueMethod ( ))
		{
			case QtTopologyGlue2BlocksPanel::VOLUMES	:
				selectedBlocks	= 
					getSelectionManager ( ).getEntitiesNames (
													FilterEntity::GeomVolume);
				break;
			default										:
				selectedBlocks	= 
					getSelectionManager ( ).getEntitiesNames (
													FilterEntity::TopoBlock);
		}	// switch (getGlueMethod ( ))
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
}	// QtTopologyGlue2BlocksPanel::autoUpdate


string QtTopologyGlue2BlocksPanel::getBlock1Name ( ) const
{
	CHECK_NULL_PTR_ERROR (_block1Panel)
	return _block1Panel->getUniqueName ( );
}	// QtTopologyGlue2BlocksPanel::getBlock1Name


string QtTopologyGlue2BlocksPanel::getBlock2Name ( ) const
{
	CHECK_NULL_PTR_ERROR (_block2Panel)
	return _block2Panel->getUniqueName ( );
}	// QtTopologyGlue2BlocksPanel::getBlock2Name


QtTopologyGlue2BlocksPanel::GLUE_METHOD
							QtTopologyGlue2BlocksPanel::getGlueMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_methodCheckBox)
	return Qt::Checked == _methodCheckBox->checkState ( ) ?
			QtTopologyGlue2BlocksPanel::VOLUMES :
			QtTopologyGlue2BlocksPanel::COMMON_FACES;
}	// QtTopologyGlue2BlocksPanel::getGlueMethod


void QtTopologyGlue2BlocksPanel::methodChangedCallback ( )
{	
	CHECK_NULL_PTR_ERROR (_block1Panel)
	CHECK_NULL_PTR_ERROR (_block2Panel)

	BEGIN_QT_TRY_CATCH_BLOCK

	_block1Panel->stopSelection ( );
	_block2Panel->stopSelection ( );
	_block1Panel->setUniqueName ("");
	_block2Panel->setUniqueName ("");

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	switch (getGlueMethod ( ))
	{
		case QtTopologyGlue2BlocksPanel::VOLUMES	:
			_block1Panel->setFilteredTypes (FilterEntity::GeomVolume);
			_block2Panel->setFilteredTypes (FilterEntity::GeomVolume);
			break;
		default										:
			_block1Panel->setFilteredTypes (FilterEntity::TopoBlock);
			_block2Panel->setFilteredTypes (FilterEntity::TopoBlock);
	}	// switch (getGlueMethod ( ))

	highlight (false);
	parametersModifiedCallback ( );
}	// QtTopologyGlue2BlocksPanel::methodChangedCallback


vector<Entity*> QtTopologyGlue2BlocksPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const string	name1	= getBlock1Name ( );
	const string	name2	= getBlock2Name ( );
	switch (getGlueMethod ( ))
	{
		case QtTopologyGlue2BlocksPanel::VOLUMES	:	
		{
			GeomEntity*		volume1	=
				getContext ( ).getGeomManager ( ).getVolume (name1, false);
			GeomEntity*		volume2	=
				getContext ( ).getGeomManager ( ).getVolume (name2, false);
			if (0 != volume1)
				entities.push_back (volume1);
			if (0 != volume2)
				entities.push_back (volume2);
		}	//	case QtTopologyGlue2BlocksPanel::VOLUMES
		break;
		default										:
		{
			TopoEntity*		block1	=
				getContext ( ).getTopoManager ( ).getBlock (name1, false);
			TopoEntity*		block2	=
				getContext ( ).getTopoManager ( ).getBlock (name2, false);
			if (0 != block1)
				entities.push_back (block1);
			if (0 != block2)
				entities.push_back (block2);
		}	// default
	}	// switch (getGlueMethod ( ))

	return entities;
}	// QtTopologyGlue2BlocksPanel::getInvolvedEntities


void QtTopologyGlue2BlocksPanel::operationCompleted ( )
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
}	// QtTopologyGlue2BlocksPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyGlue2BlocksAction
// ===========================================================================

QtTopologyGlue2BlocksAction::QtTopologyGlue2BlocksAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyGlue2BlocksPanel*	operationPanel	= 
			new QtTopologyGlue2BlocksPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyGlue2BlocksAction::QtTopologyGlue2BlocksAction


QtTopologyGlue2BlocksAction::QtTopologyGlue2BlocksAction (
										const QtTopologyGlue2BlocksAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyGlue2BlocksAction copy constructor is not allowed.")
}	// QtTopologyGlue2BlocksAction::QtTopologyGlue2BlocksAction


QtTopologyGlue2BlocksAction& QtTopologyGlue2BlocksAction::operator = (
										const QtTopologyGlue2BlocksAction&)
{
	MGX_FORBIDDEN ("QtTopologyGlue2BlocksAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyGlue2BlocksAction::operator =


QtTopologyGlue2BlocksAction::~QtTopologyGlue2BlocksAction ( )
{
}	// QtTopologyGlue2BlocksAction::~QtTopologyGlue2BlocksAction


QtTopologyGlue2BlocksPanel*
					QtTopologyGlue2BlocksAction::getTopologyGlue2BlocksPanel ( )
{
	return dynamic_cast<QtTopologyGlue2BlocksPanel*>(getOperationPanel ( ));
}	// QtTopologyGlue2BlocksAction::getTopologyGlue2BlocksPanel


void QtTopologyGlue2BlocksAction::executeOperation ( )
{
	QtTopologyGlue2BlocksPanel*	panel	= dynamic_cast<QtTopologyGlue2BlocksPanel*>(getTopologyGlue2BlocksPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de collage des blocs topologiques :
	const string	name1	= panel->getBlock1Name ( );
	const string	name2	= panel->getBlock2Name ( );

	Mgx3D::Internal::M3DCommandResultIfc*	result	= 0;
	switch (panel->getGlueMethod ( ))
	{
		case QtTopologyGlue2BlocksPanel::VOLUMES	:
			result	= getContext ( ).getTopoManager ( ).glue2Topo (name1, name2);
			break;
		default										:
			result	= getContext ( ).getTopoManager ( ).glue2Blocks (name1, name2);
			break;
	}	// switch (panel->getGlueMethod ( ))
	CHECK_NULL_PTR_ERROR (result)
	setCommandResult (result);
	if (CommandIfc::FAIL == result->getStatus ( ))
		throw Exception (result->getErrorMessage ( ));
	else if (CommandIfc::CANCELED == result->getStatus ( ))
		throw Exception ("Opération annulée");
}	// QtTopologyGlue2BlocksAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
