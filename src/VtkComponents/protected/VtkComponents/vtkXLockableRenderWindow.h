/**
 * \file		vtkXLockableRenderWindow.h
 * \author		Charles PIGNEROL
 * \date		04/12/2020
 */
#include <vtkXOpenGLRenderWindow.h>
#include <vtkIndent.h>


#ifndef VTK_X_LOCKABLE_RENDER_WINDOW_H
#define VTK_X_LOCKABLE_RENDER_WINDOW_H

/**
 * Classe de <I>render window VTK</I> dont on peut désactiver
 * directement l'affichage via sa méthode <I>LockRendering</I>.
 * Objectif : en mode client/serveur, pouvoir désactiver les
 * opérations de rendu, éventuellement provoquées via <I>Qt</I>
 *(setFocus, ...) afin que celles-ci n'entrent en conflit dans
 * le canal de communications avec d'autres opérations liées au
 * mode client/serveur.
 */
class vtkXLockableRenderWindow : public vtkXOpenGLRenderWindow
{
	public :

	virtual ~vtkXLockableRenderWindow ( );
	static vtkXLockableRenderWindow* New ( );

	vtkTypeMacro(vtkXLockableRenderWindow,vtkXOpenGLRenderWindow);

	void PrintSelf (std::ostream& os, vtkIndent indent);

	/**
	 * Effectue le rendu si <I>LockRendering ( )</I> retourne <I>true>/I>.
	 */
	virtual void Render ( );

	/**
	 * (Dé)vérouille le rendu selon la valeur de <I>lock</I>
	 * \see		Render
	 */
	virtual void LockRendering (bool lock);
	virtual bool LockRendering ( );


	private :

	vtkXLockableRenderWindow ( );
	vtkXLockableRenderWindow (const vtkXLockableRenderWindow&);
	vtkXLockableRenderWindow& operator = (const vtkXLockableRenderWindow&);
	bool	Locked;
};	// class vtkXLockableRenderWindow


class XRenderWindowLock
{
	public :

	/**
	 * Verrouille la fenêtre transmise en argument.
	 */
	XRenderWindowLock (vtkXLockableRenderWindow*);

	/**
	 * Déverrouille la fenêtre gérée.
	 */
	virtual ~XRenderWindowLock ( );

	/**
	 * (Dé)Verrouille la fenêtre gérée.
	 */
	virtual void lock ( );
	virtual void unlock ( );


	private :

	XRenderWindowLock (const XRenderWindowLock&);
	XRenderWindowLock& operator = (const XRenderWindowLock&);
	vtkXLockableRenderWindow*	RenderWindow;
};	// class XRenderWindowLock


#endif	// VTK_X_LOCKABLE_RENDER_WINDOW_H
