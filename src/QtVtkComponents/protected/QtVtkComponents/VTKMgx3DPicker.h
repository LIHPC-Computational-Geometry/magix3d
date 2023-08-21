#ifndef VTK_MGX3D_PICKER_H
#define VTK_MGX3D_PICKER_H


#include <Utils/Entity.h>
#include <QtVtkComponents/VTKECMPicker.h>

#include <vtkAssemblyPath.h>
#include <vtkRenderer.h>

#include <vtkPicker.h>
#include <vtkCellPicker.h>

#include <iostream>
#include <vector>


namespace Mgx3D
{

namespace QtVtkComponents
{

/**
 * Classe de picker <I>VTK</I> spécialisé pour le logiciel <I>Magix 3D</I>.
 *
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class VTKMgx3DPicker : public VTKECMPicker
{
	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~VTKMgx3DPicker ( );

	/**
	 * Instanciateur VTK. RAS.
	 */
	static VTKMgx3DPicker* New ( );

	virtual void PrintSelf (std::ostream& os, vtkIndent indent);

	/**
	 * \return		Les entités pointées et classées de la plus proche à la plus
	 *				éloignée. L'algorithme de calcul de la distance est fonction
	 *				du mode de fonctionnement courrant.
	 * \see			GetMode
	 */
	const std::vector<Mgx3D::Utils::Entity*>& GetPickedEntities ( ) const;


	protected :

	/**
	 * Appelé par <I>Pick</I> en fin de traitement. Actualise la liste des
	 * entités <I>Magix 3D</I> pointées.
	 * Retrie les acteurs pointés pour mettre en tête de liste ceux qui sont
	 * de type <I>VTKMgx3DActor</I>.
	 * \see			GetPickedEntities
	 */
	virtual void CompletePicking ( );
	
	/**
	 * Constructeur par défaut. RAS.
	 */
	VTKMgx3DPicker ( );


	private :

	VTKMgx3DPicker (const VTKMgx3DPicker&);
	VTKMgx3DPicker& operator = (const VTKMgx3DPicker&);

	/** Les entités Magix 3D pointées. */
	std::vector<Mgx3D::Utils::Entity*>	_entities;
};	// class VTKMgx3DPicker

}	// namespace QtVtkComponents

}	// namespace Mgx3D


#endif	// VTK_MGX3D_PICKER_H

