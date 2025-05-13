/**
 * \file        QtBlockMeshingPropertyAction.cpp
 * \author      Charles PIGNEROL
 * \date        14/11/2013
 */

#include "Internal/Context.h"
#include "Internal/Resources.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/MgxText.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtBlockMeshingPropertyAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtBlockTransfinitePanel
// ===========================================================================


QtBlockTransfinitePanel::QtBlockTransfinitePanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);
}	// QtBlockTransfinitePanel::QtBlockTransfinitePanel


QtBlockTransfinitePanel::QtBlockTransfinitePanel (
											const QtBlockTransfinitePanel& p)
	  : QtMgx3DOperationsSubPanel (p)
{
	MGX_FORBIDDEN ("QtBlockTransfinitePanel copy constructor is not allowed.");
}	// QtBlockTransfinitePanel::QtBlockTransfinitePanel


QtBlockTransfinitePanel& QtBlockTransfinitePanel::operator = (
												const QtBlockTransfinitePanel&)
{
	 MGX_FORBIDDEN ("QtBlockTransfinitePanel assignment operator is not allowed.");
	return *this;
}	// QtBlockTransfinitePanel::operator =


QtBlockTransfinitePanel::~QtBlockTransfinitePanel ( )
{
}	// QtBlockTransfinitePanel::~QtBlockTransfinitePanel


BlockMeshingPropertyTransfinite* QtBlockTransfinitePanel::getMeshingProperty ( ) const
{
	return new BlockMeshingPropertyTransfinite ( );
}	// QtBlockTransfinitePanel::getMeshingProperty


// ===========================================================================
//                        LA CLASSE QtBlockDelaunayTetgenPanel
// ===========================================================================


QtBlockDelaunayTetgenPanel::QtBlockDelaunayTetgenPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel),
	  _volMaxTextField (0), _qualityFactorTextField (0), _ratioPyramiSizeTextField(0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);

	BlockMeshingPropertyDelaunayTetgen	props;	// Pour récupérer les valeurs par défaut

	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label	= new QLabel ("Volume maximum :", this);
	hlayout->addWidget (label);
	_volMaxTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_volMaxTextField->setValue (props.getVolumeMax ( ));
	hlayout->addWidget (_volMaxTextField);
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Facteur de qualité :"), this);
	hlayout->addWidget (label);
	_qualityFactorTextField	=
					&QtNumericFieldsFactory::createDistanceTextField (this);
	_qualityFactorTextField->setValue (props.getRadiusEdgeRatio ( ));
	hlayout->addWidget (_qualityFactorTextField);

	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel ("Ration hauteur pyramides :", this);
	hlayout->addWidget (label);
	_ratioPyramiSizeTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_ratioPyramiSizeTextField->setValue (props.getRatioPyramidSize ( ));
	hlayout->addWidget (_ratioPyramiSizeTextField);

}	// QtBlockDelaunayTetgenPanel::QtBlockDelaunayTetgenPanel


QtBlockDelaunayTetgenPanel::QtBlockDelaunayTetgenPanel (
											const QtBlockDelaunayTetgenPanel& p)
	  : QtMgx3DOperationsSubPanel (p),
	    _volMaxTextField (0), _qualityFactorTextField (0), _ratioPyramiSizeTextField(0)
{
	MGX_FORBIDDEN ("QtBlockDelaunayTetgenPanel copy constructor is not allowed.");
}	// QtBlockDelaunayTetgenPanel::QtBlockDelaunayTetgenPanel


QtBlockDelaunayTetgenPanel& QtBlockDelaunayTetgenPanel::operator = (
											const QtBlockDelaunayTetgenPanel&)
{
	 MGX_FORBIDDEN ("QtBlockDelaunayTetgenPanel assignment operator is not allowed.");
	return *this;
}	// QtBlockDelaunayTetgenPanel::operator =


QtBlockDelaunayTetgenPanel::~QtBlockDelaunayTetgenPanel ( )
{
}	// QtBlockDelaunayTetgenPanel::~QtBlockDelaunayTetgenPanel


BlockMeshingPropertyDelaunayTetgen*
						QtBlockDelaunayTetgenPanel::getMeshingProperty ( ) const
{
	return new BlockMeshingPropertyDelaunayTetgen (
										getQualityFactor ( ), getVolMax ( ), getRatioPyramidSize());
}	// QtBlockDelaunayTetgenPanel::getMeshingProperty


