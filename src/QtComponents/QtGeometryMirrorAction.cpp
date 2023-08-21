/**
 * \file        QtGeometryMirrorAction.cpp
 * \author		Eric B
 * \date		12/4/2016
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeometryMirrorAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"

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
using namespace Mgx3D::Geom;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtGeometryMirrorPanel
// ===========================================================================

QtGeometryMirrorPanel::QtGeometryMirrorPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).geomMirrorOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).geomMirrorOperationTag),
	  RenderingManager::InteractorObserver ( ),
	  _geomEntitiesPanel (0), _propagateCheckBox (0),
	  _pointPanel (0), _normalPanel(0),
	  _copyCheckBox(0), _namePanel(0), _withTopologyCheckBox(0)
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

//	// Faut il appliquer la symétrie à toutes les entités ?
//	_allEntitiesCheckBox	=
//			new QCheckBox ("Appliquer à toutes les entités\n(géométriques, topologiques et au maillage)", this);
//	_allEntitiesCheckBox->setChecked (false);
//	connect (_allEntitiesCheckBox, SIGNAL (stateChanged (int)), this,
//	         SLOT (allEntitiesCallback ( )));
//	layout->addWidget (_allEntitiesCheckBox);

	// Les entités à associer :
	FilterEntity::objectType	filter	=
		(FilterEntity::objectType)(FilterEntity::GeomVertex |
		FilterEntity::GeomCurve | FilterEntity::GeomSurface |
			FilterEntity::GeomVolume);
	_geomEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
							this, mainWindow, "Entités géométriques :", 
							SelectionManagerIfc::ALL_DIMENSIONS,
							filter, SelectionManagerIfc::D3, true);
	_geomEntitiesPanel->setMultiSelectMode (true);
	connect (_geomEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_geomEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_geomEntitiesPanel);

	// Faut il propager la translation aux entités géométriques ?
/* Propagate is not yet implemented ...
	_propagateCheckBox	=
			new QCheckBox ("Propager aux entités géométriques", this);
	_propagateCheckBox->setChecked (true);
	layout->addWidget (_propagateCheckBox);
*/

	// La symétrie :
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

	addPreviewCheckBox (false);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
//	CHECK_NULL_PTR_ERROR (_translationPanel->getSegmentIDTextField ( ))
	_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
			)->setLinkedSeizureManagers (
							0, _normalPanel->getSegmentIDTextField ( ));
//	_translationPanel->getSegmentIDTextField ( )->setLinkedSeizureManagers (
//			_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ), 0);

	// Faut il faire une copie des entités avant la transformation ?
	_copyCheckBox	=
			new QCheckBox ("Copier avant transformation", this);
	connect (_copyCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (copyCallback ( )));
	_copyCheckBox->setChecked (false);
	layout->addWidget (_copyCheckBox);

	// Nom groupe (en cas de copie):
	_namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe", mainWindow, 3,
			QtMgx3DGroupNamePanel::CREATION, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// Faut il propager la copie aux entités topologiques associées ?
	_withTopologyCheckBox	=
			new QCheckBox (QString::fromUtf8("Copier également la topologie associée"), this);
	_withTopologyCheckBox->setChecked (false);
	layout->addWidget (_withTopologyCheckBox);

	_namePanel->setEnabled (false);
	_withTopologyCheckBox->setEnabled (false);

}	// QtGeometryMirrorPanel::QtGeometryMirrorPanel


