/**
 * \file        QtMeshExplorerOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        29/01/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Mesh/MeshManagerIfc.h"
#include "QtComponents/QtMeshExplorerOperationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QVBoxLayout>


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
//                        LA CLASSE QtMeshExplorerOperationPanel
// ===========================================================================

QtMeshExplorerOperationPanel::QtMeshExplorerOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			const string& edgeName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMeshSheetOperationPanel (
			0/*parent*/, panelName, creationPolicy, edgeName,
			mainWindow, action, helpURL, helpTag),
	  _edgesStepTextField (0), _previousButton (0), _nextButton (0),
	  _direction (1),
	  _displayButton (0), _hideButton (0), _reinitializeButton (0),
	  _hidden (false)
{
	setWindowTitle (panelName.c_str ( ));
	mainWindow.registerAdditionalOperationPanel (*this);

	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	hlayout->setSpacing (QtConfiguration::spacing);
	hlayout->setContentsMargins (
					QtConfiguration::margin, QtConfiguration::margin,
					QtConfiguration::margin, QtConfiguration::margin);
	getVLayout ( ).addLayout (hlayout);
	QLabel*	label	= new QLabel ("Pas", this);
	hlayout->addWidget (label, 1);
	_previousButton	= new QToolButton (this);
	_previousButton->setArrowType (Qt::LeftArrow);
	_previousButton->setFixedSize (_previousButton->sizeHint ( ));
	connect (_previousButton, SIGNAL (clicked (bool)), this,
	         SLOT (previousCallback ( )));
	hlayout->addWidget (_previousButton);
	_edgesStepTextField	= new QtIntTextField (this);
	_edgesStepTextField->setValue(1);
	_edgesStepTextField->setRange (1, 100000);
	_edgesStepTextField->setVisibleColumns (6);
	hlayout->addWidget (_edgesStepTextField, 1);
	_edgesStepTextField->setFixedSize (_edgesStepTextField->sizeHint ( ));
	_nextButton	= new QToolButton (this);
	_nextButton->setArrowType (Qt::RightArrow);
	_nextButton->setFixedSize (_nextButton->sizeHint ( ));
	connect (_nextButton, SIGNAL (clicked (bool)), this,
	         SLOT (nextCallback ( )));
	hlayout->addWidget (_nextButton);
	hlayout->addStretch (10);

	// Les boutons :
	hlayout	= new QHBoxLayout (0);
	hlayout->setSpacing (QtConfiguration::spacing);
	hlayout->setContentsMargins (
					QtConfiguration::margin, QtConfiguration::margin,
					QtConfiguration::margin, QtConfiguration::margin);
	getVLayout ( ).addLayout (hlayout);
	hlayout->addStretch (1000);
	_displayButton	= new QPushButton("Afficher", this);
	hlayout->addWidget (_displayButton, 0);
	connect (_displayButton, SIGNAL (clicked (bool)), this,
	         SLOT (applyCallback ( )));
	_hideButton	= new QPushButton("Masquer", this);
	hlayout->addWidget (_hideButton, 0);
	connect (_hideButton, SIGNAL (clicked (bool)), this,
	         SLOT (hideCallback ( )));
	_reinitializeButton	= new QPushButton(QString::fromUtf8("Réinitialiser"), this);
	hlayout->addWidget (_reinitializeButton, 0);
	connect (_reinitializeButton, SIGNAL (clicked (bool)), this,
	         SLOT (autoUpdateCallback ( )));
	QPushButton*	closeButton	= new QPushButton("Fermer", this);
	hlayout->addWidget (closeButton, 0);
	connect (closeButton, SIGNAL (clicked (bool)), this,
	         SLOT (closeCallback ( )));
	QPushButton*	helpButton	 = new QPushButton("Aide ...", this);
	hlayout->addWidget (helpButton, 0);
	connect (helpButton, SIGNAL (clicked (bool)), this, SLOT (helpCallback( )));

	getVLayout ( ).addStretch (1000);

	autoUpdate ( );
}	// QtMeshExplorerOperationPanel::QtMeshExplorerOperationPanel


