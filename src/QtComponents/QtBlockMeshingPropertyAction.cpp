/**
 * \file        QtBlockMeshingPropertyAction.cpp
 * \author      Charles PIGNEROL
 * \date        14/11/2013
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/MgxText.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtBlockMeshingPropertyAction.h"
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
//                        LA CLASSE QtBlockTransfinitePanel
// ===========================================================================


QtBlockTransfinitePanel::QtBlockTransfinitePanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);
}	// QtBlockTransfinitePanel::QtBlockTransfinitePanel


QtBlockTransfinitePanel::QtBlockTransfinitePanel (
											const QtBlockTransfinitePanel& p)
	  : QtMgx3DOperationsSubPanel (p)
{
	MGX_FORBIDDEN ("QtBlockTransfinitePanel copy constructor is not allowed.");
}	// QtBlockTransfinitePanel::QtBlockTransfinitePanel


QtBlockTransfinitePanel& QtBlockTransfinitePanel::operator = (
												const QtBlockTransfinitePanel&)
{
	 MGX_FORBIDDEN ("QtBlockTransfinitePanel assignment operator is not allowed.");
	return *this;
}	// QtBlockTransfinitePanel::operator =


QtBlockTransfinitePanel::~QtBlockTransfinitePanel ( )
{
}	// QtBlockTransfinitePanel::~QtBlockTransfinitePanel


BlockMeshingPropertyTransfinite* QtBlockTransfinitePanel::getMeshingProperty ( ) const
{
	return new BlockMeshingPropertyTransfinite ( );
}	// QtBlockTransfinitePanel::getMeshingProperty


// ===========================================================================
//                         LA CLASSE QtBlockDirectionalPanel
// ===========================================================================


QtBlockDirectionalPanel::QtBlockDirectionalPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel),
	  _edgeTextField (0), _invertCheckBox (0), _directionLabel (0),
	  _useEdgeNameCheckBox (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);

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

	// Le vecteur : ???
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label	= new QLabel ("Direction :", this);
	hlayout->addWidget (label);
	_directionLabel	= new QLabel ("", this);
	hlayout->addWidget (_directionLabel); 

	// Faut-il utiliser le nom de l'arête ou le vecteur ?
	_useEdgeNameCheckBox	= new QCheckBox(QString::fromUtf8("Utiliser le nom de l'arête"), this);
	layout->addWidget (_useEdgeNameCheckBox);
	_useEdgeNameCheckBox->setCheckState (Qt::Checked);
	_useEdgeNameCheckBox->setToolTip (QString::fromUtf8("L'utilisation du nom de l'arête permet de suivre les évolution de celle-ci. L'utilisation du vecteur est définitive."));

	hlayout->addStretch (2.);
}	// QtBlockDirectionalPanel::QtBlockDirectionalPanel


QtBlockDirectionalPanel::QtBlockDirectionalPanel (
	const QtBlockDirectionalPanel& p)
	: QtMgx3DOperationsSubPanel (p),
	  _edgeTextField (0), _invertCheckBox (0), _directionLabel (0),
	  _useEdgeNameCheckBox (0)
{
	MGX_FORBIDDEN ("QtBlockDirectionalPanel copy constructor is not allowed.");
}	// QtBlockDirectionalPanel::QtBlockDirectionalPanel


QtBlockDirectionalPanel& QtBlockDirectionalPanel::operator = (
												const QtBlockDirectionalPanel&)
{
	 MGX_FORBIDDEN ("QtBlockDirectionalPanel assignment operator is not allowed.");
	return *this;
}	// QtBlockDirectionalPanel::operator =


QtBlockDirectionalPanel::~QtBlockDirectionalPanel ( )
{
}	// QtBlockDirectionalPanel::~QtBlockDirectionalPanel


void QtBlockDirectionalPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgeTextField)
	_edgeTextField->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtBlockDirectionalPanel::reset


void QtBlockDirectionalPanel::cancel ( )
{
	CHECK_NULL_PTR_ERROR (_edgeTextField)
	_edgeTextField->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_edgeTextField->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))

	QtMgx3DOperationsSubPanel::cancel ( );
}	// QtBlockDirectionalPanel::cancel


void QtBlockDirectionalPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_edgeTextField)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedEdges	= 
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
		if (1 == selectedEdges.size ( ))
		{
			CHECK_NULL_PTR_ERROR (_edgeTextField)
			_edgeTextField->setUniqueName (selectedEdges [0]);
		}

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_edgeTextField->getEntityPanel ( ).clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationsSubPanel::autoUpdate ( );

	_edgeTextField->actualizeGui (true);
}	// QtBlockDirectionalPanel::autoUpdate


void QtBlockDirectionalPanel::setVisible (bool visible)
{
	preview (visible, true);
	QtMgx3DOperationsSubPanel::setVisible (visible);
}	// QtBlockDirectionalPanel::setVisible


Math::Point QtBlockDirectionalPanel::getPoint1 ( ) const
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
}	// QtBlockDirectionalPanel::getPoint1


Math::Point QtBlockDirectionalPanel::getPoint2 ( ) const
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
}	// QtBlockDirectionalPanel::getPoint2


BlockMeshingPropertyDirectional* QtBlockDirectionalPanel::getMeshingProperty ( ) const
{
	BlockMeshingPropertyDirectional*	bmpd	= true == useEdgeName ( ) ?
			new BlockMeshingPropertyDirectional (getEdgeName ( )) :
			new BlockMeshingPropertyDirectional (getPoint1 ( ), getPoint2 ( ));

	return bmpd;
}	// QtBlockDirectionalPanel::getMeshingProperty


bool QtBlockDirectionalPanel::isEdgeInverted ( ) const
{
	CHECK_NULL_PTR_ERROR (_invertCheckBox)
	return Qt::Checked == _invertCheckBox->checkState ( ) ? true : false;
}	// QtBlockDirectionalPanel::isEdgeInverted


bool QtBlockDirectionalPanel::useEdgeName ( ) const
{
	CHECK_NULL_PTR_ERROR (_useEdgeNameCheckBox)
	return Qt::Checked == _useEdgeNameCheckBox->checkState ( ) ? true : false;
}	// QtBlockDirectionalPanel::useEdgeName


string QtBlockDirectionalPanel::getEdgeName ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgeTextField)
	return _edgeTextField->getUniqueName ( );
}	// QtBlockDirectionalPanel::getEdgeName


void QtBlockDirectionalPanel::stopSelection ( )
{
	if (0 != _edgeTextField)
		_edgeTextField->stopSelection ( );
}	// QtBlockDirectionalPanel::stopSelection


QtEntityIDTextField* QtBlockDirectionalPanel::getEdgeTextField ( )
{
	CHECK_NULL_PTR_ERROR (_edgeTextField)
	return &_edgeTextField->getEntityTextField ( );
}	// QtBlockDirectionalPanel::getEdgeTextField


void QtBlockDirectionalPanel::preview (bool on, bool destroyInteractor)
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
	{
		// Quoi qu'il arrive on détruit l'éventuel aperçu existant :
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
				properties.setLineWidth (
								Resources::instance ( )._previewWidth.getValue( ));
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
}	// QtBlockDirectionalPanel::preview


void QtBlockDirectionalPanel::directionModifiedCallback ( )
{
	preview (true, true);
}	// QtBlockDirectionalPanel::directionModifiedCallback


// ===========================================================================
//                        LA CLASSE QtBlockOrthogonalPanel
// ===========================================================================


QtBlockOrthogonalPanel::QtBlockOrthogonalPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtBlockDirectionalPanel (parent, window, mainPanel),
	  _layersNumTextField (0)
{
	QBoxLayout*		mainLayout	= dynamic_cast<QBoxLayout*>(layout ( ));
	if (0 == mainLayout)
	{
		INTERNAL_ERROR (exc, "Mauvais type de layout.", "QtBlockOrthogonalPanel::QtBlockOrthogonalPanel")
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
}	// QtBlockOrthogonalPanel::QtBlockOrthogonalPanel


QtBlockOrthogonalPanel::QtBlockOrthogonalPanel (const QtBlockOrthogonalPanel&)
	: QtBlockDirectionalPanel (0,*new QtMgx3DMainWindow(0)),
	  _layersNumTextField (0)
{
	MGX_FORBIDDEN ("QtBlockDelaunayGMSHPanel copy constructor is not allowed.");
}	// QtBlockOrthogonalPanel::QtBlockOrthogonalPanel


QtBlockOrthogonalPanel& QtBlockOrthogonalPanel::operator = (
												const QtBlockOrthogonalPanel&)
{
	 MGX_FORBIDDEN ("QtBlockDelaunayGMSHPanel assignment operator is not allowed.");
	return *this;
}	// QtBlockOrthogonalPanel::operator =


QtBlockOrthogonalPanel::~QtBlockOrthogonalPanel ( )
{
}	// QtBlockOrthogonalPanel::~QtBlockOrthogonalPanel


void QtBlockOrthogonalPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	BlockMeshingPropertyDelaunayTetgen	props;	// Pour récupérer les valeurs par défaut
	CHECK_NULL_PTR_ERROR (_layersNumTextField)
	_layersNumTextField->setValue (5);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtBlockOrthogonalPanel::reset


BlockMeshingPropertyOrthogonal*
						QtBlockOrthogonalPanel::getMeshingProperty ( ) const
{
	return new BlockMeshingPropertyOrthogonal (
							getPoint1 ( ), getPoint2 ( ), getLayersNum ( ));
}	// QtBlockOrthogonalPanel::getMeshingProperty


size_t QtBlockOrthogonalPanel::getLayersNum( ) const
{
	CHECK_NULL_PTR_ERROR (_layersNumTextField)
	return _layersNumTextField->getValue ( );
}	// QtFaceOrthogonalPanel::getLayersNum


// ===========================================================================
//                       LA CLASSE QtBlockRotationalPanel
// ===========================================================================


QtBlockRotationalPanel::QtBlockRotationalPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel),
	  _directionTextField (0), _axePoint1Panel (0), _axePoint2Panel (0),
	  _directionLabel (0), _axeLabel (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);

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
	connect (_axePoint2Panel, SIGNAL (pointModified ( )),
	         this, SLOT (parametersModifiedCallback ( )));
	_axeLabel	= new QLabel ("", this);
	layout->addWidget (_axeLabel);

	parametersModifiedCallback ( );
}	// QtBlockRotationalPanel::QtBlockRotationalPanel


QtBlockRotationalPanel::QtBlockRotationalPanel (
						const QtBlockRotationalPanel& p)
	: QtMgx3DOperationsSubPanel (p), _directionTextField (0),
	  _axePoint1Panel (0), _axePoint2Panel (0),
	  _directionLabel (0), _axeLabel (0)
{
	MGX_FORBIDDEN ("QtBlockRotationalPanel copy constructor is not allowed.");
}	// QtBlockRotationalPanel::QtBlockRotationalPanel


QtBlockRotationalPanel& QtBlockRotationalPanel::operator = (
												const QtBlockRotationalPanel&)
{
	 MGX_FORBIDDEN ("QtBlockRotationalPanel assignment operator is not allowed.");
	return *this;
}	// QtBlockRotationalPanel::operator =


QtBlockRotationalPanel::~QtBlockRotationalPanel ( )
{
}	// QtBlockRotationalPanel::~QtBlockRotationalPanel


void QtBlockRotationalPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_directionTextField)
	CHECK_NULL_PTR_ERROR (_axePoint1Panel)
	CHECK_NULL_PTR_ERROR (_axePoint2Panel)
	_directionTextField->reset ( );
	_axePoint1Panel->reset ( );
	_axePoint2Panel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtBlockRotationalPanel::reset


void QtBlockRotationalPanel::cancel ( )
{
	CHECK_NULL_PTR_ERROR (_directionTextField)
	CHECK_NULL_PTR_ERROR (_axePoint1Panel)
	CHECK_NULL_PTR_ERROR (_axePoint2Panel)
	_directionTextField->stopSelection ( );
	_axePoint1Panel->stopSelection ( );
	_axePoint2Panel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{

		BEGIN_QT_TRY_CATCH_BLOCK

		_directionTextField->setUniqueName ("");
		_axePoint1Panel->setUniqueName ("");
		_axePoint2Panel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))

	QtMgx3DOperationsSubPanel::cancel ( );
}	// QtBlockRotationalPanel::cancel


void QtBlockRotationalPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_directionTextField)
	CHECK_NULL_PTR_ERROR (_axePoint1Panel)
	CHECK_NULL_PTR_ERROR (_axePoint2Panel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{

	BEGIN_QT_TRY_CATCH_BLOCK

	vector<string>	names	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
	if (1 == names.size ( ))
		_directionTextField->setUniqueName (names [0]);
	names	= getSelectionManager ( ).getEntitiesNames(FilterEntity::AllPoints);
	if (2 <= names.size ( ))
	{
		if (1 <= names.size ( ))
			_axePoint1Panel->setUniqueName (names [0]);
		if (2 == names.size ( ))
			_axePoint2Panel->setUniqueName (names [1]);
	}	// if (2 <= names.size ( ))

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_directionTextField->setUniqueName ("");
	_axePoint1Panel->setUniqueName ("");
	_axePoint2Panel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationsSubPanel::autoUpdate ( );

	_directionTextField->actualizeGui (true);
	_axePoint1Panel->actualizeGui (true);
	_axePoint2Panel->actualizeGui (true);
}	// QtBlockRotationalPanel::autoUpdate


void QtBlockRotationalPanel::setVisible (bool visible)
{
	preview (visible, true);
	QtMgx3DOperationsSubPanel::setVisible (visible);
}	// QtBlockRotationalPanel::setVisible


Math::Point QtBlockRotationalPanel::getDirPoint1 ( ) const
{
	CHECK_NULL_PTR_ERROR (_directionTextField)
	const CoEdge*	edge	= getContext ( ).getTopoManager ( ).getCoEdge (
										_directionTextField->getUniqueName( ));
	CHECK_NULL_PTR_ERROR (edge)
	const Vertex*	v1	= edge->getVertex (0);
	CHECK_NULL_PTR_ERROR (v1)

	return v1->getCoord ( );
}	// QtBlockRotationalPanel::getDirPoint1


Math::Point QtBlockRotationalPanel::getDirPoint2 ( ) const
{
	CHECK_NULL_PTR_ERROR (_directionTextField)
	const CoEdge*	edge	= getContext ( ).getTopoManager ( ).getCoEdge (
										_directionTextField->getUniqueName( ));
	CHECK_NULL_PTR_ERROR (edge)
	const Vertex*	v2	= edge->getVertex (edge->getNbVertices ( ) - 1);
	CHECK_NULL_PTR_ERROR (v2)

	return v2->getCoord ( );
}	// QtBlockRotationalPanel::getDirPoint2


Math::Point QtBlockRotationalPanel::getAxePoint1 ( ) const
{
	CHECK_NULL_PTR_ERROR (_axePoint1Panel)
	return Math::Point (_axePoint1Panel->getPoint ( ));
}	// QtBlockRotationalPanel::getAxePoint1


Math::Point QtBlockRotationalPanel::getAxePoint2 ( ) const
{
	CHECK_NULL_PTR_ERROR (_axePoint2Panel)
	return Math::Point (_axePoint2Panel->getPoint ( ));
}	// QtBlockRotationalPanel::getAxePoint2


BlockMeshingPropertyRotational* QtBlockRotationalPanel::getMeshingProperty ( ) const
{
	return new BlockMeshingPropertyRotational (
		getDirPoint1 ( ), getDirPoint2 ( ), getAxePoint1 ( ), getAxePoint2 ( ));
}	// QtBlockRotationalPanel::getMeshingProperty


void QtBlockRotationalPanel::stopSelection ( )
{
	if (0 != _directionTextField)
		_directionTextField->stopSelection ( );
	if (0 != _axePoint1Panel)
		_axePoint1Panel->stopSelection ( );
	if (0 != _axePoint2Panel)
		_axePoint2Panel->stopSelection ( );
}	// QtBlockRotationalPanel::stopSelection


QtEntityIDTextField* QtBlockRotationalPanel::getEdgeTextField ( )
{
	CHECK_NULL_PTR_ERROR (_directionTextField)
	return &_directionTextField->getEntityTextField ( );
}	// QtBlockRotationalPanel::getEdgeTextField


QtEntityIDTextField* QtBlockRotationalPanel::getAxePoint1TextField ( )
{
	CHECK_NULL_PTR_ERROR (_axePoint1Panel)
	return _axePoint1Panel->getVertexTextField ( );
}	// QtBlockRotationalPanel::getAxePoint1TextField


QtEntityIDTextField* QtBlockRotationalPanel::getAxePoint2TextField ( )
{
	CHECK_NULL_PTR_ERROR (_axePoint2Panel)
	return _axePoint2Panel->getVertexTextField ( );
}	// QtBlockRotationalPanel::getAxePoint2TextField


void QtBlockRotationalPanel::preview (bool on, bool destroyInteractor)
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
	{	// // Quoi qu'il arrive on détruit l'éventuel aperçu existant :
		removePreviewedObjects ( );
	}
	catch (...)
	{
	}

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
}	// QtBlockRotationalPanel::preview


// ===========================================================================
//                        LA CLASSE QtBlockDelaunayTetgenPanel
// ===========================================================================


QtBlockDelaunayTetgenPanel::QtBlockDelaunayTetgenPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel),
	  _volMaxTextField (0), _qualityFactorTextField (0), _ratioPyramiSizeTextField(0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);

	BlockMeshingPropertyDelaunayTetgen	props;	// Pour récupérer les valeurs par défaut

	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label	= new QLabel ("Volume maximum :", this);
	hlayout->addWidget (label);
	_volMaxTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_volMaxTextField->setValue (props.getVolumeMax ( ));
	hlayout->addWidget (_volMaxTextField);
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Facteur de qualité :"), this);
	hlayout->addWidget (label);
	_qualityFactorTextField	=
					&QtNumericFieldsFactory::createDistanceTextField (this);
	_qualityFactorTextField->setValue (props.getRadiusEdgeRatio ( ));
	hlayout->addWidget (_qualityFactorTextField);

	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel ("Ration hauteur pyramides :", this);
	hlayout->addWidget (label);
	_ratioPyramiSizeTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_ratioPyramiSizeTextField->setValue (props.getRatioPyramidSize ( ));
	hlayout->addWidget (_ratioPyramiSizeTextField);

}	// QtBlockDelaunayTetgenPanel::QtBlockDelaunayTetgenPanel


QtBlockDelaunayTetgenPanel::QtBlockDelaunayTetgenPanel (
											const QtBlockDelaunayTetgenPanel& p)
	  : QtMgx3DOperationsSubPanel (p),
	    _volMaxTextField (0), _qualityFactorTextField (0), _ratioPyramiSizeTextField(0)
{
	MGX_FORBIDDEN ("QtBlockDelaunayTetgenPanel copy constructor is not allowed.");
}	// QtBlockDelaunayTetgenPanel::QtBlockDelaunayTetgenPanel


QtBlockDelaunayTetgenPanel& QtBlockDelaunayTetgenPanel::operator = (
											const QtBlockDelaunayTetgenPanel&)
{
	 MGX_FORBIDDEN ("QtBlockDelaunayTetgenPanel assignment operator is not allowed.");
	return *this;
}	// QtBlockDelaunayTetgenPanel::operator =


QtBlockDelaunayTetgenPanel::~QtBlockDelaunayTetgenPanel ( )
{
}	// QtBlockDelaunayTetgenPanel::~QtBlockDelaunayTetgenPanel


BlockMeshingPropertyDelaunayTetgen*
						QtBlockDelaunayTetgenPanel::getMeshingProperty ( ) const
{
	return new BlockMeshingPropertyDelaunayTetgen (
										getQualityFactor ( ), getVolMax ( ), getRatioPyramidSize());
}	// QtBlockDelaunayTetgenPanel::getMeshingProperty


double QtBlockDelaunayTetgenPanel::getVolMax ( ) const
{
	CHECK_NULL_PTR_ERROR (_volMaxTextField)
	return _volMaxTextField->getValue ( );
}	// QtBlockDelaunayTetgenPanel::getVolMax


double QtBlockDelaunayTetgenPanel::getQualityFactor ( ) const
{
	CHECK_NULL_PTR_ERROR (_qualityFactorTextField)
	return _qualityFactorTextField->getValue ( );
}	// QtBlockDelaunayTetgenPanel::getQualityFactor

double QtBlockDelaunayTetgenPanel::getRatioPyramidSize ( ) const
{
	CHECK_NULL_PTR_ERROR (_ratioPyramiSizeTextField)
	return _ratioPyramiSizeTextField->getValue ( );
}	// QtBlockDelaunayTetgenPanel::getRatioPyramidSize


void QtBlockDelaunayTetgenPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	BlockMeshingPropertyDelaunayTetgen	props;	// Pour récupérer les valeurs par défaut
	CHECK_NULL_PTR_ERROR (_volMaxTextField)
	CHECK_NULL_PTR_ERROR (_qualityFactorTextField)
	CHECK_NULL_PTR_ERROR (_ratioPyramiSizeTextField)
	_volMaxTextField->setValue (props.getVolumeMax ( ));
	_qualityFactorTextField->setValue (props.getRadiusEdgeRatio ( ));
	_ratioPyramiSizeTextField->setValue (props.getRatioPyramidSize());

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtBlockDelaunayTetgenPanel::reset


// ===========================================================================
//                        LA CLASSE QtBlockMeshInsertionPanel
// ===========================================================================


QtBlockMeshInsertionPanel::QtBlockMeshInsertionPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);
}	// QtBlockMeshInsertionPanel::QtBlockMeshInsertionPanel


QtBlockMeshInsertionPanel::QtBlockMeshInsertionPanel (
											const QtBlockMeshInsertionPanel& p)
	  : QtMgx3DOperationsSubPanel (p)
{
	MGX_FORBIDDEN ("QtBlockMeshInsertionPanel copy constructor is not allowed.");
}	// QtBlockMeshInsertionPanel::QtBlockMeshInsertionPanel


QtBlockMeshInsertionPanel& QtBlockMeshInsertionPanel::operator = (
											const QtBlockMeshInsertionPanel&)
{
	 MGX_FORBIDDEN ("QtBlockMeshInsertionPanel assignment operator is not allowed.");
	return *this;
}	// QtBlockMeshInsertionPanel::operator =


QtBlockMeshInsertionPanel::~QtBlockMeshInsertionPanel ( )
{
}	// QtBlockMeshInsertionPanel::~QtBlockMeshInsertionPanel


BlockMeshingPropertyInsertion*
						QtBlockMeshInsertionPanel::getMeshingProperty ( ) const
{
	return new BlockMeshingPropertyInsertion ( );
}	// QtBlockMeshInsertionPanel::getMeshingProperty



// ===========================================================================
//                        LA CLASSE QtBlockMeshingPropertyPanel
// ===========================================================================

QtBlockMeshingPropertyPanel::QtBlockMeshingPropertyPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).blocsMeshingPropertyOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).blocsMeshingPropertyOperationTag),
	  _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _transfinitePanel (0), _directionalPanel (0), _orthogonalPanel (0),
	  _rotationalPanel (0),
	  _delaunayTetgenPanel (0),
	  _meshInsertionPanel (0), _blocksPanel (0)
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

	// Les blocs à mailler :
	_blocksPanel	= new QtMgx3DEntityPanel (
						this, "", true, "Blocs :", "", &mainWindow,
						SelectionManagerIfc::D3, FilterEntity::TopoBlock);
	_blocksPanel->setMultiSelectMode (true);
	layout->addWidget (_blocksPanel);
	connect (_blocksPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_blocksPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// Méthode de maillage des blocs :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Méthode"), this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem ("Maillage transfini");
	_operationMethodComboBox->addItem ("Maillage directionnel");
	_operationMethodComboBox->addItem ("Maillage directionnel orthogonal");
	_operationMethodComboBox->addItem ("Maillage suivant une rotation");
	_operationMethodComboBox->addItem ("Maillage Delaunay (Tetgen)");
	_operationMethodComboBox->addItem (QString::fromUtf8("Maillage non structuré par insertion de maillage (Nico)"));
	connect (_operationMethodComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (operationMethodCallback ( )));
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);
	
	// Définition de la maillage :
	QtGroupBox*		groupBox	=
						new QtGroupBox (this, "Paramètres de maillage");
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	groupBox->setLayout (vlayout);
//	groupBox->setContentsMargins (0, 10, 0, 0);
	groupBox->layout ( )->setContentsMargins (
				Resources::instance ( )._margin.getValue ( ),
				Resources::instance ( )._margin.getValue ( ),
				Resources::instance ( )._margin.getValue ( ),
				Resources::instance ( )._margin.getValue ( ));
	groupBox->layout ( )->setSpacing (Resources::instance ( )._spacing.getValue( ));
	layout->addWidget (groupBox);
	// Le panneau "méthode" : plusieurs panneaux possibles.
	// On garde un parent unique dans la layout :
	_currentParentWidget	= new QWidget (groupBox);
	vlayout->addWidget (_currentParentWidget);
	QHBoxLayout*	currentLayout = new QHBoxLayout (_currentParentWidget);
	_currentParentWidget->setLayout (currentLayout);
	_transfinitePanel	= new QtBlockTransfinitePanel (0, mainWindow, this);
//	_transfinitePanel->layout ( )->setSpacing (5);
//	_transfinitePanel->layout ( )->setContentsMargins (0, 0, 0, 0);
	_transfinitePanel->hide ( );
	_directionalPanel	= new QtBlockDirectionalPanel (0, mainWindow, this);
	_directionalPanel->hide ( );
	_orthogonalPanel	= new QtBlockOrthogonalPanel (0, mainWindow, this);
	_orthogonalPanel->hide ( );
	_rotationalPanel	= new QtBlockRotationalPanel (0, mainWindow, this);
	_rotationalPanel->hide ( );
	_delaunayTetgenPanel	=
					new QtBlockDelaunayTetgenPanel (0, mainWindow, this);
	_delaunayTetgenPanel->hide ( );
	_meshInsertionPanel	= new QtBlockMeshInsertionPanel (0, mainWindow, this);
	_meshInsertionPanel->hide ( );

	addPreviewCheckBox (true);

	operationMethodCallback ( );

	vlayout->addStretch (2);
	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_blocksPanel->getNameTextField ( ))
}	// QtBlockMeshingPropertyPanel::QtBlockMeshingPropertyPanel


QtBlockMeshingPropertyPanel::QtBlockMeshingPropertyPanel (
										const QtBlockMeshingPropertyPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _transfinitePanel (0), _directionalPanel (0), _orthogonalPanel (0),
	  _rotationalPanel (0),
	  _delaunayTetgenPanel (0),
	  _meshInsertionPanel (0), _blocksPanel (0)
{
	MGX_FORBIDDEN ("QtBlockMeshingPropertyPanel copy constructor is not allowed.");
}	// QtBlockMeshingPropertyPanel::QtBlockMeshingPropertyPanel (const QtBlockMeshingPropertyPanel&)


QtBlockMeshingPropertyPanel& QtBlockMeshingPropertyPanel::operator = (
											const QtBlockMeshingPropertyPanel&)
{
	MGX_FORBIDDEN ("QtBlockMeshingPropertyPanel assignment operator is not allowed.");
	return *this;
}	// QtBlockMeshingPropertyPanel::QtBlockMeshingPropertyPanel (const QtBlockMeshingPropertyPanel&)


QtBlockMeshingPropertyPanel::~QtBlockMeshingPropertyPanel ( )
{
}	// QtBlockMeshingPropertyPanel::~QtBlockMeshingPropertyPanel


void QtBlockMeshingPropertyPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_transfinitePanel)
	CHECK_NULL_PTR_ERROR (_directionalPanel)
	CHECK_NULL_PTR_ERROR (_orthogonalPanel)
	CHECK_NULL_PTR_ERROR (_rotationalPanel)
	CHECK_NULL_PTR_ERROR (_delaunayTetgenPanel)
	CHECK_NULL_PTR_ERROR (_meshInsertionPanel)
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	_transfinitePanel->reset ( );
	_directionalPanel->reset ( );
	_orthogonalPanel->reset ( );
	_rotationalPanel->reset ( );
	_delaunayTetgenPanel->reset ( );
	_meshInsertionPanel->reset ( );
	_blocksPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtBlockMeshingPropertyPanel::reset


QtBlockMeshingPropertyPanel::OPERATION_METHOD
						QtBlockMeshingPropertyPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtBlockMeshingPropertyPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtBlockMeshingPropertyPanel::getOperationMethod


BlockMeshingProperty* QtBlockMeshingPropertyPanel::getMeshingProperty ( ) const
{
	CHECK_NULL_PTR_ERROR (_transfinitePanel)
	CHECK_NULL_PTR_ERROR (_directionalPanel)
	CHECK_NULL_PTR_ERROR (_orthogonalPanel)
	CHECK_NULL_PTR_ERROR (_rotationalPanel)
	CHECK_NULL_PTR_ERROR (_delaunayTetgenPanel)
	CHECK_NULL_PTR_ERROR (_meshInsertionPanel)
	switch (getOperationMethod ( ))
	{
		case QtBlockMeshingPropertyPanel::TRANSFINITE				:
			return _transfinitePanel->getMeshingProperty ( );
		case QtBlockMeshingPropertyPanel::DIRECTIONAL				:
			return _directionalPanel->getMeshingProperty ( );
		case QtBlockMeshingPropertyPanel::ORTHOGONAL				:
			return _orthogonalPanel->getMeshingProperty ( );
		case QtBlockMeshingPropertyPanel::ROTATIONAL				:
			return _rotationalPanel->getMeshingProperty ( );
		case QtBlockMeshingPropertyPanel::DELAUNAY_TETGEN			:
			return _delaunayTetgenPanel->getMeshingProperty ( );
		case QtBlockMeshingPropertyPanel::MESH_INSERTION			:
			return _meshInsertionPanel->getMeshingProperty ( );
	}	// switch (getOperationMethod ( ))

	INTERNAL_ERROR (exc, "Méthode de maillage de blocs non supportée", "QtBlockMeshingPropertyPanel::getMeshingProperty")
	throw exc;
}	// QtBlockMeshingPropertyPanel::getMeshingProperty


vector<string> QtBlockMeshingPropertyPanel::getBlocksNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	return _blocksPanel->getUniqueNames ( );
}	// QtBlockMeshingPropertyPanel::getBlocksNames


void QtBlockMeshingPropertyPanel::setBlocksNames (const vector<string>& names)
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	_blocksPanel->setUniqueNames (names);
}	// QtBlockMeshingPropertyPanel::setBlocksNames


void QtBlockMeshingPropertyPanel::validate ( )
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
		error << "QtBlockMeshingPropertyPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtBlockMeshingPropertyPanel::TRANSFINITE			:
		case QtBlockMeshingPropertyPanel::DIRECTIONAL			:
		case QtBlockMeshingPropertyPanel::ORTHOGONAL			:
		case QtBlockMeshingPropertyPanel::ROTATIONAL			:
		case QtBlockMeshingPropertyPanel::DELAUNAY_TETGEN		:
		case QtBlockMeshingPropertyPanel::MESH_INSERTION		:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode de maillage de blocs sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtBlockMeshingPropertyPanel::validate : index de méthode "
			      << "d'opération de maillage de blocs invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtBlockMeshingPropertyPanel::validate


void QtBlockMeshingPropertyPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	if (0 != _blocksPanel)
		_blocksPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		if (0 != _blocksPanel)
			_blocksPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
	
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _currentPanel)
		_currentPanel->cancel ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtBlockMeshingPropertyPanel::cancel


void QtBlockMeshingPropertyPanel::autoUpdate ( )
{
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedBlocks	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoBlock);
		if (0 != selectedBlocks.size ( ))
			setBlocksNames (selectedBlocks);

		if (0 != _currentPanel)
			_currentPanel->autoUpdate ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	if (0 != _blocksPanel)
		_blocksPanel->setUniqueName ("");
	if (0 != _currentPanel)
		_currentPanel->autoUpdate ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	if (0 != _blocksPanel)
		_blocksPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtBlockMeshingPropertyPanel::autoUpdate


void QtBlockMeshingPropertyPanel::preview (bool show, bool destroyInteractor)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// N'afficher une boite de dialogue d'erreur que si demandé ...
	QT_PREVIEW_BEGIN_TRY_CATCH_BLOCK

	QtMgx3DOperationPanel::preview (show, destroyInteractor);

	if (0 != _currentPanel)
		_currentPanel->preview (show, destroyInteractor);

	QT_PREVIEW_COMPLETE_TRY_CATCH_BLOCK

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtBlockMeshingPropertyPanel::preview


vector<Entity*> QtBlockMeshingPropertyPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	vector<string>	names	= getBlocksNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*	entity	= getContext ( ).getTopoManager ( ).getEntity (*it);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}

	return entities;
}	// QtBlockMeshingPropertyPanel::getInvolvedEntities


void QtBlockMeshingPropertyPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _currentPanel)
		_currentPanel->stopSelection ( );
	if (0 != _blocksPanel)
		_blocksPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtBlockMeshingPropertyPanel::operationCompleted


void QtBlockMeshingPropertyPanel::operationMethodCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_blocksPanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_transfinitePanel)
	CHECK_NULL_PTR_ERROR (_directionalPanel)
	CHECK_NULL_PTR_ERROR (_directionalPanel->getEdgeTextField ( ))
	CHECK_NULL_PTR_ERROR (_orthogonalPanel)
	CHECK_NULL_PTR_ERROR (_rotationalPanel)
	CHECK_NULL_PTR_ERROR (_rotationalPanel->getEdgeTextField ( ))
	CHECK_NULL_PTR_ERROR (_rotationalPanel->getAxePoint1TextField ( ))
	CHECK_NULL_PTR_ERROR (_rotationalPanel->getAxePoint2TextField ( ))
	CHECK_NULL_PTR_ERROR (_delaunayTetgenPanel)
	CHECK_NULL_PTR_ERROR (_meshInsertionPanel)
	if (0 != _currentPanel)
	{
		_currentParentWidget->layout ( )->removeWidget (_currentPanel);
		_currentPanel->hide ( );
		_currentPanel->setParent (0);
	}
	_currentPanel	= 0;

	// Pour la majorité des cas :
	_blocksPanel->getNameTextField ( )->setLinkedSeizureManagers (0, 0);

	switch (getOperationMethod ( ))
	{
		case QtBlockMeshingPropertyPanel::TRANSFINITE			:
			_currentPanel	= _transfinitePanel;			break;
		case QtBlockMeshingPropertyPanel::DIRECTIONAL			:
			_currentPanel	= _directionalPanel;
			_directionalPanel->getEdgeTextField ( )->setLinkedSeizureManagers (
									0, _blocksPanel->getNameTextField ( ));
			_blocksPanel->getNameTextField ( )->setLinkedSeizureManagers (
									_directionalPanel->getEdgeTextField ( ), 0);
			break;
		case QtBlockMeshingPropertyPanel::ORTHOGONAL				:
			_currentPanel	= _orthogonalPanel;
			_orthogonalPanel->getEdgeTextField ( )->setLinkedSeizureManagers (
									0, _blocksPanel->getNameTextField ( ));
			_blocksPanel->getNameTextField ( )->setLinkedSeizureManagers (
									_orthogonalPanel->getEdgeTextField ( ), 0);
			break;
		case QtBlockMeshingPropertyPanel::ROTATIONAL				:
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
						_blocksPanel->getNameTextField ( ));
			_blocksPanel->getNameTextField ( )->setLinkedSeizureManagers (
				_rotationalPanel->getAxePoint2TextField ( ), 0);
			break;
		case QtBlockMeshingPropertyPanel::DELAUNAY_TETGEN			:
			_currentPanel	= _delaunayTetgenPanel;			break;
		case QtBlockMeshingPropertyPanel::MESH_INSERTION		:
			_currentPanel	= _meshInsertionPanel;			break;
		default	:
		{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Méthode d'opération non supportée ("
			        << (unsigned long)getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtBlockMeshingPropertyPanel::operationMethodCallback")
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

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtBlockMeshingPropertyPanel::operationMethodCallback


// ===========================================================================
//                  LA CLASSE QtBlockMeshingPropertyAction
// ===========================================================================

QtBlockMeshingPropertyAction::QtBlockMeshingPropertyAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtBlockMeshingPropertyPanel*	operationPanel	=
		new QtBlockMeshingPropertyPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtBlockMeshingPropertyAction::QtBlockMeshingPropertyAction


QtBlockMeshingPropertyAction::QtBlockMeshingPropertyAction (
										const QtBlockMeshingPropertyAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtBlockMeshingPropertyAction copy constructor is not allowed.")
}	// QtBlockMeshingPropertyAction::QtBlockMeshingPropertyAction


QtBlockMeshingPropertyAction& QtBlockMeshingPropertyAction::operator = (
										const QtBlockMeshingPropertyAction&)
{
	MGX_FORBIDDEN ("QtBlockMeshingPropertyAction assignment operator is not allowed.")
	return *this;
}	// QtBlockMeshingPropertyAction::operator =


QtBlockMeshingPropertyAction::~QtBlockMeshingPropertyAction ( )
{
}	// QtBlockMeshingPropertyAction::~QtBlockMeshingPropertyAction


QtBlockMeshingPropertyPanel*
					QtBlockMeshingPropertyAction::getMeshingPropertyPanel ( )
{
	return dynamic_cast<QtBlockMeshingPropertyPanel*>(getOperationPanel ( ));
}	// QtBlockMeshingPropertyAction::getMeshingPropertyPanel


void QtBlockMeshingPropertyAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres de maillage des blocs :
	QtBlockMeshingPropertyPanel*	panel	= getMeshingPropertyPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>					blocks		= panel->getBlocksNames ( );
	unique_ptr<BlockMeshingProperty>	properties (panel->getMeshingProperty ( ));
	cmdResult	= getContext ( ).getTopoManager ( ).setMeshingProperty (*properties.get ( ), blocks);

	setCommandResult (cmdResult);
}	// QtBlockMeshingPropertyAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
