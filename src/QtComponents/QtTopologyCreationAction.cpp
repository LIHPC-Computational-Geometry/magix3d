/**
 * \file        QtTopologyCreationAction.cpp
 * \author      Charles PIGNEROL
 * \date        20/11/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtTopologyCreationAction.h"
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
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtTopologyCreationPanel
// ===========================================================================

QtTopologyCreationPanel::QtTopologyCreationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			SelectionManagerIfc::DIM dimension,
			QtTopologyPanel::TOPOLOGY_TYPE defaultTopo,
			QtTopologyPanel::COORDINATES_TYPE defaultCoords,
			const string& helpURL,
			const string& helpTag)
	: QtMgx3DOperationPanel (parent, mainWindow, action, helpURL, helpTag),
	  _geomEntityPanel (0), _topologyPanel (0)
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

	// L'entité géométrique à associer :
	FilterEntity::objectType	filter	= (FilterEntity::objectType)0;
	string entityNameType = "A définir";
	switch (dimension)
	{
		case SelectionManagerIfc::D2	:
					filter	= FilterEntity::GeomSurface;
					entityNameType = "Surface :";
					break;
		case SelectionManagerIfc::D3	:
					filter	= FilterEntity::GeomVolume;
					entityNameType = "Volume :";
					break;
	}	// switch (dimension)
	_geomEntityPanel	= new QtMgx3DEntityPanel (
							this, "", true, entityNameType, "",
							&mainWindow, dimension, filter);
	layout->addWidget (_geomEntityPanel);
	connect (_geomEntityPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_geomEntityPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	QLabel* ouLabel = new QLabel("Ou", this);
	layout->addWidget (ouLabel);

	// Le panneau "Topologie créée" :
	bool	use3DLabels	=
		SelectionManagerIfc::D3 <= dimension ? true : false;
	int	groupDimension	= SelectionManagerIfc::dimensionsToDimension(dimension);
	_topologyPanel	= new QtTopologyPanel (
		this, mainWindow, true, true, groupDimension,
		defaultTopo, defaultCoords, 10, 10, 10, use3DLabels, true);
	_topologyPanel->createTopology (true);
	_topologyPanel->displayTopologyCreation (false);
	connect (_topologyPanel, SIGNAL (topologyCreationModified ( )), this,
	         SLOT (topologyModifiedCallback ( )));
	connect (_topologyPanel, SIGNAL (topologyTypeModified ( )), this,
	         SLOT (topologyModifiedCallback ( )));
	layout->addWidget (_topologyPanel);

	CHECK_NULL_PTR_ERROR (_geomEntityPanel->getNameTextField ( ))
	_geomEntityPanel->getNameTextField ( )->setLinkedSeizureManagers (0, 0);

	topologyModifiedCallback ( );

	layout->addStretch (2);

}	// QtTopologyCreationPanel::QtTopologyCreationPanel


QtTopologyCreationPanel::QtTopologyCreationPanel (
										const QtTopologyCreationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _geomEntityPanel (0), _topologyPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyCreationPanel copy constructor is not allowed.");
}	// QtTopologyCreationPanel::QtTopologyCreationPanel (const QtTopologyCreationPanel&)


QtTopologyCreationPanel& QtTopologyCreationPanel::operator = (
											const QtTopologyCreationPanel&)
{
	MGX_FORBIDDEN ("QtTopologyCreationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyCreationPanel::QtTopologyCreationPanel (const QtTopologyCreationPanel&)


QtTopologyCreationPanel::~QtTopologyCreationPanel ( )
{
}	// QtTopologyCreationPanel::~QtTopologyCreationPanel


string QtTopologyCreationPanel::getGeomEntityName ( ) const
{
	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
	return _geomEntityPanel->getUniqueName ( );
}	// QtTopologyCreationPanel::getGeomEntityName


string QtTopologyCreationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getGroupName ( );
}	// QtTopologyCreationPanel::getGroupName


QtTopologyPanel::TOPOLOGY_TYPE QtTopologyCreationPanel::getTopologyType( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return _topologyPanel->getTopologyType ( );
}	// QtTopologyCreationPanel::getTopologyType


double QtTopologyCreationPanel::getOGridRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	 return _topologyPanel->getOGridRatio ( );
}	// QtTopologyCreationPanel::getOGridRatio


void QtTopologyCreationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	_geomEntityPanel->reset ( );
	_topologyPanel->setTopologyType (_topologyPanel->getDefaultTopologyType( ));
	_topologyPanel->setAxe1EdgesNum (10);
	_topologyPanel->setAxe2EdgesNum (10);
	_topologyPanel->setAxe3EdgesNum (10);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtTopologyCreationPanel::reset


void QtTopologyCreationPanel::validate ( )
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
		error << "QtTopologyCreationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getTopoEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopologyCreationPanel::validate


void QtTopologyCreationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
	_geomEntityPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		setGeomEntityName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyCreationPanel::cancel


void QtTopologyCreationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
	CHECK_NULL_PTR_ERROR (_geomEntityPanel->getNameTextField ( ))

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		FilterEntity::objectType	types	=
				_geomEntityPanel->getNameTextField ( )->getFilteredTypes ( );
		vector<string>	selectedVolumes	=
							getSelectionManager ( ).getEntitiesNames (types);
		if (1 == selectedVolumes.size ( ))
			setGeomEntityName (selectedVolumes [0]);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_geomEntityPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_geomEntityPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyCreationPanel::autoUpdate


vector<Entity*> QtTopologyCreationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const string	geomName	= getGeomEntityName ( );

	if (0 != geomName.length ( ))
		entities.push_back (
				getContext ( ).getGeomManager ( ).getEntity (geomName, true));

	return entities;
}	// QtTopologyCreationPanel::getInvolvedEntities


void QtTopologyCreationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _geomEntityPanel)
		_geomEntityPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyCreationPanel::operationCompleted


void QtTopologyCreationPanel::setGeomEntityName (const string& name)
{
	CHECK_NULL_PTR_ERROR (_geomEntityPanel)
	_geomEntityPanel->setUniqueName (name);
}	// QtTopologyCreationPanel::setGeomEntityName


QtTopologyPanel& QtTopologyCreationPanel::getTopologyPanel ( )
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return *_topologyPanel;
}	// QtTopologyCreationPanel::getTopologyPanel


const QtTopologyPanel& QtTopologyCreationPanel::getTopologyPanel ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyPanel)
	return *_topologyPanel;
}	// QtTopologyCreationPanel::getTopologyPanel


void QtTopologyCreationPanel::topologyModifiedCallback ( )
{
	if (0 != getMainWindow ( ))
		getMainWindow ( )->getOperationsPanel ( ).updateLayoutWorkaround( );

	bool	allowGroup	= false;
	switch (getTopologyType ( ))
	{
		case QtTopologyPanel::STRUCTURED_FREE_TOPOLOGY	:
			allowGroup	= true;
			break;
	}	// switch (getTopologyType ( ))
	getTopologyPanel ( ).allowGroupName (allowGroup);
}	// QtTopologyCreationPanel::topologyModifiedCallback


// ===========================================================================
//                  LA CLASSE QtTopologyCreationAction
// ===========================================================================

QtTopologyCreationAction::QtTopologyCreationAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
}	// QtTopologyCreationAction::QtTopologyCreationAction


QtTopologyCreationAction::QtTopologyCreationAction (const QtTopologyCreationAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyCreationAction copy constructor is not allowed.")
}	// QtTopologyCreationAction::QtTopologyCreationAction


QtTopologyCreationAction& QtTopologyCreationAction::operator = (const QtTopologyCreationAction&)
{
	MGX_FORBIDDEN ("QtTopologyCreationAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyCreationAction::operator =


QtTopologyCreationAction::~QtTopologyCreationAction ( )
{
}	// QtTopologyCreationAction::~QtTopologyCreationAction


QtTopologyCreationPanel* QtTopologyCreationAction::getTopologyCreationPanel ( )
{
	return dynamic_cast<QtTopologyCreationPanel*>(getOperationPanel ( ));
}	// QtTopologyCreationAction::getTopologyCreationPanel


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
