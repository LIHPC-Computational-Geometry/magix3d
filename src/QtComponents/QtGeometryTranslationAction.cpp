/**
 * \file        QtGeometryTranslationAction.cpp
 * \author      Charles PIGNEROL
 * \date        26/05/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeometryTranslationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"

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
using namespace Mgx3D::Geom;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtGeometryTranslationPanel
// ===========================================================================

QtGeometryTranslationPanel::QtGeometryTranslationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).geomTranslationOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).geomTranslationOperationTag),
	  _geomEntitiesPanel (0), _allEntitiesCheckBox (0), _propagateCheckBox (0),
	  _translationPanel (0),
	  _copyCheckBox(0), _namePanel(0), _withTopologyCheckBox(0)
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

	// Faut il appliquer l'homothétie à toutes les entités ?
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

	// Faut il propager la translation aux entités géométriques ?
/* Propagate is not yet implemented ...
	_propagateCheckBox	=
			new QCheckBox ("Propager aux entités géométriques", this);
	_propagateCheckBox->setChecked (true);
	layout->addWidget (_propagateCheckBox);
*/

	// La translation :
	// Définition par une arête :
	_translationPanel	=
			new QtMgx3DVectorPanel (this, "", true,
					"dx :", "dy :", "dz :",
					1., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
					0., -DBL_MAX, DBL_MAX, &mainWindow,
					(FilterEntity::objectType)(
							FilterEntity::AllPoints | FilterEntity::AllEdges),
					true);
	connect (_translationPanel, SIGNAL (vectorModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));
	layout->addWidget (_translationPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_translationPanel->getSegmentIDTextField ( ))
	_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
			)->setLinkedSeizureManagers (
							0, _translationPanel->getSegmentIDTextField ( ));
	_translationPanel->getSegmentIDTextField ( )->setLinkedSeizureManagers (
			_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ), 0);

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

}	// QtGeometryTranslationPanel::QtGeometryTranslationPanel


QtGeometryTranslationPanel::QtGeometryTranslationPanel (
										const QtGeometryTranslationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _geomEntitiesPanel (0), _allEntitiesCheckBox (0), _propagateCheckBox (0),
	  _translationPanel (0),
	  _copyCheckBox(0), _namePanel(0), _withTopologyCheckBox(0)
{
	MGX_FORBIDDEN ("QtGeometryTranslationPanel copy constructor is not allowed.");
}	// QtGeometryTranslationPanel::QtGeometryTranslationPanel (const QtGeometryTranslationPanel&)


QtGeometryTranslationPanel& QtGeometryTranslationPanel::operator = (
											const QtGeometryTranslationPanel&)
{
	MGX_FORBIDDEN ("QtGeometryTranslationPanel assignment operator is not allowed.");
	return *this;
}	// QtGeometryTranslationPanel::QtGeometryTranslationPanel (const QtGeometryTranslationPanel&)


QtGeometryTranslationPanel::~QtGeometryTranslationPanel ( )
{
	preview (false, true);
}	// QtGeometryTranslationPanel::~QtGeometryTranslationPanel


void QtGeometryTranslationPanel::setDimension (SelectionManagerIfc::DIM dim)
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	_geomEntitiesPanel->clearSelection ( );
	_geomEntitiesPanel->setDimensions (dim);
}	// QtGeometryTranslationPanel::setDimension


bool QtGeometryTranslationPanel::processAllGeomEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_allEntitiesCheckBox)
	return _allEntitiesCheckBox->isChecked ( );
}	// QtGeometryTranslationPanel::processAllGeomEntities


bool QtGeometryTranslationPanel::copyEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_copyCheckBox)
	return _copyCheckBox->isChecked ( );
}	// QtGeometryTranslationPanel::copyEntities

string QtGeometryTranslationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtGeometryTranslationPanel::getGroupName

bool QtGeometryTranslationPanel::copyTopoEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)
	return _withTopologyCheckBox->isChecked ( );
}	// QtGeometryTranslationPanel::copyTopoEntities

vector<string> QtGeometryTranslationPanel::getGeomEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	return _geomEntitiesPanel->getEntitiesNames ( );
}	// QtGeometryTranslationPanel::getGeomEntitiesNames


Math::Vector QtGeometryTranslationPanel::getTranslation ( ) const
{
	CHECK_NULL_PTR_ERROR (_translationPanel)
	return Vector (_translationPanel->getDx ( ),
	               _translationPanel->getDy ( ),
	               _translationPanel->getDz ( ));;
}	// QtGeometryTranslationPanel::getTranslation


