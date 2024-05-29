/**
 * \file		QtMgx3DOperationsPanel.h
 * \author		Charles PIGNEROL
 * \date		05/12/2012
 */
#ifndef QT_MGX3D_OPERATIONS_PANEL_H
#define QT_MGX3D_OPERATIONS_PANEL_H


namespace Mgx3D
{

namespace QtComponents
{
	class QtMgx3DMainWindow;
	class RenderingManager;
}	// namespace QtComponents

}	// namespace Mgx3D


#include "Internal/ContextIfc.h"
#include "Internal/CommandInternal.h"
#include "Internal/Resources.h"

#include <TkUtil/LogOutputStream.h>
#include <QtUtil/QtGroupBox.h>
#include <QtUtil/QtMultiLineToolbar.h>
#include <QtUtil/QtWidgetAction.h>
#include <QSettings>
#include <QAction>
#include <QActionGroup>
#include <QCheckBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QWidgetAction>

#include "Utils/ValidatedField.h"
#include "Internal/Context.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "QtComponents/RenderingManager.h"
#include <QtUtil/QtIntTextField.h>

#include <vector>
#include <map>



/**
 * Macro-définition pour le comportement à adopter en cas de prévisualisation
 * demandée : faut-il afficher une boite de dialogue en cas d'erreur dans un
 * champ de saisie d'un paramètre de l'opéartion (ex : absence de valeur
 * numérique, ...).
 */
#define QT_PREVIEW_BEGIN_TRY_CATCH_BLOCK                                   \
	try { 
#define QT_PREVIEW_COMPLETE_TRY_CATCH_BLOCK                                \
	}                                                                      \
	catch (...)                                                            \
	{                                                                      \
		if (false == Mgx3D::Internal::Resources::instance ( )._prevalidateSeizure) \
			preview (false, false);                                        \
		else                                                               \
			throw;                                                         \
	}


namespace Mgx3D 
{

namespace QtComponents
{
	class QtMgx3DOperationPanel;
	class QtMgx3DOperationsPanel;
	class QtOperationsSubFamilyToolBar;
	class QtOperationsFamilyToolBar;
	class QtOperationsFamiliesToolBar;
	class QtMgx3DOperationAction;
	class QtMgx3DOperationsFamilyAction;
	class QtMgx3DOperationsFamiliesAction;
	class QtMgx3DOperationsSubPanel;

/**
 * Classe de base des panneaux d'édition d'une opération <I>Magix 3D</I>.
 * A spécialiser.
 */
class QtMgx3DOperationPanel : public QWidget
{
	friend class QtMgx3DOperationsPanel;		// Ugly

	Q_OBJECT

	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMgx3DOperationPanel ( );

	/**
	 * Si <I>false == visible</I> appelle alors <I>cancel ( )</I>, sinon
	 * appelle <I>autoUpdate</I>.
	 * Invoque <I>QWidget::setVisible (visible)</I>.
	 * \see		cancel
	 * \see		autoUpdate
	 */
	virtual void setVisible (bool visible);

	/**
	 * Réinitialise le panneau. Par défaut appelle preview (false, true) et
	 * highlight (false) (=> destruction des pré-vues et entités mises en
	 * surbrillance).
	 */
	virtual void reset ( );

	/**
	 * Méthode appelée pour vérifier les paramètres saisis par l'utilisateur.
	 * En cas de paramètrage invalide cette méthode doit lever une exception de
	 * type <I>TkUtil::Exception</I> contenant un descriptif des erreurs
	 * rencontrées.
	 * A surcharger éventuellement. Par défaut cette méthode évalue tous les
	 * champs enregistrés par <I>addValidatedField</I> et lève une exception
	 * si au moins l'un d'entre-eux lève une exception lors de l'invocation
	 * de sa méthode <I>validate</I>.
	 * \see		getContext
	 * \see		addValidatedField
	 */
	virtual void validate ( );

	/**
	 * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
	 * Doit restituer l'environnement dans son état initial.
	 * A surcharger éventuellement. Par défaut cette méthode appelle
	 * <I>highlight (false)</I> et <I>preview (false, true)</I>.
	 * \see		autoUpdate
	 * \see		preview
	 * \see		highlight
	 * \see		cancelClearEntities
	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte. Si
	 * <I>autoUpdateUsesSelection ( )</I> retourne <I>true</I> le panneau doit 
	 * être initialisé autant que possible avec la sélection en cours.
	 * \see		cancel
	 * \see		autoUpdateUsesSelection
	 * \see		highlight
	 * \see		previewResult
	 * A surcharger éventuellement. Par défaut cette méthode appelle
	 * <I>highlight (true)</I> et <I>preview (true, true)</I>.
	 */
	virtual void autoUpdate ( );

	/**
	 * \return	<I>true</I> s'il faut prévisualiser le résultat de l'opération,
	 * 			<I>false</I> dans le cas contraire.
	 * 			Retourne <I><B>false</B></I> par défaut.
	 * \addPreviewCheckBox
	 */
	virtual bool previewResult ( ) const;

	/**
	 * \return	<I>true</I> si le masquage du panneau (<I>cancel</I>) doit
	 * 			provoquer l'effacement des noms d'entités, <I>false</I> dans le
	 * 			cas contraire.
	 * \see		cancel
	 */
	virtual bool cancelClearEntities ( ) const;

	/**
	 * \return	<I>true</I> si l'affichage du panneau (<I>autoUpdate</I>) doit
	 * 			provoquer sa réinitialisation (autant que possible) à partir de
	 * 			la sélection courante, <I>false</I> dans le cas contraire.
	 * \see		autoUpdate
	 */
	virtual bool autoUpdateUsesSelection ( ) const;

