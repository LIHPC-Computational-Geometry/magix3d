/**
 * \file		QtVtkMgx3DMainWindow.h
 * \author		Charles PIGNEROL
 * \date		12/10/2012
 */
#ifndef QT_VTK_MGX3D_MAIN_WINDOW_H
#define QT_VTK_MGX3D_MAIN_WINDOW_H

#include "QtComponents/QtMgx3DMainWindow.h"

#ifdef USE_EXPERIMENTAL_ROOM
#include "QtVtkComponents/QtVtkMgx3DExperimentalRoomPanel.h"
#endif	// USE_EXPERIMENTAL_ROOM


namespace Mgx3D 
{

namespace QtVtkComponents
{

/**
 * \brief		Classe de fenêtre principale de l'application <I>Magix 3D</I>
 *				spécialisé pour du rendu 3D avec <I>VTK</I>.
 */
class QtVtkMgx3DMainWindow : public QtComponents::QtMgx3DMainWindow
{
	public :

	/**
	 * Appeler <B>init</B> pour créer le panneau.
	 * \param	Eventuelle fenêtre parente
	 * \param	Paramètres Qt de la fenêtre
	 * \see		init
	 */
	 QtVtkMgx3DMainWindow (QWidget* parent = 0, Qt::WindowFlags flags = 0);

	/**
	 * RAS.
	 */
	virtual ~QtVtkMgx3DMainWindow ( );

	/**
	 * Divers IHM.
	 */
	//@{

	/**
	 * Appelle createGui.
	 * \param	Nom du context éventuellement créé
	 * \param	Contexte <I>Magix 3D</I> de la session.
	 * \param	Eventuel widget"arbre" contenant les entités de la session.
	 * \see		createGui
	 */
	virtual void init (const std::string& name, Mgx3D::Internal::ContextIfc* context,
				QtComponents::QtGroupsPanel* groupsPanel = 0, QtComponents::QtEntitiesPanel* entitiesPanel = 0);

	/**
	 * Créé une boite de dialogue d'édition des propriétés d'affichage des entités.
	 * \param	Nom du panneau.
	 * \param	Entités soumises aux modifications.
	 * \param	Liste des valeurs aux mailles affichables,
	 * \param	Nom de la valeur (éventuellement) affichée proposée par défaut.
	 * \return	La boite de dialogue créée, à <B>détruire par la fonction
	 * 			appelante</B>.
	 */
	virtual Mgx3D::QtComponents::QtRepresentationTypesDialog* createRepresentationTypesDialog (
			const std::string& name, 
			const std::vector<Mgx3D::Utils::Entity*>& entities,
			const std::string& helpURL, const std::string& helpTag
		);

	//@}

	/**
	 * La vue graphique.
	 */
	//@{

	/**
	 * \param		La vue graphique 3D à prendre en charge.
	 * \exception	Une exception est levée si une vue graphique est déjà gérée
	 * 				par l'instance.
	 */
	virtual void setGraphicalWidget (Mgx3D::QtComponents::Qt3DGraphicalWidget& widget);

#ifdef USE_EXPERIMENTAL_ROOM
	/**
	 * <P>Créé le widget "Pointages laser".</P>
	 * \see		getExperimentalRoomPanel
	 */
	virtual void createRaysPanel ( );
#endif	// USE_EXPERIMENTAL_ROOM
	//@}	// La vue graphique.

	/**
	 * La sélection d'entités.
	 */
	//@{

	/**
	 * Associe le gestionnaire de saisie à l'interacteur utilisé dans la vue
	 * graphique.
 	 * \param		Elément d'IHM observateur de sélection de la sélection
 	 * 				interactive.
 	 * \see			unregisterEntitySeizureManager
 	 */
 	 virtual void setEntitySeizureManager (Mgx3D::QtComponents::EntitySeizureManager* seizureManager);

	/**
	 * Déréférence le sélecteur interactif d'entités.
	 */
	virtual void unregisterEntitySeizureManager (Mgx3D::QtComponents::EntitySeizureManager* seizureManager);

	//@}	// La sélection d'entités.

	/**
	 * Diverses opérations.
	 */
	//@{

	/**
	 * @return	Le jeu de caractères à utiliser par défaut lors de l'enregistrement de
	 *		lasers ou diagnostics.
	 */
	virtual TkUtil::Charset getDefaultRaysCharset ( ) const;

	//@}	// Diverses opérations

	/**
	 * \return		Les composants pour la boite de dialogue "A propos de ...".
	 */
	virtual std::vector <QtAboutDialog::Component> getAboutDlgComponents ( ) const;


	protected :

