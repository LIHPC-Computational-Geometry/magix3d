/**
 * \file        QtGeometryGlueSurfacesAction.cpp
 * \author      Charles PIGNEROL
 * \date        01/12/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Geom/Surface.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtGeometryGlueSurfacesAction.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>


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
//                        LA CLASSE QtGeometryGlueSurfacesPanel
// ===========================================================================

QtGeometryGlueSurfacesPanel::QtGeometryGlueSurfacesPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).geomGlueSurfacesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).geomGlueSurfacesOperationTag),
	  _surfacesPanel (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);

	// Le nom du panneau :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Les surfaces à coller :
	_surfacesPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Surfaces :", "", &mainWindow,
			SelectionManagerIfc::D2, FilterEntity::GeomSurface);
	_surfacesPanel->setMultiSelectMode (true);
	connect (_surfacesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_surfacesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_surfacesPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_surfacesPanel->getNameTextField ( ))
	_surfacesPanel->getNameTextField ( )->setLinkedSeizureManagers (0, 0);
}	// QtGeometryGlueSurfacesPanel::QtGeometryGlueSurfacesPanel


QtGeometryGlueSurfacesPanel::QtGeometryGlueSurfacesPanel (
										const QtGeometryGlueSurfacesPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _surfacesPanel (0)
{
	MGX_FORBIDDEN ("QtGeometryGlueSurfacesPanel copy constructor is not allowed.");
}	// QtGeometryGlueSurfacesPanel::QtGeometryGlueSurfacesPanel (const QtGeometryGlueSurfacesPanel&)


QtGeometryGlueSurfacesPanel& QtGeometryGlueSurfacesPanel::operator = (
											const QtGeometryGlueSurfacesPanel&)
{
	MGX_FORBIDDEN ("QtGeometryGlueSurfacesPanel assignment operator is not allowed.");
	return *this;
}	// QtGeometryGlueSurfacesPanel::QtGeometryGlueSurfacesPanel (const QtGeometryGlueSurfacesPanel&)


QtGeometryGlueSurfacesPanel::~QtGeometryGlueSurfacesPanel ( )
{
}	// QtGeometryGlueSurfacesPanel::~QtGeometryGlueSurfacesPanel


void QtGeometryGlueSurfacesPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_surfacesPanel)
	_surfacesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeometryGlueSurfacesPanel::reset


void QtGeometryGlueSurfacesPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_surfacesPanel)

	_surfacesPanel->stopSelection ( );
	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_surfacesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeometryGlueSurfacesPanel::cancel


void QtGeometryGlueSurfacesPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_surfacesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedSurfaces	=
			getSelectionManager ( ).getEntitiesNames(FilterEntity::GeomSurface);
		if (0 != selectedSurfaces.size ( ))
			_surfacesPanel->setUniqueNames (selectedSurfaces);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_surfacesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_surfacesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtGeometryGlueSurfacesPanel::autoUpdate


vector<string> QtGeometryGlueSurfacesPanel::getSurfacesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_surfacesPanel)
	return _surfacesPanel->getUniqueNames ( );
}	// QtGeometryGlueSurfacesPanel::getSurfacesNames


vector<Entity*> QtGeometryGlueSurfacesPanel::getInvolvedEntities ( )
{
	vector<Entity*>			entities;
	const vector<string>	names	= getSurfacesNames ( );

	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*		surface	= 
			getContext ( ).getGeomManager ( ).getSurface (*it, false);
		if (0 != surface)
			entities.push_back (surface);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtGeometryGlueSurfacesPanel::getInvolvedEntities


void QtGeometryGlueSurfacesPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _surfacesPanel)
	{
		_surfacesPanel->stopSelection ( );
		_surfacesPanel->setUniqueName ("");
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeometryGlueSurfacesPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtGeometryGlueSurfacesAction
// ===========================================================================

QtGeometryGlueSurfacesAction::QtGeometryGlueSurfacesAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeometryGlueSurfacesPanel*	operationPanel	= 
			new QtGeometryGlueSurfacesPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeometryGlueSurfacesAction::QtGeometryGlueSurfacesAction


QtGeometryGlueSurfacesAction::QtGeometryGlueSurfacesAction (
										const QtGeometryGlueSurfacesAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeometryGlueSurfacesAction copy constructor is not allowed.")
}	// QtGeometryGlueSurfacesAction::QtGeometryGlueSurfacesAction


QtGeometryGlueSurfacesAction& QtGeometryGlueSurfacesAction::operator = (
										const QtGeometryGlueSurfacesAction&)
{
	MGX_FORBIDDEN ("QtGeometryGlueSurfacesAction assignment operator is not allowed.")
	return *this;
}	// QtGeometryGlueSurfacesAction::operator =


QtGeometryGlueSurfacesAction::~QtGeometryGlueSurfacesAction ( )
{
}	// QtGeometryGlueSurfacesAction::~QtGeometryGlueSurfacesAction


QtGeometryGlueSurfacesPanel*
					QtGeometryGlueSurfacesAction::getGeometryGlueSurfacesPanel ( )
{
	return dynamic_cast<QtGeometryGlueSurfacesPanel*>(getOperationPanel ( ));
}	// QtGeometryGlueSurfacesAction::getGeometryGlueSurfacesPanel


void QtGeometryGlueSurfacesAction::executeOperation ( )
{
	M3DCommandResultIfc*			cmdResult	= 0;
	QtGeometryGlueSurfacesPanel*	panel		= dynamic_cast<QtGeometryGlueSurfacesPanel*>(getGeometryGlueSurfacesPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de collage des surfaces géométriques :
	vector<string>	surfaces	= panel->getSurfacesNames ( );

	cmdResult	= getContext ( ).getGeomManager ( ).joinSurfaces (surfaces);
	setCommandResult (cmdResult);
}	// QtGeometryGlueSurfacesAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
