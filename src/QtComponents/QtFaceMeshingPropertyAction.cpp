/**
 * \file        QtFaceMeshingPropertyAction.cpp
 * \author      Charles PIGNEROL
 * \date        05/11/2013
 */

#include "Internal/Context.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/ValidatedField.h"
#include "Internal/Resources.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtFaceMeshingPropertyAction.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QBoxLayout>

#include <values.h>	// DBL_MAX


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtFaceTransfinitePanel
// ===========================================================================


QtFaceTransfinitePanel::QtFaceTransfinitePanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel)
{
//	setContentsMargins (0, 0, 0, 0);
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (0, 0, 0, 0);
	layout->setSpacing (5);
}	// QtFaceTransfinitePanel::QtFaceTransfinitePanel


QtFaceTransfinitePanel::QtFaceTransfinitePanel (const QtFaceTransfinitePanel& p)
	  : QtMgx3DOperationsSubPanel (p)
{
	MGX_FORBIDDEN ("QtFaceTransfinitePanel copy constructor is not allowed.");
}	// QtFaceTransfinitePanel::QtFaceTransfinitePanel


QtFaceTransfinitePanel& QtFaceTransfinitePanel::operator = (
												const QtFaceTransfinitePanel&)
{
	 MGX_FORBIDDEN ("QtFaceTransfinitePanel assignment operator is not allowed.");
	return *this;
}	// QtFaceTransfinitePanel::operator =


QtFaceTransfinitePanel::~QtFaceTransfinitePanel ( )
{
}	// QtFaceTransfinitePanel::~QtFaceTransfinitePanel


FaceMeshingPropertyTransfinite* QtFaceTransfinitePanel::getMeshingProperty ( ) const
{
	return new FaceMeshingPropertyTransfinite ( );
}	// QtFaceTransfinitePanel::getMeshingProperty



// ===========================================================================
//                        LA CLASSE QtFaceDelaunayGMSHPanel
// ===========================================================================


QtFaceDelaunayGMSHPanel::QtFaceDelaunayGMSHPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel),
	  _minTextField (0), _maxTextField (0)
{
//	setContentsMargins (0, 0, 0, 0);
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( ));

	FaceMeshingPropertyDelaunayGMSH	props;	// Pour récupérer les valeurs par défaut

	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label	= new QLabel ("Minimum :", this);
	hlayout->addWidget (label);
	_minTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_minTextField->setValue (props.getMin ( ));
	hlayout->addWidget (_minTextField);
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel ("Maximum :", this);
	hlayout->addWidget (label);
	_maxTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_maxTextField->setValue (props.getMax ( ));
	hlayout->addWidget (_maxTextField);
}	// QtFaceDelaunayGMSHPanel::QtFaceDelaunayGMSHPanel


QtFaceDelaunayGMSHPanel::QtFaceDelaunayGMSHPanel (
											const QtFaceDelaunayGMSHPanel& p)
	  : QtMgx3DOperationsSubPanel (p), _minTextField (0), _maxTextField (0)
{
	MGX_FORBIDDEN ("QtFaceDelaunayGMSHPanel copy constructor is not allowed.");
}	// QtFaceDelaunayGMSHPanel::QtFaceDelaunayGMSHPanel


QtFaceDelaunayGMSHPanel& QtFaceDelaunayGMSHPanel::operator = (
												const QtFaceDelaunayGMSHPanel&)
{
	 MGX_FORBIDDEN ("QtFaceDelaunayGMSHPanel assignment operator is not allowed.");
	return *this;
}	// QtFaceDelaunayGMSHPanel::operator =


QtFaceDelaunayGMSHPanel::~QtFaceDelaunayGMSHPanel ( )
{
}	// QtFaceDelaunayGMSHPanel::~QtFaceDelaunayGMSHPanel


void QtFaceDelaunayGMSHPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	FaceMeshingPropertyDelaunayGMSH	props;	// Pour récupérer les valeurs par défaut
	CHECK_NULL_PTR_ERROR (_minTextField)
	CHECK_NULL_PTR_ERROR (_maxTextField)
	_minTextField->setValue (props.getMin ( ));
	_maxTextField->setValue (props.getMax ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationsSubPanel::reset ( );
}	// QtFaceDelaunayGMSHPanel::reset


