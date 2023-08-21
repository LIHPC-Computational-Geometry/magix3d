/**
 * \file        QtAngleMeasurementOperationAction.cpp
 * \author		Eric Brière de l'Isle
 * \date		10/03/2017
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
#include "QtComponents/QtAngleMeasurementOperationAction.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtHelpWindow.h>
#include <QtUtil/QtUnicodeHelper.h>

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
//               LA CLASSE QtAngleMeasurementOperationPanel
// ===========================================================================

QtAngleMeasurementOperationPanel::QtAngleMeasurementOperationPanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (
			0/*parent*/, mainWindow, action, helpURL, helpTag),
			_entityCenterPanel(0), _entity1Panel (0), _entity2Panel (0), _angleLabel (0), _helpURL(helpURL), _helpTag(helpTag)
{
	setWindowTitle (panelName.c_str ( ));
	mainWindow.registerAdditionalOperationPanel (*this);

	QVBoxLayout*	vlayout	= new QVBoxLayout (0);
	vlayout->setSpacing (QtConfiguration::spacing);
	vlayout->setContentsMargins (
					QtConfiguration::margin, QtConfiguration::margin,
					QtConfiguration::margin, QtConfiguration::margin);
	setLayout (vlayout);

	const FilterEntity::objectType	filter				=
		(FilterEntity::objectType)(
				FilterEntity::GeomVertex | FilterEntity::TopoVertex);

	_entityCenterPanel  = new QtMgx3DEntityPanel (this, "", true, "Centre (Vertex/Sommet) :", "",
			&mainWindow, SelectionManagerIfc::D0, filter);
	vlayout->addWidget (_entityCenterPanel);
	connect (_entityCenterPanel, SIGNAL (selectionModified (QString)),
		         this, SLOT (selectionModifiedCallback ( )));

	_entity1Panel  = new QtMgx3DEntityPanel (this, "", true, "Point 1 (Vertex/Sommet) :", "",
			&mainWindow, SelectionManagerIfc::D0, filter);
	vlayout->addWidget (_entity1Panel);
	connect (_entity1Panel, SIGNAL (selectionModified (QString)),
		         this, SLOT (selectionModifiedCallback ( )));

	_entity2Panel  = new QtMgx3DEntityPanel (this, "", true, "Point 2 (Vertex/Sommet) :", "",
			&mainWindow, SelectionManagerIfc::D0, filter);
	vlayout->addWidget (_entity2Panel);
	connect (_entity2Panel, SIGNAL (selectionModified (QString)),
		         this, SLOT (selectionModifiedCallback ( )));

	// Le résultat :
	_angleLabel	= new QLabel ("Angle : ", this);
	_angleLabel->setTextInteractionFlags (
				Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
	vlayout->addWidget (_angleLabel);

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
}	// QtAngleMeasurementOperationPanel::QtAngleMeasurementOperationPanel


QtAngleMeasurementOperationPanel::QtAngleMeasurementOperationPanel (
								const QtAngleMeasurementOperationPanel& dmop)
	: QtMgx3DOperationPanel (
				0,
				*new QtMgx3DMainWindow (0),
				0, "", ""),
				_entityCenterPanel(0), _entity1Panel (0), _entity2Panel (0), _angleLabel (0)
{
	MGX_FORBIDDEN ("QtAngleMeasurementOperationPanel copy constructor is not allowed.");
}	// QtAngleMeasurementOperationPanel::QtAngleMeasurementOperationPanel (const QtAngleMeasurementOperationPanel&)


QtAngleMeasurementOperationPanel&
					QtAngleMeasurementOperationPanel::operator = (
									const QtAngleMeasurementOperationPanel&)
{
	MGX_FORBIDDEN ("QtAngleMeasurementOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtAngleMeasurementOperationPanel::QtAngleMeasurementOperationPanel (const QtAngleMeasurementOperationPanel&)


QtAngleMeasurementOperationPanel::~QtAngleMeasurementOperationPanel ( )
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
		error << "Erreur survenue durant la fermeture du panneau mesure d'"
		      << "angle : " << exc.getFullMessage ( );
		log (ErrorLog (error));
	}
	catch (const exception& e)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur survenue durant la fermeture du panneau mesure d'"
		      << "angle : " << e.what ( );
		log (ErrorLog (error));
	}
	catch (...)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur non documentée survenue durant la fermeture du "
		      << "panneau mesure d'angle.";
		log (ErrorLog (error));
	}
}	// QtAngleMeasurementOperationPanel::~QtAngleMeasurementOperationPanel


