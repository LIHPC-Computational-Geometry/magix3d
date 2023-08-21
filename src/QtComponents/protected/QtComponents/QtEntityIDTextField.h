/**
 * \file		QtEntityIDTextField.h
 * \author		Charles PIGNEROL
 * \date		11/09/2013
 */

#ifndef QT_ENTITY_ID_TEXTFIELD_H
#define QT_ENTITY_ID_TEXTFIELD_H

#include "Internal/ContextIfc.h"

#include <QtComponents/EntitySeizureManager.h>
#include "Internal/Context.h"

#include <QtUtil/QtTextField.h>

#include <QMenu>
#include <string>
#include <vector>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * <P>Classe de champ de texte proposant un support de base pour saisir de
 * manière interactive une entité d'une dimension prédéfinie.
 * </P>
 *
 * <P>Passage en mode de saisie interactive (sélection, par exemple par clic
 * souris sur la vue graphique) par clic dans le champ de texte. Arrêt de la
 * saisie interactive par clic dans le champ de texte ou par la touche ESC
 * (champ de texte, vue graphique).
 * </P>
 *
 * <P>Dans une démarche de fluidifier une sélection faiant intervenir plusieurs
 * champs de texte (voir tous types de gestionnaires de saisie), ceux-ci peuvent
 * être ordonnés (notion de suivant).
 * </P>
 *
 * \see		EntitySeizureManager
 */
class QtEntityIDTextField : public QtTextField, public Mgx3D::QtComponents::EntitySeizureManager
{
	Q_OBJECT

	public :

	/** Constructeur
	 * \param		Widget parent
	 * \param		Fenêtre principale associée, pour la saisie de l'entité.
	 * \param		Dimensions possibles de l'entité à saisir.
	 * \param		Masque sur les types d'entités à saisir,
	 * \param		Nom du widget
	 * \see			EntitySeizureManager
	 */
	QtEntityIDTextField (
		QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow* mainWindow,
	    Mgx3D::Utils::SelectionManagerIfc::DIM dimensions,
		Mgx3D::Utils::FilterEntity::objectType types,
		char* name = 0);

	/**
	 * Destructeur. Quitte le mode de sélection interactive si nécessaire.
	 */
	virtual ~QtEntityIDTextField ( );

	/**
	 * Actualisation de la représentation de la sélection (enlève par exemple
	 * les entités détruites).
	 * \return	<I>true</I> en cas de modification, <I>false</I> dans le cas
	 * 			contraire.
	 */
	virtual bool actualizeGui (bool removeDestroyed);

	/**
	 * Passage (<I>true</I>) ou arrêt (<I>false</I>) du mode de saisie
	 * interactive. Invoque préalablement <I>updateGui</I> qui actualise le
	 * panneau selon <I>enable</I>.
	 */
	virtual void setInteractiveMode (bool enable);

	/**
	 * \return		La liste, non validée, des noms uniques d'entités figurant
	 *				dans le champ de texte.
	 * \see			setUniqueNames
	 */
	virtual std::vector<std::string> getUniqueNames ( ) const;

	/**
	 * \param		La liste des noms uniques d'entités figurant dans le champ
	 * 				de texte. Annule sélection et pré-sélection en cours.
	 * \see			getUniqueNames
	 * \warning		Ne fonctionne que si l'instance n'est pas en momde de
	 * 				saisie interactive.
	 */
	virtual void setUniqueNames (const std::vector<std::string>& names);

	/**
	 * Réinitialise le champ de saisie.
	 */
	virtual void reset ( );

	/**
	 * Appelé lorsque des entités sont (interactivement) ajoutées à la sélection.
	 * Actualise le champ de texte.
	 */
	virtual void entitiesAddedToSelection (const std::vector<std::string>& uniqueNames);

	/**
	 * Appelé lorsque des entités sont (interactivement) enlevées de la sélection.
	 * Actualise le champ de texte.
	 */
    virtual void entitiesRemovedFromSelection (const std::vector<std::string>& uniqueNames);

	/**
	 * Annule la sélection en cours, même en mode de sélection interactive.
	 */
	virtual void clearSelection ( );


	public slots :

	/**
	 * Ajoute à la liste d'entités celles qui sont sélectionnées dans la vue
	 * graphique.
	 */
	virtual void addGraphicalSelectionCallback ( );

	/**
	 * Ajoute à la sélection les entités des groupes sélectionnés dans le panneau
	 * "Groupes".
	 */
	virtual void addSelectedGroupsEntitiesCallback ( );


	signals :

	/**
	 * Signal émis lorsque la sélection est modifiée par l'utilisateur.
	 * \param		Chaine décrivant la nouvelle sélection.
	 */
	void selectionModified (QString);

	/**
	 * Signal émis lorsque des entités sont ajoutées à la sélection.
	 * \param		Nom des entités ajoutées.
	 * \see			entitiesRemovedFromSelection
	 * \see			selectionModified
	 */
	void entitiesAddedToSelection (QString);

