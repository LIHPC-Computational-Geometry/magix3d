/**
 * \file        QtMgx3DOperationsPanel.cpp
 * \author      Charles PIGNEROL
 * \date        05/12/2012
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/RenderedEntityRepresentation.h"
#include "QtComponents/QtMgx3DScrollArea.h"
#include "Topo/CoEdge.h"
#include "Topo/TopoDisplayRepresentation.h"
#include "Utils/Common.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtHelpWindow.h>
#include <QtUtil/QtFlowLayout.h>

#include <QVBoxLayout>
#include <QWidgetAction>

#include <iostream>
#include <memory>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::Topo;



namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                     LA CLASSE QtMgx3DOperationPanel
// ===========================================================================


QtMgx3DOperationPanel::QtMgx3DOperationPanel (QWidget* parent, QtMgx3DMainWindow& mainWindow, QtMgx3DOperationAction* action, const string& helpURL, const string& helpTag)
	: QWidget (parent),
	  _previewCheckBox (0), _mainWindow (&mainWindow), _validatedFields ( ),
	  _action (action),
	  _highlightedEntities ( ), _previewIDs ( ), _interactors ( ),
	  _helpURL (helpURL), _helpTag (helpTag)
{
}	// QtMgx3DOperationPanel::QtMgx3DOperationPanel


QtMgx3DOperationPanel::QtMgx3DOperationPanel (const QtMgx3DOperationPanel&)
	: QWidget (0),
	  _previewCheckBox (0), _mainWindow (0), _validatedFields ( ), _action (0),
	  _highlightedEntities ( ), _previewIDs ( ), _interactors ( ),
	  _helpURL ( ), _helpTag ( )
{
	MGX_FORBIDDEN ("QtMgx3DOperationPanel copy constructor is not allowed.");
}	// QtMgx3DOperationPanel::QtMgx3DOperationPanel


QtMgx3DOperationPanel& QtMgx3DOperationPanel::operator = (const QtMgx3DOperationPanel&)
{
	MGX_FORBIDDEN ("QtMgx3DOperationPanel assignment operator is not allowed.");
	return *this;
}	// QtMgx3DOperationPanel::QtMgx3DOperationPanel


QtMgx3DOperationPanel::~QtMgx3DOperationPanel ( )
{
	try
	{
		highlight (false);
		preview (false, true);
	}
	catch (...)
	{
	}
}	// QtMgx3DOperationPanel::~QtMgx3DOperationPanel


void QtMgx3DOperationPanel::setVisible (bool visible)
{
	try
	{

		if (false == visible)
			cancel ( );
		else
			autoUpdate ( );

	}
	catch (const Exception& exc)
	{
// TODO [CP] Panneau affichant l'erreur
cerr << "Erreur rencontrée dans QtMgx3DOperationPanel::setVisible :" << endl
     << exc.getFullMessage ( ) << "/a" << endl;
	}
	catch (...)
	{
cerr << "Erreur non documentée rencontrée dans "
    << "QtMgx3DOperationPanel::setVisible./a" << endl;
	}

	QWidget::setVisible (visible);
}	// QtMgx3DOperationPanel::void setVisible


void QtMgx3DOperationPanel::reset ( )
{
	try
	{
		preview (false, true);
	}
	catch (...)
	{
	}
	try
	{
		highlight (false);
	}
	catch (...)
	{
	}
}	// QtMgx3DOperationPanel::reset


void QtMgx3DOperationPanel::validate ( )
{
	UTF8String	errors (Charset::UTF_8);
	
	for (vector<ValidatedField*>::iterator	it = _validatedFields.begin ( );
	     _validatedFields.end ( ) != it; it++)
	{
		try
		{
			CHECK_NULL_PTR_ERROR (*it)
			(*it)->validate ( );
		}
		catch (const Exception& exc)
		{
			if (0 == errors.length ( ))
				errors << "\n";
			errors << exc.getFullMessage ( );
		}
		catch (const exception& e)
		{
			if (0 == errors.length ( ))
				errors << "\n";
			errors << e.what ( );
		}
		catch (...)
		{
			if (0 == errors.length ( ))
				errors << "\n";
			errors << "Exception imprévue dans QtMgx3DOperationPanel::validate";
		}
	}	// for (vector<ValidatedField*>::iterator  it = ...

	if (0 != errors.length ( ))
		throw Exception (errors);
}	// QtMgx3DOperationPanel::validate


void QtMgx3DOperationPanel::cancel ( )
{
	highlight (false);
	preview (false, true);
}	// QtMgx3DOperationPanel::cancel


void QtMgx3DOperationPanel::autoUpdate ( )
{
	highlight (true);
	preview (true, true);
}	// QtMgx3DOperationPanel::autoUpdate


bool QtMgx3DOperationPanel::previewResult ( ) const
{
	if (0 == _previewCheckBox)
		return false;

	return Qt::Checked == _previewCheckBox->checkState ( ) ? true : false;
}	// QtMgx3DOperationPanel::previewResult


bool QtMgx3DOperationPanel::cancelClearEntities ( ) const
{
	return Resources::instance ( )._cancelClearEntities.getValue ( );
}	// QtMgx3DOperationPanel::cancelClearEntities


bool QtMgx3DOperationPanel::autoUpdateUsesSelection ( ) const
{
	return Resources::instance ( )._autoUpdateUsesSelection.getValue ( );
}	// QtMgx3DOperationPanel::autoUpdateUsesSelection


QtMgx3DOperationAction* QtMgx3DOperationPanel::getMgx3DOperationAction ( )
{
	return _action;
}	// QtMgx3DOperationPanel::getMgx3DOperationAction


string QtMgx3DOperationPanel::helpURL ( ) const
{
	return _helpURL;
}	// QtMgx3DOperationPanel::helpURL


string QtMgx3DOperationPanel::helpTag ( ) const
{
	return _helpTag;
}	// QtMgx3DOperationPanel::helpTag


QtMgx3DMainWindow* QtMgx3DOperationPanel::getMainWindow ( )
{
	return _mainWindow;
}	// QtMgx3DOperationPanel::getMainWindow


const QtMgx3DMainWindow* QtMgx3DOperationPanel::getMainWindow ( ) const
{
	return _mainWindow;
}	// QtMgx3DOperationPanel::getMainWindow


void QtMgx3DOperationPanel::setMainWindow (QtMgx3DMainWindow* window)
{
	_mainWindow	= window;
}	// QtMgx3DOperationPanel::setMainWindow


void QtMgx3DOperationPanel::commandModifiedCallback (InfoCommand&)
{
}	// QtMgx3DOperationPanel::commandModifiedCallback


void QtMgx3DOperationPanel::preview (bool show, bool destroyInteractor)
{	// Lors de la construction getGraphicalWidget peut être nul ...
	try
	{
		getRenderingManager ( );
	}
	catch (...)
	{
		return;
	}

	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (getMainWindow ( ))
	RenderingManager&	renderingManager	= getRenderingManager ( );

	// N'afficher une boite de dialogue d'erreur que si demandé ...
	QT_PREVIEW_BEGIN_TRY_CATCH_BLOCK

	if (false == show)
	{
		if (true == destroyInteractor)
		{
			vector<RenderingManager::Interactor*>	interactors	= _interactors;
			_interactors.clear ( );
			for (vector<RenderingManager::Interactor*>::iterator it =
				interactors.begin ( ); interactors.end ( ) != it; it++)
				 renderingManager.destroyInteractor (*it);
		}	// if (true == destroyInteractor)

		removePreviewedObjects ( );
	}	// if (false == show)
	else
	{
	}	// else if (false == show)

	QT_PREVIEW_COMPLETE_TRY_CATCH_BLOCK

	renderingManager.forceRender ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : Affichage d'un aperçu des entités associées à l'opération en cours.")
}	// QtMgx3DOperationPanel::preview


void QtMgx3DOperationPanel::highlight (bool show)
{	// Lors de la construction getGraphicalWidget peut être nul ...
	try
	{
		getRenderingManager ( );
	}
	catch (...)
	{
		return;
	}

	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (getMainWindow ( ))
	RenderingManager&	renderingManager	= getRenderingManager ( );

	// N'afficher une boite de dialogue d'erreur que si demandé ...
	QT_PREVIEW_BEGIN_TRY_CATCH_BLOCK

	if (false == show)
	{
		const vector<Entity*>	entities	= _highlightedEntities;
		_highlightedEntities.clear ( );
		for (vector<Entity*>::const_iterator it = entities.begin ( );
		     entities.end ( ) != it; it++)
		{
			DisplayProperties::GraphicalRepresentation*	gr	=
				(*it)->getDisplayProperties ( ).getGraphicalRepresentation ( );
			CHECK_NULL_PTR_ERROR (gr)
			gr->setHighlighted (false);
		}	// for (vector<Entity*>::iterator it = ...
	}	// if (false == show)
	else
	{
		const vector<Entity*>	entities	= getInvolvedEntities ( );
		for (vector<Entity*>::const_iterator it = entities.begin ( );
		     entities.end ( ) != it; it++)
		{
			DisplayProperties::GraphicalRepresentation*	gr	=
				(*it)->getDisplayProperties ( ).getGraphicalRepresentation ( );
			CHECK_NULL_PTR_ERROR (gr)
			RenderedEntityRepresentation*	rep	=
								dynamic_cast<RenderedEntityRepresentation*>(gr);
			if (0 != rep)
				rep->setRenderingManager (&renderingManager);
			registerHighlightedEntity (**it);
			gr->setHighlighted (true);
		}	// for (vector<Entity*>::const_iterator it = entities.begin ( );
	}	// else if (false == show)

	QT_PREVIEW_COMPLETE_TRY_CATCH_BLOCK

	renderingManager.forceRender ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : Affichage en surbrillance des entités associées à l'opération en cours.")
}	// QtMgx3DOperationPanel::highlight


void QtMgx3DOperationPanel::addValidatedField (ValidatedField& field)
{
	_validatedFields.push_back (&field);
}	// QtMgx3DOperationPanel::addValidatedField


const ContextIfc& QtMgx3DOperationPanel::getContext ( ) const
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return _mainWindow->getContext ( );
}	// QtMgx3DOperationPanel::getContext


ContextIfc& QtMgx3DOperationPanel::getContext ( )
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return _mainWindow->getContext ( );
}	// QtMgx3DOperationPanel::getContext


SelectionManagerIfc& QtMgx3DOperationPanel::getSelectionManager ( )
{
	return getContext ( ).getSelectionManager ( );
}	// QtMgx3DOperationPanel::getSelectionManager


const SelectionManagerIfc& QtMgx3DOperationPanel::getSelectionManager ( ) const
{
	return getContext ( ).getSelectionManager ( );
}	// QtMgx3DOperationPanel::getSelectionManager


vector<Entity*> QtMgx3DOperationPanel::getInvolvedEntities ( )
{
	return vector<Entity*> ( );
}	// QtMgx3DOperationPanel::getInvolvedEntities


RenderingManager& QtMgx3DOperationPanel::getRenderingManager ( )
{
	CHECK_NULL_PTR_ERROR (getMainWindow ( ))
	return getMainWindow ( )->getGraphicalWidget ( ).getRenderingManager ( );
}	// QtMgx3DOperationPanel::getRenderingManager


void QtMgx3DOperationPanel::registerHighlightedEntity (Entity& entity)
{
	_highlightedEntities.push_back (&entity);
}	// QtMgx3DOperationPanel::registerHighlightedEntity


void QtMgx3DOperationPanel::unregisterHighlightedEntity (Entity& entity)
{
	for (vector<Entity*>::iterator it = _highlightedEntities.begin ( );
	      _highlightedEntities.end ( ) != it; it++)
	{
		if (*it == &entity)
		{
			_highlightedEntities.erase (it);
			return;
		}	// if (*it == &entity)
	}	// for (vector<Entity*>::iterator it = _highlightedEntities.begin ( );
}	// QtMgx3DOperationPanel::unregisterHighlightedEntity


vector<Entity*> QtMgx3DOperationPanel::getHighlightedEntities ( )
{	
	return _highlightedEntities;
}	// QtMgx3DOperationPanel::getHighlightedEntities


void QtMgx3DOperationPanel::addPreviewCheckBox (bool defaultValue)
{	
	if (0 == _previewCheckBox)
	{
		QLayout*	l	= layout ( );
		if (0 == l)
		{
			INTERNAL_ERROR (exc, "Absence de gestionnaire de mise en forme du panneau.", "QtMgx3DOperationPanel::addPreviewCheckBox")
			throw exc;
		}	// if (0 == l)

		QLayout*	aLayout	= l;

		QBoxLayout*		boxLayout	= dynamic_cast<QBoxLayout*>(l);
		QHBoxLayout*	hlayout		= 0;
		if (0 != boxLayout)
		{
			hlayout	= new QHBoxLayout ( );
			boxLayout->addLayout (hlayout);
			aLayout	= hlayout;
		}	// if (0 != boxLayout)

		_previewCheckBox	= new QCheckBox (QString::fromUtf8("Aperçu"), this);
		_previewCheckBox->setCheckState (
						true == defaultValue ? Qt::Checked : Qt::Unchecked);
		aLayout->addWidget (_previewCheckBox);
		connect (_previewCheckBox, SIGNAL (stateChanged (int)), this,
		         SLOT (previewCallback ( )));

		if (0 != hlayout)
			hlayout->addStretch (2);
	}	// if (0 == _previewCheckBox)
}	// QtMgx3DOperationPanel::addPreviewCheckBox


void QtMgx3DOperationPanel::registerPreviewedObject (RenderingManager::RepresentationID id)
{
#ifndef NDEBUG
	for (vector<RenderingManager::RepresentationID>::const_iterator
			itri = _previewIDs.begin ( ); _previewIDs.end ( ) != itri; itri++)
		if (*itri == id)
		{
			UTF8String	msg (Charset::UTF_8);
			msg << "Previewed object " << (unsigned long)id
			    << " is already registered.";
			INTERNAL_ERROR (exc, msg, "QtMgx3DOperationPanel::registerPreviewedObject")
			throw exc;
		}	// if (*itri == id)
#endif	// NDEBUG
	_previewIDs.push_back (id);
}	// QtMgx3DOperationPanel::registerPreviewedObject


void QtMgx3DOperationPanel::removePreviewedObjects ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	vector<RenderingManager::RepresentationID>	ids	= _previewIDs;
	_previewIDs.clear ( );
	for (vector<RenderingManager::RepresentationID>::iterator it =
	     ids.begin ( ); ids.end ( ) != it; it++)
		getRenderingManager ( ).destroyRepresentation (*it, true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : actualisation de la prévisualisation")
}	// QtMgx3DOperationPanel::removePreviewedObjects


void QtMgx3DOperationPanel::previewEdges (
		CommandInternal& command, RenderingManager::CommandPreviewMgr& manager,
		InfoCommand::type status, const DisplayProperties& props,
		unsigned long mask)
{
	InfoCommand&		infos	= command.getInfoCommand ( );

	DisplayProperties	dp (props);
	dp.setWireColor (Color (
						255 * Resources::instance ( )._previewColor.getRed ( ),
						255 * Resources::instance ( )._previewColor.getGreen ( ),
						255 * Resources::instance ( )._previewColor.getBlue ( )));
	dp.setLineWidth (Resources::instance ( )._previewWidth.getValue ( ));
	dp.setCloudColor (Color (
						255 * Resources::instance ( )._previewColor.getRed ( ),
						255 * Resources::instance ( )._previewColor.getGreen ( ),
						255 * Resources::instance ( )._previewColor.getBlue ( )));
	dp.setPointSize (Resources::instance ( )._previewPointSize.getValue ( ));
	const Color	fontColor (
						255 * Resources::instance ( )._previewColor.getRed ( ),
						255 * Resources::instance ( )._previewColor.getGreen ( ),
						255 * Resources::instance ( )._previewColor.getBlue ( ));
	const int		fontFamily  = 0, fontSize   = 12;
	const bool		bold    = false,    italic  = false;
	dp.setFontProperties (fontFamily, fontSize, bold, italic, fontColor);

	map<Topo::TopoEntity*, Internal::InfoCommand::type>&  topo_entities_info = infos.getTopoInfoEntity();
        for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::iterator itei = topo_entities_info.begin();
                itei != topo_entities_info.end(); ++itei)
	{
		Topo::TopoEntity*	topo_entity	= itei->first;
		InfoCommand::type	topo_type	= itei->second;
//if (Entity::TopoCoEdge==topo_entity->getType( ))
//	cout << " arête "<<topo_entity->getName()<<" type = "<<Internal::InfoCommand::type2String(topo_type)<<endl;

		if ((Entity::TopoCoEdge==topo_entity->getType( )) && (status == topo_type))
		{
			CoEdge*	edge	= dynamic_cast<CoEdge*>(topo_entity);
			CHECK_NULL_PTR_ERROR(edge);
			DisplayRepresentation::type	type	= DisplayRepresentation::WIRE;
			if (0 != (mask & GraphicalEntityRepresentation::ASSOCIATIONS))
				type	= (DisplayRepresentation::type)(
								type | DisplayRepresentation::SHOWASSOCIATION);
			if (0 != (mask & GraphicalEntityRepresentation::MESH_SHAPE))
				type	= (DisplayRepresentation::type)(
								type | DisplayRepresentation::SHOWMESHSHAPE);
			//cout << "  représentation demandée avec type :"<<type<<endl;
			TopoDisplayRepresentation	rep (type);
			rep.setShrink (props.getShrinkFactor ( ));
			edge->getRepresentation (rep, false);
			const vector<Math::Point>&	pts	= rep.getPoints ( );
			const size_t			number	= pts.size ( );
			const vector<size_t>&	indices	= rep.getCurveDiscretization ( );
			double			x		= 0.,	y	= 0.,	z	= 0.;
			RenderingManager::RepresentationID	repID	=
				getRenderingManager ( ).createSegmentsWireRepresentation (
									pts, indices, dp, false);
			registerPreviewedObject (repID);
			manager.registerRepresentationID (repID);

			if (0 != (mask & GraphicalEntityRepresentation::CLOUDS))
			{
				RenderingManager::RepresentationID	repID2	=
					getRenderingManager ( ).createCloudRepresentation (
																pts, dp, false);
				registerPreviewedObject (repID2);
				manager.registerRepresentationID (repID2);
			}	// if (0 != (mask & GraphicalEntityRepresentation::CLOUDS))
			for (size_t	i = 0; i < number; i++)
			{
				x	+= pts [i].getX ( );
				y	+= pts [i].getY ( );
				z	+= pts [i].getZ ( );
			}	// for (size_t i = 0; i < pts.size ( ); i++)
			if (0 != number)
			{
				x	/= number;
				y	/= number;
				z	/= number;
			}	// if (0 != number)
			Math::Point					point (x, y, z);
			if (0 != (mask & GraphicalEntityRepresentation::NBMESHINGEDGE))
			{
				RenderingManager::RepresentationID	repIDT	=
					getRenderingManager ( ).createTextRepresentation (
								point, edge->getNbMeshingEdges ( ), dp, false);
				registerPreviewedObject (repIDT);
				manager.registerRepresentationID (repIDT);
			}	// if (0 != (mask & ...
			if (0 != (mask & GraphicalEntityRepresentation::ASSOCIATIONS))
			{
				const vector<Math::Point>&	arrow	= rep.getVector ( );
				if (2 == arrow.size ( ))
				{
					RenderingManager::RepresentationID	repIDT	=
						getRenderingManager ( ).createVector (
											arrow [0], arrow [1], dp, false);
					if (repIDT)
					{
						registerPreviewedObject (repIDT);
						manager.registerRepresentationID (repIDT);
					} // if (repIDT)
				}	// if (2 == arrow.size ( ))
			}	// if (0 != (mask & ...
		}	// if ((Entity::TopoCoEdge==te->getType( )) ...
	}	// for (vector<InfoCommand::TopoEntityInfo>::iterator itei = tei.begin ( );
}	// QtMgx3DOperationPanel::previewEdges


vector<RenderingManager::RepresentationID> QtMgx3DOperationPanel::getPreviewedObjects ( )
{
	return _previewIDs;
}	// QtMgx3DOperationPanel::getPreviewedObjects


void QtMgx3DOperationPanel::registerPreviewedInteractor (RenderingManager::Interactor* interactor)
{
	_interactors.push_back (interactor);
}	// QtMgx3DOperationPanel::registerPreviewedInteractor


vector<RenderingManager::Interactor*> QtMgx3DOperationPanel::getPreviewedInteractors ( )
{
	return _interactors;
}	// QtMgx3DOperationPanel::getPreviewedInteractors


void QtMgx3DOperationPanel::operationCompleted ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	preview (false, true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : actualisation du panneau d'opérations")
}	// QtMgx3DOperationPanel::operationCompleted


void QtMgx3DOperationPanel::log (const Log& log)
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	getContext ( ).getLogDispatcher ( ).log (log);
}	// QtMgx3DOperationPanel::log


void QtMgx3DOperationPanel::autoUpdateCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	autoUpdate ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : actualisation du panneau d'opérations")
}	// QtMgx3DOperationPanel::autoUpdateCallback


void QtMgx3DOperationPanel::parametersModifiedCallback ( )
{
	try
	{
		getRenderingManager ( );
	}
	catch (...)
	{	// Probablement période d'initialisation ...
		return;
	}

	BEGIN_QT_TRY_CATCH_BLOCK

	unique_ptr<RenderingManager::DisplayLocker>	displayLocker (new RenderingManager::DisplayLocker (getRenderingManager ( )));
	preview (false, true);
	displayLocker.reset ( );
	preview (true, true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : actualisation du panneau d'opérations")
}	// QtMgx3DOperationPanel::parametersModifiedCallback


void QtMgx3DOperationPanel::discretisationModifiedCallback ( )
{
	emit discretisationModified ( );
}	// QtMgx3DOperationPanel::discretisationModifiedCallback


void QtMgx3DOperationPanel::applyCallback ( )
{
	CommandResultIfc*	commandResult	= 0;
	
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (getMgx3DOperationAction ( ))
	getMgx3DOperationAction ( )->executeOperation ( );
	commandResult	= getMgx3DOperationAction ( )->getCommandResult ( );
	getMgx3DOperationAction ( )->setCommandResult (0);
	
	if (0 != commandResult)
	{
		if (CommandIfc::DONE == commandResult->getStatus ( ))
			hasError	= false;
		else
		{
			errorString	= commandResult->getStrStatus ( );
			commandResult->setUserNotified (true);
		}	// else if (CommandIfc::DONE == commandResult->getStatus ( ))
	}	// if (0 != commandResult)
	else
	{
		// Pas de résultat => pas de commande, ou threaded ... On part du principe que tout va bien.
		hasError	= false;
	}

	// On n'affiche pas nécessairement le message d'erreur, 
	// QtMgx3DMainWindow::commandModified l'a peut être déjà fait.
	COMPLETE_QT_TRY_CATCH_BLOCK (
// On force à true car lors de pré-traitements (ex : liste d'entités vide,
// entité détruite) il n'y a pas création de commande donc
// QtMgx3DMainWindow::commandModified n'est pas appelée donc pas de message
// d'erreur.
// Le risque, avec true à la place de !QtMgx3DApplication::_showDialogOnCommandError.getValue( ),
// est que le message d'erreur soit affiché à 2 reprises.
true,
//			!Resources::instance ( )._showDialogOnCommandError.getValue( ),
			this, "Magix 3D : exécution d'une opération")

	const bool succeeded	= !hasError;
	RE_BEGIN_QT_TRY_CATCH_BLOCK

	if (true == succeeded)
	{	// Sinon on laisse le panneau tel que, avec la saisie invalide
		getSelectionManager ( ).clearSelection ( );
		operationCompleted ( );
	}	// if (true == succeeded)

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : exécution d'une opération")
}	// QtMgx3DOperationPanel::applyCallback


void QtMgx3DOperationPanel::helpCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtHelpWindow::displayURL (helpURL ( ), helpTag ( ));

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : affichage de l'aide d'une opération.")
}	// QtMgx3DOperationPanel::helpCallback


void QtMgx3DOperationPanel::entitiesAddedToSelectionCallback (QString entitiesNames)
{
	try
	{
		getRenderingManager ( );
	}
	catch (...)
	{	// Probablement période d'initialisation ...
		return;
	}

	BEGIN_QT_TRY_CATCH_BLOCK

	// Actualisation surbrillance :
	const string		names (entitiesNames.toStdString ( ));
	string::size_type	current = 0, end	= names.find (' ');
	if (((string::size_type)-1 == end) && (0 != names.length ( )))
		end	= names.length ( );
	while ((string::size_type)-1 != end)
	{
		const string	name	= names.substr (current, end - current);
		// Il arrive que des espaces soient doublés lorsqu'une entité est
		// enlevée de la sélection. La suppression de l'espace pourrait être
		// fait à ce moment, mais n'écarte pas une erreur de saisie manuelle de
		// l'utilisateur qui lèvera une exception pas forcément comprise. Donc
		// on teste ici ce cas :
		if (false == name.empty ( ))
		{
			try
			{
				Entity*	entity	= &getContext().nameToEntity (name);
				registerHighlightedEntity (*entity);
				DisplayProperties::GraphicalRepresentation*	gr	= entity->getDisplayProperties ( ).getGraphicalRepresentation ( );
				CHECK_NULL_PTR_ERROR (gr)
				gr->setHighlighted (true);
			}
			catch (...)
			{
			}
		}	// if (false == name.empty ( ))
		current	= end + 1;
		end		= names.find (' ', current);
		if (((string::size_type)-1 == end) && (current < names.length ( )))
			end	= names.length ( );
	}	// while ((string::size_type)-1 != end)

	// Actualisation aperçu résultat opération :
	unique_ptr<RenderingManager::DisplayLocker>	displayLocker (
				new RenderingManager::DisplayLocker (getRenderingManager ( )));
	preview (false, true);
	displayLocker.reset ( );
	preview (true, true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : Ajout d'entités à une opération.")
}	// QtMgx3DOperationPanel::entitiesAddedToSelectionCallback


void QtMgx3DOperationPanel::entitiesRemovedFromSelectionCallback (QString entitiesNames)
{
	try
	{
		getRenderingManager ( );
	}
	catch (...)
	{	// Probablement période d'initialisation ...
		return;
	}

	BEGIN_QT_TRY_CATCH_BLOCK

	// Actualisation surbrillance :
	const string		names (entitiesNames.toStdString ( ));
	string::size_type	current = 0, end	= names.find (' ');
	if (((string::size_type)-1 == end) && (0 != names.length ( )))
		end	= names.length ( );
	while ((string::size_type)-1 != end)
	{
		const string	name	= names.substr (current, end - current);
		// Il arrive que des espaces soient doublés lorsqu'une entité est
		// enlevée de la sélection. La suppression de l'espace pourrait être
		// fait à ce moment, mais n'écarte pas une erreur de saisie manuelle de
		// l'utilisateur qui lèvera une exception pas forcément comprise. Donc
		// on teste ici ce cas :
		if (false == name.empty ( ))
		{
			try
			{
				Entity*	entity	= &getContext().nameToEntity (name);
				unregisterHighlightedEntity (*entity);
				DisplayProperties::GraphicalRepresentation*	gr	= entity->getDisplayProperties ( ).getGraphicalRepresentation ( );
				CHECK_NULL_PTR_ERROR (gr)
				gr->setHighlighted (false);
			}
			catch (...)
			{
			}
		}	// if (false == name.empty ( ))
		current	= end + 1;
		end		= names.find (' ', current);
		if (((string::size_type)-1 == end) && (current < names.length ( )))
			end	= names.length ( );
	}	// while ((string::size_type)-1 != end)


	// Actualisation aperçu résultat opération :
	unique_ptr<RenderingManager::DisplayLocker>	displayLocker (
				new RenderingManager::DisplayLocker (getRenderingManager ( )));
	preview (false, true);
	displayLocker.reset ( );
	preview (true, true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : Enlèvement d'entités à une opération.")
}	// QtMgx3DOperationPanel::entitiesRemovedFromSelectionCallback


void QtMgx3DOperationPanel::previewCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_previewCheckBox)
	const bool	displayPreview	= Qt::Checked == _previewCheckBox->checkState ( ) ? true : false;
	preview (displayPreview, true);
	highlight (true);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : passage en mode aperçu.")
}	// QtMgx3DOperationPanel::previewCallback


// ===========================================================================
//                   LA CLASSE QtOperationsSubFamilyToolBar
// ===========================================================================

QtOperationsSubFamilyToolBar::QtOperationsSubFamilyToolBar (QWidget* parent, QtMgx3DOperationsPanel* panel)
	: QtMultiLineToolbar (parent),
	  _operationsGroup (0),
	  _operationsPanel (panel), _operationPanel (0)
{
	_operationsGroup	= new QActionGroup (this);
	_operationsGroup->setExclusive (true);
}	// QtOperationsSubFamilyToolBar::QtOperationsSubFamilyToolBar


QtOperationsSubFamilyToolBar::QtOperationsSubFamilyToolBar (const QtOperationsSubFamilyToolBar&)
	: QtMultiLineToolbar (0),
	  _operationsGroup (0),
	  _operationsPanel (0), _operationPanel (0)
{
	MGX_FORBIDDEN("QtOperationsSubFamilyToolBar copy constructor is not allowed.");
}	// QtOperationsSubFamilyToolBar::QtOperationsSubFamilyToolBar


QtOperationsSubFamilyToolBar& QtOperationsSubFamilyToolBar::operator = (const QtOperationsSubFamilyToolBar&)
{
	MGX_FORBIDDEN ("QtOperationsSubFamilyToolBar assignment operator is not allowed.");
	return *this;
}	// QtOperationsSubFamilyToolBar::operator =


QtOperationsSubFamilyToolBar::~QtOperationsSubFamilyToolBar ( )
{
}	// QtOperationsSubFamilyToolBar::~QtOperationsSubFamilyToolBar


void QtOperationsSubFamilyToolBar::addOperationAction (QtMgx3DOperationAction& action)
{
	CHECK_NULL_PTR_ERROR (_operationsGroup)
	CHECK_NULL_PTR_ERROR (_operationsPanel)
	_operationsGroup->addAction (&action);
	addAction (action);
}	// QtOperationsSubFamilyToolBar::addOperationAction


QtMgx3DOperationPanel* QtOperationsSubFamilyToolBar::getOperationPanel ( )
{
	return _operationPanel;
}	// QtOperationsFamilyToolBar::getOperationPanel


void QtOperationsSubFamilyToolBar::setOperationPanel (QtMgx3DOperationPanel* panel)
{
	_operationPanel	= panel;
}	// QtOperationsFamilyToolBar::setOperationPanel


void QtOperationsSubFamilyToolBar::sessionCleared ( )
{
	if (0 != _operationsGroup)
	{
		QList<QAction*>	actions	= _operationsGroup->actions ( );
		for (QList<QAction*>::iterator ita = actions.begin ( ); actions.end ( ) != ita; ita++)
		{
			QtMgx3DOperationAction*	mgxAction	= dynamic_cast<QtMgx3DOperationAction*>(*ita);
			QtMgx3DOperationPanel*	panel		= 0 == mgxAction ?  0 : mgxAction->getOperationPanel ( );
			if (0 != panel)
			{
				panel->reset ( );
			}	// if (0 != panel)
		}	// for (QList<QAction*>::iterator ita = actions.begin ( ); ...
	}	// if (0 != _operationsGroup)
}	// QtOperationsSubFamilyToolBar::sessionCleared


// ===========================================================================
//                   LA CLASSE QtOperationsFamilyToolBar
// ===========================================================================

QtOperationsFamilyToolBar::QtOperationsFamilyToolBar (QWidget* parent, QtMgx3DOperationsPanel* panel)
	: QtMultiLineToolbar (parent),
	  _operationsSubFamiliesGroup (0), _operationsPanel (panel), _subFamilyToolBar (0)
{
	_operationsSubFamiliesGroup	= new QActionGroup (this);
	_operationsSubFamiliesGroup->setExclusive (true);
//		parentWidget ( )->layout ( )->addWidget (this);
/*_operationsSubFamiliesToolBar->setAutoFillBackground (true);
QPalette    p   = _operationsSubFamiliesToolBar->palette ( );
p.setColor (QPalette::Active, QPalette::Window, Qt::red);
_operationsSubFamiliesToolBar->setPalette (p);*/
}	// QtOperationsFamilyToolBar::QtOperationsFamilyToolBar


