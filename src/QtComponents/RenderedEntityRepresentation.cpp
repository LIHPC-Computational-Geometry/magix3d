/**
 * \file        RenderedEntityRepresentation.cpp
 * \author      Charles PIGNEROL
 * \date        09/10/2013
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "QtComponents/RenderedEntityRepresentation.h"
#include "QtComponents/RenderingManager.h"
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <iostream>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::QtComponents;


namespace Mgx3D
{

namespace QtComponents
{

// ===========================================================================
//                      LA CLASSE RenderedEntityRepresentation
// ===========================================================================


RenderedEntityRepresentation::RenderedEntityRepresentation (Entity& entity)
	: Utils::GraphicalEntityRepresentation (entity),
	  _renderingManager (0)
{
}	// RenderedEntityRepresentation::RenderedEntityRepresentation


RenderedEntityRepresentation::RenderedEntityRepresentation (
											RenderedEntityRepresentation& ver)
	: Utils::GraphicalEntityRepresentation (*(ver.getEntity ( ))),
	  _renderingManager (0)
{
	MGX_FORBIDDEN ("RenderedEntityRepresentation copy constructor is not allowed.");
}	// RenderedEntityRepresentation::RenderedEntityRepresentation


RenderedEntityRepresentation& RenderedEntityRepresentation::operator = (
											const RenderedEntityRepresentation& er)
{
	MGX_FORBIDDEN ("RenderedEntityRepresentation assignment operator is not allowed.");
	if (&er != this)
	{
	}	// if (&er != this)

	return *this;
}	// RenderedEntityRepresentation::operator =


RenderedEntityRepresentation::~RenderedEntityRepresentation ( )
{
}	// RenderedEntityRepresentation::~RenderedEntityRepresentation


RenderingManager* RenderedEntityRepresentation::getRenderingManager ( )
{
	return _renderingManager;
}	// RenderedEntityRepresentation::getRenderingManager


const RenderingManager*
				RenderedEntityRepresentation::getRenderingManager ( ) const
{
	return _renderingManager;
}	// RenderedEntityRepresentation::getRenderingManager


void RenderedEntityRepresentation::setRenderingManager (RenderingManager* rm)
{
	_renderingManager	= rm;
}	// RenderedEntityRepresentation::setRenderingManager


const DisplayProperties
		RenderedEntityRepresentation::getDisplayPropertiesAttributes ( ) const
{
	const Entity*	entity	= getEntity ( );
	if (0 == entity)
		return DisplayProperties ( );

	if (0 != getRenderingManager ( ))
	{
		if (true == getRenderingManager ( )->useGlobalDisplayProperties ( ))
		{
			const DisplayProperties&	props	=
									entity->getDisplayProperties ( );
			DisplayProperties	properties	= 
				 getRenderingManager ( )->getContext (
								).globalDisplayProperties (entity->getType ( ));
			properties.setDisplayed (props.isDisplayed ( ));
			properties.setCloudColor (props.getCloudColor ( ));
			properties.setWireColor (props.getWireColor ( ));
			properties.setSurfacicColor (props.getSurfacicColor ( ));
			properties.setVolumicColor (props.getVolumicColor ( ));
			// Pour les valeurs aux mailles/noeuds :
			properties.setValueName (props.getValueName ( ));

			return properties;
		}	// if (true == getRenderingManager ( )->useGlobalDisplayProperties ( ))
	}	// if (0 != getRenderingManager ( ))

	return entity->getDisplayProperties ( );
}	// RenderedEntityRepresentation::getDisplayPropertiesAttributes


unsigned long RenderedEntityRepresentation::getUsedRepresentationMask ( ) const
{
	if (0 != getRenderingManager ( ))
	{
		if (true == getRenderingManager ( )->useGlobalDisplayProperties ( ))
		{
			const Entity*	entity	= getEntity ( );
			if (0 == entity)
				return getRepresentationMask ( );

			return getRenderingManager ( )->getContext (
											).globalMask (entity->getType ( ));
		}	// if (true == getRenderingManager ( )->useGlobalDisplayProperties ( ))
	}	// if (0 != getRenderingManager ( ))

	return getRepresentationMask ( );
}	// RenderedEntityRepresentation::getUsedRepresentationMask


Color RenderedEntityRepresentation::getColor (unsigned long mask) const
{
//	Color						color (0, 0, 0);
	Color						color (getBaseColor (mask));
	if (0 == getEntity ( ))
		return color;

//	const DisplayProperties	properties	= getDisplayPropertiesAttributes ( );
	const DisplayProperties::GraphicalRepresentation*	rep	=
		getEntity ( )->getDisplayProperties ( ).getGraphicalRepresentation ( );

/*
	if (mask == GraphicalEntityRepresentation::CLOUDS)
		color	= properties.getCloudColor ( );
	else if (mask == GraphicalEntityRepresentation::CURVES)
		color	= properties.getWireColor ( );
	else if (mask == GraphicalEntityRepresentation::ISOCURVES)
		color	= properties.getWireColor ( );
	else if (mask == GraphicalEntityRepresentation::SURFACES)
		color	= properties.getSurfacicColor ( );
	else if (mask == GraphicalEntityRepresentation::VOLUMES)
		color	= properties.getVolumicColor ( );
*/

	if ((0 != rep) && (true == rep->isSelected ( )))
	{
		// [EB] on ne change la couleur que des parties filaires pour les
		// maillages
		if (false == getEntity ( )->isMeshEntity ( ))
			color	= Color (
						255 * Resources::instance ( )._selectionColor.getRed ( ),
						255 * Resources::instance ( )._selectionColor.getGreen ( ),
						255 * Resources::instance ( )._selectionColor.getBlue ( ));
	}	// if ((0 != rep) && (true == rep->isSelected ( )))

	if ((0 != rep) && (true == rep->isHighlighted ( )) &&
	    (true == Resources::instance ( )._useHighLightColor.getValue ( )))
	{
		// [EB] on ne change la couleur que des parties filaires pour les
		// maillages
		if (false == getEntity ( )->isMeshEntity ( ))
			color	= Color (
						255 * Resources::instance ( )._highlightColor.getRed ( ),
						255 * Resources::instance ( )._highlightColor.getGreen ( ),
						255 * Resources::instance ( )._highlightColor.getBlue ( ));
	}

	return color;
}	// RenderedEntityRepresentation::getColor


