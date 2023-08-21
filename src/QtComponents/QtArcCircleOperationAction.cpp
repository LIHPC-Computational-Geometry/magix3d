/**
 * \file        QtArcCircleOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        08/09/2014
 */

#include "Internal/ContextIfc.h"
#include "Internal/Context.h"
#include "Internal/Resources.h"
#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "Geom/Vertex.h"
#include "Geom/CommandNewArcCircle.h"
#include "Geom/CommandNewArcCircleWithAngles.h"
#include "Geom/GeomDisplayRepresentation.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtArcCircleOperationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/UTF8String.h>
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
using namespace Mgx3D::CoordinateSystem;

namespace Mgx3D
{

namespace QtComponents
{

// ===========================================================================
//                        LA CLASSE QtCenterExtremitiesArcPanel
// ===========================================================================

QtCenterExtremitiesArcPanel::QtCenterExtremitiesArcPanel(QWidget *parent, QtMgx3DMainWindow &window, QtMgx3DOperationPanel *mainPanel)
		: QtMgx3DOperationsSubPanel(parent, window, mainPanel), _verticesPanel(0), _normalCheckBox(0), _normalPanel(0), _directionCheckBox(0)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	setLayout(layout);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(5);

	_verticesPanel = new Qt3VerticiesPanel(this, "Définition d'un arc de cercle par son centre et ses extrémités", window, FilterEntity::GeomVertex, true);
	connect(_verticesPanel, SIGNAL (pointAddedToSelection(QString)), this, SLOT (entitiesAddedToSelectionCallback(QString)));
	connect(_verticesPanel, SIGNAL (pointRemovedFromSelection(QString)), this, SLOT (entitiesRemovedFromSelectionCallback(QString)));
	connect(_verticesPanel, SIGNAL (pointAddedToSelection(QString)), this, SLOT (parametersModifiedCallback( )));
	connect(_verticesPanel, SIGNAL (pointRemovedFromSelection(QString)), this, SLOT (parametersModifiedCallback( )));
	layout->addWidget(_verticesPanel);

	// Faut-il définir la normale au plan contenant les points (3 points alignés) ?
	const string normalTip("La définition de la normale au plan est requise lorsque les 3 points sont alignés.");
	_normalCheckBox = new QCheckBox(QString::fromUtf8("Définir la normale au plan"), this);
	_normalCheckBox->setToolTip(QString::fromUtf8(normalTip.c_str()));
	layout->addWidget(_normalCheckBox);
	connect(_normalCheckBox, SIGNAL (stateChanged(int)), this, SLOT (useNormalModifiedCallback()));
	connect(_normalCheckBox, SIGNAL (stateChanged(int)), this, SLOT (parametersModifiedCallback()));
	_normalPanel = new QtMgx3DVectorPanel(this, "", true, "dx :", "dy :", "dz :", 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 1., -DBL_MAX, DBL_MAX, &window, FilterEntity::AllEdges, true);
	_normalPanel->setToolTip(QString::fromUtf8(normalTip.c_str()));
	connect(_normalPanel, SIGNAL (vectorModified()), this, SLOT (parametersModifiedCallback()));
	layout->addWidget(_normalPanel);

	// Le sens de rotation :
	QHBoxLayout *hLayout = new QHBoxLayout();
	layout->addLayout(hLayout);
	_directionCheckBox = new QCheckBox("Rotation en sens direct", this);
	connect(_directionCheckBox, SIGNAL (stateChanged(int)), this, SLOT (parametersModifiedCallback( )));
	hLayout->addWidget(_directionCheckBox);
	_directionCheckBox->setCheckState(Qt::Checked);

