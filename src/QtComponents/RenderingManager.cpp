/**
 * \file        RenderingManager.cpp
 * \author      Charles PIGNEROL
 * \date        04/06/2012
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"
#include "QtComponents/RenderingManager.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Geom/GeomEntity.h"
#include "Topo/TopoDisplayRepresentation.h"
#include "Topo/TopoEntity.h"
#include "Topo/CommandEditTopo.h"
#include "Utils/Common.h"
#include "Utils/DisplayProperties.h"
#include "Utils/Vector.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/NumericServices.h>

#include <iostream>
#include <memory>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Internal;
using namespace Mgx3D::Group;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Mesh;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtComponents
{

template <typename T> void addToList (vector<Entity*>& entities, vector<T*>& ts, bool clear)
{
	if (true == clear)
		entities.clear ( );

	// On aide le compilo :
	typedef typename vector< T* >::iterator	ItEntityType;
	for (ItEntityType it = ts.begin ( ); ts.end ( ) != it; it++)
		entities.push_back ((Entity*)*it);
}	// addToList


// ===========================================================================
//        LA STRUCTURE RenderingManager::ColorTableDefinition
// ===========================================================================

RenderingManager::ColorTableDefinition::ColorTableDefinition ( )
	: valueName ("Invalid color table"), colorNum (USHRT_MAX),
	  domainMin (-NumericServices::doubleMachMax()), domainMax (NumericServices::doubleMachMax())

{
}	// ColorTableDefinition::ColorTableDefinition

	
RenderingManager::ColorTableDefinition::ColorTableDefinition (
					const string& name, size_t colors, double min, double max)
	: valueName (name), colorNum (colors), domainMin (min), domainMax (max)
{
}	// ColorTableDefinition::ColorTableDefinition


RenderingManager::ColorTableDefinition::ColorTableDefinition (
					const RenderingManager::ColorTableDefinition& ctd)
	: valueName (ctd.valueName), colorNum (ctd.colorNum),
	  domainMin (ctd.domainMin), domainMax (ctd.domainMax)
{
}	// ColorTableDefinition::ColorTableDefinition


RenderingManager::ColorTableDefinition&
		RenderingManager::ColorTableDefinition::operator = (
							const RenderingManager::ColorTableDefinition& ctd)
{
	if (this != &ctd)
	{
		valueName		= ctd.valueName;
		colorNum		= ctd.colorNum;
		domainMin		= ctd.domainMin;
		domainMax		= ctd.domainMax;
	}	// if (this != &ctd)

	return *this;
}	// ColorTableDefinition::operator =


RenderingManager::ColorTableDefinition::~ColorTableDefinition ( )
{
}	// ColorTableDefinition::~ColorTableDefinition


bool RenderingManager::ColorTableDefinition::operator == (
				const RenderingManager::ColorTableDefinition& ctd) const
{
	if (valueName != ctd.valueName)
		return false;
	if (colorNum != ctd.colorNum)
		return false;
	if (fabs (domainMin - ctd.domainMin) > NumericServices::doubleMachMin())
		return false;
	if (fabs (domainMax - ctd.domainMax) > NumericServices::doubleMachMin())
		return false;

	return true;
}	// ColorTableDefinition::operator ==


bool RenderingManager::ColorTableDefinition::operator != (
				const RenderingManager::ColorTableDefinition& ctd) const
{
	return ! (*this == ctd);
}	// ColorTableDefinition::operator !=


void RenderingManager::ColorTableDefinition::extent (
							const RenderingManager::ColorTableDefinition& ctd)
{
	extentDomain (ctd.domainMin, ctd.domainMax);
	colorNum	= colorNum > ctd.colorNum ? colorNum : ctd.colorNum;
}	// ColorTableDefinition::extent


void RenderingManager::ColorTableDefinition::extentDomain (
														double min, double max)
{
	domainMin	= min < domainMin ? min : domainMin;
	domainMax	= max > domainMax ? max : domainMax;
}	// ColorTableDefinition::extentDomain


bool RenderingManager::ColorTableDefinition::comparable (
							const RenderingManager::ColorTableDefinition& ctd)
{
	return valueName == ctd.valueName ? true : false;
}	// ColorTableDefinition::comparable


// ===========================================================================
//                 LA CLASSE RenderingManager::ColorTable
// ===========================================================================

RenderingManager::ColorTable::ColorTable (
	RenderingManager& rm, const RenderingManager::ColorTableDefinition& def)
	: _renderingManager (&rm), _definition (def)
{
}	// ColorTable::ColorTable


RenderingManager::ColorTable::ColorTable (const RenderingManager::ColorTable& ct)
	: _renderingManager (0), _definition (ct.getDefinition ( ))
{
	MGX_FORBIDDEN ("RenderingManager::ColorTable copy constructor is not allowed.");
}	// ColorTable::ColorTable


RenderingManager::ColorTable& RenderingManager::ColorTable::operator = (
										const RenderingManager::ColorTable&)
{
	MGX_FORBIDDEN ("RenderingManager::ColorTable assignment operator is not allowed.");
	return *this;
}	// ColorTable::operator =


RenderingManager::ColorTable::~ColorTable ( )
{
}	// ColorTable::~ColorTable


void RenderingManager::ColorTable::display (bool show)
{
	throw Exception (UTF8String ("ColorTable::display should be overloaded.", Charset::UTF_8));
}	// ColorTable::display


size_t RenderingManager::ColorTable::getColorNum ( ) const
{
	return _definition.colorNum;
}	// ColorTable::getColorNum


void RenderingManager::ColorTable::setColorNum (size_t num)
{
	_definition.colorNum	= num;
}	// ColorTable::setColorNum


double RenderingManager::ColorTable::getMinValue ( ) const
{
	return _definition.domainMin;
}	// ColorTable::getMinValue


void RenderingManager::ColorTable::setMinValue (double min)
{
	setExtrema (min, _definition.domainMax);
}	// ColorTable::setMinValue


double RenderingManager::ColorTable::getMaxValue ( ) const
{
	return _definition.domainMax;
}	// ColorTable::getMaxValue


void RenderingManager::ColorTable::setMaxValue (double max)
{
	setExtrema (_definition.domainMin, max);
}	// ColorTable::setMaxValue


void RenderingManager::ColorTable::getExtrema (double& min, double& max) const
{
	min	= getMinValue ( );
	max	= getMaxValue ( );
}	// ColorTable::getExtrema


void RenderingManager::ColorTable::setExtrema (double min, double max)
{
	_definition.domainMin	= min;
	_definition.domainMax	= max;
}	// ColorTable::setExtrema


string RenderingManager::ColorTable::getValueName ( ) const
{
	return _definition.valueName;
}	// ColorTable::getValueName


void  RenderingManager::ColorTable::setValueName (const string& name)
{
	_definition.valueName	= name;
}	// ColorTable::setValueName


RenderingManager::ColorTableDefinition
						RenderingManager::ColorTable::getDefinition ( ) const
{
	return _definition;
}	// ColorTable::getDefinition


void RenderingManager::ColorTable::setDefinition (
			const RenderingManager::ColorTableDefinition& definition)
{
	if (_definition != definition)
		_definition	= definition;
}	// ColorTable::setDefinition


const RenderingManager&
					RenderingManager::ColorTable::getRenderingManager ( ) const
{
	CHECK_NULL_PTR_ERROR (_renderingManager)
	return *_renderingManager;
}	// ColorTable::getRenderingManager


RenderingManager& RenderingManager::ColorTable::getRenderingManager ( )
{
	CHECK_NULL_PTR_ERROR (_renderingManager)
	return *_renderingManager;
}	// ColorTable::getRenderingManager


// ===========================================================================
//                 LA CLASSE RenderingManager::DisplayLocker
// ===========================================================================


RenderingManager::DisplayLocker::DisplayLocker (
								RenderingManager& w, bool forceRenderAtEnd)
	: _3dwidget (&w), _forceRenderAtEnd (forceRenderAtEnd)
{
	w.lockDisplay (true);
}	// RenderingManager::DisplayLocker


RenderingManager::DisplayLocker::DisplayLocker (
									const RenderingManager::DisplayLocker&)
	: _3dwidget (0), _forceRenderAtEnd (false)
{
}	// DisplayLocker::DisplayLocker


RenderingManager::DisplayLocker&
			RenderingManager::DisplayLocker::operator = (
									const RenderingManager::DisplayLocker&)
{
	return *this;
}	// DisplayLocker::operator =


RenderingManager::DisplayLocker::~DisplayLocker ( )
{
	if (0 != _3dwidget)
	{
		_3dwidget->lockDisplay (false);
		if (true == _forceRenderAtEnd)
			_3dwidget->forceRender ( );
	}	// if (0 != _3dwidget)
}	// DisplayLocker::~DisplayLocker

// Optimisation visant à gérer les ordres d'affichage par lots.
// Ex : on a 5000 entités à afficher : on ne provoque des ordres d'actualisation
// de la vue 3D que toutes les 100 entités affichées.
#define DECLARE_DISPLAY_LOCKER                                                \
	unique_ptr<DisplayLocker>	displayLocker (new DisplayLocker (*this));        \
	size_t					displayLockerLoop	= 0;

// On veut unlock avant lock => en 2 temps (reset (0) puis	
// reset (new DisplayLocker (...)).
// Si directement reset (new DisplayLocker (...))  on a
// new DisplayLocker (...)
// puis delete de l'ancien DisplayLocker => unlock en dernier ...
#define EVALUATE_DISPLAY_LOCKER                                                            \
	displayLockerLoop++;                                                                   \
	if (0 == displayLockerLoop %                                                           \
						Resources::instance ( )._updateRefreshRate.getValue ( ))  \
	{                                                                                      \
		displayLocker.reset (0);                                                           \
		displayLocker.reset (new DisplayLocker (*this));                                   \
		/* Eventuelle actualisation IHM : */                                               \
		/*if (true == QApplication::hasPendingEvents ( ))     */                           \
			QtMgx3DApplication::processEvents (5000);                                      \
			/*QApplication::processEvents (QEventLoop::AllEvents, 5000);*/                 \
	}	/* if (0 == displayLockerLoop % QtMgx3DApplication:: ... */


