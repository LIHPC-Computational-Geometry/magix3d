/**
 * \file		QtMgx3DMainWindow.h
 * \author		Charles PIGNEROL
 * \date		29/11/2010
 */
#ifndef QT_MGX3D_MAIN_WINDOW_H
#define QT_MGX3D_MAIN_WINDOW_H

namespace Mgx3D 
{

namespace QtComponents
{
	class QtMgx3DMainWindow;
	class QtMgx3DStateView;
	class QtRepresentationTypesDialog;
}	// namespace QtComponents

}	// namespace Mgx3D

#include "Internal/ContextIfc.h"
#include "Geom/GeomManagerIfc.h"
#include "Topo/TopoManagerIfc.h"
#include "Mesh/MeshManagerIfc.h"
#include "Utils/CommandManagerIfc.h"
#include "Internal/M3DCommandManager.h"
#include "Utils/UndoRedoManager.h"
#include "QtComponents/EntitySeizureManager.h"
#include "QtComponents/Qt3DGraphicalWidget.h"
#include "QtComponents/QtCommandMonitorDialog.h"
#include "QtComponents/QtEntitiesPanel.h"
#include "QtComponents/QtGroupsPanel.h"
#include "QtComponents/QtMgx3DLogsView.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtRepresentationTypesDialog.h"
#include "QtComponents/QtSelectionCommonPropertiesPanel.h"
#include "QtComponents/QtSelectionIndividualPropertiesPanel.h"

#include <TkUtil/File.h>
#include <TkUtil/LogDispatcher.h>
#include <TkUtil/UrlFifo.h>
#include <QtUtil/QtAboutDialog.h>
#include <QtUtil/QtStatusLogOutputStream.h>
#include <QtUtil/QtWebBrowser.h>
#include "QtComponents/QtMgx3DPythonConsole.h"

#include <QTimer>
#include <QAction>
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QTabWidget>
#include <QToolBar>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Action qui s'inactive automatiquement lorsqu'elle est sollicitée
 * (objectif : <I>one shoot</I>, c'est à dire que pour 2 clics successifs et
 * rapprochés seul le premier sera pris en compte). Dispositif utilisé pour
 * interdire du clic en rafale sur I>undo/redo</I>.
 */
class QtAutoDisablabledAction : public QAction
{
	Q_OBJECT

    public :

	QtAutoDisablabledAction (const QString& label, QObject* parent);
	QtAutoDisablabledAction (const QIcon& icon, const QString& label, QObject* parent);


    public slots :

	virtual void disable ( );
};  // class QtAutoDisablabledAction


/**
 * \brief		Classe de fenêtre principale de l'application <I>Magix 3D</I>.
 *
 * Cette classe gère l'ensemble des actions possibles au niveau d'une fenêtre
 * principale de l'application <I>Magix 3D</I>.
 */
