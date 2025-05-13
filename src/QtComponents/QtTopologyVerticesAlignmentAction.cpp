/**
 * \file        QtTopologyVerticesAlignmentAction.cpp
 * \author      Charles PIGNEROL
 * \date        16/12/2014
 */

#include "Internal/Context.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Geom/Vertex.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyVerticesAlignmentAction.h"

#include "QtComponents/QtEntityByDimensionSelectorPanel.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>

#include <values.h>	// DBL_MAX

using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;


namespace Mgx3D
{

namespace QtComponents {
// ===========================================================================
//                        LA CLASSE QtBetween2PntsPanel
// ===========================================================================

QtBetween2PntsPanel::QtBetween2PntsPanel(QWidget *parent, QtComponents::QtMgx3DMainWindow &window,
                                         QtMgx3DOperationPanel *mainPanel)
: QtMgx3DOperationsSubPanel(parent, window, mainPanel), _point1TextField(0), _point2TextField(0), _verticesPanel(0)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    QLabel *label = new QLabel();
    QFont font(label->font());
    font.setBold(true);
    label->setFont(font);
    layout->addWidget(label);

    QGridLayout *hlayout = new QGridLayout(0);
    hlayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(hlayout);

    const FilterEntity::objectType pointTypes = (FilterEntity::objectType)
            (FilterEntity::GeomVertex | FilterEntity::TopoVertex);
    label = new QLabel("Point 1 :", this);
    hlayout->addWidget(label, 0, 0);
    _point1TextField = new QtEntityIDTextField(
            this, &window, SelectionManagerIfc::D0, pointTypes);
    connect(_point1TextField, SIGNAL (entitiesAddedToSelection(QString)),
            this, SLOT (entitiesAddedToSelectionCallback(QString)));
    connect(_point1TextField, SIGNAL (entitiesRemovedFromSelection(QString)),
            this, SLOT (entitiesRemovedFromSelectionCallback(QString)));
    label->setToolTip(QString::fromUtf8("Premier point définissant la droite de projection des sommets."));
    _point1TextField->setToolTip(
            QString::fromUtf8("Premier point définissant la droite de projection des sommets."));
    hlayout->addWidget(_point1TextField, 0, 1);
    label = new QLabel("Point 2 :", this);
    hlayout->addWidget(label, 1, 0);
    _point2TextField = new QtEntityIDTextField(
            this, &window, SelectionManagerIfc::D0, pointTypes);
    connect(_point2TextField, SIGNAL (entitiesAddedToSelection(QString)),
            this, SLOT (entitiesAddedToSelectionCallback(QString)));
    connect(_point2TextField, SIGNAL (entitiesRemovedFromSelection(QString)),
            this, SLOT (entitiesRemovedFromSelectionCallback(QString)));
    label->setToolTip(QString::fromUtf8("Second point définissant la droite de projection des sommets."));
    _point2TextField->setToolTip(
            QString::fromUtf8("Second point définissant la droite de projection des sommets."));
    hlayout->addWidget(_point2TextField, 1, 1);
    // Les sommets à projeter :
    _verticesPanel = new QtMgx3DEntityPanel(
            this, "", true, "Sommets :", "", &window,
            SelectionManagerIfc::D0, FilterEntity::TopoVertex);
    _verticesPanel->setMultiSelectMode(true);
    connect(_verticesPanel, SIGNAL (entitiesAddedToSelection(QString)),
            this, SLOT (entitiesAddedToSelectionCallback(QString)));
    connect(_verticesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
            this, SLOT (entitiesRemovedFromSelectionCallback(QString)));
    layout->addWidget(_verticesPanel);

    layout->addStretch(2);

