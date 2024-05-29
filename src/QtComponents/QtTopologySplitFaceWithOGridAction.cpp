/**
 * \file        QtTopologySplitFaceWithOGridAction.cpp
 * \author		Charles PIGNEROL - Eric BRIERE DE L'ISLE
 * \date		23/11/2017
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "QtComponents/QtTopologySplitFaceWithOGridAction.h"
#include "Topo/CommandSplitFacesWithOgrid.h"
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
//                        LA CLASSE QtTopologySplitFaceWithOGridPanel
// ===========================================================================

QtTopologySplitFaceWithOGridPanel::QtTopologySplitFaceWithOGridPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).splitFaceWithOGridOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).splitFaceWithOGridOperationTag),
	  _facesPanel (0), _edgesPanel (0),
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

	// Les faces à découper :
	_facesPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Faces :", "", &mainWindow,
			SelectionManagerIfc::D2, FilterEntity::TopoCoFace);
	_facesPanel->setMultiSelectMode (true);
	connect (_facesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_facesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_facesPanel);

	// Les éventuelles arêtes à découper :
	_edgesPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Arêtes :", "", &mainWindow,
			SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	_edgesPanel->setMultiSelectMode (true);
	connect (_edgesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edgesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_edgesPanel);

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

	// Le nombre de bras par arête du tour de la face centrale :
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	hlayout->setContentsMargins  (0, 0, 0, 0);
	label	= new QLabel ("Nombre de bras :", this);
	hlayout->addWidget (label);
	_edgesNumTextField	= new QtIntTextField (10, this);
	hlayout->addWidget (_edgesNumTextField);
	_edgesNumTextField->setToolTip (QString::fromUtf8("Nombre de bras par arête du tour de la face centrale."));
	connect (_edgesNumTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (parametersModifiedCallback ( )));

	addPreviewCheckBox (false);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_edgesPanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_facesPanel->getNameTextField ( ))
	_edgesPanel->getNameTextField (
			)->setLinkedSeizureManagers (0, _facesPanel->getNameTextField ( ));
	_facesPanel->getNameTextField (
			)->setLinkedSeizureManagers (_edgesPanel->getNameTextField ( ), 0);
}	// QtTopologySplitFaceWithOGridPanel::QtTopologySplitFaceWithOGridPanel


QtTopologySplitFaceWithOGridPanel::QtTopologySplitFaceWithOGridPanel (
								const QtTopologySplitFaceWithOGridPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _facesPanel (0), _edgesPanel (0),
	  _ratioTextField (0), _edgesNumTextField (0)
{
	MGX_FORBIDDEN ("QtTopologySplitFaceWithOGridPanel copy constructor is not allowed.");
}	// QtTopologySplitFaceWithOGridPanel::QtTopologySplitFaceWithOGridPanel (const QtTopologySplitFaceWithOGridPanel&)


QtTopologySplitFaceWithOGridPanel&
						QtTopologySplitFaceWithOGridPanel::operator = (
									const QtTopologySplitFaceWithOGridPanel&)
{
	MGX_FORBIDDEN ("QtTopologySplitFaceWithOGridPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologySplitFaceWithOGridPanel::QtTopologySplitFaceWithOGridPanel (const QtTopologySplitFaceWithOGridPanel&)


QtTopologySplitFaceWithOGridPanel::~QtTopologySplitFaceWithOGridPanel ( )
{
}	// QtTopologySplitFaceWithOGridPanel::~QtTopologySplitFaceWithOGridPanel


void QtTopologySplitFaceWithOGridPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgesPanel)
	CHECK_NULL_PTR_ERROR (_facesPanel)
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	CHECK_NULL_PTR_ERROR (_edgesNumTextField)
	_edgesPanel->reset ( );
	_facesPanel->reset ( );
	_ratioTextField->setValue (0.5);
	_edgesNumTextField->setValue (10);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologySplitFaceWithOGridPanel::reset


void QtTopologySplitFaceWithOGridPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_edgesPanel)
	CHECK_NULL_PTR_ERROR (_facesPanel)
	_edgesPanel->stopSelection ( );
	_facesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_edgesPanel->setUniqueName ("");
		_facesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologySplitFaceWithOGridPanel::cancel


void QtTopologySplitFaceWithOGridPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_edgesPanel)
	CHECK_NULL_PTR_ERROR (_facesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
//	if (true == autoUpdateUsesSelection ( ))
//	{
//		BEGIN_QT_TRY_CATCH_BLOCK
//
//		const vector<string>	selectedBlocks	=
//			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoBlock);
//		if (0 != selectedBlocks.size ( ))
//			_blocksPanel->setUniqueNames (selectedBlocks);
//		const vector<string>	selectedFaces	=
//			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoFace);
//		if (0 != selectedFaces.size ( ))
//			_facesPanel->setUniqueNames (selectedFaces);
//
//		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
//	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_edgesPanel->clearSelection ( );
	_facesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_edgesPanel->actualizeGui (true);
	_facesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologySplitFaceWithOGridPanel::autoUpdate


vector<string> QtTopologySplitFaceWithOGridPanel::getEdgesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgesPanel)
	return _edgesPanel->getUniqueNames ( );
}	// QtTopologySplitFaceWithOGridPanel::getEdgesNames


vector<string> QtTopologySplitFaceWithOGridPanel::getFacesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_facesPanel)
	return _facesPanel->getUniqueNames ( );
}	// QtTopologySplitFaceWithOGridPanel::getFacesNames


double QtTopologySplitFaceWithOGridPanel::getRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	return _ratioTextField->getValue ( );
}	// QtTopologySplitFaceWithOGridPanel::getRatio


size_t QtTopologySplitFaceWithOGridPanel::getCentralEdgesNumber ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgesNumTextField)
	return _edgesNumTextField->getValue ( );
}	// QtTopologySplitFaceWithOGridPanel::getCentralEdgesNumber


void QtTopologySplitFaceWithOGridPanel::preview (
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
		const vector<string>	edgesNames	= getEdgesNames ( );
		const vector<string>	facesNames	= getFacesNames ( );
		const double			ratio		= getRatio ( );
		const int				edgesNum	= getCentralEdgesNumber ( );
		vector<CoEdge*>			coedges;
		vector<CoFace*>			faces;
		for (vector<string>::const_iterator it = edgesNames.begin ( );
				edgesNames.end ( ) != it; it++)
		{
			CoEdge*	coedge		=
				getContext ( ).getTopoManager ( ).getCoEdge (*it, true);
			coedges.push_back (coedge);
		}	// for (vector<string>::const_iterator it = edgesNames.begin ( );
		for (vector<string>::const_iterator it = facesNames.begin ( );
		     facesNames.end ( ) != it; it++)
		{
			CoFace*	face		=
				getContext ( ).getTopoManager ( ).getCoFace (*it, true);
			faces.push_back (face);
		}	// for (vector<string>::const_iterator it = facesNames.begin ( );
		CommandSplitFacesWithOgrid	command (
									*context, faces, coedges, ratio, edgesNum);
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
}	// QtTopologySplitFaceWithOGridPanel::preview


vector<Entity*> QtTopologySplitFaceWithOGridPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const vector<string>	edgesNames	= getEdgesNames ( );
	const vector<string>	facesNames	= getFacesNames ( );
	for (vector<string>::const_iterator it = edgesNames.begin ( );
			edgesNames.end ( ) != it; it++)
	{
		TopoEntity*	te	= getContext( ).getTopoManager( ).getCoEdge(*it, false);
		if (0 != te)
			entities.push_back (te);
	}	// for (vector<string>::const_iterator it = edgesNames.begin ( ); ...
	for (vector<string>::const_iterator it = facesNames.begin ( );
	     facesNames.end ( ) != it; it++)
	{
		TopoEntity*	te	= getContext( ).getTopoManager( ).getCoFace(*it, false);
		if (0 != te)
			entities.push_back (te);
	}	// for (vector<string>::const_iterator it = facesNames.begin ( ); ...

	return entities;
}	// QtTopologySplitFaceWithOGridPanel::getInvolvedEntities


void QtTopologySplitFaceWithOGridPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _edgesPanel)
	{
		_edgesPanel->stopSelection ( );
		_edgesPanel->setUniqueName ("" );
	}	// if (0 != _edgesPanel)
	if (0 != _facesPanel)
	{
		_facesPanel->stopSelection ( );
		_facesPanel->setUniqueName ("");
	}	// if (0 != _facesPanel)

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologySplitFaceWithOGridPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologySplitFaceWithOGridAction
// ===========================================================================

QtTopologySplitFaceWithOGridAction::QtTopologySplitFaceWithOGridAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologySplitFaceWithOGridPanel*	operationPanel	=
			new QtTopologySplitFaceWithOGridPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologySplitFaceWithOGridAction::QtTopologySplitFaceWithOGridAction


QtTopologySplitFaceWithOGridAction::QtTopologySplitFaceWithOGridAction (
									const QtTopologySplitFaceWithOGridAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologySplitFaceWithOGridAction copy constructor is not allowed.")
}	// QtTopologySplitFaceWithOGridAction::QtTopologySplitFaceWithOGridAction


QtTopologySplitFaceWithOGridAction&
							QtTopologySplitFaceWithOGridAction::operator = (
									const QtTopologySplitFaceWithOGridAction&)
{
	MGX_FORBIDDEN ("QtTopologySplitFaceWithOGridAction assignment operator is not allowed.")
	return *this;
}	// QtTopologySplitFaceWithOGridAction::operator =


QtTopologySplitFaceWithOGridAction::~QtTopologySplitFaceWithOGridAction ( )
{
}	// QtTopologySplitFaceWithOGridAction::~QtTopologySplitFaceWithOGridAction


QtTopologySplitFaceWithOGridPanel*
	QtTopologySplitFaceWithOGridAction::getTopologySplitFaceWithOGridPanel ( )
{
	return dynamic_cast<QtTopologySplitFaceWithOGridPanel*>(getOperationPanel ( ));
}	// QtTopologySplitFaceWithOGridAction::getTopologySplitFaceWithOGridPanel


void QtTopologySplitFaceWithOGridAction::executeOperation ( )
{
	QtTopologySplitFaceWithOGridPanel*	panel	= dynamic_cast<QtTopologySplitFaceWithOGridPanel*>(getTopologySplitFaceWithOGridPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de découpage en o-grid des faces
	// topologiques :
	vector<string>	edgesNames		= panel->getEdgesNames ( );
	vector<string>	facesNames		= panel->getFacesNames ( );
	double			ratio			= panel->getRatio ( );
	size_t			centralEdgesNum	= panel->getCentralEdgesNumber ( );

	cmdResult	= getContext ( ).getTopoManager ( ).splitFacesWithOgrid (facesNames, edgesNames, ratio, centralEdgesNum);

	setCommandResult (cmdResult);
}	// QtTopologySplitFaceWithOGridAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
