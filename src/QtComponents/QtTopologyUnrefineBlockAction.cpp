/**
 * \file        QtTopologyUnrefineBlockAction.cpp
 * \author      Charles PIGNEROL
 * \date        11/12/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyUnrefineBlockAction.h"
#include "Topo/CommandUnrefineBlock.h"
#include "Topo/TopoDisplayRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/NumericServices.h>
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
//                        LA CLASSE QtTopologyUnrefineBlockPanel
// ===========================================================================

QtTopologyUnrefineBlockPanel::QtTopologyUnrefineBlockPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).unrefineBlockOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).unrefineBlockOperationTag),
	  _blockPanel (0), _edgePanel (0), _ratioTextField (0)
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

	// Le bloc à déraffiner :
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

	// La fréquence de déraffinement :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Fréquence :"), this);
	hlayout->addWidget (label);
	_ratioTextField	= new QtIntTextField (2, this);
	_ratioTextField->setRange (1,  NumericServices::unsignedShortMachMax());
	hlayout->addWidget (_ratioTextField);
	connect (_ratioTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (parametersModifiedCallback ( )));
	label->setToolTip (QString::fromUtf8("Fréquence de déraffinement (2 pour un bras sur 2, 3 pour un sur 3, ..."));
	_ratioTextField->setToolTip (QString::fromUtf8("Fréquence de déraffinement (2 pour un bras sur 2, 3 pour un sur 3, ..."));
	hlayout->addStretch (2);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_blockPanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_edgePanel->getNameTextField ( ))
	_blockPanel->getNameTextField (
			)->setLinkedSeizureManagers (0, _edgePanel->getNameTextField ( ));
	_edgePanel->getNameTextField (
			)->setLinkedSeizureManagers (_blockPanel->getNameTextField ( ), 0);
}	// QtTopologyUnrefineBlockPanel::QtTopologyUnrefineBlockPanel


QtTopologyUnrefineBlockPanel::QtTopologyUnrefineBlockPanel (
										const QtTopologyUnrefineBlockPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _blockPanel (0), _edgePanel (0), _ratioTextField (0)
{
	MGX_FORBIDDEN ("QtTopologyUnrefineBlockPanel copy constructor is not allowed.");
}	// QtTopologyUnrefineBlockPanel::QtTopologyUnrefineBlockPanel (const QtTopologyUnrefineBlockPanel&)


QtTopologyUnrefineBlockPanel& QtTopologyUnrefineBlockPanel::operator = (
											const QtTopologyUnrefineBlockPanel&)
{
	MGX_FORBIDDEN ("QtTopologyUnrefineBlockPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyUnrefineBlockPanel::QtTopologyUnrefineBlockPanel (const QtTopologyUnrefineBlockPanel&)


QtTopologyUnrefineBlockPanel::~QtTopologyUnrefineBlockPanel ( )
{
}	// QtTopologyUnrefineBlockPanel::~QtTopologyUnrefineBlockPanel


void QtTopologyUnrefineBlockPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_blockPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	_blockPanel->reset ( );
	_edgePanel->reset ( );
	_ratioTextField->setValue (2);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyUnrefineBlockPanel::reset


void QtTopologyUnrefineBlockPanel::cancel ( )
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
}	// QtTopologyUnrefineBlockPanel::cancel


void QtTopologyUnrefineBlockPanel::autoUpdate ( )
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
	_edgePanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_blockPanel->actualizeGui (true);
	_edgePanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyUnrefineBlockPanel::autoUpdate


string QtTopologyUnrefineBlockPanel::getBlockName ( ) const
{
	CHECK_NULL_PTR_ERROR (_blockPanel)
	return _blockPanel->getUniqueName ( );
}	// QtTopologyUnrefineBlockPanel::getBlockName


string QtTopologyUnrefineBlockPanel::getEdgeName ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
	return _edgePanel->getUniqueName ( );
}	// QtTopologyUnrefineBlockPanel::getEdgeName


unsigned short QtTopologyUnrefineBlockPanel::getRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	return (unsigned short)_ratioTextField->getValue ( );
}	// QtTopologyUnrefineBlockPanel::getRatio


/*
void QtTopologyUnrefineBlockPanel::preview (bool show, bool destroyInteractor)
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
		const int		ratio		= getRatio ( );
		Block*			block		=
				getContext ( ).getTopoManager ( ).getBlock (blockName, true);
		CoEdge*			edge		=
				 getContext ( ).getTopoManager ( ).getCoEdge (edgeName, true);
		CommandUnrefineBlock			command (*context, block, edge, ratio);
		TopoDisplayRepresentation	dr (DisplayRepresentation::WIRE);
		command.getPreviewRepresentation (dr);
		const vector<Math::Point>*	points	= dr.getPoints ( );
		const vector<size_t>*		indices	= dr.getCurveDiscretization ( );

		DisplayProperties	graphicalProps;
		graphicalProps.setWireColor (Color (
						255 * Resources::instance ( )._previewColor.getRed ( ),
						255 * Resources::instance ( )._previewColor.getGreen ( ),
						255 * Resources::instance ( )._previewColor.getBlue ( )));
		graphicalProps.setLineWidth (
						Resources::instance ( )._previewWidth.getValue ( ));
		RenderingManager::RepresentationID	repID	=
				getRenderingManager ( ).createSegmentsWireRepresentation (
									*points, *indices, graphicalProps, true);
		registerPreviewedObject (repID);

		getRenderingManager ( ).forceRender ( );
	}
	catch (...)
	{
	}
}	// QtTopologyUnrefineBlockPanel::preview
*/


