/**
 * \file		QtTopologyFuse2EdgesAction.h
 * \author		Charles PIGNEROL
 * \date		28/06/2016
 */
#ifndef QT_TOPOLOGY_FUSE_2_EDGES_ACTION_H
#define QT_TOPOLOGY_FUSE_2_EDGES_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de fusion de 2 arêtes topologiques
 * entre-elles.
 */
class QtTopologyFuse2EdgesPanel : public QtMgx3DOperationPanel
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
	QtTopologyFuse2EdgesPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyFuse2EdgesPanel ( );

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
	 * \return		Le nom des premières arêtes à coller.
	 * \see			getEdges2Names
	 */
	virtual std::vector<std::string> getEdges1Names ( ) const;

	/**
	 * \return		Le nom des secondes arêtes à coller.
	 * \see			getEdges1Names
	 */
	virtual std::vector<std::string> getEdges2Names ( ) const;


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
	QtTopologyFuse2EdgesPanel (const QtTopologyFuse2EdgesPanel&);
	QtTopologyFuse2EdgesPanel& operator = (const QtTopologyFuse2EdgesPanel&);

	/** Les arêtes à coller. */
	QtMgx3DEntityPanel*			_edges1Panel;
	QtMgx3DEntityPanel*			_edges2Panel;
};	// class QtTopologyFuse2EdgesPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyFuse2EdgesPanel</I> de collage de 2 arêtes topologiques
 * entre-elles.
 */
class QtTopologyFuse2EdgesAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyFuse2EdgesPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyFuse2EdgesAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyFuse2EdgesAction ( );

	/**
	 * \return		Le panneau de paramétrage du collage.
	 */
	virtual QtTopologyFuse2EdgesPanel* getTopologyFuse2EdgesPanel ( );

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
	QtTopologyFuse2EdgesAction (const QtTopologyFuse2EdgesAction&);
	QtTopologyFuse2EdgesAction& operator = (
									const QtTopologyFuse2EdgesAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_FUSE_2_EDGES_ACTION_H
