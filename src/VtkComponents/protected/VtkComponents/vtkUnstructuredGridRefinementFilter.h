#ifndef VTK_UNSTRUCTURED_GRID_REFINEMENT_FILTER_H
#define VTK_UNSTRUCTURED_GRID_REFINEMENT_FILTER_H

#ifndef VTK_5
#include <vtkUnstructuredGridAlgorithm.h>
#else	// VTK_5
#include <vtkUnstructuredGridToUnstructuredGridFilter.h>
#endif	// VTK_5
#include <vtkIndent.h>
#include <vtkCell.h>
#include <iostream>


/** Filtre permettant de raffiner les mailles d'une grille non structurée.
 *
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class vtkUnstructuredGridRefinementFilter : 
#ifndef VTK_5
					public vtkUnstructuredGridAlgorithm
#else	// VTK_5
					public vtkUnstructuredGridToUnstructuredGridFilter
#endif	// VTK_5
{
	public :

	/** Destructeur : RAS
	 */
	virtual ~vtkUnstructuredGridRefinementFilter ( );

	/** Instanciation de cette classe.
	 */
	static vtkUnstructuredGridRefinementFilter* New ( );

#ifndef VTK_5
	vtkTypeMacro(vtkUnstructuredGridRefinementFilter,vtkUnstructuredGridAlgorithm);
#else	// VTK_5
	vtkTypeMacro(vtkUnstructuredGridRefinementFilter,vtkUnstructuredGridToUnstructuredGridFilter);
#endif	// VTK_5


	/**
	 * @param	Facteur de raffinement. Il est appliqué selon chaque dimension.
	 * Exemple : si 2 les arêtes des mailles seront coupées en 2.
	 */
	virtual void SetRefinementFactor (unsigned int factor);

	/**
	 * @return	Le facteur de raffinement appliqué selon chaque dimension.
	 */
	virtual unsigned int GetRefinementFactor ( ) const
	{ return _refinementFactor; }

	/**
	 * Affiche quelques infos sur l'instance.
	 * @param		flux d'impression
	 * @param		indentation utilisée
	 */
	virtual void PrintSelf (ostream& os, vtkIndent indent);


	protected :

	/**
	 * Constructeur. Id par défaut : 0. Méthode par défaut : noeud
	 * commun.
	 */
	vtkUnstructuredGridRefinementFilter ( );

	/**
	 * Effectue le traitement de raffinage.
	 */
#ifndef VTK_5
	virtual int RequestData (
		vtkInformation* request, vtkInformationVector**	inputVector,
		vtkInformationVector* outputVector);
#else	// VTK_5
	virtual void Execute ( );
#endif	// VTK_5

	/**
	 * Les algorithmes de raffinage.
	 * @see		AllocateTriangleBuffer
	 */
	virtual void refineLine (
				vtkCell& cell, vtkPoints& points, vtkUnstructuredGrid& grid);
	virtual void refineTriangle (
				vtkCell& cell, vtkPoints& points, vtkUnstructuredGrid& grid);

	/** (Des)alloue un tampon de taille
	 * (_refinementFactor+1) x (_refinementFactor+1) pour le raffinement des
	 * mailles de type triangle.
	 */
	virtual void AllocateTriangleBuffer ( );
	virtual void DesallocateTriangleBuffer ( );


	private :

	/** Constructeur de copie : interdit. */
	vtkUnstructuredGridRefinementFilter (
							const vtkUnstructuredGridRefinementFilter&);

	/** Opérateur = : interdit. */
	vtkUnstructuredGridRefinementFilter& operator = (
							const vtkUnstructuredGridRefinementFilter&);

	/** Le facteur de raffinement. */
	unsigned int			_refinementFactor;

	/** Tampon pour stocker les ids des noeuds utilisés lors du raffinement
	 * des triangles. Ce buffer est de dimension
	 * (_refinementFactor+1) x (_refinementFactor+1).
	 * @see		AllocateTriangleBuffer
	 * @see		DesallocateTriangleBuffer
	 * */
	vtkIdType**				_triangleBuffer;
};	// class vtkUnstructuredGridRefinementFilter


#endif	// VTK_UNSTRUCTURED_GRID_REFINEMENT_FILTER_H
