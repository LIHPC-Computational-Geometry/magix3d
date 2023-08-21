//
// Created by charles on 04/05/2020.
//

#ifndef VTK_DMA_SYNCHRONIZED_OPENGL_RENDER_WINDOW_H
#define VTK_DMA_SYNCHRONIZED_OPENGL_RENDER_WINDOW_H

#include <vtkGenericOpenGLRenderWindow.h>

/**
 * Classe de fenêtre de rendu pour processus client avec synchronisation "socket" type <I>vtkSynchronizedRenderWindows</I>,
 * avec méthode Render sans fioriture, l'objectif étant d'en faire le strict minimum.
 * Cette classe a pour vocation de gérer les interactions avec l'utilisateur et d'afficher l'image d'une scène calculée par un serveur
 * (éventuellement parallèle).
 */
class vtkDMASynchronizedOpenGLRenderWindow : public vtkGenericOpenGLRenderWindow
{
	public :

	static vtkDMASynchronizedOpenGLRenderWindow* New ( );

	virtual void Render ( );


	protected :

	vtkDMASynchronizedOpenGLRenderWindow ( );
	virtual ~vtkDMASynchronizedOpenGLRenderWindow ( );
	
	
	private :
	
	vtkDMASynchronizedOpenGLRenderWindow (const vtkDMASynchronizedOpenGLRenderWindow&);
	vtkDMASynchronizedOpenGLRenderWindow& operator = (const vtkDMASynchronizedOpenGLRenderWindow&);
	
};	// class vtkDMASynchronizedOpenGLRenderWindow


#endif	// VTK_DMA_SYNCHRONIZED_OPENGL_RENDER_WINDOW_H
