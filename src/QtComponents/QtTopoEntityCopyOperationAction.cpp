/**
 * \file        QtTopoEntityCopyOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        25/06/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtTopoEntityCopyOperationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"

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
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopoEntityCopyOperationPanel
// ===========================================================================

QtTopoEntityCopyOperationPanel::QtTopoEntityCopyOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).topoCopyOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).topoCopyOperationTag),
	  _topoEntitiesPanel (0), _geomEntityPanel (0)
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

	// Les entités à copier :
	// Dans la version actuelle ce ne peut être que des blocs topologiques.
	_topoEntitiesPanel	= new QtMgx3DEntityPanel (
							this, "", true, "Blocs à copier :", "",
							&mainWindow, SelectionManagerIfc::D3,
							FilterEntity::TopoBlock);
	_topoEntitiesPanel->setMultiSelectMode (true);
	connect (_topoEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_topoEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_topoEntitiesPanel);


	// Le volume géométrique auquel on associe les blocs copiés :
	_geomEntityPanel	= new QtMgx3DEntityPanel (
							this, "", true, "Volume à associer :", "",
							&mainWindow, SelectionManagerIfc::D3,
							FilterEntity::GeomVolume);
	connect (_geomEntityPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_geomEntityPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_geomEntityPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_geomEntityPanel->getNameTextField ( ))
	_topoEntitiesPanel->getNameTextField ( )->setLinkedSeizureManagers (
								0, _geomEntityPanel->getNameTextField ( ));
	_geomEntityPanel->getNameTextField ( )->setLinkedSeizureManagers (
								_topoEntitiesPanel->getNameTextField ( ), 0);
}	// QtTopoEntityCopyOperationPanel::QtTopoEntityCopyOperationPanel


QtTopoEntityCopyOperationPanel::QtTopoEntityCopyOperationPanel (
										const QtTopoEntityCopyOperationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _topoEntitiesPanel (0), _geomEntityPanel (0)
{
	MGX_FORBIDDEN ("QtTopoEntityCopyOperationPanel copy constructor is not allowed.");
}	// QtTopoEntityCopyOperationPanel::QtTopoEntityCopyOperationPanel (const QtTopoEntityCopyOperationPanel&)


QtTopoEntityCopyOperationPanel& QtTopoEntityCopyOperationPanel::operator = (
											const QtTopoEntityCopyOperationPanel&)
{
	MGX_FORBIDDEN ("QtTopoEntityCopyOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopoEntityCopyOperationPanel::QtTopoEntityCopyOperationPanel (const QtTopoEntityCopyOperationPanel&)


QtTopoEntityCopyOperationPanel::~QtTopoEntityCopyOperationPanel ( )
{
	preview (false, true);
}	// QtTopoEntityCopyOperationPanel::~QtTopoEntityCopyOperationPanel


vector<string> QtTopoEntityCopyOperationPanel::getTopoEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	return _topoEntitiesPanel->getUniqueNames ( );
}	// QtTopoEntityCopyOperationPanel::getTopoEntitiesNames


string QtTopoEntityCopyOperationPanel::getGeomEntityName ( ) const
{
	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
	return _geomEntityPanel->getUniqueName ( );
}	// QtTopoEntityCopyOperationPanel::getGeomEntityName


void QtTopoEntityCopyOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
	_topoEntitiesPanel->reset ( );
	_geomEntityPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopoEntityCopyOperationPanel::reset


void QtTopoEntityCopyOperationPanel::validate ( )
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
		error << "QtTopoEntityCopyOperationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getTopoEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopoEntityCopyOperationPanel::validate


void QtTopoEntityCopyOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
	_topoEntitiesPanel->stopSelection ( );
	_geomEntityPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_topoEntitiesPanel->setUniqueName ("");
		_geomEntityPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopoEntityCopyOperationPanel::cancel


void QtTopoEntityCopyOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		FilterEntity::objectType	filter	= FilterEntity::TopoBlock;
		vector<string>	entities	=
							getSelectionManager ( ).getEntitiesNames (filter);
		CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
		_topoEntitiesPanel->setUniqueNames (entities);
		filter	= FilterEntity::GeomVolume;
		entities	= getSelectionManager ( ).getEntitiesNames (filter);
		CHECK_NULL_PTR_ERROR (_geomEntityPanel)
		if (1 == entities.size ( ))
			_geomEntityPanel->setUniqueName (entities [0]);
		else
			_geomEntityPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_topoEntitiesPanel->clearSelection ( );
	_geomEntityPanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	_topoEntitiesPanel->actualizeGui (true);
	_geomEntityPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopoEntityCopyOperationPanel::autoUpdate


vector<Entity*> QtTopoEntityCopyOperationPanel::getInvolvedEntities ( )
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
	const string	name	= getGeomEntityName ( );
	if (false == name.empty ( ))
	{
		GeomEntity*	entity	= getContext ( ).getGeomManager ( ).getEntity(name);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}

	return entities;
}	// QtTopoEntityCopyOperationPanel::getInvolvedEntities


void QtTopoEntityCopyOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _topoEntitiesPanel)
		_topoEntitiesPanel->stopSelection ( );
	if (0 != _geomEntityPanel)
		_geomEntityPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopoEntityCopyOperationPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopoEntityCopyOperationAction
// ===========================================================================

QtTopoEntityCopyOperationAction::QtTopoEntityCopyOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopoEntityCopyOperationPanel*	operationPanel	=
		new QtTopoEntityCopyOperationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopoEntityCopyOperationAction::QtTopoEntityCopyOperationAction


QtTopoEntityCopyOperationAction::QtTopoEntityCopyOperationAction (
										const QtTopoEntityCopyOperationAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopoEntityCopyOperationAction copy constructor is not allowed.")
}	// QtTopoEntityCopyOperationAction::QtTopoEntityCopyOperationAction


QtTopoEntityCopyOperationAction& QtTopoEntityCopyOperationAction::operator = (
										const QtTopoEntityCopyOperationAction&)
{
	MGX_FORBIDDEN ("QtTopoEntityCopyOperationAction assignment operator is not allowed.")
	return *this;
}	// QtTopoEntityCopyOperationAction::operator =


QtTopoEntityCopyOperationAction::~QtTopoEntityCopyOperationAction ( )
{
}	// QtTopoEntityCopyOperationAction::~QtTopoEntityCopyOperationAction


QtTopoEntityCopyOperationPanel*
					QtTopoEntityCopyOperationAction::getCopyPanel ( )
{
	return dynamic_cast<QtTopoEntityCopyOperationPanel*>(getOperationPanel ( ));
}	// QtTopoEntityCopyOperationAction::getCopyPanel


void QtTopoEntityCopyOperationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres de copie des entités topologiques :
	QtTopoEntityCopyOperationPanel*	panel	= getCopyPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>		entities	= panel->getTopoEntitiesNames ( );
	string				geomName	= panel->getGeomEntityName ( );

	cmdResult	= getContext ( ).getTopoManager ( ).copy (entities, geomName);
	
	setCommandResult (cmdResult);
}	// QtTopoEntityCopyOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
