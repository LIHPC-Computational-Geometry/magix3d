/**
 * \file        QtTopologyExtendSplitBlockAction.cpp
 * \author      Charles PIGNEROL
 * \date        10/03/2014
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyExtendSplitBlockAction.h"
#include "Topo/CommandExtendSplitBlock.h"
#include "Topo/TopoDisplayRepresentation.h"

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
//                        LA CLASSE QtTopologyExtendSplitBlockPanel
// ===========================================================================

QtTopologyExtendSplitBlockPanel::QtTopologyExtendSplitBlockPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).extendSplitBlockOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).extendSplitBlockOperationTag),
	  _blockPanel (0), _edgePanel (0)
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

	// Le bloc à découper :
	_blockPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Bloc  :", "", &mainWindow,
			SelectionManagerIfc::D3, FilterEntity::TopoBlock);
	connect (_blockPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_blockPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_blockPanel);

	// L'arête orthogonale au plan de coupe :
	_edgePanel	= new QtMgx3DEntityPanel (
			this, "", true, "Arête :", "", &mainWindow,
			SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	connect (_edgePanel->getNameTextField ( ),
	         SIGNAL (selectionModified (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
	layout->addWidget (_edgePanel);

	addPreviewCheckBox (false);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_blockPanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_edgePanel->getNameTextField ( ))
	_blockPanel->getNameTextField (
			)->setLinkedSeizureManagers (0, _edgePanel->getNameTextField ( ));
	_edgePanel->getNameTextField (
			)->setLinkedSeizureManagers (_blockPanel->getNameTextField ( ), 0);
}	// QtTopologyExtendSplitBlockPanel::QtTopologyExtendSplitBlockPanel


QtTopologyExtendSplitBlockPanel::QtTopologyExtendSplitBlockPanel (
										const QtTopologyExtendSplitBlockPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _blockPanel (0), _edgePanel (0)
{
	MGX_FORBIDDEN ("QtTopologyExtendSplitBlockPanel copy constructor is not allowed.");
}	// QtTopologyExtendSplitBlockPanel::QtTopologyExtendSplitBlockPanel (const QtTopologyExtendSplitBlockPanel&)


QtTopologyExtendSplitBlockPanel& QtTopologyExtendSplitBlockPanel::operator = (
											const QtTopologyExtendSplitBlockPanel&)
{
	MGX_FORBIDDEN ("QtTopologyExtendSplitBlockPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyExtendSplitBlockPanel::QtTopologyExtendSplitBlockPanel (const QtTopologyExtendSplitBlockPanel&)


QtTopologyExtendSplitBlockPanel::~QtTopologyExtendSplitBlockPanel ( )
{
}	// QtTopologyExtendSplitBlockPanel::~QtTopologyExtendSplitBlockPanel


void QtTopologyExtendSplitBlockPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_blockPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	_blockPanel->reset ( );
	_edgePanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyExtendSplitBlockPanel::reset


void QtTopologyExtendSplitBlockPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_blockPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	_blockPanel->stopSelection ( );
	_edgePanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_blockPanel->setUniqueName ("");
		_edgePanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyExtendSplitBlockPanel::cancel


void QtTopologyExtendSplitBlockPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_blockPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedBlocks	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoBlock);
		if (1 == selectedBlocks.size ( ))
			_blockPanel->setUniqueName (selectedBlocks [0]);
		vector<string>	selectedEdges	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
		if (1 == selectedEdges.size ( ))
			_edgePanel->setUniqueName (selectedEdges [0]);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_blockPanel->clearSelection ( );
	_edgePanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	_blockPanel->actualizeGui (true);
	_edgePanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyExtendSplitBlockPanel::autoUpdate


string QtTopologyExtendSplitBlockPanel::getBlockName ( ) const
{
	CHECK_NULL_PTR_ERROR (_blockPanel)
	return _blockPanel->getUniqueName ( );
}	// QtTopologyExtendSplitBlockPanel::getBlockName


string QtTopologyExtendSplitBlockPanel::getEdgeName ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
	return _edgePanel->getUniqueName ( );
}	// QtTopologyExtendSplitBlockPanel::getEdgeName


void QtTopologyExtendSplitBlockPanel::preview (
											bool show, bool destroyInteractor)
{
	// Lors de la construction getGraphicalWidget peut être nul ...
	try
	{
		getRenderingManager ( );
	}
	catch (...)
	{
		return;
	}

	QtMgx3DOperationPanel::preview (show, destroyInteractor);
	if ((false == show) || (false == previewResult ( )))
		return;

	try
	{

		Context*		context		= dynamic_cast<Context*>(&getContext ( ));
		CHECK_NULL_PTR_ERROR (context)
		const string	blockName	= getBlockName ( );
		const string	edgeName	= getEdgeName ( );
		Block*			block		=
				getContext ( ).getTopoManager ( ).getBlock (blockName, true);
		CoEdge*			edge		=
				 getContext ( ).getTopoManager ( ).getCoEdge (edgeName, true);
		CommandExtendSplitBlock			command (*context, block, edge);
		TopoDisplayRepresentation	dr (DisplayRepresentation::WIRE);
		command.getPreviewRepresentation (dr);
		const vector<Math::Point>&	points	= dr.getPoints ( );
		const vector<size_t>&		indices	= dr.getCurveDiscretization ( );

		DisplayProperties	graphicalProps;
		graphicalProps.setWireColor (Color (
						255 * Resources::instance ( )._previewColor.getRed ( ),
						255 * Resources::instance ( )._previewColor.getGreen ( ),
						255 * Resources::instance ( )._previewColor.getBlue ( )));
		graphicalProps.setLineWidth (
						Resources::instance ( )._previewWidth.getValue ( ));
		RenderingManager::RepresentationID	repID	=
				getRenderingManager ( ).createSegmentsWireRepresentation (
									points, indices, graphicalProps, true);
		registerPreviewedObject (repID);

		getRenderingManager ( ).forceRender ( );
	}
	catch (...)
	{
	}
}	// QtTopologyExtendSplitBlockPanel::preview


vector<Entity*> QtTopologyExtendSplitBlockPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const string	blockName	= getBlockName ( );
	const string	edgeName	= getEdgeName ( );
	TopoEntity*		block	=
			getContext ( ).getTopoManager ( ).getBlock (blockName, false);
	TopoEntity*		edge	=
			getContext ( ).getTopoManager ( ).getCoEdge (edgeName, false);
	if (0 != block)
		entities.push_back (block);
	if (0 != edge)
		entities.push_back (edge);

	return entities;
}	// QtTopologyExtendSplitBlockPanel::getInvolvedEntities


void QtTopologyExtendSplitBlockPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _blockPanel)
	{
		_blockPanel->stopSelection ( );
		_blockPanel->setUniqueName ("" );
	}
	if (0 != _edgePanel)
	{
		_edgePanel->stopSelection ( );
		_edgePanel->setUniqueName ("");
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyExtendSplitBlockPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyExtendSplitBlockAction
// ===========================================================================

QtTopologyExtendSplitBlockAction::QtTopologyExtendSplitBlockAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyExtendSplitBlockPanel*	operationPanel	= 
			new QtTopologyExtendSplitBlockPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyExtendSplitBlockAction::QtTopologyExtendSplitBlockAction


QtTopologyExtendSplitBlockAction::QtTopologyExtendSplitBlockAction (
										const QtTopologyExtendSplitBlockAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyExtendSplitBlockAction copy constructor is not allowed.")
}	// QtTopologyExtendSplitBlockAction::QtTopologyExtendSplitBlockAction


QtTopologyExtendSplitBlockAction& QtTopologyExtendSplitBlockAction::operator = (
										const QtTopologyExtendSplitBlockAction&)
{
	MGX_FORBIDDEN ("QtTopologyExtendSplitBlockAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyExtendSplitBlockAction::operator =


QtTopologyExtendSplitBlockAction::~QtTopologyExtendSplitBlockAction ( )
{
}	// QtTopologyExtendSplitBlockAction::~QtTopologyExtendSplitBlockAction


QtTopologyExtendSplitBlockPanel* QtTopologyExtendSplitBlockAction::getTopologyExtendSplitBlockPanel ( )
{
	return dynamic_cast<QtTopologyExtendSplitBlockPanel*>(getOperationPanel ( ));
}	// QtTopologyExtendSplitBlockAction::getTopologyExtendSplitBlockPanel


void QtTopologyExtendSplitBlockAction::executeOperation ( )
{
	QtTopologyExtendSplitBlockPanel*	panel	= dynamic_cast<QtTopologyExtendSplitBlockPanel*>(getTopologyExtendSplitBlockPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de découpage des blocs topologiques :
	const string	blockName	= panel->getBlockName ( );
	const string	edgeName	= panel->getEdgeName ( );

	Mgx3D::Internal::M3DCommandResultIfc*	result	= getContext ( ).getTopoManager ( ).extendSplitBlock (blockName, edgeName);
	CHECK_NULL_PTR_ERROR (result)
	setCommandResult (result);
	if (CommandIfc::FAIL == result->getStatus ( ))
		throw Exception (result->getErrorMessage ( ));
	else if (CommandIfc::CANCELED == result->getStatus ( ))
		throw Exception ("Opération annulée");	
}	// QtTopologyExtendSplitBlockAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
