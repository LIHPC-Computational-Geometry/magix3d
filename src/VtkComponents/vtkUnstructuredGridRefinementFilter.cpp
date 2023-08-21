//
// Rem : algos faits un peu vite. Probablement à optimiser, et idéalement
// utiliser (optionnellement) des splines ou équivalent.
//
#include "VtkComponents/vtkUnstructuredGridRefinementFilter.h"

#include <vtkCell.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkUnstructuredGrid.h>
#include <assert.h>
#include <string.h>


vtkUnstructuredGridRefinementFilter::vtkUnstructuredGridRefinementFilter ( )
#ifndef VTK_5
	: vtkUnstructuredGridAlgorithm ( ),
#else	// VTK_5
	: vtkUnstructuredGridToUnstructuredGridFilter ( ), 
#endif	// VTK_5
	  _refinementFactor (10), _triangleBuffer (0)
{
}	// vtkUnstructuredGridRefinementFilter::vtkUnstructuredGridRefinementFilter


vtkUnstructuredGridRefinementFilter::vtkUnstructuredGridRefinementFilter (
		const vtkUnstructuredGridRefinementFilter&)
#ifndef VTK_5
	: vtkUnstructuredGridAlgorithm ( ),
#else	// VTK_5
	: vtkUnstructuredGridToUnstructuredGridFilter ( ),
#endif	// VTK_5
	  _refinementFactor (10), _triangleBuffer (0)
{
	assert (0 && "vtkUnstructuredGridRefinementFilter copy constructor is not allowed.");
}	// vtkUnstructuredGridRefinementFilter::vtkUnstructuredGridRefinementFilter


vtkUnstructuredGridRefinementFilter&
				vtkUnstructuredGridRefinementFilter::operator = (
									const vtkUnstructuredGridRefinementFilter&)
{
	assert (0 && "vtkUnstructuredGridRefinementFilter operator = is not allowed.");
	return *this;
}	// vtkUnstructuredGridRefinementFilter::operator =


vtkUnstructuredGridRefinementFilter::~vtkUnstructuredGridRefinementFilter ( )
{
	DesallocateTriangleBuffer ( );
}	// vtkUnstructuredGridRefinementFilter::~vtkUnstructuredGridRefinementFilter


vtkUnstructuredGridRefinementFilter* vtkUnstructuredGridRefinementFilter::New ( )
{
	vtkUnstructuredGridRefinementFilter*	filter	= new vtkUnstructuredGridRefinementFilter ( );
#ifndef VTK_5
	filter->InitializeObjectBase ( );
#endif  // VTK_5

	return filter;
}	// vtkUnstructuredGridRefinementFilter::New


void vtkUnstructuredGridRefinementFilter::SetRefinementFactor (
													unsigned int factor)
{
	if (factor == _refinementFactor)
		return;

	DesallocateTriangleBuffer ( );	// Not usefull because only used by Execute
	_refinementFactor	= factor;
	Modified ( );
}	// vtkUnstructuredGridRefinementFilter::SetId


void vtkUnstructuredGridRefinementFilter::PrintSelf (ostream& os, vtkIndent indent)
{
	Superclass::PrintSelf(os,indent);

	os << indent << "Refinement factor  : " << GetRefinementFactor ( ) << "\n";
}	// vtkUnstructuredGridRefinementFilter::PrintSelf


#ifndef VTK_5
#define RD_SUCCESS	1
#define RD_FAILURE	0
int vtkUnstructuredGridRefinementFilter::RequestData (
		vtkInformation* request, vtkInformationVector**	inputVector,
		vtkInformationVector* outputVector)
#else	// VTK_5
#define RD_SUCCESS
#define RD_FAILURE
void vtkUnstructuredGridRefinementFilter::Execute ( )
#endif	// VTK_5
{
	const unsigned int	factor	= GetRefinementFactor ( );
	if (0 == factor)
	{
		cerr << __FILE__ << ' ' << __LINE__ << " vtkUnstructuredGridRefinementFilter::RequestData has null refinement factor." << endl;
		return RD_FAILURE;
	}	// if (0 == factor)

	// get the input and output
#ifndef VTK_5
	vtkUnstructuredGrid*	input	= GetUnstructuredGridInput (0);
#else	// VTK_5
	vtkUnstructuredGrid*	input	= GetInput ( );
#endif	// VTK_5
	vtkUnstructuredGrid*	output	= GetOutput ( );
	if ((0 == input) || (0 == output))
	{
		cerr << __FILE__ << ' ' << __LINE__ << " vtkUnstructuredGridRefinementFilter::RequestData has null input." << endl;
		return RD_FAILURE;
	}

	// On part du principe qu'on raffine des mailles de dimension 2
	// factor^2 fois plus de mailles et points.
	const vtkIdType	inPtsNum	= input->GetNumberOfPoints ( );
	const vtkIdType	inCellsNum	= input->GetNumberOfCells ( );
	vtkIdType	numPts		= inPtsNum * factor * factor;
	vtkIdType	numCells	= inCellsNum * factor * factor;
	vtkPoints*	points		= vtkPoints::New ( );
	points->SetDataTypeToDouble ( );
	points->Allocate (numPts, numPts);
	output->SetPoints (points);
	output->Allocate (numCells, numCells);

	// Les points existants sont conservés :
	points->DeepCopy (input->GetPoints ( ));

	// Allocation (éventuellement) requises :
	AllocateTriangleBuffer ( );

	// On parcourt les mailles et on raffine :
	for (vtkIdType i = 0; i < inCellsNum; i++)
	{
		vtkCell*	cell	= input->GetCell (i);
		assert (0 != cell);
		switch (cell->GetCellType ( ))
		{
			case VTK_LINE			:
			case VTK_POLY_LINE		:
				refineLine (*cell, *points, *output);
				break;
			case VTK_TRIANGLE		:
			case VTK_TRIANGLE_STRIP	:
				refineTriangle (*cell, *points, *output);
				break;
		}	// switch (cell->GetCellType ( ))
	}	// for (vtkIdType i = 0; i < inCellsNum; i++)

	// Restitution de la mémoire inutilisée :
	points->Squeeze ( );
	output->Squeeze ( );
	DesallocateTriangleBuffer ( );

	return RD_SUCCESS;
}	// vtkUnstructuredGridRefinementFilter::Execute


