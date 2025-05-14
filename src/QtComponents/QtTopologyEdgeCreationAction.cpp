/**
 * \file		QtTopologyEdgeCreationAction.cpp
 * \author		Eric B
 * \date		15/3/2019
 */

#include "Internal/Context.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyEdgeCreationAction.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>


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
//                        LA CLASSE QtTopologyEdgeCreationPanel
// ===========================================================================

QtTopologyEdgeCreationPanel::QtTopologyEdgeCreationPanel (
			QWidget* parent, const string& panelName,
            QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
: QtMgx3DOperationPanel (parent, mainWindow, action,
		QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).edgeCreationURL,
		QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).edgeCreationTag),
		_curvePanel (0), _verticesPanel(0), _namePanel(0), _curveCheckBox(0)
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

    // Nom groupe :
    _namePanel	= new QtMgx3DGroupNamePanel (this, "Groupe", mainWindow, 3, creationPolicy, "");
    layout->addWidget (_namePanel);
    addValidatedField (*_namePanel);

    _verticesPanel = new QtMgx3DEntityPanel(this, "", true, "Sommets :", "",
                                            &mainWindow, SelectionManagerIfc::D0,
                                            FilterEntity::TopoVertex);
    _verticesPanel->setMultiSelectMode(true);
    layout->addWidget (_verticesPanel);
    connect (_verticesPanel, SIGNAL (entitiesAddedToSelection(QString)),
             this, SLOT (entitiesAddedToSelectionCallback (QString)));
    connect (_verticesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
             this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

    _curveCheckBox	=
            new QCheckBox (QString::fromUtf8("Création par courbe géométrique"), this);
    _curveCheckBox->setChecked (false);
    layout->addWidget (_curveCheckBox);
    connect (_curveCheckBox, SIGNAL (stateChanged (int)), this,
             SLOT (methodCallBack ( )));

	// Courbe à associer :
	_curvePanel	= new QtMgx3DEntityPanel (
						this, "", true, "Courbe associée :", "",
						&mainWindow, SelectionManagerIfc::D1,
						FilterEntity::GeomCurve);
	layout->addWidget (_curvePanel);
	connect (_curvePanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_curvePanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
    _curvePanel->setEnabled(false);

    addPreviewCheckBox (true);

	layout->addStretch (2);

}	// QtTopologyEdgeCreationPanel::QtTopologyEdgeCreationPanel


QtTopologyEdgeCreationPanel::QtTopologyEdgeCreationPanel (
										const QtTopologyEdgeCreationPanel& cao)
: QtMgx3DOperationPanel (0, *new QtMgx3DMainWindow(0), 0, "", ""),
		_curvePanel (0)
{
	MGX_FORBIDDEN ("QtTopologyEdgeCreationPanel copy constructor is not allowed.");
}	// QtTopologyEdgeCreationPanel::QtTopologyEdgeCreationPanel (const QtTopologyEdgeCreationPanel&)


QtTopologyEdgeCreationPanel& QtTopologyEdgeCreationPanel::operator = (
											const QtTopologyEdgeCreationPanel&)
{
	MGX_FORBIDDEN ("QtTopologyEdgeCreationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyEdgeCreationPanel::QtTopologyEdgeCreationPanel (const QtTopologyEdgeCreationPanel&)


QtTopologyEdgeCreationPanel::~QtTopologyEdgeCreationPanel ( )
{
}	// QtTopologyEdgeCreationPanel::~QtTopologyEdgeCreationPanel


string QtTopologyEdgeCreationPanel::getCurveName ( ) const
{
	CHECK_NULL_PTR_ERROR (_curvePanel)
	return _curvePanel->getUniqueName ( );
}	// QtTopologyEdgeCreationPanel::getCurveName

std::vector<string> QtTopologyEdgeCreationPanel::getVerticesNames() const
{
    CHECK_NULL_PTR_ERROR (_verticesPanel)
    return _verticesPanel->getUniqueNames();
}

void QtTopologyEdgeCreationPanel::reset ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_curvePanel)
		_curvePanel->reset ( );
	}
	catch (...)
	{
	}
}	// QtTopologyEdgeCreationPanel::reset

void QtTopologyEdgeCreationPanel::validate ( )
{
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
		error << "QtCurveByProjectionOnSurfacePanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopologyEdgeCreationPanel::validate

void QtTopologyEdgeCreationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_curvePanel)
	_curvePanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_curvePanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyEdgeCreationPanel::cancel


void QtTopologyEdgeCreationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_curvePanel)

	_curvePanel->setUniqueName ("");

	_curvePanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyEdgeCreationPanel::autoUpdate

bool QtTopologyEdgeCreationPanel::curveMethod ( ) const
{
    CHECK_NULL_PTR_ERROR (_curveCheckBox)
    return Qt::Checked == _curveCheckBox->checkState ( ) ? true : false;
}	// QtTopologySplitFacesPanel::allFaces

string QtTopologyEdgeCreationPanel::getGroupName() const
{
    CHECK_NULL_PTR_ERROR (_namePanel)
    return _namePanel->getGroupName();
}

void QtTopologyEdgeCreationPanel::preview (bool show, bool destroyInteractor){
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

    try {
        Context *context = dynamic_cast<Context *>(&getContext());
        CHECK_NULL_PTR_ERROR (context)

        DisplayProperties graphicalProps;
        graphicalProps.setWireColor(Color(
                255 * Resources::instance()._previewColor.getRed(),
                255 * Resources::instance()._previewColor.getGreen(),
                255 * Resources::instance()._previewColor.getBlue()));
        graphicalProps.setLineWidth(Resources::instance()._previewWidth.getValue());

        if(_verticesPanel->getUniqueNames().size() == 2){

            Math::Point point0 = context->getTopoManager().getCoord(_verticesPanel->getUniqueNames()[0]);
            Math::Point point1 = context->getTopoManager().getCoord(_verticesPanel->getUniqueNames()[1]);
            vector<size_t>		indices;

            RenderingManager::RepresentationID	repID	=
                    getRenderingManager ( ).createSegment(point0, point1, graphicalProps, true);
            registerPreviewedObject (repID);

            getRenderingManager ( ).forceRender ( );
        }
    }catch (...)
    {
        return;
    }
}

void QtTopologyEdgeCreationPanel::methodCallBack ( )
{
    BEGIN_QT_TRY_CATCH_BLOCK

    CHECK_NULL_PTR_ERROR (_curveCheckBox)
    CHECK_NULL_PTR_ERROR (_verticesPanel)
    CHECK_NULL_PTR_ERROR (_curvePanel)

    _verticesPanel->setEnabled (Qt::Checked == _curveCheckBox->checkState ( ) ? false : true);
    _curvePanel->setEnabled (Qt::Checked == _curveCheckBox->checkState ( ) ? true : false);

    parametersModifiedCallback();

    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtTopologySplitFacesPanel::facesModifiedCallback



// ===========================================================================
//                  LA CLASSE QtTopologyEdgeCreationAction
// ===========================================================================

QtTopologyEdgeCreationAction::QtTopologyEdgeCreationAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtTopologyCreationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyEdgeCreationPanel*	operationPanel	=
			new QtTopologyEdgeCreationPanel (
					&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy,
					mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyEdgeCreationAction::QtTopologyEdgeCreationAction


QtTopologyEdgeCreationAction::QtTopologyEdgeCreationAction (
										const QtTopologyEdgeCreationAction&)
	: QtTopologyCreationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyEdgeCreationAction copy constructor is not allowed.")
}	// QtTopologyEdgeCreationAction::QtTopologyEdgeCreationAction


QtTopologyEdgeCreationAction& QtTopologyEdgeCreationAction::operator = (
										const QtTopologyEdgeCreationAction&)
{
	MGX_FORBIDDEN ("QtTopologyEdgeCreationAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyEdgeCreationAction::operator =


QtTopologyEdgeCreationAction::~QtTopologyEdgeCreationAction ( )
{
}	// QtTopologyEdgeCreationAction::~QtTopologyEdgeCreationAction


void QtTopologyEdgeCreationAction::executeOperation ( )
{
	QtTopologyEdgeCreationPanel*	panel	= dynamic_cast<QtTopologyEdgeCreationPanel*>(getOperationPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResult*	cmdResult	= 0;
	QtTopologyCreationAction::executeOperation ( );


    if (panel->curveMethod()){
        // Récupération des paramètres d'association des entités topologiques :
        const string	curve	= panel->getCurveName ( );
        cmdResult	= getContext ( ).getTopoManager( ).newTopoOnGeometry (curve);
    }
    else{
        std::vector<string> vnames = panel->getVerticesNames();
        const string group = panel->getGroupName();
        cmdResult	= getContext ( ).getTopoManager( ).newTopoEntity (vnames,1,group);
    }

	setCommandResult (cmdResult);
}	// QtTopologyFaceCreationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