QtGeometryMirrorPanel::QtGeometryMirrorPanel (
										const QtGeometryMirrorPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _geomEntitiesPanel (0), _propagateCheckBox (0),
	  _pointPanel (0), _normalPanel(0),
	  _copyCheckBox(0), _namePanel(0), _withTopologyCheckBox(0)
{
	MGX_FORBIDDEN ("QtGeometryMirrorPanel copy constructor is not allowed.");
}	// QtGeometryMirrorPanel::QtGeometryMirrorPanel (const QtGeometryMirrorPanel&)


QtGeometryMirrorPanel& QtGeometryMirrorPanel::operator = (
											const QtGeometryMirrorPanel&)
{
	MGX_FORBIDDEN ("QtGeometryMirrorPanel assignment operator is not allowed.");
	return *this;
}	// QtGeometryMirrorPanel::QtGeometryMirrorPanel (const QtGeometryMirrorPanel&)


QtGeometryMirrorPanel::~QtGeometryMirrorPanel ( )
{
	preview (false, true);
}	// QtGeometryMirrorPanel::~QtGeometryMirrorPanel


void QtGeometryMirrorPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel)
	_namePanel->autoUpdate ( );
	_geomEntitiesPanel->reset ( );
	_pointPanel->reset ( );
	_normalPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeometryMirrorPanel::reset


void QtGeometryMirrorPanel::setDimension (SelectionManagerIfc::DIM dim)
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	_geomEntitiesPanel->clearSelection ( );
	_geomEntitiesPanel->setDimensions (dim);
}	// QtGeometryMirrorPanel::setDimension


//bool QtGeometryMirrorPanel::processAllGeomEntities ( ) const
//{
//	CHECK_NULL_PTR_ERROR (_allEntitiesCheckBox)
//	return _allEntitiesCheckBox->isChecked ( );
//}	// QtGeometryMirrorPanel::processAllGeomEntities

bool QtGeometryMirrorPanel::copyEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_copyCheckBox)
	return _copyCheckBox->isChecked ( );
}	// QtGeometryMirrorPanel::copyEntities

string QtGeometryMirrorPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtGeometryMirrorPanel::getGroupName

bool QtGeometryMirrorPanel::copyTopoEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)
	return _withTopologyCheckBox->isChecked ( );
}	// QtGeometryMirrorPanel::copyTopoEntities


vector<string> QtGeometryMirrorPanel::getGeomEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	return _geomEntitiesPanel->getEntitiesNames ( );
}	// QtGeometryMirrorPanel::getGeomEntitiesNames


Math::Point QtGeometryMirrorPanel::getPoint ( ) const
{
	CHECK_NULL_PTR_ERROR (_pointPanel);

	return _pointPanel->getPoint();
}	// QtGeometryMirrorPanel::getPoint

Math::Vector QtGeometryMirrorPanel::getNormal ( ) const
{
	CHECK_NULL_PTR_ERROR (_normalPanel);
	double	dx	= 0., dy	= 0., dz	= 0.;
	dx	= _normalPanel->getDx ( );
	dy	= _normalPanel->getDy ( );
	dz	= _normalPanel->getDz ( );

	return Math::Vector (dx, dy, dz);
}	// QtGeometryMirrorPanel::getNormal

bool QtGeometryMirrorPanel::doPropagate ( ) const
{
return false;
// Not yet implemented :
//	CHECK_NULL_PTR_ERROR (_propagateCheckBox)
//	return Qt::Checked == _propagateCheckBox->checkState ( ) ? true : false;
}	// QtGeometryMirrorPanel::doPropagate


void QtGeometryMirrorPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel)
	_geomEntitiesPanel->stopSelection ( );
	_pointPanel->stopSelection ( );
	_normalPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_pointPanel->setUniqueName ("");
		_geomEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeometryMirrorPanel::cancel


void QtGeometryMirrorPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedGeomEntities	=
					getSelectionManager ( ).getEntitiesNames (
									_geomEntitiesPanel->getAllowedTypes ( ));
		if (0 != selectedGeomEntities.size ( ))
			_geomEntitiesPanel->setEntitiesNames (selectedGeomEntities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_geomEntitiesPanel->clearSelection ( );
	_pointPanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationPanel::autoUpdate ( );

	_namePanel->autoUpdate ( );
	_geomEntitiesPanel->actualizeGui (true);
	_pointPanel->actualizeGui (true);
	_normalPanel->actualizeGui (true);
}	// QtGeometryMirrorPanel::autoUpdate


