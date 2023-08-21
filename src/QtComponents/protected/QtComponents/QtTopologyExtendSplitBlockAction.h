/**
 * \file		QtTopologyExtendSplitBlockAction.h
 * \author		Charles PIGNEROL
 * \date		10/03/2014
 */
#ifndef QT_TOPOLOGY_EXTEND_SPLIT_BLOCK_ACTION_H
#define QT_TOPOLOGY_EXTEND_SPLIT_BLOCK_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de prolongation de découpe d'un bloc
 * topologique.
 */
class QtTopologyExtendSplitBlockPanel : public QtMgx3DOperationPanel
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
	QtTopologyExtendSplitBlockPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyExtendSplitBlockPanel ( );

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
	 * \return		Le nom du bloc à découper.
	 * \see			getEdgeName
	 */
	virtual std::string getBlockName ( ) const;

	/**
	 * \return		Le nom de l'arête orthogonale au plan de coupe.
	 * \see			getBlockName
	 */
	virtual std::string getEdgeName ( ) const;

	/**
	 * \param		<I>true</I> pour prévisualiser les entités concernées par
	 * 				le découpage, <I>false</I> pour arrêter la prévisualisation.
	 * \param		<I>true</I> s'il faut détruire l'éventuel interacteur dans
	 * 				le cas d'un arrêt de la prévisualisation.
	 */
	virtual void preview (bool show, bool destroyInteractor);


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
	QtTopologyExtendSplitBlockPanel (const QtTopologyExtendSplitBlockPanel&);
	QtTopologyExtendSplitBlockPanel& operator = (
										const QtTopologyExtendSplitBlockPanel&);

	/** Le bloc à découper. */
	QtMgx3DEntityPanel*			_blockPanel;

	/** L'arête orthogonale au plan de coupe. */
	QtMgx3DEntityPanel*			_edgePanel;
};	// class QtTopologyExtendSplitBlockPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyExtendSplitBlockPanel</I> de découpe de bloc topologique.
 */
class QtTopologyExtendSplitBlockAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyExtendSplitBlockPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyExtendSplitBlockAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyExtendSplitBlockAction ( );

	/**
	 * \return		Le panneau de paramétrage de prolongation de la la découpe.
	 */
	virtual QtTopologyExtendSplitBlockPanel* getTopologyExtendSplitBlockPanel ( );

	/**
	 * Prolonge la découpe du bloc topologique conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyExtendSplitBlockAction (const QtTopologyExtendSplitBlockAction&);
	QtTopologyExtendSplitBlockAction& operator = (
									const QtTopologyExtendSplitBlockAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_EXTEND_SPLIT_BLOCK_ACTION_H
