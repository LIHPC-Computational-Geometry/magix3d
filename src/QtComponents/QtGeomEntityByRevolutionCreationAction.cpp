/**
 * \file        QtGeomEntityByRevolutionCreationAction.cpp
 * \author      Charles PIGNEROL
 * \date        18/11/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "QtComponents/QtGeomEntityByRevolutionCreationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QVBoxLayout>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtGeomEntityByRevolutionCreationPanel
// ===========================================================================

QtGeomEntityByRevolutionCreationPanel::QtGeomEntityByRevolutionCreationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).geomByRevolutionOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).geomByRevolutionOperationTag),
	  _entityPanel (0), _rotationPanel (0), _keepEntitiesCheckBox (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins (0, 0, 0, 0);

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Entités à soumettre à la rotation :
	const SelectionManagerIfc::DIM	allowedDimensions	=
		(SelectionManagerIfc::DIM)(
				SelectionManagerIfc::D0 | SelectionManagerIfc::D1 | SelectionManagerIfc::D2);
	const FilterEntity::objectType	filter				=
		(FilterEntity::objectType)(
				FilterEntity::GeomVertex | FilterEntity::GeomCurve | FilterEntity::GeomSurface);
	_entityPanel	= new QtEntityByDimensionSelectorPanel (
						this, mainWindow, "Entités géométriques :",
						allowedDimensions, filter, SelectionManagerIfc::D1, 
						true);
	_entityPanel->setMultiSelectMode (true);
	layout->addWidget (_entityPanel);
	connect (_entityPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_entityPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// Rotation :
	_rotationPanel	= new QtMgx3DRotationPanel (this,
		"Rotation", true, QtAnglePanel::eightthToWhole ( ), 0, &mainWindow);
	layout->addWidget (_rotationPanel);
	connect (_rotationPanel, SIGNAL (rotationModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));

	// Conserver les entités initiales ?
	_keepEntitiesCheckBox	=
					new QCheckBox (QString::fromUtf8("Conserver les entités initiales"), this);
	_keepEntitiesCheckBox->setCheckState (Qt::Unchecked);
	layout->addWidget (_keepEntitiesCheckBox);

	layout->addStretch (2);
}	// QtGeomEntityByRevolutionCreationPanel::QtGeomEntityByRevolutionCreationPanel


QtGeomEntityByRevolutionCreationPanel::QtGeomEntityByRevolutionCreationPanel (
							const QtGeomEntityByRevolutionCreationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _entityPanel (0), _rotationPanel (0), _keepEntitiesCheckBox (0)
{
	MGX_FORBIDDEN ("QtGeomEntityByRevolutionCreationPanel copy constructor is not allowed.");
}	// QtGeomEntityByRevolutionCreationPanel::QtGeomEntityByRevolutionCreationPanel (const QtGeomEntityByRevolutionCreationPanel&)


QtGeomEntityByRevolutionCreationPanel&
			QtGeomEntityByRevolutionCreationPanel::operator = (
								const QtGeomEntityByRevolutionCreationPanel&)
{
	MGX_FORBIDDEN ("QtGeomEntityByRevolutionCreationPanel assignment operator is not allowed.");
	return *this;
}	// QtGeomEntityByRevolutionCreationPanel::QtGeomEntityByRevolutionCreationPanel (const QtGeomEntityByRevolutionCreationPanel&)


QtGeomEntityByRevolutionCreationPanel::~QtGeomEntityByRevolutionCreationPanel ( )
{
}	// QtGeomEntityByRevolutionCreationPanel::~QtGeomEntityByRevolutionCreationPanel


vector<Entity*> QtGeomEntityByRevolutionCreationPanel::getInvolvedEntities ( )
{
	CHECK_NULL_PTR_ERROR (_entityPanel)
	CHECK_NULL_PTR_ERROR (_entityPanel->getEntitiesPanel ( ))
	vector<Entity*>	entities;

	const vector<string>	names	=
					_entityPanel->getEntitiesPanel ( )->getUniqueNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*		entity		=
			getContext ( ).getGeomManager ( ).getEntity (*it, false);
	
		if (0 != entity)
			entities.push_back (entity);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtGeomEntityByRevolutionCreationPanel::getInvolvedEntities


vector<string> QtGeomEntityByRevolutionCreationPanel::getEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_entityPanel)
	return _entityPanel->getEntitiesNames ( );
}	// QtGeomEntityByRevolutionCreationPanel::getEntities


Utils::Math::Rotation
				QtGeomEntityByRevolutionCreationPanel::getRotation ( ) const
{
	CHECK_NULL_PTR_ERROR (_rotationPanel)
	return _rotationPanel->getRotation ( );
}	// QtGeomEntityByRevolutionCreationPanel::getRotation


bool QtGeomEntityByRevolutionCreationPanel::keepEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_keepEntitiesCheckBox)
	return Qt::Checked == _keepEntitiesCheckBox->checkState ( ) ? true : false;
}	// QtGeomEntityByRevolutionCreationPanel::keepEntities


void QtGeomEntityByRevolutionCreationPanel::setDimension (
										SelectionManagerIfc::DIM dimension)
{
	CHECK_NULL_PTR_ERROR (_entityPanel)
	_entityPanel->clearSelection ( );
	_entityPanel->setDimensions (dimension);
}	// QtGeomEntityByRevolutionCreationPanel::setDimension


void QtGeomEntityByRevolutionCreationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entityPanel)
	CHECK_NULL_PTR_ERROR (_rotationPanel)
	_entityPanel->reset ( );
	_rotationPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeomEntityByRevolutionCreationPanel::reset


void QtGeomEntityByRevolutionCreationPanel::validate ( )
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
		error << "QtGeomEntityByRevolutionCreationPanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGeomEntityByRevolutionCreationPanel::validate


void QtGeomEntityByRevolutionCreationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_entityPanel)
	_entityPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_entityPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeomEntityByRevolutionCreationPanel::cancel


void QtGeomEntityByRevolutionCreationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_entityPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedGeomEntities	=
			getSelectionManager ( ).getEntitiesNames (
											_entityPanel->getAllowedTypes ( ));
		if (1 == selectedGeomEntities.size ( ))
			_entityPanel->setEntitiesNames (selectedGeomEntities);
		else
			_entityPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_entityPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_entityPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtGeomEntityByRevolutionCreationPanel::autoUpdate


void QtGeomEntityByRevolutionCreationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entityPanel)
	_entityPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeomEntityByRevolutionCreationPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtGeomEntityByRevolutionCreationAction
// ===========================================================================

QtGeomEntityByRevolutionCreationAction::QtGeomEntityByRevolutionCreationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeomEntityByRevolutionCreationPanel*	operationPanel	=
		new QtGeomEntityByRevolutionCreationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeomEntityByRevolutionCreationAction::QtGeomEntityByRevolutionCreationAction


QtGeomEntityByRevolutionCreationAction::QtGeomEntityByRevolutionCreationAction (
										const QtGeomEntityByRevolutionCreationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeomEntityByRevolutionCreationAction copy constructor is not allowed.")
}	// QtGeomEntityByRevolutionCreationAction::QtGeomEntityByRevolutionCreationAction


QtGeomEntityByRevolutionCreationAction&
					 QtGeomEntityByRevolutionCreationAction::operator = (
									const QtGeomEntityByRevolutionCreationAction&)
{
	MGX_FORBIDDEN ("QtGeomEntityByRevolutionCreationAction assignment operator is not allowed.")
	return *this;
}	// QtGeomEntityByRevolutionCreationAction::operator =


QtGeomEntityByRevolutionCreationAction::~QtGeomEntityByRevolutionCreationAction ( )
{
}	// QtGeomEntityByRevolutionCreationAction::~QtGeomEntityByRevolutionCreationAction


QtGeomEntityByRevolutionCreationPanel*
		QtGeomEntityByRevolutionCreationAction::getGeomEntityByRevolutionPanel ( )
{
	return dynamic_cast<QtGeomEntityByRevolutionCreationPanel*>(getOperationPanel ( ));
}	// QtGeomEntityByRevolutionCreationAction::getGeomEntityByRevolutionPanel


void QtGeomEntityByRevolutionCreationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de création du cylindre :
	QtGeomEntityByRevolutionCreationPanel*	panel	=
										getGeomEntityByRevolutionPanel( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>	entitiesNames	= panel->getEntities ( );
	const Rotation	rotation		= panel->getRotation ( );
	const bool		keepEntities	= panel->keepEntities ( );
	cmdResult	= getContext ( ).getGeomManager ( ).makeRevol (entitiesNames, rotation, keepEntities);

	setCommandResult (cmdResult);
}	// QtGeomEntityByRevolutionCreationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
