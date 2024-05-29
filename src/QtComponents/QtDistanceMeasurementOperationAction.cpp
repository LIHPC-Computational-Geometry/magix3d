/**
 * \file        QtDistanceMeasurementOperationAction.cpp
 * \author      Charles PIGNEROL
 * \date        27/01/2015
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Internal/EntitiesHelper.h"
#include "Internal/InfoCommand.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Topo/Vertex.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtDistanceMeasurementOperationAction.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtHelpWindow.h>

#include <TkUtil/ErrorLog.h>
#include <TkUtil/InformationLog.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericServices.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtObjectSignalBlocker.h>

#include <QPushButton>
#include <QVBoxLayout>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//               LA CLASSE QtDistanceMeasurementOperationPanel
// ===========================================================================

QtDistanceMeasurementOperationPanel::QtDistanceMeasurementOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (
			0/*parent*/, mainWindow, action, helpURL, helpTag),
	  _entity1Panel (0), _entity2Panel (0), _distanceLabel (0), _helpURL(helpURL), _helpTag(helpTag)
{
	setWindowTitle (panelName.c_str ( ));
	mainWindow.registerAdditionalOperationPanel (*this);

	QVBoxLayout*	vlayout	= new QVBoxLayout (0);
	vlayout->setSpacing (QtConfiguration::spacing);
	vlayout->setContentsMargins (
					QtConfiguration::margin, QtConfiguration::margin,
					QtConfiguration::margin, QtConfiguration::margin);
	setLayout (vlayout);
	// dim1 et dim2, filter1 et filter2 : cf. MGXDDD-237
	const SelectionManagerIfc::DIM	allowedDimensions1	=
												SelectionManagerIfc::D0;
	const FilterEntity::objectType	filter1				=
		(FilterEntity::objectType)(
				FilterEntity::GeomVertex | FilterEntity::TopoVertex);
	_entity1Panel	= new QtEntityByDimensionSelectorPanel (
						this, mainWindow, "Vertex/Sommet :",
						allowedDimensions1, filter1, SelectionManagerIfc::D0,	
							true);
	vlayout->addWidget (_entity1Panel);
	connect (_entity1Panel, SIGNAL (selectionModified (QString)),
	         this, SLOT (selectionModifiedCallback ( )));
	const SelectionManagerIfc::DIM	allowedDimensions2	=
			(SelectionManagerIfc::DIM)(SelectionManagerIfc::D0 |
							SelectionManagerIfc::D1 | SelectionManagerIfc::D2);
	const FilterEntity::objectType	filter2				=
		(FilterEntity::objectType)(
					FilterEntity::TopoVertex | FilterEntity::GeomVertex |
					FilterEntity::GeomCurve | FilterEntity::GeomSurface);
	_entity2Panel	= new QtEntityByDimensionSelectorPanel (
						this, mainWindow, "Sommet/Vertex/Courbe/Surface :",
						allowedDimensions2, filter2, SelectionManagerIfc::D0,
						true);
	vlayout->addWidget (_entity2Panel);
	connect (_entity2Panel, SIGNAL (selectionModified (QString)),
	         this, SLOT (selectionModifiedCallback ( )));


	// Le résultat :
	_distanceLabel	= new QLabel ("Distance : ", this);
	_distanceLabel->setTextInteractionFlags (
					Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
	vlayout->addWidget (_distanceLabel);

	// Les boutons :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	vlayout->addLayout (hlayout);
	QPushButton*	closeButton	 = new QPushButton("Fermer", this);
	hlayout->addWidget (closeButton);
	connect (closeButton, SIGNAL(clicked (bool)), this, SLOT(closeCallback( )));
	QPushButton*	helpButton	 = new QPushButton("Aide ...", this);
	hlayout->addWidget (helpButton, 0);
	connect (helpButton, SIGNAL (clicked (bool)), this, SLOT (helpCallback( )));


	hlayout->addStretch (2.);

	vlayout->addStretch (2.);

	autoUpdate ( );
}	// QtDistanceMeasurementOperationPanel::QtDistanceMeasurementOperationPanel


QtDistanceMeasurementOperationPanel::QtDistanceMeasurementOperationPanel (
								const QtDistanceMeasurementOperationPanel& dmop)
	: QtMgx3DOperationPanel (
				0,
				*new QtMgx3DMainWindow (0),
				0, "", ""),
	  _entity1Panel (0), _entity2Panel (0), _distanceLabel (0)
{
	MGX_FORBIDDEN ("QtDistanceMeasurementOperationPanel copy constructor is not allowed.");
}	// QtDistanceMeasurementOperationPanel::QtDistanceMeasurementOperationPanel (const QtDistanceMeasurementOperationPanel&)


QtDistanceMeasurementOperationPanel&
					QtDistanceMeasurementOperationPanel::operator = (
									const QtDistanceMeasurementOperationPanel&)
{
	MGX_FORBIDDEN ("QtDistanceMeasurementOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtDistanceMeasurementOperationPanel::QtDistanceMeasurementOperationPanel (const QtDistanceMeasurementOperationPanel&)


QtDistanceMeasurementOperationPanel::~QtDistanceMeasurementOperationPanel ( )
{
	try
	{
		if (0 != getMainWindow ( ))
			getMainWindow ( )->unregisterAdditionalOperationPanel (*this);

		if (0 != getMgx3DOperationAction ( ))
		{
			// Ugly code :
			delete getMgx3DOperationAction ( );
		}
	}
	catch (const Exception& exc)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant la fermeture du panneau mesure de "
		      << "distance : " << exc.getFullMessage ( );
		log (ErrorLog (error));
	}
	catch (const exception& e)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant la fermeture du panneau mesure de "
		      << "distance : " << e.what ( );
		log (ErrorLog (error));
	}
	catch (...)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur non documentée survenue durant la fermeture du "
		      << "panneau mesure de distance.";
		log (ErrorLog (error));
	}
}	// QtDistanceMeasurementOperationPanel::~QtDistanceMeasurementOperationPanel