    CHECK_NULL_PTR_ERROR (_verticesPanel->getNameTextField())
    _point1TextField->setLinkedSeizureManagers(0, _point2TextField);
    _point2TextField->setLinkedSeizureManagers(
            _point1TextField, _verticesPanel->getNameTextField());
    _verticesPanel->getNameTextField()->setLinkedSeizureManagers(
            _point2TextField, 0);
}

QtBetween2PntsPanel::QtBetween2PntsPanel(const QtBetween2PntsPanel &p)
: QtMgx3DOperationsSubPanel(p), _point1TextField(0), _point2TextField(0), _verticesPanel(0)
{
    MGX_FORBIDDEN ("QtBetween2PntsPanel copy constructor is not allowed.");
}

QtBetween2PntsPanel &QtBetween2PntsPanel::operator=(const QtBetween2PntsPanel &)
{
    MGX_FORBIDDEN ("QtCenterExtremitiesArcPanel assignment operator is not allowed.");
    return *this;
}    // QtCenterExtremitiesArcPanel::operator =

QtBetween2PntsPanel::~QtBetween2PntsPanel ( )
{
}    // QtCenterExtremitiesArcPanel::~QtCenterExtremitiesArcPanel

Math::Point QtBetween2PntsPanel::getPoint1 ( ) const
        {
            CHECK_NULL_PTR_ERROR (getMainWindow ( ))
            CHECK_NULL_PTR_ERROR (_point1TextField)
            Math::Point	point;

            if (1 == _point1TextField->getObjectsCount ( ))
            {
                const vector<string>	names	= _point1TextField->getUniqueNames ( );
                const string			name	= names [0];


                switch (_point1TextField->getObjectType ( ))
                {
                    case Entity::GeomVertex	:
                    {

                        const Geom::Vertex*	vertex	=
                                getMainWindow ( )->getContext ( ).getGeomManager (
                                ).getVertex (name, true);
                        CHECK_NULL_PTR_ERROR (vertex)
                        point	= vertex->getPoint ( );
                    }	// case Entity::GeomVertex
                        break;
                    case Entity::TopoVertex	:
                    {
                        const Topo::Vertex*	vertex	=
                                getMainWindow ( )->getContext ( ).getTopoManager (
                                ).getVertex (name, true);
                        CHECK_NULL_PTR_ERROR (vertex)
                        point	= vertex->getCoord ( );
                    }	// case Entity::TopoVertex
                        break;
                    default	:
                    {
                        UTF8String	message (Charset::UTF_8);
                        message << "Entité de type "
                                << Entity::objectTypeToObjectTypeName (_point1TextField->getObjectType ( ))
                                << " (" << (unsigned long)_point1TextField->getObjectType ( )
                                << ") non supporté pour la saisie d'un point.";
                        throw Exception (message);
                    }	// default
                }	// switch (_point1TextField->getObjectType ( ))
            }	// if (1 == _point1TextField->getObjectsCount ( ))
            else
                throw Exception (UTF8String ("Absence de premier point de la droite saisi.", Charset::UTF_8));

            return point;
        }	// QtTopologyVerticesAlignmentPanel::getPoint1


Math::Point QtBetween2PntsPanel::getPoint2 ( ) const
        {
            CHECK_NULL_PTR_ERROR (getMainWindow ( ))
            CHECK_NULL_PTR_ERROR (_point2TextField)
            Math::Point	point;

            if (1 == _point2TextField->getObjectsCount ( ))
            {
                const vector<string>	names	= _point2TextField->getUniqueNames ( );
                const string			name	= names [0];
                switch (_point2TextField->getObjectType ( ))
                {
                    case Entity::GeomVertex	:
                    {

                        const Geom::Vertex*	vertex	=
                                getMainWindow ( )->getContext ( ).getGeomManager (
                                ).getVertex (name, true);
                        CHECK_NULL_PTR_ERROR (vertex)
                        point	= vertex->getPoint ( );
                    }	// case Entity::GeomVertex
                        break;
                    case Entity::TopoVertex	:
                    {
                        const Topo::Vertex*	vertex	=
                                getMainWindow ( )->getContext ( ).getTopoManager (
                                ).getVertex (name, true);
                        CHECK_NULL_PTR_ERROR (vertex)
                        point	= vertex->getCoord ( );
                    }	// case Entity::TopoVertex
                        break;
                    default	:
                    {
                        UTF8String	message (Charset::UTF_8);
                        message << "Entité de type "
                                << Entity::objectTypeToObjectTypeName (_point2TextField->getObjectType ( ))
                                << " (" << (unsigned long)_point2TextField->getObjectType ( )
                                << ") non supporté pour la saisie d'un point.";
                        throw Exception (message);
                    }	// default
                }	// switch (_point2TextField->getObjectType ( ))
            }	// if (1 == _point2TextField->getObjectsCount ( ))
            else
                throw Exception (UTF8String ("Absence de second point de la droite saisi.", Charset::UTF_8));

            return point;
        }	// QtTopologyVerticesAlignmentPanel::getPoint2


vector<string> QtBetween2PntsPanel::getVerticesNames ( ) const
        {
            CHECK_NULL_PTR_ERROR (_verticesPanel)
            return _verticesPanel->getUniqueNames ( );
        }	// QtTopologyVerticesAlignmentPanel::getVerticesNames

