/**
 * \file		QtTopologyFuse2BlocksAction.h
 * \author		Charles PIGNEROL
 * \date		22/06/2015
 */
#ifndef QT_TOPOLOGY_FUSE_2_BLOCKS_ACTION_H
#define QT_TOPOLOGY_FUSE_2_BLOCKS_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de fusion de 2 blocs topologiques
 * entre-eux.
 */
class QtTopologyFuse2BlocksPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyFuse2BlocksPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyFuse2BlocksPanel ( );

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
	 * \return		Le nom du premier bloc à fusionner.
	 * \see			getBlock2Name
	 */
	virtual std::string getBlock1Name ( ) const;

	/**
	 * \return		Le nom du second bloc à fusionner.
	 * \see			getBlock1Name
	 */
	virtual std::string getBlock2Name ( ) const;


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
	QtTopologyFuse2BlocksPanel (const QtTopologyFuse2BlocksPanel&);
	QtTopologyFuse2BlocksPanel& operator = (const QtTopologyFuse2BlocksPanel&);

	/** Les blocs à fusionner. */
	QtMgx3DEntityPanel*			_block1Panel;
	QtMgx3DEntityPanel*			_block2Panel;
};	// class QtTopologyFuse2BlocksPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyFuse2BlocksPanel</I> de fusion de 2 blocs topologiques
 * entre-eux.
 */
class QtTopologyFuse2BlocksAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyFuse2BlocksPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyFuse2BlocksAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyFuse2BlocksAction ( );

	/**
	 * \return		Le panneau de paramétrage de la fusion.
	 */
	virtual QtTopologyFuse2BlocksPanel* getTopologyFuse2BlocksPanel ( );

	/**
	 * Fusionne les blocs topologiques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyFuse2BlocksAction (const QtTopologyFuse2BlocksAction&);
	QtTopologyFuse2BlocksAction& operator = (
									const QtTopologyFuse2BlocksAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_FUSE_2_BLOCKS_ACTION_H
