/**
 * \file        QtSphereOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        27/05/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "Geom/Vertex.h"
#include "Utils/Vector.h"
#include "QtComponents/QtSphereOperationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Geom/CommandNewSphere.h"
#include "Geom/CommandNewHollowSphere.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
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
//                        LA CLASSE QtSphereOperationPanel
// ===========================================================================

QtSphereOperationPanel::QtSphereOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).sphereOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).sphereOperationTag),
	  _namePanel (0), _hollowedCheckBox (0), _operationMethodComboBox (0),
	  _centerPanel (0), _externalRadiusTextField (0),
	  _internalRadiusTextField (0), _internalRadiusLabel (0), _anglePanel (0),
	  _topologyPanel (0)
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

	// Creuse/Pleine / Méthode de création/modification de la sphère :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	_hollowedCheckBox	= new QCheckBox ("Creuse", this);
	hlayout->addWidget (_hollowedCheckBox);
	connect (_hollowedCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (hollowedModifiedCallback ( )));
	connect (_hollowedCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (parametersModifiedCallback ( )));
	label	= new QLabel (QString::fromUtf8("Méthode"), this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem ("Par centre/rayon");
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);

	// Définition de la sphère :
	QtGroupBox*	groupBox	= new QtGroupBox (this, "Paramètres de la sphère");
	layout->addWidget (groupBox);
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	vlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (vlayout);
	_centerPanel	= new QtMgx3DPointPanel (
		groupBox, "Centre", true, "x :", "y :", "z :",
		0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
		&mainWindow, FilterEntity::GeomVertex, true);
	_centerPanel->layout ( )->setSpacing (5);
	groupBox->layout ( )->setSpacing (5);
	vlayout->addWidget (_centerPanel);
	connect (_centerPanel, SIGNAL (pointAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_centerPanel, SIGNAL (pointRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_centerPanel, SIGNAL (pointModified()),
			this, SLOT (parametersModifiedCallback ( )));

	hlayout	= new QHBoxLayout ( );
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	vlayout->addLayout (hlayout);
	_internalRadiusLabel	= new QLabel ("Rayon interne :", this);
	hlayout->addWidget (_internalRadiusLabel);
	_internalRadiusTextField	= &QtNumericFieldsFactory::createDistanceTextField (groupBox);
	_internalRadiusTextField->setValue(0.5);
//	_internalRadiusTextField->setFixedSize (
//						_internalRadiusTextField->sizeHint ( ));
	hlayout->addWidget (_internalRadiusTextField);
	connect (_internalRadiusTextField, SIGNAL (textEdited (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
	hlayout->addStretch (5);

	hlayout	= new QHBoxLayout ( );
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	vlayout->addLayout (hlayout);
	label	= new QLabel ("Rayon externe :", this);
	hlayout->addWidget (label);
	_externalRadiusTextField	= &QtNumericFieldsFactory::createDistanceTextField (groupBox);
	_externalRadiusTextField->setValue(1.0);
//	_externalRadiusTextField->setFixedSize (
//						_externalRadiusTextField->sizeHint ( ));
	hlayout->addWidget (_externalRadiusTextField);
	connect (_externalRadiusTextField, SIGNAL (textEdited (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
	hlayout->addStretch (5);

	// Portion de sphère :
	groupBox	= new QtGroupBox (this, "Portion de la sphère");
	hlayout	= new QHBoxLayout (groupBox);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (hlayout);
	layout->addWidget (groupBox);
	_anglePanel	= new QtAnglePanel (
				groupBox, "Magix 3D", 4 + MgxNumeric::mgxAngleDecimalNum,
				MgxNumeric::mgxAngleDecimalNum, 90., "degrés");
	hlayout->addWidget (_anglePanel);
	hlayout->addStretch (1);
	_anglePanel->addPredefinedValue ("Entier", 360.);
	_anglePanel->addPredefinedValue ("Demi", 180.);
	_anglePanel->addPredefinedValue ("Quart", 90.);
	_anglePanel->addPredefinedValue ("Huitième", 45.);
	_anglePanel->setPredefinedValue ("Entier");
	connect (_anglePanel, SIGNAL (angleModified(double)), this,
			SLOT (parametersModifiedCallback ( )));

	// Topologie :
	groupBox	= new QtGroupBox (this, "Topologie :");
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
			groupBox, mainWindow, true, false, 3, QtTopologyPanel::OGRID_BLOCKS,
			QtTopologyPanel::OGRID,
			-1 /* Pas d'axe 1 */, defaultEdgesNum, defaultEdgesNum, true, true);
	_topologyPanel->enableTopologyType (
						QtTopologyPanel::STRUCTURED_TOPOLOGY, false);
	_topologyPanel->enableTopologyType (
						QtTopologyPanel::STRUCTURED_FREE_TOPOLOGY, false);
	_topologyPanel->enableTopologyType (
						QtTopologyPanel::INSERTION_TOPOLOGY, false);
	vlayout->addWidget (_topologyPanel);

	CHECK_NULL_PTR_ERROR (_centerPanel->getVertexTextField ( ))
	_centerPanel->getVertexTextField ( )->setLinkedSeizureManagers (0, 0);

	addPreviewCheckBox (true);

	hollowedModifiedCallback ( );
}	// QtSphereOperationPanel::QtSphereOperationPanel


