/**
 * \file        QtTopologyBlockCreationAction.cpp
 * \author      Charles PIGNEROL
 * \date        20/12/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyBlockCreationAction.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <QMessageBox>


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
//                        LA CLASSE QtTopologyBlockCreationPanel
// ===========================================================================

QtTopologyBlockCreationPanel::QtTopologyBlockCreationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtTopologyCreationPanel (
				parent, panelName, mainWindow, action, SelectionManagerIfc::D3,
				QtTopologyPanel::STRUCTURED_TOPOLOGY,
				QtTopologyPanel::NO_COORDINATES,
				QtMgx3DApplication::HelpSystem::instance ( ).blockCreationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).blockCreationTag)
{
}	// QtTopologyBlockCreationPanel::QtTopologyBlockCreationPanel


QtTopologyBlockCreationPanel::QtTopologyBlockCreationPanel (
										const QtTopologyBlockCreationPanel& cao)
	: QtTopologyCreationPanel (
			0, "Invalid panel", *new QtMgx3DMainWindow(0),
			0, SelectionManagerIfc::D0,
			QtTopologyPanel::STRUCTURED_TOPOLOGY,
			QtTopologyPanel::NO_COORDINATES,
			QtMgx3DApplication::HelpSystem::instance ( ).blockCreationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).blockCreationTag)
{
	MGX_FORBIDDEN ("QtTopologyBlockCreationPanel copy constructor is not allowed.");
}	// QtTopologyBlockCreationPanel::QtTopologyBlockCreationPanel (const QtTopologyBlockCreationPanel&)


QtTopologyBlockCreationPanel& QtTopologyBlockCreationPanel::operator = (
											const QtTopologyBlockCreationPanel&)
{
	MGX_FORBIDDEN ("QtTopologyBlockCreationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyBlockCreationPanel::QtTopologyBlockCreationPanel (const QtTopologyBlockCreationPanel&)


QtTopologyBlockCreationPanel::~QtTopologyBlockCreationPanel ( )
{
}	// QtTopologyBlockCreationPanel::~QtTopologyBlockCreationPanel


void QtTopologyBlockCreationPanel::cancel ( )
{
	QtTopologyCreationPanel::cancel ( );
}	// QtTopologyBlockCreationPanel::cancel


void QtTopologyBlockCreationPanel::autoUpdate ( )
{
	QtTopologyCreationPanel::autoUpdate ( );
}	// QtTopologyBlockCreationPanel::autoUpdate


// ===========================================================================
//                  LA CLASSE QtTopologyBlockCreationAction
// ===========================================================================

QtTopologyBlockCreationAction::QtTopologyBlockCreationAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtTopologyCreationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyCreationPanel*	operationPanel	= 
			new QtTopologyBlockCreationPanel (
					&getOperationPanelParent ( ), text.toStdString ( ), 
					mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyBlockCreationAction::QtTopologyBlockCreationAction


QtTopologyBlockCreationAction::QtTopologyBlockCreationAction (
										const QtTopologyBlockCreationAction&)
	: QtTopologyCreationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyBlockCreationAction copy constructor is not allowed.")
}	// QtTopologyBlockCreationAction::QtTopologyBlockCreationAction


QtTopologyBlockCreationAction& QtTopologyBlockCreationAction::operator = (
										const QtTopologyBlockCreationAction&)
{
	MGX_FORBIDDEN ("QtTopologyBlockCreationAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyBlockCreationAction::operator =


QtTopologyBlockCreationAction::~QtTopologyBlockCreationAction ( )
{
}	// QtTopologyBlockCreationAction::~QtTopologyBlockCreationAction


void QtTopologyBlockCreationAction::executeOperation ( )
{
	QtTopologyBlockCreationPanel*	panel	= dynamic_cast<QtTopologyBlockCreationPanel*>(getTopologyCreationPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtTopologyCreationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités topologiques :
	string							name		= panel->getGeomEntityName ( );
	string							groupName	= panel->getGroupName ( );
	QtTopologyPanel::TOPOLOGY_TYPE	type		= panel->getTopologyType ( );

	switch (type)
	{
		case QtTopologyPanel::UNSTRUCTURED_TOPOLOGY	:
			cmdResult	- getContext ( ).getTopoManager ( ).newUnstructuredTopoOnGeometry (name);
			break;
		case QtTopologyPanel::STRUCTURED_TOPOLOGY	:
			cmdResult	= getContext ( ).getTopoManager ( ).newStructuredTopoOnGeometry (name);
			break;
		case QtTopologyPanel::STRUCTURED_FREE_TOPOLOGY	:
			if ((0 != name.length ( )) && (0 != groupName.length ( )))
				throw Exception (UTF8String ("Choisir un nom d'entité géométrique ou un nom de groupe.", Charset::UTF_8));
			if (0 != name.length ( ))
				cmdResult	= getContext ( ).getTopoManager( ).newFreeTopoOnGeometry (name);
			else
			{
				if (0 == groupName.length ( ))
				{
					UTF8String	warning (Charset::UTF_8);
					warning << "Vous n'avez pas choisi de nom d'entité géométrique ou ou de groupe. Souhaitez-vous continuer la création de ce bloc topologique ?";
					if (0 != QMessageBox::warning (0, "Magix 3D", UTF8TOQSTRING (warning), "Oui", "Non", "", 0, 2))
						return;
				}	// if (0 == groupName.length ( ))
				cmdResult	= getContext ( ).getTopoManager( ).newFreeTopoInGroup (groupName, 3);
			}
			break;
		case QtTopologyPanel::OGRID_BLOCKS				:
			cmdResult	= getContext ( ).getTopoManager ( ).newTopoOGridOnGeometry (name, panel->getOGridRatio ( ));
			break;
		case QtTopologyPanel::INSERTION_TOPOLOGY	:
			cmdResult	= getContext ( ).getTopoManager ( ).newInsertionTopoOnGeometry (name);
			break;
		default											:
		{
			INTERNAL_ERROR (exc, "Type de topologie non supporté.", "QtTopologyBlockCreationAction::executeOperation")
			throw exc;
		}
	}	// switch (type)

	setCommandResult (cmdResult);
}	// QtTopologyBlockCreationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
