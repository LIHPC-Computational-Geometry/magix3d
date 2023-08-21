/**
 * \file        QtGeomVolumeCreationAction.cpp
 * \author      Charles PIGNEROL
 * \date        15/12/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Geom/Vertex.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeomVolumeCreationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>

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
//                        LA CLASSE QtGeomVolumeCreationPanel
// ===========================================================================

QtGeomVolumeCreationPanel::QtGeomVolumeCreationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).volumeCreationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).volumeCreationTag),
	  _namePanel (0), _contourEntitiesPanel (0)
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

	// Nom groupe :
	_namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe", mainWindow, 3,
										QtMgx3DGroupNamePanel::CREATION, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// Les courbes définissant le contour du volume à créer :
	const FilterEntity::objectType	filter	= FilterEntity::GeomSurface;
	const SelectionManagerIfc::DIM	dimensions	= SelectionManagerIfc::D2;
	_contourEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
						this, mainWindow, "Surfaces :", dimensions,
						filter, SelectionManagerIfc::D2, false);
	_contourEntitiesPanel->setMultiSelectMode (true);
	connect (_contourEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_contourEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_contourEntitiesPanel, SIGNAL (dimensionsModified ( )),
	         this, SLOT (dimensionsModifiedCallback ( )));
	layout->addWidget (_contourEntitiesPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_contourEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
											)->setLinkedSeizureManagers (0, 0);
}	// QtGeomVolumeCreationPanel::QtGeomVolumeCreationPanel


QtGeomVolumeCreationPanel::QtGeomVolumeCreationPanel (
									const QtGeomVolumeCreationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0,0), 0, "", ""),
	  _namePanel (0), _contourEntitiesPanel (0)
{
	MGX_FORBIDDEN ("QtGeomVolumeCreationPanel copy constructor is not allowed.");
}	// QtGeomVolumeCreationPanel::QtGeomVolumeCreationPanel (const QtGeomVolumeCreationPanel&)


QtGeomVolumeCreationPanel& QtGeomVolumeCreationPanel::operator = (
										const QtGeomVolumeCreationPanel&)
{
	MGX_FORBIDDEN ("QtGeomVolumeCreationPanel assignment operator is not allowed.");
	return *this;
}	// QtGeomVolumeCreationPanel::QtGeomVolumeCreationPanel (const QtGeomVolumeCreationPanel&)


QtGeomVolumeCreationPanel::~QtGeomVolumeCreationPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtGeomVolumeCreationPanel::~QtGeomVolumeCreationPanel


string QtGeomVolumeCreationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtGeomVolumeCreationPanel::getGroupName


int QtGeomVolumeCreationPanel::getDimension ( ) const
{
	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)
	return SelectionManagerIfc::dimensionsToDimension (
									_contourEntitiesPanel->getDimensions ( ));
}	// QtGeomVolumeCreationPanel::getDimension


void QtGeomVolumeCreationPanel::setDimension (int dimension)
{
	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_namePanel)
	if (dimension != getDimension ( ))
	{
		_contourEntitiesPanel->clearSelection ( );
		_contourEntitiesPanel->setDimensions (
						SelectionManagerIfc::dimensionToDimensions (dimension));
		_namePanel->setDimension (dimension);
	}	// if (dimension != getDimension ( ))
}	// QtGeomVolumeCreationPanel::setDimension


vector<string> QtGeomVolumeCreationPanel::getEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)
	return _contourEntitiesPanel->getEntitiesNames ( );
}	// QtGeomVolumeCreationPanel::getEntitiesNames


void QtGeomVolumeCreationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)
	_contourEntitiesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeomVolumeCreationPanel::reset


void QtGeomVolumeCreationPanel::validate ( )
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
		error << "QtGeomVolumeCreationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getGeomEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGeomVolumeCreationPanel::validate


void QtGeomVolumeCreationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)
	_contourEntitiesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_contourEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeomVolumeCreationPanel::cancel


void QtGeomVolumeCreationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		if (0 != _namePanel)
			_namePanel->autoUpdate ( );
		FilterEntity::objectType	filter	=
				(FilterEntity::objectType)(FilterEntity::GeomVertex |
					FilterEntity::GeomCurve);
		vector<string>	geomEntities	=
							getSelectionManager ( ).getEntitiesNames (filter);
		CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)	
		if (0 != geomEntities.size ( ))
			_contourEntitiesPanel->setEntitiesNames (geomEntities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	if (0 != _namePanel)
		_namePanel->autoUpdate ( );
	_contourEntitiesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_contourEntitiesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtGeomVolumeCreationPanel::autoUpdate


vector<Entity*> QtGeomVolumeCreationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	names	= getEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*	entity	= getContext ( ).getGeomManager ( ).getEntity (*it);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}

	return entities;
}	// QtGeomVolumeCreationPanel::getInvolvedEntities


void QtGeomVolumeCreationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _contourEntitiesPanel)
		_contourEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );

	// Actualisation des groupes : certains peuvent être créés, d'autres
	// détruits (ex : "Hors Groupes 3D" :
	if (0 != _namePanel)
		_namePanel->autoUpdate ( );
}	// QtGeomVolumeCreationPanel::operationCompleted


void QtGeomVolumeCreationPanel::dimensionsModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _namePanel)
	{
		_namePanel->setDimension (getDimension ( ));
		_namePanel->autoUpdate ( );
	}	// if (0 != _namePanel)
	
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeomVolumeCreationPanel::dimensionsModifiedCallback


// ===========================================================================
//                  LA CLASSE QtGeomVolumeCreationAction
// ===========================================================================

QtGeomVolumeCreationAction::QtGeomVolumeCreationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeomVolumeCreationPanel*	operationPanel	=
		new QtGeomVolumeCreationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeomVolumeCreationAction::QtGeomVolumeCreationAction


QtGeomVolumeCreationAction::QtGeomVolumeCreationAction (
										const QtGeomVolumeCreationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeomVolumeCreationAction copy constructor is not allowed.")
}	// QtGeomVolumeCreationAction::QtGeomVolumeCreationAction


QtGeomVolumeCreationAction& QtGeomVolumeCreationAction::operator = (
										const QtGeomVolumeCreationAction&)
{
	MGX_FORBIDDEN ("QtGeomVolumeCreationAction assignment operator is not allowed.")
	return *this;
}	// QtGeomVolumeCreationAction::operator =


QtGeomVolumeCreationAction::~QtGeomVolumeCreationAction ( )
{
}	// QtGeomVolumeCreationAction::~QtGeomVolumeCreationAction


QtGeomVolumeCreationPanel*
					QtGeomVolumeCreationAction::getVolumeCreationPanel ( )
{
	return dynamic_cast<QtGeomVolumeCreationPanel*>(getOperationPanel ( ));
}	// QtGeomVolumeCreationAction::getVolumeCreationPanel


void QtGeomVolumeCreationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de recueil des surfaces géométriques :
	QtGeomVolumeCreationPanel*	panel	= getVolumeCreationPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string		groupName	= panel->getGroupName ( );
	vector<string>		entities	= panel->getEntitiesNames ( );
	
	cmdResult	= getContext ( ).getGeomManager ( ).newVolume (entities, groupName);

	setCommandResult (cmdResult);
}	// QtGeomVolumeCreationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
