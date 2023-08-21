/**
 * \file        QtGeomSurfaceCreationAction.cpp
 * \author      Charles PIGNEROL
 * \date        20/11/2014
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Geom/Vertex.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeomSurfaceCreationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>

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
//                        LA CLASSE QtGeomSurfaceCreationPanel
// ===========================================================================

QtGeomSurfaceCreationPanel::QtGeomSurfaceCreationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).surfaceCreationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).surfaceCreationTag),
	  _namePanel (0), _contourEntitiesPanel (0)
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

	// Nom groupe :
	_namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe", mainWindow, 2,
										QtMgx3DGroupNamePanel::CREATION, "");
	layout->addWidget (_namePanel);
	addValidatedField (*_namePanel);

	// Les courbes définissant le contour de la surface à créer :
	const FilterEntity::objectType	filter	=
				(FilterEntity::objectType)(
							FilterEntity::GeomVertex | FilterEntity::GeomCurve);
	const SelectionManagerIfc::DIM	dimensions	=
				(SelectionManagerIfc::DIM)(
							SelectionManagerIfc::D0 | SelectionManagerIfc::D1);
	_contourEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
						this, mainWindow, "Entités géométriques :", dimensions,
						filter, SelectionManagerIfc::D1, false);
	_contourEntitiesPanel->setMultiSelectMode (true);
	connect (_contourEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_contourEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	connect (_contourEntitiesPanel, SIGNAL (dimensionsModified ( )),
	         this, SLOT (dimensionsModifiedCallback ( )));
	layout->addWidget (_contourEntitiesPanel);

	layout->addStretch (2);

	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel->getEntitiesPanel ( ))
	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel->getEntitiesPanel ( )->getNameTextField ( ))
	_contourEntitiesPanel->getEntitiesPanel ( )->getNameTextField (
											)->setLinkedSeizureManagers (0, 0);
}	// QtGeomSurfaceCreationPanel::QtGeomSurfaceCreationPanel


QtGeomSurfaceCreationPanel::QtGeomSurfaceCreationPanel (
									const QtGeomSurfaceCreationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _namePanel (0), _contourEntitiesPanel (0)
{
	MGX_FORBIDDEN ("QtGeomSurfaceCreationPanel copy constructor is not allowed.");
}	// QtGeomSurfaceCreationPanel::QtGeomSurfaceCreationPanel (const QtGeomSurfaceCreationPanel&)


QtGeomSurfaceCreationPanel& QtGeomSurfaceCreationPanel::operator = (
										const QtGeomSurfaceCreationPanel&)
{
	MGX_FORBIDDEN ("QtGeomSurfaceCreationPanel assignment operator is not allowed.");
	return *this;
}	// QtGeomSurfaceCreationPanel::QtGeomSurfaceCreationPanel (const QtGeomSurfaceCreationPanel&)


QtGeomSurfaceCreationPanel::~QtGeomSurfaceCreationPanel ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtGeomSurfaceCreationPanel::~QtGeomSurfaceCreationPanel


string QtGeomSurfaceCreationPanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_namePanel)
	return _namePanel->getGroupName ( );
}	// QtGeomSurfaceCreationPanel::getGroupName


int QtGeomSurfaceCreationPanel::getDimension ( ) const
{
	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)
	return SelectionManagerIfc::dimensionsToDimension (
									_contourEntitiesPanel->getDimensions ( ));
}	// QtGeomSurfaceCreationPanel::getDimension


void QtGeomSurfaceCreationPanel::setDimension (int dimension)
{
	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_namePanel)
	if (dimension != getDimension ( ))
	{
		_contourEntitiesPanel->clearSelection ( );
		_contourEntitiesPanel->setDimensions (
						SelectionManagerIfc::dimensionToDimensions (dimension));
		_namePanel->setDimension (dimension);
	}	// if (dimension != getDimension ( ))
}	// QtGeomSurfaceCreationPanel::setDimension


vector<string> QtGeomSurfaceCreationPanel::getEntitiesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)
	return _contourEntitiesPanel->getEntitiesNames ( );
}	// QtGeomSurfaceCreationPanel::getEntitiesNames


void QtGeomSurfaceCreationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)
	_contourEntitiesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtGeomSurfaceCreationPanel::reset


void QtGeomSurfaceCreationPanel::validate ( )
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
		error << "QtGeomSurfaceCreationPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (getGeomEntitiesNames->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtGeomSurfaceCreationPanel::validate


void QtGeomSurfaceCreationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)
	_contourEntitiesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_contourEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeomSurfaceCreationPanel::cancel


void QtGeomSurfaceCreationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		if (0 != _namePanel)
			_namePanel->autoUpdate ( );
		FilterEntity::objectType	filter	=
				(FilterEntity::objectType)(FilterEntity::GeomVertex |
					FilterEntity::GeomCurve);
		vector<string>	geomEntities	=
							getSelectionManager ( ).getEntitiesNames (filter);
		CHECK_NULL_PTR_ERROR (_contourEntitiesPanel)	
		if (0 != geomEntities.size ( ))
			_contourEntitiesPanel->setEntitiesNames (geomEntities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	if (0 != _namePanel)
		_namePanel->autoUpdate ( );
	_contourEntitiesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	_contourEntitiesPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtGeomSurfaceCreationPanel::autoUpdate


vector<Entity*> QtGeomSurfaceCreationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	names	= getEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*	entity	= getContext ( ).getGeomManager ( ).getEntity (*it);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}

	return entities;
}	// QtGeomSurfaceCreationPanel::getInvolvedEntities


void QtGeomSurfaceCreationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _contourEntitiesPanel)
		_contourEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );

	// Actualisation des groupes : certains peuvent être créés, d'autres
	// détruits (ex : "Hors Groupes 3D" :
	if (0 != _namePanel)
		_namePanel->autoUpdate ( );
}	// QtGeomSurfaceCreationPanel::operationCompleted


void QtGeomSurfaceCreationPanel::dimensionsModifiedCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _namePanel)
	{
		_namePanel->setDimension (getDimension ( ));
		_namePanel->autoUpdate ( );
	}	// if (0 != _namePanel)
	
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeomSurfaceCreationPanel::dimensionsModifiedCallback


// ===========================================================================
//                  LA CLASSE QtGeomSurfaceCreationAction
// ===========================================================================

QtGeomSurfaceCreationAction::QtGeomSurfaceCreationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeomSurfaceCreationPanel*	operationPanel	=
		new QtGeomSurfaceCreationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeomSurfaceCreationAction::QtGeomSurfaceCreationAction


QtGeomSurfaceCreationAction::QtGeomSurfaceCreationAction (
										const QtGeomSurfaceCreationAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeomSurfaceCreationAction copy constructor is not allowed.")
}	// QtGeomSurfaceCreationAction::QtGeomSurfaceCreationAction


QtGeomSurfaceCreationAction& QtGeomSurfaceCreationAction::operator = (
										const QtGeomSurfaceCreationAction&)
{
	MGX_FORBIDDEN ("QtGeomSurfaceCreationAction assignment operator is not allowed.")
	return *this;
}	// QtGeomSurfaceCreationAction::operator =


QtGeomSurfaceCreationAction::~QtGeomSurfaceCreationAction ( )
{
}	// QtGeomSurfaceCreationAction::~QtGeomSurfaceCreationAction


QtGeomSurfaceCreationPanel*
					QtGeomSurfaceCreationAction::getSurfaceCreationPanel ( )
{
	return dynamic_cast<QtGeomSurfaceCreationPanel*>(getOperationPanel ( ));
}	// QtGeomSurfaceCreationAction::getSurfaceCreationPanel


void QtGeomSurfaceCreationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres d'addition des entités géométriques :
	QtGeomSurfaceCreationPanel*	panel	= getSurfaceCreationPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	const string		groupName	= panel->getGroupName ( );
	vector<string>		entities	= panel->getEntitiesNames ( );
	const int			dim			= panel->getDimension ( );

	
	switch (dim)
	{
		case 0	:
		{
			vector<Math::Point>	points;
			for (vector<string>::const_iterator itn = entities.begin ( );
			     itn != entities.end ( ); itn++)
			{
				Entity&	entity	= getContext().nameToEntity (*itn);
				Vertex*	vertex	= dynamic_cast<Vertex*>(&entity);
				CHECK_NULL_PTR_ERROR (vertex)
				points.push_back (vertex->getPoint ( ));
			}	// for (vector<string>::const_iterator itn = ...
			cmdResult	= getContext ( ).getGeomManager ( ).newVerticesCurvesAndPlanarSurface (points, groupName);
		}	// case 0	
		break;
		case 1	:
			cmdResult	= getContext ( ).getGeomManager ( ).newPlanarSurface (entities, groupName);
			break;
		default	:
			UTF8String	message (Charset::UTF_8);
			message << "Dimension invalide pour les entités définissant le "
			        << "contour d'une surface à créer : " << (unsigned long)dim
			        << ". Dimensions acceptées : 0-1.";
			INTERNAL_ERROR (exc, "QtGeomSurfaceCreationAction::executeOperation", message)
			throw exc;
	}	// switch (dim)

	setCommandResult (cmdResult);
}	// QtGeomSurfaceCreationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
