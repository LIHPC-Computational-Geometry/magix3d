/**
 * \file        QtTopoEntityDestructionAction.cpp
 * \author      Charles PIGNEROL
 * \date        22/01/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtTopoEntityDestructionAction.h"
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
//                        LA CLASSE QtTopoEntityDestructionPanel
// ===========================================================================

QtTopoEntityDestructionPanel::QtTopoEntityDestructionPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).topoDestructionOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).topoDestructionOperationTag),
	  _topoEntitiesPanel (0), _propagateCheckBox (0)
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


	// Faut il propager la destruction aux entités de dimensions inférieures ?
	_propagateCheckBox	=
			new QCheckBox (QString::fromUtf8("Propager aux dimensions inférieures"), this);
	_propagateCheckBox->setChecked (true);
	layout->addWidget (_propagateCheckBox);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
											)->setLinkedSeizureManagers (0, 0);
}	// QtTopoEntityDestructionPanel::QtTopoEntityDestructionPanel


QtTopoEntityDestructionPanel::QtTopoEntityDestructionPanel (
										const QtTopoEntityDestructionPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _topoEntitiesPanel (0), _propagateCheckBox (0)
{
	MGX_FORBIDDEN ("QtTopoEntityDestructionPanel copy constructor is not allowed.");
}	// QtTopoEntityDestructionPanel::QtTopoEntityDestructionPanel (const QtTopoEntityDestructionPanel&)


QtTopoEntityDestructionPanel& QtTopoEntityDestructionPanel::operator = (
											const QtTopoEntityDestructionPanel&)
{
	MGX_FORBIDDEN ("QtTopoEntityDestructionPanel assignment operator is not allowed.");
	return *this;
}	// QtTopoEntityDestructionPanel::QtTopoEntityDestructionPanel (const QtTopoEntityDestructionPanel&)


QtTopoEntityDestructionPanel::~QtTopoEntityDestructionPanel ( )
{
	preview (false, true);
}	// QtTopoEntityDestructionPanel::~QtTopoEntityDestructionPanel


vector<string> QtTopoEntityDestructionPanel::getTopoEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	return _topoEntitiesPanel->getEntitiesNames ( );
}	// QtTopoEntityDestructionPanel::getTopoEntitiesNames


bool QtTopoEntityDestructionPanel::doPropagate ( ) const
{
	CHECK_NULL_PTR_ERROR (_propagateCheckBox)
	return Qt::Checked == _propagateCheckBox->checkState ( ) ? true : false;
}	// QtTopoEntityDestructionPanel::doPropagate


void QtTopoEntityDestructionPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	_topoEntitiesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopoEntityDestructionPanel::reset


void QtTopoEntityDestructionPanel::validate ( )
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
		error << "QtTopoEntityDestructionPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getTopoEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopoEntityDestructionPanel::validate


void QtTopoEntityDestructionPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	_topoEntitiesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_topoEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopoEntityDestructionPanel::cancel


void QtTopoEntityDestructionPanel::autoUpdate ( )
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
#endif	// AUTO_UPDATE_OLD_SCHEME

	_topoEntitiesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopoEntityDestructionPanel::autoUpdate


vector<Entity*> QtTopoEntityDestructionPanel::getInvolvedEntities ( )
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
}	// QtTopoEntityDestructionPanel::getInvolvedEntities


void QtTopoEntityDestructionPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _topoEntitiesPanel)
		_topoEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopoEntityDestructionPanel::operationCompleted

void QtTopoEntityDestructionPanel::setDimensions (Mgx3D::Utils::SelectionManagerIfc::DIM dims)
{

	_topoEntitiesPanel->setDimensions(dims);

} // QtTopoEntityDestructionPanel::setDimensions

// ===========================================================================
//                  LA CLASSE QtTopoEntityDestructionAction
// ===========================================================================

QtTopoEntityDestructionAction::QtTopoEntityDestructionAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopoEntityDestructionPanel*	operationPanel	=
		new QtTopoEntityDestructionPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopoEntityDestructionAction::QtTopoEntityDestructionAction


QtTopoEntityDestructionAction::QtTopoEntityDestructionAction (
										const QtTopoEntityDestructionAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopoEntityDestructionAction copy constructor is not allowed.")
}	// QtTopoEntityDestructionAction::QtTopoEntityDestructionAction


QtTopoEntityDestructionAction& QtTopoEntityDestructionAction::operator = (
										const QtTopoEntityDestructionAction&)
{
	MGX_FORBIDDEN ("QtTopoEntityDestructionAction assignment operator is not allowed.")
	return *this;
}	// QtTopoEntityDestructionAction::operator =


QtTopoEntityDestructionAction::~QtTopoEntityDestructionAction ( )
{
}	// QtTopoEntityDestructionAction::~QtTopoEntityDestructionAction


QtTopoEntityDestructionPanel*
					QtTopoEntityDestructionAction::getDestructionPanel ( )
{
	return dynamic_cast<QtTopoEntityDestructionPanel*>(getOperationPanel ( ));
}	// QtTopoEntityDestructionAction::getDestructionPanel


void QtTopoEntityDestructionAction::executeOperation ( )
{
	// Validation paramétrage :
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités topologiques :
	QtTopoEntityDestructionPanel*	panel	= getDestructionPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>				entities	= panel->getTopoEntitiesNames ( );
	bool						propagate	= panel->doPropagate ( );

	getContext ( ).getTopoManager ( ).destroy (entities, propagate);
}	// QtTopoEntityDestructionAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
