/**
 * \file        QtTopologySplitFacesAction.cpp
 * \author      Charles PIGNEROL
 * \date        17/03/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "QtComponents/QtTopologySplitFacesAction.h"
#include "Geom/GeomEntity.h"
#include "Topo/CommandSplitFaces.h"
#include "Topo/TopoDisplayRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

#include <QRadioButton>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologySplitFacesPanel
// ===========================================================================

QtTopologySplitFacesPanel::QtTopologySplitFacesPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).splitFacesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).splitFacesOperationTag),
	  _facesPanel (0), _cutDefinitionButtonGroup (0), _edgePanel (0),
	  _ratioTextField (0), _cutPointEntityPanel (0), _oGridRatioTextField (0),
	  _allFacesCheckBox (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);

	// Le nom du panneau :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Toutes les faces ?
	_allFacesCheckBox	= new QCheckBox ("Toutes les faces", this);
	_allFacesCheckBox->setToolTip (QString::fromUtf8("Coché toutes les faces seront soumises au découpage."));
	_allFacesCheckBox->setCheckState(Qt::Checked);
	layout->addWidget (_allFacesCheckBox);
	connect (_allFacesCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (facesModifiedCallback ( )));

	// Les faces à découper :
	_facesPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Faces  :", "", &mainWindow,
			SelectionManagerIfc::D2, FilterEntity::TopoCoFace);
	_facesPanel->setMultiSelectMode (true);
	_facesPanel->setToolTip (QString::fromUtf8("Faces soumises au découpage si \"Toutes les faces\" n'est pas coché."));
	connect (_facesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_facesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_facesPanel);

	// L'arête orthogonale au plan de coupe :
	_edgePanel	= new QtMgx3DEntityPanel (
			this, "", true, "Arête :", "", &mainWindow,
			SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	_edgePanel->setToolTip (QString::fromUtf8("Arête orthogonale au plan de coupe."));
	connect (_edgePanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edgePanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	layout->addWidget (_edgePanel);

	// La définition de la découpe :
	_cutDefinitionButtonGroup	= new QButtonGroup (this);
	_cutDefinitionButtonGroup->setExclusive (true);
	QGridLayout*	gridLayout	= new QGridLayout ( );
	gridLayout->setContentsMargins  (0, 0, 0, 0);
	layout->addLayout (gridLayout);
	int	row	= 0, col	= 0;
	// Par un ratio :
	QRadioButton*	ratioRadioButton	= new QRadioButton ("Ratio :", this);
	ratioRadioButton->setChecked (true);
	_cutDefinitionButtonGroup->addButton (
					ratioRadioButton, QtTopologySplitFacesPanel::CDM_RATIO);
	gridLayout->addWidget (ratioRadioButton, row, col++);
	_ratioTextField	= &QtNumericFieldsFactory::createRatioTextField (this);
	_ratioTextField->setValue (0.5);
	gridLayout->addWidget (_ratioTextField, row++, col++, 1, 2);
	_ratioTextField->setToolTip (QString::fromUtf8("Pourcentage de la longueur de l'arête où est le point situé dans le plan de coupe."));
	connect (_ratioTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (parametersModifiedCallback ( )));
	// Par un point :
	col	= 0;
	QRadioButton*	entityRadioButton	= new QRadioButton (QString::fromUtf8("Entité :"), this);
	_cutDefinitionButtonGroup->addButton (
					entityRadioButton, QtTopologySplitFacesPanel::CDM_POINT);
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
	         this, SLOT (cutDefinitionModifiedCallback ( )));
	double	r	= ((double)_ratioTextField->sizeHint ( ).width( )) /
								entityRadioButton->sizeHint ( ).width( ) + 0.5;
	gridLayout->setColumnStretch (0, 1);
	gridLayout->setColumnStretch (1, (int)r);
	ratioRadioButton->setFixedSize (entityRadioButton->sizeHint ( ));
	entityRadioButton->setFixedSize (entityRadioButton->sizeHint ( ));

	// Le ratio de découpage de l'éventuel o-grid :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	hlayout->setContentsMargins  (0, 0, 0, 0);
	label	= new QLabel ("Ratio de l'o-grid :", this);
	hlayout->addWidget (label);
	_oGridRatioTextField	= &QtNumericFieldsFactory::createRatioTextField (this);
	_oGridRatioTextField->setValue (0.5);
	hlayout->addWidget (_oGridRatioTextField);
	_oGridRatioTextField->setToolTip (QString::fromUtf8("Ratio de découpage en o-grid (1 face -> 3 faces) si non nul."));
	connect (_oGridRatioTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (parametersModifiedCallback ( )));

	addPreviewCheckBox (false);

	layout->addStretch (2);

	facesModifiedCallback ( );
	cutDefinitionModifiedCallback ( );

	CHECK_NULL_PTR_ERROR (_facesPanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_edgePanel->getNameTextField ( ))
	_facesPanel->getNameTextField (
			)->setLinkedSeizureManagers (0, _edgePanel->getNameTextField ( ));
	_edgePanel->getNameTextField (
			)->setLinkedSeizureManagers (_facesPanel->getNameTextField ( ), 0);
}	// QtTopologySplitFacesPanel::QtTopologySplitFacesPanel


QtTopologySplitFacesPanel::QtTopologySplitFacesPanel (
										const QtTopologySplitFacesPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _facesPanel (0), _cutDefinitionButtonGroup (0), _edgePanel (0),
	  _ratioTextField (0), _cutPointEntityPanel (0),
	  _oGridRatioTextField (0), _allFacesCheckBox (0)
{
	MGX_FORBIDDEN ("QtTopologySplitFacesPanel copy constructor is not allowed.");
}	// QtTopologySplitFacesPanel::QtTopologySplitFacesPanel (const QtTopologySplitFacesPanel&)


QtTopologySplitFacesPanel& QtTopologySplitFacesPanel::operator = (
											const QtTopologySplitFacesPanel&)
{
	MGX_FORBIDDEN ("QtTopologySplitFacesPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologySplitFacesPanel::QtTopologySplitFacesPanel (const QtTopologySplitFacesPanel&)


QtTopologySplitFacesPanel::~QtTopologySplitFacesPanel ( )
{
}	// QtTopologySplitFacesPanel::~QtTopologySplitFacesPanel


void QtTopologySplitFacesPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_facesPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
	CHECK_NULL_PTR_ERROR (_oGridRatioTextField)
	_facesPanel->reset ( );
	_edgePanel->reset ( );
	_ratioTextField->setValue (0.5);
	_cutPointEntityPanel->reset ( );
	_oGridRatioTextField->setValue (0.5);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologySplitFacesPanel::reset


void QtTopologySplitFacesPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_facesPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
	_facesPanel->stopSelection ( );
	_edgePanel->stopSelection ( );
	_cutPointEntityPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_facesPanel->setUniqueName ("");
		_edgePanel->setUniqueName ("");
		_cutPointEntityPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologySplitFacesPanel::cancel


void QtTopologySplitFacesPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_facesPanel)
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)

	Qt::CheckState state =  _allFacesCheckBox->checkState();
	_allFacesCheckBox->setCheckState(Qt::Unchecked);

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		const vector<string>	selectedFaces	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoFace);
		if (0 != selectedFaces.size ( ))
			_facesPanel->setUniqueNames (selectedFaces);
		const vector<string>	selectedEdges	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
		if (0 != selectedEdges.size ( ))
			_edgePanel->setUniqueNames (selectedEdges);
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_facesPanel->clearSelection ( );
	_edgePanel->clearSelection ( );
	_cutPointEntityPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_edgePanel->actualizeGui (true);
	_facesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );

	_allFacesCheckBox->setCheckState(state);

}	// QtTopologySplitFacesPanel::autoUpdate


bool QtTopologySplitFacesPanel::allFaces ( ) const
{
	CHECK_NULL_PTR_ERROR (_allFacesCheckBox)
	return Qt::Checked == _allFacesCheckBox->checkState ( ) ? true : false;
}	// QtTopologySplitFacesPanel::allFaces


vector<string> QtTopologySplitFacesPanel::getFacesNames ( ) const
{
	vector<string>	names;
	CHECK_NULL_PTR_ERROR (_facesPanel)
	if (true == allFaces ( ))
	{
		vector<CoFace*>	faces;
		getContext ( ).getTopoManager ( ).getCoFaces (faces);
		for (vector<CoFace*>::iterator it = faces.begin ( );
		     faces.end ( ) != it; it++)
			names.push_back ((*it)->getUniqueName ( ));
	}
	else
		names	= _facesPanel->getUniqueNames ( );

	return names;
}	// QtTopologySplitFacesPanel::getFacesNames


string QtTopologySplitFacesPanel::getEdgeName ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
	return _edgePanel->getUniqueName ( );
}	// QtTopologySplitFacesPanel::getEdgeName


QtTopologySplitFacesPanel::CUT_DEFINITION_METHOD
					QtTopologySplitFacesPanel::getCutDefinitionMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_cutDefinitionButtonGroup)
	return (QtTopologySplitFacesPanel::CUT_DEFINITION_METHOD)_cutDefinitionButtonGroup->checkedId ( );
}	// QtTopologySplitFacesPanel::getCutDefinitionMethod


Math::Point QtTopologySplitFacesPanel::getCutPoint ( ) const
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
}	// QtTopologySplitFacesPanel::getCutPoint


double QtTopologySplitFacesPanel::getRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_ratioTextField)
	return _ratioTextField->getValue ( );
}	// QtTopologySplitFacesPanel::getRatio


double QtTopologySplitFacesPanel::getOGridRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_oGridRatioTextField)
	return _oGridRatioTextField->getValue ( );
}	// QtTopologySplitFacesPanel::getOGridRatio


void QtTopologySplitFacesPanel::preview (bool show, bool destroyInteractor)
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
		std::unique_ptr<CommandSplitFaces>	command;
		const string			edgeName	= getEdgeName ( );
		const double			oGridRatio	= getOGridRatio ( );
		CoEdge*					edge		=
				 getContext ( ).getTopoManager ( ).getCoEdge (edgeName, true);

		if (true == allFaces ( ))
		{
			switch (getCutDefinitionMethod ( ))
			{
				case QtTopologySplitFacesPanel::CDM_RATIO	:
					command.reset (
						new CommandSplitFaces (
								*context,  edge, getRatio ( ), oGridRatio));
				break;
				case QtTopologySplitFacesPanel::CDM_POINT	:
					command.reset (
						new CommandSplitFaces (
								*context,  edge, getCutPoint ( ), oGridRatio));
				break;
				default	:
					throw Exception (UTF8String ("QtTopologySplitFacesPanel::preview : cas non implémenté.", Charset::UTF_8));
			}	// switch (getCutDefinitionMethod ( ))
		}	// if (true == allFaces ( ))
		else
		{
			const vector<string>	facesNames	= getFacesNames ( );
			vector<CoFace*>			faces;
			for (vector<string>::const_iterator it = facesNames.begin ( );
			     facesNames.end ( ) != it; it++)
			{
				CoFace*			face		=
					getContext ( ).getTopoManager ( ).getCoFace (*it, true);
				faces.push_back (face);
			}	// for (vector<string>::const_iterator it = ...
			switch (getCutDefinitionMethod ( ))
			{
				case QtTopologySplitFacesPanel::CDM_RATIO	:
					command.reset (
						new CommandSplitFaces (
							*context, faces, edge, getRatio ( ), oGridRatio));
				break;
				case QtTopologySplitFacesPanel::CDM_POINT	:
					command.reset (
						new CommandSplitFaces (
							*context, faces, edge, getCutPoint( ), oGridRatio));
				break;
				default	:
					throw Exception (UTF8String ("QtTopologySplitFacesPanel::preview : cas non implémenté.", Charset::UTF_8));
			}	// switch (getCutDefinitionMethod ( ))
		}	// else if (true == allFaces ( ))
		CHECK_NULL_PTR_ERROR (command.get ( ))
		RenderingManager::CommandPreviewMgr	previewManager (
							*command.get ( ), getRenderingManager ( ), true);

		// Ajouter les infos de pré-maillage ?
		if (0 != edge->getDisplayProperties ( ).getGraphicalRepresentation( ))
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

			previewEdges (*command, previewManager, InfoCommand::CREATED,
			              props, mask);
		}	// if (0 != edge->getDisplayProperties ( )...

		getRenderingManager ( ).forceRender ( );

	}
	catch (...)
	{
	}
}	// QtTopologySplitFacesPanel::preview


vector<Entity*> QtTopologySplitFacesPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	if (true == allFaces ( ))
	{
		vector<CoFace*>	faces;
		getContext ( ).getTopoManager ( ).getCoFaces (faces);
		for (vector<CoFace*>::iterator it = faces.begin ( );
		     faces.end ( ) != it; it++)
			entities.push_back (*it);
	}
	else
	{
		const vector<string>	facesNames	= getFacesNames ( );	
		for (vector<string>::const_iterator it = facesNames.begin ( );
		     facesNames.end ( ) != it; it++)
		{
			CoFace*			face		=
				getContext ( ).getTopoManager ( ).getCoFace (*it, false);
			entities.push_back (face);
		}	// for (vector<string>::const_iterator it = ...
	}	// else if (true == allFaces ( ))
	const string			edgeName	= getEdgeName ( );
	TopoEntity*		edge	=
			getContext ( ).getTopoManager ( ).getCoEdge (edgeName, false);
	if (0 != edge)
		entities.push_back (edge);
	switch (getCutDefinitionMethod ( ))
	{
		case QtTopologySplitFacesPanel::CDM_POINT	:
		{
			CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)
			const string	point	= _cutPointEntityPanel->getUniqueName ( );
			if (0 != point.length ( ))
				entities.push_back (&getContext().nameToEntity (point));
		}
		break;
	}	// switch (getCutDefinitionMethod ( ))

	return entities;
}	// QtTopologySplitFacesPanel::getInvolvedEntities


void QtTopologySplitFacesPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _facesPanel)
	{
		_facesPanel->stopSelection ( );
		_facesPanel->setUniqueName ("" );
	}
	if (0 != _edgePanel)
	{
		_edgePanel->stopSelection ( );
		_edgePanel->setUniqueName ("");
	}
	if (0 != _cutPointEntityPanel)
	{
		_cutPointEntityPanel->stopSelection ( );
		_cutPointEntityPanel->setUniqueName ("");
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologySplitFacesPanel::operationCompleted


void QtTopologySplitFacesPanel::cutDefinitionModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_ratioTextField)
	CHECK_NULL_PTR_ERROR (_cutPointEntityPanel)

	switch (getCutDefinitionMethod ( ))
	{
		case QtTopologySplitFacesPanel::CDM_RATIO	:
			_ratioTextField->setEnabled (true);
			_cutPointEntityPanel->setEnabled (false);
			break;
		case QtTopologySplitFacesPanel::CDM_POINT	:
			_ratioTextField->setEnabled (false);
			_cutPointEntityPanel->setEnabled (true);
			break;
	}	// switch (getCutDefinitionMethod ( ))

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologySplitFacesPanel::cutDefinitionModifiedCallback


void QtTopologySplitFacesPanel::facesModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_allFacesCheckBox)
	CHECK_NULL_PTR_ERROR (_facesPanel)

	_facesPanel->setEnabled (
			Qt::Checked == _allFacesCheckBox->checkState ( ) ? false : true);

	parametersModifiedCallback ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologySplitFacesPanel::facesModifiedCallback


// ===========================================================================
//                  LA CLASSE QtTopologySplitFacesAction
// ===========================================================================

QtTopologySplitFacesAction::QtTopologySplitFacesAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologySplitFacesPanel*	operationPanel	= 
			new QtTopologySplitFacesPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologySplitFacesAction::QtTopologySplitFacesAction


QtTopologySplitFacesAction::QtTopologySplitFacesAction (
										const QtTopologySplitFacesAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologySplitFacesAction copy constructor is not allowed.")
}	// QtTopologySplitFacesAction::QtTopologySplitFacesAction


QtTopologySplitFacesAction& QtTopologySplitFacesAction::operator = (
										const QtTopologySplitFacesAction&)
{
	MGX_FORBIDDEN ("QtTopologySplitFacesAction assignment operator is not allowed.")
	return *this;
}	// QtTopologySplitFacesAction::operator =


QtTopologySplitFacesAction::~QtTopologySplitFacesAction ( )
{
}	// QtTopologySplitFacesAction::~QtTopologySplitFacesAction


QtTopologySplitFacesPanel*
					QtTopologySplitFacesAction::getTopologySplitFacesPanel ( )
{
	return dynamic_cast<QtTopologySplitFacesPanel*>(getOperationPanel ( ));
}	// QtTopologySplitFacesAction::getTopologySplitFacesPanel


void QtTopologySplitFacesAction::executeOperation ( )
{
	QtTopologySplitFacesPanel*	panel	= dynamic_cast<QtTopologySplitFacesPanel*>(getTopologySplitFacesPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DOperationAction::executeOperation ( );

	// Récupération des paramètres de découpage des faces topologiques :
	const string	edgeName	= panel->getEdgeName ( );
	const double	oGridRatio	= panel->getOGridRatio ( );

	if (true == panel->allFaces ( ))
	{
		switch (panel->getCutDefinitionMethod ( ))
		{
			case QtTopologySplitFacesPanel::CDM_RATIO	:
				cmdResult	= getContext ( ).getTopoManager ( ).splitAllFaces (edgeName, panel->getRatio ( ), oGridRatio);
				break;
			case QtTopologySplitFacesPanel::CDM_POINT	:
				cmdResult	= getContext ( ).getTopoManager ( ).splitAllFaces (edgeName, panel->getCutPoint ( ), oGridRatio);
				break;
			default	:
			{
				UTF8String	message (Charset::UTF_8);
				message << "Position de découpe non supportée ("
				        << (unsigned long)panel->getCutDefinitionMethod ( )
				        << ").";
				INTERNAL_ERROR (exc, message, "QtTopologySplitFacesAction::executeOperation")
				throw exc;
			}
		}	//  switch (panel->getCutDefinitionMethod ( ))
	}
	else
	{
		vector<string>	facesNames	= panel->getFacesNames ( );
		switch (panel->getCutDefinitionMethod ( ))
		{
			case QtTopologySplitFacesPanel::CDM_RATIO	:
				cmdResult	= getContext ( ).getTopoManager ( ).splitFaces (facesNames, edgeName, panel->getRatio ( ), oGridRatio);
				break;
			case QtTopologySplitFacesPanel::CDM_POINT	:
				cmdResult	= getContext ( ).getTopoManager ( ).splitFaces (facesNames, edgeName, panel->getCutPoint ( ), oGridRatio);
				break;
			default	:
			{
				UTF8String	message (Charset::UTF_8);
				message << "Position de découpe non supportée ("
				        << (unsigned long)panel->getCutDefinitionMethod ( )
				        << ").";
				INTERNAL_ERROR (exc, message, "QtTopologySplitFacesAction::executeOperation")
				throw exc;
			}
		}	// switch (panel->getCutDefinitionMethod ( ))
	}	// else if (true == panel->allFaces ( ))

	setCommandResult (cmdResult);
}	// QtTopologySplitFacesAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
