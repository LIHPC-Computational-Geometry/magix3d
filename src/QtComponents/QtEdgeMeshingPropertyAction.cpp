/**
 * \file        QtEdgeMeshingPropertyAction.cpp
 * \author      Charles PIGNEROL
 * \date        04/11/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtEdgeMeshingPropertyAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <values.h>	// DBL_MAX
#include <TkUtil/TraceLog.h>


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
//                   LA CLASSE QtDiscretisationPanelIfc
// ===========================================================================

QtDiscretisationPanelIfc::QtDiscretisationPanelIfc (
									QWidget* parent, QtMgx3DMainWindow& window)
	: QtMgx3DOperationsSubPanel (parent, window),
	  _orthogonalOptionsPanel (0), _orthogonalCheckBox (0),
	  _directionCheckBox (0), _layersNumTextField (0),
	  _polarOptionsPanel (0), _polarCheckBox(0), _polarCenterPanel(0)
{
}	// QtDiscretisationPanelIfc::QtDiscretisationPanelIfc


QtDiscretisationPanelIfc::QtDiscretisationPanelIfc (
										const QtDiscretisationPanelIfc& p)
	  : QtMgx3DOperationsSubPanel (
						0, *new QtMgx3DMainWindow (0)),
	  _orthogonalOptionsPanel (0), _orthogonalCheckBox (0),
	  _directionCheckBox (0), _layersNumTextField (0),
	  _polarOptionsPanel (0), _polarCheckBox(0), _polarCenterPanel(0)
{
	MGX_FORBIDDEN ("QtDiscretisationPanelIfc copy constructor is not allowed.");
}	// QtDiscretisationPanelIfc::QtDiscretisationPanelIfc


QtDiscretisationPanelIfc& QtDiscretisationPanelIfc::operator = (
											const QtDiscretisationPanelIfc&)
{
	 MGX_FORBIDDEN ("QtDiscretisationPanelIfc assignment operator is not allowed.");
	return *this;
}	// QtDiscretisationPanelIfc::operator =


QtDiscretisationPanelIfc::~QtDiscretisationPanelIfc ( )
{
}	// QtDiscretisationPanelIfc::~QtDiscretisationPanelIfc


void QtDiscretisationPanelIfc::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _polarCenterPanel)
		_polarCenterPanel->reset ( );
	if (0 != _layersNumTextField)
		_layersNumTextField->setValue (5);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationsSubPanel::reset ( );
}	// QtDiscretisationPanelIfc::reset


void QtDiscretisationPanelIfc::setMeshingProperty (
										const CoEdgeMeshingProperty& property)
{
	if (0 != _orthogonalCheckBox)
		_orthogonalCheckBox->setCheckState (
				true == property.isOrthogonal( ) ? Qt::Checked : Qt::Unchecked);
	if (0 != _layersNumTextField && property.isOrthogonal( ))
		_layersNumTextField->setValue (property.getNbLayers ( ));
	if (0 != _directionCheckBox && property.isOrthogonal( ))
		_directionCheckBox->setCheckState (
				0 == property.getSide ( ) ?  Qt::Checked : Qt::Unchecked);
	if (0 != _orthogonalOptionsPanel)
		_orthogonalOptionsPanel->setVisible (property.isOrthogonal ( ));
	if (0 != _polarCheckBox)
		_polarCheckBox->setCheckState (
				true == property.isPolarCut ( ) ? Qt::Checked : Qt::Unchecked);
	if (0 != _polarOptionsPanel){
		_polarOptionsPanel->setVisible (property.isPolarCut ( ));
		if (0 != _polarCenterPanel && property.isPolarCut ( )){
			Utils::Math::Point pt = property.getPolarCenter();
			_polarCenterPanel->setX(pt.getX());
			_polarCenterPanel->setY(pt.getY());
			_polarCenterPanel->setZ(pt.getZ());
		}
	}
}	// QtDiscretisationPanelIfc::setMeshingProperty


void QtDiscretisationPanelIfc::updateProperty (
										CoEdgeMeshingProperty& property) const
{
	if (isOrthogonalToSide())
		property.setOrthogonal (getNbLayers ( ), getDirection ( ));
}	// QtDiscretisationPanelIfc::updateProperty


bool QtDiscretisationPanelIfc::isOrthogonalToSide ( ) const
{
	CHECK_NULL_PTR_ERROR (_orthogonalCheckBox)
	return Qt::Checked == _orthogonalCheckBox->checkState ( ) ? true : false;
}	// QtDiscretisationPanelIfc::isOrthogonalToSide


size_t QtDiscretisationPanelIfc::getNbLayers ( ) const
{
	CHECK_NULL_PTR_ERROR (_layersNumTextField)
	return _layersNumTextField->getValue ( );
}	// QtDiscretisationPanelIfc::getNbLayers


bool QtDiscretisationPanelIfc::getDirection ( ) const
{
	CHECK_NULL_PTR_ERROR (_directionCheckBox)
	return Qt::Checked == _directionCheckBox->checkState ( ) ? true : false;
}	// QtDiscretisationPanelIfc::getDirection


bool QtDiscretisationPanelIfc::isPolarCut ( ) const
{
	CHECK_NULL_PTR_ERROR (_polarCheckBox)
	return Qt::Checked == _polarCheckBox->checkState ( ) ? true : false;
}	// QtDiscretisationPanelIfc::isPolarCut


Utils::Math::Point QtDiscretisationPanelIfc::getPolarCenter ( ) const
{
	CHECK_NULL_PTR_ERROR (_polarCenterPanel);
	return _polarCenterPanel->getPoint();
}   // QtDiscretisationPanelIfc::getPolarCenter


void QtDiscretisationPanelIfc::createOrthogonalityArea (QLayout& layout)
{
	if (0 != _orthogonalOptionsPanel)
	{
		INTERNAL_ERROR (exc, "Widgets déjà créés.", "QtDiscretisationPanelIfc::createOrthogonalityArea")
		throw exc;
	}	// if (0 != _orthogonalOptionsPanel)

	// L'option "orthogonale à la paroi" :
	_orthogonalCheckBox	= new QCheckBox (QString::fromUtf8("Orthogonale à la paroi"), this);
	layout.addWidget (_orthogonalCheckBox);
	connect (_orthogonalCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (orthogonalityModifiedCallback ( )));
	_orthogonalOptionsPanel	= new QWidget (this);
	layout.addWidget (_orthogonalOptionsPanel);
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	_orthogonalOptionsPanel->setLayout (hlayout);
	QLabel*	label	=
				new QLabel ("Nombre de couches :", _orthogonalOptionsPanel);
	hlayout->addWidget (label);
	_layersNumTextField	= new QtIntTextField (5, _orthogonalOptionsPanel);
	_layersNumTextField->setRange (0, NumericServices::intMachMax ( ));
	hlayout->addWidget (_layersNumTextField);
	connect (_layersNumTextField, SIGNAL (editingFinished ( )), this,
		         SLOT (discretisationModifiedCallback ( )));
	_directionCheckBox	= new QCheckBox (": Sens", _orthogonalOptionsPanel);
	hlayout->addWidget (_directionCheckBox);
	connect (_directionCheckBox, SIGNAL (stateChanged (int)), this,
		         SLOT (discretisationModifiedCallback ( )));

	_orthogonalOptionsPanel->setVisible (
					Qt::Checked == _orthogonalCheckBox->checkState ( ));
}	// QtDiscretisationPanelIfc::createOrthogonalityArea


void QtDiscretisationPanelIfc::createPolarArea (QLayout& layout)
{
	if (0 != _polarOptionsPanel)
	{
		INTERNAL_ERROR (exc, "Widgets déjà créés.", "QtDiscretisationPanelIfc::createPolarArea")
					throw exc;
	}	// if (0 != _polarOptionsPanel)

	// L'option "découpage polaire" :
	_polarCheckBox	= new QCheckBox (QString::fromUtf8("Découpage polaire"), this);
	layout.addWidget (_polarCheckBox);
	connect (_polarCheckBox, SIGNAL (stateChanged (int)), this,
			SLOT (polarModifiedCallback ( )));

	_polarOptionsPanel	= new QWidget (this);
	layout.addWidget (_polarOptionsPanel);
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	_polarOptionsPanel->setLayout (hlayout);

	_polarCenterPanel = new QtMgx3DPointPanel (
			_polarOptionsPanel, "", true, "x :", "y :", "z :",
			0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
			getMainWindow(),
			(FilterEntity::objectType)(FilterEntity::GeomVertex | FilterEntity::TopoVertex), true);
	hlayout->addWidget (_polarCenterPanel);
	connect (_polarCenterPanel, SIGNAL (pointModified ( )),
		         this, SLOT (orthogonalityModifiedCallback ( )));
	_polarCenterPanel->layout ( )->setSpacing (5);
	_polarCenterPanel->layout ( )->setContentsMargins (0, 0, 0, 0);
	_polarCenterPanel->show();

	_polarOptionsPanel->setVisible (
			Qt::Checked == _polarCheckBox->checkState ( ));

}   // QtDiscretisationPanelIfc::createPolarArea


void QtDiscretisationPanelIfc::orthogonalityModifiedCallback ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_orthogonalCheckBox)
		discretisationModifiedCallback ( );

		_orthogonalOptionsPanel->setVisible (
					Qt::Checked == _orthogonalCheckBox->checkState ( ));
	}
	catch (...)
	{
	}
}	// QtDiscretisationPanelIfc::orthogonalityModifiedCallback


void QtDiscretisationPanelIfc::polarModifiedCallback ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_polarCheckBox)
		discretisationModifiedCallback ( );

		_polarOptionsPanel->setVisible (
					Qt::Checked == _polarCheckBox->checkState ( ));
	}
	catch (...)
	{
	}
}	// QtDiscretisationPanelIfc::polarModifiedCallback



// ===========================================================================
//                 LA CLASSE QtUniformDiscretisationPanel
// ===========================================================================


QtUniformDiscretisationPanel::QtUniformDiscretisationPanel (
					QWidget* parent, QtMgx3DMainWindow& window)
	: QtDiscretisationPanelIfc (parent, window)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (layout);

	// L'orthogonalité à la paroi :
	createOrthogonalityArea (*layout);

	// Le découpage en mode polaire
	createPolarArea (*layout);

}	// QtUniformDiscretisationPanel::QtUniformDiscretisationPanel


QtUniformDiscretisationPanel::QtUniformDiscretisationPanel (
										const QtUniformDiscretisationPanel& p)
	: QtDiscretisationPanelIfc (
			0, *new QtMgx3DMainWindow(0))
{
	MGX_FORBIDDEN ("QtUniformDiscretisationPanel copy constructor is not allowed.");
}	// QtUniformDiscretisationPanel::QtUniformDiscretisationPanel


QtUniformDiscretisationPanel& QtUniformDiscretisationPanel::operator = (
											const QtUniformDiscretisationPanel&)
{
	 MGX_FORBIDDEN ("QtUniformDiscretisationPanel assignment operator is not allowed.");
	return *this;
}	// QtUniformDiscretisationPanel::operator =


QtUniformDiscretisationPanel::~QtUniformDiscretisationPanel ( )
{
}	// QtUniformDiscretisationPanel::~QtUniformDiscretisationPanel


void QtUniformDiscretisationPanel::setMeshingProperty (
											const CoEdgeMeshingProperty& cemp)
{
	QtDiscretisationPanelIfc::setMeshingProperty(cemp);

}	// QtUniformDiscretisationPanel::setMeshingProperty


EdgeMeshingPropertyUniform*
					QtUniformDiscretisationPanel::getMeshingProperty (size_t edgeNum) const
{
	EdgeMeshingPropertyUniform*	property = 0;
	if (isPolarCut())
		property = new EdgeMeshingPropertyUniform (edgeNum, getPolarCenter ( ));
	else
		property = new EdgeMeshingPropertyUniform (edgeNum);
	updateProperty (*property);

	return property;
}	// QtUniformDiscretisationPanel::getMeshingProperty


// ===========================================================================
//                 LA CLASSE QtGeometricProgressionPanel
// ===========================================================================


QtGeometricProgressionPanel::QtGeometricProgressionPanel (
	QWidget* parent, double factor, QtMgx3DMainWindow& window)
	: QtDiscretisationPanelIfc (parent, window),
	  _factorTextField (0), _orientationCheckBox (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (layout);

	// Le facteur de la progression géométrique :
	QHBoxLayout* hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel* label	= new QLabel ("Raison :", this);
	hlayout->addWidget (label);
	_factorTextField	= &QtNumericFieldsFactory::createRatioTextField (this);
	_factorTextField->setValue (1.1);
	hlayout->addWidget (_factorTextField);
	connect (_factorTextField, SIGNAL (editingFinished ( )), this,
	         SLOT (discretisationModifiedCallback ( )));

	// Le sens de la discrétisation :
	_orientationCheckBox	= new QCheckBox ("Inverser le sens", this);
	layout->addWidget (_orientationCheckBox);
	connect (_orientationCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (discretisationModifiedCallback ( )));

	// L'orthogonalité à la paroi :
	createOrthogonalityArea (*layout);

	// Le découpage en mode polaire
	createPolarArea (*layout);

}	// QtGeometricProgressionPanel::QtGeometricProgressionPanel


QtGeometricProgressionPanel::QtGeometricProgressionPanel (
										const QtGeometricProgressionPanel& p)
	: QtDiscretisationPanelIfc (
		0, *new QtMgx3DMainWindow(0)),
	  _factorTextField (0), _orientationCheckBox (0)
{
	MGX_FORBIDDEN ("QtGeometricProgressionPanel copy constructor is not allowed.");
}	// QtGeometricProgressionPanel::QtGeometricProgressionPanel


QtGeometricProgressionPanel&
	QtGeometricProgressionPanel::operator = (const QtGeometricProgressionPanel&)
{
	 MGX_FORBIDDEN ("QtGeometricProgressionPanel assignment operator is not allowed.");
	return *this;
}	// QtGeometricProgressionPanel::operator =


QtGeometricProgressionPanel::~QtGeometricProgressionPanel ( )
{
}	// QtGeometricProgressionPanel::~QtGeometricProgressionPanel


void QtGeometricProgressionPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_factorTextField)
	_factorTextField->setValue (1.1);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtDiscretisationPanelIfc::reset ( );
}	// QtGeometricProgressionPanel::reset


void QtGeometricProgressionPanel::setMeshingProperty (
											const CoEdgeMeshingProperty& cemp)
{
	CHECK_NULL_PTR_ERROR (_factorTextField)
	CHECK_NULL_PTR_ERROR (_orientationCheckBox)

	const EdgeMeshingPropertyGeometric*	properties	=
					dynamic_cast<const EdgeMeshingPropertyGeometric*>(&cemp);
	if (0 == properties)
	{
		INTERNAL_ERROR (exc, "Les propriétés de maillage ne sont pas du type EdgeMeshingPropertyGeometric.", "QtGeometricProgressionPanel::setMeshingProperty")
		throw exc;
	}	// if (0 == properties)

	_factorTextField->setValue (properties->getRatio ( ));
	_orientationCheckBox->setCheckState (
			true == properties->getDirect ( ) ? Qt::Unchecked : Qt::Checked);
	QtDiscretisationPanelIfc::setMeshingProperty(cemp);

}	// QtGeometricProgressionPanel::setMeshingProperty


double QtGeometricProgressionPanel::getFactor ( ) const
{
	CHECK_NULL_PTR_ERROR (_factorTextField)
	return _factorTextField->getValue ( );
}	// QtGeometricProgressionPanel::getFactor


bool QtGeometricProgressionPanel::invertOrientation ( ) const
{
	CHECK_NULL_PTR_ERROR (_orientationCheckBox)
	return Qt::Checked == _orientationCheckBox->checkState ( ) ? true : false;
}	// QtGeometricProgressionPanel::invertOrientation


EdgeMeshingPropertyGeometric*
					QtGeometricProgressionPanel::getMeshingProperty ( size_t edgeNum ) const
{
	EdgeMeshingPropertyGeometric*	emp	= 0;
	if (isPolarCut())
		emp = new EdgeMeshingPropertyGeometric (
				edgeNum, getFactor ( ), getPolarCenter ( ),
				!invertOrientation ( ));
	else
		emp = new EdgeMeshingPropertyGeometric (
				edgeNum, getFactor ( ),
				!invertOrientation ( ));
	updateProperty (*emp);

	return emp;
}	// QtGeometricProgressionPanel::getMeshingProperty


// ===========================================================================
//                    LA CLASSE QtSpecificSizePanel
// ===========================================================================


QtSpecificSizePanel::QtSpecificSizePanel (
	QWidget* parent, double size, QtMgx3DMainWindow& window)
	: QtDiscretisationPanelIfc (parent, window),
	  _sizeTextField (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (layout);

	// Le facteur de la progression géométrique :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label	= new QLabel ("Longueur des segments :", this);
	hlayout->addWidget (label);
	_sizeTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_sizeTextField->setValue (size);
	hlayout->addWidget (_sizeTextField);
	connect (_sizeTextField, SIGNAL (editingFinished ( )), this,
	         SLOT (discretisationModifiedCallback ( )));

	// L'orthogonalité à la paroi :
	createOrthogonalityArea (*layout);
}	// QtSpecificSizePanel::QtSpecificSizePanel


QtSpecificSizePanel::QtSpecificSizePanel (const QtSpecificSizePanel& p)
	: QtDiscretisationPanelIfc (
						0, *new QtMgx3DMainWindow(0)),
	  _sizeTextField (0)
{
	MGX_FORBIDDEN ("QtSpecificSizePanel copy constructor is not allowed.");
}	// QtSpecificSizePanel::QtSpecificSizePanel


QtSpecificSizePanel& QtSpecificSizePanel::operator =(const QtSpecificSizePanel&)
{
	 MGX_FORBIDDEN ("QtSpecificSizePanel assignment operator is not allowed.");
	return *this;
}	// QtSpecificSizePanel::operator =


QtSpecificSizePanel::~QtSpecificSizePanel ( )
{
}	// QtSpecificSizePanel::~QtSpecificSizePanel


void QtSpecificSizePanel::setMeshingProperty (const CoEdgeMeshingProperty& cemp)
{
	CHECK_NULL_PTR_ERROR (_sizeTextField)

	const EdgeMeshingPropertySpecificSize*	properties	=
					dynamic_cast<const EdgeMeshingPropertySpecificSize*>(&cemp);
	if (0 == properties)
	{
		INTERNAL_ERROR (exc, "Les propriétés de maillage ne sont pas du type EdgeMeshingPropertySpecificSize.", "QtSpecificSizePanel::setMeshingProperty")
		throw exc;
	}	// if (0 == properties)

	_sizeTextField->setValue (properties->getEdgeSize ( ));
	QtDiscretisationPanelIfc::setMeshingProperty(cemp);

}	// QtSpecificSizePanel::setMeshingProperty


double QtSpecificSizePanel::getSize ( ) const
{
	CHECK_NULL_PTR_ERROR (_sizeTextField)
	return _sizeTextField->getValue ( );
}	// QtSpecificSizePanel::getSize


EdgeMeshingPropertySpecificSize*
					QtSpecificSizePanel::getMeshingProperty ( ) const
{
	EdgeMeshingPropertySpecificSize*	emp	=
			 new EdgeMeshingPropertySpecificSize (getSize ( ));
	updateProperty (*emp);

	return emp;
}	// QtSpecificSizePanel::getMeshingProperty


// ===========================================================================
//             LA CLASSE QtInterpolatedEdgeDiscretisationPanel
// ===========================================================================


QtInterpolatedEdgeDiscretisationPanel::QtInterpolatedEdgeDiscretisationPanel (
	QWidget* parent, const string& edgeName,
	QtMgx3DMainWindow& window)
	: QtDiscretisationPanelIfc (parent, window),
	  _referenceComboBox (0), _refEntitiesTextField (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (layout);

	// L'arête de référence :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	_referenceComboBox	= new QComboBox (this);
	_referenceComboBox->addItem (QString::fromUtf8("Arêtes de références"));
	_referenceComboBox->addItem (QString::fromUtf8("Face de référence"));
	_referenceComboBox->setCurrentIndex (1);
	connect (_referenceComboBox, SIGNAL (currentIndexChanged (int)), this,
	         SLOT (refEntitiesCallback ( )));
	hlayout->addWidget (_referenceComboBox);
	_refEntitiesTextField	= new QtEntityIDTextField (
		this, &window, SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	_refEntitiesTextField->setMultiSelectMode (true);
	hlayout->addWidget (_refEntitiesTextField);
	connect (_refEntitiesTextField, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesSelectedCallback (QString)));
	connect (_refEntitiesTextField,
	         SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesUnselectedCallback (QString)));

	// L'orthogonalité à la paroi :
	createOrthogonalityArea (*layout);

	refEntitiesCallback ( );	// Actualisation types d'entités de référence
}	// QtInterpolatedEdgeDiscretisationPanel::QtInterpolatedEdgeDiscretisationPanel


QtInterpolatedEdgeDiscretisationPanel::QtInterpolatedEdgeDiscretisationPanel (
								const QtInterpolatedEdgeDiscretisationPanel& p)
	: QtDiscretisationPanelIfc (
						0, *new QtMgx3DMainWindow(0)),
	  _referenceComboBox (0), _refEntitiesTextField (0)
{
	MGX_FORBIDDEN ("QtInterpolatedEdgeDiscretisationPanel copy constructor is not allowed.");
}	// QtInterpolatedEdgeDiscretisationPanel::QtInterpolatedEdgeDiscretisationPanel


QtInterpolatedEdgeDiscretisationPanel&
	QtInterpolatedEdgeDiscretisationPanel::operator = (
								const QtInterpolatedEdgeDiscretisationPanel&)
{
	 MGX_FORBIDDEN ("QtInterpolatedEdgeDiscretisationPanel assignment operator is not allowed.");
	return *this;
}	// QtInterpolatedEdgeDiscretisationPanel::operator =


QtInterpolatedEdgeDiscretisationPanel::~QtInterpolatedEdgeDiscretisationPanel ( )
{
}	// QtInterpolatedEdgeDiscretisationPanel::~QtInterpolatedEdgeDiscretisationPanel


void QtInterpolatedEdgeDiscretisationPanel::setMeshingProperty (
											const CoEdgeMeshingProperty& cemp)
{
	CHECK_NULL_PTR_ERROR (_refEntitiesTextField)
	CHECK_NULL_PTR_ERROR (_referenceComboBox)

	const EdgeMeshingPropertyInterpolate*	properties	=
					dynamic_cast<const EdgeMeshingPropertyInterpolate*>(&cemp);
	if (0 == properties)
	{
		INTERNAL_ERROR (exc, "Les propriétés de maillage ne sont pas du type EdgeMeshingPropertyInterpolate.",
				"QtInterpolatedEdgeDiscretisationPanel::setMeshingProperty")
		throw exc;
	}	// if (0 == properties)

	switch (properties->getType ( ))
	{
		case EdgeMeshingPropertyInterpolate::with_coedge_list	:
			_referenceComboBox->setCurrentIndex (0);
			_refEntitiesTextField->setUniqueNames (properties->getCoEdges ( ));
			break;
		case EdgeMeshingPropertyInterpolate::with_coface		:
		{
			vector<string>	names;
			names.push_back (properties->getCoFace ( ));
			_referenceComboBox->setCurrentIndex (1);
			_refEntitiesTextField->setUniqueNames (names);
		}
		break;
		default													:
		{
			INTERNAL_ERROR (exc,
				"Le type d'entité de référence n'est pas supporté.",
				"QtInterpolatedEdgeDiscretisationPanel::setMeshingProperty")
			throw exc;
		}	
	}	// switch (properties->getType ( ))

	QtDiscretisationPanelIfc::setMeshingProperty(cemp);

}	// QtInterpolatedEdgeDiscretisationPanel::setMeshingProperty


vector<string>
		QtInterpolatedEdgeDiscretisationPanel::getRefEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_refEntitiesTextField)
	return _refEntitiesTextField->getUniqueNames ( );
}	// QtInterpolatedEdgeDiscretisationPanel::getRefEntitiesNames


EdgeMeshingPropertyInterpolate*
			QtInterpolatedEdgeDiscretisationPanel::getMeshingProperty ( size_t edgeNum ) const
{
	CHECK_NULL_PTR_ERROR (_referenceComboBox)

	EdgeMeshingPropertyInterpolate*	emp	= 0;
	const vector<string>	names	= getRefEntitiesNames ( );
	switch (_referenceComboBox->currentIndex ( ))
	{
		case	0	: // Liste d'arêtes
			 emp	= new EdgeMeshingPropertyInterpolate (edgeNum, names);
			break;
		case	1	: // Face topologique
			if (1 != names.size ( ))
				throw Exception (UTF8String ("Erreur : le nom d'une unique face est attendu.", Charset::UTF_8));
			emp	= new EdgeMeshingPropertyInterpolate (edgeNum, names [0]);
			break;
		default	:
		{
			INTERNAL_ERROR (exc,
				"Le type d'entité de référence n'est pas supporté.",
				"QtInterpolatedEdgeDiscretisationPanel::getMeshingProperty")
			throw exc;
		}
	}	//     switch (_referenceComboBox->currentIndex ( ))
	updateProperty (*emp);

	return emp;
}	// QtInterpolatedEdgeDiscretisationPanel::getMeshingProperty


void QtInterpolatedEdgeDiscretisationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_referenceComboBox)
	CHECK_NULL_PTR_ERROR (_refEntitiesTextField)
	_refEntitiesTextField->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtDiscretisationPanelIfc::reset ( );
}	// QtInterpolatedEdgeDiscretisationPanel::reset


void QtInterpolatedEdgeDiscretisationPanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_refEntitiesTextField)
	_refEntitiesTextField->setInteractiveMode (false);
}	// QtInterpolatedEdgeDiscretisationPanel::stopSelection


void QtInterpolatedEdgeDiscretisationPanel::cancel ( )
{
	QtMgx3DOperationsSubPanel::cancel ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		CHECK_NULL_PTR_ERROR (_refEntitiesTextField)
		const vector<string>	names;
		_refEntitiesTextField->setUniqueNames (names);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtInterpolatedEdgeDiscretisationPanel::cancel


vector<Entity*> QtInterpolatedEdgeDiscretisationPanel::getPanelEntities ( )
{
	vector<Entity*>			entities;
	const vector<string>	names	= getRefEntitiesNames ( );

	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
		if (0 != (*it).length ( ))
			entities.push_back (getContext ( ).getTopoManager ( ).getEntity (
																*it, true));

	return entities;
}	// QtInterpolatedEdgeDiscretisationPanel::getPanelEntities


void QtInterpolatedEdgeDiscretisationPanel::refEntitiesCallback ( )
{
		BEGIN_QT_TRY_CATCH_BLOCK

		CHECK_NULL_PTR_ERROR (_referenceComboBox)
		CHECK_NULL_PTR_ERROR (_refEntitiesTextField)

		_refEntitiesTextField->clearSelection ( );
		switch (_referenceComboBox->currentIndex ( ))
		{
			case	0	:	// Liste d'arêtes
				_refEntitiesTextField->setFilteredTypes (
												FilterEntity::TopoCoEdge);
				_refEntitiesTextField->setFilteredDimensions (
												SelectionManagerIfc::D1);
				_refEntitiesTextField->setMultiSelectMode (true);
				break;
			case	1	:	// Face
				_refEntitiesTextField->setFilteredTypes (
												FilterEntity::TopoCoFace);
				_refEntitiesTextField->setFilteredDimensions (
												SelectionManagerIfc::D2);
				_refEntitiesTextField->setMultiSelectMode (false);
				break;
			default		:
			{
				INTERNAL_ERROR (exc,
					"Le type d'entité de référence n'est pas supporté.",
					"QtInterpolatedEdgeDiscretisationPanel::refEntitiesCallback")
				throw exc;
			}
		}	// switch (_referenceComboBox->currentIndex ( ))

		COMPLETE_QT_TRY_CATCH_BLOCK (false, this, "Magix 3D")
}	// QtInterpolatedEdgeDiscretisationPanel::refEntitiesCallback 


// ===========================================================================
//             LA CLASSE QtGlobalInterpolatedEdgeDiscretisationPanel
// ===========================================================================


QtGlobalInterpolatedEdgeDiscretisationPanel::QtGlobalInterpolatedEdgeDiscretisationPanel (
	QWidget* parent, const string& edgeName,
	QtMgx3DMainWindow& window)
	: QtDiscretisationPanelIfc (parent, window),
	  _firstEdgesPanel (0), _secondEdgesPanel(0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (layout);

	// L'arête de référence :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label	= new QLabel (QString::fromUtf8("Premières arêtes de référence :"), this);
	hlayout->addWidget (label);
	_firstEdgesPanel	= new QtEntityIDTextField (
		this, &window, SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	_firstEdgesPanel->setMultiSelectMode (true);
	hlayout->addWidget (_firstEdgesPanel);
	connect (_firstEdgesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesSelectedCallback (QString)));
	connect (_firstEdgesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesUnselectedCallback (QString)));

	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Secondes arêtes de référence :"), this);
	hlayout->addWidget (label);
	_secondEdgesPanel	= new QtEntityIDTextField (
		this, &window, SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	_secondEdgesPanel->setMultiSelectMode (true);
	hlayout->addWidget (_secondEdgesPanel);
	connect (_secondEdgesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesSelectedCallback (QString)));
	connect (_secondEdgesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesUnselectedCallback (QString)));

	// L'orthogonalité à la paroi :
	createOrthogonalityArea (*layout);
}	// QtGlobalInterpolatedEdgeDiscretisationPanel::QtGlobalInterpolatedEdgeDiscretisationPanel


QtGlobalInterpolatedEdgeDiscretisationPanel::QtGlobalInterpolatedEdgeDiscretisationPanel (
								const QtGlobalInterpolatedEdgeDiscretisationPanel& p)
	: QtDiscretisationPanelIfc (
						0, *new QtMgx3DMainWindow(0)),
						_firstEdgesPanel (0), _secondEdgesPanel(0)
{
	MGX_FORBIDDEN ("QtGlobalInterpolatedEdgeDiscretisationPanel copy constructor is not allowed.");
}	// QtGlobalInterpolatedEdgeDiscretisationPanel::QtGlobalInterpolatedEdgeDiscretisationPanel


QtGlobalInterpolatedEdgeDiscretisationPanel&
	QtGlobalInterpolatedEdgeDiscretisationPanel::operator = (
								const QtGlobalInterpolatedEdgeDiscretisationPanel&)
{
	 MGX_FORBIDDEN ("QtGlobalInterpolatedEdgeDiscretisationPanel assignment operator is not allowed.");
	return *this;
}	// QtGlobalInterpolatedEdgeDiscretisationPanel::operator =


QtGlobalInterpolatedEdgeDiscretisationPanel::~QtGlobalInterpolatedEdgeDiscretisationPanel ( )
{
}	// QtGlobalInterpolatedEdgeDiscretisationPanel::~QtGlobalInterpolatedEdgeDiscretisationPanel


void QtGlobalInterpolatedEdgeDiscretisationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_firstEdgesPanel)
	CHECK_NULL_PTR_ERROR (_secondEdgesPanel)
	_firstEdgesPanel->reset ( );
	_secondEdgesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtDiscretisationPanelIfc::reset ( );
}	// QtGlobalInterpolatedEdgeDiscretisationPanel::reset


void QtGlobalInterpolatedEdgeDiscretisationPanel::setMeshingProperty (
											const CoEdgeMeshingProperty& cemp)
{
	CHECK_NULL_PTR_ERROR (_firstEdgesPanel)
	CHECK_NULL_PTR_ERROR (_secondEdgesPanel)

	const EdgeMeshingPropertyGlobalInterpolate*	properties	=
					dynamic_cast<const EdgeMeshingPropertyGlobalInterpolate*>(&cemp);
	if (0 == properties)
	{
		INTERNAL_ERROR (exc, "Les propriétés de maillage ne sont pas du type EdgeMeshingPropertyGlobalInterpolate.",
				"QtGlobalInterpolatedEdgeDiscretisationPanel::setMeshingProperty")
		throw exc;
	}	// if (0 == properties)

	const vector<string>	names1	= properties->getFirstCoEdges();
	const vector<string>	names2	= properties->getSecondCoEdges();
	_firstEdgesPanel->setUniqueNames (names1);
	_secondEdgesPanel->setUniqueNames (names2);
	QtDiscretisationPanelIfc::setMeshingProperty(cemp);

}	// QtGlobalInterpolatedEdgeDiscretisationPanel::setMeshingProperty


vector<string> QtGlobalInterpolatedEdgeDiscretisationPanel::getFirstEdgeNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_firstEdgesPanel)
	return _firstEdgesPanel->getUniqueNames ( );
}	// QtGlobalInterpolatedEdgeDiscretisationPanel::getFirstEdgeNames


vector<string> QtGlobalInterpolatedEdgeDiscretisationPanel::getSecondEdgeNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_secondEdgesPanel)
	return _secondEdgesPanel->getUniqueNames ( );
}	// QtGlobalInterpolatedEdgeDiscretisationPanel::getSecondEdgeNames


EdgeMeshingPropertyGlobalInterpolate*
			QtGlobalInterpolatedEdgeDiscretisationPanel::getMeshingProperty ( size_t edgeNum ) const
{
	EdgeMeshingPropertyGlobalInterpolate*	emp	=
		 new EdgeMeshingPropertyGlobalInterpolate (edgeNum,
				 getFirstEdgeNames ( ), getSecondEdgeNames ( ));
	updateProperty (*emp);

	return emp;
}	// QtGlobalInterpolatedEdgeDiscretisationPanel::getMeshingProperty


void QtGlobalInterpolatedEdgeDiscretisationPanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_firstEdgesPanel)
	CHECK_NULL_PTR_ERROR (_secondEdgesPanel)
	_firstEdgesPanel->setInteractiveMode (false);
	_secondEdgesPanel->setInteractiveMode (false);
}	// QtGlobalInterpolatedEdgeDiscretisationPanel::stopSelection


void QtGlobalInterpolatedEdgeDiscretisationPanel::cancel ( )
{
	QtMgx3DOperationsSubPanel::cancel ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		CHECK_NULL_PTR_ERROR (_firstEdgesPanel)
		CHECK_NULL_PTR_ERROR (_secondEdgesPanel)
		const vector<string>	names;
		_firstEdgesPanel->setUniqueNames (names);
		_secondEdgesPanel->setUniqueNames (names);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGlobalInterpolatedEdgeDiscretisationPanel::cancel


vector<Entity*> QtGlobalInterpolatedEdgeDiscretisationPanel::getPanelEntities ( )
{
	vector<Entity*>			entities;
	const vector<string>	names1	= getFirstEdgeNames ( );

	for (vector<string>::const_iterator it = names1.begin ( );
	     names1.end ( ) != it; it++)
		if (0 != (*it).length ( ))
			entities.push_back (getContext ( ).getTopoManager ( ).getEntity (
																*it, true));
	const vector<string>	names2	= getSecondEdgeNames ( );

	for (vector<string>::const_iterator it = names2.begin ( );
	     names2.end ( ) != it; it++)
		if (0 != (*it).length ( ))
			entities.push_back (getContext ( ).getTopoManager ( ).getEntity (
																*it, true));


	return entities;
}	// QtGlobalInterpolatedEdgeDiscretisationPanel::getPanelEntities


// ===========================================================================
//                 LA CLASSE QtBiGeometricProgressionPanel
// ===========================================================================


QtBiGeometricProgressionPanel::QtBiGeometricProgressionPanel (
	QWidget* parent, double ratio1, double length1,
	double ratio2, double length2, QtMgx3DMainWindow& window)
	: QtDiscretisationPanelIfc (parent, window),
	  _ratio1TextField (0), _length1TextField (0),
	  _ratio2TextField (0), _length2TextField (0), _orientationCheckBox (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (layout);

	// La progression géométrique côté 1 :
	QHBoxLayout* hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label1	= new QLabel (QString::fromUtf8("Côté 1. Raison :"), this);
	hlayout->addWidget (label1);
	_ratio1TextField	= &QtNumericFieldsFactory::createRatioTextField (this);
	_ratio1TextField->setValue (ratio1);
	hlayout->addWidget (_ratio1TextField);
	connect (_ratio1TextField, SIGNAL (editingFinished ( )), this,
	         SLOT (discretisationModifiedCallback ( )));
	hlayout->addStretch (2);
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label2	= new QLabel (QString::fromUtf8("Côté 1. Longueur premier bras :"), this);
	label2->setFixedSize (label2->sizeHint ( ));
	label1->setFixedSize (label2->sizeHint ( ));
	hlayout->addWidget (label2);
	_length1TextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_length1TextField->setValue (length1);
	hlayout->addWidget (_length1TextField);
	hlayout->addStretch (2);
	connect (_length1TextField, SIGNAL (editingFinished ( )), this,
	         SLOT (discretisationModifiedCallback ( )));

	// La progression géométrique côté 2 :
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label1	= new QLabel (QString::fromUtf8("Côté 2. Raison :"), this);
	hlayout->addWidget (label1);
	_ratio2TextField	= &QtNumericFieldsFactory::createRatioTextField (this);
	_ratio1TextField->setValue (ratio2);
	hlayout->addWidget (_ratio2TextField);
	connect (_ratio2TextField, SIGNAL (editingFinished ( )), this,
	         SLOT (discretisationModifiedCallback ( )));
	hlayout->addStretch (2);
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label2	= new QLabel (QString::fromUtf8("Côté 2. Longueur premier bras :"), this);
	label2->setFixedSize (label2->sizeHint ( ));
	label1->setFixedSize (label2->sizeHint ( ));
	hlayout->addWidget (label2);
	_length2TextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_length2TextField->setValue (length2);
	hlayout->addWidget (_length2TextField);
	hlayout->addStretch (2);
	connect (_length2TextField, SIGNAL (editingFinished ( )), this,
	         SLOT (discretisationModifiedCallback ( )));

	// Le sens de la discrétisation :
	_orientationCheckBox	= new QCheckBox ("Inverser le sens", this);
	layout->addWidget (_orientationCheckBox);
	connect (_orientationCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (discretisationModifiedCallback ( )));

	// L'orthogonalité à la paroi :
	createOrthogonalityArea (*layout);

	// Le découpage en mode polaire
	createPolarArea (*layout);

}	// QtBiGeometricProgressionPanel::QtBiGeometricProgressionPanel


QtBiGeometricProgressionPanel::QtBiGeometricProgressionPanel (
										const QtBiGeometricProgressionPanel& p)
	: QtDiscretisationPanelIfc (
						0, *new QtMgx3DMainWindow(0)),
	  _ratio1TextField (0), _length1TextField (0),
	  _ratio2TextField (0), _length2TextField (0), _orientationCheckBox (0)
{
	MGX_FORBIDDEN ("QtBiGeometricProgressionPanel copy constructor is not allowed.");
}	// QtBiGeometricProgressionPanel::QtBiGeometricProgressionPanel


QtBiGeometricProgressionPanel&
	QtBiGeometricProgressionPanel::operator = (
										const QtBiGeometricProgressionPanel&)
{
	 MGX_FORBIDDEN ("QtBiGeometricProgressionPanel assignment operator is not allowed.");
	return *this;
}	// QtBiGeometricProgressionPanel::operator =


QtBiGeometricProgressionPanel::~QtBiGeometricProgressionPanel ( )
{
}	// QtBiGeometricProgressionPanel::~QtBiGeometricProgressionPanel


void QtBiGeometricProgressionPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_ratio1TextField)
	CHECK_NULL_PTR_ERROR (_length1TextField)
	CHECK_NULL_PTR_ERROR (_ratio2TextField)
	CHECK_NULL_PTR_ERROR (_length2TextField)
	_ratio1TextField->setValue (1.);
	_length1TextField->setValue (1.);
	_ratio2TextField->setValue (1.);
	_length2TextField->setValue (1.);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtDiscretisationPanelIfc::reset ( );
}	// QtBiGeometricProgressionPanel::reset


void QtBiGeometricProgressionPanel::setMeshingProperty (
											const CoEdgeMeshingProperty& cemp)
{
	CHECK_NULL_PTR_ERROR (_ratio1TextField)
	CHECK_NULL_PTR_ERROR (_length1TextField)
	CHECK_NULL_PTR_ERROR (_ratio2TextField)
	CHECK_NULL_PTR_ERROR (_length2TextField)
	CHECK_NULL_PTR_ERROR (_orientationCheckBox)

	const EdgeMeshingPropertyBigeometric*	properties	=
					dynamic_cast<const EdgeMeshingPropertyBigeometric*>(&cemp);
	if (0 == properties)
	{
		INTERNAL_ERROR (exc, "Les propriétés de maillage ne sont pas du type EdgeMeshingPropertyBigeometric.",
				"QtBiGeometricProgressionPanel::setMeshingProperty")
		throw exc;
	}	// if (0 == properties)

	_ratio1TextField->setValue (properties->getRatio1 ( ));
	_length1TextField->setValue (properties->getLength1 ( ));
	_ratio2TextField->setValue (properties->getRatio2 ( ));
	_length2TextField->setValue (properties->getLength2 ( ));
	_orientationCheckBox->setCheckState (
			true == properties->getDirect ( ) ? Qt::Unchecked : Qt::Checked);
	QtDiscretisationPanelIfc::setMeshingProperty(cemp);

}	// QtBiGeometricProgressionPanel::setMeshingProperty


double QtBiGeometricProgressionPanel::getRatio1 ( ) const
{
	CHECK_NULL_PTR_ERROR (_ratio1TextField)
	return _ratio1TextField->getValue ( );
}	// QtBiGeometricProgressionPanel::getRatio1


double QtBiGeometricProgressionPanel::getLength1 ( ) const
{
	CHECK_NULL_PTR_ERROR (_length1TextField)
	return _length1TextField->getValue ( );
}	// QtBiGeometricProgressionPanel::getLength1


double QtBiGeometricProgressionPanel::getRatio2 ( ) const
{
	CHECK_NULL_PTR_ERROR (_ratio2TextField)
	return _ratio2TextField->getValue ( );
}	// QtBiGeometricProgressionPanel::getRatio2


double QtBiGeometricProgressionPanel::getLength2 ( ) const
{
	CHECK_NULL_PTR_ERROR (_length2TextField)
	return _length2TextField->getValue ( );
}	// QtBiGeometricProgressionPanel::getLength2


bool QtBiGeometricProgressionPanel::invertOrientation ( ) const
{
	CHECK_NULL_PTR_ERROR (_orientationCheckBox)
	return Qt::Checked == _orientationCheckBox->checkState ( ) ? true : false;
}	// QtBiGeometricProgressionPanel::invertOrientation


EdgeMeshingPropertyBigeometric*
					QtBiGeometricProgressionPanel::getMeshingProperty ( size_t edgeNum ) const
{
	EdgeMeshingPropertyBigeometric*	emp	= 0;
	if (isPolarCut())
		emp = new EdgeMeshingPropertyBigeometric (edgeNum,
				getRatio1 ( ), getLength1 ( ), getRatio2 ( ), getLength2 ( ),
				getPolarCenter ( ),
				!invertOrientation ( ));
	else
		emp = new EdgeMeshingPropertyBigeometric (edgeNum,
				getRatio1 ( ), getLength1 ( ), getRatio2 ( ), getLength2 ( ),
				!invertOrientation ( ));
	updateProperty (*emp);

	return emp;
}	// QtBiGeometricProgressionPanel::getMeshingProperty


// ===========================================================================
//                 LA CLASSE QtHyperbolicDiscretisationPanel
// ===========================================================================


QtHyperbolicDiscretisationPanel::QtHyperbolicDiscretisationPanel (
	QWidget* parent, double length1, double length2,
	QtMgx3DMainWindow& window)
	: QtDiscretisationPanelIfc (parent, window),
	  _length1TextField (0), _length2TextField (0),
	  _orientationCheckBox (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));

	// Le premier bras côté 1 :
	QHBoxLayout* hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel* label	= new QLabel (QString::fromUtf8("Longueur premier bras côté 1 :"), this);
	hlayout->addWidget (label);
	_length1TextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_length1TextField->setValue (length1);
	hlayout->addWidget (_length1TextField);
	connect (_length1TextField, SIGNAL (editingFinished ( )), this,
	         SLOT (discretisationModifiedCallback ( )));

	// Le premier bras côté 2 :
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Longueur premier bras côté 2 :"), this);
	hlayout->addWidget (label);
	_length2TextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_length2TextField->setValue (length2);
	hlayout->addWidget (_length2TextField);
	connect (_length2TextField, SIGNAL (editingFinished ( )), this,
	         SLOT (discretisationModifiedCallback ( )));

	// Le sens de la discrétisation :
	_orientationCheckBox	= new QCheckBox ("Inverser le sens", this);
	layout->addWidget (_orientationCheckBox);
	connect (_orientationCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (discretisationModifiedCallback ( )));

	// L'orthogonalité à la paroi :
	createOrthogonalityArea (*layout);
}	// QtHyperbolicDiscretisationPanel::QtHyperbolicDiscretisationPanel


QtHyperbolicDiscretisationPanel::QtHyperbolicDiscretisationPanel (
									const QtHyperbolicDiscretisationPanel& p)
	: QtDiscretisationPanelIfc (
					0, *new QtMgx3DMainWindow(0)),
	  _length1TextField (0), _length2TextField (0),
	  _orientationCheckBox (0)
{
	MGX_FORBIDDEN ("QtHyperbolicDiscretisationPanel copy constructor is not allowed.");
}	// QtHyperbolicDiscretisationPanel::QtHyperbolicDiscretisationPanel


QtHyperbolicDiscretisationPanel&
	QtHyperbolicDiscretisationPanel::operator = (
										const QtHyperbolicDiscretisationPanel&)
{
	 MGX_FORBIDDEN ("QtHyperbolicDiscretisationPanel assignment operator is not allowed.");
	return *this;
}	// QtHyperbolicDiscretisationPanel::operator =


QtHyperbolicDiscretisationPanel::~QtHyperbolicDiscretisationPanel ( )
{
}	// QtHyperbolicDiscretisationPanel::~QtHyperbolicDiscretisationPanel


void QtHyperbolicDiscretisationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_length1TextField)
	CHECK_NULL_PTR_ERROR (_length2TextField)
	_length1TextField->setValue (1.);
	_length2TextField->setValue (1.);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtDiscretisationPanelIfc::reset ( );
}	// QtHyperbolicDiscretisationPanel::reset


void QtHyperbolicDiscretisationPanel::setMeshingProperty (
											const CoEdgeMeshingProperty& cemp)
{
	CHECK_NULL_PTR_ERROR (_length1TextField)
	CHECK_NULL_PTR_ERROR (_length2TextField)
	CHECK_NULL_PTR_ERROR (_orientationCheckBox)

	const EdgeMeshingPropertyHyperbolic*	properties	=
					dynamic_cast<const EdgeMeshingPropertyHyperbolic*>(&cemp);
	if (0 == properties)
	{
		INTERNAL_ERROR (exc, "Les propriétés de maillage ne sont pas du type EdgeMeshingPropertyHyperbolic.",
				"QtHyperbolicDiscretisationPanel::setMeshingProperty")
		throw exc;
	}	// if (0 == properties)

	_length1TextField->setValue (properties->getLength1 ( ));
	_length2TextField->setValue (properties->getLength2 ( ));
	_orientationCheckBox->setCheckState (
			true == properties->getDirect ( ) ? Qt::Unchecked : Qt::Checked);
	QtDiscretisationPanelIfc::setMeshingProperty(cemp);

}	// QtHyperbolicDiscretisationPanel::setMeshingProperty


double QtHyperbolicDiscretisationPanel::getLength1 ( ) const
{
	CHECK_NULL_PTR_ERROR (_length1TextField)
	return _length1TextField->getValue ( );
}	// QtHyperbolicDiscretisationPanel::getLength1


double QtHyperbolicDiscretisationPanel::getLength2 ( ) const
{
	CHECK_NULL_PTR_ERROR (_length2TextField)
	return _length2TextField->getValue ( );
}	// QtHyperbolicDiscretisationPanel::getLength2


bool QtHyperbolicDiscretisationPanel::invertOrientation ( ) const
{
	CHECK_NULL_PTR_ERROR (_orientationCheckBox)
	return Qt::Checked == _orientationCheckBox->checkState ( ) ? true : false;
}	// QtHyperbolicDiscretisationPanel::invertOrientation


EdgeMeshingPropertyHyperbolic*
				QtHyperbolicDiscretisationPanel::getMeshingProperty ( size_t edgeNum ) const
{
	EdgeMeshingPropertyHyperbolic*	emp	=
			 new EdgeMeshingPropertyHyperbolic (
					 edgeNum, getLength1 ( ), getLength2 ( ),
							!invertOrientation ( ));
	updateProperty (*emp);

	return emp;
}	// QtHyperbolicDiscretisationPanel::getMeshingProperty


// ===========================================================================
//                 LA CLASSE QtBetaDiscretisationPanel
// ===========================================================================


QtBetaDiscretisationPanel::QtBetaDiscretisationPanel (
	QWidget* parent, double factor, QtMgx3DMainWindow& window)
	: QtDiscretisationPanelIfc (parent, window),
	  _betaTextField (0), _orientationCheckBox (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));

	// Le facteur de resserrement beta :
	QHBoxLayout* hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel* label	= new QLabel (QString::fromUtf8("Bêta resserrement :"), this);
	hlayout->addWidget (label);
	_betaTextField	= &QtNumericFieldsFactory::createRatioTextField (this);
	_betaTextField->setValue (1.1);
	hlayout->addWidget (_betaTextField);
	connect (_betaTextField, SIGNAL (editingFinished ( )), this,
	         SLOT (discretisationModifiedCallback ( )));

	// Le sens de la discrétisation :
	_orientationCheckBox	= new QCheckBox ("Inverser le sens", this);
	layout->addWidget (_orientationCheckBox);
	connect (_orientationCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (discretisationModifiedCallback ( )));

	// L'orthogonalité à la paroi :
	createOrthogonalityArea (*layout);
}	// QtBetaDiscretisationPanel::QtBetaDiscretisationPanel


QtBetaDiscretisationPanel::QtBetaDiscretisationPanel (
										const QtBetaDiscretisationPanel& p)
	: QtDiscretisationPanelIfc (
		0, *new QtMgx3DMainWindow(0)),
	  _betaTextField (0), _orientationCheckBox (0)
{
	MGX_FORBIDDEN ("QtBetaDiscretisationPanel copy constructor is not allowed.");
}	// QtBetaDiscretisationPanel::QtBetaDiscretisationPanel


QtBetaDiscretisationPanel&
	QtBetaDiscretisationPanel::operator = (const QtBetaDiscretisationPanel&)
{
	 MGX_FORBIDDEN ("QtBetaDiscretisationPanel assignment operator is not allowed.");
	return *this;
}	// QtBetaDiscretisationPanel::operator =


QtBetaDiscretisationPanel::~QtBetaDiscretisationPanel ( )
{
}	// QtBetaDiscretisationPanel::~QtBetaDiscretisationPanel


void QtBetaDiscretisationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_betaTextField)
	_betaTextField->setValue (1.1);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtDiscretisationPanelIfc::reset ( );
}	// QtBetaDiscretisationPanel::reset


void QtBetaDiscretisationPanel::setMeshingProperty (
											const CoEdgeMeshingProperty& cemp)
{
	CHECK_NULL_PTR_ERROR (_betaTextField)
	CHECK_NULL_PTR_ERROR (_orientationCheckBox)

	const EdgeMeshingPropertyBeta*	properties	=
					dynamic_cast<const EdgeMeshingPropertyBeta*>(&cemp);
	if (0 == properties)
	{
		INTERNAL_ERROR (exc, "Les propriétés de maillage ne sont pas du type EdgeMeshingPropertyBeta.",
				"QtBetaDiscretisationPanel::setMeshingProperty")
		throw exc;
	}	// if (0 == properties)

	_betaTextField->setValue (properties->getBeta ( ));
	_orientationCheckBox->setCheckState (
			true == properties->getDirect ( ) ? Qt::Unchecked : Qt::Checked);
	QtDiscretisationPanelIfc::setMeshingProperty(cemp);

}	// QtBetaDiscretisationPanel::setMeshingProperty


double QtBetaDiscretisationPanel::getBeta ( ) const
{
	CHECK_NULL_PTR_ERROR (_betaTextField)
	return _betaTextField->getValue ( );
}	// QtBetaDiscretisationPanel::getBeta


bool QtBetaDiscretisationPanel::invertOrientation ( ) const
{
	CHECK_NULL_PTR_ERROR (_orientationCheckBox)
	return Qt::Checked == _orientationCheckBox->checkState ( ) ? true : false;
}	// QtBetaDiscretisationPanel::invertOrientation


EdgeMeshingPropertyBeta*
					QtBetaDiscretisationPanel::getMeshingProperty ( size_t edgeNum ) const
{
	EdgeMeshingPropertyBeta*	emp	=
			 new EdgeMeshingPropertyBeta (
					 edgeNum, getBeta ( ), !invertOrientation ( ));
	updateProperty (*emp);

	return emp;
}	// QtBetaDiscretisationPanel::getMeshingProperty


// ===========================================================================
//                   LA CLASSE QtEdgeMeshingPropertyPanelIfc
// ===========================================================================

QtEdgeMeshingPropertyPanelIfc::QtEdgeMeshingPropertyPanelIfc (
								QWidget* parent, QtMgx3DMainWindow& mainWindow)
	: QtDiscretisationPanelIfc (parent, mainWindow)
{
}	// QtEdgeMeshingPropertyPanelIfc::QtEdgeMeshingPropertyPanelIfc


QtEdgeMeshingPropertyPanelIfc::QtEdgeMeshingPropertyPanelIfc (
									const QtEdgeMeshingPropertyPanelIfc& cao)
	: QtDiscretisationPanelIfc (
							0, *new QtMgx3DMainWindow(0))
{
	MGX_FORBIDDEN ("QtEdgeMeshingPropertyPanelIfc copy constructor is not allowed.");
}	// QtEdgeMeshingPropertyPanelIfc::QtEdgeMeshingPropertyPanelIfc


QtEdgeMeshingPropertyPanelIfc& QtEdgeMeshingPropertyPanelIfc::operator = (
										const QtEdgeMeshingPropertyPanelIfc&)
{
	MGX_FORBIDDEN ("QtEdgeMeshingPropertyPanelIfc assignment operator is not allowed.");
	return *this;
}	// QtEdgeMeshingPropertyPanelIfc::operator =


QtEdgeMeshingPropertyPanelIfc::~QtEdgeMeshingPropertyPanelIfc ( )
{
}	// QtEdgeMeshingPropertyPanelIfc::~QtEdgeMeshingPropertyPanelIfc


CoEdgeMeshingProperty*
					QtEdgeMeshingPropertyPanelIfc::getMeshingProperty ( ) const
{
	throw Exception (UTF8String ("QtEdgeMeshingPropertyPanelIfc::getMeshingProperty should be overloaded.", Charset::UTF_8));
}	// QtEdgeMeshingPropertyPanelIfc::getMeshingProperty


void QtEdgeMeshingPropertyPanelIfc::setMeshingProperty (
												const CoEdgeMeshingProperty&)
{
	throw Exception (UTF8String ("QtEdgeMeshingPropertyPanelIfc::setMeshingProperty should be overloaded.", Charset::UTF_8));
}	// QtEdgeMeshingPropertyPanelIfc::setMeshingProperty


EntitySeizureManager*
			 QtEdgeMeshingPropertyPanelIfc::getFirstSeizureManager ( ) const
{
	return 0;
}	// QtEdgeMeshingPropertyPanelIfc::getFirstSeizureManager


EntitySeizureManager*
			 QtEdgeMeshingPropertyPanelIfc::getLastSeizureManager ( ) const
{
	return 0;
}	// QtEdgeMeshingPropertyPanelIfc::getLastSeizureManager


void QtEdgeMeshingPropertyPanelIfc::entitiesAddedToSelectionCallback (
														QString entitiesNames)
{
	emit entitiesAddedToSelection (entitiesNames);
}	// QtEdgeMeshingPropertyPanelIfc::entitiesAddedToSelectionCallback


void QtEdgeMeshingPropertyPanelIfc::entitiesRemovedFromSelectionCallback (
														QString entitiesNames)
{
	emit entitiesRemovedFromSelection (entitiesNames);
}	// QtEdgeMeshingPropertyPanelIfc::entitiesRemovedFromSelectionCallback


// ===========================================================================
//                   LA CLASSE QtEdgeListMeshingPropertyPanel
// ===========================================================================

QtEdgeListMeshingPropertyPanel::QtEdgeListMeshingPropertyPanel (
			QWidget* parent, QtMgx3DMainWindow& mainWindow, QtIntTextField* edgesNumTextField)
	: QtEdgeMeshingPropertyPanelIfc (parent, mainWindow),
	  _algorithmComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _uniformDiscretisationPanel (0), _geometricProgressionPanel (0),
	  _specificSizePanel (0), _interpolatedPanel (0), _globalInterpolatedPanel(0),
	  _bigeometricProgressionPanel (0), _hyperbolicPanel (0),
	  _betaPanel (0), _edgesNumTextField (edgesNumTextField)
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

	// Méthode de discrétisation des arêtes :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	QLabel* label	= new QLabel ("Algorithme", this);
	hlayout->addWidget (label);
	_algorithmComboBox	= new QComboBox (this);
	_algorithmComboBox->addItem ("Uniforme");
	_algorithmComboBox->addItem (QString::fromUtf8("Progression géométrique"));
	_algorithmComboBox->addItem (QString::fromUtf8("Taille définie"));
	_algorithmComboBox->addItem (QString::fromUtf8("Interpolation / face ou 1 ensemble d'arêtes"));
	_algorithmComboBox->addItem (QString::fromUtf8("Interpolation entre 2 ensembles d'arêtes"));
	_algorithmComboBox->addItem (QString::fromUtf8("Progressions géométriques"));
	_algorithmComboBox->addItem (QString::fromUtf8("Progression hyperbolique"));
	_algorithmComboBox->addItem (QString::fromUtf8("Loi beta de resserrement à la paroi"));
	connect (_algorithmComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (algorithmCallback ( )));
	hlayout->addWidget (_algorithmComboBox);
	hlayout->addStretch (10);
	
	// Définition de la discrétisation :
	QtGroupBox*		groupBox	=
						new QtGroupBox (this, "Autres paramètres de discrétisation");
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
	currentLayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	currentLayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	_currentParentWidget->setLayout (currentLayout);
	_uniformDiscretisationPanel	=
					new QtUniformDiscretisationPanel (0, mainWindow);
	_uniformDiscretisationPanel->hide ( );
	connect (_uniformDiscretisationPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_uniformDiscretisationPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_uniformDiscretisationPanel, SIGNAL (discretisationModified ( )),
	         this, SLOT (discretisationModifiedCallback ( )));
	_geometricProgressionPanel	=
			new QtGeometricProgressionPanel (0, 1., mainWindow);
	_geometricProgressionPanel->hide ( );
	connect (_geometricProgressionPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_geometricProgressionPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_geometricProgressionPanel,
	         SIGNAL (discretisationModified ( )),
	         this, SLOT (discretisationModifiedCallback ( )));
	_specificSizePanel	=
			new QtSpecificSizePanel (0, 1., mainWindow);
	_specificSizePanel->hide ( );
	connect (_specificSizePanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_specificSizePanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_specificSizePanel, SIGNAL (discretisationModified ( )),
	         this, SLOT (discretisationModifiedCallback ( )));
	_interpolatedPanel	=
			new QtInterpolatedEdgeDiscretisationPanel (0, "", mainWindow);
	_interpolatedPanel->hide ( );
	connect (_interpolatedPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_interpolatedPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_interpolatedPanel, SIGNAL (discretisationModified ( )),
	         this, SLOT (discretisationModifiedCallback ( )));
	_globalInterpolatedPanel	=
			new QtGlobalInterpolatedEdgeDiscretisationPanel (0, "", mainWindow);
	_globalInterpolatedPanel->hide ( );
	connect (_globalInterpolatedPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_globalInterpolatedPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_globalInterpolatedPanel, SIGNAL (discretisationModified ( )),
	         this, SLOT (discretisationModifiedCallback ( )));
	_bigeometricProgressionPanel	=
			new QtBiGeometricProgressionPanel (0, 1., 1., 1., 1., mainWindow);
	_bigeometricProgressionPanel->hide ( );
	connect (_bigeometricProgressionPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_bigeometricProgressionPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_bigeometricProgressionPanel, SIGNAL (discretisationModified ( )),
	         this, SLOT (discretisationModifiedCallback ( )));
	_hyperbolicPanel	=
				new QtHyperbolicDiscretisationPanel (0, 1., 1., mainWindow);
	_hyperbolicPanel->hide ( );
	connect (_hyperbolicPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_hyperbolicPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_hyperbolicPanel, SIGNAL (discretisationModified ( )),
	         this, SLOT (discretisationModifiedCallback ( )));
	_betaPanel	=
			new QtBetaDiscretisationPanel (0, 1.1, mainWindow);
	_betaPanel->hide ( );
	connect (_betaPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_betaPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_betaPanel,
	         SIGNAL (discretisationModified ( )),
	         this, SLOT (discretisationModifiedCallback ( )));

	vlayout->addStretch (2);

	algorithmCallback ( );
}	// QtEdgeListMeshingPropertyPanel::QtEdgeListMeshingPropertyPanel


QtEdgeListMeshingPropertyPanel::QtEdgeListMeshingPropertyPanel (
									const QtEdgeListMeshingPropertyPanel& cao)
	: QtEdgeMeshingPropertyPanelIfc (
			0, *new QtMgx3DMainWindow(0)),
	  _algorithmComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _uniformDiscretisationPanel (0), _geometricProgressionPanel (0),
	  _specificSizePanel (0), _interpolatedPanel (0), _globalInterpolatedPanel(0),
	  _bigeometricProgressionPanel (0), _hyperbolicPanel (0),
	  _betaPanel(0), _edgesNumTextField (0)
{
	MGX_FORBIDDEN ("QtEdgeListMeshingPropertyPanel copy constructor is not allowed.");
}	// QtEdgeListMeshingPropertyPanel::QtEdgeListMeshingPropertyPanel (


QtEdgeListMeshingPropertyPanel& QtEdgeListMeshingPropertyPanel::operator = (
										const QtEdgeListMeshingPropertyPanel&)
{
	MGX_FORBIDDEN ("QtEdgeListMeshingPropertyPanel assignment operator is not allowed.");
	return *this;
}	// QtEdgeListMeshingPropertyPanel::operator =


QtEdgeListMeshingPropertyPanel::~QtEdgeListMeshingPropertyPanel ( )
{
}	// QtEdgeListMeshingPropertyPanel::~QtEdgeListMeshingPropertyPanel


QtEdgeListMeshingPropertyPanel::ALGORITHM
						QtEdgeListMeshingPropertyPanel::getAlgorithm ( ) const
{
	CHECK_NULL_PTR_ERROR (_algorithmComboBox);
	return (QtEdgeListMeshingPropertyPanel::ALGORITHM)_algorithmComboBox->currentIndex ( );
}	// QtEdgeListMeshingPropertyPanel::getAlgorithm


CoEdgeMeshingProperty*
					QtEdgeListMeshingPropertyPanel::getMeshingProperty ( ) const
{
	CHECK_NULL_PTR_ERROR (_uniformDiscretisationPanel)
	CHECK_NULL_PTR_ERROR (_geometricProgressionPanel)
	CHECK_NULL_PTR_ERROR (_specificSizePanel)
	CHECK_NULL_PTR_ERROR (_interpolatedPanel)
	CHECK_NULL_PTR_ERROR (_globalInterpolatedPanel)
	CHECK_NULL_PTR_ERROR (_bigeometricProgressionPanel)
	CHECK_NULL_PTR_ERROR (_hyperbolicPanel)
	CHECK_NULL_PTR_ERROR (_betaPanel)


	CHECK_NULL_PTR_ERROR (_edgesNumTextField)
	int edgesNum = _edgesNumTextField->getValue ( );

	switch (getAlgorithm ( ))
	{
		case QtEdgeListMeshingPropertyPanel::UNIFORM				:
			return _uniformDiscretisationPanel->getMeshingProperty ( edgesNum );
		case QtEdgeListMeshingPropertyPanel::GEOMETRIC_PROGRESSION	:
			return _geometricProgressionPanel->getMeshingProperty ( edgesNum );
		case QtEdgeListMeshingPropertyPanel::SPECIFIC_SIZE	:
			return _specificSizePanel->getMeshingProperty ( );
		case QtEdgeListMeshingPropertyPanel::INTERPOLATION	:
			return _interpolatedPanel->getMeshingProperty ( edgesNum );
		case QtEdgeListMeshingPropertyPanel::GLOBAL_INTERPOLATION	:
			return _globalInterpolatedPanel->getMeshingProperty ( edgesNum );
		case QtEdgeListMeshingPropertyPanel::BI_GEOMETRIC	:
			return _bigeometricProgressionPanel->getMeshingProperty ( edgesNum );
		case QtEdgeListMeshingPropertyPanel::HYPERBOLIC	:
			return _hyperbolicPanel->getMeshingProperty ( edgesNum );
		case QtEdgeListMeshingPropertyPanel::BETA	:
			return _betaPanel->getMeshingProperty ( edgesNum );
	}	// switch (getAlgorithm ( ))

	INTERNAL_ERROR (exc, "Méthode de discrétisation d'arête non supportée", "QtEdgeListMeshingPropertyPanel::getMeshingProperty")
	throw exc;
}	// QtEdgeListMeshingPropertyPanel::getMeshingProperty


void QtEdgeListMeshingPropertyPanel::setMeshingProperty (
										const CoEdgeMeshingProperty& properties)
{
	CHECK_NULL_PTR_ERROR (_algorithmComboBox)
	CHECK_NULL_PTR_ERROR (_edgesNumTextField)

	const QtEdgeListMeshingPropertyPanel::ALGORITHM	currentAlgo	=
					(QtEdgeListMeshingPropertyPanel::ALGORITHM)
										_algorithmComboBox->currentIndex ( );
	QtEdgeListMeshingPropertyPanel::ALGORITHM		newAlgo	=
								(QtEdgeListMeshingPropertyPanel::ALGORITHM)-1;
	int	currentEdgesNum	= -1;
	try
	{
		currentEdgesNum	= _edgesNumTextField->getValue ( );
	}
	catch (...)
	{
	}
	switch (properties.getMeshLaw ( ))
	{
		case CoEdgeMeshingProperty::uniforme		:
			newAlgo	= QtEdgeListMeshingPropertyPanel::UNIFORM;
			_edgesNumTextField->setValue (properties.getNbEdges ( ));
			break;
		case CoEdgeMeshingProperty::geometrique		:
			newAlgo	= QtEdgeListMeshingPropertyPanel::GEOMETRIC_PROGRESSION;
			_edgesNumTextField->setValue (properties.getNbEdges ( ));
			break;
		case CoEdgeMeshingProperty::bigeometrique	:
			newAlgo	= QtEdgeListMeshingPropertyPanel::BI_GEOMETRIC;
			_edgesNumTextField->setValue (properties.getNbEdges ( ));
			break;
		case CoEdgeMeshingProperty::hyperbolique	:
			newAlgo	= QtEdgeListMeshingPropertyPanel::HYPERBOLIC;
			_edgesNumTextField->setValue (properties.getNbEdges ( ));
			break;
		case CoEdgeMeshingProperty::specific_size	:
			newAlgo	= QtEdgeListMeshingPropertyPanel::SPECIFIC_SIZE;
			break;
		case CoEdgeMeshingProperty::interpolate		:
			newAlgo	= QtEdgeListMeshingPropertyPanel::INTERPOLATION;
			_edgesNumTextField->setValue (properties.getNbEdges ( ));
			break;
		case CoEdgeMeshingProperty::globalinterpolate		:
			newAlgo	= QtEdgeListMeshingPropertyPanel::GLOBAL_INTERPOLATION;
			_edgesNumTextField->setValue (properties.getNbEdges ( ));
			break;
		case CoEdgeMeshingProperty::beta_resserrement		:
			newAlgo	= QtEdgeListMeshingPropertyPanel::BETA;
			_edgesNumTextField->setValue (properties.getNbEdges ( ));
			break;
		default	:
			throw Exception (UTF8String ("Réinitialisation du panneau impossible : alorithme non supporté.", Charset::UTF_8));
	}	// switch (properties.getMeshLaw ( ))
	_algorithmComboBox->setCurrentIndex (newAlgo);

	if ((currentAlgo != newAlgo) ||
	    (currentEdgesNum != properties.getNbEdges ( )))
		algorithmCallback ( );

	CHECK_NULL_PTR_ERROR (_currentPanel)
	_currentPanel->setMeshingProperty (properties);
}	// QtEdgeListMeshingPropertyPanel::setMeshingProperty


void QtEdgeListMeshingPropertyPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_algorithmComboBox)
	CHECK_NULL_PTR_ERROR (_uniformDiscretisationPanel)
	CHECK_NULL_PTR_ERROR (_geometricProgressionPanel)
	CHECK_NULL_PTR_ERROR (_specificSizePanel)
	CHECK_NULL_PTR_ERROR (_interpolatedPanel)
	CHECK_NULL_PTR_ERROR (_globalInterpolatedPanel)
	CHECK_NULL_PTR_ERROR (_bigeometricProgressionPanel)
	CHECK_NULL_PTR_ERROR (_hyperbolicPanel)
	CHECK_NULL_PTR_ERROR (_betaPanel)
	CHECK_NULL_PTR_ERROR (_edgesNumTextField)
	_uniformDiscretisationPanel->reset ( );
	_geometricProgressionPanel->reset ( );
	_specificSizePanel->reset ( );
	_interpolatedPanel->reset ( );
	_globalInterpolatedPanel->reset ( );
	_bigeometricProgressionPanel->reset ( );
	_hyperbolicPanel->reset ( );
	_betaPanel->reset ( );
	_edgesNumTextField->setValue (10);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtEdgeMeshingPropertyPanelIfc::reset ( );
}	// QtEdgeListMeshingPropertyPanel::reset


void QtEdgeListMeshingPropertyPanel::validate ( )
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
		error << "QtEdgeListMeshingPropertyPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_algorithmComboBox)
	switch (_algorithmComboBox->currentIndex ( ))
	{
		case QtEdgeListMeshingPropertyPanel::UNIFORM				:
		case QtEdgeListMeshingPropertyPanel::GEOMETRIC_PROGRESSION	:
		case QtEdgeListMeshingPropertyPanel::SPECIFIC_SIZE			:
		case QtEdgeListMeshingPropertyPanel::INTERPOLATION			:
		case QtEdgeListMeshingPropertyPanel::BI_GEOMETRIC			:
		case QtEdgeListMeshingPropertyPanel::HYPERBOLIC				:
		case QtEdgeListMeshingPropertyPanel::BETA		    		:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence d'algorithme de discrétisation d'arêtes "
			      << "sélectionné.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtEdgeListMeshingPropertyPanel::validate : index "
			      << "d'algorithme de discrétisation d'arêtes invalide ("
			      << (long)_algorithmComboBox->currentIndex ( ) << ").";
	}	// switch (_algorithmComboBox->currentIndex ( ))

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtEdgeListMeshingPropertyPanel::validate


void QtEdgeListMeshingPropertyPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	if (0 != _currentPanel)
		_currentPanel->cancel ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtEdgeListMeshingPropertyPanel::cancel


void QtEdgeListMeshingPropertyPanel::autoUpdate ( )
{
	if (0 != _currentPanel)
		_currentPanel->autoUpdate ( );

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtEdgeListMeshingPropertyPanel::autoUpdate


EntitySeizureManager*
			QtEdgeListMeshingPropertyPanel::getFirstSeizureManager ( ) const
{
	CHECK_NULL_PTR_ERROR (_interpolatedPanel)
	switch (getAlgorithm ( ))
	{
		case QtEdgeListMeshingPropertyPanel::INTERPOLATION			:
			return _interpolatedPanel->getRefEntitiesTextField ( );
		case QtEdgeListMeshingPropertyPanel::GLOBAL_INTERPOLATION			:
			return _globalInterpolatedPanel->getFirstEdgeTextField();
	}	// switch (getAlgorithm ( ))

	return 0;
}	// QtEdgeListMeshingPropertyPanel::getFirstSeizureManager


EntitySeizureManager*
			QtEdgeListMeshingPropertyPanel::getLastSeizureManager ( ) const
{
	CHECK_NULL_PTR_ERROR (_interpolatedPanel)
	switch (getAlgorithm ( ))
	{
		case QtEdgeListMeshingPropertyPanel::INTERPOLATION			:
			return _interpolatedPanel->getRefEntitiesTextField ( );
		case QtEdgeListMeshingPropertyPanel::GLOBAL_INTERPOLATION			:
			return _globalInterpolatedPanel->getSecondEdgeTextField();
	}	// switch (getAlgorithm ( ))

	return 0;
}	// QtEdgeListMeshingPropertyPanel::getLastSeizureManager


vector<Entity*> QtEdgeListMeshingPropertyPanel::getPanelEntities ( )
{
	vector<Entity*>			entities;
	if (0 != _currentPanel)
		entities	= _currentPanel->getPanelEntities ( );

	return entities;
}	// QtEdgeListMeshingPropertyPanel::getPanelEntities


void QtEdgeListMeshingPropertyPanel::algorithmCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgesNumTextField)
	CHECK_NULL_PTR_ERROR (_currentParentWidget)
	CHECK_NULL_PTR_ERROR (_currentParentWidget->layout ( ))
	CHECK_NULL_PTR_ERROR (_uniformDiscretisationPanel)
	CHECK_NULL_PTR_ERROR (_geometricProgressionPanel)
	CHECK_NULL_PTR_ERROR (_specificSizePanel)
	CHECK_NULL_PTR_ERROR (_interpolatedPanel)
	CHECK_NULL_PTR_ERROR (_interpolatedPanel->getRefEntitiesTextField ( ))
	CHECK_NULL_PTR_ERROR (_globalInterpolatedPanel)
	CHECK_NULL_PTR_ERROR (_globalInterpolatedPanel->getFirstEdgeTextField ( ))
	CHECK_NULL_PTR_ERROR (_globalInterpolatedPanel->getSecondEdgeTextField ( ))
	CHECK_NULL_PTR_ERROR (_bigeometricProgressionPanel)
	CHECK_NULL_PTR_ERROR (_hyperbolicPanel)
	CHECK_NULL_PTR_ERROR (_betaPanel)
	if (0 != _currentPanel)
	{
		_currentParentWidget->layout ( )->removeWidget (_currentPanel);
		_currentPanel->hide ( );
		_currentPanel->setParent (0);
	}	// if (0 != _currentPanel)
	_currentPanel	= 0;

	switch (getAlgorithm ( ))
	{
		case QtEdgeListMeshingPropertyPanel::UNIFORM				:
			_currentPanel	= _uniformDiscretisationPanel;
			_edgesNumTextField->setVisible(true);
			break;
		case QtEdgeListMeshingPropertyPanel::GEOMETRIC_PROGRESSION	:
			_currentPanel	= _geometricProgressionPanel;
			_edgesNumTextField->setVisible(true);
			break;
		case QtEdgeListMeshingPropertyPanel::SPECIFIC_SIZE			:
			_currentPanel	= _specificSizePanel;
			_edgesNumTextField->setVisible(false);
			break;
		case QtEdgeListMeshingPropertyPanel::INTERPOLATION			:
			_currentPanel	= _interpolatedPanel;
			_edgesNumTextField->setVisible(true);
			break;
		case QtEdgeListMeshingPropertyPanel::GLOBAL_INTERPOLATION			:
			_currentPanel	= _globalInterpolatedPanel;
			_edgesNumTextField->setVisible(true);
			break;
		case QtEdgeListMeshingPropertyPanel::BI_GEOMETRIC			:
			_currentPanel	= _bigeometricProgressionPanel;
			_edgesNumTextField->setVisible(true);
			break;
		case QtEdgeListMeshingPropertyPanel::HYPERBOLIC			:
			_currentPanel	= _hyperbolicPanel;
			_edgesNumTextField->setVisible(true);
			break;
		case QtEdgeListMeshingPropertyPanel::BETA	:
			_currentPanel	= _betaPanel;
			_edgesNumTextField->setVisible(true);
			break;
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Algorithme non supporté ("
			        << (unsigned long)getAlgorithm ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtEdgeListMeshingPropertyPanel::algorithmCallback")
			throw exc;
		}
	}	// switch (getAlgorithm ( ))
	if (0 != _currentPanel)
	{
		_currentPanel->setParent (_currentParentWidget);
		_currentParentWidget->layout ( )->addWidget (_currentPanel);
		_currentPanel->show ( );
		if (0 != getMainWindow ( ))
			getMainWindow ( )->getOperationsPanel ( ).updateLayoutWorkaround( );
	}
	else {
		UTF8String	message (Charset::UTF_8);
		message << "Référence nulle vers un panneau courant";
		INTERNAL_ERROR (exc, message, "QtEdgeListMeshingPropertyPanel::algorithmCallback")
		throw exc;
	}

	emit algorithmModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtEdgeListMeshingPropertyPanel::algorithmCallback


void QtEdgeListMeshingPropertyPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _currentPanel)
		_currentPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtEdgeListMeshingPropertyPanel::operationCompleted


// ===========================================================================
//                   LA CLASSE QtTopologyEdgesRefinementPanel
// ===========================================================================

QtTopologyEdgesRefinementPanel::QtTopologyEdgesRefinementPanel (
			QWidget* parent, QtMgx3DMainWindow& mainWindow)
	: QtEdgeMeshingPropertyPanelIfc (parent, mainWindow),
	  _edgePanel (0), _fixedEdgesPanel (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (layout);

	// Nom opération :
	QLabel*	label	= new QLabel ("Nombre de bras avec contrainte", this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// L'arête de référence à discrétiser :
	_edgePanel	= new QtMgx3DEdgePanel (
						this, "Arête de référence à discrétiser", true, "",
						&mainWindow,
						FilterEntity::TopoCoEdge);
	layout->addWidget (_edgePanel);
	connect (_edgePanel, SIGNAL (edgesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edgePanel, SIGNAL (edgesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// Les arêtes à ne pas discrétiser :
	_fixedEdgesPanel	= new QtMgx3DEdgePanel (
						this, "Arêtes à ne pas modifier", true, "",
						&mainWindow,
						FilterEntity::TopoCoEdge);
	_fixedEdgesPanel->getEntityTextField ( ).setMultiSelectMode (true);
	layout->addWidget (_fixedEdgesPanel);
	connect (_fixedEdgesPanel, SIGNAL (edgesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_fixedEdgesPanel, SIGNAL (edgesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	_edgePanel->getEntityTextField ( ).setLinkedSeizureManagers (
								0, &_fixedEdgesPanel->getEntityTextField ( ));
	_fixedEdgesPanel->getEntityTextField ( ).setLinkedSeizureManagers (
								&_edgePanel->getEntityTextField ( ), 0);

	// L'orthogonalité à la paroi :
	createOrthogonalityArea (*layout);
}	// QtTopologyEdgesRefinementPanel::QtTopologyEdgesRefinementPanel


QtTopologyEdgesRefinementPanel::QtTopologyEdgesRefinementPanel (
										const QtTopologyEdgesRefinementPanel&)
	: QtEdgeMeshingPropertyPanelIfc (
		0, *new QtMgx3DMainWindow (0)),
	  _edgePanel (0), _fixedEdgesPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyEdgesRefinementPanel copy constructor is not allowed.");
}	// QtTopologyEdgesRefinementPanel::QtTopologyEdgesRefinementPanel (const QtTopologyEdgesRefinementPanel&)


QtTopologyEdgesRefinementPanel& QtTopologyEdgesRefinementPanel::operator = (
										const QtTopologyEdgesRefinementPanel&)
{
	MGX_FORBIDDEN ("QtTopologyEdgesRefinementPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyEdgesRefinementPanel::QtTopologyEdgesRefinementPanel (const QtTopologyEdgesRefinementPanel&)


QtTopologyEdgesRefinementPanel::~QtTopologyEdgesRefinementPanel ( )
{
}	// QtTopologyEdgesRefinementPanel::~QtTopologyEdgesRefinementPanel


void QtTopologyEdgesRefinementPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_fixedEdgesPanel)
	_edgePanel->reset ( );
	_fixedEdgesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtEdgeMeshingPropertyPanelIfc::reset ( );
}	// QtTopologyEdgesRefinementPanel::reset


void QtTopologyEdgesRefinementPanel::setMeshingProperty (
											const CoEdgeMeshingProperty& cemp)
{
throw Exception (UTF8String ("QtTopologyEdgesRefinementPanel::setMeshingProperty is not yet implemented.", Charset::UTF_8));
}	// QtTopologyEdgesRefinementPanel::setMeshingProperty


void QtTopologyEdgesRefinementPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_fixedEdgesPanel)
	_edgePanel->stopSelection ( );
	_fixedEdgesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_edgePanel->setUniqueName ("");
		_fixedEdgesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyEdgesRefinementPanel::cancel


void QtTopologyEdgesRefinementPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_fixedEdgesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		 vector<string>	selectedEdges	= 
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
		if (1 <= selectedEdges.size ( ))
			_edgePanel->setUniqueName (selectedEdges [0]);
		if (2 <= selectedEdges.size ( ))
		{
			selectedEdges.erase (selectedEdges.begin ( ));
			_fixedEdgesPanel->getEntityTextField ( ).setUniqueNames (selectedEdges);
		}	// if (2 <= selectedEdges.size ( ))
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_edgePanel->setUniqueName ("");
	_fixedEdgesPanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationPanel::autoUpdate ( );

	_edgePanel->actualizeGui (true);
	_fixedEdgesPanel->actualizeGui (true);
}	// QtTopologyEdgesRefinementPanel::autoUpdate


string QtTopologyEdgesRefinementPanel::getRefinedReferenceEdgeName ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgePanel)	
	return _edgePanel->getUniqueName ( );
}	// QtTopologyEdgesRefinementPanel::getRefinedReferenceEdgeName


vector<string> QtTopologyEdgesRefinementPanel::getFixedEdgeNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_fixedEdgesPanel)	
	return _fixedEdgesPanel->getEntityTextField ( ).getUniqueNames ( );
}	// QtTopologyEdgesRefinementPanel::getFixedEdgeNames


vector<Entity*> QtTopologyEdgesRefinementPanel::getPanelEntities ( )
{
	vector<Entity*>	entities;
	const string	name	= getRefinedReferenceEdgeName ( );
	if (0 != name.length ( ))
	{
		TopoEntity*	entity	= getContext ( ).getTopoManager ( ).getCoEdge(name);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}	// if (0 != name.length ( ))
	const vector<string>	names	= getFixedEdgeNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		if (0 != (*it).length ( ))
		{
			TopoEntity*	entity	=
							getContext ( ).getTopoManager ( ).getCoEdge (*it);
			CHECK_NULL_PTR_ERROR (entity)
			entities.push_back (entity);
		}	// if (0 != (*it).length ( ))
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtTopologyEdgesRefinementPanel::getPanelEntities


EntitySeizureManager*
			QtTopologyEdgesRefinementPanel::getFirstSeizureManager ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
	return &_edgePanel->getEntityTextField ( );
}	// QtTopologyEdgesRefinementPanel::getFirstSeizureManager


EntitySeizureManager*
			QtTopologyEdgesRefinementPanel::getLastSeizureManager ( ) const
{
	CHECK_NULL_PTR_ERROR (_fixedEdgesPanel)
	return &_fixedEdgesPanel->getEntityTextField ( );
}	// QtTopologyEdgesRefinementPanel::getLastSeizureManager


void QtTopologyEdgesRefinementPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _edgePanel)
		_edgePanel->stopSelection ( );
	if (0 != _fixedEdgesPanel)
		_fixedEdgesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyEdgesRefinementPanel::operationCompleted


// ===========================================================================
//                   LA CLASSE QtEdgeMeshingPropertyPanel
// ===========================================================================

QtEdgeMeshingPropertyPanel::QtEdgeMeshingPropertyPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).edgesRefinementOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).edgesRefinementOperationTag),
	  _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _edgeListPanel (0), _edgesRefinementPanel (0), _edgesPanel (0),
      _previewProjectedCheckBox(0), _edgesNumTextField(0)
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

	// Méthode de discrétisation des arêtes :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Méthode"), this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem (QString::fromUtf8("Discrétisation liste d'arêtes"));
	_operationMethodComboBox->addItem (QString::fromUtf8("Discrétisation d'arêtes parallèles"));
	_operationMethodComboBox->addItem (QString::fromUtf8("Nombre de bras avec contrainte"));
	_operationMethodComboBox->setCurrentIndex (0);
	connect (_operationMethodComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (operationMethodCallback ( )));
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);
	
	// Définition de la discrétisation :
	QtGroupBox*		groupBox	=
						new QtGroupBox (this, "Paramètres de discrétisation");
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	vlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (vlayout);
	layout->addWidget (groupBox);


	//  La saisie du nombre d'arêtes (mutualisé pour toutes les méthodes)
	hlayout	= new QHBoxLayout ( );
	vlayout->addLayout (hlayout);
	label	= new QLabel ("Nombre de bras :", this);
	hlayout->addWidget (label);
	label->setToolTip (QString::fromUtf8("Nouveau nombre de bras des arêtes."));
	_edgesNumTextField	= new QtIntTextField (10, this);
	hlayout->addWidget (_edgesNumTextField);
	_edgesNumTextField->setToolTip (QString::fromUtf8("Nouveau nombre de bras de l'arête de référence et des arêtes parallèles."));
	connect (_edgesNumTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (discretisationModifiedCallback ( )));


	// Le panneau "méthode" : plusieurs panneaux possibles.
	// On garde un parent unique dans la layout :
	_currentParentWidget	= new QWidget (groupBox);
	vlayout->addWidget (_currentParentWidget);
	QHBoxLayout*	currentLayout = new QHBoxLayout (_currentParentWidget);
	_currentParentWidget->setLayout (currentLayout);
	_edgeListPanel	= new QtEdgeListMeshingPropertyPanel (0, mainWindow, _edgesNumTextField);
	_edgeListPanel->hide ( );
	connect (_edgeListPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edgeListPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_edgeListPanel, SIGNAL (discretisationModified ( )),
	         this, SLOT (parametersModifiedCallback ( )));
	connect (_edgeListPanel, SIGNAL (algorithmModified ( )),
	         this, SLOT (algorithmModifiedCallback ( )));
	_edgesRefinementPanel	=
			new QtTopologyEdgesRefinementPanel (0, mainWindow);
	_edgesRefinementPanel->hide ( );
	connect (_edgesRefinementPanel,
	         SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edgesRefinementPanel,
	         SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_edgesRefinementPanel,
	         SIGNAL (discretisationModified ( )),
	         this, SLOT (parametersModifiedCallback ( )));

	// Les arêtes à discrétiser :
	_edgesPanel	= new QtMgx3DEntityPanel (
						this, "", true, "Arêtes :", "", &mainWindow,
						SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	_edgesPanel->setMultiSelectMode (true);
	layout->addWidget (_edgesPanel);
	connect (_edgesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edgesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// Réinitialiser à partir de la sélection ?
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QPushButton*	button	=
			new QPushButton ("Actualiser le panneau", this);
	hlayout->addWidget (button);
	button->setToolTip (QString::fromUtf8("Actualise - si possible - le panneau avec les paramètres des arêtes correspondantes."));
	connect (button, SIGNAL (clicked (bool)), this,
	         SLOT (reinitializeCallback ( )));
	hlayout->addStretch (2);

	// aperçu avec projection ou non
	_previewProjectedCheckBox	= new QCheckBox (QString::fromUtf8("Projection de l'aperçu"), this);
	_previewProjectedCheckBox->setCheckState (Qt::Checked);
	layout->addWidget (_previewProjectedCheckBox);
	connect (_previewProjectedCheckBox, SIGNAL (stateChanged (int)), this,
			SLOT (operationMethodCallback ( )));

	addPreviewCheckBox (true);

	operationMethodCallback ( );

	vlayout->addStretch (2);
	layout->addStretch (2);
}	// QtEdgeMeshingPropertyPanel::QtEdgeMeshingPropertyPanel


QtEdgeMeshingPropertyPanel::QtEdgeMeshingPropertyPanel (
										const QtEdgeMeshingPropertyPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _edgeListPanel (0), _edgesRefinementPanel (0), _edgesPanel (0), _previewProjectedCheckBox(0), _edgesNumTextField(0)
{
	MGX_FORBIDDEN ("QtEdgeMeshingPropertyPanel copy constructor is not allowed.");
}	// QtEdgeMeshingPropertyPanel::QtEdgeMeshingPropertyPanel (const QtEdgeMeshingPropertyPanel&)


QtEdgeMeshingPropertyPanel& QtEdgeMeshingPropertyPanel::operator = (
											const QtEdgeMeshingPropertyPanel&)
{
	MGX_FORBIDDEN ("QtEdgeMeshingPropertyPanel assignment operator is not allowed.");
	return *this;
}	// QtEdgeMeshingPropertyPanel::QtEdgeMeshingPropertyPanel (const QtEdgeMeshingPropertyPanel&)


QtEdgeMeshingPropertyPanel::~QtEdgeMeshingPropertyPanel ( )
{
}	// QtEdgeMeshingPropertyPanel::~QtEdgeMeshingPropertyPanel


QtEdgeMeshingPropertyPanel::OPERATION_METHOD
						QtEdgeMeshingPropertyPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtEdgeMeshingPropertyPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtEdgeMeshingPropertyPanel::getOperationMethod


CoEdgeMeshingProperty* QtEdgeMeshingPropertyPanel::getMeshingProperty ( ) const
{
	CHECK_NULL_PTR_ERROR (_currentPanel)

	return _currentPanel->getMeshingProperty ( );
}	// QtEdgeMeshingPropertyPanel::getMeshingProperty


vector<string> QtEdgeMeshingPropertyPanel::getEdgesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgesPanel)

	return _edgesPanel->getUniqueNames ( );
}	// QtEdgeMeshingPropertyPanel::getEdgesNames


void QtEdgeMeshingPropertyPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgeListPanel)
	CHECK_NULL_PTR_ERROR (_edgesRefinementPanel)
	CHECK_NULL_PTR_ERROR (_edgesPanel)
	CHECK_NULL_PTR_ERROR (_edgesNumTextField)
	_edgeListPanel->reset ( );
	_edgesRefinementPanel->reset ( );
	_edgesPanel->reset ( );
	_edgesNumTextField->setValue (10);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtEdgeMeshingPropertyPanel::reset


void QtEdgeMeshingPropertyPanel::validate ( )
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
		error << "QtEdgeMeshingPropertyPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtEdgeMeshingPropertyPanel::EDGE_LIST				:
		case QtEdgeMeshingPropertyPanel::PARALLEL_EDGES			:
		case QtEdgeMeshingPropertyPanel::CONSTRAINED_EDGES		:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode de discrétisation d'arêtes "
			      << "sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtEdgeMeshingPropertyPanel::validate : index de méthode "
			      << "d'opération de discrétisation d'arêtes invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtEdgeMeshingPropertyPanel::validate


void QtEdgeMeshingPropertyPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );	// => highlight (false)

	CHECK_NULL_PTR_ERROR (_edgesPanel)
	_edgesPanel->stopSelection ( );

	if (0 != _currentPanel)
		_currentPanel->cancel ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_edgesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtEdgeMeshingPropertyPanel::cancel


void QtEdgeMeshingPropertyPanel::autoUpdate ( )
{
	highlight (false);	// car champs de texte modifiés

	if (0 != _currentPanel)
		_currentPanel->autoUpdate ( );

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		CHECK_NULL_PTR_ERROR (_edgesPanel)
		const vector<string>	edges	= 
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
		UTF8String	text (Charset::UTF_8);
		for (vector<string>::const_iterator it = edges.begin ( );
		     edges.end ( ) != it; it++)
		{
			if (0 != text.length ( ))
				text << ' ';
			text << *it;
		}
		_edgesPanel->setUniqueName (text);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_edgesPanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationPanel::autoUpdate ( );

	_edgesPanel->actualizeGui (true);
}	// QtEdgeMeshingPropertyPanel::autoUpdate


QtEdgeMeshingPropertyPanelIfc* QtEdgeMeshingPropertyPanel::getCurrentPanel ( )
{
	return _currentPanel;
}	// QtEdgeMeshingPropertyPanel::getCurrentPanel


vector<Entity*> QtEdgeMeshingPropertyPanel::getInvolvedEntities ( )
{
	vector<Entity*>			entities;
	
	if (CONSTRAINED_EDGES != getOperationMethod ( ))
	{
		const vector<string>	names	= getEdgesNames ( );

		for (vector<string>::const_iterator it = names.begin ( );
		     names.end ( ) != it; it++)
			entities.push_back (getContext ( ).getTopoManager ( ).getEntity (
																	*it, true));
	}	// if (CONSTRAINED_EDGES != getOperationMethod ( ))

	vector<Entity*>	others;
	if (0 != _currentPanel)
		others	= _currentPanel->getPanelEntities ( );
	for (vector<Entity*>::iterator ito = others.begin ( );
	     others.end ( ) != ito; ito++)
		entities.push_back (*ito);

	return entities;
}	// QtEdgeMeshingPropertyPanel::getInvolvedEntities


void QtEdgeMeshingPropertyPanel::operationMethodCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgesPanel)
	CHECK_NULL_PTR_ERROR (_edgeListPanel)
	CHECK_NULL_PTR_ERROR (_edgesRefinementPanel)
	if (0 != _currentPanel)
	{
		preview (false, true);
		highlight (false);
		_currentParentWidget->layout ( )->removeWidget (_currentPanel);
		_currentPanel->hide ( );
		_currentPanel->setParent (0);
	}
	_currentPanel	= 0;

	// Pour la plupart des cas :
	_edgesPanel->getNameTextField ( )->setLinkedSeizureManagers (0, 0);
	_edgesPanel->setMultiSelectMode (true);
	_edgesPanel->show ( );

	switch (getOperationMethod ( ))
	{
		case QtEdgeMeshingPropertyPanel::PARALLEL_EDGES			:
			_currentPanel	= _edgeListPanel; // [EB] à faire avant setMultiSelectMode, car _currentPanel est utilisé, alors qu'il vaut 0 sinon
			_edgesPanel->setMultiSelectMode (false);
			break;
		case QtEdgeMeshingPropertyPanel::EDGE_LIST				:
			_currentPanel	= _edgeListPanel;
			break;
		case QtEdgeMeshingPropertyPanel::CONSTRAINED_EDGES		:
			_currentPanel	= _edgesRefinementPanel;
			_edgesPanel->hide ( );
			break;
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode d'opération non supportée ("
			        << (unsigned long)getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtEdgeMeshingPropertyPanel::operationMethodCallback")
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
		EntitySeizureManager*	first	=
									_currentPanel->getFirstSeizureManager ( );
		EntitySeizureManager*	last	=
									_currentPanel->getLastSeizureManager ( );
		_edgesPanel->getNameTextField ( )->setLinkedSeizureManagers (first, 0);
		if (0 != last)
		{
			first	= last->getPreviousSeizureManager ( );
			if (false == _edgesPanel->isHidden ( ))
				last->setLinkedSeizureManagers (
									first, _edgesPanel->getNameTextField ( ));
		}	// if (0 != last)
	}	// if (0 != _currentPanel)
	else {
		UTF8String	message (Charset::UTF_8);
		message << "Référence nulle vers un panneau courant";
		INTERNAL_ERROR (exc, message, "QtEdgeMeshingPropertyPanel::operationMethodCallback")
		throw exc;
	}

	parametersModifiedCallback ( );
	highlight (true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtEdgeMeshingPropertyPanel::operationMethodCallback


void QtEdgeMeshingPropertyPanel::algorithmModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgesPanel)
	CHECK_NULL_PTR_ERROR (_edgeListPanel)
	CHECK_NULL_PTR_ERROR (_edgesRefinementPanel)
	if (0 != _currentPanel)
	{
		preview (false, true);
		highlight (false);
	}

	// Pour la plupart des cas :
	_edgesPanel->getNameTextField ( )->setLinkedSeizureManagers (0, 0);

	switch (getOperationMethod ( ))
	{
		case QtEdgeMeshingPropertyPanel::PARALLEL_EDGES			:
		case QtEdgeMeshingPropertyPanel::EDGE_LIST				:
			_currentPanel	= _edgeListPanel;						break;
		case QtEdgeMeshingPropertyPanel::CONSTRAINED_EDGES		:
			_currentPanel	= _edgesRefinementPanel;				break;
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode d'opération non supportée ("
			        << (unsigned long)getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtEdgeMeshingPropertyPanel::operationMethodCallback")
			throw exc;
		}
	}	// switch (getOperationMethod ( ))
	if (0 != _currentPanel)
	{
		EntitySeizureManager*	first	=
									_currentPanel->getFirstSeizureManager ( );
		EntitySeizureManager*	last	=
									_currentPanel->getLastSeizureManager ( );
		_edgesPanel->getNameTextField ( )->setLinkedSeizureManagers (first, 0);
		if (0 != last)
		{
			first	= last->getPreviousSeizureManager ( );
			last->setLinkedSeizureManagers (
									first, _edgesPanel->getNameTextField ( ));
		}	// if (0 != last)
	}	// if (0 != _currentPanel)

	preview (true, false);
	highlight (true);

	parametersModifiedCallback ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtEdgeMeshingPropertyPanel::algorithmModifiedCallback


void QtEdgeMeshingPropertyPanel::reinitializeCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_currentPanel)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)

	const vector<string>	names	= getEdgesNames ( );
//			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
	if (0 == names.size ( ))
		throw Exception (UTF8String ("Réinitialisation du panneau impossible : absence d'arête sélectionnée.", Charset::UTF_8));

	vector<string>::const_iterator itn	= names.begin ( );
	const CoEdge*	ref	=	
					getContext ( ).getTopoManager ( ).getCoEdge (*itn, true);
	CHECK_NULL_PTR_ERROR (ref)
	const CoEdgeMeshingProperty*	properties	= ref->getMeshingProperty ( );
	itn++;
	for ( ; itn != names.end ( ); itn++)
	{
		const CoEdge*	edge	=
					getContext ( ).getTopoManager ( ).getCoEdge (*itn, true);
		CHECK_NULL_PTR_ERROR (edge)
		const CoEdgeMeshingProperty*	props	= edge->getMeshingProperty ( );

		if (*props != *properties)
			throw Exception (UTF8String ("Réinitialisation du panneau impossible : arêtes de paramètres de discrétisation différents.", Charset::UTF_8));
	}	// for ( ; itn != names.end ( ); itn++)

//	if (1 == properties->getNbEdges ( ))
//		_operationMethodComboBox->setCurrentIndex (
//								QtEdgeMeshingPropertyPanel::PARALLEL_EDGES);
//	else
//		_operationMethodComboBox->setCurrentIndex (
//								QtEdgeMeshingPropertyPanel::EDGE_LIST);

	operationMethodCallback ( );

	getCurrentPanel ( )->setMeshingProperty (*properties);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtEdgeMeshingPropertyPanel::reinitializeCallback


void QtEdgeMeshingPropertyPanel::preview (bool show, bool destroyInteractor)
{
	QtMgx3DOperationPanel::preview (show, destroyInteractor);

	if ((false == show) || (false ==  previewResult ( )))
		return;

	bool edge_preview_activated = false;

	BEGIN_QT_TRY_CATCH_BLOCK

	// Dispositif différent pour CONSTRAINED_EDGES :
	if (CONSTRAINED_EDGES == getOperationMethod ( ))
		return;

	const vector<string>			edges	= getEdgesNames ( );
	edge_preview_activated = !edges.empty();
	unique_ptr<CoEdgeMeshingProperty>	properties (getMeshingProperty ( ));
	DisplayProperties	graphicalProps;
	graphicalProps.setCloudColor (Color (
					255 * Resources::instance ( )._previewColor.getRed ( ),
					255 * Resources::instance ( )._previewColor.getGreen ( ),
					255 * Resources::instance ( )._previewColor.getBlue ( )));
	graphicalProps.setPointSize (
						Resources::instance ( )._previewPointSize.getValue ( ));
	for (vector<string>::const_iterator ite = edges.begin ( );
	     ite != edges.end ( ); ite++)
	{
		CoEdge*	coEdge	= getContext ( ).getTopoManager ( ).getCoEdge (*ite);
		CHECK_NULL_PTR_ERROR (coEdge)
		vector<Math::Point>	points;
		coEdge->getPoints (properties.get( ), points, Qt::Checked == _previewProjectedCheckBox->checkState());
		RenderingManager::RepresentationID	edgeID	=
			getRenderingManager ( ).createCloudRepresentation (
												points, graphicalProps, true);
		registerPreviewedObject (edgeID);
	}	// for (vector<string>::const_iterator ite = edges.begin ( ); ...

	getRenderingManager ( ).forceRender ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (false, this, "Magix 3D")

	if (hasError)
		getContext ( ).getLogStream()->log(TraceLog (errorString, Log::WARNING));
	else if (edge_preview_activated)
		getContext ( ).getLogStream()->log(TraceLog (UTF8String ("Prévisualisation de la discrétisation ...", Charset::UTF_8), Log::TRACE_1));
}	// QtEdgeMeshingPropertyPanel::preview


void QtEdgeMeshingPropertyPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _edgesPanel)
		_edgesPanel->stopSelection ( );
	if (0 != _currentPanel)
		_currentPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtEdgeMeshingPropertyPanel::operationCompleted


size_t QtEdgeMeshingPropertyPanel::getEdgesNum ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgesNumTextField)
	return _edgesNumTextField->getValue ( );
}	// QtEdgeMeshingPropertyPanel::getEdgesNum


// ===========================================================================
//                  LA CLASSE QtEdgeMeshingPropertyAction
// ===========================================================================

QtEdgeMeshingPropertyAction::QtEdgeMeshingPropertyAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtEdgeMeshingPropertyPanel*	operationPanel	=
		new QtEdgeMeshingPropertyPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtEdgeMeshingPropertyAction::QtEdgeMeshingPropertyAction


QtEdgeMeshingPropertyAction::QtEdgeMeshingPropertyAction (
										const QtEdgeMeshingPropertyAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtEdgeMeshingPropertyAction copy constructor is not allowed.")
}	// QtEdgeMeshingPropertyAction::QtEdgeMeshingPropertyAction


QtEdgeMeshingPropertyAction& QtEdgeMeshingPropertyAction::operator = (
										const QtEdgeMeshingPropertyAction&)
{
	MGX_FORBIDDEN ("QtEdgeMeshingPropertyAction assignment operator is not allowed.")
	return *this;
}	// QtEdgeMeshingPropertyAction::operator =


QtEdgeMeshingPropertyAction::~QtEdgeMeshingPropertyAction ( )
{
}	// QtEdgeMeshingPropertyAction::~QtEdgeMeshingPropertyAction


QtEdgeMeshingPropertyPanel*
					QtEdgeMeshingPropertyAction::getMeshingPropertyPanel ( )
{
	return dynamic_cast<QtEdgeMeshingPropertyPanel*>(getOperationPanel ( ));
}	// QtEdgeMeshingPropertyAction::getMeshingPropertyPanel


void QtEdgeMeshingPropertyAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres de discrétisation des arêtes :
	QtEdgeMeshingPropertyPanel*	panel	= getMeshingPropertyPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>					edges		= panel->getEdgesNames ( );

	switch (panel->getOperationMethod ( ))
	{
		case QtEdgeMeshingPropertyPanel::EDGE_LIST			:
		{
			unique_ptr<CoEdgeMeshingProperty>	properties (panel->getMeshingProperty ( ));
			cmdResult	= getContext ( ).getTopoManager ( ).setMeshingProperty (*properties.get ( ), edges);
		}
		break;
		case QtEdgeMeshingPropertyPanel::PARALLEL_EDGES		:
		{
			if (1 != edges.size ( ))
			{
				UTF8String	error (Charset::UTF_8);
				error << "Discrétisation d'arêtes parallèles : une et une "
				      << "seule arête doit être sélectionnée.";
				throw Exception (error);
			}	// if (1 != edges.size ( ))
			unique_ptr<CoEdgeMeshingProperty>	properties (panel->getMeshingProperty ( ));
			cmdResult	= getContext ( ).getTopoManager ( ).setParallelMeshingProperty (*properties.get ( ), edges [0]);
		}
		break;
		case QtEdgeMeshingPropertyPanel::CONSTRAINED_EDGES	:
		{
			QtTopologyEdgesRefinementPanel*	erp	=
						dynamic_cast<QtTopologyEdgesRefinementPanel*>(panel->getCurrentPanel ( ));
			CHECK_NULL_PTR_ERROR (erp)
			string			name	= erp->getRefinedReferenceEdgeName ( );
			vector<string>	names	= erp->getFixedEdgeNames ( );
			int				number	= panel->getEdgesNum ( );
			cmdResult	= getContext ( ).getTopoManager ( ).setNbMeshingEdges (name, number, names);
		}
		break;
		default												:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode d'opération non supportée ("
			        << (unsigned long)panel->getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtEdgeMeshingPropertyAction::executeOperation")
			throw exc;
		}
	}	// switch (panel->getOperationMethod ( ))

	setCommandResult (cmdResult);
}	// QtEdgeMeshingPropertyAction::executeOperation



// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
