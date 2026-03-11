/**
 * \file        QtExtrusionAction.cpp
 * \author      Charles PIGNEROL
 * \date        14/11/2014
 *
 *
 *
 *  Modified on: 21/02/2022
 *      Author: Simon C
 *      ajout de la possibilité de conserver les entités géométriques
 */

#include "Internal/Context.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManager.h"
#include "Geom/Surface.h"
#include "QtComponents/QtExtrusionAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/MemoryError.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QVBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Group;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtExtrusionPanel
// ===========================================================================

QtExtrusionPanel::QtExtrusionPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow,
			Internal::SelectionManager::DIM dimension,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).prismOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).prismOperationTag),
	  _entitiesPanel (0), _vectorPanel (0),
	  _withTopologyCheckBox (0), _keepEntitiesCheckBox (0)
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

	// Entitiés à extruder :
	FilterEntity::objectType	filter	= FilterEntity::NoneEntity;
	UTF8String					extrudedEntitiesLabel (Charset::UTF_8);
	switch (dimension)
	{
		case SelectionManager::D1	:
			filter	= FilterEntity::GeomCurve;
			extrudedEntitiesLabel << "Courbes à extruder :";
			break;
		case SelectionManager::D2	:
			filter	= FilterEntity::GeomSurface;
			extrudedEntitiesLabel << "Surfaces à extruder :";
			break;
		default						:
			extrudedEntitiesLabel << "Erreur interne !";
	}	// switch (dimension)
	_entitiesPanel	= new QtMgx3DEntityPanel (
						this, "", true, extrudedEntitiesLabel, "",
						&mainWindow, dimension, filter);
	_entitiesPanel->setMultiSelectMode (true);
	layout->addWidget (_entitiesPanel);
	connect (_entitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_entitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	
	// Définition du vecteur d'extrusion :
	_vectorPanel	=
				new QtMgx3DVectorPanel (this, "", true,
					"dx :", "dy :", "dz :",
					0., -DBL_MAX, DBL_MAX,
					0., -DBL_MAX, DBL_MAX,
					1., -DBL_MAX, DBL_MAX,
					&mainWindow, FilterEntity::AllEdges, true);
	connect (_vectorPanel, SIGNAL (vectorModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));
	layout->addWidget (_vectorPanel);

	// Faut il propager la copie aux entités topologiques associées ?
	if (SelectionManager::D2 == dimension)
	{
		_withTopologyCheckBox	= new QCheckBox ("Extrusion de la topologie", this);
		_withTopologyCheckBox->setChecked (true);
		layout->addWidget (_withTopologyCheckBox);
	}	// if (SelectionManager::D2 == dimension)

    // Conserver les entités initiales ?
    _keepEntitiesCheckBox	=
            new QCheckBox (QString::fromUtf8("Conserver les entités initiales"), this);
    _keepEntitiesCheckBox->setCheckState (Qt::Unchecked);
    layout->addWidget (_keepEntitiesCheckBox);

	layout->addStretch (2);
}	// QtExtrusionPanel::QtExtrusionPanel


QtExtrusionPanel::QtExtrusionPanel (const QtExtrusionPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _entitiesPanel (0), _vectorPanel(0), _withTopologyCheckBox (0), _keepEntitiesCheckBox (0)
{
	MGX_FORBIDDEN ("QtExtrusionPanel copy constructor is not allowed.");
}	// QtExtrusionPanel::QtExtrusionPanel (const QtExtrusionPanel&)


QtExtrusionPanel& QtExtrusionPanel::operator = (const QtExtrusionPanel&)
{
	MGX_FORBIDDEN ("QtExtrusionPanel assignment operator is not allowed.");
	return *this;
}	// QtExtrusionPanel::QtExtrusionPanel (const QtExtrusionPanel&)


QtExtrusionPanel::~QtExtrusionPanel ( )
{
}	// QtExtrusionPanel::~QtExtrusionPanel


vector<string> QtExtrusionPanel::getEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	return _entitiesPanel->getUniqueNames ( );
}	// QtExtrusionPanel::getEntitiesNames


Vector QtExtrusionPanel::getVector ( ) const
{
	 CHECK_NULL_PTR_ERROR (_vectorPanel)

	return Vector (_vectorPanel->getDx ( ), _vectorPanel->getDy ( ), _vectorPanel->getDz ( ));
}	// QtExtrusionPanel::getVector


