/**
 * Interacteur unifié DSSI de base utilisé dans les projets LEM et LOVE.
 * Spécialisé par LEM dans <I>vtkCustomizableInteractorStyleTrackball</I>.
 * 
 * \see		vtkCustomizableInteractorStyleTrackball
 */

#ifndef VTK_UNIFIED_INTERACTOR_STYLE_H
#define VTK_UNIFIED_INTERACTOR_STYLE_H


//#include "VtkContrib/vtk_versions.h"	// Pour versions VTK < 5
#include <vtkInteractorStyleTrackballCamera.h>


/** <P>Classe d'interacteur VTK dont le comportement est voisin de celui 
 * d'Ensight.</P>
 * <P>Cet interacteur propose également, par défaut, les services
 * suivants :<BR>
 * <OL>
 * <LI>Informations sur les touches <I>Shift</I> et <I>Control</I>,
 * <LI>Déplacement à l'aide des touches "flèches",
 * <LI>Positionnement de la vue dans des plans à l'aide des touches <I>x</I>,
 * <I>y</I> et <I>z</I>,
 * <I>Définitions du centre de rotation à l'aide de la touche <I>F</I>
 * (centre d'un acteur ou d'une maille).
 * </OL>
 * </P>
 * <P>Les raccourcis clavier sont personnalisables de même que plusieurs
 * services de cette classe.</P>
 *
 * @author		Charles PIGNEROL, Daniel AGUILERA	CEA/DAM/DSSI
 * @date		16/10/2012
 */
class vtkUnifiedInteractorStyle : public vtkInteractorStyleTrackballCamera
{
	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~vtkUnifiedInteractorStyle ( );

	/**
	 * Instancie cette classe selon le "pattern" de fabrication VTK.
	 */
	static vtkUnifiedInteractorStyle* New ( );

	vtkTypeMacro(vtkUnifiedInteractorStyle,vtkInteractorStyleTrackballCamera);

	/**
	 * Affiche des informations dans os sur l'instance.
	 */
	virtual void PrintSelf (ostream& os, vtkIndent indent);

	/** <P>Surcharge les actions suivantes :<BR>
	 * <OL> 
	 * <LI> <I>f</I> : le centre de rotation est l'acteur,
	 * <LI> <I>F</I> : le centre de rotation est le point visé,
	 * <LI> <I>x</I> ou <I>X</I> : vue positionnée dans le plan yOz,
	 * <LI> <I>y</I> ou <I>Y</I> : vue positionnée dans le plan xOz,
	 * <LI> <I>z</I> ou <I>Z</I> : vue positionnée dans le plan xOy,
	 * <LI> <I>a</I> ou <I>A</I> : suppression du roulis,
	 * <LI> <I>r</I> ou <I>R</I> : annule un éventuel zoom préalable,
	 * puis invoque vtkInteractorStyleTrackballCamera::OnChar.
	 * <LI> Suppression du passage au mode stéréo sur la touche 3.
	 * </OL>
	 * </P>
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	 * \see		FlyTo
	 * \see		DisplayxOyViewPlane
	 * \see		DisplayxOzViewPlane
	 * \see		DisplayxOzViewPlane
	 * \see		ResetRoll
	 * \see		ResetView
	 */
	virtual void OnChar ( );

	/**
	 * <P>Surcharge pour éviter de redéterminer le renderer ou se trouve
	 * l'interacteur à chaque mouvement de souris : cela permet dans le cas de
	 * renderers multiples d'appliquer les mouvements au renderer où l'action a
	 * debuté.</P>
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	 */
	virtual void OnMouseMove  ( );

	/** @return		true si la touche <I>control</I> est pressée, sinon 
	 * false. */
	virtual bool isControlKeyPressed ( );

	/** @return		true si la touche <I>shift</I> est pressée, sinon false.
	  */
	virtual bool isShiftKeyPressed ( );

	/** 
	 * <P>Teste si le caractère est Key_Up, Key_Left, Key_Right, ou Key_Down
	 * (de Qt). Appelle, si c'est le cas, la méthode associée. Dans le
	 * cas contraire invoque OnKeyRelease de sa classe parente. </P>
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	 * <P>En mode stéréo, OnChar prend en compte les touches
	 * increaseEyeAngleKey '>' et decreaseEyeAngleKey '<' pour augmenter ou
	 * diminuer l'angle entre les yeux.</P>
	 * @see		OnLeftArrow
	 * @see		OnUpArrow
	 * @see		OnRightArrow
	 * @see		OnDownArrow
	 * @see		GetMotionRatio
	 * @see		SetMotionRatio
	 */
	virtual void OnKeyRelease ( );

	/**
	 * @return		le ratio utilisé pour déterminer l'amplitude d'un 
	 *				déplacement par rapport à la taille de la fenêtre.
	 * @see			OnKeyRelease
	 * @see			SetMotionRatio
	 */
	virtual double GetMotionRatio ( ) const
	{ return _motionRatio; }

