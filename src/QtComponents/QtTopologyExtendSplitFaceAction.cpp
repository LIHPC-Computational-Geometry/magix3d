/**
 * \file        QtTopologyExtendSplitFaceAction.cpp
 * \author      Charles PIGNEROL
 * \date        22/05/2014
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyExtendSplitFaceAction.h"
#include "Topo/CommandExtendSplitFace.h"
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
//                        LA CLASSE QtTopologyExtendSplitFacePanel
// ===========================================================================

QtTopologyExtendSplitFacePanel::QtTopologyExtendSplitFacePanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).extendSplitFacesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).extendSplitFacesOperationTag),
	  _facePanel (0), _vertexPanel (0)
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

	// La face à découper :
	_facePanel	= new QtMgx3DEntityPanel (
			this, "", true, "Face  :", "", &mainWindow,
			SelectionManagerIfc::D2, FilterEntity::TopoCoFace);
	connect (_facePanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_facePanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_facePanel);

	// L'arête orthogonale au plan de coupe :
	_vertexPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Sommet :", "", &mainWindow,
			SelectionManagerIfc::D0, FilterEntity::TopoVertex);
	connect (_vertexPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_vertexPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_vertexPanel);

	addPreviewCheckBox (false);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_facePanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_vertexPanel->getNameTextField ( ))
	_facePanel->getNameTextField (
			)->setLinkedSeizureManagers (0, _vertexPanel->getNameTextField ( ));
	_vertexPanel->getNameTextField (
			)->setLinkedSeizureManagers (_facePanel->getNameTextField ( ), 0);
}	// QtTopologyExtendSplitFacePanel::QtTopologyExtendSplitFacePanel


QtTopologyExtendSplitFacePanel::QtTopologyExtendSplitFacePanel (
										const QtTopologyExtendSplitFacePanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _facePanel (0), _vertexPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyExtendSplitFacePanel copy constructor is not allowed.");
}	// QtTopologyExtendSplitFacePanel::QtTopologyExtendSplitFacePanel (const QtTopologyExtendSplitFacePanel&)


QtTopologyExtendSplitFacePanel& QtTopologyExtendSplitFacePanel::operator = (
											const QtTopologyExtendSplitFacePanel&)
{
	MGX_FORBIDDEN ("QtTopologyExtendSplitFacePanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyExtendSplitFacePanel::QtTopologyExtendSplitFacePanel (const QtTopologyExtendSplitFacePanel&)


QtTopologyExtendSplitFacePanel::~QtTopologyExtendSplitFacePanel ( )
{
}	// QtTopologyExtendSplitFacePanel::~QtTopologyExtendSplitFacePanel


void QtTopologyExtendSplitFacePanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_facePanel)
	CHECK_NULL_PTR_ERROR (_vertexPanel)
	_facePanel->reset ( );
	_vertexPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyExtendSplitFacePanel::reset


void QtTopologyExtendSplitFacePanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_facePanel)
	CHECK_NULL_PTR_ERROR (_vertexPanel)
	_facePanel->stopSelection ( );
	_vertexPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_facePanel->setUniqueName ("");
		_vertexPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyExtendSplitFacePanel::cancel


void QtTopologyExtendSplitFacePanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_facePanel)
	CHECK_NULL_PTR_ERROR (_vertexPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedFaces	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoFace);
		if (1 == selectedFaces.size ( ))
			_facePanel->setUniqueName (selectedFaces [0]);
		vector<string>	selectedVertices	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoVertex);
		if (1 == selectedVertices.size ( ))
			_vertexPanel->setUniqueName (selectedVertices [0]);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_facePanel->setUniqueName ("");
	_vertexPanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	_facePanel->actualizeGui (true);
	_vertexPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyExtendSplitFacePanel::autoUpdate


string QtTopologyExtendSplitFacePanel::getFaceName ( ) const
{
	CHECK_NULL_PTR_ERROR (_facePanel)
	return _facePanel->getUniqueName ( );
}	// QtTopologyExtendSplitFacePanel::getFaceName


string QtTopologyExtendSplitFacePanel::getVertexName ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertexPanel)
	return _vertexPanel->getUniqueName ( );
}	// QtTopologyExtendSplitFacePanel::getVertexName


void QtTopologyExtendSplitFacePanel::preview (
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
		const string	faceName	= getFaceName ( );
		const string	vertexName	= getVertexName ( );
		CoFace*			face		=
				getContext ( ).getTopoManager ( ).getCoFace (faceName, true);
		Topo::Vertex*	vertex		=
				 getContext ( ).getTopoManager ( ).getVertex (vertexName, true);
		CommandExtendSplitFace			command (*context, face, vertex);
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
}	// QtTopologyExtendSplitFacePanel::preview


vector<Entity*> QtTopologyExtendSplitFacePanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const string	faceName	= getFaceName ( );
	const string	vertexName	= getVertexName ( );
	TopoEntity*		face	=
			getContext ( ).getTopoManager ( ).getCoFace (faceName, false);
	TopoEntity*		vertex	=
			getContext ( ).getTopoManager ( ).getVertex (vertexName, false);
	if (0 != face)
		entities.push_back (face);
	if (0 != vertex)
		entities.push_back (vertex);

	return entities;
}	// QtTopologyExtendSplitFacePanel::getInvolvedEntities


void QtTopologyExtendSplitFacePanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _facePanel)
	{
		_facePanel->stopSelection ( );
		_facePanel->setUniqueName ("" );
	}
	if (0 != _vertexPanel)
	{
		_vertexPanel->stopSelection ( );
		_vertexPanel->setUniqueName ("");
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyExtendSplitFacePanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyExtendSplitFaceAction
// ===========================================================================

QtTopologyExtendSplitFaceAction::QtTopologyExtendSplitFaceAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyExtendSplitFacePanel*	operationPanel	= 
			new QtTopologyExtendSplitFacePanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyExtendSplitFaceAction::QtTopologyExtendSplitFaceAction


QtTopologyExtendSplitFaceAction::QtTopologyExtendSplitFaceAction (
										const QtTopologyExtendSplitFaceAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyExtendSplitFaceAction copy constructor is not allowed.")
}	// QtTopologyExtendSplitFaceAction::QtTopologyExtendSplitFaceAction


QtTopologyExtendSplitFaceAction& QtTopologyExtendSplitFaceAction::operator = (
										const QtTopologyExtendSplitFaceAction&)
{
	MGX_FORBIDDEN ("QtTopologyExtendSplitFaceAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyExtendSplitFaceAction::operator =


QtTopologyExtendSplitFaceAction::~QtTopologyExtendSplitFaceAction ( )
{
}	// QtTopologyExtendSplitFaceAction::~QtTopologyExtendSplitFaceAction


QtTopologyExtendSplitFacePanel*
					QtTopologyExtendSplitFaceAction::getTopologyExtendSplitFacePanel ( )
{
	return dynamic_cast<QtTopologyExtendSplitFacePanel*>(getOperationPanel ( ));
}	// QtTopologyExtendSplitFaceAction::getTopologyExtendSplitFacePanel


void QtTopologyExtendSplitFaceAction::executeOperation ( )
{
	QtTopologyExtendSplitFacePanel*	panel	= dynamic_cast<QtTopologyExtendSplitFacePanel*>(getTopologyExtendSplitFacePanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de découpage des faces topologiques :
	const string	faceName	= panel->getFaceName ( );
	const string	vertexName	= panel->getVertexName ( );

	Mgx3D::Internal::M3DCommandResultIfc*	result	= getContext ( ).getTopoManager ( ).extendSplitFace (faceName, vertexName);
	CHECK_NULL_PTR_ERROR (result)
	setCommandResult (result);
	if (CommandIfc::FAIL == result->getStatus ( ))
		throw Exception (result->getErrorMessage ( ));
	else if (CommandIfc::CANCELED == result->getStatus ( ))
		throw Exception ("Opération annulée");
}	// QtTopologyExtendSplitFaceAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