double QtBlockDelaunayTetgenPanel::getVolMax ( ) const
{
	CHECK_NULL_PTR_ERROR (_volMaxTextField)
	return _volMaxTextField->getValue ( );
}	// QtBlockDelaunayTetgenPanel::getVolMax


double QtBlockDelaunayTetgenPanel::getQualityFactor ( ) const
{
	CHECK_NULL_PTR_ERROR (_qualityFactorTextField)
	return _qualityFactorTextField->getValue ( );
}	// QtBlockDelaunayTetgenPanel::getQualityFactor

double QtBlockDelaunayTetgenPanel::getRatioPyramidSize ( ) const
{
	CHECK_NULL_PTR_ERROR (_ratioPyramiSizeTextField)
	return _ratioPyramiSizeTextField->getValue ( );
}	// QtBlockDelaunayTetgenPanel::getRatioPyramidSize


void QtBlockDelaunayTetgenPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	BlockMeshingPropertyDelaunayTetgen	props;	// Pour récupérer les valeurs par défaut
	CHECK_NULL_PTR_ERROR (_volMaxTextField)
	CHECK_NULL_PTR_ERROR (_qualityFactorTextField)
	CHECK_NULL_PTR_ERROR (_ratioPyramiSizeTextField)
	_volMaxTextField->setValue (props.getVolumeMax ( ));
	_qualityFactorTextField->setValue (props.getRadiusEdgeRatio ( ));
	_ratioPyramiSizeTextField->setValue (props.getRatioPyramidSize());

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtBlockDelaunayTetgenPanel::reset


// ===========================================================================
//                        LA CLASSE QtBlockMeshingPropertyPanel
// ===========================================================================

QtBlockMeshingPropertyPanel::QtBlockMeshingPropertyPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).blocsMeshingPropertyOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).blocsMeshingPropertyOperationTag),
	  _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _transfinitePanel (0),
	  _delaunayTetgenPanel (0),
	  _blocksPanel (0)
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

	// Les blocs à mailler :
	_blocksPanel	= new QtMgx3DEntityPanel (
						this, "", true, "Blocs :", "", &mainWindow,
						SelectionManagerIfc::D3, FilterEntity::TopoBlock);
	_blocksPanel->setMultiSelectMode (true);
	layout->addWidget (_blocksPanel);
	connect (_blocksPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_blocksPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// Méthode de maillage des blocs :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Méthode"), this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem ("Maillage transfini");
	_operationMethodComboBox->addItem ("Maillage Delaunay (Tetgen)");
	connect (_operationMethodComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (operationMethodCallback ( )));
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (10);
	
	// Définition de la maillage :
	QtGroupBox*		groupBox	=
						new QtGroupBox (this, "Paramètres de maillage");
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	groupBox->setLayout (vlayout);
//	groupBox->setContentsMargins (0, 10, 0, 0);
	groupBox->layout ( )->setContentsMargins (
				Resources::instance ( )._margin.getValue ( ),
				Resources::instance ( )._margin.getValue ( ),
				Resources::instance ( )._margin.getValue ( ),
				Resources::instance ( )._margin.getValue ( ));
	groupBox->layout ( )->setSpacing (Resources::instance ( )._spacing.getValue( ));
	layout->addWidget (groupBox);
	// Le panneau "méthode" : plusieurs panneaux possibles.
	// On garde un parent unique dans la layout :
	_currentParentWidget	= new QWidget (groupBox);
	vlayout->addWidget (_currentParentWidget);
	QHBoxLayout*	currentLayout = new QHBoxLayout (_currentParentWidget);
	_currentParentWidget->setLayout (currentLayout);
	_transfinitePanel	= new QtBlockTransfinitePanel (0, mainWindow, this);
//	_transfinitePanel->layout ( )->setSpacing (5);
//	_transfinitePanel->layout ( )->setContentsMargins (0, 0, 0, 0);
	_transfinitePanel->hide ( );
	_delaunayTetgenPanel	=
					new QtBlockDelaunayTetgenPanel (0, mainWindow, this);
	_delaunayTetgenPanel->hide ( );

	addPreviewCheckBox (true);

	operationMethodCallback ( );

	vlayout->addStretch (2);
	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_blocksPanel->getNameTextField ( ))
}	// QtBlockMeshingPropertyPanel::QtBlockMeshingPropertyPanel


