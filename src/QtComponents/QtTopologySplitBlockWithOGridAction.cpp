/**
 * \file        QtTopologySplitBlockWithOGridAction.cpp
 * \author      Charles PIGNEROL
 * \date        14/03/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Internal/Resources.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "QtComponents/QtTopologySplitBlockWithOGridAction.h"
#include "Topo/CommandSplitBlocksWithOgrid.h"
#include "Topo/TopoDisplayRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologySplitBlockWithOGridPanel
// ===========================================================================

QtTopologySplitBlockWithOGridPanel::QtTopologySplitBlockWithOGridPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).splitBlockWithOGridOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).splitBlockWithOGridOperationTag),
	  _blocksPanel (0), _facesPanel (0), _propagateNeighborBlocks(0),
	  _ratioTextField (0), _edgesNumTextField (0)
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

	// Les blocs à découper :
	_blocksPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Blocs :", "", &mainWindow,
			SelectionManagerIfc::D3, FilterEntity::TopoBlock);
	_blocksPanel->setMultiSelectMode (true);
	connect (_blocksPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_blocksPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_blocksPanel);

	// Les éventuelles faces à découper :
	_facesPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Faces :", "", &mainWindow,
			SelectionManagerIfc::D2, FilterEntity::TopoCoFace);
	_facesPanel->setMultiSelectMode (true);
	connect (_facesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_facesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_facesPanel);

	// propage aux blocs voisins le découpage ?
	_propagateNeighborBlocks = new QCheckBox ("Propage aux blocs voisins suivant face sélectionnées ?", this);
	_propagateNeighborBlocks->setToolTip (QString::fromUtf8("Coché, les blocs voisins seront découpés (méthode active par défaut jusqu'en 2018)."));
	_propagateNeighborBlocks->setChecked(false);
	layout->addWidget (_propagateNeighborBlocks);
	connect (_propagateNeighborBlocks, SIGNAL (stateChanged (int)), this,
	         SLOT (parametersModifiedCallback ( )));


	// Le ratio de découpage du o-grid :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	hlayout->setContentsMargins  (0, 0, 0, 0);
	label	= new QLabel ("Ratio :", this);
	hlayout->addWidget (label);
	_ratioTextField	= &QtNumericFieldsFactory::createRatioTextField (this);
	_ratioTextField->setValue (0.5);
	hlayout->addWidget (_ratioTextField);
	_ratioTextField->setToolTip (QString::fromUtf8("Pourcentage de la longueur de l'arête où se situe le découpage en o-grid."));
	connect (_ratioTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (parametersModifiedCallback ( )));

	// Le nombre de bras par arête du tour du bloc central :
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	hlayout->setContentsMargins  (0, 0, 0, 0);
	label	= new QLabel ("Nombre de bras :", this);
	hlayout->addWidget (label);
	_edgesNumTextField	= new QtIntTextField (10, this);
	hlayout->addWidget (_edgesNumTextField);
	_edgesNumTextField->setToolTip (QString::fromUtf8("Nombre de bras par arête du tour du bloc central."));
	connect (_edgesNumTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (parametersModifiedCallback ( )));

	addPreviewCheckBox (false);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_blocksPanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_facesPanel->getNameTextField ( ))
	_blocksPanel->getNameTextField (
			)->setLinkedSeizureManagers (0, _facesPanel->getNameTextField ( ));
	_facesPanel->getNameTextField (
			)->setLinkedSeizureManagers (_blocksPanel->getNameTextField ( ), 0);
}	// QtTopologySplitBlockWithOGridPanel::QtTopologySplitBlockWithOGridPanel


QtTopologySplitBlockWithOGridPanel::QtTopologySplitBlockWithOGridPanel (
								const QtTopologySplitBlockWithOGridPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _blocksPanel (0), _facesPanel (0), _propagateNeighborBlocks(0),
	  _ratioTextField (0), _edgesNumTextField (0)
{
	MGX_FORBIDDEN ("QtTopologySplitBlockWithOGridPanel copy constructor is not allowed.");
}	// QtTopologySplitBlockWithOGridPanel::QtTopologySplitBlockWithOGridPanel (const QtTopologySplitBlockWithOGridPanel&)


QtTopologySplitBlockWithOGridPanel&
						QtTopologySplitBlockWithOGridPanel::operator = (
									const QtTopologySplitBlockWithOGridPanel&)
{
	MGX_FORBIDDEN ("QtTopologySplitBlockWithOGridPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologySplitBlockWithOGridPanel::QtTopologySplitBlockWithOGridPanel (const QtTopologySplitBlockWithOGridPanel&)


QtTopologySplitBlockWithOGridPanel::~QtTopologySplitBlockWithOGridPanel ( )
{
}	// QtTopologySplitBlockWithOGridPanel::~QtTopologySplitBlockWithOGridPanel


void QtTopologySplitBlockWithOGridPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_facesPanel)
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	CHECK_NULL_PTR_ERROR (_edgesNumTextField)
	_blocksPanel->reset ( );
	_facesPanel->reset ( );
	_ratioTextField->setValue (0.5);
	_edgesNumTextField->setValue (10);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologySplitBlockWithOGridPanel::reset


void QtTopologySplitBlockWithOGridPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_facesPanel)
	_blocksPanel->stopSelection ( );
	_facesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_blocksPanel->setUniqueName ("");
		_facesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologySplitBlockWithOGridPanel::cancel


void QtTopologySplitBlockWithOGridPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_facesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedBlocks	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoBlock);
		if (0 != selectedBlocks.size ( ))
			_blocksPanel->setUniqueNames (selectedBlocks);
		const vector<string>	selectedFaces	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoFace);
		if (0 != selectedFaces.size ( ))
			_facesPanel->setUniqueNames (selectedFaces);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_blocksPanel->clearSelection ( );
	_facesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_blocksPanel->actualizeGui (true);
	_facesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologySplitBlockWithOGridPanel::autoUpdate


vector<string> QtTopologySplitBlockWithOGridPanel::getBlocksNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	return _blocksPanel->getUniqueNames ( );
}	// QtTopologySplitBlockWithOGridPanel::getBlocksNames


vector<string> QtTopologySplitBlockWithOGridPanel::getFacesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_facesPanel)
	return _facesPanel->getUniqueNames ( );
}	// QtTopologySplitBlockWithOGridPanel::getFacesNames


bool QtTopologySplitBlockWithOGridPanel::propagate ( ) const
{
	CHECK_NULL_PTR_ERROR (_propagateNeighborBlocks)
	return Qt::Checked == _propagateNeighborBlocks->checkState ( ) ? true : false;
}	// QtTopologySplitBlockPanel::propagate


double QtTopologySplitBlockWithOGridPanel::getRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	return _ratioTextField->getValue ( );
}	// QtTopologySplitBlockWithOGridPanel::getRatio


size_t QtTopologySplitBlockWithOGridPanel::getCentralEdgesNumber ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgesNumTextField)
	return _edgesNumTextField->getValue ( );
}	// QtTopologySplitBlockWithOGridPanel::getCentralEdgesNumber


void QtTopologySplitBlockWithOGridPanel::preview (
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
		const vector<string>	blocksNames	= getBlocksNames ( );
		const vector<string>	facesNames	= getFacesNames ( );
		const double			ratio		= getRatio ( );
		const int				edgesNum	= getCentralEdgesNumber ( );
		vector<Block*>			blocks;
		vector<CoFace*>			faces;
		for (vector<string>::const_iterator it = blocksNames.begin ( );
		     blocksNames.end ( ) != it; it++)
		{
			Block*	block		=
				getContext ( ).getTopoManager ( ).getBlock (*it, true);
			blocks.push_back (block);
		}	// for (vector<string>::const_iterator it = blocksNames.begin ( );
		for (vector<string>::const_iterator it = facesNames.begin ( );
		     facesNames.end ( ) != it; it++)
		{
			CoFace*	face		=
				getContext ( ).getTopoManager ( ).getCoFace (*it, true);
			faces.push_back (face);
		}	// for (vector<string>::const_iterator it = facesNames.begin ( );
		CommandSplitBlocksWithOgrid	command (
									*context, blocks, faces, ratio, edgesNum, false, propagate());
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
}	// QtTopologySplitBlockWithOGridPanel::preview


vector<Entity*> QtTopologySplitBlockWithOGridPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const vector<string>	blocksNames	= getBlocksNames ( );
	const vector<string>	facesNames	= getFacesNames ( );
	for (vector<string>::const_iterator it = blocksNames.begin ( );
	     blocksNames.end ( ) != it; it++)
	{
		TopoEntity*	te	= getContext( ).getTopoManager( ).getBlock(*it, false);
		if (0 != te)
			entities.push_back (te);
	}	// for (vector<string>::const_iterator it = blocksNames.begin ( ); ...
	for (vector<string>::const_iterator it = facesNames.begin ( );
	     facesNames.end ( ) != it; it++)
	{
		TopoEntity*	te	= getContext( ).getTopoManager( ).getCoFace(*it, false);
		if (0 != te)
			entities.push_back (te);
	}	// for (vector<string>::const_iterator it = facesNames.begin ( ); ...

	return entities;
}	// QtTopologySplitBlockWithOGridPanel::getInvolvedEntities


void QtTopologySplitBlockWithOGridPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _blocksPanel)
	{
		_blocksPanel->stopSelection ( );
		_blocksPanel->setUniqueName ("" );
	}	// if (0 != _blocksPanel)
	if (0 != _facesPanel)
	{
		_facesPanel->stopSelection ( );
		_facesPanel->setUniqueName ("");
	}	// if (0 != _facesPanel)

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologySplitBlockWithOGridPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologySplitBlockWithOGridAction
// ===========================================================================

QtTopologySplitBlockWithOGridAction::QtTopologySplitBlockWithOGridAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologySplitBlockWithOGridPanel*	operationPanel	= 
			new QtTopologySplitBlockWithOGridPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologySplitBlockWithOGridAction::QtTopologySplitBlockWithOGridAction


QtTopologySplitBlockWithOGridAction::QtTopologySplitBlockWithOGridAction (
									const QtTopologySplitBlockWithOGridAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologySplitBlockWithOGridAction copy constructor is not allowed.")
}	// QtTopologySplitBlockWithOGridAction::QtTopologySplitBlockWithOGridAction


QtTopologySplitBlockWithOGridAction&
							QtTopologySplitBlockWithOGridAction::operator = (
									const QtTopologySplitBlockWithOGridAction&)
{
	MGX_FORBIDDEN ("QtTopologySplitBlockWithOGridAction assignment operator is not allowed.")
	return *this;
}	// QtTopologySplitBlockWithOGridAction::operator =


QtTopologySplitBlockWithOGridAction::~QtTopologySplitBlockWithOGridAction ( )
{
}	// QtTopologySplitBlockWithOGridAction::~QtTopologySplitBlockWithOGridAction


QtTopologySplitBlockWithOGridPanel*
	QtTopologySplitBlockWithOGridAction::getTopologySplitBlockWithOGridPanel ( )
{
	return dynamic_cast<QtTopologySplitBlockWithOGridPanel*>(getOperationPanel ( ));
}	// QtTopologySplitBlockWithOGridAction::getTopologySplitBlockWithOGridPanel


void QtTopologySplitBlockWithOGridAction::executeOperation ( )
{
	QtTopologySplitBlockWithOGridPanel*	panel	= dynamic_cast<QtTopologySplitBlockWithOGridPanel*>(getTopologySplitBlockWithOGridPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de découpage en o-grid des blocs
	// topologiques :
	vector<string>	blocksNames		= panel->getBlocksNames ( );
	vector<string>	facesNames		= panel->getFacesNames ( );
	double			ratio			= panel->getRatio ( );
	size_t			centralEdgesNum	= panel->getCentralEdgesNumber ( );
	if (panel->propagate())
		cmdResult	= getContext ( ).getTopoManager ( ).splitBlocksWithOgrid (blocksNames, facesNames, ratio, centralEdgesNum);
	else
		cmdResult	= getContext ( ).getTopoManager ( ).splitBlocksWithOgridV2 (blocksNames, facesNames, ratio, centralEdgesNum);

	setCommandResult (cmdResult);
}	// QtTopologySplitBlockWithOGridAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
