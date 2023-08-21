/**
 * \file        QtTopologyRotationAction.cpp
 * \author      Charles PIGNEROL
 * \date        22/05/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtTopologyRotationAction.h"
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
//                        LA CLASSE QtTopologyRotationPanel
// ===========================================================================

QtTopologyRotationPanel::QtTopologyRotationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).topoRotationOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).topoRotationOperationTag),
	  _topoEntitiesPanel (0), _propagateCheckBox (0), _rotationPanel (0)
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

	// Faut il propager la rotation aux entités géométriques ?
	_propagateCheckBox	=
			new QCheckBox (QString::fromUtf8("Propager aux entités géométriques"), this);
	_propagateCheckBox->setChecked (true);
	layout->addWidget (_propagateCheckBox);

	// La rotation :
	_rotationPanel	= new QtMgx3DRotationPanel (
			this, "", true, QtAnglePanel::eightthToHalf ( ), 0, &mainWindow);
	connect (_rotationPanel, SIGNAL (rotationModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));
	layout->addWidget (_rotationPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
			)->setLinkedSeizureManagers (0, 0);
// Souvent disabled :
//							0, _rotationPanel->getSegmentIDTextField ( ));
}	// QtTopologyRotationPanel::QtTopologyRotationPanel


QtTopologyRotationPanel::QtTopologyRotationPanel (
										const QtTopologyRotationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _topoEntitiesPanel (0), _propagateCheckBox (0), _rotationPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyRotationPanel copy constructor is not allowed.");
}	// QtTopologyRotationPanel::QtTopologyRotationPanel (const QtTopologyRotationPanel&)


QtTopologyRotationPanel& QtTopologyRotationPanel::operator = (
											const QtTopologyRotationPanel&)
{
	MGX_FORBIDDEN ("QtTopologyRotationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyRotationPanel::QtTopologyRotationPanel (const QtTopologyRotationPanel&)


QtTopologyRotationPanel::~QtTopologyRotationPanel ( )
{
	preview (false, true);
}	// QtTopologyRotationPanel::~QtTopologyRotationPanel


void QtTopologyRotationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_rotationPanel)
	_topoEntitiesPanel->reset ( );
	_rotationPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyRotationPanel::reset


void QtTopologyRotationPanel::setDimension (SelectionManagerIfc::DIM dim)
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	_topoEntitiesPanel->clearSelection ( );
	_topoEntitiesPanel->setDimensions (dim);
}	// QtTopologyRotationPanel::setDimension


vector<string> QtTopologyRotationPanel::getTopoEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	return _topoEntitiesPanel->getEntitiesNames ( );
}	// QtTopologyRotationPanel::getTopoEntitiesNames


Math::Rotation QtTopologyRotationPanel::getRotation ( ) const
{
	CHECK_NULL_PTR_ERROR (_rotationPanel)
	return _rotationPanel->getRotation ( );
}	// QtTopologyRotationPanel::getRotation


bool QtTopologyRotationPanel::doPropagate ( ) const
{
	CHECK_NULL_PTR_ERROR (_propagateCheckBox)
	return Qt::Checked == _propagateCheckBox->checkState ( ) ? true : false;
}	// QtTopologyRotationPanel::doPropagate



void QtTopologyRotationPanel::validate ( )
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
		error << "QtTopologyRotationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getTopoEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopologyRotationPanel::validate


void QtTopologyRotationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_rotationPanel)
	_topoEntitiesPanel->stopSelection ( );
	_rotationPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_topoEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyRotationPanel::cancel


void QtTopologyRotationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_rotationPanel)
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
#endif	// AUTO_UPDATE_OLD_SCHEME

	_topoEntitiesPanel->actualizeGui (true);
	_rotationPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyRotationPanel::autoUpdate


vector<Entity*> QtTopologyRotationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

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
}	// QtTopologyRotationPanel::getInvolvedEntities


void QtTopologyRotationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _topoEntitiesPanel)
		_topoEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyRotationPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyRotationAction
// ===========================================================================

QtTopologyRotationAction::QtTopologyRotationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyRotationPanel*	operationPanel	=
		new QtTopologyRotationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyRotationAction::QtTopologyRotationAction


QtTopologyRotationAction::QtTopologyRotationAction (
										const QtTopologyRotationAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyRotationAction copy constructor is not allowed.")
}	// QtTopologyRotationAction::QtTopologyRotationAction


QtTopologyRotationAction& QtTopologyRotationAction::operator = (
										const QtTopologyRotationAction&)
{
	MGX_FORBIDDEN ("QtTopologyRotationAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyRotationAction::operator =


QtTopologyRotationAction::~QtTopologyRotationAction ( )
{
}	// QtTopologyRotationAction::~QtTopologyRotationAction


QtTopologyRotationPanel*
					QtTopologyRotationAction::getRotationPanel ( )
{
	return dynamic_cast<QtTopologyRotationPanel*>(getOperationPanel ( ));
}	// QtTopologyRotationAction::getRotationPanel


void QtTopologyRotationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités topologiques :
	QtTopologyRotationPanel*	panel	= getRotationPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>			entities	= panel->getTopoEntitiesNames ( );
	Math::Rotation			rotation	= panel->getRotation ( );
	bool					propagate	= panel->doPropagate ( );

	cmdResult	= getContext ( ).getTopoManager ( ).rotate (entities, rotation, propagate);

	setCommandResult (cmdResult);
}	// QtTopologyRotationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
