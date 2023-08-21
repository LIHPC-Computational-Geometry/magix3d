/**
 * \file		VTKMgx3DSelectionManager.h
 * \author		Charles PIGNEROL
 * \date		18/10/2012
 */

#ifndef VTK_MGX3D_SELECTION_MANAGER_H
#define VTK_MGX3D_SELECTION_MANAGER_H


#include "Utils/SelectionManager.h"
#include "QtVtkComponents/VTKECMSelectionManager.h"
#include "QtVtkComponents/VTKRenderingManager.h"


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace QtVtkComponents {

class VTKRenderingManager;


/**
 * <P>Classe de base de gestionnaire de sélection Magix 3D pour une IHM reposant
 * sur <I>VTK</I>.
 * </P>
 *
 * <P>Cette class hérite de ce fait :
 * <OL>
 * <LI>De la classe Mgx3D::Utils::SelectionManager, afin de gérer la sélection
 * au niveau du noyau <I>Magix 3D</I>,
 * <LI>VTKECMSelectionManager afin d'offrir le même comportement graphique, au
 * niveau gestion de la sélection, que les autres logiciels graphiques utilisant
 * cette <I>API</I>.
 * </OL>
 * </P>
 *
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 * @date		23/04/2013
 */
class VTKMgx3DSelectionManager :
						public virtual Mgx3D::Utils::SelectionManager,
						public VTKECMSelectionManager
{
	public :

	/**
	 * Constructeur.
	 * \param		Nom unique du gestionnaire de sélection
	 * \param		los le LogOutputStream
	 */
	VTKMgx3DSelectionManager (
					const std::string& name, TkUtil::LogOutputStream* los);

	/**
	 * Invoque selectionManagerDeleted de chacun de ses observateurs.
	 */
	virtual ~VTKMgx3DSelectionManager ( );

	/**
	 * Le gestionnaire de rendu associé.
	 */
	//@{

	/**
	 * \return		Une référence sur le gestionnaire de rendu associé.
	 * \exception	Une exception est levée en l'absence de gestionnaire de
	 *				rendu associé.
	 * \see			setRenderingManager
	 */
	virtual Mgx3D::QtVtkComponents::VTKRenderingManager& getRenderingManager( );
	virtual const Mgx3D::QtVtkComponents::VTKRenderingManager&
												getRenderingManager ( ) const;

	/**
	 * \param		Nouveau gestionnaire de rendu asosocié.
	 * \see			getRenderingManager
	 */
	virtual void setRenderingManager (
				Mgx3D::QtVtkComponents::VTKRenderingManager* renderingManager);

	//@}	// Le gestionnaire de rendu associé.

	/**
     * Les actions directes sur la sélection
     */
    //@{

	/**
	 * Ajoute l'entité <I>Magix D</I> transmise en argument à la sélection.
	 * Invoque à ce titre <I>SelectionManager::addToSelection ( )</I>.
	 * \except	Lève une exception en cas d'action impossible à réaliser.
	 * \see		RemoveFromSelection
	 * \see		SelectionManager::addToSelection
	 */
	virtual void AddToSelection (vtkActor& actor);

	/**
	 * Enlève l'entité <I>Magix D</I> transmise en argument de la sélection.
	 * Invoque à ce titre <I>SelectionManager::removeFromSelection ( )</I>.
	 * \except	Lève une exception en cas d'action impossible à réaliser.
	 * \see		AddToSelection
	 * \see		SelectionManager::removeFromSelection
	 */
	virtual void RemoveFromSelection (vtkActor& actor);

	/**
	 * @return		true si l'entité associée à l'acteur reçu en 
	 *			argument fait parti de la sélection, ou false.
	 */
	virtual bool IsSelected (const vtkActor& actor) const;

	/**
	 * Annule la sélection en cours.
	 * Invoque à ce titre <I>SelectionManager::clearSelection ( )</I>.
	 * \see		SelectionManager::clearSelection
	 */
	virtual void ClearSelection ( );

	//@}	// Les actions directes sur la sélection

	/**
	 * La gestion des messages.
	 */
	//@{

	/**
	 * \return		 Le nom de l'objet sélectionné associé à l'acteur.
	 */
	virtual std::string GetName (const vtkActor& actor) const;

	//@}	// La gestion des messages.



	protected :


	private :

	/**
	 * Constructeur de copie : interdit.
	 */
	VTKMgx3DSelectionManager (const VTKMgx3DSelectionManager&);

	/**
	 * Opérateur = : interdit.
	 */
	VTKMgx3DSelectionManager& operator = (
									const VTKMgx3DSelectionManager&);

	/** Le gestionnaire de rendu associé. */
	Mgx3D::QtVtkComponents::VTKRenderingManager*	_renderingManager;
};	// class VTKMgx3DSelectionManager

/*----------------------------------------------------------------------------*/
}	// end namespace QtVtkComponents

/*----------------------------------------------------------------------------*/
}	// end namespace Mgx3D

/*----------------------------------------------------------------------------*/

#endif	// VTK_MGX3D_SELECTION_MANAGER_H
