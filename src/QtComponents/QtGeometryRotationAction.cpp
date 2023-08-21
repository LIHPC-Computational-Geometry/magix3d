/**
 * \file        QtGeometryRotationAction.cpp
 * \author      Charles PIGNEROL
 * \date        22/05/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeometryRotationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtGeometryRotationPanel
// ===========================================================================

QtGeometryRotationPanel::QtGeometryRotationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).geomRotationOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).geomRotationOperationTag),
	  _geomEntitiesPanel (0), _allEntitiesCheckBox (0), _propagateCheckBox (0),
	  _rotationPanel (0), _copyCheckBox(0), _namePanel(0), _withTopologyCheckBox(0)
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

	// Faut il appliquer la rotation à toutes les entités ?
	_allEntitiesCheckBox	= 
			new QCheckBox (QString::fromUtf8("Appliquer à toutes les entités\n(géométriques, topologiques et au maillage)"), this);
	_allEntitiesCheckBox->setChecked (false);
	connect (_allEntitiesCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (allEntitiesCallback ( )));
	layout->addWidget (_allEntitiesCheckBox);

	// Les entités à associer :
	FilterEntity::objectType	filter	=
		(FilterEntity::objectType)(FilterEntity::GeomVertex |
		FilterEntity::GeomCurve | FilterEntity::GeomSurface |
			FilterEntity::GeomVolume);
	_geomEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
							this, mainWindow, "Entités géométriques :", 
							SelectionManagerIfc::ALL_DIMENSIONS,
							filter, SelectionManagerIfc::D3, true);
	_geomEntitiesPanel->setMultiSelectMode (true);
	connect (_geomEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_geomEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_geomEntitiesPanel);

	// Faut il propager la rotation aux entités géométriques ?
/* Propagate is not yet implemented ...
	_propagateCheckBox	=
			new QCheckBox ("Propager aux entités géométriques", this);
	_propagateCheckBox->setChecked (true);
	layout->addWidget (_propagateCheckBox);
*/

	// La rotation :
	_rotationPanel	= new QtMgx3DRotationPanel (
		this, "", true, QtAnglePanel::eightthToHalf ( ), 0, &mainWindow);
	connect (_rotationPanel, SIGNAL (rotationModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));
	layout->addWidget (_rotationPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
			)->setLinkedSeizureManagers (0, 0);
// Plus souvent disabled :
//							0, _rotationPanel->getSegmentIDTextField ( ));

	// Faut il faire une copie des entités avant la transformation ?
	_copyCheckBox	=
			new QCheckBox ("Copier avant transformation", this);
	connect (_copyCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (copyCallback ( )));
	_copyCheckBox->setChecked (false);
	layout->addWidget (_copyCheckBox);

	// Nom groupe (en cas de copie):
	_namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe", mainWindow, 3,
			QtMgx3DGroupNamePanel::CREATION, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// Faut il propager la copie aux entités topologiques associées ?
	_withTopologyCheckBox	=
			new QCheckBox (QString::fromUtf8("Copier également la topologie associée"), this);
	_withTopologyCheckBox->setChecked (false);
	layout->addWidget (_withTopologyCheckBox);

	_namePanel->setEnabled (false);
	_withTopologyCheckBox->setEnabled (false);

}	// QtGeometryRotationPanel::QtGeometryRotationPanel


QtGeometryRotationPanel::QtGeometryRotationPanel (
										const QtGeometryRotationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _geomEntitiesPanel (0), _allEntitiesCheckBox (0), _propagateCheckBox (0),
	  _rotationPanel (0), _copyCheckBox(0), _namePanel(0), _withTopologyCheckBox(0)
{
	MGX_FORBIDDEN ("QtGeometryRotationPanel copy constructor is not allowed.");
}	// QtGeometryRotationPanel::QtGeometryRotationPanel (const QtGeometryRotationPanel&)


QtGeometryRotationPanel& QtGeometryRotationPanel::operator = (
											const QtGeometryRotationPanel&)
{
	MGX_FORBIDDEN ("QtGeometryRotationPanel assignment operator is not allowed.");
	return *this;
}	// QtGeometryRotationPanel::QtGeometryRotationPanel (const QtGeometryRotationPanel&)


QtGeometryRotationPanel::~QtGeometryRotationPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtGeometryRotationPanel::~QtGeometryRotationPanel


void QtGeometryRotationPanel::setDimension (SelectionManagerIfc::DIM dim)
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	_geomEntitiesPanel->clearSelection ( );
	_geomEntitiesPanel->setDimensions (dim);
}	// QtGeometryRotationPanel::setDimension


bool QtGeometryRotationPanel::processAllGeomEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_allEntitiesCheckBox)
	return _allEntitiesCheckBox->isChecked ( );
}	// QtGeometryRotationPanel::processAllGeomEntities

bool QtGeometryRotationPanel::copyEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_copyCheckBox)
	return _copyCheckBox->isChecked ( );
}	// QtGeometryRotationPanel::copyEntities

string QtGeometryRotationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtGeometryRotationPanel::getGroupName

bool QtGeometryRotationPanel::copyTopoEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)
	return _withTopologyCheckBox->isChecked ( );
}	// QtGeometryRotationPanel::copyTopoEntities

