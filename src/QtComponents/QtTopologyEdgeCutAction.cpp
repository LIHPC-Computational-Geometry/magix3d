/**
 * \file        QtTopologyEdgeCutAction.cpp
 * \author      Charles PIGNEROL
 * \date        03/02/2014
 */

#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
#include "Geom/GeomEntity.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyEdgeCutAction.h"
#include "Topo/CommandSplitEdge.h"
#include "Topo/TopoDisplayRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

#include <QGridLayout>
#include <QRadioButton>


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
//                        LA CLASSE QtTopologyEdgeCutPanel
// ===========================================================================

QtTopologyEdgeCutPanel::QtTopologyEdgeCutPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).edgeCutOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).edgeCutOperationTag),
	  _edgePanel (0), _cutDefinitionButtonGroup (0), _ratioTextField (0),
	  _cutPointEntityPanel (0)
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

	// L'arête à découper :
	_edgePanel	= new QtMgx3DEdgePanel (
								this, "Arête à découper", true, "", &mainWindow,
								FilterEntity::TopoCoEdge);
	layout->addWidget (_edgePanel);
	connect (_edgePanel, SIGNAL (edgesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edgePanel, SIGNAL (edgesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// La définition de la découpe :
	_cutDefinitionButtonGroup	= new QButtonGroup (this);
	_cutDefinitionButtonGroup->setExclusive (true);
	QGridLayout*	gridLayout	= new QGridLayout ( );
	int	row	= 0, col	= 0;
	layout->addLayout (gridLayout);
	gridLayout->setContentsMargins  (0, 0, 0, 0);
	// Par un ratio :
	QRadioButton*	ratioRadioButton	= new QRadioButton ("Ratio :", this);
	ratioRadioButton->setChecked (true);
	_cutDefinitionButtonGroup->addButton (
						ratioRadioButton, QtTopologyEdgeCutPanel::CDM_RATIO);
	gridLayout->addWidget (ratioRadioButton, row, col++);
	_ratioTextField	= new QtDoubleTextField (0.5, this);
	gridLayout->addWidget (_ratioTextField, row++, col++, 1, 2);
	_ratioTextField->setToolTip (QString::fromUtf8("Pourcentage de la longueur de l'arête attribué à la première arête."));
	connect (_ratioTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (parametersModifiedCallback ( )));
	// Par un point :
	col	= 0;
	QRadioButton*	entityRadioButton	= new QRadioButton ("Entité :", this);
	_cutDefinitionButtonGroup->addButton (
						entityRadioButton, QtTopologyEdgeCutPanel::CDM_POINT);
	gridLayout->addWidget (entityRadioButton, row, col++);
	const FilterEntity::objectType	filter	=
		(FilterEntity::objectType)(
							FilterEntity::AllGeom | FilterEntity::TopoVertex);
	_cutPointEntityPanel	=
		new QtMgx3DEntityPanel (this, "", true, "", "", &mainWindow,
								SelectionManagerIfc::ALL_DIMENSIONS, filter);
	_cutPointEntityPanel->setToolTip (QString::fromUtf8("Entité géométrique dont le centre sert de position de découpe, ou sommet topologique."));
	gridLayout->addWidget (_cutPointEntityPanel, row, col++, 1, 2);
	connect (_cutPointEntityPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_cutPointEntityPanel,
	         SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_cutDefinitionButtonGroup, SIGNAL (buttonClicked (int)),
	         this, SLOT (parametersModifiedCallback ( )));
	connect (_cutDefinitionButtonGroup, SIGNAL (buttonClicked (int)),
	         this, SLOT (cutDefinitionModifiedCallback ( )));
	double	r	= ((double)_ratioTextField->sizeHint ( ).width( )) /
								entityRadioButton->sizeHint ( ).width( ) + 0.5;
	gridLayout->setColumnStretch (0, 1);
	gridLayout->setColumnStretch (1, (int)r);
	ratioRadioButton->setFixedSize (entityRadioButton->sizeHint ( ));
	entityRadioButton->setFixedSize (entityRadioButton->sizeHint ( ));

	cutDefinitionModifiedCallback ( );

	addPreviewCheckBox (true);

	layout->addStretch (2);

}	// QtTopologyEdgeCutPanel::QtTopologyEdgeCutPanel


QtTopologyEdgeCutPanel::QtTopologyEdgeCutPanel (const QtTopologyEdgeCutPanel&)
	: QtMgx3DOperationPanel (
		0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _edgePanel (0), _cutDefinitionButtonGroup (0), _ratioTextField (0),
	  _cutPointEntityPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyEdgeCutPanel copy constructor is not allowed.");
}	// QtTopologyEdgeCutPanel::QtTopologyEdgeCutPanel (const QtTopologyEdgeCutPanel&)


QtTopologyEdgeCutPanel& QtTopologyEdgeCutPanel::operator = (
											const QtTopologyEdgeCutPanel&)
{
	MGX_FORBIDDEN ("QtTopologyEdgeCutPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyEdgeCutPanel::QtTopologyEdgeCutPanel (const QtTopologyEdgeCutPanel&)


QtTopologyEdgeCutPanel::~QtTopologyEdgeCutPanel ( )
{
}	// QtTopologyEdgeCutPanel::~QtTopologyEdgeCutPanel


void QtTopologyEdgeCutPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
	_edgePanel->reset ( );
	_ratioTextField->setValue (0.5);
	_cutPointEntityPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyEdgeCutPanel::reset


void QtTopologyEdgeCutPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
	_edgePanel->stopSelection ( );
	_cutPointEntityPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_edgePanel->setUniqueName ("");
		_cutPointEntityPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyEdgeCutPanel::cancel


void QtTopologyEdgeCutPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel->getNameTextField ( ))

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		 vector<string>	selectedEdges	= 
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
		if (1 == selectedEdges.size ( ))
			_edgePanel->setUniqueName (selectedEdges [0]);
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_edgePanel->getEntityTextField ( ).setInteractiveMode (false);
	_edgePanel->setUniqueName ("");
	_cutPointEntityPanel->getNameTextField ( )->setInteractiveMode (false);
	_cutPointEntityPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_edgePanel->actualizeGui (true);
	_cutPointEntityPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyEdgeCutPanel::autoUpdate


string QtTopologyEdgeCutPanel::getEdgeName ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgePanel)	
	return _edgePanel->getUniqueName ( );
}	// QtTopologyEdgeCutPanel::getEdgeName


QtTopologyEdgeCutPanel::CUT_DEFINITION_METHOD
					QtTopologyEdgeCutPanel::getCutDefinitionMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_cutDefinitionButtonGroup)
	return (QtTopologyEdgeCutPanel::CUT_DEFINITION_METHOD)_cutDefinitionButtonGroup->checkedId ( );
}	// QtTopologyEdgeCutPanel::getCutDefinitionMethod