QtOperationsFamilyToolBar::QtOperationsFamilyToolBar (const QtOperationsFamilyToolBar&)
	: QtMultiLineToolbar (0),
	  _operationsSubFamiliesGroup (0),
	  _operationsPanel (0), _subFamilyToolBar (0)
{
	MGX_FORBIDDEN("QtOperationsFamilyToolBar copy constructor is not allowed.");
}	// QtOperationsFamilyToolBar::QtOperationsFamilyToolBar


QtOperationsFamilyToolBar& QtOperationsFamilyToolBar::operator = (const QtOperationsFamilyToolBar&)
{
	MGX_FORBIDDEN ("QtOperationsFamilyToolBar assignment operator is not allowed.");
	return *this;
}	// QtOperationsFamilyToolBar::operator =


QtOperationsFamilyToolBar::~QtOperationsFamilyToolBar ( )
{
}	// QtOperationsFamilyToolBar::~QtOperationsFamilyToolBar


void QtOperationsFamilyToolBar::addOperationsSubFamilyToolBar (QtMgx3DOperationsFamilyAction& action)
{
	CHECK_NULL_PTR_ERROR (_operationsSubFamiliesGroup)
	CHECK_NULL_PTR_ERROR (_operationsPanel)
	_operationsSubFamiliesGroup->addAction (&action);
	addAction (action);
}	// QtOperationsFamilyToolBar::addOperationsSubFamilyToolBar