vector<Entity*> QtAngleMeasurementOperationPanel::getInvolvedEntities ( )
{
	CHECK_NULL_PTR_ERROR (_entityCenterPanel)
	CHECK_NULL_PTR_ERROR (_entity1Panel)
	CHECK_NULL_PTR_ERROR (_entity2Panel)
	Entity*	entity0	= (Entity*)getEntityCenter ( );
	Entity*	entity1	= (Entity*)getEntity1 ( );
	Entity*	entity2	= (Entity*)getEntity2 ( );

	vector<Entity*>	entities;
	if (0 != entity0)
		entities.push_back (entity0);
	if (0 != entity1)
		entities.push_back (entity1);
	if (0 != entity2)
		entities.push_back (entity2);

	return entities;
}	// QtAngleMeasurementOperationPanel::getInvolvedEntities


void QtAngleMeasurementOperationPanel::setVisible (bool visible)
{
	QWidget::setVisible (visible);
}	// QtAngleMeasurementOperationPanel::void setVisible


void QtAngleMeasurementOperationPanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entity1Panel)
	CHECK_NULL_PTR_ERROR (_entity2Panel)
	CHECK_NULL_PTR_ERROR (_entityCenterPanel)
	_entity1Panel->reset ( );
	_entity2Panel->reset ( );
	_entityCenterPanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtAngleMeasurementOperationPanel::reset


void QtAngleMeasurementOperationPanel::commandModifiedCallback (
												InfoCommand& infoCommand)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_entity1Panel)
	CHECK_NULL_PTR_ERROR (_entity2Panel)
	CHECK_NULL_PTR_ERROR (_entityCenterPanel)

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
		if ((ge == getEntityCenter ( )) || (ge == getEntity1 ( )) || (ge == getEntity2 ( )))
		{
			if (InfoCommand::DELETED == t)
			{
				QtObjectSignalBlocker	blocker0 (_entityCenterPanel);
				QtObjectSignalBlocker	blocker1 (_entity1Panel);
				QtObjectSignalBlocker	blocker2 (_entity2Panel);
				if (ge == getEntityCenter ( ))
				{
					_entityCenterPanel->clearSelection ( );
					reinited	= true;
				}
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
}	// QtAngleMeasurementOperationPanel::commandModifiedCallback


void QtAngleMeasurementOperationPanel::validate ( )
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
		error << "QtAngleMeasurementOperationPanel::validate : erreur non documentée.";
	}
//	VALIDATED_FIELD_EXPRESSION_EVALUATION (_namePanel->validate ( ), "Nom : ", error)

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtAngleMeasurementOperationPanel::validate


void QtAngleMeasurementOperationPanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );	// => highlight (false)

	CHECK_NULL_PTR_ERROR (_entityCenterPanel)
	CHECK_NULL_PTR_ERROR (_entity1Panel)
	CHECK_NULL_PTR_ERROR (_entity2Panel)
	CHECK_NULL_PTR_ERROR (_angleLabel)
	_entityCenterPanel->stopSelection ( );
	_entity1Panel->stopSelection ( );
	_entity2Panel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_entityCenterPanel->clearSelection ( );
		_entity1Panel->clearSelection ( );
		_entity2Panel->clearSelection ( );
		updateAngle (NumericServices::doubleMachInfinity());

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtAngleMeasurementOperationPanel::cancel


void QtAngleMeasurementOperationPanel::autoUpdate ( )
{
	highlight (false);

	CHECK_NULL_PTR_ERROR (_entityCenterPanel)
	CHECK_NULL_PTR_ERROR (_entity1Panel)
	CHECK_NULL_PTR_ERROR (_entity2Panel)
	_entityCenterPanel->clearSelection ( );
	_entity1Panel->clearSelection ( );
	_entity2Panel->clearSelection ( );
}	// QtAngleMeasurementOperationPanel::autoUpdate


const Entity* QtAngleMeasurementOperationPanel::getEntityCenter ( ) const
{
	CHECK_NULL_PTR_ERROR (_entityCenterPanel)
	string	entity	= _entityCenterPanel->getUniqueName ( );
	return &getContext().nameToEntity (entity);
}	// QtAngleMeasurementOperationPanel::getEntityCenter


const Entity* QtAngleMeasurementOperationPanel::getEntity1 ( ) const
{
	CHECK_NULL_PTR_ERROR (_entity1Panel)
	string	entity	= _entity1Panel->getUniqueName ( );
	return &getContext().nameToEntity (entity);
}	// QtAngleMeasurementOperationPanel::getEntity1


const Entity* QtAngleMeasurementOperationPanel::getEntity2 ( ) const
{
	CHECK_NULL_PTR_ERROR (_entity2Panel)
	string	entity	= _entity2Panel->getUniqueName ( );
	return &getContext().nameToEntity (entity);
}	// QtAngleMeasurementOperationPanel::getEntity2


