/**
 * \file		QtTopologySplitBlockWithOGridAction.h
 * \author		Charles PIGNEROL
 * \date		14/03/2014
 */
#ifndef QT_TOPOLOGY_SPLIT_BLOCK_WITH_OGRID_ACTION_H
#define QT_TOPOLOGY_SPLIT_BLOCK_WITH_OGRID_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QtUtil/QtDoubleTextField.h>
#include <QtUtil/QtIntTextField.h>

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de découpe de blocs topologiques avec
 * o-grid.
 */
class QtTopologySplitBlockWithOGridPanel : public QtMgx3DOperationPanel
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
	QtTopologySplitBlockWithOGridPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologySplitBlockWithOGridPanel ( );

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
	 * \return		Les noms des blocs à découper.
	 * \see			getFacesNames
	 */
	virtual std::vector<std::string> getBlocksNames ( ) const;

	/**
	 * \return		Les noms des faces à découper.
	 * \see			getBlocksNames
	 */
	virtual std::vector<std::string> getFacesNames ( ) const;

	/**
	 * \return		Le ratio pour positionner l'o-grid au centre.
	 */
	virtual double getRatio ( ) const;

	/**
	 * \return		Le nombre de bras sur les arêtes qui font le tour des blocs
	 * 				centraux.
	 */
	virtual size_t getCentralEdgesNumber ( ) const;

	/**
	 * \param		<I>true</I> pour prévisualiser les entités concernées par
	 * 				le découpage, <I>false</I> pour arrêter la prévisualisation.
	 * \param		<I>true</I> s'il faut détruire l'éventuel interacteur dans 
	 * 				le cas d'un arrêt de la prévisualisation.
	 */
	virtual void preview (bool show, bool destroyInteractor);

	/** retourna la valeur de _propagateNeighborBlocks */
	virtual bool propagate() const;

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
	QtTopologySplitBlockWithOGridPanel (
							const QtTopologySplitBlockWithOGridPanel&);
	QtTopologySplitBlockWithOGridPanel& operator = (
							const QtTopologySplitBlockWithOGridPanel&);

	/** Les blocs à découper. */
	QtMgx3DEntityPanel*			_blocksPanel;

	/** Les faces à découper. */
	QtMgx3DEntityPanel*			_facesPanel;

	/** Propage ou non le découpage aux blocs adjacents via les faces sélectionnées */
	QCheckBox*					_propagateNeighborBlocks;

	/** Le ratio de découpage du o-grid. */
	QtDoubleTextField*			_ratioTextField;

	/** Le nombre d'arêtes qui font le tour des blocs centraux du o-grid. */
	QtIntTextField*				_edgesNumTextField;
};	// class QtTopologySplitBlockWithOGridPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologySplitBlockWithOGridPanel</I> de découpe de bloc topologique.
 */
class QtTopologySplitBlockWithOGridAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologySplitBlockWithOGridPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologySplitBlockWithOGridAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologySplitBlockWithOGridAction ( );

	/**
	 * \return		Le panneau de paramétrage de la découpe.
	 */
	virtual QtTopologySplitBlockWithOGridPanel* getTopologySplitBlockWithOGridPanel ( );

	/**
	 * Découpe le bloc topologique conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologySplitBlockWithOGridAction (const QtTopologySplitBlockWithOGridAction&);
	QtTopologySplitBlockWithOGridAction& operator = (
									const QtTopologySplitBlockWithOGridAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_SPLIT_BLOCK_WITH_OGRID_ACTION_H