	useNormalModifiedCallback();
}    // QtCenterExtremitiesArcPanel::QtCenterExtremitiesArcPanel


QtCenterExtremitiesArcPanel::QtCenterExtremitiesArcPanel(const QtCenterExtremitiesArcPanel &p)
		: QtMgx3DOperationsSubPanel(p), _verticesPanel(0), _normalCheckBox(0), _normalPanel(0),_directionCheckBox(0)
{
	MGX_FORBIDDEN ("QtCenterExtremitiesArcPanel copy constructor is not allowed.");
}    // QtCenterExtremitiesArcPanel::QtCenterExtremitiesArcPanel


QtCenterExtremitiesArcPanel &QtCenterExtremitiesArcPanel::operator=(const QtCenterExtremitiesArcPanel &)
{
	MGX_FORBIDDEN ("QtCenterExtremitiesArcPanel assignment operator is not allowed.");
	return *this;
}    // QtCenterExtremitiesArcPanel::operator =


QtCenterExtremitiesArcPanel::~QtCenterExtremitiesArcPanel ( )
{
}    // QtCenterExtremitiesArcPanel::~QtCenterExtremitiesArcPanel


string QtCenterExtremitiesArcPanel::getStartVertexUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	return _verticesPanel->getStartVertexUniqueName();
}    // QtCenterExtremitiesArcPanel::getStartVertexUniqueName


string QtCenterExtremitiesArcPanel::getEndVertexUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	return _verticesPanel->getEndVertexUniqueName ( );
}    // QtCenterExtremitiesArcPanel::getEndVertexUniqueName


string QtCenterExtremitiesArcPanel::getCenterVertexUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	return _verticesPanel->getCenterVertexUniqueName ( );
}    // QtCenterExtremitiesArcPanel::getCenterVertexUniqueName


bool QtCenterExtremitiesArcPanel::defineNormal ( ) const
{
	CHECK_NULL_PTR_ERROR (_normalCheckBox)
	return Qt::Checked == _normalCheckBox->checkState ( ) ? true : false;
}    // QtCenterExtremitiesArcPanel::defineNormal


Math::Vector QtCenterExtremitiesArcPanel::getNormal ( ) const
{
	CHECK_NULL_PTR_ERROR (_normalPanel)
	return _normalPanel->getVector();
}    // QtCenterExtremitiesArcPanel::getNormal


bool QtCenterExtremitiesArcPanel::directOrientation ( ) const
{
	CHECK_NULL_PTR_ERROR (_directionCheckBox)
	return Qt::Checked == _directionCheckBox->checkState ( ) ? true : false;
}    // QtCenterExtremitiesArcPanel::directOrientation


void QtCenterExtremitiesArcPanel::parametersModifiedCallback ( )
{
	emit parametersModified ( );
}   // QtCenterExtremitiesArcPanel::parametersModifiedCallback()


void QtCenterExtremitiesArcPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_verticesPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel)
	_verticesPanel->reset ( );
	_normalPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtCenterExtremitiesArcPanel::reset


void QtCenterExtremitiesArcPanel::cancel ( )
{
	QtMgx3DOperationsSubPanel::cancel ( );

	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_verticesPanel)
	CHECK_NULL_PTR_ERROR (_normalPanel)
	_verticesPanel->stopSelection ( );
	_normalPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		_verticesPanel->cancel ( );
		_normalPanel->reset  ( );
	}	// if (true == cancelClearEntities ( ))

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}   // QtCenterExtremitiesArcPanel::cancel


