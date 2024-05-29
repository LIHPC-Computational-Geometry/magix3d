/**
 * \file        QtTopologyMirrorAction.cpp
 * \author		Eric B
 * \date		13/4/2016
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Internal/Resources.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtTopologyMirrorAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>
#include <TkUtil/NumericServices.h>

#include <QLabel>
#include <QBoxLayout>

#include <values.h>	// DBL_MAX


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
//                        LA CLASSE QtTopologyMirrorPanel
// ===========================================================================

QtTopologyMirrorPanel::QtTopologyMirrorPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).topoMirrorOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).topoMirrorOperationTag),
	  RenderingManager::InteractorObserver ( ),
	  _topoEntitiesPanel (0), _propagateCheckBox (0),
	  _pointPanel (0), _normalPanel(0)
{
//	SET_WIDGET_BACKGROUND (this, Qt::yellow)
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins (0, 0, 0, 0);

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Les entités à associer :
	FilterEntity::objectType	filter	=
		(FilterEntity::objectType)(FilterEntity::TopoVertex |
		FilterEntity::TopoCoEdge | FilterEntity::TopoCoFace |
			FilterEntity::TopoBlock);
	_topoEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
							this, mainWindow, "Entités topologiques :",
							SelectionManagerIfc::ALL_DIMENSIONS,
							filter, SelectionManagerIfc::D3, true);
	_topoEntitiesPanel->setMultiSelectMode (true);
	connect (_topoEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_topoEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_topoEntitiesPanel);

	// Faut il propager la symétrie aux entités géométriques ?
	_propagateCheckBox	=
			new QCheckBox (QString::fromUtf8("Propager aux entités géométriques"), this);
	_propagateCheckBox->setChecked (true);
	layout->addWidget (_propagateCheckBox);

	// La symétrie :
	// "Centre" du plan :
	_pointPanel	= new QtMgx3DPointPanel (
		this, "Point ", true, "x :", "y :", "z :",
		0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
		&mainWindow, FilterEntity::TopoVertex, true);
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

	addPreviewCheckBox (false);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
			)->setLinkedSeizureManagers (
							0, _normalPanel->getSegmentIDTextField ( ));
}	// QtTopologyMirrorPanel::QtTopologyMirrorPanel


QtTopologyMirrorPanel::QtTopologyMirrorPanel (
										const QtTopologyMirrorPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0,0), 0, "", ""),
	  _topoEntitiesPanel (0), _propagateCheckBox (0),
	  _pointPanel (0), _normalPanel(0)
{
	MGX_FORBIDDEN ("QtTopologyMirrorPanel copy constructor is not allowed.");
}	// QtTopologyMirrorPanel::QtTopologyMirrorPanel (const QtTopologyMirrorPanel&)


QtTopologyMirrorPanel& QtTopologyMirrorPanel::operator = (
											const QtTopologyMirrorPanel&)
{
	MGX_FORBIDDEN ("QtTopologyMirrorPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyMirrorPanel::QtTopologyMirrorPanel (const QtTopologyMirrorPanel&)


QtTopologyMirrorPanel::~QtTopologyMirrorPanel ( )
{
	preview (false, true);
}	// QtTopologyMirrorPanel::~QtTopologyMirrorPanel


void QtTopologyMirrorPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel)
	_topoEntitiesPanel->reset ( );
	_pointPanel->reset ( );
	_normalPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyMirrorPanel::reset


void QtTopologyMirrorPanel::setDimension (SelectionManagerIfc::DIM dim)
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	_topoEntitiesPanel->clearSelection ( );
	_topoEntitiesPanel->setDimensions (dim);
}	// QtTopologyMirrorPanel::setDimension


vector<string> QtTopologyMirrorPanel::getTopoEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	return _topoEntitiesPanel->getEntitiesNames ( );
}	// QtTopologyMirrorPanel::getTopoEntitiesNames


Math::Point QtTopologyMirrorPanel::getPoint ( ) const
{
	CHECK_NULL_PTR_ERROR (_pointPanel);

	return _pointPanel->getPoint();
}	// QtTopologyMirrorPanel::getPoint

Math::Vector QtTopologyMirrorPanel::getNormal ( ) const
{
	CHECK_NULL_PTR_ERROR (_normalPanel);
	double	dx	= 0., dy	= 0., dz	= 0.;
	dx	= _normalPanel->getDx ( );
	dy	= _normalPanel->getDy ( );
	dz	= _normalPanel->getDz ( );

	return Math::Vector (dx, dy, dz);
}	// QtTopologyMirrorPanel::getNormal


bool QtTopologyMirrorPanel::doPropagate ( ) const
{
	CHECK_NULL_PTR_ERROR (_propagateCheckBox)
	return Qt::Checked == _propagateCheckBox->checkState ( ) ? true : false;
}	// QtTopologyMirrorPanel::doPropagate



void QtTopologyMirrorPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel)
	_topoEntitiesPanel->stopSelection ( );
	_pointPanel->stopSelection ( );
	_normalPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_pointPanel->setUniqueName ("");
		_topoEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyMirrorPanel::cancel


void QtTopologyMirrorPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedTopoEntities	=
					getSelectionManager ( ).getEntitiesNames (
									_topoEntitiesPanel->getAllowedTypes ( ));
		if (0 != selectedTopoEntities.size ( ))
			_topoEntitiesPanel->setEntitiesNames (selectedTopoEntities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_topoEntitiesPanel->clearSelection ( );
	_pointPanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationPanel::autoUpdate ( );

	_topoEntitiesPanel->actualizeGui (true);
	_pointPanel->actualizeGui (true);
	_normalPanel->actualizeGui (true);
}	// QtTopologyMirrorPanel::autoUpdate


vector<Entity*> QtTopologyMirrorPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	names	= getTopoEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*	entity	=
					getContext ( ).getTopoManager ( ).getEntity (*it, false);
		if (0 != entity)
			entities.push_back (entity);
	}

	return entities;
}	// QtTopologyMirrorPanel::getInvolvedEntities


void QtTopologyMirrorPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _topoEntitiesPanel)
		_topoEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyMirrorPanel::operationCompleted


void QtTopologyMirrorPanel::cutModifiedCallback ( )
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
}	// QtTopologyMirrorPanel::cutModifiedCallback

RenderingManager::PlaneInteractor* QtTopologyMirrorPanel::getInteractor ( )
{
	vector <RenderingManager::Interactor*>	interactors	=
												getPreviewedInteractors ( );
	if (0 != interactors.size ( ))
		return dynamic_cast<RenderingManager::PlaneInteractor*>(interactors[0]);

	return 0;
}	// QtTopologyMirrorPanel::getInteractor

void QtTopologyMirrorPanel::preview (bool show, bool destroyInteractor)
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
	double	xMin = DBL_MAX, xMax = -DBL_MAX, yMin = DBL_MAX,
	        yMax = -DBL_MAX, zMin = DBL_MAX, zMax = -DBL_MAX;

	std::vector<Mgx3D::Utils::Entity*> entities = getInvolvedEntities ( );

	for (std::vector<Mgx3D::Utils::Entity*>::iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		Entity*	entity	= (*it);
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

	QT_PREVIEW_COMPLETE_TRY_CATCH_BLOCK

	getRenderingManager ( ).forceRender ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

}	// QtTopologyMirrorPanel::preview


// ===========================================================================
//                  LA CLASSE QtTopologyMirrorAction
// ===========================================================================

QtTopologyMirrorAction::QtTopologyMirrorAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyMirrorPanel*	operationPanel	=
		new QtTopologyMirrorPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyMirrorAction::QtTopologyMirrorAction


QtTopologyMirrorAction::QtTopologyMirrorAction (
										const QtTopologyMirrorAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyMirrorAction copy constructor is not allowed.")
}	// QtTopologyMirrorAction::QtTopologyMirrorAction


QtTopologyMirrorAction& QtTopologyMirrorAction::operator = (
										const QtTopologyMirrorAction&)
{
	MGX_FORBIDDEN ("QtTopologyMirrorAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyMirrorAction::operator =


QtTopologyMirrorAction::~QtTopologyMirrorAction ( )
{
}	// QtTopologyMirrorAction::~QtTopologyMirrorAction


QtTopologyMirrorPanel*
					QtTopologyMirrorAction::getMirrorPanel ( )
{
	return dynamic_cast<QtTopologyMirrorPanel*>(getOperationPanel ( ));
}	// QtTopologyMirrorAction::getMirrorPanel


void QtTopologyMirrorAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités topologiques :
	QtTopologyMirrorPanel*	panel	= getMirrorPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	Math::Point		point (panel->getPoint ( ));
	Math::Vector	normal (panel->getNormal ( ));
	Plane			plane (point, normal);
	bool			propagate	= panel->doPropagate ( );

	vector<string>			entities	= panel->getTopoEntitiesNames ( );
	cmdResult	= getContext ( ).getTopoManager ( ).mirror (entities, &plane, propagate);

	setCommandResult (cmdResult);
}	// QtTopologyMirrorAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
