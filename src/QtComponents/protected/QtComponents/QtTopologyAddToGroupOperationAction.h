/**
 * \file		QtTopologyAddToGroupOperationAction.h
 * \author		Charles PIGNEROL
 * \date		11/05/2015
 */
#ifndef QT_TOPOLOGY_ADD_TO_GROUP_OPERATION_ACTION_H
#define QT_TOPOLOGY_ADD_TO_GROUP_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"

#include "Utils/SelectionManagerIfc.h"

#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres d'addition/suppression/affectation
 * d'entités topologiques.
 */
class QtTopologyAddToGroupOperationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * L'opération a effectuer : ajouter/enlever/affecter les entités au groupe
	 * spécifié.
	 */
	enum OPERATION { ADD, REMOVE, SET };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyAddToGroupOperationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtTopologyAddToGroupOperationPanel ( );

	/**
	 * \return		Le nom du groupe concerné, éventuellement créé.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		L'opération à effectuer.
	 */
	virtual OPERATION getOperation ( ) const;

	/**
	 * \return		La dimension des entités sélectionnées
	 * \see			setDimension
	 */
	virtual Mgx3D::Utils::SelectionManagerIfc::DIM getDimension ( ) const;

	/**
	 * \param		Nouvelle dimension de saisie des entités topologiques.
	 * \see			getDimension
	 */
	virtual void setDimension (Mgx3D::Utils::SelectionManagerIfc::DIM dim);

	/**
	 * \return		La liste des entités topologiques à ajouter.
	 */
	virtual std::vector<std::string> getTopoEntitiesNames ( ) const;

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
     * Invoque <I>preview (false)</I>.
	 * \see		autoUpdate
	 * \see		preview
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true)</I>.
	 * \see		cancel
	 * \see		preview
	 */
	virtual void autoUpdate ( );


	protected :

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );


	protected slots :

	/**
	 * Appelé lorsque la dimension des groupes sélectionnés est modifiée.
	 * Actualise la liste des noms de groupe.
	 */
	virtual void dimensionsModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyAddToGroupOperationPanel (
									const QtTopologyAddToGroupOperationPanel&);
	QtTopologyAddToGroupOperationPanel& operator = (
									const QtTopologyAddToGroupOperationPanel&);

	/** Le nom du groupe créé. */
	QtMgx3DGroupNamePanel*						_namePanel;

	/** L'opération à effectuer. */
	QComboBox*									_operationComboBox;

	/** Les entités topologiques à ajouter. */
	QtEntityByDimensionSelectorPanel*			_topoEntitiesPanel;
};	// class QtTopologyAddToGroupOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyAddToGroupOperationPanel</I> d'addition d'entités topologiques.
 */
class QtTopologyAddToGroupOperationAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyAddToGroupOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyAddToGroupOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyAddToGroupOperationAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres d'addition d'entités
	 *				topologiques.
	 */
	virtual QtTopologyAddToGroupOperationPanel* getAdditionPanel ( );

	/**
	 * Actualise le paramétrage des entités topologiques avec le paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyAddToGroupOperationAction (
								const QtTopologyAddToGroupOperationAction&);
	QtTopologyAddToGroupOperationAction& operator = (
								const QtTopologyAddToGroupOperationAction&);
};  // class QtTopologyAddToGroupOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_ADD_TO_GROUP_OPERATION_ACTION_H