QtOperationsSubFamilyToolBar* QtOperationsFamilyToolBar::getSubFamilyToolBar ( )
{
	return _subFamilyToolBar;
}	// QtOperationsFamilyToolBar::getSubFamilyToolBar


void QtOperationsFamilyToolBar::setSubFamilyToolBar (QtOperationsSubFamilyToolBar* tb)
{
	_subFamilyToolBar	= tb;
}	// QtOperationsFamilyToolBar::setSubFamilyToolBar


// ===========================================================================
//                   LA CLASSE QtOperationsFamiliesToolBar
// ===========================================================================

QtOperationsFamiliesToolBar::QtOperationsFamiliesToolBar (QWidget* parent, QtMgx3DOperationsPanel* panel)
	: QtMultiLineToolbar (parent),
	  _operationsFamiliesGroup (0),
	  _operationsPanel (panel)
{
	_operationsFamiliesGroup	= new QActionGroup (this);
	_operationsFamiliesGroup->setExclusive (true);
//		parentWidget ( )->layout ( )->addWidget (this);
/*_operationsSubFamiliesToolBar->setAutoFillBackground (true);
QPalette    p   = _operationsSubFamiliesToolBar->palette ( );
p.setColor (QPalette::Active, QPalette::Window, Qt::red);
_operationsSubFamiliesToolBar->setPalette (p);*/
}	// QtOperationsFamiliesToolBar::QtOperationsFamiliesToolBar