        void QtBetween2PntsPanel::parametersModifiedCallback()
        {
            emit parametersModified();
        }

void QtBetween2PntsPanel::reset ( )
{
    BEGIN_QT_TRY_CATCH_BLOCK

    CHECK_NULL_PTR_ERROR (_verticesPanel)
    CHECK_NULL_PTR_ERROR (_point1TextField)
    CHECK_NULL_PTR_ERROR (_point2TextField)
    _point1TextField->reset ( );
    _point2TextField->reset ( );
    _verticesPanel->reset ( );

    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

    QtMgx3DOperationPanel::reset ( );
}	// QtTopologyVerticesAlignmentPanel::reset


void QtBetween2PntsPanel::cancel ( )
        {
            QtMgx3DOperationPanel::cancel ( );

            CHECK_NULL_PTR_ERROR (_point1TextField)
            CHECK_NULL_PTR_ERROR (_point2TextField)
            CHECK_NULL_PTR_ERROR (_verticesPanel)

            _point1TextField->setInteractiveMode (false);
            _point2TextField->setInteractiveMode (false);
            _verticesPanel->stopSelection ( );
            if (true == cancelClearEntities ( ))
            {
                BEGIN_QT_TRY_CATCH_BLOCK

                    const vector<string>	names;
                    _point1TextField->setUniqueNames (names);
                    _point2TextField->setUniqueNames (names);
                    _verticesPanel->setUniqueName ("");

                COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
            }	// if (true == cancelClearEntities ( ))
        }	// QtTopologyVerticesAlignmentPanel::cancel


void QtBetween2PntsPanel::autoUpdate ( )
{
    CHECK_NULL_PTR_ERROR (_verticesPanel)
    CHECK_NULL_PTR_ERROR (_point1TextField)
    CHECK_NULL_PTR_ERROR (_point2TextField)

#ifdef AUTO_UPDATE_OLD_SCHEME
            if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedVertices	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoVertex);
		if (0 != selectedVertices.size ( ))
			_verticesPanel->setUniqueNames (selectedVertices);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
    _point1TextField->clearSelection ( );
    _point2TextField->clearSelection ( );
    _verticesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

    _verticesPanel->actualizeGui (true);

    QtMgx3DOperationPanel::autoUpdate ( );
}


