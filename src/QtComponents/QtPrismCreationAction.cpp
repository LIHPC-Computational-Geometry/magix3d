/**
 * \file        QtPrismCreationAction.cpp
 * \author      Charles PIGNEROL
 * \date        14/11/2014
 *
 *
 *
 *  Modified on: 21/02/2022
 *      Author: Simon C
 *      ajout de la possibilité de conserver les entités géométriques
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "Geom/Surface.h"
#include "QtComponents/QtPrismCreationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/MemoryError.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QVBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Group;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtPrismCreationPanel
// ===========================================================================

QtPrismCreationPanel::QtPrismCreationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).prismOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).prismOperationTag),
	  _surfacesPanel (0), _vectorPanel (0),
	  _withTopologyCheckBox (0), _keepEntitiesCheckBox (0)
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

	// Surfaces de base :
	_surfacesPanel	= new QtMgx3DEntityPanel (
						this, "", true, "Surfaces à extruder :", "",
						&mainWindow, SelectionManagerIfc::D2,
						FilterEntity::GeomSurface);
	_surfacesPanel->setMultiSelectMode (true);
	layout->addWidget (_surfacesPanel);
	connect (_surfacesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_surfacesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	
	// Définition du vecteur d'extrusion :
	_vectorPanel	=
				new QtMgx3DVectorPanel (this, "", true,
					"dx :", "dy :", "dz :",
					0., -DBL_MAX, DBL_MAX,
					0., -DBL_MAX, DBL_MAX,
					1., -DBL_MAX, DBL_MAX,
					&mainWindow, FilterEntity::AllEdges, true);
	connect (_vectorPanel, SIGNAL (vectorModified ( )), this,
	         SLOT (parametersModifiedCallback ( )));
	layout->addWidget (_vectorPanel);

	// Faut il propager la copie aux entités topologiques associées ?
	_withTopologyCheckBox	=
			new QCheckBox ("Extrusion de la topologie", this);
	_withTopologyCheckBox->setChecked (true);
	layout->addWidget (_withTopologyCheckBox);

    // Conserver les entités initiales ?
    _keepEntitiesCheckBox	=
            new QCheckBox (QString::fromUtf8("Conserver les entités initiales"), this);
    _keepEntitiesCheckBox->setCheckState (Qt::Unchecked);
    layout->addWidget (_keepEntitiesCheckBox);

	layout->addStretch (2);
}	// QtPrismCreationPanel::QtPrismCreationPanel


QtPrismCreationPanel::QtPrismCreationPanel (const QtPrismCreationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _surfacesPanel (0), _vectorPanel(0), _withTopologyCheckBox (0), _keepEntitiesCheckBox (0)
{
	MGX_FORBIDDEN ("QtPrismCreationPanel copy constructor is not allowed.");
}	// QtPrismCreationPanel::QtPrismCreationPanel (const QtPrismCreationPanel&)


QtPrismCreationPanel&
			QtPrismCreationPanel::operator = (const QtPrismCreationPanel&)
{
	MGX_FORBIDDEN ("QtPrismCreationPanel assignment operator is not allowed.");
	return *this;
}	// QtPrismCreationPanel::QtPrismCreationPanel (const QtPrismCreationPanel&)


QtPrismCreationPanel::~QtPrismCreationPanel ( )
{
}	// QtPrismCreationPanel::~QtPrismCreationPanel


vector<string> QtPrismCreationPanel::getSurfaceNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_surfacesPanel)
	return _surfacesPanel->getUniqueNames ( );
}	// QtPrismCreationPanel::getSurfaceNames


Vector QtPrismCreationPanel::getVector ( ) const
{
	 CHECK_NULL_PTR_ERROR (_vectorPanel)

	return Vector (_vectorPanel->getDx ( ),
	               _vectorPanel->getDy ( ),
	               _vectorPanel->getDz ( ));
}	// QtPrismCreationPanel::getVector


bool QtPrismCreationPanel::withTopology ( ) const
{
	CHECK_NULL_PTR_ERROR (_withTopologyCheckBox)
	return Qt::Checked == _withTopologyCheckBox->checkState ( ) ? true : false;
}	// QtPrismCreationPanel::withTopology


void QtPrismCreationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_surfacesPanel)
	CHECK_NULL_PTR_ERROR (_vectorPanel)
	_surfacesPanel->reset ( );
	_vectorPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtPrismCreationPanel::reset


void QtPrismCreationPanel::validate ( )
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
		error << "QtPrismCreationPanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtPrismCreationPanel::validate


void QtPrismCreationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_surfacesPanel)
	_surfacesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_surfacesPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtPrismCreationPanel::cancel


void QtPrismCreationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_surfacesPanel)
	CHECK_NULL_PTR_ERROR (_vectorPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedSurfaces	=
			getSelectionManager ( ).getEntitiesNames(FilterEntity::GeomSurface);
		if (1 == selectedSurfaces.size ( ))
			_surfacesPanel->setUniqueName (selectedSurfaces [0]);
		vector<string>	selectedEdges	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::AllEdges);
		if (1 == selectedEdges.size ( ))
			_vectorPanel->setSegment (selectedEdges [0]);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_surfacesPanel->clearSelection ( );
	_vectorPanel->setSegment ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationPanel::autoUpdate ( );

	_surfacesPanel->actualizeGui (true);
	_vectorPanel->actualizeGui (true);
}	// QtPrismCreationPanel::autoUpdate


vector<Entity*> QtPrismCreationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	const vector<string>	names	= getSurfaceNames ( );

	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*		surface	=
			getContext ( ).getGeomManager ( ).getSurface (*it, false);
		if (0 != surface)
			entities.push_back (surface);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtPrismCreationPanel::getInvolvedEntities


void QtPrismCreationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_surfacesPanel)
	_surfacesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtPrismCreationPanel::operationCompleted


bool QtPrismCreationPanel::keepEntities ( ) const
{
    CHECK_NULL_PTR_ERROR (_keepEntitiesCheckBox)
    return Qt::Checked == _keepEntitiesCheckBox->checkState ( ) ? true : false;
}	// QtGeomEntityByRevolutionCreationPanel::keepEntities

// ===========================================================================
//                  LA CLASSE QtPrismCreationAction
// ===========================================================================

QtPrismCreationAction::QtPrismCreationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtPrismCreationPanel*	operationPanel	=
		new QtPrismCreationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, creationPolicy, this);
	setOperationPanel (operationPanel);
}	// QtPrismCreationAction::QtPrismCreationAction


QtPrismCreationAction::QtPrismCreationAction (
										const QtPrismCreationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0, ""), "")
{
	MGX_FORBIDDEN ("QtPrismCreationAction copy constructor is not allowed.")
}	// QtPrismCreationAction::QtPrismCreationAction


QtPrismCreationAction&
					 QtPrismCreationAction::operator = (
									const QtPrismCreationAction&)
{
	MGX_FORBIDDEN ("QtPrismCreationAction assignment operator is not allowed.")
	return *this;
}	// QtPrismCreationAction::operator =


QtPrismCreationAction::~QtPrismCreationAction ( )
{
}	// QtPrismCreationAction::~QtPrismCreationAction


QtPrismCreationPanel* QtPrismCreationAction::getPrismPanel ( )
{
	return dynamic_cast<QtPrismCreationPanel*>(getOperationPanel ( ));
}	// QtPrismCreationAction::getPrismPanel


void QtPrismCreationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres de création des prismes :
	QtPrismCreationPanel*	panel	= getPrismPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	std::vector<std::string>	surfaces	= panel->getSurfaceNames ( );
	const Vector	v		= panel->getVector ( );
	const bool		withTopo	= panel->withTopology ( );
    const bool		keepEntities	= panel->keepEntities ( );
	if (withTopo)
		cmdResult	= getContext ( ).getGeomManager ( ).makeBlocksByExtrude(surfaces, v, keepEntities);
	else
		cmdResult	= getContext ( ).getGeomManager ( ).makeExtrude (surfaces, v, keepEntities);

	setCommandResult (cmdResult);
}	// QtPrismCreationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
