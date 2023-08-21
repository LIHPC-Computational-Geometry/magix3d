/**
 * \file        QtBlockAssociationAction.cpp
 * \author      Charles PIGNEROL
 * \date        15/11/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtBlockAssociationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"

#include "Topo/Block.h"
#include "Geom/Volume.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtBlockAssociationPanel
// ===========================================================================

QtBlockAssociationPanel::QtBlockAssociationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).vertexOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).vertexOperationTag),
	  _blocksPanel (0), _volumePanel (0)
{
//	SET_WIDGET_BACKGROUND (this, Qt::yellow)
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins (0, 0, 0, 0);

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Les blocs à associer :
	_blocksPanel	= new QtMgx3DEntityPanel (
						this, "", true, "Blocs :", "", &mainWindow, 
						SelectionManagerIfc::D3, FilterEntity::TopoBlock);
	_blocksPanel->setMultiSelectMode (true);
	layout->addWidget (_blocksPanel);
	connect (_blocksPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_blocksPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// L'éventuel volume à associer :
	_volumePanel	= new QtMgx3DEntityPanel (
						this, "", true, "Volume :", "", &mainWindow, 
						SelectionManagerIfc::D3, FilterEntity::GeomVolume);
	layout->addWidget (_volumePanel);
//	connect (_volumePanel->getNameTextField ( ),
	connect (_volumePanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_volumePanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_blocksPanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_volumePanel->getNameTextField ( ))
	_blocksPanel->getNameTextField ( )->setLinkedSeizureManagers (
							0, _volumePanel->getNameTextField ( ));
	_volumePanel->getNameTextField ( )->setLinkedSeizureManagers (
							_blocksPanel->getNameTextField ( ), 0);
}	// QtBlockAssociationPanel::QtBlockAssociationPanel


QtBlockAssociationPanel::QtBlockAssociationPanel (
										const QtBlockAssociationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _blocksPanel (0), _volumePanel (0)
{
	MGX_FORBIDDEN ("QtBlockAssociationPanel copy constructor is not allowed.");
}	// QtBlockAssociationPanel::QtBlockAssociationPanel (const QtBlockAssociationPanel&)


QtBlockAssociationPanel& QtBlockAssociationPanel::operator = (
											const QtBlockAssociationPanel&)
{
	MGX_FORBIDDEN ("QtBlockAssociationPanel assignment operator is not allowed.");
	return *this;
}	// QtBlockAssociationPanel::QtBlockAssociationPanel (const QtBlockAssociationPanel&)


QtBlockAssociationPanel::~QtBlockAssociationPanel ( )
{
	preview (false, true);
}	// QtBlockAssociationPanel::~QtBlockAssociationPanel


vector<string> QtBlockAssociationPanel::getBlocksNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	return _blocksPanel->getUniqueNames ( );
}	// QtBlockAssociationPanel::getBlocksNames


void QtBlockAssociationPanel::setBlocksNames (const vector<string>& names)
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	_blocksPanel->setUniqueNames (names);
}	// QtBlockAssociationPanel::setBlocksNames


string QtBlockAssociationPanel::getVolumeName ( ) const
{
	CHECK_NULL_PTR_ERROR (_volumePanel)
	return _volumePanel->getUniqueName ( );
}	// QtBlockAssociationPanel::getVolumeName


void QtBlockAssociationPanel::setVolumeName (const string& name)
{
	CHECK_NULL_PTR_ERROR (_volumePanel)
	_volumePanel->setUniqueName (name);
}	// QtBlockAssociationPanel::setVolumeName


void QtBlockAssociationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_volumePanel)
	_blocksPanel->reset ( );
	_volumePanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtBlockAssociationPanel::reset


void QtBlockAssociationPanel::validate ( )
{
// CP : suite discussion EBL/FL, il est convenu que la validation des
// paramètres de l'opération est effectuée par le "noyau" et qu'un mauvais
// paramétrage est remonté sous forme d'exception à la fonction appelante, donc
// avant exécution de la commande.
// Les validations des valeurs des paramètres sont donc ici commentées.
	TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);

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
		error << "QtBlockAssociationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getBlocksNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtBlockAssociationPanel::validate


void QtBlockAssociationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_volumePanel)
	_blocksPanel->stopSelection ( );
	_volumePanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		setVolumeName ("");
		vector<string>	names;
		setBlocksNames (names);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtBlockAssociationPanel::cancel


void QtBlockAssociationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_volumePanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedVolumes	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomVolume);
		if (1 == selectedVolumes.size ( ))
			setVolumeName (selectedVolumes [0]);
		vector<string>	selectedBlocks	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoBlock);
		if (0 != selectedBlocks.size ( ))
			setBlocksNames (selectedBlocks);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_blocksPanel->setUniqueName ("");
	_volumePanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	_blocksPanel->actualizeGui (true);
	_volumePanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtBlockAssociationPanel::autoUpdate


vector<Entity*> QtBlockAssociationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const string	volName	= getVolumeName ( );
	if (0 != volName.length ( ))
	{
		GeomEntity*	volume	=
					getContext ( ).getGeomManager ( ).getVolume (volName);	
		CHECK_NULL_PTR_ERROR (volume)
		entities.push_back (volume);
	}

	const vector<string>	blocks	= getBlocksNames ( );
	for (vector<string>::const_iterator it = blocks.begin ( );
	     blocks.end ( ) != it; it++)
	{
		TopoEntity*	block	= getContext ( ).getTopoManager ( ).getBlock (*it);
		CHECK_NULL_PTR_ERROR (block)
		entities.push_back (block);
	}

	return entities;
}	// QtBlockAssociationPanel::getInvolvedEntities


void QtBlockAssociationPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _blocksPanel){
		_blocksPanel->stopSelection ( );
		_blocksPanel->clearSelection();
	}
	if (0 != _volumePanel){
		_volumePanel->stopSelection ( );
		_volumePanel->clearSelection ( );
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtBlockAssociationPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtBlockAssociationAction
// ===========================================================================

QtBlockAssociationAction::QtBlockAssociationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtBlockAssociationPanel*	operationPanel	=
		new QtBlockAssociationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtBlockAssociationAction::QtBlockAssociationAction


QtBlockAssociationAction::QtBlockAssociationAction (
										const QtBlockAssociationAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtBlockAssociationAction copy constructor is not allowed.")
}	// QtBlockAssociationAction::QtBlockAssociationAction


QtBlockAssociationAction& QtBlockAssociationAction::operator = (
										const QtBlockAssociationAction&)
{
	MGX_FORBIDDEN ("QtBlockAssociationAction assignment operator is not allowed.")
	return *this;
}	// QtBlockAssociationAction::operator =


QtBlockAssociationAction::~QtBlockAssociationAction ( )
{
}	// QtBlockAssociationAction::~QtBlockAssociationAction


QtBlockAssociationPanel*
					QtBlockAssociationAction::getAssociationPanel ( )
{
	return dynamic_cast<QtBlockAssociationPanel*>(getOperationPanel ( ));
}	// QtBlockAssociationAction::getAssociationPanel


void QtBlockAssociationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des blocs :
	QtBlockAssociationPanel*	panel	= getAssociationPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>				blocks	= panel->getBlocksNames ( );
	string						volume	= panel->getVolumeName ( );

	cmdResult	= getContext ( ).getTopoManager ( ).setGeomAssociation (blocks, volume, false);
	setCommandResult (cmdResult);
}	// QtBlockAssociationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