QtOperationsFamiliesToolBar::QtOperationsFamiliesToolBar (const QtOperationsFamiliesToolBar&)
	: QtMultiLineToolbar (0),
	  _operationsFamiliesGroup (0),
	  _operationsPanel (0)
{
	MGX_FORBIDDEN("QtOperationsFamiliesToolBar copy constructor is not allowed.");
}	// QtOperationsFamiliesToolBar::QtOperationsFamiliesToolBar


QtOperationsFamiliesToolBar& QtOperationsFamiliesToolBar::operator = (
											const QtOperationsFamiliesToolBar&)
{
	MGX_FORBIDDEN ("QtOperationsFamiliesToolBar assignment operator is not allowed.");
	return *this;
}	// QtOperationsFamiliesToolBar::operator =


QtOperationsFamiliesToolBar::~QtOperationsFamiliesToolBar ( )
{
}	// QtOperationsFamiliesToolBar::~QtOperationsFamiliesToolBar


void QtOperationsFamiliesToolBar::addOperationsFamilyToolBar (QtMgx3DOperationsFamiliesAction& action)
{
	CHECK_NULL_PTR_ERROR (_operationsFamiliesGroup)
	CHECK_NULL_PTR_ERROR (_operationsPanel)
	_operationsFamiliesGroup->addAction (&action);
	addAction (action);
}	// QtOperationsFamiliesToolBar::addOperationsSubFamilyToolBar


// ===========================================================================
//                    LA CLASSE QtMgx3DWidgetedCheckedAction
// ===========================================================================

QtMgx3DWidgetedCheckedAction::QtMgx3DWidgetedCheckedAction (const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtWidgetAction (icon, text, &mainWindow.getOperationsPanel ( )), _operationsPanel (&mainWindow.getOperationsPanel ( )), _mainWindow (&mainWindow), _widget (0)
{
//	CHECK_NULL_PTR_ERROR (panel)
	setCheckable (true);
	if (false == tooltip.isEmpty ( ))
		setToolTip (tooltip);
	connect (this, SIGNAL (toggled (bool)), this, SLOT (checkedCallback (bool)));
}	// QtMgx3DWidgetedCheckedAction::QtMgx3DWidgetedCheckedAction


QtMgx3DWidgetedCheckedAction::QtMgx3DWidgetedCheckedAction (const QtMgx3DWidgetedCheckedAction&)
	: QtWidgetAction (0), _operationsPanel (0), _mainWindow (0), _widget (0)
{
	MGX_FORBIDDEN ("QtMgx3DWidgetedCheckedAction copy constructor is not allowed.");
}	// QtMgx3DWidgetedCheckedAction::QtMgx3DWidgetedCheckedAction


QtMgx3DWidgetedCheckedAction& QtMgx3DWidgetedCheckedAction::operator = (const QtMgx3DWidgetedCheckedAction&)
{
	MGX_FORBIDDEN ("QtMgx3DWidgetedCheckedAction assignment operator is not allowed.");
	return *this;
}	// QtMgx3DWidgetedCheckedAction::operator =


QtMgx3DWidgetedCheckedAction::~QtMgx3DWidgetedCheckedAction ( )
{
}	// QtMgx3DWidgetedCheckedAction::~QtMgx3DWidgetedCheckedAction


const QtMgx3DOperationsPanel& QtMgx3DWidgetedCheckedAction::getOperationsPanel ( ) const
{
	CHECK_NULL_PTR_ERROR (_operationsPanel)
	return *_operationsPanel;
}	// QtMgx3DWidgetedCheckedAction::getOperationsPanel


QtMgx3DOperationsPanel& QtMgx3DWidgetedCheckedAction::getOperationsPanel ( )
{
	CHECK_NULL_PTR_ERROR (_operationsPanel)
	return *_operationsPanel;
}	// QtMgx3DWidgetedCheckedAction::getOperationsPanel


const QtMgx3DMainWindow& QtMgx3DWidgetedCheckedAction::getMainWindow ( ) const
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return *_mainWindow;
}	// QtMgx3DWidgetedCheckedAction::getMainWindow


QtMgx3DMainWindow& QtMgx3DWidgetedCheckedAction::getMainWindow ( )
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return *_mainWindow;
}	// QtMgx3DWidgetedCheckedAction::getMainWindow


QWidget* QtMgx3DWidgetedCheckedAction::getWidget ( )
{
	return _widget;
}	// QtMgx3DWidgetedCheckedAction::getWidget


void QtMgx3DWidgetedCheckedAction::setWidget (QWidget* widget)
{
	_widget	= widget;
}	// QtMgx3DWidgetedCheckedAction::setWidget


QWidget& QtMgx3DWidgetedCheckedAction::getOperationPanelParent ( )
{
	return getOperationsPanel ( ).getOperationPanelParent ( );
}	// QtMgx3DWidgetedCheckedAction::getOperationPanelParent


const ContextIfc& QtMgx3DWidgetedCheckedAction::getContext ( ) const
{
	return getMainWindow ( ).getContext ( );
}	// QtMgx3DWidgetedCheckedAction::getContext


ContextIfc& QtMgx3DWidgetedCheckedAction::getContext ( )
{
	return getMainWindow ( ).getContext ( );
}	// QtMgx3DWidgetedCheckedAction::getContext


void QtMgx3DWidgetedCheckedAction::checkedCallback (bool checked)
{
}	// QtMgx3DWidgetedCheckedAction::checkedCallback


// ===========================================================================
//                    LA CLASSE QtMgx3DOperationAction
// ===========================================================================

QtMgx3DOperationAction::QtMgx3DOperationAction (const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow, const QString& tooltip)
	: QtMgx3DWidgetedCheckedAction (icon, text, mainWindow, tooltip), _commandResult (0)
{
}	// QtMgx3DOperationAction::QtMgx3DOperationAction


QtMgx3DOperationAction::QtMgx3DOperationAction (const QtMgx3DOperationAction&)
	: QtMgx3DWidgetedCheckedAction (QIcon (""), "", *new QtMgx3DMainWindow (0), ""), _commandResult (0)
{
	MGX_FORBIDDEN ("QtMgx3DOperationAction copy constructor is not allowed.");
}	// QtMgx3DOperationAction::QtMgx3DOperationAction


QtMgx3DOperationAction& QtMgx3DOperationAction::operator = (const QtMgx3DOperationAction&)
{
	MGX_FORBIDDEN ("QtMgx3DOperationAction assignment operator is not allowed.");
	return *this;
}	// QtMgx3DOperationAction::operator =


QtMgx3DOperationAction::~QtMgx3DOperationAction ( )
{
}	// QtMgx3DOperationAction::~QtMgx3DOperationAction


QtMgx3DOperationPanel* QtMgx3DOperationAction::getOperationPanel ( )
{
	return dynamic_cast<QtMgx3DOperationPanel*>(getWidget ( ));
}	// QtMgx3DOperationAction::getOperationPanel


void QtMgx3DOperationAction::executeOperation ( )
{
	CHECK_NULL_PTR_ERROR (getOperationPanel ( ))
// CP : suite discussion EBL/FL, il est convenu que la validation des
// paramètres de l'opération est effectuée par le "noyau" et qu'un mauvais
// paramétrage est remonté sous forme d'exception à la fonction appelante, donc
// avant exécution de la commande.
// Le mécanisme est néanmoins laissé afin de pouvoir vérifier d'autres aspects
// qui seraient par exemple spécifiques à l'IHM :
	getOperationPanel ( )->validate ( );
}	// QtMgx3DOperationAction::executeOperation


CommandResultIfc* QtMgx3DOperationAction::getCommandResult ( )
{
	return _commandResult;
}	// QtMgx3DOperationAction::getCommandResult


void QtMgx3DOperationAction::setCommandResult (CommandResultIfc* commandResult)
{
	_commandResult	= commandResult;
}	// QtMgx3DOperationAction::setCommandResult


