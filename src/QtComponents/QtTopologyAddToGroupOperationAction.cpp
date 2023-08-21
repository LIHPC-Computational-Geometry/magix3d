/**
 * \file        QtTopologyAddToGroupOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        11/05/2015
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtTopologyAddToGroupOperationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <QLabel>
#include <QBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologyAddToGroupOperationPanel
// ===========================================================================

QtTopologyAddToGroupOperationPanel::QtTopologyAddToGroupOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).topoAddToGroupOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).topoAddToGroupOperationTag),
	  _namePanel (0), _operationComboBox (0), _topoEntitiesPanel (0)
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
		(FilterEntity::objectType)(FilterEntity::TopoVertex |
			FilterEntity::TopoCoEdge | FilterEntity::TopoCoFace |
			FilterEntity::TopoBlock);
	_topoEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
							this, mainWindow, "Entités topologiques :",
							SelectionManagerIfc::ALL_DIMENSIONS,
							filter, SelectionManagerIfc::D3, false);
	_topoEntitiesPanel->setMultiSelectMode (true);
	connect (_topoEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_topoEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_topoEntitiesPanel, SIGNAL (dimensionsModified ( )),
	         this, SLOT (dimensionsModifiedCallback ( )));
	layout->addWidget (_topoEntitiesPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
											)->setLinkedSeizureManagers (0, 0);
}	// QtTopologyAddToGroupOperationPanel::QtTopologyAddToGroupOperationPanel


QtTopologyAddToGroupOperationPanel::QtTopologyAddToGroupOperationPanel (
									const QtTopologyAddToGroupOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _operationComboBox (0), _topoEntitiesPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyAddToGroupOperationPanel copy constructor is not allowed.");
}	// QtTopologyAddToGroupOperationPanel::QtTopologyAddToGroupOperationPanel (const QtTopologyAddToGroupOperationPanel&)


QtTopologyAddToGroupOperationPanel& QtTopologyAddToGroupOperationPanel::operator = (
										const QtTopologyAddToGroupOperationPanel&)
{
	MGX_FORBIDDEN ("QtTopologyAddToGroupOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyAddToGroupOperationPanel::QtTopologyAddToGroupOperationPanel (const QtTopologyAddToGroupOperationPanel&)


QtTopologyAddToGroupOperationPanel::~QtTopologyAddToGroupOperationPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtTopologyAddToGroupOperationPanel::~QtTopologyAddToGroupOperationPanel


string QtTopologyAddToGroupOperationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtTopologyAddToGroupOperationPanel::getGroupName


QtTopologyAddToGroupOperationPanel::OPERATION
						QtTopologyAddToGroupOperationPanel::getOperation ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationComboBox)
	return (QtTopologyAddToGroupOperationPanel::OPERATION)
										_operationComboBox->currentIndex ( );
}	// QtTopologyAddToGroupOperationPanel::getOperation


SelectionManagerIfc::DIM QtTopologyAddToGroupOperationPanel::getDimension ( ) const
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	return _topoEntitiesPanel->getDimensions ( );
}	// QtTopologyAddToGroupOperationPanel::getDimension


void QtTopologyAddToGroupOperationPanel::setDimension (SelectionManagerIfc::DIM dim)
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_namePanel)
	if (dim != _topoEntitiesPanel->getDimensions ( ))
	{
		_topoEntitiesPanel->clearSelection ( );
		_topoEntitiesPanel->setDimensions (dim);
		int	dimension	= 3;
		switch (dim)
		{
			case	SelectionManagerIfc::D0	: dimension	= 0;	break;
			case	SelectionManagerIfc::D1	: dimension	= 1;	break;
			case	SelectionManagerIfc::D2	: dimension	= 2;	break;
			case	SelectionManagerIfc::D3	: dimension	= 3;	break;
		}	// switch (dim)
		_namePanel->setDimension (dimension);
	}	// if (dim != _topoEntitiesPanel->getDimensions ( ))
}	// QtTopologyAddToGroupOperationPanel::setDimension


vector<string> QtTopologyAddToGroupOperationPanel::getTopoEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	return _topoEntitiesPanel->getEntitiesNames ( );
}	// QtTopologyAddToGroupOperationPanel::getTopoEntitiesNames


void QtTopologyAddToGroupOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	_topoEntitiesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyAddToGroupOperationPanel::reset


void QtTopologyAddToGroupOperationPanel::validate ( )
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
		error << "QtTopologyAddToGroupOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getTopoEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopologyAddToGroupOperationPanel::validate


void QtTopologyAddToGroupOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	_topoEntitiesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_topoEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyAddToGroupOperationPanel::cancel


void QtTopologyAddToGroupOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		if (0 != _namePanel)
			_namePanel->autoUpdate ( );
		FilterEntity::objectType	filter	=
				(FilterEntity::objectType)(FilterEntity::TopoVertex |
					FilterEntity::TopoCoEdge | FilterEntity::TopoCoFace |
					FilterEntity::TopoBlock);
		vector<string>	geomEntities	=
							getSelectionManager ( ).getEntitiesNames (filter);
		CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)	
		if (0 != geomEntities.size ( ))
			_topoEntitiesPanel->setEntitiesNames (geomEntities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	if (0 != _namePanel)
		_namePanel->autoUpdate ( );
	_topoEntitiesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_topoEntitiesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyAddToGroupOperationPanel::autoUpdate


vector<Entity*> QtTopologyAddToGroupOperationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	names	= getTopoEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*	entity	= getContext ( ).getTopoManager ( ).getEntity (*it);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}

	return entities;
}	// QtTopologyAddToGroupOperationPanel::getInvolvedEntities


void QtTopologyAddToGroupOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _topoEntitiesPanel)
		_topoEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );

	// Actualisation des groupes : certains peuvent être créés, d'autres
	// détruits (ex : "Hors Groupes 3D" :
	if (0 != _namePanel)
		_namePanel->autoUpdate ( );
}	// QtTopologyAddToGroupOperationPanel::operationCompleted


void QtTopologyAddToGroupOperationPanel::dimensionsModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _namePanel)
	{
		_namePanel->setDimension (
			SelectionManagerIfc::dimensionsToDimension (getDimension ( )));
		_namePanel->autoUpdate ( );
	}	// if (0 != _namePanel)
	
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologyAddToGroupOperationPanel::dimensionsModifiedCallback


// ===========================================================================
//                  LA CLASSE QtTopologyAddToGroupOperationAction
// ===========================================================================

QtTopologyAddToGroupOperationAction::QtTopologyAddToGroupOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyAddToGroupOperationPanel*	operationPanel	=
		new QtTopologyAddToGroupOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyAddToGroupOperationAction::QtTopologyAddToGroupOperationAction


QtTopologyAddToGroupOperationAction::QtTopologyAddToGroupOperationAction (
										const QtTopologyAddToGroupOperationAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyAddToGroupOperationAction copy constructor is not allowed.")
}	// QtTopologyAddToGroupOperationAction::QtTopologyAddToGroupOperationAction


QtTopologyAddToGroupOperationAction& QtTopologyAddToGroupOperationAction::operator = (
										const QtTopologyAddToGroupOperationAction&)
{
	MGX_FORBIDDEN ("QtTopologyAddToGroupOperationAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyAddToGroupOperationAction::operator =


QtTopologyAddToGroupOperationAction::~QtTopologyAddToGroupOperationAction ( )
{
}	// QtTopologyAddToGroupOperationAction::~QtTopologyAddToGroupOperationAction


QtTopologyAddToGroupOperationPanel*
					QtTopologyAddToGroupOperationAction::getAdditionPanel ( )
{
	return dynamic_cast<QtTopologyAddToGroupOperationPanel*>(getOperationPanel ( ));
}	// QtTopologyAddToGroupOperationAction::getAdditionPanel


void QtTopologyAddToGroupOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres d'addition des entités topologiques :
	QtTopologyAddToGroupOperationPanel*	panel	= getAdditionPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string		groupName	= panel->getGroupName ( );
	vector<string>		entities	= panel->getTopoEntitiesNames ( );
	const int			dim			= SelectionManagerIfc::dimensionsToDimension (panel->getDimension ( ));
	const QtTopologyAddToGroupOperationPanel::OPERATION operation	= panel->getOperation ( );

	switch (operation)
	{
		case QtTopologyAddToGroupOperationPanel::ADD	:
			cmdResult	= getContext ( ).getTopoManager ( ).addToGroup (entities, dim, groupName);
			break;
		case QtTopologyAddToGroupOperationPanel::REMOVE	:
			cmdResult	= getContext ( ).getTopoManager ( ).removeFromGroup (entities, dim, groupName);
			break;
		case QtTopologyAddToGroupOperationPanel::SET	:
			cmdResult	= getContext ( ).getTopoManager ( ).setGroup (entities, dim, groupName);
			break;
		default										:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Opération non suppportée : " << (unsigned long)operation
			        << ".";
			INTERNAL_ERROR (exc, message, "QtTopologyAddToGroupOperationAction::executeOperation")
			throw exc;
		}
	}	// switch (operation)

	setCommandResult (cmdResult);
}	// QtTopologyAddToGroupOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
