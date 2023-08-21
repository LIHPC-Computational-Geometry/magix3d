/**
 * \file        QtSegmentOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        10/09/2013
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"
#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "Geom/Vertex.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtSegmentOperationAction.h"
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
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
//                        LA CLASSE QtSegmentOperationPanel
// ===========================================================================

QtSegmentOperationPanel::QtSegmentOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).segmentOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).segmentOperationTag),
	  _namePanel (0), _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _verticesPanel (0)
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

	// Nom groupe :
	_namePanel	= new QtMgx3DGroupNamePanel (
							this, "Groupe", mainWindow, 1, creationPolicy, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// Méthode de création/modification du segment :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Méthode"), this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem (QString::fromUtf8("Par saisie des extrémités"));
	connect (_operationMethodComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (operationMethodCallback ( )));
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);
	
	// Définition du segment :
	QtGroupBox*		groupBox	= new QtGroupBox(QString::fromUtf8("Paramètres du segment"), this);
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
	_verticesPanel	= new Qt2VerticiesPanel (
				0, "Définition d'un segment par ses 2 extrémités", mainWindow,
				 FilterEntity::GeomVertex);
	connect (_verticesPanel, SIGNAL (pointAddedToSelection (QString)), this,
	         SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_verticesPanel, SIGNAL (pointRemovedFromSelection (QString)), this,
	         SLOT (entitiesRemovedFromSelectionCallback (QString)));
	_verticesPanel->hide ( );
	operationMethodCallback ( );

	vlayout->addStretch (2);
}	// QtSegmentOperationPanel::QtSegmentOperationPanel


QtSegmentOperationPanel::QtSegmentOperationPanel (
										const QtSegmentOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _verticesPanel (0)
{
	MGX_FORBIDDEN ("QtSegmentOperationPanel copy constructor is not allowed.");
}	// QtSegmentOperationPanel::QtSegmentOperationPanel (const QtSegmentOperationPanel&)


QtSegmentOperationPanel& QtSegmentOperationPanel::operator = (
											const QtSegmentOperationPanel&)
{
	MGX_FORBIDDEN ("QtSegmentOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtSegmentOperationPanel::QtSegmentOperationPanel (const QtSegmentOperationPanel&)


QtSegmentOperationPanel::~QtSegmentOperationPanel ( )
{
}	// QtSegmentOperationPanel::~QtSegmentOperationPanel


string QtSegmentOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtSegmentOperationPanel::getGroupName


QtSegmentOperationPanel::OPERATION_METHOD
						QtSegmentOperationPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtSegmentOperationPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtSegmentOperationPanel::getOperationMethod


string QtSegmentOperationPanel::getVertex1Name ( ) const
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	return _verticesPanel->getVertex1UniqueName ( );
}	// QtSegmentOperationPanel::getVertex1Name


string QtSegmentOperationPanel::getVertex2Name ( ) const
{
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	return _verticesPanel->getVertex2UniqueName ( );
}	// QtSegmentOperationPanel::getVertex2Name


void QtSegmentOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_verticesPanel)
	_namePanel->autoUpdate ( );
	_verticesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtSegmentOperationPanel::reset


void QtSegmentOperationPanel::validate ( )
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
		error << "QtSegmentOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtSegmentOperationPanel::VERTEX_VERTEX	:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode d'opération de création/modification "
			      << "de segment sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtSegmentOperationPanel::validate : index de méthode "
			      << "d'opération de création/modification de segment "
			      << "invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getX ( ), "Abscisse point : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getY ( ), "Ordonnée point : ", error)
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getZ ( ), "Elévation point : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtSegmentOperationPanel::validate


void QtSegmentOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	if (0 != _currentPanel)
		_currentPanel->cancel ( );
}	// QtSegmentOperationPanel::cancel


void QtSegmentOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	_namePanel->autoUpdate ( );

	if (0 != _currentPanel)
		_currentPanel->autoUpdate ( );

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtSegmentOperationPanel::autoUpdate


void QtSegmentOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _verticesPanel)
		_verticesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtSegmentOperationPanel::operationCompleted


void QtSegmentOperationPanel::operationMethodCallback ( )
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

	switch (getOperationMethod ( ))
	{
		case QtSegmentOperationPanel::VERTEX_VERTEX				:
			_currentPanel	= _verticesPanel;					break;
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode d'opération non supportée ("
			        << (unsigned long)getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtSegmentOperationPanel::operationMethodCallback")
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
}	// QtSegmentOperationPanel::operationMethodCallback


// ===========================================================================
//                  LA CLASSE QtSegmentOperationAction
// ===========================================================================

QtSegmentOperationAction::QtSegmentOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtSegmentOperationPanel*	operationPanel	=
		new QtSegmentOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy,
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtSegmentOperationAction::QtSegmentOperationAction


QtSegmentOperationAction::QtSegmentOperationAction (
										const QtSegmentOperationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtSegmentOperationAction copy constructor is not allowed.")
}	// QtSegmentOperationAction::QtSegmentOperationAction


QtSegmentOperationAction& QtSegmentOperationAction::operator = (
										const QtSegmentOperationAction&)
{
	MGX_FORBIDDEN ("QtSegmentOperationAction assignment operator is not allowed.")
	return *this;
}	// QtSegmentOperationAction::operator =


QtSegmentOperationAction::~QtSegmentOperationAction ( )
{
}	// QtSegmentOperationAction::~QtSegmentOperationAction


QtSegmentOperationPanel* QtSegmentOperationAction::getSegmentPanel ( )
{
	return dynamic_cast<QtSegmentOperationPanel*>(getOperationPanel ( ));
}	// QtSegmentOperationAction::getSegmentPanel


void QtSegmentOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
//	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de création du segment :
	QtSegmentOperationPanel*	panel	= getSegmentPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string	name	= panel->getGroupName ( );
	switch (getSegmentPanel ( )->getOperationMethod ( ))
	{
		case QtSegmentOperationPanel::VERTEX_VERTEX				:
		{
			const string	vertex1	= getSegmentPanel ( )->getVertex1Name ( );
			const string	vertex2	= getSegmentPanel ( )->getVertex2Name ( );
			cmdResult	= getContext ( ).getGeomManager ( ).newSegment (vertex1, vertex2, name);
		}
		break;
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode d'opération non supportée ("
			        << (unsigned long)getSegmentPanel ( )->getOperationMethod ( )
			        << ").";
			INTERNAL_ERROR (exc, message, "QtSegmentOperationAction::executeOperation")
			throw exc;
		}
		break;
	}	// switch (getSegmentPanel ( )->getOperationMethod ( ))

	setCommandResult (cmdResult);
}	// QtSegmentOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