FaceMeshingPropertyDelaunayGMSH*
						QtFaceDelaunayGMSHPanel::getMeshingProperty ( ) const
{
	return new FaceMeshingPropertyDelaunayGMSH (getMin ( ), getMax ( ));
}	// QtFaceDelaunayGMSHPanel::getMeshingProperty


double QtFaceDelaunayGMSHPanel::getMin ( ) const
{
	CHECK_NULL_PTR_ERROR (_minTextField)
	return _minTextField->getValue ( );
}	// QtFaceDelaunayGMSHPanel::getMin


double QtFaceDelaunayGMSHPanel::getMax ( ) const
{
	CHECK_NULL_PTR_ERROR (_maxTextField)
	return _maxTextField->getValue ( );
}	// QtFaceDelaunayGMSHPanel::getMax


// ===========================================================================
//                        LA CLASSE QtFaceQuadPairingPanel
// ===========================================================================


QtFaceQuadPairingPanel::QtFaceQuadPairingPanel (
	QWidget* parent, QtMgx3DMainWindow& window, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationsSubPanel (parent, window, mainPanel),
	  _minTextField (0), _maxTextField (0)
{
//	setContentsMargins (0, 0, 0, 0);
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));

	FaceMeshingPropertyQuadPairing props;	// Pour récupérer les valeurs par défaut

	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	QLabel*	label	= new QLabel ("Minimum :", this);
	hlayout->addWidget (label);
	_minTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_minTextField->setValue (props.getMin ( ));
	hlayout->addWidget (_minTextField);
	hlayout	= new QHBoxLayout ( );
	layout->addLayout (hlayout);
	label	= new QLabel ("Maximum :", this);
	hlayout->addWidget (label);
	_maxTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	_maxTextField->setValue (props.getMax ( ));
	hlayout->addWidget (_maxTextField);
}	// QtFaceQuadPairingPanel::QtFaceQuadPairingPanel


QtFaceQuadPairingPanel::QtFaceQuadPairingPanel (
											const QtFaceQuadPairingPanel& p)
	  : QtMgx3DOperationsSubPanel (p), _minTextField (0), _maxTextField (0)
{
	MGX_FORBIDDEN ("QtFaceQuadPairingPanel copy constructor is not allowed.");
}	// QtFaceQuadPairingPanel::QtFaceQuadPairingPanel


QtFaceQuadPairingPanel& QtFaceQuadPairingPanel::operator = (
												const QtFaceQuadPairingPanel&)
{
	 MGX_FORBIDDEN ("QtFaceQuadPairingPanel assignment operator is not allowed.");
	return *this;
}	// QtFaceQuadPairingPanel::operator =


QtFaceQuadPairingPanel::~QtFaceQuadPairingPanel ( )
{
}	// QtFaceQuadPairingPanel::~QtFaceDelaunayGMSHPanel


void QtFaceQuadPairingPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	FaceMeshingPropertyQuadPairing props;	// Pour récupérer les valeurs par défaut

	CHECK_NULL_PTR_ERROR (_minTextField)
	CHECK_NULL_PTR_ERROR (_maxTextField)
	_minTextField->setValue (props.getMin ( ));
	_maxTextField->setValue (props.getMax ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationsSubPanel::reset ( );
}	// QtFaceQuadPairingPanel::reset


FaceMeshingPropertyQuadPairing*
						QtFaceQuadPairingPanel::getMeshingProperty ( ) const
{
	return new FaceMeshingPropertyQuadPairing(getMin ( ), getMax ( ));
}	// QtFaceQuadPairingPanel::getMeshingProperty


double QtFaceQuadPairingPanel::getMin ( ) const
{
	CHECK_NULL_PTR_ERROR (_minTextField)
	return _minTextField->getValue ( );
}	// QtFaceQuadPairingPanel::getMin


double QtFaceQuadPairingPanel::getMax ( ) const
{
	CHECK_NULL_PTR_ERROR (_maxTextField)
	return _maxTextField->getValue ( );
}	// QtFaceQuadPairingPanel::getMax

// ===========================================================================
//                        LA CLASSE QtFaceMeshingPropertyPanel
// ===========================================================================