QtMeshExplorerOperationPanel::QtMeshExplorerOperationPanel (
										const QtMeshExplorerOperationPanel& cao)
	: QtMeshSheetOperationPanel (
				0, "Invalid name", QtMgx3DGroupNamePanel::CREATION,
				"Invalid edge", *new QtMgx3DMainWindow (0),
				0, "", ""),
	  _edgesStepTextField (0), _previousButton (0), _nextButton (0),
	  _direction (1),
	  _displayButton (0), _hideButton (0), _reinitializeButton (0),
	  _hidden (false)
{
	MGX_FORBIDDEN ("QtMeshExplorerOperationPanel copy constructor is not allowed.");
}	// QtMeshExplorerOperationPanel::QtMeshExplorerOperationPanel (const QtMeshExplorerOperationPanel&)


QtMeshExplorerOperationPanel& QtMeshExplorerOperationPanel::operator = (
											const QtMeshExplorerOperationPanel&)
{
	MGX_FORBIDDEN ("QtMeshExplorerOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtMeshExplorerOperationPanel::QtMeshExplorerOperationPanel (const QtMeshExplorerOperationPanel&)


QtMeshExplorerOperationPanel::~QtMeshExplorerOperationPanel ( )
{
	if (0 != getMainWindow ( ))
		getMainWindow ( )->unregisterAdditionalOperationPanel (*this);

	if (0 != getMgx3DOperationAction ( ))
	{
		// Ugly code :
		delete getMgx3DOperationAction ( );
	}
}	// QtMeshExplorerOperationPanel::~QtMeshExplorerOperationPanel


void QtMeshExplorerOperationPanel::setVisible (bool visible)
{
	if (0 != parent ( ))
		QWidget::setVisible (visible);	// Panneau additionnel
	else
		QtMeshSheetOperationPanel::setVisible (visible);	// API operations
}	// QtMeshExplorerOperationPanel::setVisible


long QtMeshExplorerOperationPanel::getEdgesStep ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgesStepTextField)
	return _edgesStepTextField->getValue ( );
}	// QtMeshExplorerOperationPanel::getEdgesStep


long QtMeshExplorerOperationPanel::getDirection ( ) const
{
	return _direction;
}	// QtMeshExplorerOperationPanel::getDirection


void QtMeshExplorerOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMeshSheetOperationPanel::reset ( );
}	// QtMeshExplorerOperationPanel::reset


void QtMeshExplorerOperationPanel::validate ( )
{
// CP : suite discussion EBL/FL, il est convenu que la validation des
// paramètres de l'opération est effectuée par le "noyau" et qu'un mauvais
// paramétrage est remonté sous forme d'exception à la fonction appelante, donc
// avant exécution de la commande.
// Les validations des valeurs des paramètres sont donc ici commentées.
	UTF8String	error (Charset::UTF_8);

	try
	{
		QtMeshSheetOperationPanel::validate ( );
	}
	catch (const Exception& exc)
	{
		error << exc.getFullMessage ( );
	}
	catch (...)
	{
		error << "QtMeshExplorerOperationPanel::validate : erreur non documentée.";
	}
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtMeshExplorerOperationPanel::validate


void QtMeshExplorerOperationPanel::cancel ( )
{
	QtMeshSheetOperationPanel::cancel ( );
}	// QtMeshExplorerOperationPanel::cancel


void QtMeshExplorerOperationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_edgesStepTextField)
	_edgesStepTextField->setValue (1);

	QtMeshSheetOperationPanel::autoUpdate ( );
}	// QtMeshExplorerOperationPanel::autoUpdate


void QtMeshExplorerOperationPanel::applyCallback ( )
{
	try
	{
		if (true == _hidden)
		{
			if (0 != getMgx3DOperationAction ( ))
			{
				getMgx3DOperationAction ( )->setActivated (true);
				_hidden	= false;
			}	// if (0 != getMgx3DOperationAction ( ))

			return;
		}	// if (true == _hidden)

		// on masque les volumes de mailles pour rendre le feuillet visible [EB]
		if (0 != getMainWindow ( )
				&& getMainWindow ( )->getGroupsPanel()
				.getEntityTypeItem(Utils::FilterEntity::MeshVolume)
				->checkState(0) == Qt::Checked){
			getMainWindow ( )->getGroupsPanel().
					getEntityTypeItem(Utils::FilterEntity::MeshVolume)
					->setCheckState (0, Qt::Unchecked);
			getMainWindow ( )->getGroupsPanel().
					entitiesTypesStateChangeCallback(
							getMainWindow ( )->getGroupsPanel().
							getEntityTypeItem(Utils::FilterEntity::MeshVolume), 0);
		}


		QtMeshSheetOperationPanel::applyCallback ( );
	}
	catch (...)
	{
	}
}	// QtMeshExplorerOperationPanel::applyCallback


