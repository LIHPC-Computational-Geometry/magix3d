#ifndef VTK_CUSTOMIZABLE_INTERACTOR_STYLE_TRACKBALL_H
#define VTK_CUSTOMIZABLE_INTERACTOR_STYLE_TRACKBALL_H


#include "QtVtkComponents/vtkMgx3DInteractorStyle.h"

#include <vtkCamera.h>


/**
 * Etat d'interaction où le zoom est commandé par l'IHM.
 */
#define VTKIS_GUI_ZOOM			100


/** Classe d'interacteur VTK type "vtkInteractorStyleTrackballCamera" 
 * dont on peut activer ou désactiver l'affichage de la boite englobante de 
 * sélection. Ce type d'interacteur permet également de faire fonctionner des
 * commandes sur le nouvel état d'interaction <B>VTKIS_GUI_ZOOM</B> 
 * (bouton gauche de la souris et touches CTRL et SHIFT enfoncés).
 */
class vtkCustomizableInteractorStyleTrackball : public vtkMgx3DInteractorStyle
{
	public :

	virtual ~vtkCustomizableInteractorStyleTrackball ( );

	static vtkCustomizableInteractorStyleTrackball* New ( );

	vtkTypeMacro(vtkCustomizableInteractorStyleTrackball,vtkMgx3DInteractorStyle);

	/**
	 * Classe contenant les paramètres utiles à la gestion de l'évènement
	 * VTKIS_GUI_ZOOM.
	 */
	class GuiZoomEvent
	{
		friend class vtkCustomizableInteractorStyleTrackball;


		public :

		/** 
		 * Constructeurs. Opérateur =. RAS.
		 */
		GuiZoomEvent ( )
			: _x (0), _y (0), _interactorStyle (0)
		{ }
		GuiZoomEvent (const GuiZoomEvent& gze)
			: _x (gze.x ()), _y (gze.y ( )),
			  _interactorStyle (gze.getInteractorStyle ( ))
		{ }
		GuiZoomEvent& operator = (const GuiZoomEvent& gze)
		{ _x	= gze.x ( ), _y	= gze.y ( ), 
		  _interactorStyle	= gze.getInteractorStyle ( ); 
		  return *this; }

		/**
		 * Destructeur. RAS.
		 */
		virtual ~GuiZoomEvent ( )
		{ }

		/**
		 * @return	l'abscisse siège de l'évènement.
		 */
		virtual int x ( ) const
		{ return _x; }

		/**
		 * @return	l'ordonnée siège de l'évènement.
		 */
		virtual int y ( ) const
		{ return _y; }

		/**
		 * @return		l'interacteur associé à l'opération
		 */
		virtual vtkInteractorStyle* getInteractorStyle ( ) const
		{ return _interactorStyle; }


		protected :

		/**
		 * @param		Coordonnées de l'évènement.
		 */
		virtual void setCoordinates (int x, int y)
		{ _x	= x;	_y	= y; }

		/**
		 * @param		Caméra active.
		 */
		virtual void setInteractorStyle (vtkInteractorStyle* interactorStyle)
		{ _interactorStyle	= interactorStyle; }


		private :

		/** Coordonnées de l'évènement. */
		int						_x, _y;

		/** Interacteur associé à l'opération. */
		vtkInteractorStyle*		_interactorStyle;
	};	// struct GuiZoomEvent


	/**
	 * interactor->GetKeyCode ( ) ne retourne pas la même valeur selon qu'un
	 * modificateur est pressé ou non.
	 * Cette méthode invoque interactor->SetKeyCode (*interactor->GetKeySym ( ))
	 * puis vtkMgx3DInteractorStyle::OnChar ( ).
	 */
	virtual void OnChar ( );

	/**
	 * Passe dans l'état VTKIS_GUI_ZOOM en envoyant l'évènement
	 * vtkCommand::StartInteractionEvent si les touches shift et control
	 * sont enfoncées. Dans le cas contraire invoque 
	 * vtkMgx3DInteractorStyle::OnLeftButtonDown.
	 */
	virtual void OnLeftButtonDown ( );

	/**
	 * Quitte l'état VTKIS_GUI_ZOOM si c'est l'état courrant de 
	 * l'interacteur, et envoie l'évènement vtkCommand::EndInteractionEvent. 
	 * Dans le cas contraire invoque vtkMgx3DInteractorStyle::OnLeftButtonUp.
	 */
	virtual void OnLeftButtonUp ( );

	/**
	 * Envoie l'évènement vtkCommand::InteractionEvent si l'état est
	 * VTKIS_GUI_ZOOM. Dans le cas contraire invoque 
	 * vtkMgx3DInteractorStyle::OnMouseMove.
	 */
	virtual void OnMouseMove ( );

