/**
 * \file        QtTopoEntitiesAssociationsAction.cpp
 * \author      Charles PIGNEROL
 * \date        20/11/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtTopoEntitiesAssociationsAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "Topo/CommandSetGeomAssociation.h"
#include "Topo/CommandProjectVerticesOnNearestGeomEntities.h"
#include "Topo/CommandProjectEdgesOnCurves.h"
#include "Topo/CommandProjectFacesOnSurfaces.h"
#include "Topo/TopoDisplayRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                       LA CLASSE QtTopoToGeomAssociationPanel
// ===========================================================================

QtTopoToGeomAssociationPanel::QtTopoToGeomAssociationPanel (
			QWidget* parent,
			QtTopoToGeomAssociationPanel::PROJECTION_METHOD method,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::Utils::SelectionManagerIfc::DIM defaultDimensions)
	: QtMgx3DOperationsSubPanel (parent, mainWindow),
	  _projectionMethod (method), _topoEntitiesPanel (0),
	  _geomEntitiesPanel (0), _projectAllCheckBox (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins (0, 0, 0, 0);

	// Les entités à associer :
	SelectionManagerIfc::DIM	allowedDimensions	= SelectionManagerIfc::D0;
	//SelectionManagerIfc::DIM	defaultDimension	= SelectionManagerIfc::D2;
	SelectionManagerIfc::DIM	defaultDimension = defaultDimensions;
	FilterEntity::objectType	filter				= FilterEntity::NoneEntity;
	bool						allowDifferentDims	= false;
	bool						multiMode			= true;
	string						name ("Entités topologiques :");
	string						projectAllLabel;
	switch (_projectionMethod)
	{
		case QtTopoToGeomAssociationPanel::TOPO_ENTITIES_GEOM_ENTITY	:
			allowedDimensions	=
				(SelectionManagerIfc::DIM)(SelectionManagerIfc::D0 |
							SelectionManagerIfc::D1 | SelectionManagerIfc::D2);
			filter	= (FilterEntity::objectType)(FilterEntity::TopoVertex |
						  FilterEntity::TopoCoEdge | FilterEntity::TopoCoFace);	
			allowDifferentDims	= true;
			break;
		case QtTopoToGeomAssociationPanel::VERTICES_GEOM_ENTITIES		:
			name	= "Sommets :";
			allowedDimensions	= SelectionManagerIfc::D0;
			defaultDimension	= SelectionManagerIfc::D0;
			filter				= FilterEntity::TopoVertex;
			break;
		case QtTopoToGeomAssociationPanel::EDGES_CURVES					:
			name	= "Arêtes :";
			allowedDimensions	= SelectionManagerIfc::D1;
			defaultDimension	= SelectionManagerIfc::D1;
			filter				= FilterEntity::TopoCoEdge;
			projectAllLabel		= "Toutes les arêtes";
			break;
		case QtTopoToGeomAssociationPanel::FACES_SURFACES				:
			name	= "Faces :";
			allowedDimensions	= SelectionManagerIfc::D2;
			defaultDimension	= SelectionManagerIfc::D2;
			filter				= FilterEntity::TopoCoFace;
			projectAllLabel		= "Toutes les faces";
			break;
	}	// switch (_projectionMethod)
	_topoEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
							this, mainWindow, name,
							allowedDimensions, filter, defaultDimension,
							allowDifferentDims);
	connect (_topoEntitiesPanel, SIGNAL (dimensionsModified ( )),
	         this, SLOT (dimensionsModifiedCallback ( )));
	_topoEntitiesPanel->setMultiSelectMode (multiMode);
	connect (_topoEntitiesPanel, SIGNAL (dimensionsModified ( )),
	         this, SLOT (dimensionsModifiedCallback ( )));
	connect (_topoEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesSelectedCallback (QString)));
	connect (_topoEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesUnselectedCallback (QString)));
	layout->addWidget (_topoEntitiesPanel);

	// Les entités géométriques à associer :
	name	= "Entités géométriques :";
	allowDifferentDims	= false;
//	defaultDimension	= SelectionManagerIfc::D2;
	defaultDimension = defaultDimensions;
	allowedDimensions	=
				(SelectionManagerIfc::DIM)(SelectionManagerIfc::D0 |
							SelectionManagerIfc::D1 | SelectionManagerIfc::D2);
	filter	= (FilterEntity::objectType)(FilterEntity::GeomVertex |
						  FilterEntity::GeomCurve | FilterEntity::GeomSurface);	
	multiMode			= true;
	switch (_projectionMethod)
	{
		case QtTopoToGeomAssociationPanel::TOPO_ENTITIES_GEOM_ENTITY	:
			name	= "Entité géométrique :";
			multiMode			= false;
			allowDifferentDims	= true;
			break;
		case QtTopoToGeomAssociationPanel::VERTICES_GEOM_ENTITIES		:
			allowDifferentDims	= true;
			break;
/*		case QtTopoToGeomAssociationPanel::EDGES_CURVES					:
			name				= "Courbes :";
			allowedDimensions	= SelectionManagerIfc::D1;
			defaultDimension	= SelectionManagerIfc::D1;
			filter				= FilterEntity::GeomCurve;
			break;
		case QtTopoToGeomAssociationPanel::FACES_SURFACES				:
			name				= "Surfaces :";
			allowedDimensions	= SelectionManagerIfc::D2;
			defaultDimension	= SelectionManagerIfc::D2;
			filter				= FilterEntity::GeomSurface;
			break;*/
	}	// switch (_projectionMethod)
	if ((QtTopoToGeomAssociationPanel::TOPO_ENTITIES_GEOM_ENTITY
													== _projectionMethod) ||
	    (QtTopoToGeomAssociationPanel::VERTICES_GEOM_ENTITIES
													== _projectionMethod))
	{
		_geomEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
							this, mainWindow, name,
							allowedDimensions, filter, defaultDimension,
							allowDifferentDims);
		_geomEntitiesPanel->setMultiSelectMode (multiMode);
		connect (_geomEntitiesPanel, SIGNAL (dimensionsModified ( )),
	         this, SLOT (dimensionsModifiedCallback ( )));
		connect (_geomEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesSelectedCallback (QString)));
		connect (_geomEntitiesPanel,
			SIGNAL (entitiesRemovedFromSelection(QString)),
	        this, SLOT (entitiesUnselectedCallback (QString)));
		layout->addWidget (_geomEntitiesPanel);
	}	// if ((QtTopoToGeomAssociationPanel::TOPO_ENTITIES_GEOM_ENTITY == ...

	if (false == projectAllLabel.empty ( ))
	{
		_projectAllCheckBox	= new QCheckBox (projectAllLabel.c_str ( ), this);
		connect (_projectAllCheckBox, SIGNAL (stateChanged (int)), this,
		         SLOT (projectAllTopoEntitiesModifiedCallback ( )));
		layout->addWidget (_projectAllCheckBox);
	}	// if (false == projectAllLabel.empty ( ))

	// La sélection interactive :
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	if (0 != _geomEntitiesPanel)
	{
		CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( ))
		CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
			)->setLinkedSeizureManagers (
				_topoEntitiesPanel->getEntitiesPanel( )->getNameTextField( ),0);
	}	// if (0 != _geomEntitiesPanel)
	_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
			)->setLinkedSeizureManagers (0, 0 == _geomEntitiesPanel ?
			0 : _geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField( ));

	layout->addStretch (2);

	projectAllTopoEntitiesModifiedCallback ( );
}	// QtTopoToGeomAssociationPanel::QtTopoToGeomAssociationPanel


