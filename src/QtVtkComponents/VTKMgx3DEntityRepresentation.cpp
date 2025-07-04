/**
 * \file        VTKMgx3DEntityRepresentation.cpp
 * \author      Charles PIGNEROL
 * \date        28/11/2011
 */

#include "Internal/Context.h"
#include "Internal/Resources.h"

#include "QtVtkComponents/VTKMgx3DEntityRepresentation.h"
#include "QtComponents/QtMgx3DApplication.h"

#include "Utils/DisplayProperties.h"
#include "Utils/MgxNumeric.h"
#include "Utils/Vector.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <vtkCellArray.h>
#include <vtkCellType.h>
#include <vtkDoubleArray.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkLine.h>
#include <vtkMatrix4x4.h>
#include <vtkTriangle.h>
#include <vtkTransform.h>
#include <vtkVertex.h>
#include <vtkRenderer.h>

#include <iostream>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;


namespace Mgx3D
{

namespace QtVtkComponents
{


static double distance (
			double x1, double y1, double z1, double x2, double y2, double z2)
{
	return Point (x2 - x1, y2 - y1, z2 - z1).norme ( );
}	// distance


static bool sameDirection (vtkTransform& transform, const vector<Point>& vector)
{
	// Fonction très spécialisée !
	double p0 [3] = {0., 0., 0.}, p1 [3]	= {1., 0., 0.};
	double t0 [3]	= {0., 0., 0.}, t1 [3] = {0., 0., 0.};
	transform.TransformPoint (p0, t0);
	transform.TransformPoint (p1, t1);
	if ((distance (
			t0[0], t0[1], t0[2], vector[0].getX ( ), vector[0].getY ( ), vector[0].getZ ( )) > MgxNumeric::mgxTopoDoubleEpsilon) ||
	    (distance (
			t1[0], t1[1], t1[2], vector[1].getX ( ), vector[1].getY ( ), vector[1].getZ ( )) > MgxNumeric::mgxTopoDoubleEpsilon))
		return false;

	return true;
}	// sameDirection


// ===========================================================================
//                      LA CLASSE VTKMgx3DEntityRepresentation
// ===========================================================================


VTKMgx3DEntityRepresentation::VTKMgx3DEntityRepresentation (Entity& entity)
	: VTKEntityRepresentation (entity)
{
}	// VTKMgx3DEntityRepresentation::VTKMgx3DEntityRepresentation


VTKMgx3DEntityRepresentation::VTKMgx3DEntityRepresentation (VTKMgx3DEntityRepresentation& ver)
	: VTKEntityRepresentation (*(ver.getEntity ( )))
{
	MGX_FORBIDDEN ("VTKMgx3DEntityRepresentation copy constructor is not allowed.");
}	// VTKMgx3DEntityRepresentation::VTKMgx3DEntityRepresentation


VTKMgx3DEntityRepresentation& VTKMgx3DEntityRepresentation::operator = (const VTKMgx3DEntityRepresentation& er)
{
	MGX_FORBIDDEN ("VTKMgx3DEntityRepresentation assignment operator is not allowed.");
	if (&er != this)
	{
	}	// if (&er != this)

	return *this;
}	// VTKMgx3DEntityRepresentation::operator =


VTKMgx3DEntityRepresentation::~VTKMgx3DEntityRepresentation ( )
{
	destroyRepresentations (true);
}	// VTKMgx3DEntityRepresentation::~VTKMgx3DEntityRepresentation


void VTKMgx3DEntityRepresentation::createPointsCloudRepresentation (
	Entity* entity, VTKMgx3DActor*& actor, vtkDataSetMapper*& mapper,
	vtkUnstructuredGrid*& grid,
	const vector<Math::Point>& meshPts)
{
	if ((0 != grid) || (0 != mapper) || (0 != actor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.",
		             "VTKMgx3DEntityRepresentation::createPointsCloudRepresentation")
		throw exc;
	}	// if ((0 != grid) || ...
	grid		= vtkUnstructuredGrid::New ( );
	mapper	= vtkDataSetMapper::New ( );
	mapper->SetInputData (grid);	
	mapper->ScalarVisibilityOff ( );
#if	VTK_MAJOR_VERSION < 8
	mapper->SetImmediateModeRendering (!Internal::Resources::instance ( )._useDisplayList);
#endif	// VTK_MAJOR_VERSION < 8
	actor		= VTKMgx3DActor::New ( );
	actor->SetEntity (entity);
	actor->SetRepresentationType (
		(DisplayRepresentation::type)(DisplayRepresentation::SOLID | DisplayRepresentation::WIRE));
	DisplayProperties	properties;
	if (0 != entity)
		properties	= entity->getDisplayProperties ( );
	const Color&	cloudColor	= properties.getCloudColor ( );
	actor->GetProperty ( )->SetColor (
		cloudColor.getRed ( ) / 255., cloudColor.getGreen ( ) / 255.,
		cloudColor.getBlue ( ) / 255.);
	actor->SetMapper (mapper);
	CHECK_NULL_PTR_ERROR (grid)
	vtkPoints*	points	= vtkPoints::New ( );
	CHECK_NULL_PTR_ERROR (points)
	grid->Initialize ( );
	const size_t	pointsNum		= meshPts.size ( );
	// Les sommets :
	points->SetDataTypeToDouble ( );
	points->SetNumberOfPoints (pointsNum);
	grid->Allocate (pointsNum, 1000);
	grid->SetPoints (points);
	vtkIdType	id	= 0;
	for (vector<Math::Point>::const_iterator itp = meshPts.begin ( );
	     meshPts.end ( ) != itp; itp++, id++)
	{
		// InsertPoint : range checking, réallocation si nécessaire.
		// SetPoints   : pas de range checking ou  réallocation.
//		points->InsertPoint(id, (*itp).getX( ), (*itp).getY( ), (*itp).getZ( ));
		double	coords [3] = {(*itp).getX( ), (*itp).getY( ), (*itp).getZ( )};
		points->SetPoint (id, coords);
	}	// for (vector<Math::Point>::const_iterator itp = meshPts.begin ( );

	// Les points : on part ici du principe qu'on a que des points.
#ifndef VTK_SLOW_CODE
#	ifndef VTK_VERTEX_GENERALIZATION
	// => Spécialisation : triangles
	vtkCellArray*	cellArray	= vtkCellArray::New ( );
	vtkIdTypeArray*	idsArray	= vtkIdTypeArray::New ( );
	// idsArray est forcément du type : nbPts p1 p2 .. pn pour chaque point,
	// à savoir qu'on ne peut s'affranchir de mettre nbPts si toutes les mailles
	// sont de même type (vertex) et que c'est renseigné par ailleurs via
	// grid->SetCells (VTK_VERTEX, cellArray);
	// Par rapport au cas général (polygones de types différents) on fait tout
	// de même l'économie du tableau cellTypes de taille 
	// pointsNum * sizeof (int).
	idsArray->SetNumberOfValues (2 * pointsNum);
	vtkIdType*		cellsPtr	= idsArray->GetPointer (0);
	for (id = 0; id < pointsNum; id++)
	{
		size_t	pos	= 2 * id;
		cellsPtr [pos]	= 1;
		cellsPtr [pos + 1]	= id;
	}	// for (id = 0; id < pointsNum; id++)
	idsArray->Squeeze ( );
	cellArray->SetCells (pointsNum, idsArray);
	cellArray->Squeeze ( );
	grid->SetCells (VTK_VERTEX, cellArray);
	idsArray->Delete ( );	idsArray	= 0;
	cellArray->Delete ( );	cellArray	= 0;
#	else	// VTK_VERTEX_GENERALIZATION
	int*			cellTypes	= new int [pointsNum];
	vtkCellArray*	cellArray	= vtkCellArray::New ( );
	vtkIdTypeArray*	idsArray	= vtkIdTypeArray::New ( );
	idsArray->SetNumberOfValues (2 * pointsNum);
	vtkIdType*		cellsPtr	= idsArray->GetPointer (0);
	for (id = 0; id < pointsNum; id++)
	{
		size_t	pos	= 2 * id;
		cellTypes [id]	= VTK_VERTEX;
		cellsPtr [pos]	= 1;
		cellsPtr [pos + 1]	= id;
	}	// for (id = 0; id < pointsNum; id++)
	idsArray->Squeeze ( );
	cellArray->SetCells (pointsNum, idsArray);
	cellArray->Squeeze ( );
	grid->SetCells (cellTypes, cellArray);
	delete [] cellTypes;	cellTypes	= 0;
	idsArray->Delete ( );	idsArray	= 0;
	cellArray->Delete ( );	cellArray	= 0;
#	endif	//  VTK_VERTEX_GENERALIZATION
#else	// VTK_SLOW_CODE
	// Ca marche mais l'algo est loin d'être optimal :
	vtkVertex* vertex	= vtkVertex::New ( );
	for (id = 0; id < pointsNum; id++)
	{
		vertex->GetPointIds ( )->SetId (0, id);
		grid->InsertNextCell (
						vertex->GetCellType ( ), vertex->GetPointIds ( ));
	}	// for (id = 0; id < pointsNum; id++)
	vertex->Delete ( );
#endif	// VTK_SLOW_CODE
	if (0 != points)
		points->Delete ( );
}	// VTKMgx3DEntityRepresentation::createPointsCloudRepresentation


void VTKMgx3DEntityRepresentation::createTrianglesSurfacicRepresentation (
			Mgx3D::Utils::Entity* entity,
			VTKMgx3DActor*& actor, vtkPolyDataMapper*& mapper,
			vtkPolyData*& polydata,
			const std::vector<Mgx3D::Utils::Math::Point>& meshPts,
			const std::vector<size_t>& triangles)
{
	if ((0 != polydata) || (0 != mapper) || (0 != actor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.", "VTKMgx3DEntityRepresentation::createTrianglesSurfacicRepresentation")
		throw exc;
	}	// if ((0 != polydata) || ...

	polydata	= vtkPolyData::New ( );
	mapper		= vtkPolyDataMapper::New ( );
	mapper->SetInputData (polydata);
	mapper->ScalarVisibilityOff ( );
#if	VTK_MAJOR_VERSION < 8
	mapper->SetImmediateModeRendering (!Internal::Resources::instance ( )._useDisplayList);
#endif	// VTK_MAJOR_VERSION < 8
	actor		= VTKMgx3DActor::New ( );
	actor->SetEntity (entity);
	actor->SetRepresentationType (DisplayRepresentation::SOLID);

	CHECK_NULL_PTR_ERROR (entity)
	const DisplayProperties&	properties	= entity->getDisplayProperties ( );
	const Color&	surfacicColor	= properties.getSurfacicColor ( );
	actor->GetProperty ( )->SetColor (surfacicColor.getRed ( ) / 255., surfacicColor.getGreen ( ) / 255., surfacicColor.getBlue ( ) / 255.);
	actor->SetMapper (mapper);
	CHECK_NULL_PTR_ERROR (polydata)
	vtkPoints*	points	= vtkPoints::New ( );
	CHECK_NULL_PTR_ERROR (points)
	polydata->Initialize ( );
	const size_t	pointsNum	= meshPts.size ( );
	const size_t	trianglesNum	= triangles.size ( ) / 3;
	// Les sommets :
	points->SetDataTypeToDouble ( );
	points->SetNumberOfPoints (pointsNum);
	polydata->Allocate (trianglesNum, 1000);
	polydata->SetPoints (points);
	vtkIdType	id	= 0;
	for (vector<Math::Point>::const_iterator itp = meshPts.begin ( ); meshPts.end ( ) != itp; itp++, id++)
	{
		// InsertPoint : range checking, réallocation si nécessaire.
		// SetPoints   : pas de range checking ou  réallocation.
//		points->InsertPoint(id, (*itp).getX( ), (*itp).getY( ), (*itp).getZ( ));
		double	coords [3] = {(*itp).getX( ), (*itp).getY( ), (*itp).getZ( )};
		points->SetPoint (id, coords);
	}	// for (vector<Math::Point>::const_iterator itp = meshPts.begin ( );

// CP NEW CODE
	// Les triangles : on part ici du principe qu'on a que des triangles.
	vtkCellArray*	cellArray	= vtkCellArray::New ( );
	vtkIdTypeArray*	idsArray	= vtkIdTypeArray::New ( );
	CHECK_NULL_PTR_ERROR (cellArray)
	CHECK_NULL_PTR_ERROR (idsArray)
	idsArray->SetNumberOfValues (4 * trianglesNum);
	vtkIdType*		cellsPtr	= idsArray->GetPointer (0);
	size_t			pos			= 0;
	for (id = 0; id < trianglesNum; id++)
	{
		cellsPtr [pos++]	= 3;
		for (size_t i = 0; i < 3; i++)
			cellsPtr [pos++]	= triangles [3 * id + i];
	}	// for (id = 0; id < trianglesNum; id++)
	cellArray->SetCells (trianglesNum, idsArray);
	polydata->SetPolys (cellArray);
	polydata->BuildCells ( );	// Indispensable à vtkCellPicker, sinon plantage (:
	idsArray->Delete ( );	idsArray	= 0;
	cellArray->Delete ( );	cellArray	= 0;
	points->Delete ( );		points		= 0;
/* ============================================
	// Les triangles : on part ici du principe qu'on a que des triangles.
	vtkCellArray*	cellArray	= vtkCellArray::New ( );
	CHECK_NULL_PTR_ERROR (cellArray)
	cellArray->Allocate (3 * trianglesNum, 3 * trianglesNum);
	for (id = 0; id < trianglesNum; id++)
	{
		vtkIdType	pts [VTK_CELL_SIZE]	= { 0, 0, 0 };
		for (size_t i = 0; i < 3; i++)
			pts [i]	= triangles [3 * id + i];
		cellArray->InsertNextCell (3, pts);
	}	// for (vector<size_t>::iterator itt = mesh->begin( ); ...
	cellArray->Squeeze ( );
	polydata->SetPolys (cellArray);
	cellArray->Delete ( );	cellArray	= 0;
	points->Delete ( );	points	= 0;
*/
}	// VTKMgx3DEntityRepresentation::createTrianglesSurfacicRepresentation


void VTKMgx3DEntityRepresentation::createTrianglesSurfacicRepresentation (const vector<Math::Point>& meshPts, const vector<size_t>& mesh)
{
	createTrianglesSurfacicRepresentation (getEntity ( ), _surfacicActor, _surfacicMapper, _surfacicPolyData, meshPts, mesh);
}	// VTKMgx3DEntityRepresentation::createTrianglesSurfacicRepresentation


void VTKMgx3DEntityRepresentation::createPolygonsSurfacicRepresentation (const vector<Math::Point>& meshPts, const vector<size_t>& mesh)
{
	if ((0 != _surfacicPolyData) || (0 != _surfacicMapper) || (0 != _surfacicActor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.", "VTKMgx3DEntityRepresentation::createPolygonsSurfacicRepresentation")
		throw exc;
	}	// if ((0 != _surfacicPolyData) || ...
	_surfacicPolyData		= vtkPolyData::New ( );
	_surfacicMapper		= vtkPolyDataMapper::New ( );
	_surfacicMapper->SetInputData (_surfacicPolyData);
	_surfacicMapper->ScalarVisibilityOff ( );
#if	VTK_MAJOR_VERSION < 8
	_surfacicMapper->SetImmediateModeRendering (!Internal::Resources::instance ( )._useDisplayList);
#endif	// VTK_MAJOR_VERSION < 8	
	_surfacicActor		= VTKMgx3DActor::New ( );
	_surfacicActor->SetEntity (getEntity ( ));
	_surfacicActor->SetRepresentationType (DisplayRepresentation::SOLID);

	CHECK_NULL_PTR_ERROR (getEntity ( ))
	const DisplayProperties&	properties	= getEntity ( )->getDisplayProperties ( );
	const Color&	surfacicColor	= properties.getSurfacicColor ( );
	_surfacicActor->GetProperty ( )->SetColor (
		surfacicColor.getRed ( ) / 255., surfacicColor.getGreen ( ) / 255.,
		surfacicColor.getBlue ( ) / 255.);
	_surfacicActor->SetMapper (_surfacicMapper);
	CHECK_NULL_PTR_ERROR (_surfacicPolyData)
	vtkPoints*	points	= vtkPoints::New ( );
	CHECK_NULL_PTR_ERROR (points)
	_surfacicPolyData->Initialize ( );
	const size_t	pointsNum		= meshPts.size ( );
	const size_t	polygonsNum	= mesh.size ( ) / 3;	// Approximation ...
	
	// Les sommets :
	points->SetDataTypeToDouble ( );
	points->SetNumberOfPoints (pointsNum);
	_surfacicPolyData->Allocate (polygonsNum, 1000);
	_surfacicPolyData->SetPoints (points);
	vtkIdType	id	= 0;
	for (vector<Math::Point>::const_iterator itp = meshPts.begin ( );
	     meshPts.end ( ) != itp; itp++, id++)
	{
		double	coords [3] = {(*itp).getX( ), (*itp).getY( ), (*itp).getZ( )};
		points->SetPoint (id, coords);
	}	// for (vector<Math::Point>::const_iterator itp = meshPts.begin ( );

	const size_t	meshSize	= mesh.size ( );
	vtkCellArray*	cellArray	= vtkCellArray::New ( );
	vtkIdTypeArray*	idsArray	= vtkIdTypeArray::New ( );
	idsArray->SetNumberOfValues (meshSize);
	vtkIdType*		cellsPtr	= idsArray->GetPointer (0);
	size_t			current		= 0, pos	= 0;
	for (id = 0, pos = 0; pos < meshSize; id++)
	{
		const size_t	count	= mesh [current++];
		cellsPtr [pos++]	= count;
		for (size_t j = 0; j < count; j++)
			cellsPtr [pos++]	= mesh [current++];
	}	// for (id = 0; id < polygonsNum; id++)
	idsArray->Squeeze ( );
	cellArray->SetCells (id, idsArray);
	cellArray->Squeeze ( );
	_surfacicPolyData->SetPolys (cellArray);
	_surfacicPolyData->BuildCells ( );
	idsArray->Delete ( );	idsArray	= 0;
	cellArray->Delete ( );	cellArray	= 0;
	points->Delete ( );		points		= 0;
}	// VTKMgx3DEntityRepresentation::createPolygonsSurfacicRepresentation


void VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation (
		Entity* entity, VTKMgx3DActor*& actor, vtkPolyDataMapper*& mapper, vtkPolyData*& grid, const vector<Math::Point>& meshPts, const vector<size_t>& mesh)
{
	if ((0 != grid) || (0 != mapper) || (0 != actor))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.", "VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation")
		throw exc;
	}	// if ((0 != grid) || ...
	grid		= vtkPolyData::New ( );
	mapper		= vtkPolyDataMapper::New ( );
	mapper->SetInputData (grid);
	mapper->ScalarVisibilityOff ( );
#if	VTK_MAJOR_VERSION < 8
	mapper->SetImmediateModeRendering (!Internal::Resources::instance ( )._useDisplayList);
#endif	// VTK_MAJOR_VERSION < 8
	actor		= VTKMgx3DActor::New ( );
	actor->SetEntity (entity);
	actor->SetRepresentationType (DisplayRepresentation::WIRE);
	DisplayProperties	properties;
	if (0 != entity)
		properties	= entity->getDisplayProperties ( );
	const Color&	wireColor	= properties.getWireColor ( );
	actor->GetProperty ( )->SetColor (wireColor.getRed ( ) / 255., wireColor.getGreen ( ) / 255., wireColor.getBlue ( ) / 255.);
	actor->SetMapper (mapper);
	CHECK_NULL_PTR_ERROR (grid)
	vtkPoints*	points	= vtkPoints::New ( );
	CHECK_NULL_PTR_ERROR (points)
	grid->Initialize ( );
	const size_t	pointsNum	= meshPts.size ( );
	const size_t	edgesNum	= mesh.size ( ) / 2;

	// Les sommets :
	points->SetDataTypeToDouble ( );
	points->SetNumberOfPoints (pointsNum);
	grid->Allocate (edgesNum, 1000);
	grid->SetPoints (points);
	vtkIdType	id	= 0;
	for (vector<Math::Point>::const_iterator itp = meshPts.begin ( ); meshPts.end ( ) != itp; itp++, id++)
	{
		// InsertPoint : range checking, réallocation si nécessaire.
		// SetPoints   : pas de range checking ou  réallocation.
//		points->InsertPoint(id, (*itp).getX( ), (*itp).getY( ), (*itp).getZ( ));
		double	coords [3] = {(*itp).getX( ), (*itp).getY( ), (*itp).getZ( )};
		points->SetPoint (id, coords);
	}	// for (vector<Math::Point>::const_iterator itp = meshPts.begin ( );

	// Les segments :
	vtkCellArray*	cellArray	= vtkCellArray::New ( );
	CHECK_NULL_PTR_ERROR (cellArray)
	cellArray->Allocate (2 * edgesNum, 2 * edgesNum);
	for (id = 0; id < edgesNum; id++)
	{
		vtkIdType	pts [VTK_CELL_SIZE]	= { 0, 0 };
		for (size_t i = 0; i < 2; i++)
			pts [i]	= mesh [2 * id + i];
		cellArray->InsertNextCell (2, pts);
	}	// for (id = 0; id < edgesNum; id++
	cellArray->Squeeze ( );
	grid->SetLines (cellArray);
	grid->BuildCells ( );
	cellArray->Delete ( );	cellArray	= 0;
	if (0 != points)
		points->Delete ( );
}	// VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation


void VTKMgx3DEntityRepresentation::createAssociationVectorRepresentation (const vector<Point>& vector, const Color& color)
{
	if (true == vector.empty ( ))
		return;

	if (2 != vector.size ( ))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Arguments invalides (" << vector.size ( ) << " points pour le vecteur.";
		INTERNAL_ERROR (exc, message, "VTKMgx3DEntityRepresentation::createAssociationVectorRepresentation")
		throw exc;
	}	// if (2 != vector.size ( ))

	if ((0 != _vectAssArrow) || (0 != _vectAssActor) || (0 != _vectAssMapper))
	{
		INTERNAL_ERROR (exc, "Représentation déjà créée.", "VTKMgx3DEntityRepresentation::createAssociationVectorRepresentation")
		throw exc;
	}	// if ((0 != _vectAssArrow) || ...

	// Points confondus => pas de représentation.
	//if (vector [0] == vector [1])
	if (vector [0].isEpsilonEqual(vector [1], Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon))
		return;

	// Soit les vecteurs u, flèche VTK initiale, et v, flèche demandée.
	// Soit w un vecteur ortogonal à u et v, autour duquel est effectuée la rotation (=> rotation autour d'un angle quelconque).
	// Aspects redimensionnement : On passe d'un vecteur de norme 1 sur X à un vecteur de norme v.abs ( ) => multiplication de mat (0, 0) par v.abs ( ).
	const Vector	u (1., 0., 0.), v (vector [0], vector [1]);
	const Vector	w	= u * v;	// produit vectoriel
	// Rem : u et v sont non nuls, u car connu, v car testé ci-dessus.
	const double	cosinus	= scaMul (u, v) / (u.abs ( ) * v.abs ( ));
	const double	sinus	= std::sqrt (1 - cosinus * cosinus);

	// A partir des mêmes données et du même raisonnement on laisse le soin
	// à VTK de calculer la matrice de transformation.
	vtkTransform*	transform	= vtkTransform::New ( );	// Identité
	transform->Scale (v.abs ( ), v.abs ( ), v.abs ( ));
	const double	angle	= acos (cosinus) * 180. / M_PI;
	transform->RotateWXYZ (angle, w.getX ( ), w.getY ( ), w.getZ ( ));
	transform->PostMultiply ( );
	// Le vecteur recherché et celui de référence sont ils orientés dans le même sens ? On évalue avec une vtkTransform temporaire, et on finalise
	// la transformation selon le résultat de l'évaluation :
	vtkTransform*	transformTest	= vtkTransform::New ( );
	transformTest->DeepCopy (transform);
	transformTest->Translate (vector [0].getX ( ), vector [0].getY ( ), vector [0].getZ ( ));
	const bool	sameDir	= sameDirection (*transformTest, vector);
	transformTest->Delete ( );
	if (true == sameDir)
		transform->Translate (vector [0].getX ( ), vector [0].getY ( ), vector [0].getZ ( ));
	else
		transform->Translate (vector [1].getX ( ), vector [1].getY ( ), vector [1].getZ ( ));

	// On récupère le facteur d'agrandissement, à des fins esthétiques (cf.
	// suite).
	double	xyzScales [3]	= { 0., 0., 0. };
	transform->GetScale (xyzScales);
	const double	scale	= xyzScales [0];

	_vectAssArrow	= vtkArrowSource::New ( );
	_vectAssMapper	= vtkPolyDataMapper::New ( );
	_vectAssActor	= VTKMgx3DActor::New ( );
	_vectAssActor->SetEntity (getEntity ( ));
	CHECK_NULL_PTR_ERROR (getEntity ( ))
	CHECK_NULL_PTR_ERROR (_vectAssArrow)
	CHECK_NULL_PTR_ERROR (_vectAssMapper)
	CHECK_NULL_PTR_ERROR (_vectAssActor)
	_vectAssActor->SetUserMatrix (transform->GetMatrix ( ));
	transform->Delete ( );		transform	= 0;

	// La gueule de la flèche :
	// Tip   : tête de la flèche (partie conique)
	// Shaft : base de la flèche
	// Rem : la flèche va être redimensionnée (cf. transformation ci-dessus).
	// Sa longueur avant transformation est 1 ((0, 0, 0) -> (1, 0, 0)). => à prendre en compte dans les longueurs des rayons.
	_vectAssArrow->SetShaftResolution (24);
	_vectAssArrow->SetTipResolution (24);
	// On veut que les diamètres du cône et de la tige ne soient pas excessif.
	// => On bride celui de la tige, et on dit que celui du cône est 3 x plus grand.
	// Rem : c'est un peu de la cuisine, si ça ne donne pas satisfaction il serait bon de coder une classe vtkArrowSource sur laquelle on soit en
	// mesure d'imposer la taille du cône et de la tige (Points 1 et 2,  SetRadius). Cela devrait être possible à partir de vtkLineSource et
	// vtkConeSource, en héritant de vtkPolyDataAlgorithm, en surchargeant  GetOutputDataObject et/ou GetOutputPort et utilisant
	// SetNumberOfOutputPorts. Peut être passer par un vtkMergeFilter.
//	double	shaftRadiusPercent	= 0.03;		// OK en général
//	const double	length	= vector [0].length (vector [1]);
	const double	arrowComul	= ((0 != getRenderingManager ( )) && (true == getRenderingManager ( )->useGlobalDisplayProperties ( ))) ?
			getRenderingManager ( )->getContext ( ).globalDisplayProperties (getEntity ( )->getType ( )).getArrowComul ( ) : getEntity ( )->getDisplayProperties ( ).getArrowComul ( );
	const double	targetShaftRadius	= 5.;
	double		shaftRadiusPercent	= targetShaftRadius / scale;
	//std::cout<<"shaftRadiusPercent = "<<shaftRadiusPercent<<std::endl;
	if (shaftRadiusPercent > 0.03)
		shaftRadiusPercent	= 0.03;
	shaftRadiusPercent	*= arrowComul;
	double	tipRadiusPercent	= 3 * shaftRadiusPercent;
	// Longueur du rayon de la base :
	_vectAssArrow->SetShaftRadius (shaftRadiusPercent);
	// Longueur du rayon du cône :
	_vectAssArrow->SetTipRadius (tipRadiusPercent);
	_vectAssArrow->SetInvert (!sameDir);	// Inverser la flèche ?
	_vectAssMapper->SetInputConnection (_vectAssArrow->GetOutputPort ( ));
	_vectAssMapper->ScalarVisibilityOff ( );
	_vectAssActor->GetProperty ( )->SetColor (color.getRed ( ) / 255., color.getGreen ( ) / 255., color.getBlue ( ) / 255.);
	_vectAssActor->SetMapper (_vectAssMapper);
}	// VTKMgx3DEntityRepresentation::createAssociationVectorRepresentation


void VTKMgx3DEntityRepresentation::createTrihedronRepresentation (vtkTransform* transf)
{
	CHECK_NULL_PTR_ERROR (getEntity ( ));

	_trihedronArrowX	= vtkArrowSource::New ( );
	_trihedronArrowY	= vtkArrowSource::New ( );
	_trihedronArrowZ	= vtkArrowSource::New ( );
	_triedronXMapper	= vtkPolyDataMapper::New ( );
	_triedronYMapper	= vtkPolyDataMapper::New ( );
	_triedronZMapper	= vtkPolyDataMapper::New ( );
	CHECK_NULL_PTR_ERROR(_trihedronArrowX);
	CHECK_NULL_PTR_ERROR(_trihedronArrowY);
	CHECK_NULL_PTR_ERROR(_trihedronArrowZ);
	CHECK_NULL_PTR_ERROR(_triedronXMapper);
	CHECK_NULL_PTR_ERROR(_triedronYMapper);
	CHECK_NULL_PTR_ERROR(_triedronZMapper);

	_triedronXActor	= VTKMgx3DActor::New ( );
	_triedronYActor	= VTKMgx3DActor::New ( );
	_triedronZActor	= VTKMgx3DActor::New ( );
	CHECK_NULL_PTR_ERROR(_triedronXActor);
	CHECK_NULL_PTR_ERROR(_triedronYActor);
	CHECK_NULL_PTR_ERROR(_triedronZActor);
	_triedronXActor->SetEntity (getEntity ( ));
	_triedronYActor->SetEntity (getEntity ( ));
	_triedronZActor->SetEntity (getEntity ( ));

	_triedronYActor->RotateZ(90);
	_triedronZActor->RotateY (-90);

	_triedronXActor->SetUserMatrix (transf->GetMatrix ( ));
	_triedronYActor->SetUserMatrix (transf->GetMatrix ( ));
	_triedronZActor->SetUserMatrix (transf->GetMatrix ( ));

	int ArrowResolution = 8;
	int TipResolution   = 12;

	_trihedronArrowX->SetShaftResolution (ArrowResolution);
	_trihedronArrowX->SetTipResolution (TipResolution);
	_trihedronArrowY->SetShaftResolution (ArrowResolution);
	_trihedronArrowY->SetTipResolution (TipResolution);
	_trihedronArrowZ->SetShaftResolution (ArrowResolution);
	_trihedronArrowZ->SetTipResolution (TipResolution);

//	std::cout<<"shaftRadius = "<<_trihedronArrowX->GetShaftRadius()<<std::endl;
//	std::cout<<"tipRadius = "<<_trihedronArrowX->GetTipRadius()<<std::endl;

	double shaftRadius = 0.03; // 0.03 par défaut
	double tipRadius = 0.1; // 0.1 par défaut
	// rayon du cylindre
	_trihedronArrowX->SetShaftRadius (shaftRadius);
	_trihedronArrowY->SetShaftRadius (shaftRadius);
	_trihedronArrowY->SetShaftRadius (shaftRadius);
	// Rayon du cône :
	_trihedronArrowX->SetTipRadius (tipRadius);
	_trihedronArrowY->SetTipRadius (tipRadius);
	_trihedronArrowZ->SetTipRadius (tipRadius);

	_triedronXMapper->SetInputConnection (_trihedronArrowX->GetOutputPort ( ));
	_triedronYMapper->SetInputConnection (_trihedronArrowY->GetOutputPort ( ));
	_triedronZMapper->SetInputConnection (_trihedronArrowZ->GetOutputPort ( ));
	_triedronXMapper->ScalarVisibilityOff ( );
	_triedronYMapper->ScalarVisibilityOff ( );
	_triedronZMapper->ScalarVisibilityOff ( );
	_triedronXActor->GetProperty ( )->SetColor (1,0,0); // rouge
	_triedronYActor->GetProperty ( )->SetColor (0,1,0); // vert
	_triedronZActor->GetProperty ( )->SetColor (0,0,1); //bleu
	_triedronXActor->SetMapper (_triedronXMapper);
	_triedronYActor->SetMapper (_triedronYMapper);
	_triedronZActor->SetMapper (_triedronZMapper);

	_triedronXActor->PickableOn ( );
	_triedronYActor->PickableOn ( );
	_triedronZActor->PickableOn ( );
}

void VTKMgx3DEntityRepresentation::createRefinedRepresentation (size_t factor)
{
	CHECK_NULL_PTR_ERROR (getEntity ( ))
	destroyRefinedRepresentation ( );

	vector<Math::Point>	points;
	vector<size_t>		triangles;
	const bool			shouldRefine	= !getRefinedRepresentation (points, triangles, factor);
	switch (getEntity ( )->getDim ( ))
	{
		case 2	:
		case 3	:
			VTKMgx3DEntityRepresentation::createTrianglesSurfacicRepresentation (getEntity ( ), _refinedActor, _refinedMapper, _refinedPolyData, points, triangles);
			break;
		default	:
			VTKMgx3DEntityRepresentation::createSegmentsWireRepresentation (getEntity ( ), _refinedActor, _refinedMapper, _refinedPolyData, points, triangles);
	}	// switch (getEntity ( )->getDim ( ))

	if (true == shouldRefine)
	{
		_refineFilter	= vtkPolyDataRefinementFilter::New ( );
		_refinedMapper->SetInputData (_refineFilter->GetOutput ( ));
		_refineFilter->SetInputData (_refinedPolyData);
		_refineFilter->SetRefinementFactor (factor);
		_refineFilter->Update ( );
		_refinedMapper->SetInputData (_refineFilter->GetOutput ( ));
		_refinedActor->GetProperty ( )->SetColor (0., 1., 0.);
	}	// if (true == shouldRefine)
}	// VTKMgx3DEntityRepresentation::createRefinedRepresentation



// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtVtkComponents

}	// namespace Mgx3D
