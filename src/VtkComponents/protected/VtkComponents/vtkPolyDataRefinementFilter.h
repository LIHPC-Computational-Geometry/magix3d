#ifndef VTK_POLYDATA_REFINEMENT_FILTER_H
#define VTK_POLYDATA_REFINEMENT_FILTER_H

#ifndef VTK_5
#include <vtkPolyDataAlgorithm.h>
#else	// VTK_5
#include <vtkPolyDataToPolyDataFilter.h>
#endif	// VTK_5
#include <vtkIndent.h>
#include <vtkCell.h>
#include <iostream>


/** Filtre permettant de raffiner les mailles d'un polydata.
 * 
 * @warning		Ne gère que les triangles dans la version actuelle.
 *
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class vtkPolyDataRefinementFilter : public vtkPolyDataAlgorithm
{
	public :

	/** Destructeur : RAS
	 */
	virtual ~vtkPolyDataRefinementFilter ( );

	/** Instanciation de cette classe.
	 */
	static vtkPolyDataRefinementFilter* New ( );

	vtkTypeMacro(vtkPolyDataRefinementFilter,vtkPolyDataAlgorithm);

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
	 * Constructeur. Id par défaut : 0. Méthode par défaut : noeud commun.
	 */
	vtkPolyDataRefinementFilter ( );

	/**
	 * Effectue le traitement de raffinage.
	 */
	virtual int RequestData (vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);

	/**
	 * Les algorithmes de raffinage.
	 * @see		AllocateTriangleBuffer
	 */
	virtual void refineLine (vtkCell& cell, vtkPoints& points, vtkPolyData& grid);
	virtual void refineTriangle (vtkCell& cell, vtkPoints& points, vtkPolyData& grid);

	/** (Des)alloue un tampon de taille (_refinementFactor+1) x (_refinementFactor+1) pour le raffinement des
	 * mailles de type triangle.
	 */
	virtual void AllocateTriangleBuffer ( );
	virtual void DesallocateTriangleBuffer ( );


	private :

	/** Constructeur de copie : interdit. */
	vtkPolyDataRefinementFilter (const vtkPolyDataRefinementFilter&);

	/** Opérateur = : interdit. */
	vtkPolyDataRefinementFilter& operator = (const vtkPolyDataRefinementFilter&);

	/** Le facteur de raffinement. */
	unsigned int			_refinementFactor;

	/** Tampon pour stocker les ids des noeuds utilisés lors du raffinement des triangles. Ce buffer est de dimension
	 * (_refinementFactor+1) x (_refinementFactor+1).
	 * @see		AllocateTriangleBuffer
	 * @see		DesallocateTriangleBuffer
	 * */
	vtkIdType**				_triangleBuffer;
};	// class vtkPolyDataRefinementFilter


#endif	// VTK_POLYDATA_REFINEMENT_FILTER_H
