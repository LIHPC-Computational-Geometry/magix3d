/**
 * \file		QtBlocksByExtrusionCreationAction.cpp
 * \author		Simon CALDERAN
 * \date		23/02/2022
 */

#include "Internal/Context.h"
#include "Internal/Resources.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/ValidatedField.h"
#include "QtComponents/QtBlocksByExtrusionCreationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtNumericFieldsFactory.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QVBoxLayout>

#include <float.h>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Group;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;

namespace Mgx3D {

    namespace QtComponents {

        // ===========================================================================
        //                        LA CLASSE QtBlocksByExtrusionCreationPanel
        // ===========================================================================

        QtBlocksByExtrusionCreationPanel::QtBlocksByExtrusionCreationPanel (
                QWidget* parent, const string& panelName,
                QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
                : QtMgx3DOperationPanel (parent, mainWindow, action,
                                         QtMgx3DApplication::HelpSystem::instance ( ).blocksByRevolutionCreationURL,
                                         QtMgx3DApplication::HelpSystem::instance ( ).blocksByRevolutionCreationTag),
                _facesPanel(0), _directionPanel(0) {
            QVBoxLayout *layout = new QVBoxLayout(this);
            setLayout(layout);
            layout->setContentsMargins(
                    Resources::instance()._margin.getValue(),
                    Resources::instance()._margin.getValue(),
                    Resources::instance()._margin.getValue(),
                    Resources::instance()._margin.getValue());
            layout->setSpacing(Resources::instance()._spacing.getValue());

            // Nom opération :
            QLabel *label = new QLabel(panelName.c_str(), this);
            QFont font(label->font());
            font.setBold(true);
            label->setFont(font);
            layout->addWidget(label);

            // Définition du bloc :
            QtGroupBox *groupBox =
                    new QtGroupBox(this, "Paramètres pour l'extrusion");
            layout->addWidget(groupBox);
            QVBoxLayout *vlayout = new QVBoxLayout(groupBox);
            vlayout->setContentsMargins(
                    Resources::instance()._margin.getValue(),
                    Resources::instance()._margin.getValue(),
                    Resources::instance()._margin.getValue(),
                    Resources::instance()._margin.getValue());
            vlayout->setSpacing(Resources::instance()._spacing.getValue());
            groupBox->setLayout(vlayout);
            const string suggestedID;

            //Panneau de sélection des faces à extruder
            _facesPanel = new QtMgx3DEntityPanel(
                    groupBox, "", true, "Sélection des faces à extruder",
                    "", &mainWindow, SelectionManager::D2,
                    FilterEntity::TopoCoFace);
            _facesPanel->setMultiSelectMode(true);
            connect(_facesPanel, SIGNAL (entitiesAddedToSelection(QString)),
                    this, SLOT (entitiesAddedToSelectionCallback(QString)));
            connect(_facesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
                    this, SLOT (entitiesRemovedFromSelectionCallback(QString)));
            layout->addWidget(_facesPanel);

            _directionPanel =
                    new QtMgx3DVectorPanel(this, "", true,
                                           "dx :", "dy :", "dz :",
                                           0., -DBL_MAX, DBL_MAX, 0., -DBL_MAX, DBL_MAX,
                                           1., -DBL_MAX, DBL_MAX,
                                           &mainWindow, (FilterEntity::objectType) (
                                    FilterEntity::AllPoints | FilterEntity::AllEdges),
                                           true);
            connect(_directionPanel, SIGNAL (vectorModified()), this,
                    SLOT (parametersModifiedCallback()));
            layout->addWidget(_directionPanel);

            layout->addStretch(2);
        }// QtBlocksByExtrusionCreationPanel::QtBlocksByExtrusionCreationPanel
        //===================================================================================================

        /*QtBlocksByExtrusionCreationPanel::QtBlocksByExtrusionCreationPanel (
                const QtBlocksByExtrusionCreationPanel& cao)
                : QtMgx3DOperationPanel (0, *new QtMgx3DMainWindow(0), 0, "", ""),
                  _facesPanel (0), _directionPanel(0)
        {
            MGX_FORBIDDEN ("QtBlocksByRevolutionCreationPanel copy constructor is not allowed.");
        }*/	// QtBlocksByRevolutionCreationPanel::QtBlocksByRevolutionCreationPanel (const QtBlocksByRevolutionCreationPanel&)
        //===================================================================================================

        QtBlocksByExtrusionCreationPanel&
        QtBlocksByExtrusionCreationPanel::operator = (
                const QtBlocksByExtrusionCreationPanel&)
        {
            MGX_FORBIDDEN ("QtBlocksByExtrusionCreationPanel assignment operator is not allowed.");
            return *this;
        }	// QtBlocksByExtrusionCreationPanel::operator = (const QtBlocksByExtrusionCreationPanel&)
        //===================================================================================================

        QtBlocksByExtrusionCreationPanel::~QtBlocksByExtrusionCreationPanel()
        {
        }//QtBlocksByExtrusionCreationPanel::~QtBlocksByExtrusionCreationPanel
        //===================================================================================================

        void QtBlocksByExtrusionCreationPanel::reset ( )
        {
            BEGIN_QT_TRY_CATCH_BLOCK

                CHECK_NULL_PTR_ERROR (_facesPanel)
                CHECK_NULL_PTR_ERROR (_directionPanel)
                _facesPanel->reset ( );
                _directionPanel->reset();

            COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

            QtMgx3DOperationPanel::reset ( );
        }	// QtBlocksByExtrusionCreationPanel::reset
        // ===================================================================================================

        vector<Entity*> QtBlocksByExtrusionCreationPanel::getInvolvedEntities ( )
        {
            vector<Entity*>	entities;

            const vector<string>	facesNames	= getFaces ( );
            for (vector<string>::const_iterator it = facesNames.begin ( );
                 facesNames.end ( ) != it; it++)
            {
                TopoEntity*		edge		=
                        getContext ( ).getTopoManager ( ).getCoEdge (*it, false);

                if (0 != edge)
                    entities.push_back (edge);
            }	// for (vector<string>::const_iterator it = edgesNames.begin ( ); ...

            return entities;
        }	// QtBlocksByExtrusionCreationPanel::getInvolvedEntities
        // ===================================================================================================

        vector<string> QtBlocksByExtrusionCreationPanel::getFaces ( ) const
        {
            CHECK_NULL_PTR_ERROR (_facesPanel)
            return _facesPanel->getUniqueNames ( );
        }	// QtBlocksByExtrusionCreationPanel::getEdges
        // ===================================================================================================

        Utils::Math::Vector QtBlocksByExtrusionCreationPanel::getDirection( )  const
        {
            CHECK_NULL_PTR_ERROR (_directionPanel)
            return Vector (_directionPanel->getDx ( ),
                           _directionPanel->getDy ( ),
                           _directionPanel->getDz ( ));
        }	// QtBlocksByExtrusionCreationPanel::getEdges
        // ===================================================================================================

        void QtBlocksByExtrusionCreationPanel::validate ( )
        {
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
                //A documenter justement ====>
                error << "QtBlocksByExtrusionCreationPanel::validate : erreur non documentée.";
            }

            if (0 != error.length ( ))
                throw Exception (error);
        }	// QtBlocksByExtrusionCreationPanel::validate
        // ===================================================================================================