vector<string> QtGeometryRotationPanel::getGeomEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	return _geomEntitiesPanel->getEntitiesNames ( );
}	// QtGeometryRotationPanel::getGeomEntitiesNames


Math::Rotation QtGeometryRotationPanel::getRotation ( ) const
{
	CHECK_NULL_PTR_ERROR (_rotationPanel)
	return _rotationPanel->getRotation ( );
}	// QtGeometryRotationPanel::getRotation


bool QtGeometryRotationPanel::doPropagate ( ) const
{
return false;
// Not yet implemented :
//	CHECK_NULL_PTR_ERROR (_propagateCheckBox)
//	return Qt::Checked == _propagateCheckBox->checkState ( ) ? true : false;
}	// QtGeometryRotationPanel::doPropagate


void QtGeometryRotationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_rotationPanel)
	_namePanel->autoUpdate ( );
	_rotationPanel->reset ( );
	_geomEntitiesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeometryRotationPanel::reset


void QtGeometryRotationPanel::validate ( )
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
		error << "QtGeometryRotationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getGeomEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGeometryRotationPanel::validate


void QtGeometryRotationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_rotationPanel)
	_geomEntitiesPanel->stopSelection ( );
	_rotationPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_geomEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeometryRotationPanel::cancel


void QtGeometryRotationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedGeomEntities	=
					getSelectionManager ( ).getEntitiesNames (
									_geomEntitiesPanel->getAllowedTypes ( ));
		if (0 != selectedGeomEntities.size ( ))
			_geomEntitiesPanel->setEntitiesNames (selectedGeomEntities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_geomEntitiesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_namePanel->autoUpdate ( );
	_geomEntitiesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtGeometryRotationPanel::autoUpdate


vector<Entity*> QtGeometryRotationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	names	= getGeomEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*	entity	=
					getContext ( ).getGeomManager ( ).getEntity (*it, false);
		if (0 != entity)
			entities.push_back (entity);
	}

	return entities;
}	// QtGeometryRotationPanel::getInvolvedEntities


void QtGeometryRotationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _geomEntitiesPanel)
		_geomEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeometryRotationPanel::operationCompleted


void QtGeometryRotationPanel::allEntitiesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)
	_geomEntitiesPanel->setEnabled (!processAllGeomEntities ( ));
	_withTopologyCheckBox->setEnabled (copyEntities ( ) && !processAllGeomEntities ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeometryRotationPanel::allEntitiesCallback

void QtGeometryRotationPanel::copyCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)

	_namePanel->setEnabled (copyEntities ( ));
	_withTopologyCheckBox->setEnabled (copyEntities ( ) && !processAllGeomEntities ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeometryRotationPanel::copyCallback

// ===========================================================================
//                  LA CLASSE QtGeometryRotationAction
// ===========================================================================

QtGeometryRotationAction::QtGeometryRotationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeometryRotationPanel*	operationPanel	=
		new QtGeometryRotationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeometryRotationAction::QtGeometryRotationAction


QtGeometryRotationAction::QtGeometryRotationAction (
										const QtGeometryRotationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0, ""), "")
{
	MGX_FORBIDDEN ("QtGeometryRotationAction copy constructor is not allowed.")
}	// QtGeometryRotationAction::QtGeometryRotationAction


QtGeometryRotationAction& QtGeometryRotationAction::operator = (
										const QtGeometryRotationAction&)
{
	MGX_FORBIDDEN ("QtGeometryRotationAction assignment operator is not allowed.")
	return *this;
}	// QtGeometryRotationAction::operator =


QtGeometryRotationAction::~QtGeometryRotationAction ( )
{
}	// QtGeometryRotationAction::~QtGeometryRotationAction


QtGeometryRotationPanel*
					QtGeometryRotationAction::getRotationPanel ( )
{
	return dynamic_cast<QtGeometryRotationPanel*>(getOperationPanel ( ));
}	// QtGeometryRotationAction::getRotationPanel


void QtGeometryRotationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités géométriques :
	QtGeometryRotationPanel*	panel	= getRotationPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	Math::Rotation			rotation	= panel->getRotation ( );
	bool					propagate	= panel->doPropagate ( );
	const bool			    copy        = panel->copyEntities();
	const string	      	groupName	= panel->getGroupName ( );
	const bool			    withTopo	= panel->copyTopoEntities ( );

	if (true == panel->processAllGeomEntities ( )){
		if (copy)
			cmdResult	= getContext ( ).getGeomManager ( ).copyAndRotateAll (rotation, groupName);
		else
			cmdResult	= getContext ( ).getGeomManager ( ).rotateAll (rotation);
	}
	else
	{
		vector<string>	entities	= panel->getGeomEntitiesNames ( );
		if (copy)
			cmdResult	= getContext ( ).getGeomManager ( ).copyAndRotate (entities, rotation, withTopo, groupName);
		else
			cmdResult	= getContext ( ).getGeomManager ( ).rotate (entities, rotation);
	}	// else if (true == panel->processAllGeomEntities ( ))

	setCommandResult (cmdResult);
}	// QtGeometryRotationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
