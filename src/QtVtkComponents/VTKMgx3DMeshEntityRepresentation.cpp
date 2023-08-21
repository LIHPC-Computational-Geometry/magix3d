/**
 * \file        VTKMgx3DMeshEntityRepresentation.cpp
 * \author      Charles PIGNEROL
 * \date        22/05/2012
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "QtVtkComponents/VTKMgx3DMeshEntityRepresentation.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "Mesh/MeshEntity.h"
#include "Mesh/MeshDisplayRepresentation.h"
#include "Mesh/Volume.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <vtkCellArray.h>
#include <vtkExtractEdges.h>
#include <vtkIdTypeArray.h>
#include <vtkProperty.h>

#include <iostream>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Mesh;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtVtkComponents
{


// ===========================================================================
//                      LA CLASSE VTKMgx3DMeshEntityRepresentation
// ===========================================================================


VTKMgx3DMeshEntityRepresentation::VTKMgx3DMeshEntityRepresentation (
															MeshEntity& entity)
	: QtVtkComponents::VTKMgx3DEntityRepresentation (entity)
{
}	// VTKMgx3DMeshEntityRepresentation::VTKMgx3DMeshEntityRepresentation


VTKMgx3DMeshEntityRepresentation::VTKMgx3DMeshEntityRepresentation (
										VTKMgx3DMeshEntityRepresentation& ver)
	: QtVtkComponents::VTKMgx3DEntityRepresentation (*(ver.getEntity ( )))
{
	MGX_FORBIDDEN ("VTKMgx3DMeshEntityRepresentation copy constructor is not allowed.");
}	// VTKMgx3DMeshEntityRepresentation::VTKMgx3DMeshEntityRepresentation


VTKMgx3DMeshEntityRepresentation& VTKMgx3DMeshEntityRepresentation::operator = (
									const VTKMgx3DMeshEntityRepresentation& er)
{
	MGX_FORBIDDEN ("VTKMgx3DMeshEntityRepresentation assignment operator is not allowed.");
	if (&er != this)
	{
	}	// if (&er != this)

	return *this;
}	// VTKMgx3DMeshEntityRepresentation::operator =


VTKMgx3DMeshEntityRepresentation::~VTKMgx3DMeshEntityRepresentation ( )
{
	destroyRepresentations (true);
}	// VTKMgx3DMeshEntityRepresentation::~VTKMgx3DMeshEntityRepresentation


void VTKMgx3DMeshEntityRepresentation::createCloudRepresentation ( )
{
	if ((0 != _cloudGrid) || (0 != _cloudMapper) || (0 != _cloudActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
               "VTKMgx3DMeshEntityRepresentation::createCloudRepresentation")
		throw exc;
	}	// if ((0 != _cloudGrid) || ...

	MeshDisplayRepresentation	gr (DisplayRepresentation::SOLID);
	getEntity ( )->getRepresentation (gr, true);
	vector<Math::Point>&	points		= gr.getPoints ( );
	VTKMgx3DEntityRepresentation::createPointsCloudRepresentation (
					getEntity ( ), _cloudActor, _cloudMapper, _cloudGrid, points);
}	// VTKMgx3DMeshEntityRepresentation::createCloudRepresentation


void VTKMgx3DMeshEntityRepresentation::createSurfacicRepresentation ( )
{
	if ((0 != _surfacicGrid) || (0 != _surfacicMapper) || (0 != _surfacicActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
               "VTKMgx3DMeshEntityRepresentation::createSurfacicRepresentation")
		throw exc;
	}	// if ((0 != _surfacicGrid) || ...

	MeshDisplayRepresentation	mdr (DisplayRepresentation::SOLID);
	//std::cout<<"VTKMgx3DMeshEntityRepresentation::createSurfacicRepresentation avec shrink à "<<getEntity ( )->getDisplayProperties().getShrinkFactor()<<std::endl;
	// Si c'est un volume on s'intéresse ici à sa peau :
	mdr.setSkinDisplayed (3 == getEntity ( )->getDim ( ) ? true : false);
	// Mais attention, c'est optimisé structuration par bloc +
	// VTKGMDSEntityRepresentation. Si on n'a pas de bloc (ex : classe Qualif)
	// ça retourne un maillage vide ...
	// Donc si pas de bloc on fait setSkinDisplayed (false) histoire de
	// récupérer le maillage.
	if (3 == getEntity ( )->getDim ( ))
	{
		Mesh::Volume*	volume	= dynamic_cast<Mesh::Volume*>(getEntity ( ));
		CHECK_NULL_PTR_ERROR (volume)
		vector<Topo::Block*>	blocks;
		volume->getBlocks (blocks);
		mdr.setSkinDisplayed (0 == blocks.size ( ) ? false : true);
	}	// if (3 == getEntity ( )->getDim ( ))
	getEntity ( )->getRepresentation (mdr, true);
	vector<Math::Point>&	points		= mdr.getPoints ( );
	const vector<size_t>&	polygons	= mdr.getCells ( );
	std::map<int,int>       points2gmdsID = mdr.getPoints2nodesID ( );
	VTKMgx3DEntityRepresentation::createPolygonsSurfacicRepresentation (
														points, polygons);
	_surfacicActor->SetVisibility (true);
	_surfacicPointsVTK2GMDSID = points2gmdsID;
//_surfacicActor->GetProperty ( )->SetOpacity (.75);
}	// VTKMgx3DMeshEntityRepresentation::createSurfacicRepresentation


void VTKMgx3DMeshEntityRepresentation::createVolumicRepresentation ( )
{
	if ((0 != _volumicGrid) || (0 != _volumicMapper) || (0 != _volumicActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
               "VTKMgx3DMeshEntityRepresentation::createVolumicRepresentation")
		throw exc;
	}	// if ((0 != _volumicGrid) || ...

	MeshDisplayRepresentation	mdr (DisplayRepresentation::SOLID);
	mdr.setSkinDisplayed (false);
	getEntity ( )->getRepresentation (mdr, true);
	vector<Math::Point>&	points		= mdr.getPoints ( );
	const vector<size_t>&	polyedrons	= mdr.getCells ( );
	createVolumicRepresentation (points, polyedrons);
	CHECK_NULL_PTR_ERROR (_volumicActor)
	_volumicActor->SetVisibility (true);
//_volumicActor->GetProperty ( )->SetOpacity (.75);
}	// VTKMgx3DMeshEntityRepresentation::createVolumicRepresentation


void VTKMgx3DMeshEntityRepresentation::createVolumicRepresentation (
			const vector<Math::Point>& meshPts, const vector<size_t>& mesh)
{
	if ((0 != _volumicGrid) || (0 != _volumicMapper) || (0 != _volumicActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
               "VTKMgx3DMeshEntityRepresentation::createVolumicRepresentation")
		throw exc;
	}	// if ((0 != _volumicGrid) || ...

	_volumicGrid	= vtkUnstructuredGrid::New ( );
	_volumicMapper	= vtkDataSetMapper::New ( );
#ifndef VTK_5
	_volumicMapper->SetInputData (_volumicGrid);
#else	// VTK_5
	_volumicMapper->SetInput (_volumicGrid);
#endif	// VTK_5
	_volumicMapper->ScalarVisibilityOff ( );
#if	VTK_MAJOR_VERSION < 8
	_volumicMapper->SetImmediateModeRendering (!Internal::Resources::instance ( )._useDisplayList);
#endif	// VTK_MAJOR_VERSION < 8
	_volumicActor	  = VTKMgx3DActor::New ( );
	_volumicActor->SetEntity (getEntity ( ));
	_volumicActor->SetRepresentationType (DisplayRepresentation::SOLID);

	CHECK_NULL_PTR_ERROR (getEntity ( ))
	const DisplayProperties&	properties  =
									getEntity ( )->getDisplayProperties ( );
	const Color&	volumicColor   = properties.getVolumicColor ( );
	_volumicActor->GetProperty ( )->SetColor (volumicColor.getRed ( ),
						volumicColor.getGreen ( ), volumicColor.getBlue ( ));
	_volumicActor->SetMapper (_volumicMapper);
	CHECK_NULL_PTR_ERROR (_volumicGrid)
	vtkPoints*  points  = vtkPoints::New ( );
	CHECK_NULL_PTR_ERROR (points)
	_volumicGrid->Initialize ( );
	const size_t	pointsNum	= meshPts.size ( );
	const size_t	cellsNum	= mesh.size ( ) / 5;	// Estimation
	// Les sommets :
	points->SetDataTypeToDouble ( );
	points->SetNumberOfPoints (pointsNum);
	_volumicGrid->Allocate (cellsNum, 1000);
	_volumicGrid->SetPoints (points);
	vtkIdType   id  = 0;
	for (vector<Math::Point>::const_iterator itp = meshPts.begin ( );
		 meshPts.end ( ) != itp; itp++, id++)
	{
		double  coords [3] = {(*itp).getX( ), (*itp).getY( ), (*itp).getZ( )};
		points->SetPoint (id, coords);
	}   // for (vector<Math::Point>::const_iterator itp = meshPts.begin ( );

	const size_t	meshSize	= mesh.size ( );
	int*			cellTypes   = new int [(mesh.size ( ) + 1) / 2];// Surévalué
	vtkCellArray*   cellArray   = vtkCellArray::New ( );
	vtkIdTypeArray* idsArray	= vtkIdTypeArray::New ( );
	idsArray->SetNumberOfValues (meshSize);
	vtkIdType*		cellsPtr	= idsArray->GetPointer (0);
	size_t  		current		= 0, pos = 0;
	for (id = 0, pos = 0; pos < meshSize; id++)
	{
		const size_t	count	= mesh [current++];
		switch (count)
		{
			case	4	:
				cellTypes [id]	= VTK_TETRA;
				break;
			case	5	:
				cellTypes [id]	= VTK_PYRAMID;
				break;
			case	6	:
				cellTypes [id]	= VTK_WEDGE;
				break;
			case	8	:
				cellTypes [id]	= VTK_HEXAHEDRON;
				break;
			default		:
			{
				INTERNAL_ERROR (exc, "Représentation volumique d'un polyèdre d'un type non prévu.",
						"VTKMgx3DMeshEntityRepresentation::createVolumicRepresentation");
				throw exc;
			}	// default
		}	// switch (count)

		cellsPtr [pos++] = count;
		for (size_t j = 0; j < count; j++)
			cellsPtr [pos++] = mesh [current++];
	}   // for (id = 0, pos = 0; pos < meshSize; id++)
	idsArray->Squeeze ( );
	cellArray->SetCells (id, idsArray);
	cellArray->Squeeze ( );
	_volumicGrid->SetCells (cellTypes, cellArray);
	delete [] cellTypes;	cellTypes   = 0;
	idsArray->Delete ( );   idsArray	= 0;
	cellArray->Delete ( );  cellArray   = 0;
	points->Delete ( );		points		= 0;
}	// VTKMgx3DMeshEntityRepresentation::createVolumicRepresentation


void VTKMgx3DMeshEntityRepresentation::createWireRepresentation ( )
{
	if ((0 != _wireGrid) || (0 != _wireMapper) || (0 != _wireActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DMeshEntityRepresentation::createWireRepresentation")
		throw exc;
	}	// if ((0 != _wireGrid) || ...
	CHECK_NULL_PTR_ERROR (getEntity ( ))

	// Note CP : Au 29/05/12, la représentation générale de la vue filaire
	// est effectuée par extraction des arêtes sur la vue "solide".
	// [EB] : le problème c'est que l'on prend systématiquement toutes les mailles
	// et pas seulement la peau

	MeshDisplayRepresentation	mdr (DisplayRepresentation::WIRE);
	getEntity ( )->getRepresentation (mdr, true);
	vector<Math::Point>&	points		= mdr.getPoints ( );
	const vector<size_t>&	polygons	= mdr.getCells ( );
	vtkUnstructuredGrid*	grid	=
				2 == getEntity ( )->getDim ( ) ? _surfacicGrid : _volumicGrid;
	vtkActor*				actor	=
				2 == getEntity ( )->getDim ( ) ? _surfacicActor : _volumicActor;
	bool					hideGrid	= false;
	if (0 == grid)
	{
		if (2 == getEntity ( )->getDim ( ))
			createSurfacicRepresentation ( );
		else
			createVolumicRepresentation ( );
		grid	= 2 == getEntity( )->getDim( ) ? _surfacicGrid : _volumicGrid;
		actor	= 2 == getEntity( )->getDim( ) ? _surfacicActor : _volumicActor;
		hideGrid	= true;
	}	// if (0 == grid)
	CHECK_NULL_PTR_ERROR (grid)
	CHECK_NULL_PTR_ERROR (actor)

	vtkExtractEdges*	edgesExtractor	= vtkExtractEdges::New ( );
#ifndef VTK_5
	edgesExtractor->SetInputData (grid);
#else	// VTK_5
	edgesExtractor->SetInput (grid);
#endif	// VTK_5

	_wireMapper	= vtkDataSetMapper::New ( );
#ifndef VTK_5
	_wireMapper->SetInputConnection (edgesExtractor->GetOutputPort ( ));
#else	// VTK_5
	_wireMapper->SetInput (edgesExtractor->GetOutput ( ));
#endif	// VTK_5
	_wireMapper->ScalarVisibilityOff ( );
#if	VTK_MAJOR_VERSION < 8
	_wireMapper->SetImmediateModeRendering (!Internal::Resources::instance ( )._useDisplayList);
#endif	// VTK_MAJOR_VERSION < 8
	_wireActor	= VTKMgx3DActor::New ( );
	_wireActor->SetEntity (getEntity ( ));
	_wireActor->SetRepresentationType (DisplayRepresentation::WIRE);
	const DisplayProperties&	properties	=
									getEntity ( )->getDisplayProperties ( );
	const Color&				wireColor	= properties.getWireColor ( );
	_wireActor->GetProperty ( )->SetColor (
			wireColor.getRed ( ), wireColor.getGreen ( ), wireColor.getBlue( ));	
	_wireActor->SetMapper (_wireMapper);
	actor->SetVisibility (!hideGrid);
	edgesExtractor->Delete ( );		edgesExtractor	= 0;
}	// VTKMgx3DMeshEntityRepresentation::createWireRepresentation


void VTKMgx3DMeshEntityRepresentation::createIsoWireRepresentation ( )
{
	if ((0 != _isoWireGrid) || (0 != _isoWireMapper) || (0 != _isoWireActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DMeshEntityRepresentation::createIsoWireRepresentation")
		throw exc;
	}	// if ((0 != _isoWireGrid) || ...

	MeshDisplayRepresentation	gr (DisplayRepresentation::ISOCURVE);
	getEntity ( )->getRepresentation (gr, true);
	vector<Math::Point>&	points		= gr.getPoints ( );
	vector<size_t>&			lines	= gr.getCurveDiscretization ( );
	VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation (
			getEntity ( ), _isoWireActor, _isoWireMapper,_isoWireGrid, points, lines);
}	// VTKMgx3DMeshEntityRepresentation::createIsoWireRepresentation


void VTKMgx3DMeshEntityRepresentation::createAssociationVectorRepresentation ( )
{
}	// VTKMgx3DMeshEntityRepresentation::createAssociationVectorRepresentation


bool VTKMgx3DMeshEntityRepresentation::getRefinedRepresentation (
	vector<Math::Point>& points, vector<size_t>& triangles, size_t factor)
{
	return false;	// Raffinement non fait comme demandé
}	// VTKMgx3DMeshEntityRepresentation::getRefinedRepresentation



// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtVtkComponents

}	// namespace Mgx3D
