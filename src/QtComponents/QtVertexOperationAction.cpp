/**
 * \file        QtVertexOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        09/09/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtVertexOperationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "Geom/CommandNewVertexByCurveParameterization.h"
#include "Geom/CommandNewVertexByProjection.h"
#include "Geom/CommandNewVertex.h"
#include "Geom/GeomDisplayRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QBoxLayout>

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
//      LA CLASSE QtVertexOperationPanel::QtVertexCurveProjectionPanel
// ===========================================================================


QtVertexOperationPanel::QtVertexCurveProjectionPanel::QtVertexCurveProjectionPanel (
			 QWidget* parent, const string& appTitle, QtMgx3DMainWindow& window)
	: QtMgx3DOperationsSubPanel (parent, window),
	  _pointPanel (0), _curvePanel (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (layout);

	// Le point projeté :
	_pointPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Point projeté :", "", &window, SelectionManagerIfc::D0,
			FilterEntity::GeomVertex);
	layout->addWidget (_pointPanel);
	connect (_pointPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_pointPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// La courbe sur laquelle est projeté le point :
	_curvePanel	= new QtMgx3DEntityPanel (
					this, "", true, "Courbe :", "", &window, SelectionManagerIfc::D1,
					FilterEntity::GeomCurve);
	layout->addWidget (_curvePanel);
	connect (_curvePanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_curvePanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// enchainement pour sélection interactive
	CHECK_NULL_PTR_ERROR(_pointPanel->getNameTextField())
	CHECK_NULL_PTR_ERROR(_curvePanel->getNameTextField())
	_pointPanel->getNameTextField()->setLinkedSeizureManagers(0, _curvePanel->getNameTextField());
	_curvePanel->getNameTextField()->setLinkedSeizureManagers(_pointPanel->getNameTextField(), 0);

}	// QtVertexCurveProjectionPanel::QtVertexCurveProjectionPanel


QtVertexOperationPanel::QtVertexCurveProjectionPanel::QtVertexCurveProjectionPanel (
	const QtVertexOperationPanel::QtVertexCurveProjectionPanel& p)
	: QtMgx3DOperationsSubPanel (p),
	  _pointPanel (0), _curvePanel (0)
{
	MGX_FORBIDDEN ("QtVertexCurveProjectionPanel copy constructor is not allowed.");
}	// QtVertexCurveProjectionPanel::QtVertexCurveProjectionPanel


QtVertexOperationPanel::QtVertexCurveProjectionPanel&
	QtVertexOperationPanel::QtVertexCurveProjectionPanel::operator = (
					const QtVertexOperationPanel::QtVertexCurveProjectionPanel&)
{
	 MGX_FORBIDDEN ("QtVertexCurveProjectionPanel assignment operator is not allowed.");
	return *this;
}	// QtVertexCurveProjectionPanel::operator =


QtVertexOperationPanel::QtVertexCurveProjectionPanel::~QtVertexCurveProjectionPanel ( )
{
}	// QtVertexCurveProjectionPanel::~QtVertexCurveProjectionPanel


void QtVertexOperationPanel::QtVertexCurveProjectionPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_curvePanel)
	_pointPanel->reset ( );
	_curvePanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationsSubPanel::reset ( );
}	// QtVertexCurveProjectionPanel::reset


void QtVertexOperationPanel::QtVertexCurveProjectionPanel::cancel ( )
{
	QtMgx3DOperationsSubPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_curvePanel)
	_pointPanel->stopSelection ( );
	_curvePanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_pointPanel->setUniqueName ("");
		_curvePanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtVertexCurveProjectionPanel::cancel


void QtVertexOperationPanel::QtVertexCurveProjectionPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_curvePanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedVertices	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomVertex);
		if (1 == selectedVertices.size ( ))
			_pointPanel->setUniqueName (selectedVertices [0]);
		const vector<string>	selectedCurves	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomCurve);
		if (1 == selectedCurves.size ( ))
			_curvePanel->setUniqueName (selectedCurves [0]);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_pointPanel->clearSelection ( );
	_curvePanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_pointPanel->actualizeGui (true);
	_curvePanel->actualizeGui (true);

	QtMgx3DOperationsSubPanel::autoUpdate ( );
}	// QtVertexCurveProjectionPanel::autoUpdate


string QtVertexOperationPanel::QtVertexCurveProjectionPanel::getVertexUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_pointPanel)
	return _pointPanel->getUniqueName ( );
}	// QtVertexCurveProjectionPanel::getVertexUniqueName


string QtVertexOperationPanel::QtVertexCurveProjectionPanel::getCurveUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_curvePanel)
	return _curvePanel->getUniqueName ( );
}	// QtVertexCurveProjectionPanel::getCurveUniqueName


void QtVertexOperationPanel::QtVertexCurveProjectionPanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_curvePanel)
	_pointPanel->stopSelection ( );
	_curvePanel->stopSelection ( );
}	// QtVertexCurveProjectionPanel::stopSelection


vector<Entity*>
	QtVertexOperationPanel::QtVertexCurveProjectionPanel::getInvolvedEntities( )
{
	vector<Entity*>	entities;

	const string	vertexName	= getVertexUniqueName ( );
	GeomEntity*		entity		=
			 getContext ( ).getGeomManager ( ).getVertex (vertexName, false);
	if (0 != entity)
		entities.push_back (entity);
	const string	curveName	= getCurveUniqueName ( );
	entity		= getContext ( ).getGeomManager ( ).getCurve (curveName, false);
	if (0 != entity)
		entities.push_back (entity);

	return entities;
}	// QtVertexCurveProjectionPanel::getInvolvedEntities


QtMgx3DEntityPanel& QtVertexOperationPanel::QtVertexCurveProjectionPanel::getCurvePanel ( )
{
	CHECK_NULL_PTR_ERROR (_curvePanel)
	return *_curvePanel;
}	// QtVertexCurveProjectionPanel::getCurvePanel

QtMgx3DEntityPanel& QtVertexOperationPanel::QtVertexCurveProjectionPanel::getPointPanel ( )
{
	CHECK_NULL_PTR_ERROR (_pointPanel)
	return *_pointPanel;
}	// QtVertexCurveProjectionPanel::getPointPanel



// ===========================================================================
//      LA CLASSE QtVertexOperationPanel::QtVertexSurfaceProjectionPanel
// ===========================================================================


QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::QtVertexSurfaceProjectionPanel (
			 QWidget* parent, const string& appTitle, QtMgx3DMainWindow& window)
	: QtMgx3DOperationsSubPanel (parent, window),
	  _pointPanel (0), _surfacePanel (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (layout);

	// Le point projeté :
	_pointPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Point projeté :", "", &window, SelectionManagerIfc::D0,
			FilterEntity::GeomVertex);
	layout->addWidget (_pointPanel);
	connect (_pointPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_pointPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// La surface sur laquelle est projeté le point :
	_surfacePanel	= new QtMgx3DEntityPanel (
					this, "", true, "Surface :", "", &window, SelectionManagerIfc::D2,
					FilterEntity::GeomSurface);
	layout->addWidget (_surfacePanel);
	connect (_surfacePanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_surfacePanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// enchainement pour sélection interactive
	CHECK_NULL_PTR_ERROR(_pointPanel->getNameTextField())
	CHECK_NULL_PTR_ERROR(_surfacePanel->getNameTextField())
	_pointPanel->getNameTextField()->setLinkedSeizureManagers(0, _surfacePanel->getNameTextField());
	_surfacePanel->getNameTextField()->setLinkedSeizureManagers(_pointPanel->getNameTextField(), 0);

}	// QtVertexSurfaceProjectionPanel::QtVertexSurfaceProjectionPanel


QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::QtVertexSurfaceProjectionPanel (
	const QtVertexOperationPanel::QtVertexSurfaceProjectionPanel& p)
	: QtMgx3DOperationsSubPanel (p),
	  _pointPanel (0), _surfacePanel (0)
{
	MGX_FORBIDDEN ("QtVertexSurfaceProjectionPanel copy constructor is not allowed.");
}	// QtVertexSurfaceProjectionPanel::QtVertexSurfaceProjectionPanel


QtVertexOperationPanel::QtVertexSurfaceProjectionPanel&
	QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::operator = (
					const QtVertexOperationPanel::QtVertexSurfaceProjectionPanel&)
{
	 MGX_FORBIDDEN ("QtVertexSurfaceProjectionPanel assignment operator is not allowed.");
	return *this;
}	// QtVertexSurfaceProjectionPanel::operator =


QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::~QtVertexSurfaceProjectionPanel ( )
{
}	// QtVertexSurfaceProjectionPanel::~QtVertexSurfaceProjectionPanel


void QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_surfacePanel)
	_pointPanel->reset ( );
	_surfacePanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationsSubPanel::reset ( );
}	// QtVertexSurfaceProjectionPanel::reset


void QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::cancel ( )
{
	QtMgx3DOperationsSubPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_surfacePanel)
	_pointPanel->stopSelection ( );
	_surfacePanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_pointPanel->setUniqueName ("");
		_surfacePanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtVertexSurfaceProjectionPanel::cancel


void QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_surfacePanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedVertices	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomVertex);
		if (1 == selectedVertices.size ( ))
			_pointPanel->setUniqueName (selectedVertices [0]);
		const vector<string>	selectedSurfaces	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomSurface);
		if (1 == selectedSurfaces.size ( ))
			_surfacePanel->setUniqueName (selectedSurfaces [0]);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_pointPanel->clearSelection ( );
	_surfacePanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_pointPanel->actualizeGui (true);
	_surfacePanel->actualizeGui (true);

	QtMgx3DOperationsSubPanel::autoUpdate ( );
}	// QtVertexSurfaceProjectionPanel::autoUpdate


string QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::getVertexUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_pointPanel)
	return _pointPanel->getUniqueName ( );
}	// QtVertexSurfaceProjectionPanel::getVertexUniqueName


string QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::getSurfaceUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_surfacePanel)
	return _surfacePanel->getUniqueName ( );
}	// QtVertexSurfaceProjectionPanel::getSurfaceUniqueName


void QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_surfacePanel)
	_pointPanel->stopSelection ( );
	_surfacePanel->stopSelection ( );
}	// QtVertexSurfaceProjectionPanel::stopSelection


vector<Entity*>
	QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::getInvolvedEntities( )
{
	vector<Entity*>	entities;

	const string	vertexName	= getVertexUniqueName ( );
	GeomEntity*		entity		=
			 getContext ( ).getGeomManager ( ).getVertex (vertexName, false);
	if (0 != entity)
		entities.push_back (entity);
	const string	surfaceName	= getSurfaceUniqueName ( );
	entity		= getContext ( ).getGeomManager ( ).getCurve (surfaceName, false);
	if (0 != entity)
		entities.push_back (entity);

	return entities;
}	// QtVertexSurfaceProjectionPanel::getInvolvedEntities


QtMgx3DEntityPanel& QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::getSurfacePanel ( )
{
	CHECK_NULL_PTR_ERROR (_surfacePanel)
	return *_surfacePanel;
}	// QtVertexSurfaceProjectionPanel::getCurvePanel

QtMgx3DEntityPanel& QtVertexOperationPanel::QtVertexSurfaceProjectionPanel::getPointPanel ( )
{
	CHECK_NULL_PTR_ERROR (_pointPanel)
	return *_pointPanel;
}	// QtVertexSurfaceProjectionPanel::getPointPanel



// ===========================================================================
//      LA CLASSE QtVertexOperationPanel::QtVerticesRatioPanel
// ===========================================================================


QtVertexOperationPanel::QtVerticesRatioPanel::QtVerticesRatioPanel (
        QWidget* parent, const string& appTitle, QtMgx3DMainWindow& window)
        : QtMgx3DOperationsSubPanel (parent, window),
          _firstPointPanel (0), _secondPointPanel (0), _ratePanel (0)
{
    QVBoxLayout*	layout	= new QVBoxLayout (this);
    layout->setContentsMargins  (
            Resources::instance ( )._margin.getValue ( ),
            Resources::instance ( )._margin.getValue ( ),
            Resources::instance ( )._margin.getValue ( ),
            Resources::instance ( )._margin.getValue ( ));
    layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
    setLayout (layout);

    // Le premier point :
    _firstPointPanel	= new QtMgx3DEntityPanel (
            this, "", true, "Premier point :", "", &window, SelectionManagerIfc::D0,
            FilterEntity::GeomVertex);
    layout->addWidget (_firstPointPanel);
    connect (_firstPointPanel, SIGNAL (entitiesAddedToSelection(QString)),
             this, SLOT (entitiesAddedToSelectionCallback (QString)));
    connect (_firstPointPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
             this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

      // Le second point :
    _secondPointPanel	= new QtMgx3DEntityPanel (
            this, "", true, "Second point :", "", &window, SelectionManagerIfc::D0,
            FilterEntity::GeomVertex);
    layout->addWidget (_secondPointPanel);
    connect (_secondPointPanel, SIGNAL (entitiesAddedToSelection(QString)),
             this, SLOT (entitiesAddedToSelectionCallback (QString)));
    connect (_secondPointPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
             this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

    // Le ratio :
    QHBoxLayout*	hLayout	= new QHBoxLayout (0);
    layout->addLayout (hLayout);
    QLabel*	label	= new QLabel ("Ratio / 2 points :", this);
    hLayout->addWidget (label);
    _ratePanel	= &QtNumericFieldsFactory::createPositionTextField (this);
    _ratePanel->setValue (0.5);
    hLayout->addWidget (_ratePanel);
    hLayout->addStretch (2);

}	// QtVerticesRatioPanel::QtVerticesRatioPanel


QtVertexOperationPanel::QtVerticesRatioPanel::QtVerticesRatioPanel (
        const QtVertexOperationPanel::QtVerticesRatioPanel& p)
        : QtMgx3DOperationsSubPanel (p),
          _firstPointPanel (0), _secondPointPanel (0), _ratePanel (0)
{
    MGX_FORBIDDEN ("QtVerticesRatioPanel copy constructor is not allowed.");
}	// QtVerticesRatioPanel::QtVerticesRatioPanel


QtVertexOperationPanel::QtVerticesRatioPanel&
QtVertexOperationPanel::QtVerticesRatioPanel::operator = (
        const QtVertexOperationPanel::QtVerticesRatioPanel&)
{
    MGX_FORBIDDEN ("QtVerticesRatioPanel assignment operator is not allowed.");
    return *this;
}	// QtVerticesRatioPanel::operator =


QtVertexOperationPanel::QtVerticesRatioPanel::~QtVerticesRatioPanel ( )
{
}	// QtVerticesRatioPanel::~QtVerticesRatioPanel


void QtVertexOperationPanel::QtVerticesRatioPanel::reset ( )
{
    BEGIN_QT_TRY_CATCH_BLOCK

        CHECK_NULL_PTR_ERROR (_firstPointPanel)
        CHECK_NULL_PTR_ERROR (_secondPointPanel)
        _firstPointPanel->reset ( );
        _secondPointPanel->reset ( );


    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

    QtMgx3DOperationsSubPanel::reset ( );
}	// QtVerticesRatioPanel::reset


void QtVertexOperationPanel::QtVerticesRatioPanel::cancel ( )
{
    QtMgx3DOperationsSubPanel::cancel ( );

    CHECK_NULL_PTR_ERROR (_firstPointPanel)
    CHECK_NULL_PTR_ERROR (_secondPointPanel)

    _firstPointPanel->stopSelection ( );
    _secondPointPanel->stopSelection ( );

    if (true == cancelClearEntities ( ))
    {
        BEGIN_QT_TRY_CATCH_BLOCK

            _firstPointPanel->setUniqueName ("");
            _secondPointPanel->setUniqueName ("");

        COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
    }	// if (true == cancelClearEntities ( ))
}	// QtVerticesRatioPanel::cancel


void QtVertexOperationPanel::QtVerticesRatioPanel::autoUpdate ( )
{
    CHECK_NULL_PTR_ERROR (_firstPointPanel)
    CHECK_NULL_PTR_ERROR (_secondPointPanel)

    _firstPointPanel->clearSelection ( );
    _secondPointPanel->clearSelection ( );

    _firstPointPanel->actualizeGui (true);
    _secondPointPanel->actualizeGui (true);

    QtMgx3DOperationsSubPanel::autoUpdate ( );
}	// QtVerticesRatioPanel::autoUpdate


string QtVertexOperationPanel::QtVerticesRatioPanel::getFirstVertexUniqueName ( ) const
{
    CHECK_NULL_PTR_ERROR (_firstPointPanel)
    return _firstPointPanel->getUniqueName ( );
}	// QtVerticesRatioPanel::getFirstVertexUniqueName

string QtVertexOperationPanel::QtVerticesRatioPanel::getSecondVertexUniqueName ( ) const
{
    CHECK_NULL_PTR_ERROR (_secondPointPanel)
    return _secondPointPanel->getUniqueName ( );
}	// QtVerticesRatioPanel::getSecondVertexUniqueName


 double QtVertexOperationPanel::QtVerticesRatioPanel::getRate ( ) const
{
    CHECK_NULL_PTR_ERROR (_ratePanel)
    return _ratePanel->getValue ( );
}	// QtVerticesRatioPanel::getRate



void QtVertexOperationPanel::QtVerticesRatioPanel::stopSelection ( )
{
    CHECK_NULL_PTR_ERROR (_firstPointPanel)
    CHECK_NULL_PTR_ERROR (_secondPointPanel)
    _firstPointPanel->stopSelection ( );
    _secondPointPanel->stopSelection ( );
}	// QtVerticesRatioPanel::stopSelection


vector<Entity*>
QtVertexOperationPanel::QtVerticesRatioPanel::getInvolvedEntities( )
{
    vector<Entity*>	entities;

    const string	vertexName	= getFirstVertexUniqueName ( );
    GeomEntity*		entity		=
            getContext ( ).getGeomManager ( ).getVertex (vertexName, false);
    if (0 != entity)
        entities.push_back (entity);
    const string	curveName	= getSecondVertexUniqueName ( );
    entity		= getContext ( ).getGeomManager ( ).getCurve (curveName, false);
    if (0 != entity)
        entities.push_back (entity);

    return entities;
}	// QtVerticesRatioPanel::getInvolvedEntities


QtMgx3DEntityPanel& QtVertexOperationPanel::QtVerticesRatioPanel::getFirstPointPanel ( )
{
    CHECK_NULL_PTR_ERROR (_firstPointPanel)
    return *_firstPointPanel;
}	// QtVerticesRatioPanel::getFirstPointPanel

QtMgx3DEntityPanel& QtVertexOperationPanel::QtVerticesRatioPanel::getSecondPointPanel ( )
{
    CHECK_NULL_PTR_ERROR (_secondPointPanel)
    return *_secondPointPanel;
}	// QtVerticesRatioPanel::getSecondPointPanel


QtDoubleTextField& QtVertexOperationPanel::QtVerticesRatioPanel::getRatePanel ( )
{
    CHECK_NULL_PTR_ERROR (_ratePanel)
    return *_ratePanel;
}	// QtVerticesRatioPanel::getRatePanel


// ===========================================================================
//                        LA CLASSE QtVertexOperationPanel
// ===========================================================================

QtVertexOperationPanel::QtVertexOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).vertexOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).vertexOperationTag),
	  _namePanel (0), _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _pointPanel (0), _vertexCurveProjectionPanel (0), _vertexSurfaceProjectionPanel (0),
	  _curvilinearAbscissaPanel (0), _verticesRatioPanel (0)
{
//	SET_WIDGET_BACKGROUND (this, Qt::yellow)
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (layout);

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Nom groupe :
	_namePanel	= new QtMgx3DGroupNamePanel (
							this, "Groupe", mainWindow, 0, creationPolicy, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// Méthode de création/modification du vertex :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Méthode"), this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem (QString::fromUtf8("Par saisie des coordonnées"));
	_operationMethodComboBox->addItem ("Par projection d'un point sur une courbe");
	_operationMethodComboBox->addItem ("Par projection d'un point sur une surface");
	_operationMethodComboBox->addItem ("Par saisie d'une abscisse curviligne");
	_operationMethodComboBox->addItem ("Par ratio entre deux points");
	connect (_operationMethodComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (operationMethodCallback ( )));
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);
	
	// Définition du point :
	QtGroupBox*		groupBox	= new QtGroupBox (this, "Paramètres du point");
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	vlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (vlayout);
	layout->addWidget (groupBox);
	// Le panneau "méthode" : plusieurs panneaux possibles.
	// On garde un parent unique dans la layout :
	_currentParentWidget	= new QWidget (groupBox);
	vlayout->addWidget (_currentParentWidget);
	QHBoxLayout*	currentLayout = new QHBoxLayout (_currentParentWidget);
	_currentParentWidget->setLayout (currentLayout);
	_pointPanel	= new QtMgx3DPointPanel (
		0, "", true, "x :", "y :", "z :",
		0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
		&mainWindow,
		(FilterEntity::objectType)(FilterEntity::GeomVertex|FilterEntity::TopoVertex),
		true);
	connect (_pointPanel, SIGNAL (pointModified ( )),
	         this, SLOT (parametersModifiedCallback ( )));
	_pointPanel->layout ( )->setSpacing (
						Resources::instance ( )._spacing.getValue ( ));
	_pointPanel->layout ( )->setContentsMargins (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	_pointPanel->hide ( );
	_vertexCurveProjectionPanel	=
			new QtVertexCurveProjectionPanel (
					0, "Projection d'un vertex sur une courbe", mainWindow);
	_vertexCurveProjectionPanel->hide ( );
	connect (_vertexCurveProjectionPanel->getPointPanel().getNameTextField(),
			SIGNAL (selectionModified (QString)), this,
			SLOT (parametersModifiedCallback ( )));
	connect (_vertexCurveProjectionPanel->getCurvePanel().getNameTextField(),
			SIGNAL (selectionModified (QString)), this,
			SLOT (parametersModifiedCallback ( )));

	_vertexSurfaceProjectionPanel	=
			new QtVertexSurfaceProjectionPanel (
					0, "Projection d'un vertex sur une surface", mainWindow);
	_vertexSurfaceProjectionPanel->hide ( );
	connect (_vertexSurfaceProjectionPanel->getPointPanel().getNameTextField(),
			SIGNAL (selectionModified (QString)), this,
			SLOT (parametersModifiedCallback ( )));
	connect (_vertexSurfaceProjectionPanel->getSurfacePanel().getNameTextField(),
			SIGNAL (selectionModified (QString)), this,
			SLOT (parametersModifiedCallback ( )));


	_curvilinearAbscissaPanel	= new QtCurvilinearAbscissaPanel (
					0, "Abscisse curviligne sur une courbe", &mainWindow);
	_curvilinearAbscissaPanel->hide ( );
	connect (_curvilinearAbscissaPanel->getCurvePanel ( ).getNameTextField ( ),
	         SIGNAL (selectionModified (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
	connect (&_curvilinearAbscissaPanel->getCurvilinearAbscissaPanel ( ),
		         SIGNAL (textEdited (const QString&)), this,
		         SLOT (parametersModifiedCallback ( )));

	_verticesRatioPanel   = new QtVerticesRatioPanel (
	        0, "Ratio entre 2 points", mainWindow);
	_verticesRatioPanel->hide();
	connect (_verticesRatioPanel->getFirstPointPanel ( ).getNameTextField ( ),
	         SIGNAL (selectionModified (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
	connect (_verticesRatioPanel->getSecondPointPanel ( ).getNameTextField ( ),
	         SIGNAL (selectionModified (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
    connect (&_verticesRatioPanel->getRatePanel ( ),
         SIGNAL (textEdited (const QString&)), this,
         SLOT (parametersModifiedCallback ( )));

	operationMethodCallback ( );

	addPreviewCheckBox (true);

	vlayout->addStretch (2);
}	// QtVertexOperationPanel::QtVertexOperationPanel


QtVertexOperationPanel::QtVertexOperationPanel (
										const QtVertexOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _pointPanel (0), _vertexCurveProjectionPanel (0), _vertexSurfaceProjectionPanel (0),
	  _curvilinearAbscissaPanel (0), _verticesRatioPanel (0)
{
	MGX_FORBIDDEN ("QtVertexOperationPanel copy constructor is not allowed.");
}	// QtVertexOperationPanel::QtVertexOperationPanel (const QtVertexOperationPanel&)


QtVertexOperationPanel& QtVertexOperationPanel::operator = (
											const QtVertexOperationPanel&)
{
	MGX_FORBIDDEN ("QtVertexOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtVertexOperationPanel::QtVertexOperationPanel (const QtVertexOperationPanel&)


QtVertexOperationPanel::~QtVertexOperationPanel ( )
{
}	// QtVertexOperationPanel::~QtVertexOperationPanel


string QtVertexOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtVertexOperationPanel::getGroupName


QtVertexOperationPanel::OPERATION_METHOD
						QtVertexOperationPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtVertexOperationPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtVertexOperationPanel::getOperationMethod


Math::Point QtVertexOperationPanel::getPoint ( ) const
{
	CHECK_NULL_PTR_ERROR (_pointPanel);

	return _pointPanel->getPoint();
}	// QtVertexOperationPanel::getPoint


string QtVertexOperationPanel::getCurveName ( ) const
{
	switch (getOperationMethod ( ))
	{
		case QtVertexOperationPanel::VERTEX_PROJECTED_ON_CURVE	:
			CHECK_NULL_PTR_ERROR (_vertexCurveProjectionPanel)
			return _vertexCurveProjectionPanel->getCurveUniqueName ( );
			break;
		case QtVertexOperationPanel::CURVILINEAR_ABSCISSA		:
			CHECK_NULL_PTR_ERROR (_curvilinearAbscissaPanel)
			return _curvilinearAbscissaPanel->getCurveUniqueName ( );
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode getCurveName ( ) invoquée pour une méthode "
			        << "(" << (unsigned long)getOperationMethod ( )
			        << ") ne reposant pas sur une courbe.";
			throw Exception (message);
		}
	}	// switch (getOperationMethod ( ))
}	// QtVertexOperationPanel::getCurveName

string QtVertexOperationPanel::getFirstPointName ( ) const
{
    switch (getOperationMethod ( ))
    {
        case QtVertexOperationPanel::RATE_WITH_2_POINTS	:
            CHECK_NULL_PTR_ERROR (_verticesRatioPanel)
            return _verticesRatioPanel->getFirstVertexUniqueName ( );
            break;
        default	:
        {
            UTF8String	message (Charset::UTF_8);
            message << "Méthode getFirstPointName ( ) invoquée pour une méthode "
                    << "(" << (unsigned long)getOperationMethod ( )
                    << ") ne reposant pas sur un premier point.";
            throw Exception (message);
        }
    }	// switch (getOperationMethod ( ))
}	// QtVertexOperationPanel::getFirstPointName

string QtVertexOperationPanel::getSecondPointName ( ) const
{
    switch (getOperationMethod ( ))
    {
        case QtVertexOperationPanel::RATE_WITH_2_POINTS	:
            CHECK_NULL_PTR_ERROR (_verticesRatioPanel)
            return _verticesRatioPanel->getSecondVertexUniqueName ( );
            break;
        default	:
        {
            UTF8String	message (Charset::UTF_8);
            message << "Méthode getSecondPointName ( ) invoquée pour une méthode "
                    << "(" << (unsigned long)getOperationMethod ( )
                    << ") ne reposant pas sur un second point.";
            throw Exception (message);
        }
    }	// switch (getOperationMethod ( ))
}	// QtVertexOperationPanel::getSecondPointName


string QtVertexOperationPanel::getSurfaceName ( ) const
{
	switch (getOperationMethod ( ))
	{
		case QtVertexOperationPanel::VERTEX_PROJECTED_ON_SURFACE	:
			CHECK_NULL_PTR_ERROR (_vertexSurfaceProjectionPanel)
			return _vertexSurfaceProjectionPanel->getSurfaceUniqueName ( );
			break;
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode getSurfaceName ( ) invoquée pour une méthode "
			        << "(" << (unsigned long)getOperationMethod ( )
			        << ") ne reposant pas sur une surface.";
			throw Exception (message);
		}
	}	// switch (getOperationMethod ( ))
}	// QtVertexOperationPanel::getSurfaceName


string QtVertexOperationPanel::getProjectedVertexName ( ) const
{
	switch (getOperationMethod ( ))
	{
	case QtVertexOperationPanel::VERTEX_PROJECTED_ON_CURVE	:
		CHECK_NULL_PTR_ERROR (_vertexCurveProjectionPanel)
		return _vertexCurveProjectionPanel->getVertexUniqueName ( );
		break;
	case QtVertexOperationPanel::VERTEX_PROJECTED_ON_SURFACE	:
		CHECK_NULL_PTR_ERROR (_vertexSurfaceProjectionPanel)
		return _vertexSurfaceProjectionPanel->getVertexUniqueName ( );
		break;
	default	:
	{
		UTF8String	message (Charset::UTF_8);
		message << "Méthode getProjectedVertexName ( ) invoquée pour une méthode "
				<< "(" << (unsigned long)getOperationMethod ( )
				<< ") ne reposant pas sur une courbe ou une surface.";
		throw Exception (message);
	}
	}
}	// QtVertexOperationPanel::getProjectedVertexName


double QtVertexOperationPanel::getCurvilinearAbscissa ( ) const
{
    switch (getOperationMethod ( )) {
        case QtVertexOperationPanel::CURVILINEAR_ABSCISSA        :
            CHECK_NULL_PTR_ERROR (_curvilinearAbscissaPanel)
            return _curvilinearAbscissaPanel->getCurvilinearAbscissa();
            break;
        case QtVertexOperationPanel::RATE_WITH_2_POINTS    :
            CHECK_NULL_PTR_ERROR (_verticesRatioPanel)
            return _verticesRatioPanel->getRate();
            break;
        default    : {
            UTF8String message(Charset::UTF_8);
            message << "Méthode getCurvilinearAbscissa ( ) invoquée pour une méthode "
                    << "(" << (unsigned long) getOperationMethod()
                    << ") ne reposant pas sur une abscisse curviligne ou un ratio.";
            throw Exception(message);
        }
    }
}	// QtVertexOperationPanel::getCurvilinearAbscissa


void QtVertexOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_vertexCurveProjectionPanel)
	CHECK_NULL_PTR_ERROR (_vertexSurfaceProjectionPanel)
	CHECK_NULL_PTR_ERROR (_curvilinearAbscissaPanel)
	CHECK_NULL_PTR_ERROR (_verticesRatioPanel)
	_pointPanel->reset ( );
	_vertexCurveProjectionPanel->reset ( );
	_vertexSurfaceProjectionPanel->reset ( );
	_curvilinearAbscissaPanel->reset ( );
	_verticesRatioPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtVertexOperationPanel::reset


void QtVertexOperationPanel::validate ( )
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
		error << "QtVertexOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtVertexOperationPanel::COORDINATES	:
		case QtVertexOperationPanel::VERTEX_PROJECTED_ON_CURVE	:
		case QtVertexOperationPanel::VERTEX_PROJECTED_ON_SURFACE	:
		case QtVertexOperationPanel::CURVILINEAR_ABSCISSA	:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode d'opération de création/modification "
			      << "de vertex sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtVertexOperationPanel::validate : index de méthode "
			      << "d'opération de création/modification de vertex "
			      << "invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX ( ), "Abscisse point : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY ( ), "Ordonnée point : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ ( ), "Elévation point : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtVertexOperationPanel::validate


void QtVertexOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_curvilinearAbscissaPanel)
	QtMgx3DOperationsSubPanel*	subPanel	=
					dynamic_cast<QtMgx3DOperationsSubPanel*>(_currentPanel);
	_pointPanel->stopSelection ( );
	_curvilinearAbscissaPanel->stopSelection ( );

	if (0 != subPanel)
		subPanel->cancel ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_pointPanel->setUniqueName ("");
		_curvilinearAbscissaPanel->setCurveUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtVertexOperationPanel::cancel


void QtVertexOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_curvilinearAbscissaPanel)
//	CHECK_NULL_PTR_ERROR (_namePanel)
//	_namePanel->autoUpdate ( );

	QtMgx3DOperationsSubPanel*	subPanel	=
					dynamic_cast<QtMgx3DOperationsSubPanel*>(_currentPanel);
	if (0 != subPanel)
		subPanel->autoUpdate ( );

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedVertices	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomVertex);
		if (1 == selectedVertices.size ( ))
			_pointPanel->setUniqueName (selectedVertices [0]);
		const vector<string>	selectedCurves	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomCurve);
		if (1 == selectedCurves.size ( ))
			_curvilinearAbscissaPanel->setCurveUniqueName (selectedCurves [0]);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_pointPanel->clearSelection ( );
	_curvilinearAbscissaPanel->getCurvePanel ( ).clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtVertexOperationPanel::autoUpdate


vector<Entity*> QtVertexOperationPanel::getInvolvedEntities ( )
{
	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_curvilinearAbscissaPanel)
	CHECK_NULL_PTR_ERROR (_namePanel)
	vector<Entity*>	entities;

	string	pointName, curveName, surfaceName;
	switch (getOperationMethod ( ))
	{
		case QtVertexOperationPanel::COORDINATES	:
			pointName	= _pointPanel->getUniqueName ( );
		break;
		case QtVertexOperationPanel::VERTEX_PROJECTED_ON_CURVE	:
			pointName	= getProjectedVertexName ( );
			curveName	= getCurveName ( );
		break;
		case QtVertexOperationPanel::VERTEX_PROJECTED_ON_SURFACE	:
			pointName	= getProjectedVertexName ( );
			surfaceName	= getSurfaceName ( );
		break;
		case QtVertexOperationPanel::CURVILINEAR_ABSCISSA	:
			curveName	= getCurveName ( );
		break;
		default	:
		{
			INTERNAL_ERROR (exc, "Cas non implémenté.", "QtVertexOperationPanel::getInvolvedEntities")
			throw exc;
		}
	}	// switch (getOperationMethod ( ))

	if (0 != pointName.length ( ))
	{
		Entity*	entity	= getContext ( ).getGeomManager ( ).getVertex (
															pointName, false);
		if (0 != entity)
			entities.push_back (entity);
	}	// if (0 != pointName.length ( ))
	if (0 != curveName.length ( ))
	{
		Entity*	entity	= getContext ( ).getGeomManager ( ).getCurve (
															curveName, false);
		if (0 != entity)
			entities.push_back (entity);
	}	// if (0 != curveName.length ( ))

	if (0 != surfaceName.length ( ))
	{
		Entity*	entity	= getContext ( ).getGeomManager ( ).getSurface (
				surfaceName, false);
		if (0 != entity)
			entities.push_back (entity);
	}	// if (0 != surfaceName.length ( ))

	return entities;
}	// QtVertexOperationPanel::getInvolvedEntities


void QtVertexOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _pointPanel)
		_pointPanel->stopSelection ( );
	if (0 != _vertexCurveProjectionPanel)
		_vertexCurveProjectionPanel->stopSelection ( );
	if (0 != _vertexSurfaceProjectionPanel)
		_vertexSurfaceProjectionPanel->stopSelection ( );
	if (0 != _curvilinearAbscissaPanel)
		_curvilinearAbscissaPanel->stopSelection ( );
	if (0 != _verticesRatioPanel)
        _verticesRatioPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtVertexOperationPanel::operationCompleted


void QtVertexOperationPanel::operationMethodCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_pointPanel)
	CHECK_NULL_PTR_ERROR (_vertexCurveProjectionPanel)
	CHECK_NULL_PTR_ERROR (_vertexSurfaceProjectionPanel)
	CHECK_NULL_PTR_ERROR (_curvilinearAbscissaPanel)
	CHECK_NULL_PTR_ERROR (_verticesRatioPanel)

	// Changement de méthode => on annule la mise en évidence des paramètres en
	// cours, puis on les mettra en évidence dans le panneau affiché :
	cancel ( );

	if (0 != _currentPanel)
	{
		_currentParentWidget->layout ( )->removeWidget (_currentPanel);
		_currentPanel->hide ( );
		_currentPanel->setParent (0);
	}
	_currentPanel	= 0;

	switch (getOperationMethod ( ))
	{
		case QtVertexOperationPanel::COORDINATES	:
			_currentPanel	= _pointPanel;					break;
		case QtVertexOperationPanel::VERTEX_PROJECTED_ON_CURVE	:
			_currentPanel	= _vertexCurveProjectionPanel;	break;
		case QtVertexOperationPanel::VERTEX_PROJECTED_ON_SURFACE	:
			_currentPanel	= _vertexSurfaceProjectionPanel;	break;
		case QtVertexOperationPanel::CURVILINEAR_ABSCISSA	:
			_currentPanel	= _curvilinearAbscissaPanel;	break;
	    case QtVertexOperationPanel::RATE_WITH_2_POINTS  :
	        _currentPanel   = _verticesRatioPanel;   break;
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode d'opération non supportée ("
			        << (unsigned long)getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtVertexOperationPanel::operationMethodCallback")
			throw exc;
		}
	}	// switch (getOperationMethod ( ))
	if (0 != _currentPanel)
	{
		_currentPanel->setParent (_currentParentWidget);
		_currentParentWidget->layout ( )->addWidget (_currentPanel);
		_currentPanel->show ( );
		if (0 != getMainWindow ( ))
			getMainWindow ( )->getOperationsPanel ( ).updateLayoutWorkaround( );
	}

	autoUpdate ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtVertexOperationPanel::operationMethodCallback


void QtVertexOperationPanel::preview (bool show, bool destroyInteractor)
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

		DisplayProperties	graphicalProps;
		graphicalProps.setCloudColor (Color (
				255 * Resources::instance ( )._previewColor.getRed ( ),
				255 * Resources::instance ( )._previewColor.getGreen ( ),
				255 * Resources::instance ( )._previewColor.getBlue ( )));
		graphicalProps.setPointSize (
						Resources::instance ( )._previewPointSize.getValue ( ));

		switch (getOperationMethod ( )) {
		case QtVertexOperationPanel::COORDINATES	:
		{
			Geom::CommandNewVertex command(*context, getPoint(), "");

			Geom::GeomDisplayRepresentation	dr (DisplayRepresentation::WIRE);
			command.getPreviewRepresentation (dr);
			const vector<Math::Point>&	points	= dr.getPoints ( );

			RenderingManager::RepresentationID	repID	=
					getRenderingManager ( ).createCloudRepresentation (
										points, graphicalProps, true);
			registerPreviewedObject (repID);

			getRenderingManager ( ).forceRender ( );
		}
		break;
		case QtVertexOperationPanel::VERTEX_PROJECTED_ON_CURVE	:
		{
			Geom::Curve *curve = getContext().getGeomManager().getCurve(getCurveName());
			Geom::Vertex *vertex = getContext().getGeomManager().getVertex(getProjectedVertexName());

			Geom::CommandNewVertexByProjection command(*context, vertex, curve, "");

			Geom::GeomDisplayRepresentation	dr (DisplayRepresentation::WIRE);
			command.getPreviewRepresentation (dr);
			const vector<Math::Point>&	points	= dr.getPoints ( );

			RenderingManager::RepresentationID	repID	=
					getRenderingManager ( ).createCloudRepresentation (
										points, graphicalProps, true);
			registerPreviewedObject (repID);

			getRenderingManager ( ).forceRender ( );
		}
		break;
		case QtVertexOperationPanel::VERTEX_PROJECTED_ON_SURFACE	:
		{
			Geom::Surface *surface = getContext().getGeomManager().getSurface(getSurfaceName());
			Geom::Vertex *vertex = getContext().getGeomManager().getVertex(getProjectedVertexName());

			Geom::CommandNewVertexByProjection command(*context, vertex, surface, "");

			Geom::GeomDisplayRepresentation	dr (DisplayRepresentation::WIRE);
			command.getPreviewRepresentation (dr);
			const vector<Math::Point>&	points	= dr.getPoints ( );

			RenderingManager::RepresentationID	repID	=
					getRenderingManager ( ).createCloudRepresentation (
										points, graphicalProps, true);
			registerPreviewedObject (repID);

			getRenderingManager ( ).forceRender ( );
		}
		break;
		case QtVertexOperationPanel::CURVILINEAR_ABSCISSA	:
		{
			Geom::Curve *curve = getContext().getGeomManager().getCurve(getCurveName());
			const double param = getCurvilinearAbscissa();

			Geom::CommandNewVertexByCurveParameterization command(*context,curve, param, "");

			Geom::GeomDisplayRepresentation	dr (DisplayRepresentation::WIRE);
			command.getPreviewRepresentation (dr);
			const vector<Math::Point>&	points	= dr.getPoints ( );

			RenderingManager::RepresentationID	repID	=
					getRenderingManager ( ).createCloudRepresentation (
										points, graphicalProps, true);
			registerPreviewedObject (repID);

			getRenderingManager ( ).forceRender ( );

		}
		break;
            case QtVertexOperationPanel::RATE_WITH_2_POINTS	:
            {
                Geom::Vertex *vtx1 = getContext().getGeomManager().getVertex(getFirstPointName());
                Geom::Vertex *vtx2 = getContext().getGeomManager().getVertex(getSecondPointName());
                Point p1 = vtx1->getPoint();
                Point p2 = vtx2->getPoint();
                if (p1 == p2){
                    TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message <<"newVertex impossible, entités "<<vtx1->getName()<<" et "<<vtx2->getName()<<" trop proches";
                    throw TkUtil::Exception(message);
                }
                const double param = getCurvilinearAbscissa();
                Point p = p1 + (p2-p1)*param;
                Geom::CommandNewVertex command(*context, p, "");

                Geom::GeomDisplayRepresentation	dr (DisplayRepresentation::WIRE);
                command.getPreviewRepresentation (dr);
                const vector<Math::Point>&	points	= dr.getPoints ( );

                RenderingManager::RepresentationID	repID	=
                        getRenderingManager ( ).createCloudRepresentation (
                                points, graphicalProps, true);
                registerPreviewedObject (repID);

                getRenderingManager ( ).forceRender ( );
            }
                break;
		default	:
		{
			std::cerr<<"Cas non implémenté. QtVertexOperationPanel::preview"<<std::endl;
		}
		}	// switch (getOperationMethod ( ))

	}
	catch (...)
	{
	}
}	// QtVertexOperationPanel::preview


// ===========================================================================
//                  LA CLASSE QtVertexOperationAction
// ===========================================================================

QtVertexOperationAction::QtVertexOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtVertexOperationPanel*	operationPanel	=
		new QtVertexOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy,
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtVertexOperationAction::QtVertexOperationAction


QtVertexOperationAction::QtVertexOperationAction (
										const QtVertexOperationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0, ""), "")
{
	MGX_FORBIDDEN ("QtVertexOperationAction copy constructor is not allowed.")
}	// QtVertexOperationAction::QtVertexOperationAction


QtVertexOperationAction& QtVertexOperationAction::operator = (
										const QtVertexOperationAction&)
{
	MGX_FORBIDDEN ("QtVertexOperationAction assignment operator is not allowed.")
	return *this;
}	// QtVertexOperationAction::operator =


QtVertexOperationAction::~QtVertexOperationAction ( )
{
}	// QtVertexOperationAction::~QtVertexOperationAction


QtVertexOperationPanel* QtVertexOperationAction::getVertexPanel ( )
{
	return dynamic_cast<QtVertexOperationPanel*>(getOperationPanel ( ));
}	// QtVertexOperationAction::getVertexPanel


void QtVertexOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	CommandResultIfc*	cmdResult	= 0;
//	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de création du vertex :
	QtVertexOperationPanel*	panel	= getVertexPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string	name	= panel->getGroupName ( );
	switch (getVertexPanel ( )->getOperationMethod ( ))
	{
		case QtVertexOperationPanel::COORDINATES	:
		{
			const Point	p (panel->getPoint ( ));
			cmdResult	= getContext ( ).getGeomManager ( ).newVertex (p, name);
		}
		break;
		case QtVertexOperationPanel::VERTEX_PROJECTED_ON_CURVE	:
		{
			const string	vertex	= panel->getProjectedVertexName ( );
			const string	curve	= panel->getCurveName ( );
			cmdResult	= getContext ( ).getGeomManager ( ).newVertex (vertex, curve, name);
		}
		break;
		case QtVertexOperationPanel::VERTEX_PROJECTED_ON_SURFACE	:
		{
			const string	vertex	= panel->getProjectedVertexName ( );
			const string	surface	= panel->getSurfaceName ( );
			cmdResult	= getContext ( ).getGeomManager ( ).newVertex (vertex, surface, name);
		}
		break;
		case QtVertexOperationPanel::CURVILINEAR_ABSCISSA	:
		{
			const string	curve	= panel->getCurveName ( );
			const double	t		= panel->getCurvilinearAbscissa ( );
			cmdResult	= getContext ( ).getGeomManager ( ).newVertex (curve, t, name);
		}
		break;
	    case QtVertexOperationPanel::RATE_WITH_2_POINTS  :
	    {
			const string	firstPoint	= panel->getFirstPointName ( );
			const string	secondPoint	= panel->getSecondPointName ( );
			const double	t		= panel->getCurvilinearAbscissa ( );
			cmdResult	= getContext ( ).getGeomManager ( ).newVertex (firstPoint, secondPoint, t, name);
		}
		break;
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode d'opération non supportée ("
			        << (unsigned long)getVertexPanel ( )->getOperationMethod ( )
			        << ").";
			INTERNAL_ERROR (exc, message, "QtVertexOperationAction::executeOperation")
			throw exc;
		}
		break;
	}	// switch (getVertexPanel ( )->getOperationMethod ( ))

	setCommandResult (cmdResult);
}	// QtVertexOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
