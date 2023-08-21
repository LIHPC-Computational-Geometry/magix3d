/**
 * \file        QtGeomAddToGroupOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        09/09/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeomAddToGroupOperationAction.h"
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
//                        LA CLASSE QtGeomAddToGroupOperationPanel
// ===========================================================================

QtGeomAddToGroupOperationPanel::QtGeomAddToGroupOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).geomAddToGroupOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).geomAddToGroupOperationTag),
	  _namePanel (0), _operationComboBox (0), _geomEntitiesPanel (0)
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

	// Opération à effectuer :
	QHBoxLayout*	hLayout	= new QHBoxLayout ( );
	layout->addLayout (hLayout);
	label	= new QLabel (QString::fromUtf8("Opération :"), this);
	hLayout->addWidget (label);
	_operationComboBox	= new QComboBox (this);
	hLayout->addWidget (_operationComboBox);
	_operationComboBox->addItem ("Ajouter");
	_operationComboBox->addItem ("Enlever");
	_operationComboBox->addItem ("Affecter");
	UTF8String	tip (Charset::UTF_8);
	tip << "\"Ajouter\" ajoute au groupe les entités sélectionnées. "
	    << "Le groupe est créé si nécessaire, et les entités demeurent "
	    << "éventuellement rattachées à d'autres groupes."
	    << "\n"
	    << "\"Enlever\" enlève du groupe les entités sélectionnées. "
	    << "\n"
	    << "\"Affecter\" ajoute au groupe les entités sélectionnées, et les "
	    << "enlève des groupes auxquelles elles sont rattachées.";
	label->setToolTip (UTF8TOQSTRING (tip));
	_operationComboBox->setToolTip (UTF8TOQSTRING (tip));
	hLayout->addStretch (2.);

	// Les entités à ajouter :
	FilterEntity::objectType	filter	=
		(FilterEntity::objectType)(FilterEntity::GeomVertex |
			FilterEntity::GeomCurve | FilterEntity::GeomSurface |
			FilterEntity::GeomVolume);
	_geomEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
							this, mainWindow, "Entités géométriques :", 
							SelectionManagerIfc::ALL_DIMENSIONS,
							filter, SelectionManagerIfc::D3, false);
	_geomEntitiesPanel->setMultiSelectMode (true);
	connect (_geomEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_geomEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_geomEntitiesPanel, SIGNAL (dimensionsModified ( )),
	         this, SLOT (dimensionsModifiedCallback ( )));
	layout->addWidget (_geomEntitiesPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
											)->setLinkedSeizureManagers (0, 0);
}	// QtGeomAddToGroupOperationPanel::QtGeomAddToGroupOperationPanel


QtGeomAddToGroupOperationPanel::QtGeomAddToGroupOperationPanel (
									const QtGeomAddToGroupOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _operationComboBox (0), _geomEntitiesPanel (0)
{
	MGX_FORBIDDEN ("QtGeomAddToGroupOperationPanel copy constructor is not allowed.");
}	// QtGeomAddToGroupOperationPanel::QtGeomAddToGroupOperationPanel (const QtGeomAddToGroupOperationPanel&)


QtGeomAddToGroupOperationPanel& QtGeomAddToGroupOperationPanel::operator = (
										const QtGeomAddToGroupOperationPanel&)
{
	MGX_FORBIDDEN ("QtGeomAddToGroupOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtGeomAddToGroupOperationPanel::QtGeomAddToGroupOperationPanel (const QtGeomAddToGroupOperationPanel&)


QtGeomAddToGroupOperationPanel::~QtGeomAddToGroupOperationPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtGeomAddToGroupOperationPanel::~QtGeomAddToGroupOperationPanel


string QtGeomAddToGroupOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtGeomAddToGroupOperationPanel::getGroupName


QtGeomAddToGroupOperationPanel::OPERATION
						QtGeomAddToGroupOperationPanel::getOperation ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationComboBox)
	return (QtGeomAddToGroupOperationPanel::OPERATION)
										_operationComboBox->currentIndex ( );
}	// QtGeomAddToGroupOperationPanel::getOperation


SelectionManagerIfc::DIM QtGeomAddToGroupOperationPanel::getDimension ( ) const
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	return _geomEntitiesPanel->getDimensions ( );
}	// QtGeomAddToGroupOperationPanel::getDimension


void QtGeomAddToGroupOperationPanel::setDimension (SelectionManagerIfc::DIM dim)
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
}	// QtGeomAddToGroupOperationPanel::setDimension


vector<string> QtGeomAddToGroupOperationPanel::getGeomEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	return _geomEntitiesPanel->getEntitiesNames ( );
}	// QtGeomAddToGroupOperationPanel::getGeomEntitiesNames


void QtGeomAddToGroupOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	_namePanel->autoUpdate ( );
	_geomEntitiesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeomAddToGroupOperationPanel::reset


void QtGeomAddToGroupOperationPanel::validate ( )
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
		error << "QtGeomAddToGroupOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getGeomEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGeomAddToGroupOperationPanel::validate


void QtGeomAddToGroupOperationPanel::cancel ( )
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
}	// QtGeomAddToGroupOperationPanel::cancel


void QtGeomAddToGroupOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		if (0 != _namePanel)
			_namePanel->autoUpdate ( );
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
	if (0 != _namePanel)
		_namePanel->autoUpdate ( );
	_geomEntitiesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_geomEntitiesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtGeomAddToGroupOperationPanel::autoUpdate


vector<Entity*> QtGeomAddToGroupOperationPanel::getInvolvedEntities ( )
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
}	// QtGeomAddToGroupOperationPanel::getInvolvedEntities


void QtGeomAddToGroupOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _geomEntitiesPanel)
		_geomEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );

	// Actualisation des groupes : certains peuvent être créés, d'autres
	// détruits (ex : "Hors Groupes 3D" :
	if (0 != _namePanel)
		_namePanel->autoUpdate ( );
}	// QtGeomAddToGroupOperationPanel::operationCompleted


void QtGeomAddToGroupOperationPanel::dimensionsModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _namePanel)
	{
		_namePanel->setDimension (
			SelectionManagerIfc::dimensionsToDimension (getDimension ( )));
		_namePanel->autoUpdate ( );
	}	// if (0 != _namePanel)
	
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeomAddToGroupOperationPanel::dimensionsModifiedCallback


// ===========================================================================
//                  LA CLASSE QtGeomAddToGroupOperationAction
// ===========================================================================

QtGeomAddToGroupOperationAction::QtGeomAddToGroupOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeomAddToGroupOperationPanel*	operationPanel	=
		new QtGeomAddToGroupOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeomAddToGroupOperationAction::QtGeomAddToGroupOperationAction


QtGeomAddToGroupOperationAction::QtGeomAddToGroupOperationAction (
										const QtGeomAddToGroupOperationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeomAddToGroupOperationAction copy constructor is not allowed.")
}	// QtGeomAddToGroupOperationAction::QtGeomAddToGroupOperationAction


QtGeomAddToGroupOperationAction& QtGeomAddToGroupOperationAction::operator = (
										const QtGeomAddToGroupOperationAction&)
{
	MGX_FORBIDDEN ("QtGeomAddToGroupOperationAction assignment operator is not allowed.")
	return *this;
}	// QtGeomAddToGroupOperationAction::operator =


QtGeomAddToGroupOperationAction::~QtGeomAddToGroupOperationAction ( )
{
}	// QtGeomAddToGroupOperationAction::~QtGeomAddToGroupOperationAction


QtGeomAddToGroupOperationPanel*
					QtGeomAddToGroupOperationAction::getAdditionPanel ( )
{
	return dynamic_cast<QtGeomAddToGroupOperationPanel*>(getOperationPanel ( ));
}	// QtGeomAddToGroupOperationAction::getAdditionPanel


void QtGeomAddToGroupOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres d'addition des entités géométriques :
	QtGeomAddToGroupOperationPanel*	panel	= getAdditionPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string		groupName	= panel->getGroupName ( );
	vector<string>		entities	= panel->getGeomEntitiesNames ( );
	const int			dim			= SelectionManagerIfc::dimensionsToDimension (panel->getDimension ( ));
	const QtGeomAddToGroupOperationPanel::OPERATION	operation	= panel->getOperation ( );

	switch (operation)
	{
		case QtGeomAddToGroupOperationPanel::ADD	:
			cmdResult	= getContext ( ).getGeomManager ( ).addToGroup (entities, dim, groupName);
			break;
		case QtGeomAddToGroupOperationPanel::REMOVE	:
			cmdResult	= getContext ( ).getGeomManager ( ).removeFromGroup (entities, dim, groupName);
			break;
		case QtGeomAddToGroupOperationPanel::SET	:
			cmdResult	= getContext ( ).getGeomManager ( ).setGroup (entities, dim, groupName);
			break;
		default										:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Opération non suppportée : " << (unsigned long)operation << ".";
			INTERNAL_ERROR (exc, message, "QtGeomAddToGroupOperationAction::executeOperation")
			throw exc;
		}
	}	// switch (operation)

	setCommandResult (cmdResult);
}	// QtGeomAddToGroupOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