void QtMeshExplorerOperationPanel::previousCallback ( )
{
	_direction	= -1;
	applyCallback ( );
}	// QtMeshExplorerOperationPanel::previousCallback


void QtMeshExplorerOperationPanel::nextCallback ( )
{
	_direction	= 1;
	applyCallback ( );
}	// QtMeshExplorerOperationPanel::nextCallback


void QtMeshExplorerOperationPanel::hideCallback ( )
{
	try
	{
		if (0 != getMgx3DOperationAction ( ))
		{
			getMgx3DOperationAction ( )->setActivated (false);
			_hidden	= true;
		}	// if (0 != getMgx3DOperationAction ( ))
	}
	catch (...)
	{
	}
}	// QtMeshExplorerOperationPanel::hideCallback


void QtMeshExplorerOperationPanel::closeCallback ( )
{
//	if (0 != parent ( ))
//		parent ( )->deleteLater ( );	// dock widget
//	else
		deleteLater ( );
}	// QtMeshExplorerOperationPanel::closeCallback


// ===========================================================================
//                  LA CLASSE QtMeshExplorerOperationAction
// ===========================================================================

QtMeshExplorerOperationAction::QtMeshExplorerOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMeshSheetOperationAction (
						icon, text, mainWindow, tooltip, creationPolicy, false),
	  _observers ( ), _mutex (false)
{
	QtMeshExplorerOperationPanel*	operationPanel	=
		new QtMeshExplorerOperationPanel (
			0, text.toStdString ( ),
			creationPolicy, "", mainWindow, this,
			QtMgx3DApplication::HelpSystem::instance ( ).sheetExplorationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).sheetExplorationTag);
	setWidget (operationPanel);
}	// QtMeshExplorerOperationAction::QtMeshExplorerOperationAction


QtMeshExplorerOperationAction::QtMeshExplorerOperationAction (
										const QtMeshExplorerOperationAction&)
	: QtMeshSheetOperationAction (
					QIcon (""), "", *new QtMgx3DMainWindow (0), "", QtMgx3DGroupNamePanel::CREATION),
	  _observers ( ), _mutex (false)
{
	MGX_FORBIDDEN ("QtMeshExplorerOperationAction copy constructor is not allowed.")
}	// QtMeshExplorerOperationAction::QtMeshExplorerOperationAction


QtMeshExplorerOperationAction& QtMeshExplorerOperationAction::operator = (
										const QtMeshExplorerOperationAction&)
{
	MGX_FORBIDDEN ("QtMeshExplorerOperationAction assignment operator is not allowed.")
	return *this;
}	// QtMeshExplorerOperationAction::operator =


QtMeshExplorerOperationAction::~QtMeshExplorerOperationAction ( )
{
	unique_ptr<CommandMeshExplorer>	oldExplorer (releaseMeshExplorer ( ));
	if (0 != oldExplorer.get ( ))
	{
		CommandMeshExplorer*	explorer	=
					getContext ( ).getMeshManager ( ).endExplorer (
													oldExplorer.get ( ), false);
		if (explorer){
			for (vector<ObjectBase*>::iterator it = _observers.begin ( );
					_observers.end ( ) != it; it++)
				(*it)->registerObservable (explorer, false);
			setMeshExplorer (explorer);
			explorer->execute ( );
		}
		else
			std::cerr<<"Erreur interne dans ~QtMeshExplorerOperationAction: explorer nul"<<std::endl;
	}	// if (0 != oldExplorer.get ( ))
}	// QtMeshExplorerOperationAction::~QtMeshExplorerOperationAction


QtMeshExplorerOperationPanel*
						QtMeshExplorerOperationAction::getMeshExplorerPanel ( )
{
	return dynamic_cast<QtMeshExplorerOperationPanel*>(getOperationPanel ( ));
}	// QtMeshExplorerOperationAction::getMeshExplorerPanel