QtSphereOperationPanel::QtSphereOperationPanel (const QtSphereOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _hollowedCheckBox (0), _operationMethodComboBox (0),
	  _centerPanel (0), _externalRadiusTextField (0),
	  _internalRadiusTextField (0), _internalRadiusLabel (0), _anglePanel (0),
	  _topologyPanel (0)
{
	MGX_FORBIDDEN ("QtSphereOperationPanel copy constructor is not allowed.");
}	// QtSphereOperationPanel::QtSphereOperationPanel (const QtSphereOperationPanel&)


QtSphereOperationPanel& QtSphereOperationPanel::operator = (const QtSphereOperationPanel&)
{
	MGX_FORBIDDEN ("QtSphereOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtSphereOperationPanel::QtSphereOperationPanel (const QtSphereOperationPanel&)


QtSphereOperationPanel::~QtSphereOperationPanel ( )
{
}	// QtSphereOperationPanel::~QtSphereOperationPanel


vector<Entity*> QtSphereOperationPanel::getInvolvedEntities ( )
{
	CHECK_NULL_PTR_ERROR (_centerPanel)
	vector<Entity*>	entities;

	if (0 != _centerPanel->getUniqueName ( ).length ( ))
	{
		Geom::Vertex*	vertex	= 
			getContext ( ).getGeomManager ( ).getVertex (
									_centerPanel->getUniqueName ( ), false);
		if (0 != vertex)
			entities.push_back (vertex);
	}	// if (0 != _centerPanel->getUniqueName ( ).length ( ))

	return entities;
}	// QtSphereOperationPanel::getInvolvedEntities


string QtSphereOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtSphereOperationPanel::getGroupName


bool QtSphereOperationPanel::isHollowed ( ) const
{
	CHECK_NULL_PTR_ERROR (_hollowedCheckBox)
	return Qt::Checked == _hollowedCheckBox->checkState ( ) ? true : false;
}	// QtSphereOperationPanel::isHollowed


QtSphereOperationPanel::OPERATION_METHOD
						QtSphereOperationPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtSphereOperationPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtSphereOperationPanel::getOperationMethod


Utils::Math::Point QtSphereOperationPanel::getCenter ( ) const
{
	CHECK_NULL_PTR_ERROR (_centerPanel)
	return _centerPanel->getPoint();
}	// QtSphereOperationPanel::getCenter

double QtSphereOperationPanel::getExternalRadius ( ) const
{
	CHECK_NULL_PTR_ERROR (_externalRadiusTextField)
	return _externalRadiusTextField->getValue ( );
}	// QtSphereOperationPanel::getExternalRadius


double QtSphereOperationPanel::getInternalRadius ( ) const
{
	CHECK_NULL_PTR_ERROR (_internalRadiusTextField)
	return _internalRadiusTextField->getValue ( );
}	// QtSphereOperationPanel::getInternalRadius


Portion::Type QtSphereOperationPanel::getPortion ( ) const
{
	CHECK_NULL_PTR_ERROR (_anglePanel)
	try
	{
		return (Portion::Type)(_anglePanel->getPortion ( ));
	}
	catch (...)
	{
		return (Portion::Type)0;
	}
}	// QtSphereOperationPanel::getPortion


double QtSphereOperationPanel::getAngle ( ) const
{
	CHECK_NULL_PTR_ERROR (_anglePanel)
	return _anglePanel->getAngle ( );
}	// QtSphereOperationPanel::getAngle


bool QtSphereOperationPanel::createTopology ( ) const
{
	 CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->createTopology ( );
}	// QtSphereOperationPanel::createTopology


QtTopologyPanel::TOPOLOGY_TYPE QtSphereOperationPanel::getTopologyType ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getTopologyType ( );
}	// QtSphereOperationPanel::getTopologyType


int QtSphereOperationPanel::getNj ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe2EdgesNum ( );
}	// QtSphereOperationPanel::getNj


int QtSphereOperationPanel::getNk ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe3EdgesNum ( );
}	// QtSphereOperationPanel::getNk