class QtMgx3DMainWindow :
				public QMainWindow, public TkUtil::ObjectBase,
				public Mgx3D::Utils::SelectionManagerObserver
{
	Q_OBJECT

	public :

	/**
	 * Appeler <B>init</B> pour créer le panneau.
	 * \param	Eventuelle fenêtre parente
	 * \param	Nom Qt de la fenêtre
	 * \param	Paramètres Qt de la fenêtre
	 * \see		init
	 */
	 QtMgx3DMainWindow (QWidget* parent = 0, char* name = 0, Qt::WindowFlags flags = 0);
	
	/**
	 * RAS.
	 */
	virtual ~QtMgx3DMainWindow ( );

	/**
	 * Divers IHM.
	 */
	//@{

	/**
	 * Affiche la fenêtre, menu actualisé. Initialise le graphique (Open GL) si nécessaire.
	 * \warning	A utiliser à la place de <I>show</I>.
	 */
	 virtual void showReady ( );
	 
	/**
	 * Appelle createGui.
	 * \param	Nom du context éventuellement créé
	 * \param	Contexte <I>Magix 3D</I> de la session.
	 * \param	Eventuel widget"arbre" contenant les groupes d'entités de la session.
	 * \param	Eventuel widget"arbre" contenant les entités de la session.
	 * \see		createGui
	 */
	virtual void init (const std::string& name, Mgx3D::Internal::ContextIfc* context,
				QtComponents::QtGroupsPanel* groupsPanel = 0, QtComponents::QtEntitiesPanel* entitiesPanel = 0);

	/**
	 * Enregistre les paramètres d'affichage (taille, position, ...) de cette
	 * fenêtre.
	 * \see		editSettingsCallback
	 * \see		readSettings
	 * \see		closeEvent
	 */
	virtual void writeSettings ( );

	/**
	 * Lit et s'applique les paramètres d'affichage (taille, position, ...) de
	 * cette fenêtre.
	 * \see		editSettingsCallback
	 * \see		writeSettings
	 */
	virtual void readSettings ( );

	/**
	 * Créé une boite de dialogue d'édition des propriétés d'affichage des
	 * entités.
	 * \param	Nom du panneau.
	 * \param	Entités soumises aux modifications.
	 * \param   URL de l'aide
	 * \param   Tag de l'aide dans la page précédemment donnée
	 * \return	La boite de dialogue créée, non modale, qui sera détruite lors
	 *			de sa fermeture.
	 */
	virtual QtRepresentationTypesDialog* createRepresentationTypesDialog (
			const std::string& name, 
			const std::vector<Mgx3D::Utils::Entity*>& entities,
			const std::string& helpURL, const std::string& helpTag
		);

	//@}	// Diverses opérations.


	protected slots :

	/**
	 * Callbacks du menu Projet.
	 */
	//@{

	/**
	 * Permet de modifier les préférences utilisateur.
	 */
	virtual void preferencesCallback ( );

	/**
	 * Permet de modifier les préférences de l'utilisateur liées à l'état de
	 * l'IHM.
	 * Les sauvegareds de manière persistante, mais ne sauvegarde pas d'autres
	 * modifications qui auraient pu être faites préalablement mais pas de
	 * manière persistante (seules les modifications liées à l'état de l'IHM
	 * sont susceptibles d'être sauvegardées).
	 */
	virtual void editSettingsCallback ( );

	/**
	 * Met fin de l'application.
	 */
	virtual void exitCallback ( );

	//@}

	/**
	 * Callbacks du menu Session.
	 */
	//@{

	/**
	 * Annule la dernière commande.
	 */
	virtual void undoCallback ( );

	/**
	 * Rejoue la dernière commande.
	 */
	virtual void redoCallback ( );

	/**
	 * Réinitialise la session en cours.
	 */
	virtual void reinitializeCallback ( );

	/**
	 * Modifient l'unité du système métrique.
	 */
	virtual void unitMeterCallback ( );
	virtual void unitCentimeterCallback ( );
	virtual void unitMillimeterCallback ( );
	virtual void unitMicrometerCallback ( );
	virtual void unitUndefinedCallback ( );

	/**
	 * Modifient le repère utilisé.
	 */
	virtual void meshLandmarkCallback ( );
	virtual void expRoomLandmarkCallback ( );
	virtual void undefinedLandmarkCallback ( );

	/**
	 * Modifient la dimension du maillage en sortie.
	 */
	virtual void mesh2DDimCallback();

	/**
	 * Importe CAO/Maillage depuis un fichier.
	 */
	virtual void importCallback ( );

	/**
	 * Exporte CAO/Maillage vers un fichier.
	 */
	virtual void exportAllCallback ( );
	virtual void exportSelectionCallback ( );

	/**
	 * Enregistre CAO/Maillage vers un fichier Magix 3D python minimal.
	 */
	virtual void saveMagix3DScriptCallback ( );
	virtual void saveAsMagix3DScriptCallback ( );

	/**
	 * Exécute une commande longue, a priori dans un autre thread.
	 * \warning	Pour tests uniquement.
	 */
	virtual void longCommandCallback ( );

	/**
	 * Exécute une commande longue et non interruptible,
	 * a priori dans un autre thread.
	 * \warning	Pour tests uniquement.
	 */
	virtual void openCascadeLongCommandCallback ( );

	/**
	 * Propose à l'utilisateur un sélecteur de fichiers et exécute le
	 * fichier sélectionné dans la session <I>python</I> courante.
	 * \see		executePythonScript
	 */
	virtual void executePythonScriptCallback ( );

	//@}	// Divers IHM


	public :

	/**
	 * La vue textuelle.
	 */
	//@{
	//
	/**
	 * \return		Une référence sur le panneau de gestion des entités.
	 */
	QtEntitiesPanel& getEntitiesPanel ( );
	const QtEntitiesPanel& getEntitiesPanel ( ) const;

	/**
	 * \return		Une référence sur le panneau de gestion des groupes
	 *				d'entités.
	 */
	QtGroupsPanel& getGroupsPanel ( );
	const QtGroupsPanel& getGroupsPanel ( ) const;

	//@}	// La vue textuelle.

	/**
	 * La vue graphique.
	 */
	//@{

	/**
	 * @param		La vue graphique 3D à prendre en charge.
	 * @exception	Une exception est levée si une vue graphique est déjà gérée
	 *				par l'instance.
	 */
	virtual void setGraphicalWidget (Qt3DGraphicalWidget& widget);

	/**
	 * \return		Une référence sur la vue graphique 3D.
	 */
	virtual Qt3DGraphicalWidget& getGraphicalWidget ( );
	virtual const Qt3DGraphicalWidget& getGraphicalWidget ( ) const;

	/**
	 * Affiche une boite de dialogue amodale permettant d'éditer les types de
	 * représentation graphique des entités transmises en argument.
	 * \see			createRepresentationTypesDialog
	 */
	virtual void changeRepresentationTypes (const std::vector<Mgx3D::Utils::Entity*>& entities);

	//@}	// La vue graphique.

	/**
	 * \return		Le titre de l'application pour d'éventuels messages
	 *				d'erreur.
	 * \see			setAppTitle
	 */
	virtual const std::string& getAppTitle ( ) const
	{ return _appTitle; }

	/**
	 * \param		Le titre de l'application pour d'éventuels messages
	 *				d'erreur.
	 * \see			getAppTitle
	 */
	virtual void setAppTitle (const std::string& title)
	{ _appTitle	= title; }

	/**
	 * \return		Les composants pour la boite de dialogue "A propos de ...".
	 */
	virtual std::vector <QtAboutDialog::Component> getAboutDlgComponents ( ) const;

	/**
	 * Structure comprenant les <I>actions</I> Qt exécutables depuis cette
	 * fenêtre.
	 */
	struct WindowsActions
	{
		WindowsActions ( );
		WindowsActions (const WindowsActions&);
		WindowsActions& operator = (const WindowsActions&);
		virtual ~WindowsActions ( );
		virtual void setEnabled (bool enable);
		QAction			*_undoAction, *_redoAction, *_clearAction, *_zoomViewAction;
		QAction			*_meterAction, *_centimeterAction, *_millimeterAction,
						*_micrometerAction, *_undefinedUnitAction;
		QAction			*_meshLandmarkAction, 
						*_expRoomLandmarkAction,
						*_undefinedLandmarkAction;
		QAction         *_mesh2DDimAction;
		QAction			*_importAction,
						*_exportAllAction, *_exportSelectionAction,
						*_saveMagix3DScriptAction, *_saveAsMagix3DScriptAction;
		QAction			*_longCommandAction, *_openCascadeLongCommandAction;
		QAction			*_executePythonScriptAction;
		QAction			*_preferencesAction, *_editSettingsAction, *_quitAction;
		QAction			*_print3DViewAction, *_print3DViewToFileAction;
		QAction			*_useGlobalDisplayPropertiesAction;
		QAction			*_displayTrihedronAction, *_displayLandmarkAction,
						*_parametrizeLandmarkAction, *_displayFocalPointAction,
						*_xOyViewAction, *_xOzViewAction, *_yOzViewAction,
						*_resetViewAction, *_clearViewAction, *_parallelViewAction;
#ifdef USE_EXPERIMENTAL_ROOM						
		QAction			*_loadRaysAction, *_importRaysAction,
						*_saveRaysAction, *_saveAsRaysAction,
						*_setRaysContextAction, *_setRaysTargetSurfacesAction;
		QAction			*_loadDiagsAction, *_importDiagsAction,
						*_saveDiagsAction, *_saveAsDiagsAction;
#endif	// USE_EXPERIMENTAL_ROOM
		QAction			*_displayLookupTableAction,
						*_displayLookupTableEditorAction;
		QAction			*_distanceMeasurementAction, *_angleMeasurementAction, *_extremaMeshingEdgeLengthOnEdgeAction;
		QAction			*_topoOptimMeshMethod;
		QAction			*_topoInformationAction, *_meshInformationAction;
		QAction			*_topoRefineAction,
						*_topoSetDefaultNbMeshingEdgesAction;
        QAction         *_topoNew3x3BoxesWithTopoAction;
		QAction			*_meshSelectionAction, *_meshVisibleAction,
						*_meshAllAction;
		QAction			*_unrefineMeshRepresentationAction;
		QAction			*_addMeshExplorerAction;
		QAction			*_addMeshQualityAction;
		QAction			*_displaySelectedGroupsAction,
						*_hideSelectedGroupsAction,
						*_displaySelectedTypesAction,
						*_hideSelectedTypesAction,
						*_displaySelectedEntitiesAction,
						*_hideSelectedEntitiesAction,
						*_displaySelectionPropertiesAction,
						*_displayComputableSelectionPropertiesAction,
						*_selectionRepresentationAction;
		QAction			*_selectEntitiesAction;
		QAction			*_selectVisibleEntitiesAction,
		                *_unselectVisibleEntitiesAction;
		QAction			*_selectFusableEdgesAction,
						*_unselectFusableEdgesAction,
						*_selectInvalidEdges,
						*_unselectInvalidEdges;
		QAction			*_selectBorderFacesAction, *_unselectBorderFacesAction;
		QAction			*_selectFacesWithoutBlockAction, *_unselectFacesWithoutBlockAction;
		QAction			*_selectSemiConformFacesAction,
						*_unselectSemiConformFacesAction;
		QAction			*_selectInvalidFaces,
		                *_unselectInvalidFaces,
						*_selectUnstructuredFaces,
						*_unselectUnstructuredFaces,
						*_selectTransfiniteFaces,
						*_unselectTransfiniteFaces;
		QAction			*_selectInvalidBlocks,
		                *_unselectInvalidBlocks,
						*_selectUnstructuredBlocks,
						*_unselectUnstructuredBlocks,
						*_selectTransfiniteBlocks,
						*_unselectTransfiniteBlocks;
		QAction			*_selectNodesAction, *_selectEdgesAction,
						*_selectSurfacesAction, *_selectVolumesAction;
		QAction			*_selectionModeAction;
		QAction			*_showCommandMonitorDialogAction;
		QAction			*_displayUsersGuideAction,
						*_displayUsersGuideContextAction,
						*_displayWikiAction,
						*_displayTutorialAction,
						*_displayPythonAPIUsersGuideAction,
						*_displayQualifAction,
						*_displayShortKeyAction,
						*_displaySelectAction,
						*_displayHistoriqueAction,
						*_aboutDialogAction;
		QMenu			*_mgx3DScriptsMenu;
		size_t			_recentFilesCapacity;
		QAction			**_mgx3DScriptsActions;
		Utils::UndoRedoManagerIfc*  _undoManager;
	};	// struct WindowsActions

	/**
	 * \return		Les actions exécutables depuis cette fenêtre.
	 */
	virtual WindowsActions& getActions ( )
	{ return _actions; }
	virtual const WindowsActions& getActions ( ) const
	{ return _actions; }

	/**
	 * Les opérations.
	 */
	//@{

	/** Le mécanisme utilisé par défaut par les callbacks <I>Qt</I>. */
	static Qt::ConnectionType	defaultConnectionType;

	/**
	 * Recense l'action transmise en argument dans la barre d'outils
	 * représentative du type transmis en second argument.
	 * Cette action est adoptée, et sa destruction est donc prise en charge
	 * par cette instance.
	 */
	virtual void registerOperationAction (
			Mgx3D::QtComponents::QtMgx3DOperationAction& action,
			Mgx3D::QtComponents::QtMgx3DOperationsPanel::OPERATION_TYPES type);

	/**
	 * \return		Une référence sur le panneau de gestion des opérations.
	 */
	QtMgx3DOperationsPanel& getOperationsPanel ( );
	const QtMgx3DOperationsPanel& getOperationsPanel ( ) const;

	/**
	 * Enregistre le panneau d'opération transmis en argument. Ce panneau doit
	 * être un panneau supplémentaire, non géré automatiquement via le l'API
	 * <I>registerOperationAction</I>, et ne pas avoir de parent.
	 * Informe ce panneau de la destruction de la fenêtre principale si
	 * nécessaire.
	 * @see		unregisterAdditionalOperationPanel
	 */
	virtual void registerAdditionalOperationPanel (QtMgx3DOperationPanel& panel);

	/**
	 * Désenregistre le panneau d'opération additionnel transmis en argument.
	 * @see		registerAdditionalOperationPanel
	 */
	virtual void unregisterAdditionalOperationPanel (QtMgx3DOperationPanel& panel);

	/**
	 * Forcer l'inactivation des menus, actions et opérations.
	 */
	virtual bool actionsDisabled ( ) const;
	virtual bool disableActions (bool);

	//@}	// Les opérations.

	/**
	 * Méthodes relatives au suivi des <I>observables</I>.
	 */
	//@{

	/**
	 * Actualise l'IHM conformément à la modification de l'observable
	 * tranmis en argument.
	 */
	virtual void observableModified (TkUtil::ReferencedObject* object, unsigned long event);

	//@}

	/**
	 * La session de travail.
	 */
	//@{

	/**
	 * \param		Le nouveau contexte de la session de travail. Invoque
	 *			registerToManagers
	 * \warning		<B>Ce nouveau contexte est adopté par l'instance</B>,
	 *			qui se chargera de ce fait de sa destruction.
	 * \see			registerToManagers
	 */
	virtual void setContext (Mgx3D::Internal::ContextIfc* context);

	/**
	 * \return		Le contexte de la session de travail.
	 */
	virtual Mgx3D::Internal::ContextIfc& getContext ( );
	virtual const Mgx3D::Internal::ContextIfc& getContext ( ) const;

	/**
	 * \return		La console python.
	 */
	virtual QtMgx3DPythonConsole* getPythonPanel ( )
	{ return _pythonPanel; }

	/**
	 * Exécute le script python transmis en argument.
	 * \exception		Une exception est levée en cas d'erreur.
	 * \warning		N'inactive ni ne réactive rien (menus, affichages
	 * 			graphiques, ...). Ces opérations sont à la charge de
	 * 			l'appelant.
	 */
	virtual void executePythonScript (const std::string& file);

	/**
	 * \return		Le gestionnaire de commandes.
	 */
	virtual Mgx3D::Utils::CommandManagerIfc& getCommandManager ( );

	/**
	 * Affiche une fenêtre informant du changement de version
	 * et donne un lien vers l'historique
	 */
	virtual void infoNewVersion(const TkUtil::Version& lastVersion, const TkUtil::Version& actualVersion);



	/**
	 * La gestion de la sélection.
	 */
	//@{
	//

    /**
     * Informe l'éventuel élément d'IHM observateur de sélection de la
     * reinitialisation de la sélection interactive.
     * \see     setEntitySeizureManager
     */
    virtual void selectionCleared ( );

	/**
	 * Informe l'éventuel élément d'IHM observateur de sélection de la
	 * sélection interactive d'une nouvelle entité.
	 * \see		setEntitySeizureManager
	 */
	virtual void entitiesAddedToSelection (const std::vector<Mgx3D::Utils::Entity*>& entities);

    /**
     * Informe l'éventuel élément d'IHM observateur de sélection de la
     * desélection interactive d'une nouvelle entité.
	 * @param	Entités enlevées de la sélection
	 * @param	<I>true</I> s'il s'agit d'un <I>clearSelection</I>, à des fins
	 * 			d'optimisation.
     * \see     setEntitySeizureManager
     */
    virtual void entitiesRemovedFromSelection (
				const std::vector<Mgx3D::Utils::Entity*>& entities, bool clear);

	/**
	 * \return	Elément d'IHM observateur de sélection de la sélection
	 * 			interactive.
	 * \see		unregisterEntitySeizureManager
	 * \see		setEntitySeizureManager
	 */
	virtual Mgx3D::QtComponents::EntitySeizureManager* getEntitySeizureManager ( );

	/**
	 * \param	Elément d'IHM observateur de sélection de la sélection
	 * 			interactive.
	 * \see		unregisterEntitySeizureManager
	 * \see		getEntitySeizureManager
	 */
	virtual void setEntitySeizureManager (Mgx3D::QtComponents::EntitySeizureManager* seizureManager);

	/**
	 * Déréférence le sélecteur interactif d'entités.
	 */
	virtual void unregisterEntitySeizureManager (Mgx3D::QtComponents::EntitySeizureManager* seizureManager);

	//@}	// La gestion de la sélection.

	/**
	 * \return		Le gestionnaire de modélisation.
	 */
	virtual Mgx3D::Geom::GeomManagerIfc& getGeomManager ( );

    /**
     * \return      Le gestionnaire de topologie.
     */
    virtual Mgx3D::Topo::TopoManagerIfc& getTopoManager ( );

    /**
     * \return      Le gestionnaire de maillage.
     */
    virtual Mgx3D::Mesh::MeshManagerIfc& getMeshManager ( );

    /**
     * \return      Le gestionnaire de groupe d'entités.
     */
    virtual Mgx3D::Group::GroupManagerIfc& getGroupManager ( );

    /**
     * \return      Le gestionnaire de commandes composites.
     */
    virtual Mgx3D::Internal::M3DCommandManager& getM3DCommandManager ( );

	/**
	 * \return		Le gestionnaire de <I>undo/redo</I>.
	 */
	virtual Mgx3D::Utils::UndoRedoManagerIfc& getUndoManager ( );

	//@}	// La session de travail.


	signals :

	/**
	 * Les signaux émis.
	 */
	//@{

	/**
	 * Signal émis lorsque l'instance est informée de la modification
	 * d'un objet observé dans un thread autre que celui de l'IHM.
	 * Ce cas de figure se produit notamment lorsqu'une commande n'est pas
	 * exécutée séquentiellement.
	 */
	void nonGuiThreadObservableModified (TkUtil::ReferencedObject* o, unsigned long event);

	//@}	// Les signaux émis.


	protected :

	/**
	 * Méthodes de création de l'IHM.
	 */
	//@{

	/**
	 * Création de l'IHM (aspects purements Qt). Appelle createActions,
	 * createOperations puis createMenu.
	 * \see		createMenu
	 * \see		createToolbars
	 * \see		createActions
	 * \see		createOperations
	 */
	virtual void createGui ( );

	/**
	 * Création du menu.
	 * @warning		Les actions doivent être préalablement créés.
	 * \see			createActions
	 * \see			createToolbars
	 */
	virtual void createMenu ( );

	/**
	 * Création des barres d'outils.
	 * @warning		Les actions doivent être préalablement créés.
	 * \see			createActions
	 * \see			createMenu
	 */
	virtual void createToolbars ( );

	/**
	 * Création des actions accessibles depuis cette fenêtre.
	 * \see			getActions
	 * \see			createMenu
	 * \see			createToolbars
	 * \see			WindowsActions
	 */
	virtual void createActions ( );

	/**
	 * Création des actions paramétrables depuis un panneau dans la partie
	 * "opérations" de l'IHM et accessibles depuis cette fenêtre.
	 * \see			createActions
	 * \see			createMenu
	 * \see			createToolbars
	 * \see			registerOperationAction
	 */
	virtual void createOperations ( );

#ifdef USE_EXPERIMENTAL_ROOM
	/**
	 * \return	L'éventuel panneau "Pointages laser".
	 * \warning	Retourne 0 par défaut, méthode à surcharger.
	 */
	virtual QWidget* getExperimentalRoomPanel ( );

	/**
	 * <P>Affecte le widget "Pointages laser". Ce widget sera détruit lors de
	 * la fermeture de la main window, avant sa destruction.</P>
	 * \see			getExperimentalRoomPanel
	 * \warning		Détruit l'éventuel ancien panneau existant
	 */
	virtual void setExperimentalRoomPanel (QWidget*);

	/**
	 * @return		Le menu Chambre expérimentale.
	 */
	virtual QMenu* getRoomMenu ( )
	{ return _roomMenu; }
#endif	// USE_EXPERIMENTAL_ROOM

	/**
	 * Initialise les aspects <I>scripting python</I>.
	 */
	virtual void initPythonScripting ( );

	/**
	 * <P>Méthode appelée par <I>Qt</I> en temps utile.
	 * Invoque <I>updateActions</I>.</P>
	 * <P>Cette méthode a pour objectifs :<BR>
	 * <OL>
	 * <LI>Réactiver les actions (menus) que si le contexte le permet,
	 * <LI>Evaluer fréquement si cette réactivation est nécessaire
	 * </OL>
	 * </P>
	 * <P>En effet, de part la possibilité de gestion de files d'attentes de
	 * commandes, de <I>main loops</I> imbriquées pour effectuer des
	 * raffraichissements d'écran, la détermination du moment opportun pour
	 * réactiver les actions devient très délicate.
	 * </P>
	 * \see		updateActions
	 */
	virtual void timerEvent (QTimerEvent* event);

	/**
	 * Mise à jour des actions (actif/inactif, libellés, ...) si le contexte le
	 * permet.
	 */
	virtual void updateActions ( );

	/**
	 * Invoque writeSettings.
	 */
	virtual void closeEvent (QCloseEvent* event);

	//@}	// Méthodes de création de l'IHM.

	/**
	 * Les objets observés.
	 */
	//@{

	/**
	 * S'enregistre auprès des gestionnaires d'objets pour être informé
	 * des modifications des objets.
	 */
	virtual void registerToManagers ( );

	/**
	 * Appelé lorsqu'une commande est modifiée. Effectue les traitements
	 * de mise à jour de l'IHM spécifique aux commandes.
	 * \see		observableModified
	 */
	virtual void commandModified (Mgx3D::Utils::Command& command, unsigned long event);

	/**
	 * Appelé lorsque l'unité de longueur du contexte est modifiée. Actualise
	 * l'IHM.
	 */
	virtual void lengthUnitModifiedEvent ( );

	/**
	 * Appelé lorsque l'unité de longueur du contexte est modifiée. Actualise
	 * l'IHM.
	 */
	virtual void landmarkModifiedEvent ( );

	//@}	// Les objets observés.

	/**
	 * La gestion de l'affichage des informations relatives au déroulement de la
	 * session.
	 */
	//@{

	/**
	 * Transmet le log reçu en argument aux flux sortants de logs utilisés
	 * pour afficher les informations relatives au déroulement de la session.
	 */
	 virtual void log (const TkUtil::Log& log);

	/**
	 * \return		Le gestionnaire de flux sortants utilisés pour afficher les
	 *				informations relatives au déroulement de la session.
	 */
	virtual TkUtil::LogDispatcher& getLogDispatcher ( );
	virtual const TkUtil::LogDispatcher& getLogDispatcher ( ) const;

	/**
	 * \return		Le fenêtre texte utilisée pour afficher les informations
	 *				relatives au déroulement de la session.	
	 */
	virtual QtLogsView& getLogView ( );
	virtual const QtLogsView& getLogView ( ) const;

	//@}	// La gestion des logs

	/**
	 * La gestion des menus et actions.
	 */
	//@{

	virtual QMenu& getProjectMenu ( );
	virtual const QMenu& getProjectMenu ( ) const;
	virtual QMenu& getSessionMenu ( );
	virtual const QMenu& getSessionMenu ( ) const;
	virtual QMenu& getViewMenu ( );
	virtual const QMenu& getViewMenu ( ) const;
	virtual QMenu& getCADMenu ( );
	virtual const QMenu& getCADMenu ( ) const;
	virtual QMenu& getTopologyMenu ( );
	virtual const QMenu& getTopologyMenu ( ) const;
	virtual QMenu& getMeshingMenu ( );
	virtual const QMenu& getMeshingMenu ( ) const;
	virtual QMenu& getToolsMenu ( );
	virtual const QMenu& getToolsMenu ( ) const;
	virtual QMenu& getHelpMenu ( );
	virtual const QMenu& getHelpMenu ( ) const;

	//@}	// La gestion des menus et actions.

	/**
	 * La gestion du fenêtrage et des évènements.
	 */
	//@{

	/**
	 * Actualise l'IHM en vidant la boucle évènementielle. A invoquer dans des
	 * boucles ou tâches longues. Permet par exemple à l'utilisateur, lors d'une
	 * tâche longue, de modifier la vue 3D et de la voir s'actualiser.
	 * \see			QtMgx3DApplication::flushEventQueue
	 */
	virtual void refreshGui ( );

	/**
	 * Appelé lors de redimensionnements de la fenêtre. Détermine la taille
	 * maximale acceptable au premier évènement possible et interdit que la
	 * fenêtre ne devienne plus grande.
	 */
	virtual void resizeEvent (QResizeEvent*);

	//@}	// La gestion du fenêtrage et des évènements.

	/**
	 * Diverses opérations.
	 */
	//@{

	/** Modifie le repère utilisé du contexte.
	 * \warning		N'actualise pas l'IHM.
	 */
	virtual void setLandmark (Utils::Landmark::kind);

	/**
	 * Demande à l'utilisateur de renseigner l'unité de longueur à utiliser et applique ce choix.
	 * @param		Unité de longueur suggérée
	 * @return		<I>true</I> si l'utilisateur a appliqué une unité de longueur, <I>false</I> dans le cas contraire.
	 */
	virtual bool requireUnitMeter (Mgx3D::Utils::Unit::lengthUnit suggested);

	/** Modifie le système d'unité du contexte.
	 * \warning		N'actualise pas l'IHM.
	 */
	virtual void setUnitMeter (Mgx3D::Utils::Unit::lengthUnit);

	/** Modifie la dimension en sortie pour le maillage pour ce contexte.
	 * \warning		N'actualise pas l'IHM.
	 */
	virtual void setMesh2D();

	/**
	 * Demander à l'utilisateur de sélectionner un (et un seul) fichier.
	 * \param		Widget parent
	 * \param		Répertoire par défaut
	 * \param		Filtre appliqué
	 * \param		Titre de la boite de dialogue
	 * \param		Type de fichier sélectionnable
	 * \param		Type d'ouverture
	 * \return		Le fichier sélectionné, de path nul si l'opération est
	 * 				annulée
	 */
	virtual TkUtil::File selectFileName (
				QWidget* parent, const std::string& dir,
				const std::string& filter, const std::string& title,
				QFileDialog::FileMode fm = QFileDialog::ExistingFile,
				QFileDialog::AcceptMode am = QFileDialog::AcceptOpen);

	/**
	 * @return	Le jeu de caractères à utiliser par défaut lors de l'enregistrement de
	 *		scripts.
	 */
	virtual TkUtil::Charset getDefaultScriptsCharset ( ) const;

	/**
	 * @return	Le jeu de caractères à utiliser par défaut lors de l'enregistrement de
	 *		lasers ou diagnostics.
	 * @warning	A surcharger, retourne Charset::UTF_8.
	 */
	virtual TkUtil::Charset getDefaultRaysCharset ( ) const;

	//@}	// Diverses opérations

	public  slots :

	/**
	 * Callbacks publics du menu sélection.
	 */
	//@{

	/**
	 * Affiche une boite de dialogue non modale de modification des paramètres
	 * d'affichage de la sélection en cours.
	 * \see		changeRepresentationTypes
	 * \see		representationTypesDialogClosedCallback
	 */
	virtual void showRepresentationTypesCallback ( );

	/**
	 * Affiche une boite de dialogue de modification des paramètres
	 * d'affichage de la sélection.
	 */
	virtual void representationTypesDialogClosedCallback ( );

	//@}	// Callbacks publics du menu sélection.


	protected slots :

	virtual void executeRecentMgx3DScriptCallback ( );

	/**
	 * Exécute le script <I>Magix 3D</I> sélectionné par l'utilisateur parmi
	 * une liste de scripts récents.
	 */

	/**
	 * Callbacks du menu Vue
	 */
	//@{

	/**
	 * Affiche une boite de dialogue d'impression de la vue 3D.
	 */
	virtual void print3DViewCallback ( );

	/**
	 * Affiche une boite de dialogue d'impression dans un fichier de la vue 3D.
	 */
	virtual void print3DViewToFileCallback ( );

	/**
	 * Passe/Quitte le mode d'utilisation des propriétés d'affichage global.
	 */
	virtual void useGlobalDisplayPropertiesCallback (bool global);

	/**
	 * Affiche/masque le trièdre.
	 */
	virtual void displayTrihedronCallback (bool display);

	/**
	 * Affiche/masque le repère.
	 */
	virtual void displayLandmarkCallback (bool display);

	/**
	 * Affiche une boite de dialogue de paramétrage du repère.
	 */
	virtual void parametrizeLandmarkCallback ( );

	/**
	 * Affiche/masque la croix matérialisant le point focal.
	 */
	virtual void displayFocalPointCrossCallback (bool display);

	/**
	 * Positionne la vue respectivement dans le plan <I>xOy</I>, <I>xOz</I>,
	 * <I>yOz</I>.
	 */
	virtual void xOyViewPlaneCallback ( );
	virtual void xOzViewPlaneCallback ( );
	virtual void yOzViewPlaneCallback ( );

	/**
	 * Recadre la vue.
	 */
	virtual void resetViewCallback ( );

	/**
	 * Masque toutes les entités de la vue
	 */
	virtual void clearViewCallback ( );

	/**
	 * Demande à l'utilisateur de saisir le facteur de zoom (action sur l'angle entre les yeux) et effectue le zoom.
	 */
	virtual void zoomViewCallback ( );

	/**
	 * Passage mode de projection perspective <-> parallèle.
	 */
	virtual void parallelProjectionCallback (bool);

	/**
	 * Affiche la table de couleurs utilisée pour la représentation des valeurs
	 * aux noeuds/mailles.
	 */
	virtual void displayLookupTableCallback ( );

	/**
	 * Affiche l'éditeur de tables de couleurs
	 */
	virtual void displayLookupTableEditorCallback ( );

	//@}	// Callbacks du menu Vue

	/**
	 * Callbacks du menu CAO.
	 */
	//@{

	/**
	 * Affiche un onglet de mesure de distance entre entités CAO.
	 */
	virtual void addDistanceMeasurementCallback ( );

	/**
	 * Affiche un onglet de mesure d'angles entre 3 points.
	 */
	virtual void addAngleMeasurementCallback ( );

	/**
	 * Affiche un onglet d'information sur les longueurs des bras aux extrémités d'une arête.
	 */
	virtual void addExtremaMeshingEdgeLengthOnEdgeCallback ( );

	//@}

    /**
     * Callbacks du menu Topologie.
     */
    //@{

	/**
	 * Affiche une boite de dialogue de raffinement de la discrétisation des
	 * arêtes.
	 */
	virtual void refineTopologyCallback ( );

	/**
	 * Affiche une boite de dialogue de saisie du nombre d'arêtes par défaut
	 * lors de la discrétisation de la topologie en maillage.
	 */
	virtual void defaultNbMeshingEdgesCallback ( );

    /**
     * Création d'une grille de 3x3 boites
     * et de leurs blocs conformes,
     * alternativement structurés/non-structurés.
     */
    virtual void new3x3BoxesWithTopoCallback ();

    /**
     * Remplace la méthode de maillage Transfini par une méthode plus rapide du type directionnelle.
     */
    virtual void optimMeshMethodCallback ();

    /**
     * Affiche une nouvelle fenêtre "Information sur la topologie".
     */
    virtual void addTopoInformationCallback ( );

    //@}

    /**
     * Callbacks du menu Maillage.
     */
    //@{

	/**
	 * Maille la sélection.
	 */
	virtual void meshSelectionCallback ( );

	/**
	 * Maille ce qui est visible.
	 */
	virtual void meshVisibleCallback ( );

	/**
	 * Maille tout.
	 */
	virtual void meshAllCallback ( );

    /**
     * Affiche une Boite de dialogue de déraffinement de la représentation des
     * maillages.
     */
    virtual void unrefineMeshRepresentationCallback ( );

    /**
     * Affiche un nouvel explorateur de feuillets.
     */
    virtual void addMeshExplorerCallback ( );

    /**
     * Affiche une nouvelle fenêtre "Qualité de maillage".
     */
    virtual void addMeshQualityCallback ( );

    /**
     * Affiche une nouvelle fenêtre "Information sur le maillage".
     */
    virtual void addMeshInformationCallback ( );

    //@}

	/**
	 * Callbacks du menu "Sélection".
	 */
	//@{

	/**
	 * Affiche/masque les groupes/types de la sélection.
	 */
	virtual void displaySelectedGroupsCallback ( );
	virtual void hideSelectedGroupsCallback ( );
	virtual void displaySelectedTypesCallback ( );
	virtual void hideSelectedTypesCallback ( );

	/**
	 * Affiche/masque les entités de la sélection.
	 */
	virtual void displaySelectedEntitiesCallback ( );
	virtual void hideSelectedEntitiesCallback ( );

	/**
	 * Affiche ou non les propriétés (toutes ou non calculées) de la sélection.
	 */
	virtual void displaySelectedEntitiesPropertiesCallback ( );
	virtual void displaySelectedEntitiesComputablePropertiesCallback ( );

	/**
	 * Permet à l'utilisateur de sélectionner des entités en renseignant leurs
	 * identifiants.
	 */
	virtual void selectEntitiesCallback ( );

	/**
	 * (Dé)Sélectionne les entités visibles.
	 */
	virtual void selectVisibleEntitiesCallback ( );
	virtual void unselectVisibleEntitiesCallback ( );

	/**
	 * (Dé)Sélectionne les arêtes fusionnables.
	 */
	virtual void selectFusableEdgesCallback ( );
	virtual void unselectFusableEdgesCallback ( );

	/**
	 * (Dé)Sélectionne les arêtes invalides.
	 */
	virtual void selectInvalidEdgesCallback ( );
	virtual void unselectInvalidEdgesCallback ( );

	/**
	 * (Dé)Sélectionne les faces aux bords.
	 */
	virtual void selectBorderFacesCallback ( );
	virtual void unselectBorderFacesCallback ( );

	/**
	 * (Dé)Sélectionne les faces sans blocs.
	 */
	virtual void selectFacesWithoutBlockCallback ( );
	virtual void unselectFacesWithoutBlockCallback ( );

	/**
	 * (Dé)Sélectionne les faces semi-conformes.
	 */
	virtual void selectSemiConformFacesCallback ( );
	virtual void unselectSemiConformFacesCallback ( );

	/**
	 * (Dé)Sélectionne les faces invalides.
	 */
	virtual void selectInvalidFacesCallback ( );
	virtual void unselectInvalidFacesCallback ( );

	/**
	 * (Dé)Sélectionne les faces non-structurées.
	 */
	virtual void selectUnstructuredFacesCallback ( );
	virtual void unselectUnstructuredFacesCallback ( );

	/**
	 * (Dé)Sélectionne les faces avec méthode transfinie.
	 */
	virtual void selectTransfiniteFacesCallback ( );
	virtual void unselectTransfiniteFacesCallback ( );

	/**
	 * (Dé)Sélectionne les blocs invalides.
	 */
	virtual void selectInvalidBlocksCallback ( );
	virtual void unselectInvalidBlocksCallback ( );

	/**
	 * (Dé)Sélectionne les blocs non-structurés.
	 */
	virtual void selectUnstructuredBlocksCallback ( );
	virtual void unselectUnstructuredBlocksCallback ( );

	/**
	 * (Dé)Sélectionne les blocs avec méthode transfinie.
	 */
	virtual void selectTransfiniteBlocksCallback ( );
	virtual void unselectTransfiniteBlocksCallback ( );


	/**
	 * <P>Filtres sur la dimension des entités sélectionnées. Propagent le
	 * l'état en cours au gestionnaire de sélection.</P>
	 * <P>Quatre méthodes sont ici mises en place, une par dimension, afin
	 * de pouvoir facilement changer de politique, que ce soit au niveau des
	 * développements ou au niveau de l'utilsation. On peut par exemple
	 * vouloir :<BR>
	 * <UL>
	 * <LI>que ces callbacks contribuent à l'actualisation d'une sélection,
	 * <LI>que ces callbacks contribuent à l'établissement d'un filtre de
	 * sélection ( ex : 0D et 1D sélectionnables, pas les autres), mais pas
	 * qu'ils déclenchent directement une modification de la sélection,
	 * </UL>
	 * Ce choix est remonté au niveau du gestionnaire de sélection qui est
	 * supposé interchangeable.
	 * </P>
	 */
	virtual void selectNodesCallback (bool enabled);
	virtual void selectEdgesCallback (bool enabled);
	virtual void selectSurfacesCallback (bool enabled);
	virtual void selectVolumesCallback (bool enabled);

	/**
	 * Callback sur le mode de sélection : boite englobante ou distance à
	 * l'entité.
	 * \warning		Ne fait rien par défaut, méthode à surcharger.
	 */
	virtual void selectionModeCallback (bool boundingBox);

	//@}	// Callbacks du menu "Sélection".

#ifdef USE_EXPERIMENTAL_ROOM
	/**
	 * Callbacks du menu "Chambre expérimentale".
	 */
	//@{

	/**
	 * Affiche un sélecteur de fichier de chargement de pointages laser.
	 * \warning	Ne fait rien par défaut, à surcharger.
	 */
	virtual void loadRaysCallback ( );

	/**
	 * Affiche un sélecteur de fichier de chargement de pointages laser
	 * positionné dans la base de données de pointages laser.
	 * \warning	Ne fait rien par défaut, à surcharger.
	 */
	virtual void importRaysCallback ( );

	/**
	 * Enregistre les pointages laser dans le fichier courant.
	 * \warning	Ne fait rien par défaut, à surcharger.
	 */
	virtual void saveRaysCallback ( );

	/**
	 * Affiche un sélecteur de fichier d'enregistrement de pointages laser.
	 * \warning	Ne fait rien par défaut, à surcharger.
	 */
	virtual void saveAsRaysCallback ( );

	/**
	 * Utilise le contexte courant lors de la projetection les pointages laser.
	 * \warning	Ne fait rien par défaut, à surcharger.
	 */
	virtual void setRaysContextCallback ( );

	/**
	 * Projette les pointages laser sur les surfaces et volumes sélectionnés.
	 */
	virtual void setRaysTargetSurfacesCallback ( );

	/**
	 * Affiche un sélecteur de fichier de chargement de diagnostics.
	 * \warning	Ne fait rien par défaut, à surcharger.
	 */
	virtual void loadDiagsCallback ( );

	/**
	 * Affiche un sélecteur de fichier de chargement de diagnostics
	 * positionné dans la base de données de diagnostics.
	 * \warning	Ne fait rien par défaut, à surcharger.
	 */
	virtual void importDiagsCallback ( );

	/**
	 * Enregistre les diagnostics dans le fichier courant.
	 * \warning	Ne fait rien par défaut, à surcharger.
	 */
	virtual void saveDiagsCallback ( );

	/**
	 * Affiche un sélecteur de fichier d'enregistrement de diagnostics.
	 * \warning	Ne fait rien par défaut, à surcharger.
	 */
	virtual void saveAsDiagsCallback ( );

	//@}	// Callbacks du menu "Chambre expérimentale".
#endif	// USE_EXPERIMENTAL_ROOM

	/**
	 * Callbacks du menu "Outils".
	 */
	//@{

	/**
	 * Affiche la boite de dialogue de gestion des commandes.
	 */
	virtual void showCommandMonitorDialogCallback ( );

	//@}	// Callbacks du menu "Outils".

	/**
	 * Callbacks du menu Aide
	 */
	//@{

	/**
	 * Affiche le manuel utilisateur au format imprimable (<I>pdf</I> ou
	 * équivalent).
	 */
	virtual void usersGuideCallback ( );

	/**
	 * Affiche l'aide contextuelle (manuel utilisateur au format hypertexte).
	 */
	virtual void usersGuideContextCallback ( );

	/**
	 * Affiche la page web <I>Wiki Magix 3D</I>.
	 */
	virtual void wikiCallback ( );
		/**
	 * Affiche la page web <I>Wiki Magix 3D</I>.
	 */
	virtual void tutorialCallback ( );

	/**
	 * Affiche la page web <I>API python Magix 3D</I>.
	 */
	virtual void pythonAPIUsersGuideCallback ( );

	/**
	 * Affiche la page web <I>Qualif (Qualité de maillages)</I>.
	 */
	virtual void qualifCallback ( );

	/**
	 * Affiche la page web <I>raccourcis clavier<I> du manuel utilisateur.
	 */
	virtual void shortKeyCallback ( );

	/**
	 * Affiche la page web <I>sélection d'entités<I> du manuel utilisateur.
	 */
	virtual void selectCallback ( );

	/**
	 * Affiche la page web <I>historique<I>.
	 */
	virtual void historiqueCallback ( );

	/**
	 * Affiche une boite de dialogue d'impression de la vue 3D.
	 */
	virtual void aboutDialogCallback ( );

	//@}	// Callbacks du menu Aide

	/**
	 * Callbacks liés au multithreading.
	 */
	//@{

	/**
	 * Invoque <I>observableModified</I> avec les arguments reçus. On est
	 * supposés être dans le thread principal de l'application.
	 */
	virtual void observableModifiedCallback (TkUtil::ReferencedObject*, unsigned long);

	//@}	// Callbacks liés au multithreading.


	protected :

	/**
	 * Redimensionnement du menu Session.Scripts Magix 3D récents ...
	 * \param		Nouveau nombre de scripts conservé en mémoire.
	 */
	virtual void updateRecentScriptsURLFifoCapacity (size_t count);

	/**
	 * \return		Un nom de fichier script Magix 3D ayant pour base filename.
	 * 				Ajoute si nécessaire l'extension .py à filename.
	 * \param		Nom proposé par défaut.
	 * \exception	Une exception est levée si l'extension de filename est non
	 *				nulle et différente de py.
	 */
	virtual std::string checkMagix3DScriptFileName (const std::string& filename) const;


	private :

	// Opérations interdites :
	QtMgx3DMainWindow (const QtMgx3DMainWindow&);
	QtMgx3DMainWindow& operator = (const QtMgx3DMainWindow&);

	/** La taille maximale autorisée a t'elle été définie. */
	bool							_maxSizeSet;

	/** Le contexte de la session de travail. */
	Mgx3D::Internal::ContextIfc*				_context;

	/** Le panneau de gestion des groupes d'entités. */
	QtGroupsPanel*						_groupsPanel;

	/** Le panneau de gestion des entités. */
	QtEntitiesPanel*					_entitiesPanel;

	/** La fenêtre graphique. */
	Qt3DGraphicalWidget*					_graphicalWidget;

	/** Les <I>opérations Magix 3D</I>. */
	QtMgx3DOperationsPanel*					_operationsPanel;

	/** Les panneaux additionnels.
	 * @ see		registerAdditionalOperationPanel
	 */
	std::vector<QtMgx3DOperationPanel*>			_additionalPanels;

	/** Les menus. */
	QMenu							*_projectMenu, *_sessionMenu,
								*_viewMenu,
								*_cadMenu, *_topologyMenu,
								*_meshingMenu, *_selectionMenu,	
								*_roomMenu,	*_toolsMenu, *_helpMenu;
	bool							_actionsDisabled;

	/** Les commandes python. */
	QtMgx3DPythonConsole*					_pythonPanel;

	/** Les propriétés de la sélection. */
	QtSelectionCommonPropertiesPanel*			_selectionCommonPropertiesPanel;
	QtSelectionIndividualPropertiesPanel*			_selectionIndividualPropertiesPanel;

	/** L'affichage des informations relatives au déroulement de la session. */
	QtMgx3DLogsView*					_logView;
	QtStatusLogOutputStream*				_statusView;
	QtMgx3DStateView*					_stateView;
	QtLogsView*						_pythonLogView;

	/** Les dock widgets. */
	QDockWidget						*_entitiesDockWidget,
								*_operationsDockWidget,
								*_historyDockWidget,
								*_selectionCPDockWidget,
								*_selectionIPDockWidget,
								*_pythonDockWidget,
								*_pythonOutputDockWidget,
								*_commandMonitorDockWidget;

	/** Les tab widgets. */
	QTabWidget						*_entitiesTabWidget;

	/** Les barres d'outils. */
	QToolBar						*_commandToolbar,
								*_3dViewToolbar,
								*_meshingToolbar,
								*_selectionToolbar;

	/** Les actions possibles. */
	WindowsActions						_actions;

	/** Le titre de l'application, pour d'éventuels messages d'erreur. */
	std::string						_appTitle;

	/** L'éventuel observateur de sélection d'entité. */
	Mgx3D::QtComponents::EntitySeizureManager*		_seizureManager;

	/** La boite de dialogue de surveillance des commandes. */
	QtCommandMonitorPanel*					_commandMonitorPanel;

#ifdef USE_EXPERIMENTAL_ROOM
	/** Le panneau "Chambre expérimentale". */
	QWidget*							_experimentalRoomPanel;
#endif	// USE_EXPERIMENTAL_ROOM

	/** L'éventuelle boite de dialogue de modification des propriétés graphiques
	 * des entités affichées. */
	Mgx3D::QtComponents::QtRepresentationTypesDialog*	_repTypesDialog;

	/** L'éventuel script python minimal en cours et ses options
	 * d'enregistrement.
	 * @see		saveMagix3DScriptCallback
	 */
	std::string										_pythonMinScript;
	TkUtil::Charset::CHARSET						_pytMinScriptCharset;
	Internal::ContextIfc::encodageScripts			_encodageScripts;

	/** La liste des scripts <I>Magix 3D</I> récemments exécutés. */
	static TkUtil::UrlFifo					_recentScriptsURLFifo;

	/** Le fichier contenant la liste des scripts <I>Magix 3D</I> récemments
	 * exécutés. */
	static const std::string				_recentScriptsFileName;
};	// class QtMgx3DMainWindow


