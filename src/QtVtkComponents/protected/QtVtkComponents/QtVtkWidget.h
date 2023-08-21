#ifndef VTK_9

/**
 * \file		QtVtkWidget.h
 * \author		Charles PIGNEROL
 * \date		04/06/2012
 */
#ifndef QT_VTK_WIDGET_H
#define QT_VTK_WIDGET_H

#include <TkUtil/Mutex.h>

#include <QVTKWidget.h>

#include <vtkUnsignedCharArray.h>


namespace Mgx3D 
{

namespace QtVtkComponents
{

/**
 * <P>Classe de widget VTK/Qt utilisée dans le projet. Le fait de dériver la
 * classe de la classe <I>VTK</I> <I>QVTKWidget</I> permet d'adapter ce widget
 * facilement au besoins du projet.
 * </P>
 * 
 * <P>Ce widget a deux modes de fonctionnement :<BR>
 * <OL>
 * <LI>Un mode <I>classique</I>, le widget à sa fenêtre de rendu et son
 * <I>renderer</I qui effectue les rendus dans la fenêtre de rendu,
 * <LI>Un mode <I>client/serveur</I>, où le gestionnaire de ce widget met à jour
 * l'image (fournie par un serveur graphique, parallèle ou non) dans un cache
 * et en informe le widget. Celui-ci réactualise alors le ontenu de la fenêtre
 * de rendu. Le <I>renderer</I> est désactivé dans ce mode.
 * </OL>
 * </P>
 */
class QtVtkWidget : public QVTKWidget
{
	Q_OBJECT

	public :

	/**
	 * Constructeur. RAS.
	 * \param		Widget parent.
	 * \param		Eventuelle window <I>VTK</I> à utiliser.
	 */
	QtVtkWidget (QWidget* parent, vtkRenderWindow* window);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtVtkWidget ( );

	/**
	 * Passe en mode de fonctionnement client/serveur où le rendu de l'image
	 * est effectué côté serveur. Le gestionnaire de ce widget a pour mission
	 * d'actualiser l'image en cache puis d'invoquer <I>imageCacheUpdated</I>
	 * qui réactualise la fenêtre de rendu dans le thread de l'IHM.
	 * \param		Largeur maximale acceptable pour l'image
	 * \param		Hauteur maximale acceptable pour l'image
	 * \param		Profondeur de l'image
	 * \see			getImageCache
	 * \see			imageCacheUpdated
	 * \see			getImageCacheMutex
	 * \see			deactivateClientServerMode
	 */
	virtual void activateClientServerMode (
		unsigned short maxWidth, unsigned short maxHeight, unsigned char depth);

	/**
	 * Repasse en mode de rendu local.
	 * \see			activateClientServerMode
	 */
	virtual void deactivateClientServerMode ( );

	/**
	 * \return		Le cache de l'image rendue.
	 * \see			imageCacheUpdated
	 * \see			getImageCacheMutex
	 */
	virtual vtkUnsignedCharArray& getImageCache ( );

	/**
	 * A appeler lorsque les données en cache ont été actualisées
	 * (mode client/serveur).
	 * Actualise, dans le thread de l'IHM, la fenêre graphique.
	 * \warning		ATTENTION, code pour VTK 5.6 et ultérieur.
	 * \see			getImageCache
	 * \see			getImageCacheMutex
	 */
	virtual void imageCacheUpdated ( );

	/**
	 * \return		Un mutex permettant de protéger le cache de l'image.
	 *				Utile en mode client/serveur, lorsque le serveur envoie une
	 *				nouvelle image. Permet de modifier les données en cache
	 *				sans que celles-ci ne soit utilisées par le widget.
	 */
	virtual TkUtil::Mutex& getImageCacheMutex ( );


	public slots :

	/**
	 * Demande un raffraîchissement de la vue dans le thread de l'IHM.
	 * \param		<I>true</I> si un render doit être préalablement fait.
	 * \see		
	 */
	virtual void updateView (bool render);


	signals :

	/**
	 * Emet ce signal pour provoquer un raffraîchissement de la vue
	 * graphique. Dispositif à utiliser pour travailler dans le thread
	 * de l'IHM.
	 */
	void needUpdate (bool render);

	/**
	 * Emis lorsqu'une touche est relachée.
	 */
	void keyRelease (QKeyEvent*);


	protected :

	/**
	 * S'assure du focus lorsque qu'elle est survolée par le curseur de la
	 * souris.
	 */
	virtual void enterEvent (QEvent*);

	/**
	 * Raffarîchit la fenêtre lorsqu'elle est démasquée.
	 */
	virtual void focusInEvent (QFocusEvent* event);

	/**
	 * Emet le signal keyRelease
	 */
	virtual void keyReleaseEvent (QKeyEvent * event);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtVtkWidget (const QtVtkWidget&);
	QtVtkWidget& operator = (const QtVtkWidget&);

	/** Le cache pour l'image (utile en fonctionnement client/serveur). */
	vtkUnsignedCharArray*			_imageCache;

	/** Dimensions max d'une image. */
	unsigned short					_maxImageWidth, _maxImageHeight;

	/** Profondeur des images. */
	unsigned char					_imageDepth;

	/** Un mutex pour protéger le cache de l'image (utile en fonctionnement
	    client/serveur). */
	TkUtil::Mutex					_imageCacheMutex;

	/** Faut il actualiser la fenêtre graphique avec le cache lors d'un
	 * raffraichissement ? */
	bool							_updateFromCache;
};	// class QtVtkWidget



}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// QT_VTK_WIDGET_H

#endif	// VTK_9