	/**
	 * La fermeture de la fenêtre. cf. API Qt.
	 */
	virtual void closeEvent (QCloseEvent* event);

	/**
 	 * \return	le panneau "Pointages laser".
 	 */
#ifdef USE_EXPERIMENTAL_ROOM 	 
	virtual QtVtkMgx3DExperimentalRoomPanel* getExperimentalRoomPanel ( );
#endif	// USE_EXPERIMENTAL_ROOM
	virtual void setLandmark (Utils::Landmark::kind landmark);

	/**
	 * Permet de modifier les préférences utilisateur.
	 */
	virtual void preferencesCallback ( );

	/**
	 * Passe en mode d'interaction 2D/3D.
	 */
	virtual void mode2DCallback (bool enable2D);

	/**
	 * Affiche une boite de dialogue d'impression de la vue 3D.
	 */
	virtual void print3DViewCallback ( );

	/**
	 * Affiche une boite de dialogue d'impression dans un fichier de la vue 3D.
	 */
	 virtual void print3DViewToFileCallback ( );

	/**
	 * Callback sur le mode de sélection : boite englobante ou distance à l'entité.
	*/
	virtual void selectionModeCallback (bool boundingBox);

	/**
	 * Callback sur le mode de sélection : actualisent menus/boutons/interacteur.
	 */	
	virtual void pickingSelectionCallback (bool on);
	virtual void rubberBandSelectionCallback (bool on);
	virtual void rubberBandInsideSelectionCallback (bool on);
	virtual void visibleSelectionCallback (bool on);

#ifdef USE_EXPERIMENTAL_ROOM
	/**
	 * Callbacks du menu "Chambre expérimentale".
	 */
	//@{

	/**
	 * Affiche un sélecteur de fichier de chargement de pointages laser.
	 * \see		loadRays
	 */
	virtual void loadRaysCallback ( );

	/**
	 * Affiche un sélecteur de fichier de chargement de pointages laser
	 * positionné dans la base de données de pointages laser.
	 * \see		loadRays
	 */
	virtual void importRaysCallback ( );

	/**
	 * Enregistre les pointages laser dans le fichier courant.
	 */
	virtual void saveRaysCallback ( );

	/**
	 * Affiche un sélecteur de fichier d'enregistrement de pointages laser.
	 */
	virtual void saveAsRaysCallback ( );

	/**
	 * Utilise le contexte courant lors de la projetection les pointages laser.
	 */
	virtual void setRaysContextCallback ( );

	/**
	 * Projette les pointages laser sur les surfaces et volumes sélectionnés.
	 */
	virtual void setRaysTargetSurfacesCallback ( );

	/**
	 * Affiche un sélecteur de fichier de chargement de diagnostics.
	 */
	virtual void loadDiagsCallback ( );

	/**
	 * Affiche un sélecteur de fichier de chargement de diagnostics
	 * positionné dans la base de données de diagnostics.
	 */
	virtual void importDiagsCallback ( );

	/**
	 * Enregistre les diagnostics dans le fichier courant.
	 */
	virtual void saveDiagsCallback ( );

	/**
	 * Affiche un sélecteur de fichier d'enregistrement de diagnostics.
	 */
	virtual void saveAsDiagsCallback ( );

	//@}	// Callbacks du menu "Chambre expérimentale".

	/**
	 * Affiche un sélecteur de fichier de chargement de pointages laser
	 * positionné dans le répertoire transmis en argument.
	 * Le répertoire est éventuellement modifié par l'utilisateur.
	 * \except	Une exception est levée en cas d'erreur.
	 */
	virtual void loadRays (std::string& dir);

	/**
	 * Affiche un sélecteur de fichier de chargement de diagnostics
	 * positionné dans le répertoire transmis en argument.
	 * Le répertoire est éventuellement modifié par l'utilisateur.
	 * \except	Une exception est levée en cas d'erreur.
	 */
	virtual void loadDiags (std::string& dir);
#endif	// USE_EXPERIMENTAL_ROOM


	private :

	// Opérations interdites :
	QtVtkMgx3DMainWindow (const QtVtkMgx3DMainWindow&);
	QtVtkMgx3DMainWindow& operator = (const QtVtkMgx3DMainWindow&);

#ifdef USE_EXPERIMENTAL_ROOM
	/** Le panneau "Chambre expérimentale. */
	QtVtkMgx3DExperimentalRoomPanel*	_experimentalRoomPanel;
#endif	// USE_EXPERIMENTAL_ROOM
};	// class QtVtkMgx3DMainWindow


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// QT_VTK_MGX3D_MAIN_WINDOW_H
