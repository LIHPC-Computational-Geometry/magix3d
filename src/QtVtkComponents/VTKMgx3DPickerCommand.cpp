#include "Internal/ContextIfc.h"

#include "QtVtkComponents/VTKMgx3DPickerCommand.h"
#include "QtVtkComponents/vtkCustomizableInteractorStyleTrackball.h"
#include "QtVtkComponents/VTKMgx3DActor.h"

#include <TkUtil/Exception.h>
#include <TkUtil/ErrorLog.h>
#include <TkUtil/InformationLog.h>
#include <TkUtil/MemoryError.h>

#include <assert.h>
#include <map>
#include <stdexcept>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;

namespace Mgx3D
{

namespace QtVtkComponents
{


VTKMgx3DPickerCommand::VTKMgx3DPickerCommand (
				vtkRenderer* renderer, vtkRenderWindowInteractor* interactor)
	: VTKECMPickerCommand (renderer, interactor)
{
}	// VTKMgx3DPickerCommand::VTKMgx3DPickerCommand


VTKMgx3DPickerCommand::VTKMgx3DPickerCommand (const VTKMgx3DPickerCommand&)
	: VTKECMPickerCommand (0, 0)
{
	assert (0 && "VTKMgx3DPickerCommand copy constructor is not allowed.");
}	// VTKMgx3DPickerCommand copy constructor


VTKMgx3DPickerCommand& VTKMgx3DPickerCommand::operator = (
												const VTKMgx3DPickerCommand&)
{
	assert (0 && "VTKMgx3DPickerCommand::operator = is not allowed.");
	return *this;
}	// VTKMgx3DPickerCommand::operator =


VTKMgx3DPickerCommand::~VTKMgx3DPickerCommand ( )
{
}	// VTKMgx3DPickerCommand::~VTKMgx3DPickerCommand


string VTKMgx3DPickerCommand::GetName (const vtkActor& actor) const
{
	const VTKMgx3DActor*	mgxActor	=
								dynamic_cast<const VTKMgx3DActor*>(&actor);
	const Entity*			entity		=
								0 == mgxActor ? 0 : mgxActor->GetEntity ( );

	return 0 == entity ? "" : entity->getUniqueName ( );
}	// VTKMgx3DPickerCommand::GetName


}	// namespace Mgx3D

}	// namespace QtVtkComponents