QtBlockMeshingPropertyPanel::QtBlockMeshingPropertyPanel (
										const QtBlockMeshingPropertyPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _transfinitePanel (0),
	  _delaunayTetgenPanel (0),
	  _blocksPanel (0)
{
	MGX_FORBIDDEN ("QtBlockMeshingPropertyPanel copy constructor is not allowed.");
}	// QtBlockMeshingPropertyPanel::QtBlockMeshingPropertyPanel (const QtBlockMeshingPropertyPanel&)


QtBlockMeshingPropertyPanel& QtBlockMeshingPropertyPanel::operator = (
											const QtBlockMeshingPropertyPanel&)
{
	MGX_FORBIDDEN ("QtBlockMeshingPropertyPanel assignment operator is not allowed.");
	return *this;
}	// QtBlockMeshingPropertyPanel::QtBlockMeshingPropertyPanel (const QtBlockMeshingPropertyPanel&)


QtBlockMeshingPropertyPanel::~QtBlockMeshingPropertyPanel ( )
{
}	// QtBlockMeshingPropertyPanel::~QtBlockMeshingPropertyPanel


void QtBlockMeshingPropertyPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_transfinitePanel)
	CHECK_NULL_PTR_ERROR (_delaunayTetgenPanel)
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	_transfinitePanel->reset ( );
	_delaunayTetgenPanel->reset ( );
	_blocksPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtBlockMeshingPropertyPanel::reset


QtBlockMeshingPropertyPanel::OPERATION_METHOD
						QtBlockMeshingPropertyPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtBlockMeshingPropertyPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtBlockMeshingPropertyPanel::getOperationMethod


BlockMeshingProperty* QtBlockMeshingPropertyPanel::getMeshingProperty ( ) const
{
	CHECK_NULL_PTR_ERROR (_transfinitePanel)
	CHECK_NULL_PTR_ERROR (_delaunayTetgenPanel)
	switch (getOperationMethod ( ))
	{
		case QtBlockMeshingPropertyPanel::TRANSFINITE				:
			return _transfinitePanel->getMeshingProperty ( );
		case QtBlockMeshingPropertyPanel::DELAUNAY_TETGEN			:
			return _delaunayTetgenPanel->getMeshingProperty ( );
	}	// switch (getOperationMethod ( ))

	INTERNAL_ERROR (exc, "Méthode de maillage de blocs non supportée", "QtBlockMeshingPropertyPanel::getMeshingProperty")
	throw exc;
}	// QtBlockMeshingPropertyPanel::getMeshingProperty


vector<string> QtBlockMeshingPropertyPanel::getBlocksNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	return _blocksPanel->getUniqueNames ( );
}	// QtBlockMeshingPropertyPanel::getBlocksNames


void QtBlockMeshingPropertyPanel::setBlocksNames (const vector<string>& names)
{
	CHECK_NULL_PTR_ERROR (_blocksPanel)
	_blocksPanel->setUniqueNames (names);
}	// QtBlockMeshingPropertyPanel::setBlocksNames


void QtBlockMeshingPropertyPanel::validate ( )
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
		error << "QtBlockMeshingPropertyPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtBlockMeshingPropertyPanel::TRANSFINITE			:
		case QtBlockMeshingPropertyPanel::DELAUNAY_TETGEN		:
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode de maillage de blocs sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtBlockMeshingPropertyPanel::validate : index de méthode "
			      << "d'opération de maillage de blocs invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtBlockMeshingPropertyPanel::validate


void QtBlockMeshingPropertyPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	if (0 != _blocksPanel)
		_blocksPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		if (0 != _blocksPanel)
			_blocksPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
	
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _currentPanel)
		_currentPanel->cancel ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtBlockMeshingPropertyPanel::cancel


void QtBlockMeshingPropertyPanel::autoUpdate ( )
{
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedBlocks	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoBlock);
		if (0 != selectedBlocks.size ( ))
			setBlocksNames (selectedBlocks);

		if (0 != _currentPanel)
			_currentPanel->autoUpdate ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	if (0 != _blocksPanel)
		_blocksPanel->setUniqueName ("");
	if (0 != _currentPanel)
		_currentPanel->autoUpdate ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	if (0 != _blocksPanel)
		_blocksPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtBlockMeshingPropertyPanel::autoUpdate


void QtBlockMeshingPropertyPanel::preview (bool show, bool destroyInteractor)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// N'afficher une boite de dialogue d'erreur que si demandé ...
	QT_PREVIEW_BEGIN_TRY_CATCH_BLOCK

	QtMgx3DOperationPanel::preview (show, destroyInteractor);

	if (0 != _currentPanel)
		_currentPanel->preview (show, destroyInteractor);

	QT_PREVIEW_COMPLETE_TRY_CATCH_BLOCK

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtBlockMeshingPropertyPanel::preview


vector<Entity*> QtBlockMeshingPropertyPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	vector<string>	names	= getBlocksNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*	entity	= getContext ( ).getTopoManager ( ).getEntity (*it);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}

	return entities;
}	// QtBlockMeshingPropertyPanel::getInvolvedEntities


void QtBlockMeshingPropertyPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _currentPanel)
		_currentPanel->stopSelection ( );
	if (0 != _blocksPanel)
		_blocksPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtBlockMeshingPropertyPanel::operationCompleted


void QtBlockMeshingPropertyPanel::operationMethodCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_blocksPanel)
	CHECK_NULL_PTR_ERROR (_blocksPanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_transfinitePanel)
	CHECK_NULL_PTR_ERROR (_delaunayTetgenPanel)
	if (0 != _currentPanel)
	{
		_currentParentWidget->layout ( )->removeWidget (_currentPanel);
		_currentPanel->hide ( );
		_currentPanel->setParent (0);
	}
	_currentPanel	= 0;

	// Pour la majorité des cas :
	_blocksPanel->getNameTextField ( )->setLinkedSeizureManagers (0, 0);

	switch (getOperationMethod ( ))
	{
		case QtBlockMeshingPropertyPanel::TRANSFINITE			:
			_currentPanel	= _transfinitePanel;			break;
		case QtBlockMeshingPropertyPanel::DELAUNAY_TETGEN			:
			_currentPanel	= _delaunayTetgenPanel;			break;
		default	:
		{
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Méthode d'opération non supportée ("
			        << (unsigned long)getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtBlockMeshingPropertyPanel::operationMethodCallback")
			throw exc;
		}
	}	// switch (getOperationMethod ( ))
	if (0 != _currentPanel)
	{
		_currentPanel->setParent (_currentParentWidget);
		_currentParentWidget->layout ( )->addWidget (_currentPanel);
		_currentPanel->show ( );
		if (0 != getMainWindow ( ))
			getMainWindow ( )->getOperationsPanel ( ).updateLayoutWorkaround( );
	}

	try
	{
		if (0 != getMainWindow ( ))
			getMainWindow ( )->getGraphicalWidget ( ).getRenderingManager (
															).forceRender ( );
	}
	catch (...)
	{
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtBlockMeshingPropertyPanel::operationMethodCallback


// ===========================================================================
//                  LA CLASSE QtBlockMeshingPropertyAction
// ===========================================================================

QtBlockMeshingPropertyAction::QtBlockMeshingPropertyAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtBlockMeshingPropertyPanel*	operationPanel	=
		new QtBlockMeshingPropertyPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtBlockMeshingPropertyAction::QtBlockMeshingPropertyAction


QtBlockMeshingPropertyAction::QtBlockMeshingPropertyAction (
										const QtBlockMeshingPropertyAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtBlockMeshingPropertyAction copy constructor is not allowed.")
}	// QtBlockMeshingPropertyAction::QtBlockMeshingPropertyAction


QtBlockMeshingPropertyAction& QtBlockMeshingPropertyAction::operator = (
										const QtBlockMeshingPropertyAction&)
{
	MGX_FORBIDDEN ("QtBlockMeshingPropertyAction assignment operator is not allowed.")
	return *this;
}	// QtBlockMeshingPropertyAction::operator =


QtBlockMeshingPropertyAction::~QtBlockMeshingPropertyAction ( )
{
}	// QtBlockMeshingPropertyAction::~QtBlockMeshingPropertyAction


QtBlockMeshingPropertyPanel*
					QtBlockMeshingPropertyAction::getMeshingPropertyPanel ( )
{
	return dynamic_cast<QtBlockMeshingPropertyPanel*>(getOperationPanel ( ));
}	// QtBlockMeshingPropertyAction::getMeshingPropertyPanel


void QtBlockMeshingPropertyAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres de maillage des blocs :
	QtBlockMeshingPropertyPanel*	panel	= getMeshingPropertyPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>					blocks		= panel->getBlocksNames ( );
	unique_ptr<BlockMeshingProperty>	properties (panel->getMeshingProperty ( ));
	cmdResult	= getContext ( ).getTopoManager ( ).setMeshingProperty (*properties.get ( ), blocks);

	setCommandResult (cmdResult);
}	// QtBlockMeshingPropertyAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