double QtTopologyEdgeCutPanel::getRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_ratioTextField)	
	return _ratioTextField->getValue ( );
}	// QtTopologyEdgeCutPanel::getRatio


Math::Point QtTopologyEdgeCutPanel::getCutPoint ( ) const
{
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
	const string	name	= _cutPointEntityPanel->getUniqueName ( );
	if (0 == name.length ( ))
		return Math::Point ( );

	Entity*			entity	= &getContext().nameToEntity (name);
	GeomEntity*		ge		= dynamic_cast<GeomEntity*>(entity);
	Topo::Vertex*	v		= dynamic_cast<Topo::Vertex*>(entity);

	if (0 != ge)
		return ge->getCenteredPosition ( );
	else if (0 != v)
		return v->getCoord ( );

	throw Exception (UTF8String ("Définition de la position de découpe : absence d'entité sélectionnée.", Charset::UTF_8));
}	// QtTopologyEdgeCutPanel::getCutPoint


vector<Entity*> QtTopologyEdgeCutPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const string	name	= getEdgeName ( );
	if (0 != name.length ( ))
	{
		TopoEntity*	entity	= getContext ( ).getTopoManager ( ).getCoEdge(name);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}	// if (0 != name.length ( ))

	switch (getCutDefinitionMethod ( ))
	{
		case QtTopologyEdgeCutPanel::CDM_POINT	:
		{
			CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
			const string	point	= _cutPointEntityPanel->getUniqueName ( );
			if (0 != point.length ( ))
				entities.push_back (&getContext().nameToEntity (point));
		}
		break;
	}	// switch (getCutDefinitionMethod ( ))

	return entities;
}	// QtTopologyEdgeCutPanel::getInvolvedEntities


void QtTopologyEdgeCutPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _edgePanel)
		_edgePanel->stopSelection ( );
	if (0 != _cutPointEntityPanel)
		_cutPointEntityPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyEdgeCutPanel::operationCompleted


void QtTopologyEdgeCutPanel::cutDefinitionModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_ratioTextField)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)

	switch (getCutDefinitionMethod ( ))
	{
		case QtTopologyEdgeCutPanel::CDM_RATIO	:
			_ratioTextField->setEnabled (true);
			_cutPointEntityPanel->setEnabled (false);
			break;
		case QtTopologyEdgeCutPanel::CDM_POINT	:
			_ratioTextField->setEnabled (false);
			_cutPointEntityPanel->setEnabled (true);
			break;
	}	// switch (getCutDefinitionMethod ( ))

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologyEdgeCutPanel::cutDefinitionModifiedCallback


