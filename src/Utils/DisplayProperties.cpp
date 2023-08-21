/*----------------------------------------------------------------------------*/
/*
 * \file DisplayProperties.cpp
 *
 *  \author Charles PIGNEROL
 *
 *  \date 22 déc. 2010
 */
/*----------------------------------------------------------------------------*/
#include "Utils/DisplayProperties.h"
#include "Utils/Common.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <iostream>
using namespace std;
using namespace TkUtil;


/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {


// =========================================================================
//          LA CLASSE DisplayProperties::GraphicalRepresentation
// =========================================================================


DisplayProperties::GraphicalRepresentation::GraphicalRepresentation ( )
	: _selected (false), _selectable (true), _highlighted (false),
	  _representationMask (0)
{
}	// GraphicalRepresentation::GraphicalRepresentation


DisplayProperties::GraphicalRepresentation::GraphicalRepresentation (const DisplayProperties::GraphicalRepresentation& gr)
	: _selected (gr._selected), _selectable (gr._selectable),
	  _highlighted (gr._highlighted),
	  _representationMask (gr._representationMask)
{
}	// GraphicalRepresentation::GraphicalRepresentation


DisplayProperties::GraphicalRepresentation& DisplayProperties::GraphicalRepresentation::operator = (const DisplayProperties::GraphicalRepresentation& gr)
{
	_selected		= gr._selected;
	_selectable		= gr._selectable;
	_highlighted		= gr._highlighted;
	_representationMask	= gr._representationMask;

	return *this;
}	// GraphicalRepresentation::operator =


DisplayProperties::GraphicalRepresentation::~GraphicalRepresentation ( )
{
}	// GraphicalRepresentation::~GraphicalRepresentation


void DisplayProperties::GraphicalRepresentation::setSelected (bool selected)
{
	_selected	= selected;
}	// GraphicalRepresentation::setSelected


bool DisplayProperties::GraphicalRepresentation::isSelected ( ) const
{
	return _selected;
}	// GraphicalRepresentation::isSelected


void DisplayProperties::GraphicalRepresentation::setSelectable (bool selectable)
{
	_selectable	= selectable;
}	// GraphicalRepresentation::setSelectable


bool DisplayProperties::GraphicalRepresentation::isSelectable ( ) const
{
	return _selectable;
}	// GraphicalRepresentation::isSelectable


void DisplayProperties::GraphicalRepresentation::setHighlighted (bool highlighted)
{
	_highlighted	= highlighted;
}	// GraphicalRepresentation::setHighlighted


bool DisplayProperties::GraphicalRepresentation::isHighlighted ( ) const
{
	return _highlighted;
}	// GraphicalRepresentation::isHighlighted


void DisplayProperties::GraphicalRepresentation::setRepresentationMask (unsigned long mask)
{
	if (mask == _representationMask)
		return;

	_representationMask	= mask;
}	// GraphicalRepresentation::setRepresentationMask


void DisplayProperties::GraphicalRepresentation::updateRepresentation (unsigned long mask, bool force)
{
	setRepresentationMask (mask);
}	// GraphicalRepresentation::updateRepresentation


void DisplayProperties::GraphicalRepresentation::updateRepresentationProperties ( )
{
}	// GraphicalRepresentation::updateRepresentationProperties


void DisplayProperties::GraphicalRepresentation::bringToTop ( )
{
}	// GraphicalRepresentation::bringToTop


void DisplayProperties::GraphicalRepresentation::createRefinedRepresentation (size_t factor)
{
}	// GraphicalRepresentation::createRefinedRepresentation


void DisplayProperties::GraphicalRepresentation::destroyRefinedRepresentation ( )
{
}	// GraphicalRepresentation::destroyRefinedRepresentation


// =========================================================================
//          LA CLASSE DisplayProperties::EntityView
// =========================================================================

DisplayProperties::EntityView::EntityView (Entity* entity)
	: _entity (entity)
{
}	// EntityView::EntityView


DisplayProperties::EntityView::EntityView (const DisplayProperties::EntityView&)
	: _entity (0)
{
	MGX_FORBIDDEN ("EntityView copy constructor is not allowed.");
}	// EntityView::EntityView


DisplayProperties::EntityView&
		DisplayProperties::EntityView::operator = (const DisplayProperties::EntityView&)
{
	MGX_FORBIDDEN ("EntityView::operator = is not allowed.");
	return *this;
}	// EntityView::operator =


DisplayProperties::EntityView::~EntityView ( )
{
	_entity	= 0;
}	// EntityView::~EntityView


const Entity& DisplayProperties::EntityView::getViewEntity ( ) const
{
	CHECK_NULL_PTR_ERROR (_entity)
	return *_entity;
}	// EntityView::getViewEntity


Entity& DisplayProperties::EntityView::getViewEntity ( )
{
	CHECK_NULL_PTR_ERROR (_entity)
	return *_entity;
}	// EntityView::getViewEntity


void DisplayProperties::EntityView::entityDisplayed (bool displayed)
{
}	// EntityView::entityDisplayed


// =========================================================================
//                          LA CLASSE DisplayProperties
// =========================================================================


unsigned short	DisplayProperties::_defaultFontFamily	= 0;
unsigned short	DisplayProperties::_defaultFontSize	= 12;
bool		DisplayProperties::_defaultFontBold	= false;
bool		DisplayProperties::_defaultFontItalic	= false;


DisplayProperties::DisplayProperties ( )
	: _cloudColor (0, 0, 0), _wireColor (0, 0, 0),
	  _solidColor (0, 0, 0),
	  _cloudOpacity (1.), _wireOpacity (1.), _surfacicOpacity (1.),
	  _volumicOpacity (1.), _shrinkFactor (1.), _arrowComul (1.),
	  _pointSize (5.), _lineWidth (2.),
	  _fontFamily (_defaultFontFamily), _fontSize (_defaultFontSize),
	  _bold (_defaultFontBold), _italic (_defaultFontItalic),
	  _fontColor (0, 0, 0), _valueName ( ),
	  _displayed (false), _graphicalRepresentation (0),
	  _displayable(true), _views ( )
{
}	// DisplayProperties::DisplayProperties


DisplayProperties::DisplayProperties (const DisplayProperties& dp)
	: _cloudColor (dp._cloudColor), _wireColor (dp._wireColor),
	  _solidColor (dp._solidColor),
	  _cloudOpacity (dp._cloudOpacity), _wireOpacity (dp._wireOpacity),
	  _surfacicOpacity (dp._surfacicOpacity),
	  _volumicOpacity (dp._volumicOpacity),
	  _shrinkFactor (dp._shrinkFactor), _arrowComul (dp._arrowComul),
	  _pointSize (dp._pointSize), _lineWidth (dp._lineWidth),
	  _fontFamily (dp._fontFamily), _fontSize (dp._fontSize),
	  _bold (dp._bold), _italic (dp._italic),
	  _fontColor (dp._fontColor), _valueName (dp._valueName),
	  _displayed (dp._displayed),
// ---------------------------------------------------------------------------
// IMPORTANT
// NOTE CP : _graphicalRepresentation est volontairement à 0 et non recopié.
// C'est un pointeur vers la vue 3D. S'il est copié il sera détruit par les
// 2 instances de DisplayProperties, et ça finira mal ...
// En revanche la copie d'instance est utilisée pour récupérer des propriétes
// graphiques (couleur, ...).
// ---------------------------------------------------------------------------
	  _graphicalRepresentation (0),
      _displayable(dp._displayable), _views (dp._views)
{
}	// DisplayProperties::DisplayProperties


DisplayProperties& DisplayProperties::operator = (const DisplayProperties& dp)
{
	if (&dp != this)
	{
		_cloudColor				= dp._cloudColor;
		_wireColor				= dp._wireColor;
		_solidColor				= dp._solidColor;
		_cloudOpacity				= dp._cloudOpacity;
		_wireOpacity				= dp._wireOpacity;
		_surfacicOpacity			= dp._surfacicOpacity;
		_volumicOpacity				= dp._volumicOpacity;
		_shrinkFactor				= dp._shrinkFactor;
		_arrowComul				= dp._arrowComul;
		_pointSize				= dp._pointSize;
		_lineWidth				= dp._lineWidth;
		_fontFamily				= dp._fontFamily;
		_fontSize				= dp._fontSize;
		_bold					= dp._bold;
		_italic					= dp._italic;
		_fontColor				= dp._fontColor;
		_valueName				= dp._valueName;
		_displayed      			= dp._displayed;
// ---------------------------------------------------------------------------
// IMPORTANT
// NOTE CP : _graphicalRepresentation est volontairement à 0 et non recopié.
// C'est un pointeur vers la vue 3D. S'il est copié il sera détruit par les
// 2 instances de DisplayProperties, et ça finira mal ...
// En revanche la copie d'instance est utilisée pour récupérer des propriétes
// graphiques (couleur, ...).
// ---------------------------------------------------------------------------
		_graphicalRepresentation	= 0;
		_displayable                = dp._displayable;
		_views						= dp._views;
	}	// if (&dp != this)

	return *this;
}	// DisplayProperties::operator =


DisplayProperties::~DisplayProperties ( )
{
	delete _graphicalRepresentation;	_graphicalRepresentation = 0;
}	// DisplayProperties::~DisplayProperties


void DisplayProperties::setCloudColor (const Color& color)
{
	if (color != _cloudColor)
	{
		_cloudColor	= color;
		// Notifier les vues ?
	}	// if (color != _cloudColor)
}	// DisplayProperties::setCloudColor


void DisplayProperties::setWireColor (const Color& color)
{
	if (color != _wireColor)
	{
		_wireColor	= color;
		// Notifier les vues ?
	}	// if (color != _wireColor)
}	// DisplayProperties::setWireColor


void DisplayProperties::setSurfacicColor (const Color& color)
{
	if (color != _solidColor)
	{
		_solidColor	= color;
		// Notifier les vues ?
	}	// if (color != _solidColor)
}	// DisplayProperties::setSurfacicColor


void DisplayProperties::setVolumicColor (const Color& color)
{
	if (color != _solidColor)
	{
		_solidColor	= color;
		// Notifier les vues ?
	}	// if (color != _solidColor)
}	// DisplayProperties::setVolumicColor


void DisplayProperties::setFontColor (const Color& color)
{
	if (color != _fontColor)
	{
		_fontColor	= color;
		// Notifier les vues ?
	}	// if (color != _fontColor)
}	// DisplayProperties::setFontColor


void DisplayProperties::setValueName (const string& name)
{
	if (name != _valueName)
	{
		_valueName	= name;
		// Notifier les vues ?
	}	// if (name != _valueName)
}	// DisplayProperties::setValueName


void DisplayProperties::setCloudOpacity (double opacity)
{
	_cloudOpacity	= opacity;
}	// DisplayProperties::setCloudOpacity


void DisplayProperties::setWireOpacity (double opacity)
{
	_wireOpacity	= opacity;
}	// DisplayProperties::setWireOpacity


void DisplayProperties::setSurfacicOpacity (double opacity)
{
	_surfacicOpacity	= opacity;
}	// DisplayProperties::setSurfacicOpacity


void DisplayProperties::setVolumicOpacity (double opacity)
{
	_volumicOpacity	= opacity;
}	// DisplayProperties::setVolumicOpacity


void DisplayProperties::setShrinkFactor (double factor)
{
	_shrinkFactor	= factor;
}	// DisplayProperties::setShrinkFactor


void DisplayProperties::setArrowComul (double factor)
{
	_arrowComul	= factor;
}	// DisplayProperties::setArrowComul


float DisplayProperties::getPointSize ( ) const
{
	return _pointSize;
}	// DisplayProperties::getPointSize


void DisplayProperties::setPointSize (float size)
{
	if (size != _pointSize)
	{
		_pointSize	= size;
		// Notifier les vues ?
	}	// if (size != _pointSize)
}	// DisplayProperties::setPointSize


float DisplayProperties::getLineWidth ( ) const
{
	return _lineWidth;
}	// DisplayProperties::getLineWidth


void DisplayProperties::setLineWidth (float width)
{
	if (width != _lineWidth)
	{
		_lineWidth	= width;
		// Notifier les vues ?
	}	// if (width != _lineWidth)
}	// DisplayProperties::setLineWidth


void DisplayProperties::getFontProperties (
		int& family, int& size, bool& bold, bool& italic, Color& color) const
{
	family	= _fontFamily;
	size	= _fontSize;
	bold	= _bold;
	italic	= _italic;
	color	= _fontColor;
}	// DisplayProperties::getFontProperties


void DisplayProperties::setFontProperties (
			int family, int size, bool bold, bool italic, const Color& color)
{
	_fontFamily	= family;
	_fontSize	= size;
	_bold		= bold;
	_italic		= italic;
	_fontColor	= color;
}	// DisplayProperties::setFontProperties


bool DisplayProperties::isDisplayed ( ) const
{
    return _displayed;
}

void DisplayProperties::setDisplayed(bool disp)
{
	if (_displayed != disp)
		notifyViews (disp);
    _displayed = disp;
    // c'est la vue qui met à jour l'info, ou le GroupManager à la fin d'une commande [EB]
}


const DisplayProperties::GraphicalRepresentation* DisplayProperties::getGraphicalRepresentation ( ) const
{
	return _graphicalRepresentation;
}	// DisplayProperties::getGraphicalRepresentation


DisplayProperties::GraphicalRepresentation* DisplayProperties::getGraphicalRepresentation ( )
{
	return _graphicalRepresentation;
}	// DisplayProperties::getGraphicalRepresentation


void DisplayProperties::setGraphicalRepresentation (DisplayProperties::GraphicalRepresentation* gr)
{
	if ((0 != gr) && (0 != _graphicalRepresentation) &&
	    (gr != _graphicalRepresentation))
		throw TkUtil::Exception ("DisplayProperties::setGraphicalRepresentation : représentation non nulle.");

	_graphicalRepresentation	= gr;
}	// DisplayProperties::setGraphicalRepresentation


void DisplayProperties::registerEntityView (EntityView& view)
{
	_views.push_back (&view);
}	// DisplayProperties::registerEntityView


void DisplayProperties::unregisterEntityView (EntityView& view)
{
	for (vector<EntityView*>::iterator it = _views.begin ( );
	     _views.end ( ) != it; it++)
	{
		if (&view == *it)
		{
			_views.erase (it);
			break;
		}
	}
}	// DisplayProperties::unregisterEntityView


void DisplayProperties::notifyViews (bool displayed)
{
	for (vector<EntityView*>::iterator it = _views.begin ( );
	     _views.end ( ) != it; it++)
		(*it)->entityDisplayed (displayed);
}	// DisplayProperties::notifyViews


bool DisplayProperties::operator == (const DisplayProperties& dp) const
{
	if ((getCloudColor ( ) != dp.getCloudColor ( ))           ||
	    (getWireColor ( ) != dp.getWireColor ( ))             ||
	    (getSurfacicColor ( ) != dp.getSurfacicColor ( ))     ||
	    (getPointSize ( ) != dp.getPointSize ( ))             ||
	    (getLineWidth ( ) != dp.getLineWidth ( ))             ||
	    (isDisplayed ( ) != dp.isDisplayed ( ))               ||
	    (getGraphicalRepresentation ( ) != dp.getGraphicalRepresentation ( )) ||
	    (isDisplayable ( ) != dp.isDisplayable ( )))
		return false;

	return true;
}	// DisplayProperties::operator ==


bool DisplayProperties::operator != (const DisplayProperties& dp) const
{
	return ! (*this == dp);
}	// DisplayProperties::operator !=


// =========================================================================
//                 LA CLASSE GraphicalRepresentationFactoryIfc
// =========================================================================

GraphicalRepresentationFactoryIfc*	GraphicalRepresentationFactoryIfc::_instance	= 0;


GraphicalRepresentationFactoryIfc::GraphicalRepresentationFactoryIfc ( )
{
}	// GraphicalRepresentationFactoryIfc::GraphicalRepresentationFactoryIfc


GraphicalRepresentationFactoryIfc::GraphicalRepresentationFactoryIfc (const GraphicalRepresentationFactoryIfc&)
{
	MGX_FORBIDDEN ("GraphicalRepresentationFactoryIfc copy constructor is not allowed.")
}	// GraphicalRepresentationFactoryIfc::GraphicalRepresentationFactoryIfc


GraphicalRepresentationFactoryIfc& GraphicalRepresentationFactoryIfc::operator = (const GraphicalRepresentationFactoryIfc&)
{
	MGX_FORBIDDEN ("GraphicalRepresentationFactoryIfc assignment operator is not allowed.")
	return *this;
}	// GraphicalRepresentationFactoryIfc::operator =


GraphicalRepresentationFactoryIfc::~GraphicalRepresentationFactoryIfc ( )
{
}	// GraphicalRepresentationFactoryIfc::~GraphicalRepresentationFactoryIfc


bool GraphicalRepresentationFactoryIfc::hasInstance ( )
{
	return 0 == _instance ? false : true;
}	// GraphicalRepresentationFactoryIfc::hasInstance


GraphicalRepresentationFactoryIfc& GraphicalRepresentationFactoryIfc::getInstance ( )
{
	if (0 == _instance)
	{
		INTERNAL_ERROR (exc, "Absence de fabrication.", "GraphicalRepresentationFactoryIfc::getInstance")
		throw exc;
	}	// if (0 == _instance)

	return *_instance;
}	// GraphicalRepresentationFactoryIfc::getInstance


void GraphicalRepresentationFactoryIfc::setInstance (GraphicalRepresentationFactoryIfc* instance)
{
	if (instance == _instance)
		return;

	delete _instance;
	_instance	= instance;
}	// GraphicalRepresentationFactoryIfc::setInstance


} // end namespace Utils

} // end namespace Mgx3D
