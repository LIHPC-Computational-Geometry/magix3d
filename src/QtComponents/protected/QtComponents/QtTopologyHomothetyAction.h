/**
 * \file		QtTopologyHomothetyAction.h
 * \author		Charles PIGNEROL
 * \date		26/05/2014
 */
#ifndef QT_TOPOLOGY_HOMOTHETY_ACTION_H
#define QT_TOPOLOGY_HOMOTHETY_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DHomothetyPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include "Utils/SelectionManagerIfc.h"

#include <QCheckBox>
#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{

// ===========================================================================
//                   LA CLASSE QtTopologyHomothetyPanel
// ===========================================================================

/**
 * Panneau d'édition des paramètres d'homothétie d'entités topologiques.
 */
class QtTopologyHomothetyPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * Le type d'homothétie.
	 */
	enum HOMOTHETY_TYPE { HOMOGENEOUS, HETEROGENEOUS };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyHomothetyPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtTopologyHomothetyPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * \return		Le type d'homothétie.
	 */
	virtual HOMOTHETY_TYPE getHomothetyType ( ) const;

	/**
	 * \param		Nouvelle dimension de saisie des entités topologiques.
	 */
	virtual void setDimension (Mgx3D::Utils::SelectionManagerIfc::DIM dim);

	/**
	 * \return		La liste des entités topologiques devant effectuer une
	 *				homothétie.
	 * \see			getHomothetyFactor
	 * \see			doPropagate
	 */
	virtual std::vector<std::string> getTopoEntitiesNames ( ) const;

	/**
	 * \return		<I>true</I> s'il faut également faire effectuer L'homothétie
	 *				aux entités géométriques associées.
	 * \see			getTopoEntitiesNames
	 */
	virtual bool doPropagate ( ) const;

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
	 * \return		Le panneau de saisie des paramètres d'une homothétie
	 * 				homogène.
	 */
	virtual const QtMgx3DHomogeneousHomothetyPanel&
											getHomogeneousPanel ( ) const;

	/**
	 * \return		Le panneau de saisie des paramètres d'une homothétie
	 * 				hétérogène.
	 */
	virtual const QtMgx3DHeterogeneousHomothetyPanel&
											getHeterogeneousPanel ( ) const;


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

	/**
	 * \return		Le panneau courant de paramétrage de l'homothétie.
	 */
	virtual QtMgx3DOperationsSubPanel& getParametersPanel ( ) const;


	protected slots :

	/**
	 * Le changement de type d'homothétie. Actualisation du panneau.
	 */
	virtual void operationMethodCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyHomothetyPanel (const QtTopologyHomothetyPanel&);
	QtTopologyHomothetyPanel& operator = (
										const QtTopologyHomothetyPanel&);

	/** Les dimensions possibles pour les entités topologiques suivant
	 * l'homothétie. */
	QtEntityByDimensionSelectorPanel*			_topoEntitiesPanel;

	/** Case à cocher <I>propager</I>. */
	QCheckBox*									_propagateCheckBox;

	/** Le type d'homothétie. */
	QComboBox*									_operationMethodComboBox;

	/** Le paramétrage de l'homothétie. */
	QtMgx3DOperationsSubPanel*					_currentPanel;

	/** Paramétrage d'une homothétie homogène. */
	QtMgx3DHomogeneousHomothetyPanel*			_homogeneousPanel;

	/** Paramétrage d'une homothétie hétérogène. */
	QtMgx3DHeterogeneousHomothetyPanel*			_heterogeneousPanel;
};	// class QtTopologyHomothetyPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyHomothetyPanel</I> d'homothétie d'entités topologiques.
 */
class QtTopologyHomothetyAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyHomothetyPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyHomothetyAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyHomothetyAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres d'homothétie d'entités
	 *				topologiques.
	 */
	virtual QtTopologyHomothetyPanel* getHomothetyPanel ( );

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
	QtTopologyHomothetyAction (const QtTopologyHomothetyAction&);
	QtTopologyHomothetyAction& operator = (
									const QtTopologyHomothetyAction&);
};  // class QtTopologyHomothetyAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_HOMOTHETY_ACTION_H