vector<Entity*> QtDistanceMeasurementOperationPanel::getInvolvedEntities ( )
{
	CHECK_NULL_PTR_ERROR (_entity1Panel)
	CHECK_NULL_PTR_ERROR (_entity2Panel)
	Entity*	entity1	= (Entity*)getEntity1 ( );
	Entity*	entity2	= (Entity*)getEntity2 ( );

	vector<Entity*>	entities;
	if (0 != entity1)
		entities.push_back (entity1);
	if (0 != entity2)
		entities.push_back (entity2);

	return entities;
}	// QtDistanceMeasurementOperationPanel::getInvolvedEntities


void QtDistanceMeasurementOperationPanel::setVisible (bool visible)
{
	QWidget::setVisible (visible);
}	// QtDistanceMeasurementOperationPanel::void setVisible


void QtDistanceMeasurementOperationPanel::commandModifiedCallback (
												InfoCommand& infoCommand)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entity1Panel)
	CHECK_NULL_PTR_ERROR (_entity2Panel)

	// Est-on concerné ?
	if (0 == infoCommand.getNbGeomInfoEntity ( ))
		return;
	bool	reinited	= false;
	for (uint i = 0;
	     (false == reinited) && (i < infoCommand.getNbGeomInfoEntity ( )); i++)
	{
		InfoCommand::type   t	= InfoCommand::DELETED;
		GeomEntity*			ge	= 0;
		infoCommand.getGeomInfoEntity (i, ge, t);
		if ((ge == getEntity1 ( )) || (ge == getEntity2 ( )))
		{
			if (InfoCommand::DELETED == t)
			{
				QtObjectSignalBlocker	blocker1 (_entity1Panel);
				QtObjectSignalBlocker	blocker2 (_entity2Panel);
				if (ge == getEntity1 ( ))
				{
					_entity1Panel->clearSelection ( );
					reinited	= true;
				}
				if (ge == getEntity2 ( ))
				{
					_entity2Panel->clearSelection ( );	
					reinited	= true;
				}
			}	// if (InfoCommand::DELETED == t)
		}	// if ((ge == getEntity1 ( )) || (ge == getEntity2 ( )))
	}	// for (uint i = 0; i < infoCommand.getNbGeomInfoEntity ( ); i++)

	if (true == reinited)
		selectionModifiedCallback ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtDistanceMeasurementOperationPanel::commandModifiedCallback


void QtDistanceMeasurementOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entity1Panel)
	CHECK_NULL_PTR_ERROR (_entity2Panel)
	_entity1Panel->reset ( );
	_entity2Panel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtDistanceMeasurementOperationPanel::reset


void QtDistanceMeasurementOperationPanel::validate ( )
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
		error << "QtDistanceMeasurementOperationPanel::validate : erreur non documentée.";
	}
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtDistanceMeasurementOperationPanel::validate


void QtDistanceMeasurementOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );	// => highlight (false)

	CHECK_NULL_PTR_ERROR (_entity1Panel)
	CHECK_NULL_PTR_ERROR (_entity2Panel)
	CHECK_NULL_PTR_ERROR (_distanceLabel)
	_entity1Panel->stopSelection ( );
	_entity2Panel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_entity1Panel->clearSelection ( );
		_entity2Panel->clearSelection ( );
		updateDistance (NumericServices::doubleMachInfinity());

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtDistanceMeasurementOperationPanel::cancel


void QtDistanceMeasurementOperationPanel::autoUpdate ( )
{
	highlight (false);	// car champs de texte modifés

	CHECK_NULL_PTR_ERROR (_entity1Panel)
	CHECK_NULL_PTR_ERROR (_entity2Panel)
#ifdef AUTO_UPDATE_OLD_SCHEME
	try
	{

	Internal::Context*	context	=
					dynamic_cast<Internal::Context*>(&getContext ( ));
	CHECK_NULL_PTR_ERROR (context)
	Geom::GeomManager*	manager	=
			dynamic_cast<Geom::GeomManager*>(&getContext ( ).getGeomManager( ));
	CHECK_NULL_PTR_ERROR (manager)

	QtMgx3DOperationPanel::autoUpdate ( );

	// Récupération des éléments de maillage :
	vector<string>	selection	=
			getContext ( ).getSelectionManager ( ).getEntitiesNames (
														FilterEntity::AllGeom);
	if (2 >= selection.size ( ))
	{
		vector<string>	entity1, entity2;
		if (1 <= selection.size ( ))
			entity1.push_back (selection [0]);
		if (2 <= selection.size ( ))
			entity2.push_back (selection [1]);
		_entity2Panel->setEntitiesNames (entity2);
	}	// if (2 == selection.size ( ))

	}
	catch (const Exception& exc)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant l'actualisation du panneau mesure de "
		      << "distance : " << exc.getFullMessage ( );
		log (ErrorLog (error));
	}
	catch (const exception& e)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant l'actualisation du panneau mesure de "
		      << "distance : " << e.what ( );
		log (ErrorLog (error));
	}
	catch (...)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur non documentée survenue durant l'actualisation du "
		      << "panneau mesure de distance.";
		log (ErrorLog (error));
	}
#else	// AUTO_UPDATE_OLD_SCHEME
	_entity1Panel->clearSelection ( );
	_entity2Panel->clearSelection ( );
#endif	// AUTO_UPDATE_OLD_SCHEME
}	// QtDistanceMeasurementOperationPanel::autoUpdate


const Entity* QtDistanceMeasurementOperationPanel::getEntity1 ( ) const
{
	CHECK_NULL_PTR_ERROR (_entity1Panel)
	vector<string>	entities	= _entity1Panel->getEntitiesNames ( );
	if (1 < entities.size ( ))
	{
		INTERNAL_ERROR (exc, "Le champ Point 1 contient plusieurs entités.", "QtDistanceMeasurementOperationPanel::getEntity1")
		throw exc;
	}	// if (1 < entities.size ( ))

	if (1 == entities.size ( ))
		return &getContext().nameToEntity (entities[0]);

	return 0;
}	// QtDistanceMeasurementOperationPanel::getEntity1


const Entity* QtDistanceMeasurementOperationPanel::getEntity2 ( ) const
{
	CHECK_NULL_PTR_ERROR (_entity2Panel)
	vector<string>	entities	= _entity2Panel->getEntitiesNames ( );
	if (1 < entities.size ( ))
	{
		INTERNAL_ERROR (exc, "Le champ Entité 2 contient plusieurs entités.", "QtDistanceMeasurementOperationPanel::getEntity2")
		throw exc;
	}	// if (1 < entities.size ( ))

	if (1 == entities.size ( ))
		return &getContext().nameToEntity (entities[0]);

	return 0;
}	// QtDistanceMeasurementOperationPanel::getEntity2


void QtDistanceMeasurementOperationPanel::updateDistance (double d)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_distanceLabel)
	UTF8String		text ("Distance : ", Charset::UTF_8);
	UTF8String		wholeText ("Distance", Charset::UTF_8);
	const bool		valid	= NumericServices::isValid (d);

	if (valid == NumericServices::isValid (d))
	{
		text << MgxNumeric::userRepresentation (d);
		const Entity*	entity1	= getEntity1 ( );
		const Entity*	entity2	= getEntity2 ( );
		if ((0 != entity1) && (0 != entity2))
			wholeText << " " << entity1->getName ( ) << "-"
			          << entity2->getName ( );
		wholeText << " : " << MgxNumeric::userRepresentation (d);
	}	// if (valid == NumericServices::isValid (d))

	ContextIfc&	context	= getContext ( );
	if (Unit::undefined != context.getLengthUnit ( ))
	{
		text << Unit::toShortString (context.getLengthUnit ( ));
		wholeText << Unit::toShortString (context.getLengthUnit ( ));
	}	// if (Unit::undefined != context.getLengthUnit ( ))

	_distanceLabel->setText (text.iso ( ).c_str ( ));
	if (true == valid)
		log (InformationLog (wholeText));

	COMPLETE_QT_TRY_CATCH_BLOCK (false, this, "Magix 3D")
}	// QtDistanceMeasurementOperationPanel::updateDistance


