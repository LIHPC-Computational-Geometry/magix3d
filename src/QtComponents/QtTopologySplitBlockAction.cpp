/**
 * \file        QtTopologySplitBlockAction.cpp
 * \author      Charles PIGNEROL
 * \date        10/03/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "QtComponents/QtTopologySplitBlockAction.h"
#include "Topo/CommandSplitBlocks.h"
#include "Topo/TopoDisplayRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

#include <QGridLayout>
#include <QRadioButton>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologySplitBlockPanel
// ===========================================================================

QtTopologySplitBlockPanel::QtTopologySplitBlockPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).splitBlockOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).splitBlockOperationTag),
	  _blocksPanel (0), _edgePanel (0), _cutDefinitionButtonGroup (0),
	  _ratioTextField (0), _cutPointEntityPanel (0), _allBlocksCheckBox (0)
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

	// Tous les blocs ?
	_allBlocksCheckBox	= new QCheckBox ("Tous les blocs ?", this);
	_allBlocksCheckBox->setToolTip (QString::fromUtf8("Coché tous les blocs seront soumis au découpage."));
	layout->addWidget (_allBlocksCheckBox);
	connect (_allBlocksCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (blocksModifiedCallback ( )));

	// Le bloc à découper :
	_blocksPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Blocs  :", "", &mainWindow,
			SelectionManagerIfc::D3, FilterEntity::TopoBlock);
	_blocksPanel->setMultiSelectMode (true);
	connect (_blocksPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_blocksPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_blocksPanel);

	// L'arête orthogonale au plan de coupe :
	_edgePanel	= new QtMgx3DEntityPanel (
			this, "", true, "Arête :", "", &mainWindow,
			SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	connect (_edgePanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edgePanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_edgePanel);

	// La définition de la découpe :
	_cutDefinitionButtonGroup	= new QButtonGroup (this);
	_cutDefinitionButtonGroup->setExclusive (true);
	QGridLayout*	gridLayout	= new QGridLayout ( );
	int	row	= 0, col	= 0;
	layout->addLayout (gridLayout);
	gridLayout->setContentsMargins  (0, 0, 0, 0);
	// Par un ratio :
	QRadioButton*	ratioRadioButton	= new QRadioButton ("Ratio :", this);
	ratioRadioButton->setChecked (true);
	_cutDefinitionButtonGroup->addButton (
						ratioRadioButton, QtTopologySplitBlockPanel::CDM_RATIO);
	gridLayout->addWidget (ratioRadioButton, row, col++);
	_ratioTextField	= &QtNumericFieldsFactory::createRatioTextField (this);
	_ratioTextField->setValue (0.5);
	gridLayout->addWidget (_ratioTextField, row++, col++, 1, 2);
	_ratioTextField->setToolTip (QString::fromUtf8("Pourcentage de la longueur de l'arête où est le point situé dans le plan de coupe."));
	connect (_ratioTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (parametersModifiedCallback ( )));
	// Par un point :
	col	= 0;
	QRadioButton*	entityRadioButton	= new QRadioButton (QString::fromUtf8("Entité :"), this);
	_cutDefinitionButtonGroup->addButton (
						entityRadioButton, QtTopologySplitBlockPanel::CDM_POINT);
	gridLayout->addWidget (entityRadioButton, row, col++);
	const FilterEntity::objectType	filter	=
			(FilterEntity::objectType)(
							FilterEntity::AllGeom | FilterEntity::TopoVertex);
	_cutPointEntityPanel	=
		new QtMgx3DEntityPanel (this, "", true, "", "", &mainWindow,
								SelectionManagerIfc::ALL_DIMENSIONS, filter);
	_cutPointEntityPanel->setToolTip (QString::fromUtf8("Entité géométrique dont le centre sert de position de découpe, ou sommet topologique."));
	gridLayout->addWidget (_cutPointEntityPanel, row, col++, 1, 2);
	connect (_cutPointEntityPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_cutPointEntityPanel,
	         SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_cutDefinitionButtonGroup, SIGNAL (buttonClicked (int)),
	         this, SLOT (parametersModifiedCallback ( )));
	connect (_cutDefinitionButtonGroup, SIGNAL (buttonClicked (int)),
	         this, SLOT (cutDefinitionModifiedCallback ( )));
	double	r	= ((double)_ratioTextField->sizeHint ( ).width( )) /
								entityRadioButton->sizeHint ( ).width( ) + 0.5;
	gridLayout->setColumnStretch (0, 1);
	gridLayout->setColumnStretch (1, (int)r);
	ratioRadioButton->setFixedSize (entityRadioButton->sizeHint ( ));
	entityRadioButton->setFixedSize (entityRadioButton->sizeHint ( ));

	addPreviewCheckBox (false);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_blocksPanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_edgePanel->getNameTextField ( ))
	_blocksPanel->getNameTextField ( )->setLinkedSeizureManagers (
										0, _edgePanel->getNameTextField ( ));
	_edgePanel->getNameTextField ( )->setLinkedSeizureManagers (
										_blocksPanel->getNameTextField ( ), 0);

	blocksModifiedCallback ( );
	cutDefinitionModifiedCallback ( );
}	// QtTopologySplitBlockPanel::QtTopologySplitBlockPanel


QtTopologySplitBlockPanel::QtTopologySplitBlockPanel (
										const QtTopologySplitBlockPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _blocksPanel (0), _cutDefinitionButtonGroup (0), _edgePanel (0),
	  _ratioTextField (0), _cutPointEntityPanel (0), _allBlocksCheckBox (0)
{
	MGX_FORBIDDEN ("QtTopologySplitBlockPanel copy constructor is not allowed.");
}	// QtTopologySplitBlockPanel::QtTopologySplitBlockPanel (const QtTopologySplitBlockPanel&)


QtTopologySplitBlockPanel& QtTopologySplitBlockPanel::operator = (
											const QtTopologySplitBlockPanel&)
{
	MGX_FORBIDDEN ("QtTopologySplitBlockPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologySplitBlockPanel::QtTopologySplitBlockPanel (const QtTopologySplitBlockPanel&)


QtTopologySplitBlockPanel::~QtTopologySplitBlockPanel ( )
{
}	// QtTopologySplitBlockPanel::~QtTopologySplitBlockPanel


void QtTopologySplitBlockPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
	_blocksPanel->reset ( );
	_edgePanel->reset ( );
	_ratioTextField->setValue (0.5);
	_cutPointEntityPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologySplitBlockPanel::reset


void QtTopologySplitBlockPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
	_blocksPanel->stopSelection ( );
	_edgePanel->stopSelection ( );
	_cutPointEntityPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_blocksPanel->setUniqueName ("");
		_edgePanel->setUniqueName ("");
		_cutPointEntityPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}   // if (true == cancelClearEntities ( ))
}	// QtTopologySplitBlockPanel::cancel


void QtTopologySplitBlockPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedBlocks  =
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoBlock);
		if (0 != selectedBlocks.size ( ))
			_blocksPanel->setUniqueNames (selectedBlocks);
		vector<string>	selectedEdges   =
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
		if (1 == selectedEdges.size ( ))
			_edgePanel->setUniqueName (selectedEdges [0]);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}   // if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_blocksPanel->clearSelection ( );
	_cutPointEntityPanel->clearSelection ( );
	_edgePanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	_blocksPanel->actualizeGui (true);
	_edgePanel->actualizeGui (true);
	_cutPointEntityPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologySplitBlockPanel::autoUpdate


bool QtTopologySplitBlockPanel::allBlocks ( ) const
{
	CHECK_NULL_PTR_ERROR (_allBlocksCheckBox)
	return Qt::Checked == _allBlocksCheckBox->checkState ( ) ? true : false;
}	// QtTopologySplitBlockPanel::allBlocks


vector<string> QtTopologySplitBlockPanel::getBlocksNames ( ) const
{
	vector<string>	names;
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	if (true == allBlocks ( ))
	{
		vector<Block*>	blocks;
		getContext ( ).getTopoManager ( ).getBlocks (blocks);
		for (vector<Block*>::iterator	it	= blocks.begin ( );
		     blocks.end ( ) != it; it++)
			names.push_back ((*it)->getUniqueName ( ));
	}	// if (true == allBlocks ( ))
	else
		names	= _blocksPanel->getUniqueNames ( );

	return names;
}	// QtTopologySplitBlockPanel::getBlocksNames


string QtTopologySplitBlockPanel::getEdgeName ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
	return _edgePanel->getUniqueName ( );
}	// QtTopologySplitBlockPanel::getEdgeName


QtTopologySplitBlockPanel::CUT_DEFINITION_METHOD
					QtTopologySplitBlockPanel::getCutDefinitionMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_cutDefinitionButtonGroup)
	return (QtTopologySplitBlockPanel::CUT_DEFINITION_METHOD)_cutDefinitionButtonGroup->checkedId ( );
}	// QtTopologySplitBlockPanel::getCutDefinitionMethod


double QtTopologySplitBlockPanel::getRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	return _ratioTextField->getValue ( );
}	// QtTopologySplitBlockPanel::getRatio


Math::Point QtTopologySplitBlockPanel::getCutPoint ( ) const
{
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
	const string	name	= _cutPointEntityPanel->getUniqueName ( );
	if (0 == name.length ( ))
		return Math::Point ( );

	Entity*			entity	= &getContext().nameToEntity (name);
	GeomEntity*		ge		= dynamic_cast<GeomEntity*>(entity);
	Topo::Vertex*	v		= dynamic_cast<Topo::Vertex*>(entity);

	if (0 != ge)
		return ge->getCenteredPosition ( );
	else if (0 != v)
		return v->getCoord ( );

	throw Exception (UTF8String ("Définition de la position de découpe : absence d'entité sélectionnée.", Charset::UTF_8));
}	// QtTopologySplitBlockPanel::getCutPoint


void QtTopologySplitBlockPanel::preview (bool show, bool destroyInteractor)
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
		std::unique_ptr<CommandSplitBlocks	>		command;
		const string			edgeName	= getEdgeName ( );
		CoEdge*					edge		=
				 getContext ( ).getTopoManager ( ).getCoEdge (edgeName, true);

		if (true == allBlocks ( ))
		{
			switch (getCutDefinitionMethod ( ))
			{
				case QtTopologySplitBlockPanel::CDM_RATIO	:
					command.reset (new CommandSplitBlocks (
											*context, edge, getRatio ( )));
				break;
				case QtTopologySplitBlockPanel::CDM_POINT	:
					command.reset (new CommandSplitBlocks (
											*context, edge, getCutPoint ( )));
				break;
				default	:
					throw Exception (UTF8String ("QtTopologySplitBlockPanel::preview : cas non implémenté.", Charset::UTF_8));
			}	// switch (getCutDefinitionMethod ( ))
		}	// if (true == allBlocks ( ))
		else
		{
			const vector<string>	blocksNames	= getBlocksNames ( );
			vector<Block*>			blocks;
			for (vector<string>::const_iterator it = blocksNames.begin ( );
			     blocksNames.end ( ) != it; it++)
			{
				Block*	block	=
					getContext ( ).getTopoManager ( ).getBlock (*it, true);
				blocks.push_back (block);
			}	// for (vector<string>::const_iterator it = ...
			switch (getCutDefinitionMethod ( ))
			{
				case QtTopologySplitBlockPanel::CDM_RATIO	:
					command.reset (new CommandSplitBlocks (
									*context, blocks, edge, getRatio ( )));
				break;
				case QtTopologySplitBlockPanel::CDM_POINT	:
					command.reset (new CommandSplitBlocks (
									*context, blocks, edge, getCutPoint ( )));
				break;
				default	:
					throw Exception (UTF8String ("QtTopologySplitBlockPanel::preview : cas non implémenté.", Charset::UTF_8));
			}	// switch (getCutDefinitionMethod ( ))
		}	// else if (true == allBlocks ( ))

		TopoDisplayRepresentation	dr (DisplayRepresentation::WIRE);
		command->getPreviewRepresentation (dr);
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
}	// QtTopologySplitBlockPanel::preview


vector<Entity*> QtTopologySplitBlockPanel::getInvolvedEntities ( )
{
	vector<Entity*>			entities;

	if (true == allBlocks ( ))
	{
		vector<Block*>	blocks;
		getContext ( ).getTopoManager ( ).getBlocks (blocks);
		for (vector<Block*>::iterator	it	= blocks.begin ( );
		     blocks.end ( ) != it; it++)
			entities.push_back (*it);
	}	// if (true == allBlocks ( ))
	else
	{
		const vector<string>	blocksNames	= getBlocksNames ( );
		for (vector<string>::const_iterator it = blocksNames.begin ( );
		     blocksNames.end ( ) != it; it++)
		{
			TopoEntity*	te	=
				getContext( ).getTopoManager( ).getBlock(*it, false);
			if (0 != te)
				entities.push_back (te);
		}	// for (vector<string>::const_iterator it = ...
	}	// else if (true == allBlocks ( ))

	const string	edgeName	= getEdgeName ( );
	TopoEntity*		edge		=
			getContext ( ).getTopoManager ( ).getCoEdge (edgeName, false);
	if (0 != edge)
		entities.push_back (edge);
	switch (getCutDefinitionMethod ( ))
	{
		case QtTopologySplitBlockPanel::CDM_POINT	:
		{
			CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
			const string	point	= _cutPointEntityPanel->getUniqueName ( );
			if (0 != point.length ( ))
				entities.push_back (&getContext().nameToEntity (point));
		}
		break;
	}	// switch (getCutDefinitionMethod ( ))

	return entities;
}	// QtTopologySplitBlockPanel::getInvolvedEntities


void QtTopologySplitBlockPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _blocksPanel)
	{
		_blocksPanel->stopSelection ( );
		_blocksPanel->setUniqueName ("" );
	}
	if (0 != _edgePanel)
	{
		_edgePanel->stopSelection ( );
		_edgePanel->setUniqueName ("");
	}
	if (0 != _cutPointEntityPanel)
	{
		_cutPointEntityPanel->stopSelection ( );
		_cutPointEntityPanel->setUniqueName ("");
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologySplitBlockPanel::operationCompleted


void QtTopologySplitBlockPanel::cutDefinitionModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_ratioTextField)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)

	switch (getCutDefinitionMethod ( ))
	{
		case QtTopologySplitBlockPanel::CDM_RATIO	:
			_ratioTextField->setEnabled (true);
			_cutPointEntityPanel->setEnabled (false);
			break;
		case QtTopologySplitBlockPanel::CDM_POINT	:
			_ratioTextField->setEnabled (false);
			_cutPointEntityPanel->setEnabled (true);
			break;
	}	// switch (getCutDefinitionMethod ( ))

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologySplitBlockPanel::cutDefinitionModifiedCallback


void QtTopologySplitBlockPanel::blocksModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_allBlocksCheckBox)
	CHECK_NULL_PTR_ERROR (_blocksPanel)

	_blocksPanel->setEnabled (
		Qt::Checked == _allBlocksCheckBox->checkState ( ) ? false : true);

	parametersModifiedCallback ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologySplitBlockPanel::blocksModifiedCallback


// ===========================================================================
//                  LA CLASSE QtTopologySplitBlockAction
// ===========================================================================

QtTopologySplitBlockAction::QtTopologySplitBlockAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologySplitBlockPanel*	operationPanel	= 
			new QtTopologySplitBlockPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologySplitBlockAction::QtTopologySplitBlockAction


QtTopologySplitBlockAction::QtTopologySplitBlockAction (
										const QtTopologySplitBlockAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologySplitBlockAction copy constructor is not allowed.")
}	// QtTopologySplitBlockAction::QtTopologySplitBlockAction


QtTopologySplitBlockAction& QtTopologySplitBlockAction::operator = (
										const QtTopologySplitBlockAction&)
{
	MGX_FORBIDDEN ("QtTopologySplitBlockAction assignment operator is not allowed.")
	return *this;
}	// QtTopologySplitBlockAction::operator =


QtTopologySplitBlockAction::~QtTopologySplitBlockAction ( )
{
}	// QtTopologySplitBlockAction::~QtTopologySplitBlockAction


QtTopologySplitBlockPanel*
					QtTopologySplitBlockAction::getTopologySplitBlockPanel ( )
{
	return dynamic_cast<QtTopologySplitBlockPanel*>(getOperationPanel ( ));
}	// QtTopologySplitBlockAction::getTopologySplitBlockPanel


void QtTopologySplitBlockAction::executeOperation ( )
{
	QtTopologySplitBlockPanel*	panel	= dynamic_cast<QtTopologySplitBlockPanel*>(getTopologySplitBlockPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de découpage des blocs topologiques :
	vector<string>	blocksNames	= panel->getBlocksNames ( );
	const string	edgeName	= panel->getEdgeName ( );

	Mgx3D::Internal::M3DCommandResultIfc*	result	= 0;
	if (true == panel->allBlocks ( ))
	{
		switch (panel->getCutDefinitionMethod ( ))
		{
			case QtTopologySplitBlockPanel::CDM_RATIO	:
				result	= getContext( ).getTopoManager( ).splitAllBlocks(
											edgeName, panel->getRatio ( ));
			break;
			case QtTopologySplitBlockPanel::CDM_POINT	:
					result	= getContext( ).getTopoManager( ).splitAllBlocks(
											edgeName, panel->getCutPoint ( ));
			break;
			default	:
			{
				UTF8String	message (Charset::UTF_8);
				message << "Position de découpe non supportée ("
				        << (unsigned long)panel->getCutDefinitionMethod ( )
				        << ").";
				INTERNAL_ERROR (exc, message, "QtTopologySplitFaceAction::executeOperation")
				throw exc;	
			}
		}	// switch (panel->getCutDefinitionMethod ( ))
	}	// if (true == panel->allBlocks ( ))
	else
	{
		switch (panel->getCutDefinitionMethod ( ))
		{
			case QtTopologySplitBlockPanel::CDM_RATIO	:
				result	= getContext( ).getTopoManager( ).splitBlocks(
								blocksNames, edgeName, panel->getRatio ( ));
			break;
			case QtTopologySplitBlockPanel::CDM_POINT	:
					result	= getContext( ).getTopoManager( ).splitBlocks(
								blocksNames, edgeName, panel->getCutPoint ( ));
			break;
			default	:
			{
				UTF8String	message (Charset::UTF_8);
				message << "Position de découpe non supportée ("
				        << (unsigned long)panel->getCutDefinitionMethod ( )
				        << ").";
				INTERNAL_ERROR (exc, message, "QtTopologySplitFaceAction::executeOperation")
				throw exc;	
			}
		}	// switch (panel->getCutDefinitionMethod ( ))
	}	// if (true == panel->allBlocks ( ))
	CHECK_NULL_PTR_ERROR (result)
	setCommandResult (result);
	if (CommandIfc::FAIL == result->getStatus ( ))
		throw Exception (result->getErrorMessage ( ));
	else if (CommandIfc::CANCELED == result->getStatus ( ))
		throw Exception ("Opération annulée");
}	// QtTopologySplitBlockAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
