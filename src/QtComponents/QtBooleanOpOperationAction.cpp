/*
 * QtBooleanOpOperationAction.cpp
 *
 *  Created on: 20 mars 2013
 *      Author: ledouxf
 */




/**
 * \file        QtBooleanOpOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        11/12/2012
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ValidatedField.h"
#include "Geom/GeomManagerIfc.h"
#include "Utils/Vector.h"
#include "QtComponents/QtBooleanOpOperationAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtErrorManagement.h>

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QVBoxLayout>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Group;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtBooleanOpOperationPanel
// ===========================================================================

QtBooleanOpOperationPanel::QtBooleanOpOperationPanel (
            QWidget* parent, const string& panelName,
            QtMgx3DGroupNamePanel::POLICY creationPolicy,
            QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
    : QtMgx3DOperationPanel (parent, mainWindow, action,
            QtMgx3DApplication::HelpSystem::instance ( ).fuseOperationURL,
            QtMgx3DApplication::HelpSystem::instance ( ).fuseOperationTag),
      //_namePanel (0),
			_operationTypeComboBox(0), _entitiesPanel (0), _toolsToCutPanel(0)
{
/*
setAutoFillBackground (true);
QPalette    p   = palette ( );
p.setColor (QPalette::Active, QPalette::Window, Qt::cyan);
setPalette (p);
*/
    QVBoxLayout*    layout  = new QVBoxLayout (this);
    setLayout (layout);


    // Nom opération :
    QLabel* label   = new QLabel (panelName.c_str ( ), this);
    layout->addWidget (label);

    // Type d'opération booléenne
    QHBoxLayout*    hlayout = new QHBoxLayout (0);
    layout->addLayout (hlayout);
    label   = new QLabel (QString::fromUtf8("Opération"), this);
    hlayout->addWidget (label);
    _operationTypeComboBox    = new QComboBox (this);
    _operationTypeComboBox->addItem ("Collage");
    _operationTypeComboBox->addItem ("Fusion (Union)");
    _operationTypeComboBox->addItem ("Intersection");
    _operationTypeComboBox->addItem (QString::fromUtf8("Différence"));
    _operationTypeComboBox->addItem ("Section");

    hlayout->addWidget (_operationTypeComboBox);
    hlayout->addStretch (10);

    // Nom volumes sur lesquels se font l'opération
    _entitiesPanel  = new QtMgx3DEntityNamePanel (
			this, "Volumes", mainWindow, SelectionManagerIfc::D3, FilterEntity::GeomVolume);
	connect (_entitiesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_entitiesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));


    // Nom volumes à supprimer pour le CUT
    _toolsToCutPanel  = new QtMgx3DEntityNamePanel (
            this, "a définir", mainWindow, SelectionManagerIfc::D3, FilterEntity::GeomVolume);

    layout->addWidget (_entitiesPanel);
    layout->addWidget (_toolsToCutPanel);

    _toolsToCutPanel->hide();

    connect (_operationTypeComboBox, SIGNAL (activated(int)),
            this, SLOT (newOperationType(int)));
	connect (_toolsToCutPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_toolsToCutPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
    // addValidatedField (*_entitiesPanel);

	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	CHECK_NULL_PTR_ERROR (_toolsToCutPanel)
	CHECK_NULL_PTR_ERROR (_entitiesPanel->getNamesTextField ( ))
	CHECK_NULL_PTR_ERROR (_toolsToCutPanel->getNamesTextField ( ))
	_entitiesPanel->getNamesTextField ( )->setLinkedSeizureManagers (
									0, _toolsToCutPanel->getNamesTextField ( ));
	_toolsToCutPanel->getNamesTextField ( )->setLinkedSeizureManagers (
									_entitiesPanel->getNamesTextField ( ), 0);
	layout->addStretch (2);

}   // QtBooleanOpOperationPanel::QtBooleanOpOperationPanel


QtBooleanOpOperationPanel::QtBooleanOpOperationPanel (
                                        const QtBooleanOpOperationPanel& cao)
    : QtMgx3DOperationPanel (
            0, *new QtMgx3DMainWindow(0), 0, "", ""),
      //_namePanel (0),
			_operationTypeComboBox(0), _entitiesPanel (0), _toolsToCutPanel(0)
{
    MGX_FORBIDDEN ("QtBooleanOpOperationPanel copy constructor is not allowed.");
}   // QtBooleanOpOperationPanel::QtBooleanOpOperationPanel (const QtBooleanOpOperationPanel&)


QtBooleanOpOperationPanel& QtBooleanOpOperationPanel::operator = (
                                            const QtBooleanOpOperationPanel&)
{
    MGX_FORBIDDEN ("QtBooleanOpOperationPanel assignment operator is not allowed.");
    return *this;
}   // QtBooleanOpOperationPanel::QtBooleanOpOperationPanel (const QtBooleanOpOperationPanel&)


QtBooleanOpOperationPanel::~QtBooleanOpOperationPanel ( )
{
}   // QtBooleanOpOperationPanel::~QtBooleanOpOperationPanel


void QtBooleanOpOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	CHECK_NULL_PTR_ERROR (_toolsToCutPanel)
	_entitiesPanel->reset ( );
	_toolsToCutPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtBooleanOpOperationPanel::reset



QtBooleanOpOperationPanel::OPERATION_TYPE
QtBooleanOpOperationPanel::getOperationType ( ) const
{
    CHECK_NULL_PTR_ERROR (_operationTypeComboBox)
    QtBooleanOpOperationPanel::OPERATION_TYPE r = GLUE;

	switch (_operationTypeComboBox->currentIndex ( ))
	{
		case	0	: return GLUE;
		case	1	: return FUSE;
		case	2	: return COMMON;
		case	3	: return CUT;
		case	4	: return SECTION;
	}

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "Cas non implémenté : "
	        << (unsigned long)_operationTypeComboBox->currentIndex ( ) << ".";
	INTERNAL_ERROR (exc, message, "QtBooleanOpOperationPanel::getOperationType")
	throw exc;
}   // QtBooleanOpOperationPanel::getOperationType

vector<string> QtBooleanOpOperationPanel::getEntityNames ( ) const
{
    CHECK_NULL_PTR_ERROR (_entitiesPanel)
    return _entitiesPanel->getNames();
}   // QtBooleanOpOperationPanel::getEntityNames

vector<string> QtBooleanOpOperationPanel::getCuttingNames() const
{
    CHECK_NULL_PTR_ERROR (_toolsToCutPanel)
    return _toolsToCutPanel->getNames();
}   // QtBooleanOpOperationPanel::getCuttingNames


void QtBooleanOpOperationPanel::validate ( )
{
// CP : suite discussion EBL/FL, il est convenu que la validation des
// paramètres de l'opération est effectuée par le "noyau" et qu'un mauvais
// paramétrage est remonté sous forme d'exception à la fonction appelante, donc
// avant exécution de la commande.
// Les validations des valeurs des paramètres sont donc ici commentées.
	TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);

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
        error << "QtBooleanOpOperationPanel::validate : erreur non documentée.";
    }

    if (getEntityNames().empty())
        throw Exception (UTF8String ("Il faut au moins un volume pour effectuer une opération", Charset::UTF_8));

    QtBooleanOpOperationPanel::OPERATION_TYPE type = getOperationType();
    if ((type==QtBooleanOpOperationPanel::SECTION || type==QtBooleanOpOperationPanel::CUT)
            && getCuttingNames().empty())
        throw Exception (UTF8String ("Il faut au moins une entité pour couper", Charset::UTF_8));


    if (0 != error.length ( ))
        throw Exception (error);
}   // QtBooleanOpOperationPanel::validate

void QtBooleanOpOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_entitiesPanel)

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_entitiesPanel->clear ( );
		if (0 != _toolsToCutPanel)
			_toolsToCutPanel->clear ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}   // QtBooleanOpOperationPanel::cancel


void QtBooleanOpOperationPanel::autoUpdate ( )
{
#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedVolumes	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::GeomVolume);
		if ((0 != _entitiesPanel) && (0 != selectedVolumes.size ( )))
			_entitiesPanel->setNames (selectedVolumes);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	if (0 != _entitiesPanel)
		_entitiesPanel->clear ( );
	if (0 != _toolsToCutPanel)
		_toolsToCutPanel->clear ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

    QtMgx3DOperationPanel::autoUpdate ( );
}   // QtBooleanOpOperationPanel::autoUpdate


void QtBooleanOpOperationPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _entitiesPanel){
		_entitiesPanel->stopSelection ( );
		_entitiesPanel->clear ( );
	}
	if (0 != _toolsToCutPanel){
		_toolsToCutPanel->stopSelection ( );
		_toolsToCutPanel->clear ( );
	}
	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtBooleanOpOperationPanel::operationCompleted


vector<Entity*> QtBooleanOpOperationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;
	vector<string>	names	= getEntityNames ( );

	for (vector<string>::const_iterator it = names.begin ( );
	     it != names.end ( ); it++)
		entities.push_back (getContext ( ).getGeomManager ( ).getEntity (*it, true));

	names	= getCuttingNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     it != names.end ( ); it++)
		entities.push_back (getContext ( ).getGeomManager ( ).getEntity (*it, true));

	return entities;
}	// QtBooleanOpOperationPanel::getInvolvedEntities


// ===========================================================================
//                  LA CLASSE QtBooleanOpOperationAction
// ===========================================================================

QtBooleanOpOperationAction::QtBooleanOpOperationAction (
    const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
    const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
    : QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip)
{
    QtBooleanOpOperationPanel*   operationPanel  =
        new QtBooleanOpOperationPanel (
            &getOperationPanelParent ( ), text.toStdString ( ), creationPolicy,
            mainWindow, this);
    setOperationPanel (operationPanel);
}   // QtBooleanOpOperationAction::QtBooleanOpOperationAction


QtBooleanOpOperationAction::QtBooleanOpOperationAction (
                                        const QtBooleanOpOperationAction&)
    : QtMgx3DGeomOperationAction (
                        QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
    MGX_FORBIDDEN ("QtBooleanOpOperationAction copy constructor is not allowed.")
}   // QtBooleanOpOperationAction::QtBooleanOpOperationAction