// ===========================================================================
//                        LA CLASSE QtBetween2PntsPanel
// ===========================================================================

        QtSurfaceConstraintPanel::QtSurfaceConstraintPanel(QWidget *parent, QtComponents::QtMgx3DMainWindow& window,
                                                 QtMgx3DOperationPanel *mainPanel)
                : QtMgx3DOperationsSubPanel(parent, window, mainPanel),_point1TextField(0), _point2TextField(0), _geomEntityPanel(0), _verticesPanel(0)
        {
            QVBoxLayout *layout = new QVBoxLayout(this);
            setLayout(layout);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(5);

            QLabel *label = new QLabel();
            QFont font(label->font());
            font.setBold(true);
            label->setFont(font);
            layout->addWidget(label);

            QGridLayout *hlayout = new QGridLayout(0);
            hlayout->setContentsMargins(0, 0, 0, 0);
            layout->addLayout(hlayout);

            const FilterEntity::objectType pointTypes = (FilterEntity::objectType)
                    (FilterEntity::GeomVertex | FilterEntity::TopoVertex);
            label = new QLabel("Point 1 :", this);
            hlayout->addWidget(label, 0, 0);
            _point1TextField = new QtEntityIDTextField(
                    this, &window, SelectionManagerIfc::D0, pointTypes);
            connect(_point1TextField, SIGNAL (entitiesAddedToSelection(QString)),
                    this, SLOT (entitiesAddedToSelectionCallback(QString)));
            connect(_point1TextField, SIGNAL (entitiesRemovedFromSelection(QString)),
                    this, SLOT (entitiesRemovedFromSelectionCallback(QString)));
            label->setToolTip(QString::fromUtf8("Premier point définissant la droite de projection des sommets."));
            _point1TextField->setToolTip(
                    QString::fromUtf8("Premier point définissant la droite de projection des sommets."));
            hlayout->addWidget(_point1TextField, 0, 1);
            label = new QLabel("Point 2 :", this);
            hlayout->addWidget(label, 1, 0);
            _point2TextField = new QtEntityIDTextField(
                    this, &window, SelectionManagerIfc::D0, pointTypes);
            connect(_point2TextField, SIGNAL (entitiesAddedToSelection(QString)),
                    this, SLOT (entitiesAddedToSelectionCallback(QString)));
            connect(_point2TextField, SIGNAL (entitiesRemovedFromSelection(QString)),
                    this, SLOT (entitiesRemovedFromSelectionCallback(QString)));
            label->setToolTip(QString::fromUtf8("Second point définissant la droite de projection des sommets."));
            _point2TextField->setToolTip(
                    QString::fromUtf8("Second point définissant la droite de projection des sommets."));
            hlayout->addWidget(_point2TextField, 1, 1);
            const FilterEntity::objectType surfaceTypes = (FilterEntity::objectType)
                    (FilterEntity::GeomSurface);
            string entityNameType = "Surface :";
            _geomEntityPanel = new QtMgx3DEntityPanel(
                    this, "", true, entityNameType, "",
                    &window, SelectionManagerIfc::D2, surfaceTypes);
            connect(_geomEntityPanel, SIGNAL (entitiesAddedToSelection(QString)),
                    this, SLOT (entitiesAddedToSelectionCallback(QString)));
            connect(_geomEntityPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
                    this, SLOT (entitiesRemovedFromSelectionCallback(QString)));
            //label->setToolTip(QString::fromUtf8("Surface à intersecter sur laquelle sera projeté le sommet."));
            _geomEntityPanel->setToolTip(
                    QString::fromUtf8("Surface à intersecter sur laquelle sera projeté le sommet."));
            layout->addWidget(_geomEntityPanel);
            // Les sommets à projeter :
            _verticesPanel = new QtMgx3DEntityPanel(
                    this, "", true, "Sommet :", "", &window,
                    SelectionManagerIfc::D0, FilterEntity::TopoVertex);
            _verticesPanel->setMultiSelectMode(false);
            connect(_verticesPanel, SIGNAL (entitiesAddedToSelection(QString)),
                    this, SLOT (entitiesAddedToSelectionCallback(QString)));
            connect(_verticesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
                    this, SLOT (entitiesRemovedFromSelectionCallback(QString)));
            layout->addWidget(_verticesPanel);

            layout->addStretch(2);

            CHECK_NULL_PTR_ERROR (_verticesPanel->getNameTextField())
            CHECK_NULL_PTR_ERROR (_geomEntityPanel->getNameTextField ( ))
            _point1TextField->setLinkedSeizureManagers(0, _point2TextField);
            _point2TextField->setLinkedSeizureManagers(
                    _point1TextField, _geomEntityPanel->getNameTextField());
            _geomEntityPanel->getNameTextField()->setLinkedSeizureManagers(
                    _point2TextField, _verticesPanel->getNameTextField());
            _verticesPanel->getNameTextField ()->setLinkedSeizureManagers (_geomEntityPanel->getNameTextField(), 0);
        }

        QtSurfaceConstraintPanel::QtSurfaceConstraintPanel(const QtSurfaceConstraintPanel &p)
                : QtMgx3DOperationsSubPanel(p),_point1TextField(0), _point2TextField(0), _geomEntityPanel(0), _verticesPanel(0)
        {
            MGX_FORBIDDEN ("QtBetween2PntsPanel copy constructor is not allowed.");
        }

        QtSurfaceConstraintPanel &QtSurfaceConstraintPanel::operator=(const QtSurfaceConstraintPanel &)
        {
            MGX_FORBIDDEN ("QtCenterExtremitiesArcPanel assignment operator is not allowed.");
            return *this;
        }    // QtCenterExtremitiesArcPanel::operator =

        QtSurfaceConstraintPanel::~QtSurfaceConstraintPanel ( )
        {
        }    // QtCenterExtremitiesArcPanel::~QtCenterExtremitiesArcPanel

        string QtSurfaceConstraintPanel::getGeomEntityName() const
        {
            CHECK_NULL_PTR_ERROR (getMainWindow ( ))
            CHECK_NULL_PTR_ERROR (_geomEntityPanel)


            return _geomEntityPanel->getUniqueName();

        }	// QtTopologyVerticesAlignmentPanel::getPoint1


        Math::Point QtSurfaceConstraintPanel::getPoint1 ( ) const
        {
            CHECK_NULL_PTR_ERROR (getMainWindow ( ))
            CHECK_NULL_PTR_ERROR (_point1TextField)
            Math::Point	point;

            if (1 == _point1TextField->getObjectsCount ( ))
            {
                const vector<string>	names	= _point1TextField->getUniqueNames ( );
                const string			name	= names [0];


                switch (_point1TextField->getObjectType ( ))
                {
                    case Entity::GeomVertex	:
                    {

                        const Geom::Vertex*	vertex	=
                                getMainWindow ( )->getContext ( ).getGeomManager (
                                ).getVertex (name, true);
                        CHECK_NULL_PTR_ERROR (vertex)
                        point	= vertex->getPoint ( );
                    }	// case Entity::GeomVertex
                        break;
                    case Entity::TopoVertex	:
                    {
                        const Topo::Vertex*	vertex	=
                                getMainWindow ( )->getContext ( ).getTopoManager (
                                ).getVertex (name, true);
                        CHECK_NULL_PTR_ERROR (vertex)
                        point	= vertex->getCoord ( );
                    }	// case Entity::TopoVertex
                        break;
                    default	:
                    {
                        UTF8String	message (Charset::UTF_8);
                        message << "Entité de type "
                                << Entity::objectTypeToObjectTypeName (_point1TextField->getObjectType ( ))
                                << " (" << (unsigned long)_point1TextField->getObjectType ( )
                                << ") non supporté pour la saisie d'un point.";
                        throw Exception (message);
                    }	// default
                }	// switch (_point1TextField->getObjectType ( ))
            }	// if (1 == _point1TextField->getObjectsCount ( ))
            else
                throw Exception (UTF8String ("Absence de premier point de la droite saisi.", Charset::UTF_8));

            return point;
        }	// QtTopologyVerticesAlignmentPanel::getPoint1


        Math::Point QtSurfaceConstraintPanel::getPoint2 ( ) const
        {
            CHECK_NULL_PTR_ERROR (getMainWindow ( ))
            CHECK_NULL_PTR_ERROR (_point2TextField)
            Math::Point	point;

            if (1 == _point2TextField->getObjectsCount ( ))
            {
                const vector<string>	names	= _point2TextField->getUniqueNames ( );
                const string			name	= names [0];
                switch (_point2TextField->getObjectType ( ))
                {
                    case Entity::GeomVertex	:
                    {

                        const Geom::Vertex*	vertex	=
                                getMainWindow ( )->getContext ( ).getGeomManager (
                                ).getVertex (name, true);
                        CHECK_NULL_PTR_ERROR (vertex)
                        point	= vertex->getPoint ( );
                    }	// case Entity::GeomVertex
                        break;
                    case Entity::TopoVertex	:
                    {
                        const Topo::Vertex*	vertex	=
                                getMainWindow ( )->getContext ( ).getTopoManager (
                                ).getVertex (name, true);
                        CHECK_NULL_PTR_ERROR (vertex)
                        point	= vertex->getCoord ( );
                    }	// case Entity::TopoVertex
                        break;
                    default	:
                    {
                        UTF8String	message (Charset::UTF_8);
                        message << "Entité de type "
                                << Entity::objectTypeToObjectTypeName (_point2TextField->getObjectType ( ))
                                << " (" << (unsigned long)_point2TextField->getObjectType ( )
                                << ") non supporté pour la saisie d'un point.";
                        throw Exception (message);
                    }	// default
                }	// switch (_point2TextField->getObjectType ( ))
            }	// if (1 == _point2TextField->getObjectsCount ( ))
            else
                throw Exception (UTF8String ("Absence de second point de la droite saisi.", Charset::UTF_8));

            return point;
        }	// QtTopologyVerticesAlignmentPanel::getPoint2


        vector<string> QtSurfaceConstraintPanel::getVerticesNames ( ) const
        {
            CHECK_NULL_PTR_ERROR (_verticesPanel)
            return _verticesPanel->getUniqueNames ( );
        }	// QtTopologyVerticesAlignmentPanel::getVerticesNames

        void QtSurfaceConstraintPanel::parametersModifiedCallback()
        {
            emit parametersModified();
        }


        void QtSurfaceConstraintPanel::reset ( )
        {
            BEGIN_QT_TRY_CATCH_BLOCK

                CHECK_NULL_PTR_ERROR (_point1TextField)
                CHECK_NULL_PTR_ERROR (_point2TextField)
                CHECK_NULL_PTR_ERROR (_verticesPanel)
                CHECK_NULL_PTR_ERROR (_geomEntityPanel)
                _point1TextField->reset ( );
                _point2TextField->reset ( );
                _geomEntityPanel->reset ( );
                _verticesPanel->reset ( );

            COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

            QtMgx3DOperationPanel::reset ( );
        }	// QtTopologyVerticesAlignmentPanel::reset


        void QtSurfaceConstraintPanel::cancel ( )
        {
            QtMgx3DOperationPanel::cancel ( );

            CHECK_NULL_PTR_ERROR (_point1TextField)
            CHECK_NULL_PTR_ERROR (_point2TextField)
            CHECK_NULL_PTR_ERROR (_geomEntityPanel)
            CHECK_NULL_PTR_ERROR (_verticesPanel)

            _point1TextField->setInteractiveMode (false);
            _point2TextField->setInteractiveMode (false);
            _geomEntityPanel->stopSelection ();
            _verticesPanel->stopSelection ( );
            if (true == cancelClearEntities ( ))
            {
                BEGIN_QT_TRY_CATCH_BLOCK

                    const vector<string>	names;
                    _point1TextField->setUniqueNames (names);
                    _point2TextField->setUniqueNames (names);
                    _geomEntityPanel->clearSelection ();
                    _verticesPanel->setUniqueName ("");

                COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
            }	// if (true == cancelClearEntities ( ))
        }	// QtTopologyVerticesAlignmentPanel::cancel


        void QtSurfaceConstraintPanel::autoUpdate ( )
        {
            CHECK_NULL_PTR_ERROR (_point1TextField)
            CHECK_NULL_PTR_ERROR (_point2TextField)
            CHECK_NULL_PTR_ERROR (_geomEntityPanel)
            CHECK_NULL_PTR_ERROR (_verticesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
            if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedVertices	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoVertex);
		if (0 != selectedVertices.size ( ))
			_verticesPanel->setUniqueNames (selectedVertices);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
            _point1TextField->clearSelection ( );
            _point2TextField->clearSelection ( );
            _geomEntityPanel->clearSelection( );
            _verticesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME
            _verticesPanel->actualizeGui (true);

            QtMgx3DOperationPanel::autoUpdate ( );
        }