/**
 * Macros utilitaires pour la classe QtMgx3DMainWindow et ses classes dérivées.
 */
//@{

#define CHECK_ACTIONS( method)                                                      \
if (0 == getActions ( )._quitAction)                                                \
{                                                                                   \
	INTERNAL_ERROR (exc, "Actions non instanciées.", method)                    \
	throw exc;                                                                  \
}

#define DISABLE_COMMAND_ACTIONS                                                     \
	getActions ( ).setEnabled (false);                                          \
	if (0 != getPythonPanel ( ))                                                \
		getPythonPanel ( )->setUsabled (false);

#define CHECK_COMMAND_CREATION_STATUS                                               \
	if (true == hasError)                                                       \
	{                                                                           \
		message << "Echec lors de la création de la commande "              \
		        << " : " << errorString;                                    \
		log (ErrorLog (message));                                           \
		getActions ( ).setEnabled (true);                                   \
		if (0 != getPythonPanel ( ))                                        \
			getPythonPanel ( )->setUsabled (true);                      \
	}	/* if (true == hasError) */


#define DISABLE_GRAPHICAL_OPERATIONS                                        \
	RenderingManager::DisplayLocker	displayLocker (getGraphicalWidget ( ).getRenderingManager ( ));

//@}


/**
 * Class <I>indicateur d'état de l'IHM</I>.
 */