QtFaceMeshingPropertyPanel::QtFaceMeshingPropertyPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).facesMeshingPropertyOperationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).facesMeshingPropertyOperationTag),
	  _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _transfinitePanel (0), 
	  _delaunayGMSHPanel (0),
	  _quadPairingPanel(0),  _facesPanel (0)
{
//	SET_WIDGET_BACKGROUND (this, Qt::yellow)
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Les faces à mailler :
	_facesPanel	= new QtMgx3DEntityPanel (
						this, "", true, "Faces :", "", &mainWindow,
						SelectionManagerIfc::D2, FilterEntity::TopoCoFace);
	_facesPanel->setMultiSelectMode (true);
	layout->addWidget (_facesPanel);
	connect (_facesPanel, SIGNAL (entitiesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_facesPanel, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));

	// Méthode de maillage des faces :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	label	= new QLabel (QString::fromUtf8("Méthode"), this);
	hlayout->addWidget (label);
	_operationMethodComboBox	= new QComboBox (this);
	_operationMethodComboBox->addItem ("Maillage transfini");
	_operationMethodComboBox->addItem ("Maillage Delaunay (GMSH)");
    //_operationMethodComboBox->addItem ("Maillage Quad non structuré (pairing) [inactif]");
	connect (_operationMethodComboBox, SIGNAL (currentIndexChanged (int)),
	         this, SLOT (operationMethodCallback ( )));
	hlayout->addWidget (_operationMethodComboBox);
	hlayout->addStretch (2);
	
	// Définition de la maillage :
	QtGroupBox*		groupBox	=
						new QtGroupBox (this, "Paramètres de maillage");
//	SET_WIDGET_BACKGROUND (groupBox, Qt::green)
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	vlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (vlayout);
	layout->addWidget (groupBox);
	// Le panneau "méthode" : plusieurs panneaux possibles.
	// On garde un parent unique dans la layout :
	_currentParentWidget	= new QWidget (groupBox);
	vlayout->addWidget (_currentParentWidget);
	QHBoxLayout*	currentLayout = new QHBoxLayout (_currentParentWidget);
	_currentParentWidget->setLayout (currentLayout);
	_transfinitePanel	= new QtFaceTransfinitePanel (0, mainWindow, this);
//	_transfinitePanel->layout ( )->setSpacing (5);
//	_transfinitePanel->layout ( )->setContentsMargins (0, 0, 0, 0);
	_transfinitePanel->hide ( );
	_delaunayGMSHPanel	= new QtFaceDelaunayGMSHPanel (0, mainWindow, this);
	_delaunayGMSHPanel->hide ( );
//    _quadPairingPanel = new QtFaceQuadPairingPanel(0, mainWindow, this);
//    _quadPairingPanel->hide ( );

	addPreviewCheckBox (true);

	operationMethodCallback ( );

	vlayout->addStretch (2);
	layout->addStretch (2);
}	// QtFaceMeshingPropertyPanel::QtFaceMeshingPropertyPanel


QtFaceMeshingPropertyPanel::QtFaceMeshingPropertyPanel (const QtFaceMeshingPropertyPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _operationMethodComboBox (0),
	  _currentParentWidget (0), _currentPanel (0),
	  _transfinitePanel (0), 
	  _delaunayGMSHPanel (0),
	  _quadPairingPanel(0), _facesPanel (0)
{
	MGX_FORBIDDEN ("QtFaceMeshingPropertyPanel copy constructor is not allowed.");
}	// QtFaceMeshingPropertyPanel::QtFaceMeshingPropertyPanel (const QtFaceMeshingPropertyPanel&)


QtFaceMeshingPropertyPanel& QtFaceMeshingPropertyPanel::operator = (
											const QtFaceMeshingPropertyPanel&)
{
	MGX_FORBIDDEN ("QtFaceMeshingPropertyPanel assignment operator is not allowed.");
	return *this;
}	// QtFaceMeshingPropertyPanel::QtFaceMeshingPropertyPanel (const QtFaceMeshingPropertyPanel&)


QtFaceMeshingPropertyPanel::~QtFaceMeshingPropertyPanel ( )
{
}	// QtFaceMeshingPropertyPanel::~QtFaceMeshingPropertyPanel


QtFaceMeshingPropertyPanel::OPERATION_METHOD
						QtFaceMeshingPropertyPanel::getOperationMethod ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox);
	return (QtFaceMeshingPropertyPanel::OPERATION_METHOD)_operationMethodComboBox->currentIndex ( );
}	// QtFaceMeshingPropertyPanel::getOperationMethod