QtTopoToGeomAssociationPanel::QtTopoToGeomAssociationPanel (
										const QtTopoToGeomAssociationPanel& p)
	: QtMgx3DOperationsSubPanel (p),
	  _projectionMethod (
			QtTopoToGeomAssociationPanel::TOPO_ENTITIES_GEOM_ENTITY),
	  _topoEntitiesPanel (0), _geomEntitiesPanel (0), _projectAllCheckBox (0)
{
	MGX_FORBIDDEN ("QtTopoToGeomAssociationPanel copy constructor is not allowed.");
}	// QtTopoToGeomAssociationPanel::QtTopoToGeomAssociationPanel


QtTopoToGeomAssociationPanel& QtTopoToGeomAssociationPanel::operator = (
										const QtTopoToGeomAssociationPanel& p)
{
	MGX_FORBIDDEN ("QtTopoToGeomAssociationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopoToGeomAssociationPanel::operator =


QtTopoToGeomAssociationPanel::~QtTopoToGeomAssociationPanel ( )
{
}	// QtTopoToGeomAssociationPanel::~QtTopoToGeomAssociationPanel


void QtTopoToGeomAssociationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _topoEntitiesPanel)
		_topoEntitiesPanel->reset ( );
	if (0 != _geomEntitiesPanel)
		_geomEntitiesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationsSubPanel::reset ( );
}	// QtTopoToGeomAssociationPanel::reset