bool QtExtrusionPanel::withTopology ( ) const
{
	if (0 == _withTopologyCheckBox)
		return false;

	return Qt::Checked == _withTopologyCheckBox->checkState ( ) ? true : false;
}	// QtExtrusionPanel::withTopology


void QtExtrusionPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	CHECK_NULL_PTR_ERROR (_vectorPanel)
	_entitiesPanel->reset ( );
	_vectorPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtExtrusionPanel::reset


void QtExtrusionPanel::validate ( )
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
		error << "QtExtrusionPanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtExtrusionPanel::validate


void QtExtrusionPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	_entitiesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_entitiesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtExtrusionPanel::cancel


void QtExtrusionPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	CHECK_NULL_PTR_ERROR (_vectorPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK
/* Actuellement inutilisé
		vector<string>	selectedSurfaces	=
			getSelectionManager ( ).getEntitiesNames(FilterEntity::GeomSurface);
		if (1 == selectedSurfaces.size ( ))
			_entitiesPanel->setUniqueName (selectedSurfaces [0]);
		vector<string>	selectedEdges	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::AllEdges);
		if (1 == selectedEdges.size ( ))
			_vectorPanel->setSegment (selectedEdges [0]);
*/
		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_entitiesPanel->clearSelection ( );
	_vectorPanel->setSegment ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationPanel::autoUpdate ( );

	_entitiesPanel->actualizeGui (true);
	_vectorPanel->actualizeGui (true);
}	// QtExtrusionPanel::autoUpdate


vector<Entity*> QtExtrusionPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const vector<string>	names	= getEntitiesNames ( );

	for (vector<string>::const_iterator it = names.begin ( ); names.end ( ) != it; it++)
	{
		GeomEntity*		entity	=
			getContext ( ).getGeomManager ( ).getEntity (*it, false);
		if (0 != entity)
			entities.push_back (entity);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtExtrusionPanel::getInvolvedEntities


void QtExtrusionPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	_entitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtExtrusionPanel::operationCompleted


bool QtExtrusionPanel::keepEntities ( ) const
{
    CHECK_NULL_PTR_ERROR (_keepEntitiesCheckBox)
    return Qt::Checked == _keepEntitiesCheckBox->checkState ( ) ? true : false;
}	// QtGeomEntityByRevolutionCreationPanel::keepEntities

// ===========================================================================
//                  LA CLASSE QtExtrusionAction
// ===========================================================================

QtExtrusionAction::QtExtrusionAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	Internal::SelectionManager::DIM dimension,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtExtrusionPanel*	operationPanel	=
		new QtExtrusionPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, dimension, creationPolicy, this);
	setOperationPanel (operationPanel);
}	// QtExtrusionAction::QtExtrusionAction


QtExtrusionAction::QtExtrusionAction (const QtExtrusionAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0, ""), "")
{
	MGX_FORBIDDEN ("QtExtrusionAction copy constructor is not allowed.")
}	// QtExtrusionAction::QtExtrusionAction


QtExtrusionAction& QtExtrusionAction::operator = (const QtExtrusionAction&)
{
	MGX_FORBIDDEN ("QtExtrusionAction assignment operator is not allowed.")
	return *this;
}	// QtExtrusionAction::operator =


QtExtrusionAction::~QtExtrusionAction ( )
{
}	// QtExtrusionAction::~QtExtrusionAction


QtExtrusionPanel* QtExtrusionAction::getExtrusionPanel ( )
{
	return dynamic_cast<QtExtrusionPanel*>(getOperationPanel ( ));
}	// QtExtrusionAction::getExtrusionPanel


void QtExtrusionAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResult*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de l'extrusion :
	QtExtrusionPanel*	panel	= getExtrusionPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	std::vector<std::string>	entities	= panel->getEntitiesNames ( );
	const Vector				v			= panel->getVector ( );
	const bool					withTopo	= panel->withTopology ( );
    const bool					keepEntities= panel->keepEntities ( );
	if (withTopo)
		cmdResult	= getContext ( ).getGeomManager ( ).makeBlocksByExtrude(entities, v, keepEntities);
	else
		cmdResult	= getContext ( ).getGeomManager ( ).makeExtrude (entities, v, keepEntities);

	setCommandResult (cmdResult);
}	// QtExtrusionAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
