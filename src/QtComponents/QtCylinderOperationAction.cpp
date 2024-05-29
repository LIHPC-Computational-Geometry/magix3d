/**
 * \file        QtCylinderOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        11/12/2012
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "Geom/Vertex.h"
#include "Utils/MgxNumeric.h"
#include "Utils/Vector.h"
#include "QtComponents/QtCylinderOperationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Geom/CommandNewCylinder.h"
#include "Geom/CommandNewHollowCylinder.h"

#include <TkUtil/MemoryError.h>
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
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtCylinderOperationPanel
// ===========================================================================

QtCylinderOperationPanel::QtCylinderOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).cylinderOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).cylinderOperationTag),
	  _namePanel (0), _hollowedCheckBox (0), _operationMethodComboBox (0),
	  _coordinatesPanel (0), _externalRadiusTextField (0),
	  _internalRadiusTextField (0), _internalRadiusLabel (0),
	  _vectorPanel (0), _anglePanel (0), _topologyPanel (0)
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
							this, "Groupe", mainWindow, 3, creationPolicy, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// Creux/Plein / Méthode de création/modification du cylindre :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	_hollowedCheckBox	= new QCheckBox ("Creux", this);
	hlayout->addWidget (_hollowedCheckBox);
	connect (_hollowedCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (hollowedModifiedCallback ( )));
	connect (_hollowedCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (parametersModifiedCallback ( )));
	label	= new QLabel (QString::fromUtf8("Méthode"), this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem ("Par centre, rayon, vecteur et angle");
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);

	// Définition du cylindre :
	QtGroupBox*	groupBox	= new QtGroupBox (this, "Paramètres du cylindre");
	layout->addWidget (groupBox);
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	vlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (vlayout);
	_coordinatesPanel	= new QtMgx3DPointPanel (
		groupBox, "Centre de la base", true, "x :", "y :", "z :",
		0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
		&mainWindow, FilterEntity::GeomVertex, true);
	_coordinatesPanel->layout ( )->setSpacing (5);
	connect (_coordinatesPanel, SIGNAL (pointModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));
	groupBox->setMargin (0);
	groupBox->layout ( )->setSpacing (5);
	vlayout->addWidget (_coordinatesPanel);

	CHECK_NULL_PTR_ERROR (_coordinatesPanel->getVertexTextField ( ))
	_coordinatesPanel->getVertexTextField ( )->setLinkedSeizureManagers (0, 0);

	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	vlayout->addLayout (hlayout);
	_internalRadiusLabel	= new QLabel ("Rayon interne :", groupBox);
	hlayout->addWidget (_internalRadiusLabel, 1);
	_internalRadiusTextField	= &QtNumericFieldsFactory::createDistanceTextField (groupBox);
//	_internalRadiusTextField->setRange (0., 100000);	// Quel max ???
	_internalRadiusTextField->setValue(0.5); // histoire d'avoir une première valeur
	hlayout->addWidget (_internalRadiusTextField, 1);
//	_internalRadiusTextField->setFixedSize (
//								_internalRadiusTextField->sizeHint ( ));
	connect (_internalRadiusTextField, SIGNAL (textEdited (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
	hlayout->addStretch (10);

	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	vlayout->addLayout (hlayout);
	label	= new QLabel ("Rayon externe :", groupBox);
	hlayout->addWidget (label, 1);
	_externalRadiusTextField	= &QtNumericFieldsFactory::createDistanceTextField (groupBox);
	_externalRadiusTextField->setValue(1.0); // histoire d'avoir une première valeur
//	_externalRadiusTextField->setRange (0., 100000);	// Quel max ???
	hlayout->addWidget (_externalRadiusTextField, 1);
//	_externalRadiusTextField->setFixedSize (
//								_externalRadiusTextField->sizeHint ( ));
	connect (_externalRadiusTextField, SIGNAL (textEdited (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
	hlayout->addStretch (10.);

	_vectorPanel	= new QtMgx3DVectorPanel (
		groupBox, "Vecteur directeur", true, "dx :", "dy :", "dz :",
		10., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
		&mainWindow, FilterEntity::AllEdges, true);
	vlayout->addWidget (_vectorPanel);
	connect (_vectorPanel, SIGNAL (vectorModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));

	QtGroupBox*	angleGroupBox	= new QtGroupBox ("Portion du cylindre", this);
	hlayout	= new QHBoxLayout (angleGroupBox);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	angleGroupBox->setLayout (hlayout);
	vlayout->addWidget (angleGroupBox);
	_anglePanel	= new QtAnglePanel (
			angleGroupBox, "Magix 3D", 4 + MgxNumeric::mgxAngleDecimalNum,
			MgxNumeric::mgxAngleDecimalNum, 90., "degrés");
	hlayout->addWidget (_anglePanel);
	hlayout->addStretch (1);
//	_anglePanel->setRange (0., 360.);
	_anglePanel->addPredefinedValue ("Entier", 360.);
	_anglePanel->addPredefinedValue ("Demi", 180.);
	_anglePanel->addPredefinedValue ("Quart", 90.);
	_anglePanel->addPredefinedValue ("Cinquième", 72.);
	_anglePanel->addPredefinedValue ("Sixième", 60.);
	_anglePanel->addPredefinedValue ("Huitième", 45.);
	_anglePanel->setPredefinedValue ("Entier");
	connect (_anglePanel, SIGNAL (angleModified(double)), this,
			SLOT (parametersModifiedCallback ( )));

	// Topologie :
	groupBox	= new QtGroupBox ("Topologie :", this);
	layout->addWidget (groupBox);
	vlayout	= new QVBoxLayout (groupBox);
	vlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (vlayout);
	const int	defaultEdgesNum	=
		mainWindow.getContext( ).getTopoManager( ).getDefaultNbMeshingEdges( );
	_topologyPanel	= new QtTopologyPanel (
			groupBox, mainWindow, true, false, 3,
			QtTopologyPanel::OGRID_BLOCKS, QtTopologyPanel::OGRID,
			defaultEdgesNum, defaultEdgesNum, defaultEdgesNum, true, true);
	_topologyPanel->enableTopologyType (
							QtTopologyPanel::STRUCTURED_TOPOLOGY, false);
	_topologyPanel->enableTopologyType (
							QtTopologyPanel::STRUCTURED_FREE_TOPOLOGY, false);
	_topologyPanel->enableTopologyType (
							QtTopologyPanel::INSERTION_TOPOLOGY, false);
	vlayout->addWidget (_topologyPanel);

	addPreviewCheckBox (true);

	hollowedModifiedCallback ( );
}	// QtCylinderOperationPanel::QtCylinderOperationPanel


QtCylinderOperationPanel::QtCylinderOperationPanel (
										const QtCylinderOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _hollowedCheckBox (0), _operationMethodComboBox (0),
	  _coordinatesPanel (0), _externalRadiusTextField (0),
	  _internalRadiusTextField (0), _internalRadiusLabel (0),
	  _vectorPanel (0), _anglePanel(0), _topologyPanel (0)
{
	MGX_FORBIDDEN ("QtCylinderOperationPanel copy constructor is not allowed.");
}	// QtCylinderOperationPanel::QtCylinderOperationPanel (const QtCylinderOperationPanel&)


QtCylinderOperationPanel& QtCylinderOperationPanel::operator = (
											const QtCylinderOperationPanel&)
{
	MGX_FORBIDDEN ("QtCylinderOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtCylinderOperationPanel::QtCylinderOperationPanel (const QtCylinderOperationPanel&)


QtCylinderOperationPanel::~QtCylinderOperationPanel ( )
{
}	// QtCylinderOperationPanel::~QtCylinderOperationPanel


vector<Entity*> QtCylinderOperationPanel::getInvolvedEntities ( )
{
	CHECK_NULL_PTR_ERROR (_coordinatesPanel)
	vector<Entity*>	entities;

	if (0 != _coordinatesPanel->getUniqueName ( ).length ( ))
	{
		Geom::Vertex*	vertex	=
			getContext ( ).getGeomManager ( ).getVertex (
								_coordinatesPanel->getUniqueName ( ), false);
		if (0 != vertex)
			entities.push_back (vertex);
	}	// if (0 != _coordinatesPanel->getUniqueName ( ).length ( ))

	return entities;
}	// QtCylinderOperationPanel::getInvolvedEntities


string QtCylinderOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtCylinderOperationPanel::getGroupName


bool QtCylinderOperationPanel::isHollowed ( ) const
{
	CHECK_NULL_PTR_ERROR (_hollowedCheckBox)
	return Qt::Checked == _hollowedCheckBox->checkState ( ) ? true : false;
}	// QtCylinderOperationPanel::isHollowed


QtCylinderOperationPanel::OPERATION_METHOD
						QtCylinderOperationPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtCylinderOperationPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtCylinderOperationPanel::getOperationMethod


Utils::Math::Point QtCylinderOperationPanel::getCenter() const
{
	CHECK_NULL_PTR_ERROR (_coordinatesPanel)
	return _coordinatesPanel->getPoint ( );
}


double QtCylinderOperationPanel::getExternalRadius ( ) const
{
	CHECK_NULL_PTR_ERROR (_externalRadiusTextField)
	return _externalRadiusTextField->getValue ( );
}	// QtCylinderOperationPanel::getExternalRadius


double QtCylinderOperationPanel::getInternalRadius ( ) const
{
	CHECK_NULL_PTR_ERROR (_internalRadiusTextField)
	return _internalRadiusTextField->getValue ( );
}	// QtCylinderOperationPanel::getInternalRadius


Math::Vector QtCylinderOperationPanel::getAxis ( ) const
{
	CHECK_NULL_PTR_ERROR (_vectorPanel)
	return _vectorPanel->getVector ( );
}	// QtCylinderOperationPanel::getAxis


double QtCylinderOperationPanel::getAngle ( ) const
{
	CHECK_NULL_PTR_ERROR (_anglePanel)
	return _anglePanel->getAngle ( );
}	// QtCylinderOperationPanel::getAngle


bool QtCylinderOperationPanel::createTopology ( ) const
{
	 CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->createTopology ( );
}	// QtCylinderOperationPanel::createTopology


QtTopologyPanel::TOPOLOGY_TYPE QtCylinderOperationPanel::getTopologyType ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getTopologyType ( );
}	// QtCylinderOperationPanel::getTopologyType


double QtCylinderOperationPanel::getOGridRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getOGridRatio ( );
}	// QtCylinderOperationPanel::getOGridRatio


int QtCylinderOperationPanel::getAxeEdgesNum ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe1EdgesNum ( );
}	// QtCylinderOperationPanel::getAxeEdgesNum


int QtCylinderOperationPanel::getiEdgesNum ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe2EdgesNum ( );
}	// QtCylinderOperationPanel::getiEdgesNum


int QtCylinderOperationPanel::getrLayersNum ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe3EdgesNum ( );
}	// QtCylinderOperationPanel::getrLayersNum


void QtCylinderOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_externalRadiusTextField)
	CHECK_NULL_PTR_ERROR (_internalRadiusTextField)
	CHECK_NULL_PTR_ERROR (_coordinatesPanel)
	CHECK_NULL_PTR_ERROR (_vectorPanel)
	CHECK_NULL_PTR_ERROR (getMainWindow ( ))
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	_namePanel->autoUpdate ( );
	_coordinatesPanel->reset ( );
	_vectorPanel->reset ( );
	_internalRadiusTextField->setValue(0.5);
	_externalRadiusTextField->setValue(1.0);
	const int	defaultEdgesNum	=
		getMainWindow ( )->getContext( ).getTopoManager( ).getDefaultNbMeshingEdges( );
	_topologyPanel->setTopologyType (QtTopologyPanel::OGRID_BLOCKS);
	_topologyPanel->setAxe1EdgesNum (defaultEdgesNum);
	_topologyPanel->setAxe2EdgesNum (defaultEdgesNum);
	_topologyPanel->setAxe3EdgesNum (defaultEdgesNum);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtCylinderOperationPanel::reset


void QtCylinderOperationPanel::validate ( )
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
		error << "QtCylinderOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtCylinderOperationPanel::CENTER_RADIUS_VECTOR_ANGLE	:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode d'opération de création/modification "
			      << "de cylindre sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtCylinderOperationPanel::validate : index de méthode "
			      << "d'opération de création/modification de cylindre "
			      << "invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX ( ), "Abscisse : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY ( ), "Ordonnée : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ ( ), "Elévation : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getRadius ( ), "Rayon : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getXVector ( ), "dx : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getYVector ( ), "dy : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZVector ( ), "dz : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getAngle ( ), "Angle : ", error)
	VALIDATED_FIELD_EXPRESSION_EVALUATION (_topologyPanel->validate ( ), "Topologie : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtCylinderOperationPanel::validate


void QtCylinderOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_coordinatesPanel)
	_coordinatesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		// CP : pas de ->setUniqueName ("") car en Qt 5.14.* le panneau se retrouve initialisé à la construction avec des (0, 0, 0)
		_coordinatesPanel->getVertexTextField ( )->setText (QString (""));
//		_coordinatesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtCylinderOperationPanel::cancel


void QtCylinderOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	CHECK_NULL_PTR_ERROR (_coordinatesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedVertices	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomVertex);
		if (1 == selectedVertices.size ( ))
			_coordinatesPanel->setUniqueName (selectedVertices [0]);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	 _coordinatesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_namePanel->autoUpdate ( );
	_topologyPanel->autoUpdate ( );
	_coordinatesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtCylinderOperationPanel::autoUpdate


void QtCylinderOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _coordinatesPanel)
		_coordinatesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtCylinderOperationPanel::operationCompleted


void QtCylinderOperationPanel::hollowedModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_internalRadiusTextField)
	CHECK_NULL_PTR_ERROR (_internalRadiusLabel)
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	const bool	hollowed	= isHollowed ( );
	
	_internalRadiusLabel->setEnabled (hollowed);
	_internalRadiusTextField->setEnabled (hollowed);
	_topologyPanel->enableOGridRatio (!hollowed);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtCylinderOperationPanel::hollowedModifiedCallback

void QtCylinderOperationPanel::preview (bool show, bool destroyInteractor)
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

	CommandCreateGeom* command = 0;
	try
	{
		Context*		context		= dynamic_cast<Context*>(&getContext ( ));
		CHECK_NULL_PTR_ERROR (context)

		DisplayProperties	graphicalProps;
		graphicalProps.setWireColor (Color (
				255 * Resources::instance ( )._previewColor.getRed ( ),
				255 * Resources::instance ( )._previewColor.getGreen ( ),
				255 * Resources::instance ( )._previewColor.getBlue ( )));
		graphicalProps.setLineWidth (
						Resources::instance ( )._previewWidth.getValue ( ));

		const bool			hollowed	= isHollowed ( );
		const Point			center (getCenter());
		const double	externalRadius	= getExternalRadius ( );
		const double	internalRadius	= getInternalRadius ( );
		const Vector	v  = getAxis ( );
		const double	angle			= getAngle ( );
		const string	name			= getGroupName ( );
		if (false == hollowed)
			command = new CommandNewCylinder (*context, center, externalRadius, v, angle, "");
		else
			command = new CommandNewHollowCylinder (*context, center, internalRadius, externalRadius, v, angle, "");

		Geom::GeomDisplayRepresentation	dr (DisplayRepresentation::WIRE);
		command->getPreviewRepresentation (dr);
		const vector<Math::Point>&	points	= dr.getPoints ( );
		const vector<size_t>&		indices	= dr.getCurveDiscretization ( );

		RenderingManager::RepresentationID	repID	=
				getRenderingManager ( ).createSegmentsWireRepresentation (
										points, indices, graphicalProps, true);
		registerPreviewedObject (repID);

		getRenderingManager ( ).forceRender ( );
		delete command;
	}
	catch (...)
	{
		if (command)
			delete command;
	}
}	// QtCylinderOperationPanel::preview


// ===========================================================================
//                  LA CLASSE QtCylinderOperationAction
// ===========================================================================

QtCylinderOperationAction::QtCylinderOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtCylinderOperationPanel*	operationPanel	=
		new QtCylinderOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy,
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtCylinderOperationAction::QtCylinderOperationAction


QtCylinderOperationAction::QtCylinderOperationAction (
										const QtCylinderOperationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtCylinderOperationAction copy constructor is not allowed.")
}	// QtCylinderOperationAction::QtCylinderOperationAction


QtCylinderOperationAction& QtCylinderOperationAction::operator = (
										const QtCylinderOperationAction&)
{
	MGX_FORBIDDEN ("QtCylinderOperationAction assignment operator is not allowed.")
	return *this;
}	// QtCylinderOperationAction::operator =


QtCylinderOperationAction::~QtCylinderOperationAction ( )
{
}	// QtCylinderOperationAction::~QtCylinderOperationAction


QtCylinderOperationPanel* QtCylinderOperationAction::getCylinderPanel ( )
{
	return dynamic_cast<QtCylinderOperationPanel*>(getOperationPanel ( ));
}	// QtCylinderOperationAction::getCylinderPanel


void QtCylinderOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
//	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de création du cylindre :
	QtCylinderOperationPanel*	panel	= getCylinderPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const bool		hollowed		= panel->isHollowed ( );
	const Point	center (panel->getCenter ( ));
	const double	externalRadius	= panel->getExternalRadius ( );
	const double	internalRadius	= panel->getInternalRadius ( );
	const Vector	v               = panel->getAxis ( );
	const double	angle			= panel->getAngle ( );
	const string	name			= panel->getGroupName ( );
	if (false == panel->createTopology ( ))
	{
		if (false == hollowed)
			cmdResult	= getContext ( ).getGeomManager ( ).newCylinder (center, externalRadius, v, angle, name);
		else
			cmdResult	= getContext ( ).getGeomManager ( ).newHollowCylinder (center, internalRadius, externalRadius, v, angle, name);
	}	// if (false == panel->createTopology ( ))
	else
	{
		bool	structured	= false;
		double	ratio		= 1.0;
// TODO [CP] A revoir
		switch (panel->getTopologyType ( ))
		{
			case QtTopologyPanel::UNSTRUCTURED_TOPOLOGY	:
			    structured  = false;
			    ratio       = 1.0;
				break;
			case QtTopologyPanel::STRUCTURED_TOPOLOGY	:
                structured  = true;
                ratio       = 1.0; // c'est le ratio à 1 qui dicte le fait de n'avoir qu'un bloc
                break;
			case QtTopologyPanel::OGRID_BLOCKS	:
				structured	= true;
				ratio		= panel->getOGridRatio ( );
		}	// switch (panel->getTopologyType ( ))
		const int	naxe	= panel->getAxeEdgesNum ( );
		const int	ni		= panel->getiEdgesNum ( );
		const int	nr		= panel->getrLayersNum ( );
		if (false == hollowed)
			cmdResult	= getContext ( ).getTopoManager ( ).newCylinderWithTopo (
								center, externalRadius, v, angle, structured, ratio, naxe, ni, nr, name);
		else
			getContext ( ).getTopoManager ( ).newHollowCylinderWithTopo (
								center, internalRadius, externalRadius, v, angle, structured, naxe, ni, nr, name);
	}	// else if (false == panel->createTopology ( ))

	setCommandResult (cmdResult);
}	// QtCylinderOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