vector<Entity*> QtTopologyUnrefineBlockPanel::getInvolvedEntities ( )
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
}	// QtTopologyUnrefineBlockPanel::getInvolvedEntities


void QtTopologyUnrefineBlockPanel::operationCompleted ( )
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
}	// QtTopologyUnrefineBlockPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyUnrefineBlockAction
// ===========================================================================

QtTopologyUnrefineBlockAction::QtTopologyUnrefineBlockAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyUnrefineBlockPanel*	operationPanel	= 
			new QtTopologyUnrefineBlockPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyUnrefineBlockAction::QtTopologyUnrefineBlockAction


QtTopologyUnrefineBlockAction::QtTopologyUnrefineBlockAction (
										const QtTopologyUnrefineBlockAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyUnrefineBlockAction copy constructor is not allowed.")
}	// QtTopologyUnrefineBlockAction::QtTopologyUnrefineBlockAction


QtTopologyUnrefineBlockAction& QtTopologyUnrefineBlockAction::operator = (
										const QtTopologyUnrefineBlockAction&)
{
	MGX_FORBIDDEN ("QtTopologyUnrefineBlockAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyUnrefineBlockAction::operator =


QtTopologyUnrefineBlockAction::~QtTopologyUnrefineBlockAction ( )
{
}	// QtTopologyUnrefineBlockAction::~QtTopologyUnrefineBlockAction


QtTopologyUnrefineBlockPanel*
					QtTopologyUnrefineBlockAction::getTopologyUnrefineBlockPanel ( )
{
	return dynamic_cast<QtTopologyUnrefineBlockPanel*>(getOperationPanel ( ));
}	// QtTopologyUnrefineBlockAction::getTopologyUnrefineBlockPanel


void QtTopologyUnrefineBlockAction::executeOperation ( )
{
	QtTopologyUnrefineBlockPanel*	panel	= dynamic_cast<QtTopologyUnrefineBlockPanel*>(getTopologyUnrefineBlockPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de déraffinement du bloc topologique :
	const string	blockName	= panel->getBlockName ( );
	const string	edgeName	= panel->getEdgeName ( );
	const int		ratio		= panel->getRatio ( );

	cmdResult	= getContext ( ).getTopoManager ( ).unrefine (blockName, edgeName, ratio);

	setCommandResult (cmdResult);
}	// QtTopologyUnrefineBlockAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
