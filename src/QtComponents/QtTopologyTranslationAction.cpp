/**
 * \file        QtTopologyTranslationAction.cpp
 * \author      Charles PIGNEROL
 * \date        22/05/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtTopologyTranslationAction.h"
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
//                        LA CLASSE QtTopologyTranslationPanel
// ===========================================================================

QtTopologyTranslationPanel::QtTopologyTranslationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).topoTranslationOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).topoTranslationOperationTag),
	  _topoEntitiesPanel (0), _propagateCheckBox (0), _translationPanel (0)
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

	// Faut il propager la translation aux entités géométriques ?
	_propagateCheckBox	=
			new QCheckBox (QString::fromUtf8("Propager aux entités géométriques"), this);
	_propagateCheckBox->setChecked (true);
	layout->addWidget (_propagateCheckBox);

	// La translation :
	_translationPanel	=
			new QtMgx3DVectorPanel (this, "", true,
					"dx :", "dy :", "dz :",
					1., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
					0., -DBL_MAX, DBL_MAX,
					&mainWindow, (FilterEntity::objectType)(
							FilterEntity::AllPoints | FilterEntity::AllEdges),
					true);
	connect (_translationPanel, SIGNAL (vectorModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));
	layout->addWidget (_translationPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
			)->setLinkedSeizureManagers (
							0, _translationPanel->getSegmentIDTextField ( ));
	_translationPanel->getSegmentIDTextField ( )->setLinkedSeizureManagers (
			_topoEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ), 0);
}	// QtTopologyTranslationPanel::QtTopologyTranslationPanel


QtTopologyTranslationPanel::QtTopologyTranslationPanel (
										const QtTopologyTranslationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _topoEntitiesPanel (0), _propagateCheckBox (0), _translationPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyTranslationPanel copy constructor is not allowed.");
}	// QtTopologyTranslationPanel::QtTopologyTranslationPanel (const QtTopologyTranslationPanel&)


QtTopologyTranslationPanel& QtTopologyTranslationPanel::operator = (
											const QtTopologyTranslationPanel&)
{
	MGX_FORBIDDEN ("QtTopologyTranslationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyTranslationPanel::QtTopologyTranslationPanel (const QtTopologyTranslationPanel&)


QtTopologyTranslationPanel::~QtTopologyTranslationPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtTopologyTranslationPanel::~QtTopologyTranslationPanel


void QtTopologyTranslationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_translationPanel)
	_topoEntitiesPanel->reset ( );
	_translationPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyTranslationPanel::reset


void QtTopologyTranslationPanel::setDimension (SelectionManagerIfc::DIM dim)
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	_topoEntitiesPanel->clearSelection ( );
	_topoEntitiesPanel->setDimensions (dim);
}	// QtTopologyTranslationPanel::setDimension


vector<string> QtTopologyTranslationPanel::getTopoEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	return _topoEntitiesPanel->getEntitiesNames ( );
}	// QtTopologyTranslationPanel::getTopoEntitiesNames


Math::Vector QtTopologyTranslationPanel::getTranslation ( ) const
{
	CHECK_NULL_PTR_ERROR (_translationPanel)
	return Vector (_translationPanel->getDx ( ),
	               _translationPanel->getDy ( ),
	               _translationPanel->getDz ( ));;
}	// QtTopologyTranslationPanel::getTranslation


bool QtTopologyTranslationPanel::doPropagate ( ) const
{
	CHECK_NULL_PTR_ERROR (_propagateCheckBox)
	return Qt::Checked == _propagateCheckBox->checkState ( ) ? true : false;
}	// QtTopologyTranslationPanel::doPropagate



void QtTopologyTranslationPanel::validate ( )
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
		error << "QtTopologyTranslationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getTopoEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopologyTranslationPanel::validate


void QtTopologyTranslationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_translationPanel)
	_topoEntitiesPanel->stopSelection ( );
	_translationPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_topoEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyTranslationPanel::cancel


void QtTopologyTranslationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_topoEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_translationPanel)
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
	_translationPanel->setSegment ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationPanel::autoUpdate ( );

	_topoEntitiesPanel->actualizeGui (true);
	_translationPanel->actualizeGui (true);
}	// QtTopologyTranslationPanel::autoUpdate


vector<Entity*> QtTopologyTranslationPanel::getInvolvedEntities ( )
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
}	// QtTopologyTranslationPanel::getInvolvedEntities


void QtTopologyTranslationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _topoEntitiesPanel)
		_topoEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyTranslationPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtTopologyTranslationAction
// ===========================================================================

QtTopologyTranslationAction::QtTopologyTranslationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyTranslationPanel*	operationPanel	=
		new QtTopologyTranslationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyTranslationAction::QtTopologyTranslationAction


QtTopologyTranslationAction::QtTopologyTranslationAction (
										const QtTopologyTranslationAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyTranslationAction copy constructor is not allowed.")
}	// QtTopologyTranslationAction::QtTopologyTranslationAction


QtTopologyTranslationAction& QtTopologyTranslationAction::operator = (
										const QtTopologyTranslationAction&)
{
	MGX_FORBIDDEN ("QtTopologyTranslationAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyTranslationAction::operator =


QtTopologyTranslationAction::~QtTopologyTranslationAction ( )
{
}	// QtTopologyTranslationAction::~QtTopologyTranslationAction


QtTopologyTranslationPanel*
					QtTopologyTranslationAction::getTranslationPanel ( )
{
	return dynamic_cast<QtTopologyTranslationPanel*>(getOperationPanel ( ));
}	// QtTopologyTranslationAction::getTranslationPanel


void QtTopologyTranslationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités topologiques :
	QtTopologyTranslationPanel*	panel	= getTranslationPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>			entities	= panel->getTopoEntitiesNames ( );
	Math::Vector			translation	(panel->getTranslation ( ));
	bool					propagate	= panel->doPropagate ( );

	cmdResult	= getContext ( ).getTopoManager ( ).translate (entities, translation, propagate);

	setCommandResult (cmdResult);
}	// QtTopologyTranslationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
