/**
 * \file        QtCircleOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        09/09/2014
 */

#include "Internal/Context.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Geom/CommandNewCircle.h"
#include "Geom/CommandNewEllipse.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Geom/GeomManager.h"
#include "Geom/Vertex.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtCircleOperationAction.h"
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
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


namespace Mgx3D
{

namespace QtComponents
{

// ===========================================================================
//                        LA CLASSE QtCircleOperationPanel
// ===========================================================================

QtCircleOperationPanel::QtCircleOperationPanel (
			QWidget* parent, const string& panelName, QtMgx3DGroupNamePanel::POLICY creationPolicy, QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action, QtMgx3DApplication::HelpSystem::instance ( ).circleOperationURL, QtMgx3DApplication::HelpSystem::instance ( ).circleOperationTag),
	  _namePanel (0), _operationMethodComboBox (0),  _currentParentWidget (0), _currentPanel (0), _verticesPanel (0)
{
//	SET_WIDGET_BACKGROUND (this, Qt::yellow)
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (Resources::instance ( )._margin.getValue ( ), Resources::instance ( )._margin.getValue ( ), Resources::instance ( )._margin.getValue ( ), Resources::instance ( )._margin.getValue ( ));
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

	// Méthode de création/modification du cercle :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Méthode"), this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem ("Cercle (saisie de 3 points)");
	_operationMethodComboBox->addItem ("Ellipse (saisie de 3 points)");
	connect (_operationMethodComboBox, SIGNAL (currentIndexChanged (int)), this, SLOT (operationMethodCallback ( )));
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);
	
