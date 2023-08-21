#include <VtkComponents/vtkDMASynchronizedRenderers.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>


vtkDMASynchronizedRenderers::vtkDMASynchronizedRenderers ( )
	: vtkSynchronizedRenderers ( )
{
}	// vtkDMASynchronizedRenderers::vtkDMASynchronizedRenderers


vtkDMASynchronizedRenderers::vtkDMASynchronizedRenderers (const vtkDMASynchronizedRenderers&)
	: vtkSynchronizedRenderers ( )
{
}	// vtkDMASynchronizedRenderers::vtkDMASynchronizedRenderers


vtkDMASynchronizedRenderers& vtkDMASynchronizedRenderers::operator = (const vtkDMASynchronizedRenderers&)
{
	return *this;
}	// vtkDMASynchronizedRenderers::operator =


vtkDMASynchronizedRenderers::~vtkDMASynchronizedRenderers ( )
{
}	// vtkDMASynchronizedRenderers::~vtkDMASynchronizedRenderers


void vtkDMASynchronizedRenderers::copyRenderingParameters (vtkRenderer& fromRenderer, vtkCompositeRenderManager& toRenderMgr)
{
	// Recopie de la fenêtre de rendu MPI dans la fenêtre de rendu socket
	// côté serveur.
	vtkRenderer*	toRenderer	=
		toRenderMgr.GetRenderWindow( )->GetRenderers( )->GetFirstRenderer();

	vtkSynchronizedRenderers::RendererInfo	renInfo;
	renInfo.CopyFrom (&fromRenderer);
	renInfo.CopyTo (toRenderer);
	toRenderMgr.GetRenderWindow( )->Render ( );
}	// vtkDMASynchronizedRenderers::copyRenderingParameters


void vtkDMASynchronizedRenderers::copyRenderingParameters (vtkRenderer& fromRenderer, vtkDMAMultiProcessStream& serializedMessage)
{
	vtkSynchronizedRenderers::RendererInfo	renInfo;
	renInfo.ImageReductionFactor	= 1;	// unused
	renInfo.CopyFrom (&fromRenderer);
	renInfo.Save ((vtkMultiProcessStream&)serializedMessage);
}	// vtkDMASynchronizedRenderers::copyRenderingParameters


void vtkDMASynchronizedRenderers::copyRenderingParameters (vtkDMAMultiProcessStream& serializedMessage, vtkRenderer& toRenderer)
{
	vtkSynchronizedRenderers::RendererInfo	renInfo;
	renInfo.Restore ((vtkMultiProcessStream&)serializedMessage);
	renInfo.CopyTo (&toRenderer);
}	// vtkDMASynchronizedRenderers::copyRenderingParameters