// ===========================================================================
//                        LA CLASSE QtTopologyVerticesAlignmentPanel
// ===========================================================================

QtTopologyVerticesAlignmentPanel::QtTopologyVerticesAlignmentPanel (
			QWidget* parent, const string& panelName,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (
				parent, mainWindow, action,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).alignVerticesOperationURL,
				QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).alignVerticesOperationTag),
      _operationMethodComboBox (0), _currentParentWidget (0), _currentPanel (0), _between2PntsPanel(0), _surfaceConstraintPanel(0)
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

	// La droite de projection :
//	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	QGridLayout*	hlayout	= new QGridLayout (0);
	hlayout->setContentsMargins (0, 0, 0, 0);
	layout->addLayout (hlayout);


    _operationMethodComboBox	= new QComboBox (this);
    _operationMethodComboBox->addItem (QString::fromUtf8("Entre deux points"));
    _operationMethodComboBox->addItem (QString::fromUtf8("Contraint sur une surface"));
    connect (_operationMethodComboBox, SIGNAL (currentIndexChanged (int)), this, SLOT (operationMethodCallback ( )));
    hlayout->addWidget (_operationMethodComboBox);

    // Définition de l'arc de cercle :
	QtGroupBox*		groupBox	= new QtGroupBox(QString::fromUtf8("Paramètres de l'alignement"), this);
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	vlayout->setContentsMargins  (
            Internal::Resources::instance ( )._margin.getValue ( ), Internal::Resources::instance ( )._margin.getValue ( ),
            Internal::Resources::instance ( )._margin.getValue ( ), Internal::Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Internal::Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (vlayout);
	layout->addWidget (groupBox);
	// Le panneau "méthode" : plusieurs panneaux possibles.
	// On garde un parent unique dans la layout :
	_currentParentWidget	= new QWidget (groupBox);
	vlayout->addWidget (_currentParentWidget);
	QHBoxLayout*	currentLayout = new QHBoxLayout (_currentParentWidget);
	_currentParentWidget->setLayout (currentLayout);
	// Panneau centre/extrémités :
	_between2PntsPanel	= new QtBetween2PntsPanel (0, mainWindow);
	connect (_between2PntsPanel, SIGNAL (parametersModified ( )), this, SLOT (parametersModifiedCallback ( )));
    _between2PntsPanel->hide ( );
	// Panneau angles/rayon/repère :
	_surfaceConstraintPanel   = new QtSurfaceConstraintPanel (0, mainWindow);
	connect (_surfaceConstraintPanel, SIGNAL (parametersModified ( )), this, SLOT (parametersModifiedCallback ( )));
    _surfaceConstraintPanel->hide ( );
    operationMethodCallback ( );

    vlayout->addStretch (2);
}	// QtTopologyVerticesAlignmentPanel::QtTopologyVerticesAlignmentPanel