	// Définition du cercle :
	QtGroupBox*		groupBox	= new QtGroupBox (QString::fromUtf8 ("Paramètres du cercle"), this);
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	vlayout->setContentsMargins  (Resources::instance ( )._margin.getValue ( ), Resources::instance ( )._margin.getValue ( ), Resources::instance ( )._margin.getValue ( ), Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (vlayout);
	layout->addWidget (groupBox);
	// Le panneau "méthode" : plusieurs panneaux possibles.
	// On garde un parent unique dans la layout :
	_currentParentWidget	= new QWidget (groupBox);
	vlayout->addWidget (_currentParentWidget);
	QHBoxLayout*	currentLayout = new QHBoxLayout (_currentParentWidget);
	_currentParentWidget->setLayout (currentLayout);
	_verticesPanel	= new Qt3VerticiesPanel (0, "Définition d'un cercle par 3 points", mainWindow, FilterEntity::GeomVertex, false);
	connect (_verticesPanel, SIGNAL (pointAddedToSelection (QString)), this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_verticesPanel, SIGNAL (pointRemovedFromSelection (QString)), this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	_verticesPanel->hide ( );
	operationMethodCallback ( );

	vlayout->addStretch (2);
	
	addPreviewCheckBox (true);
}	// QtCircleOperationPanel::QtCircleOperationPanel


QtCircleOperationPanel::QtCircleOperationPanel (const QtCircleOperationPanel& cao)
	: QtMgx3DOperationPanel (0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _operationMethodComboBox (0), _currentParentWidget (0), _currentPanel (0), _verticesPanel (0)
{
	MGX_FORBIDDEN ("QtCircleOperationPanel copy constructor is not allowed.");
}	// QtCircleOperationPanel::QtCircleOperationPanel (const QtCircleOperationPanel&)


QtCircleOperationPanel& QtCircleOperationPanel::operator = (const QtCircleOperationPanel&)
{
	MGX_FORBIDDEN ("QtCircleOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtCircleOperationPanel::QtCircleOperationPanel (const QtCircleOperationPanel&)


QtCircleOperationPanel::~QtCircleOperationPanel ( )
{
}	// QtCircleOperationPanel::~QtCircleOperationPanel


string QtCircleOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtCircleOperationPanel::getGroupName


QtCircleOperationPanel::OPERATION_METHOD QtCircleOperationPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtCircleOperationPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtCircleOperationPanel::getOperationMethod


string QtCircleOperationPanel::getVertex1UniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	return _verticesPanel->getVertex1UniqueName ( );
}	// QtCircleOperationPanel::getStartVertexUniqueName


string QtCircleOperationPanel::getVertex2UniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	return _verticesPanel->getVertex2UniqueName ( );
}	// QtCircleOperationPanel::getVertex2UniqueName


string QtCircleOperationPanel::getVertex3UniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	return _verticesPanel->getVertex3UniqueName ( );
}	// QtCircleOperationPanel::getVertex3UniqueName


void QtCircleOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	_namePanel->autoUpdate ( );
	_verticesPanel->reset ( );
	if (0 != _currentPanel)
		_currentPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtCircleOperationPanel::reset


void QtCircleOperationPanel::validate ( )
{
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
		error << "QtCircleOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtCircleOperationPanel::THREE_POINTS			:
		case QtCircleOperationPanel::ELLIPSE_THREE_POINTS	:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode d'opération de création/modification d'un cercle sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtCircleOperationPanel::validate : index de méthode d'opération de création/modification d'un cercle invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX ( ), "Abscisse point : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY ( ), "Ordonnée point : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ ( ), "Elévation point : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtCircleOperationPanel::validate


void QtCircleOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	if (0 != _currentPanel)
		_currentPanel->cancel ( );
}	// QtCircleOperationPanel::cancel


void QtCircleOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	_namePanel->autoUpdate ( );

	if (0 != _currentPanel)
		_currentPanel->autoUpdate ( );

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtCircleOperationPanel::autoUpdate


void QtCircleOperationPanel::preview (bool show, bool destroyInteractor)
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
		graphicalProps.setLineWidth (Resources::instance ( )._previewWidth.getValue ( ));

		CHECK_NULL_PTR_ERROR (_verticesPanel)
		Geom::Vertex	*v1 = 0, *v2 = 0, *v3 = 0;
		switch (getOperationMethod ( ))
		{
			case QtCircleOperationPanel::THREE_POINTS			:
			case QtCircleOperationPanel::ELLIPSE_THREE_POINTS	:
				v1	= getContext ( ).getGeomManager ( ).getVertex (_verticesPanel->getVertex1UniqueName ( ), false);
				v2	= getContext ( ).getGeomManager ( ).getVertex (_verticesPanel->getVertex2UniqueName ( ), false);
				v3	= getContext ( ).getGeomManager ( ).getVertex (_verticesPanel->getVertex3UniqueName ( ), false);
				break;
			default												: throw Exception ("Erreur interne dans QtCircleOperationPanel::preview : cas non recensé.");
		}	// switch (getOperationMethod ( ))
		if ((0 == v1) || (0 == v2) || (0 == v3))
			return;

		unique_ptr<CommandCreateGeom>	command;
		switch (getOperationMethod ( ))
		{
			case QtCircleOperationPanel::THREE_POINTS			:
				command.reset (new Geom::CommandNewCircle (*context, v1, v2, v3, ""));
				break;
			case QtCircleOperationPanel::ELLIPSE_THREE_POINTS	:
				command.reset (new Geom::CommandNewEllipse (*context, v1, v2, v3, ""));
				break;
			default												: throw Exception ("Erreur interne dans QtCircleOperationPanel::preview : cas non recensé.");
		}	// switch (getOperationMethod ( ))

		CHECK_NULL_PTR_ERROR (command.get ( ))
		Geom::GeomDisplayRepresentation	dr (DisplayRepresentation::WIRE);
		command->getPreviewRepresentation (dr);
		const vector<Math::Point>&	points	= dr.getPoints ( );
		const vector<size_t>&		indices	= dr.getCurveDiscretization ( );

		RenderingManager::RepresentationID	repID	= getRenderingManager ( ).createSegmentsWireRepresentation (points, indices, graphicalProps, true);
		registerPreviewedObject (repID);

		getRenderingManager ( ).forceRender ( );
	}
	catch (...)
	{
	}
}	// QtCircleOperationPanel::preview


vector<Entity*> QtCircleOperationPanel::getInvolvedEntities ( )
{
	vector<Entity*>		entities;

	if (0 != _currentPanel)
		entities	= _currentPanel->getPanelEntities ( );

	return entities;
}	// QtCircleOperationPanel::getInvolvedEntities


void QtCircleOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _verticesPanel)
		_verticesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtCircleOperationPanel::operationCompleted


