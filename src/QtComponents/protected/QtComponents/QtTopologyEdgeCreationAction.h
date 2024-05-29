/**
 * \file		QtTopologyEdgeCreationAction.h
 * \author		Eric B
 * \date		15/3/2019
 */
#ifndef QT_TOPOLOGY_EDGE_CREATION_ACTION_H
#define QT_TOPOLOGY_EDGE_CREATION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtTopologyCreationAction.h"

namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de création d'une arête topologique
 * par association à une courbe géométrique.
 */
class QtTopologyEdgeCreationPanel : public QtMgx3DOperationPanel
{
	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyEdgeCreationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyEdgeCreationPanel ( );

	/**
	 * \return		Le nom de la courbe à projeter.
	 * \see			getSurfaceName
	 */
	virtual std::string getCurveName ( ) const;

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


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyEdgeCreationPanel (const QtTopologyEdgeCreationPanel&);
	QtTopologyEdgeCreationPanel& operator = (
							const QtTopologyEdgeCreationPanel&);

	/** La courbe associée. */
	QtMgx3DEntityPanel*				_curvePanel;

};	// class QtTopologyEdgeCreationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyEdgeCreationPanel</I> de création d'arête topologique associée
 * à une courbe géométrique.
 */
class QtTopologyEdgeCreationAction : public QtTopologyCreationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyEdgeCreationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyEdgeCreationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyEdgeCreationAction ( );

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
	QtTopologyEdgeCreationAction (const QtTopologyEdgeCreationAction&);
	QtTopologyEdgeCreationAction& operator = (
									const QtTopologyEdgeCreationAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_EDGE_CREATION_ACTION_H
