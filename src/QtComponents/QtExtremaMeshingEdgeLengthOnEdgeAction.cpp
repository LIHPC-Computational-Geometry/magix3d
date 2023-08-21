/**
 * \file        QtExtremaMeshingEdgeLengthOnEdgeAction.cpp
 * \author		Eric BRIERE DE L'ISLE
 * \date		05/12/2016
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Internal/EntitiesHelper.h"
#include "Internal/InfoCommand.h"
#include "Topo/CoEdge.h"
#include "Topo/TopoDisplayRepresentation.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtExtremaMeshingEdgeLengthOnEdgeAction.h"
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
#include <TkUtil/TraceLog.h>

#include <QPushButton>
#include <QVBoxLayout>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//               LA CLASSE QtExtremaMeshingEdgeLengthOnEdgePanel
// ===========================================================================

QtExtremaMeshingEdgeLengthOnEdgePanel::QtExtremaMeshingEdgeLengthOnEdgePanel (
			QWidget* parent, const string& panelName, 
			QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action,
			const string& helpURL, const string& helpTag)
	: QtMgx3DOperationPanel (
			0/*parent*/, mainWindow, action, helpURL, helpTag),
			_coedgePanel (0), _startLengthLabel(0), _endLengthLabel(0), _helpURL(helpURL), _helpTag(helpTag)
{
	setWindowTitle (panelName.c_str ( ));
	mainWindow.registerAdditionalOperationPanel (*this);

	QVBoxLayout*	vlayout	= new QVBoxLayout (0);
	vlayout->setSpacing (QtConfiguration::spacing);
	vlayout->setContentsMargins (
					QtConfiguration::margin, QtConfiguration::margin,
					QtConfiguration::margin, QtConfiguration::margin);

	setLayout (vlayout);

	_coedgePanel	= new QtMgx3DEntityPanel (
			this, "", true, "Arête :", "", &mainWindow,
			SelectionManagerIfc::D1, FilterEntity::TopoCoEdge);
	_coedgePanel->setToolTip (QString::fromUtf8("Arête dont on souhaite connaitre la longueur des bras aux extrémités."));

	vlayout->addWidget (_coedgePanel);
	connect (_coedgePanel, SIGNAL (selectionModified (QString)),
	         this, SLOT (selectionModifiedCallback ( )));

	// Le résultat :
	_startLengthLabel = new QLabel (QString::fromUtf8("Longueur du bras au départ : "), this);
	_endLengthLabel = new QLabel (QString::fromUtf8("Longueur du bras à l'arrivée : "), this);
	_startLengthLabel->setTextInteractionFlags (
					Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
	_endLengthLabel->setTextInteractionFlags (
					Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
	vlayout->addWidget (_startLengthLabel);
	vlayout->addWidget (_endLengthLabel);

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
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::QtExtremaMeshingEdgeLengthOnEdgePanel


QtExtremaMeshingEdgeLengthOnEdgePanel::QtExtremaMeshingEdgeLengthOnEdgePanel (
								const QtExtremaMeshingEdgeLengthOnEdgePanel& dmop)
	: QtMgx3DOperationPanel (
				0,
				*new QtMgx3DMainWindow (0),
				0, "", ""),
				_coedgePanel (0), _startLengthLabel(0), _endLengthLabel(0)
{
	MGX_FORBIDDEN ("QtExtremaMeshingEdgeLengthOnEdgePanel copy constructor is not allowed.");
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::QtExtremaMeshingEdgeLengthOnEdgePanel (const QtExtremaMeshingEdgeLengthOnEdgePanel&)


QtExtremaMeshingEdgeLengthOnEdgePanel&
					QtExtremaMeshingEdgeLengthOnEdgePanel::operator = (
									const QtExtremaMeshingEdgeLengthOnEdgePanel&)
{
	MGX_FORBIDDEN ("QtExtremaMeshingEdgeLengthOnEdgePanel assignment operator is not allowed.");
	return *this;
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::QtExtremaMeshingEdgeLengthOnEdgePanel (const QtExtremaMeshingEdgeLengthOnEdgePanel&)


QtExtremaMeshingEdgeLengthOnEdgePanel::~QtExtremaMeshingEdgeLengthOnEdgePanel ( )
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
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::~QtExtremaMeshingEdgeLengthOnEdgePanel


vector<Entity*> QtExtremaMeshingEdgeLengthOnEdgePanel::getInvolvedEntities ( )
{
	CHECK_NULL_PTR_ERROR (_coedgePanel)
    Topo::CoEdge* coedge = (Topo::CoEdge*)getCoEdge ( );

	vector<Entity*>	entities;
	if (0 != coedge)
		entities.push_back (coedge);

	return entities;
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::getInvolvedEntities


void QtExtremaMeshingEdgeLengthOnEdgePanel::setVisible (bool visible)
{
	QWidget::setVisible (visible);
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::void setVisible


void QtExtremaMeshingEdgeLengthOnEdgePanel::commandModifiedCallback (
												InfoCommand& infoCommand)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_coedgePanel)

	bool	reinited	= false;

	std::map<Topo::TopoEntity*, InfoCommand::type> tei = infoCommand.getTopoInfoEntity();
	const Topo::CoEdge* coedge = getCoEdge();
	for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::const_iterator iter = tei.begin();
			(false == reinited) && (iter != tei.end()); ++iter) {
		Topo::TopoEntity* te = iter->first;
		InfoCommand::type  t = iter->second;
		if (te == coedge)
		{
			if (InfoCommand::DELETED == t)
			{
				QtObjectSignalBlocker	blocker1 (_coedgePanel);
				_coedgePanel->clearSelection ( );
				reinited	= true;
			}	// if (InfoCommand::DELETED == t)
			else if (InfoCommand::DISPMODIFIED == t)
			{
				reinited	= true;
			}
		}	// if (te == coedge)
	}	// for (iter = tei.begin() ...)

	if (true == reinited)
		selectionModifiedCallback ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::commandModifiedCallback


void QtExtremaMeshingEdgeLengthOnEdgePanel::reset ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_coedgePanel)
	_coedgePanel->reset ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")

	QtMgx3DOperationPanel::reset ( );
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::reset


void QtExtremaMeshingEdgeLengthOnEdgePanel::validate ( )
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
		error << "QtExtremaMeshingEdgeLengthOnEdgePanel::validate : erreur non documentée.";
	}

	if (0 != error.length ( ))
		throw Exception (error);
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::validate


void QtExtremaMeshingEdgeLengthOnEdgePanel::cancel ( )
{
	QtMgx3DOperationPanel::cancel ( );	// => highlight (false)

	CHECK_NULL_PTR_ERROR (_coedgePanel)
	CHECK_NULL_PTR_ERROR (_startLengthLabel)
	CHECK_NULL_PTR_ERROR (_endLengthLabel)
	_coedgePanel->stopSelection ( );

	if (true == cancelClearEntities ( ))
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		_coedgePanel->clearSelection ( );
		updateLengths (NumericServices::doubleMachInfinity(),
				NumericServices::doubleMachInfinity());

		COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D")
	}	// if (true == cancelClearEntities ( ))
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::cancel


void QtExtremaMeshingEdgeLengthOnEdgePanel::autoUpdate ( )
{
	highlight (false);	// car champs de texte modifiés

	CHECK_NULL_PTR_ERROR (_coedgePanel)
    _coedgePanel->clearSelection ( );
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::autoUpdate


const Topo::CoEdge* QtExtremaMeshingEdgeLengthOnEdgePanel::getCoEdge ( ) const
{
	CHECK_NULL_PTR_ERROR (_coedgePanel)
	string	entitie	= _coedgePanel->getUniqueName ( );

	return (Topo::CoEdge*)&getContext().nameToEntity (entitie);

}	// QtExtremaMeshingEdgeLengthOnEdgePanel::getCoEdge



void QtExtremaMeshingEdgeLengthOnEdgePanel::updateLengths (double d1, double d2)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_startLengthLabel)
	CHECK_NULL_PTR_ERROR (_endLengthLabel)
	UTF8String		text1 ("Longueur du bras au départ : ", Charset::UTF_8);
	UTF8String		text2 ("Longueur du bras à l'arrivée : ", Charset::UTF_8);
	const bool		valid	= NumericServices::isValid (d1) && NumericServices::isValid (d2);

	if (valid) {
		text1 << MgxNumeric::userRepresentation (d1);
		text2 << MgxNumeric::userRepresentation (d2);
	}	// if (valid)

	ContextIfc&	context	= getContext ( );
	if (Unit::undefined != context.getLengthUnit ( )) {
		text1 << Unit::toShortString (context.getLengthUnit ( ));
		text2 << Unit::toShortString (context.getLengthUnit ( ));
	}	// if (Unit::undefined != context.getLengthUnit ( ))

	_startLengthLabel->setText (UTF8TOQSTRING (text1));
	_endLengthLabel->setText (UTF8TOQSTRING (text2));

	COMPLETE_QT_TRY_CATCH_BLOCK (false, this, "Magix 3D")
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::updateLengths


void QtExtremaMeshingEdgeLengthOnEdgePanel::selectionModifiedCallback ( )
{
	bool	dlg			= false;

	BEGIN_QT_TRY_CATCH_BLOCK

	viewDirection(false, true);

	highlight (false);	// "Eteint" l'éventuelle sélection en cours
	highlight (true);	// "Allume" la nouvelle sélection en cours

	Topo::CoEdge* coedge = (Topo::CoEdge*)getCoEdge();
	if (0 != coedge)
		dlg	= true;

	std::vector<Utils::Math::Point> points;
	if (coedge)
		coedge->getPoints(points);

	double d1 = 0;
	double d2 = 0;
	uint nb = points.size();
	if (nb>=2){
		d1 = points[0].length(points[1]);
		d2 = points[nb-1].length(points[nb-2]);
	}

	updateLengths (d1, d2);

	if (coedge)
		viewDirection(true, false);
	else
		viewDirection(false, true);

	COMPLETE_QT_TRY_CATCH_BLOCK (dlg, this, "Magix 3D : mesure des bras aux extrémités")

	if (true == hasError)
		updateLengths (NumericServices::doubleMachInfinity(),
				NumericServices::doubleMachInfinity());

}	// QtExtremaMeshingEdgeLengthOnEdgePanel::selectionModifiedCallback


void QtExtremaMeshingEdgeLengthOnEdgePanel::closeCallback ( )
{
	QWidget*	parent	= parentWidget ( );

	BEGIN_QT_TRY_CATCH_BLOCK

	viewDirection(false, true);

	deleteLater ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, parent, "Magix 3D : fermeture du panneau de mesure des bras aux extrémités")
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::closeCallback


void QtExtremaMeshingEdgeLengthOnEdgePanel::helpCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtHelpWindow::displayURL (_helpURL, _helpTag );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : affichage de l'aide.")

} // QtExtremaMeshingEdgeLengthOnEdgePanel::helpCallback


void QtExtremaMeshingEdgeLengthOnEdgePanel::viewDirection (bool show, bool destroyInteractor)
{
	//std::cout<<"QtExtremaMeshingEdgeLengthOnEdgePanel::viewDirection("<<show<<", "<<destroyInteractor<<")"<<std::endl;

	QtMgx3DOperationPanel::preview (show, destroyInteractor);

	if (false == show)
		return;

	bool edge_direction_activated = false;

	BEGIN_QT_TRY_CATCH_BLOCK

	const Topo::CoEdge*			coedge	= getCoEdge( );
	if (coedge != 0){
		edge_direction_activated = true;

		Topo::TopoDisplayRepresentation   tr (DisplayRepresentation::SHOWDISCRETISATIONTYPE);

		coedge->getRepresentation (tr, true);
		vector<Math::Point>&    points      = tr.getPoints ( );
		vector<size_t>&         segments   = tr.getCurveDiscretization ( );

		DisplayProperties	graphicalProps;
		graphicalProps.setWireColor (Color (
				255 * Resources::instance ( )._previewColor.getRed ( ),
				255 * Resources::instance ( )._previewColor.getGreen ( ),
				255 * Resources::instance ( )._previewColor.getBlue ( )));
		graphicalProps.setLineWidth (
				Resources::instance ( )._previewWidth.getValue ( ));


		RenderingManager::RepresentationID	edgeID	=
				getRenderingManager ( ).createSegmentsWireRepresentation (
						points, segments, graphicalProps, true);
		registerPreviewedObject (edgeID);

		getRenderingManager ( ).forceRender ( );
	}

	COMPLETE_QT_TRY_CATCH_BLOCK (false, this, "Magix 3D")

	if (hasError)
		getContext ( ).getLogStream()->log(TkUtil::TraceLog (errorString, TkUtil::Log::WARNING));
	else if (edge_direction_activated)
		getContext ( ).getLogStream()->log(TkUtil::TraceLog (UTF8String ("Prévisualisation du sens de la discrétisation ...", Charset::UTF_8), TkUtil::Log::TRACE_1));
}	// QtExtremaMeshingEdgeLengthOnEdgePanel::viewDirection


// ===========================================================================
//                  LA CLASSE QtExtremaMeshingEdgeLengthOnEdgeAction
// ===========================================================================

QtExtremaMeshingEdgeLengthOnEdgeAction::QtExtremaMeshingEdgeLengthOnEdgeAction (
	const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow,
	const QString& tooltip)
	: QtMgx3DOperationAction (icon, text, mainWindow, tooltip),
	  _mutex (false)
{
	QtExtremaMeshingEdgeLengthOnEdgePanel*	operationPanel	=
		new QtExtremaMeshingEdgeLengthOnEdgePanel (
			0, text.toStdString ( ),
			mainWindow, this,
			QtMgx3DApplication::HelpSystem::instance ( ).extremaMeshingEdgeLengthURL,
			QtMgx3DApplication::HelpSystem::instance ( ).extremaMeshingEdgeLengthTag);
	setWidget (operationPanel);
}	// QtExtremaMeshingEdgeLengthOnEdgeAction::QtExtremaMeshingEdgeLengthOnEdgeAction


QtExtremaMeshingEdgeLengthOnEdgeAction::QtExtremaMeshingEdgeLengthOnEdgeAction (
									const QtExtremaMeshingEdgeLengthOnEdgeAction&)
	: QtMgx3DOperationAction (QIcon (""), "", *new QtMgx3DMainWindow (0), ""),
	  _mutex (false)
{
	MGX_FORBIDDEN ("QtExtremaMeshingEdgeLengthOnEdgeAction copy constructor is not allowed.")
}	// QtExtremaMeshingEdgeLengthOnEdgeAction::QtExtremaMeshingEdgeLengthOnEdgeAction


QtExtremaMeshingEdgeLengthOnEdgeAction&
				QtExtremaMeshingEdgeLengthOnEdgeAction::operator = (
									const QtExtremaMeshingEdgeLengthOnEdgeAction&)
{
	MGX_FORBIDDEN ("QtExtremaMeshingEdgeLengthOnEdgeAction assignment operator is not allowed.")
	return *this;
}	// QtExtremaMeshingEdgeLengthOnEdgeAction::operator =


QtExtremaMeshingEdgeLengthOnEdgeAction::~QtExtremaMeshingEdgeLengthOnEdgeAction ( )
{
}	// QtExtremaMeshingEdgeLengthOnEdgeAction::~QtExtremaMeshingEdgeLengthOnEdgeAction


QtExtremaMeshingEdgeLengthOnEdgePanel*
		QtExtremaMeshingEdgeLengthOnEdgeAction::getExtremaMeshingEdgeLengthPanel ( )
{
	return dynamic_cast<QtExtremaMeshingEdgeLengthOnEdgePanel*>(getOperationPanel ( ));
}	// QtExtremaMeshingEdgeLengthOnEdgeAction::getExtremaMeshingEdgeLengthPanel


void QtExtremaMeshingEdgeLengthOnEdgeAction::executeOperation ( )
{
	if (false == _mutex.tryLock ( ))
		return;

	try
	{
	    // Validation paramétrage :
		QtMgx3DOperationAction::executeOperation ( );


	    _mutex.unlock ( );

	}
	catch (...)
	{
		_mutex.unlock ( );
		throw;
		// unique_ptr => delete oldQuality
	}
}	// QtExtremaMeshingEdgeLengthOnEdgeAction::executeOperation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