	/**
	 * Quitte l'état VTKIS_GUI_ZOOM si c'est l'état courrant de l'interacteur
	 * et que la touche shift ou que la touche control n'est pas pressée.
	 * Dans le cas contraire invoque vtkMgx3DInteractorStyle::OnKeyRelease.
	 */
	virtual void OnKeyRelease ( );

	/**
	 * Le comportement par défaut est d'invoquer <I>Dolly</I>
	 * (rapprochement/éloignement du point focale). Cette classe permet de jouer
	 * sur le zoom lorsque cet évènement est appelé.
	 */
	virtual void OnMouseWheelForward ( );
	virtual void OnMouseWheelBackward ( );

	virtual void PrintSelf (ostream& os, vtkIndent indent);

	/** @return		true si une boite englobante doit être affichée autour
	 *				de l'acteur sélectionné, sinon false. */
	virtual bool IsActorHighlightingEnabled ( ) const
	{ return _displayBoundingBox; }

	/** Affiche une boite englobante autour des acteurs sélectionnés si
	 *  enable vaut true. Dans le cas contraire, rien n'est affiché.  Vaut
	 * true par défaut. */
	virtual void EnableActorHighlighting (bool enable)
	{ _displayBoundingBox	= enable; }

	/** Affiche (ou masque si déjà affichée) la boite englobante de
	 * l'acteur reçue en argument et si IsActorHighlightingEnabled retourne
	 * true. */
	virtual void HighlightActor2D (vtkActor2D* actor);

	/** Affiche (ou masque si déjà affichée) la boite englobante de
	 * l'acteur reçue en argument et si IsActorHighlightingEnabled retourne
	 * true. */
	virtual void HighlightProp3D (vtkProp3D* prop);

	/**
	 * @return		<I>true</I> si les évènements de type <I>WheelEvent</I>
	 *				provoquent une action de type <I>zoom</I>, <I>false</I> dans
	 *				le cas contraire.
	 * @see			EnableZoomOnWheelEvent
	 */
	virtual bool isZoomOnWheelEventEnabled ( ) const
	{ return _zoomOnWheelEvent; }

	/**
	 * @param		<I>true</I> si les évènements de type <I>WheelEvent</I>
	 *				provoquent une action de type <I>zoom</I>, <I>false</I> dans
	 *				le cas contraire.
	 * @see			isZoomOnWheelEventEnabled
	 */
	virtual void EnableZoomOnWheelEvent (bool enable)
	{ _zoomOnWheelEvent = enable; }

	/**
	 * @return		Le facteur de grossissement/réduction appliqué lors des
	 *				opérations de zoom.
	 * @see			SetZoomFactor
	 */
	virtual double GetZoomFactor ( ) const
	{ return _zoomFactor; }

	/**
	 * @param		Le facteur de grossissement/réduction appliqué lors des
	 *				opérations de zoom. Doit être supérieur ou égal à 1.
	 * @see			GetZoomFactor
	 */
	virtual void SetZoomFactor (double factor);


	protected :

	vtkCustomizableInteractorStyleTrackball ( );

	/**
	 * @return		Les paramètres de l'évènement VTKIS_GUI_ZOOM en cours.
	 */
	virtual GuiZoomEvent& getGuiZoomParameters ( )
	{ return _guiZoomParameters; }

	/**
	 * Met à jour les paramètres de l'évènement VTKIS_GUI_ZOOM en cours.
	 */
	virtual void setGuiZoomParameters (int x, int y, vtkInteractorStyle* interactorStyle);


	private :

	vtkCustomizableInteractorStyleTrackball (const vtkCustomizableInteractorStyleTrackball&);
	vtkCustomizableInteractorStyleTrackball& operator = (const vtkCustomizableInteractorStyleTrackball&);

	/** Affichage (true) ou non (false) d'une boite englobante autour des
	 * groupe de données sélectionnés.
	 */
	bool			_displayBoundingBox;

	/**
	 * Zoom (<I>true</I> ou non <I>false</I> lors d'évènements de type
	 * <I>MouseWheel</I>. Vaut <I>false</I> par défaut.
	 */
	bool			_zoomOnWheelEvent;

	/**
	 * Facteur de grossissement/réduction appliqué lors des opérations de zoom.
	 * Vaut <I>1.1</I> par défaut. Ne doit pas être nul !
	 */
	double			_zoomFactor;

	/**
	 * Paramètres de l'évènement de type VTKIS_GUI_ZOOM en cours.
	 */
	GuiZoomEvent	_guiZoomParameters;
};	// class vtkCustomizableInteractorStyleTrackball

#endif	// VTK_CUSTOMIZABLE_INTERACTOR_STYLE_TRACKBALL_H

