/**
 * \file        QtTopologyFuse2FacesAction.cpp
 * \author      Charles PIGNEROL
 * \date        06/03/2014
 */

#include "Internal/ContextIfc.h"

#include <QtComponents/QtTopologyFuse2FacesAction.h>
#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
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
//                        LA CLASSE QtTopologyFuse2FacesPanel
// ===========================================================================

QtTopologyFuse2FacesPanel::QtTopologyFuse2FacesPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).fuse2facesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).fuse2facesOperationTag),
	  _faces1Panel (0), _faces2Panel (0)
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

	// La face 1 à coller :
	_faces1Panel	= new QtMgx3DEntityPanel (
			this, "", true, "Faces 1 :", "", &mainWindow,
			SelectionManagerIfc::D2, FilterEntity::TopoCoFace);
	_faces1Panel->setMultiSelectMode (true);
	connect (_faces1Panel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_faces1Panel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_faces1Panel);

	// La face 2 à coller :
	_faces2Panel	= new QtMgx3DEntityPanel (
			this, "", true, "Faces 2 :", "", &mainWindow,
			SelectionManagerIfc::D2, FilterEntity::TopoCoFace);
	_faces2Panel->setMultiSelectMode (true);
	connect (_faces2Panel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_faces2Panel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_faces2Panel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_faces1Panel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_faces2Panel->getNameTextField ( ))
	_faces1Panel->getNameTextField (
			)->setLinkedSeizureManagers (0, _faces2Panel->getNameTextField ( ));
	_faces2Panel->getNameTextField (
			)->setLinkedSeizureManagers (_faces1Panel->getNameTextField ( ), 0);
}	// QtTopologyFuse2FacesPanel::QtTopologyFuse2FacesPanel


QtTopologyFuse2FacesPanel::QtTopologyFuse2FacesPanel (
										const QtTopologyFuse2FacesPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _faces1Panel (0), _faces2Panel (0)
{
	MGX_FORBIDDEN ("QtTopologyFuse2FacesPanel copy constructor is not allowed.");
}	// QtTopologyFuse2FacesPanel::QtTopologyFuse2FacesPanel (const QtTopologyFuse2FacesPanel&)


QtTopologyFuse2FacesPanel& QtTopologyFuse2FacesPanel::operator = (
											const QtTopologyFuse2FacesPanel&)
{
	MGX_FORBIDDEN ("QtTopologyFuse2FacesPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyFuse2FacesPanel::QtTopologyFuse2FacesPanel (const QtTopologyFuse2FacesPanel&)


QtTopologyFuse2FacesPanel::~QtTopologyFuse2FacesPanel ( )
{
}	// QtTopologyFuse2FacesPanel::~QtTopologyFuse2FacesPanel


void QtTopologyFuse2FacesPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_faces1Panel)
	CHECK_NULL_PTR_ERROR (_faces2Panel)
	_faces1Panel->reset ( );
	_faces2Panel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyFuse2FacesPanel::reset


void QtTopologyFuse2FacesPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_faces1Panel)
	CHECK_NULL_PTR_ERROR (_faces2Panel)

	_faces1Panel->stopSelection ( );
	_faces2Panel->stopSelection ( );
	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_faces1Panel->setUniqueName ("");
		_faces2Panel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyFuse2FacesPanel::cancel


void QtTopologyFuse2FacesPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_faces1Panel)
	CHECK_NULL_PTR_ERROR (_faces2Panel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedFaces	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoFace);
		if (2 >= selectedFaces.size ( ))
		{
			if (1 <= selectedFaces.size ( ))
				_faces1Panel->setUniqueName (selectedFaces [0]);
			if (2 == selectedFaces.size ( ))
				_faces2Panel->setUniqueName (selectedFaces [1]);
		}	// if (2 >= selectedFaces.size ( ))

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_faces1Panel->clearSelection ( );
	_faces2Panel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_faces1Panel->actualizeGui (true);
	_faces2Panel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyFuse2FacesPanel::autoUpdate


vector<string> QtTopologyFuse2FacesPanel::getFaces1Names ( ) const
{
	CHECK_NULL_PTR_ERROR (_faces1Panel)
	return _faces1Panel->getUniqueNames ( );
}	// QtTopologyFuse2FacesPanel::getFaces1Names


