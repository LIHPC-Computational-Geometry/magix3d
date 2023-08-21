/**
 * \file		QtSpherePartOperationAction.cpp
 * \author		Eric
 * \date		30/3/2016
 *
 * à partir de QtSphereOperationAction fait par Charles
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"
#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "Geom/Vertex.h"
#include "Utils/Vector.h"
#include "QtComponents/QtSpherePartOperationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Geom/CommandNewSpherePart.h"
#include "Geom/CommandNewHollowSpherePart.h"

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
//                        LA CLASSE QtSpherePartOperationPanel
// ===========================================================================

QtSpherePartOperationPanel::QtSpherePartOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).aiguilleOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).aiguilleOperationTag),
	  _namePanel (0), _hollowedCheckBox (0), _operationMethodComboBox (0),
	  _externalRadiusTextField (0),
	  _internalRadiusTextField (0), _internalRadiusLabel (0), _angleYPanel (0),
	  _angleZPanel (0), _topologyPanel (0)
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

	// Creuse/Pleine / Méthode de création/modification de l'aiguille :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
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
	_operationMethodComboBox->addItem ("Suivant rayon");
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);

	// Définition de l'aiguille :
	QtGroupBox*	groupBox	= new QtGroupBox (this, "Paramètres de l'aiguille");
	layout->addWidget (groupBox);
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	vlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (vlayout);

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
	hlayout->addWidget (_externalRadiusTextField);
	connect (_externalRadiusTextField, SIGNAL (textEdited (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
	hlayout->addStretch (5);

	// Angle Y :
	groupBox	= new QtGroupBox (this, "Ouverture de l'aiguille dans le plan xOy");
	hlayout	= new QHBoxLayout (groupBox);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (hlayout);
	layout->addWidget (groupBox);
	_angleYPanel	= new QtAnglePanel (
				groupBox, "Magix 3D", 4 + MgxNumeric::mgxAngleDecimalNum,
				MgxNumeric::mgxAngleDecimalNum, 5., "degrés");
	_angleYPanel->hidePartComboBox();
	hlayout->addWidget (_angleYPanel);
	hlayout->addStretch (1);
	connect (_angleYPanel, SIGNAL (angleModified (double)), this,
	         SLOT (parametersModifiedCallback ( )));

	// Angle Z :
	groupBox	= new QtGroupBox (this, "Ouverture de l'aiguille dans le plan xOz");
	hlayout	= new QHBoxLayout (groupBox);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (hlayout);
	layout->addWidget (groupBox);
	_angleZPanel	= new QtAnglePanel (
				groupBox, "Magix 3D", 4 + MgxNumeric::mgxAngleDecimalNum,
				MgxNumeric::mgxAngleDecimalNum, 5., "degrés");
	_angleZPanel->hidePartComboBox();
	hlayout->addWidget (_angleZPanel);
	hlayout->addStretch (1);
	connect (_angleZPanel, SIGNAL (angleModified (double)), this,
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
			groupBox, mainWindow, true, false, 3, QtTopologyPanel::STRUCTURED_TOPOLOGY,
			QtTopologyPanel::CARTESIAN,
			defaultEdgesNum, defaultEdgesNum, defaultEdgesNum, true, true);
	_topologyPanel->enableTopologyType (
						QtTopologyPanel::STRUCTURED_TOPOLOGY, true);
	_topologyPanel->enableTopologyType (
							QtTopologyPanel::OGRID_BLOCKS, false);
	_topologyPanel->enableTopologyType (
							QtTopologyPanel::STRUCTURED_FREE_TOPOLOGY, false);
	_topologyPanel->enableTopologyType (
							QtTopologyPanel::UNSTRUCTURED_TOPOLOGY, false);
	_topologyPanel->enableTopologyType (
							QtTopologyPanel::INSERTION_TOPOLOGY, false);

	vlayout->addWidget (_topologyPanel);

	addPreviewCheckBox (true);

	hollowedModifiedCallback ( );
}	// QtSpherePartOperationPanel::QtSpherePartOperationPanel


QtSpherePartOperationPanel::QtSpherePartOperationPanel (const QtSpherePartOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _hollowedCheckBox (0), _operationMethodComboBox (0),
	  _externalRadiusTextField (0),
	  _internalRadiusTextField (0), _internalRadiusLabel (0), _angleYPanel (0),
	  _angleZPanel (0), _topologyPanel (0)
{
	MGX_FORBIDDEN ("QtSpherePartOperationPanel copy constructor is not allowed.");
}	// QtSpherePartOperationPanel::QtSpherePartOperationPanel (const QtSpherePartOperationPanel&)


QtSpherePartOperationPanel& QtSpherePartOperationPanel::operator = (
											const QtSpherePartOperationPanel&)
{
	MGX_FORBIDDEN ("QtSpherePartOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtSpherePartOperationPanel::QtSpherePartOperationPanel (const QtSpherePartOperationPanel&)


QtSpherePartOperationPanel::~QtSpherePartOperationPanel ( )
{
}	// QtSpherePartOperationPanel::~QtSpherePartOperationPanel


void QtSpherePartOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_externalRadiusTextField)
	CHECK_NULL_PTR_ERROR (_internalRadiusTextField)
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	CHECK_NULL_PTR_ERROR (getMainWindow ( ))
	const int	defaultEdgesNum	= getMainWindow ( )->getContext (
								).getTopoManager( ).getDefaultNbMeshingEdges( );
	_namePanel->autoUpdate ( );
	_externalRadiusTextField->setValue (1.);
	_internalRadiusTextField->setValue (0.5);
	_topologyPanel->setTopologyType (QtTopologyPanel::STRUCTURED_TOPOLOGY);
	_topologyPanel->setAxe1EdgesNum (defaultEdgesNum);
	_topologyPanel->setAxe2EdgesNum (defaultEdgesNum);
	_topologyPanel->setAxe3EdgesNum (defaultEdgesNum);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtSpherePartOperationPanel::reset


vector<Entity*> QtSpherePartOperationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	return entities;
}	// QtSpherePartOperationPanel::getInvolvedEntities


string QtSpherePartOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtSpherePartOperationPanel::getGroupName


bool QtSpherePartOperationPanel::isHollowed ( ) const
{
	CHECK_NULL_PTR_ERROR (_hollowedCheckBox)
	return Qt::Checked == _hollowedCheckBox->checkState ( ) ? true : false;
}	// QtSpherePartOperationPanel::isHollowed


QtSpherePartOperationPanel::OPERATION_METHOD
						QtSpherePartOperationPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtSpherePartOperationPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtSpherePartOperationPanel::getOperationMethod


double QtSpherePartOperationPanel::getExternalRadius ( ) const
{
	CHECK_NULL_PTR_ERROR (_externalRadiusTextField)
	return _externalRadiusTextField->getValue ( );
}	// QtSpherePartOperationPanel::getExternalRadius


double QtSpherePartOperationPanel::getInternalRadius ( ) const
{
	CHECK_NULL_PTR_ERROR (_internalRadiusTextField)
	return _internalRadiusTextField->getValue ( );
}	// QtSpherePartOperationPanel::getInternalRadius


double QtSpherePartOperationPanel::getAngleY ( ) const
{
	CHECK_NULL_PTR_ERROR (_angleYPanel)
	return _angleYPanel->getAngle ( );
}	// QtSpherePartOperationPanel::getAngleY

double QtSpherePartOperationPanel::getAngleZ ( ) const
{
	CHECK_NULL_PTR_ERROR (_angleZPanel)
	return _angleZPanel->getAngle ( );
}	// QtSpherePartOperationPanel::getAngleZ


bool QtSpherePartOperationPanel::createTopology ( ) const
{
	 CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->createTopology ( );
}	// QtSpherePartOperationPanel::createTopology


QtTopologyPanel::TOPOLOGY_TYPE QtSpherePartOperationPanel::getTopologyType ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getTopologyType ( );
}	// QtSpherePartOperationPanel::getTopologyType


int QtSpherePartOperationPanel::getNi ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe1EdgesNum ( );
}	// QtSpherePartOperationPanel::getNi

int QtSpherePartOperationPanel::getNj ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe2EdgesNum ( );
}	// QtSpherePartOperationPanel::getNj


int QtSpherePartOperationPanel::getNk ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe3EdgesNum ( );
}	// QtSpherePartOperationPanel::getNk


void QtSpherePartOperationPanel::validate ( )
{
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
		error << "QtSpherePartOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtSpherePartOperationPanel::RAYON	:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode d'opération de création/modification "
			      << "d'aiguille sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtSpherePartOperationPanel::validate : index de méthode "
			      << "d'opération de création/modification d'aiguille "
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
}	// QtSpherePartOperationPanel::validate


void QtSpherePartOperationPanel::autoUpdate ( )
{

	_namePanel->autoUpdate ( );
	_topologyPanel->autoUpdate ( );

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtSpherePartOperationPanel::autoUpdate


void QtSpherePartOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtSpherePartOperationPanel::operationCompleted


void QtSpherePartOperationPanel::hollowedModifiedCallback ( )
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
}	// QtSpherePartOperationPanel::hollowedModifiedCallback

void QtSpherePartOperationPanel::preview (bool show, bool destroyInteractor)
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
		const double		externalRadius (getExternalRadius ( ));
		const double		internalRadius (getInternalRadius ( ));
		const double		angleY (getAngleY ( ));
		const double		angleZ (getAngleZ ( ));

		if (false == hollowed)
			command = new CommandNewSpherePart(*context, externalRadius, angleY, angleZ, "");
		else
			command = new CommandNewHollowSpherePart(*context, internalRadius, externalRadius, angleY, angleZ, "");

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
}	// QtSpherePartOperationPanel::preview


// ===========================================================================
//                  LA CLASSE QtSpherePartOperationAction
// ===========================================================================

QtSpherePartOperationAction::QtSpherePartOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtSpherePartOperationPanel*	operationPanel	=
		new QtSpherePartOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy,
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtSpherePartOperationAction::QtSpherePartOperationAction


QtSpherePartOperationAction::QtSpherePartOperationAction (
										const QtSpherePartOperationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtSpherePartOperationAction copy constructor is not allowed.")
}	// QtSpherePartOperationAction::QtSpherePartOperationAction


QtSpherePartOperationAction& QtSpherePartOperationAction::operator = (
										const QtSpherePartOperationAction&)
{
	MGX_FORBIDDEN ("QtSpherePartOperationAction assignment operator is not allowed.")
	return *this;
}	// QtSpherePartOperationAction::operator =


QtSpherePartOperationAction::~QtSpherePartOperationAction ( )
{
}	// QtSpherePartOperationAction::~QtSpherePartOperationAction


QtSpherePartOperationPanel* QtSpherePartOperationAction::getSpherePartPanel ( )
{
	return dynamic_cast<QtSpherePartOperationPanel*>(getOperationPanel ( ));
}	// QtSpherePartOperationAction::getSpherePanel


void QtSpherePartOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
//	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de création de l'aiguille :
	QtSpherePartOperationPanel*	panel	= getSpherePartPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const bool			hollowed	= panel->isHollowed ( );
	const double		externalRadius (panel->getExternalRadius ( ));
	const double		internalRadius (panel->getInternalRadius ( ));
	const double		angleY (panel->getAngleY ( ));
	const double		angleZ (panel->getAngleZ ( ));
	const string		name	= panel->getGroupName ( );
	if (false == panel->createTopology ( ))
	{
		if (false == hollowed)
			cmdResult	= getContext ( ).getGeomManager ( ).newSpherePart (externalRadius, angleY, angleZ, name);
		else
			cmdResult	= getContext ( ).getGeomManager ( ).newHollowSpherePart (internalRadius, externalRadius, angleY, angleZ, name);

	}
	else
	{
		switch (panel->getTopologyType ( ))
		{
			case QtTopologyPanel::STRUCTURED_TOPOLOGY	:
			break;
			default	:
			{
				INTERNAL_ERROR (exc, "QtSpherePartOperationAction::executeOperation", "Topologie non renseignée")
				throw exc;
			}
		}	// switch (panel->getTopologyType ( ))
		const int		ni		= panel->getNi ( );
		const int		nj		= panel->getNj ( );
		const int		nk		= panel->getNk ( );
		if (false == hollowed)
			cmdResult	= getContext ( ).getTopoManager ( ).newSpherePartWithTopo (externalRadius, angleY, angleZ, ni, nj, nk,name);
		else
			cmdResult	= getContext ( ).getTopoManager ( ).newHollowSpherePartWithTopo (internalRadius, externalRadius, angleY, angleZ, ni, nj, nk, name);
	}	// else if (false == panel->createTopology ( ))

	setCommandResult (cmdResult);
}	// QtSpherePartOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