QtBooleanOpOperationAction& QtBooleanOpOperationAction::operator = (
                                        const QtBooleanOpOperationAction&)
{
    MGX_FORBIDDEN ("QtBooleanOpOperationAction assignment operator is not allowed.")
    return *this;
}   // QtBooleanOpOperationAction::operator =


QtBooleanOpOperationAction::~QtBooleanOpOperationAction ( )
{
}   // QtBooleanOpOperationAction::~QtBooleanOpOperationAction


QtBooleanOpOperationPanel* QtBooleanOpOperationAction::getBooleanPanel ( )
{
    return dynamic_cast<QtBooleanOpOperationPanel*>(getOperationPanel ( ));
}   // QtBooleanOpOperationAction::getFusePanel


void QtBooleanOpOperationAction::executeOperation ( )
{
    // Validation paramétrage :
    M3DCommandResultIfc*	cmdResult	= 0;
//  QtMgx3DGeomOperationAction::executeOperation ( );

    // Récupération des paramètres de création du cylindre :
    QtBooleanOpOperationPanel*   panel   = getBooleanPanel ( );
    CHECK_NULL_PTR_ERROR (panel)
    //string  name    = panel->getGroupName ( );
    // TODO [FL] prendre en compte le name pour les nouvelles entités
    vector<string> entities = panel->getEntityNames();
    QtBooleanOpOperationPanel::OPERATION_TYPE type = panel->getOperationType();
    if(type==QtBooleanOpOperationPanel::FUSE)
        cmdResult	= getContext ( ).getGeomManager ( ).fuse(entities);
    else if(type==QtBooleanOpOperationPanel::COMMON)
        cmdResult	= getContext ( ).getGeomManager ( ).common(entities);
    else if(type==QtBooleanOpOperationPanel::CUT)
    {
        vector<string> cuttingEntities = panel->getCuttingNames();
        cmdResult	= getContext ( ).getGeomManager ( ).cut(entities, cuttingEntities);
    }
    else if(type==QtBooleanOpOperationPanel::SECTION)
    {
        string  cutting = panel->getCuttingNames()[0];
        cmdResult	= getContext ( ).getGeomManager ( ).section(entities, cutting);
    }
    else if(type==QtBooleanOpOperationPanel::GLUE)
        cmdResult	= getContext ( ).getGeomManager ( ).glue(entities);

	setCommandResult (cmdResult);
}   // QtBooleanOpOperationAction::executeOperation

void QtBooleanOpOperationPanel::newOperationType (int index)
{
	CHECK_NULL_PTR_ERROR (_entitiesPanel)
	CHECK_NULL_PTR_ERROR (_toolsToCutPanel)
	CHECK_NULL_PTR_ERROR (_entitiesPanel->getNamesTextField ( ))
	CHECK_NULL_PTR_ERROR (_toolsToCutPanel->getNamesTextField ( ))
    if(index==CUT){
        _entitiesPanel->setLabel("Volumes à rogner");
        _entitiesPanel->setMultiSelectMode(true);
        _entitiesPanel->show();

        // on rogne avec d'autres volumes
        _toolsToCutPanel->setDimensions(SelectionManagerIfc::D3);
        _toolsToCutPanel->setFilteredTypes (FilterEntity::GeomVolume);
        _toolsToCutPanel->setLabel("Volumes à supprimer");
        _toolsToCutPanel->setMultiSelectMode(true);
        _toolsToCutPanel->show();
		_entitiesPanel->getNamesTextField ( )->setLinkedSeizureManagers (
									0, _toolsToCutPanel->getNamesTextField ( ));
		_toolsToCutPanel->getNamesTextField ( )->setLinkedSeizureManagers (
									_entitiesPanel->getNamesTextField ( ), 0);
    }
    else if (index==SECTION){
        _entitiesPanel->setLabel("Volumes à couper");
        _entitiesPanel->setMultiSelectMode(true);
        _entitiesPanel->show();

        // on coupe avec une surface
        _toolsToCutPanel->setDimensions(SelectionManagerIfc::D2);
        _toolsToCutPanel->setFilteredTypes (FilterEntity::GeomSurface);
        _toolsToCutPanel->setLabel("Surface pour couper");
        _toolsToCutPanel->setMultiSelectMode(false);
        _toolsToCutPanel->show();
		_entitiesPanel->getNamesTextField ( )->setLinkedSeizureManagers (
									0, _toolsToCutPanel->getNamesTextField ( ));
		_toolsToCutPanel->getNamesTextField ( )->setLinkedSeizureManagers (
									_entitiesPanel->getNamesTextField ( ), 0);
    }
    else{
        _entitiesPanel->setLabel("Volumes");
        _entitiesPanel->setMultiSelectMode(true);
        _entitiesPanel->show();

        _toolsToCutPanel->hide();
		_entitiesPanel->getNamesTextField ( )->setLinkedSeizureManagers (0, 0);
    }
}   // QtBooleanOpOperationPanel::QtBooleanOpOperationPanel (const QtBooleanOpOperationPanel&)


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}   // namespace QtComponents

}   // namespace Mgx3D
