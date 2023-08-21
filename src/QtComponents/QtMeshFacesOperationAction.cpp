/**
 * \file        QtMeshFacesOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        09/09/2013
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"
#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Mesh/MeshManagerIfc.h"
#include "QtComponents/QtMeshFacesOperationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Mesh;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtMeshFacesOperationPanel
// ===========================================================================

QtMeshFacesOperationPanel::QtMeshFacesOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (parent, mainWindow, action, helpURL, helpTag),
	  _namePanel (0), _meshAllCheckBox (0), _facesPanel (0), _verticalLayout (0)
{
	_verticalLayout	= new QVBoxLayout (this);
	_verticalLayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	_verticalLayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	setLayout (_verticalLayout);

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	_verticalLayout->addWidget (label);

	// Tout mailler ?
	_meshAllCheckBox	= new QCheckBox ("Tout mailler", this);
	connect (_meshAllCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (meshedSelectionCallback ( )));
	_verticalLayout->addWidget (_meshAllCheckBox);

	// Surfaces à mailler :
	_facesPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Faces topologiques à mailler :", "", &mainWindow,
			SelectionManagerIfc::D2, FilterEntity::TopoCoFace);
	_facesPanel->setMultiSelectMode (true);
	_verticalLayout->addWidget (_facesPanel);
	connect (_facesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_facesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	_verticalLayout->addStretch (10);

	meshedSelectionCallback ( );
}	// QtMeshFacesOperationPanel::QtMeshFacesOperationPanel


QtMeshFacesOperationPanel::QtMeshFacesOperationPanel (
									const QtMeshFacesOperationPanel& cao)
	: QtMgx3DOperationPanel (
				0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _namePanel (0), _meshAllCheckBox (0), _facesPanel (0), _verticalLayout (0)
{
	MGX_FORBIDDEN ("QtMeshFacesOperationPanel copy constructor is not allowed.");
}	// QtMeshFacesOperationPanel::QtMeshFacesOperationPanel (const QtMeshFacesOperationPanel&)


QtMeshFacesOperationPanel& QtMeshFacesOperationPanel::operator = (
										const QtMeshFacesOperationPanel&)
{
	MGX_FORBIDDEN ("QtMeshFacesOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtMeshFacesOperationPanel::QtMeshFacesOperationPanel (const QtMeshFacesOperationPanel&)


QtMeshFacesOperationPanel::~QtMeshFacesOperationPanel ( )
{
}	// QtMeshFacesOperationPanel::~QtMeshFacesOperationPanel


void QtMeshFacesOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_facesPanel)
	if (0 != _namePanel)
		_namePanel->autoUpdate ( );
	_facesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtMeshFacesOperationPanel::reset


void QtMeshFacesOperationPanel::validate ( )
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
		error << "QtMeshFacesOperationPanel::validate : erreur non documentée.";
	}
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtMeshFacesOperationPanel::validate


void QtMeshFacesOperationPanel::cancel ( )
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
}	// QtMeshFacesOperationPanel::cancel


void QtMeshFacesOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_facesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	names	= getContext ( ).getSelectionManager (
										).getEntitiesNames(Entity::TopoCoFace);
		UTF8String	text (Charset::UTF_8);
		for (vector<string>::const_iterator it = names.begin ( );
		     names.end ( ) != it; it++)
		{
			if (0 != text.length ( ))
				text << ' ';
			text << *it;
		}
		_facesPanel->setUniqueName (text);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_facesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_facesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtMeshFacesOperationPanel::autoUpdate


void QtMeshFacesOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _facesPanel)
		_facesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtMeshFacesOperationPanel::operationCompleted


bool QtMeshFacesOperationPanel::meshAll ( ) const
{
	CHECK_NULL_PTR_ERROR (_meshAllCheckBox)
	return Qt::Checked == _meshAllCheckBox->checkState ( ) ? true : false;
}	// QtMeshFacesOperationPanel::meshAll


vector<string> QtMeshFacesOperationPanel::getFaces ( ) const
{
	CHECK_NULL_PTR_ERROR (_facesPanel)
	return _facesPanel->getUniqueNames ( );
}	// QtMeshFacesOperationPanel::getFaces


void QtMeshFacesOperationPanel::stopInteractiveMode ( )
{
	CHECK_NULL_PTR_ERROR (_facesPanel)
	CHECK_NULL_PTR_ERROR (_facesPanel->getNameTextField ( ))
	_facesPanel->getNameTextField ( )->setInteractiveMode (false);
}	// QtMeshFacesOperationPanel::stopInteractiveMode


vector<Entity*> QtMeshFacesOperationPanel::getInvolvedEntities ( )
{
	vector<Entity*>			entities;
	const vector<string>	faces	= getFaces ( );

	for (vector<string>::const_iterator it = faces.begin ( );
	     faces.end ( ) != it; it++)
	{
		Entity*	entity	= getContext( ).getTopoManager( ).getCoFace(*it, false);
		if (0 != entity)
			entities.push_back (entity);
	}	// for (vector<string>::const_iterator it = faces.begin ( );

	return entities;
}	// QtMeshFacesOperationPanel::getInvolvedEntities


void QtMeshFacesOperationPanel::meshedSelectionCallback ( )
{
	CHECK_NULL_PTR_ERROR (_facesPanel)
	_facesPanel->setEnabled (!meshAll ( ));
}	// QtMeshFacesOperationPanel::meshedSelectionCallback


QVBoxLayout& QtMeshFacesOperationPanel::getVLayout ( )
{
	CHECK_NULL_PTR_ERROR (_verticalLayout)
	return *_verticalLayout;
}	// QtMeshFacesOperationPanel::getVLayout


// ===========================================================================
//                  LA CLASSE QtMeshFacesOperationAction
// ===========================================================================

QtMeshFacesOperationAction::QtMeshFacesOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DMeshOperationAction (icon, text, mainWindow, tooltip)
{
	QtMeshFacesOperationPanel*	operationPanel	=
			new QtMeshFacesOperationPanel (
				&getOperationPanelParent ( ), text.toStdString ( ),
				mainWindow, this,
				QtMgx3DApplication::HelpSystem::instance ( ).meshAllSurfacesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).meshAllSurfacesOperationTag
				);
		if ((0 != operationPanel) && (getOperationPanel ( ) != operationPanel))
			setOperationPanel (operationPanel);
}	// QtMeshFacesOperationAction::QtMeshFacesOperationAction


QtMeshFacesOperationAction::QtMeshFacesOperationAction (
										const QtMeshFacesOperationAction&)
	: QtMgx3DMeshOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtMeshFacesOperationAction copy constructor is not allowed.")
}	// QtMeshFacesOperationAction::QtMeshFacesOperationAction


QtMeshFacesOperationAction& QtMeshFacesOperationAction::operator = (
										const QtMeshFacesOperationAction&)
{
	MGX_FORBIDDEN ("QtMeshFacesOperationAction assignment operator is not allowed.")
	return *this;
}	// QtMeshFacesOperationAction::operator =


QtMeshFacesOperationAction::~QtMeshFacesOperationAction ( )
{
}	// QtMeshFacesOperationAction::~QtMeshFacesOperationAction


QtMeshFacesOperationPanel* QtMeshFacesOperationAction::getMeshPanel ( )
{
	return dynamic_cast<QtMeshFacesOperationPanel*>(getOperationPanel ( ));
}	// QtMeshFacesOperationAction::getMeshPanel


void QtMeshFacesOperationAction::executeOperation ( )
{
	CHECK_NULL_PTR_ERROR (getMeshPanel ( ))

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DMeshOperationAction::executeOperation ( );

	if (true == getMeshPanel ( )->meshAll ( ))
		cmdResult	= getContext ( ).getMeshManager ( ).newAllFacesMesh ( );
	else
	{
		vector<string>	faces	= getMeshPanel ( )->getFaces ( );
		getMeshPanel ( )->stopInteractiveMode ( );
		cmdResult	= getContext ( ).getMeshManager ( ).newFacesMesh (faces);
	}

	setCommandResult (cmdResult);
}	// QtMeshFacesOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