        void QtBlocksByExtrusionCreationPanel::cancel ( )
        {
            QtMgx3DOperationPanel::cancel ( );

            CHECK_NULL_PTR_ERROR (_facesPanel)
            _facesPanel->stopSelection ( );

            if (true == cancelClearEntities ( ))
            {
                BEGIN_QT_TRY_CATCH_BLOCK

                    _facesPanel->setUniqueName ("");

                COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
            }	// if (true == cancelClearEntities ( ))
        }	// QtBlocksByExtrusionCreationPanel::cancel
        // ===================================================================================================

        void QtBlocksByExtrusionCreationPanel::autoUpdate ( )
        {
            CHECK_NULL_PTR_ERROR (_facesPanel)
            CHECK_NULL_PTR_ERROR (_directionPanel)

            #ifdef AUTO_UPDATE_OLD_SCHEME
            if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		vector<string>	selectedEdges	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoEdge);
		if (0 != selectedEdges.size ( ))
			_edgePanel->setUniqueNames (selectedEdges);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
            #else	    // AUTO_UPDATE_OLD_SCHEME
            _facesPanel->clearSelection ( );
            _directionPanel->setSegment ("");
            #endif	// AUTO_UPDATE_OLD_SCHEME

            _facesPanel->actualizeGui (true);
            _directionPanel->actualizeGui(true);

