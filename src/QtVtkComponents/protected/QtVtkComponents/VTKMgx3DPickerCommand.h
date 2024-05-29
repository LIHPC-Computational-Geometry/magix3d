#ifndef VTK_MGX3D_PICKER_COMMAND_H
#define VTK_MGX3D_PICKER_COMMAND_H

#include "VTKMgx3DPicker.h"
#include "QtVtkComponents/VTKECMPickerCommand.h"
#include <Utils/SelectionManagerIfc.h>
#include <TkUtil/LogDispatcher.h>

#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <vector>


namespace Mgx3D
{

namespace QtVtkComponents
{



/** Commande reliant un picker, utilisé pour effectuer des sélections à
 * la souris, au théatre, et spécialisée pour l'application <I>Magix 3D</I>.
 * Attention : cette classe est faite pour fonctionner avec une instance de la
 * classe VTKMgx3DPicker.
 * \author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class VTKMgx3DPickerCommand : public VTKECMPickerCommand
{
	public :

	VTKMgx3DPickerCommand (vtkRenderer* renderer, vtkRenderWindowInteractor* interactor);

	virtual ~VTKMgx3DPickerCommand ( );

	/**
	 * \return		Le nom de l'entité <I>Magix 3D</I> représentée par l'acteur.
	 */
	 virtual std::string GetName (const vtkActor& actor) const;


	protected :


	private :

	VTKMgx3DPickerCommand (const VTKMgx3DPickerCommand&);
	VTKMgx3DPickerCommand& operator = (const VTKMgx3DPickerCommand&);
};	// class VTKMgx3DPickerCommand


}	// namespace Mgx3D

}	// namespace QtVtkComponents


#endif	// VTK_MGX3D_PICKER_COMMAND_H
