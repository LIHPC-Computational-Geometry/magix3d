/**
 * \file        QtMeshSubVolumeBetweenSheetsAction.cpp
 * \author      Charles PIGNEROL
 * \date        07/12/2016
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Mesh/MeshManagerIfc.h"
#include "QtComponents/QtMeshSubVolumeBetweenSheetsAction.h"
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
//                     LA CLASSE QtMeshSubVolumeBetweenSheetsPanel
// ===========================================================================

QtMeshSubVolumeBetweenSheetsPanel::QtMeshSubVolumeBetweenSheetsPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (parent, mainWindow, action, helpURL, helpTag),
	  _nameTextField (0), _blocksPanel (0), _edgePanel (0),
	  _fromTextField (0), _toTextField (0), _verticalLayout (0)
{
	_verticalLayout	= new QVBoxLayout (this);
	setLayout (_verticalLayout);

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	_verticalLayout->addWidget (label);

	// Le nom du sous-volume créé :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	_verticalLayout->addLayout (hlayout);
	label	= new QLabel ("Nom :", this);
	hlayout->addWidget (label);
	_nameTextField	= new QtTextField (this);
	hlayout->addWidget (_nameTextField);
	label->setToolTip (QString::fromUtf8("Nom du sous-volume créé."));
	_nameTextField->setToolTip (QString::fromUtf8("Nom du sous-volume créé."));
	hlayout->addStretch (10.);

	// La blocs sièges de l'extraction :
	_blocksPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Blocs :", "", &mainWindow,
			SelectionManagerIfc::D3, FilterEntity::TopoBlock);
	_blocksPanel->setToolTip (QString::fromUtf8("Blocs sièges de l'extraction. Doivent être structurés et maillés."));
	_blocksPanel->setMultiSelectMode (true);
	_verticalLayout->addWidget (_blocksPanel);
	connect (_blocksPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_blocksPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// L'arête perpendiculaire aux feuillets :
	_edgePanel	= new QtMgx3DEntityPanel (
			this, "", true, "Arête :", "", &mainWindow,
			SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	connect (_edgePanel->getNameTextField ( ),
	         SIGNAL (selectionModified (QString)), this,
	         SLOT (parametersModifiedCallback ( )));
	_verticalLayout->addWidget (_edgePanel);
	connect (_edgePanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edgePanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	
	// Les indices des feuillets pris en compte :
	hlayout	= new QHBoxLayout ( );
	_verticalLayout->addLayout (hlayout);
	label	= new QLabel ("Depuis :", this);
	hlayout->addWidget (label);
	_fromTextField	= new QtIntTextField (1, this);
	hlayout->addWidget (_fromTextField);
	connect (_fromTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (parametersModifiedCallback ( )));
	label->setToolTip (QString::fromUtf8("Indice du premier feuillet délimitant le sous-volume à extraire."));
	_fromTextField->setToolTip (QString::fromUtf8("Indice du premier feuillet délimitant le sous-volume à extraire."));
	hlayout->addStretch (2);
	hlayout	= new QHBoxLayout ( );
	_verticalLayout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Jusqu'à :"), this);
	hlayout->addWidget (label);
	_toTextField	= new QtIntTextField (1, this);
	hlayout->addWidget (_toTextField);
	connect (_toTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (parametersModifiedCallback ( )));
	label->setToolTip (QString::fromUtf8("Indice du second feuillet délimitant le sous-volume à extraire."));
	_toTextField->setToolTip (QString::fromUtf8("Indice du second feuillet délimitant le sous-volume à extraire."));
	hlayout->addStretch (2);

	_verticalLayout->addStretch (10);
}	// QtMeshSubVolumeBetweenSheetsPanel::QtMeshSubVolumeBetweenSheetsPanel


QtMeshSubVolumeBetweenSheetsPanel::QtMeshSubVolumeBetweenSheetsPanel (
								const QtMeshSubVolumeBetweenSheetsPanel& cao)
	: QtMgx3DOperationPanel (
				0, *new QtMgx3DMainWindow (0,"Invalid main window"), 0, "", ""),
	  _nameTextField (0), _blocksPanel (0), _edgePanel (0),
	  _fromTextField (0), _toTextField (0), _verticalLayout (0)
{
	MGX_FORBIDDEN ("QtMeshSubVolumeBetweenSheetsPanel copy constructor is not allowed.");
}	// QtMeshSubVolumeBetweenSheetsPanel::QtMeshSubVolumeBetweenSheetsPanel (const QtMeshSubVolumeBetweenSheetsPanel&)


QtMeshSubVolumeBetweenSheetsPanel& QtMeshSubVolumeBetweenSheetsPanel::operator = (
									const QtMeshSubVolumeBetweenSheetsPanel&)
{
	MGX_FORBIDDEN ("QtMeshSubVolumeBetweenSheetsPanel assignment operator is not allowed.");
	return *this;
}	// QtMeshSubVolumeBetweenSheetsPanel::QtMeshSubVolumeBetweenSheetsPanel (const QtMeshSubVolumeBetweenSheetsPanel&)


QtMeshSubVolumeBetweenSheetsPanel::~QtMeshSubVolumeBetweenSheetsPanel ( )
{
}	// QtMeshSubVolumeBetweenSheetsPanel::~QtMeshSubVolumeBetweenSheetsPanel


void QtMeshSubVolumeBetweenSheetsPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_nameTextField)
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_fromTextField)
	CHECK_NULL_PTR_ERROR (_toTextField)
	_nameTextField->setText (QString (""));
	_blocksPanel->reset ( );
	_edgePanel->reset ( );
	_fromTextField->setValue (1);
	_toTextField->setValue (1);
	_fromTextField->setValue (1);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtMeshSubVolumeBetweenSheetsPanel::reset


void QtMeshSubVolumeBetweenSheetsPanel::validate ( )
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
		error << "QtMeshSubVolumeBetweenSheetsPanel::validate : erreur non documentée.";
	}
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_nameTextField->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtMeshSubVolumeBetweenSheetsPanel::validate


void QtMeshSubVolumeBetweenSheetsPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	_blocksPanel->stopSelection ( );
	_edgePanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_blocksPanel->setUniqueName ("");
		_edgePanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtMeshSubVolumeBetweenSheetsPanel::cancel


void QtMeshSubVolumeBetweenSheetsPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_fromTextField)
	CHECK_NULL_PTR_ERROR (_toTextField)

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

		vector<string>	names	= getContext ( ).getSelectionManager (
									).getEntitiesNames(Entity::TopoBlock);
		names	= getContext ( ).getSelectionManager (
									).getEntitiesNames(Entity::TopoCoEdge);
		if (1 == names.size ( ))
			_edgePanel->setUniqueName (names [0]);
		else
			_edgePanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_nameTextField->setText (string (""));
	_blocksPanel->clearSelection ( );
	_edgePanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_blocksPanel->actualizeGui (true);
	_edgePanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtMeshSubVolumeBetweenSheetsPanel::autoUpdate


void QtMeshSubVolumeBetweenSheetsPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _blocksPanel)
		_blocksPanel->stopSelection ( );
	if (0 != _edgePanel)
		_edgePanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtMeshSubVolumeBetweenSheetsPanel::operationCompleted


string QtMeshSubVolumeBetweenSheetsPanel::getName ( ) const
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	return _nameTextField->getText ( );
}   // QtMeshSubVolumeBetweenSheetsPanel::getName


vector<string> QtMeshSubVolumeBetweenSheetsPanel::getBlocks ( ) const
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	return _blocksPanel->getUniqueNames ( );
}   // QtMeshSubVolumeBetweenSheetsPanel::getBlocks


string QtMeshSubVolumeBetweenSheetsPanel::getEdge ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
	return _edgePanel->getUniqueName ( );
}	// QtMeshSubVolumeBetweenSheetsPanel::getEdge


int QtMeshSubVolumeBetweenSheetsPanel::from ( ) const
{
	CHECK_NULL_PTR_ERROR (_fromTextField)
	return _fromTextField->getValue ( );
}	// QtMeshSubVolumeBetweenSheetsPanel::from


int QtMeshSubVolumeBetweenSheetsPanel::to ( ) const
{
	CHECK_NULL_PTR_ERROR (_toTextField)
	return _toTextField->getValue ( );
}	// QtMeshSubVolumeBetweenSheetsPanel::to


void QtMeshSubVolumeBetweenSheetsPanel::stopInteractiveMode ( )
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_blocksPanel->getNameTextField ( ))
	_blocksPanel->getNameTextField ( )->setInteractiveMode (false);
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_edgePanel->getNameTextField ( ))
	_edgePanel->getNameTextField ( )->setInteractiveMode (false);
}	// QtMeshSubVolumeBetweenSheetsPanel::stopInteractiveMode


vector<Entity*> QtMeshSubVolumeBetweenSheetsPanel::getInvolvedEntities ( )
{
	vector<Entity*>			entities;
	const vector<string>	blocks	= getBlocks ( );
	const string			edge	= getEdge ( );

	for (vector<string>::const_iterator it = blocks.begin ( );
	     blocks.end ( ) != it; it++)
	{
		Entity*	entity	= getContext( ).getTopoManager( ).getBlock(*it, false);
		if (0 != entity)
			entities.push_back (entity);
	}	// for (vector<string>::const_iterator it = blocks.begin ( ); ...
	if (false == edge.empty ( ))
	{
		Entity*	coEdge	= getContext( ).getTopoManager( ).getCoEdge(edge,false);
		if (0 != coEdge)
			entities.push_back (coEdge);
	}	// if (false == edge.empty ( ))

	return entities;
}	// QtMeshSubVolumeBetweenSheetsPanel::getInvolvedEntities


QVBoxLayout& QtMeshSubVolumeBetweenSheetsPanel::getVLayout ( )
{
	CHECK_NULL_PTR_ERROR (_verticalLayout)
	return *_verticalLayout;
}	// QtMeshSubVolumeBetweenSheetsPanel::getVLayout


// ===========================================================================
//                  LA CLASSE QtMeshSubVolumeBetweenSheetsAction
// ===========================================================================

QtMeshSubVolumeBetweenSheetsAction::QtMeshSubVolumeBetweenSheetsAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DMeshOperationAction (icon, text, mainWindow, tooltip)
{
	QtMeshSubVolumeBetweenSheetsPanel*	operationPanel	=
			new QtMeshSubVolumeBetweenSheetsPanel (
				&getOperationPanelParent ( ), text.toStdString ( ),
				mainWindow, this,
				QtMgx3DApplication::HelpSystem::instance ( ).meshSheetsVolumeOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).meshSheetsVolumeOperationTag
				);
		if ((0 != operationPanel) && (getOperationPanel ( ) != operationPanel))
			setOperationPanel (operationPanel);
}	// QtMeshSubVolumeBetweenSheetsAction::QtMeshSubVolumeBetweenSheetsAction


QtMeshSubVolumeBetweenSheetsAction::QtMeshSubVolumeBetweenSheetsAction (
									const QtMeshSubVolumeBetweenSheetsAction&)
	: QtMgx3DMeshOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0, ""), "")
{
	MGX_FORBIDDEN ("QtMeshSubVolumeBetweenSheetsAction copy constructor is not allowed.")
}	// QtMeshSubVolumeBetweenSheetsAction::QtMeshSubVolumeBetweenSheetsAction


QtMeshSubVolumeBetweenSheetsAction&
				QtMeshSubVolumeBetweenSheetsAction::operator = (
									const QtMeshSubVolumeBetweenSheetsAction&)
{
	MGX_FORBIDDEN ("QtMeshSubVolumeBetweenSheetsAction assignment operator is not allowed.")
	return *this;
}	// QtMeshSubVolumeBetweenSheetsAction::operator =


QtMeshSubVolumeBetweenSheetsAction::~QtMeshSubVolumeBetweenSheetsAction ( )
{
}	// QtMeshSubVolumeBetweenSheetsAction::~QtMeshSubVolumeBetweenSheetsAction


QtMeshSubVolumeBetweenSheetsPanel*
							QtMeshSubVolumeBetweenSheetsAction::getMeshPanel ( )
{
	return dynamic_cast<QtMeshSubVolumeBetweenSheetsPanel*>(
														getOperationPanel ( ));
}	// QtMeshSubVolumeBetweenSheetsAction::getMeshPanel


void QtMeshSubVolumeBetweenSheetsAction::executeOperation ( )
{
	CHECK_NULL_PTR_ERROR (getMeshPanel ( ))

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DMeshOperationAction::executeOperation ( );

	getMeshPanel ( )->stopInteractiveMode ( );
	string			name	= getMeshPanel ( )->getName ( );
	vector<string>	blocks	= getMeshPanel ( )->getBlocks ( );
	string			edge	= getMeshPanel ( )->getEdge ( );
	const int				from	= getMeshPanel ( )->from ( );
	const int				to		= getMeshPanel ( )->to ( );
	cmdResult	= getContext ( ).getMeshManager ( ).newSubVolumeBetweenSheets (blocks, edge, from, to, name);

	setCommandResult (cmdResult);
}	// QtMeshSubVolumeBetweenSheetsAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
