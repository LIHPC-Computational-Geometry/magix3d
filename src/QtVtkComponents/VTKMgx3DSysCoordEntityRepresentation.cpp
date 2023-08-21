/**
 * \file		VTKMgx3DSysCoordEntityRepresentation.cpp
 * \author		Eric Brière de l'Isle
 * \date		23/5/2018
 */

#include "Internal/ContextIfc.h"
#include "SysCoord/SysCoordDisplayRepresentation.h"

#include "QtVtkComponents/VTKMgx3DSysCoordEntityRepresentation.h"

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
using namespace Mgx3D::Utils;
using namespace Mgx3D::QtComponents;
using namespace Mgx3D::CoordinateSystem;

namespace Mgx3D
{

namespace QtVtkComponents
{


// ===========================================================================
//                      LA CLASSE VTKMgx3DSysCoordEntityRepresentation
// ===========================================================================


VTKMgx3DSysCoordEntityRepresentation::VTKMgx3DSysCoordEntityRepresentation (
		CoordinateSystem::SysCoord& entity)
	: QtVtkComponents::VTKMgx3DEntityRepresentation (entity)
{
}	// VTKMgx3DSysCoordEntityRepresentation::VTKMgx3DSysCoordEntityRepresentation


VTKMgx3DSysCoordEntityRepresentation::VTKMgx3DSysCoordEntityRepresentation (
										VTKMgx3DSysCoordEntityRepresentation& ver)
	: QtVtkComponents::VTKMgx3DEntityRepresentation (*(ver.getEntity ( )))
{
	MGX_FORBIDDEN ("VTKMgx3DSysCoordEntityRepresentation copy constructor is not allowed.");
}	// VTKMgx3DSysCoordEntityRepresentation::VTKMgx3DSysCoordEntityRepresentation


VTKMgx3DSysCoordEntityRepresentation& VTKMgx3DSysCoordEntityRepresentation::operator = (
									const VTKMgx3DSysCoordEntityRepresentation& er)
{
	MGX_FORBIDDEN ("VTKMgx3DSysCoordEntityRepresentation assignment operator is not allowed.");
	if (&er != this)
	{
	}	// if (&er != this)

	return *this;
}	// VTKMgx3DSysCoordEntityRepresentation::operator =


VTKMgx3DSysCoordEntityRepresentation::~VTKMgx3DSysCoordEntityRepresentation ( )
{
//	destroyRepresentations (true);	// Called by parent
}	// VTKMgx3DSysCoordEntityRepresentation::~VTKMgx3DSysCoordEntityRepresentation



void VTKMgx3DSysCoordEntityRepresentation::show (
							VTKRenderingManager& renderingManager, bool display)
{
	VTKMgx3DEntityRepresentation::show (renderingManager, display);
	vtkRenderer&	renderer	= renderingManager.getRenderer ( );
	if (true == display)
	{
		if (0 != _triedronXActor)
		{
			renderer.AddActor (_triedronXActor);
			renderer.AddActor (_triedronYActor);
			renderer.AddActor (_triedronZActor);
		}
	}
	else	// if (true == display)
	{
		if (0 != _triedronXActor)
		{
			renderer.RemoveActor (_triedronXActor);
			renderer.RemoveActor (_triedronYActor);
			renderer.RemoveActor (_triedronZActor);
		}
	}	// else if (true == display)
	//renderingManager.forceRender ( );    // Forcer l'actualisation de la vue 3D compte-tenu de l'affichage du trièdre

}	// VTKMgx3DSysCoordEntityRepresentation::show



void VTKMgx3DSysCoordEntityRepresentation::updateRepresentation (
												unsigned long mask, bool force)
{
	VTKMgx3DEntityRepresentation::updateRepresentation (mask, force);
}	// VTKMgx3DSysCoordEntityRepresentation::updateRepresentation



void VTKMgx3DSysCoordEntityRepresentation::createTrihedronRepresentation ( )
{
	if ((0 != _triedronXActor) || (0 != _triedronYActor) || (0 != _triedronZActor) ||
		(0 != _trihedronArrowX) || (0 != _trihedronArrowY) || (0 != _trihedronArrowZ) ||
		(0 != _triedronXMapper) || (0 != _triedronYMapper) || (0 != _triedronZMapper) )
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
                "VTKMgx3DSysCoordEntityRepresentation::createTrihedronRepresentation")
		throw exc;
	}	// if ((0 != _trihedron) || ...
	DisplayRepresentation::type	t	=
		(DisplayRepresentation::type)(DisplayRepresentation::SHOWTRIHEDRON);
	SysCoordDisplayRepresentation	scdr (t);
	getEntity ( )->getRepresentation (scdr, true);
	const Utils::Math::Point	center	= scdr.getTransform ( ).getCenter ( );
	const Utils::Math::Matrix33	transf	= scdr.getTransform ( ).getTransf ( );
    	vtkTransform* trans = vtkTransform::New ( );
    	CHECK_NULL_PTR_ERROR(trans);

    	trans->Identity();
    	vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
    	for (uint i=0; i<3; i++)
    		for (uint j=0;j<3; j++)
    			matrix->SetElement(j, i, transf.getCoord(i,j));

    	matrix->SetElement(0,3,center.getX());
    	matrix->SetElement(1,3,center.getY());
    	matrix->SetElement(2,3,center.getZ());

    	trans->SetMatrix(matrix);
    	matrix->Delete(); 
	VTKMgx3DEntityRepresentation::createTrihedronRepresentation (trans);
	trans->Delete ( );
}	// VTKMgx3DSysCoordEntityRepresentation::createTrihedronRepresentation




void VTKMgx3DSysCoordEntityRepresentation::destroyRepresentations (bool realyDestroy)
{
	VTKMgx3DEntityRepresentation::destroyRepresentations (realyDestroy);
}	// VTKMgx3DSysCoordEntityRepresentation::destroyRepresentations ( )


void VTKMgx3DSysCoordEntityRepresentation::updateRepresentationProperties ( )
{
	VTKMgx3DEntityRepresentation::updateRepresentationProperties ( );

}	// VTKMgx3DSysCoordEntityRepresentation::updateRepresentationProperties


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtVtkComponents

}	// namespace Mgx3D