void QtCenterExtremitiesArcPanel::useNormalModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_normalPanel)
	_normalPanel->setEnabled (defineNormal ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtCenterExtremitiesArcPanel::useNormalModifiedCallback


// ===========================================================================
//                        LA CLASSE QtAnglesSysCoordsArcPanel
// ===========================================================================

QtAnglesSysCoordsArcPanel::QtAnglesSysCoordsArcPanel (QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
		: QtMgx3DOperationsSubPanel (parent, window, mainPanel), _startTextField (0), _endTextField (0), _rayLengthTextField (0), _sysCoordPanel (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);

	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label	= new QLabel ("Angle de départ :", this);
	QSize   labelSize   = label->sizeHint ( );
	hlayout->addWidget (label);
	_startTextField	= &QtNumericFieldsFactory::createAngleTextField (this);
	connect(_startTextField, SIGNAL (textEdited (const QString&)), this, SLOT (parametersModifiedCallback()));
	hlayout->addWidget (_startTextField);
	hlayout->addWidget (new QLabel ("degrés", this));
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel ("Angle de fin    :", this);
	label->setFixedSize (labelSize);
	hlayout->addWidget (label);
	_endTextField	= &QtNumericFieldsFactory::createAngleTextField (this);
	connect(_endTextField, SIGNAL (textEdited (const QString&)), this, SLOT (parametersModifiedCallback()));
	hlayout->addWidget (_endTextField);
	hlayout->addWidget (new QLabel ("degrés", this));
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel ("Rayon :", this);
	hlayout->addWidget (label);
	_rayLengthTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_rayLengthTextField->setRange (0., NumericServices::doubleMachMax ( ));
	connect(_rayLengthTextField, SIGNAL (textEdited (const QString&)), this, SLOT (parametersModifiedCallback()));
	hlayout->addWidget (_rayLengthTextField);
	_sysCoordPanel	= new QtMgx3DEntityPanel (this, "", true, "Repère :", "", &window, SelectionManagerIfc::D3, FilterEntity::SysCoord);
	_sysCoordPanel->setMultiSelectMode (false);
	connect(_sysCoordPanel, SIGNAL (selectionModified (QString)), this, SLOT (parametersModifiedCallback()));
	layout->addWidget (_sysCoordPanel);
}	// QtAnglesSysCoordsArcPanel::QtAnglesSysCoordsArcPanel


QtAnglesSysCoordsArcPanel::QtAnglesSysCoordsArcPanel (const QtAnglesSysCoordsArcPanel& p)
	: QtMgx3DOperationsSubPanel (p), _startTextField (0), _endTextField (0), _rayLengthTextField (0), _sysCoordPanel (0)
{
	MGX_FORBIDDEN ("QtAnglesSysCoordsArcPanel copy constructor is not allowed.");
}	// QtAnglesSysCoordsArcPanel::QtAnglesSysCoordsArcPanel


QtAnglesSysCoordsArcPanel& QtAnglesSysCoordsArcPanel::operator = (const QtAnglesSysCoordsArcPanel&)
{
	MGX_FORBIDDEN ("QtAnglesSysCoordsArcPanel assignment operator is not allowed.");
	return *this;
}	// QtAnglesSysCoordsArcPanel::operator =


QtAnglesSysCoordsArcPanel::~QtAnglesSysCoordsArcPanel ( )
{
}	// QtAnglesSysCoordsArcPanel::~QtAnglesSysCoordsArcPanel


double QtAnglesSysCoordsArcPanel::getStartAngle ( ) const
{
	CHECK_NULL_PTR_ERROR (_startTextField)
	return _startTextField->getValue ( );
}	// QtAnglesSysCoordsArcPanel::getStartAngle


double QtAnglesSysCoordsArcPanel::getEndAngle ( ) const
{
	CHECK_NULL_PTR_ERROR (_endTextField)
	return _endTextField->getValue ( );
}	// QtAnglesSysCoordsArcPanel::getEndAngle

		
double QtAnglesSysCoordsArcPanel::getRayLength ( ) const
{
	CHECK_NULL_PTR_ERROR (_rayLengthTextField)
	return _rayLengthTextField->getValue ( );
}	// QtAnglesSysCoordsArcPanel::getRayLength


CoordinateSystem::SysCoord* QtAnglesSysCoordsArcPanel::getSysCoord ( ) const
{
	CHECK_NULL_PTR_ERROR (_sysCoordPanel)
	return dynamic_cast<CoordinateSystem::SysCoord*>(&getMainWindow ( )->getContext ( ).nameToEntity (_sysCoordPanel->getUniqueName ( )));
}   // QtAnglesSysCoordsArcPanel::getSysCoord


string QtAnglesSysCoordsArcPanel::getSysCoordName ( ) const
{
	CHECK_NULL_PTR_ERROR (_sysCoordPanel)
	return _sysCoordPanel->getUniqueName ( );
}   // QtAnglesSysCoordsArcPanel::getSysCoordName


void QtAnglesSysCoordsArcPanel::parametersModifiedCallback ( )
{
	emit parametersModified ( );
}   // QtAnglesSysCoordsArcPanel::parametersModifiedCallback()


void QtAnglesSysCoordsArcPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_startTextField)
	CHECK_NULL_PTR_ERROR (_endTextField)
	CHECK_NULL_PTR_ERROR (_rayLengthTextField)
	CHECK_NULL_PTR_ERROR (_sysCoordPanel)
	_startTextField->setValue (0.);
	_endTextField->setValue (0.);
	_rayLengthTextField->setValue (0.);
	_sysCoordPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtAnglesSysCoordsArcPanel::reset


void QtAnglesSysCoordsArcPanel::cancel ( )
{
	QtMgx3DOperationsSubPanel::cancel ( );

	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_startTextField)
	CHECK_NULL_PTR_ERROR (_endTextField)
	CHECK_NULL_PTR_ERROR (_rayLengthTextField)
	CHECK_NULL_PTR_ERROR (_sysCoordPanel)
	_sysCoordPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		_startTextField->setValue (0.);
		_endTextField->setValue (0.);
		_rayLengthTextField->setValue (0.);
		_sysCoordPanel->setUniqueName ("");
	}	// if (true == cancelClearEntities ( ))

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}   // QtAnglesSysCoordsArcPanel::cancel

		
// ===========================================================================
//                        LA CLASSE QtArcCircleOperationPanel
// ===========================================================================

QtArcCircleOperationPanel::QtArcCircleOperationPanel (
			QWidget* parent, const string& panelName,  QtMgx3DGroupNamePanel::POLICY creationPolicy, QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).arcCircleOperationURL, QtMgx3DApplication::HelpSystem::instance ( ).arcCircleOperationTag),
	  _namePanel (0), _operationMethodComboBox (0), _currentParentWidget (0), _currentPanel (0), _centerExtremitiesPanel (0), _anglesSysCoordsPanel (0)
{
//	SET_WIDGET_BACKGROUND (this, Qt::yellow)
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ), Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ), Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (layout);

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Nom groupe :
	_namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe", mainWindow, 1, creationPolicy, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// Méthode de création/modification de l'arc de cercle :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Méthode"), this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem (QString::fromUtf8("Par saisie du centre et des extrémités"));
	_operationMethodComboBox->addItem (QString::fromUtf8("Par saisie des angles et du repère"));
	connect (_operationMethodComboBox, SIGNAL (currentIndexChanged (int)), this, SLOT (operationMethodCallback ( )));
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);
	
	// Définition de l'arc de cercle :
	QtGroupBox*		groupBox	= new QtGroupBox(QString::fromUtf8("Paramètres de l'arc de cercle"), this);
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	vlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ), Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ), Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (vlayout);
	layout->addWidget (groupBox);
	// Le panneau "méthode" : plusieurs panneaux possibles.
	// On garde un parent unique dans la layout :
	_currentParentWidget	= new QWidget (groupBox);
	vlayout->addWidget (_currentParentWidget);
	QHBoxLayout*	currentLayout = new QHBoxLayout (_currentParentWidget);
	_currentParentWidget->setLayout (currentLayout);
	// Panneau centre/extrémités :
	_centerExtremitiesPanel	= new QtCenterExtremitiesArcPanel (0, mainWindow);
	connect (_centerExtremitiesPanel, SIGNAL (parametersModified ( )), this, SLOT (parametersModifiedCallback ( )));
	_centerExtremitiesPanel->hide ( );
	// Panneau angles/rayon/repère :
	_anglesSysCoordsPanel   = new QtAnglesSysCoordsArcPanel (0, mainWindow);
	connect (_anglesSysCoordsPanel, SIGNAL (parametersModified ( )), this, SLOT (parametersModifiedCallback ( )));
	_anglesSysCoordsPanel->hide ( );
	operationMethodCallback ( );

	addPreviewCheckBox (true);

	vlayout->addStretch (2);
}	// QtArcCircleOperationPanel::QtArcCircleOperationPanel