Color RenderedEntityRepresentation::getBaseColor (unsigned long mask) const
{
	Color						color (0, 0, 0);
	if (0 == getEntity ( ))
		return color;

	const DisplayProperties	properties	= getDisplayPropertiesAttributes ( );
	const DisplayProperties::GraphicalRepresentation*	rep	=
		getEntity ( )->getDisplayProperties ( ).getGraphicalRepresentation ( );

	if (mask == GraphicalEntityRepresentation::CLOUDS)
		color	= properties.getCloudColor ( );
	else if (mask == GraphicalEntityRepresentation::CURVES)
		color	= properties.getWireColor ( );
	else if (mask == GraphicalEntityRepresentation::ISOCURVES)
		color	= properties.getWireColor ( );
	else if (mask == GraphicalEntityRepresentation::SURFACES)
		color	= properties.getSurfacicColor ( );
	else if (mask == GraphicalEntityRepresentation::VOLUMES)
		color	= properties.getVolumicColor ( );

	return color;
}	// RenderedEntityRepresentation::getBaseColor


float RenderedEntityRepresentation::getPointSize (
									const Entity& entity, unsigned long mask)
{
	const DisplayProperties	properties	= getDisplayPropertiesAttributes ( );
	const DisplayProperties::GraphicalRepresentation*	rep	=
		entity.getDisplayProperties ( ).getGraphicalRepresentation ( );
	float pointSize	= entity.getDisplayProperties ( ).getPointSize ( );
	if ((0 != rep) && (true == rep->isSelected ( )))
	{
		// [EB] on ne change la couleur que des parties filaires pour les
		// maillages
		if (false == entity.isMeshEntity ( ))
			pointSize	= Resources::instance ( )._selectionPointSize.getValue ( );
	}	// if ((0 != rep) && (true == rep->isSelected ( )))

	return pointSize;
}	// RenderedEntityRepresentation::getPointSize


float RenderedEntityRepresentation::getLineWidth (
									const Entity& entity, unsigned long mask)
{
	float lineWidth	= 1;
	//const DisplayProperties&	properties	= entity.getDisplayProperties ( );
	const DisplayProperties	properties	= getDisplayPropertiesAttributes ( );
	const DisplayProperties::GraphicalRepresentation*	rep	=
		entity.getDisplayProperties ( ).getGraphicalRepresentation ( );

	if ((0 != rep) && (true == rep->isSelected ( )))
	{
		// [EB] on ne change la couleur que des parties filaires pour les
		// maillages
		if (false == entity.isMeshEntity ( ))
			lineWidth	= Resources::instance ( )._selectionLineWidth.getValue ( );
	}	// if ((0 != rep) && (true == rep->isSelected ( )))

	if ((0 != rep) && (true == rep->isHighlighted ( )) &&
	    (true == Resources::instance ( )._useHighLightColor.getValue ( )))
	{
		// [EB] on ne change la couleur que des parties filaires pour les
		// maillages
		if (false == entity.isMeshEntity ( ))
			lineWidth	= Resources::instance ( )._highLightWidth.getValue ( );
	}

	return lineWidth;
}	// RenderedEntityRepresentation::getLineWidth




// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



void RenderedEntityRepresentation::destroyRepresentations (bool realyDestroy)
{
}	// RenderedEntityRepresentation::destroyRepresentations


bool RenderedEntityRepresentation::destroyDataOnHide ( ) const
{
	const Entity*	entity	= getEntity ( );
	if (0 == entity)
		return true;

	if (true == entity->isGeomEntity ( ))
		return Resources::instance ( )._geomDestroyOnHide.getValue ( );
	if (true == entity->isTopoEntity ( ))
		return Resources::instance ( )._topoDestroyOnHide.getValue ( );
	if (true == entity->isMeshEntity ( ))
		return Resources::instance ( )._meshDestroyOnHide.getValue ( );

	return true;
}	// RenderedEntityRepresentation::destroyDataOnHide


}	// namespace QtComponents

}	// namespace Mgx3D
