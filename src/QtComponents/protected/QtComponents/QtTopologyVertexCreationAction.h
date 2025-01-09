/**
 * \file		QtTopologyVertexCreationAction.h
 * \author		Eric B
 * \date		15/3/2019
 */
#ifndef QT_TOPOLOGY_VERTEX_CREATION_ACTION_H
#define QT_TOPOLOGY_VERTEX_CREATION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtTopologyCreationAction.h"
#include "QtMgx3DPointPanel.h"

namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de création d'une arête topologique
 * par association à un point géométrique.
 */
class QtTopologyVertexCreationPanel : public QtMgx3DOperationPanel
{

    Q_OBJECT

	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyVertexCreationPanel (
			QWidget* parent, const std::string& panelName,
            QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyVertexCreationPanel ( );

	/**
	 * \return		Le nom de la courbe à projeter.
	 * \see			getSurfaceName
	 */
	virtual std::string getVertexName ( ) const;

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

    Utils::Math::Point getPoint ( ) const;

    std::string getGroupName() const;

    void updateCoordinates (const std::string& name);

    /**
 * \return			true si l'utilisateur a modifié une composante
 *					de la donnée représentée.
 */
    virtual bool isModified ( ) const;

    signals :

    /**
     * Signal émis lorsque l'utilisateur vient de finir une action de
     * modification du point.
     */
    void pointModified ( );

    /**
	 * Signal émis lorsqu'un point est ajoutée à la sélection.
	 * \param		Nom du point ajouté.
	 * \see			pointRemovedFromSelection
	 * \see			selectionModified
	 */
    void pointAddedToSelection (QString);

    /**
	 * Signal émis lorsqu'un point est enlevé de la sélection.
	 * \param		Nom du point enlevé.
	 * \see			pointAddedToSelection
	 * \see			selectionModified
	 */
    void pointRemovedFromSelection (QString);

    protected slots :

    void entitiesAddedToSelectionCallback (QString entitiesNames);

    /**
	 * Callback appelé lorsque l'utilisateur modifie les coordonnées du point.
	 * N'est pas appelé lorsque les coordonnées sont modifiées par le programme.
	 * => Efface le contenu du champ de texte contenant l'ID du vertex.
	 * Emet le signal <I>pointModified</I>.
	 */
    virtual void coordinatesEditedCallback ( );

    /**
     * Callback appelé lorsque les coordonnées du point sont modifiées
     * (utilisateur et/ou programme).
     * Emet le signal <I>pointModified</I>.
     */
    virtual void coordinatesModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyVertexCreationPanel (const QtTopologyVertexCreationPanel&);
	QtTopologyVertexCreationPanel& operator = (
							const QtTopologyVertexCreationPanel&);

	/** La courbe associée. */
	QtMgx3DEntityPanel*				_vertexPanel;

    /** Les valeurs initiales des trois composantes. */
    double				_initialX, _initialY, _initialZ;

    /** Les noms des types des composantes. */
    QLabel			*_xTextLabel, *_yTextLabel, *_zTextLabel;

    /** Les champs de saisie des composantes de la donnée représentée.
     */
    QtDoubleTextField	*_xTextField, *_yTextField, *_zTextField;

    /** Le sens de rotation. */
    QtMgx3DGroupNamePanel*			_namePanel;

};	// class QtTopologyVertexCreationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyVertexCreationPanel</I> de création d'arête topologique associée
 * à un point géométrique.
 */
class QtTopologyVertexCreationAction : public QtTopologyCreationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyVertexCreationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyVertexCreationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyVertexCreationAction ( );

	/**
	 * Créé le bloc topologique conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyVertexCreationAction (const QtTopologyVertexCreationAction&);
	QtTopologyVertexCreationAction& operator = (
									const QtTopologyVertexCreationAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_VERTEX_CREATION_ACTION_H
