#ifndef VTK_DMA_PARALLEL_RENDER_MANAGER_H
#define VTK_DMA_PARALLEL_RENDER_MANAGER_H

#include <vtkCompositeRenderManager.h>


/*
 * Classe créés pour surcharger vtkParallelRenderManager::ComputeVisiblePropBounds qui ne prend pas en compte
 * les bounds éventuellement non initialisés (cas de renderers n'affichant ... rien).
 */
class vtkDMAParallelRenderManager : public vtkCompositeRenderManager
{
	public :

	static vtkDMAParallelRenderManager* New ( );
	virtual void ComputeVisiblePropBounds (vtkRenderer *ren, double bounds[6]);


	private :

	vtkDMAParallelRenderManager ( );
	vtkDMAParallelRenderManager (const vtkDMAParallelRenderManager&);
	vtkDMAParallelRenderManager& operator = (const vtkDMAParallelRenderManager&);
	~vtkDMAParallelRenderManager ( );
};	// class vtkDMAParallelRenderManager

#endif	// VTK_DMA_PARALLEL_RENDER_MANAGER_H