void QtMgx3DOperationAction::setActivated (bool)
{	// Ne fait rien par défaut, une opération est toujours active.
}	// QtMgx3DOperationAction::setActivated


void QtMgx3DOperationAction::setOperationPanel (QtMgx3DOperationPanel* panel)
{
	QtMgx3DOperationPanel*	operationPanel	= getOperationPanel ( );

	if ((0 != operationPanel) && (operationPanel != panel))
		throw Exception (UTF8String ("QtMgx3DOperationAction::setOperationPanel : panneau déjà affecté.", Charset::UTF_8));

	setWidget (panel);
	if (0 != panel)
	{
		CHECK_NULL_PTR_ERROR  (getOperationPanelParent ( ).layout ( ))
		getOperationPanelParent ( ).layout ( )->addWidget (panel);
		panel->hide ( );
	}	// if (0 != panel)
}	// QtMgx3DOperationAction::setOperationPanel


void QtMgx3DOperationAction::checkedCallback (bool checked)
{
	if (false == checked)
		return;

	try
	{
		QtMgx3DOperationPanel*	panel	= getOperationPanel ( );
		CHECK_NULL_PTR_ERROR (panel)
		getOperationsPanel ( ).setOperationsPanel (panel);
	}
	catch (...)
	{
	}
}	// QtMgx3DOperationAction::checkedCallback


// ===========================================================================
//        LA CLASSE QtMgx3DOperationsFamilyAction
// ===========================================================================

QtMgx3DOperationsFamilyAction::QtMgx3DOperationsFamilyAction (const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow, QtOperationsSubFamilyToolBar& toolbar, const QString& tooltip)
	: QtMgx3DWidgetedCheckedAction (icon, text, mainWindow, tooltip)
{
	setWidget (&toolbar);
}	// QtMgx3DOperationsFamilyAction::QtMgx3DOperationsFamilyAction


QtMgx3DOperationsFamilyAction::QtMgx3DOperationsFamilyAction (const QtMgx3DOperationsFamilyAction&)
	: QtMgx3DWidgetedCheckedAction (QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtMgx3DOperationsFamilyAction copy constructor is not allowed.");
}	// QtMgx3DOperationsFamilyAction::QtMgx3DOperationsFamilyAction


QtMgx3DOperationsFamilyAction& QtMgx3DOperationsFamilyAction::operator = (const QtMgx3DOperationsFamilyAction&)
{
	MGX_FORBIDDEN ("QtMgx3DOperationsFamilyAction assignment operator is not allowed.");
	return *this;
}	// QtMgx3DOperationsFamilyAction::operator =


QtMgx3DOperationsFamilyAction::~QtMgx3DOperationsFamilyAction ( )
{
}	// QtMgx3DOperationsFamilyAction::~QtMgx3DOperationsFamilyAction


void QtMgx3DOperationsFamilyAction::checkedCallback (bool checked)
{
	if (false == checked)
		return;

	try
	{
		QtOperationsSubFamilyToolBar*	toolbar	= dynamic_cast<QtOperationsSubFamilyToolBar*>(getWidget ( ));
		CHECK_NULL_PTR_ERROR (toolbar)
		getOperationsPanel ( ).setOperationsSubFamilyToolBar (toolbar);
	}
	catch (...)
	{
	}
}	// QtMgx3DOperationsFamilyAction::checkedCallback


// ===========================================================================
//        LA CLASSE QtMgx3DOperationsFamiliesAction
// ===========================================================================

QtMgx3DOperationsFamiliesAction::QtMgx3DOperationsFamiliesAction (const QIcon& icon, const QString& text, QtMgx3DMainWindow& mainWindow, QtOperationsFamilyToolBar& toolbar, const QString& tooltip)
	: QtMgx3DWidgetedCheckedAction (icon, text, mainWindow, tooltip)
{
	setWidget (&toolbar);
}	// QtMgx3DOperationsFamiliesAction::QtMgx3DOperationsFamiliesAction


QtMgx3DOperationsFamiliesAction::QtMgx3DOperationsFamiliesAction (const QtMgx3DOperationsFamiliesAction&)
	: QtMgx3DWidgetedCheckedAction (
					QIcon (""), "", *new QtMgx3DMainWindow (0), "")
{
	MGX_FORBIDDEN ("QtMgx3DOperationsFamiliesAction copy constructor is not allowed.");
}	// QtMgx3DOperationsFamiliesAction::QtMgx3DOperationsFamiliesAction


QtMgx3DOperationsFamiliesAction& QtMgx3DOperationsFamiliesAction::operator = (const QtMgx3DOperationsFamiliesAction&)
{
	MGX_FORBIDDEN ("QtMgx3DOperationsFamiliesAction assignment operator is not allowed.");
	return *this;
}	// QtMgx3DOperationsFamiliesAction::operator =


QtMgx3DOperationsFamiliesAction::~QtMgx3DOperationsFamiliesAction ( )
{
}	// QtMgx3DOperationsFamiliesAction::~QtMgx3DOperationsFamiliesAction


void QtMgx3DOperationsFamiliesAction::checkedCallback (bool checked)
{
	if (false == checked)
		return;

	try
	{
		QtOperationsFamilyToolBar*	toolbar	= dynamic_cast<QtOperationsFamilyToolBar*>(getWidget ( ));
		CHECK_NULL_PTR_ERROR (toolbar)
		getOperationsPanel ( ).setOperationsFamilyToolBar (toolbar);
	}
	catch (...)
	{
	}
}	// QtMgx3DOperationsFamiliesAction::checkedCallback


// ===========================================================================
//                        LA CLASSE QtMgx3DOperationsPanel
// ===========================================================================

QtMgx3DOperationsPanel::QtMgx3DOperationsPanel (QWidget* parent, const string& name, QtMgx3DMainWindow& mainWindow, const string& appTitle)
	: QtGroupBox (name.c_str ( ), parent),
	  _appTitle (appTitle), _logStream (0),
	  _mainWindow (&mainWindow),
	  _operationsFamiliesToolBar (0),
	  _currentFamilyToolBar (0), _currentSubFamilyToolBar (0),
	  _operationsToolbars ( ),
	  _operationsTabWidget (0), _currentOperationAreaWidget (0),
	  _operationPanelScrollArea (0), _operationPanel (0),
	  _reinitializeButton (0), _applyButton (0), _showOperationPanelCheckbox (0)
{
}	// QtMgx3DOperationsPanel::QtMgx3DOperationsPanel


QtMgx3DOperationsPanel::QtMgx3DOperationsPanel (const QtMgx3DOperationsPanel&)
	: QtGroupBox ("Invalid QtMgx3DOperationsPanel", 0),
	  _appTitle ("Invalid App"), _logStream (0), _mainWindow (0),
	  _operationsFamiliesToolBar (0),
	  _currentFamilyToolBar (0), _currentSubFamilyToolBar (0),
	  _operationsToolbars ( ),
	  _operationsTabWidget (0), _currentOperationAreaWidget (0),
	  _operationPanelScrollArea (0), _operationPanel (0),
	  _reinitializeButton (0), _applyButton (0), _showOperationPanelCheckbox (0)
{
	MGX_FORBIDDEN ("QtMgx3DOperationsPanel copy constructor is not allowed.");
}	// QtMgx3DOperationsPanel::QtMgx3DOperationsPanel (const QtMgx3DOperationsPanel&)


QtMgx3DOperationsPanel& QtMgx3DOperationsPanel::operator = (const QtMgx3DOperationsPanel&)
{
	MGX_FORBIDDEN ("QtMgx3DOperationsPanel assignment operator is not allowed.");
	return *this;
}	// QtMgx3DOperationsPanel::QtMgx3DOperationsPanel (const QtMgx3DOperationsPanel&)


QtMgx3DOperationsPanel::~QtMgx3DOperationsPanel ( )
{
}	// QtMgx3DOperationsPanel::~QtMgx3DOperationsPanel


void QtMgx3DOperationsPanel::writeSettings (QSettings& settings)
{
	settings.beginGroup ("OperationsPanel");
	settings.setValue ("size", size ( ));
	settings.endGroup ( );
}	// QtMgx3DOperationsPanel::writeSettings


void QtMgx3DOperationsPanel::readSettings (QSettings& settings)
{
	settings.beginGroup ("OperationsPanel");
	resize (settings.value ("size", size ( )).toSize ( ));
	settings.endGroup ( );
}	// QtMgx3DOperationsPanel::readSettings


void QtMgx3DOperationsPanel::autoUpdate ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _operationPanel)
	{
		_operationPanel->autoUpdate ( );
	}	// if (0 != _operationPanel)

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : actualisation du panneau d'opérations")
}	// QtMgx3DOperationsPanel::autoUpdate


void QtMgx3DOperationsPanel::setEnabled (bool enable)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QWidget::setEnabled (enable);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : (in)activation du panneau d'opérations")
}	// QtMgx3DOperationsPanel::setEnabled


const QtMgx3DMainWindow& QtMgx3DOperationsPanel::getMainWindow ( ) const
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return *_mainWindow;
}	// QtMgx3DOperationsPanel::getMainWindow


QtMgx3DMainWindow& QtMgx3DOperationsPanel::getMainWindow ( )
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return *_mainWindow;
}	// QtMgx3DOperationsPanel::getMainWindow


const ContextIfc& QtMgx3DOperationsPanel::getContext ( ) const
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return _mainWindow->getContext ( );
}	// QtMgx3DOperationsPanel::getContext


ContextIfc& QtMgx3DOperationsPanel::getContext ( )
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return _mainWindow->getContext ( );
}	// QtMgx3DOperationsPanel::getContext


void QtMgx3DOperationsPanel::sessionCleared ( )
{
	for (map<OPERATION_TYPES, QtOperationsSubFamilyToolBar*>::iterator ittb = _operationsToolbars.begin ( ); _operationsToolbars.end ( ) != ittb; ittb++)
	{
		try
		{
			ittb->second->sessionCleared ( );
		}
		catch (...)
		{
		}
	}	// for (map<OPERATION_TYPES, QtOperationsSubFamilyToolBar*> ...
}	// QtMgx3DOperationsPanel::sessionCleared


