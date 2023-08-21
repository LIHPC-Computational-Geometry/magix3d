/**
 * \file		vtkLockableRenderWindow.h
 * \author		Charles PIGNEROL
 * \date		19/11/2018
 */
#include <vtkRenderingOpenGLConfigure.h>    // => VTK_USE_X, VTK_USE_OSMESA, ...
#include <vtkIndent.h>

//
// ATTENTION, VALIDE UNIQUEMENT CODE VTK >= 8
//
#if VTK_MAJOR_VERSION < 8
#ifdef VTK_USE_X            // Eventuellement défini dans vtkRenderingOpenGLConfigure.h de la distribution VTK.
#   include "VtkComponents/vtkECMXOpenGLRenderWindow.h"
#else
#   ifdef VTK_USE_OSMESA    // Eventuellement défini dans vtkRenderingOpenGLConfigure.h de la distribution VTK.
#       include <vtkOSOpenGLRenderWindow.h>
#   else
#       error "ABSENCE DE SYSTEME DE FENETRAGE"
# endif   // VTK_USE_OSMESA
#endif  // VTK_USE_X
#else	// VTK_MAJOR_VERSION < 8
#       include "VtkComponents/vtkDMASynchronizedOpenGLRenderWindow.h"	// VTK >= 8
#endif	// VTK_MAJOR_VERSION < 8

#ifndef VTK_LOCKABLE_RENDER_WINDOW_H
#define VTK_LOCKABLE_RENDER_WINDOW_H

/**
 * Classe de <I>render window VTK</I> dont on peut désactiver
 * directement l'affichage via sa méthode <I>LockRendering</I>.
 * Objectif : en mode client/serveur, pouvoir désactiver les
 * opérations de rendu, éventuellement provoquées via <I>Qt</I>
 *(setFocus, ...) afin que celles-ci n'entrent en conflit dans
 * le canal de communications avec d'autres opérations liées au
 * mode client/serveur.
 */
#if VTK_MAJOR_VERSION < 8
#	ifdef VTK_USE_X
class vtkLockableRenderWindow : public vtkECMXOpenGLRenderWindow
#	else
class vtkLockableRenderWindow : public vtkOSOpenGLRenderWindow
#	endif  // VTK_USE_X
#else 	// VTK_MAJOR_VERSION < 8
class vtkLockableRenderWindow : public vtkDMASynchronizedOpenGLRenderWindow
#endif	// VTK_MAJOR_VERSION < 8
{
	public :

	virtual ~vtkLockableRenderWindow ( );
	static vtkLockableRenderWindow* New ( );
#if VTK_MAJOR_VERSION < 8
#	ifdef VTK_USE_X
	vtkTypeMacro(vtkLockableRenderWindow,vtkECMXOpenGLRenderWindow);
#	else
	vtkTypeMacro(vtkLockableRenderWindow,vtkOSOpenGLRenderWindow);
#	endif  // VTK_USE_X
#else 	// VTK_MAJOR_VERSION < 8
	vtkTypeMacro(vtkLockableRenderWindow,vtkDMASynchronizedOpenGLRenderWindow);
#endif	// VTK_MAJOR_VERSION < 8
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

	vtkLockableRenderWindow ( );
	vtkLockableRenderWindow (const vtkLockableRenderWindow&);
	vtkLockableRenderWindow& operator = (const vtkLockableRenderWindow&);
	bool	Locked;
};	// class vtkLockableRenderWindow


class RenderWindowLock
{
	public :

	/**
	 * Verrouille la fenêtre transmise en argument.
	 */
	RenderWindowLock (vtkLockableRenderWindow*);

	/**
	 * Déverrouille la fenêtre gérée.
	 */
	virtual ~RenderWindowLock ( );

	/**
	 * (Dé)Verrouille la fenêtre gérée.
	 */
	virtual void lock ( );
	virtual void unlock ( );


	private :

	RenderWindowLock (const RenderWindowLock&);
	RenderWindowLock& operator = (const RenderWindowLock&);
	vtkLockableRenderWindow*	RenderWindow;
};	// class RenderWindowLock


#endif	// VTK_LOCKABLE_RENDER_WINDOW_H