void QtTopoToGeomAssociationPanel::cancel ( )
{
	QtMgx3DOperationsSubPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	_topoEntitiesPanel->stopSelection ( );
	if (0 != _geomEntitiesPanel)
		_geomEntitiesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_topoEntitiesPanel->clearSelection ( );
		if (0 != _geomEntitiesPanel)
			_geomEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopoToGeomAssociationPanel::cancel


void QtTopoToGeomAssociationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK
		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_topoEntitiesPanel->stopSelection ( );
	_topoEntitiesPanel->clearSelection ( );
	if (0 != _geomEntitiesPanel){
		_geomEntitiesPanel->clearSelection ( );
		_geomEntitiesPanel->stopSelection ( );
	}
#endif	// AUTO_UPDATE_OLD_SCHEME

	_topoEntitiesPanel->actualizeGui (true);
	if (0 != _geomEntitiesPanel)
		_geomEntitiesPanel->actualizeGui (true);

	QtMgx3DOperationsSubPanel::autoUpdate ( );
}	// QtTopoToGeomAssociationPanel::autoUpdate


void QtTopoToGeomAssociationPanel::stopSelection ()
{
	_topoEntitiesPanel->stopSelection();
	if (0 != _geomEntitiesPanel)
			_geomEntitiesPanel->stopSelection();
} // QtTopoToGeomAssociationPanel::stopSelection ()

void QtTopoToGeomAssociationPanel::clearSelection ( )
{
	_topoEntitiesPanel->clearSelection();
	if (0 != _geomEntitiesPanel)
		_geomEntitiesPanel->clearSelection();
}

QtTopoToGeomAssociationPanel::PROJECTION_METHOD
					QtTopoToGeomAssociationPanel::getProjectionMethod ( ) const
{
	return _projectionMethod;
}	// QtTopoToGeomAssociationPanel::getProjectionMethod


vector<string> QtTopoToGeomAssociationPanel::getTopoEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	return _topoEntitiesPanel->getEntitiesNames ( );
}	// QtTopoToGeomAssociationPanel::getTopoEntitiesNames


vector<string> QtTopoToGeomAssociationPanel::getGeomEntitiesNames ( ) const
{
	vector<string>	names;

	if (0 != _geomEntitiesPanel)
		names	= _geomEntitiesPanel->getEntitiesNames ( );

	return names;
}	// QtTopoToGeomAssociationPanel::getGeomEntitiesNames


bool QtTopoToGeomAssociationPanel::projectAllTopoEntities ( ) const
{
	if (0 == _projectAllCheckBox)
		return false;

	return _projectAllCheckBox->isChecked ( );
}	// QtTopoToGeomAssociationPanel::projectAllTopoEntities


vector<Entity*> QtTopoToGeomAssociationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	topoNames	= getTopoEntitiesNames ( );
	const vector<string>	geomNames	= getGeomEntitiesNames ( );

	for (vector<string>::const_iterator itt = topoNames.begin ( );
		 topoNames.end ( ) != itt; itt++)
	{
		TopoEntity*	entity	= getContext ( ).getTopoManager ( ).getEntity(*itt);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}	// for (vector<string>::const_iterator itt = topoNames.begin ( ); ...
	for (vector<string>::const_iterator itg = geomNames.begin ( );
		 geomNames.end ( ) != itg; itg++)
	{
		GeomEntity*	entity	= getContext ( ).getGeomManager ( ).getEntity(*itg);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}	// for (vector<string>::const_iterator itg = geomNames.begin ( ); ...

	return entities;
}	// QtTopoToGeomAssociationPanel::getInvolvedEntities


void QtTopoToGeomAssociationPanel::dimensionsModifiedCallback ( )
{
	if (0 == _geomEntitiesPanel)
		return;

	// Lors de la construction getGraphicalWidget peut être nul ...
	try
	{
		CHECK_NULL_PTR_ERROR (getMainWindow ( ))
		getMainWindow ( )->getGraphicalWidget ( ).getRenderingManager ( );
	}
	catch (...)
	{
		return;
	}

	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)

	const SelectionManagerIfc::DIM	topoDims	=
										_topoEntitiesPanel->getDimensions ( );
	SelectionManagerIfc::DIM	geomDim	= SelectionManagerIfc::ALL_DIMENSIONS;
	// On enlève les dimensions interdites : l'association se fait vers une
	// entité géométrique de dimension au moins égale et au plus 2 :
	geomDim	= (SelectionManagerIfc::DIM)(geomDim & SelectionManagerIfc::NOT_D3);
	if (0 == (topoDims & SelectionManagerIfc::D0))
	{
		geomDim	= (SelectionManagerIfc::DIM)(
									geomDim & SelectionManagerIfc::NOT_D0);
		if (0 == (topoDims & SelectionManagerIfc::D1))
		{
			geomDim	= (SelectionManagerIfc::DIM)(
									geomDim & SelectionManagerIfc::NOT_D1);
			if (0 == (topoDims & SelectionManagerIfc::D2))
				geomDim	= (SelectionManagerIfc::DIM)(
									geomDim & SelectionManagerIfc::NOT_D2);
		}	// if (0 == (topoDims & SelectionManagerIfc::D1))
	}	// if (0 == (topoDims & SelectionManagerIfc::D0))
	_geomEntitiesPanel->setAllowedDimensions (geomDim);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopoToGeomAssociationPanel::dimensionsModifiedCallback


