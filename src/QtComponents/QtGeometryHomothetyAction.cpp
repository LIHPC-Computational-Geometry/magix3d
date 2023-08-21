/**
 * \file        QtGeometryHomothetyAction.cpp
 * \author      Charles PIGNEROL
 * \date        26/05/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeometryHomothetyAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "QtComponents/RenderedEntityRepresentation.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"

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
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtGeometryHomothetyPanel
// ===========================================================================

QtGeometryHomothetyPanel::QtGeometryHomothetyPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).geomHomothetyOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).geomHomothetyOperationTag),
	  _geomEntitiesPanel (0), _allEntitiesCheckBox (0), 
	  _operationMethodComboBox (0), _currentPanel (0),
	  _homogeneousPanel (0), _heterogeneousPanel (0),
	  _copyCheckBox(0), _namePanel(0), _withTopologyCheckBox(0)
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

	// Faut il appliquer l'homothétie à toutes les entités ?
	_allEntitiesCheckBox	= 
			new QCheckBox (QString::fromUtf8("Appliquer à toutes les entités\n(géométriques, topologiques et au maillage)"), this);
	_allEntitiesCheckBox->setChecked (false);
	connect (_allEntitiesCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (allEntitiesCallback ( )));
	layout->addWidget (_allEntitiesCheckBox);

	// Les entités à associer :
	FilterEntity::objectType	filter	=
		(FilterEntity::objectType)(FilterEntity::GeomVertex |
		FilterEntity::GeomCurve | FilterEntity::GeomSurface |
			FilterEntity::GeomVolume);
	_geomEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
							this, mainWindow, "Entités géométriques :",
							SelectionManagerIfc::ALL_DIMENSIONS,
							filter, SelectionManagerIfc::D3, true);
	_geomEntitiesPanel->setMultiSelectMode (true);
	connect (_geomEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_geomEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_geomEntitiesPanel);

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

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_geomEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
											)->setLinkedSeizureManagers (0, 0);

	// Faut il faire une copie des entités avant la transformation ?
	_copyCheckBox	=
			new QCheckBox ("Copier avant transformation", this);
	connect (_copyCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (copyCallback ( )));
	_copyCheckBox->setChecked (false);
	layout->addWidget (_copyCheckBox);

	// Nom groupe (en cas de copie):
	_namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe", mainWindow, 3,
			QtMgx3DGroupNamePanel::CREATION, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// Faut il propager la copie aux entités topologiques associées ?
	_withTopologyCheckBox	=
			new QCheckBox (QString::fromUtf8("Copier également la topologie associée"), this);
	_withTopologyCheckBox->setChecked (false);
	layout->addWidget (_withTopologyCheckBox);

	_namePanel->setEnabled (false);
	_withTopologyCheckBox->setEnabled (false);

	operationMethodCallback ( );
}	// QtGeometryHomothetyPanel::QtGeometryHomothetyPanel


QtGeometryHomothetyPanel::QtGeometryHomothetyPanel (
										const QtGeometryHomothetyPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _geomEntitiesPanel (0), _allEntitiesCheckBox (0), 
	  _operationMethodComboBox (0), _currentPanel (0),
	  _homogeneousPanel (0), _heterogeneousPanel (0),
	  _copyCheckBox(0), _namePanel(0), _withTopologyCheckBox(0)
{
	MGX_FORBIDDEN ("QtGeometryHomothetyPanel copy constructor is not allowed.");
}	// QtGeometryHomothetyPanel::QtGeometryHomothetyPanel (const QtGeometryHomothetyPanel&)


QtGeometryHomothetyPanel& QtGeometryHomothetyPanel::operator = (
											const QtGeometryHomothetyPanel&)
{
	MGX_FORBIDDEN ("QtGeometryHomothetyPanel assignment operator is not allowed.");
	return *this;
}	// QtGeometryHomothetyPanel::QtGeometryHomothetyPanel (const QtGeometryHomothetyPanel&)


QtGeometryHomothetyPanel::~QtGeometryHomothetyPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtGeometryHomothetyPanel::~QtGeometryHomothetyPanel


void QtGeometryHomothetyPanel::setDimension (SelectionManagerIfc::DIM dim)
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	_geomEntitiesPanel->clearSelection ( );
	_geomEntitiesPanel->setDimensions (dim);
}	// QtGeometryHomothetyPanel::setDimension


bool QtGeometryHomothetyPanel::processAllGeomEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_allEntitiesCheckBox)
	return _allEntitiesCheckBox->isChecked ( );
}	// QtGeometryHomothetyPanel::processAllGeomEntities


bool QtGeometryHomothetyPanel::copyEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_copyCheckBox)
	return _copyCheckBox->isChecked ( );
}	// QtGeometryHomothetyPanel::copyEntities

string QtGeometryHomothetyPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtGeometryHomothetyPanel::getGroupName

bool QtGeometryHomothetyPanel::copyTopoEntities ( ) const
{
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)
	return _withTopologyCheckBox->isChecked ( );
}	// QtGeometryHomothetyPanel::copyTopoEntities

vector<string> QtGeometryHomothetyPanel::getGeomEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	return _geomEntitiesPanel->getEntitiesNames ( );
}	// QtGeometryHomothetyPanel::getGeomEntitiesNames


QtGeometryHomothetyPanel::HOMOTHETY_TYPE
					QtGeometryHomothetyPanel::getHomothetyType ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	return (QtGeometryHomothetyPanel::HOMOTHETY_TYPE)
									_operationMethodComboBox->currentIndex ( );
}	// QtGeometryHomothetyPanel::getHomothetyType


void QtGeometryHomothetyPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_homogeneousPanel)
	CHECK_NULL_PTR_ERROR (_heterogeneousPanel)
	_namePanel->autoUpdate ( );
	_geomEntitiesPanel->reset ( );
	_homogeneousPanel->reset ( );
	_heterogeneousPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeometryHomothetyPanel::reset


void QtGeometryHomothetyPanel::validate ( )
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
		error << "QtGeometryHomothetyPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getGeomEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGeometryHomothetyPanel::validate


void QtGeometryHomothetyPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	if (0 != _currentPanel)
		_currentPanel->cancel ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_geomEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeometryHomothetyPanel::cancel


void QtGeometryHomothetyPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedGeomEntities	=
					getSelectionManager ( ).getEntitiesNames (
									_geomEntitiesPanel->getAllowedTypes ( ));
		if (0 != selectedGeomEntities.size ( ))
			_geomEntitiesPanel->setEntitiesNames (selectedGeomEntities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_geomEntitiesPanel->clearSelection ( );
	if (0 != _currentPanel)
		_currentPanel->autoUpdate ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_namePanel->autoUpdate ( );
	_geomEntitiesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtGeometryHomothetyPanel::autoUpdate


const QtMgx3DHomogeneousHomothetyPanel&
						QtGeometryHomothetyPanel::getHomogeneousPanel ( ) const
{
	CHECK_NULL_PTR_ERROR (_homogeneousPanel)
	return *_homogeneousPanel;
}	// QtGeometryHomothetyPanel::getHomogeneousPanel


const QtMgx3DHeterogeneousHomothetyPanel&
					QtGeometryHomothetyPanel::getHeterogeneousPanel ( ) const
{
	CHECK_NULL_PTR_ERROR (_heterogeneousPanel)
	return *_heterogeneousPanel;
}	// QtGeometryHomothetyPanel::getHeterogeneousPanel


void QtGeometryHomothetyPanel::operationMethodCallback ( )
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
		case QtGeometryHomothetyPanel::HOMOGENEOUS	:
							_currentPanel	= _homogeneousPanel;	break;
		case QtGeometryHomothetyPanel::HETEROGENEOUS	:
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
}	// QtGeometryHomothetyPanel::operationMethodCallback 


vector<Entity*> QtGeometryHomothetyPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities	= getParametersPanel ( ).getPanelEntities ( );

	const vector<string>	names	= getGeomEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*	entity	=
					getContext ( ).getGeomManager ( ).getEntity (*it, false);
		if (0 != entity)
			entities.push_back (entity);
	}

	return entities;
}	// QtGeometryHomothetyPanel::getInvolvedEntities


void QtGeometryHomothetyPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _geomEntitiesPanel)
		_geomEntitiesPanel->stopSelection ( );
	if (0 != _homogeneousPanel)
		_homogeneousPanel->stopSelection ( );
	if (0 != _heterogeneousPanel)
		_heterogeneousPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeometryHomothetyPanel::operationCompleted


QtMgx3DOperationsSubPanel&
					QtGeometryHomothetyPanel::getParametersPanel ( ) const
{
	CHECK_NULL_PTR_ERROR (_currentPanel)
	return *_currentPanel;
}	// QtGeometryHomothetyPanel::getParametersPanel



void QtGeometryHomothetyPanel::allEntitiesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)
	_geomEntitiesPanel->setEnabled (!processAllGeomEntities ( ));
	_withTopologyCheckBox->setEnabled (
							copyEntities ( ) && !processAllGeomEntities ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeometryHomothetyPanel::allEntitiesCallback


void QtGeometryHomothetyPanel::copyCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)

	_namePanel->setEnabled (copyEntities ( ));
	_withTopologyCheckBox->setEnabled (copyEntities ( ) && !processAllGeomEntities ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeometryHomothetyPanel::copyCallback


// ===========================================================================
//                  LA CLASSE QtGeometryHomothetyAction
// ===========================================================================

QtGeometryHomothetyAction::QtGeometryHomothetyAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeometryHomothetyPanel*	operationPanel	=
		new QtGeometryHomothetyPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeometryHomothetyAction::QtGeometryHomothetyAction


QtGeometryHomothetyAction::QtGeometryHomothetyAction (
										const QtGeometryHomothetyAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeometryHomothetyAction copy constructor is not allowed.")
}	// QtGeometryHomothetyAction::QtGeometryHomothetyAction


QtGeometryHomothetyAction& QtGeometryHomothetyAction::operator = (
										const QtGeometryHomothetyAction&)
{
	MGX_FORBIDDEN ("QtGeometryHomothetyAction assignment operator is not allowed.")
	return *this;
}	// QtGeometryHomothetyAction::operator =


QtGeometryHomothetyAction::~QtGeometryHomothetyAction ( )
{
}	// QtGeometryHomothetyAction::~QtGeometryHomothetyAction


QtGeometryHomothetyPanel*
					QtGeometryHomothetyAction::getHomothetyPanel ( )
{
	return dynamic_cast<QtGeometryHomothetyPanel*>(getOperationPanel ( ));
}	// QtGeometryHomothetyAction::getHomothetyPanel


void QtGeometryHomothetyAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités géométriques :
	QtGeometryHomothetyPanel*	panel	= getHomothetyPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const bool			copy        = panel->copyEntities();
	const string		groupName	= panel->getGroupName ( );
	const bool			withTopo	= panel->copyTopoEntities ( );
	const bool		    processAll	= panel->processAllGeomEntities ( );
	const QtGeometryHomothetyPanel::HOMOTHETY_TYPE	type	=
											panel->getHomothetyType ( );
	GeomManagerIfc&			geomManager	= getContext ( ).getGeomManager ( );
	if (true == processAll)
	{
		if (copy)
		{
			switch (type)
			{
				case QtGeometryHomothetyPanel::HOMOGENEOUS		:
				{
					const double		factor	(panel->getHomogeneousPanel ( ).getHomothetyFactor ( ));
					const Math::Point	center (panel->getHomogeneousPanel ( ).getCenter ( ));
					cmdResult	= geomManager.copyAndScaleAll (factor, center, groupName);
				}	// case QtGeometryHomothetyPanel::HOMOGENEOUS
				break;
				case QtGeometryHomothetyPanel::HETEROGENEOUS	:
				{
					const double	xFactor	(panel->getHeterogeneousPanel( ).getHomothetyXFactor( ));
					const double	yFactor	(panel->getHeterogeneousPanel( ).getHomothetyYFactor( ));
					const double	zFactor	(panel->getHeterogeneousPanel( ).getHomothetyZFactor( ));
					cmdResult	= geomManager.copyAndScaleAll (xFactor, yFactor, zFactor, groupName);
				}	// QtGeometryHomothetyPanel::HETEROGENEOUS
				break;
			}	// switch (type)
		}	// if (copy)
		else
		{
			switch (type)
			{
				case QtGeometryHomothetyPanel::HOMOGENEOUS		:
				{
					const double		factor	(panel->getHomogeneousPanel ( ).getHomothetyFactor ( ));
					const Math::Point	center (panel->getHomogeneousPanel ( ).getCenter ( ));
					cmdResult	= geomManager.scaleAll (factor, center);
				}
				break;
				case QtGeometryHomothetyPanel::HETEROGENEOUS	:
				{
					const double	xFactor	(panel->getHeterogeneousPanel( ).getHomothetyXFactor( ));
					const double	yFactor	(panel->getHeterogeneousPanel( ).getHomothetyYFactor( ));
					const double	zFactor	(panel->getHeterogeneousPanel( ).getHomothetyZFactor( ));
					cmdResult	= geomManager.scaleAll (xFactor, yFactor, zFactor);
				}
				break;
			}	// switch (type)

			// recentre la vue [EB]
			getMainWindow().getGraphicalWidget ( ).getRenderingManager ( ).resetView (true);

		}	// else if (copy)
	}	// if (true == processAll)
	else
	{
		vector<string>	entities	= panel->getGeomEntitiesNames ( );	
		if (copy)
		{
			switch (type)
			{
				case QtGeometryHomothetyPanel::HOMOGENEOUS		:
				{
					const double		factor	(panel->getHomogeneousPanel ( ).getHomothetyFactor ( ));
					const Math::Point	center (panel->getHomogeneousPanel ( ).getCenter ( ));
					cmdResult	= geomManager.copyAndScale (entities, factor, center, withTopo, groupName);
				}	// case QtGeometryHomothetyPanel::HOMOGENEOUS
				break;
				case QtGeometryHomothetyPanel::HETEROGENEOUS	:
				{
					const double	xFactor	(panel->getHeterogeneousPanel( ).getHomothetyXFactor( ));
					const double	yFactor	(panel->getHeterogeneousPanel( ).getHomothetyYFactor( ));
					const double	zFactor	(panel->getHeterogeneousPanel( ).getHomothetyZFactor( ));
					cmdResult	= geomManager.copyAndScale (entities, xFactor, yFactor, zFactor, withTopo, groupName);
				}	// QtGeometryHomothetyPanel::HETEROGENEOUS
				break;
			}	// switch (type)
		}	// if (copy)
		else
		{
			switch (type)
			{
				case QtGeometryHomothetyPanel::HOMOGENEOUS		:
				{
					const double		factor	(panel->getHomogeneousPanel ( ).getHomothetyFactor ( ));
					const Math::Point	center (panel->getHomogeneousPanel ( ).getCenter ( ));
					cmdResult	= geomManager.scale (entities, factor, center);
				}	// case QtGeometryHomothetyPanel::HOMOGENEOUS
				break;
				case QtGeometryHomothetyPanel::HETEROGENEOUS	:
				{
					const double	xFactor	(panel->getHeterogeneousPanel( ).getHomothetyXFactor( ));
					const double	yFactor	(panel->getHeterogeneousPanel( ).getHomothetyYFactor( ));
					const double	zFactor	(panel->getHeterogeneousPanel( ).getHomothetyZFactor( ));
					cmdResult	= geomManager.scale (entities, xFactor, yFactor, zFactor);
				}	// QtGeometryHomothetyPanel::HETEROGENEOUS
				break;
			}	// switch (type)
		}	// else if (copy)
	}	// else if (true == processAll)

	setCommandResult (cmdResult);
}	// QtGeometryHomothetyAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
