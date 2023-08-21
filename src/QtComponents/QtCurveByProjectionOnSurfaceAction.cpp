/**
 * \file        QtCurveByProjectionOnSurfaceAction.cpp
 * \author      Charles PIGNEROL
 * \date        19/05/2015
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "QtComponents/QtCurveByProjectionOnSurfaceAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QVBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Group;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtCurveByProjectionOnSurfacePanel
// ===========================================================================

QtCurveByProjectionOnSurfacePanel::QtCurveByProjectionOnSurfacePanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).curveByProjectionOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).curveByProjectionOperationTag),
	  _namePanel (0), _curvePanel (0), _surfacePanel (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins (0, 0, 0, 0);

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Nom du groupe de rattachement :
	_namePanel	= new QtMgx3DGroupNamePanel (
							this, "Groupe", mainWindow, 1, creationPolicy, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// Courbe à projeter :
	_curvePanel	= new QtMgx3DEntityPanel (
						this, "", true, "Courbe projetée :", "",
						&mainWindow, SelectionManagerIfc::D1,
						FilterEntity::GeomCurve);
	layout->addWidget (_curvePanel);
	connect (_curvePanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_curvePanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// Surface de projection :
	_surfacePanel	= new QtMgx3DEntityPanel (
						this, "", true, "Surface de projection :", "",
						&mainWindow, SelectionManagerIfc::D2,
						FilterEntity::GeomSurface);
	layout->addWidget (_surfacePanel);
	connect (_surfacePanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_surfacePanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	
	layout->addStretch (2);
}	// QtCurveByProjectionOnSurfacePanel::QtCurveByProjectionOnSurfacePanel


QtCurveByProjectionOnSurfacePanel::QtCurveByProjectionOnSurfacePanel (
								const QtCurveByProjectionOnSurfacePanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _curvePanel (0), _surfacePanel (0)
{
	MGX_FORBIDDEN ("QtCurveByProjectionOnSurfacePanel copy constructor is not allowed.");
}	// QtCurveByProjectionOnSurfacePanel::QtCurveByProjectionOnSurfacePanel (const QtCurveByProjectionOnSurfacePanel&)


QtCurveByProjectionOnSurfacePanel&
			QtCurveByProjectionOnSurfacePanel::operator = (
									const QtCurveByProjectionOnSurfacePanel&)
{
	MGX_FORBIDDEN ("QtCurveByProjectionOnSurfacePanel assignment operator is not allowed.");
	return *this;
}	// QtCurveByProjectionOnSurfacePanel::QtCurveByProjectionOnSurfacePanel (const QtCurveByProjectionOnSurfacePanel&)


QtCurveByProjectionOnSurfacePanel::~QtCurveByProjectionOnSurfacePanel ( )
{
}	// QtCurveByProjectionOnSurfacePanel::~QtCurveByProjectionOnSurfacePanel


string QtCurveByProjectionOnSurfacePanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtCurveByProjectionOnSurfacePanel::getGroupName


string QtCurveByProjectionOnSurfacePanel::getCurveName ( ) const
{
	CHECK_NULL_PTR_ERROR (_curvePanel)
	return _curvePanel->getUniqueName ( );
}	// QtCurveByProjectionOnSurfacePanel::getCurveName


string QtCurveByProjectionOnSurfacePanel::getSurfaceName ( ) const
{
	CHECK_NULL_PTR_ERROR (_surfacePanel)
	return _surfacePanel->getUniqueName ( );
}	// QtCurveByProjectionOnSurfacePanel::getSurfaceName


void QtCurveByProjectionOnSurfacePanel::reset ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_namePanel)
		CHECK_NULL_PTR_ERROR (_curvePanel)
		CHECK_NULL_PTR_ERROR (_surfacePanel)
		_namePanel->autoUpdate ( );
		_curvePanel->reset ( );
		_surfacePanel->reset ( );
	}
	catch (...)
	{
	}
}	// QtCurveByProjectionOnSurfacePanel::reset


void QtCurveByProjectionOnSurfacePanel::validate ( )
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
		error << "QtCurveByProjectionOnSurfacePanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtCurveByProjectionOnSurfacePanel::validate


void QtCurveByProjectionOnSurfacePanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_curvePanel)
	CHECK_NULL_PTR_ERROR (_surfacePanel)
	_curvePanel->stopSelection ( );
	_surfacePanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_curvePanel->setUniqueName ("");
		_surfacePanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtCurveByProjectionOnSurfacePanel::cancel


void QtCurveByProjectionOnSurfacePanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	CHECK_NULL_PTR_ERROR (_curvePanel)
	CHECK_NULL_PTR_ERROR (_surfacePanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedCurves	=
			getSelectionManager ( ).getEntitiesNames(FilterEntity::GeomCurve);
		if (1 == selectedCurves.size ( ))
			_curvePanel->setUniqueName (selectedCurves [0]);
		vector<string>	selectedSurfaces	=
			getSelectionManager ( ).getEntitiesNames(FilterEntity::GeomSurface);
		if (1 == selectedSurfaces.size ( ))
			_surfacePanel->setUniqueName (selectedSurfaces [0]);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_curvePanel->setUniqueName ("");
	_surfacePanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	_namePanel->autoUpdate ( );
	_curvePanel->actualizeGui (true);
	_surfacePanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtCurveByProjectionOnSurfacePanel::autoUpdate


vector<Entity*> QtCurveByProjectionOnSurfacePanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const string	curveName	= getCurveName ( );
	const string	surfaceName	= getSurfaceName ( );
	if (0 != curveName.length ( ))
	{
		Geom::Curve*	curve	=
			getContext ( ).getGeomManager ( ).getCurve (curveName, false);
		if (0 !=curve)
			entities.push_back ((Entity*)curve);
	}	// if (0 != curveName.length ( ))
	if (0 != surfaceName.length ( ))
	{
		Geom::Surface*	surface	=
			getContext ( ).getGeomManager ( ).getSurface (surfaceName, false);
		if (0 !=surface)
			entities.push_back ((Entity*)surface);
	}	// if (0 != surfaceName.length ( ))

	return entities;
}	// QtCurveByProjectionOnSurfacePanel::getInvolvedEntities


void QtCurveByProjectionOnSurfacePanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_curvePanel)
	CHECK_NULL_PTR_ERROR (_surfacePanel)
	_curvePanel->stopSelection ( );
	_surfacePanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtCurveByProjectionOnSurfacePanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtCurveByProjectionOnSurfaceAction
// ===========================================================================

QtCurveByProjectionOnSurfaceAction::QtCurveByProjectionOnSurfaceAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtCurveByProjectionOnSurfacePanel*	operationPanel	=
		new QtCurveByProjectionOnSurfacePanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, creationPolicy, this);
	setOperationPanel (operationPanel);
}	// QtCurveByProjectionOnSurfaceAction::QtCurveByProjectionOnSurfaceAction


QtCurveByProjectionOnSurfaceAction::QtCurveByProjectionOnSurfaceAction (
									const QtCurveByProjectionOnSurfaceAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtCurveByProjectionOnSurfaceAction copy constructor is not allowed.")
}	// QtCurveByProjectionOnSurfaceAction::QtCurveByProjectionOnSurfaceAction


QtCurveByProjectionOnSurfaceAction&
					 QtCurveByProjectionOnSurfaceAction::operator = (
									const QtCurveByProjectionOnSurfaceAction&)
{
	MGX_FORBIDDEN ("QtCurveByProjectionOnSurfaceAction assignment operator is not allowed.")
	return *this;
}	// QtCurveByProjectionOnSurfaceAction::operator =


QtCurveByProjectionOnSurfaceAction::~QtCurveByProjectionOnSurfaceAction ( )
{
}	// QtCurveByProjectionOnSurfaceAction::~QtCurveByProjectionOnSurfaceAction


QtCurveByProjectionOnSurfacePanel*
						QtCurveByProjectionOnSurfaceAction::getCurvePanel ( )
{
	return dynamic_cast<QtCurveByProjectionOnSurfacePanel*>(
														getOperationPanel ( ));
}	// QtCurveByProjectionOnSurfaceAction::getCurvePanel


void QtCurveByProjectionOnSurfaceAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de création de la courbe :
	QtCurveByProjectionOnSurfacePanel*	panel	= getCurvePanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string	name	= panel->getGroupName ( );
	const string	curve	= panel->getCurveName ( );
	const string	surface	= panel->getSurfaceName ( );
	cmdResult	= getContext ( ).getGeomManager ( ).newCurveByCurveProjectionOnSurface (curve, surface, name);

	setCommandResult (cmdResult);
}	// QtCurveByProjectionOnSurfaceAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
