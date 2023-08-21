/**
 * \file			vtkECMFactory.h
 * \author			Charles PIGNEROL
 * \date			21/06/2012
 *
 * \warning			Fournit la classe <I>vtkECMXOpenGLRenderWindow</I> pourles
 *					rendus off-screen.
 */
#ifndef VTK_ECM_FACTORY_H
#define VTK_ECM_FACTORY_H

#include <vtkObjectFactory.h>
#include <vtkWindow.h>
#include <vtkRenderWindow.h>


/**
 * Affiche/masque la fenêtre <I>VTK</I> transmise en argument selon la valeur
 * retournée par <I>window.GetMapped ( )</I>.
 */
void restoreMapState (vtkRenderWindow& window);

/** 
 * <P>Fabrication <I>VTK</I> fournissant des classes surchargeant des
 * classes <I>VTK</I> de base.
 * </P>
 *
 * <P>Cette classe repose sur le mécanisme des fabrications <I>VTK</I>.
 * Usage, au début de l'initialisation de l'appication :<BR>
 * <CODE>
 * vtkECMFactory*  ecmVTKFactory   = vtkECMFactory::New ( );
 * vtkObjectFactory::RegisterFactory (ecmVTKFactory);
 * ecmVTKFactory->Delete ( );
 * </CODE>
 * </P>
 *
 * <P>
 * Classes fournies par cette version :<BR>
 * <UL>
 * <LI><I>vtkECMXOpenGLRenderWindow</I> : classe de <I>vtkRenderWindow</I>
 * fonctionnant sous <I>Open GL/X11</I>. En mode <I>off screen</I> cette
 * classe n'est pas affichée à l'écran alors que la classe qu'elle
 * remplace (<I>vtkXOpenGLRenderWindow</I>) le fait (en <I>VTK 5.8.0</I>).
 * </UL>
 * </P>
 */
class vtkECMFactory : public vtkObjectFactory
{
	public :

	vtkECMFactory ( );

	virtual ~vtkECMFactory ( );

	virtual const char* GetVTKSourceVersion ( );

	virtual const char* GetDescription ( );

	static vtkECMFactory* New ( );


	private :

	vtkECMFactory (const vtkECMFactory&);
	vtkECMFactory& operator = (const vtkECMFactory&);
};	// class vtkECMFactory

#endif	// VTK_ECM_FACTORY_H