	/**
	 * \return		Un pointeur sur l'éventuelle action <I>Magix 3D</I>
	 *				associée. Cette action permet d'exécuter une operation
	 *				<I>Magix 3D</I> avec les paramètres de ce panneau.
	 * \see			getMgx3DOperationAction
	 */
	virtual QtMgx3DOperationAction* getMgx3DOperationAction ( );

	/**
	 * \return		Le nom du fichier contenant l'aide contextuelle associée
	 *				à ce panneau (chemin relatif par rapport à la racine de
	 *				l'aide contextuelle du logiciel.
	 * \see			helpTag
	 */
	virtual std::string helpURL ( ) const;

	/**
	 * \see			La balise identifiant le paragraphe d'aide contextuelle
	 *				associée à ce panneau dans le fichier d'aide contextuelle
	 *				référencé par <I>helpURL</I>.
	 * \see			helpURL
	 */
	virtual std::string helpTag ( ) const;

	/**
	 * \return		La fenêtre principale à la quelle est associée cette
	 *				instance.
	 */
	virtual QtMgx3DMainWindow* getMainWindow ( );
	virtual const QtMgx3DMainWindow* getMainWindow ( ) const;

	/**
	 * Ne pas utiliser, pour mécanismes internes.
	 */
	virtual void setMainWindow (QtMgx3DMainWindow* window);

	/**
	 * Invoqué lorsqu'une commande est modifiée.
	 * \param		Détails sur la commande modifiée. Permet notamment de savoir
	 *				si des entités ont été créées, modifiées ou détruites, et
	 *				d'actualiser le panneau en conséquence.
	 * \warning		Ne fait rien par défaut, méthode à surcharger si nécessaire.
	 */
	virtual void commandModifiedCallback (Mgx3D::Internal::InfoCommand&);
	
	/**
	 * <P>
	 * Cette méthode fait (pour <I>show</I> valant <I>true</I>) :<BR>
	 * <UL>
	 * <LI>Apparaître d'éventuels interacteurs permettant le paramétrage
	 * interactif de l'opération,
	 * <LI>Apparaître un aperçu du résultat si <I>previewResult</I> retourne
	 * <I>true</I>.
	 * <UL>
	 * </P>
	 * \param	<I>true</I> pour prévisualiser le résultat de l'opération,
	 * 			<I>false</I> pour arrêter la prévisualisation.
	 * \param	<I>true</I> si les éventuels interacteurs en place doivent
	 *			être détruits, <I>false</I>  dans le cas contraire.
	 * \see		previewResult
	 * \see		highlight
	 * \see		registerPreviewedObject
	 * \see		registerPreviewedInteractor
	 */
	virtual void preview (bool show, bool destroyInteractor);

	/**
	 * Cette méthode fait (pour <I>show</I> valant <I>true</I>),
	 * apparaître en surbrillance les entités impliquées dans
	 * l'opération (obtenues par appel à <I>getInvolvedEntities</I>).
	 * \see		getInvolvedEntities
	 * \see		preview
	 * \see		registerHighlightedEntity
	 */
	virtual void highlight (bool show);

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * Invoque <I>preview (false, true).</I>
	 * Méthode à surcharger éventuellement pour stopper d'éventuelles saisies
	 * en cours, ...
	 * \see		preview
	 */
	virtual void operationCompleted ( );


	protected :

	/**
	 * Constructeur. RAS.
	 * \param	Widget parent.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 * \param	Fichier d'aide contextuelle associé à ce panneau.
	 * \param	Balise dans le fichier d'aide contextuelle associé à ce panneau
	 *			identifiant le paragraphe sur ce panneau.
	 * \see		validate
	 * \see		getContext
	 * \see		getMgx3DOperationAction
	 * \see		helpURL
	 */
	QtMgx3DOperationPanel (
						QWidget* parent,
						Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
						Mgx3D::QtComponents::QtMgx3DOperationAction*,
						const std::string& helpURL, const std::string& helpTag);

	/**
	 * \param	Champs de saisi à évaluer lors d'un appel à <I>validate</I>.
	 * \see		validate
	 */
	virtual void addValidatedField (Mgx3D::Utils::ValidatedField& field);

	/**
	 * \return	Une référence sur le contexte <I>Magix 3D</I> associé à
	 *			l'opération.
	 */
	virtual const Mgx3D::Internal::ContextIfc& getContext ( ) const;
	virtual Mgx3D::Internal::ContextIfc& getContext ( );

	/**
	 * \return	Une référence sur le gestionnaire de sélection.
	 * \except	Une exception est levée en l'absence de gestionnaire associé.
	 */
	virtual Mgx3D::Utils::SelectionManagerIfc& getSelectionManager ( );
	virtual const Mgx3D::Utils::SelectionManagerIfc&
											getSelectionManager ( ) const;

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 * \warning	<B>Méthode à surcharger</I>.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return	Une référence vers le gestionnaire de rendu.
	 * \except	Une exception est levée en l'absence de gestionnaire de rendu.
	 */
	virtual Mgx3D::QtComponents::RenderingManager& getRenderingManager ( );

	/**
	 * Ajoute à la liste des entités mise en évidence celle
	 * transmise en argument.
	 * \see		unregisterHighlightedEntity
	 * \see		getHighlightedEntities
	 */
	virtual void registerHighlightedEntity (Mgx3D::Utils::Entity& entity);

	/**
	 * Enlève de la liste des entités mise en évidence celle
	 * transmise en argument.
	 * \see		registerHighlightedEntity
	 * \see		getHighlightedEntities
	 */
	virtual void unregisterHighlightedEntity (Mgx3D::Utils::Entity& entity);