void QtTopoToGeomAssociationPanel::projectAllTopoEntitiesModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)

	_topoEntitiesPanel->setEnabled (!projectAllTopoEntities ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopoToGeomAssociationPanel::projectAllTopoEntitiesModifiedCallback


// ===========================================================================
//                        LA CLASSE QtTopoEntitiesAssociationsPanel
// ===========================================================================

QtTopoEntitiesAssociationsPanel::QtTopoEntitiesAssociationsPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			Mgx3D::Utils::SelectionManagerIfc::DIM defaultDimensions)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).associateOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).associateOperationTag),
	  _projectionComboBox (0),
	  _currentPanelParentWidget (0), _currentPanel (0),
	  _topoToGeomEntityPanel (0), _verticesToGeomEntitiesPanel (0),
	  _edgesToCurvesPanel (0), _facesToSurfacesPanel (0),
	  _moveVerticiesCheckBox (0)
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

	// La méthode utilisée :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Méthode :"), this);
	hlayout->addWidget (label);
	_projectionComboBox	= new QComboBox (this);
	hlayout->addWidget (_projectionComboBox);
	_projectionComboBox->addItem (QString::fromUtf8("E. topologiques -> E. géométrique"));
	_projectionComboBox->addItem (QString::fromUtf8("Sommets -> E. géométriques"));
	_projectionComboBox->addItem (QString::fromUtf8("Arêtes -> Courbes"));
	_projectionComboBox->addItem (QString::fromUtf8("Faces -> Surfaces"));
	connect (_projectionComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (projectionMethodCallback ( )));
	hlayout->addStretch (2);

	// Les différents panneaux de projection.
	_currentPanelParentWidget	= new QWidget (this);
	layout->addWidget (_currentPanelParentWidget);
	QHBoxLayout*	currentLayout	=
							new QHBoxLayout (_currentPanelParentWidget);
	_currentPanelParentWidget->setLayout (currentLayout);
	_topoToGeomEntityPanel	=
		new QtTopoToGeomAssociationPanel (0,
			QtTopoToGeomAssociationPanel::TOPO_ENTITIES_GEOM_ENTITY,
			mainWindow,
			defaultDimensions);
	_topoToGeomEntityPanel->hide ( );
	connect (_topoToGeomEntityPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_topoToGeomEntityPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	_verticesToGeomEntitiesPanel	=
		new QtTopoToGeomAssociationPanel (0,
			QtTopoToGeomAssociationPanel::VERTICES_GEOM_ENTITIES,
			mainWindow,
			defaultDimensions);
	_verticesToGeomEntitiesPanel->hide ( );
	connect (_verticesToGeomEntitiesPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_verticesToGeomEntitiesPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	_edgesToCurvesPanel	=
		new QtTopoToGeomAssociationPanel (0,
			QtTopoToGeomAssociationPanel::EDGES_CURVES,
			mainWindow,
			defaultDimensions);
	_edgesToCurvesPanel->hide ( );
	connect (_edgesToCurvesPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edgesToCurvesPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	_facesToSurfacesPanel	=
		new QtTopoToGeomAssociationPanel (0,
			QtTopoToGeomAssociationPanel::FACES_SURFACES,
			mainWindow,
			defaultDimensions);
	_facesToSurfacesPanel->hide ( );
	connect (_facesToSurfacesPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_facesToSurfacesPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// Faut il déplacer les sommets lors du changement d'association ?
	hlayout	= new QHBoxLayout ( );
	hlayout->setContentsMargins (10, 0, 0, 0);
	layout->addLayout (hlayout);
	_moveVerticiesCheckBox	= new QCheckBox (QString::fromUtf8("Déplacer les sommets"), this);
	_moveVerticiesCheckBox->setCheckState (Qt::Checked);
	hlayout->addWidget (_moveVerticiesCheckBox);
	_moveVerticiesCheckBox->setToolTip (QString::fromUtf8("Coché les sommets topologiques sont déplacés lors de la modification d'association."));
	hlayout->addStretch (5);

	layout->addStretch (2);

	addPreviewCheckBox (false);

	projectionMethodCallback ( );
}	// QtTopoEntitiesAssociationsPanel::QtTopoEntitiesAssociationsPanel


QtTopoEntitiesAssociationsPanel::QtTopoEntitiesAssociationsPanel (
										const QtTopoEntitiesAssociationsPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _projectionComboBox (0),
	  _currentPanelParentWidget (0), _currentPanel (0),
	  _topoToGeomEntityPanel (0), _verticesToGeomEntitiesPanel (0),
	  _edgesToCurvesPanel (0), _facesToSurfacesPanel (0),
	  _moveVerticiesCheckBox (0)
{
	MGX_FORBIDDEN ("QtTopoEntitiesAssociationsPanel copy constructor is not allowed.");
}	// QtTopoEntitiesAssociationsPanel::QtTopoEntitiesAssociationsPanel (const QtTopoEntitiesAssociationsPanel&)


QtTopoEntitiesAssociationsPanel& QtTopoEntitiesAssociationsPanel::operator = (
											const QtTopoEntitiesAssociationsPanel&)
{
	MGX_FORBIDDEN ("QtTopoEntitiesAssociationsPanel assignment operator is not allowed.");
	return *this;
}	// QtTopoEntitiesAssociationsPanel::QtTopoEntitiesAssociationsPanel (const QtTopoEntitiesAssociationsPanel&)


QtTopoEntitiesAssociationsPanel::~QtTopoEntitiesAssociationsPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtTopoEntitiesAssociationsPanel::~QtTopoEntitiesAssociationsPanel


QtTopoToGeomAssociationPanel::PROJECTION_METHOD
					QtTopoEntitiesAssociationsPanel::getProjectionMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_projectionComboBox)
	return (QtTopoToGeomAssociationPanel::PROJECTION_METHOD)_projectionComboBox->currentIndex ( );
}	// QtTopoEntitiesAssociationsPanel::getProjectionMethod


vector<string> QtTopoEntitiesAssociationsPanel::getTopoEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_currentPanel)
	return _currentPanel->getTopoEntitiesNames ( );
}	// QtTopoEntitiesAssociationsPanel::getTopoEntitiesNames


