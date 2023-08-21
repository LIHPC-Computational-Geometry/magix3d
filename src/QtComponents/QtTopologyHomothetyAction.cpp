/**
 * \file        QtTopologyHomothetyAction.cpp
 * \author      Charles PIGNEROL
 * \date        26/05/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtTopologyHomothetyAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QBoxLayout>

#include <values.h>	// DBL_MAX


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
//                        LA CLASSE QtTopologyHomothetyPanel
// ===========================================================================

QtTopologyHomothetyPanel::QtTopologyHomothetyPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).topoHomothetyOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).topoHomothetyOperationTag),
	  _topoEntitiesPanel (0), _propagateCheckBox (0), 
	  _operationMethodComboBox (0), _currentPanel (0),
	  _homogeneousPanel (0), _heterogeneousPanel (0)
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

	// Les entités à associer :
	FilterEntity::objectType	filter	=
		(FilterEntity::objectType)(FilterEntity::TopoVertex |
		FilterEntity::TopoCoEdge | FilterEntity::TopoCoFace |
			FilterEntity::TopoBlock);
	_topoEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
							this, mainWindow, "Entités topologiques :", 
							SelectionManagerIfc::ALL_DIMENSIONS,
							filter, SelectionManagerIfc::D3, true);
	_topoEntitiesPanel->setMultiSelectMode (true);
	connect (_topoEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_topoEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_topoEntitiesPanel);

	// Faut il propager l'homothétie aux entités géométriques ?
	_propagateCheckBox	=
			new QCheckBox (QString::fromUtf8("Propager aux entités géométriques"), this);
	_propagateCheckBox->setChecked (true);
	layout->addWidget (_propagateCheckBox);

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
											)->setLinkedSeizureManagers (0, 0);

	// Les différents types d'homothétie :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	label	= new QLabel ("Type", this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem ("HOMOGENE");
	_operationMethodComboBox->addItem ("HETEROGENE");
	_operationMethodComboBox->setCurrentIndex (0);
	connect (_operationMethodComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (operationMethodCallback ( )));
	hlayout->addWidget (_operationMethodComboBox);
	// Homogène :
	_homogeneousPanel	=
			new QtMgx3DHomogeneousHomothetyPanel (this, panelName, mainWindow);
	connect (&_homogeneousPanel->getFactorTextField ( ),
	         SIGNAL (textEdited (const QString&)),
	         this, SLOT (parametersModifiedCallback ( )));
	connect (&_homogeneousPanel->getCenterPanel ( ), SIGNAL (pointModified ( )),
	         this, SLOT (parametersModifiedCallback ( )));
	_homogeneousPanel->hide ( );
	layout->addWidget (_homogeneousPanel);
	// Hétérogène :
	_heterogeneousPanel	=
		new QtMgx3DHeterogeneousHomothetyPanel (this, panelName, mainWindow);
	connect (&_heterogeneousPanel->getXFactorTextField ( ),
	         SIGNAL (textEdited (const QString&)),
	         this, SLOT (parametersModifiedCallback ( )));
	connect (&_heterogeneousPanel->getYFactorTextField ( ),
	         SIGNAL (textEdited (const QString&)),
	         this, SLOT (parametersModifiedCallback ( )));
	connect (&_heterogeneousPanel->getZFactorTextField ( ),
	         SIGNAL (textEdited (const QString&)),
	         this, SLOT (parametersModifiedCallback ( )));
	_heterogeneousPanel->hide ( );
	layout->addWidget (_heterogeneousPanel);

	layout->addStretch (2);

	operationMethodCallback ( );
}	// QtTopologyHomothetyPanel::QtTopologyHomothetyPanel


QtTopologyHomothetyPanel::QtTopologyHomothetyPanel (
										const QtTopologyHomothetyPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _topoEntitiesPanel (0), _propagateCheckBox (0),
	  _operationMethodComboBox (0), _currentPanel (0),
	  _homogeneousPanel (0), _heterogeneousPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyHomothetyPanel copy constructor is not allowed.");
}	// QtTopologyHomothetyPanel::QtTopologyHomothetyPanel (const QtTopologyHomothetyPanel&)


QtTopologyHomothetyPanel& QtTopologyHomothetyPanel::operator = (
											const QtTopologyHomothetyPanel&)
{
	MGX_FORBIDDEN ("QtTopologyHomothetyPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyHomothetyPanel::QtTopologyHomothetyPanel (const QtTopologyHomothetyPanel&)


QtTopologyHomothetyPanel::~QtTopologyHomothetyPanel ( )
{
	preview (false, true);
}	// QtTopologyHomothetyPanel::~QtTopologyHomothetyPanel


void QtTopologyHomothetyPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_homogeneousPanel)
	CHECK_NULL_PTR_ERROR (_heterogeneousPanel)
	_topoEntitiesPanel->reset ( );
	_homogeneousPanel->reset ( );
	_heterogeneousPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyHomothetyPanel::reset


QtTopologyHomothetyPanel::HOMOTHETY_TYPE
					QtTopologyHomothetyPanel::getHomothetyType ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	return (QtTopologyHomothetyPanel::HOMOTHETY_TYPE)
									_operationMethodComboBox->currentIndex ( );
}	// QtTopologyHomothetyPanel::getHomothetyType


void QtTopologyHomothetyPanel::setDimension (SelectionManagerIfc::DIM dim)
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	_topoEntitiesPanel->clearSelection ( );
	_topoEntitiesPanel->setDimensions (dim);
}	// QtTopologyHomothetyPanel::setDimension


vector<string> QtTopologyHomothetyPanel::getTopoEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	return _topoEntitiesPanel->getEntitiesNames ( );
}	// QtTopologyHomothetyPanel::getTopoEntitiesNames


bool QtTopologyHomothetyPanel::doPropagate ( ) const
{
	CHECK_NULL_PTR_ERROR (_propagateCheckBox)
	return Qt::Checked == _propagateCheckBox->checkState ( ) ? true : false;
}	// QtTopologyHomothetyPanel::doPropagate


void QtTopologyHomothetyPanel::validate ( )
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
		error << "QtTopologyHomothetyPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getTopoEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopologyHomothetyPanel::validate


void QtTopologyHomothetyPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	_topoEntitiesPanel->stopSelection ( );
	if (0 != _currentPanel)
		_currentPanel->cancel ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_topoEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyHomothetyPanel::cancel


void QtTopologyHomothetyPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedTopoEntities	=
					getSelectionManager ( ).getEntitiesNames (
									_topoEntitiesPanel->getAllowedTypes ( ));
		if (0 != selectedTopoEntities.size ( ))
			_topoEntitiesPanel->setEntitiesNames (selectedTopoEntities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_topoEntitiesPanel->clearSelection ( );
	if (0 != _currentPanel)
		_currentPanel->autoUpdate ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_topoEntitiesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyHomothetyPanel::autoUpdate


const QtMgx3DHomogeneousHomothetyPanel&
						QtTopologyHomothetyPanel::getHomogeneousPanel ( ) const
{
	CHECK_NULL_PTR_ERROR (_homogeneousPanel)
	return *_homogeneousPanel;
}	// QtTopologyHomothetyPanel::getHomogeneousPanel


const QtMgx3DHeterogeneousHomothetyPanel&
						QtTopologyHomothetyPanel::getHeterogeneousPanel ( ) const
{
	CHECK_NULL_PTR_ERROR (_heterogeneousPanel)
	return *_heterogeneousPanel;
}	// QtTopologyHomothetyPanel::getHeterogeneousPanel


vector<Entity*> QtTopologyHomothetyPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities	= getParametersPanel ( ).getPanelEntities ( );

	const vector<string>	names	= getTopoEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*	entity	=
					getContext ( ).getTopoManager ( ).getEntity (*it, false);
		if (0 != entity)
			entities.push_back (entity);
	}

	return entities;
}	// QtTopologyHomothetyPanel::getInvolvedEntities


QtMgx3DOperationsSubPanel& QtTopologyHomothetyPanel::getParametersPanel ( ) const
{
	CHECK_NULL_PTR_ERROR (_currentPanel)
	return *_currentPanel;
}	// QtTopologyHomothetyPanel::getParametersPanel


void QtTopologyHomothetyPanel::operationMethodCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	CHECK_NULL_PTR_ERROR (_homogeneousPanel)
	CHECK_NULL_PTR_ERROR (_heterogeneousPanel)
	if (0 != _currentPanel)
	{
		_currentPanel->hide ( );
	}	// if (0 != _currentPanel)
	_currentPanel	= 0;

	switch (getHomothetyType ( ))
	{
		case QtTopologyHomothetyPanel::HOMOGENEOUS	:
							_currentPanel	= _homogeneousPanel;	break;
		case QtTopologyHomothetyPanel::HETEROGENEOUS	:
							_currentPanel	= _heterogeneousPanel;	break;
		default										:
			throw Exception (UTF8String ("Type d'homothétie non supporté.", Charset::UTF_8));
	}	// switch (getHomothetyType ( ))

	if (0 != _currentPanel)
	{
		_currentPanel->show ( );
		if (0 != getMainWindow ( ))
			getMainWindow ( )->getOperationsPanel ( ).updateLayoutWorkaround( );
	}	// if (0 != _currentPanel)

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologyHomothetyPanel::operationMethodCallback 


void QtTopologyHomothetyPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _topoEntitiesPanel)
		_topoEntitiesPanel->stopSelection ( );
	if (0 != _homogeneousPanel)
		_homogeneousPanel->stopSelection ( );
	if (0 != _heterogeneousPanel)
		_heterogeneousPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyHomothetyPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyHomothetyAction
// ===========================================================================

QtTopologyHomothetyAction::QtTopologyHomothetyAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyHomothetyPanel*	operationPanel	=
		new QtTopologyHomothetyPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyHomothetyAction::QtTopologyHomothetyAction


QtTopologyHomothetyAction::QtTopologyHomothetyAction (
										const QtTopologyHomothetyAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyHomothetyAction copy constructor is not allowed.")
}	// QtTopologyHomothetyAction::QtTopologyHomothetyAction


QtTopologyHomothetyAction& QtTopologyHomothetyAction::operator = (
										const QtTopologyHomothetyAction&)
{
	MGX_FORBIDDEN ("QtTopologyHomothetyAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyHomothetyAction::operator =


QtTopologyHomothetyAction::~QtTopologyHomothetyAction ( )
{
}	// QtTopologyHomothetyAction::~QtTopologyHomothetyAction


QtTopologyHomothetyPanel*
					QtTopologyHomothetyAction::getHomothetyPanel ( )
{
	return dynamic_cast<QtTopologyHomothetyPanel*>(getOperationPanel ( ));
}	// QtTopologyHomothetyAction::getHomothetyPanel


void QtTopologyHomothetyAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités topologiques :
	QtTopologyHomothetyPanel*	panel	= getHomothetyPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>		entities	= panel->getTopoEntitiesNames ( );
	bool				propagate	= panel->doPropagate ( );
	switch (panel->getHomothetyType ( ))
	{
		case QtTopologyHomothetyPanel::HOMOGENEOUS	:
		{
			const double	factor	= panel->getHomogeneousPanel ( ).getHomothetyFactor ( );
			const Point	center	= panel->getHomogeneousPanel ( ).getCenter ( );
			cmdResult	= getContext ( ).getTopoManager ( ).scale (entities, factor, center, propagate);
		}	// QtTopologyHomothetyPanel::HOMOGENEOUS
		break;
		case QtTopologyHomothetyPanel::HETEROGENEOUS	:
		{
			const double	xFactor	= panel->getHeterogeneousPanel ( ).getHomothetyXFactor ( );
			const double	yFactor	= panel->getHeterogeneousPanel ( ).getHomothetyYFactor ( );
			const double	zFactor	= panel->getHeterogeneousPanel ( ).getHomothetyZFactor ( );
			cmdResult	= getContext ( ).getTopoManager ( ).scale (entities, xFactor, yFactor, zFactor, propagate);
		}	// QtTopologyHomothetyPanel::HETEROGENEOUS
		break;
		default											:
		{
			INTERNAL_ERROR (exc, "Type d'homothétie non supporté.", "QtTopologyHomothetyAction::executeOperation")
			throw exc;
		}
	}	// switch (panel->getHomothetyType ( ))

	setCommandResult (cmdResult);
}	// QtTopologyHomothetyAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