QtArcCircleOperationPanel::QtArcCircleOperationPanel (const QtArcCircleOperationPanel& cao)
	: QtMgx3DOperationPanel (0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _operationMethodComboBox (0), _currentParentWidget (0), _currentPanel (0), _centerExtremitiesPanel (0)
{
	MGX_FORBIDDEN ("QtArcCircleOperationPanel copy constructor is not allowed.");
}	// QtArcCircleOperationPanel::QtArcCircleOperationPanel (const QtArcCircleOperationPanel&)


QtArcCircleOperationPanel& QtArcCircleOperationPanel::operator = (const QtArcCircleOperationPanel&)
{
	MGX_FORBIDDEN ("QtArcCircleOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtArcCircleOperationPanel::QtArcCircleOperationPanel (const QtArcCircleOperationPanel&)


QtArcCircleOperationPanel::~QtArcCircleOperationPanel ( )
{
}	// QtArcCircleOperationPanel::~QtArcCircleOperationPanel


string QtArcCircleOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtArcCircleOperationPanel::getGroupName


QtArcCircleOperationPanel::OPERATION_METHOD QtArcCircleOperationPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtArcCircleOperationPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtArcCircleOperationPanel::getOperationMethod


string QtArcCircleOperationPanel::getStartVertexUniqueName ( ) const
{
	switch (getOperationMethod ( ))
	{
		case EXTREMITIES_CENTER :
			CHECK_NULL_PTR_ERROR (_centerExtremitiesPanel)
			return _centerExtremitiesPanel->getStartVertexUniqueName ( );
	}   // switch (getOperationMethod ( ))

	throw Exception ("QtArcCircleOperationPanel::getStartVertexUniqueName appellée hors contexte.");
}	// QtArcCircleOperationPanel::getStartVertexUniqueName


string QtArcCircleOperationPanel::getEndVertexUniqueName ( ) const
{
	switch (getOperationMethod ( ))
	{
		case EXTREMITIES_CENTER :
			CHECK_NULL_PTR_ERROR (_centerExtremitiesPanel)
			return _centerExtremitiesPanel->getEndVertexUniqueName ( );
	}   // switch (getOperationMethod ( ))

	throw Exception ("QtArcCircleOperationPanel::getEndVertexUniqueName appellée hors contexte.");
}	// QtArcCircleOperationPanel::getEndVertexUniqueName


string QtArcCircleOperationPanel::getCenterVertexUniqueName ( ) const
{
	switch (getOperationMethod ( ))
	{
		case EXTREMITIES_CENTER :
			CHECK_NULL_PTR_ERROR (_centerExtremitiesPanel)
			return _centerExtremitiesPanel->getCenterVertexUniqueName ( );
	}   // switch (getOperationMethod ( ))

	throw Exception ("QtArcCircleOperationPanel::getCenterVertexUniqueName appellée hors contexte.");
}	// QtArcCircleOperationPanel::getCenterVertexUniqueName


double QtArcCircleOperationPanel::getStartAngle ( ) const
{
	switch (getOperationMethod ( ))
	{
		case SYSCOORD_2_ANGLES :
			CHECK_NULL_PTR_ERROR (_anglesSysCoordsPanel)
			return _anglesSysCoordsPanel->getStartAngle ( );
	}   // switch (getOperationMethod ( ))

	throw Exception ("QtArcCircleOperationPanel::getStartAngle appellée hors contexte.");

	CHECK_NULL_PTR_ERROR (_anglesSysCoordsPanel)
}	// QtArcCircleOperationPanel::getStartAngle


double QtArcCircleOperationPanel::getEndAngle ( ) const
{
	switch (getOperationMethod ( ))
	{
		case SYSCOORD_2_ANGLES :
			CHECK_NULL_PTR_ERROR (_anglesSysCoordsPanel)
			return _anglesSysCoordsPanel->getEndAngle ( );
	}   // switch (getOperationMethod ( ))

	throw Exception ("QtArcCircleOperationPanel::getEndAngle appellée hors contexte.");

	CHECK_NULL_PTR_ERROR (_anglesSysCoordsPanel)
}	// QtArcCircleOperationPanel::getEndAngle


double QtArcCircleOperationPanel::getRayLength ( ) const
{
	CHECK_NULL_PTR_ERROR (_anglesSysCoordsPanel)
	return _anglesSysCoordsPanel->getRayLength ( );
}	// QtArcCircleOperationPanel::getRayLength


CoordinateSystem::SysCoord* QtArcCircleOperationPanel::getSysCoord ( ) const
{
	CHECK_NULL_PTR_ERROR (_anglesSysCoordsPanel)
	return _anglesSysCoordsPanel->getSysCoord ( );
}   // QtArcCircleOperationPanel::getSysCoord


std::string QtArcCircleOperationPanel::getSysCoordName ( ) const
{
	CHECK_NULL_PTR_ERROR (_anglesSysCoordsPanel)
	return _anglesSysCoordsPanel->getSysCoordName ( );
}	// QtArcCircleOperationPanel::getCenterVertexUniqueName


bool QtArcCircleOperationPanel::defineNormal ( ) const
{
	switch (getOperationMethod ( ))
	{
		case EXTREMITIES_CENTER :
			CHECK_NULL_PTR_ERROR (_centerExtremitiesPanel)
			return _centerExtremitiesPanel->defineNormal ( );
	}   // switch (getOperationMethod ( ))

	throw Exception ("QtArcCircleOperationPanel::defineNormal appellée hors contexte.");
}	// QtArcCircleOperationPanel::defineNormal


Math::Vector QtArcCircleOperationPanel::getNormal ( ) const
{
	switch (getOperationMethod ( ))
	{
		case EXTREMITIES_CENTER :
			CHECK_NULL_PTR_ERROR (_centerExtremitiesPanel)
			return _centerExtremitiesPanel->getNormal ( );
	}   // switch (getOperationMethod ( ))

	throw Exception ("QtArcCircleOperationPanel::getNormal appellée hors contexte.");
}	// QtArcCircleOperationPanel::getNormal


bool QtArcCircleOperationPanel::directOrientation ( ) const
{
	switch (getOperationMethod ( ))
	{
		case EXTREMITIES_CENTER :
			CHECK_NULL_PTR_ERROR (_centerExtremitiesPanel)
			return _centerExtremitiesPanel->directOrientation ( );
	}   // switch (getOperationMethod ( ))

	throw Exception ("QtArcCircleOperationPanel::directOrientation appellée hors contexte.");
}	// QtArcCircleOperationPanel::directOrientation


void QtArcCircleOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_currentPanel)
	CHECK_NULL_PTR_ERROR (_centerExtremitiesPanel)
	CHECK_NULL_PTR_ERROR (_anglesSysCoordsPanel)
	_namePanel->autoUpdate ( );
	_currentPanel->reset ( );
	_centerExtremitiesPanel->reset ( );
	_anglesSysCoordsPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtArcCircleOperationPanel::reset


void QtArcCircleOperationPanel::validate ( )
{
// CP : suite discussion EBL/FL, il est convenu que la validation des
// paramètres de l'opération est effectuée par le "noyau" et qu'un mauvais
// paramétrage est remonté sous forme d'exception à la fonction appelante, donc
// avant exécution de la commande.
// Les validations des valeurs des paramètres sont donc ici commentées.
	TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);

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
		error << "QtArcCircleOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtArcCircleOperationPanel::EXTREMITIES_CENTER	:
		case QtArcCircleOperationPanel::SYSCOORD_2_ANGLES	:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode d'opération de création/modification "
			      << "d'arc de cercle sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtArcCircleOperationPanel::validate : index de méthode "
			      << "d'opération de création/modification d'arc de cercle "
			      << "invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX ( ), "Abscisse point : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY ( ), "Ordonnée point : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ ( ), "Elévation point : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtArcCircleOperationPanel::validate


void QtArcCircleOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	if (0 != _currentPanel)
		_currentPanel->cancel ( );
}	// QtArcCircleOperationPanel::cancel


void QtArcCircleOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	_namePanel->autoUpdate ( );

	if (0 != _currentPanel)
		_currentPanel->autoUpdate ( );

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtArcCircleOperationPanel::autoUpdate


void QtArcCircleOperationPanel::preview (bool on, bool destroyInteractor)
{
	try
	{
		// Quoi qu'il arrive on détruit l'éventuel aperçu existant :
		removePreviewedObjects ( );
	}
	catch (...)
	{
	}

	if ((false == on) || (false == previewResult ( )))
		return;

	try
	{
		Context*		context		= dynamic_cast<Context*>(&getContext ( ));
		CHECK_NULL_PTR_ERROR (context)

		CommandCreateGeom*  command = 0;
		const string	    groupName = getGroupName ( );

		switch (_operationMethodComboBox->currentIndex ( ))
		{
			case QtArcCircleOperationPanel::EXTREMITIES_CENTER    :
			{
				// Pour la normale :
				if (true == defineNormal())
				{
					if (true == on)
					{
						DisplayProperties properties;
						properties.setWireColor(Color::create(255 * Resources::instance()._previewColor.getRed(),
								255 * Resources::instance()._previewColor.getGreen(), 255 * Resources::instance()._previewColor.getBlue()));
						properties.setLineWidth(Resources::instance()._previewWidth.getValue());
						Math::Point                        p1(0., 0., 0.);
						Math::Point                        p2(getNormal());
						RenderingManager::RepresentationID arrowID = getRenderingManager().createVector(p1, p2, properties, true);
						registerPreviewedObject(arrowID);
					}    // if (true == on)
				}    // if (true == defineNormal ( ))

				// pour l'arc de courbe
				const Vector        normal   = getNormal();
				const bool          direct   = directOrientation();
				Vertex              *pc      = context->getLocalGeomManager().getVertex(getCenterVertexUniqueName());
				Vertex              *pd      = context->getLocalGeomManager().getVertex(getStartVertexUniqueName());
				Vertex              *pe      = context->getLocalGeomManager().getVertex(getEndVertexUniqueName());
				command = new CommandNewArcCircle (*context, pc, pd, pe, direct, normal, groupName);
				break;
			}   // case QtArcCircleOperationPanel::EXTREMITIES_CENTER
			case QtArcCircleOperationPanel::SYSCOORD_2_ANGLES	:
			{
				const double                        start       = getStartAngle ( );
				const double                        end         = getEndAngle ( );
				const double                        rayLength   = getRayLength ( );
				CoordinateSystem::SysCoord*         sysCoord    = getSysCoord ( );
				command                     = new CommandNewArcCircleWithAngles (*context, start, end, rayLength, sysCoord, groupName);
				break;
			}   // case QtArcCircleOperationPanel::SYSCOORD_2_ANGLES	:
		}   // switch (_operationMethodComboBox->currentIndex ( ))

		CHECK_NULL_PTR_ERROR (command)
		DisplayProperties	graphicalProps;
		graphicalProps.setWireColor (Color (255 * Resources::instance ( )._previewColor.getRed ( ), 255 * Resources::instance ( )._previewColor.getGreen ( ), 255 * Resources::instance ( )._previewColor.getBlue ( )));
		graphicalProps.setLineWidth (Resources::instance ( )._previewWidth.getValue ( ));
		Geom::GeomDisplayRepresentation dr (DisplayRepresentation::WIRE);
		command->getPreviewRepresentation (dr);
		const vector<Math::Point> &points  = dr.getPoints();
		const vector<size_t>      &indices = dr.getCurveDiscretization();

		RenderingManager::RepresentationID repID = getRenderingManager().createSegmentsWireRepresentation (points, indices, graphicalProps, true);
		registerPreviewedObject(repID);

		delete command;

	}
	catch (...)
	{
	}
}	// QtArcCircleOperationPanel::preview


vector<Entity*> QtArcCircleOperationPanel::getInvolvedEntities ( )
{
	vector<Entity*>		entities;

	if (0 != _currentPanel)
		entities	= _currentPanel->getPanelEntities ( );

	return entities;
}	// QtArcCircleOperationPanel::getInvolvedEntities


void QtArcCircleOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _centerExtremitiesPanel)
		_centerExtremitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtArcCircleOperationPanel::operationCompleted