vector<Entity*> QtGeometryMirrorPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	names	= getGeomEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*	entity	=
					getContext ( ).getGeomManager ( ).getEntity (*it, false);
		if (0 != entity)
			entities.push_back (entity);
	}

	return entities;
}	// QtGeometryMirrorPanel::getInvolvedEntities


void QtGeometryMirrorPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _geomEntitiesPanel)
		_geomEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeometryMirrorPanel::operationCompleted


void QtGeometryMirrorPanel::cutModifiedCallback ( )
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
}	// QtGeometryMirrorPanel::cutModifiedCallback

RenderingManager::PlaneInteractor* QtGeometryMirrorPanel::getInteractor ( )
{
	vector <RenderingManager::Interactor*>	interactors	=
												getPreviewedInteractors ( );
	if (0 != interactors.size ( ))
		return dynamic_cast<RenderingManager::PlaneInteractor*>(interactors[0]);

	return 0;
}	// QtGeometryMirrorPanel::getInteractor

void QtGeometryMirrorPanel::preview (bool show, bool destroyInteractor)
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
//		GeomEntity*	entity	= dynamic_cast<GeomEntity*>(*it);
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

}	// QtGeometryMirrorPanel::preview


//void QtGeometryMirrorPanel::allEntitiesCallback ( )
//{
//	BEGIN_QT_TRY_CATCH_BLOCK
//
//	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
//	_geomEntitiesPanel->setEnabled (!processAllGeomEntities ( ));
//
//	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
//}	// QtGeometryMirrorPanel::allEntitiesCallback

void QtGeometryMirrorPanel::copyCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)

	_namePanel->setEnabled (copyEntities ( ));
	_withTopologyCheckBox->setEnabled (copyEntities ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeometryMirrorPanel::copyCallback



// ===========================================================================
//                  LA CLASSE QtGeometryMirrorAction
// ===========================================================================

QtGeometryMirrorAction::QtGeometryMirrorAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeometryMirrorPanel*	operationPanel	=
		new QtGeometryMirrorPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeometryMirrorAction::QtGeometryMirrorAction


QtGeometryMirrorAction::QtGeometryMirrorAction (
										const QtGeometryMirrorAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeometryMirrorAction copy constructor is not allowed.")
}	// QtGeometryMirrorAction::QtGeometryMirrorAction


QtGeometryMirrorAction& QtGeometryMirrorAction::operator = (
										const QtGeometryMirrorAction&)
{
	MGX_FORBIDDEN ("QtGeometryMirrorAction assignment operator is not allowed.")
	return *this;
}	// QtGeometryMirrorAction::operator =


QtGeometryMirrorAction::~QtGeometryMirrorAction ( )
{
}	// QtGeometryMirrorAction::~QtGeometryMirrorAction


QtGeometryMirrorPanel*
					QtGeometryMirrorAction::getMirrorPanel ( )
{
	return dynamic_cast<QtGeometryMirrorPanel*>(getOperationPanel ( ));
}	// QtGeometryMirrorAction::getMirrorPanel


void QtGeometryMirrorAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités géométriques :
	QtGeometryMirrorPanel*	panel	= getMirrorPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	Math::Point		point (panel->getPoint ( ));
	Math::Vector	normal (panel->getNormal ( ));
	Plane			plane (point, normal);
	//bool					propagate	= panel->doPropagate ( );
	const bool			    copy        = panel->copyEntities();
	const string	      	groupName	= panel->getGroupName ( );
	const bool			    withTopo	= panel->copyTopoEntities ( );

	vector<string>			entities	= panel->getGeomEntitiesNames ( );
	if (copy)
		cmdResult	= getContext ( ).getGeomManager ( ).copyAndMirror (entities, &plane, withTopo, groupName);
	else
		cmdResult	= getContext ( ).getGeomManager ( ).mirror (entities, &plane);

	setCommandResult (cmdResult);
}	// QtGeometryMirrorAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