void QtMgx3DOperationsPanel::createGui ( )
{
	setMargin (0);
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (0);
	layout->setSpacing (0);
//	layout->setSizeConstraint (QLayout::SetFixedSize);
//	layout->setSizeConstraint (QLayout::SetMinAndMaxSize);
	setLayout (layout);

	// Familles d'opérations :
	QtGroupBox*	groupBox	= new QtGroupBox (QString::fromUtf8("Familles d'opérations"), this);
	layout->addWidget (groupBox, 0);
	_operationsFamiliesToolBar	= new QtOperationsFamiliesToolBar (groupBox, this);
	QHBoxLayout*	familiesLayout	= new QHBoxLayout (groupBox);
	groupBox->setLayout (familiesLayout);
	familiesLayout->setMargin (0);
	familiesLayout->setSpacing (0);
	familiesLayout->addWidget (_operationsFamiliesToolBar);

	// Sous-familles d'opérations :
	QtGroupBox*	subFamiliesBox	= new QtGroupBox (QString::fromUtf8("Sous-familles d'opérations"), this);
	layout->addWidget (subFamiliesBox, 0);
	QHBoxLayout*	subFamilieslayout	= new QHBoxLayout (subFamiliesBox);
	subFamiliesBox->setLayout (subFamilieslayout);
	subFamilieslayout->setMargin (0);
	subFamilieslayout->setSpacing (0);
	// Opérations des sous-familles :
	QtGroupBox*	operationsBox	= new QtGroupBox (QString::fromUtf8("Opérations"), this);
	layout->addWidget (operationsBox, 0);
	QHBoxLayout*	operationsLayout	= new QHBoxLayout (operationsBox);
	operationsBox->setLayout (operationsLayout);
	operationsLayout->setMargin (0);
	operationsLayout->setSpacing (0);

	// Les opérations et explorations :
	_operationsTabWidget	= new QTabWidget (this);
	layout->addWidget (_operationsTabWidget);

	// Panneau d'édition de l'opération courante : on créé maintenant cette
	// zone => création du widget parent des panneaux d'édition de "l'opération
	// courante".
	QWidget*		currentOperationWidget	= new QWidget(_operationsTabWidget);
//	SET_WIDGET_BACKGROUND (currentOperationWidget, Qt::green)
	QVBoxLayout*	vLayout	= new QVBoxLayout (currentOperationWidget);
	currentOperationWidget->setLayout (vLayout);
	_operationsTabWidget->addTab (currentOperationWidget, "Opération courante");
//	SET_WIDGET_BACKGROUND (_operationsTabWidget, Qt::cyan)
	const QSizePolicy::Policy	grow	= (QSizePolicy::Policy)QSizePolicy::GrowFlag;
	const QSizePolicy::Policy	growAndShrink	= (QSizePolicy::Policy)(QSizePolicy::GrowFlag|QSizePolicy::ShrinkFlag);
	const QSizePolicy::Policy	ignore	= (QSizePolicy::Policy)QSizePolicy::IgnoreFlag;	// sans grow/shrink
//	_currentOperationAreaWidget	= new QtGroupBox ("", currentOperationWidget);
	_currentOperationAreaWidget	= new QWidget (currentOperationWidget);
//	SET_WIDGET_BACKGROUND (_currentOperationAreaWidget, Qt::red)
	vLayout->addWidget (_currentOperationAreaWidget, 10);
	QVBoxLayout*	opLayout	= new QVBoxLayout (_currentOperationAreaWidget);
//	opLayout->setSizeConstraint (QLayout::SetFixedSize);	// -> sizeHint ( )
	_currentOperationAreaWidget->setLayout (opLayout);
//	_operationPanelScrollArea	= new QtMgx3DScrollArea ( );
	_operationPanelScrollArea	= new QScrollArea ( );
//	SET_WIDGET_BACKGROUND (_operationPanelScrollArea, Qt::yellow)
	_operationPanelScrollArea->setFrameStyle (QFrame::NoFrame | QFrame::Plain);
	opLayout->addWidget (_operationPanelScrollArea, 10);
	// Eviter si possible les scrollbars :
	_operationPanelScrollArea->setWidgetResizable (true);
// commit 1963 : on inactive la ligne ci-après (->setSizePolicy (grow, grow),
// responsable de dimensionnements trop grands de l'IHM sur de petits écrans.
// cmt 1963 : _operationPanelScrollArea->setSizePolicy (grow, grow);
	// Un widget tampon qui absorbe les variations de taille :
//	QWidget*	w	= new QWidget (currentOperationWidget);
//	SET_WIDGET_BACKGROUND (w, Qt::green)
//w->setMinimumSize (100, 100);
//	w->setSizePolicy (growAndShrink, growAndShrink);
//	vLayout->addWidget (w, 100);
	// Les boutons "Afficher", "Réinitialiser" et "Appliquer" :
	QWidget*	applyAreaWidget	= new QWidget (currentOperationWidget);
//	SET_WIDGET_BACKGROUND (applyAreaWidget, Qt::blue)
	vLayout->addWidget (applyAreaWidget, 1);
	QHBoxLayout*	applyLayout	= new QHBoxLayout (applyAreaWidget);
	applyAreaWidget->setLayout (applyLayout);
	applyLayout->setSizeConstraint (QLayout::SetFixedSize);
	_showOperationPanelCheckbox	= new QCheckBox ("Afficher", applyAreaWidget);
	_showOperationPanelCheckbox->setToolTip (QString::fromUtf8("Cocher la case pour afficher les panneaux Opérations."));
	_showOperationPanelCheckbox->setChecked (Qt::Checked);
	connect (_showOperationPanelCheckbox, SIGNAL (stateChanged (int)), this, SLOT (displayOperationPanelCallback ( )));
	applyLayout->addWidget (_showOperationPanelCheckbox, 0);
	applyLayout->addStretch (1000);	// Apply collé à droite.
	_reinitializeButton	= new QPushButton(QString::fromUtf8("Réinitialiser"), applyAreaWidget);
	applyLayout->addWidget (_reinitializeButton, 0);
	_reinitializeButton->setEnabled (false);
	connect (_reinitializeButton, SIGNAL (clicked (bool)), this, SLOT (resetCallback ( )));		// 15/01/18
//	         SLOT (autoUpdateCallback ( )));
	_applyButton	= new QPushButton("Appliquer", applyAreaWidget);
	applyLayout->addWidget (_applyButton, 0);
	_applyButton->setEnabled (false);
	connect (_applyButton, SIGNAL (clicked (bool)), this, SLOT (applyCallback ( )));

	// Le bouton "Aide ..." :
	QPushButton*	helpButton	= new QPushButton("Aide ...", applyAreaWidget);
	applyLayout->addWidget (helpButton, 0);
	connect (helpButton, SIGNAL (clicked (bool)), this, SLOT (helpCallback( )));


	// Géométrie :
	QtOperationsFamilyToolBar*	geomFamilyToolbar	=
						new QtOperationsFamilyToolBar (subFamiliesBox, this);
	subFamilieslayout->addWidget (geomFamilyToolbar);
	geomFamilyToolbar->hide ( );
	QtMgx3DOperationsFamiliesAction*	fa	= new QtMgx3DOperationsFamiliesAction (
			QIcon (":/images/geometry.png"),
			QString::fromUtf8("Opérations géométriques"),
			getMainWindow ( ), *geomFamilyToolbar,
			QString::fromUtf8("Opérations géométriques de Magix 3D"));
	addOperationsFamilyAction (*fa);
	// Géométrie/Points :
	QtOperationsSubFamilyToolBar*	geomPointsFamilyToolbar	=
						new QtOperationsSubFamilyToolBar (operationsBox, this);
	operationsLayout->addWidget (geomPointsFamilyToolbar);
	QtMgx3DOperationsFamilyAction*	ofa	=
		new QtMgx3DOperationsFamilyAction (
			QIcon (":/images/create_point.png"),
			QString::fromUtf8("Création de points"),
			getMainWindow ( ), *geomPointsFamilyToolbar,
			QString::fromUtf8("Opérations géométriques associées aux points."));
	geomFamilyToolbar->addOperationsSubFamilyToolBar (*ofa);
	geomPointsFamilyToolbar->hide ( );
	registerSubFamilyToolBar (*geomPointsFamilyToolbar, GEOM_POINT_OPERATION);
	// Géométrie/Courbes :
	QtOperationsSubFamilyToolBar*	geomCurvesFamilyToolbar	=
						new QtOperationsSubFamilyToolBar (operationsBox, this);
	operationsLayout->addWidget (geomCurvesFamilyToolbar);
	ofa	= new QtMgx3DOperationsFamilyAction (
			QIcon (":/images/create_curve.png"),
			QString::fromUtf8("Création de courbes"),
			getMainWindow ( ), *geomCurvesFamilyToolbar,
			QString::fromUtf8("Opérations géométriques associées aux courbes."));
	geomFamilyToolbar->addOperationsSubFamilyToolBar (*ofa);
	geomCurvesFamilyToolbar->hide ( );
	registerSubFamilyToolBar (*geomCurvesFamilyToolbar, GEOM_CURVE_OPERATION);
	// Géométrie/Surfaces :
	QtOperationsSubFamilyToolBar*	geomSurfacesFamilyToolbar	=
						new QtOperationsSubFamilyToolBar (operationsBox, this);
	operationsLayout->addWidget (geomSurfacesFamilyToolbar);
	ofa	= new QtMgx3DOperationsFamilyAction (
			QIcon (":/images/create_surface.png"),
			QString::fromUtf8("Création de surfaces"),
			getMainWindow ( ), *geomSurfacesFamilyToolbar,
			QString::fromUtf8("Opérations géométriques associées aux surfaces."));
	geomFamilyToolbar->addOperationsSubFamilyToolBar (*ofa);
	geomSurfacesFamilyToolbar->hide ( );
	registerSubFamilyToolBar(*geomSurfacesFamilyToolbar,GEOM_SURFACE_OPERATION);
	// Géométrie/Volumes :
	QtOperationsSubFamilyToolBar*	geomVolumesFamilyToolbar	=
						new QtOperationsSubFamilyToolBar (operationsBox, this);
	operationsLayout->addWidget (geomVolumesFamilyToolbar);
	ofa	= new QtMgx3DOperationsFamilyAction (
			QIcon (":/images/create_volume.png"),
			QString::fromUtf8("Création de volumes"),
			getMainWindow ( ), *geomVolumesFamilyToolbar,
			QString::fromUtf8("Opérations géométriques associées aux volumes."));
	geomFamilyToolbar->addOperationsSubFamilyToolBar (*ofa);
	geomVolumesFamilyToolbar->hide ( );
	registerSubFamilyToolBar (*geomVolumesFamilyToolbar, GEOM_VOLUME_OPERATION);

	// Topologie :
	QtOperationsFamilyToolBar*	topoFamilyToolbar	=
						new QtOperationsFamilyToolBar (subFamiliesBox, this);
	subFamilieslayout->addWidget (topoFamilyToolbar);
	topoFamilyToolbar->hide ( );
	fa	= new QtMgx3DOperationsFamiliesAction (
			QIcon (":/images/topology.png"),
			QString::fromUtf8("Opérations topologiques"),
			getMainWindow ( ), *topoFamilyToolbar,
			QString::fromUtf8("Opérations topologiques de Magix 3D"));
	addOperationsFamilyAction (*fa);
	// Topologie/Vertex :
	QtOperationsSubFamilyToolBar*	topoVerticesFamilyToolbar	=
						new QtOperationsSubFamilyToolBar (operationsBox, this);
	operationsLayout->addWidget (topoVerticesFamilyToolbar);
	ofa	= new QtMgx3DOperationsFamilyAction (
			QIcon (":/images/create_vertex.png"),
			QString::fromUtf8("Création de sommets"),
			getMainWindow ( ), *topoVerticesFamilyToolbar,
			QString::fromUtf8("Opérations topologiques associées aux sommets."));
	topoFamilyToolbar->addOperationsSubFamilyToolBar (*ofa);
	topoVerticesFamilyToolbar->hide ( );
	registerSubFamilyToolBar (*topoVerticesFamilyToolbar,TOPO_VERTEX_OPERATION);
	// Topologie/Arêtes :
	QtOperationsSubFamilyToolBar*	topoEdgesFamilyToolbar	=
						new QtOperationsSubFamilyToolBar (operationsBox, this);
	operationsLayout->addWidget (topoEdgesFamilyToolbar);
	ofa	= new QtMgx3DOperationsFamilyAction (
			QIcon (":/images/create_edge.png"),
			QString::fromUtf8("Création de courbes"),
			getMainWindow ( ), *topoEdgesFamilyToolbar,
			QString::fromUtf8("Opérations topologiques associées aux arêtes."));
	topoFamilyToolbar->addOperationsSubFamilyToolBar (*ofa);
	topoEdgesFamilyToolbar->hide ( );
	registerSubFamilyToolBar (*topoEdgesFamilyToolbar, TOPO_EDGE_OPERATION);
	// Topologie/Faces :
	QtOperationsSubFamilyToolBar*	topoFacesFamilyToolbar	=
						new QtOperationsSubFamilyToolBar (operationsBox, this);
	operationsLayout->addWidget (topoFacesFamilyToolbar);
	ofa	= new QtMgx3DOperationsFamilyAction (
			QIcon (":/images/create_face.png"),
			QString::fromUtf8("Création de faces"),
			getMainWindow ( ), *topoFacesFamilyToolbar,
			QString::fromUtf8("Opérations topologiques associées aux faces."));
	topoFamilyToolbar->addOperationsSubFamilyToolBar (*ofa);
	topoFacesFamilyToolbar->hide ( );
	registerSubFamilyToolBar(*topoFacesFamilyToolbar, TOPO_FACE_OPERATION);
	// Topologie/Blocs :
	QtOperationsSubFamilyToolBar*	topoBlocksFamilyToolbar	=
						new QtOperationsSubFamilyToolBar (operationsBox, this);
	operationsLayout->addWidget (topoBlocksFamilyToolbar);
	ofa	= new QtMgx3DOperationsFamilyAction (
			QIcon (":/images/create_block.png"),
			QString::fromUtf8("Création de blocs"),
			getMainWindow ( ), *topoBlocksFamilyToolbar,
			QString::fromUtf8("Opérations topologiques associées aux blocs."));
	topoFamilyToolbar->addOperationsSubFamilyToolBar (*ofa);
	topoBlocksFamilyToolbar->hide ( );
	registerSubFamilyToolBar (*topoBlocksFamilyToolbar, TOPO_BLOCK_OPERATION);

	// Maillage :
	QtOperationsFamilyToolBar*	meshFamilyToolbar	=
						new QtOperationsFamilyToolBar (subFamiliesBox, this);
	subFamilieslayout->addWidget (meshFamilyToolbar);
	meshFamilyToolbar->hide ( );
	fa	= new QtMgx3DOperationsFamiliesAction (
			QIcon (":/images/meshing.png"),
			QString::fromUtf8("Opérations maillages"),
			getMainWindow ( ), *meshFamilyToolbar,
			QString::fromUtf8("Opérations maillages de Magix 3D"));
	addOperationsFamilyAction (*fa);
	// Maillage/Nuage :
	QtOperationsSubFamilyToolBar*	meshCloudsFamilyToolbar	=
						new QtOperationsSubFamilyToolBar (operationsBox, this);
	operationsLayout->addWidget (meshCloudsFamilyToolbar);
	ofa	= new QtMgx3DOperationsFamilyAction (
			QIcon (":/images/create_cloud.png"),
			QString::fromUtf8("Création de nuages"),
			getMainWindow ( ), *meshCloudsFamilyToolbar,
			QString::fromUtf8("Opérations maillages associées aux nuages."));
	meshFamilyToolbar->addOperationsSubFamilyToolBar (*ofa);
	meshCloudsFamilyToolbar->hide ( );
	registerSubFamilyToolBar (*meshCloudsFamilyToolbar, MESH_CLOUD_OPERATION);
	// Maillage/Surface :
	QtOperationsSubFamilyToolBar*	meshSurfacesFamilyToolbar	=
						new QtOperationsSubFamilyToolBar (operationsBox, this);
	operationsLayout->addWidget (meshSurfacesFamilyToolbar);
	ofa	= new QtMgx3DOperationsFamilyAction (
			QIcon (":/images/create_meshSurface.png"),
			QString::fromUtf8("Création de surfaces"),
			getMainWindow ( ), *meshSurfacesFamilyToolbar,
			QString::fromUtf8("Opérations maillages associées aux surfaces."));
	meshFamilyToolbar->addOperationsSubFamilyToolBar (*ofa);
	meshSurfacesFamilyToolbar->hide ( );
	registerSubFamilyToolBar (*meshSurfacesFamilyToolbar, MESH_SURFACE_OPERATION);
	// Maillage/Volume :
	QtOperationsSubFamilyToolBar*	meshVolumesFamilyToolbar	=
						new QtOperationsSubFamilyToolBar (operationsBox, this);
	operationsLayout->addWidget (meshVolumesFamilyToolbar);
	ofa	= new QtMgx3DOperationsFamilyAction (
			QIcon (":/images/create_meshVolume.png"),
			QString::fromUtf8("Création de volumes"),
			getMainWindow ( ), *meshVolumesFamilyToolbar,
			QString::fromUtf8("Opérations maillages associées aux volumes."));
	meshFamilyToolbar->addOperationsSubFamilyToolBar (*ofa);
	meshVolumesFamilyToolbar->hide ( );
	registerSubFamilyToolBar (*meshVolumesFamilyToolbar, MESH_VOLUME_OPERATION);

	// Repères :
	QtOperationsFamilyToolBar*	sysCoordFamilyToolbar	=
			new QtOperationsFamilyToolBar (subFamiliesBox, this);
	subFamilieslayout->addWidget (sysCoordFamilyToolbar);
	sysCoordFamilyToolbar->hide ( );
	fa	= new QtMgx3DOperationsFamiliesAction (
			QIcon (":/images/syscoord.png"),
			QString::fromUtf8("Opérations repères"),
			getMainWindow ( ), *sysCoordFamilyToolbar,
			QString::fromUtf8("Opérations repères de Magix 3D"));
	addOperationsFamilyAction (*fa);
	// Repère/Repère (une seule catégorie)
	QtOperationsSubFamilyToolBar*	sysCoordSubFamilyToolbar	=
			new QtOperationsSubFamilyToolBar (operationsBox, this);
	operationsLayout->addWidget (sysCoordSubFamilyToolbar);
	ofa	= new QtMgx3DOperationsFamilyAction (
			QIcon (":/images/syscoord.png"),
			QString::fromUtf8("Création de repères"),
			getMainWindow ( ), *sysCoordSubFamilyToolbar,
			QString::fromUtf8("Opérations associées aux repères."));
	sysCoordFamilyToolbar->addOperationsSubFamilyToolBar (*ofa);
	sysCoordSubFamilyToolbar->hide ( );
	registerSubFamilyToolBar (*sysCoordSubFamilyToolbar, SYSCOORD_OPERATION);



	// On dimensionne a minima certains widgets, par pure coqueterie !
	const QSize	familiesSize	= groupBox->layout ( )->sizeHint ( );
// Commit 1963 : on met fin à une certaine coqueterie, incompatible avec des
// "petits" écrans => pas de subFamiliesBox->setMinimumSize (200, 400)
// 1963	subFamiliesBox->setMinimumSize (QSize (200, 400));//familiesSize);
	subFamilieslayout->setSizeConstraint (QLayout::SetMinimumSize);
	operationsBox->setMinimumSize (familiesSize);
	operationsLayout->setSizeConstraint (QLayout::SetMinimumSize);
	applyAreaWidget->setMinimumSize (familiesSize);
	applyLayout->setSizeConstraint (QLayout::SetMinimumSize);

	// Par défaut la barre CAO est affichée :
	setOperationsFamilyToolBar (geomFamilyToolbar);
	setOperationsSubFamilyToolBar (geomPointsFamilyToolbar);

	resize (sizeHint ( ));
}	// QtMgx3DOperationsPanel::createGui


