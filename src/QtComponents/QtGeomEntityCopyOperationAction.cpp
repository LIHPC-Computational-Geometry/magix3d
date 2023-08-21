/**
 * \file        QtGeomEntityCopyOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        25/06/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeomEntityCopyOperationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"

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
//                        LA CLASSE QtGeomEntityCopyOperationPanel
// ===========================================================================

QtGeomEntityCopyOperationPanel::QtGeomEntityCopyOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).geomCopyOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).geomCopyOperationTag),
	  _namePanel (0), _geomEntitiesPanel (0), _withTopologyCheckBox (0)
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

	// Les entités à copier :
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
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
											)->setLinkedSeizureManagers (0, 0);


	// Faut il propager la copie aux entités topologiques associées ?
	_withTopologyCheckBox	=
			new QCheckBox (QString::fromUtf8("Copier également la topologie associée"), this);
	_withTopologyCheckBox->setChecked (true);
	layout->addWidget (_withTopologyCheckBox);

	layout->addStretch (2);
}	// QtGeomEntityCopyOperationPanel::QtGeomEntityCopyOperationPanel


QtGeomEntityCopyOperationPanel::QtGeomEntityCopyOperationPanel (
										const QtGeomEntityCopyOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _geomEntitiesPanel (0), _withTopologyCheckBox (0)
{
	MGX_FORBIDDEN ("QtGeomEntityCopyOperationPanel copy constructor is not allowed.");
}	// QtGeomEntityCopyOperationPanel::QtGeomEntityCopyOperationPanel (const QtGeomEntityCopyOperationPanel&)


QtGeomEntityCopyOperationPanel& QtGeomEntityCopyOperationPanel::operator = (
											const QtGeomEntityCopyOperationPanel&)
{
	MGX_FORBIDDEN ("QtGeomEntityCopyOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtGeomEntityCopyOperationPanel::QtGeomEntityCopyOperationPanel (const QtGeomEntityCopyOperationPanel&)


QtGeomEntityCopyOperationPanel::~QtGeomEntityCopyOperationPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtGeomEntityCopyOperationPanel::~QtGeomEntityCopyOperationPanel


string QtGeomEntityCopyOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtGeomEntityCopyOperationPanel::getGroupName


void QtGeomEntityCopyOperationPanel::setDimension (SelectionManagerIfc::DIM dim)
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_namePanel)
	if (dim != _geomEntitiesPanel->getDimensions ( ))
	{
		_geomEntitiesPanel->clearSelection ( );
		_geomEntitiesPanel->setDimensions (dim);
		int	dimension	= 3;
		switch (dim)
		{
			case	SelectionManagerIfc::D0	: dimension	= 0;	break;
			case	SelectionManagerIfc::D1	: dimension	= 1;	break;
			case	SelectionManagerIfc::D2	: dimension	= 2;	break;
			case	SelectionManagerIfc::D3	: dimension	= 3;	break;
		}	// switch (dim)
		_namePanel->setDimension (dimension);
	}	// if (dim != _geomEntitiesPanel->getDimensions ( ))
}	// QtGeomEntityCopyOperationPanel::setDimension


vector<string> QtGeomEntityCopyOperationPanel::getGeomEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	return _geomEntitiesPanel->getEntitiesNames ( );
}	// QtGeomEntityCopyOperationPanel::getGeomEntitiesNames


bool QtGeomEntityCopyOperationPanel::withTopology ( ) const
{
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)
	return Qt::Checked == _withTopologyCheckBox->checkState ( ) ? true : false;
}	// QtGeomEntityCopyOperationPanel::withTopology


void QtGeomEntityCopyOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	_namePanel->autoUpdate ( );
	_geomEntitiesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeomEntityCopyOperationPanel::reset


void QtGeomEntityCopyOperationPanel::validate ( )
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
		error << "QtGeomEntityCopyOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getGeomEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGeomEntityCopyOperationPanel::validate


void QtGeomEntityCopyOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	_geomEntitiesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_geomEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeomEntityCopyOperationPanel::cancel


void QtGeomEntityCopyOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		FilterEntity::objectType	filter	=
				(FilterEntity::objectType)(FilterEntity::GeomVertex |
					FilterEntity::GeomCurve | FilterEntity::GeomSurface |
					FilterEntity::GeomVolume);
		vector<string>	geomEntities	=
							getSelectionManager ( ).getEntitiesNames (filter);
		CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)	
		if (0 != geomEntities.size ( ))
			_geomEntitiesPanel->setEntitiesNames (geomEntities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_geomEntitiesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_namePanel->autoUpdate ( );
	_geomEntitiesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtGeomEntityCopyOperationPanel::autoUpdate


vector<Entity*> QtGeomEntityCopyOperationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	names	= getGeomEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*	entity	= getContext ( ).getGeomManager ( ).getEntity (*it);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}

	return entities;
}	// QtGeomEntityCopyOperationPanel::getInvolvedEntities


void QtGeomEntityCopyOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _geomEntitiesPanel)
		_geomEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeomEntityCopyOperationPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtGeomEntityCopyOperationAction
// ===========================================================================

QtGeomEntityCopyOperationAction::QtGeomEntityCopyOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeomEntityCopyOperationPanel*	operationPanel	=
		new QtGeomEntityCopyOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeomEntityCopyOperationAction::QtGeomEntityCopyOperationAction


QtGeomEntityCopyOperationAction::QtGeomEntityCopyOperationAction (
										const QtGeomEntityCopyOperationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeomEntityCopyOperationAction copy constructor is not allowed.")
}	// QtGeomEntityCopyOperationAction::QtGeomEntityCopyOperationAction


QtGeomEntityCopyOperationAction& QtGeomEntityCopyOperationAction::operator = (
										const QtGeomEntityCopyOperationAction&)
{
	MGX_FORBIDDEN ("QtGeomEntityCopyOperationAction assignment operator is not allowed.")
	return *this;
}	// QtGeomEntityCopyOperationAction::operator =


QtGeomEntityCopyOperationAction::~QtGeomEntityCopyOperationAction ( )
{
}	// QtGeomEntityCopyOperationAction::~QtGeomEntityCopyOperationAction


QtGeomEntityCopyOperationPanel*
					QtGeomEntityCopyOperationAction::getCopyPanel ( )
{
	return dynamic_cast<QtGeomEntityCopyOperationPanel*>(getOperationPanel ( ));
}	// QtGeomEntityCopyOperationAction::getCopyPanel


void QtGeomEntityCopyOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de copie des entités géométriques :
	QtGeomEntityCopyOperationPanel*	panel	= getCopyPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string		groupName	= panel->getGroupName ( );
	vector<string>		entities	= panel->getGeomEntitiesNames ( );
	const bool			withTopo	= panel->withTopology ( );

	cmdResult	= getContext ( ).getGeomManager ( ).copy (entities, withTopo, groupName);

	setCommandResult (cmdResult);
}	// QtGeomEntityCopyOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