void QtDistanceMeasurementOperationPanel::selectionModifiedCallback ( )
{
	bool	dlg			= false;

	BEGIN_QT_TRY_CATCH_BLOCK

	highlight (false);	// "Eteint" l'éventuelle sélection en cours
	highlight (true);	// "Allume" la nouvelle sélection en cours

	if ((0 != getEntity1 ( )) && (0 != getEntity2 ( )))
		dlg	= true;

	double distance = Internal::EntitiesHelper::length(getEntity1(), getEntity2());

	updateDistance (distance);

	COMPLETE_QT_TRY_CATCH_BLOCK (dlg, this, "Magix 3D : mesure de la distance entre 2 entités")

	if (true == hasError)
		updateDistance (NumericServices::doubleMachInfinity());
}	// QtDistanceMeasurementOperationPanel::selectionModifiedCallback


void QtDistanceMeasurementOperationPanel::closeCallback ( )
{
	QWidget*	parent	= parentWidget ( );

	BEGIN_QT_TRY_CATCH_BLOCK

	deleteLater ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, parent, "Magix 3D : fermeture du panneau de mesure de distance")
}	// QtDistanceMeasurementOperationPanel::closeCallback


void QtDistanceMeasurementOperationPanel::helpCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtHelpWindow::displayURL (_helpURL, _helpTag );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : affichage de l'aide.")

} // QtDistanceMeasurementOperationPanel::helpCallback


// ===========================================================================
//                  LA CLASSE QtDistanceMeasurementOperationAction
// ===========================================================================

QtDistanceMeasurementOperationAction::QtDistanceMeasurementOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (icon, text, mainWindow, tooltip),
	  _mutex (false)
{
	QtDistanceMeasurementOperationPanel*	operationPanel	=
		new QtDistanceMeasurementOperationPanel (
			0, text.toStdString ( ),
			mainWindow, this,
			QtMgx3DApplication::HelpSystem::instance ( ).distanceMeasurementURL,
			QtMgx3DApplication::HelpSystem::instance ( ).distanceMeasurementTag);
	setWidget (operationPanel);
}	// QtDistanceMeasurementOperationAction::QtDistanceMeasurementOperationAction


QtDistanceMeasurementOperationAction::QtDistanceMeasurementOperationAction (
									const QtDistanceMeasurementOperationAction&)
	: QtMgx3DGeomOperationAction (
					QIcon (""), "", *new QtMgx3DMainWindow (0), ""),
	  _mutex (false)
{
	MGX_FORBIDDEN ("QtDistanceMeasurementOperationAction copy constructor is not allowed.")
}	// QtDistanceMeasurementOperationAction::QtDistanceMeasurementOperationAction


QtDistanceMeasurementOperationAction&
				QtDistanceMeasurementOperationAction::operator = (
									const QtDistanceMeasurementOperationAction&)
{
	MGX_FORBIDDEN ("QtDistanceMeasurementOperationAction assignment operator is not allowed.")
	return *this;
}	// QtDistanceMeasurementOperationAction::operator =


QtDistanceMeasurementOperationAction::~QtDistanceMeasurementOperationAction ( )
{
}	// QtDistanceMeasurementOperationAction::~QtDistanceMeasurementOperationAction


QtDistanceMeasurementOperationPanel*
		QtDistanceMeasurementOperationAction::getDistanceMeasurementPanel ( )
{
	return dynamic_cast<QtDistanceMeasurementOperationPanel*>(getOperationPanel ( ));
}	// QtDistanceMeasurementOperationAction::getDistanceMeasurementPanel


void QtDistanceMeasurementOperationAction::executeOperation ( )
{
	if (false == _mutex.tryLock ( ))
		return;

	try
	{
	    // Validation paramétrage :
	    QtMgx3DGeomOperationAction::executeOperation ( );


	    _mutex.unlock ( );

	}
	catch (...)
	{
		_mutex.unlock ( );
		throw;
		// unique_ptr => delete oldQuality
	}
}	// QtDistanceMeasurementOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