// ===========================================================================
//               LA CLASSE RenderingManager::InteractorObserver
// ===========================================================================

RenderingManager::InteractorObserver::InteractorObserver ( )
{
}	// InteractorObserver::InteractorObserver


RenderingManager::InteractorObserver::InteractorObserver (
									const RenderingManager::InteractorObserver&)
{
	MGX_FORBIDDEN ("RenderingManager::InteractorObserver copy constructor is not allowed.");
}	// InteractorObserver::InteractorObserver


RenderingManager::InteractorObserver&
		RenderingManager::InteractorObserver::operator = (
									const RenderingManager::InteractorObserver&)
{
	MGX_FORBIDDEN ("RenderingManager::InteractorObserver assignment operator is not allowed.");
	return *this;
}	// InteractorObserver::operator =


RenderingManager::InteractorObserver::~InteractorObserver ( )
{
}	// InteractorObserver::~InteractorObserver


void RenderingManager::InteractorObserver::pointModifiedCallback (
												Mgx3D::Utils::Math::Point point)
{
}	// InteractorObserver::pointModifiedCallback


void RenderingManager::InteractorObserver::planeModifiedCallback (
			Mgx3D::Utils::Math::Point point, Mgx3D::Utils::Math::Vector normal)
{
}	// InteractorObserver::planeModifiedCallback


// ===========================================================================
//                  LA CLASSE RenderingManager::Interactor
// ===========================================================================

RenderingManager::Interactor::Interactor (
								RenderingManager::InteractorObserver* observer)
	: _observer (observer)
{
}	// Interactor::Interactor


RenderingManager::Interactor::Interactor (const RenderingManager::Interactor&)
	: _observer (0)
{
	MGX_FORBIDDEN ("RenderingManager::Interactor assignment operator is not allowed.");
}	// Interactor::Interactor


RenderingManager::Interactor& RenderingManager::Interactor::operator = (
											const RenderingManager::Interactor&)
{
	MGX_FORBIDDEN ("RenderingManager::Interactor assignment operator is not allowed.");
	return *this;
}	// RenderingManager::Interactor::operator =


RenderingManager::Interactor::~Interactor ( )
{
}	// Interactor::~Interactor


RenderingManager::InteractorObserver*
								RenderingManager::Interactor::getObserver ( )
{
	return _observer;
}	// RenderingManager::Interactor


void RenderingManager::Interactor::notifyObserverForModifications ( )
{
	throw Exception (UTF8String ("RenderingManager::Interactor::notifyObserverForModifications should be overloaded.", Charset::UTF_8));
}	// Interactor::notifyObserverForModifications


// ===========================================================================
//                  LA CLASSE RenderingManager::PointInteractor
// ===========================================================================

RenderingManager::PointInteractor::PointInteractor (
								RenderingManager::InteractorObserver* observer)
	: RenderingManager::Interactor (observer)
{
}	// PointInteractor::PointInteractor


RenderingManager::PointInteractor::PointInteractor (
									const RenderingManager::PointInteractor&)
	: RenderingManager::Interactor (0)
{
	MGX_FORBIDDEN ("RenderingManager::PointInteractor assignment operator is not allowed.");
}	// PointInteractor::PointInteractor


RenderingManager::PointInteractor&
				RenderingManager::PointInteractor::operator = (
									const RenderingManager::PointInteractor&)
{
	MGX_FORBIDDEN ("RenderingManager::PointInteractor assignment operator is not allowed.");
	return *this;
}	// RenderingManager::PointInteractor::operator =


RenderingManager::PointInteractor::~PointInteractor ( )
{
}	// PointInteractor::~PointInteractor


Math::Point RenderingManager::PointInteractor::getPoint ( ) const
{
	throw Exception (UTF8String ("PointInteractor::getPoint should be overloaded.", Charset::UTF_8));
}	// PointInteractor::getPoint