	/**
	 * \return	La liste des entités mise en évidence.
	 * \see		registerHighlightedEntity
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getHighlightedEntities ( );

	/**
	 * Ajoute une case à cocher "Aperçu" au panneau. Utilise le <I>layout</I>
	 * existant (=> il doit y en avoir un).
	 * \param	Valeur par défaut de la case à cocher.
	 * \see		previewResult
	 */
	virtual void addPreviewCheckBox (bool defaultValue);

	/**
	 * Ajoute à la liste des objets de prévisualisation celui transmis en
	 * argument.
	 * \see		getPreviewedObjects
	 * \see		removePreviewedObjects
	 */
	virtual void registerPreviewedObject (
				Mgx3D::QtComponents::RenderingManager::RepresentationID id);

	/**
	 * \return	La liste des objets de prévisualisation.
	 * \see		registerPreviewedObject
	 * \see		removePreviewedObjects
	 */
	virtual std::vector<Mgx3D::QtComponents::RenderingManager::RepresentationID> getPreviewedObjects ( );

	/**
	 * Détruit les objets de prévisualisation.
	 * \see		registerPreviewedObject
	 * \see		getPreviewedObjects
	 */
	virtual void removePreviewedObjects ( );

	/**
	 * Affiche en pré-visualisation les arêtes et leurs informations de la
	 * commande tranmise en argument dont le status est également transmis en
	 * argument.
	 * En troisième argument les propriétés d'affichage à utiliser.
	 * En quatrième argument le masque de représentation.
	 * \warning	N'effectue pas de forceRender.
	 * \see		removePreviewedObjects
	 */
	virtual void previewEdges (
			Mgx3D::Internal::CommandInternal& command,
			Mgx3D::QtComponents::RenderingManager::CommandPreviewMgr& manager,
			Mgx3D::Internal::InfoCommand::type status,
			const Mgx3D::Utils::DisplayProperties& props,
			unsigned long mask);

	/**
	 * Ajoute à la liste des interacteurs utilisés celui transmis en argument.
	 * \see		getPreviewedInteractors
	 */
	virtual void registerPreviewedInteractor (
				Mgx3D::QtComponents::RenderingManager::Interactor* interactor);

	/**
	 * \return	La liste des interacteurs en cours d'utilisation.
	 * \see		registerPreviewedInteractor
	 */
	virtual std::vector<Mgx3D::QtComponents::RenderingManager::Interactor*> getPreviewedInteractors ( );


	/**
	 * Affiche le message transmis en argument dans la sortie de message de la
	 * fenêtre principale associée.
	 */
	virtual void log (const TkUtil::Log& log);


	protected slots :

	/**
	 * Services utilisables dans un contexte autre que celui du panneau
	 * "Opérations Magix 3D" (donc par exemple dans un contexte de panneaux
	 * isolés type "explorateur de feuillets").
	 */
	//@{

	/**
	 * Réinitialise le panneau selon le contexte actuel.
	 */
	virtual void autoUpdateCallback ( );

	/**
	 * Actualise la vue selon les paramètres du panneau en appelant
	 * <I>preview (false, true)</I> puis <I>preview (true, true)</I>.
	 */
	virtual void parametersModifiedCallback ( );

	/**
	 * Exécute l'action associée puis invoque <I>operationCompleted</I>.
	 * \see		operationCompleted
	 */
	virtual void applyCallback ( );

	/**
	 * Affiche l'aide associée au panneau.
	 */
	virtual void helpCallback ( );

	//@}	// Services utilisables dans un contexte "autonome"

	/**
	 * Services "optimisés" de gestion de la sélection.
	 */
	//@{

	/**
	 * Invoqué lorsque des entités sont ajoutées à la sélection.
	 * Met ces entités en surbrillance.
	 * Réactualise la prévisualisation.
	 * \see		entitiesRemovedFromSelectionCallback
	 * \see		preview
	 */
	virtual void entitiesAddedToSelectionCallback (QString entitiesNames);

	/**
	 * Invoqué lorsque des entités sont enlevées de la sélection.
	 * Arrête l'affichage en surbrillance de ces entités.
	 * Réactualise la prévisualisation.
	 * \see		entitiesAddedToSelectionCallback
	 * \see		preview
	 */
	virtual void entitiesRemovedFromSelectionCallback (QString entitiesNames);

	//@}	// Services "optimisés" de gestion de la sélection.

	/**
	 * Services liés à l'opération.
	 */
	//@{

	/**
 	 * Passe en mode "aperçu", ou quitte ce mode.
 	 */
	virtual void previewCallback ( );

	//@}	// Services liés à l'opération.
	
	/**
	 * Invoqué lorsque l'utilisateur a fini de modifier les paramètres de
	 * discrétisation. Emet le signal <I>discretisationModified</I>.
	 * \see		discretisationModified
	 */
	virtual void discretisationModifiedCallback ( );

	signals :

	/**
	 * Signal emit lorsque l'utilisateur modifie le paramétrage de la
	 * discrétisation.
	 * \see		discretisationModifiedCallback
	 */
	void discretisationModified ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMgx3DOperationPanel (const QtMgx3DOperationPanel&);
	QtMgx3DOperationPanel& operator = (const QtMgx3DOperationPanel&);

	/** Le contexte <I>Magix 3D</I> de l'opération. */
	Mgx3D::QtComponents::QtMgx3DMainWindow*			_mainWindow;

	/** Faut-il faire une prévisualisation du résultat de l'opération ? */
	QCheckBox*										_previewCheckBox;

	/** Les champs de saisi à valider. */
	std::vector <Mgx3D::Utils::ValidatedField*>		_validatedFields;

	/** Une éventuelle action <I>Magix 3D</I> associée. */
	QtMgx3DOperationAction*							_action;

	/** Les entités en cours de prévisualisation. */
	std::vector<Mgx3D::Utils::Entity*>				_highlightedEntities;