bool QtGeometryTranslationPanel::doPropagate ( ) const
{
return false;
// Not yet implemented :
//	CHECK_NULL_PTR_ERROR (_propagateCheckBox)
//	return Qt::Checked == _propagateCheckBox->checkState ( ) ? true : false;
}	// QtGeometryTranslationPanel::doPropagate


void QtGeometryTranslationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_translationPanel)
	_namePanel->autoUpdate ( );
	_geomEntitiesPanel->reset ( );
	_translationPanel->reset ( );
	_translationPanel->setDx (1.);
	_translationPanel->setDy (0.);
	_translationPanel->setDz (0.);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeometryTranslationPanel::reset


void QtGeometryTranslationPanel::validate ( )
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
		error << "QtGeometryTranslationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getGeomEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGeometryTranslationPanel::validate


void QtGeometryTranslationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_translationPanel)
	_geomEntitiesPanel->stopSelection ( );
	_translationPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_geomEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeometryTranslationPanel::cancel


void QtGeometryTranslationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_translationPanel)
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
	_translationPanel->setSegment ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationPanel::autoUpdate ( );

	_namePanel->autoUpdate ( );
	_geomEntitiesPanel->actualizeGui (true);
	_translationPanel->actualizeGui (true);
}	// QtGeometryTranslationPanel::autoUpdate


vector<Entity*> QtGeometryTranslationPanel::getInvolvedEntities ( )
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
}	// QtGeometryTranslationPanel::getInvolvedEntities


void QtGeometryTranslationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _geomEntitiesPanel)
		_geomEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeometryTranslationPanel::operationCompleted


void QtGeometryTranslationPanel::allEntitiesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)
	_geomEntitiesPanel->setEnabled (!processAllGeomEntities ( ));
	_withTopologyCheckBox->setEnabled (copyEntities ( ) && !processAllGeomEntities ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeometryTranslationPanel::allEntitiesCallback

void QtGeometryTranslationPanel::copyCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)

	_namePanel->setEnabled (copyEntities ( ));
	_withTopologyCheckBox->setEnabled (copyEntities ( ) && !processAllGeomEntities ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeometryTranslationPanel::copyCallback


// ===========================================================================
//                  LA CLASSE QtGeometryTranslationAction
// ===========================================================================

QtGeometryTranslationAction::QtGeometryTranslationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeometryTranslationPanel*	operationPanel	=
		new QtGeometryTranslationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeometryTranslationAction::QtGeometryTranslationAction


QtGeometryTranslationAction::QtGeometryTranslationAction (
										const QtGeometryTranslationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeometryTranslationAction copy constructor is not allowed.")
}	// QtGeometryTranslationAction::QtGeometryTranslationAction


QtGeometryTranslationAction& QtGeometryTranslationAction::operator = (
										const QtGeometryTranslationAction&)
{
	MGX_FORBIDDEN ("QtGeometryTranslationAction assignment operator is not allowed.")
	return *this;
}	// QtGeometryTranslationAction::operator =


QtGeometryTranslationAction::~QtGeometryTranslationAction ( )
{
}	// QtGeometryTranslationAction::~QtGeometryTranslationAction


QtGeometryTranslationPanel*
					QtGeometryTranslationAction::getTranslationPanel ( )
{
	return dynamic_cast<QtGeometryTranslationPanel*>(getOperationPanel ( ));
}	// QtGeometryTranslationAction::getTranslationPanel


void QtGeometryTranslationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités géométriques :
	QtGeometryTranslationPanel*	panel	= getTranslationPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	Math::Vector			translation	(panel->getTranslation ( ));
	bool					propagate	= panel->doPropagate ( );
	const bool			    copy        = panel->copyEntities();
	const string	      	groupName	= panel->getGroupName ( );
	const bool			    withTopo	= panel->copyTopoEntities ( );

	if (true == panel->processAllGeomEntities ( )){
		if (copy)
			cmdResult	= getContext ( ).getGeomManager ( ).copyAndTranslateAll (translation, groupName);
		else
			cmdResult	= getContext ( ).getGeomManager ( ).translateAll (translation);
	}
	else
	{
		vector<string>			entities	= panel->getGeomEntitiesNames ( );
		if (copy)
			cmdResult	= getContext ( ).getGeomManager ( ).copyAndTranslate (entities, translation, withTopo, groupName);
		else
			cmdResult	= getContext ( ).getGeomManager ( ).translate (entities, translation);
	}	// else if (true == panel->processAllGeomEntities ( ))

	setCommandResult (cmdResult);
}	// QtGeometryTranslationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
