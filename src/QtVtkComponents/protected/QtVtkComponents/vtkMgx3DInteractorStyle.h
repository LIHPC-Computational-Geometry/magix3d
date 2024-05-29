/**
 * Interacteur DSSI spécialisé pour le logiciel <I>Magix 3D</I>. Il
 * hérite de la classe <I>vtkUnifiedInteractorStyle</I> afin d'offrir le
 * même comportement de base que les autres logiciels graphiques du service.
 * 
 * \see		vtkUnifiedInteractorStyle
 */

#ifndef VTK_MGX3D_INTERACTOR_STYLE_H
#define VTK_MGX3D_INTERACTOR_STYLE_H


#include "QtVtkComponents/vtkUnifiedInteractorStyle.h"
#include "QtVtkComponents/VTKMgx3DPicker.h"
#include "QtVtkComponents/VTKMgx3DPickerCommand.h"
#include "QtComponents/EntitySeizureManager.h"
#include "Utils/Entity.h"
#include "Utils/SelectionManagerIfc.h"


/** <P>En plus de la classe <I>vtkUnifiedInteractorStyle</I> permet :<BR>
 * <OL>
 * <LI>L'annulation optionnelle du roulis lors  du repositionnement dans un plan
 * (touches X, Y, Z),
 * <LI>Du <I>picking</I> au bouton gauche de la souris,
 * </OL>
 * </P>
 *
 * @author		Charles PIGNEROL			CEA/DAM/DSSI
 */
class vtkMgx3DInteractorStyle : public vtkUnifiedInteractorStyle
{
	public :

	/**
	 * Evènements déclenchés par cette classe dans le prolongement des
	 * <I>vtkCommand::EventIds</I>.
	 */
	//@{

	static const unsigned long		ViewRedefinedEvent;

	//@}	// Evènements déclenchés par cette classe ...

	// Fin Evènements déclenchés par cette classe

	/**
	 * Destructeur. RAS.
	 */
	virtual ~vtkMgx3DInteractorStyle ( );

	/**
	 * Instancie cette classe selon le "pattern" de fabrication VTK.
	 */
	static vtkMgx3DInteractorStyle* New ( );

	vtkTypeMacro(vtkMgx3DInteractorStyle,vtkUnifiedInteractorStyle);

	/**
	 * Affiche des informations dans os sur l'instance.
	 */
	virtual void PrintSelf (ostream& os, vtkIndent indent);

	/** <P>Surcharge les actions suivantes :<BR>
	 * <OL>
	 * <LI> <I>h</I> : l'entité sélectionnée est masquée
	 * </OL>
	 * \see     HideSelection
	 */
	virtual void OnChar ( );

	/**
	 * <P>
	 * Si la touche pressée est <I>QtMgx3DApplication::_nextSelectionKey</I>
	 * appelle alors <I>NextSelection</I>.
	 * </P>
	 * <P>
	 * Dans les autres cas invoque
	 * <I>vtkUnifiedInteractorStyle::OnKeyRelease ( )</I>.
	 * </P>
	 * \see		NextSelection
	 */
	virtual void OnKeyRelease ( );

	/** Action effectuée lorsque la touche "Flèche vers la gauche"
	 * est activée. Invoque Pan avec un déplacement de souris de
	 * - GetMotionRatio ( ) * largeur de la fenêtre. 
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (ViewRedefinedEvent, NULL);</I>.
	 */
	virtual void OnLeftArrow ( );

	/** Action effectuée lorsque la touche "Flèche vers le haut"
	 * est activée. Invoque Pan avec un déplacement de souris de
	 * GetMotionRatio ( ) * hauteur de la fenêtre. 
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (ViewRedefinedEvent, NULL);</I>.
	 */
	virtual void OnUpArrow ( );

	/** Action effectuée lorsque la touche "Flèche vers la droite"
	 * est activée. Invoque Pan avec un déplacement de souris de
	 * GetMotionRatio ( ) * largeur de la fenêtre. 
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (ViewRedefinedEvent, NULL);</I>.
	 */
	virtual void OnRightArrow ( );