	/** Les objets de prévisualisation. */
	std::vector<RenderingManager::RepresentationID>	_previewIDs;

	/** Les interacteurs en cours d'utilisation. */
	std::vector<RenderingManager::Interactor*>		_interactors;

	/** Le nom du fichier contenant l'aide contextuelle associée à ce panneau
	 * (chemin relatif par rapport à la racine de l'aide contextuelle du
	 * logiciel).
	 */
	std::string										_helpURL;

	/** La balise identifiant le paragraphe d'aide contextuelle associée à ce
	 * panneau dans le fichier d'aide contextuelle référencé par <I>_helpURL</I>
	 * .*/
	std::string										_helpTag;

};	// class QtMgx3DOperationPanel


/**
 * Conteneur d'actions type "Sous-famille d'opérations".
 */
class QtOperationsSubFamilyToolBar : public QtMultiLineToolbar
{
	public :

	/**
	 * \param		Parent de la barre d'icônes.
	 * \param		Gestionnaire des barres d'icônes.
	 */
	QtOperationsSubFamilyToolBar (
						QWidget* parent, QtMgx3DOperationsPanel* panel);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtOperationsSubFamilyToolBar ( );


	/**
	 * Les actions donnant l'accès aux opérations de la sous-famille.
	 */
	//@{

		/**
		 * Ajoute une action donnant accès à une opération de la sous-famille.
		 */
		virtual void addOperationAction (QtMgx3DOperationAction& action);

		/**
		 * \return	Eventuel panneau opération courante.
		 * \see		setOperationPanel
		 */
		virtual QtMgx3DOperationPanel* getOperationPanel ( );

		/**
		 * \param	Eventuel panneau opération courante.
		 * \see		getOperationPanel
		 */
		virtual void setOperationPanel (QtMgx3DOperationPanel*);

	//@}	// Les actions donnant l'accès aux opérations de la sous-famille.

	/**
	 * Appelé lorsque la session est en passe d'être réinitialisée. Purge
	 * d'éventuels pointeurs vers des entités.
	 */
	virtual void sessionCleared ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtOperationsSubFamilyToolBar (const QtOperationsSubFamilyToolBar&);
	QtOperationsSubFamilyToolBar& operator = (const QtOperationsSubFamilyToolBar&);

	/** Le groupe "Opérations de la sous-famille". */
	QActionGroup*				_operationsGroup;

	/** le gestionnaire du panneau. */
	QtMgx3DOperationsPanel*		_operationsPanel;

	/** L'éventuel panneau opération courante. */
	QtMgx3DOperationPanel*		_operationPanel;
};	// class QtOperationsSubFamilyToolBar


/**
 * Conteneur d'actions type "Famille d'opérations".
 */
class QtOperationsFamilyToolBar : public QtMultiLineToolbar
{
	public :

	/**
	 * \param		Parent de la barre d'icônes.
	 * \param		Gestionnaire des barres d'icônes.
	 */
	QtOperationsFamilyToolBar (QWidget* parent, QtMgx3DOperationsPanel* panel);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtOperationsFamilyToolBar ( );


	/**
	 * Les actions donnant l'accès aux sous-familles d'opérations.
	 */
	//@{

		/**
		 * Ajoute une action donnant accès à une sous-famille d'opérations.
		 */
		virtual void addOperationsSubFamilyToolBar (QtMgx3DOperationsFamilyAction& action);

		/**
		 * \return	Eventuel panneau sous-famille d'opérations courant.
		 * \see		setSubFamilyToolBar
		 */
		virtual QtOperationsSubFamilyToolBar* getSubFamilyToolBar ( );

		/**
		 * \param	Eventuel panneau sous-famille d'opérations courant.
		 * \see		getSubFamilyToolBar
		 */
		virtual void setSubFamilyToolBar (QtOperationsSubFamilyToolBar*);

	//@}	// Les actions donnant l'accès aux sous-familles d'opérations.


	protected :


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtOperationsFamilyToolBar (const QtOperationsFamilyToolBar&);
	QtOperationsFamilyToolBar& operator =(const QtOperationsFamilyToolBar&);

	/** Le groupe "Sous-familles d'opérations". */
	QActionGroup*						_operationsSubFamiliesGroup;

	/** le gestionnaire du panneau. */
	QtMgx3DOperationsPanel*				_operationsPanel;

	/** L'éventuel panneau sous-famille d'opérations courant. */
	QtOperationsSubFamilyToolBar*		_subFamilyToolBar;
};	// class QtOperationsFamilyToolBar


/**
 * Conteneur des actions "Famille d'opérations" (Géométrie/Topologie/Maillage).
 */
class QtOperationsFamiliesToolBar : public QtMultiLineToolbar
{
	public :

	/**
	 * \param		Parent de la barre d'icônes.
	 * \param		Gestionnaire des barres d'icônes.
	 */
	QtOperationsFamiliesToolBar (QWidget* parent, QtMgx3DOperationsPanel* panel);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtOperationsFamiliesToolBar ( );


	/**
	 * Les actions donnant l'accès aux familles d'opérations.
	 */
	//@{

		/**
		 * Ajoute une action donnant accès à une famille d'opérations.
		 */
		virtual void addOperationsFamilyToolBar (QtMgx3DOperationsFamiliesAction& action);

	//@}	// Les actions donnant l'accès aux familles d'opérations.


	protected :


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtOperationsFamiliesToolBar (const QtOperationsFamiliesToolBar&);
	QtOperationsFamiliesToolBar& operator =(const QtOperationsFamiliesToolBar&);

	/** Le groupe "Familles d'opérations". */
	QActionGroup*				_operationsFamiliesGroup;