vector<TopoEntity*> QtTopoEntitiesAssociationsPanel::getTopoEntities ( ) const
{
	vector<TopoEntity*>		entities;
	const vector<string>	names	= getTopoEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*	te	= getContext( ).getTopoManager( ).getEntity (*it, true);
		CHECK_NULL_PTR_ERROR (te)
		entities.push_back (te);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtTopoEntitiesAssociationsPanel::getTopoEntities


vector<Topo::Vertex*> QtTopoEntitiesAssociationsPanel::getVertices ( ) const
{
	vector<Topo::Vertex*>		entities;
	const vector<string>	names	= getTopoEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		Topo::Vertex*	te	=
				getContext( ).getTopoManager( ).getVertex (*it, true);
		CHECK_NULL_PTR_ERROR (te)
		entities.push_back (te);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtTopoEntitiesAssociationsPanel::getVertices


vector<CoEdge*> QtTopoEntitiesAssociationsPanel::getCoEdges ( ) const
{
	vector<CoEdge*>		entities;
	const vector<string>	names	= getTopoEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		CoEdge*	te	= getContext( ).getTopoManager( ).getCoEdge (*it, true);
		CHECK_NULL_PTR_ERROR (te)
		entities.push_back (te);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtTopoEntitiesAssociationsPanel::getCoEdges


vector<CoFace*> QtTopoEntitiesAssociationsPanel::getCoFaces ( ) const
{
	vector<CoFace*>		entities;
	const vector<string>	names	= getTopoEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		CoFace*	te	= getContext( ).getTopoManager( ).getCoFace (*it, true);
		CHECK_NULL_PTR_ERROR (te)
		entities.push_back (te);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtTopoEntitiesAssociationsPanel::getCoFaces


vector<string> QtTopoEntitiesAssociationsPanel::getGeomEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_currentPanel)
	return _currentPanel->getGeomEntitiesNames ( );
}	// QtTopoEntitiesAssociationsPanel::getGeomEntitiesNames


vector<GeomEntity*> QtTopoEntitiesAssociationsPanel::getGeomEntities ( ) const
{
	vector<GeomEntity*>		entities;
	const vector<string>	names	= getGeomEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*	te	= getContext( ).getGeomManager( ).getEntity (*it, true);
		CHECK_NULL_PTR_ERROR (te)
		entities.push_back (te);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtTopoEntitiesAssociationsPanel::getGeomEntities


bool QtTopoEntitiesAssociationsPanel::projectAllTopoEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_currentPanel)
	return _currentPanel->projectAllTopoEntities ( );
}	// QtTopoEntitiesAssociationsPanel::projectAllTopoEntities


bool QtTopoEntitiesAssociationsPanel::moveVertices ( ) const
{
	CHECK_NULL_PTR_ERROR (_moveVerticiesCheckBox)
	return Qt::Checked == _moveVerticiesCheckBox->checkState ( ) ? true : false;
}	// QtTopoEntitiesAssociationsPanel::moveVertices


void QtTopoEntitiesAssociationsPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_topoToGeomEntityPanel)
	CHECK_NULL_PTR_ERROR (_verticesToGeomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_edgesToCurvesPanel)
	CHECK_NULL_PTR_ERROR (_facesToSurfacesPanel)
	_topoToGeomEntityPanel->reset ( );
	_verticesToGeomEntitiesPanel->reset ( );
	_edgesToCurvesPanel->reset ( );
	_facesToSurfacesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopoEntitiesAssociationsPanel::reset


void QtTopoEntitiesAssociationsPanel::validate ( )
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
		error << "QtTopoEntitiesAssociationsPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getTopoEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopoEntitiesAssociationsPanel::validate


void QtTopoEntitiesAssociationsPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_currentPanel)
	_currentPanel->cancel ( );
}	// QtTopoEntitiesAssociationsPanel::cancel


void QtTopoEntitiesAssociationsPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_currentPanel)
	_currentPanel->autoUpdate ( );

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopoEntitiesAssociationsPanel::autoUpdate


vector<Entity*> QtTopoEntitiesAssociationsPanel::getInvolvedEntities ( )
{
	CHECK_NULL_PTR_ERROR (_currentPanel)
	return _currentPanel->getPanelEntities ( );
}	// QtTopoEntitiesAssociationsPanel::getInvolvedEntities


void QtTopoEntitiesAssociationsPanel::preview (
											bool show, bool destroyInteractor)
{
	QtMgx3DOperationPanel::preview (show, destroyInteractor);

	if ((false == show) || (false == previewResult ( )))
		return;

	// Lors de la construction getGraphicalWidget peut être nul ...
	try
	{
		CHECK_NULL_PTR_ERROR (getMainWindow ( ))
		getMainWindow ( )->getGraphicalWidget ( ).getRenderingManager ( );
	}
	catch (...)
	{
		return;
	}

	try
	{

	unique_ptr<CommandEditTopo>	command;
	Context*		context			= dynamic_cast<Context*>(&getContext ( ));
	CHECK_NULL_PTR_ERROR (context)
	vector<GeomEntity*>			geomEntities	= getGeomEntities ( );
	GeomEntity*				geomEntity	=
						0 == geomEntities.size ( ) ? 0 : geomEntities [0];
	const bool				move		= moveVertices ( );
	CoEdge*					edge		= 0;	// 02/08/18
	bool					general		= false;// 02/08/18

	switch (getProjectionMethod ( ))
	{
		case QtTopoToGeomAssociationPanel::TOPO_ENTITIES_GEOM_ENTITY	:
		{
			vector<TopoEntity*>		topoEntities	= getTopoEntities ( );
			command.reset (new CommandSetGeomAssociation (
							*context, topoEntities, geomEntity, move));
			for (vector<TopoEntity*>::iterator itte = topoEntities.begin ( );
			     itte != topoEntities.end ( ); itte++)
			{	// 02/08/18
				CoEdge*	ce	= dynamic_cast<CoEdge*>(*itte);
				if (0 == ce)
					general	= true;
				else
					edge	= ce;
			}	// or (vector<TopoEntity*>::iterator itte = topoEntities.begin ( );
		}
		break;
		case QtTopoToGeomAssociationPanel::VERTICES_GEOM_ENTITIES		:
		{
			vector<Topo::Vertex*>	vertices	= getVertices ( );
			command.reset (new CommandProjectVerticesOnNearestGeomEntities (
							*context, vertices, geomEntities, move));
			general	= true;
		}
		break;
		case QtTopoToGeomAssociationPanel::EDGES_CURVES					:
		{
			vector<Topo::CoEdge*>	edges	= getCoEdges ( );
			command.reset (new CommandProjectEdgesOnCurves (*context, edges));
			edge	= 0 != edges.size ( ) ? edges [0] : 0;	// 02/08/18
		}
		break;
		case QtTopoToGeomAssociationPanel::FACES_SURFACES				:
		{
			vector<Topo::CoFace*>	faces	= getCoFaces ( );
			command.reset (new CommandProjectFacesOnSurfaces (*context, faces));
			general	= true;
		}
		break;
		default	:
			throw Exception (UTF8String ("QtTopoEntitiesAssociationsPanel::preview : cas non supporté.", Charset::UTF_8));
	}	// switch
	CHECK_NULL_PTR_ERROR (command.get ( ))

	RenderingManager::CommandPreviewMgr	previewManager (
							*command.get ( ), getRenderingManager ( ), true);

	// Traitement cas général (pas que des arêtes) ?
	if (true == general)
	{
		DisplayRepresentation::type t   = 
			(DisplayRepresentation::type)(DisplayRepresentation::WIRE | DisplayRepresentation::SHOWMESHSHAPE);
		TopoDisplayRepresentation	dr (t);
		command->getPreviewRepresentation (dr);
		const vector<Math::Point>&	points	= dr.getPoints ( );
		const vector<size_t>&		indices	= dr.getCurveDiscretization ( );

		DisplayProperties	graphicalProps;
		graphicalProps.setWireColor (Color (
						255 * Resources::instance ( )._previewColor.getRed ( ),
						255 * Resources::instance ( )._previewColor.getGreen ( ),
						255 * Resources::instance ( )._previewColor.getBlue ( )));
		graphicalProps.setLineWidth (Resources::instance ( )._previewWidth.getValue ( ));
		RenderingManager::RepresentationID	repID	=
			getRenderingManager ( ).createSegmentsWireRepresentation (
									points, indices, graphicalProps, true);
		registerPreviewedObject (repID);
	}	// if (true == general)

	// Ajouter les infos de pré-maillage ?
	if ((0 != edge) && (0 != edge->getDisplayProperties ( ).getGraphicalRepresentation ( )))
	{
		const DisplayProperties&	props	=
				true == getRenderingManager ( ).useGlobalDisplayProperties ( ) ?
				getRenderingManager ( ).getContext ( ).globalDisplayProperties (
					edge->getType ( )) : edge->getDisplayProperties ( );
		const unsigned long		mask	=
				true == getRenderingManager ( ).useGlobalDisplayProperties ( ) ?
				getRenderingManager().getContext().globalMask(edge->getType()) :
				edge->getDisplayProperties ( ).getGraphicalRepresentation ( )->getRepresentationMask( );

		previewEdges (*command, previewManager, InfoCommand::DISPMODIFIED, props, mask);
	}	// if ((0 != edge) && (0 != edge->getDisplayProperties ( ).getGraphicalRepresentation ( )))

	getRenderingManager ( ).forceRender ( );

	}
	catch (const Exception& exc)
	{
#ifdef _DEBUG
		cout << "EXC dans QtTopoEntitiesAssociationsPanel::preview : " << exc.getFullMessage ( ) << endl;
#endif
	}
	catch (...)
	{
	}
}	// QtTopoEntitiesAssociationsPanel::preview


void QtTopoEntitiesAssociationsPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _currentPanel){
		_currentPanel->stopSelection ( );
		_currentPanel->clearSelection();
	}
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopoEntitiesAssociationsPanel::operationCompleted


void QtTopoEntitiesAssociationsPanel::projectionMethodCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _currentPanel)
	{
		preview (false, true);
		highlight (false);
	}	// if (0 != _currentPanel)

	CHECK_NULL_PTR_ERROR (_currentPanelParentWidget)
	CHECK_NULL_PTR_ERROR (_currentPanelParentWidget->layout ( ))
	CHECK_NULL_PTR_ERROR (_topoToGeomEntityPanel)
	CHECK_NULL_PTR_ERROR (_verticesToGeomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_edgesToCurvesPanel)
	CHECK_NULL_PTR_ERROR (_facesToSurfacesPanel)
	CHECK_NULL_PTR_ERROR (_moveVerticiesCheckBox)

	if (0 != _currentPanel)	
	{
		_currentPanelParentWidget->layout ( )->removeWidget (_currentPanel);
		_currentPanel->hide ( );
		_currentPanel->setParent (0);
	}	// if (0 != _currentPanel)
	_currentPanel	= 0;

	bool	allowMove	= true;
	switch (getProjectionMethod ( ))
	{
		case QtTopoToGeomAssociationPanel::TOPO_ENTITIES_GEOM_ENTITY	:
			_currentPanel	= _topoToGeomEntityPanel;		break;
		case QtTopoToGeomAssociationPanel::VERTICES_GEOM_ENTITIES		:
			_currentPanel	= _verticesToGeomEntitiesPanel;	break;
		case QtTopoToGeomAssociationPanel::EDGES_CURVES					:
			_currentPanel	= _edgesToCurvesPanel;
			allowMove		= false;						break;
		case QtTopoToGeomAssociationPanel::FACES_SURFACES				:
			_currentPanel	= _facesToSurfacesPanel;
			allowMove		= false;						break;
		default															:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode de projection non supportée ("
			        << (unsigned long)getProjectionMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtTopoEntitiesAssociationsPanel::projectionMethodCallback")
			throw exc;
		}
	}	// switch (getProjectionMethod ( ))

	_moveVerticiesCheckBox->setEnabled (allowMove);
	if (0 != _currentPanel)
	{
		_currentPanel->setParent (_currentPanelParentWidget);
		_currentPanelParentWidget->layout ( )->addWidget (_currentPanel);
		_currentPanel->show ( );
		if (0 != getMainWindow ( ))
			getMainWindow ( )->getOperationsPanel ( ).updateLayoutWorkaround( );

		preview (true, false);
		highlight (true);
	}	// if (0 != _currentPanel)

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopoEntitiesAssociationsPanel::projectionMethodCallback