	/** Action effectuée lorsque la touche "Flèche vers le bas"
	 * est activée. Invoque Pan avec un déplacement de souris de
	 * - GetMotionRatio ( ) * hauteur de la fenêtre. 
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (ViewRedefinedEvent, NULL);</I>.
	 */
	virtual void OnDownArrow ( );

	/**
	 * Si <I>centerOnActor</I> vaut <I>true</I> le centre de rotation devient
	 * l'acteur, sinon il devient le point visé.
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (ViewRedefinedEvent, NULL);</I>.
	  */
	virtual void FlyTo (bool centerOnActor);

	/**
	 * Positionne la vue respectivement dans le plan <I>xOy</I>, <I>xOz</I>,
     * <I>yOz</I>. Supprime également le roulis si
	 * QtVtkMgx3DApplication::_xyzCancelRoll vaut <I>true</I>.
	 * Invoque <I>InvokeEvent (ViewRedefinedEvent, NULL)</I>.
     */
	virtual void DisplayxOyViewPlane ( );
	virtual void DisplayxOzViewPlane ( );
	virtual void DisplayyOzViewPlane ( );

	/**
	 * <P>Supprime le roulis.</P>
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (ViewRedefinedEvent, NULL);</I>.
	 */
	virtual void ResetRoll ( );

	/**
	 * <P>Annule un éventuel zoom préalable puis réinitialise la vue.</P>
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (ViewRedefinedEvent, NULL);</I>.
	 */
	virtual void ResetView ( );

	/**
	 * Recentre la vue sur la sélection.
	 */
	virtual void ResetViewOnSelection ( );

	/**
	 * \param	<I>Picker</I> à utiliser pour les opérations de sélection
	 *			interactive à la souris.
	 * \param	Commande associée.
	 * L'ensemble gère les opérations de sélection interactive avec le
	 * gestionnaire de sélection de <I>Magix 3D</I>.
	 * \see		SetEntitySeizureManager
	 */
	virtual void SetPickingTools (Mgx3D::QtVtkComponents::VTKMgx3DPicker*, Mgx3D::QtVtkComponents::VTKMgx3DPickerCommand*);
	virtual Mgx3D::QtVtkComponents::VTKMgx3DPicker* GetMgx3DPicker ( );

	/**
	 * \return	Eventuel gestionnaire de saisie interactive associé.
	 * \see		OnMiddleButtonDown
	 * \see		SetEntitySeizureManager
	 */
	virtual Mgx3D::QtComponents::EntitySeizureManager* GetEntitySeizureManager ( );

	/**
	 * \param	Gestionnaire de saisie interactive associé à l'opération de
	 *			sélection à effectuer.
	 * \see		OnMiddleButtonDown
	 * \see		GetEntitySeizureManager
	 */
	virtual void SetEntitySeizureManager (Mgx3D::QtComponents::EntitySeizureManager*);

	/**
	 * Effectue un picking si
	 * <I>QtMgx3DApplication::_pickOnLeftButtonDown.getValue ( )</I> vaut
	 * <I>true</I> et que la sélection interactive est activée, sinon invoque
	 * <I>vtkUnifiedInteractorStyle::OnLeftButtonDown</I>.
	 * \see		OnRightButtonDown
	 * \see		OnLeftButtonUp
	 * \see		Pick
	 * \see		SetInteractiveSelectionActivated
	 */
	virtual void OnLeftButtonDown ( );

	/**
	 * Effectue un picking si
	 * <I>QtMgx3DApplication::_pickOnRightButtonDown.getValue ( )</I> vaut
	 * <I>true</I> et que la sélection interactive est activée, sinon invoque
	 * <I>vtkUnifiedInteractorStyle::OnRightButtonDown</I>.
	 * \see		OnLeftButtonDown
	 * \see		OnRightButtonUp
	 * \see		Pick
	 * \see		SetInteractiveSelectionActivated
	 */
	virtual void OnRightButtonDown ( );

