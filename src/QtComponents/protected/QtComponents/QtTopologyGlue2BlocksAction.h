/**
 * \file		QtTopologyGlue2BlocksAction.h
 * \author		Charles PIGNEROL
 * \date		09/12/2014
 */
#ifndef QT_TOPOLOGY_GLUE_2_BLOCKS_ACTION_H
#define QT_TOPOLOGY_GLUE_2_BLOCKS_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de collage de 2 blocs topologiques
 * entre-eux.
 */
class QtTopologyGlue2BlocksPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * La méthode de collage : faces communes de 2 blocs topologiques,
	 * blocs topologiques issus de volumes.
	 */
	enum GLUE_METHOD { COMMON_FACES, VOLUMES };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyGlue2BlocksPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyGlue2BlocksPanel ( );

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
	 * \return		Le nom du premier bloc à coller.
	 * \see			getBlock2Name
	 */
	virtual std::string getBlock1Name ( ) const;

	/**
	 * \return		Le nom du second bloc à coller.
	 * \see			getBlock1Name
	 */
	virtual std::string getBlock2Name ( ) const;

	/**
 	 * \return		La méthode de collage.
 	 */
	virtual GLUE_METHOD getGlueMethod ( ) const;


	protected slots :

	/**
	 * Appelé lorsque la méthode change. Actualise l'IHM.
	 */
	virtual void methodChangedCallback ( );


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
	QtTopologyGlue2BlocksPanel (const QtTopologyGlue2BlocksPanel&);
	QtTopologyGlue2BlocksPanel& operator = (const QtTopologyGlue2BlocksPanel&);

	/** Les blocs à coller. */
	QtMgx3DEntityPanel*			_block1Panel;
	QtMgx3DEntityPanel*			_block2Panel;

	/** L'algorithme utilisé. */
	QCheckBox*					_methodCheckBox;
};	// class QtTopologyGlue2BlocksPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyGlue2BlocksPanel</I> de collage de 2 blocs topologiques
 * entre-eux.
 */
class QtTopologyGlue2BlocksAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyGlue2BlocksPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyGlue2BlocksAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyGlue2BlocksAction ( );

	/**
	 * \return		Le panneau de paramétrage du collage.
	 */
	virtual QtTopologyGlue2BlocksPanel* getTopologyGlue2BlocksPanel ( );

	/**
	 * Colle les blocs topologiques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyGlue2BlocksAction (const QtTopologyGlue2BlocksAction&);
	QtTopologyGlue2BlocksAction& operator = (
									const QtTopologyGlue2BlocksAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_GLUE_2_BLOCKS_ACTION_H