void QtTopologyEdgeCutPanel::preview (bool show, bool destroyInteractor)
{
	// Lors de la construction getGraphicalWidget peut être nul ...
	try
	{
		getRenderingManager ( );
	}
	catch (...)
	{
		return;
	}

	QtMgx3DOperationPanel::preview (show, destroyInteractor);
	if ((false == show) || (false == previewResult ( )))
		return;

	try
	{

		Context*		context		= dynamic_cast<Context*>(&getContext ( ));
		CHECK_NULL_PTR_ERROR (context)
		const string	edgeName	= getEdgeName ( );
		CoEdge*			edge		=
				 getContext ( ).getTopoManager ( ).getCoEdge (edgeName, true);
		std::unique_ptr<CommandSplitEdge>	command;
		switch (getCutDefinitionMethod ( ))
		{
			case QtTopologyEdgeCutPanel::CDM_RATIO	:
				command.reset (new CommandSplitEdge (
					*context, edge, getRatio ( )));
			break;
			case QtTopologyEdgeCutPanel::CDM_POINT	:
				command.reset (new CommandSplitEdge (
					*context, edge, getCutPoint ( )));
			break;
			default										:
				throw Exception (UTF8String ("QtTopologyEdgeCutPanel::preview : cas non implémenté.", Charset::UTF_8));
		}	// switch (getCutDefinitionMethod ( ))
		RenderingManager::CommandPreviewMgr	previewManager (
							*command.get ( ), getRenderingManager ( ), true);

		// Ajouter les infos de pré-maillage ?
		if (0 != edge->getDisplayProperties ( ).getGraphicalRepresentation ( ))
		{
			const DisplayProperties&	props	=
				true == getRenderingManager ( ).useGlobalDisplayProperties ( ) ?
				getRenderingManager ( ).getContext ( ).globalDisplayProperties (
					edge->getType ( )) : edge->getDisplayProperties ( );
			const unsigned long		mask	=
				true == getRenderingManager ( ).useGlobalDisplayProperties ( ) ?
				getRenderingManager().getContext().globalMask(edge->getType()) :
				edge->getDisplayProperties ( ).getGraphicalRepresentation (
													)->getRepresentationMask( );

			previewEdges (
					*command, previewManager, InfoCommand::CREATED,
					props, mask);
		}	// if (0 != edge->getDisplayProperties ( ).getGraphicalRepresentation ( ))

		getRenderingManager ( ).forceRender ( );
	}
	catch (const Exception& exc)
	{
#ifndef NDEBUG
cerr << __FILE__ << ' ' << __LINE__ << " Exception caught : " << exc.getFullMessage ( ) << endl;
#endif
	}
	catch (...)
	{
	}
}	// QtTopologyEdgeCutPanel::preview


// ===========================================================================
//                  LA CLASSE QtTopologyEdgeCutAction
// ===========================================================================

QtTopologyEdgeCutAction::QtTopologyEdgeCutAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyEdgeCutPanel*	operationPanel	= 
			new QtTopologyEdgeCutPanel (
					&getOperationPanelParent ( ), text.toStdString ( ), 
					mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyEdgeCutAction::QtTopologyEdgeCutAction


QtTopologyEdgeCutAction::QtTopologyEdgeCutAction (
										const QtTopologyEdgeCutAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyEdgeCutAction copy constructor is not allowed.")
}	// QtTopologyEdgeCutAction::QtTopologyEdgeCutAction


QtTopologyEdgeCutAction& QtTopologyEdgeCutAction::operator = (
										const QtTopologyEdgeCutAction&)
{
	MGX_FORBIDDEN ("QtTopologyEdgeCutAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyEdgeCutAction::operator =


QtTopologyEdgeCutAction::~QtTopologyEdgeCutAction ( )
{
}	// QtTopologyEdgeCutAction::~QtTopologyEdgeCutAction


QtTopologyEdgeCutPanel* QtTopologyEdgeCutAction::getEdgeCutPanel ( )
{
	return dynamic_cast<QtTopologyEdgeCutPanel*>(getOperationPanel ( ));
}	// QtTopologyEdgeCutAction::getEdgeCutPanel


void QtTopologyEdgeCutAction::executeOperation ( )
{
	QtTopologyEdgeCutPanel*	panel	= getEdgeCutPanel ( );
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres de découpage de l'arête topologique :
	string	name	= panel->getEdgeName ( );
	double	ratio	= panel->getRatio ( );

	switch (panel->getCutDefinitionMethod ( ))
	{
		case QtTopologyEdgeCutPanel::CDM_RATIO	:
			cmdResult	= getContext ( ).getTopoManager ( ).splitEdge (name, panel->getRatio ( ));
			break;
		case QtTopologyEdgeCutPanel::CDM_POINT	:
			cmdResult	= getContext ( ).getTopoManager ( ).splitEdge (name, panel->getCutPoint ( ));
			break;
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Position de découpe non supportée ("
			        << (unsigned long)panel->getCutDefinitionMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtTopologyEdgeCutAction::executeOperation")
			throw exc;
		}
	}	// switch (panel->getCutDefinitionMethod ( ))

	setCommandResult (cmdResult);
}	// QtTopologyEdgeCutAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
