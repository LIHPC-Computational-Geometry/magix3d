/**
 * \file		QtGeomEntityDestructionAction.h
 * \author		Charles PIGNEROL
 * \date		22/01/2014
 */
#ifndef QT_GEOM_ENTITY_DESTRUCTION_ACTION_H
#define QT_GEOM_ENTITY_DESTRUCTION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"

#include "Utils/SelectionManagerIfc.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de destruction d'entités géométriques.
 */
class QtGeomEntityDestructionPanel : public QtMgx3DOperationPanel
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
	QtGeomEntityDestructionPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGeomEntityDestructionPanel ( );

	/**
	 * \return		La liste des entités géométriques à détruire.
	 * \see			doPropagate
	 */
	virtual std::vector<std::string> getGeomEntitiesNames ( ) const;

	/**
	 * \return		<I>true</I> s'il faut également supprimer les entités
	 *				géométriques de dimensions inférieures.
	 * \see			getGeomEntitiesNames
	 */
	virtual bool doPropagate ( ) const;

	/**
	 * \return		<I>true</I> s'il faut également supprimer les entités
	 *				topologiques associées aux entités géométriques détruites.
	 */
	virtual bool withTopo ( ) const;

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

	/**
	 * \return      Le panneau de sélection par dimension
	 */
	virtual QtEntityByDimensionSelectorPanel* getEntityByDimensionSelectorPanel ( );


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


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtGeomEntityDestructionPanel (const QtGeomEntityDestructionPanel&);
	QtGeomEntityDestructionPanel& operator = (
										const QtGeomEntityDestructionPanel&);

	/** Les dimensions possibles pour les entités géométriques à détruire. */
	QtEntityByDimensionSelectorPanel*			_geomEntitiesPanel;

	/** Case à cocher <I>propager</I>. */
	QCheckBox*									_propagateCheckBox;

	/** Case à cocher <I>détruit la topologie associée</I>. */
	QCheckBox*									_withTopoCheckBox;


};	// class QtGeomEntityDestructionPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeomEntityDestructionPanel</I> de destruction d'entités géométriques.
 */
class QtGeomEntityDestructionAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeomEntityDestructionPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtGeomEntityDestructionAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeomEntityDestructionAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de destruction d'entités
	 *				géométriques.
	 */
	virtual QtGeomEntityDestructionPanel* getDestructionPanel ( );

	/**
	 * Actualise le paramétrage des entités géométriques avec le paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGeomEntityDestructionAction (const QtGeomEntityDestructionAction&);
	QtGeomEntityDestructionAction& operator = (
									const QtGeomEntityDestructionAction&);
};  // class QtGeomEntityDestructionAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOM_ENTITY_DESTRUCTION_ACTION_H