void QtArcCircleOperationPanel::operationMethodCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_centerExtremitiesPanel)
	if (0 != _currentPanel)
	{
		preview (false, true);
		highlight (false);
		_currentParentWidget->layout ( )->removeWidget (_currentPanel);
		_currentPanel->hide ( );
		_currentPanel->setParent (0);
	}
	_currentPanel	= 0;

	switch (getOperationMethod ( ))
	{
		case QtArcCircleOperationPanel::EXTREMITIES_CENTER				: _currentPanel	= _centerExtremitiesPanel;  break;
		case QtArcCircleOperationPanel::SYSCOORD_2_ANGLES				: _currentPanel	= _anglesSysCoordsPanel;    break;
		default	:
		{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Méthode d'opération non supportée (" << (unsigned long)getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtArcCircleOperationPanel::operationMethodCallback")
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

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtArcCircleOperationPanel::operationMethodCallback


// ===========================================================================
//                  LA CLASSE QtArcCircleOperationAction
// ===========================================================================

QtArcCircleOperationAction::QtArcCircleOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow, const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtArcCircleOperationPanel*	operationPanel	= new QtArcCircleOperationPanel (&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy, mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtArcCircleOperationAction::QtArcCircleOperationAction


QtArcCircleOperationAction::QtArcCircleOperationAction (const QtArcCircleOperationAction&)
	: QtMgx3DGeomOperationAction (QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtArcCircleOperationAction copy constructor is not allowed.")
}	// QtArcCircleOperationAction::QtArcCircleOperationAction


QtArcCircleOperationAction& QtArcCircleOperationAction::operator = (const QtArcCircleOperationAction&)
{
	MGX_FORBIDDEN ("QtArcCircleOperationAction assignment operator is not allowed.")
	return *this;
}	// QtArcCircleOperationAction::operator =


QtArcCircleOperationAction::~QtArcCircleOperationAction ( )
{
}	// QtArcCircleOperationAction::~QtArcCircleOperationAction


QtArcCircleOperationPanel* QtArcCircleOperationAction::getArcCirclePanel ( )
{
	return dynamic_cast<QtArcCircleOperationPanel*>(getOperationPanel ( ));
}	// QtArcCircleOperationAction::getArcCirclePanel


void QtArcCircleOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;

//	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de création de l'arc de cercle :
	QtArcCircleOperationPanel*	panel	= getArcCirclePanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string	name	= panel->getGroupName ( );
	switch (getArcCirclePanel ( )->getOperationMethod ( ))
	{
		case QtArcCircleOperationPanel::EXTREMITIES_CENTER				:
		{
			const string	            vertex1	        = getArcCirclePanel ( )->getStartVertexUniqueName ( );
			const string	            vertex2	        = getArcCirclePanel ( )->getEndVertexUniqueName ( );
			const string	            vertex3	        = getArcCirclePanel ( )->getCenterVertexUniqueName ( );
			const bool		            direct	        = getArcCirclePanel ( )->directOrientation ( );
			const Utils::Math::Vector   normal	        = getArcCirclePanel ( )->getNormal ( );
			const bool		            defineNormal	= getArcCirclePanel ( )->defineNormal ( );
			if (true == defineNormal)
				cmdResult	= getContext ( ).getGeomManager ( ).newArcCircle (vertex3, vertex1, vertex2, direct, normal, name);
			else
				cmdResult	= getContext ( ).getGeomManager ( ).newArcCircle (vertex3, vertex1, vertex2, direct, name);
		}
		break;
		case QtArcCircleOperationPanel::SYSCOORD_2_ANGLES	:
		{
			const double    start       = getArcCirclePanel ( )->getStartAngle ( );
			const double    end         = getArcCirclePanel ( )->getEndAngle ( );
			const double    rayLength   = getArcCirclePanel ( )->getRayLength ( );
			const string    sysCoordName= getArcCirclePanel ( )->getSysCoordName ( );
			cmdResult   = getContext ( ).getGeomManager ( ).newArcCircle (start, end, rayLength, sysCoordName, name);
		}   // case QtArcCircleOperationPanel::SYSCOORD_2_ANGLES	:
		break;
		default	:
		{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Méthode d'opération non supportée (" << (unsigned long)getArcCirclePanel ( )->getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtArcCircleOperationAction::executeOperation")
			throw exc;
		}
		break;
	}	// switch (getArcCirclePanel ( )->getOperationMethod ( ))
	
	setCommandResult (cmdResult);
}	// QtArcCircleOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
