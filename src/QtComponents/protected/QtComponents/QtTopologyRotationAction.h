/**
 * \file		QtTopologyRotationAction.h
 * \author		Charles PIGNEROL
 * \date		22/05/2014
 */
#ifndef QT_TOPOLOGY_ROTATION_ACTION_H
#define QT_TOPOLOGY_ROTATION_ACTION_H


#include "QtComponents/QtMgx3DRotationPanel.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include "Utils/SelectionManagerIfc.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de rotation d'entités topologiques.
 */
class QtTopologyRotationPanel : public QtMgx3DOperationPanel
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
	QtTopologyRotationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtTopologyRotationPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * \param		Nouvelle dimension de saisie des entités topologiques.
	 */
	virtual void setDimension (Mgx3D::Utils::SelectionManagerIfc::DIM dim);

	/**
	 * \return		La liste des entités topologiques devant effectuer une
	 *				rotation.
	 * \see			getRotation
	 * \see			doPropagate
	 */
	virtual std::vector<std::string> getTopoEntitiesNames ( ) const;

	/**
	 * \return		La rotation à effectuer.
	 * \see			getTopoEntitiesNames
	 */
	virtual Utils::Math::Rotation getRotation ( ) const;

	/**
	 * \return		<I>true</I> s'il faut également faire effectuer la rotation
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
	QtTopologyRotationPanel (const QtTopologyRotationPanel&);
	QtTopologyRotationPanel& operator = (
										const QtTopologyRotationPanel&);

	/** Les dimensions possibles pour les entités topologiques suivant la
	 * rotation. */
	QtEntityByDimensionSelectorPanel*			_topoEntitiesPanel;

	/** Case à cocher <I>propager</I>. */
	QCheckBox*									_propagateCheckBox;

	/** Le panneau décrivant la rotation. */
	QtMgx3DRotationPanel*						_rotationPanel;
};	// class QtTopologyRotationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyRotationPanel</I> de rotation d'entités topologiques.
 */
class QtTopologyRotationAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyRotationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyRotationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyRotationAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de rotation d'entités
	 *				topologiques.
	 */
	virtual QtTopologyRotationPanel* getRotationPanel ( );

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
	QtTopologyRotationAction (const QtTopologyRotationAction&);
	QtTopologyRotationAction& operator = (
									const QtTopologyRotationAction&);
};  // class QtTopologyRotationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_ROTATION_ACTION_H