void vtkUnstructuredGridRefinementFilter::refineLine (
					vtkCell& cell, vtkPoints& points, vtkUnstructuredGrid& grid)
{
	const VTKCellType	type	= (VTKCellType)cell.GetCellType ( );
	switch (type)
	{
		case VTK_LINE		:
		case VTK_POLY_LINE	:
			break;
		default				:
			cerr << __FILE__ << ' ' << __LINE__ << " vtkUnstructuredGridRefinementFilter::refineLine. Invalid cell type (" << (int)type << ")." << endl;
			return;
	}	// switch (type)

	const unsigned int	factor	= GetRefinementFactor ( );
	vtkIdList*	cellPts	= cell.GetPointIds ( );
	assert (0 != cellPts);
	const vtkIdType	ptsNum	= cellPts->GetNumberOfIds ( );
	// Les points utilisés : ceux existants + subdivisions :
	vtkIdType*	pointset	= new vtkIdType [factor + 1];
	memset (pointset, 0, (factor + 1) * sizeof (vtkIdType));
	// On découpe chaque segment :
	for (vtkIdType i = 1; i < ptsNum; i++)
	{
		vtkIdType	id0	= cellPts->GetId (i - 1), id1	= cellPts->GetId (i);
		double	pt0 [3]	= { 0., 0., 0. }, pt1 [3] = { 0., 0., 0. };
		points.GetPoint (id0, pt0);
		points.GetPoint (id1, pt1);
		pointset [0]		= id0;
		pointset [factor]	= id1;
		double d [3] = { (pt1 [0] - pt0 [0]) / factor,
				(pt1 [1] - pt0 [1]) / factor, (pt1 [2] - pt0 [2]) / factor };

		// Création des nouveaux points :
		for (unsigned int r = 1; r < factor; r++)
		{
			double	coords [3]	= { 0., 0., 0. };
			for (int c = 0; c < 3; c++)
				coords [c]	= pt0 [c] + r * d [c];
//			pointset [r]	= points.InsertNextPoint (coords);
			const vtkIdType	pt	= points.InsertNextPoint (coords);
			pointset [r]	= pt;
		}	// for (unsigned int r = 1; r < factor - 1; r++)

		// Insertion des segments créés :
		for (unsigned int s = 0; s < factor; s++)
			grid.InsertNextCell (VTK_LINE, 2, (pointset + s));
	}	// for (vtkIdType i = 1; i < ptsNum; i++)

	delete [] pointset;	pointset	= 0;
}	// vtkUnstructuredGridRefinementFilter::refineLine