class QtMgx3DStateView : public QWidget
{
	public :

	QtMgx3DStateView (const QString& title, QWidget* parent = 0);
	virtual ~QtMgx3DStateView ( );

	/**
	 * Modifie l'apparence du widget conformément à l'état transmis en 
	 * argument. Si un widget label est associé à cette fenêtre alors un
	 * message temporaire y est affiché.
	 * \param		Nom de la commande concernée
	 * \param		Etat de la commande concernée
	 * \param		setLabel
	 */
	virtual void setState (const std::string& cmdName, Mgx3D::Utils::CommandIfc::status state);

	/**
 	 * \param		label où afficher des informations temporaires sur l'état de
 	 *				l'IHM.
 	 * \see			setState
 	 */
	virtual void setLabel (QLabel* label);

	/**
	 * Taille de l'indicateur.
	 */
	static unsigned short	gaugeSize;

	/**
	 * Temps d'affichage des messages fugaces.
	 */
	static unsigned short	volatilesMessagesDelay;

	/**
 	 * Demi-période de l'indicateur clignotant.
 	 */
	static unsigned short	timerDelay;


	protected :

	/**
	 * Affiche l'état courrant.
	 */
	virtual void paintEvent (QPaintEvent* event);


	private :

	QtMgx3DStateView (const QtMgx3DStateView&);
	QtMgx3DStateView& operator = (const QtMgx3DStateView&);

	/** L'état courrant. */
	Mgx3D::Utils::CommandIfc::status	_state;

	/** Un chronomètre pour le l'affichage intermittent. */
	QTimer*								_timer;

	/** Le dernier affichage intermittent a été fait (<I>true</I>) ou
	 * non (<I>false</I>). */
	bool								_on;

	/** Le label où sont affichés des messages temporaires. */
	QLabel*								_label;

	/** Un chronomètre pour le l'affichage fugace dans le label. */
	QTimer*								_labelTimer;
};	// class QtMgx3DStateView


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MGX3D_MAIN_WINDOW_H