CoFaceMeshingProperty* QtFaceMeshingPropertyPanel::getMeshingProperty ( ) const
{
	CHECK_NULL_PTR_ERROR (_transfinitePanel)
	CHECK_NULL_PTR_ERROR (_delaunayGMSHPanel)
	switch (getOperationMethod ( ))
	{
		case QtFaceMeshingPropertyPanel::TRANSFINITE			:
			return _transfinitePanel->getMeshingProperty ( );
		case QtFaceMeshingPropertyPanel::DELAUNAY_GMSH			:
			return _delaunayGMSHPanel->getMeshingProperty ( );
//        case QtFaceMeshingPropertyPanel::QUAD_PAIRING                :
//            return _quadPairingPanel->getMeshingProperty ( );
	}	// switch (getOperationMethod ( ))

	INTERNAL_ERROR (exc, "Méthode de maillage de face non supportée", "QtFaceMeshingPropertyPanel::getMeshingProperty")
	throw exc;
}	// QtFaceMeshingPropertyPanel::getMeshingProperty


vector<string> QtFaceMeshingPropertyPanel::getFacesNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_facesPanel)
	return _facesPanel->getUniqueNames ( );
	
}	// QtFaceMeshingPropertyPanel::getFacesNames


void QtFaceMeshingPropertyPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_transfinitePanel)
	CHECK_NULL_PTR_ERROR (_delaunayGMSHPanel)
//	CHECK_NULL_PTR_ERROR (_quadPairingPanel)
	CHECK_NULL_PTR_ERROR (_facesPanel)
	_transfinitePanel->reset ( );
	_delaunayGMSHPanel->reset ( );
//	_quadPairingPanel->reset ( );
	_facesPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtFaceMeshingPropertyPanel::reset


void QtFaceMeshingPropertyPanel::validate ( )
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
		error << "QtFaceMeshingPropertyPanel::validate : erreur non documentée.";
	}

//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)
	CHECK_NULL_PTR_ERROR (_operationMethodComboBox)
	switch (_operationMethodComboBox->currentIndex ( ))
	{
		case QtFaceMeshingPropertyPanel::TRANSFINITE			:
		case QtFaceMeshingPropertyPanel::DELAUNAY_GMSH			:
//        case QtFaceMeshingPropertyPanel::QUAD_PAIRING           :
			break;
		case -1	:
			if (0 != error.length ( ))
				error << "\n";
			error << "Absence de méthode de maillage de faces sélectionnée.";
			break;
		default		:
			if (0 != error.length ( ))
				error << "\n";
			error << "QtFaceMeshingPropertyPanel::validate : index de méthode "
			      << "d'opération de maillage de faces invalide ("
			      << (long)_operationMethodComboBox->currentIndex ( ) << ").";
	}	// switch (_operationMethodComboBox->currentIndex ( ))

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtFaceMeshingPropertyPanel::validate


void QtFaceMeshingPropertyPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_facesPanel)
	_facesPanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		if (0 != _currentPanel)
			_currentPanel->cancel ( );

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtFaceMeshingPropertyPanel::cancel


void QtFaceMeshingPropertyPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_facesPanel)

#ifdef AUTO_UPDATE_OLD_SCHEME
	if (true == autoUpdateUsesSelection ( ))
	{
		const vector<string>	selectedFaces	=
			getSelectionManager ( ).getEntitiesNames (FilterEntity::TopoCoFace);
		if (0 != selectedFaces.size ( ))
			_facesPanel->setUniqueNames (selectedFaces);
	}	// if (true == autoUpdateUsesSelection ( ))
#else	// AUTO_UPDATE_OLD_SCHEME
	_facesPanel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME

	if (0 != _currentPanel)
		_currentPanel->autoUpdate ( );

	QtMgx3DOperationPanel::autoUpdate ( );

	_facesPanel->actualizeGui (true);
}	// QtFaceMeshingPropertyPanel::autoUpdate


void QtFaceMeshingPropertyPanel::preview (bool show, bool destroyInteractor)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// N'afficher une boite de dialogue d'erreur que si demandé ...
	QT_PREVIEW_BEGIN_TRY_CATCH_BLOCK

	QtMgx3DOperationPanel::preview (show, destroyInteractor);

	if (0 != _currentPanel)
		_currentPanel->preview (show, destroyInteractor);

	QT_PREVIEW_COMPLETE_TRY_CATCH_BLOCK

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtFaceMeshingPropertyPanel::preview


vector<Entity*> QtFaceMeshingPropertyPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	vector<string>	names	= getFacesNames ( );
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		TopoEntity*	entity	= getContext ( ).getTopoManager ( ).getEntity (*it);
		CHECK_NULL_PTR_ERROR (entity)
		entities.push_back (entity);
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	return entities;
}	// QtFaceMeshingPropertyPanel::getInvolvedEntities


void QtFaceMeshingPropertyPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _currentPanel)
		_currentPanel->stopSelection ( );
	if (0 != _facesPanel)
		_facesPanel->stopSelection ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtFaceMeshingPropertyPanel::operationCompleted


void QtFaceMeshingPropertyPanel::operationMethodCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_facesPanel)
	CHECK_NULL_PTR_ERROR (_facesPanel->getNameTextField ( ))
	CHECK_NULL_PTR_ERROR (_transfinitePanel)
	CHECK_NULL_PTR_ERROR (_delaunayGMSHPanel)
//    CHECK_NULL_PTR_ERROR (_quadPairingPanel)
	if (0 != _currentPanel)
	{
		_currentParentWidget->layout ( )->removeWidget (_currentPanel);
		_currentPanel->hide ( );
		_currentPanel->setParent (0);
	}
	_currentPanel	= 0;

	// Pour la majorité des cas :
	_facesPanel->getNameTextField ( )->setLinkedSeizureManagers (0, 0);

	switch (getOperationMethod ( ))
	{
		case QtFaceMeshingPropertyPanel::TRANSFINITE			:
			_currentPanel	= _transfinitePanel;			break;
		case QtFaceMeshingPropertyPanel::DELAUNAY_GMSH			:
			_currentPanel	= _delaunayGMSHPanel;			break;
//		case QtFaceMeshingPropertyPanel::QUAD_PAIRING      :
//	        _currentPanel   = _quadPairingPanel;            break;
		default	:
		{
			UTF8String	message (Charset::UTF_8);
			message << "Méthode d'opération non supportée ("
			        << (unsigned long)getOperationMethod ( ) << ").";
			INTERNAL_ERROR (exc, message, "QtFaceMeshingPropertyPanel::operationMethodCallback")
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

	try
	{
		if (0 != getMainWindow ( ))
			getMainWindow ( )->getGraphicalWidget ( ).getRenderingManager (
															).forceRender ( );
	}
	catch (...)
	{
	}

	parametersModifiedCallback ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtFaceMeshingPropertyPanel::operationMethodCallback


// ===========================================================================
//                  LA CLASSE QtFaceMeshingPropertyAction
// ===========================================================================

QtFaceMeshingPropertyAction::QtFaceMeshingPropertyAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtFaceMeshingPropertyPanel*	operationPanel	=
		new QtFaceMeshingPropertyPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtFaceMeshingPropertyAction::QtFaceMeshingPropertyAction


QtFaceMeshingPropertyAction::QtFaceMeshingPropertyAction (
										const QtFaceMeshingPropertyAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtFaceMeshingPropertyAction copy constructor is not allowed.")
}	// QtFaceMeshingPropertyAction::QtFaceMeshingPropertyAction


QtFaceMeshingPropertyAction& QtFaceMeshingPropertyAction::operator = (
										const QtFaceMeshingPropertyAction&)
{
	MGX_FORBIDDEN ("QtFaceMeshingPropertyAction assignment operator is not allowed.")
	return *this;
}	// QtFaceMeshingPropertyAction::operator =


QtFaceMeshingPropertyAction::~QtFaceMeshingPropertyAction ( )
{
}	// QtFaceMeshingPropertyAction::~QtFaceMeshingPropertyAction


QtFaceMeshingPropertyPanel*
					QtFaceMeshingPropertyAction::getMeshingPropertyPanel ( )
{
	return dynamic_cast<QtFaceMeshingPropertyPanel*>(getOperationPanel ( ));
}	// QtFaceMeshingPropertyAction::getMeshingPropertyPanel


void QtFaceMeshingPropertyAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResult*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres de maillage des faces :
	QtFaceMeshingPropertyPanel*	panel	= getMeshingPropertyPanel ( );
	CHECK_NULL_PTR_ERROR (panel)
	vector<string>					faces		= panel->getFacesNames ( );
	unique_ptr<CoFaceMeshingProperty>	properties (panel->getMeshingProperty ( ));
	cmdResult	= getContext ( ).getTopoManager ( ).setMeshingProperty (*properties.get ( ), faces);

	setCommandResult (cmdResult);
}	// QtFaceMeshingPropertyAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
