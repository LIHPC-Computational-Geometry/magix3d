/**
 * \file		QtTopologyEdgeDirectionAction.h
 * \author		Charles PIGNEROL
 * \date		18/12/2014
 */
#ifndef QT_TOPOLOGY_EDGE_DIRECTION_ACTION_H
#define QT_TOPOLOGY_EDGE_DIRECTION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres d'orientation d'arêtes topologiques.
 */
class QtTopologyEdgeDirectionPanel : public QtMgx3DOperationPanel
{
	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyEdgeDirectionPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyEdgeDirectionPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
	 * Restitue l'environnement dans son état initial.
	 * Invoque <I>preview (false)</I>.
	 * \see	preview
	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true, true)</I>.
	 * \see	preview
	 */
	virtual void autoUpdate ( );

	/**
	 * \return		Les noms des arêtes à modifier.
	 */
	virtual std::vector<std::string> getEdgesNames ( ) const;


	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyEdgeDirectionPanel (const QtTopologyEdgeDirectionPanel&);
	QtTopologyEdgeDirectionPanel& operator = (
									const QtTopologyEdgeDirectionPanel&);

	/** Les arêtes à modifier. */
	QtMgx3DEntityPanel*			_edgesPanel;
};	// class QtTopologyEdgeDirectionPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyEdgeDirectionPanel</I> d'orientation d'arêtes topologiques.
 */
class QtTopologyEdgeDirectionAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyEdgeDirectionPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyEdgeDirectionAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyEdgeDirectionAction ( );

	/**
	 * \return		Le panneau de paramétrage de la modification.
	 */
	virtual QtTopologyEdgeDirectionPanel* getTopologyEdgeDirectionPanel ( );

	/**
	 * Modifie les arêtes topologiques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyEdgeDirectionAction (const QtTopologyEdgeDirectionAction&);
	QtTopologyEdgeDirectionAction& operator = (
									const QtTopologyEdgeDirectionAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_EDGE_DIRECTION_ACTION_H
