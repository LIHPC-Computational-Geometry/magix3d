#ifndef VTK_DMA_SYNCHRONIZED_RENDERERS_H
#define VTK_DMA_SYNCHRONIZED_RENDERERS_H

#include <VtkComponents/vtkDMAMultiProcessStream.h>
#include <vtkSynchronizedRenderers.h>
#include <vtkCompositeRenderManager.h>


/*
 * Classe créé juste pour profiter de la classe
 * vtkSynchronizedRenderers::RendererInfo qui est protégée et exploiter
 * la classe vtkDMAMultiProcessStream dont l'objectif est d'appeler aussi
 * rarement que possible <I>assert</I> ...
 */
class vtkDMASynchronizedRenderers : public vtkSynchronizedRenderers
{
	public :

	static void copyRenderingParameters (vtkRenderer& fromRenderer, vtkCompositeRenderManager& toRenderMgr);
	static void copyRenderingParameters (vtkRenderer& fromRenderer, vtkDMAMultiProcessStream& serializedMessage);
	static void copyRenderingParameters (vtkDMAMultiProcessStream& serializedMessage, vtkRenderer& toRenderer);


	private :

	vtkDMASynchronizedRenderers ( );
	vtkDMASynchronizedRenderers (const vtkDMASynchronizedRenderers&);
	vtkDMASynchronizedRenderers& operator = (const vtkDMASynchronizedRenderers&);
	~vtkDMASynchronizedRenderers ( );
};	// class vtkDMASynchronizedRenderers

#endif	// VTK_DMA_SYNCHRONIZED_RENDERERS_H
