/**
 * \file		QtSysCoordOperationAction.cpp
 * \author		Eric Brière de l'Isle
 * \date		7/6/2018
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "SysCoord/CommandNewSysCoord.h"
#include "SysCoord/SysCoordManagerIfc.h"
#include "SysCoord/SysCoordDisplayRepresentation.h"
#include "QtComponents/QtSysCoordOperationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"

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
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtSysCoordOperationPanel
// ===========================================================================

QtSysCoordOperationPanel::QtSysCoordOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).syscoordOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).syscoordCreationTag),
	  _namePanel (0), _operationMethodComboBox (0),
	  _centerPanel(0), _point1Panel (0), _point2Panel (0)
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

	// Méthode de création/modification du repère :
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
	_operationMethodComboBox->addItem (QString::fromUtf8("Par défaut"));
	_operationMethodComboBox->addItem (QString::fromUtf8("Suivant un centre"));
	_operationMethodComboBox->addItem (QString::fromUtf8("Suivant un centre et 2 autres points"));
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);

	// Définition du repère :
	QtGroupBox*	groupBox	= new QtGroupBox (this, "Paramètres du repère");
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
		&mainWindow, (FilterEntity::objectType)(FilterEntity::GeomVertex | FilterEntity::TopoVertex), true);
	vlayout->addWidget (_centerPanel);
	connect (_centerPanel, SIGNAL (pointAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_centerPanel, SIGNAL (pointRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_centerPanel, SIGNAL (pointModified()),
			this, SLOT (parametersModifiedCallback ( )));
	_point1Panel	= new QtMgx3DPointPanel (
		groupBox, "Point oX", true, "x :", "y :", "z :",
		1., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
		&mainWindow, (FilterEntity::objectType)(FilterEntity::GeomVertex | FilterEntity::TopoVertex), true);
	vlayout->addWidget (_point1Panel);
	connect (_point1Panel, SIGNAL (pointAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_point1Panel, SIGNAL (pointRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_point1Panel, SIGNAL (pointModified()),
			this, SLOT (parametersModifiedCallback ( )));
	_point2Panel	= new QtMgx3DPointPanel (
		groupBox, "Point oY", true, "x :", "y :", "z :",
		0., -DBL_MAX, DBL_MAX, 1., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
		&mainWindow, (FilterEntity::objectType)(FilterEntity::GeomVertex | FilterEntity::TopoVertex), true);
	vlayout->addWidget (_point2Panel);
	connect (_point2Panel, SIGNAL (pointAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_point2Panel, SIGNAL (pointRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_point2Panel, SIGNAL (pointModified()),
			this, SLOT (parametersModifiedCallback ( )));

	connect (_operationMethodComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (operationMethodCallback ( )));

	operationMethodCallback();
	layout->addStretch (2);

	addPreviewCheckBox (true);

}	// QtSysCoordOperationPanel::QtSysCoordOperationPanel


QtSysCoordOperationPanel::QtSysCoordOperationPanel (const QtSysCoordOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _operationMethodComboBox (0),
	  _centerPanel(0), _point1Panel (0), _point2Panel (0)
{
	MGX_FORBIDDEN ("QtSysCoordOperationPanel copy constructor is not allowed.");
}	// QtSysCoordOperationPanel::QtSysCoordOperationPanel (const QtSysCoordOperationPanel&)


QtSysCoordOperationPanel& QtSysCoordOperationPanel::operator = (const QtSysCoordOperationPanel&)
{
	MGX_FORBIDDEN ("QtSysCoordOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtSysCoordOperationPanel::QtSysCoordOperationPanel (const QtSysCoordOperationPanel&)


QtSysCoordOperationPanel::~QtSysCoordOperationPanel ( )
{
}	// QtSysCoordOperationPanel::~QtSysCoordOperationPanel


vector<Entity*> QtSysCoordOperationPanel::getInvolvedEntities ( )
{
	CHECK_NULL_PTR_ERROR (_centerPanel)
	CHECK_NULL_PTR_ERROR (_point1Panel)
	CHECK_NULL_PTR_ERROR (_point2Panel)
	vector<Entity*>	entities;

	if (0 != _centerPanel->getUniqueName ( ).length ( ))
		entities.push_back (&getContext().nameToEntity (_centerPanel->getUniqueName ( )));
	if (0 != _point1Panel->getUniqueName ( ).length ( ))
		entities.push_back (&getContext().nameToEntity (_point1Panel->getUniqueName ( )));
	if (0 != _point2Panel->getUniqueName ( ).length ( ))
		entities.push_back (&getContext().nameToEntity (_point2Panel->getUniqueName ( )));

	return entities;
}	// QtSysCoordOperationPanel::getInvolvedEntities


string QtSysCoordOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtSysCoordOperationPanel::getGroupName


QtSysCoordOperationPanel::OPERATION_METHOD
						QtSysCoordOperationPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtSysCoordOperationPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtSysCoordOperationPanel::getOperationMethod

Utils::Math::Point QtSysCoordOperationPanel::getCenter() const
{
	CHECK_NULL_PTR_ERROR (_centerPanel)
	return _centerPanel->getPoint();
}

Utils::Math::Point QtSysCoordOperationPanel::getPoint1() const
{
	CHECK_NULL_PTR_ERROR (_point1Panel)
	return _point1Panel->getPoint();
}

Utils::Math::Point QtSysCoordOperationPanel::getPoint2() const
{
	CHECK_NULL_PTR_ERROR (_point2Panel)
	return _point2Panel->getPoint();
}

void QtSysCoordOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (getMainWindow ( ))
	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_centerPanel)
	CHECK_NULL_PTR_ERROR (_point1Panel)
	CHECK_NULL_PTR_ERROR (_point2Panel)
	_namePanel->autoUpdate ( );
	_centerPanel->reset ( );
	_point1Panel->reset ( );
	_point2Panel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtSysCoordOperationPanel::reset


void QtSysCoordOperationPanel::validate ( )
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
		error << "QtSysCoordOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtSysCoordOperationPanel::DEFAUT	:
		case QtSysCoordOperationPanel::CENTRE	:
		case QtSysCoordOperationPanel::TROIS_POINTS	:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode d'opération de création/modification "
			      << "de repère sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtSysCoordOperationPanel::validate : index de méthode "
			      << "d'opération de création/modification de repère "
			      << "invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))


	if (0 != error.length ( ))
		throw Exception (error);
}	// QtSysCoordOperationPanel::validate


void QtSysCoordOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_centerPanel)
	CHECK_NULL_PTR_ERROR (_point1Panel)
	CHECK_NULL_PTR_ERROR (_point2Panel)
	_centerPanel->stopSelection ( );
	_point1Panel->stopSelection ( );
	_point2Panel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_centerPanel->setUniqueName ("");
		_point1Panel->setUniqueName ("");
		_point2Panel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtSysCoordOperationPanel::cancel


void QtSysCoordOperationPanel::autoUpdate ( )
{
	_centerPanel->clearSelection ( );
	_point1Panel->clearSelection ( );
	_point2Panel->clearSelection ( );

	_namePanel->autoUpdate ( );
	_centerPanel->actualizeGui (true);
	_point1Panel->actualizeGui (true);
	_point2Panel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtSysCoordOperationPanel::autoUpdate


void QtSysCoordOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_centerPanel)
	CHECK_NULL_PTR_ERROR (_point1Panel)
	CHECK_NULL_PTR_ERROR (_point2Panel)
	_centerPanel->stopSelection ( );
	_point1Panel->stopSelection ( );
	_point2Panel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtSysCoordOperationPanel::operationCompleted


void QtSysCoordOperationPanel::operationMethodCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// on suprime les anciens previews
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}

	CHECK_NULL_PTR_ERROR (_centerPanel)
	CHECK_NULL_PTR_ERROR (_point1Panel)
	CHECK_NULL_PTR_ERROR (_point2Panel)

	switch (getOperationMethod ( )){
	case QtSysCoordOperationPanel::DEFAUT	:
		_centerPanel->hide();
		_point1Panel->hide();
		_point2Panel->hide();
		break;
	case QtSysCoordOperationPanel::CENTRE	:
		_centerPanel->show();
		_point1Panel->hide();
		_point2Panel->hide();
		break;
	case QtSysCoordOperationPanel::TROIS_POINTS	:
		_centerPanel->show();
		_point1Panel->show();
		_point2Panel->show();
		break;
	default		:
		INTERNAL_ERROR (exc, "QtSysCoordOperationPanel::operationMethodCallback",
				"opération en dehors des clous")
		throw exc;
	}	// switch


	autoUpdate ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

} // QtSysCoordOperationPanel::operationMethodCallback ( )


void QtSysCoordOperationPanel::preview (bool show, bool destroyInteractor)
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

		CoordinateSystem::SysCoordDisplayRepresentation	dr (DisplayRepresentation::SHOWTRIHEDRON);

		switch (getOperationMethod ( )){
		case DEFAUT	:
		{
			CoordinateSystem::CommandNewSysCoord command(*context, "");
			command.getPreviewRepresentation (dr);
		}
		break;
		case CENTRE	:
		{
			CoordinateSystem::CommandNewSysCoord command(*context, getCenter(), "");
			command.getPreviewRepresentation (dr);
		}
		break;
		case TROIS_POINTS	:
		{
			CoordinateSystem::CommandNewSysCoord command(*context, getCenter(), getPoint1(), getPoint2(), "");
			command.getPreviewRepresentation (dr);
		}
		break;
		default		:
			INTERNAL_ERROR (exc, "QtSysCoordOperationPanel::preview",
					"opération en dehors des clous")
					throw exc;
		}	// switch

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
}	// QtSysCoordOperationPanel::preview



// ===========================================================================
//                  LA CLASSE QtSysCoordOperationAction
// ===========================================================================

QtSysCoordOperationAction::QtSysCoordOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DOperationAction (icon, text, mainWindow, tooltip)
{
	QtSysCoordOperationPanel*	operationPanel	=
		new QtSysCoordOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy,
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtSysCoordOperationAction::QtSysCoordOperationAction


QtSysCoordOperationAction::QtSysCoordOperationAction (
										const QtSysCoordOperationAction&)
	: QtMgx3DOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtSysCoordOperationAction copy constructor is not allowed.")
}	// QtSysCoordOperationAction::QtSysCoordOperationAction


QtSysCoordOperationAction& QtSysCoordOperationAction::operator = (
										const QtSysCoordOperationAction&)
{
	MGX_FORBIDDEN ("QtSysCoordOperationAction assignment operator is not allowed.")
	return *this;
}	// QtSysCoordOperationAction::operator =


QtSysCoordOperationAction::~QtSysCoordOperationAction ( )
{
}	// QtSysCoordOperationAction::~QtSysCoordOperationAction


QtSysCoordOperationPanel* QtSysCoordOperationAction::getSysCoordPanel ( )
{
	return dynamic_cast<QtSysCoordOperationPanel*>(getOperationPanel ( ));
}	// QtSysCoordOperationAction::getSysCoordPanel


void QtSysCoordOperationAction::executeOperation ( )
{
	M3DCommandResultIfc*	cmdResult	= 0;
	
	// Récupération des paramètres de création du repère :
	QtSysCoordOperationPanel*	panel	= getSysCoordPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	Point	centre = panel->getCenter();
	Point	p1 = panel->getPoint1();
	Point	p2 = panel->getPoint2();
	string	name	= panel->getGroupName ( );

	switch (panel->getOperationMethod ( )){
	case QtSysCoordOperationPanel::DEFAUT	:
		cmdResult	= getContext ( ).getSysCoordManager().newSysCoord(name);
		break;
	case QtSysCoordOperationPanel::CENTRE	:
		cmdResult	= getContext ( ).getSysCoordManager().newSysCoord(centre, name);
		break;
	case QtSysCoordOperationPanel::TROIS_POINTS	:
		cmdResult	= getContext ( ).getSysCoordManager().newSysCoord(centre, p1, p2, name);
		break;
	default		:
		INTERNAL_ERROR (exc, "QtSysCoordOperationAction::executeOperation",
				"opération en dehors des clous")
		throw exc;
	}	// switch

	setCommandResult (cmdResult);
}	// QtSysCoordOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
