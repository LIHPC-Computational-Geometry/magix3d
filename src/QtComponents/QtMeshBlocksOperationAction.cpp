/**
 * \file        QtMeshBlocksOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        06/09/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Mesh/MeshManagerIfc.h"
#include "QtComponents/QtMeshBlocksOperationAction.h"
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
//                        LA CLASSE QtMeshBlocksOperationPanel
// ===========================================================================

QtMeshBlocksOperationPanel::QtMeshBlocksOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (parent, mainWindow, action, helpURL, helpTag),
	  _namePanel (0), _meshAllCheckBox (0), _blocksPanel (0), _verticalLayout (0)
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

	// La sélection à mailler :
	_blocksPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Blocs à mailler :", "", &mainWindow,
			SelectionManagerIfc::D3, FilterEntity::TopoBlock);
	_blocksPanel->setMultiSelectMode (true);
	_verticalLayout->addWidget (_blocksPanel);
	connect (_blocksPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_blocksPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	_verticalLayout->addStretch (10);

	meshedSelectionCallback ( );
}	// QtMeshBlocksOperationPanel::QtMeshBlocksOperationPanel


QtMeshBlocksOperationPanel::QtMeshBlocksOperationPanel (
									const QtMeshBlocksOperationPanel& cao)
	: QtMgx3DOperationPanel (
				0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _namePanel (0), _meshAllCheckBox (0), _blocksPanel (0), _verticalLayout (0)
{
	MGX_FORBIDDEN ("QtMeshBlocksOperationPanel copy constructor is not allowed.");
}	// QtMeshBlocksOperationPanel::QtMeshBlocksOperationPanel (const QtMeshBlocksOperationPanel&)


QtMeshBlocksOperationPanel& QtMeshBlocksOperationPanel::operator = (
										const QtMeshBlocksOperationPanel&)
{
	MGX_FORBIDDEN ("QtMeshBlocksOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtMeshBlocksOperationPanel::QtMeshBlocksOperationPanel (const QtMeshBlocksOperationPanel&)


QtMeshBlocksOperationPanel::~QtMeshBlocksOperationPanel ( )
{
}	// QtMeshBlocksOperationPanel::~QtMeshBlocksOperationPanel


void QtMeshBlocksOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_blocksPanel)
	if (0 != _namePanel)
		_namePanel->autoUpdate ( );
	_blocksPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtMeshBlocksOperationPanel::reset


void QtMeshBlocksOperationPanel::validate ( )
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
		error << "QtMeshBlocksOperationPanel::validate : erreur non documentée.";
	}
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtMeshBlocksOperationPanel::validate


void QtMeshBlocksOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_blocksPanel)
	_blocksPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_blocksPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtMeshBlocksOperationPanel::cancel


void QtMeshBlocksOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	names	= getContext ( ).getSelectionManager (
									).getEntitiesNames(Entity::TopoBlock);
		UTF8String	text (Charset::UTF_8);
		for (vector<string>::const_iterator it = names.begin ( );
		     names.end ( ) != it; it++)
		{
			if (0 != text.length ( ))
				text << ' ';
			text << *it;
		}	// for (vector<string>::const_iterator it = names.begin ( );
		_blocksPanel->setUniqueName (text);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_blocksPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_blocksPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtMeshBlocksOperationPanel::autoUpdate


void QtMeshBlocksOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _blocksPanel)
		_blocksPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtMeshBlocksOperationPanel::operationCompleted


bool QtMeshBlocksOperationPanel::meshAll ( ) const
{
	CHECK_NULL_PTR_ERROR (_meshAllCheckBox)
	return Qt::Checked == _meshAllCheckBox->checkState ( ) ? true : false;
}   // QtMeshBlocksOperationPanel::meshAll


vector<string> QtMeshBlocksOperationPanel::getBlocks ( ) const
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	return _blocksPanel->getUniqueNames ( );
}   // QtMeshBlocksOperationPanel::getBlocks


void QtMeshBlocksOperationPanel::stopInteractiveMode ( )
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_blocksPanel->getNameTextField ( ))
	_blocksPanel->getNameTextField ( )->setInteractiveMode (false);
}	// QtMeshBlocksOperationPanel::stopInteractiveMode


vector<Entity*> QtMeshBlocksOperationPanel::getInvolvedEntities ( )
{
	vector<Entity*>			entities;
	const vector<string>	blocks	= getBlocks ( );

	for (vector<string>::const_iterator it = blocks.begin ( );
	     blocks.end ( ) != it; it++)
	{
		Entity*	entity	= getContext( ).getTopoManager( ).getBlock(*it, false);
		if (0 != entity)
			entities.push_back (entity);
	}	// for (vector<string>::const_iterator it = blocks.begin ( ); ...

	return entities;
}	// QtMeshBlocksOperationPanel::getInvolvedEntities


void QtMeshBlocksOperationPanel::meshedSelectionCallback ( )
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	_blocksPanel->setEnabled (!meshAll ( ));
}	// QtMeshBlocksOperationPanel::meshedSelectionCallback


QVBoxLayout& QtMeshBlocksOperationPanel::getVLayout ( )
{
	CHECK_NULL_PTR_ERROR (_verticalLayout)
	return *_verticalLayout;
}	// QtMeshBlocksOperationPanel::getVLayout


// ===========================================================================
//                  LA CLASSE QtMeshBlocksOperationAction
// ===========================================================================

QtMeshBlocksOperationAction::QtMeshBlocksOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DMeshOperationAction (icon, text, mainWindow, tooltip)
{
	QtMeshBlocksOperationPanel*	operationPanel	=
			new QtMeshBlocksOperationPanel (
				&getOperationPanelParent ( ), text.toStdString ( ),
				mainWindow, this,
				QtMgx3DApplication::HelpSystem::instance ( ).meshAllBlocksOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).meshAllBlocksOperationTag
				);
		if ((0 != operationPanel) && (getOperationPanel ( ) != operationPanel))
			setOperationPanel (operationPanel);
}	// QtMeshBlocksOperationAction::QtMeshBlocksOperationAction


QtMeshBlocksOperationAction::QtMeshBlocksOperationAction (
										const QtMeshBlocksOperationAction&)
	: QtMgx3DMeshOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtMeshBlocksOperationAction copy constructor is not allowed.")
}	// QtMeshBlocksOperationAction::QtMeshBlocksOperationAction


QtMeshBlocksOperationAction& QtMeshBlocksOperationAction::operator = (
										const QtMeshBlocksOperationAction&)
{
	MGX_FORBIDDEN ("QtMeshBlocksOperationAction assignment operator is not allowed.")
	return *this;
}	// QtMeshBlocksOperationAction::operator =


QtMeshBlocksOperationAction::~QtMeshBlocksOperationAction ( )
{
}	// QtMeshBlocksOperationAction::~QtMeshBlocksOperationAction


QtMeshBlocksOperationPanel* QtMeshBlocksOperationAction::getMeshPanel ( )
{
	return dynamic_cast<QtMeshBlocksOperationPanel*>(getOperationPanel ( ));
}	// QtMeshBlocksOperationAction::getMeshPanel


void QtMeshBlocksOperationAction::executeOperation ( )
{
	CHECK_NULL_PTR_ERROR (getMeshPanel ( ))

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DMeshOperationAction::executeOperation ( );

	if (true == getMeshPanel ( )->meshAll ( ))
		cmdResult	= getContext ( ).getMeshManager ( ).newAllBlocksMesh ( );
	else
	{
		vector<string>	blocks	= getMeshPanel ( )->getBlocks ( );
		getMeshPanel ( )->stopInteractiveMode ( );
		cmdResult	= getContext ( ).getMeshManager ( ).newBlocksMesh (blocks);
	}

	setCommandResult (cmdResult);
}	// QtMeshBlocksOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