vector<string> QtTopologyFuse2FacesPanel::getFaces2Names ( ) const
{
	CHECK_NULL_PTR_ERROR (_faces2Panel)
	return _faces2Panel->getUniqueNames ( );
}	// QtTopologyFuse2FacesPanel::getFaces2Names


vector<Entity*> QtTopologyFuse2FacesPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const vector<string>	names1	= getFaces1Names ( );
	const vector<string>	names2	= getFaces2Names ( );
	for (vector<string>::const_iterator it1 = names1.begin ( );
	     names1.end ( ) != it1; it1++)
	{
		TopoEntity*		face1	=
			getContext ( ).getTopoManager ( ).getCoFace (*it1, false);
		if (0 != face1)
			entities.push_back (face1);
	}	// for (vector<string>::const_iterator it1 = names1.begin ( ); ...
	for (vector<string>::const_iterator it2 = names2.begin ( );
	     names2.end ( ) != it2; it2++)
	{
		TopoEntity*		face2	=
			getContext ( ).getTopoManager ( ).getCoFace (*it2, false);
		if (0 != face2)
			entities.push_back (face2);
	}	// for (vector<string>::const_iterator it2 = names1.begin ( ); ...

	return entities;
}	// QtTopologyFuse2FacesPanel::getInvolvedEntities


void QtTopologyFuse2FacesPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _faces1Panel)
	{
		_faces1Panel->stopSelection ( );
		_faces1Panel->setUniqueName ("");
	}
	if (0 != _faces2Panel)
	{
		_faces2Panel->stopSelection ( );	
		_faces2Panel->setUniqueName ("");	
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyFuse2FacesPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyFuse2FacesAction
// ===========================================================================

QtTopologyFuse2FacesAction::QtTopologyFuse2FacesAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyFuse2FacesPanel*	operationPanel	= 
			new QtTopologyFuse2FacesPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyFuse2FacesAction::QtTopologyFuse2FacesAction


QtTopologyFuse2FacesAction::QtTopologyFuse2FacesAction (
										const QtTopologyFuse2FacesAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyFuse2FacesAction copy constructor is not allowed.")
}	// QtTopologyFuse2FacesAction::QtTopologyFuse2FacesAction


QtTopologyFuse2FacesAction& QtTopologyFuse2FacesAction::operator = (
										const QtTopologyFuse2FacesAction&)
{
	MGX_FORBIDDEN ("QtTopologyFuse2FacesAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyFuse2FacesAction::operator =


QtTopologyFuse2FacesAction::~QtTopologyFuse2FacesAction ( )
{
}	// QtTopologyFuse2FacesAction::~QtTopologyFuse2FacesAction


QtTopologyFuse2FacesPanel*
					QtTopologyFuse2FacesAction::getTopologyFuse2FacesPanel ( )
{
	return dynamic_cast<QtTopologyFuse2FacesPanel*>(getOperationPanel ( ));
}	// QtTopologyFuse2FacesAction::getTopologyFuse2FacesPanel


void QtTopologyFuse2FacesAction::executeOperation ( )
{
	QtTopologyFuse2FacesPanel*	panel	= dynamic_cast<QtTopologyFuse2FacesPanel*>(getTopologyFuse2FacesPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de collage des faces topologiques :
	vector<string>	names1	= panel->getFaces1Names ( );
	vector<string>	names2	= panel->getFaces2Names ( );

	Mgx3D::Internal::M3DCommandResultIfc*	result	= 0;
	if ((names1.size ( ) == 1) && (names2.size ( ) == 1))
		result	= getContext ( ).getTopoManager ( ).fuse2Faces (names1 [0], names2 [0]);
	else
		result	= getContext ( ).getTopoManager ( ).fuse2FaceList (names1, names2);
	CHECK_NULL_PTR_ERROR (result)
	setCommandResult (result);
	if (CommandIfc::FAIL == result->getStatus ( ))
		throw Exception (result->getErrorMessage ( ));
	else if (CommandIfc::CANCELED == result->getStatus ( ))
		throw Exception ("Opération annulée");
}	// QtTopologyFuse2FacesAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
