/**
 * \file        QtGeomPlaneCutAction.cpp
 * \author      Charles PIGNEROL
 * \date        22/11/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/Plane.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "Utils/Vector.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeomPlaneCutAction.h"
#include "QtComponents/RenderedEntityRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QVBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Group;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtGeomPlaneCutPanel
// ===========================================================================

QtGeomPlaneCutPanel::QtGeomPlaneCutPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			const string& entitiesLabel,
			SelectionManagerIfc::DIM dimensions, FilterEntity::objectType types,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).geomPlaneCutOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).geomPlaneCutOperationTag),
	  RenderingManager::InteractorObserver ( ),
	  _namePanel (0), _pointPanel (0), _normalPanel (0),
	  _entitiesPanel (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Nom groupe :
	_namePanel	= new QtMgx3DGroupNamePanel (
							this, "Groupe", mainWindow, 2, creationPolicy, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// "Centre" du plan :
	_pointPanel	= new QtMgx3DPointPanel (
		this, "Point ", true, "x :", "y :", "z :",
		0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
		&mainWindow, FilterEntity::GeomVertex, true);
	_pointPanel->layout ( )->setSpacing (5);
	connect (_pointPanel, SIGNAL (pointModified ( )), this,
	         SLOT (cutModifiedCallback ( )));
	layout->addWidget (_pointPanel);

	// Normale :
	_normalPanel	= new QtMgx3DVectorPanel (
		this, "Normale", true, "dx :", "dy :", "dz :",
		0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 1., -DBL_MAX, DBL_MAX,
		&mainWindow, FilterEntity::AllEdges, true);
	_normalPanel->layout ( )->setSpacing (5);
	connect (_normalPanel, SIGNAL (vectorModified ( )), this,
	         SLOT (cutModifiedCallback ( )));
	layout->addWidget (_normalPanel);

	// Entités concernées :
	// On met le label ici afin de laisser le plus de place possible au champ
	// de texte recevant la liste des entités.
	label	= new QLabel (QString::fromUtf8("Entités à couper"), this);
	layout->addWidget (label);
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	SelectionManagerIfc::DIM allowedDimensions	= (SelectionManagerIfc::DIM)(
		SelectionManagerIfc::D1|SelectionManagerIfc::D2|SelectionManagerIfc::D3);
	_entitiesPanel	= new QtEntityByDimensionSelectorPanel (
			this, mainWindow, "", allowedDimensions, types, dimensions, true);
	hlayout->addWidget (_entitiesPanel);
	_entitiesPanel->setMultiSelectMode (true);
	connect (_entitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_entitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	hlayout->addWidget (_entitiesPanel);

	addPreviewCheckBox (false);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_pointPanel->getVertexTextField ( ))
	CHECK_NULL_PTR_ERROR (_normalPanel->getSegmentIDTextField ( ))
	CHECK_NULL_PTR_ERROR(_entitiesPanel->getEntitiesPanel()->getNameTextField())
	_pointPanel->getVertexTextField (
		)->setLinkedSeizureManagers (0, _normalPanel->getSegmentIDTextField( ));
	_normalPanel->getSegmentIDTextField (
		)->setLinkedSeizureManagers (_pointPanel->getVertexTextField ( ),
				_entitiesPanel->getEntitiesPanel()->getNameTextField ( ));
	_entitiesPanel->getEntitiesPanel ( )->getNameTextField (
		)->setLinkedSeizureManagers (_pointPanel->getVertexTextField ( ), 0);
}	// QtGeomPlaneCutPanel::QtGeomPlaneCutPanel


QtGeomPlaneCutPanel::QtGeomPlaneCutPanel (const QtGeomPlaneCutPanel& cp)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  RenderingManager::InteractorObserver ( ),
	  _namePanel (0), _pointPanel (0), _normalPanel (0), _entitiesPanel (0)
{
	MGX_FORBIDDEN ("QtGeomPlaneCutPanel copy constructor is not allowed.");
}	// QtGeomPlaneCutPanel::QtGeomPlaneCutPanel (const QtGeomPlaneCutPanel&)


QtGeomPlaneCutPanel& QtGeomPlaneCutPanel::operator = (const QtGeomPlaneCutPanel&)
{
	MGX_FORBIDDEN ("QtGeomPlaneCutPanel assignment operator is not allowed.");
	return *this;
}	// QtGeomPlaneCutPanel::QtGeomPlaneCutPanel (const QtGeomPlaneCutPanel&)


QtGeomPlaneCutPanel::~QtGeomPlaneCutPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtGeomPlaneCutPanel::~QtGeomPlaneCutPanel


string QtGeomPlaneCutPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtGeomPlaneCutPanel::getGroupName


Math::Point QtGeomPlaneCutPanel::getPoint ( ) const
{
	CHECK_NULL_PTR_ERROR (_pointPanel)
	return _pointPanel->getPoint();
}	// QtGeomPlaneCutPanel::getPoint


void QtGeomPlaneCutPanel::getNormal (double& dx, double& dy, double& dz) const
{
	CHECK_NULL_PTR_ERROR (_normalPanel)
	dx	= _normalPanel->getDx ( );
	dy	= _normalPanel->getDy ( );
	dz	= _normalPanel->getDz ( );
}	// QtGeomPlaneCutPanel::getNormal


Math::Vector QtGeomPlaneCutPanel::getNormal ( ) const
{
	double	dx	= 0., dy	= 0., dz	= 0.;
	getNormal (dx, dy, dz);

	return Math::Vector (dx, dy, dz);
}	// QtGeomPlaneCutPanel::getNormal


vector<string> QtGeomPlaneCutPanel::getCutEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	return _entitiesPanel->getEntitiesNames ( );
}	// QtGeomPlaneCutPanel::getCutEntities


void QtGeomPlaneCutPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel)
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	_namePanel->autoUpdate ( );
	_pointPanel->reset ( );
	_normalPanel->reset ( );
	_entitiesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeomPlaneCutPanel::reset


void QtGeomPlaneCutPanel::validate ( )
{
// CP : suite discussion EBL/FL, il est convenu que la validation des
// paramètres de l'opération est effectuée par le "noyau" et qu'un mauvais
// paramétrage est remonté sous forme d'exception à la fonction appelante, donc
// avant exécution de la commande.
// Les validations des valeurs des paramètres sont donc ici commentées.
	UTF8String	error (Charset::UTF_8);

	try
	{
		QtMgx3DOperationPanel::validate ( );
	}
	catch (const Exception& exc)
	{
		error << exc.getFullMessage ( );
	}
	catch (...)
	{
		error << "QtGeomPlaneCutPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX1 ( ), "Abscisse point 1 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY1 ( ), "Ordonnée point 1 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ1 ( ), "Elévation point 1 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX2 ( ), "Abscisse point 2 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY2 ( ), "Ordonnée point 2 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ2 ( ), "Elévation point 2 : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGeomPlaneCutPanel::validate


void QtGeomPlaneCutPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel)
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	_pointPanel->stopSelection ( );
	_normalPanel->stopSelection ( );
	_entitiesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_pointPanel->setUniqueName ("");
		_entitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeomPlaneCutPanel::cancel


void QtGeomPlaneCutPanel::autoUpdate ( )
{
	QtMgx3DOperationPanel::autoUpdate ( );
	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel->getSegmentIDTextField ( ))
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	_namePanel->autoUpdate ( );

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	points	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomVertex);
		if (1 == points.size ( ))
			_pointPanel->setUniqueName (points [0]);
		const vector<string>	edges	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::AllEdges);
		if (1 == edges.size ( ))
			_normalPanel->setSegment (edges [0]);
		const vector<string>	entities	=
			getSelectionManager ( ).getEntitiesNames (
										_entitiesPanel->getAllowedTypes ( ));
		if (0 != entities.size ( ))
			_entitiesPanel->setEntitiesNames (entities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_pointPanel->clearSelection ( );
	_normalPanel->setSegment ("");
	_entitiesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationPanel::autoUpdate ( );

	_pointPanel->actualizeGui (true);
	_normalPanel->actualizeGui (true);
	_normalPanel->getSegmentIDTextField ( )->actualizeGui (true);
	_entitiesPanel->actualizeGui (true);
}	// QtGeomPlaneCutPanel::autoUpdate


void QtGeomPlaneCutPanel::planeModifiedCallback (
										Math::Point point, Math::Vector normal)
{
//	RenderingManager::PlaneInteractor*	interactor	= getInteractor ( );
//	if (0 == interactor)
//		return;

	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel)
//	Math::Point		point	= interactor->getPoint ( );
//	Math::Vector	normal	= interactor->getNormal ( );
	_pointPanel->blockFieldsSignals (true);
	_normalPanel->blockFieldsSignals (true);
	_pointPanel->setX (point.getX ( ));
	_pointPanel->setY (point.getY ( ));
	_pointPanel->setZ (point.getZ ( ));
	_normalPanel->setDx (normal.getX ( ));
	_normalPanel->setDy (normal.getY ( ));
	_normalPanel->setDz (normal.getZ ( ));
	_pointPanel->blockFieldsSignals (false);
	_normalPanel->blockFieldsSignals (false);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeomPlaneCutPanel::planeModifiedCallback


void QtGeomPlaneCutPanel::clearEntitiesField ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entitiesPanel)

	preview (false, true);
	vector<string>	names;
	_entitiesPanel->clearSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeomPlaneCutPanel::clearEntitiesField


vector<Entity*> QtGeomPlaneCutPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	names	= getCutEntities ( );
	for (vector<string>::const_iterator it = names.begin ( );
		     names.end ( ) != it; it++)
	{
		GeomEntity*	entity	= getContext ( ).getGeomManager ( ).getEntity (*it);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}	// for (vector<string>::const_iterator it = names.begin ( );

	return entities;
}	// QtGeomPlaneCutPanel::getInvolvedEntities


RenderingManager::PlaneInteractor* QtGeomPlaneCutPanel::getInteractor ( )
{
	vector <RenderingManager::Interactor*>	interactors	=
												getPreviewedInteractors ( );
	if (0 != interactors.size ( ))
		return dynamic_cast<RenderingManager::PlaneInteractor*>(interactors[0]);

	return 0;
}	// QtGeomPlaneCutPanel::getInteractor


void QtGeomPlaneCutPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _pointPanel)
		_pointPanel->stopSelection ( );
	if (0 != _normalPanel)
		_normalPanel->stopSelection ( );
	if (0 != _entitiesPanel)
		_entitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeomPlaneCutPanel::operationCompleted


void QtGeomPlaneCutPanel::preview (bool show, bool destroyInteractor)
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

	BEGIN_QT_TRY_CATCH_BLOCK

	// N'afficher une boite de dialogue d'erreur que si demandé ...
	QT_PREVIEW_BEGIN_TRY_CATCH_BLOCK

	DisplayProperties	graphicalProps;
	graphicalProps.setCloudColor (Color (
						255 * Resources::instance ( )._previewColor.getRed ( ),
						255 * Resources::instance ( )._previewColor.getGreen ( ),
						255 * Resources::instance ( )._previewColor.getBlue ( )));
	graphicalProps.setPointSize (
							Resources::instance ( )._previewPointSize.getValue ( ));
	graphicalProps.setWireColor (Color (
						255 * Resources::instance ( )._previewColor.getRed ( ),
						255 * Resources::instance ( )._previewColor.getGreen ( ),
						255 * Resources::instance ( )._previewColor.getBlue ( )));
	graphicalProps.setLineWidth (
								Resources::instance ( )._previewWidth.getValue ( ));

	// Les entités soumises à la coupe :
	const vector<string>	entities	= getCutEntities ( );
	double	xMin = DBL_MAX, xMax = -DBL_MAX, yMin = DBL_MAX,
	        yMax = -DBL_MAX, zMin = DBL_MAX, zMax = -DBL_MAX;
	for (vector<string>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		GeomEntity*	entity	= getContext ( ).getGeomManager ( ).getEntity (*it);
		CHECK_NULL_PTR_ERROR (entity)
		double		bounds [6] = { 0., 0., 0., 0., 0., 0. };
		entity->getBounds (bounds);
		xMin	= bounds [0] < xMin ? bounds [0] : xMin;
		xMax	= bounds [1] > xMax ? bounds [1] : xMax;
		yMin	= bounds [2] < yMin ? bounds [2] : yMin;
		yMax	= bounds [3] > yMax ? bounds [3] : yMax;
		zMin	= bounds [4] < zMin ? bounds [4] : zMin;
		zMax	= bounds [5] > zMax ? bounds [5] : zMax;
		DisplayProperties::GraphicalRepresentation*	gr	=
				entity->getDisplayProperties ( ).getGraphicalRepresentation ( );
		CHECK_NULL_PTR_ERROR (gr)
		RenderedEntityRepresentation*	rep	=
								dynamic_cast<RenderedEntityRepresentation*>(gr);
		if (0 != rep)
				rep->setRenderingManager (&getRenderingManager ( ));
		registerHighlightedEntity (*entity);
		gr->setHighlighted (true);
	}	// for (vector<string>::const_iterator it = entities.begin ( ); ...

	// La représentation de la coupe :
	Math::Point		p1 (getPoint ( ));
	Math::Vector	normal (getNormal ( ));
	Math::Point		p2 (p1.getX ( ) + normal.getX ( ),
					    p1.getY ( ) + normal.getY ( ),
					    p1.getZ ( ) + normal.getZ ( ));
	// Si pb de boite englobante, par ex car 0==entities.size ( ), on
	// prend la bounding box de la vue 3D.
	if ((false == NumericServices::isValidAndNotMax (xMin)) ||
	    (false == NumericServices::isValidAndNotMax (xMax)) ||
	    (false == NumericServices::isValidAndNotMax (yMin)) ||
	    (false == NumericServices::isValidAndNotMax (yMax)) ||
	    (false == NumericServices::isValidAndNotMax (zMin)) ||
	    (false == NumericServices::isValidAndNotMax (zMax)))
		getRenderingManager ( ).getBoundingBox (
											xMin, xMax, yMin, yMax, zMin, zMax);
	if ((false == NumericServices::isValidAndNotMax (xMin)) ||
	    (false == NumericServices::isValidAndNotMax (xMax)) ||
	    (false == NumericServices::isValidAndNotMax (yMin)) ||
	    (false == NumericServices::isValidAndNotMax (yMax)) ||
	    (false == NumericServices::isValidAndNotMax (zMin)) ||
	    (false == NumericServices::isValidAndNotMax (zMax)))
		{ xMin = yMin = zMin = -1.; xMax = yMax = zMax = 1.;}
	RenderingManager::PlaneInteractor*	interactor	= getInteractor ( );
	if (0 == interactor)
	{
		interactor	= getRenderingManager ( ).createPlaneInteractor (
						p1, normal, xMin, xMax, yMin, yMax, zMin, zMax, this);
		registerPreviewedInteractor (interactor);
	}
	else
	{
		interactor->setPlane (p1, normal);
	}
/*
		if (0 == _interactor)
		{
			vector<Math::Point>	points;
			points.push_back (p1);
			_previewIDs.push_back (
				renderingManager.createCloudRepresentation (
												points, graphicalProps, true));
			_previewIDs.push_back (
				renderingManager.createVector (p1, p2, graphicalProps, true));
		}	// if (0 == _interactor)
*/

	QT_PREVIEW_COMPLETE_TRY_CATCH_BLOCK

	getRenderingManager ( ).forceRender ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeomPlaneCutPanel::preview