void RenderingManager::PointInteractor::setPoint (Math::Point point)
{
	throw Exception (UTF8String ("PointInteractor::setPoint should be overloaded.", Charset::UTF_8));
}	// PointInteractor::setPoint


void RenderingManager::PointInteractor::getBoxDimensions (
									double& dx, double& dy, double& dz) const
{
	throw Exception (UTF8String ("PointInteractor::getBoxDimensions should be overloaded.", Charset::UTF_8));
}	// PointInteractor::getBoxDimensions


void RenderingManager::PointInteractor::setBoxDimensions (
											double dx, double dy, double dz)
{
	throw Exception (UTF8String ("PointInteractor::setBoxDimensions should be overloaded.", Charset::UTF_8));
}	// PointInteractor::setBoxDimensions


void RenderingManager::PointInteractor::notifyObserverForModifications ( )
{
	if (0 != getObserver ( ))
		getObserver ( )->pointModifiedCallback (getPoint ( ));
}	// PointInteractor::notifyObserverForModifications


// ===========================================================================
//                  LA CLASSE RenderingManager::ConstrainedPointInteractor
// ===========================================================================

RenderingManager::ConstrainedPointInteractor::ConstrainedPointInteractor (
								RenderingManager::InteractorObserver* observer)
	: RenderingManager::Interactor (observer)
{
}	// ConstrainedPointInteractor::ConstrainedPointInteractor


RenderingManager::ConstrainedPointInteractor::ConstrainedPointInteractor (
									const RenderingManager::ConstrainedPointInteractor&)
	: RenderingManager::Interactor (0)
{
	MGX_FORBIDDEN ("RenderingManager::ConstrainedPointInteractor assignment operator is not allowed.");
}	// ConstrainedPointInteractor::ConstrainedPointInteractor


RenderingManager::ConstrainedPointInteractor&
				RenderingManager::ConstrainedPointInteractor::operator = (
									const RenderingManager::ConstrainedPointInteractor&)
{
	MGX_FORBIDDEN ("RenderingManager::ConstrainedPointInteractor assignment operator is not allowed.");
	return *this;
}	// RenderingManager::ConstrainedPointInteractor::operator =


RenderingManager::ConstrainedPointInteractor::~ConstrainedPointInteractor ( )
{
}	// ConstrainedPointInteractor::~ConstrainedPointInteractor


Math::Point RenderingManager::ConstrainedPointInteractor::getPoint ( ) const
{
	throw Exception (UTF8String ("ConstrainedPointInteractor::getPoint should be overloaded.", Charset::UTF_8));
}	// ConstrainedPointInteractor::getPoint


void RenderingManager::ConstrainedPointInteractor::setPoint (Math::Point point)
{
	throw Exception (UTF8String ("ConstrainedPointInteractor::setPoint should be overloaded.", Charset::UTF_8));
}	// ConstrainedPointInteractor::setPoint


void RenderingManager::ConstrainedPointInteractor::setConstraint (Entity* constraint, size_t factor)
{
	throw Exception (UTF8String ("ConstrainedPointInteractor::setConstraint should be overloaded.", Charset::UTF_8));
}	// ConstrainedPointInteractor::setConstraint


void RenderingManager::ConstrainedPointInteractor::notifyObserverForModifications ( )
{
	if (0 != getObserver ( ))
		getObserver ( )->pointModifiedCallback (getPoint ( ));
}	// ConstrainedPointInteractor::notifyObserverForModifications


// ===========================================================================
//                  LA CLASSE RenderingManager::PlaneInteractor
// ===========================================================================

RenderingManager::PlaneInteractor::PlaneInteractor (RenderingManager::InteractorObserver* observer)
	: RenderingManager::Interactor (observer)
{
}	// PlaneInteractor::PlaneInteractor


RenderingManager::PlaneInteractor::PlaneInteractor (const RenderingManager::PlaneInteractor&)
	: RenderingManager::Interactor (0)
{
	MGX_FORBIDDEN ("RenderingManager::PlaneInteractor assignment operator is not allowed.");
}	// PlaneInteractor::PlaneInteractor


RenderingManager::PlaneInteractor&
				RenderingManager::PlaneInteractor::operator = (
									const RenderingManager::PlaneInteractor&)
{
	MGX_FORBIDDEN ("RenderingManager::PlaneInteractor assignment operator is not allowed.");
	return *this;
}	// RenderingManager::PlaneInteractor::operator =


RenderingManager::PlaneInteractor::~PlaneInteractor ( )
{
}	// PlaneInteractor::~PlaneInteractor


Math::Point RenderingManager::PlaneInteractor::getPoint ( ) const
{
	throw Exception (UTF8String ("PlaneInteractor::getPoint should be overloaded.", Charset::UTF_8));
}	// PlaneInteractor::getPoint


Math::Vector RenderingManager::PlaneInteractor::getNormal ( ) const
{
	throw Exception (UTF8String ("PlaneInteractor::getNormal should be overloaded.", Charset::UTF_8));
}	// PlaneInteractor::getNormal


void RenderingManager::PlaneInteractor::setPlane (Math::Point point, Math::Vector normal)
{
	throw Exception (UTF8String ("PlaneInteractor::setPlane should be overloaded.", Charset::UTF_8));
}	// PlaneInteractor::setPlane


void RenderingManager::PlaneInteractor::notifyObserverForModifications ( )
{
	if (0 != getObserver ( ))
		getObserver ( )->planeModifiedCallback (getPoint ( ), getNormal ( ));
}	// PlaneInteractor::notifyObserverForModifications


// ===========================================================================
//                 LA CLASSE RenderingManager::CommandPreviewMgr
// ===========================================================================

RenderingManager::CommandPreviewMgr::CommandPreviewMgr (
	CommandInternal& command, RenderingManager& renderingManager, bool begin)
	: _command (&command), _renderingManager (&renderingManager), _ids ( )
{
	if (true == begin)
	{
		CommandEditTopo*	cet	=
								dynamic_cast<CommandEditTopo*>(&getCommand ( ));
		if (0 != cet)
			cet->previewBegin ( );
	}	// if (true == begin)
}	// CommandPreviewMgr::CommandPreviewMgr


RenderingManager::CommandPreviewMgr::CommandPreviewMgr ( )
	: _command (0), _renderingManager (0), _ids ( )
{
}	// CommandPreviewMgr::CommandPreviewMgr


RenderingManager::CommandPreviewMgr::CommandPreviewMgr (const RenderingManager::CommandPreviewMgr&)
	: _command (0), _renderingManager (0), _ids ( )
{
	MGX_FORBIDDEN ("CommandPreviewMgr copy constructor is not allowed.");
}	// CommandPreviewMgr::CommandPreviewMgr


RenderingManager::CommandPreviewMgr& RenderingManager::CommandPreviewMgr::operator = (
									const RenderingManager::CommandPreviewMgr&)
{
	MGX_FORBIDDEN ("CommandPreviewMgr assignment operator is not allowed.");
	return *this;
}	// CommandPreviewMgr::operator =


