/**
 * \file        QtTopologyFaceCreationAction.cpp
 * \author      Charles PIGNEROL
 * \date        20/12/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyFaceCreationAction.h"

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
//                        LA CLASSE QtTopologyFaceCreationPanel
// ===========================================================================

QtTopologyFaceCreationPanel::QtTopologyFaceCreationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtTopologyCreationPanel (
				parent, panelName, mainWindow, action, SelectionManagerIfc::D2,
				QtTopologyPanel::STRUCTURED_TOPOLOGY,
				QtTopologyPanel::NO_COORDINATES,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).faceCreationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).faceCreationTag)
{
	getTopologyPanel ( ).enableTopologyType (
										QtTopologyPanel::OGRID_BLOCKS, false);
	getTopologyPanel ( ).enableTopologyType (
							QtTopologyPanel::INSERTION_TOPOLOGY, false);
}	// QtTopologyFaceCreationPanel::QtTopologyFaceCreationPanel


QtTopologyFaceCreationPanel::QtTopologyFaceCreationPanel (
										const QtTopologyFaceCreationPanel& cao)
	: QtTopologyCreationPanel (
			0, "Invalid panel", *new QtMgx3DMainWindow(0),
			0, SelectionManagerIfc::D0,
			QtTopologyPanel::STRUCTURED_TOPOLOGY,
			QtTopologyPanel::NO_COORDINATES,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).faceCreationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).faceCreationTag)
{
	MGX_FORBIDDEN ("QtTopologyFaceCreationPanel copy constructor is not allowed.");
}	// QtTopologyFaceCreationPanel::QtTopologyFaceCreationPanel (const QtTopologyFaceCreationPanel&)


QtTopologyFaceCreationPanel& QtTopologyFaceCreationPanel::operator = (
											const QtTopologyFaceCreationPanel&)
{
	MGX_FORBIDDEN ("QtTopologyFaceCreationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyFaceCreationPanel::QtTopologyFaceCreationPanel (const QtTopologyFaceCreationPanel&)


QtTopologyFaceCreationPanel::~QtTopologyFaceCreationPanel ( )
{
}	// QtTopologyFaceCreationPanel::~QtTopologyFaceCreationPanel


// ===========================================================================
//                  LA CLASSE QtTopologyFaceCreationAction
// ===========================================================================

QtTopologyFaceCreationAction::QtTopologyFaceCreationAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtTopologyCreationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyCreationPanel*	operationPanel	= 
			new QtTopologyFaceCreationPanel (
					&getOperationPanelParent ( ), text.toStdString ( ), 
					mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyFaceCreationAction::QtTopologyFaceCreationAction


QtTopologyFaceCreationAction::QtTopologyFaceCreationAction (
										const QtTopologyFaceCreationAction&)
	: QtTopologyCreationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyFaceCreationAction copy constructor is not allowed.")
}	// QtTopologyFaceCreationAction::QtTopologyFaceCreationAction


QtTopologyFaceCreationAction& QtTopologyFaceCreationAction::operator = (
										const QtTopologyFaceCreationAction&)
{
	MGX_FORBIDDEN ("QtTopologyFaceCreationAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyFaceCreationAction::operator =


QtTopologyFaceCreationAction::~QtTopologyFaceCreationAction ( )
{
}	// QtTopologyFaceCreationAction::~QtTopologyFaceCreationAction


void QtTopologyFaceCreationAction::executeOperation ( )
{
	QtTopologyFaceCreationPanel*	panel	= dynamic_cast<QtTopologyFaceCreationPanel*>(getTopologyCreationPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtTopologyCreationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités topologiques :
	string							name	    = panel->getGeomEntityName ( );
	string							groupName	= panel->getGroupName ( );
	QtTopologyPanel::TOPOLOGY_TYPE	type	    = panel->getTopologyType ( );

	switch (type)
	{
		case QtTopologyPanel::UNSTRUCTURED_TOPOLOGY	:
			cmdResult	= getContext ( ).getTopoManager ( ).newUnstructuredTopoOnGeometry (name);
			break;
		case QtTopologyPanel::STRUCTURED_TOPOLOGY	:
			cmdResult	= getContext ( ).getTopoManager ( ).newStructuredTopoOnGeometry (name);
			break;
		case QtTopologyPanel::STRUCTURED_FREE_TOPOLOGY	:
			if (0 != name.length ( ))
				cmdResult	= getContext ( ).getTopoManager ( ).newFreeTopoOnGeometry (name);
			else
			{
				if (0 == groupName.length ( ))
				{
					UTF8String	warning (Charset::UTF_8);
					warning << "Vous n'avez pas choisi de nom d'entité géométrique ou ou de groupe. Souhaitez-vous continuer la création de cette face topologique ?";
					if (0 != QMessageBox::warning (0, "Magix 3D", UTF8TOQSTRING (warning), "Oui", "Non", "", 0, 2))
						return;
				}	// if (0 == groupName.length ( ))
				cmdResult	= getContext ( ).getTopoManager( ).newFreeTopoInGroup (groupName, 2);
			}
			break;
		default											:
		{
			INTERNAL_ERROR (exc, "Type de topologie non supporté.", "QtTopologyFaceCreationAction::executeOperation")
			throw exc;
		}
	}	// switch (type)

	setCommandResult (cmdResult);
}	// QtTopologyFaceCreationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
