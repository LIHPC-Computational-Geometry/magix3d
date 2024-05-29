/**
 * \file        QtConeOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        09/09/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "QtComponents/QtConeOperationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Geom/CommandNewCone.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QGridLayout>

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
//                        LA CLASSE QtConeOperationPanel
// ===========================================================================

QtConeOperationPanel::QtConeOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).coneOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).coneOperationTag),
	  _namePanel (0), _operationMethodComboBox (0),
	  _ray1Panel (0), _ray2Panel (0), _vectorPanel (0), _anglePanel (0)
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

	// Méthode de création/modification du cône :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Méthode"), this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem (QString::fromUtf8("Par définition des rayons et axe"));
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);

	// Définition du cône :
	QtGroupBox*	groupCone	= new QtGroupBox (this, "Paramètres du cône");
	groupCone->setContentsMargins (0, 10, 0, 0);
	layout->addWidget (groupCone);
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupCone);
	vlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupCone->setLayout (vlayout);
	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	vlayout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Rayon du cône à l'origine :"), groupCone);
	hlayout->addWidget (label, 1);
	_ray1Panel	= &QtNumericFieldsFactory::createDistanceTextField (groupCone);
	_ray1Panel->setValue (0.);
	hlayout->addWidget (_ray1Panel, 1);
	hlayout->addStretch (10);
	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	vlayout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Rayon du cône à l'extrémité :"), groupCone);
	hlayout->addWidget (label, 1);
	_ray2Panel	= &QtNumericFieldsFactory::createDistanceTextField (groupCone);
	_ray2Panel->setValue (1.);
	hlayout->addWidget (_ray2Panel, 1);
	hlayout->addStretch (10);
	connect (_ray1Panel, SIGNAL (textEdited (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
	connect (_ray2Panel, SIGNAL (textEdited (QString)), this,
	         SLOT (parametersModifiedCallback ( )));

	_vectorPanel	= new QtMgx3DVectorPanel (
					groupCone, "Vecteur directeur", true, "dx :", "dy :", "dz :",
					10., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
					0., -DBL_MAX, DBL_MAX,
					&mainWindow, FilterEntity::AllEdges, true);
	vlayout->addWidget (_vectorPanel);
	connect (_vectorPanel, SIGNAL (vectorModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));

	CHECK_NULL_PTR_ERROR (_vectorPanel->getSegmentIDTextField ( ))
	_vectorPanel->getSegmentIDTextField ( )->setLinkedSeizureManagers (0, 0);


	QtGroupBox*	angleGroupBox	= new QtGroupBox (QString::fromUtf8("Portion du cône"), this);
	angleGroupBox->setContentsMargins (0, 10, 0, 0);
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
			_anglePanel, "Magix 3D", 4 + MgxNumeric::mgxAngleDecimalNum,
			MgxNumeric::mgxAngleDecimalNum, 360., "degrés");
	hlayout->addWidget (_anglePanel);
	hlayout->addStretch (1);
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
	groupCone	= new QtGroupBox ("Topologie :", this);
	layout->addWidget (groupCone);
	vlayout	= new QVBoxLayout (groupCone);
	vlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupCone->setLayout (vlayout);
	const int	defaultEdgesNum	=
		mainWindow.getContext( ).getTopoManager( ).getDefaultNbMeshingEdges( );
	_topologyPanel	= new QtTopologyPanel (
			groupCone, mainWindow, true, false, 3,
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

}	// QtConeOperationPanel::QtConeOperationPanel


QtConeOperationPanel::QtConeOperationPanel (const QtConeOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _operationMethodComboBox (0),
	  _ray1Panel (0), _ray2Panel (0), _vectorPanel (0), _anglePanel (0),
	  _topologyPanel (0)
{
	MGX_FORBIDDEN ("QtConeOperationPanel copy constructor is not allowed.");
}	// QtConeOperationPanel::QtConeOperationPanel (const QtConeOperationPanel&)


QtConeOperationPanel& QtConeOperationPanel::operator = (
												const QtConeOperationPanel&)
{
	MGX_FORBIDDEN ("QtConeOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtConeOperationPanel::QtConeOperationPanel (const QtConeOperationPanel&)


QtConeOperationPanel::~QtConeOperationPanel ( )
{
}	// QtConeOperationPanel::~QtConeOperationPanel


string QtConeOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtConeOperationPanel::getGroupName


QtConeOperationPanel::OPERATION_METHOD
						QtConeOperationPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtConeOperationPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtConeOperationPanel::getOperationMethod


double QtConeOperationPanel::getRay1 ( ) const
{
	CHECK_NULL_PTR_ERROR (_ray1Panel)
	return _ray1Panel->getValue ( );
}	// QtConeOperationPanel::getRay1


double QtConeOperationPanel::getRay2 ( ) const
{
	CHECK_NULL_PTR_ERROR (_ray2Panel)
	return _ray2Panel->getValue ( );
}	// QtConeOperationPanel::getRay2


Math::Vector QtConeOperationPanel::getAxis ( ) const
{
	CHECK_NULL_PTR_ERROR (_vectorPanel)
	return _vectorPanel->getVector ( );
}	// QtConeOperationPanel::getAxis


double QtConeOperationPanel::getAngle ( ) const
{
	CHECK_NULL_PTR_ERROR (_anglePanel)
	return _anglePanel->getAngle ( );
}	// QtConeOperationPanel::getAngle


bool QtConeOperationPanel::createTopology ( ) const
{
	 CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->createTopology ( );
}	// QtConeOperationPanel::createTopology


QtTopologyPanel::TOPOLOGY_TYPE QtConeOperationPanel::getTopologyType ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getTopologyType ( );
}	// QtConeOperationPanel::getTopologyType


double QtConeOperationPanel::getOGridRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getOGridRatio ( );
}	// QtConeOperationPanel::getOGridRatio


int QtConeOperationPanel::getAxeEdgesNum ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe1EdgesNum ( );
}	// QtConeOperationPanel::getAxeEdgesNum


int QtConeOperationPanel::getiEdgesNum ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe2EdgesNum ( );
}	// QtConeOperationPanel::getiEdgesNum


int QtConeOperationPanel::getrLayersNum ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe3EdgesNum ( );
}	// QtConeOperationPanel::getrLayersNum


void QtConeOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	CHECK_NULL_PTR_ERROR (_ray1Panel)
	CHECK_NULL_PTR_ERROR (_ray2Panel)
	CHECK_NULL_PTR_ERROR (_vectorPanel)
	CHECK_NULL_PTR_ERROR (_anglePanel)
	_namePanel->autoUpdate ( );
	_ray1Panel->setValue (0.);
	_ray2Panel->setValue (1.);
	_vectorPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtConeOperationPanel::reset


void QtConeOperationPanel::validate ( )
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
		error << "QtConeOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtConeOperationPanel::RAYS_LENGTH	:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode d'opération de création/modification "
			      << "de cône sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtConeOperationPanel::validate : index de méthode "
			      << "d'opération de création/modification de cône "
			      << "invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX1 ( ), "Abscisse point 1 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY1 ( ), "Ordonnée point 1 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ1 ( ), "Elévation point 1 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX2 ( ), "Abscisse point 2 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY2 ( ), "Ordonnée point 2 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ2 ( ), "Elévation point 2 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_topologyPanel->validate ( ), "Topologie : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtConeOperationPanel::validate


void QtConeOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_vectorPanel)
	_vectorPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		if (0 != _vectorPanel->getSegmentIDTextField ( ))
		{
			vector<string>	names;
			_vectorPanel->getSegmentIDTextField ( )->setUniqueNames (names);
		}

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtConeOperationPanel::cancel


void QtConeOperationPanel::autoUpdate ( )
{
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		CHECK_NULL_PTR_ERROR (_namePanel)
		CHECK_NULL_PTR_ERROR (_vectorPanel)

		vector<string>	selectedEdges	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::AllEdges);
		if (1 == selectedEdges.size ( ))
			_vectorPanel->setSegment (selectedEdges [0]);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	//_vectorPanel->setSegment ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	_namePanel->autoUpdate ( );
	_topologyPanel->autoUpdate ( );
	_vectorPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtConeOperationPanel::autoUpdate


void QtConeOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_vectorPanel)
	_vectorPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtConeOperationPanel::operationCompleted

void QtConeOperationPanel::preview (bool show, bool destroyInteractor)
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
		graphicalProps.setWireColor (Color (
				255 * Resources::instance ( )._previewColor.getRed ( ),
				255 * Resources::instance ( )._previewColor.getGreen ( ),
				255 * Resources::instance ( )._previewColor.getBlue ( )));
		graphicalProps.setLineWidth (
						Resources::instance ( )._previewWidth.getValue ( ));

		const double		ray1	= getRay1 ( );
		const double		ray2	= getRay2 ( );
		const double		angle	= getAngle ( );
		const Math::Vector	axis	= getAxis ( );

		CommandNewCone command (*context, ray1, ray2, axis, angle, "");

		Geom::GeomDisplayRepresentation	dr (DisplayRepresentation::WIRE);
		command.getPreviewRepresentation (dr);
		const vector<Math::Point>&	points	= dr.getPoints ( );
		const vector<size_t>&		indices	= dr.getCurveDiscretization ( );

		RenderingManager::RepresentationID	repID	=
				getRenderingManager ( ).createSegmentsWireRepresentation (
										points, indices, graphicalProps, true);
		registerPreviewedObject (repID);

		getRenderingManager ( ).forceRender ( );
	}
	catch (...)
	{
	}
}	// QtConeOperationPanel::preview

// ===========================================================================
//                  LA CLASSE QtConeOperationAction
// ===========================================================================

QtConeOperationAction::QtConeOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtConeOperationPanel*	operationPanel	=
		new QtConeOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy,
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtConeOperationAction::QtConeOperationAction


QtConeOperationAction::QtConeOperationAction (
										const QtConeOperationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtConeOperationAction copy constructor is not allowed.")
}	// QtConeOperationAction::QtConeOperationAction


QtConeOperationAction& QtConeOperationAction::operator = (
										const QtConeOperationAction&)
{
	MGX_FORBIDDEN ("QtConeOperationAction assignment operator is not allowed.")
	return *this;
}	// QtConeOperationAction::operator =


QtConeOperationAction::~QtConeOperationAction ( )
{
}	// QtConeOperationAction::~QtConeOperationAction


QtConeOperationPanel* QtConeOperationAction::getConePanel ( )
{
	return dynamic_cast<QtConeOperationPanel*>(getOperationPanel ( ));
}	// QtConeOperationAction::getConePanel


void QtConeOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
//	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de création du cône :
	QtConeOperationPanel*	panel	= getConePanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const double		ray1	= panel->getRay1 ( );
	const double		ray2	= panel->getRay2 ( );
	const double		angle	= panel->getAngle ( );
	const Math::Vector	axis	= panel->getAxis ( );
	const string		name	= panel->getGroupName ( );

	if (false == panel->createTopology ( ))
	{
		cmdResult	= getContext ( ).getGeomManager ( ).newCone (ray1, ray2, axis, angle, name);
	}
	else
	{
		bool	structured	= false;
		double	ratio		= 1.0;
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
		cmdResult	= getContext ( ).getTopoManager ( ).newConeWithTopo (ray1, ray2, axis, angle, structured, ratio, naxe, ni, nr, name);
	}

	setCommandResult (cmdResult);
}	// QtConeOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