void QtMeshExplorerOperationAction::executeOperation ( )
{
	/** Si l'utilisateur clique plein de fois sur le bouton avant/après, les
	 callbacks s'enchainent alors dans la pile d'appel alors que les traitements
	 ne sont pas finis, c.a.d. que la pile est comme suit :
	 QtMgxMainWindow::commandModified ( );
	 ...
	 QtMeshExplorerOperationAction::executeOperation ( )
	 QtMgx3DOperationPanel::applyCallback ( )
	 QtMeshExplorerOperationPanel::previsousCallback ( )
	 QtMeshExplorerOperationPanel::qt_metacall ( )
	 ...
	 QtMeshExplorerOperationAction::executeOperation ( )
	 QtMgx3DOperationPanel::applyCallback ( )
	 QtMeshExplorerOperationPanel::previsousCallback ( )
	 QtMeshExplorerOperationPanel::qt_metacall ( )
	 ...
	 => plantage
	 On évite ce plantage à l'aide d'un mutex non récursif, qui empêche le même
	 thread de le verrouiller 2 fois. En cas d'échec de verrouillage on ne fait
	 rien.
	 */
	 
	if (false == _mutex.tryLock ( ))
		return;

	try
	{
	    // Validation paramétrage :
	    QtMgx3DMeshOperationAction::executeOperation ( );

	    update (true);

	    _mutex.unlock ( );

	}
	catch (...)
	{
		_mutex.unlock ( );
		throw;
		// unique_ptr => delete oldExplorer
	}
}	// QtMeshExplorerOperationAction::executeOperation


void QtMeshExplorerOperationAction::addObservers (
										const vector<ObjectBase*>& observers)
{
	for (vector<ObjectBase*>::const_iterator it = observers.begin ( );
	     observers.end ( ) != it; it++)
		_observers.push_back (*it);

	update (false);
}	// QtMeshExplorerOperationAction::addObservers


void QtMeshExplorerOperationAction::update (bool withStep)
{
    // Récupération des paramètres de création du feuillet :
    QtMeshExplorerOperationPanel*   panel   = getMeshExplorerPanel ( );
    CHECK_NULL_PTR_ERROR (panel)
    const string    edgeName    = panel->getEdgeUniqueName ( );
    const long      steps       =
            panel->getDirection ( ) * panel->getEdgesStep ( );

    if (!edgeName.empty()) {
        unique_ptr<CommandMeshExplorer>   oldExplorer (releaseMeshExplorer ( ));

        CommandMeshExplorer*    explorer    =
                getContext ( ).getMeshManager ( ).newExplorer (
                        oldExplorer.get ( ),
                        (withStep?steps:0),
                        edgeName,
                        false);
        CHECK_NULL_PTR_ERROR (explorer)
        for (vector<ObjectBase*>::iterator it = _observers.begin ( );
                _observers.end ( ) != it; it++)
            (*it)->registerObservable (explorer, false);
        setMeshExplorer (explorer);
        explorer->execute ( );

        delete oldExplorer.release ( );
    }
}


void QtMeshExplorerOperationAction::setActivated (bool activated)
{
	if (false == _mutex.tryLock ( ))
		return;

	try
	{
		CHECK_NULL_PTR_ERROR (getOperationPanel ( ))
		CHECK_NULL_PTR_ERROR (getOperationPanel ( )->getMainWindow ( ))

		CommandInternal*	ci =
						dynamic_cast<CommandInternal*>(getMeshExplorer ( ));
		if (0 != ci)
		{
			InfoCommand&		icmd	= ci->getInfoCommand ( );
			uint				i		= 0;
			InfoCommand::type	t		= InfoCommand::UNITIALIZED;
			MeshEntity*			me		= 0;
			vector<Entity*>		entities;
			for (i = 0; i < icmd.getNbMeshInfoEntity ( ); i++)
			{
				icmd.getMeshInfoEntity (i, me, t);
				CHECK_NULL_PTR_ERROR (me)
				if (!me->getDisplayProperties ( ).isDisplayable ( ))
					continue;

				if (InfoCommand::CREATED == t)
					entities.push_back (me);
			}	// for (i = 0; i < icmd.getNbMeshInfoEntity ( ); i++)
			if (0 != entities.size ( ))
				getOperationPanel ( )->getMainWindow ( )->getGraphicalWidget (
					).getRenderingManager ( ).displayRepresentations (
					entities, activated, 0,DisplayRepresentation::DISPLAY_MESH);
		}	// if (0 != ci)

		_mutex.unlock ( );
	}
	catch (...)
	{
		_mutex.unlock ( );
	}
}	// QtMeshExplorerOperationAction::setActivated


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