	/** le gestionnaire du panneau. */
	QtMgx3DOperationsPanel*		_operationsPanel;
};	// class QtMgx3DOperationsFamilyAction


/**
 * Classe de base d'action type <I>check box</I> associée à un widget. Cette
 * classe permet d'automatiser :<BR>
 * <UL>- Un callback sur l'évènement <I>checked</I>,
 * <UL>- La conservation de la référence sur un widget.
 * </UL>
 */
class QtMgx3DWidgetedCheckedAction : public QtWidgetAction
{
	Q_OBJECT

	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMgx3DWidgetedCheckedAction ( );

	/**
	 * \return		Le panneau de "Familles d'opérations Magix 3D".
	 */
	virtual const QtMgx3DOperationsPanel& getOperationsPanel ( ) const;
	virtual QtMgx3DOperationsPanel& getOperationsPanel ( );

	/**
	 * \return		La fenêtre principale associée à l'action.
	 */
	virtual const Mgx3D::QtComponents::QtMgx3DMainWindow& getMainWindow ( ) const;
	virtual Mgx3D::QtComponents::QtMgx3DMainWindow& getMainWindow ( );

	/**
	 * \return		Le widget associé.
	 * \see			setWidget
	 */
	virtual QWidget* getWidget ( );

	/**
	 * \return		Une référence sur le contexte <I>Magix 3D</I> associé
	 *				à cette opération.
	 */
	virtual const Mgx3D::Internal::ContextIfc& getContext ( ) const;
	virtual Mgx3D::Internal::ContextIfc& getContext ( );

	/**
	 * \param		Le widget associé.
	 * \see			getWidget
	 */
	virtual void setWidget (QWidget* widget);

	/**
	 * \return		Le parent du panneau d'édition de l'opération Magix 3D.
	 */
	virtual QWidget& getOperationPanelParent ( );


	protected :

	/**
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Gestionnaire de barres d'icônes d'opérations Magix 3D.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte.
	 * \param		Tooltip décrivant l'action.
	 */
	QtMgx3DWidgetedCheckedAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);


	protected slots :

	/**
	 * Appelée lorsque l'action est "checked".
	 * Ne fait rien par défaut, méthode à surcharger.
	 */
	virtual void checkedCallback (bool);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMgx3DWidgetedCheckedAction (const QtMgx3DWidgetedCheckedAction&);
	QtMgx3DWidgetedCheckedAction& operator = (
										const QtMgx3DWidgetedCheckedAction&);

	QtMgx3DOperationsPanel*						_operationsPanel;
	Mgx3D::QtComponents::QtMgx3DMainWindow*		_mainWindow;
	QWidget*									_widget;
};	// class QtMgx3DWidgetedCheckedAction


/**
 * Classe de base d'action provoquant l'affichage d'un panneau de "Opération
 * Magix 3D".
 * A spécialiser.
 * \see		QtMgx3DOperationPanel
 */
class QtMgx3DOperationAction : public QtMgx3DWidgetedCheckedAction
{
	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMgx3DOperationAction ( );

	/**
	 * \return		Le panneau d'édition de l'opération Magix 3D.
	 * \see			setOperationPanel
	 * \see			getOperationPanelParent
	 */
	virtual QtMgx3DOperationPanel* getOperationPanel ( );

	/**
	 * Effectue l'opération <I>Magix 3D</I> associée, en récupérant le
	 * paramétrage de l'opération dans le panneau d'édition de l'opération
	 * Magix 3D.
	 * Associe - quand il y en a un - le résultat à l'action via setCommandResult.
	 * \warning		Méthode à surcharger, ne fait rien par défaut.
	 * \exception	En cas d'erreur dans le paramétrage ou d'exécution de
	 *				l'opération <I>Magix 3D</I>.
	 * \see			getOperationPanel
	 * \see			setCommandResult
	 * \see			getCommandResult
	 */
	virtual void executeOperation ( );
	
	/**
	 * \return		L'éventuel résultat associé à la commande en cours (concerne
	 *				les commandes à exécution séquentielles).
	 */
	virtual Mgx3D::Utils::CommandResultIfc* getCommandResult ( );

	/**
	 * \return		L'éventuel résultat associé à la commande en cours (concerne
	 *				les commandes à exécution séquentielles).
	 */
	virtual void setCommandResult (Mgx3D::Utils::CommandResultIfc*);

	/**
	 * <P>
	 * Suspend (<I>false</I>) ou réactive (<I>true</I>) le déroulement de
	 * l'opération.
	 * </P>
	 * <P>Par défaut ne fait rien, l'opération est toujours activée, mais
	 * certaines opérations requièrent de pouvoir être débrayées sans pour
	 * autant fermer leur panneau (et perdre leur paramétrage). Cette fonction
	 * est prévu à cet effet.
	 */
	virtual void setActivated (bool activated);


	protected :

	/**
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et la barre d'icônes
	 *				d'opérations Magix 3D..
	 * \param		Tooltip décrivant l'action.
	 * \warning		Les classes dérivées doivent créer le panneau et le
	 *				recenser auprès de cette classe via
	 *				<I>setOperationPanel</I>. Le widget parent du panneau de
	 *				l'opération est accessible via la méthode
	 *				<I>getOperationPanelParent ( )</I>.
	 * \see			setOperationPanel
	 * \see			getOperationPanelParent
	 */
	QtMgx3DOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * \param		Le panneau d'édition de l'opération Magix 3D.
	 * \see			getOperationPanel
	 */
	virtual void setOperationPanel (QtMgx3DOperationPanel* panel);
	

	protected :

	/**
	 * Appelée lorsque l'action est "checked". Active la barre d'icônes de
	 * sous-familles d'opérations associée.
	 */
	virtual void checkedCallback (bool);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMgx3DOperationAction(const QtMgx3DOperationAction&);
	QtMgx3DOperationAction& operator = (const QtMgx3DOperationAction&);
	
