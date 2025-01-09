/**
 * \file		QtTopologyVertexCreationAction.cpp
 * \author		Eric B
 * \date		15/3/2019
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtTopologyVertexCreationAction.h"
#include "QtComponents/QtNumericFieldsFactory.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>
#include "Geom/Vertex.h"

#include <values.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents {
// ===========================================================================
//                        LA CLASSE QtTopologyVertexCreationPanel
// ===========================================================================

QtTopologyVertexCreationPanel::QtTopologyVertexCreationPanel (
			QWidget* parent, const string& panelName,
            QtMgx3DGroupNamePanel::POLICY creationPolicy,
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
: QtMgx3DOperationPanel (parent, mainWindow, action,
		QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).vertexCreationURL,
		QtMgx3DApplication::HelpSystem::instance ( ).instance ( ).vertexCreationTag),
		_namePanel(0), _vertexPanel (0)
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

	// Point à associer :
	_vertexPanel	= new QtMgx3DEntityPanel (
						this, "", true, "Point associé :", "",
						&mainWindow, SelectionManagerIfc::D0,
						FilterEntity::GeomVertex);
	layout->addWidget (_vertexPanel);
	connect (_vertexPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_vertexPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

    QHBoxLayout*	hlayout	= new QHBoxLayout (0);
    hlayout->setContentsMargins (0, 0, 0, 0);
    layout->addLayout (hlayout);
    hlayout->addStretch (2.);
    hlayout	= new QHBoxLayout (0);
    hlayout->setContentsMargins (0, 0, 0, 0);
    layout->addLayout (hlayout);
    hlayout->addWidget(new QLabel ("X :", this));
    _xTextField		= &QtNumericFieldsFactory::createPositionTextField (this);
    hlayout->addWidget (_xTextField);
    hlayout->addStretch (2.);
    hlayout	= new QHBoxLayout (0);
    hlayout->setContentsMargins (0, 0, 0, 0);
    layout->addLayout (hlayout);
    hlayout->addWidget (new QLabel ("Y :", this));
    _yTextField		= &QtNumericFieldsFactory::createPositionTextField (this);
    hlayout->addWidget (_yTextField);
    hlayout->addStretch (2.);
    hlayout	= new QHBoxLayout (0);
    hlayout->setContentsMargins (0, 0, 0, 0);
    layout->addLayout (hlayout);
    hlayout->addWidget (new QLabel ("Z :", this));
    _zTextField		= &QtNumericFieldsFactory::createPositionTextField (this);
    hlayout->addWidget (_zTextField);
    hlayout->addStretch (2.);

	layout->addStretch (2);

    _xTextField->setValue (0);
    _yTextField->setValue (0);
    _zTextField->setValue (0);


    connect (_xTextField,  SIGNAL (textChanged (const QString&)), this,
             SLOT (coordinatesModifiedCallback ( )));
    connect (_yTextField,  SIGNAL (textChanged (const QString&)), this,
             SLOT (coordinatesModifiedCallback ( )));
    connect (_zTextField,  SIGNAL (textChanged (const QString&)), this,
             SLOT (coordinatesModifiedCallback ( )));
    connect (_xTextField,  SIGNAL (textEdited (const QString&)), this,
             SLOT (coordinatesEditedCallback ( )));
    connect (_yTextField,  SIGNAL (textEdited (const QString&)), this,
             SLOT (coordinatesEditedCallback ( )));
    connect (_zTextField,  SIGNAL (textEdited (const QString&)), this,
             SLOT (coordinatesEditedCallback ( )));

}	// QtTopologyVertexCreationPanel::QtTopologyVertexCreationPanel


QtTopologyVertexCreationPanel::QtTopologyVertexCreationPanel (
										const QtTopologyVertexCreationPanel& cao)
: QtMgx3DOperationPanel (0, *new QtMgx3DMainWindow(0), 0, "", ""),
	_namePanel(0),	_vertexPanel (0)
{
	MGX_FORBIDDEN ("QtTopologyVertexCreationPanel copy constructor is not allowed.");
}	// QtTopologyVertexCreationPanel::QtTopologyVertexCreationPanel (const QtTopologyVertexCreationPanel&)


QtTopologyVertexCreationPanel& QtTopologyVertexCreationPanel::operator = (
											const QtTopologyVertexCreationPanel&)
{
	MGX_FORBIDDEN ("QtTopologyVertexCreationPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyVertexCreationPanel::QtTopologyVertexCreationPanel (const QtTopologyVertexCreationPanel&)


QtTopologyVertexCreationPanel::~QtTopologyVertexCreationPanel ( )
{
}	// QtTopologyVertexCreationPanel::~QtTopologyVertexCreationPanel


string QtTopologyVertexCreationPanel::getVertexName ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertexPanel)
	return _vertexPanel->getUniqueName ( );
}	// QtTopologyVertexCreationPanel::getVertexName

void QtTopologyVertexCreationPanel::reset ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_vertexPanel)
		_vertexPanel->reset ( );
	}
	catch (...)
	{
	}
}	// QtTopologyVertexCreationPanel::reset

void QtTopologyVertexCreationPanel::validate ( )
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
		error << "QtVertexByProjectionOnSurfacePanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtTopologyVertexCreationPanel::validate

void QtTopologyVertexCreationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_vertexPanel)
	_vertexPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_vertexPanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtTopologyVertexCreationPanel::cancel


void QtTopologyVertexCreationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_vertexPanel)

	_vertexPanel->setUniqueName ("");

	_vertexPanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtTopologyVertexCreationPanel::autoUpdate

Math::Point QtTopologyVertexCreationPanel::getPoint ( ) const
{
            bool	ok;
            double	x, y, z;

            CHECK_NULL_PTR_ERROR (_xTextField)
            ok		= true;
            x	= _xTextField->text ( ).toDouble (&ok);

            if (false == ok) {
                UTF8String	errorMsg (Charset::UTF_8);
                errorMsg << "Création sommet topologique"
                         << " : valeur invalide de la première composante ("
                         << _xTextField->text ( ).toStdString ( ) << ").";
                throw Exception (errorMsg);
            }	// if (false == ok)

            CHECK_NULL_PTR_ERROR (_yTextField)
            ok		= true;
            y	= _yTextField->text ( ).toDouble (&ok);

            if (false == ok) {
                UTF8String	errorMsg (Charset::UTF_8);
                errorMsg << "Création sommet topologique"
                         << " : valeur invalide de la deuxième composante ("
                         << _yTextField->text ( ).toStdString ( ) << ").";
                throw Exception (errorMsg);
            }	// if (false == ok)


            CHECK_NULL_PTR_ERROR (_zTextField)
            ok		= true;
            z	= _zTextField->text ( ).toDouble (&ok);

            if (false == ok) {
                UTF8String	errorMsg (Charset::UTF_8);
                errorMsg << "Création sommet topologique"
                         << " : valeur invalide de la deuxième composante ("
                         << _zTextField->text ( ).toStdString ( ) << ").";
                throw Exception (errorMsg);
            }	// if (false == ok)


            Math::Point	point (x, y, z);

            return point;
}	// QtTopologyVertexCreationPanel::getPoint

string QtTopologyVertexCreationPanel::getGroupName() const
{
    return _namePanel->getGroupName();
}   //QtTopologyVertexCreationPanel::getGroupName

void QtTopologyVertexCreationPanel::updateCoordinates (const string& name)
{

    CHECK_NULL_PTR_ERROR (_vertexPanel)

    Utils::Math::Point pt;


    Geom::Vertex*	vertex	= getContext ().getGeomManager ( ).getVertex (name, true);
    CHECK_NULL_PTR_ERROR (vertex);
    pt = vertex->getPoint();

    CHECK_NULL_PTR_ERROR (_xTextField)
    CHECK_NULL_PTR_ERROR (_yTextField)
    CHECK_NULL_PTR_ERROR (_zTextField)

    _xTextField->setText (QString::number (pt.getX()));
    _yTextField->setText (QString::number (pt.getY()));
    _zTextField->setText (QString::number (pt.getZ()));

}	// QtTopologyVertexCreationPanel::updateCoordinates

        bool QtTopologyVertexCreationPanel::isModified ( ) const
        {
            Math::Point pt;
            double x,y,z;
            try
            {
                pt = getPoint();
                x	= pt.getX ( );
                y	= pt.getY ( );
                z	= pt.getZ ( );
            }
            catch (...)
            {
                return true;
            }

            if ((x != _initialX) || (y != _initialY) || (z != _initialZ))
                return true;

            return false;
        }	// QtTopologyVertexCreationPanel::isModified



//-------------------------------------------------
//                      Callbacks
//-------------------------------------------------
void QtTopologyVertexCreationPanel::entitiesAddedToSelectionCallback (QString entitiesNames)
{
    BEGIN_QT_TRY_CATCH_BLOCK

    updateCoordinates (entitiesNames.toStdString ( ));

    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Création sommet topologique")
}


void QtTopologyVertexCreationPanel::coordinatesEditedCallback ( )
{	// Appelé en cas de changement des coordonnées par l'utilisateur.
    // => l'éventuel vertex correspondant aux coordonnées n'est plus vrai
    // => on l'efface
    BEGIN_QT_TRY_CATCH_BLOCK

    CHECK_NULL_PTR_ERROR (_vertexPanel)
    _vertexPanel->blockSignals (true);
    _vertexPanel->reset();
    _vertexPanel->blockSignals (false);
    emit pointModified ( );

    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Création sommet topologique")
}	// QtMgx3DPointPanel::coordinatesEditedCallback

void QtTopologyVertexCreationPanel::coordinatesModifiedCallback ( )
{	// Appelé en cas de changement des coordonnées, que ce soit du fait de
    // l'utilisateur ou du programme.
    BEGIN_QT_TRY_CATCH_BLOCK

    CHECK_NULL_PTR_ERROR (_vertexPanel)
    emit pointModified ( );

    COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Création sommet topologique")
}	// QtMgx3DPointPanel::coordinatesModifiedCallback


// ===========================================================================
//                  LA CLASSE QtTopologyVertexCreationAction
// ===========================================================================

QtTopologyVertexCreationAction::QtTopologyVertexCreationAction (
	const QIcon& icon, const QString& text,
	QtMgx3DMainWindow& mainWindow, const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy)
	: QtTopologyCreationAction (icon, text, mainWindow, tooltip)
{
	QtTopologyVertexCreationPanel*	operationPanel	=
			new QtTopologyVertexCreationPanel (
					&getOperationPanelParent ( ), text.toStdString ( ), creationPolicy,
					mainWindow, this);
	setOperationPanel (operationPanel);

}	// QtTopologyVertexCreationAction::QtTopologyVertexCreationAction


QtTopologyVertexCreationAction::QtTopologyVertexCreationAction (
										const QtTopologyVertexCreationAction&)
	: QtTopologyCreationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtTopologyVertexCreationAction copy constructor is not allowed.")
}	// QtTopologyVertexCreationAction::QtTopologyVertexCreationAction


QtTopologyVertexCreationAction& QtTopologyVertexCreationAction::operator = (
										const QtTopologyVertexCreationAction&)
{
	MGX_FORBIDDEN ("QtTopologyVertexCreationAction assignment operator is not allowed.")
	return *this;
}	// QtTopologyVertexCreationAction::operator =


QtTopologyVertexCreationAction::~QtTopologyVertexCreationAction ( )
{
}	// QtTopologyVertexCreationAction::~QtTopologyVertexCreationAction


void QtTopologyVertexCreationAction::executeOperation ( )
{
	QtTopologyVertexCreationPanel*	panel	= dynamic_cast<QtTopologyVertexCreationPanel*>(getOperationPanel ( ));
	CHECK_NULL_PTR_ERROR (panel)

	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtTopologyCreationAction::executeOperation ( );

	// Récupération des paramètres d'association des entités topologiques :
	const string	vertex	= panel->getVertexName ( );
    const string    group   = panel->getGroupName();

    if(vertex.empty()){
       Utils::Math::Point pt = panel->getPoint();
       cmdResult    = getContext ( ).getTopoManager( ).newTopoVertex(pt,group);
    }else{
        cmdResult	= getContext ( ).getTopoManager( ).newTopoOnGeometry (vertex);
    }

	setCommandResult (cmdResult);
}	// QtTopologyFaceCreationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