	/**
	 * En cas de fonctionnement avec une instance de la classe
	 * <I>EntitySeizureManager</I>, provoque le passage au gestionnaire de
	 * saisie suivant.
	 */
	virtual void OnMiddleButtonDown ( );

	/**
	 * Effectue un picking si
	 * <I>QtMgx3DApplication::_pickOnLeftButtonUp.getValue ( )</I> vaut
	 * <I>true</I>, que le curseur de la souris n'a pas bougé depuis que le
	 * bouton a été enfoncé, et que la sélection interactive est activée, sinon
	 * invoque <I>vtkUnifiedInteractorStyle::OnLeftButtonUp</I>.
	 * \see		OnLeftButtonDown
	 * \see		OnRightButtonUp
	 * \see		Pick
	 * \see		SetInteractiveSelectionActivated
	 */
	virtual void OnLeftButtonUp ( );

	/**
	 * Effectue un picking si
	 * <I>QtMgx3DApplication::_pickOnRightButtonUp.getValue ( )</I> vaut
	 * <I>true</I>, que le curseur de la souris n'a pas bougé depuis que le
	 * bouton a été enfoncé, et que la sélection interactive est activée, sinon
	 * invoque <I>vtkUnifiedInteractorStyle::OnRightButtonUp</I>.
	 * \see		OnRightButtonDown
	 * \see		OnLeftButtonUp
	 * \see		Pick
	 * \see		SetInteractiveSelectionActivated
	 */
	virtual void OnRightButtonUp ( );

	/**
	 * \param	Gestionnaire de sélection <I>Magix 3D</I> associé.
	 */
	virtual void SetSelectionManager (Mgx3D::Utils::SelectionManagerIfc*);

	/**
	 * (Dés)Active le mode de <I>sélection interactive</I> par clic souris.
	 * \see		OnLeftButtonDown
	 * \see		OnRightButtonDown
	 */
	virtual void SetInteractiveSelectionActivated (bool activate);
	virtual bool GetInteractiveSelectionActivated ( ) const;


	protected :

	/**
	 * Constructeur. RAS.
	 */
	vtkMgx3DInteractorStyle ( );

	/**
	 * Effectue une action de <I>picking</I>.
	 */
	virtual void Pick ( );

	/**
	 * En mode sélection, désélectionne la dernière sélection
	 * effectuée et passe à la suivante pointée au même endroit par la souris.
	 * Effectue la même opération mais en sens inverse si la touche <I>shift</I>
	 * est pressée.
	 */
	virtual void NextSelection ( );

	/**
	 *  Masque les entités sélectionnées
	 */
	virtual void HideSelection();


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	vtkMgx3DInteractorStyle (const vtkMgx3DInteractorStyle&);
	vtkMgx3DInteractorStyle& operator = (const vtkMgx3DInteractorStyle&);

	/** Outils de sélection interactive à la souris. */
	Mgx3D::QtVtkComponents::VTKMgx3DPicker*			Mgx3DPicker;
	Mgx3D::QtVtkComponents::VTKMgx3DPickerCommand*	Mgx3DPickerCommand;

	/** Le gestionnaire de sélection associé. */
	Mgx3D::Utils::SelectionManagerIfc*				SelectionManager;

	/** L'éventuelle fenêtre principale associée. */
	Mgx3D::QtComponents::EntitySeizureManager*		SeizureManager;

	/** Vaut <I>true</I> si une opération de <I>picking</I> doit être
	 * déclenchée respectivement sur bouton gauche ou bouton droit de la souris
	 * pressé, et <I>false</I> dans le cas contraire.
	 * \see			SetInteractiveSelectionActivated
	 */
	bool			 							InteractiveSelectionActivated;

	/** Position du curseur de la souris au moment où le bouton est pressé. */
	int											ButtonPressPosition [2];

	/** Liste des objets poités par la souris et pouvant prétendre à être
	 * sélectionnés. */
	std::vector<Mgx3D::Utils::Entity*>			PointedObjects;
};	// class vtkMgx3DInteractorStyle



#endif	// VTK_MGX3D_INTERACTOR_STYLE_H

