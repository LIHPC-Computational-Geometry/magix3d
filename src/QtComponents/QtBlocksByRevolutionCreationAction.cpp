/**
 * \file        QtBlocksByRevolutionCreationAction.cpp
 * \author      Charles PIGNEROL
 * \date        28/10/1312
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/ValidatedField.h"
#include "Topo/TopoManagerIfc.h"
#include "QtComponents/QtBlocksByRevolutionCreationAction.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/QtNumericFieldsFactory.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QLabel>
#include <QVBoxLayout>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Group;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtBlocksByRevolutionCreationPanel
// ===========================================================================

QtBlocksByRevolutionCreationPanel::QtBlocksByRevolutionCreationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action)
	: QtMgx3DOperationPanel (parent, mainWindow, action,
			QtMgx3DApplication::HelpSystem::instance ( ).blocksByRevolutionCreationURL,
			QtMgx3DApplication::HelpSystem::instance ( ).blocksByRevolutionCreationTag),
	  _edgePanel (0), _anglePanel (0), _oGridRatioTextField (0)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	layout->setContentsMargins (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	layout->setSpacing (Resources::instance ( )._spacing.getValue ( )); 

	// Nom opération :
	QLabel*	label	= new QLabel (panelName.c_str ( ), this);
	QFont	font (label->font ( ));
	font.setBold (true);
	label->setFont (font);
	layout->addWidget (label);

	// Définition du bloc :
	QtGroupBox*	groupBox	=
					new QtGroupBox (this, "Paramètres pour l'extrusion");
	layout->addWidget (groupBox);
	QVBoxLayout*	vlayout	= new QVBoxLayout (groupBox);
	vlayout->setContentsMargins (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	vlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	groupBox->setLayout (vlayout);
	const string	suggestedID;
	_edgePanel	= new QtMgx3DEdgePanel (
					groupBox, "Sélection d'arêtes sur le tracé de l'o-grid",
					true, suggestedID, &mainWindow,
					FilterEntity::TopoCoEdge);
	_edgePanel->setMultiSelectMode (true);
	connect (_edgePanel, SIGNAL (edgesAddedToSelection(QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_edgePanel, SIGNAL (edgesRemovedFromSelection(QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	groupBox->setMargin (0);
	vlayout->addWidget (_edgePanel);

	QtGroupBox*	angleGroupBox	= new QtGroupBox (QString::fromUtf8("Portion de la révolution"), this);
	QHBoxLayout*	hlayout	= new QHBoxLayout (angleGroupBox);
	hlayout->setContentsMargins (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( )); 
	angleGroupBox->setLayout (hlayout);
	vlayout->addWidget (angleGroupBox);
	_anglePanel	= new QtAnglePanel (
				angleGroupBox, "Magix 3D", 4 + MgxNumeric::mgxAngleDecimalNum,
				MgxNumeric::mgxAngleDecimalNum, 90., "degrés", false);
	hlayout->addWidget (_anglePanel);
	hlayout->addStretch (1);
//	_anglePanel->setRange (0., 360.);
	_anglePanel->addPredefinedValue ("Entier", 360.);
	_anglePanel->addPredefinedValue ("Demi", 180.);
	_anglePanel->addPredefinedValue ("Tiers", 120.);
	_anglePanel->addPredefinedValue ("Quart", 90.);
	_anglePanel->addPredefinedValue ("Cinquième", 72.);
	_anglePanel->addPredefinedValue ("Sixième", 60.);
	_anglePanel->setPredefinedValue ("Quart");
	connect (_anglePanel, SIGNAL (angleModified (double)), this,
	         SLOT (parametersModifiedCallback ( )));

	// Le ratio de positionnement de l'o-grid :
	QHBoxLayout*	hlayout2	= new QHBoxLayout ( );
	layout->addLayout (hlayout2);
	hlayout2->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout2->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	label	= new QLabel ("Ratio pour sommets de l'o-grid :", this);
	hlayout2->addWidget (label);
	_oGridRatioTextField	= &QtNumericFieldsFactory::createRatioTextField (this);
	_oGridRatioTextField->setValue (1.0);
	hlayout2->addWidget (_oGridRatioTextField);
	_oGridRatioTextField->setToolTip (QString::fromUtf8("Ratio de positionnement de l'o-grid (1~carré, 0~cercle)."));
	connect (_oGridRatioTextField, SIGNAL (textEdited (const QString&)), this,
	         SLOT (parametersModifiedCallback ( )));

	layout->addStretch (2);

}	// QtBlocksByRevolutionCreationPanel::QtBlocksByRevolutionCreationPanel


QtBlocksByRevolutionCreationPanel::QtBlocksByRevolutionCreationPanel (
								const QtBlocksByRevolutionCreationPanel& cao)
	: QtMgx3DOperationPanel (
			0, *new QtMgx3DMainWindow(0), 0, "", ""),
	  _edgePanel (0), _anglePanel(0), _oGridRatioTextField (0)
{
	MGX_FORBIDDEN ("QtBlocksByRevolutionCreationPanel copy constructor is not allowed.");
}	// QtBlocksByRevolutionCreationPanel::QtBlocksByRevolutionCreationPanel (const QtBlocksByRevolutionCreationPanel&)


QtBlocksByRevolutionCreationPanel&
			QtBlocksByRevolutionCreationPanel::operator = (
									const QtBlocksByRevolutionCreationPanel&)
{
	MGX_FORBIDDEN ("QtBlocksByRevolutionCreationPanel assignment operator is not allowed.");
	return *this;
}	// QtBlocksByRevolutionCreationPanel::QtBlocksByRevolutionCreationPanel (const QtBlocksByRevolutionCreationPanel&)


QtBlocksByRevolutionCreationPanel::~QtBlocksByRevolutionCreationPanel ( )
{
}	// QtBlocksByRevolutionCreationPanel::~QtBlocksByRevolutionCreationPanel


void QtBlocksByRevolutionCreationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_anglePanel)
	CHECK_NULL_PTR_ERROR (_oGridRatioTextField)
	_edgePanel->reset ( );
	_oGridRatioTextField->setValue (1.0);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtBlocksByRevolutionCreationPanel::reset


vector<Entity*> QtBlocksByRevolutionCreationPanel::getInvolvedEntities ( )
{
	vector<Entity*>	entities;

	const vector<string>	edgesNames	= getEdges ( );
	for (vector<string>::const_iterator it = edgesNames.begin ( );
	     edgesNames.end ( ) != it; it++)
	{
		TopoEntity*		edge		=
			getContext ( ).getTopoManager ( ).getCoEdge (*it, false);
	
		if (0 != edge)
			entities.push_back (edge);
	}	// for (vector<string>::const_iterator it = edgesNames.begin ( ); ...

	return entities;
}	// QtBlocksByRevolutionCreationPanel::getInvolvedEntities


vector<string> QtBlocksByRevolutionCreationPanel::getEdges ( ) const
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
	return _edgePanel->getUniqueNames ( );
}	// QtBlocksByRevolutionCreationPanel::getEdges


//unsigned short QtBlocksByRevolutionCreationPanel::getCylinderFraction ( ) const
//{
//	CHECK_NULL_PTR_ERROR (_anglePanel)
//	return _anglePanel->getCircleFraction ( );
//}	// QtBlocksByRevolutionCreationPanel::getCylinderFraction

Utils::Portion::Type QtBlocksByRevolutionCreationPanel::getPortion ( ) const
{
	CHECK_NULL_PTR_ERROR (_anglePanel)
	return _anglePanel->getPortion ( );
}	// QtBlocksByRevolutionCreationPanel::getPortion


double QtBlocksByRevolutionCreationPanel::getAngle ( ) const
{
	CHECK_NULL_PTR_ERROR (_anglePanel)
	return _anglePanel->getAngle ( );
}	// QtBlocksByRevolutionCreationPanel::getAngle

double QtBlocksByRevolutionCreationPanel::getOGridRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_oGridRatioTextField)
	return _oGridRatioTextField->getValue ( );
}	// QtBlocksByRevolutionCreationPanel::getOGridRatio


void QtBlocksByRevolutionCreationPanel::validate ( )
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
		error << "QtBlocksByRevolutionCreationPanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtBlocksByRevolutionCreationPanel::validate


void QtBlocksByRevolutionCreationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );

	CHECK_NULL_PTR_ERROR (_edgePanel)
	_edgePanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_edgePanel->setUniqueName ("");

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtBlocksByRevolutionCreationPanel::cancel


void QtBlocksByRevolutionCreationPanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_edgePanel)
	CHECK_NULL_PTR_ERROR (_anglePanel)

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
#else	// AUTO_UPDATE_OLD_SCHEME
	_edgePanel->setUniqueName ("");
#endif	// AUTO_UPDATE_OLD_SCHEME

	_edgePanel->actualizeGui (true);

	QtMgx3DOperationPanel::autoUpdate ( );
}	// QtBlocksByRevolutionCreationPanel::autoUpdate


void QtBlocksByRevolutionCreationPanel::operationCompleted ( )
{
	highlight (false);	// car champs de texte modifiés

	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_edgePanel)
	_edgePanel->stopSelection ( );
	_edgePanel->setUniqueName ("");

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::operationCompleted ( );
}	// QtBlocksByRevolutionCreationPanel::operationCompleted


// ===========================================================================
//                  LA CLASSE QtBlocksByRevolutionCreationAction
// ===========================================================================

QtBlocksByRevolutionCreationAction::QtBlocksByRevolutionCreationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DTopoOperationAction (icon, text, mainWindow, tooltip)
{
	QtBlocksByRevolutionCreationPanel*	operationPanel	=
		new QtBlocksByRevolutionCreationPanel (
			&getOperationPanelParent ( ), text.toStdString ( ), 
			mainWindow, this);
	setOperationPanel (operationPanel);
}	// QtBlocksByRevolutionCreationAction::QtBlocksByRevolutionCreationAction


QtBlocksByRevolutionCreationAction::QtBlocksByRevolutionCreationAction (
										const QtBlocksByRevolutionCreationAction&)
	: QtMgx3DTopoOperationAction (
						QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtBlocksByRevolutionCreationAction copy constructor is not allowed.")
}	// QtBlocksByRevolutionCreationAction::QtBlocksByRevolutionCreationAction


QtBlocksByRevolutionCreationAction&
					 QtBlocksByRevolutionCreationAction::operator = (
									const QtBlocksByRevolutionCreationAction&)
{
	MGX_FORBIDDEN ("QtBlocksByRevolutionCreationAction assignment operator is not allowed.")
	return *this;
}	// QtBlocksByRevolutionCreationAction::operator =


QtBlocksByRevolutionCreationAction::~QtBlocksByRevolutionCreationAction ( )
{
}	// QtBlocksByRevolutionCreationAction::~QtBlocksByRevolutionCreationAction


QtBlocksByRevolutionCreationPanel*
		QtBlocksByRevolutionCreationAction::getBlocksByRevolutionPanel ( )
{
	return dynamic_cast<QtBlocksByRevolutionCreationPanel*>(getOperationPanel ( ));
}	// QtBlocksByRevolutionCreationAction::getBlocksByRevolutionPanel


void QtBlocksByRevolutionCreationAction::executeOperation ( )
{
	// Validation paramétrage :
	M3DCommandResultIfc*	cmdResult	= 0;
	QtMgx3DTopoOperationAction::executeOperation ( );

	// Récupération des paramètres de création du cylindre :
	QtBlocksByRevolutionCreationPanel*	panel	= getBlocksByRevolutionPanel( );
	CHECK_NULL_PTR_ERROR (panel)
	double ratio = panel->getOGridRatio();
	vector<string> coedgeNames	= panel->getEdges ( );
	if (ratio == 1.0)
		cmdResult	= getContext ( ).getTopoManager ( ).makeBlocksByRevol (coedgeNames, panel->getPortion ( ));
	else
		cmdResult	= getContext ( ).getTopoManager ( ).makeBlocksByRevolWithRatioOgrid(coedgeNames, panel->getPortion ( ), ratio);

	setCommandResult (cmdResult);
}	// QtBlocksByRevolutionCreationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