RenderingManager::CommandPreviewMgr::~CommandPreviewMgr ( )
{
	CommandEditTopo*	cet	= dynamic_cast<CommandEditTopo*>(&getCommand ( ));
	if (0 != cet)
		cet->previewEnd ( );
	const vector<RenderingManager::RepresentationID>	ids	= getRepresentationsIDs ( );
	for (vector<RenderingManager::RepresentationID>::const_iterator
			it = ids.begin ( ); ids.end ( ) != it; it++)
			getRenderingManager ( ).displayRepresentation (*it, true);
}	// CommandPreviewMgr::::~CommandPreviewMgr


void RenderingManager::CommandPreviewMgr::registerRepresentationID (RenderingManager::RepresentationID id)
{
	if (0 != id)	// Peut arriver si segment nul par exemple
		_ids.push_back (id);
}	// CommandPreviewMgr::registerRepresentationID


CommandInternal& RenderingManager::CommandPreviewMgr::getCommand ( )
{
	CHECK_NULL_PTR_ERROR (_command)
	return *_command;
}	// CommandPreviewMgr::getCommand


RenderingManager& RenderingManager::CommandPreviewMgr::getRenderingManager ( )
{
	CHECK_NULL_PTR_ERROR (_renderingManager)
	return *_renderingManager;
}	// CommandPreviewMgr::getRenderingManager


vector<RenderingManager::RepresentationID>
			RenderingManager::CommandPreviewMgr::getRepresentationsIDs ( ) const
{
	return _ids;
}	// CommandPreviewMgr::getRepresentationsIDs


// ===========================================================================
//                        LA CLASSE RenderingManager
// ===========================================================================

RenderingManager::RenderingManager ( )
	: SelectionManagerObserver (0),
	  _context (0), _displayLocked (false), _useGlobalDisplayProperties (true), _displayedTypes (FilterEntity::NoneEntity), _topoUseGeomColor (false),
	  _structuredDataThreshold (-NumericServices::doubleMachInfinity ( ))
{
}	// RenderingManager::RenderingManager


RenderingManager::RenderingManager (const RenderingManager&)
	: SelectionManagerObserver (0),
	  _context (0), _displayLocked (false), _useGlobalDisplayProperties (true), _displayedTypes (FilterEntity::NoneEntity), _topoUseGeomColor (false),
	  _structuredDataThreshold (-NumericServices::doubleMachInfinity ( ))
{
	MGX_FORBIDDEN ("RenderingManager copy constructor is not allowed.");
}	// RenderingManager::RenderingManager (const RenderingManager&)


RenderingManager& RenderingManager::operator = (const RenderingManager&)
{
	MGX_FORBIDDEN ("RenderingManager assignment operator is not allowed.");
	return *this;
}	// RenderingManager::RenderingManager (const RenderingManager&)


RenderingManager::~RenderingManager ( )
{
	_context	= 0;
}	// RenderingManager::~RenderingManager


Context& RenderingManager::getContext ( )
{
	if (0 == _context)
		throw Exception (UTF8String ("RenderingManager::getContext : absence de contexte Magix 3D.", Charset::UTF_8));

	return *_context;
}	// RenderingManager::getContext


const Context& RenderingManager::getContext ( ) const
{
	if (0 == _context)
		throw Exception (UTF8String ("RenderingManager::getContext : absence de contexte Magix 3D.", Charset::UTF_8));

	return *_context;
}	// RenderingManager::getContext


void RenderingManager::setContext (Context* context)
{
	if (_context != context)
	{
		_context	= context;
		try
		{
			// Rem CP : getBackground ( ) sous try/catch car en mode batch
			// il n'y a pas de gestionnaire de rendu => exception.
			if (0 != _context)
				_context->setBackground (getBackground ( ));
		}
		catch (const Exception& exc)
		{
			cerr << "RenderingManager::setContext. Actualisation de la couleur de fond impossible : "
			     << exc.getFullMessage ( ) << endl;
		}
		catch (...)
		{
		}
	}	// if (_context != context)
}	// RenderingManager::setContext


void RenderingManager::addEntity (Entity& entity)
{
/*
	GraphicalEntityRepresentation*	representation	=
												createRepresentation (entity);
	CHECK_NULL_PTR_ERROR (representation)
	entity.getDisplayProperties ( ).setGraphicalRepresentation (representation);
*/
}	// RenderingManager::addEntity


void RenderingManager::addEntities (const vector<Entity*>& entities, bool shown, DisplayRepresentation::display_type type)
{
	DECLARE_DISPLAY_LOCKER

	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		addEntity (**it);

		EVALUATE_DISPLAY_LOCKER
	}	// for (vector<Entity*>::const_iterator it = entities.begin ( );
}	// RenderingManager::addEntities


void RenderingManager::removeEntity (Entity& entity)
{
	DisplayProperties::GraphicalRepresentation*	gr	=
				entity.getDisplayProperties ( ).getGraphicalRepresentation ( );
	if (0 != gr)
	{
		GraphicalEntityRepresentation*	ger	=
							dynamic_cast<GraphicalEntityRepresentation*>(gr);
		displayRepresentation (*ger, false, false, 0);
	}	// if (0 ! = gr)
}	// RenderingManager::removeEntity


void RenderingManager::removeEntities (const vector<Entity*>& entities, DisplayRepresentation::display_type type)
{
	DECLARE_DISPLAY_LOCKER

	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		removeEntity (**it);

		EVALUATE_DISPLAY_LOCKER
	}	// for (vector<Entity*>::const_iterator it = entities.begin ( ); ...
}	// RenderingManager::removeEntities


void RenderingManager::displayRepresentation (Entity& entity, bool show, unsigned long mask)
{
	bool	first	= !hasDisplayedRepresentation ( );
	GraphicalEntityRepresentation*	representation	= getRepresentation(entity);
	if (0 == representation)
		representation	= createRepresentation (entity);
	CHECK_NULL_PTR_ERROR (representation)

	if (0 != getSelectionManager ( ))
		representation->setSelected(getSelectionManager( )->isSelected(entity));
	bool	forceUpdate	=
		(true==show) && (false==entity.getDisplayProperties( ).isDisplayed( )) ?
		true : false;
	entity.getDisplayProperties ( ).setDisplayed (show);
	entity.getDisplayProperties ( ).setGraphicalRepresentation (representation);
	displayRepresentation (*representation, show, forceUpdate, mask);

// resetView inhibé car perd la vue lors du changement de filtre sur le type
// d'entités affichées (MGXDDD-236).
//	if (true == first)
//		resetView (true);
}	// RenderingManager::displayRepresentation


void RenderingManager::displayRepresentations (
		const vector<Entity*>& entities, bool show, unsigned long mask,
		DisplayRepresentation::display_type type)
{
	DECLARE_DISPLAY_LOCKER

	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		// CP : Nouveau code : on utilise le masque pré-existant s'il y en a un
		unsigned long	currentMask	=
			0 == (*it)->getDisplayProperties( ).getGraphicalRepresentation( ) ?
			mask : (*it)->getDisplayProperties ( ).getGraphicalRepresentation ( )->getRepresentationMask ( );
		displayRepresentation (**it, show, currentMask);

		EVALUATE_DISPLAY_LOCKER
	}	// RenderingManager::displayRepresentations
}	// RenderingManager::displayRepresentations