void vtkUnstructuredGridRefinementFilter::refineTriangle (
					vtkCell& cell, vtkPoints& points, vtkUnstructuredGrid& grid)
{
	assert (0 != _triangleBuffer);
	const size_t	factor	= GetRefinementFactor ( );
	const size_t	rows	= factor + 1;
	const size_t	cols	= factor + 1;
	vtkIdList*	cellPts	= cell.GetPointIds ( );
	assert (0 != cellPts);
	const vtkIdType		ptsNum	= cellPts->GetNumberOfIds ( );
	if (3 != ptsNum)
	{
		cerr << __FILE__ << ' ' << __LINE__ << " vtkUnstructuredGridRefinementFilter::refineTriangle. Invalid cell type (" << (int)cell.GetCellType ( ) << ")." << endl;
	}	// if (3 != ptsNum)
	const vtkIdType	idA = cellPts->GetId (0), idB = cellPts->GetId (1), idC = cellPts->GetId (2);
	double	ptA [3]	= { 0., 0., 0. }, ptB [3] = { 0., 0., 0. }, ptC [3] = { 0., 0., 0. };
	points.GetPoint (idA, ptA);
	points.GetPoint (idB, ptB);
	points.GetPoint (idC, ptC);

	// Les points utilisés : les 3 existants + subdivisions. On transforme le
	// triangle en quad dégénéré de factor lignes x factor colonnes et on
	// subdivise en quads.
	// Chacun de ces quads formera 2 triangles :
	// A        A ________C
	// |\        |_|_|_|_|C
	// | \       |_|_|_|_|C
	// |  \C     |_|_|_|_|C
	// |  /   => |_|_|_|_|C
	// | /       |_|_|_|_|C
	// |/        |_|_|_|_|C
	// B        B|_|_|_|_|C

	// Les angles :
	for (vtkIdType r = 0; r < rows; r++)
		for (vtkIdType c = 0; c < cols; c++)
			_triangleBuffer [r][c]	= (vtkIdType)-1;
	_triangleBuffer [0][0]			= idA;
	_triangleBuffer [rows - 1][0]	= idB;
	for (vtkIdType r = 0; r < rows; r++)
		_triangleBuffer [r][cols - 1]	= idC;

	// Progression sur AC et BC :
	double dAC [3] = { (ptC [0] - ptA [0]) / factor,
				(ptC [1] - ptA [1]) / factor, (ptC [2] - ptA [2]) / factor };
	double dBC [3] = { (ptC [0] - ptB [0]) / factor,
				(ptC [1] - ptB [1]) / factor, (ptC [2] - ptB [2]) / factor };

	// Création des nouveaux points :
	for (size_t c = 0; c < cols; c++)
	{
		double	ptAC [3]	= { 0., 0., 0. };	// Point sur segment AC
		double	ptBC [3]	= { 0., 0., 0. };	// Point sur segment BC
		for (int i = 0; i < 3; i++)
		{
			ptAC [i]	= ptA [i] + c * dAC [i];
			ptBC [i]	= ptB [i] + c * dBC [i];
		}	// for (int i = 0; i < 3; i++)

		// A partir de ces 2 points on a un segment qu'on raffine également :
		for (size_t r = 0; r < rows; r++)
		{
			double d [3] = {(ptBC [0] - ptAC [0]) / factor,
				(ptBC [1] - ptAC [1]) / factor, (ptBC [2] - ptAC [2]) / factor};
			if ((vtkIdType)-1 == _triangleBuffer [r][c])
			{
				double	coords [3]	= { 0., 0., 0. };
				for (int i = 0; i < 3; i++)
					coords [i]	= ptAC [i] + r * d [i];
				const vtkIdType	pt	= points.InsertNextPoint (coords);
				_triangleBuffer [r][c]	= pt;
			}	// if ((vtkIdType)-1 == _triangleBuffer [r][c])
		}	// for (size_t r = 0; r < rows; r++)
	}	// for (size_t c = 0; c < cols; c++)
	// On ajoute les triangles créés :
	for (size_t r = 0; r < rows - 1; r++)
	{
		for (size_t c = 0; c < cols - 1; c++)
		{
			const vtkIdType	id1	= _triangleBuffer [r][c];
			const vtkIdType	id2	= _triangleBuffer [r][c + 1];
			const vtkIdType	id3	= _triangleBuffer [r + 1][c];
			const vtkIdType	id4	= _triangleBuffer [r + 1][c + 1];
			vtkIdType	triangle1Ids [3]	= { id1, id2, id3 };
			vtkIdType	triangle2Ids [3]	= { id2, id4, id3 };
			grid.InsertNextCell (VTK_TRIANGLE, 3, triangle1Ids);
			if (cols -1 != c)	// Dernière colonne => dégénérescence
				grid.InsertNextCell (VTK_TRIANGLE, 3, triangle2Ids);
		}	// for (size_t c = 0; c < cols; c++)
	}	// for (size_t r = 0; r < rows; r++)
}	// vtkUnstructuredGridRefinementFilter::refineTriangle


void vtkUnstructuredGridRefinementFilter::AllocateTriangleBuffer ( )
{
	const vtkIdType	size	= GetRefinementFactor ( ) + 1;
	_triangleBuffer	= new vtkIdType* [size];
	for (vtkIdType i = 0; i < size; i++)
	{
		_triangleBuffer	[i]	= new vtkIdType [size];	
		memset (_triangleBuffer [i], 0, size * sizeof (vtkIdType));
	}
}	// vtkUnstructuredGridRefinementFilter::AllocateTriangleBuffer


void vtkUnstructuredGridRefinementFilter::DesallocateTriangleBuffer ( )
{
	if (0 != _triangleBuffer)
	{
		const vtkIdType	size	= GetRefinementFactor ( ) + 1;
		for (vtkIdType i = 0; i < size; i++)
			delete [] _triangleBuffer [i];
		delete [] _triangleBuffer;
		_triangleBuffer	= 0;
	}	// if (0 != _triangleBuffer)
}	// vtkUnstructuredGridRefinementFilter::DesallocateTriangleBuffer