QtTopologyVerticesAlignmentPanel::QtTopologyVerticesAlignmentPanel (
									const QtTopologyVerticesAlignmentPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow (0), 0, "", ""),
      _operationMethodComboBox (0), _currentParentWidget (0), _currentPanel (0), _between2PntsPanel(0), _surfaceConstraintPanel(0)
{
	MGX_FORBIDDEN ("QtTopologyVerticesAlignmentPanel copy constructor is not allowed.");
}	// QtTopologyVerticesAlignmentPanel::QtTopologyVerticesAlignmentPanel (const QtTopologyVerticesAlignmentPanel&)


QtTopologyVerticesAlignmentPanel& QtTopologyVerticesAlignmentPanel::operator = (
										const QtTopologyVerticesAlignmentPanel&)
{
	MGX_FORBIDDEN ("QtTopologyVerticesAlignmentPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyVerticesAlignmentPanel::QtTopologyVerticesAlignmentPanel (const QtTopologyVerticesAlignmentPanel&)


QtTopologyVerticesAlignmentPanel::~QtTopologyVerticesAlignmentPanel ( )
{
}	// QtTopologyVerticesAlignmentPanel::~QtTopologyVerticesAlignmentPanel


void QtTopologyVerticesAlignmentPanel::reset ( )
{
    BEGIN_QT_TRY_CATCH_BLOCK

        //CHECK_NULL_PTR_ERROR (_namePanel)
        CHECK_NULL_PTR_ERROR (_currentPanel)
        CHECK_NULL_PTR_ERROR (_between2PntsPanel)
        CHECK_NULL_PTR_ERROR (_surfaceConstraintPanel)
        //_namePanel->autoUpdate ( );
        _currentPanel->reset ( );
        _between2PntsPanel->reset ( );
        _surfaceConstraintPanel->reset ( );

    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

    QtMgx3DOperationPanel::reset ( );
}	// QtTopologyVerticesAlignmentPanel::reset


void QtTopologyVerticesAlignmentPanel::cancel ( )
{
    QtMgx3DOperationPanel::cancel ( );

    if (0 != _currentPanel)
        _currentPanel->cancel ( );
}	// QtTopologyVerticesAlignmentPanel::cancel


void QtTopologyVerticesAlignmentPanel::autoUpdate ( )
{
    CHECK_NULL_PTR_ERROR (_currentPanel)
    CHECK_NULL_PTR_ERROR (_between2PntsPanel)
    CHECK_NULL_PTR_ERROR (_surfaceConstraintPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		const vector<string>	selectedVertices	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoVertex);
		if (0 != selectedVertices.size ( ))
			_verticesPanel->setUniqueNames (selectedVertices);

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
    if (0 != _currentPanel)
        _currentPanel->autoUpdate( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyVerticesAlignmentPanel::autoUpdate

vector<Entity*> QtTopologyVerticesAlignmentPanel::getInvolvedEntities ( )
{
    vector<Entity*>			entities;
    if (0 != _currentPanel)
        entities	= _currentPanel->getPanelEntities ( );

    return entities;
}	// QtTopologyVerticesAlignmentPanel::getInvolvedEntities

void QtTopologyVerticesAlignmentPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

        if (0 != _currentPanel)
            _currentPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtTopologyVerticesAlignmentPanel::operationCompleted

void QtTopologyVerticesAlignmentPanel::operationMethodCallback ( )
{
    BEGIN_QT_TRY_CATCH_BLOCK

    CHECK_NULL_PTR_ERROR (_between2PntsPanel)
    if (0 != _currentPanel)
    {
        preview (false, true);
        highlight (false);
        _currentParentWidget->layout ( )->removeWidget (_currentPanel);
        _currentPanel->hide ( );
        _currentPanel->setParent (0);
    }
    _currentPanel	= 0;

    switch (getOperationMethod ( ))
    {
        case QtTopologyVerticesAlignmentPanel::BETWEEN_2_PNTS			: _currentPanel	= _between2PntsPanel;     break;
        case QtTopologyVerticesAlignmentPanel::SURFACE_CONSTRAINED		: _currentPanel	= _surfaceConstraintPanel;break;
        default	:
        {
            TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "Méthode d'opération non supportée (" << (unsigned long)getOperationMethod ( ) << ").";
            INTERNAL_ERROR (exc, message, "QtArcCircleOperationPanel::operationMethodCallback")
            throw exc;
        }
    }	// switch (getOperationMethod ( ))
    if (0 != _currentPanel)
    {
        _currentPanel->setParent (_currentParentWidget);
        _currentParentWidget->layout ( )->addWidget (_currentPanel);
        _currentPanel->show ( );
        if (0 != getMainWindow ( ))
            getMainWindow ( )->getOperationsPanel ( ).updateLayoutWorkaround( );
    }

    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}

        vector <string> QtTopologyVerticesAlignmentPanel::getVerticesNames() {
            switch (getOperationMethod ( ))
            {
                case BETWEEN_2_PNTS :
                    CHECK_NULL_PTR_ERROR (_between2PntsPanel)
                    return _between2PntsPanel->getVerticesNames( );
                case SURFACE_CONSTRAINED :
                    CHECK_NULL_PTR_ERROR (_surfaceConstraintPanel)
                    return _surfaceConstraintPanel->getVerticesNames ( );
            }   // switch (getOperationMethod ( ))

            throw Exception ("QtArcCircleOperationPanel::getVertex1UniqueName appellée hors contexte.");
        }

        Point QtTopologyVerticesAlignmentPanel::getPoint1() {
            switch (getOperationMethod ( ))
            {
                case BETWEEN_2_PNTS :
                    CHECK_NULL_PTR_ERROR (_between2PntsPanel)
                    return _between2PntsPanel->getPoint1( );
                case SURFACE_CONSTRAINED :
                    CHECK_NULL_PTR_ERROR (_surfaceConstraintPanel)
                    return _surfaceConstraintPanel->getPoint1( );
            }   // switch (getOperationMethod ( ))

            throw Exception ("QtArcCircleOperationPanel::getVertex1UniqueName appellée hors contexte.");
        }
        // QtArcCircleOperationPanel::operationMethodCallback

        Point QtTopologyVerticesAlignmentPanel::getPoint2() {
            switch (getOperationMethod ( ))
            {
                case BETWEEN_2_PNTS :
                    CHECK_NULL_PTR_ERROR (_between2PntsPanel)
                    return _between2PntsPanel->getPoint2( );
                case SURFACE_CONSTRAINED :
                    CHECK_NULL_PTR_ERROR (_surfaceConstraintPanel)
                    return _surfaceConstraintPanel->getPoint2( );
            }   // switch (getOperationMethod ( ))

            throw Exception ("QtArcCircleOperationPanel::getVertex1UniqueName appellée hors contexte.");
        }

        const string QtTopologyVerticesAlignmentPanel::getSurfaceName() {
            switch (getOperationMethod ( ))
            {
                case SURFACE_CONSTRAINED :
                    CHECK_NULL_PTR_ERROR (_surfaceConstraintPanel)
                    return _surfaceConstraintPanel->getGeomEntityName( );
            }   // switch (getOperationMethod ( ))

            throw Exception ("QtArcCircleOperationPanel::getVertex1UniqueName appellée hors contexte.");
        }

        const Vector QtTopologyVerticesAlignmentPanel::getDir() {
            switch (getOperationMethod ( ))
            {
                case SURFACE_CONSTRAINED :
                    CHECK_NULL_PTR_ERROR (_surfaceConstraintPanel)
                    //return _surfaceConstraintPanel->getTranslation( );
            }   // switch (getOperationMethod ( ))

            throw Exception ("QtArcCircleOperationPanel::getVertex1UniqueName appellée hors contexte.");
        }

        QtTopologyVerticesAlignmentPanel::OPERATION_METHOD QtTopologyVerticesAlignmentPanel::getOperationMethod ( ) const
        {
            CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
            return (QtTopologyVerticesAlignmentPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
        }	// QtArcCircleOperationPanel::getOperationMethod


// ===========================================================================
//                  LA CLASSE QtTopologyVerticesAlignmentAction
// ===========================================================================

QtTopologyVerticesAlignmentAction::QtTopologyVerticesAlignmentAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyVerticesAlignmentPanel*	operationPanel	= 
			new QtTopologyVerticesAlignmentPanel (&getOperationPanelParent ( ),
					text.toStdString ( ), mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtTopologyVerticesAlignmentAction::QtTopologyVerticesAlignmentAction


QtTopologyVerticesAlignmentAction::QtTopologyVerticesAlignmentAction (
										const QtTopologyVerticesAlignmentAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyVerticesAlignmentAction copy constructor is not allowed.")
}	// QtTopologyVerticesAlignmentAction::QtTopologyVerticesAlignmentAction


QtTopologyVerticesAlignmentAction& QtTopologyVerticesAlignmentAction::operator = (
										const QtTopologyVerticesAlignmentAction&)
{
	MGX_FORBIDDEN ("QtTopologyVerticesAlignmentAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyVerticesAlignmentAction::operator =


QtTopologyVerticesAlignmentAction::~QtTopologyVerticesAlignmentAction ( )
{
}	// QtTopologyVerticesAlignmentAction::~QtTopologyVerticesAlignmentAction


QtTopologyVerticesAlignmentPanel*
					QtTopologyVerticesAlignmentAction::getTopologyVerticesAlignmentPanel ( )
{
	return dynamic_cast<QtTopologyVerticesAlignmentPanel*>(getOperationPanel ( ));
}	// QtTopologyVerticesAlignmentAction::getTopologyVerticesAlignmentPanel


void QtTopologyVerticesAlignmentAction::executeOperation ( )
{
    Mgx3D::Internal::M3DCommandResultIfc*	cmdResult	= 0;

	QtTopologyVerticesAlignmentPanel*	panel	= dynamic_cast<QtTopologyVerticesAlignmentPanel*>(getTopologyVerticesAlignmentPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

    switch (panel->getOperationMethod ( ))
    {
        case QtTopologyVerticesAlignmentPanel::BETWEEN_2_PNTS:{
            // Récupération des paramètres d'alignement des sommets topologiques :
            vector<string>	vertices	= panel->getVerticesNames ( );
            Math::Point		p1			= panel->getPoint1 ( );
            Math::Point		p2			= panel->getPoint2 ( );

            cmdResult =    getContext ( ).getTopoManager ( ).alignVertices (p1, p2, vertices);
        }
        break;
        case QtTopologyVerticesAlignmentPanel::SURFACE_CONSTRAINED:
        {
            vector<string>	            vertices    = panel->getVerticesNames();
            const string                surface     = panel->getSurfaceName();
            Math::Point		p1			= panel->getPoint1 ( );
            Math::Point		p2			= panel->getPoint2 ( );

            cmdResult =    getContext ( ).getTopoManager ( ).alignVerticesOnSurface(surface, vertices[0],p1, p2);
        }
        break;
    }
	// Validation paramétrage :
	QtMgx3DOperationAction::executeOperation ( );

	CHECK_NULL_PTR_ERROR (cmdResult)
	setCommandResult (cmdResult);
	if (CommandIfc::FAIL == cmdResult->getStatus ( ))
		throw Exception (cmdResult->getErrorMessage ( ));
	else if (CommandIfc::CANCELED == cmdResult->getStatus ( ))
		throw Exception ("Opération annulée");
}	// QtTopologyVerticesAlignmentAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