void RenderingManager::displayRepresentationsSelectedEntities (bool show)
{
	DECLARE_DISPLAY_LOCKER

	vector<Entity*> entities = getContext().getSelectionManager().getEntities();

	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		// CP : Nouveau code : on utilise le masque pré-existant s'il y en a un
		unsigned long	currentMask	=
			0 == (*it)->getDisplayProperties( ).getGraphicalRepresentation( ) ?
			0 : (*it)->getDisplayProperties ( ).getGraphicalRepresentation ( )->getRepresentationMask ( );
		displayRepresentation (**it, show, currentMask);

		EVALUATE_DISPLAY_LOCKER
	}
}	// RenderingManager::displayRepresentationsSelectedEntities


void RenderingManager::displayRepresentations (const vector<GroupEntity*>& groups, bool show)
{
	vector<Group::GroupEntity*>			groupAddedShown, groupAddedHidden;
	vector<Geom::GeomEntity*>			geomAddedShown, geomAddedHidden;
	vector<Topo::TopoEntity*>			topoAddedShown, topoAddedHidden;
	vector<Mesh::MeshEntity*>			meshAddedShown, meshAddedHidden;
	vector<CoordinateSystem::SysCoord*>		sysCoordAddedShown, sysCoordAddedHidden;
	vector<Structured::StructuredMeshEntity*>	sMeshAddedShown, sMeshAddedHidden;
	for (vector<GroupEntity*>::const_iterator itg = groups.begin ( );
	     groups.end ( ) != itg; itg++)
		if (true == show)
			groupAddedShown.push_back (*itg);
		else
			groupAddedHidden.push_back (*itg);
	getContext ( ).getGroupManager().getAddedShownAndHidden (
		groupAddedShown, groupAddedHidden, geomAddedShown, geomAddedHidden,
		topoAddedShown, topoAddedHidden, meshAddedShown, meshAddedHidden,
		sysCoordAddedShown, sysCoordAddedHidden);
	updateDisplayedGroupsEntities (geomAddedShown, geomAddedHidden,
		topoAddedShown, topoAddedHidden, meshAddedShown, meshAddedHidden,
		sysCoordAddedShown, sysCoordAddedHidden, sMeshAddedShown, sMeshAddedHidden);
}	// RenderingManager::displayRepresentations


FilterEntity::objectType RenderingManager::displayedTypes ( ) const
{
	return _displayedTypes;
}   // RenderingManager::displayedTypes ( ) const


void RenderingManager::displayTypes (FilterEntity::objectType types)
{
	_displayedTypes  = types;

	vector<Geom::GeomEntity*>			geomAddedShown, geomAddedHidden;
	vector<Topo::TopoEntity*>			topoAddedShown, topoAddedHidden;
	vector<Mesh::MeshEntity*>			meshAddedShown, meshAddedHidden;
	vector<CoordinateSystem::SysCoord*>		sysCoordAddedShown, sysCoordAddedHidden;
	vector<Structured::StructuredMeshEntity*>	sMeshAddedShown, sMeshAddedHidden;
	getContext ( ).getGroupManager().getAddedShownAndHidden (
		types, geomAddedShown, geomAddedHidden,
		topoAddedShown, topoAddedHidden, meshAddedShown, meshAddedHidden,
		sysCoordAddedShown, sysCoordAddedHidden);
	if (0 != (types & FilterEntity::StructuredMesh))
		sMeshAddedShown	= getContext ( ).getStructuredMeshManager ( ).getMeshEntities ( );
	else
		sMeshAddedHidden= getContext ( ).getStructuredMeshManager ( ).getMeshEntities ( );
	updateDisplayedGroupsEntities (
			geomAddedShown, geomAddedHidden, topoAddedShown, topoAddedHidden,
			meshAddedShown, meshAddedHidden,sysCoordAddedShown, sysCoordAddedHidden, sMeshAddedShown, sMeshAddedHidden);
}	// RenderingManager::displayTypes


void RenderingManager::updateRepresentation (Mgx3D::Utils::Entity& entity, unsigned long m, bool updateRep)
{
	DisplayProperties::GraphicalRepresentation*	gr	=
				entity.getDisplayProperties ( ).getGraphicalRepresentation ( );
	if (0 != gr)
	{
		GraphicalEntityRepresentation*	ger	=
							dynamic_cast<GraphicalEntityRepresentation*>(gr);
		CHECK_NULL_PTR_ERROR (ger)
		unsigned long	mask	= ger->getRepresentationMask ( );
		// Rem CP : updateRep == true si shrink ou taille flèche modifié
		if ((mask != m) || (true == updateRep))
			ger->updateRepresentation (m, entity.getDisplayProperties ( ).isDisplayed ( ));
		else
			gr->updateRepresentationProperties ( );
	}	// if (0 ! = gr)
}	// RenderingManager::updateRepresentation


void RenderingManager::updateRepresentation (Mgx3D::Utils::Entity& entity)
{
	DisplayProperties::GraphicalRepresentation*	gr	=
				entity.getDisplayProperties ( ).getGraphicalRepresentation ( );
	if (0 != gr)
	{
		if (0 != getSelectionManager ( ))
			gr->setSelected (getSelectionManager ( )->isSelected(entity));
		GraphicalEntityRepresentation*	ger	=
							dynamic_cast<GraphicalEntityRepresentation*>(gr);
		CHECK_NULL_PTR_ERROR (ger)
		unsigned long	mask	= ger->getRepresentationMask ( );
		ger->updateRepresentation (mask, true);
	}	// if (0 ! = gr)
}	// RenderingManager::updateRepresentation


void RenderingManager::updateRepresentations ( )
{
	vector<Entity*>	entities	= getDisplayedEntities ( );
	for (vector<Entity*>::iterator it = entities.begin ( );
	     entities.end ( ) !=it; it++)
	{
		DisplayProperties::GraphicalRepresentation*	rep	=
				(*it)->getDisplayProperties ( ).getGraphicalRepresentation ( );
		if (0 != rep)
		{
			unsigned long	mask	= rep->getRepresentationMask ( );
			rep->updateRepresentation (mask, true);
		}
	}	// for (vector<Entity*>::iterator it = entities.begin ( ); ...
}	// RenderingManager::updateRepresentations


bool RenderingManager::isDisplayed (const Mgx3D::Utils::Entity& entity) const
{
	return entity.getDisplayProperties ( ).isDisplayed ( );
}	// RenderingManager::isDisplayed


bool RenderingManager::hasDisplayedRepresentation ( ) const
{
	throw Exception (UTF8String ("RenderingManager::hasDisplayedRepresentation should be overloaded.", Charset::UTF_8));
}	// RenderingManager::hasDisplayedRepresentation


vector<Entity*> RenderingManager::getDisplayedEntities ( )
{
	throw Exception (UTF8String ("RenderingManager::getDisplayedEntities should be overloaded.", Charset::UTF_8));
}	// RenderingManager::getDisplayedEntities