	/** L'éventuel résultat associé à la commande en cours. */
	Mgx3D::Utils::CommandResultIfc*		_commandResult;
};	// class QtMgx3DOperationAction


/**
 * Classe d'action provoquant l'affichage d'un panneau de "Sous-familles
 * d'opérations Magix 3D".
 */
class QtMgx3DOperationsFamilyAction : public QtMgx3DWidgetedCheckedAction
{
	public :

	/**
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et la barre d'icônes
	 *				d'opérations Magix 3D.
	 * \param		Barre d'icônes associée à activer lorsque cette action est
	 *				activée.
	 * \param		Tooltip décrivant l'action.
	 */
	QtMgx3DOperationsFamilyAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		QtOperationsSubFamilyToolBar& toolbar, const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMgx3DOperationsFamilyAction ( );


	protected :

	/**
	 * Appelée lorsque l'action est "checked". Active la barre d'icônes de
	 * sous-familles d'opérations associée.
	 */
	virtual void checkedCallback (bool);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMgx3DOperationsFamilyAction(const QtMgx3DOperationsFamilyAction&);
	QtMgx3DOperationsFamilyAction& operator = (const QtMgx3DOperationsFamilyAction&);
};	// class QtMgx3DOperationsFamilyAction


/**
 * Classe d'action provoquant l'affichage d'un panneau de "Sous-familles
 * d'opérations Magix 3D".
 */
class QtMgx3DOperationsFamiliesAction : public QtMgx3DWidgetedCheckedAction
{
	public :

	/**
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et la barres d'icônes 
	 *				d'opérations Magix 3D.
	 * \param		Barre d'icônes associée à activer lorsque cette action est
	 *				activée.
	 * \param		Tooltip décrivant l'action.
	 */
	QtMgx3DOperationsFamiliesAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		QtOperationsFamilyToolBar& toolbar, const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMgx3DOperationsFamiliesAction ( );


	protected :

	/**
	 * Appelée lorsque l'action est "checked". Active la barre d'icônes de
	 * sous-familles d'opérations associée.
	 */
	virtual void checkedCallback (bool);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMgx3DOperationsFamiliesAction(const QtMgx3DOperationsFamiliesAction&);
	QtMgx3DOperationsFamiliesAction& operator = (const QtMgx3DOperationsFamiliesAction&);
};	// class QtMgx3DOperationsFamiliesAction


/**
 * <P>Classe "Panneau d'opérations Magix 3D". Permet à l'utilisateur de créer un
 * context de création/modification/suppression d'entités <I>Magix 3D</I> et de
 * réaliser ces opérations.</P>
 *
 * <P>Ces panneaux sont constitués de 4 zones :<BR>
 * <OL>
 * <LI>Une barre d'icônes mutuellement exclusives permettant de sélectionner
 * une famille d'opérations,
 * <LI>Une barre d'icônes mutuellement exclusives permettant de sélectionner
 * une sous-famille d'opérations,
 * <LI>Une barre d'icônes mutuellement exclusives permettant de sélectionner
 * une d'opération,
 * <I>QtOperationsFamilyPanel</I>),
 * <LI>Un panneau représentant l'opération sélectionnée (instance de la classe
 * <I>QtOperationPanel</I>).
 * </OL>
 * </P>
 */
class QtMgx3DOperationsPanel : public QtGroupBox
{
	Q_OBJECT

	public :

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Titre de l'application (pour les éventuels messages d'erreur).
	 * \warning	<B>N'appelle pas createGui, à faire ultérieurement</B>.
	 * \see		createGui
	 */
	QtMgx3DOperationsPanel (
				QWidget* parent, const std::string& name,
				Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
				const std::string& appTitle);

	/**
	 * Appelle <I>highlight (false)</I> et <I>preview (false, true)</I>.
	 */
	virtual ~QtMgx3DOperationsPanel ( );

	/**
	 * Divers IHM.
	 */
	//@{

	/**
	 * Enregistre les paramètres d'affichage (taille, position, ...) de cette
	 * fenêtre.
	 * \see		readSettings
	 */
	virtual void writeSettings (QSettings& settings);

	/**
	 * Lit et s'applique les paramètres d'affichage (taille, position, ...) de
	 * cette fenêtre.
	 * \see		writeSettings
	 */
	virtual void readSettings (QSettings& settings);

	//@}	// Divers IHM

	/**
	 * \return		Le titre de l'application.
	 */
	virtual const std::string& getAppTitle ( ) const
	{ return _appTitle; }

	/**
	 * Actualisation du panneau selon le contexte actuel.
	 * \see			setEnabled
	 */
	virtual void autoUpdate ( );

	/**
	 * \param		<I>true</I> pour activer le panneau, <I>false</I> pour
	 *				l'inactiver.
	 */
	virtual void setEnabled (bool enable);

	/**
	 * \return		Une référence sur la a fenêtre principale associée à
	 * 				l'action.
	 */
	virtual const Mgx3D::QtComponents::QtMgx3DMainWindow& getMainWindow ( ) const;
	virtual Mgx3D::QtComponents::QtMgx3DMainWindow& getMainWindow ( );

	/**
	 * Pour contournement de bogue observé sur station RH5 avec Qt 4.7.4.
	 * Lorsqu'un panneau d'opération est modifié par changement de sous-panneau
	 * piloté par combo box, l'ihm n'est pas actualisée, et la scrolled area
	 * active ses scrollbars alors que c'est inutile => moche et inefficace.
	 * L'appel de cette fonction y remédie.
	 */
	virtual void updateLayoutWorkaround ( );

