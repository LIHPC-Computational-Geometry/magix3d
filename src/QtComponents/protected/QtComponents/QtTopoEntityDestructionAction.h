/**
 * \file		QtTopoEntityDestructionAction.h
 * \author		Charles PIGNEROL
 * \date		22/01/2014
 */
#ifndef QT_TOPO_ENTITY_DESTRUCTION_ACTION_H
#define QT_TOPO_ENTITY_DESTRUCTION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"

#include "Utils/SelectionManagerIfc.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de destruction d'entités topologiques.
 */
class QtTopoEntityDestructionPanel : public QtMgx3DOperationPanel
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
	QtTopoEntityDestructionPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtTopoEntityDestructionPanel ( );

	/**
	 * \return		La liste des entités topologiques à détruire.
	 * \see			doPropagate
	 */
	virtual std::vector<std::string> getTopoEntitiesNames ( ) const;

	/**
	 * \return		<I>true</I> s'il faut également supprimer les entités
	 *				topologiques de dimensions inférieures.
	 * \see			getTopoEntitiesNames
	 */
	virtual bool doPropagate ( ) const;

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
     * Invoque <I>preview (false, true)</I>.
	 * \see		autoUpdate
	 * \see		preview
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true, true)</I>.
	 * \see		cancel
	 * \see		preview
	 */
	virtual void autoUpdate ( );

	/**
	 * \param		Nouvelles dimensions.
	 */
	virtual void setDimensions (Mgx3D::Utils::SelectionManagerIfc::DIM dims);


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
	QtTopoEntityDestructionPanel (const QtTopoEntityDestructionPanel&);
	QtTopoEntityDestructionPanel& operator = (
										const QtTopoEntityDestructionPanel&);

	/** Les dimensions possibles pour les entités topologiques à détruire. */
	QtEntityByDimensionSelectorPanel*			_topoEntitiesPanel;

	/** Case à cocher <I>propager</I>. */
	QCheckBox*									_propagateCheckBox;
};	// class QtTopoEntityDestructionPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopoEntityDestructionPanel</I> de destruction d'entités topologiques.
 */
class QtTopoEntityDestructionAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopoEntityDestructionPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopoEntityDestructionAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopoEntityDestructionAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de destruction d'entités
	 *				topologiques.
	 */
	virtual QtTopoEntityDestructionPanel* getDestructionPanel ( );

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
	QtTopoEntityDestructionAction (const QtTopoEntityDestructionAction&);
	QtTopoEntityDestructionAction& operator = (
									const QtTopoEntityDestructionAction&);
};  // class QtTopoEntityDestructionAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPO_ENTITY_DESTRUCTION_ACTION_H
