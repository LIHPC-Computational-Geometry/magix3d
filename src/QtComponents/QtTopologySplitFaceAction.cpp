/**
 * \file        QtTopologySplitFaceAction.cpp
 * \author      Charles PIGNEROL
 * \date        17/03/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "QtComponents/QtTopologySplitFaceAction.h"
#include "Geom/GeomEntity.h"
#include "Topo/CommandSplitFaces.h"
#include "Topo/TopoDisplayRepresentation.h"
#include "Topo/Vertex.h"

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
//                        LA CLASSE QtTopologySplitFacePanel
// ===========================================================================

QtTopologySplitFacePanel::QtTopologySplitFacePanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).splitFaceOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).splitFaceOperationTag),
	  _facePanel (0), _edgePanel (0), _cutDefinitionButtonGroup (0),
	  _ratioTextField (0), _cutPointEntityPanel (0),
	  _projectVerticesCheckBox (0)
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
	_facePanel->setToolTip (QString::fromUtf8("Face soumise au découpage."));
	connect (_facePanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_facePanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_facePanel);

	// L'arête orthogonale au plan de coupe :
	_edgePanel	= new QtMgx3DEntityPanel (
			this, "", true, "Arête :", "", &mainWindow,
			SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	_edgePanel->setToolTip (QString::fromUtf8("Arête orthogonale au plan de coupe."));
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
						ratioRadioButton, QtTopologySplitFacePanel::CDM_RATIO);
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
						entityRadioButton, QtTopologySplitFacePanel::CDM_POINT);
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

	// Faut il projeter les sommets créés sur la discrétisation initiale ?
	_projectVerticesCheckBox	= new QCheckBox (
			"Projeter les sommets créés sur la discrétisation initiale", this);
	_projectVerticesCheckBox->setChecked (true);
	layout->addWidget (_projectVerticesCheckBox);
	connect (_projectVerticesCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (parametersModifiedCallback ( )));

	addPreviewCheckBox (false);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_facePanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_edgePanel->getNameTextField ( ))
	_facePanel->getNameTextField (
			)->setLinkedSeizureManagers (0, _edgePanel->getNameTextField ( ));
	_edgePanel->getNameTextField (
			)->setLinkedSeizureManagers (_facePanel->getNameTextField ( ), 0);

	cutDefinitionModifiedCallback ( );
}	// QtTopologySplitFacePanel::QtTopologySplitFacePanel


QtTopologySplitFacePanel::QtTopologySplitFacePanel (
										const QtTopologySplitFacePanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _facePanel (0), _edgePanel (0), _cutDefinitionButtonGroup (0),
	  _ratioTextField (0), _cutPointEntityPanel (0),
	  _projectVerticesCheckBox (0)
{
	MGX_FORBIDDEN ("QtTopologySplitFacePanel copy constructor is not allowed.");
}	// QtTopologySplitFacePanel::QtTopologySplitFacePanel (const QtTopologySplitFacePanel&)


QtTopologySplitFacePanel& QtTopologySplitFacePanel::operator = (
											const QtTopologySplitFacePanel&)
{
	MGX_FORBIDDEN ("QtTopologySplitFacePanel assignment operator is not allowed.");
	return *this;
}	// QtTopologySplitFacePanel::QtTopologySplitFacePanel (const QtTopologySplitFacePanel&)


QtTopologySplitFacePanel::~QtTopologySplitFacePanel ( )
{
}	// QtTopologySplitFacePanel::~QtTopologySplitFacePanel


void QtTopologySplitFacePanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_facePanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
	_facePanel->reset ( );
	_edgePanel->reset ( );
	_ratioTextField->setValue (0.5);
	_cutPointEntityPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologySplitFacePanel::reset


void QtTopologySplitFacePanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_facePanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
	_facePanel->stopSelection ( );
	_edgePanel->stopSelection ( );
	_cutPointEntityPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_facePanel->setUniqueName ("");
		_edgePanel->setUniqueName ("");
		_cutPointEntityPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologySplitFacePanel::cancel


void QtTopologySplitFacePanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_facePanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		const vector<string>	selectedFaces	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoFace);
		if (1 == selectedFaces.size ( ))
			_facePanel->setUniqueName (selectedFaces [0]);
		const vector<string>	selectedEdges	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
		if (1 == selectedEdges.size ( ))
			_edgePanel->setUniqueName (selectedEdges [0]);
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_facePanel->setUniqueName ("");
	_edgePanel->setUniqueName ("");
	_cutPointEntityPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_facePanel->actualizeGui (true);
	_edgePanel->actualizeGui (true);
	_cutPointEntityPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologySplitFacePanel::autoUpdate


string QtTopologySplitFacePanel::getFaceName ( ) const
{
	CHECK_NULL_PTR_ERROR (_facePanel)
	return _facePanel->getUniqueName ( );
}	// QtTopologySplitFacePanel::getFaceName


string QtTopologySplitFacePanel::getEdgeName ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
	return _edgePanel->getUniqueName ( );
}	// QtTopologySplitFacePanel::getEdgeName


QtTopologySplitFacePanel::CUT_DEFINITION_METHOD
					QtTopologySplitFacePanel::getCutDefinitionMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_cutDefinitionButtonGroup)
	return (QtTopologySplitFacePanel::CUT_DEFINITION_METHOD)_cutDefinitionButtonGroup->checkedId ( );
}	// QtTopologySplitFacePanel::getCutDefinitionMethod


double QtTopologySplitFacePanel::getRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	return _ratioTextField->getValue ( );
}	// QtTopologySplitFacePanel::getRatio


Math::Point QtTopologySplitFacePanel::getCutPoint ( ) const
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
}	// QtTopologySplitFacePanel::getCutPoint


bool QtTopologySplitFacePanel::projectCreatedVertices ( ) const
{
	CHECK_NULL_PTR_ERROR (_projectVerticesCheckBox)
	return Qt::Checked == _projectVerticesCheckBox->checkState ( ) ?
	       true : false;
}	// QtTopologySplitFacePanel::projectCreatedVertices


void QtTopologySplitFacePanel::preview (bool show, bool destroyInteractor)
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
		const string	edgeName	= getEdgeName ( );
		const bool projectVertices	= projectCreatedVertices ( );
		CoFace*			face		=
				getContext ( ).getTopoManager ( ).getCoFace (faceName, true);
		CoEdge*			edge		=
				 getContext ( ).getTopoManager ( ).getCoEdge (edgeName, true);
		std::unique_ptr<CommandSplitFaces>	command;
		switch (getCutDefinitionMethod ( ))
		{
			case QtTopologySplitFacePanel::CDM_RATIO	:
				command.reset (new CommandSplitFaces (
					*context, face, edge, getRatio ( ), projectVertices));
			break;
			case QtTopologySplitFacePanel::CDM_POINT	:
				command.reset (new CommandSplitFaces (
					*context, face, edge, getCutPoint ( ), projectVertices));
			break;
			default										:
				throw Exception (UTF8String ("QtTopologySplitFacePanel::preview : cas non implémenté.", Charset::UTF_8));
		}	// switch (getCutDefinitionMethod ( ))
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
}	// QtTopologySplitFacePanel::preview


vector<Entity*> QtTopologySplitFacePanel::getInvolvedEntities ( )
{
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)

	vector<Entity*>	entities;
	const string	faceName	= getFaceName ( );
	const string	edgeName	= getEdgeName ( );
	CoFace*			face		=
			getContext ( ).getTopoManager ( ).getCoFace (faceName, false);
	if (0 != face)
		entities.push_back (face);
	TopoEntity*		edge	=
			getContext ( ).getTopoManager ( ).getCoEdge (edgeName, false);
	if (0 != edge)
		entities.push_back (edge);

	switch (getCutDefinitionMethod ( ))
	{
		case QtTopologySplitFacePanel::CDM_POINT	:
		{
			CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
			const string	point	= _cutPointEntityPanel->getUniqueName ( );
			if (0 != point.length ( ))
				entities.push_back (&getContext().nameToEntity (point));
		}
		break;
	}	// switch (getCutDefinitionMethod ( ))

	return entities;
}	// QtTopologySplitFacePanel::getInvolvedEntities


void QtTopologySplitFacePanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _facePanel)
	{
		_facePanel->stopSelection ( );
		_facePanel->setUniqueName ("" );
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
}	// QtTopologySplitFacePanel::operationCompleted


void QtTopologySplitFacePanel::cutDefinitionModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_ratioTextField)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)

	switch (getCutDefinitionMethod ( ))
	{
		case QtTopologySplitFacePanel::CDM_RATIO	:
			_ratioTextField->setEnabled (true);
			_cutPointEntityPanel->setEnabled (false);
			break;
		case QtTopologySplitFacePanel::CDM_POINT	:
			_ratioTextField->setEnabled (false);
			_cutPointEntityPanel->setEnabled (true);
			break;
	}	// switch (getCutDefinitionMethod ( ))

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologySplitFacePanel::cutDefinitionModifiedCallback


// ===========================================================================
//                  LA CLASSE QtTopologySplitFaceAction
// ===========================================================================

QtTopologySplitFaceAction::QtTopologySplitFaceAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologySplitFacePanel*	operationPanel	= 
			new QtTopologySplitFacePanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologySplitFaceAction::QtTopologySplitFaceAction


QtTopologySplitFaceAction::QtTopologySplitFaceAction (
										const QtTopologySplitFaceAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologySplitFaceAction copy constructor is not allowed.")
}	// QtTopologySplitFaceAction::QtTopologySplitFaceAction


QtTopologySplitFaceAction& QtTopologySplitFaceAction::operator = (
										const QtTopologySplitFaceAction&)
{
	MGX_FORBIDDEN ("QtTopologySplitFaceAction assignment operator is not allowed.")
	return *this;
}	// QtTopologySplitFaceAction::operator =


QtTopologySplitFaceAction::~QtTopologySplitFaceAction ( )
{
}	// QtTopologySplitFaceAction::~QtTopologySplitFaceAction


QtTopologySplitFacePanel*
					QtTopologySplitFaceAction::getTopologySplitFacePanel ( )
{
	return dynamic_cast<QtTopologySplitFacePanel*>(getOperationPanel ( ));
}	// QtTopologySplitFaceAction::getTopologySplitFacePanel


void QtTopologySplitFaceAction::executeOperation ( )
{
	QtTopologySplitFacePanel*	panel	= dynamic_cast<QtTopologySplitFacePanel*>(getTopologySplitFacePanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de découpage de la face topologique :
	const string	faceName		= panel->getFaceName ( );
	const string	edgeName		= panel->getEdgeName ( );
	const bool	 	projectVertices	= panel->projectCreatedVertices ( );

	Mgx3D::Internal::M3DCommandResultIfc*	result	= 0;
	switch (panel->getCutDefinitionMethod ( ))
	{
		case QtTopologySplitFacePanel::CDM_RATIO	:
			result	= getContext ( ).getTopoManager ( ).splitFace (
				faceName, edgeName, panel->getRatio ( ), projectVertices);
		break;
		case QtTopologySplitFacePanel::CDM_POINT	:
			result	= getContext ( ).getTopoManager ( ).splitFace (
				faceName, edgeName, panel->getCutPoint ( ), projectVertices);
		break;
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Position de découpe non supportée ("
			        << (unsigned long)panel->getCutDefinitionMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtTopologySplitFaceAction::executeOperation")
			throw exc;
		}
	}	// switch (panel->getCutDefinitionMethod ( ))
	CHECK_NULL_PTR_ERROR (result)
	setCommandResult (result);
	if (CommandIfc::FAIL == result->getStatus ( ))
		throw Exception (result->getErrorMessage ( ));
	else if (CommandIfc::CANCELED == result->getStatus ( ))
		throw Exception ("Opération annulée");
}	// QtTopologySplitFaceAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
