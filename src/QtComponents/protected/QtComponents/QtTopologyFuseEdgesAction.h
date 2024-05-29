/**
 * \file		QtTopologyFuseEdgesAction.h
 * \author		Charles PIGNEROL
 * \date		25/03/2014
 */
#ifndef QT_TOPOLOGY_FUSE_EDGES_ACTION_H
#define QT_TOPOLOGY_FUSE_EDGES_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de collage d'arêtes topologiques
 * (d'une même surface) entre-elles.
 */
class QtTopologyFuseEdgesPanel : public QtMgx3DOperationPanel
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
	QtTopologyFuseEdgesPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyFuseEdgesPanel ( );

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
	 * \return		Les noms des arêtes à coller.
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
	QtTopologyFuseEdgesPanel (const QtTopologyFuseEdgesPanel&);
	QtTopologyFuseEdgesPanel& operator = (const QtTopologyFuseEdgesPanel&);

	/** Les arêtes à coller. */
	QtMgx3DEntityPanel*			_edgesPanel;
};	// class QtTopologyFuseEdgesPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyFuseEdgesPanel</I> de collage d'arêtes topologiques
 * entre-elles.
 */
class QtTopologyFuseEdgesAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyFuseEdgesPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyFuseEdgesAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyFuseEdgesAction ( );

	/**
	 * \return		Le panneau de paramétrage du collage.
	 */
	virtual QtTopologyFuseEdgesPanel* getTopologyFuseEdgesPanel ( );

	/**
	 * Colle les arêtes topologiques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyFuseEdgesAction (const QtTopologyFuseEdgesAction&);
	QtTopologyFuseEdgesAction& operator = (
									const QtTopologyFuseEdgesAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_FUSE_EDGES_ACTION_H