void RenderingManager::getDisplayedValues (vector<RenderingManager::ColorTableDefinition>& definitions)
{
	throw Exception (UTF8String ("RenderingManager::getDisplayedValues should be overloaded.", Charset::UTF_8));
}	// RenderingManager::getDisplayedValues


void RenderingManager::useGlobalDisplayProperties (bool global)
{
	_useGlobalDisplayProperties	= global;
}	// RenderingManager::useGlobalDisplayProperties


bool RenderingManager::useGlobalDisplayProperties ( ) const
{
	return _useGlobalDisplayProperties;
}	// RenderingManager::useGlobalDisplayProperties


void RenderingManager::topoUseGeomColor (bool useGeomColor)
{
	_topoUseGeomColor	= useGeomColor;
}	// RenderingManager::topoUseGeomColor


bool RenderingManager::topoUseGeomColor ( ) const
{
	return _topoUseGeomColor;
}	// RenderingManager::topoUseGeomColor


void RenderingManager::displayRepresentation (RenderingManager::RepresentationID, bool show)
{
	throw Exception (UTF8String ("RenderingManager::displayRepresentation should be overloaded.", Charset::UTF_8));
}	// RenderingManager::displayRepresentation


RenderingManager::RepresentationID RenderingManager::createSegment (
							const Math::Point& p1, const Math::Point& p2,
							const DisplayProperties& properties, bool display)
{
	throw Exception (UTF8String ("RenderingManager::createSegment should be overloaded.", Charset::UTF_8));
}	// RenderingManager::createSegment


RenderingManager::RepresentationID RenderingManager::createVector (
							const Math::Point& p1, const Math::Point& p2,
							const DisplayProperties& properties, bool display)
{
	throw Exception (UTF8String ("RenderingManager::createVector should be overloaded.", Charset::UTF_8));
}	// RenderingManager::createVector


RenderingManager::RepresentationID RenderingManager::createRotationAxe (
							const Math::Point& p1, const Math::Point& p2,
							const DisplayProperties& properties, bool display)
{
	throw Exception (UTF8String ("RenderingManager::createRotationAxe should be overloaded.", Charset::UTF_8));
}	// RenderingManager::createRotationAxe


RenderingManager::RepresentationID RenderingManager::createOutlineBox (
							const Math::Point& p1, const Math::Point& p2,
							const DisplayProperties& properties, bool display)
{
	throw Exception(UTF8String ("RenderingManager::createOutlineBox should be overloaded.", Charset::UTF_8));
}	// RenderingManager::createOutlineBox


RenderingManager::RepresentationID RenderingManager::createCloudRepresentation (
		const vector<Math::Point>& points, const DisplayProperties& properties,
		bool display)
{
	throw Exception (UTF8String ("RenderingManager::createCloudRepresentation should be overloaded.", Charset::UTF_8));
}	// RenderingManager::createCloudRepresentation


RenderingManager::RepresentationID RenderingManager::createSegmentsWireRepresentation (
		const vector<Math::Point>& points, const vector<size_t>& segments,
		const DisplayProperties& properties, bool display)
{
	throw Exception (UTF8String ("RenderingManager::createSegmentsWireRepresentation should be overloaded.", Charset::UTF_8));
}	// RenderingManager::createSegmentsWireRepresentation


RenderingManager::RepresentationID RenderingManager::createTextRepresentation (
		const Math::Point& position, size_t number,
		const DisplayProperties& properties, bool display)
{
	throw Exception (UTF8String ("RenderingManager::createTextRepresentation should be overloaded.", Charset::UTF_8));
}	// RenderingManager::createTextRepresentation


void RenderingManager::destroyRepresentation (RenderingManager::RepresentationID id, bool hide)
{
	throw Exception (UTF8String ("RenderingManager::destroyRepresentation should be overloaded.", Charset::UTF_8));
}	// RenderingManager::destroyRepresentation


RenderingManager::PointInteractor* RenderingManager::createPointInteractor (
							Math::Point point, double dx, double dy, double dz,
							RenderingManager::InteractorObserver* observer)
{
	throw Exception (UTF8String ("RenderingManager::createPointInteractor should be overloaded.", Charset::UTF_8));
}	// RenderingManager::createPointInteractor


RenderingManager::ConstrainedPointInteractor* RenderingManager::createConstrainedPointInteractor (
				Math::Point point, Entity* constraint, size_t factor, RenderingManager::InteractorObserver* observer)
{
	throw Exception (UTF8String ("RenderingManager::createConstrainedPointInteractor should be overloaded.", Charset::UTF_8));
}	// RenderingManager::createConstrainedPointInteractor


RenderingManager::PlaneInteractor* RenderingManager::createPlaneInteractor (
							Math::Point point, Math::Vector normal,
							double xMin, double xMax, double yMin, double yMax,
							 double zMin, double zMx,
							RenderingManager::InteractorObserver* observer)
{
	throw Exception (UTF8String ("RenderingManager::createPlaneInteractor should be overloaded.", Charset::UTF_8));
}	// RenderingManager::createPlaneInteractor


void RenderingManager::destroyInteractor (RenderingManager::Interactor* interactor)
{
	throw Exception (UTF8String ("RenderingManager::destroyInteractor should be overloaded.", Charset::UTF_8));
}	// RenderingManager::destroyInteractor


RenderingManager::ColorTable* RenderingManager::getColorTable (const ColorTableDefinition&)
{
	throw Exception (UTF8String ("RenderingManager::getColorTable should be overloaded.", Charset::UTF_8));
}	// RenderingManager::getColorTable


vector<RenderingManager::ColorTable*> RenderingManager::getColorTables ( ) const
{
	throw Exception (UTF8String ("RenderingManager::getColorTables should be overloaded.", Charset::UTF_8));
}	// RenderingManager::getColorTables


void RenderingManager::removeColorTable (RenderingManager::ColorTable* table)
{
	throw Exception (UTF8String ("RenderingManager::removeColorTable should be overloaded.", Charset::UTF_8));
}	// RenderingManager::removeColorTable


void RenderingManager::setTableDefinition (const RenderingManager::ColorTableDefinition& definition)
{
	throw Exception (UTF8String ("RenderingManager::setTableDefinition should be overloaded.", Charset::UTF_8));
}   // RenderingManager::setTableDefinition


void RenderingManager::setSelectionManager (SelectionManagerIfc* manager)
{
	if ((manager != getSelectionManager ( )) && (0 != getSelectionManager ( )))
	{	// TODO [CP] : annuler la sélection en cours (représentation)
	}	// if ((manager != getSelectionManager ( )) && ...
	SelectionManagerObserver::setSelectionManager (manager);
}	// RenderingManager::setSelectionManager


SelectionManagerIfc* RenderingManager::getSelectionManager ( )
{
	return SelectionManagerObserver::getSelectionManager ( );
}	// RenderingManager::getSelectionManager


const SelectionManagerIfc* RenderingManager::getSelectionManager ( ) const
{
	return SelectionManagerObserver::getSelectionManager ( );
}	// RenderingManager::getSelectionManager