void QtMgx3DOperationsPanel::displayOperationPanelCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_showOperationPanelCheckbox)
	CHECK_NULL_PTR_ERROR (_operationPanelScrollArea)

	const bool	show	= Qt::Checked == _showOperationPanelCheckbox->checkState ( ) ? true : false;

	_operationPanelScrollArea->setVisible (show);
	if (0 != _operationPanel)
	{
		if (true == show)
			_operationPanel->autoUpdate ( );
		else
			_operationPanel->cancel ( );
	}	// if (0 != _operationPanel)

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : affichage du panneau opération")
}	// QtMgx3DOperationsPanel::displayOperationPanelCallback


void QtMgx3DOperationsPanel::resetCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _operationPanel)
	{
		// Au 22/09/17, cette méthode ne peut être appelée que via le bouton
		// Réinitialiser, et l'on veut repartir de 0 => highlight et preview
		// à false.
		_operationPanel->highlight (false);
		_operationPanel->preview (false, true);
		CHECK_NULL_PTR_ERROR (_operationPanel->getMgx3DOperationAction ( ))
		_operationPanel->reset ( );
	}	// if (0 != _operationPanel)

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : réinitialisation d'une opération")
}	// QtMgx3DOperationsPanel::resetCallback


void QtMgx3DOperationsPanel::autoUpdateCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _operationPanel)
	{
		// Au 22/09/17, cette méthode ne peut être appelée que via le bouton
		// Réinitialiser, et l'on veut repartir de 0 => highlight et preview
		// à false.
		_operationPanel->highlight (false);
		_operationPanel->preview (false, true);
		CHECK_NULL_PTR_ERROR (_operationPanel->getMgx3DOperationAction ( ))
		_operationPanel->autoUpdate ( );
	}	// if (0 != _operationPanel)

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : réinitialisation d'une opération")
}	// QtMgx3DOperationsPanel::autoUpdateCallback


void QtMgx3DOperationsPanel::applyCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _operationPanel)
	{
//		CHECK_NULL_PTR_ERROR (_operationPanel->getMgx3DOperationAction ( ))
//		_operationPanel->getMgx3DOperationAction ( )->executeOperation ( );
		_operationPanel->applyCallback ( );
	}	// if (0 != _operationPanel)

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : exécution d'une opération")
}	// QtMgx3DOperationsPanel::applyCallback


