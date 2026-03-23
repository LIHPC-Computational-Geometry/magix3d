#include "Internal/Context.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtGeometryMoveToAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/RenderedEntityRepresentation.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

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

namespace QtComponents {



// ===========================================================================
//                        LA CLASSE QtGeometryMoveToAction
// ===========================================================================

QtGeometryMoveToPanel::QtGeometryMoveToPanel (
            QWidget* parent, const string& panelName,
            QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
    : QtMgx3DOperationPanel (parent, mainWindow, action,
            QtMgx3DApplication::HelpSystem::instance ( ).geomTranslationOperationURL,
            QtMgx3DApplication::HelpSystem::instance ( ).geomTranslationOperationTag),
        _geomEntitiesPanel (0), _pointPanel (0), _copyCheckBox(0), _namePanel(0)
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

    // Les entités à associer :
    FilterEntity::objectType	filter	=
        (FilterEntity::objectType)(FilterEntity::GeomVertex |
        FilterEntity::GeomCurve | FilterEntity::GeomSurface |
            FilterEntity::GeomVolume);
    _geomEntitiesPanel	= new QtEntityByDimensionSelectorPanel (
                            this, mainWindow, "Entités géométriques :",
                            SelectionManager::ALL_DIMENSIONS,
                            filter, SelectionManager::D3, true);
    _geomEntitiesPanel->setMultiSelectMode (true);
    connect (_geomEntitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
             this, SLOT (entitiesAddedToSelectionCallback (QString)));
    connect (_geomEntitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
             this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
    layout->addWidget (_geomEntitiesPanel);

    _pointPanel	= new QtMgx3DPointPanel (
        this, "Point ", true, "x :", "y :", "z :",
        0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
        &mainWindow, FilterEntity::GeomVertex, true);
    _pointPanel->layout ( )->setSpacing (5);
    connect (_pointPanel, SIGNAL (pointModified ( )), this,
             SLOT (parametersModifiedCallback ( )));
    layout->addWidget (_pointPanel);


    // Faut il faire une copie des entités avant la transformation ?
    _copyCheckBox	=
            new QCheckBox ("Copier avant transformation", this);
    connect (_copyCheckBox, SIGNAL (stateChanged (int)), this,
             SLOT (copyCallback ( )));
    _copyCheckBox->setChecked (false);
    layout->addWidget (_copyCheckBox);

    // Nom groupe (en cas de copie):
    _namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe", mainWindow, 3,
            QtMgx3DGroupNamePanel::CREATION, "");
    layout->addWidget (_namePanel);
    addValidatedField (*_namePanel);

    _namePanel->setEnabled (false);

}   // QtGeometryMoveToPanel::QtGeometryMoveToPanel


QtGeometryMoveToPanel::QtGeometryMoveToPanel (
                                const QtGeometryMoveToPanel& cao)
    : QtMgx3DOperationPanel (
    0, *new QtMgx3DMainWindow(0), 0, "", ""),
        _geomEntitiesPanel (0), _copyCheckBox(0), _namePanel(0)
{
    MGX_FORBIDDEN ("QtGeometryTranslationPanel copy constructor is not allowed.");
}   // QtGeometryMoveToPanel::QtGeometryMoveToPanel (const QtGeometryMoveToPanel& cao)


QtGeometryMoveToPanel& QtGeometryMoveToPanel::operator = (
                                            const QtGeometryMoveToPanel&)
{
    MGX_FORBIDDEN ("QtGeometryTranslationPanel assignment operator is not allowed.");
    return *this;
}   // QtGeometryMoveToPanel& QtGeometryMoveToPanel::operator = (const QtGeometryMoveToPanel&)


QtGeometryMoveToPanel::~QtGeometryMoveToPanel ( )
{
    preview (false, true);
}   // QtGeometryMoveToPanel::~QtGeometryMoveToPanel ( )


void QtGeometryMoveToPanel::setDimension (SelectionManager::DIM dim)
{
    CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
    _geomEntitiesPanel->clearSelection ( );
    _geomEntitiesPanel->setDimensions (dim);
}	// QtGeometryMoveToPanel::setDimension


bool QtGeometryMoveToPanel::copyEntities ( ) const
{
    CHECK_NULL_PTR_ERROR (_copyCheckBox)
    return _copyCheckBox->isChecked ( );
}	// QtGeometryMoveToPanel::copyEntities

string QtGeometryMoveToPanel::getGroupName ( ) const
{
    CHECK_NULL_PTR_ERROR (_namePanel)
    return _namePanel->getGroupName ( );
}	// QtGeometryMoveToPanel::getGroupName

vector<string> QtGeometryMoveToPanel::getGeomEntitiesNames ( ) const
{
    CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
    return _geomEntitiesPanel->getEntitiesNames ( );
}	// QtGeometryMoveToPanel::getGeomEntitiesNames


Math::Point QtGeometryMoveToPanel::getPoint ( ) const
{
    CHECK_NULL_PTR_ERROR (_pointPanel)
    return _pointPanel->getPoint ( );
}	// QtGeometryMoveToPanel::getTranslation


void QtGeometryMoveToPanel::reset ( )
{
    BEGIN_QT_TRY_CATCH_BLOCK

    CHECK_NULL_PTR_ERROR (_namePanel)
    CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
    CHECK_NULL_PTR_ERROR (_pointPanel)
    _namePanel->autoUpdate ( );
    _geomEntitiesPanel->reset ( );
    _pointPanel->reset ( );
    _pointPanel->setX (0);
    _pointPanel->setY (0);
    _pointPanel->setZ (0);

    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

    QtMgx3DOperationPanel::reset ( );
}	// QtGeometryMoveToPanel::reset


void QtGeometryMoveToPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_pointPanel)
	_geomEntitiesPanel->stopSelection ( );
	_pointPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_geomEntitiesPanel->clearSelection ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtGeometryMoveToPanel::cancel


void QtGeometryMoveToPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_geomEntitiesPanel)
	CHECK_NULL_PTR_ERROR (_pointPanel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedGeomEntities	=
					getSelectionManager ( ).getEntitiesNames (
									_geomEntitiesPanel->getAllowedTypes ( ));
		if (0 != selectedGeomEntities.size ( ))
			_geomEntitiesPanel->setEntitiesNames (selectedGeomEntities);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_geomEntitiesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationPanel::autoUpdate ( );

	_namePanel->autoUpdate ( );
	_geomEntitiesPanel->actualizeGui (true);
	_pointPanel->actualizeGui (true);
}	// QtGeometryMoveToPanel::autoUpdate


vector<Entity*> QtGeometryMoveToPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	names	= getGeomEntitiesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		GeomEntity*	entity	=
					getContext ( ).getGeomManager ( ).getEntity (*it, false);
		if (0 != entity)
			entities.push_back (entity);
	}

	return entities;
}	// QtGeometryMoveToPanel::getInvolvedEntities


void QtGeometryMoveToPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _geomEntitiesPanel)
		_geomEntitiesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtGeometryMoveToPanel::operationCompleted

	void QtGeometryMoveToPanel::copyCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_namePanel)

	_namePanel->setEnabled (copyEntities ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtGeometryMoveToPanel::copyCallback


// ===========================================================================
//                  LA CLASSE QtGeometryMoveToAction
// ===========================================================================

QtGeometryMoveToAction::QtGeometryMoveToAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
	QtGeometryMoveToPanel*	operationPanel	=
		new QtGeometryMoveToPanel (
			&getOperationPanelParent ( ), text.toStdString ( ),
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtGeometryMoveToAction::QtGeometryMoveToAction


QtGeometryMoveToAction::QtGeometryMoveToAction (
										const QtGeometryMoveToAction&)
	: QtMgx3DGeomOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtGeometryMoveToAction copy constructor is not allowed.")
}	// QtGeometryMoveToAction::QtGeometryMoveToAction


QtGeometryMoveToAction& QtGeometryMoveToAction::operator = (
										const QtGeometryMoveToAction&)
{
	MGX_FORBIDDEN ("QtGeometryTranslationAction assignment operator is not allowed.")
	return *this;
}	// QtGeometryMoveToAction::operator =


QtGeometryMoveToAction::~QtGeometryMoveToAction ( )
{
}	// QtGeometryMoveToAction::~QtGeometryMoveToAction


QtGeometryMoveToPanel* QtGeometryMoveToAction::getMoveToPanel ( )
{
	return dynamic_cast<QtGeometryMoveToPanel*>(getOperationPanel ( ));
}	// QtGeometryMoveToAction::getMoveToPanel


void QtGeometryMoveToAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResult*	cmdResult	= 0;
	QtMgx3DGeomOperationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités géométriques :
	QtGeometryMoveToPanel*	panel	= getMoveToPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	Math::Point			point	(panel->getPoint ( ));
	const bool			    copy        = panel->copyEntities();
	const string	      	groupName	= panel->getGroupName ( );

		vector<string>			entities	= panel->getGeomEntitiesNames ( );
		if (copy)
			cmdResult	= getContext ( ).getGeomManager ( ).copyAndMove (entities, point, groupName);
		else
			cmdResult	= getContext ( ).getGeomManager ( ).moveTo (entities, point);
		// else if (true == panel->processAllGeomEntities ( ))

	setCommandResult (cmdResult);
}	// QtGeometryMoveToAction::executeOperation

}	// namespace QtComponents

}	// namespace Mgx3D