// ===========================================================================
//                  LA CLASSE QtTopoEntitiesAssociationsAction
// ===========================================================================

QtTopoEntitiesAssociationsAction::QtTopoEntitiesAssociationsAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip,
	Mgx3D::Utils::SelectionManagerIfc::DIM defaultDimensions)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopoEntitiesAssociationsPanel*	operationPanel	=
		new QtTopoEntitiesAssociationsPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this, defaultDimensions);
	setOperationPanel (operationPanel);
}	// QtTopoEntitiesAssociationsAction::QtTopoEntitiesAssociationsAction


QtTopoEntitiesAssociationsAction::QtTopoEntitiesAssociationsAction (
										const QtTopoEntitiesAssociationsAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopoEntitiesAssociationsAction copy constructor is not allowed.")
}	// QtTopoEntitiesAssociationsAction::QtTopoEntitiesAssociationsAction


QtTopoEntitiesAssociationsAction& QtTopoEntitiesAssociationsAction::operator = (
										const QtTopoEntitiesAssociationsAction&)
{
	MGX_FORBIDDEN ("QtTopoEntitiesAssociationsAction assignment operator is not allowed.")
	return *this;
}	// QtTopoEntitiesAssociationsAction::operator =


QtTopoEntitiesAssociationsAction::~QtTopoEntitiesAssociationsAction ( )
{
}	// QtTopoEntitiesAssociationsAction::~QtTopoEntitiesAssociationsAction


QtTopoEntitiesAssociationsPanel*
					QtTopoEntitiesAssociationsAction::getAssociationPanel ( )
{
	return dynamic_cast<QtTopoEntitiesAssociationsPanel*>(getOperationPanel ( ));
}	// QtTopoEntitiesAssociationsAction::getAssociationPanel


void QtTopoEntitiesAssociationsAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités topologiques :
	QtTopoEntitiesAssociationsPanel*	panel	= getAssociationPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>			topoEntities	= panel->getTopoEntitiesNames ( );
	vector<string>			geomEntities	= panel->getGeomEntitiesNames ( );
	const bool				move			= panel->moveVertices ( );
	const bool				projectAll		= panel->projectAllTopoEntities ( );

	switch (panel->getProjectionMethod ( ))
	{
		case QtTopoToGeomAssociationPanel::TOPO_ENTITIES_GEOM_ENTITY	:
		{
			const string	name (0 == geomEntities.size ( ) ? string ( ) : geomEntities [0]);
			cmdResult	= getContext ( ).getTopoManager ( ).setGeomAssociation (topoEntities, name, move);
		}
		break;
		case QtTopoToGeomAssociationPanel::VERTICES_GEOM_ENTITIES		:
			cmdResult	= getContext ( ).getTopoManager ().projectVerticesOnNearestGeomEntities (topoEntities, geomEntities, move);
		break;
		case QtTopoToGeomAssociationPanel::EDGES_CURVES					:
			if (false == projectAll)
				cmdResult	= getContext ( ).getTopoManager ( ).projectEdgesOnCurves (topoEntities);
			else
				cmdResult	= getContext ( ).getTopoManager ( ).projectAllEdgesOnCurves ( );
		break;
		case QtTopoToGeomAssociationPanel::FACES_SURFACES				:
			if (false == projectAll)
				cmdResult	= getContext ( ).getTopoManager ( ).projectFacesOnSurfaces (topoEntities);
			else
				cmdResult	= getContext ( ).getTopoManager ( ).projectAllFacesOnSurfaces ( );
		break;
		default															:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode de projection non supportée ("
			        << (unsigned long)panel->getProjectionMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtTopoEntitiesAssociationsAction::executeOperation")
			throw exc;
		}
	}	// switch (panel->getProjectionMethod ( ))

	setCommandResult (cmdResult);
}	// QtTopoEntitiesAssociationsAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
