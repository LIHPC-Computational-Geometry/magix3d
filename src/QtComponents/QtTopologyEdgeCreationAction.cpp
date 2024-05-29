/**
 * \file		QtTopologyEdgeCreationAction.cpp
 * \author		Eric B
 * \date		15/3/2019
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyEdgeCreationAction.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>


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
//                        LA CLASSE QtTopologyEdgeCreationPanel
// ===========================================================================

QtTopologyEdgeCreationPanel::QtTopologyEdgeCreationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
: QtMgx3DOperationPanel (parent, mainWindow, action,
		QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).edgeCreationURL,
		QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).edgeCreationTag),
		_curvePanel (0)
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

	// Courbe à associer :
	_curvePanel	= new QtMgx3DEntityPanel (
						this, "", true, "Courbe associée :", "",
						&mainWindow, SelectionManagerIfc::D1,
						FilterEntity::GeomCurve);
	layout->addWidget (_curvePanel);
	connect (_curvePanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_curvePanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));


	layout->addStretch (2);

}	// QtTopologyEdgeCreationPanel::QtTopologyEdgeCreationPanel


QtTopologyEdgeCreationPanel::QtTopologyEdgeCreationPanel (
										const QtTopologyEdgeCreationPanel& cao)
: QtMgx3DOperationPanel (0, *new QtMgx3DMainWindow(0), 0, "", ""),
		_curvePanel (0)
{
	MGX_FORBIDDEN ("QtTopologyEdgeCreationPanel copy constructor is not allowed.");
}	// QtTopologyEdgeCreationPanel::QtTopologyEdgeCreationPanel (const QtTopologyEdgeCreationPanel&)


QtTopologyEdgeCreationPanel& QtTopologyEdgeCreationPanel::operator = (
											const QtTopologyEdgeCreationPanel&)
{
	MGX_FORBIDDEN ("QtTopologyEdgeCreationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyEdgeCreationPanel::QtTopologyEdgeCreationPanel (const QtTopologyEdgeCreationPanel&)


QtTopologyEdgeCreationPanel::~QtTopologyEdgeCreationPanel ( )
{
}	// QtTopologyEdgeCreationPanel::~QtTopologyEdgeCreationPanel


string QtTopologyEdgeCreationPanel::getCurveName ( ) const
{
	CHECK_NULL_PTR_ERROR (_curvePanel)
	return _curvePanel->getUniqueName ( );
}	// QtTopologyEdgeCreationPanel::getCurveName

void QtTopologyEdgeCreationPanel::reset ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_curvePanel)
		_curvePanel->reset ( );
	}
	catch (...)
	{
	}
}	// QtTopologyEdgeCreationPanel::reset

void QtTopologyEdgeCreationPanel::validate ( )
{
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
		error << "QtCurveByProjectionOnSurfacePanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopologyEdgeCreationPanel::validate

void QtTopologyEdgeCreationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_curvePanel)
	_curvePanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_curvePanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyEdgeCreationPanel::cancel


void QtTopologyEdgeCreationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_curvePanel)

	_curvePanel->setUniqueName ("");

	_curvePanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyEdgeCreationPanel::autoUpdate





// ===========================================================================
//                  LA CLASSE QtTopologyEdgeCreationAction
// ===========================================================================

QtTopologyEdgeCreationAction::QtTopologyEdgeCreationAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtTopologyCreationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyEdgeCreationPanel*	operationPanel	=
			new QtTopologyEdgeCreationPanel (
					&getOperationPanelParent ( ), text.toStdString ( ), 
					mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyEdgeCreationAction::QtTopologyEdgeCreationAction


QtTopologyEdgeCreationAction::QtTopologyEdgeCreationAction (
										const QtTopologyEdgeCreationAction&)
	: QtTopologyCreationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyEdgeCreationAction copy constructor is not allowed.")
}	// QtTopologyEdgeCreationAction::QtTopologyEdgeCreationAction


QtTopologyEdgeCreationAction& QtTopologyEdgeCreationAction::operator = (
										const QtTopologyEdgeCreationAction&)
{
	MGX_FORBIDDEN ("QtTopologyEdgeCreationAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyEdgeCreationAction::operator =


QtTopologyEdgeCreationAction::~QtTopologyEdgeCreationAction ( )
{
}	// QtTopologyEdgeCreationAction::~QtTopologyEdgeCreationAction


void QtTopologyEdgeCreationAction::executeOperation ( )
{
	QtTopologyEdgeCreationPanel*	panel	= dynamic_cast<QtTopologyEdgeCreationPanel*>(getOperationPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtTopologyCreationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités topologiques :
	const string	curve	= panel->getCurveName ( );

	cmdResult	= getContext ( ).getTopoManager( ).newTopoOnGeometry (curve);

	setCommandResult (cmdResult);
}	// QtTopologyFaceCreationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
