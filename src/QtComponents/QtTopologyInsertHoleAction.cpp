/**
 * \file        QtTopologyInsertHoleAction.cpp
 * \author      Charles PIGNEROL
 * \date        06/03/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyInsertHoleAction.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologyInsertHolePanel
// ===========================================================================

QtTopologyInsertHolePanel::QtTopologyInsertHolePanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).insertHoleOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).insertHoleOperationTag),
	  _facesPanel (0)
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

	// Les faces à dupliquer :
	_facesPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Faces :", "", &mainWindow,
			SelectionManagerIfc::D2, FilterEntity::TopoCoFace);
	_facesPanel->setMultiSelectMode (true);
	connect (_facesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_facesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_facesPanel);

	layout->addStretch (2);
}	// QtTopologyInsertHolePanel::QtTopologyInsertHolePanel


QtTopologyInsertHolePanel::QtTopologyInsertHolePanel (
										const QtTopologyInsertHolePanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _facesPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyInsertHolePanel copy constructor is not allowed.");
}	// QtTopologyInsertHolePanel::QtTopologyInsertHolePanel (const QtTopologyInsertHolePanel&)


QtTopologyInsertHolePanel& QtTopologyInsertHolePanel::operator = (
											const QtTopologyInsertHolePanel&)
{
	MGX_FORBIDDEN ("QtTopologyInsertHolePanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyInsertHolePanel::QtTopologyInsertHolePanel (const QtTopologyInsertHolePanel&)


QtTopologyInsertHolePanel::~QtTopologyInsertHolePanel ( )
{
}	// QtTopologyInsertHolePanel::~QtTopologyInsertHolePanel


void QtTopologyInsertHolePanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_facesPanel)
	_facesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtArcCircleOperationPanel::reset


void QtTopologyInsertHolePanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_facesPanel)

	_facesPanel->stopSelection ( );
	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_facesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyInsertHolePanel::cancel


void QtTopologyInsertHolePanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_facesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedFaces	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoFace);
		_facesPanel->setUniqueNames (selectedFaces);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_facesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_facesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyInsertHolePanel::autoUpdate


vector<string> QtTopologyInsertHolePanel::getFacesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_facesPanel)
	return _facesPanel->getUniqueNames ( );
}	// QtTopologyInsertHolePanel::getFacesNames


vector<Entity*> QtTopologyInsertHolePanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const vector<string>	names	= getFacesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*		face	=
			getContext ( ).getTopoManager ( ).getCoFace (*it, false);
		if (0 != face)
			entities.push_back (face);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtTopologyInsertHolePanel::getInvolvedEntities


void QtTopologyInsertHolePanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _facesPanel)
	{
		_facesPanel->stopSelection ( );
		_facesPanel->setUniqueName ("");
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyInsertHolePanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyInsertHoleAction
// ===========================================================================

QtTopologyInsertHoleAction::QtTopologyInsertHoleAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyInsertHolePanel*	operationPanel	= 
			new QtTopologyInsertHolePanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	operationPanel->setToolTip (tooltip);
	setOperationPanel (operationPanel);
}	// QtTopologyInsertHoleAction::QtTopologyInsertHoleAction


QtTopologyInsertHoleAction::QtTopologyInsertHoleAction (
										const QtTopologyInsertHoleAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyInsertHoleAction copy constructor is not allowed.")
}	// QtTopologyInsertHoleAction::QtTopologyInsertHoleAction


QtTopologyInsertHoleAction& QtTopologyInsertHoleAction::operator = (
										const QtTopologyInsertHoleAction&)
{
	MGX_FORBIDDEN ("QtTopologyInsertHoleAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyInsertHoleAction::operator =


QtTopologyInsertHoleAction::~QtTopologyInsertHoleAction ( )
{
}	// QtTopologyInsertHoleAction::~QtTopologyInsertHoleAction


QtTopologyInsertHolePanel*
					QtTopologyInsertHoleAction::getTopologyInsertHolePanel ( )
{
	return dynamic_cast<QtTopologyInsertHolePanel*>(getOperationPanel ( ));
}	// QtTopologyInsertHoleAction::getTopologyInsertHolePanel


void QtTopologyInsertHoleAction::executeOperation ( )
{
	QtTopologyInsertHolePanel*	panel	= dynamic_cast<QtTopologyInsertHolePanel*>(getTopologyInsertHolePanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres d'insertion de trou dans la topologie :
	vector<string>	names	= panel->getFacesNames ( );

	Mgx3D::Internal::M3DCommandResultIfc*	result	= getContext ( ).getTopoManager ( ).insertHole (names);
	CHECK_NULL_PTR_ERROR (result)
	setCommandResult (result);
	if (CommandIfc::FAIL == result->getStatus ( ))
		throw Exception (result->getErrorMessage ( ));
	else if (CommandIfc::CANCELED == result->getStatus ( ))
		throw Exception ("Opération annulée");
}	// QtTopologyInsertHoleAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