void QtGeomPlaneCutPanel::cutModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	preview (false, false);

	RenderingManager::PlaneInteractor*	interactor	= getInteractor ( );
	if ((0 != interactor) && (true == Resources::instance ( )._prevalidateSeizure))
	{
		Math::Point		point	= getPoint ( );
		Math::Vector	normal	= getNormal ( );
		interactor->setPlane (point, normal);
	}	// if (0 != interactor)

	preview (true, true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeomPlaneCutPanel::cutModifiedCallback


// ===========================================================================
//                  LA CLASSE QtGeomPlaneCutAction
// ===========================================================================

QtGeomPlaneCutAction::QtGeomPlaneCutAction (
	const QIcon& icon, const QString& text,
	SelectionManagerIfc::DIM  dimensions, FilterEntity::objectType types,
	QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	string	entitiesLabel;

	switch (dimensions)
	{
		case	SelectionManagerIfc::D2	:
			types			= FilterEntity::GeomSurface;
			entitiesLabel	= "Surfaces :";
			break;
		case	SelectionManagerIfc::D3	:
			types			= FilterEntity::GeomVolume;
			entitiesLabel	= "Volumes :";
			break;
		default		:
			entitiesLabel	= "Entités géométriques :";
	}	// switch (dimensions)

	QtGeomPlaneCutPanel*	operationPanel	=
		new QtGeomPlaneCutPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy,
			entitiesLabel, dimensions, types, mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeomPlaneCutAction::QtGeomPlaneCutAction


QtGeomPlaneCutAction::QtGeomPlaneCutAction (const QtGeomPlaneCutAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeomPlaneCutAction copy constructor is not allowed.")
}	// QtGeomPlaneCutAction::QtGeomPlaneCutAction


QtGeomPlaneCutAction& QtGeomPlaneCutAction::operator = (const QtGeomPlaneCutAction&)
{
	MGX_FORBIDDEN ("QtGeomPlaneCutAction assignment operator is not allowed.")
	return *this;
}	// QtGeomPlaneCutAction::operator =


QtGeomPlaneCutAction::~QtGeomPlaneCutAction ( )
{
}	// QtGeomPlaneCutAction::~QtGeomPlaneCutAction


QtGeomPlaneCutPanel* QtGeomPlaneCutAction::getPlaneCutPanel ( )
{
	return dynamic_cast<QtGeomPlaneCutPanel*>(getOperationPanel ( ));
}	// QtGeomPlaneCutAction::getPlaneCutPanel


void QtGeomPlaneCutAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de la coupe :
	QtGeomPlaneCutPanel*	panel		= getPlaneCutPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	Math::Point		point (panel->getPoint ( ));
	Math::Vector	normal (panel->getNormal ( ));
	Plane			plane (point, normal);
	vector<string>	entities	= panel->getCutEntities ( );
	string			name		= panel->getGroupName ( );
	panel->clearEntitiesField ( );	// Coupe : les entités sont détruites ...
	cmdResult	= getContext ( ).getGeomManager ( ).sectionByPlane (entities, &plane, name);

	setCommandResult (cmdResult);
}	// QtGeomPlaneCutAction::executeOperation



// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
