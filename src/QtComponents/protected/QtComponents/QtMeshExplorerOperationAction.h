/**
 * \file		QtMeshExplorerOperationAction.h
 * \author		Charles PIGNEROL
 * \date		29/01/2013
 */
#ifndef QT_MESH_EXPLORER_OPERATION_ACTION_H
#define QT_MESH_EXPLORER_OPERATION_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMeshSheetOperationAction.h"

#include <QtUtil/QtIntTextField.h>
#include <TkUtil/Mutex.h>

#include <QToolButton>

#include <memory>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau de paramètrage d'une opération d'exploration de maillage
 * le long de "feuillets".
 */
class QtMeshExplorerOperationPanel :
					public Mgx3D::QtComponents::QtMeshSheetOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Politique de création/modification du groupe
	 * \param	Arête proposée par défaut.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 * \param	Fichier d'aide contextuelle associé à ce panneau.
	 * \param	Balise dans le fichier d'aide contextuelle associé à ce panneau
	 *			identifiant le paragraphe sur ce panneau.
	 */
	QtMeshExplorerOperationPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			const std::string& edgeUID,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action,
			const std::string& helpURL, const std::string& helpTag);

	/**
	 * RAS.
	 */
	virtual ~QtMeshExplorerOperationPanel ( );

	/**
	 * Ne fait pas QtMgx3DOperationPanel::setVisible qui est invoque cancel
	 * ou autoUpdate (API opérations). Ici on reste à l'aspect visible ou non
	 * de la fenêtre et de son contenu.
	 * Cette surcharge est due au fait que dans un système d'onglets, quand on
	 * affiche un onglet celui qui était au premier plan devient non visible ...
	 * => appel de cancel, clear, ...
 	*/
	virtual void setVisible (bool visible);


	/**
	 * \return		Le pas de progression d'un feuillet à l'autre.
	 * \see			getDirection
	 */
	virtual long getEdgesStep ( ) const;

	/**
	 * \return		La direction de la progression d'un feuillet à l'autre
	 *				(+-1).
	 * \see			getEdgesStep
	 */
	virtual long getDirection ( ) const;

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Méthode appelée pour vérifier les paramètres saisis par l'utilisateur.
	 * En cas de paramètrage invalide cette méthode doit leve une exception de
	 * type <I>TkUtil::Exception</I> contenant un descriptif des erreurs
	 * rencontrées.
	 */
	virtual void validate ( );

    /**
     * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
     * Restitue l'environnement dans son état initial.
	 * \see		autoUpdate
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );


	protected slots :

	/**
	 * (Ré)affiche le feuillet. Le calcule avec les paramètres en cours s'il
	 * n'était pas masqué.
	 * \see		hideCallback
	 */
	virtual void applyCallback ( );

	/**
	 * Progresser en arrière. Callback lorsque l'utilisateur clique sur la
	 * flèche "arrière". Appelle <I>applyCallback</I>.
	 * \see		nextCallback
	 * \see		applyCallback
	 */
	virtual void previousCallback ( );

	/**
	 * Progresser en avant. Callback lorsque l'utilisateur clique sur la
	 * flèche "avant". Appelle <I>applyCallback</I>.
	 * \see		previousCallback
	 * \see		applyCallback
	 */
	virtual void nextCallback ( );

	/**
	 * Masquer le feuillet sans pour autant fermer le panneau (et donc perdre
	 * le paramétrage en cours).
	 * \see		applyCallback
	 * \see		closeCallback
	 */
	virtual void hideCallback ( );

	/**
	 * Fermeture du panneau, et destruction de l'ensemble de l'ihm et des
	 * éléments de maillage.
	 * \see		hideCallback
	 */
	virtual void closeCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtMeshExplorerOperationPanel (const QtMeshExplorerOperationPanel&);
	QtMeshExplorerOperationPanel& operator = (
										const QtMeshExplorerOperationPanel&);

	/** Le pas de progression d'un feuillet à l'autre. */
	QtIntTextField*			_edgesStepTextField;
	QToolButton				*_previousButton, *_nextButton;

	/** La direction : -1 (arrière) ou 1 (avant). */
	long					_direction;

	/** Afficher, Masquer, Réinitialiser, Appliquer, Aide. */
	QPushButton				*_displayButton, *_hideButton, *_reinitializeButton;

	/** Le feuillet est-il actuellement masqué ? */
	bool					_hidden;
};	// class QtMeshExplorerOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtMeshExplorerOperationPanel</I> de contrôle d'une exploration de
 * maillage type "feuillet".
 */
class QtMeshExplorerOperationAction :
					public Mgx3D::QtComponents::QtMeshSheetOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtMeshExplorerOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtMeshExplorerOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMeshExplorerOperationAction ( );

	/**
	 * \return		Le panneau d'édition de l'explorateur de maillage associé.
	 */
	virtual QtMeshExplorerOperationPanel* getMeshExplorerPanel ( );

	/**
	 * Créé/modifie le feuillet avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DMeshOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );

	/**
	 * \param		Liste des observateurs à informer des évolutions de
	 *				l'exploration.
	 */
	virtual void addObservers (
						const std::vector<TkUtil::ObjectBase*>& observers);


    /** Création de l'explorateur et destruction du précédent s'il existe
     *
     * \param   	withStep vrai si l'on décale l'observation, faux si on veut
     * 				juste accéder à l'exploration initiale
     */
	virtual void update ( bool withStep );

	/**
	 * Suspend (<I>false</I>) ou réactive (<I>true</I>) le déroulement de
	 * l'opération.
	 */
	virtual void setActivated (bool activated);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMeshExplorerOperationAction (const QtMeshExplorerOperationAction&);
	QtMeshExplorerOperationAction& operator = (
									const QtMeshExplorerOperationAction&);

	/** Les observateurs de l'exploration. */
	std::vector<TkUtil::ObjectBase*>		_observers;

	/** Un mutex non récursif pour empêcher toute réentrance.
	 * cf. executeOperation pour davantage d'explications. */
	TkUtil::Mutex							_mutex;
};  // class QtMeshExplorerOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MESH_EXPLORER_OPERATION_ACTION_H