void RenderingManager::entitiesAddedToSelection (const vector<Entity*>& entities)
{
	if (0 == entities.size ( ))
		return;

	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		CHECK_NULL_PTR_ERROR (*it)
		DisplayProperties::GraphicalRepresentation*	gr	=
				(*it)->getDisplayProperties ( ).getGraphicalRepresentation ( );
		if ((0 != gr) && (false == gr->isSelected ( )))
			gr->setSelected (true);
	}	// for (vector<Entity*>::const_iterator it = entities.begin ( ); ...

	if (false == displayLocked ( ))
		forceRender ( );
}	// RenderingManager::entitiesAddedToSelection


void RenderingManager::entitiesRemovedFromSelection (const vector<Entity*>& entities, bool clear)
{
	if (0 == entities.size ( ))
		return;

	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		CHECK_NULL_PTR_ERROR (*it)
		DisplayProperties::GraphicalRepresentation*	gr	=
				(*it)->getDisplayProperties ( ).getGraphicalRepresentation ( );
		if ((0 != gr) && (true == gr->isSelected ( )))
			gr->setSelected (false);
	}	// for (vector<Entity*>::const_iterator it = entities.begin ( ); ...

	if (false == displayLocked ( ))
		forceRender ( );
}	// RenderingManager::entitiesRemovedFromSelection


void RenderingManager::selectionCleared ( )
{
	if (0 == getSelectionManager ( ))
		return;

	const bool	locked	= displayLocked ( );
	lockDisplay (true);

	const vector<Entity*>	entities	= getSelectionManager ( )->getEntities ( );
	entitiesRemovedFromSelection (entities, true);

	if (false == locked)
	{
		lockDisplay (false);
		forceRender ( );
	}	// if (false == locked)
}	// RenderingManager::selectionCleared


void RenderingManager::getBoundingBox (
				double& xmin, double& xmax, double& ymin, double& ymax,
				double& zmin, double& zmax)
{
cerr << "RenderingManager::getBoundingBox should be overloaded." << endl;
}	// RenderingManager::getBoundingBoxA


static double sizeScale (double d)
{
	string				str		= NumericConversions::userRepresentation(d,1,1);
	string::size_type	expPos	= str.find ('e');
	if (string::npos == expPos)
		expPos	= str.find ('E');
	if (string::npos == expPos)
		return 10.;

	string	exp	= str.substr (expPos);
	if (1 < exp.length ( ))
		exp	= exp.substr (1);	// On élimine le e/E => nombre
	long	e	= 0 == exp.length ( ) ?  0 : NumericConversions::strToLong(exp);

	return pow (10., (int) e); // [EB] sinon cela ne compile pas sur Rhel5, on attend soit un double, soit un int, mais avec un long c'est enbigüe
}	// sizeScale


void RenderingManager::getBoundingBoxMargins (double& xmargin, double& ymargin, double& zmargin)
{
	double	xmin = 0., xmax = 0., ymin = 0., ymax = 0., zmin = 0., zmax = 0.;
	getBoundingBox (xmin, xmax, ymin, ymax, zmin, zmax);
	double	dx	= xmax - xmin, dy = ymax - ymin, dz = zmax - zmin;
	xmargin	= sizeScale (dx) * Resources::instance ( )._marginBoundingBoxPercentage.getValue ( );
	ymargin	= sizeScale (dy) * Resources::instance ( )._marginBoundingBoxPercentage.getValue ( );
	zmargin	= sizeScale (dz) * Resources::instance ( )._marginBoundingBoxPercentage.getValue ( );
}	// RenderingManager::getBoundingBoxMargins


void RenderingManager::getBackground (double& red, double& green, double& blue) const
{
}	// RenderingManager::getBackground


Color RenderingManager::getBackground ( ) const
{
	return Color (0, 0, 0);
}	// RenderingManager::getBackground


void RenderingManager::setBackground (double red, double green, double blue)
{
	try
	{
		getContext ( ).setBackground (Color (255 * red, 255 * green, 255 * blue));
	}
	catch (...)
	{
	}
}	// RenderingManager::setBackground


void RenderingManager::setBackground (const Color& background)
{
	try
	{
		getContext ( ).setBackground (background);
	}
	catch (...)
	{
	}
}	// RenderingManager::setBackground


void RenderingManager::setDisplayTrihedron (bool display)
{
}	// RenderingManager::setDisplayTrihedron


void RenderingManager::setDisplayLandmark (bool display)
{
}	// RenderingManager::setDisplayLandmark


LandmarkAttributes RenderingManager::getAxisProperties ( )
{
	LandmarkAttributes	props;
	return props;
}	// RenderingManager::getAxisProperties


void RenderingManager::setAxisProperties (const LandmarkAttributes&)
{
}	// RenderingManager::setAxisProperties


void RenderingManager::setDisplayFocalPoint (bool display)
{
}	// RenderingManager::setDisplayFocalPoint


void RenderingManager::resizeView (unsigned long w, unsigned long h)
{
}	// RenderingManager::resizeView


void RenderingManager::zoomView (double factor)
{
}	// RenderingManager::zoomView


void RenderingManager::getViewDefinition (
				double& x, double& y, double& z, double& fx, double& fy, double& fz,
				double& upx, double& upy, double& upz)
{
}	// RenderingManager::getViewDefinition


void RenderingManager::defineView (
				double x, double y, double z, double fx, double fy, double fz,
				double upx, double upy, double upz)
{
}	// RenderingManager::defineView


void RenderingManager::resetView (bool render)
{
	if (true == render)
		forceRender ( );
}	// RenderingManager::resetView


void RenderingManager::setParallelProjection (bool parallel)
{
}	// RenderingManager::setParallelProjection


void RenderingManager::displayxOyViewPlane ( )
{
}   // RenderingManager::displayxOyViewPlane


void RenderingManager::displayxOzViewPlane ( )
{
}   // RenderingManager::displayxOzViewPlane


void RenderingManager::displayyOzViewPlane ( )
{
}   // RenderingManager::displayyOzViewPlane


void RenderingManager::resetRoll ( )
{
}   // RenderingManager::resetRoll


void RenderingManager::forceRender ( )
{
}	// RenderingManager::forceRender


void RenderingManager::setFrameRates (double stillRF, double desiredFR)
{
}	// RenderingManager::setFrameRates


void RenderingManager::startInteractionCallback ( )
{
}	// RenderingManager::startInteractionCallback


void RenderingManager::endInteractionCallback ( )
{
}	// RenderingManager::endInteractionCallback


void RenderingManager::updateConfiguration ( )
{
}	// RenderingManager::updateConfiguration


double RenderingManager::getStructuredDataThresholdValue ( ) const
{
	return _structuredDataThreshold;
}   // RenderingManager::getStructuredDataThresholdValue


void RenderingManager::setStructuredDataThresholdValue (double threshold)
{
	_structuredDataThreshold    = threshold;
}   // RenderingManager::setStructuredDataThresholdValue