void QtCircleOperationPanel::operationMethodCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_verticesPanel)
	if (0 != _currentPanel)
	{
		_currentParentWidget->layout ( )->removeWidget (_currentPanel);
		_currentPanel->hide ( );
		_currentPanel->setParent (0);
	}
	_currentPanel	= 0;

	QGroupBox*	parent	= dynamic_cast<QGroupBox*>(_currentParentWidget->parent ( ));
	switch (getOperationMethod ( ))
	{
		case QtCircleOperationPanel::THREE_POINTS				:
			_currentPanel	= _verticesPanel;
			_verticesPanel->setLabels (UTF8String ("Point 1"), UTF8String ("Point 2"), UTF8String ("Point 3"));
			if (0 != parent)
				parent->setTitle (QString::fromUtf8 ("Paramètres du cercle"));
			break;
		case QtCircleOperationPanel::ELLIPSE_THREE_POINTS		:
			_currentPanel	= _verticesPanel;
			_verticesPanel->setLabels (UTF8String ("Point 1"), UTF8String ("Point 2"), UTF8String ("Centre"));
			if (0 != parent)
				parent->setTitle (QString::fromUtf8 ("Paramètres de l'ellipse"));
			break;
		default	:
		{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Méthode d'opération non supportée (" << (unsigned long)getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtCircleOperationPanel::operationMethodCallback")
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
}	// QtCircleOperationPanel::operationMethodCallback


// ===========================================================================
//                  LA CLASSE QtCircleOperationAction
// ===========================================================================

QtCircleOperationAction::QtCircleOperationAction (const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow, const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtCircleOperationPanel*	operationPanel	= new QtCircleOperationPanel (&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy, mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtCircleOperationAction::QtCircleOperationAction


QtCircleOperationAction::QtCircleOperationAction (const QtCircleOperationAction&)
	: QtMgx3DGeomOperationAction (QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtCircleOperationAction copy constructor is not allowed.")
}	// QtCircleOperationAction::QtCircleOperationAction


QtCircleOperationAction& QtCircleOperationAction::operator = (const QtCircleOperationAction&)
{
	MGX_FORBIDDEN ("QtCircleOperationAction assignment operator is not allowed.")
	return *this;
}	// QtCircleOperationAction::operator =


QtCircleOperationAction::~QtCircleOperationAction ( )
{
}	// QtCircleOperationAction::~QtCircleOperationAction


QtCircleOperationPanel* QtCircleOperationAction::getCirclePanel ( )
{
	return dynamic_cast<QtCircleOperationPanel*>(getOperationPanel ( ));
}	// QtCircleOperationAction::getCirclePanel


void QtCircleOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResult*	cmdResult	= 0;
//	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de création du cercle :
	QtCircleOperationPanel*	panel	= getCirclePanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string	name	= panel->getGroupName ( );
	switch (getCirclePanel ( )->getOperationMethod ( ))
	{
	
		case QtCircleOperationPanel::THREE_POINTS				:
		{
			const string	vertex1	= getCirclePanel ( )->getVertex1UniqueName ( );
			const string	vertex2	= getCirclePanel ( )->getVertex2UniqueName ( );
			const string	vertex3	= getCirclePanel ( )->getVertex3UniqueName ( );
			cmdResult	= getContext ( ).getGeomManager ( ).newCircle (vertex1, vertex2, vertex3, name);
		}
		break;
		case QtCircleOperationPanel::ELLIPSE_THREE_POINTS		:
		{
			const string	vertex1	= getCirclePanel ( )->getVertex1UniqueName ( );
			const string	vertex2	= getCirclePanel ( )->getVertex2UniqueName ( );
			const string	vertex3	= getCirclePanel ( )->getVertex3UniqueName ( );
			cmdResult	= getContext ( ).getGeomManager ( ).newEllipse (vertex1, vertex2, vertex3, name);
		}
		break;
		default	:
		{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Méthode d'opération non supportée ("  << (unsigned long)getCirclePanel ( )->getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtCircleOperationAction::executeOperation")
			throw exc;
		}
		break;
	}	// switch (getCirclePanel ( )->getOperationMethod ( ))

	setCommandResult (cmdResult);
}	// QtCircleOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
