/**
 * \file        VTKMgx3DGeomEntityRepresentation.cpp
 * \author      Charles PIGNEROL
 * \date        28/11/2011
 */

#include "Internal/ContextIfc.h"

#include "QtVtkComponents/VTKMgx3DGeomEntityRepresentation.h"
#include "Geom/GeomEntity.h"
#include "Geom/GeomDisplayRepresentation.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <iostream>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Geom;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtVtkComponents
{


// ===========================================================================
//                      LA CLASSE VTKMgx3DGeomEntityRepresentation
// ===========================================================================


VTKMgx3DGeomEntityRepresentation::VTKMgx3DGeomEntityRepresentation (
															GeomEntity& entity)
	: QtVtkComponents::VTKMgx3DEntityRepresentation (entity)
{
}	// VTKMgx3DGeomEntityRepresentation::VTKMgx3DGeomEntityRepresentation


VTKMgx3DGeomEntityRepresentation::VTKMgx3DGeomEntityRepresentation (
										VTKMgx3DGeomEntityRepresentation& ver)
	: QtVtkComponents::VTKMgx3DEntityRepresentation (*(ver.getEntity ( )))
{
	MGX_FORBIDDEN ("VTKMgx3DGeomEntityRepresentation copy constructor is not allowed.");
}	// VTKMgx3DGeomEntityRepresentation::VTKMgx3DGeomEntityRepresentation


VTKMgx3DGeomEntityRepresentation& VTKMgx3DGeomEntityRepresentation::operator = (
									const VTKMgx3DGeomEntityRepresentation& er)
{
	MGX_FORBIDDEN ("VTKMgx3DGeomEntityRepresentation assignment operator is not allowed.");
	if (&er != this)
	{
	}	// if (&er != this)

	return *this;
}	// VTKMgx3DGeomEntityRepresentation::operator =


VTKMgx3DGeomEntityRepresentation::~VTKMgx3DGeomEntityRepresentation ( )
{
	destroyRepresentations (true);
}	// VTKMgx3DGeomEntityRepresentation::~VTKMgx3DGeomEntityRepresentation


void VTKMgx3DGeomEntityRepresentation::createCloudRepresentation ( )
{
	if ((0 != _cloudGrid) || (0 != _cloudMapper) || (0 != _cloudActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
               "VTKMgx3DGeomEntityRepresentation::createCloudRepresentation")
		throw exc;
	}	// if ((0 != _cloudGrid) || ...
	CHECK_NULL_PTR_ERROR (getEntity ( ))

	GeomDisplayRepresentation	gr (DisplayRepresentation::SOLID, 0.01);
	const DisplayProperties	props	= getDisplayPropertiesAttributes ( );
	gr.setShrink (props.getShrinkFactor ( ));
	getEntity ( )->getRepresentation (gr, true);
	vector<Math::Point>&	points		= gr.getPoints ( );
	VTKMgx3DEntityRepresentation::createPointsCloudRepresentation (
					getEntity ( ), _cloudActor, _cloudMapper, _cloudGrid, points);
}	// VTKMgx3DGeomEntityRepresentation::createCloudRepresentation


void VTKMgx3DGeomEntityRepresentation::createSurfacicRepresentation ( )
{
	if ((0 != _surfacicGrid) || (0 != _surfacicMapper) || (0 != _surfacicActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
               "VTKMgx3DGeomEntityRepresentation::createSurfacicRepresentation")
		throw exc;
	}	// if ((0 != _surfacicGrid) || ...
	CHECK_NULL_PTR_ERROR (getEntity ( ))

	GeomDisplayRepresentation	gr (DisplayRepresentation::SOLID, 0.001);
	const DisplayProperties	props	= getDisplayPropertiesAttributes ( );
	gr.setShrink (props.getShrinkFactor ( ));
	getEntity ( )->getRepresentation (gr, true);
	vector<Math::Point>&	points		= gr.getPoints ( );
	vector<size_t>&			triangles	= gr.getSurfaceDiscretization ( );
	VTKMgx3DEntityRepresentation::createTrianglesSurfacicRepresentation (
														points, triangles);
}	// VTKMgx3DGeomEntityRepresentation::createSurfacicRepresentation


void VTKMgx3DGeomEntityRepresentation::createWireRepresentation ( )
{
	if ((0 != _wireGrid) || (0 != _wireMapper) || (0 != _wireActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DGeomEntityRepresentation::createWireRepresentation")
		throw exc;
	}	// if ((0 != _wireGrid) || ...
	CHECK_NULL_PTR_ERROR (getEntity ( ))

	GeomDisplayRepresentation	gr (DisplayRepresentation::WIRE, 0.01);
	const DisplayProperties	props	= getDisplayPropertiesAttributes ( );
	gr.setShrink (props.getShrinkFactor ( ));
	//std::cout<<"VTKMgx3DGeomEntityRepresentation::createWireRepresentation avec shrink à "<<getEntity ( )->getDisplayProperties().getShrinkFactor()<<std::endl;
	getEntity ( )->getRepresentation (gr, true);
	vector<Math::Point>&	points		= gr.getPoints ( );
	vector<size_t>&			segments	= gr.getCurveDiscretization ( );
	VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation(
			getEntity ( ), _wireActor, _wireMapper, _wireGrid, points, segments);
}	// VTKMgx3DGeomEntityRepresentation::createWireRepresentation


void VTKMgx3DGeomEntityRepresentation::createIsoWireRepresentation ( )
{
	if ((0 != _isoWireGrid) || (0 != _isoWireMapper) || (0 != _isoWireActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DGeomEntityRepresentation::createIsoWireRepresentation")
		throw exc;
	}	// if ((0 != _isoWireGrid) || ...
	CHECK_NULL_PTR_ERROR (getEntity ( ))

	GeomDisplayRepresentation	gr (DisplayRepresentation::ISOCURVE, 0.01);
	const DisplayProperties	props	= getDisplayPropertiesAttributes ( );
	gr.setShrink (props.getShrinkFactor ( ));
	getEntity ( )->getRepresentation (gr, true);
	vector<Math::Point>&	points		= gr.getPoints ( );
	vector<size_t>&			lines	= gr.getCurveDiscretization ( );
	VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation (
			getEntity ( ), _isoWireActor, _isoWireMapper,_isoWireGrid, points, lines);
}	// VTKMgx3DGeomEntityRepresentation::createIsoWireRepresentation


void VTKMgx3DGeomEntityRepresentation::createAssociationVectorRepresentation ( )
{
}	// VTKMgx3DGeomEntityRepresentation::createAssociationVectorRepresentation


bool VTKMgx3DGeomEntityRepresentation::getRefinedRepresentation (
	vector<Math::Point>& points, vector<size_t>& discretization, size_t factor)
{
	CHECK_NULL_PTR_ERROR (getEntity ( ))
	const DisplayRepresentation::type	repType	= getEntity ( )->getDim ( ) < 2 ? 
				DisplayRepresentation::WIRE : DisplayRepresentation::SOLID;
	GeomDisplayRepresentation	gr (repType, 0.01);
	gr.setNbPts(gr.getNbPts()*factor); // raffinement actif pour les courbes
	//gr.setDeflection(gr.getDeflection()/(double)factor); // raffinement actif pour les surfaces
	getEntity ( )->getRepresentation (gr, true);
	points		= gr.getPoints ( );
	if (DisplayRepresentation::SOLID == repType){
		discretization	= gr.getSurfaceDiscretization ( );
		return false;	// Raffinement non fait comme demandé
	}
	else {
		discretization	= gr.getCurveDiscretization ( );
		return true;
	}

}	// VTKMgx3DGeomEntityRepresentation::getRefinedRepresentation


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtVtkComponents

}	// namespace Mgx3D
