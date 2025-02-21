/**
 * \file        VTKMgx3DStructuredMeshEntityRepresentation.cpp
 * \author      Charles PIGNEROL
 * \date        07/12/2018
 */

#include "Internal/ContextIfc.h"

#include "QtVtkComponents/VTKMgx3DStructuredMeshEntityRepresentation.h"
#include "QtComponents/QtMgx3DApplication.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkIdTypeArray.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>

#include <cstdint>
#include <iostream>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Structured;
using namespace Mgx3D::Utils;
using namespace Mgx3D::QtComponents;


namespace Mgx3D
{

namespace QtVtkComponents
{


// ===========================================================================
//                      LA CLASSE VTKMgx3DStructuredMeshEntityRepresentation
// ===========================================================================


VTKMgx3DStructuredMeshEntityRepresentation::VTKMgx3DStructuredMeshEntityRepresentation (StructuredMeshEntity& entity)
	: QtVtkComponents::VTKMgx3DEntityRepresentation (entity)
{
}	// VTKMgx3DStructuredMeshEntityRepresentation::VTKMgx3DStructuredMeshEntityRepresentation


VTKMgx3DStructuredMeshEntityRepresentation::VTKMgx3DStructuredMeshEntityRepresentation (VTKMgx3DStructuredMeshEntityRepresentation& ver)
	: QtVtkComponents::VTKMgx3DEntityRepresentation (*(ver.getEntity ( )))
{
	MGX_FORBIDDEN ("VTKMgx3DStructuredMeshEntityRepresentation copy constructor is not allowed.");
}	// VTKMgx3DStructuredMeshEntityRepresentation::VTKMgx3DStructuredMeshEntityRepresentation


VTKMgx3DStructuredMeshEntityRepresentation& VTKMgx3DStructuredMeshEntityRepresentation::operator = (const VTKMgx3DStructuredMeshEntityRepresentation& er)
{
	MGX_FORBIDDEN ("VTKMgx3DStructuredMeshEntityRepresentation assignment operator is not allowed.");
	if (&er != this)
	{
	}	// if (&er != this)

	return *this;
}	// VTKMgx3DStructuredMeshEntityRepresentation::operator =


VTKMgx3DStructuredMeshEntityRepresentation::~VTKMgx3DStructuredMeshEntityRepresentation ( )
{
	destroyRepresentations (true);
}	// VTKMgx3DStructuredMeshEntityRepresentation::~VTKMgx3DStructuredMeshEntityRepresentation


const StructuredMeshEntity& VTKMgx3DStructuredMeshEntityRepresentation::getStructuredMeshEntity ( ) const
{
	const StructuredMeshEntity*	entity	= dynamic_cast<const StructuredMeshEntity*>(getEntity ( ));
	CHECK_NULL_PTR_ERROR (entity)

	return *entity;
}	// VTKMgx3DStructuredMeshEntityRepresentation::getStructuredMeshEntity


StructuredMeshEntity& VTKMgx3DStructuredMeshEntityRepresentation::getStructuredMeshEntity ( )
{
	StructuredMeshEntity*	entity	= dynamic_cast<StructuredMeshEntity*>(getEntity ( ));
	CHECK_NULL_PTR_ERROR (entity)

	return *entity;
}	// VTKMgx3DStructuredMeshEntityRepresentation::getStructuredMeshEntity


void VTKMgx3DStructuredMeshEntityRepresentation::updateRepresentationProperties ( )
{
	vtkMapper*  mapper  = 0 == getVolumicMapper ( ) ? (vtkMapper*)getSurfacicMapper ( ) : (vtkMapper*)getVolumicMapper ( );
	const DisplayProperties properties	= getDisplayPropertiesAttributes ( );
	double                  domain [2]	= { 0., 0. };
	int                     colorNum    = USHRT_MAX;
	if (0 != mapper)
	{
		_volumicMapper->GetScalarRange (domain);
		vtkScalarsToColors* lut = _volumicMapper->GetLookupTable ( );
		if (0 != lut)
		{
			colorNum    = lut->GetNumberOfAvailableColors ( );
			domain [0]  = lut->GetRange ( )[0];
			domain [1]  = lut->GetRange ( )[1];
		}   // if (0 != lut)
	}   // if (0 != mapper)
	RenderingManager::ColorTableDefinition	colorTableDefinition (properties.getValueName ( ), colorNum, domain [0], domain [1]);
	VTKRenderingManager::VTKColorTable*     colorTable	= 0 == getRenderingManager ( ) ?
			0 : dynamic_cast<VTKRenderingManager::VTKColorTable*>(getRenderingManager ( )->getColorTable (colorTableDefinition));
	CHECK_NULL_PTR_ERROR (colorTable)

	if (0 != mapper)
	{
		mapper->UseLookupTableScalarRangeOn ( );
		mapper->SetLookupTable (&colorTable->getLookupTable ( ));
	}	// if (0 != mapper)
}	// VTKMgx3DStructuredMeshEntityRepresentation::updateRepresentationProperties


void VTKMgx3DStructuredMeshEntityRepresentation::createCloudRepresentation ( )
{
	if ((0 != _cloudGrid) || (0 != _cloudMapper) || (0 != _cloudActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
               "VTKMgx3DStructuredMeshEntityRepresentation::createCloudRepresentation")
		throw exc;
	}	// if ((0 != _cloudGrid) || ...

	throw Exception (UTF8String ("VTKMgx3DStructuredMeshEntityRepresentation::createCloudRepresentation is not supported.", Charset::UTF_8));
}	// VTKMgx3DStructuredMeshEntityRepresentation::createCloudRepresentation


void VTKMgx3DStructuredMeshEntityRepresentation::createSurfacicRepresentation ( )
{
	if ((0 != _surfacicGrid) || (0 != _surfacicMapper) || (0 != _surfacicActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
               "VTKMgx3DStructuredMeshEntityRepresentation::createSurfacicRepresentation")
		throw exc;
	}	// if ((0 != _surfacicGrid) || ...

	throw Exception (UTF8String ("VTKMgx3DStructuredMeshEntityRepresentation::createSurfacicRepresentation is not supported.", Charset::UTF_8));
}	// VTKMgx3DStructuredMeshEntityRepresentation::createSurfacicRepresentation


void VTKMgx3DStructuredMeshEntityRepresentation::createVolumicRepresentation ( )
{
	createVolumicRepresentation (getStructuredMeshEntity ( ).getMesh ( ));
}	// VTKMgx3DStructuredMeshEntityRepresentation::createVolumicRepresentation


void VTKMgx3DStructuredMeshEntityRepresentation::createVolumicRepresentation (const StructuredMesh& mesh)
{
	if (0 == getRenderingManager ( ))
	{
		cerr << "VTKMgx3DStructuredMeshEntityRepresentation::createVolumicRepresentation : null rendering manager." << endl;
		return;
	}   // if (0 == getRenderingManager ( ))

	const double	threshold	= getRenderingManager ( )->getStructuredDataThresholdValue ( );
	if ((0 != _volumicGrid) || (0 != _volumicMapper) || (0 != _volumicActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
               "VTKMgx3DStructuredMeshEntityRepresentation::createVolumicRepresentation")
		throw exc;
	}	// if ((0 != _volumicGrid) || ...

	_volumicGrid	= vtkUnstructuredGrid::New ( );
	_volumicMapper	= vtkDataSetMapper::New ( );
	_volumicMapper->SetInputData (_volumicGrid);
//	_volumicMapper->ScalarVisibilityOff ( );
#if	VTK_MAJOR_VERSION < 8
	_volumicMapper->SetImmediateModeRendering (!Internal::Resources::instance ( )._useDisplayList);
#endif	// VTK_MAJOR_VERSION < 8
	_volumicActor	  = VTKMgx3DActor::New ( );
	_volumicActor->SetEntity (getEntity ( ));
//	_volumicActor->SetRepresentationType (DisplayRepresentation::SOLID);

	CHECK_NULL_PTR_ERROR (getEntity ( ))
	const double*  abs 		= mesh.abs ( );
	const double*  ord 		= mesh.ord ( );
	const double*  ele 		= mesh.elev ( );
	const double*	densities	= mesh.cellDensities ( );
	CHECK_NULL_PTR_ERROR (abs)
	CHECK_NULL_PTR_ERROR (ord)
	CHECK_NULL_PTR_ERROR (ele)
	CHECK_NULL_PTR_ERROR (densities)
	// Dans cette version on impose l'affichage des densités pour valeurs aux mailles :
	getEntity ( )->getDisplayProperties ( ).setValueName ("densities");
	const DisplayProperties&	properties  = getEntity ( )->getDisplayProperties ( );
	const Color&	volumicColor   = properties.getVolumicColor ( );
//	_volumicActor->GetProperty ( )->SetColor (volumicColor.getRed ( ), volumicColor.getGreen ( ), volumicColor.getBlue ( ));
	_volumicActor->SetMapper (_volumicMapper);
	CHECK_NULL_PTR_ERROR (_volumicGrid)
	vtkPoints*  points  = vtkPoints::New ( );
	CHECK_NULL_PTR_ERROR (points)
	_volumicGrid->Initialize ( );

	const int64_t	ni	= mesh.ni ( ), nj	= mesh.nj ( ), nk	= mesh.nk ( );
	const size_t	pointsNum	= (ni+1) * (nj+1) * (nk+1);
	const size_t	cellsNum	= mesh.cellNum ( );
	// Les sommets :
	points->SetDataTypeToDouble ( );
	points->Allocate ((ni+1) * (nj+1) * (nk+1));
	for (int64_t i = 0; i <= ni; i++)
	{
		const vtkIdType iOffset = i * (nj + 1) * (nk + 1);
		for (int64_t j = 0; j <= nj; j++)
		{
			const vtkIdType jOffset = j * (nk + 1);
			for (int64_t k = 0; k <= nk; k++)
			{
				const vtkIdType offset  = iOffset + jOffset + k;
				// Warum : inverser i et k (issu des tests préliminaires à intégration dans Mgx3D)
				const double    coords [3]  = { abs [i], ord [j], ele [k] };
//const double    coords [3]  = { abs [k], ord [j], ele [i] };
				points->InsertPoint (offset, coords);
			}	// for (int64_t k = 0; i <= nk; k++)
		}	// for (int64_t j = 0; j <= nj; j++)
	}	// for (int64_t i = 0; i <= ni; i++)
	_volumicGrid->SetPoints (points);
	points->Delete ( );		points		= 0;

	vtkCellArray*	cellArray	= vtkCellArray::New ( );
	vtkIdTypeArray*	idArray		= vtkIdTypeArray::New ( );
	idArray->Allocate (9 * (ni * nj * nk), ni * nj * nk);
	vtkIdType*	cellsPtr	= idArray->GetPointer (0);
	int*		cellTypes	= new int [ni * nj * nk];
	size_t		c		= 0;
	for (int64_t i = 0; i < ni; i++)
	{
		const vtkIdType iOffset		= i * (nj + 1) * (nk + 1);
		const vtkIdType ip1Offset	= (i + 1) * (nj + 1) * (nk + 1);
		for (int64_t j = 0; j < nj; j++)
		{
			const vtkIdType jOffset		= j * (nk + 1);
			const vtkIdType jp1Offset	= (j + 1) * (nk + 1);
			for (int64_t k = 0; k < nk; k++)
			{
				const int64_t	id	= k * nj * ni + j * ni + i;
				if (densities [id] < threshold)
					continue;

				*cellsPtr++ = 8;    // Hexahedron
				*cellsPtr++ = iOffset + jOffset + k;
				*cellsPtr++ = ip1Offset + jOffset + k;
				*cellsPtr++ = ip1Offset + jp1Offset + k;
				*cellsPtr++ = iOffset + jp1Offset + k;
				*cellsPtr++ = iOffset + jOffset + k + 1;
				*cellsPtr++ = ip1Offset + jOffset + k + 1;
				*cellsPtr++ = ip1Offset + jp1Offset + k + 1;
				*cellsPtr++ = iOffset + jp1Offset + k + 1;
				cellTypes [c++] = VTK_HEXAHEDRON;
			}	// for (int64_t k = 0; k < nk; k++)
		}	// for (int64_t j = 0; j < nj; j++)
	}	// for (int64_t i = 0; i <= ni; i++)

	idArray->SetNumberOfValues (9 * c);
	cellArray->SetCells (c, idArray);
	cellArray->Squeeze ( );
	_volumicGrid->SetCells (cellTypes, cellArray);
	delete [] cellTypes;	cellTypes	= 0;
	cellArray->Delete ( );	cellArray	= 0;
	idArray->Delete ( );	idArray		= 0;

	// Les valeurs aux mailles :
	vtkSmartPointer<vtkDoubleArray> vdensities  = vtkDoubleArray::New ( );
	vdensities->SetName ("densities");
	vdensities->SetNumberOfValues (ni * nj * nk);
	c		= 0;
	int64_t	n	= 0;
	for (int64_t i = 0; i < ni; i++)
	{
		for (int64_t j = 0; j < nj; j++)
		{
			for (int64_t k = 0; k < nk; k++, n++)
			{
				const int64_t	    id	= k * nj * ni + j * ni + i;
				const double	d	= densities [id];
				if (d < threshold)
					continue;
	
				vdensities->SetValue (c++, d);
			}	// for (int64_t k = 0; k < nk; k++, n++)
		}	// for (int64_t j = 0; j < nj; j++)
	}	// for (int64_t i = 0; i < ni; i++)

	if (0 == c)
	{
		_volumicActor->SetMapper (0);
		_volumicMapper->SetInputData (0);
	}
	vdensities->SetNumberOfValues (c);
	vdensities->Squeeze ( );
	_volumicGrid->GetCellData ( )->AddArray (vdensities);
	_volumicGrid->GetCellData ( )->SetActiveAttribute ("densities", vtkDataSetAttributes::SCALARS);
	_volumicMapper->SetScalarModeToUseCellData ( );
	_volumicMapper->ScalarVisibilityOn ( );
	_volumicMapper->SetColorModeToMapScalars ( );
	_volumicMapper->SetScalarRange (mesh.densityMin ( ), mesh.densityMax ( ));
	RenderingManager::ColorTableDefinition  definition ("densities", 256, mesh.densityMin ( ), mesh.densityMax ( ));
	RenderingManager::ColorTable*       colorTable  = getRenderingManager ( )->getColorTable (definition);
	VTKRenderingManager::VTKColorTable* ct          = dynamic_cast<VTKRenderingManager::VTKColorTable*>(colorTable);
	if (0 != ct)
		_volumicMapper->SetLookupTable (&(ct->getLookupTable ( )));
}	// VTKMgx3DStructuredMeshEntityRepresentation::createVolumicRepresentation


void VTKMgx3DStructuredMeshEntityRepresentation::createWireRepresentation ( )
{
	if ((0 != _wireGrid) || (0 != _wireMapper) || (0 != _wireActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DStructuredMeshEntityRepresentation::createWireRepresentation")
		throw exc;
	}	// if ((0 != _wireGrid) || ...
	CHECK_NULL_PTR_ERROR (getEntity ( ))

	throw Exception (UTF8String ("VTKMgx3DStructuredMeshEntityRepresentation::createWireRepresentation is not supported.", Charset::UTF_8));
}	// VTKMgx3DStructuredMeshEntityRepresentation::createWireRepresentation


void VTKMgx3DStructuredMeshEntityRepresentation::createIsoWireRepresentation ( )
{
	if ((0 != _isoWireGrid) || (0 != _isoWireMapper) || (0 != _isoWireActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DStructuredMeshEntityRepresentation::createIsoWireRepresentation")
		throw exc;
	}	// if ((0 != _isoWireGrid) || ...

	throw Exception (UTF8String ("VTKMgx3DStructuredMeshEntityRepresentation::createIsoWireRepresentation is not supported.", Charset::UTF_8));
}	// VTKMgx3DStructuredMeshEntityRepresentation::createIsoWireRepresentation


void VTKMgx3DStructuredMeshEntityRepresentation::createAssociationVectorRepresentation ( )
{
}	// VTKMgx3DStructuredMeshEntityRepresentation::createAssociationVectorRepresentation


bool VTKMgx3DStructuredMeshEntityRepresentation::getRefinedRepresentation (
	vector<Math::Point>& points, vector<size_t>& triangles, size_t factor)
{
	return false;	// Raffinement non fait comme demandé
}	// VTKMgx3DStructuredMeshEntityRepresentation::getRefinedRepresentation



// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtVtkComponents

}	// namespace Mgx3D