	/**
	 * @param		Nouveau ratio à utiliser pour déterminer
	 *				l'amplitude d'un déplacement par rapport à la taille 
	 *				de la fenêtre. Doit être strictement positif pour 
	 *				être pris en compte.
	 * @see			GetMotionRatio
	 * @see			OnKeyRelease
	 */
	virtual void SetMotionRatio (double ratio);

	/** Action effectuée lorsque la touche "Flèche vers la gauche"
	 * est activée. Invoque Pan avec un déplacement de souris de
	 * - GetMotionRatio ( ) * largeur de la fenêtre. 
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	 */
	virtual void OnLeftArrow ( );

	/** Action effectuée lorsque la touche "Flèche vers le haut"
	 * est activée. Invoque Pan avec un déplacement de souris de
	 * GetMotionRatio ( ) * hauteur de la fenêtre. 
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	 */
	virtual void OnUpArrow ( );

	/** Action effectuée lorsque la touche "Flèche vers la droite"
	 * est activée. Invoque Pan avec un déplacement de souris de
	 * GetMotionRatio ( ) * largeur de la fenêtre. 
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	 */
	virtual void OnRightArrow ( );

	/** Action effectuée lorsque la touche "Flèche vers le bas"
	 * est activée. Invoque Pan avec un déplacement de souris de
	 * - GetMotionRatio ( ) * hauteur de la fenêtre. 
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	 */
	virtual void OnDownArrow ( );

	/**
	 * <P>Augmente l'angle entre les yeux de <I>eyeAngleModifier</I> si
	 * <I>increase</I> vaut <I>true</I>, et le diminue de
	 * <I>eyeAngleModifier</I> dans le cas contraire.</P>
	 * <P>Cet angle est utilisé durant la génération d'images en mode
	 * <B>stéréo</B>.</P>
	 */
	virtual void IncreaseEyeAngle (bool increase);

	/**
	 * Si <I>centerOnActor</I> vaut <I>true</I> le centre de rotation devient
	 * l'acteur, sinon il devient le point visé.
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	  */
	virtual void FlyTo (bool centerOnActor);

	/**
	 * Positionne la vue dans le plan <I>xOy</I>.
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	 */
	virtual void DisplayxOyViewPlane ( );

	/**
	 * Positionne la vue dans le plan <I>xOz</I>.
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	 */
	virtual void DisplayxOzViewPlane ( );

	/**
	 * Positionne la vue dans le plan <I>yOz</I>.
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	 */
	virtual void DisplayyOzViewPlane ( );

	/**
	 * Supprime le roulis.
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	 */
	virtual void ResetRoll ( );

	/**
	 * Annule un éventuel zoom préalable puis réinitialise la vue.
	 * <P>Ces évènements ponctuels provoquent l'invocation de
	 * <I>InvokeEvent (vtkCommand::InteractionEvent, NULL);</I>.
	 */
	virtual void ResetView ( );

	/**
	 * Touches augmentant et diminuant l'angle entre les yeux en mode
	 * stéréo. ('>' et '<' par défaut
	 */
	static char		increaseEyeAngleKey, decreaseEyeAngleKey;

	/**
	 * Touche permettant d'activer ou de desactiver la stéréo (3 par 
	 * défaut)
	 */
	static char		toggleStereoKey;

	/**
	 * Valeur dont est augmenté ou diminué l'angle entre les yeux en
	 * mode stéréo lorsque les touches increaseEyeAngleKey et
	 * decreaseEyeAngleKey sont pressées.
	 */
	static double	eyeAngleModifier;

	/**
	 * Utilisé lors des déplacements verticaux de la souris avec le bouton
	 * droit enfoncé. Si cet attribut vaut <I>false</I> (valeur par 
	 * défaut) et que la souris est déplacée vers le haut, on 
	 * s'éloigne du maillage, alors que si la souris est déplacée vers
	 * le bas on zoom.
	 * @see		Dolly
	 */
	static bool		upZoom;


	protected :

	/**
	 * Constructeur. RAS.
	 */
	vtkUnifiedInteractorStyle ( );

	/**
	 * <P>Par défaut, se rapproche si le curseur de la souris se déplace 
	 * vers le bas, recule s'il se déplace vers le haut.</P>
	 * <P>Se comportement peut être inversé par modification de 
	 * l'attribut statique <I>upZoom</I>.</P>
	 * @param	facteur de rapprochement ou d'éloignement.
	 * @see		upZoom
	 */
	virtual void Dolly (double factor);
	virtual void Dolly ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	vtkUnifiedInteractorStyle (const vtkUnifiedInteractorStyle&);
	vtkUnifiedInteractorStyle& operator = (const vtkUnifiedInteractorStyle&);

	/**
	 * Le ratio utilisé pour déterminer l'amplitude du déplacement
	 * suite à un évènement dû à l'activation d'une touche
	 * flèche de déplacement (Haut/Bas/Gauche/Droite).
	 */
	double		_motionRatio;
};	// class vtkUnifiedInteractorStyle



#endif	// VTK_UNIFIED_INTERACTOR_STYLE_H

