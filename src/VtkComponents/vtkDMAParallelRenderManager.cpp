#include <VtkComponents/vtkDMAParallelRenderManager.h>
#include <vtkMultiProcessController.h>
#include <vtkRendererCollection.h>
#include <vtkMath.h>


vtkDMAParallelRenderManager::vtkDMAParallelRenderManager ( )
	: vtkCompositeRenderManager ( )
{
}	// vtkDMAParallelRenderManager::vtkDMAParallelRenderManager


vtkDMAParallelRenderManager::vtkDMAParallelRenderManager (const vtkDMAParallelRenderManager&)
	: vtkCompositeRenderManager ( )
{
}	// vtkDMAParallelRenderManager::vtkDMAParallelRenderManager


vtkDMAParallelRenderManager& vtkDMAParallelRenderManager::operator = (const vtkDMAParallelRenderManager&)
{
	return *this;
}	// vtkDMAParallelRenderManager::operator =


vtkDMAParallelRenderManager::~vtkDMAParallelRenderManager ( )
{
}	// vtkDMAParallelRenderManager::~vtkDMAParallelRenderManager


vtkDMAParallelRenderManager* vtkDMAParallelRenderManager::New ( )
{
	return new vtkDMAParallelRenderManager ( );
}	// vtkDMAParallelRenderManager::New


void vtkDMAParallelRenderManager::ComputeVisiblePropBounds (vtkRenderer *ren, double bounds[6])
{	// CP Code from VTK 5.10.0 or 7.1.1 (same sources)
	  if (!this->ParallelRendering)
	{
	ren->ComputeVisiblePropBounds(bounds);
	return;
	}	

  if (this->Controller)
	{	
	if (this->Controller->GetLocalProcessId() != this->RootProcessId)
	  {	
	  vtkErrorMacro("ComputeVisiblePropBounds/ResetCamera can only be called on root process");
	  return;
	  }	

	vtkRendererCollection *rens = this->GetRenderers();
	vtkCollectionSimpleIterator rsit;
	rens->InitTraversal(rsit);
	int renderId = 0; 
	while (1)
	  {	
	  vtkRenderer *myren = rens->GetNextRenderer(rsit);
	  if (myren == NULL)
		{
		vtkWarningMacro("ComputeVisiblePropBounds called with unregistered renderer " << ren << "\nDefaulting to first renderer.");
		renderId = 0; 
		break;
		}
	  if (myren == ren) 
		{
		//Found correct renderer.
		break;
		}
	  renderId++;
	  }	

	//Invoke RMI's on servers to perform their own ComputeVisiblePropBounds.
	int numProcs = this->Controller->GetNumberOfProcesses();
	int id;
	this->Controller->TriggerRMIOnAllChildren(&renderId, sizeof(int),
	  vtkParallelRenderManager::COMPUTE_VISIBLE_PROP_BOUNDS_RMI_TAG);
	
	//Now that all the RMI's have been invoked, we can safely query our
	//local bounds even if an Update requires a parallel operation.

	this->LocalComputeVisiblePropBounds(ren, bounds);
	
		//Collect all the bounds.
	for (id = 0; id < numProcs; id++)
	  {
	  double tmp[6];

	  if (id == this->RootProcessId)
		{
		continue;
		}

	  this->Controller->Receive(tmp, 6, id, vtkParallelRenderManager::BOUNDS_TAG);
	  if (!vtkMath::AreBoundsInitialized (tmp))
		continue;	// CP : Patch against children uninitialized bounds
		
		if (!vtkMath::AreBoundsInitialized (bounds))	// CP : Patch against local uninitialized bounds
			memcpy (bounds, tmp, 6 * sizeof (double));

	  if (tmp[0] < bounds[0])
		{
		bounds[0] = tmp[0];
		}
	  if (tmp[1] > bounds[1])
		{
		bounds[1] = tmp[1];
		}
	  if (tmp[2] < bounds[2])
		{
		bounds[2] = tmp[2];
		}
	  if (tmp[3] > bounds[3])
		{
		bounds[3] = tmp[3];
		}
	  if (tmp[4] < bounds[4])
		{
		bounds[4] = tmp[4];
		}
	  if (tmp[5] > bounds[5])
		{
		bounds[5] = tmp[5];
		}
	  }
	}
  else
	{
	vtkWarningMacro("ComputeVisiblePropBounds/ResetCamera called before Controller set");

	ren->ComputeVisiblePropBounds(bounds);
	}
}	// vtkDMAParallelRenderManager::ComputeVisiblePropBounds