            QtMgx3DOperationPanel::autoUpdate ( );
        }	// QtBlocksByExtrusionCreationPanel::autoUpdate
        // ===================================================================================================

        void QtBlocksByExtrusionCreationPanel::operationCompleted ( )
        {
            highlight (false);	// car champs de texte modifiés

            BEGIN_QT_TRY_CATCH_BLOCK

                CHECK_NULL_PTR_ERROR (_facesPanel)
                _facesPanel->stopSelection ( );
                _facesPanel->setUniqueName ("");

            COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

            QtMgx3DOperationPanel::operationCompleted ( );
        }	// QtBlocksByExtrusionCreationPanel::operationCompleted
        // ===================================================================================================


        // ===========================================================================
        //                  LA CLASSE QtBlocksByExtrusionCreationAction
        // ===========================================================================

        QtBlocksByExtrusionCreationAction::QtBlocksByExtrusionCreationAction (
                const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
                const QString& tooltip)
                : QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
        {
            QtBlocksByExtrusionCreationPanel*	operationPanel	=
                    new QtBlocksByExtrusionCreationPanel (
                            &getOperationPanelParent ( ), text.toStdString ( ),
                            mainWindow, this);
            setOperationPanel (operationPanel);
        }	// QtBlocksByExtrusionCreationAction::QtBlocksByExtrusionCreationAction
        // ===================================================================================================

        /*QtBlocksByExtrusionCreationAction::QtBlocksByExtrusionCreationAction (
                const QtBlocksByExtrusionCreationAction&)
                : QtMgx3DTopoOperationAction (
                QIcon (""), "", *new QtMgx3DMainWindow (0), "")
        {
            MGX_FORBIDDEN ("QtBlocksByExtrusionCreationAction copy constructor is not allowed.")
        }*/	// QtBlocksByExtrusionCreationAction::QtBlocksByExtrusionCreationAction
        // ===================================================================================================

        QtBlocksByExtrusionCreationAction& QtBlocksByExtrusionCreationAction::operator = (
                const QtBlocksByExtrusionCreationAction&)
        {
            MGX_FORBIDDEN ("QtBlocksByExtrusionCreationAction assignment operator is not allowed.")
            return *this;
        }	// QtBlocksByExtrusionCreationAction::operator =
        // ===================================================================================================

        QtBlocksByExtrusionCreationAction::~QtBlocksByExtrusionCreationAction ( )
        {
        }	// QtBlocksByExtrusionCreationAction::~QtBlocksByExtrusionCreationAction
        // ===================================================================================================

        QtBlocksByExtrusionCreationPanel*
        QtBlocksByExtrusionCreationAction::getBlocksByExtrusionPanel ( )
        {
            return dynamic_cast<QtBlocksByExtrusionCreationPanel*>(getOperationPanel ( ));
        }	// QtBlocksByExtrusionCreationAction::getBlocksByExtrusionPanel
        // ===================================================================================================

        void QtBlocksByExtrusionCreationAction::executeOperation ( )
        {
            // Validation paramétrage :
            M3DCommandResult*	cmdResult	= 0;
            QtMgx3DTopoOperationAction::executeOperation ( );

            // Récupération des paramètres de création du cylindre :
            QtBlocksByExtrusionCreationPanel*	panel	= getBlocksByExtrusionPanel( );
            CHECK_NULL_PTR_ERROR (panel)
            Vector direction = panel->getDirection();
            vector<string> cofaceNames	= panel->getFaces( );
            cmdResult	= getContext ( ).getTopoManager ( ).makeBlocksByExtrude(cofaceNames, direction);

            setCommandResult (cmdResult);
        }	// QtBlocksByRevolutionCreationAction::executeOperation

    }

}