double QtSphereOperationPanel::getOGridRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getOGridRatio ( );
}	// QtSphereOperationPanel::getOGridRatio


void QtSphereOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_centerPanel)
	CHECK_NULL_PTR_ERROR (_externalRadiusTextField)
	CHECK_NULL_PTR_ERROR (_internalRadiusTextField)
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	_namePanel->autoUpdate ( );
	_centerPanel->reset ( );
	_externalRadiusTextField->setValue (1.);
	_externalRadiusTextField->setValue (0.5);
	const int	defaultEdgesNum	=
		getMainWindow ( )->getContext( ).getTopoManager( ).getDefaultNbMeshingEdges( );
	_topologyPanel->setTopologyType (QtTopologyPanel::OGRID_BLOCKS);
//	_topologyPanel->setAxe1EdgesNum (-1);	// Non instancié
	_topologyPanel->setAxe2EdgesNum (defaultEdgesNum);
	_topologyPanel->setAxe3EdgesNum (defaultEdgesNum);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtSphereOperationPanel::reset


void QtSphereOperationPanel::validate ( )
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
		error << "QtSphereOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtSphereOperationPanel::POINT_POINT	:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode d'opération de création/modification "
			      << "de sphère sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtSphereOperationPanel::validate : index de méthode "
			      << "d'opération de création/modification de sphère "
			      << "invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX ( ), "Abscisse point 1 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY ( ), "Ordonnée point 1 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ ( ), "Elévation point 1 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX2 ( ), "Abscisse point 2 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY2 ( ), "Ordonnée point 2 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ2 ( ), "Elévation point 2 : ", error)
	VALIDATED_FIELD_EXPRESSION_EVALUATION (_topologyPanel->validate ( ), "Topologie : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtSphereOperationPanel::validate


void QtSphereOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_centerPanel)
	_centerPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		// CP : pas de ->setUniqueName ("") car en Qt 5.14.* le panneau se retrouve initialisé à la construction avec des (0, 0, 0)
		_centerPanel->getVertexTextField ( )->setText (QString (""));
//		_centerPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtSphereOperationPanel::cancel


void QtSphereOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_centerPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		CHECK_NULL_PTR_ERROR (_namePanel)
		CHECK_NULL_PTR_ERROR (_topologyPanel)
		CHECK_NULL_PTR_ERROR (_centerPanel)

		vector<string>	selectedVertices	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomVertex);
		if (2 >= selectedVertices.size ( ))
		{
			if (1 <= selectedVertices.size ( ))
				_centerPanel->setUniqueName (selectedVertices [0]);
		}	// if (2 >= selectedVertices.size ( ))

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_centerPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_namePanel->autoUpdate ( );
	_topologyPanel->autoUpdate ( );
	_centerPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtSphereOperationPanel::autoUpdate


void QtSphereOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_centerPanel)
	_centerPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtSphereOperationPanel::operationCompleted


void QtSphereOperationPanel::hollowedModifiedCallback ( )
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
}	// QtSphereOperationPanel::hollowedModifiedCallback

void QtSphereOperationPanel::preview (bool show, bool destroyInteractor)
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
		const double		externalRadius (getExternalRadius ( ));
		const double		internalRadius (getInternalRadius ( ));
		const double		angle (getAngle ( ));
		const Portion::Type	portion	= getPortion ( );

		if ((Portion::Type)0 == portion)
		{	// Angle personnalisé
			if (false == hollowed)
				command = new CommandNewSphere(*context, center, externalRadius, angle, "");
			else
				command = new CommandNewHollowSphere(*context, center, internalRadius, externalRadius, angle, "");
		}	// if ((Portion::Type)0 == portion)
		else
		{	// Portion prédéfinie
			if (false == hollowed)
				command = new CommandNewSphere(*context, center, externalRadius, portion, "");
			else
				command = new CommandNewHollowSphere(*context, center, internalRadius, externalRadius, portion, "");
		}	// else if ((Portion::Type)0 == portion)
		Geom::GeomDisplayRepresentation	dr (DisplayRepresentation::WIRE);
		dr.addRepresentation(DisplayRepresentation::ISOCURVE);
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
}	// QtSphereOperationPanel::preview


