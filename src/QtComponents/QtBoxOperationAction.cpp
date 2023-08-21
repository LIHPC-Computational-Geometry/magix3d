/**
 * \file        QtBoxOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        06/09/2013
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "Geom/Vertex.h"
#include "Utils/Vector.h"
#include "QtComponents/QtBoxOperationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"
#include "Geom/CommandNewBox.h"
#include "Geom/GeomDisplayRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
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
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtBoxOperationPanel
// ===========================================================================

QtBoxOperationPanel::QtBoxOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).boxOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).boxOperationTag),
	  _namePanel (0), _operationMethodComboBox (0),
	  _point1Panel (0), _point2Panel (0), _topologyPanel (0)
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

	// Méthode de création/modification de la boite :
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
	_operationMethodComboBox->addItem (QString::fromUtf8("Par points diamétralement opposés"));
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);

	// Définition de la boite :
	QtGroupBox*	groupBox	= new QtGroupBox (this, "Paramètres de la boite");
	layout->addWidget (groupBox);
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	vlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (vlayout);
	_point1Panel	= new QtMgx3DPointPanel (
		groupBox, "Point 1", true, "x :", "y :", "z :",
		0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
		&mainWindow, FilterEntity::GeomVertex, true);
	vlayout->addWidget (_point1Panel);
	connect (_point1Panel, SIGNAL (pointAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_point1Panel, SIGNAL (pointRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_point1Panel, SIGNAL (pointModified()),
			this, SLOT (parametersModifiedCallback ( )));
	_point2Panel	= new QtMgx3DPointPanel (
		groupBox, "Point 2", true, "x :", "y :", "z :",
		1., -DBL_MAX, DBL_MAX, 1., -DBL_MAX, DBL_MAX, 1., -DBL_MAX, DBL_MAX,
		&mainWindow, FilterEntity::GeomVertex, true);
	vlayout->addWidget (_point2Panel);
	connect (_point2Panel, SIGNAL (pointAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_point2Panel, SIGNAL (pointRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_point2Panel, SIGNAL (pointModified()),
			this, SLOT (parametersModifiedCallback ( )));

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
							QtTopologyPanel::OGRID_BLOCKS, false);
	_topologyPanel->enableTopologyType (
							QtTopologyPanel::STRUCTURED_FREE_TOPOLOGY, false);
	_topologyPanel->enableTopologyType (
							QtTopologyPanel::INSERTION_TOPOLOGY, false);
	vlayout->addWidget (_topologyPanel);

	CHECK_NULL_PTR_ERROR (_point1Panel->getVertexTextField ( ))
	CHECK_NULL_PTR_ERROR (_point2Panel->getVertexTextField ( ))
	_point1Panel->getVertexTextField (
		)->setLinkedSeizureManagers (0, _point2Panel->getVertexTextField ( ));
	_point2Panel->getVertexTextField (
		)->setLinkedSeizureManagers (_point1Panel->getVertexTextField ( ), 0);

	addPreviewCheckBox (true);

}	// QtBoxOperationPanel::QtBoxOperationPanel


QtBoxOperationPanel::QtBoxOperationPanel (const QtBoxOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _operationMethodComboBox (0),
	  _point1Panel (0), _point2Panel (0), _topologyPanel (0)
{
	MGX_FORBIDDEN ("QtBoxOperationPanel copy constructor is not allowed.");
}	// QtBoxOperationPanel::QtBoxOperationPanel (const QtBoxOperationPanel&)


QtBoxOperationPanel& QtBoxOperationPanel::operator = (const QtBoxOperationPanel&)
{
	MGX_FORBIDDEN ("QtBoxOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtBoxOperationPanel::QtBoxOperationPanel (const QtBoxOperationPanel&)


QtBoxOperationPanel::~QtBoxOperationPanel ( )
{
}	// QtBoxOperationPanel::~QtBoxOperationPanel


vector<Entity*> QtBoxOperationPanel::getInvolvedEntities ( )
{
	CHECK_NULL_PTR_ERROR (_point1Panel)
	CHECK_NULL_PTR_ERROR (_point2Panel)
	vector<Entity*>	entities;

	if (0 != _point1Panel->getUniqueName ( ).length ( ))
	{
		Geom::Vertex*	vertex	= 
			getContext ( ).getGeomManager ( ).getVertex (
									_point1Panel->getUniqueName ( ), false);
		if (0 != vertex)
			entities.push_back (vertex);
	}	// if (0 != _point1Panel->getUniqueName ( ).length ( ))
	if (0 != _point2Panel->getUniqueName ( ).length ( ))
	{
		Geom::Vertex*	vertex	= 
			getContext ( ).getGeomManager ( ).getVertex (
									_point2Panel->getUniqueName ( ), false);
		if (0 != vertex)
			entities.push_back (vertex);
	}	// if (0 != _point2Panel->getUniqueName ( ).length ( ))

	return entities;
}	// QtBoxOperationPanel::getInvolvedEntities


string QtBoxOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtBoxOperationPanel::getGroupName


QtBoxOperationPanel::OPERATION_METHOD
						QtBoxOperationPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtBoxOperationPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtBoxOperationPanel::getOperationMethod

Utils::Math::Point QtBoxOperationPanel::getPoint1() const
{
	CHECK_NULL_PTR_ERROR (_point1Panel)
	return _point1Panel->getPoint();
}

Utils::Math::Point QtBoxOperationPanel::getPoint2() const
{
	CHECK_NULL_PTR_ERROR (_point2Panel)
	return _point2Panel->getPoint();
}

bool QtBoxOperationPanel::createTopology ( ) const
{
	 CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->createTopology ( );
}	// QtBoxOperationPanel::createTopology


QtTopologyPanel::TOPOLOGY_TYPE QtBoxOperationPanel::getTopologyType ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getTopologyType ( );
}	// QtBoxOperationPanel::getTopologyType


int QtBoxOperationPanel::getNi ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe1EdgesNum ( );
}	// QtBoxOperationPanel::getNi


int QtBoxOperationPanel::getNj ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe2EdgesNum ( );
}	// QtBoxOperationPanel::getNj


int QtBoxOperationPanel::getNk ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getAxe3EdgesNum ( );
}	// QtBoxOperationPanel::getNk


void QtBoxOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (getMainWindow ( ))
	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_point1Panel)
	CHECK_NULL_PTR_ERROR (_point2Panel)
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	_namePanel->autoUpdate ( );
	_point1Panel->reset ( );
	_point2Panel->reset ( );
	const int	defaultEdgesNum	=
		getMainWindow ( )->getContext( ).getTopoManager( ).getDefaultNbMeshingEdges( );
	_topologyPanel->setTopologyType (QtTopologyPanel::STRUCTURED_TOPOLOGY);
	_topologyPanel->setAxe1EdgesNum (defaultEdgesNum);
	_topologyPanel->setAxe2EdgesNum (defaultEdgesNum);
	_topologyPanel->setAxe3EdgesNum (defaultEdgesNum);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtBoxOperationPanel::reset


void QtBoxOperationPanel::validate ( )
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
		error << "QtBoxOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtBoxOperationPanel::POINT_POINT	:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode d'opération de création/modification "
			      << "de boite sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtBoxOperationPanel::validate : index de méthode "
			      << "d'opération de création/modification de boite "
			      << "invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX1 ( ), "Abscisse point 1 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY1 ( ), "Ordonnée point 1 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ1 ( ), "Elévation point 1 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX2 ( ), "Abscisse point 2 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY2 ( ), "Ordonnée point 2 : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ2 ( ), "Elévation point 2 : ", error)
	VALIDATED_FIELD_EXPRESSION_EVALUATION (_topologyPanel->validate ( ), "Topologie : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtBoxOperationPanel::validate


void QtBoxOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_point1Panel)
	CHECK_NULL_PTR_ERROR (_point2Panel)
	CHECK_NULL_PTR_ERROR (_point1Panel->getVertexTextField ( ))
	CHECK_NULL_PTR_ERROR (_point2Panel->getVertexTextField ( ))
	_point1Panel->stopSelection ( );
	_point2Panel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		// CP : pas de ->setUniqueName ("") car en Qt 5.14.* le panneau se retrouve initialisé à la construction avec des (0, 0, 0)
		_point1Panel->getVertexTextField ( )->setText (QString (""));
		_point2Panel->getVertexTextField ( )->setText (QString (""));

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtBoxOperationPanel::cancel


void QtBoxOperationPanel::autoUpdate ( )
{
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		CHECK_NULL_PTR_ERROR (_namePanel)
		CHECK_NULL_PTR_ERROR (_topologyPanel)
		CHECK_NULL_PTR_ERROR (_point1Panel)
		CHECK_NULL_PTR_ERROR (_point2Panel)

		vector<string>	selectedVertices	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomVertex);
		if (2 >= selectedVertices.size ( ))
		{
			if (1 <= selectedVertices.size ( ))
				_point1Panel->setUniqueName (selectedVertices [0]);
			if (2 == selectedVertices.size ( ))
				_point2Panel->setUniqueName (selectedVertices [1]);
		}	// if (2 >= selectedVertices.size ( ))

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
		_point1Panel->clearSelection ( );
		_point2Panel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_namePanel->autoUpdate ( );
	_topologyPanel->autoUpdate ( );
	_point1Panel->actualizeGui (true);
	_point2Panel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtBoxOperationPanel::autoUpdate


void QtBoxOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_point1Panel)
	CHECK_NULL_PTR_ERROR (_point2Panel)
	_point1Panel->stopSelection ( );
	_point2Panel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtBoxOperationPanel::operationCompleted


void QtBoxOperationPanel::preview (bool show, bool destroyInteractor)
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

		Geom::CommandNewBox command(*context, getPoint1(), getPoint2(), "");

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
}	// QtBoxOperationPanel::preview


// ===========================================================================
//                  LA CLASSE QtBoxOperationAction
// ===========================================================================

QtBoxOperationAction::QtBoxOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtBoxOperationPanel*	operationPanel	=
		new QtBoxOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy,
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtBoxOperationAction::QtBoxOperationAction


QtBoxOperationAction::QtBoxOperationAction (
										const QtBoxOperationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtBoxOperationAction copy constructor is not allowed.")
}	// QtBoxOperationAction::QtBoxOperationAction


QtBoxOperationAction& QtBoxOperationAction::operator = (
										const QtBoxOperationAction&)
{
	MGX_FORBIDDEN ("QtBoxOperationAction assignment operator is not allowed.")
	return *this;
}	// QtBoxOperationAction::operator =


QtBoxOperationAction::~QtBoxOperationAction ( )
{
}	// QtBoxOperationAction::~QtBoxOperationAction


QtBoxOperationPanel* QtBoxOperationAction::getBoxPanel ( )
{
	return dynamic_cast<QtBoxOperationPanel*>(getOperationPanel ( ));
}	// QtBoxOperationAction::getBoxPanel


void QtBoxOperationAction::executeOperation ( )
{
	// Validation paramétrage :
//	QtMgx3DGeomOperationAction::executeOperation ( );
	M3DCommandResultIfc*	cmdResult	= 0;

	// Récupération des paramètres de création de la boite :
	QtBoxOperationPanel*	panel	= getBoxPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	Point	p1 = panel->getPoint1();
	Point	p2 = panel->getPoint2();
	string	name	= panel->getGroupName ( );
	if (false == panel->createTopology ( ))
		cmdResult	= getContext ( ).getGeomManager ( ).newBox (p1, p2, name);
	else
	{
		bool	structured	= false;
// TODO [CP] A revoir
		switch (panel->getTopologyType ( ))
		{
			case QtTopologyPanel::UNSTRUCTURED_TOPOLOGY	:
			    structured  = false;
			    cmdResult	= getContext ( ).getTopoManager ( ).newBoxWithTopo (p1, p2, false, name);
			break;
			case QtTopologyPanel::STRUCTURED_TOPOLOGY	:
			{
                structured  = true;
				const int	ni	= panel->getNi ( );
				const int	nj	= panel->getNj ( );
				const int	nk	= panel->getNk ( );
				cmdResult	= getContext ( ).getTopoManager ( ).newBoxWithTopo (p1, p2, ni, nj, nk, name);
			}
            break;
			case QtTopologyPanel::OGRID_BLOCKS	:
			{
				INTERNAL_ERROR (exc, "QtBoxOperationAction::executeOperation", "Création de boite englobante avec O-Grid impossible")
				throw exc;
			}
			default	:
			{
				INTERNAL_ERROR (exc, "QtBoxOperationAction::executeOperation", "Topologie non renseignée")
				throw exc;
			}
		}	// switch (panel->getTopologyType ( ))
	}	// else if (false == panel->createTopology ( ))
	
	setCommandResult (cmdResult);
}	// QtBoxOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