GraphicalEntityRepresentation* RenderingManager::createRepresentation (Entity&)
{
	throw Exception (UTF8String ("RenderingManager::createRepresentation should be overloaded.", Charset::UTF_8));
}	// RenderingManager::createRepresentation


void RenderingManager::displayRepresentation (
					GraphicalEntityRepresentation&, bool, bool, unsigned long)
{
	throw Exception (UTF8String ("RenderingManager::displayRepresentation should be overloaded.", Charset::UTF_8));
}	// RenderingManager::displayRepresentation


void RenderingManager::updateView ( )
{
}	// RenderingManager::updateView


GraphicalEntityRepresentation* RenderingManager::getRepresentation (Mgx3D::Utils::Entity& e)
{
	DisplayProperties::GraphicalRepresentation*	gr	=
				e.getDisplayProperties ( ).getGraphicalRepresentation ( );
	return dynamic_cast<GraphicalEntityRepresentation*>(gr);
}	// RenderingManager::getRepresentation


const GraphicalEntityRepresentation* RenderingManager::getRepresentation (const Mgx3D::Utils::Entity& e) const
{
	const DisplayProperties::GraphicalRepresentation*	gr	=
				e.getDisplayProperties ( ).getGraphicalRepresentation ( );
	return dynamic_cast<const GraphicalEntityRepresentation*>(gr);
}	// RenderingManager::getRepresentation


void RenderingManager::updateDisplayedGroupsEntities (
	vector<GeomEntity*>& geomAddedShown, vector<GeomEntity*>& geomAddedHidden,
	vector<TopoEntity*>& topoAddedShown, vector<TopoEntity*>& topoAddedHidden,
	vector<MeshEntity*>& meshAddedShown, vector<MeshEntity*>& meshAddedHidden,
	std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedShown,
	std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedHidden,
	std::vector<Mgx3D::Structured::StructuredMeshEntity*>& sMeshAddedShown,
	std::vector<Mgx3D::Structured::StructuredMeshEntity*>& sMeshAddedHidden)
{
	vector<Utils::Entity*>	entities;
	addToList<Geom::GeomEntity>(entities, geomAddedShown, true);
	if (0 != entities.size ( ))
	{
		addEntities (entities, true, DisplayRepresentation::DISPLAY_GEOM);
		const unsigned long	typesMask	= GraphicalEntityRepresentation::getDefaultRepresentationMask (entities [0]->getType ( ));
		displayRepresentations (entities, true, typesMask, DisplayRepresentation::DISPLAY_GEOM);
	}	// if (0 != entities.size ( ))
	entities.clear ( );
	addToList<Topo::TopoEntity>(entities, topoAddedShown, true);

	if (0 != entities.size ( ))
	{
		addEntities (entities, true, DisplayRepresentation::DISPLAY_TOPO);
		const unsigned long	typesMask	= GraphicalEntityRepresentation::getDefaultRepresentationMask (entities [0]->getType ( ));
		displayRepresentations (entities, true, typesMask, DisplayRepresentation::DISPLAY_TOPO);
	}	// if (0 != entities.size ( ))
	entities.clear ( );
	addToList<Mesh::MeshEntity>(entities, meshAddedShown, true);

	if (0 != entities.size ( ))
	{
		addEntities (entities, true, DisplayRepresentation::DISPLAY_MESH);
		const unsigned long	typesMask	= GraphicalEntityRepresentation::getDefaultRepresentationMask (entities [0]->getType ( ));
		displayRepresentations (entities, true, typesMask, DisplayRepresentation::DISPLAY_MESH);
	}	// if (0 != entities.size ( ))
	entities.clear ( );
	addToList<CoordinateSystem::SysCoord>(entities, sysCoordAddedShown, true);
	if (0 != entities.size ( ))
	{
		addEntities (entities, true, DisplayRepresentation::DISPLAY_SYSCOORD);
		const unsigned long	typesMask	= GraphicalEntityRepresentation::getDefaultRepresentationMask (entities [0]->getType ( ));
		displayRepresentations (entities, true, typesMask, DisplayRepresentation::DISPLAY_SYSCOORD);
	}	// if (0 != entities.size ( ))
	entities.clear ( );
	addToList<Structured::StructuredMeshEntity>(entities, sMeshAddedShown, true);
	if (0 != entities.size ( ))
	{
		addEntities (entities, true, DisplayRepresentation::DISPLAY_STRUCTURED_MESH);
		const unsigned long	typesMask	= GraphicalEntityRepresentation::getDefaultRepresentationMask (entities [0]->getType ( ));
		displayRepresentations (entities, true, typesMask, DisplayRepresentation::DISPLAY_STRUCTURED_MESH);
	}	// if (0 != entities.size ( ))

	entities.clear ( );
	addToList<Geom::GeomEntity>(entities, geomAddedHidden, true);
	if (0 != entities.size ( ))
	{
		removeEntities (entities, DisplayRepresentation::DISPLAY_GEOM);
		displayRepresentations (
			entities, false, 0, DisplayRepresentation::DISPLAY_GEOM);
	}	// if (0 != entities.size ( ))
	entities.clear ( );
	addToList<Topo::TopoEntity>(entities, topoAddedHidden, true);
	if (0 != entities.size ( ))
	{
		removeEntities (entities, DisplayRepresentation::DISPLAY_TOPO);
		displayRepresentations (
			entities, false, 0, DisplayRepresentation::DISPLAY_TOPO);
	}	// if (0 != entities.size ( ))
	entities.clear ( );
	addToList<Mesh::MeshEntity>(entities, meshAddedHidden, true);
	if (0 != entities.size ( ))
	{
		removeEntities (entities, DisplayRepresentation::DISPLAY_MESH);
		displayRepresentations (
			entities, false, 0, DisplayRepresentation::DISPLAY_MESH);
	}	// if (0 != entities.size ( ))
	entities.clear ( );
	addToList<CoordinateSystem::SysCoord>(entities, sysCoordAddedHidden, true);
	if (0 != entities.size ( ))
	{
		removeEntities (entities, DisplayRepresentation::DISPLAY_SYSCOORD);
		displayRepresentations (
			entities, false, 0, DisplayRepresentation::DISPLAY_SYSCOORD);
	}	// if (0 != entities.size ( ))
	entities.clear ( );
	addToList<Structured::StructuredMeshEntity>(entities, sMeshAddedHidden, true);
	if (0 != entities.size ( ))
	{
		removeEntities (entities, DisplayRepresentation::DISPLAY_STRUCTURED_MESH);
const unsigned long	typesMask	= GraphicalEntityRepresentation::getDefaultRepresentationMask (entities [0]->getType ( ));
		displayRepresentations (
			entities, false, typesMask, DisplayRepresentation::DISPLAY_STRUCTURED_MESH);
	}	// if (0 != entities.size ( ))

	updateView ( );
}	// RenderingManager::updateDisplayedGroupsEntities


bool RenderingManager::displayLocked ( ) const
{
	return _displayLocked;
}	// RenderingManager::displayLocked


void RenderingManager::lockDisplay (bool lock)
{
	_displayLocked	= lock;
}	// RenderingManager::lockDisplay


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