// ===========================================================================
//                  LA CLASSE QtSphereOperationAction
// ===========================================================================

QtSphereOperationAction::QtSphereOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtSphereOperationPanel*	operationPanel	=
		new QtSphereOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy,
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtSphereOperationAction::QtSphereOperationAction


QtSphereOperationAction::QtSphereOperationAction (
										const QtSphereOperationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtSphereOperationAction copy constructor is not allowed.")
}	// QtSphereOperationAction::QtSphereOperationAction


QtSphereOperationAction& QtSphereOperationAction::operator = (
										const QtSphereOperationAction&)
{
	MGX_FORBIDDEN ("QtSphereOperationAction assignment operator is not allowed.")
	return *this;
}	// QtSphereOperationAction::operator =


QtSphereOperationAction::~QtSphereOperationAction ( )
{
}	// QtSphereOperationAction::~QtSphereOperationAction


QtSphereOperationPanel* QtSphereOperationAction::getSpherePanel ( )
{
	return dynamic_cast<QtSphereOperationPanel*>(getOperationPanel ( ));
}	// QtSphereOperationAction::getSpherePanel


void QtSphereOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
//	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de création de la sphère :
	QtSphereOperationPanel*	panel	= getSpherePanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const bool			hollowed	= panel->isHollowed ( );
	const Point			center (panel->getCenter ( ));
	const double		externalRadius (panel->getExternalRadius ( ));
	const double		internalRadius (panel->getInternalRadius ( ));
	const double		angle (panel->getAngle ( ));
	const Portion::Type	portion	= panel->getPortion ( );
	const string		name	= panel->getGroupName ( );
	if (false == panel->createTopology ( ))
	{
		if ((Portion::Type)0 == portion)
		{	// Angle personnalisé
			if (false == hollowed)
				cmdResult	= getContext ( ).getGeomManager ( ).newSphere (center, externalRadius, angle, name);
			else
				cmdResult	= getContext ( ).getGeomManager ( ).newHollowSphere (center, internalRadius, externalRadius, angle, name);
		}	// if ((Portion::Type)0 == portion)
		else
		{	// Portion prédéfinie
			if (false == hollowed)
				cmdResult	= getContext ( ).getGeomManager ( ).newSphere (center, externalRadius, portion, name);
			else
				cmdResult	= getContext ( ).getGeomManager ( ).newHollowSphere (center, internalRadius, externalRadius, portion, name);
		}	// else if ((Portion::Type)0 == portion)
	}
	else
	{
		if ((Portion::Type)0 == portion){
			UTF8String	error (Charset::UTF_8);
			error << "Il n'est pas permis d'utiliser un angle personnalisé dans le cas d'une sphère (pleine ou creuse) avec topologie.";
			throw Exception (error);
		}

		bool	structured	= false;
		switch (panel->getTopologyType ( ))
		{
			case QtTopologyPanel::UNSTRUCTURED_TOPOLOGY	:
			    structured  = false;
			break;
			case QtTopologyPanel::OGRID_BLOCKS	:
				structured	= true;
			break;
			default	:
			{
				INTERNAL_ERROR (exc, "QtSphereOperationAction::executeOperation", "Topologie non renseignée")
				throw exc;
			}
		}	// switch (panel->getTopologyType ( ))
		const int		ni		= panel->getNj ( );
		const int		nr		= panel->getNk ( );
		const double	ratio	= panel->getOGridRatio ( );
		if (false == hollowed)
			cmdResult	= getContext ( ).getTopoManager ( ).newSphereWithTopo (center, externalRadius, portion, structured, ratio, ni, nr,name);
		else
			cmdResult	= getContext ( ).getTopoManager ( ).newHollowSphereWithTopo (center, internalRadius, externalRadius, portion, structured, ni, nr, name);
	}	// else if (false == panel->createTopology ( ))

	setCommandResult (cmdResult);
}	// QtSphereOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