	/**
	 * Signal émis lorsque des entités sont enlevées de la sélection.
	 * \param		Nom des entités enlevées.
	 * \see			entitiesAddedToSelection
	 * \see			selectionModified
	 */
	void entitiesRemovedFromSelection (QString);


	protected :

	/**
	 * Le panneau n'est plus affiché => on quitte le mode de saisie interactive.
	 */
	virtual void hideEvent (QHideEvent* event);

	/**
	 * Arrêt éventuel mode de sélection interactive par touche ESC dans la
	 * fenêtre graphique.
	 * \see		keyReleaseCallback
	 * \see		setInteractiveMode
	 */
	virtual void keyPressEvent (QKeyEvent* event);

	/**
 	 * Passage/arrêt mode de sélection interactive.
 	 */
	virtual void mousePressEvent (QMouseEvent* e);

	/**
	 * Affiche un menu contextuel.
	 */
	virtual void contextMenuEvent (QContextMenuEvent* event);

	/**
 	 * Actualise le panneau selon la valeur de <I>enable</I> (<I>true</I> : mode
 	 * de saisie interactif activé, <I>false</I> : mode de saisie interactif 
 	 * inactivé).
 	 * \see			setInteractiveMode
 	 */
	virtual void updateGui (bool enable);

	/**
	 * Appelé lorsque l'utilisateur a sélectionné de manière interactive une
	 * entité de la dimension souhaitée.
	 * Emet le signal <I>selectionModified</I>.
	 * \param		Entité sélectionnée
	 * \see			selectionModified
	 */
	virtual void setSelectedEntity (const Mgx3D::Utils::Entity& entity);

	/**
	 * Appelé lorsque
	 * l'utilisateur a sélectionné de manière interactive une liste d'entités de
	 * la dimension souhaitée, cette méthode est appelée avec en argument
	 * un vecteur d'entités.
	 * \see			selectionModified
	 */
	virtual void setSelectedEntities (const std::vector<Mgx3D::Utils::Entity*>);

	/**
	 * Appelé lorsque l'utilisateur ajoute des entités à la sélection.
	 * \param		Entités ajoutées à la sélection
	 * \see			removeFromSelection
	 * \see			setSelectedEntity
	 * \see			setSelectedEntities
	 */
	virtual void addToSelection (const std::vector<Mgx3D::Utils::Entity*>& entities);

	/**
	 * Appelé lorsque l'utilisateur enlève des entités de la sélection.
	 * \param		Entités enlevées de la sélection
	 * \see			addToSelection
	 * \see			setSelectedEntity
	 * \see			setSelectedEntities
	 */
	virtual void removeFromSelection (const std::vector<Mgx3D::Utils::Entity*>& entities);

	/**
	 * \return	Une référence sur le contexte <I>Magix 3D</I> associé à
	 *			l'opération.
	 */
	virtual const Mgx3D::Internal::ContextIfc& getContext ( ) const;
	virtual Mgx3D::Internal::ContextIfc& getContext ( );


	protected slots :

	/**
	 * Appelé lorsque le texte est modifié, par exemple par copié/collé.
	 * Enlève les éventuels caractères indésirables type virgule, guillemets et
	 * autres.
	 */
	virtual void filterText ( );

	/**
	 * Appelé lorsque l'utilisateur valide une sélection saisie dans le champ
	 * de texte. Emet le signal <I>selectionModified</I>.
	 * \see		selectionModified
	 */
	virtual void returnPressedCallback ( );

	/**
	 * Arrêt éventuel mode de sélection interactive par touche ESC dans la
	 * fenêtre graphique.
	 *\see	keyPressEvent
	 *\see	setInteractiveMode
	 */
	virtual void keyReleaseCallback (QKeyEvent* event);

	/**
	 * Appelé lorsque la sélection est modifiée via le champ de texte et suite à
	 * la perte du focus.
	 */
	virtual void textModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtEntityIDTextField (const QtEntityIDTextField&);
	QtEntityIDTextField& operator = (const QtEntityIDTextField&);

	/** Le contexte <I>Magix 3D</I> de l'opération. */
	Mgx3D::QtComponents::QtMgx3DMainWindow*			_mainWindow;

	/** Les actions Magix3D du menu contextuel. */
	QAction*						_addGraphicalSelectionAction;
	QAction*						_addGroupsEntitiesAction;

	/** Couleur d'origine de fond. */
	QColor							_backgroundColor;

	/** Faut-il bloquer l'actualisation du champ de saisie des IDs. Objectif :
	 * ne pas dupliquer les noms, par exemple, lorsque
	 * <I>setInteractiveMode (true)</I> est appelé. */
	bool							_updating;
};	// class QtEntityIDTextField


}	// namespace QtComponents

}	// namespace Mgx3D


#endif	// QT_ENTITY_ID_TEXTFIELD_H