void QtMgx3DOperationsPanel::helpCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _operationPanel)
		QtHelpWindow::displayURL (_operationPanel->helpURL ( ), _operationPanel->helpTag ( ));
	else
		QtHelpWindow::displayURL (QtMgx3DApplication::HelpSystem::instance ( ).operationsURL);

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : aide contextuelle des opérations.")
}	// QtMgx3DOperationsPanel::helpCallback


void QtMgx3DOperationsPanel::addOperationsFamilyAction (QtMgx3DOperationsFamiliesAction& action)
{
	CHECK_NULL_PTR_ERROR (_operationsFamiliesToolBar)
	_operationsFamiliesToolBar->addOperationsFamilyToolBar (action);
}	// QtMgx3DOperationsPanel::addOperationsFamilyAction


void QtMgx3DOperationsPanel::setOperationsFamilyToolBar (QtOperationsFamilyToolBar* tb)
{
	if (tb != _currentFamilyToolBar)
	{
		if (0 != _currentFamilyToolBar)
			_currentFamilyToolBar->hide ( );
		_currentFamilyToolBar	= tb;
		if (0 != _currentFamilyToolBar)
		{
			_currentFamilyToolBar->show ( );
			setOperationsSubFamilyToolBar (
								_currentFamilyToolBar->getSubFamilyToolBar ( ));
		}	// if (0 != _currentFamilyToolBar)
		else
			setOperationsSubFamilyToolBar (0);
	}	// if (tb != _currentFamilyToolBar)
}	// QtMgx3DOperationsPanel::setOperationsFamilyToolBar


void QtMgx3DOperationsPanel::setOperationsSubFamilyToolBar (QtOperationsSubFamilyToolBar* tb)
{
	if (tb != _currentSubFamilyToolBar)
	{
		if (0 != _currentFamilyToolBar)
			_currentFamilyToolBar->setSubFamilyToolBar (tb);
		if (0 != _currentSubFamilyToolBar)
			_currentSubFamilyToolBar->hide ( );
		_currentSubFamilyToolBar	= tb;
		if (0 != _currentSubFamilyToolBar)
		{
			_currentSubFamilyToolBar->show ( );
			setOperationsPanel (_currentSubFamilyToolBar->getOperationPanel( ));
		}	// if (0 != _currentSubFamilyToolBar)
		else
			setOperationsPanel (0);
	}	// if (tb != _currentSubFamilyToolBar)
}	// QtMgx3DOperationsPanel::setOperationsSubFamilyToolBar


QWidget& QtMgx3DOperationsPanel::getOperationPanelParent ( )
{
	CHECK_NULL_PTR_ERROR (_currentOperationAreaWidget)
	return *_currentOperationAreaWidget;
}	// QtMgx3DOperationsPanel::getOperationPanelParent


QtMgx3DOperationPanel* QtMgx3DOperationsPanel::getOperationsPanel ( )
{
	return _operationPanel;
}	// QtMgx3DOperationsPanel::getOperationsPanel


void QtMgx3DOperationsPanel::updateLayoutWorkaround ( )
{	// Pour une raison inconnue, en RH5/Qt 4.7.4, les panneaux ne se
	// redimensionnent pas forcément lorsqu'on change de sous panneau, par
	// exemple suite à un choix à partir d'une combo box.
	// Cette fonction permet d'y remédier ...
#ifdef RHEL_5
	QtMgx3DOperationPanel*	panel	= _operationPanel;
	setOperationsPanel (0);
	if (0 != panel)
		setOperationsPanel (panel);
#endif	// RHEL_5
}	// QtMgx3DOperationsPanel::updateLayout


void QtMgx3DOperationsPanel::setOperationsPanel (QtMgx3DOperationPanel* panel)
{
	// On travaille dans une QScrollArea -> setWidget provoque la destruction du
	// widget actuellement scrollé, ce qu'on ne veut pas => takeWidget
	// préalable.
	// scrollArea->setWidget fait que le scroller devient parent du widget 
	// transmis en argument.
	if (panel != _operationPanel)
	{
		CHECK_NULL_PTR_ERROR (_operationPanelScrollArea)

		if (0 != _currentSubFamilyToolBar)
			_currentSubFamilyToolBar->setOperationPanel (panel);
		if (0 != _operationPanel)
			_operationPanel->hide ( );
		_operationPanelScrollArea->takeWidget ( );
		if (0 != panel)
			_operationPanelScrollArea->setWidget (panel);	// calls show
		_operationPanel	= panel;
		if (0 != _operationPanel)
			_currentOperationAreaWidget->updateGeometry ( );
		if (0 != _operationPanel)
			_operationPanelScrollArea->updateGeometry ( );
//		if (0 != _operationPanel)
//			_operationPanel->show ( );	
	}	// if (panel != _operationPanel)

	CHECK_NULL_PTR_ERROR (_reinitializeButton)
	CHECK_NULL_PTR_ERROR (_applyButton)
	_reinitializeButton->setEnabled (0 == _operationPanel ? false : true);
	_applyButton->setEnabled (0 == _operationPanel ? false : true);
}	// QtMgx3DOperationsPanel::setOperationsPanel


void QtMgx3DOperationsPanel::registerOperationAction (QtMgx3DOperationAction& action, OPERATION_TYPES type)
{
	getToolbar (type).addOperationAction (action);
}	// QtMgx3DOperationsPanel::registerOperationAction


void QtMgx3DOperationsPanel::addSeparator (OPERATION_TYPES type)
{
	getToolbar (type).addSeparator ( );
}	// QtMgx3DOperationsPanel::addSeparator


void QtMgx3DOperationsPanel::registerAdditionalOperationPanel (QtMgx3DOperationPanel& panel)
{
	CHECK_NULL_PTR_ERROR (_operationsTabWidget)
	_operationsTabWidget->addTab (&panel, panel.windowTitle ( ));
	_operationsTabWidget->setCurrentWidget (&panel);
}	// QtMgx3DOperationsPanel::registerAdditionalOperationPanel


void QtMgx3DOperationsPanel::unregisterAdditionalOperationPanel (QtMgx3DOperationPanel& panel)
{
	CHECK_NULL_PTR_ERROR (_operationsTabWidget)
	// panel est en cours de destruction, rien d'autre à faire pour le moment.
}	// QtMgx3DOperationsPanel::unregisterAdditionalOperationPanel


void QtMgx3DOperationsPanel::setLogStream (LogOutputStream* stream)
{
	_logStream	= stream;
}	// QtMgx3DOperationsPanel::setLogStream


void QtMgx3DOperationsPanel::log (const Log& log)
{
	if (0 != _logStream)
		_logStream->log (log);
}	// QtMgx3DOperationsPanel::log


void QtMgx3DOperationsPanel::registerSubFamilyToolBar (QtOperationsSubFamilyToolBar& toolBar, OPERATION_TYPES type)
{
	try
	{
		// Déjà recensée ???
		getToolbar (type);
		UTF8String	message (Charset::UTF_8);
		message << "Barre d'icônes déjà recensée pour le type "
		        << (unsigned long)type << ".";
		INTERNAL_ERROR (exc, message.iso ( ), "QtMgx3DOperationsPanel::registerSubFamilyToolBar")
		throw exc;
	}
	catch (...)
	{
		_operationsToolbars.insert (pair<OPERATION_TYPES, QtOperationsSubFamilyToolBar*> (type, &toolBar));
	}
}	// QtMgx3DOperationsPanel::registerSubFamilyToolBar


QtOperationsSubFamilyToolBar& QtMgx3DOperationsPanel::getToolbar (OPERATION_TYPES type)
{
	map<OPERATION_TYPES, QtOperationsSubFamilyToolBar*>::iterator	it	= _operationsToolbars.find (type);
	if (_operationsToolbars.end ( ) == it)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Absence de barre d'icônes pour le type " << (unsigned long)type << ".";
		INTERNAL_ERROR (
					exc, message.iso ( ), "QtMgx3DOperationsPanel::getToolbar")
		throw exc;
	}
	else
		return *((*it).second);
}	// QtMgx3DOperationsPanel::getToolbar


// ===========================================================================
//                     LA CLASSE QtMgx3DOperationsSubPanel
// ===========================================================================


QtMgx3DOperationsSubPanel::QtMgx3DOperationsSubPanel (QWidget* parent, QtMgx3DMainWindow& mainWindow, QtMgx3DOperationPanel* mainPanel)
	: QtMgx3DOperationPanel (parent, mainWindow, 0, "", ""),
	  _mainPanel (mainPanel)
{
}	// QtMgx3DOperationsSubPanel::QtMgx3DOperationsSubPanel


QtMgx3DOperationsSubPanel::QtMgx3DOperationsSubPanel (const QtMgx3DOperationsSubPanel&)
	: QtMgx3DOperationPanel (0, *new QtMgx3DMainWindow (0), 0, "", ""),
	  _mainPanel (0)
{
	assert ("QtMgx3DOperationsSubPanel copy constructor is not allowed.");
}	// QtMgx3DOperationsSubPanel::QtMgx3DOperationsSubPanel


QtMgx3DOperationsSubPanel& QtMgx3DOperationsSubPanel::operator = (const QtMgx3DOperationsSubPanel&)
{
	assert ("QtMgx3DOperationsSubPanel assignment operator is not allowed.");
	return *this;
}	// QtMgx3DOperationsSubPanel::operator =


QtMgx3DOperationsSubPanel::~QtMgx3DOperationsSubPanel ( )
{
}	// QtMgx3DOperationsSubPanel::~QtMgx3DOperationsSubPanel


void QtMgx3DOperationsSubPanel::stopSelection ( )
{
}	// QtMgx3DOperationsSubPanel::stopSelection


bool QtMgx3DOperationsSubPanel::previewResult ( ) const
{
	if (0 != getMainPanel ( ))
		return getMainPanel ( )->previewResult ( );

	return QtMgx3DOperationPanel::previewResult ( );
}	// QtMgx3DOperationsSubPanel::previewResult


const QtMgx3DOperationPanel* QtMgx3DOperationsSubPanel::getMainPanel ( ) const
{
	return _mainPanel;
}	// QtMgx3DOperationsSubPanel::getMainPanel


QtMgx3DOperationPanel* QtMgx3DOperationsSubPanel::getMainPanel ( )
{
	return _mainPanel;
}	// QtMgx3DOperationsSubPanel::getMainPanel


vector<Entity*> QtMgx3DOperationsSubPanel::getPanelEntities ( )
{
	return getInvolvedEntities ( );
}	// QtMgx3DOperationsSubPanel::getPanelEntities


void QtMgx3DOperationsSubPanel::entitiesSelectedCallback (QString names)
{
	emit entitiesAddedToSelection (names);
}	// QtMgx3DOperationsSubPanel::entitiesSelectedCallback


void QtMgx3DOperationsSubPanel::entitiesUnselectedCallback (QString names)
{
	emit entitiesRemovedFromSelection (names);
}	// QtMgx3DOperationsSubPanel::entitiesUnselectedCallback


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