	/**
	 * \return		Une référence sur le contexte <I>Magix 3D</I>.
	 */
	virtual const Mgx3D::Internal::ContextIfc& getContext ( ) const;
	virtual Mgx3D::Internal::ContextIfc& getContext ( );

	/**
	 * Appellé lorsque la session va être réinitialisée. Purge les éventuels
	 * liens vers des entités.
	 */
	virtual void sessionCleared ( );


	/**
	 * Les familles d'opérations.
	 */
	//@{

		/**
		 * Ajoute l'action transmise en argument comme "Famille d'opérations"
		 * à ce panneau.
		 */
		virtual void addOperationsFamilyAction (QtMgx3DOperationsFamiliesAction& action);

		/**
		 * Affiche la barre d'icônes "Famille d'opérations" transmise en
		 * argument.
		 */
		virtual void setOperationsFamilyToolBar (QtOperationsFamilyToolBar* tb);

		/**
		 * Affiche la barre d'icônes "Sous-famille d'opérations" transmise en
		 * argument.
		 */
		virtual void setOperationsSubFamilyToolBar (QtOperationsSubFamilyToolBar* tb);

		/**
		 * \return		Le parent du panneau d'édition de l'opération Magix 3D.
		 * \see			setOperationPanel
		 */
		virtual QWidget& getOperationPanelParent ( );

		/**
		 * \return		L'éventuel "panneau d'édition de l'opération courante".
		 * \see			setOperationsPanel
		 */
		virtual QtMgx3DOperationPanel* getOperationsPanel ( );

		/**
		 * Affiche le "panneau d'édition de l'opération courante" transmis en
		 * argument.
		 * \see			getOperationsPanel
		 */
		virtual void setOperationsPanel (QtMgx3DOperationPanel* panel);

	//@}	// Les familles d'opérations.

	/**
	 * Les opérations.
	 */
	//@{

	/**
	 * Les types d'actions, classées selon leurs sous-familles.
	 */
	enum OPERATION_TYPES {
			GEOM_POINT_OPERATION,
			GEOM_CURVE_OPERATION,
			GEOM_SURFACE_OPERATION,
			GEOM_VOLUME_OPERATION,
			TOPO_VERTEX_OPERATION,
			TOPO_EDGE_OPERATION,
			TOPO_FACE_OPERATION,
			TOPO_BLOCK_OPERATION,
			MESH_CLOUD_OPERATION,
			MESH_SURFACE_OPERATION,
			MESH_VOLUME_OPERATION,
			SYSCOORD_OPERATION
		};

		/**
		 * Recense l'action transmise en argument dans la barre d'outils
		 * représentative du type transmis en second argument.
		 * Cette action est adoptée, et sa destruction est donc prise en charge
		 * par cette instance.
		 * \see		addSeparator
		 */
		virtual void registerOperationAction (QtMgx3DOperationAction& action, OPERATION_TYPES type);

		/**
		 * Ajoute un séparateur dans la barre d'outils représentative du type
		 * transmis en argument.
		 */	
		virtual void addSeparator (OPERATION_TYPES type);

	//@}	// Les opérations.

	/**
	 * Des panneaux d'opération additionnelles peuvent être affichés, par
	 * exemple à des fins d'exploration du maillage (explorateur de feuillets,
	 * histogrammes de qualité).
	 */
	//@{

	/**
	 * Enregistre le panneau d'opération transmis en argument.
	 * \warning		ATTENTION, passer par la fonction de même nom de la classe
	 *				<I>QtMgx3DMainWindow</I>.
	 */
	virtual void registerAdditionalOperationPanel (QtMgx3DOperationPanel& panel);

	/**
	 * Désenregistre le panneau d'opération transmis en argument.
	 * \warning		ATTENTION, passer par la fonction de même nom de la classe
	 *				<I>QtMgx3DMainWindow</I>.
	 */
	virtual void unregisterAdditionalOperationPanel (QtMgx3DOperationPanel& panel);

	//@}	// Les opérations additionnelles

	/**
	 * La gestion des informations à remonter à l'utilisateur.
	 */
	//@{

	/**
	 * \param		Flux d'informations à utiliser.
	 * \see			log
	 */
	virtual void setLogStream (TkUtil::LogOutputStream* stream);

	/**
	 * \param		Ecrit le log transmis en argument dans le flux
	 *				d'informations associé.
	 */
	virtual void log (const TkUtil::Log& log);

	//@}	// La gestion des informations à remonter à l'utilisateur.

	/**
	 * Création de l'IHM (aspects purements Qt).
	 */
	virtual void createGui ( );


	protected :

	/**
	 * Recense la barre d'outils transmise en argument dans la barre d'outils
	 * représentative du type transmis en second argument.
	 */
		virtual void registerSubFamilyToolBar (QtOperationsSubFamilyToolBar& toolBar, OPERATION_TYPES type);

	/**
	 * @return		La barre d'outils correspondant au type d'action transmis en
	 *				argument.
	 * \see			registerSubFamilyToolBar
	 */
	virtual QtOperationsSubFamilyToolBar& getToolbar (OPERATION_TYPES type);


	protected slots :

	/**
	 * Méthode appelée lorsque l'utilisateur clique sur la case à cocher
	 * "Afficher le panneau opération".
	 */
	virtual void displayOperationPanelCallback ( );

	/**
	 * Réinitialise le panneau indépendamment du contexte actuel.
	 * Remplace autoUpdateCallback au niveau du bouton Réinitialiser.
	 */
	virtual void resetCallback ( );

	/**
	 * Méthode appelée lorsque l'utilisateur clique sur le bouton
	 * "Réinitialiser".
	 * Valide la saisie effectuée et exécute l'opération le cas échéant.
	 * \warning	Obsolète depuis la version du 15/01/18, remplacée par
	 * 			resetCallback.
	 */
	virtual void autoUpdateCallback ( );

