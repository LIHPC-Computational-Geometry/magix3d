/**
 * \file        QtFaceMeshingPropertyAction.cpp
 * \author      Charles PIGNEROL
 * \date        05/11/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/ValidatedField.h"
#include "Internal/Resources.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtFaceMeshingPropertyAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
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
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtFaceTransfinitePanel
// ===========================================================================


QtFaceTransfinitePanel::QtFaceTransfinitePanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel)
{
//	setContentsMargins (0, 0, 0, 0);
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);
}	// QtFaceTransfinitePanel::QtFaceTransfinitePanel


QtFaceTransfinitePanel::QtFaceTransfinitePanel (const QtFaceTransfinitePanel& p)
	  : QtMgx3DOperationsSubPanel (p)
{
	MGX_FORBIDDEN ("QtFaceTransfinitePanel copy constructor is not allowed.");
}	// QtFaceTransfinitePanel::QtFaceTransfinitePanel


QtFaceTransfinitePanel& QtFaceTransfinitePanel::operator = (
												const QtFaceTransfinitePanel&)
{
	 MGX_FORBIDDEN ("QtFaceTransfinitePanel assignment operator is not allowed.");
	return *this;
}	// QtFaceTransfinitePanel::operator =


QtFaceTransfinitePanel::~QtFaceTransfinitePanel ( )
{
}	// QtFaceTransfinitePanel::~QtFaceTransfinitePanel


FaceMeshingPropertyTransfinite* QtFaceTransfinitePanel::getMeshingProperty ( ) const
{
	return new FaceMeshingPropertyTransfinite ( );
}	// QtFaceTransfinitePanel::getMeshingProperty


// ===========================================================================
//                         LA CLASSE QtFaceDirectionalPanel
// ===========================================================================


QtFaceDirectionalPanel::QtFaceDirectionalPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel),
	  _edgeTextField (0), _invertCheckBox (0), _directionLabel (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));

	// La direction du maillage :
	_edgeTextField	= new QtMgx3DEdgePanel (
				this, "", true, "", &window, FilterEntity::TopoCoEdge);
	_edgeTextField->getEntityPanel ( ).setLabel ("Arête directionnelle :");
	layout->addWidget (_edgeTextField);
	connect (&_edgeTextField->getEntityTextField ( ),
	         SIGNAL (selectionModified (QString)), this,
	         SLOT (directionModifiedCallback ( )));

	// Faut il inverser le vecteur ?
	_invertCheckBox	= new QCheckBox ("Inverser le vecteur", this);
	layout->addWidget (_invertCheckBox);
	connect (_invertCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (directionModifiedCallback ( )));

	// Le vecteur :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label	= new QLabel ("Direction :", this);
	hlayout->addWidget (label);
	_directionLabel	= new QLabel ("", this);
	hlayout->addWidget (_directionLabel);
	hlayout->addStretch (2.);
}	// QtFaceDirectionalPanel::QtFaceDirectionalPanel


QtFaceDirectionalPanel::QtFaceDirectionalPanel (
	const QtFaceDirectionalPanel& p)
	: QtMgx3DOperationsSubPanel (p),
	  _edgeTextField (0), _invertCheckBox (0), _directionLabel (0)
{
	MGX_FORBIDDEN ("QtFaceDirectionalPanel copy constructor is not allowed.");
}	// QtFaceDirectionalPanel::QtFaceDirectionalPanel


QtFaceDirectionalPanel& QtFaceDirectionalPanel::operator = (
												const QtFaceDirectionalPanel&)
{
	 MGX_FORBIDDEN ("QtFaceDirectionalPanel assignment operator is not allowed.");
	return *this;
}	// QtFaceDirectionalPanel::operator =


QtFaceDirectionalPanel::~QtFaceDirectionalPanel ( )
{
}	// QtFaceDirectionalPanel::~QtFaceDirectionalPanel


void QtFaceDirectionalPanel::setVisible (bool visible)
{
	preview (visible, true);
	QtMgx3DOperationsSubPanel::setVisible (visible);
}	// QtFaceDirectionalPanel::setVisible


Math::Point QtFaceDirectionalPanel::getPoint1 ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgeTextField)
	const CoEdge*	edge	= getContext ( ).getTopoManager ( ).getCoEdge (
											_edgeTextField->getUniqueName( ));
	CHECK_NULL_PTR_ERROR (edge)
	size_t	index	= false == isEdgeInverted ( ) ?
					  0 : edge->getNbVertices ( ) - 1;
	const Vertex*	v1	= edge->getVertex (index);
	CHECK_NULL_PTR_ERROR (v1)

	return v1->getCoord ( );
}	// QtFaceDirectionalPanel::getPoint1


Math::Point QtFaceDirectionalPanel::getPoint2 ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgeTextField)
	const CoEdge*	edge	= getContext ( ).getTopoManager ( ).getCoEdge (
											_edgeTextField->getUniqueName( ));
	CHECK_NULL_PTR_ERROR (edge)
	size_t	index	= true == isEdgeInverted ( ) ?
					  0 : edge->getNbVertices ( ) - 1;
	const Vertex*	v2	= edge->getVertex (index);
	CHECK_NULL_PTR_ERROR (v2)

	return v2->getCoord ( );
}	// QtFaceDirectionalPanel::getPoint2


FaceMeshingPropertyDirectional* QtFaceDirectionalPanel::getMeshingProperty ( ) const
{
	return new FaceMeshingPropertyDirectional (getPoint1 ( ), getPoint2 ( ));
}	// QtFaceDirectionalPanel::getMeshingProperty


bool QtFaceDirectionalPanel::isEdgeInverted ( ) const
{
	CHECK_NULL_PTR_ERROR (_invertCheckBox)
	return Qt::Checked == _invertCheckBox->checkState ( ) ? true : false;
}	// QtFaceDirectionalPanel::isEdgeInverted


void QtFaceDirectionalPanel::stopSelection ( )
{
	if (0 != _edgeTextField)
		_edgeTextField->stopSelection ( );
}	// QtFaceDirectionalPanel::stopSelection


QtEntityIDTextField* QtFaceDirectionalPanel::getEdgeTextField ( )
{
	CHECK_NULL_PTR_ERROR (_edgeTextField)
	return &_edgeTextField->getEntityTextField ( );
}	// QtFaceDirectionalPanel::getEdgeTextField


void QtFaceDirectionalPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgeTextField)
	_edgeTextField->reset ( );

	CHECK_NULL_PTR_ERROR(_directionLabel)
	_directionLabel->clear();

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationsSubPanel::reset ( );
}	// QtFaceDirectionalPanel::reset


void QtFaceDirectionalPanel::preview (bool on, bool destroyInteractor)
{
	Math::Point	p1, p2;
	try
	{
		CHECK_NULL_PTR_ERROR (_directionLabel)
		p1	= getPoint1 ( );
		p2	= getPoint2 ( );
		UTF8String	label (Charset::UTF_8);
		label << "(" << MgxNumeric::userRepresentation (p1.getX ( ))
		      << ", " << MgxNumeric::userRepresentation (p1.getY ( )) << ",\n"
		      << MgxNumeric::userRepresentation (p1.getZ ( )) << ") -> ("
		      << MgxNumeric::userRepresentation (p2.getX ( ))
		      << ",\n" << MgxNumeric::userRepresentation (p2.getY ( )) << ", "
		      << MgxNumeric::userRepresentation (p2.getZ ( ))
		      << ")";
		_directionLabel->setText (label.ascii ( ).c_str ( ));
	}
	catch (...)
	{
	}

	try
	{	// Quoi qu'il arrive on détruit l'éventuel aperçu existant :
		removePreviewedObjects ( );
	}
	catch (...)
	{
	}

	try
	{
		if (true == previewResult ( ))
		{
			if (true == on)
			{
				DisplayProperties	properties;
				properties.setWireColor (Color (
						255 * Resources::instance ( )._previewColor.getRed ( ),
						255 * Resources::instance ( )._previewColor.getGreen ( ),
						255 * Resources::instance ( )._previewColor.getBlue ( )));
				properties.setLineWidth (Resources::instance ( )._previewWidth.getValue( ));
				RenderingManager::RepresentationID	arrowID	=
					getRenderingManager ( ).createVector (p1, p2, properties, true);
				registerPreviewedObject (arrowID);
			}	// if (true == on)
		}	// if (true == previewResult ( ))
	}
	catch (...)
	{
	}

	try
	{
		getRenderingManager ( ).forceRender ( );
	}
	catch (...)
	{
	}
}	// QtFaceDirectionalPanel::preview


void QtFaceDirectionalPanel::directionModifiedCallback ( )
{
	preview (true, true);
}	// QtFaceDirectionalPanel::directionModifiedCallback


// ===========================================================================
//                        LA CLASSE QtFaceOrthogonalPanel
// ===========================================================================


QtFaceOrthogonalPanel::QtFaceOrthogonalPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtFaceDirectionalPanel (parent, window, mainPanel),
	  _layersNumTextField (0)
{
	QBoxLayout*		mainLayout	= dynamic_cast<QBoxLayout*>(layout ( ));
	if (0 == mainLayout)
	{
		INTERNAL_ERROR (exc, "Mauvais type de layout.", "QtFaceOrthogonalPanel::QtFaceOrthogonalPanel")
		throw exc;
	}	// if (0 == mainLayout)
	QHBoxLayout*	hlayout		= new QHBoxLayout ( );
	mainLayout->addLayout (hlayout);
	QLabel*	label	= new QLabel ("Nombre de couches :", this);
	hlayout->addWidget (label);
	_layersNumTextField	= new QtIntTextField (this);
	_layersNumTextField->setValue (5);
	hlayout->addWidget (_layersNumTextField);
	_layersNumTextField->setRange (0, NumericServices::intMachMax ( ));
}	// QtFaceOrthogonalPanel::QtFaceOrthogonalPanel


QtFaceOrthogonalPanel::QtFaceOrthogonalPanel (const QtFaceOrthogonalPanel&)
	: QtFaceDirectionalPanel (0,*new QtMgx3DMainWindow(0), 0),
	  _layersNumTextField (0)
{
	MGX_FORBIDDEN ("QtFaceDelaunayGMSHPanel copy constructor is not allowed.");
}	// QtFaceOrthogonalPanel::QtFaceOrthogonalPanel


QtFaceOrthogonalPanel& QtFaceOrthogonalPanel::operator = (
												const QtFaceOrthogonalPanel&)
{
	 MGX_FORBIDDEN ("QtFaceDelaunayGMSHPanel assignment operator is not allowed.");
	return *this;
}	// QtFaceOrthogonalPanel::operator =


QtFaceOrthogonalPanel::~QtFaceOrthogonalPanel ( )
{
}	// QtFaceOrthogonalPanel::~QtFaceOrthogonalPanel


void QtFaceOrthogonalPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_layersNumTextField)
	_layersNumTextField->setValue (5);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtFaceDirectionalPanel::reset ( );
}	// QtFaceOrthogonalPanel::reset


FaceMeshingPropertyOrthogonal*
						QtFaceOrthogonalPanel::getMeshingProperty ( ) const
{
	return new FaceMeshingPropertyOrthogonal (
							getPoint1 ( ), getPoint2 ( ), getLayersNum ( ));
}	// QtFaceOrthogonalPanel::getMeshingProperty


size_t QtFaceOrthogonalPanel::getLayersNum( ) const
{
	CHECK_NULL_PTR_ERROR (_layersNumTextField)
	return _layersNumTextField->getValue ( );
}	// QtFaceOrthogonalPanel::getLayersNum



// ===========================================================================
//                       LA CLASSE QtFaceRotationalPanel
// ===========================================================================


QtFaceRotationalPanel::QtFaceRotationalPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel),
	  _directionTextField (0), _axePoint1Panel (0), _axePoint2Panel (0),
	  _directionLabel (0), _axeLabel (0)
{
//	setContentsMargins (0, 0, 0, 0);
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));

	// La direction du maillage :
	QLabel*	label	= new QLabel ("Direction", this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);
	_directionTextField	= new QtMgx3DEdgePanel (
				this, "", true, "", &window, FilterEntity::TopoCoEdge);
	_directionTextField->getEntityPanel ( ).setLabel ("Arête directionnelle :");
	_directionTextField->layout ( )->setContentsMargins  (0, 0, 0, 0);
	_directionTextField->layout ( )->setSpacing  (0);
	_directionTextField->setFixedSize (_directionTextField->sizeHint ( ));
	layout->addWidget (_directionTextField);
	connect (&_directionTextField->getEntityTextField ( ),
	         SIGNAL (selectionModified (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel ("Direction :", this);
	hlayout->addWidget (label);
	_directionLabel	= new QLabel ("", this);
	hlayout->addWidget (_directionLabel);
	hlayout->addStretch (2.);

	// L'axe de rotation :
	label	= new QLabel ("Axe de rotation", this);
	label->setFont (font);
	layout->addWidget (label);
	_axePoint1Panel	= new QtMgx3DPointPanel (
			this, "", true, "x :", "y :", "z :",
			0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
			&window, FilterEntity::AllPoints, true);
	layout->addWidget (_axePoint1Panel);
	connect (_axePoint1Panel, SIGNAL (pointModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));
	_axePoint2Panel	= new QtMgx3DPointPanel (
			this, "", true, "x :", "y :", "z :",
			1., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
			&window, FilterEntity::AllPoints, true);
	layout->addWidget (_axePoint2Panel);
	connect (_axePoint2Panel, SIGNAL (pointModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));
	_axeLabel	= new QLabel ("", this);
	layout->addWidget (_axeLabel);

	parametersModifiedCallback ( );
}	// QtFaceRotationalPanel::QtFaceRotationalPanel


QtFaceRotationalPanel::QtFaceRotationalPanel (
	const QtFaceRotationalPanel& p)
	: QtMgx3DOperationsSubPanel (p), _directionTextField (0),
	  _axePoint1Panel (0), _axePoint2Panel (0),
	  _directionLabel (0), _axeLabel (0)
{
	MGX_FORBIDDEN ("QtFaceRotationalPanel copy constructor is not allowed.");
}	// QtFaceRotationalPanel::QtFaceRotationalPanel


QtFaceRotationalPanel& QtFaceRotationalPanel::operator = (
												const QtFaceRotationalPanel&)
{
	 MGX_FORBIDDEN ("QtFaceRotationalPanel assignment operator is not allowed.");
	return *this;
}	// QtFaceRotationalPanel::operator =


QtFaceRotationalPanel::~QtFaceRotationalPanel ( )
{
}	// QtFaceRotationalPanel::~QtFaceRotationalPanel


void QtFaceRotationalPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_directionTextField)
	CHECK_NULL_PTR_ERROR (_axePoint1Panel)
	CHECK_NULL_PTR_ERROR (_axePoint2Panel)
	_directionTextField->reset ( );
	_axePoint1Panel->reset ( );
	_axePoint2Panel->reset ( );

	CHECK_NULL_PTR_ERROR(_directionLabel)
	_directionLabel->clear();

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationsSubPanel::reset ( );
}	// QtFaceRotationalPanel::reset


void QtFaceRotationalPanel::setVisible (bool visible)
{
	preview (visible, true);
	QtMgx3DOperationsSubPanel::setVisible (visible);
}	// QtFaceRotationalPanel::setVisible


Math::Point QtFaceRotationalPanel::getDirPoint1 ( ) const
{
	CHECK_NULL_PTR_ERROR (_directionTextField)
	const CoEdge*	edge	= getContext ( ).getTopoManager ( ).getCoEdge (
									_directionTextField->getUniqueName( ));
	CHECK_NULL_PTR_ERROR (edge)
	const Vertex*	v1	= edge->getVertex (0);
	CHECK_NULL_PTR_ERROR (v1)

	return v1->getCoord ( );
}	// QtFaceRotationalPanel::getDirPoint1


Math::Point QtFaceRotationalPanel::getDirPoint2 ( ) const
{
	CHECK_NULL_PTR_ERROR (_directionTextField)
	const CoEdge*	edge	= getContext ( ).getTopoManager ( ).getCoEdge (
										_directionTextField->getUniqueName( ));
	CHECK_NULL_PTR_ERROR (edge)
	const Vertex*	v2	= edge->getVertex (edge->getNbVertices ( ) - 1);
	CHECK_NULL_PTR_ERROR (v2)

	return v2->getCoord ( );
}	// QtFaceRotationalPanel::getDirPoint2


Math::Point QtFaceRotationalPanel::getAxePoint1 ( ) const
{
	CHECK_NULL_PTR_ERROR (_axePoint1Panel)
	return _axePoint1Panel->getPoint();
}	// QtFaceRotationalPanel::getAxePoint1


Math::Point QtFaceRotationalPanel::getAxePoint2 ( ) const
{
	CHECK_NULL_PTR_ERROR (_axePoint2Panel)
	return _axePoint2Panel->getPoint();
}	// QtFaceRotationalPanel::getAxePoint2


FaceMeshingPropertyRotational* QtFaceRotationalPanel::getMeshingProperty ( ) const
{
	return new FaceMeshingPropertyRotational (
		getDirPoint1 ( ), getDirPoint2 ( ), getAxePoint1 ( ), getAxePoint2 ( ));
}	// QtFaceRotationalPanel::getMeshingProperty


void QtFaceRotationalPanel::stopSelection ( )
{
	if (0 != _directionTextField)
		_directionTextField->stopSelection ( );
	if (0 != _axePoint1Panel)
		_axePoint1Panel->stopSelection ( );
	if (0 != _axePoint2Panel)
		_axePoint2Panel->stopSelection ( );
}	// QtFaceRotationalPanel::stopSelection


QtEntityIDTextField* QtFaceRotationalPanel::getEdgeTextField ( )
{
	CHECK_NULL_PTR_ERROR (_directionTextField)
	return &_directionTextField->getEntityTextField ( );
}	// QtFaceRotationalPanel::getEdgeTextField


QtEntityIDTextField* QtFaceRotationalPanel::getAxePoint1TextField ( )
{
	CHECK_NULL_PTR_ERROR (_axePoint1Panel)
	return _axePoint1Panel->getVertexTextField ( );
}	// QtFaceRotationalPanel::getAxePoint1TextField


QtEntityIDTextField* QtFaceRotationalPanel::getAxePoint2TextField ( )
{
	CHECK_NULL_PTR_ERROR (_axePoint2Panel)
	return _axePoint2Panel->getVertexTextField ( );
}	// QtFaceRotationalPanel::getAxePoint2TextField


void QtFaceRotationalPanel::preview (bool on, bool destroyInteractor)
{
	try
	{
		CHECK_NULL_PTR_ERROR (_directionLabel)
		Math::Point		p1	= getDirPoint1 ( ), p2	= getDirPoint2 ( );
		UTF8String	label (Charset::UTF_8);
		label << "(" << MgxNumeric::userRepresentation (p1.getX ( ))
		      << ", " << MgxNumeric::userRepresentation (p1.getY ( )) << ",\n"
		      << MgxNumeric::userRepresentation (p1.getZ ( )) << ") -> ("
		      << MgxNumeric::userRepresentation (p2.getX ( ))
		      << ",\n" << MgxNumeric::userRepresentation (p2.getY ( )) << ", "
		      << MgxNumeric::userRepresentation (p2.getZ ( ))
		      << ")";
		_directionLabel->setText (label.ascii ( ).c_str ( ));
	}
	catch (...)
	{
	}

	try
	{
		CHECK_NULL_PTR_ERROR (_axeLabel)
		Math::Point		p1	= getAxePoint1 ( ), p2	= getAxePoint2 ( );
		UTF8String	label (Charset::UTF_8);
		label << "(" << MgxNumeric::userRepresentation (p1.getX ( ))
		      << ", " << MgxNumeric::userRepresentation (p1.getY ( )) << ", "
		      << MgxNumeric::userRepresentation (p1.getZ ( )) << ") -> ("
		      << MgxNumeric::userRepresentation (p2.getX ( ))
		      << ", " << MgxNumeric::userRepresentation (p2.getY ( )) << ", "
		      << MgxNumeric::userRepresentation (p2.getZ ( ))
		      << ")";
		_axeLabel->setText (label.ascii ( ).c_str ( ));
	}
	catch (...)
	{
	}

	try
	{	// Quoi qu'il arrive on détruit l'éventuel aperçu existant :
		removePreviewedObjects ( );
	}
	catch (...)
	{
	}

	if (false == previewResult ( ))
		return;

	DisplayProperties	properties;
	properties.setWireColor (Color (
						255 * Resources::instance ( )._previewColor.getRed ( ),
						255 * Resources::instance ( )._previewColor.getGreen ( ),
						255 * Resources::instance ( )._previewColor.getBlue ( )));
	properties.setLineWidth (Resources::instance ( )._previewWidth.getValue ( ));

	try
	{
		if (true == on)
		{
			Math::Point			p1	= getAxePoint1 ( ), p2	= getAxePoint2 ( );
			RenderingManager::RepresentationID	axeID	=
					getRenderingManager ( ).createRotationAxe (
													p1, p2, properties, true);
			registerPreviewedObject (axeID);
		}	// if (true == on)
	}
	catch (...)
	{
	}
	try
	{
		if (true == on)
		{
			Math::Point			p1	= getDirPoint1 ( ), p2	= getDirPoint2 ( );
			RenderingManager::RepresentationID	directionID	=
				getRenderingManager ( ).createSegment(p1, p2, properties, true);
			registerPreviewedObject (directionID);
		}	// if (true == on)
	}
	catch (...)
	{
	}

	try
	{
		getRenderingManager ( ).forceRender ( );
	}
	catch (...)
	{
	}
}	// QtFaceRotationalPanel::preview


// ===========================================================================
//                        LA CLASSE QtFaceDelaunayGMSHPanel
// ===========================================================================


QtFaceDelaunayGMSHPanel::QtFaceDelaunayGMSHPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel),
	  _minTextField (0), _maxTextField (0)
{
//	setContentsMargins (0, 0, 0, 0);
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));

	FaceMeshingPropertyDelaunayGMSH	props;	// Pour récupérer les valeurs par défaut

	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label	= new QLabel ("Minimum :", this);
	hlayout->addWidget (label);
	_minTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_minTextField->setValue (props.getMin ( ));
	hlayout->addWidget (_minTextField);
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel ("Maximum :", this);
	hlayout->addWidget (label);
	_maxTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_maxTextField->setValue (props.getMax ( ));
	hlayout->addWidget (_maxTextField);
}	// QtFaceDelaunayGMSHPanel::QtFaceDelaunayGMSHPanel


QtFaceDelaunayGMSHPanel::QtFaceDelaunayGMSHPanel (
											const QtFaceDelaunayGMSHPanel& p)
	  : QtMgx3DOperationsSubPanel (p), _minTextField (0), _maxTextField (0)
{
	MGX_FORBIDDEN ("QtFaceDelaunayGMSHPanel copy constructor is not allowed.");
}	// QtFaceDelaunayGMSHPanel::QtFaceDelaunayGMSHPanel


QtFaceDelaunayGMSHPanel& QtFaceDelaunayGMSHPanel::operator = (
												const QtFaceDelaunayGMSHPanel&)
{
	 MGX_FORBIDDEN ("QtFaceDelaunayGMSHPanel assignment operator is not allowed.");
	return *this;
}	// QtFaceDelaunayGMSHPanel::operator =


QtFaceDelaunayGMSHPanel::~QtFaceDelaunayGMSHPanel ( )
{
}	// QtFaceDelaunayGMSHPanel::~QtFaceDelaunayGMSHPanel


void QtFaceDelaunayGMSHPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	FaceMeshingPropertyDelaunayGMSH	props;	// Pour récupérer les valeurs par défaut
	CHECK_NULL_PTR_ERROR (_minTextField)
	CHECK_NULL_PTR_ERROR (_maxTextField)
	_minTextField->setValue (props.getMin ( ));
	_maxTextField->setValue (props.getMax ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationsSubPanel::reset ( );
}	// QtFaceDelaunayGMSHPanel::reset


FaceMeshingPropertyDelaunayGMSH*
						QtFaceDelaunayGMSHPanel::getMeshingProperty ( ) const
{
	return new FaceMeshingPropertyDelaunayGMSH (getMin ( ), getMax ( ));
}	// QtFaceDelaunayGMSHPanel::getMeshingProperty


double QtFaceDelaunayGMSHPanel::getMin ( ) const
{
	CHECK_NULL_PTR_ERROR (_minTextField)
	return _minTextField->getValue ( );
}	// QtFaceDelaunayGMSHPanel::getMin


double QtFaceDelaunayGMSHPanel::getMax ( ) const
{
	CHECK_NULL_PTR_ERROR (_maxTextField)
	return _maxTextField->getValue ( );
}	// QtFaceDelaunayGMSHPanel::getMax


// ===========================================================================
//                        LA CLASSE QtFaceQuadPairingPanel
// ===========================================================================


QtFaceQuadPairingPanel::QtFaceQuadPairingPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel),
	  _minTextField (0), _maxTextField (0)
{
//	setContentsMargins (0, 0, 0, 0);
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));

	FaceMeshingPropertyQuadPairing props;	// Pour récupérer les valeurs par défaut

	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label	= new QLabel ("Minimum :", this);
	hlayout->addWidget (label);
	_minTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_minTextField->setValue (props.getMin ( ));
	hlayout->addWidget (_minTextField);
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel ("Maximum :", this);
	hlayout->addWidget (label);
	_maxTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_maxTextField->setValue (props.getMax ( ));
	hlayout->addWidget (_maxTextField);
}	// QtFaceQuadPairingPanel::QtFaceQuadPairingPanel


QtFaceQuadPairingPanel::QtFaceQuadPairingPanel (
											const QtFaceQuadPairingPanel& p)
	  : QtMgx3DOperationsSubPanel (p), _minTextField (0), _maxTextField (0)
{
	MGX_FORBIDDEN ("QtFaceQuadPairingPanel copy constructor is not allowed.");
}	// QtFaceQuadPairingPanel::QtFaceQuadPairingPanel


QtFaceQuadPairingPanel& QtFaceQuadPairingPanel::operator = (
												const QtFaceQuadPairingPanel&)
{
	 MGX_FORBIDDEN ("QtFaceQuadPairingPanel assignment operator is not allowed.");
	return *this;
}	// QtFaceQuadPairingPanel::operator =


QtFaceQuadPairingPanel::~QtFaceQuadPairingPanel ( )
{
}	// QtFaceQuadPairingPanel::~QtFaceDelaunayGMSHPanel


void QtFaceQuadPairingPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	FaceMeshingPropertyQuadPairing props;	// Pour récupérer les valeurs par défaut

	CHECK_NULL_PTR_ERROR (_minTextField)
	CHECK_NULL_PTR_ERROR (_maxTextField)
	_minTextField->setValue (props.getMin ( ));
	_maxTextField->setValue (props.getMax ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationsSubPanel::reset ( );
}	// QtFaceQuadPairingPanel::reset


FaceMeshingPropertyQuadPairing*
						QtFaceQuadPairingPanel::getMeshingProperty ( ) const
{
	return new FaceMeshingPropertyQuadPairing(getMin ( ), getMax ( ));
}	// QtFaceQuadPairingPanel::getMeshingProperty


double QtFaceQuadPairingPanel::getMin ( ) const
{
	CHECK_NULL_PTR_ERROR (_minTextField)
	return _minTextField->getValue ( );
}	// QtFaceQuadPairingPanel::getMin


double QtFaceQuadPairingPanel::getMax ( ) const
{
	CHECK_NULL_PTR_ERROR (_maxTextField)
	return _maxTextField->getValue ( );
}	// QtFaceQuadPairingPanel::getMax

// ===========================================================================
//                        LA CLASSE QtFaceMeshingPropertyPanel
// ===========================================================================

QtFaceMeshingPropertyPanel::QtFaceMeshingPropertyPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).facesMeshingPropertyOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).facesMeshingPropertyOperationTag),
	  _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _transfinitePanel (0), _directionalPanel (0), _orthogonalPanel (0),
	  _rotationalPanel (0),
	  _delaunayGMSHPanel (0),
	  _quadPairingPanel(0),  _facesPanel (0)
{
//	SET_WIDGET_BACKGROUND (this, Qt::yellow)
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Les faces à mailler :
	_facesPanel	= new QtMgx3DEntityPanel (
						this, "", true, "Faces :", "", &mainWindow,
						SelectionManagerIfc::D2, FilterEntity::TopoCoFace);
	_facesPanel->setMultiSelectMode (true);
	layout->addWidget (_facesPanel);
	connect (_facesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_facesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// Méthode de maillage des faces :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Méthode"), this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem ("Maillage transfini");
	_operationMethodComboBox->addItem ("Maillage directionnel");
	_operationMethodComboBox->addItem ("Maillage directionnel orthogonal");
	_operationMethodComboBox->addItem ("Maillage suivant une rotation");
	_operationMethodComboBox->addItem ("Maillage Delaunay (GMSH)");
    //_operationMethodComboBox->addItem ("Maillage Quad non structuré (pairing) [inactif]");
	connect (_operationMethodComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (operationMethodCallback ( )));
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (2);
	
	// Définition de la maillage :
	QtGroupBox*		groupBox	=
						new QtGroupBox (this, "Paramètres de maillage");
//	SET_WIDGET_BACKGROUND (groupBox, Qt::green)
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
	_transfinitePanel	= new QtFaceTransfinitePanel (0, mainWindow, this);
//	_transfinitePanel->layout ( )->setSpacing (5);
//	_transfinitePanel->layout ( )->setContentsMargins (0, 0, 0, 0);
	_transfinitePanel->hide ( );
	_directionalPanel	= new QtFaceDirectionalPanel (0, mainWindow, this);
	_directionalPanel->hide ( );
	_orthogonalPanel	= new QtFaceOrthogonalPanel (0, mainWindow, this);
	_orthogonalPanel->hide ( );
	_rotationalPanel	= new QtFaceRotationalPanel (0, mainWindow, this);
	_rotationalPanel->hide ( );
	_delaunayGMSHPanel	= new QtFaceDelaunayGMSHPanel (0, mainWindow, this);
	_delaunayGMSHPanel->hide ( );
//    _quadPairingPanel = new QtFaceQuadPairingPanel(0, mainWindow, this);
//    _quadPairingPanel->hide ( );

	addPreviewCheckBox (true);

	operationMethodCallback ( );

	vlayout->addStretch (2);
	layout->addStretch (2);
}	// QtFaceMeshingPropertyPanel::QtFaceMeshingPropertyPanel


QtFaceMeshingPropertyPanel::QtFaceMeshingPropertyPanel (const QtFaceMeshingPropertyPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _transfinitePanel (0), _directionalPanel (0), _orthogonalPanel (0),
	  _rotationalPanel (0),
	  _delaunayGMSHPanel (0),
	  _quadPairingPanel(0), _facesPanel (0)
{
	MGX_FORBIDDEN ("QtFaceMeshingPropertyPanel copy constructor is not allowed.");
}	// QtFaceMeshingPropertyPanel::QtFaceMeshingPropertyPanel (const QtFaceMeshingPropertyPanel&)


QtFaceMeshingPropertyPanel& QtFaceMeshingPropertyPanel::operator = (
											const QtFaceMeshingPropertyPanel&)
{
	MGX_FORBIDDEN ("QtFaceMeshingPropertyPanel assignment operator is not allowed.");
	return *this;
}	// QtFaceMeshingPropertyPanel::QtFaceMeshingPropertyPanel (const QtFaceMeshingPropertyPanel&)


QtFaceMeshingPropertyPanel::~QtFaceMeshingPropertyPanel ( )
{
}	// QtFaceMeshingPropertyPanel::~QtFaceMeshingPropertyPanel


QtFaceMeshingPropertyPanel::OPERATION_METHOD
						QtFaceMeshingPropertyPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtFaceMeshingPropertyPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtFaceMeshingPropertyPanel::getOperationMethod


CoFaceMeshingProperty* QtFaceMeshingPropertyPanel::getMeshingProperty ( ) const
{
	CHECK_NULL_PTR_ERROR (_transfinitePanel)
	CHECK_NULL_PTR_ERROR (_directionalPanel)
	CHECK_NULL_PTR_ERROR (_orthogonalPanel)
	CHECK_NULL_PTR_ERROR (_rotationalPanel)
	CHECK_NULL_PTR_ERROR (_delaunayGMSHPanel)
	switch (getOperationMethod ( ))
	{
		case QtFaceMeshingPropertyPanel::TRANSFINITE			:
			return _transfinitePanel->getMeshingProperty ( );
		case QtFaceMeshingPropertyPanel::DIRECTIONAL			:
			return _directionalPanel->getMeshingProperty ( );
		case QtFaceMeshingPropertyPanel::ORTHOGONAL				:
			return _orthogonalPanel->getMeshingProperty ( );
		case QtFaceMeshingPropertyPanel::ROTATIONAL				:
			return _rotationalPanel->getMeshingProperty ( );
		case QtFaceMeshingPropertyPanel::DELAUNAY_GMSH			:
			return _delaunayGMSHPanel->getMeshingProperty ( );
//        case QtFaceMeshingPropertyPanel::QUAD_PAIRING                :
//            return _quadPairingPanel->getMeshingProperty ( );
	}	// switch (getOperationMethod ( ))

	INTERNAL_ERROR (exc, "Méthode de maillage de face non supportée", "QtFaceMeshingPropertyPanel::getMeshingProperty")
	throw exc;
}	// QtFaceMeshingPropertyPanel::getMeshingProperty


vector<string> QtFaceMeshingPropertyPanel::getFacesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_facesPanel)
	return _facesPanel->getUniqueNames ( );
	
}	// QtFaceMeshingPropertyPanel::getFacesNames


void QtFaceMeshingPropertyPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_transfinitePanel)
	CHECK_NULL_PTR_ERROR (_directionalPanel)
	CHECK_NULL_PTR_ERROR (_orthogonalPanel)
	CHECK_NULL_PTR_ERROR (_rotationalPanel)
	CHECK_NULL_PTR_ERROR (_delaunayGMSHPanel)
//	CHECK_NULL_PTR_ERROR (_quadPairingPanel)
	CHECK_NULL_PTR_ERROR (_facesPanel)
	_transfinitePanel->reset ( );
	_directionalPanel->reset ( );
	_orthogonalPanel->reset ( );
	_rotationalPanel->reset ( );
	_delaunayGMSHPanel->reset ( );
//	_quadPairingPanel->reset ( );
	_facesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtFaceMeshingPropertyPanel::reset


void QtFaceMeshingPropertyPanel::validate ( )
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
		error << "QtFaceMeshingPropertyPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtFaceMeshingPropertyPanel::TRANSFINITE			:
		case QtFaceMeshingPropertyPanel::DIRECTIONAL			:
		case QtFaceMeshingPropertyPanel::ORTHOGONAL				:
		case QtFaceMeshingPropertyPanel::ROTATIONAL				:
		case QtFaceMeshingPropertyPanel::DELAUNAY_GMSH			:
//        case QtFaceMeshingPropertyPanel::QUAD_PAIRING           :
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode de maillage de faces sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtFaceMeshingPropertyPanel::validate : index de méthode "
			      << "d'opération de maillage de faces invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtFaceMeshingPropertyPanel::validate


void QtFaceMeshingPropertyPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_facesPanel)
	_facesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		if (0 != _currentPanel)
			_currentPanel->cancel ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtFaceMeshingPropertyPanel::cancel


void QtFaceMeshingPropertyPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_facesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		const vector<string>	selectedFaces	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoFace);
		if (0 != selectedFaces.size ( ))
			_facesPanel->setUniqueNames (selectedFaces);
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_facesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	if (0 != _currentPanel)
		_currentPanel->autoUpdate ( );

	QtMgx3DOperationPanel::autoUpdate ( );

	_facesPanel->actualizeGui (true);
}	// QtFaceMeshingPropertyPanel::autoUpdate


void QtFaceMeshingPropertyPanel::preview (bool show, bool destroyInteractor)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// N'afficher une boite de dialogue d'erreur que si demandé ...
	QT_PREVIEW_BEGIN_TRY_CATCH_BLOCK

	QtMgx3DOperationPanel::preview (show, destroyInteractor);

	if (0 != _currentPanel)
		_currentPanel->preview (show, destroyInteractor);

	QT_PREVIEW_COMPLETE_TRY_CATCH_BLOCK

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtFaceMeshingPropertyPanel::preview


vector<Entity*> QtFaceMeshingPropertyPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	vector<string>	names	= getFacesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*	entity	= getContext ( ).getTopoManager ( ).getEntity (*it);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtFaceMeshingPropertyPanel::getInvolvedEntities


void QtFaceMeshingPropertyPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _currentPanel)
		_currentPanel->stopSelection ( );
	if (0 != _facesPanel)
		_facesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtFaceMeshingPropertyPanel::operationCompleted


void QtFaceMeshingPropertyPanel::operationMethodCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_facesPanel)
	CHECK_NULL_PTR_ERROR (_facesPanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_transfinitePanel)
	CHECK_NULL_PTR_ERROR (_directionalPanel)
	CHECK_NULL_PTR_ERROR (_directionalPanel->getEdgeTextField ( ))
	CHECK_NULL_PTR_ERROR (_orthogonalPanel)
	CHECK_NULL_PTR_ERROR (_rotationalPanel)
	CHECK_NULL_PTR_ERROR (_rotationalPanel->getEdgeTextField ( ))
	CHECK_NULL_PTR_ERROR (_rotationalPanel->getAxePoint1TextField ( ))
	CHECK_NULL_PTR_ERROR (_rotationalPanel->getAxePoint2TextField ( ))
	CHECK_NULL_PTR_ERROR (_delaunayGMSHPanel)
//    CHECK_NULL_PTR_ERROR (_quadPairingPanel)
	if (0 != _currentPanel)
	{
		_currentParentWidget->layout ( )->removeWidget (_currentPanel);
		_currentPanel->hide ( );
		_currentPanel->setParent (0);
	}
	_currentPanel	= 0;

	// Pour la majorité des cas :
	_facesPanel->getNameTextField ( )->setLinkedSeizureManagers (0, 0);

	switch (getOperationMethod ( ))
	{
		case QtFaceMeshingPropertyPanel::TRANSFINITE			:
			_currentPanel	= _transfinitePanel;			break;
		case QtFaceMeshingPropertyPanel::DIRECTIONAL			:
			_currentPanel	= _directionalPanel;
			_directionalPanel->getEdgeTextField ( )->setLinkedSeizureManagers (
									0, _facesPanel->getNameTextField ( ));
			_facesPanel->getNameTextField ( )->setLinkedSeizureManagers (
									_directionalPanel->getEdgeTextField ( ), 0);
			break;
		case QtFaceMeshingPropertyPanel::ORTHOGONAL				:
			_currentPanel	= _orthogonalPanel;
			_orthogonalPanel->getEdgeTextField ( )->setLinkedSeizureManagers (
									0, _facesPanel->getNameTextField ( ));
			_facesPanel->getNameTextField ( )->setLinkedSeizureManagers (
									_orthogonalPanel->getEdgeTextField ( ), 0);
			break;
		case QtFaceMeshingPropertyPanel::ROTATIONAL				:
			_currentPanel	= _rotationalPanel	;
			_rotationalPanel->getEdgeTextField ( )->setLinkedSeizureManagers (
						0, _rotationalPanel->getAxePoint1TextField ( ));
			_rotationalPanel->getAxePoint1TextField (
				)->setLinkedSeizureManagers (
						_rotationalPanel->getEdgeTextField ( ),
						_rotationalPanel->getAxePoint2TextField ( ));
			_rotationalPanel->getAxePoint2TextField (
				)->setLinkedSeizureManagers (
						_rotationalPanel->getAxePoint1TextField ( ),
						_facesPanel->getNameTextField ( ));
			_facesPanel->getNameTextField ( )->setLinkedSeizureManagers (
				_rotationalPanel->getAxePoint2TextField ( ), 0);
			break;
		case QtFaceMeshingPropertyPanel::DELAUNAY_GMSH			:
			_currentPanel	= _delaunayGMSHPanel;			break;
//		case QtFaceMeshingPropertyPanel::QUAD_PAIRING      :
//	        _currentPanel   = _quadPairingPanel;            break;
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode d'opération non supportée ("
			        << (unsigned long)getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtFaceMeshingPropertyPanel::operationMethodCallback")
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

	try
	{
		if (0 != getMainWindow ( ))
			getMainWindow ( )->getGraphicalWidget ( ).getRenderingManager (
															).forceRender ( );
	}
	catch (...)
	{
	}

	parametersModifiedCallback ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtFaceMeshingPropertyPanel::operationMethodCallback


// ===========================================================================
//                  LA CLASSE QtFaceMeshingPropertyAction
// ===========================================================================

QtFaceMeshingPropertyAction::QtFaceMeshingPropertyAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtFaceMeshingPropertyPanel*	operationPanel	=
		new QtFaceMeshingPropertyPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtFaceMeshingPropertyAction::QtFaceMeshingPropertyAction


QtFaceMeshingPropertyAction::QtFaceMeshingPropertyAction (
										const QtFaceMeshingPropertyAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtFaceMeshingPropertyAction copy constructor is not allowed.")
}	// QtFaceMeshingPropertyAction::QtFaceMeshingPropertyAction


QtFaceMeshingPropertyAction& QtFaceMeshingPropertyAction::operator = (
										const QtFaceMeshingPropertyAction&)
{
	MGX_FORBIDDEN ("QtFaceMeshingPropertyAction assignment operator is not allowed.")
	return *this;
}	// QtFaceMeshingPropertyAction::operator =


QtFaceMeshingPropertyAction::~QtFaceMeshingPropertyAction ( )
{
}	// QtFaceMeshingPropertyAction::~QtFaceMeshingPropertyAction


QtFaceMeshingPropertyPanel*
					QtFaceMeshingPropertyAction::getMeshingPropertyPanel ( )
{
	return dynamic_cast<QtFaceMeshingPropertyPanel*>(getOperationPanel ( ));
}	// QtFaceMeshingPropertyAction::getMeshingPropertyPanel


void QtFaceMeshingPropertyAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres de maillage des faces :
	QtFaceMeshingPropertyPanel*	panel	= getMeshingPropertyPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>					faces		= panel->getFacesNames ( );
	unique_ptr<CoFaceMeshingProperty>	properties (panel->getMeshingProperty ( ));
	cmdResult	= getContext ( ).getTopoManager ( ).setMeshingProperty (*properties.get ( ), faces);

	setCommandResult (cmdResult);
}	// QtFaceMeshingPropertyAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
