/**
 * \file        QtGeometryGlueCurvesAction.cpp
 * \author      Charles PIGNEROL
 * \date        20/11/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Geom/Curve.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtGeometryGlueCurvesAction.h"

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
//                        LA CLASSE QtGeometryGlueCurvesPanel
// ===========================================================================

QtGeometryGlueCurvesPanel::QtGeometryGlueCurvesPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).geomGlueCurvesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).geomGlueCurvesOperationTag),
	  _curvesPanel (0)
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

	// Les courbes à coller :
	_curvesPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Courbes :", "", &mainWindow,
			SelectionManagerIfc::D1, FilterEntity::GeomCurve);
	_curvesPanel->setMultiSelectMode (true);
	connect (_curvesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_curvesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_curvesPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_curvesPanel->getNameTextField ( ))
	_curvesPanel->getNameTextField ( )->setLinkedSeizureManagers (0, 0);
}	// QtGeometryGlueCurvesPanel::QtGeometryGlueCurvesPanel


QtGeometryGlueCurvesPanel::QtGeometryGlueCurvesPanel (
										const QtGeometryGlueCurvesPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _curvesPanel (0)
{
	MGX_FORBIDDEN ("QtGeometryGlueCurvesPanel copy constructor is not allowed.");
}	// QtGeometryGlueCurvesPanel::QtGeometryGlueCurvesPanel (const QtGeometryGlueCurvesPanel&)


QtGeometryGlueCurvesPanel& QtGeometryGlueCurvesPanel::operator = (
											const QtGeometryGlueCurvesPanel&)
{
	MGX_FORBIDDEN ("QtGeometryGlueCurvesPanel assignment operator is not allowed.");
	return *this;
}	// QtGeometryGlueCurvesPanel::QtGeometryGlueCurvesPanel (const QtGeometryGlueCurvesPanel&)


QtGeometryGlueCurvesPanel::~QtGeometryGlueCurvesPanel ( )
{
}	// QtGeometryGlueCurvesPanel::~QtGeometryGlueCurvesPanel


void QtGeometryGlueCurvesPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_curvesPanel)
	_curvesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeometryGlueCurvesPanel::reset


void QtGeometryGlueCurvesPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_curvesPanel)

	_curvesPanel->stopSelection ( );
	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_curvesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeometryGlueCurvesPanel::cancel


void QtGeometryGlueCurvesPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_curvesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedCurves	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomCurve);
		if (0 != selectedCurves.size ( ))
			_curvesPanel->setUniqueNames (selectedCurves);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_curvesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_curvesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtGeometryGlueCurvesPanel::autoUpdate


vector<string> QtGeometryGlueCurvesPanel::getCurvesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_curvesPanel)
	return _curvesPanel->getUniqueNames ( );
}	// QtGeometryGlueCurvesPanel::getCurvesNames


vector<Entity*> QtGeometryGlueCurvesPanel::getInvolvedEntities ( )
{
	vector<Entity*>			entities;
	const vector<string>	names	= getCurvesNames ( );

	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*		curve	= 
			getContext ( ).getGeomManager ( ).getCurve (*it, false);
		if (0 != curve)
			entities.push_back (curve);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtGeometryGlueCurvesPanel::getInvolvedEntities


void QtGeometryGlueCurvesPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _curvesPanel)
	{
		_curvesPanel->stopSelection ( );
		_curvesPanel->setUniqueName ("");
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeometryGlueCurvesPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtGeometryGlueCurvesAction
// ===========================================================================

QtGeometryGlueCurvesAction::QtGeometryGlueCurvesAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeometryGlueCurvesPanel*	operationPanel	= 
			new QtGeometryGlueCurvesPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeometryGlueCurvesAction::QtGeometryGlueCurvesAction


QtGeometryGlueCurvesAction::QtGeometryGlueCurvesAction (
										const QtGeometryGlueCurvesAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeometryGlueCurvesAction copy constructor is not allowed.")
}	// QtGeometryGlueCurvesAction::QtGeometryGlueCurvesAction


QtGeometryGlueCurvesAction& QtGeometryGlueCurvesAction::operator = (
										const QtGeometryGlueCurvesAction&)
{
	MGX_FORBIDDEN ("QtGeometryGlueCurvesAction assignment operator is not allowed.")
	return *this;
}	// QtGeometryGlueCurvesAction::operator =


QtGeometryGlueCurvesAction::~QtGeometryGlueCurvesAction ( )
{
}	// QtGeometryGlueCurvesAction::~QtGeometryGlueCurvesAction


QtGeometryGlueCurvesPanel*
					QtGeometryGlueCurvesAction::getGeometryGlueCurvesPanel ( )
{
	return dynamic_cast<QtGeometryGlueCurvesPanel*>(getOperationPanel ( ));
}	// QtGeometryGlueCurvesAction::getGeometryGlueCurvesPanel


void QtGeometryGlueCurvesAction::executeOperation ( )
{
	M3DCommandResultIfc*		cmdResult	= 0;
	QtGeometryGlueCurvesPanel*	panel	=
			dynamic_cast<QtGeometryGlueCurvesPanel*>(getGeometryGlueCurvesPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de collage des courbes géométriques :
	vector<string>	curves	= panel->getCurvesNames ( );

	cmdResult	= getContext ( ).getGeomManager ( ).joinCurves (curves);

	setCommandResult (cmdResult);
}	// QtGeometryGlueCurvesAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
