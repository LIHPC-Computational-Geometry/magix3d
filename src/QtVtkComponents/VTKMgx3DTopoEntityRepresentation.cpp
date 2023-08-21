/**
 * \file        VTKMgx3DTopoEntityRepresentation.cpp
 * \author      Charles PIGNEROL
 * \date        28/11/2011
 */

#include "Internal/ContextIfc.h"

#include "QtVtkComponents/VTKMgx3DTopoEntityRepresentation.h"
#include "Topo/CoEdge.h"
#include "Topo/TopoEntity.h"
#include "Topo/TopoDisplayRepresentation.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkDataSetAttributes.h>
#include <vtkIdTypeArray.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>

#include <iostream>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::QtComponents;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtVtkComponents
{


// ===========================================================================
//                      LA CLASSE VTKMgx3DTopoEntityRepresentation
// ===========================================================================


const string VTKMgx3DTopoEntityRepresentation::edgesNumFieldName ("edgesNum");

VTKMgx3DTopoEntityRepresentation::VTKMgx3DTopoEntityRepresentation (
															TopoEntity& entity)
	: QtVtkComponents::VTKMgx3DEntityRepresentation (entity)
#ifdef OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
	  , _meshingEdgesNumActor (0), _meshingEdgesNumMapper (0),
	  _visibleMeshingEdgesFilter (0)
#endif	// OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
{
}	// VTKMgx3DTopoEntityRepresentation::VTKMgx3DTopoEntityRepresentation


VTKMgx3DTopoEntityRepresentation::VTKMgx3DTopoEntityRepresentation (
										VTKMgx3DTopoEntityRepresentation& ver)
	: QtVtkComponents::VTKMgx3DEntityRepresentation (*(ver.getEntity ( )))
#ifdef OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
	  , _meshingEdgesNumActor (0), _meshingEdgesNumMapper (0),
	  _visibleMeshingEdgesFilter (0)
#endif	// OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
{
	MGX_FORBIDDEN ("VTKMgx3DTopoEntityRepresentation copy constructor is not allowed.");
}	// VTKMgx3DTopoEntityRepresentation::VTKMgx3DTopoEntityRepresentation


VTKMgx3DTopoEntityRepresentation& VTKMgx3DTopoEntityRepresentation::operator = (
									const VTKMgx3DTopoEntityRepresentation& er)
{
	MGX_FORBIDDEN ("VTKMgx3DTopoEntityRepresentation assignment operator is not allowed.");
	if (&er != this)
	{
	}	// if (&er != this)

	return *this;
}	// VTKMgx3DTopoEntityRepresentation::operator =


VTKMgx3DTopoEntityRepresentation::~VTKMgx3DTopoEntityRepresentation ( )
{
//	destroyRepresentations (true);	// Called by parent
}	// VTKMgx3DTopoEntityRepresentation::~VTKMgx3DTopoEntityRepresentation


void VTKMgx3DTopoEntityRepresentation::setRenderingManager (
														RenderingManager* rm)
{
	VTKMgx3DEntityRepresentation::setRenderingManager (rm);

	if ((0 != rm) && (true == rm->topoUseGeomColor ( )))
		updateRepresentation (getRepresentationMask ( ), true);
}	// VTKMgx3DTopoEntityRepresentation::setRenderingManager


void VTKMgx3DTopoEntityRepresentation::show (
							VTKRenderingManager& renderingManager, bool display)
{
	VTKMgx3DEntityRepresentation::show (renderingManager, display);
#ifdef OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION

	vtkRenderer&	renderer	= renderingManager.getRenderer ( );

	if (true == display)
	{
		if (0 != _meshingEdgesNumActor)
		{
			if (0 != _visibleMeshingEdgesFilter)
				_visibleMeshingEdgesFilter->SetRenderer (&renderer);
			renderer.AddActor2D (_meshingEdgesNumActor);
		}
	}
	else	// if (true == display)
	{
		if (0 != _meshingEdgesNumActor)
			renderer.RemoveActor2D (_meshingEdgesNumActor);
	}	// else if (true == display)
#endif	// OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
}	// VTKMgx3DTopoEntityRepresentation::show


string VTKMgx3DTopoEntityRepresentation::getRepresentationText (
													unsigned long mask) const
{
	UTF8String	text (
				VTKMgx3DEntityRepresentation::getRepresentationText (mask),
				Charset::UTF_8);

	if (0 != (GraphicalEntityRepresentation::NBMESHINGEDGE & mask))
	{
		const CoEdge*	edge	= dynamic_cast<const CoEdge*>(getEntity ( ));
		if (0 != edge)
			text << ' ' << (unsigned long)edge->getNbMeshingEdges ( );
	}	// if (0 != (GraphicalEntityRepresentation::NBMESHINGEDGE & mask))

	return text.iso ( );
}	// VTKMgx3DTopoEntityRepresentation::getRepresentationText


Math::Point
	VTKMgx3DTopoEntityRepresentation::getRepresentationTextPosition (
													unsigned long mask) const
{
	Math::Point	p;

	const TopoEntity*	entity	= dynamic_cast<const TopoEntity*>(getEntity( ));
	if (0 != entity)
	{
		DisplayRepresentation::type	type	= DisplayRepresentation::WIRE;
		if (0 != (mask & MESH_SHAPE))
			type	= (DisplayRepresentation::type)(
			DisplayRepresentation::WIRE | DisplayRepresentation::SHOWMESHSHAPE);
		TopoDisplayRepresentation	tr (type);
		const DisplayProperties		props	= getDisplayPropertiesAttributes( );
		tr.setShrink (props.getShrinkFactor ( ));
		getEntity ( )->getRepresentation (tr, true);
		const vector<Math::Point>&	pts	= tr.getPoints ( );
		const size_t	number	= pts.size ( );
		double			x		= 0.,	y	= 0.,	z	= 0.;
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

		p.setX (x);
		p.setY (y);
		p.setZ (z);
	}	// if (0 != entity)

	return p;
}	// VTKMgx3DTopoEntityRepresentation::getRepresentationTextPosition


void VTKMgx3DTopoEntityRepresentation::updateRepresentation (
												unsigned long mask, bool force)
{
#ifdef OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
	const unsigned long		oldMask = getRepresentationMask ( );

	if (0 != _renderer)
	{
		if (0 != _meshingEdgesNumActor)
			_renderer->RemoveActor2D (_meshingEdgesNumActor);
	}	// if (0 != _renderer)
	// Rem : destroyRepresentations sera appelé si nécessaire.

	// Actualisation des représentations :
	VTKMgx3DEntityRepresentation::updateRepresentation (mask, force);

	// On prend ici en compte le mode d'affichage des propriétés : global
	// ou non (MGXDDD-208) :
	const unsigned long	usedMask	= getUsedRepresentationMask ( );
	if (0 != (usedMask & NBMESHINGEDGE))
		if (0 == _meshingEdgesNumActor)
			createMeshingEdgesNumRepresentation (usedMask);

		if (0 != _renderer)
		{
			if (0 != _meshingEdgesNumActor)
			{
				CHECK_NULL_PTR_ERROR (_visibleMeshingEdgesFilter)
				_visibleMeshingEdgesFilter->SetRenderer (_renderer);
				_renderer->AddActor2D (_meshingEdgesNumActor);
			}	// if (0 != _meshingEdgesNumActor)
		}	// if (0 != _renderer)

	updateRepresentationProperties ( );
#else	// OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
	VTKMgx3DEntityRepresentation::updateRepresentation (mask, force);
#endif	// OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
}	// VTKMgx3DTopoEntityRepresentation::updateRepresentation


Color VTKMgx3DTopoEntityRepresentation::getColor (unsigned long rep) const
{
	if ((0 != getRenderingManager ( )) && (false == isSelected ( )) &&
	    (false == isHighlighted ( )) &&
	    (true == getRenderingManager ( )->topoUseGeomColor ( )))
	{
		const TopoEntity*	te	= dynamic_cast<const TopoEntity*>(getEntity( ));
		CHECK_NULL_PTR_ERROR (te)
		Geom::GeomEntity*	ge	= te->getGeomAssociation ( );
		if (0 != ge)
		{
			DisplayProperties&			dp	= ge->getDisplayProperties ( );
			GraphicalRepresentation*	gr	= dp.getGraphicalRepresentation ( );
			RenderedEntityRepresentation*	rer	=
							dynamic_cast<RenderedEntityRepresentation*>(gr);
			if (0 != rer)
				return rer->getBaseColor (rep);
		}	// if (0 != ge)
	}	// if ((0 != getRenderingManager ( )) && ...

	return VTKMgx3DEntityRepresentation::getColor (rep);
}	// VTKMgx3DTopoEntityRepresentation::getColor


void VTKMgx3DTopoEntityRepresentation::createCloudRepresentation ( )
{
	if ((0 != _cloudGrid) || (0 != _cloudMapper) || (0 != _cloudActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
               "VTKMgx3DTopoEntityRepresentation::createCloudRepresentation")
		throw exc;
	}	// if ((0 != _cloudGrid) || ...

	TopoDisplayRepresentation	tr (DisplayRepresentation::WIRE);
	const DisplayProperties		props	= getDisplayPropertiesAttributes ( );
	tr.setShrink (props.getShrinkFactor ( ));
	getEntity ( )->getRepresentation (tr, true);
	vector<Math::Point>&	points		= tr.getPoints ( );
	VTKMgx3DEntityRepresentation::createPointsCloudRepresentation (
			getEntity ( ), _cloudActor, _cloudMapper, _cloudGrid, points);
}	// VTKMgx3DTopoEntityRepresentation::createCloudRepresentation


void VTKMgx3DTopoEntityRepresentation::createSurfacicRepresentation ( )
{
	if ((0 != _surfacicGrid) || (0 != _surfacicMapper) || (0 != _surfacicActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
               "VTKMgx3DTopoEntityRepresentation::createSurfacicRepresentation")
		throw exc;
	}	// if ((0 != _surfacicGrid) || ...

	TopoDisplayRepresentation	tr (DisplayRepresentation::SOLID);
	const DisplayProperties		props	= getDisplayPropertiesAttributes ( );
	tr.setShrink (props.getShrinkFactor ( ));
	getEntity ( )->getRepresentation (tr, true);
	vector<Math::Point>&	points		= tr.getPoints ( );
	vector<size_t>&			triangles	= tr.getSurfaceDiscretization ( );
	VTKMgx3DEntityRepresentation::createTrianglesSurfacicRepresentation (
														points, triangles);
}	// VTKMgx3DTopoEntityRepresentation::createSurfacicRepresentation


void VTKMgx3DTopoEntityRepresentation::createWireRepresentation ( )
{
	if ((0 != _wireGrid) || (0 != _wireMapper) || (0 != _wireActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DTopoEntityRepresentation::createWireRepresentation")
		throw exc;
	}	// if ((0 != _wireGrid) || ...
	DisplayRepresentation::type	t	= (DisplayRepresentation::type)(DisplayRepresentation::WIRE|DisplayRepresentation::MINIMUMWIRE);
	TopoDisplayRepresentation	tr (t);
	//std::cout<<"VTKMgx3DTopoEntityRepresentation::createWireRepresentation avec shrink à "<<getEntity ( )->getDisplayProperties().getShrinkFactor()<<std::endl;
	const DisplayProperties		props	= getDisplayPropertiesAttributes ( );
	tr.setShrink (props.getShrinkFactor ( ));
	getEntity ( )->getRepresentation (tr, true);
	vector<Math::Point>&	points		= tr.getPoints ( );
	vector<size_t>&			segments	= tr.getCurveDiscretization ( );
	VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation (
			getEntity ( ), _wireActor, _wireMapper, _wireGrid, points, segments);
}	// VTKMgx3DTopoEntityRepresentation::createWireRepresentation


void VTKMgx3DTopoEntityRepresentation::createMeshShapeCloudRepresentation ( )
{
	if ((0 != _cloudGrid) || (0 != _cloudMapper) || (0 != _cloudActor))
    {
        INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DTopoEntityRepresentation::createMeshShapeCloudRepresentation")
        throw exc;
    }   // if ((0 != _cloudGrid) || ...
    DisplayRepresentation::type t   = (DisplayRepresentation::type)(DisplayRepresentation::WIRE
            | DisplayRepresentation::SHOWMESHSHAPE);
    TopoDisplayRepresentation   tr (t);
	const DisplayProperties		props	= getDisplayPropertiesAttributes ( );
	tr.setShrink (props.getShrinkFactor ( ));
    getEntity ( )->getRepresentation (tr, true);
    vector<Math::Point>&    points      = tr.getPoints ( );
    VTKMgx3DEntityRepresentation::createPointsCloudRepresentation (
			getEntity ( ), _cloudActor, _cloudMapper, _cloudGrid, points);

}   // VTKMgx3DTopoEntityRepresentation::createMeshShapeCloudRepresentation


void VTKMgx3DTopoEntityRepresentation::createMeshShapeWireRepresentation ( )
{
    if ((0 != _wireGrid) || (0 != _wireMapper) || (0 != _wireActor))
    {
        INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DTopoEntityRepresentation::createMeshShapeWireRepresentation")
        throw exc;
    }   // if ((0 != _wireGrid) || ...
    DisplayRepresentation::type t   = (DisplayRepresentation::type)(DisplayRepresentation::WIRE
            | DisplayRepresentation::SHOWMESHSHAPE);
    TopoDisplayRepresentation   tr (t);
	const DisplayProperties		props	= getDisplayPropertiesAttributes ( );
	tr.setShrink (props.getShrinkFactor ( ));
    getEntity ( )->getRepresentation (tr, true);
    vector<Math::Point>&    points      = tr.getPoints ( );
    vector<size_t>&         segments   = tr.getCurveDiscretization ( );
    VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation (
    		getEntity ( ), _wireActor, _wireMapper, _wireGrid, points, segments);

}   // VTKMgx3DTopoEntityRepresentation::createMeshShapeWireRepresentation


void VTKMgx3DTopoEntityRepresentation::createDiscretisationRepresentation ( )
{
    if ((0 != _discGrid) || (0 != _discMapper) || (0 != _discActor))
    {
        INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DTopoEntityRepresentation::createDiscretisationRepresentation")
        throw exc;
    }   // if ((0 != _discGrid) || ...
    DisplayRepresentation::type t   = (DisplayRepresentation::type)(DisplayRepresentation::SHOWDISCRETISATIONTYPE);
    TopoDisplayRepresentation   tr (t);
	const DisplayProperties		props	= getDisplayPropertiesAttributes ( );
	tr.setShrink (props.getShrinkFactor ( ));
    getEntity ( )->getRepresentation (tr, true);
    vector<Math::Point>&    points      = tr.getPoints ( );
    vector<size_t>&         segments   = tr.getCurveDiscretization ( );
    VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation (
		    		getEntity ( ), _discActor, _discMapper, _discGrid, points, segments);
}   // VTKMgx3DTopoEntityRepresentation::createDiscretisationRepresentation


void VTKMgx3DTopoEntityRepresentation::createIsoWireRepresentation ( )
{
	if ((0 != _isoWireGrid) || (0 != _isoWireMapper) || (0 != _isoWireActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DTopoEntityRepresentation::createIsoWireRepresentation")
		throw exc;
	}	// if ((0 != _isoWireGrid) || ...

	TopoDisplayRepresentation	tr (DisplayRepresentation::ISOCURVE);
	const DisplayProperties		props	= getDisplayPropertiesAttributes ( );
	tr.setShrink (props.getShrinkFactor ( ));
	getEntity ( )->getRepresentation (tr, true);
	vector<Math::Point>&	points	= tr.getPoints ( );
	vector<size_t>&			lines	= tr.getCurveDiscretization ( );
	VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation (
				getEntity ( ), _isoWireActor, _isoWireMapper,_isoWireGrid, points, lines);
}	// VTKMgx3DTopoEntityRepresentation::createIsoWireRepresentation


void VTKMgx3DTopoEntityRepresentation::createAssociationVectorRepresentation ( )
{
	if ((0 != _vectAssArrow) || (0 != _vectAssMapper) || (0 != _vectAssActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DTopoEntityRepresentation::createAssociationVectorRepresentation")
		throw exc;
	}	// if ((0 != _vectAssArrow) || ...
	DisplayRepresentation::type	t	=
		(DisplayRepresentation::type)(DisplayRepresentation::SHOWASSOCIATION);
	TopoDisplayRepresentation	tr (t);
	const DisplayProperties		props	= getDisplayPropertiesAttributes ( );
	tr.setShrink (props.getShrinkFactor ( ));
	getEntity ( )->getRepresentation (tr, true);
//	const DisplayProperties&	properties	= getEntity ( )->getDisplayProperties ( );
	const DisplayProperties		properties	= getDisplayPropertiesAttributes ( );
	const Color&						color	= properties.getWireColor ( );
	const vector<Math::Point>&      	vector	= tr.getVector ( );
	VTKMgx3DEntityRepresentation::createAssociationVectorRepresentation (
																vector, color);
}	// VTKMgx3DTopoEntityRepresentation::createAssociationVectorRepresentation


bool VTKMgx3DTopoEntityRepresentation::getRefinedRepresentation (
	vector<Math::Point>& points, vector<size_t>& discretization, size_t factor)
{
	CHECK_NULL_PTR_ERROR (getEntity ( ))
	const DisplayRepresentation::type	repType	= getEntity ( )->getDim ( ) < 2 ? 
			DisplayRepresentation::WIRE	: DisplayRepresentation::SOLID;
	TopoDisplayRepresentation	gr (repType);
	gr.setShrink(1.0);
	getEntity ( )->getRepresentation (gr, true);
	points		= gr.getPoints ( );
	if (DisplayRepresentation::SOLID == repType)
		discretization	= gr.getSurfaceDiscretization ( );
	else
		discretization	= gr.getCurveDiscretization ( );

	return false;	// Raffinement non fait comme demandé
}	// VTKMgx3DTopoEntityRepresentation::getRefinedRepresentation



#ifdef OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
void VTKMgx3DTopoEntityRepresentation::createMeshingEdgesNumRepresentation (
															unsigned long mask)
{
	CHECK_NULL_PTR_ERROR (getEntity ( ))
	CoEdge*	edge	= dynamic_cast<CoEdge*>(getEntity ( ));
	if ((0 == _meshingEdgesNumActor) && (0 != edge))
	{
		DisplayRepresentation::type	type	= DisplayRepresentation::WIRE;
		if (0 != (mask & MESH_SHAPE))
			type	= (DisplayRepresentation::type)(
			DisplayRepresentation::WIRE | DisplayRepresentation::SHOWMESHSHAPE);
		TopoDisplayRepresentation	tr (type);
		const DisplayProperties		props	= getDisplayPropertiesAttributes( );
		tr.setShrink (props.getShrinkFactor ( ));
		getEntity ( )->getRepresentation (tr, true);
		CHECK_NULL_PTR_ERROR (tr.getPoints ( ))
		const vector<Math::Point>&	pts	= *(tr.getPoints ( ));
		if (2 > pts.size ( ))
			return;
		_meshingEdgesNumActor 			= vtkActor2D::New ( );
		_meshingEdgesNumMapper			= vtkLabeledDataMapper::New ( );
		_visibleMeshingEdgesFilter		= vtkSelectVisiblePoints::New ( );
		vtkUnstructuredGrid*	grid	= vtkUnstructuredGrid::New ( );
		Math::Point				point;
		if (0 == pts.size ( ) % 2)
		{
			const size_t	i	= pts.size ( ) / 2 - 1;	// pts.size() >= 2
			point.setX ((pts [i+1].getX ( ) + pts [i].getX ( )) / 2);
			point.setY ((pts [i+1].getY ( ) + pts [i].getY ( )) / 2);
			point.setZ ((pts [i+1].getZ ( ) + pts [i].getZ ( )) / 2);
		}
		else	// if (0 == pts.size ( ) % 2)
		{
			size_t	i	= pts.size ( ) / 2;
			point	= pts [i];
		}	// else if (0 == pts.size ( ) % 2)
		vtkPoints*	points	= vtkPoints::New ( );
		points->SetNumberOfPoints (1);
		points->SetPoint (0, point.getX ( ), point.getY ( ), point.getZ ( ));
		grid->Initialize ( );
		grid->Allocate (1, 1);
		grid->SetPoints (points);
		vtkCellArray*	cellArray	= vtkCellArray::New ( );
		vtkIdTypeArray*	idsArray	= vtkIdTypeArray::New ( );
		idsArray->SetNumberOfValues (2);
		vtkIdType*		cellsPtr	= idsArray->GetPointer (0);
		cellsPtr [0]	= 1;	// Nombre de noeuds
		cellsPtr [1]	= 0;	// Id du premier noeud
		cellArray->SetCells (1, idsArray);
		grid->SetCells (VTK_VERTEX, cellArray);
		vtkPointData*	data	= grid->GetPointData ( );
		CHECK_NULL_PTR_ERROR (data)
		vtkIdTypeArray*	array	= vtkIdTypeArray::New ( );
		array->SetName (edgesNumFieldName.c_str ( ));
		array->Allocate (1, 1);
		array->SetValue (0, edge->getNbMeshingEdges ( ));
		data->SetScalars (array);
		data->SetActiveScalars (edgesNumFieldName.c_str ( ));
		data->CopyScalarsOn ( );
		data->SetActiveAttribute (
					edgesNumFieldName.c_str ( ), vtkDataSetAttributes::SCALARS);
		_visibleMeshingEdgesFilter->SetInput (grid);
		CHECK_NULL_PTR_ERROR (_visibleMeshingEdgesFilter->GetOutput ( ))
		vtkDataSetAttributes*	attr	=
					_visibleMeshingEdgesFilter->GetOutput ( )->GetPointData ( );
		CHECK_NULL_PTR_ERROR (attr)
		int	index	= attr->SetActiveAttribute (
					edgesNumFieldName.c_str ( ), vtkDataSetAttributes::SCALARS);
//		const DisplayProperties&	properties	= edge->getDisplayProperties( );
		const DisplayProperties	properties	= getDisplayPropertiesAttributes( );
		Color						color		= properties.getWireColor ( );
		int							fontFamily	= 0, fontSize	= 12;
		bool						bold	= false,	italic	= false;
		// Rem CP : on ne fait pas getFontProperties (..., color) car les
		// arêtes topo ont un code couleur pour la discrétisation.
		// => on utilise une instance de Color qui ne sera pas utilisé.
		Color	c (0, 0, 0);
		properties.getFontProperties(fontFamily, fontSize, bold, italic, c);
		_meshingEdgesNumMapper->SetInput (
									_visibleMeshingEdgesFilter->GetOutput ( ));
		_meshingEdgesNumMapper->SetLabelModeToLabelScalars ( );
		_meshingEdgesNumMapper->GetLabelTextProperty ( )->SetColor (
					color.getRed ( ) / 255., color.getGreen ( ) / 255.,
					color.getBlue ( ) / 255.);
		_meshingEdgesNumMapper->GetLabelTextProperty (
												)->SetFontFamily (fontFamily);
		_meshingEdgesNumMapper->GetLabelTextProperty (
												)->SetFontSize (fontSize);
		_meshingEdgesNumMapper->GetLabelTextProperty ( )->SetBold (bold);
		_meshingEdgesNumMapper->GetLabelTextProperty ( )->SetItalic (italic);
		_meshingEdgesNumMapper->GetLabelTextProperty ( )->ShadowOff ( );
		_meshingEdgesNumActor->SetMapper (_meshingEdgesNumMapper);
		_meshingEdgesNumActor->PickableOff ( );
		grid->Delete ( );		grid		= 0;
		points->Delete ( );		points		= 0;
		idsArray->Delete ( );	idsArray	= 0;
		cellArray->Delete ( );	cellArray	= 0;
		array->Delete ( );		array		= 0;
	}	// if ((0 == _meshingEdgesNumActor) && (0 != edge))
}	// VTKMgx3DTopoEntityRepresentation::createMeshingEdgesNumRepresentation
#endif	// OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION


void VTKMgx3DTopoEntityRepresentation::destroyRepresentations (bool realyDestroy)
{
#ifdef OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
	if (0 != _meshingEdgesNumActor)
	{
		VTKRenderingManager*	renderingManager	=
					dynamic_cast<VTKRenderingManager*>(getRenderingManager ( ));
		if (0 != renderingManager)
		{
			vtkRenderer&	renderer	= renderingManager->getRenderer ( );
			if (0 != renderer.HasViewProp (_meshingEdgesNumActor))
					renderer.RemoveViewProp (_meshingEdgesNumActor);
		}	// if (0 != renderingManager)
		if (true == realyDestroy)
		{
			_meshingEdgesNumActor->Delete ( );
			_meshingEdgesNumActor	= 0;
		}	// if (true == realyDestroy)
	}	// if (0 != _meshingEdgesNumActor)
	if (true == realyDestroy)
	{
		if (0 != _meshingEdgesNumMapper)
			_meshingEdgesNumMapper->Delete ( );
		_meshingEdgesNumMapper	= 0;
		if (0 != _visibleMeshingEdgesFilter)
			_visibleMeshingEdgesFilter->Delete ( );
		_visibleMeshingEdgesFilter	= 0;
	}	// if (true == realyDestroy)
#endif	// OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION

	VTKMgx3DEntityRepresentation::destroyRepresentations (realyDestroy);
}	// VTKMgx3DTopoEntityRepresentation::destroyRepresentations ( )


void VTKMgx3DTopoEntityRepresentation::updateRepresentationProperties ( )
{
	VTKMgx3DEntityRepresentation::updateRepresentationProperties ( );

#ifdef OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
//	const DisplayProperties&	properties	=
//									getEntity ( )->getDisplayProperties ( );
	const DisplayProperties	properties	= getDisplayPropertiesAttributes ( );
	if (0 != _meshingEdgesNumMapper)
	{
		Color		color		= properties.getWireColor ( );
		int			fontFamily	= 0, fontSize	= 12;
		bool		bold	= false,	italic	= false;
		// Rem CP : on ne fait pas getFontProperties (..., color) car les
		// arêtes topo ont un code couleur pour la discrétisation.
		// => on utilise une instance de Color qui ne sera pas utilisé.
		Color	c (0, 0, 0);
		properties.getFontProperties(fontFamily, fontSize, bold, italic, c);
		_meshingEdgesNumMapper->GetLabelTextProperty ( )->SetColor (
					color.getRed ( ) / 255., color.getGreen ( ) / 255.,
					color.getBlue ( ) / 255.);
		_meshingEdgesNumMapper->GetLabelTextProperty ( )->SetFontFamily (
																	fontFamily);
		_meshingEdgesNumMapper->GetLabelTextProperty ( )->SetFontSize(fontSize);
		_meshingEdgesNumMapper->GetLabelTextProperty ( )->SetBold (bold);
		_meshingEdgesNumMapper->GetLabelTextProperty ( )->SetItalic (italic);
	}	// if (0 != _meshingEdgesNumMapper)
#endif	// OLD_TOPO_ENTITY_EDGESNUM_REPRESENTATION
}	// VTKMgx3DTopoEntityRepresentation::updateRepresentationProperties


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtVtkComponents

}	// namespace Mgx3D