	/**
	 * Méthode appelée lorsque l'utilisateur clique sur le bouton "Appliquer".
	 * Valide la saisie effectuée et exécute l'opération le cas échéant en
	 * invocant la méthode <I>applyCallback</I> du panneau d'ópération actif.
	 */
	virtual void applyCallback ( );

	/**
	 * Méthode appelée lorsque l'utilisateur clique sur le bouton "Aide".
	 * Affiche l'aide contextuelle du panneau.
	 * \see		QtMgx3DOperationPanel#helpURL
	 */
	virtual void helpCallback ( );


	private :

	// Opérations interdites :
	QtMgx3DOperationsPanel (const QtMgx3DOperationsPanel&);
	QtMgx3DOperationsPanel& operator = (const QtMgx3DOperationsPanel&);

	/** Le titre de l'application (pour les éventuels message d'erreur). */
	std::string								_appTitle;

	/** L'éventuel flux sortant d'informations. */
	TkUtil::LogOutputStream*				_logStream;

	/** Le context d'utilisation Magix 3D. */
	Mgx3D::QtComponents::QtMgx3DMainWindow*	_mainWindow;

	/** La barre d'icônes "Familles d'opérations". */
	QtOperationsFamiliesToolBar*			_operationsFamiliesToolBar;

	/** La barre d'icônes "Famille d'opérations" courrante. */
	QtOperationsFamilyToolBar*				_currentFamilyToolBar;

	/** La barre d'icônes "Sous-famille d'opérations" courrante. */
	QtOperationsSubFamilyToolBar*			_currentSubFamilyToolBar;

	/** Les barres d'outils "Opérations". */
	std::map<OPERATION_TYPES, QtOperationsSubFamilyToolBar*>
											_operationsToolbars;

	/** Le conteneur des panneaux d'opérations et d'explorations. */
	QTabWidget*								_operationsTabWidget;

	/** La zone "Panneau d'édition de l'opération courante". */
	QWidget*								_currentOperationAreaWidget;

	/** Le conteneur de "Panneau d'édition de l'opération courante". */
	QScrollArea*							_operationPanelScrollArea;

	/** Le "Panneau d'édition de l'opération courante". */
	QtMgx3DOperationPanel*					_operationPanel;

	/** Le bouton "Réinitialiser". */
	QPushButton*							_reinitializeButton;

	/** Le bouton "Appliquer". */
	QPushButton*							_applyButton;

	/** Faut-ill montrer le panneau opération ? */
	QCheckBox*								_showOperationPanelCheckbox;
};	// class QtMgx3DOperationsPanel


/**
 * Classe de sous-panneau de base offrant un support pour les mécanismes de
 * <B>previsualisation</B>.
 */
class QtMgx3DOperationsSubPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMgx3DOperationsSubPanel ( );

	/**
	 * Arrête (éventuellement) la sélection interactive en cours.
	 * Méthode à surcharger si nécessaire, ne fait rien par défaut.
	 */
	virtual void stopSelection ( );

	/**
	 * \return	En cas de panneau de rattachement renvoit <I>previewResult</I>
	 *			de ce panneau, sinon renvoit la valeur de la classe parente.
	 */
	virtual bool previewResult ( ) const;

	/**
	 * \return	La liste des entités du panneau impliquées dans l'opération.
	 * 			Par défaut retourne <I>getInvolvedEntities</I>.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getPanelEntities ( );


	protected :

	/**
	 * Constructeur.
	 * \param		Widget parent
	 * \param		Fenêtre principale, permettant l'accès au contexte et à la
	 * 				fenêtre graphique.
	 * \param		Eventuel panneau de rattachement. Permet de récupérer par
	 * 				exemple certaines informations t.q. "est on en mode
	 * 				preview ?".
	 */
	QtMgx3DOperationsSubPanel (
						QWidget* parent, QtMgx3DMainWindow& mainWindow,
						QtMgx3DOperationPanel* mainPanel = 0);

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMgx3DOperationsSubPanel (const QtMgx3DOperationsSubPanel&);
	QtMgx3DOperationsSubPanel& operator = (const QtMgx3DOperationsSubPanel&);

	/**
	 * @return		L'éventuel panneau de rattachement.
	 */
	virtual const QtMgx3DOperationPanel* getMainPanel ( ) const;
	virtual QtMgx3DOperationPanel* getMainPanel ( );

	/**
	 * La gestion des entités sélectionnées.
	 *
	 * Méthodes callback permettant de remonter au panneau principal des
	 * évènements liés à la sélection d'entités au sein d'un sous panneau.
	 * L'idée est ici de connecter des instances de <I>EntitySeizureManager</I>
	 * à ces callbacks qui propagent l'évènement au panneau principal. Cela
	 * requiert bien sûr que le panneau principal s'abonne à ces évènements.
	 */
	//@{

	signals :

	 void entitiesAddedToSelection (QString entitiesNames);
	 void entitiesRemovedFromSelection (QString entitiesNames);


	protected slots :

	/**
	 * Emet le signal <I>entitiesAddedToSelection(QString)</I>.
	 */
	virtual void entitiesSelectedCallback (QString);

	/**
	 * Emet le signal <I>entitiesRemovedFromSelection(QString)</I>.
	 */
	virtual void entitiesUnselectedCallback (QString);

	private :

	/** Eventuel panneau de rattachement. */
	QtMgx3DOperationPanel*			_mainPanel;
	//@}	//  La gestion des entités sélectionnées.
};	// class QtMgx3DOperationsSubPanel


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MGX3D_OPERATIONS_PANEL_H
