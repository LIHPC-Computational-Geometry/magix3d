/**
 * \file		QtTopologyVertexCreationAction.cpp
 * \author		Eric B
 * \date		15/3/2019
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyVertexCreationAction.h"

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
//                        LA CLASSE QtTopologyVertexCreationPanel
// ===========================================================================

QtTopologyVertexCreationPanel::QtTopologyVertexCreationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
: QtMgx3DOperationPanel (parent, mainWindow, action,
		QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).vertexCreationURL,
		QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).vertexCreationTag),
		_vertexPanel (0)
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

	// Point à associer :
	_vertexPanel	= new QtMgx3DEntityPanel (
						this, "", true, "Point associé :", "",
						&mainWindow, SelectionManagerIfc::D0,
						FilterEntity::GeomVertex);
	layout->addWidget (_vertexPanel);
	connect (_vertexPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_vertexPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));


	layout->addStretch (2);

}	// QtTopologyVertexCreationPanel::QtTopologyVertexCreationPanel


QtTopologyVertexCreationPanel::QtTopologyVertexCreationPanel (
										const QtTopologyVertexCreationPanel& cao)
: QtMgx3DOperationPanel (0, *new QtMgx3DMainWindow(0), 0, "", ""),
		_vertexPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyVertexCreationPanel copy constructor is not allowed.");
}	// QtTopologyVertexCreationPanel::QtTopologyVertexCreationPanel (const QtTopologyVertexCreationPanel&)


QtTopologyVertexCreationPanel& QtTopologyVertexCreationPanel::operator = (
											const QtTopologyVertexCreationPanel&)
{
	MGX_FORBIDDEN ("QtTopologyVertexCreationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyVertexCreationPanel::QtTopologyVertexCreationPanel (const QtTopologyVertexCreationPanel&)


QtTopologyVertexCreationPanel::~QtTopologyVertexCreationPanel ( )
{
}	// QtTopologyVertexCreationPanel::~QtTopologyVertexCreationPanel


string QtTopologyVertexCreationPanel::getVertexName ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertexPanel)
	return _vertexPanel->getUniqueName ( );
}	// QtTopologyVertexCreationPanel::getVertexName

void QtTopologyVertexCreationPanel::reset ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_vertexPanel)
		_vertexPanel->reset ( );
	}
	catch (...)
	{
	}
}	// QtTopologyVertexCreationPanel::reset

void QtTopologyVertexCreationPanel::validate ( )
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
		error << "QtVertexByProjectionOnSurfacePanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopologyVertexCreationPanel::validate

void QtTopologyVertexCreationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_vertexPanel)
	_vertexPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_vertexPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyVertexCreationPanel::cancel


void QtTopologyVertexCreationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_vertexPanel)

	_vertexPanel->setUniqueName ("");

	_vertexPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyVertexCreationPanel::autoUpdate





// ===========================================================================
//                  LA CLASSE QtTopologyVertexCreationAction
// ===========================================================================

QtTopologyVertexCreationAction::QtTopologyVertexCreationAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtTopologyCreationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyVertexCreationPanel*	operationPanel	=
			new QtTopologyVertexCreationPanel (
					&getOperationPanelParent ( ), text.toStdString ( ), 
					mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyVertexCreationAction::QtTopologyVertexCreationAction


QtTopologyVertexCreationAction::QtTopologyVertexCreationAction (
										const QtTopologyVertexCreationAction&)
	: QtTopologyCreationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyVertexCreationAction copy constructor is not allowed.")
}	// QtTopologyVertexCreationAction::QtTopologyVertexCreationAction


QtTopologyVertexCreationAction& QtTopologyVertexCreationAction::operator = (
										const QtTopologyVertexCreationAction&)
{
	MGX_FORBIDDEN ("QtTopologyVertexCreationAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyVertexCreationAction::operator =


QtTopologyVertexCreationAction::~QtTopologyVertexCreationAction ( )
{
}	// QtTopologyVertexCreationAction::~QtTopologyVertexCreationAction


void QtTopologyVertexCreationAction::executeOperation ( )
{
	QtTopologyVertexCreationPanel*	panel	= dynamic_cast<QtTopologyVertexCreationPanel*>(getOperationPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtTopologyCreationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités topologiques :
	const string	vertex	= panel->getVertexName ( );

	cmdResult	= getContext ( ).getTopoManager( ).newTopoOnGeometry (vertex);

	setCommandResult (cmdResult);
}	// QtTopologyFaceCreationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
