/**
 * \file		QtTopologyEdgeCutAction.h
 * \author		Charles PIGNEROL
 * \date		03/02/2014
 */
#ifndef QT_TOPOLOGY_EDGE_CUT_ACTION_H
#define QT_TOPOLOGY_EDGE_CUT_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DEdgePanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QtUtil/QtDoubleTextField.h>

#include <QButtonGroup>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de découpage d'une arête topologique.
 */
class QtTopologyEdgeCutPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * Le positionnement de la coupe.
	 */
	enum CUT_DEFINITION_METHOD { CDM_RATIO, CDM_POINT };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyEdgeCutPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyEdgeCutPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
	 * Restitue l'environnement dans son état initial.
	 * Invoque <I>preview (false, true)</I>.
	 * \see		autoUpdate
	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true, true)</I>.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );

	/**
	 * \return	Le nom de l'arête à découper.
	 */
	virtual std::string getEdgeName ( ) const;

	/**
	 * \return	La méthode de définition de la coupe.
	 */
	virtual CUT_DEFINITION_METHOD getCutDefinitionMethod ( ) const;

	/**
	 * \return	Le ratio à appliquer lors de la découpe. Devrait être compris
	 * entre 0 et 1.
	 */
	virtual double getRatio ( ) const;

	/**
	 * \return	Le point de découpe.
	 */
	virtual Mgx3D::Utils::Math::Point getCutPoint ( ) const;

	/**
	 * \param		<I>true</I> pour prévisualiser les entités concernées par
	 * 				le découpage, <I>false</I> pour arrêter la prévisualisation.
	 * \param		<I>true</I> s'il faut détruire l'éventuel interacteur dans
	 * 				le cas d'un arrêt de la prévisualisation.
	 */
	virtual void preview (bool show, bool destroyInteractor);


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
	 * Appelé lorsque l'utilisateur change de méthode de définition de la
	 * découpe. Actualise l'IHM.
	 */
	virtual void cutDefinitionModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyEdgeCutPanel (const QtTopologyEdgeCutPanel&);
	QtTopologyEdgeCutPanel& operator = (const QtTopologyEdgeCutPanel&);

	/** La saisie de l'arête. */
	QtMgx3DEdgePanel*			_edgePanel;

	/** La définition de la position de la découpe. */
	QButtonGroup*				_cutDefinitionButtonGroup;

	/** La saisie du ratio. */
	QtDoubleTextField*			_ratioTextField;

	/** L'entité de définition du positionnement de la coupe. */
	QtMgx3DEntityPanel*			_cutPointEntityPanel;
};	// class QtTopologyEdgeCutPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyEdgeCutPanel</I> de découpage d'arête.
 */
class QtTopologyEdgeCutAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyEdgeCutPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyEdgeCutAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyEdgeCutAction ( );

	/**
	 * \return	Le panneau de paramétrage de la découpe.
	 */
	virtual QtTopologyEdgeCutPanel* getEdgeCutPanel ( );

	/**
	 * Découpe l'arête topologique conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyEdgeCutAction (const QtTopologyEdgeCutAction&);
	QtTopologyEdgeCutAction& operator = (const QtTopologyEdgeCutAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_EDGE_CUT_ACTION_H