void QtAngleMeasurementOperationPanel::updateAngle (double d)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_angleLabel)
	UTF8String		text ("Angle : ", Charset::UTF_8);
	UTF8String		wholeText ("Angle", Charset::UTF_8);
	const bool		valid	= NumericServices::isValid (d);

	if (valid == NumericServices::isValid (d))
	{
		text << MgxNumeric::userRepresentation (d);
		const Entity*	entity1	= getEntity1 ( );
		const Entity*	entity2	= getEntity2 ( );
		const Entity*	entityC	= getEntityCenter ( );
		if ((0 != entity1) && (0 != entity2))
			wholeText << " " << entity1->getName ( )
			<< "-" << entityC->getName ( )
			<< "-" << entity2->getName ( );
		wholeText << " : " << MgxNumeric::userRepresentation (d) << " degrés";
	}	// if (valid == NumericServices::isValid (d))

	_angleLabel->setText (UTF8TOQSTRING (text));
	if (true == valid)
		log (InformationLog (wholeText));

	COMPLETE_QT_TRY_CATCH_BLOCK (false, this, "Magix 3D")
}	// QtAngleMeasurementOperationPanel::updateAngle


void QtAngleMeasurementOperationPanel::selectionModifiedCallback ( )
{
	bool	dlg			= false;

	BEGIN_QT_TRY_CATCH_BLOCK

	highlight (false);	// "Eteint" l'éventuelle sélection en cours
	highlight (true);	// "Allume" la nouvelle sélection en cours

	if ((0 != getEntityCenter ( )) &&(0 != getEntity1 ( )) && (0 != getEntity2 ( )))
		dlg	= true;

	double angle = Internal::EntitiesHelper::angle(getEntityCenter(), getEntity1(), getEntity2());

	updateAngle (angle);

	COMPLETE_QT_TRY_CATCH_BLOCK (dlg, this, "Magix 3D : mesure de l'angle entre 3 entités")

	if (true == hasError)
		updateAngle (NumericServices::doubleMachInfinity());
}	// QtAngleMeasurementOperationPanel::selectionModifiedCallback


void QtAngleMeasurementOperationPanel::closeCallback ( )
{
	QWidget*	parent	= parentWidget ( );

	BEGIN_QT_TRY_CATCH_BLOCK

	deleteLater ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, parent, "Magix 3D : fermeture du panneau de mesure d'angle")
}	// QtAngleMeasurementOperationPanel::closeCallback


void QtAngleMeasurementOperationPanel::helpCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtHelpWindow::displayURL (_helpURL, _helpTag );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : affichage de l'aide.")

} // QtAngleMeasurementOperationPanel::helpCallback


// ===========================================================================
//                  LA CLASSE QtAngleMeasurementOperationAction
// ===========================================================================

QtAngleMeasurementOperationAction::QtAngleMeasurementOperationAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DGeomOperationAction (
						icon, text, mainWindow, tooltip),
	  _mutex (false)
{
	QtAngleMeasurementOperationPanel*	operationPanel	=
		new QtAngleMeasurementOperationPanel (
			0, text.toStdString ( ),
			mainWindow, this,
			QtMgx3DApplication::HelpSystem::instance ( ).angleMeasurementURL,
			QtMgx3DApplication::HelpSystem::instance ( ).angleMeasurementTag);
	setWidget (operationPanel);
}	// QtAngleMeasurementOperationAction::QtAngleMeasurementOperationAction


QtAngleMeasurementOperationAction::QtAngleMeasurementOperationAction (
									const QtAngleMeasurementOperationAction&)
	: QtMgx3DGeomOperationAction (
					QIcon (""), "", *new QtMgx3DMainWindow (0), ""),
	  _mutex (false)
{
	MGX_FORBIDDEN ("QtAngleMeasurementOperationAction copy constructor is not allowed.")
}	// QtAngleMeasurementOperationAction::QtAngleMeasurementOperationAction


QtAngleMeasurementOperationAction&
				QtAngleMeasurementOperationAction::operator = (
									const QtAngleMeasurementOperationAction&)
{
	MGX_FORBIDDEN ("QtAngleMeasurementOperationAction assignment operator is not allowed.")
	return *this;
}	// QtAngleMeasurementOperationAction::operator =


QtAngleMeasurementOperationAction::~QtAngleMeasurementOperationAction ( )
{
}	// QtAngleMeasurementOperationAction::~QtAngleMeasurementOperationAction


QtAngleMeasurementOperationPanel*
		QtAngleMeasurementOperationAction::getAngleMeasurementPanel ( )
{
	return dynamic_cast<QtAngleMeasurementOperationPanel*>(getOperationPanel ( ));
}	// QtAngleMeasurementOperationAction::getAngleMeasurementPanel


void QtAngleMeasurementOperationAction::executeOperation ( )
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
}	// QtAngleMeasurementOperationAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
