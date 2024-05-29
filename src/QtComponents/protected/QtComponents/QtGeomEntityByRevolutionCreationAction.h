/**
 * \file		QtGeomEntityByRevolutionCreationAction.h
 * \author		Charles PIGNEROL
 * \date		18/11/2014
 */
#ifndef QT_GEOM_ENTITY_BY_REVOLUTION_CREATION_ACTION_H
#define QT_GEOM_ENTITY_BY_REVOLUTION_CREATION_ACTION_H


#include "QtComponents/QtMgx3DRotationPanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"



namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau de création d'entité géométrique par révolution d'entité géométrique
 * de dimension inférieure.
 */
class QtGeomEntityByRevolutionCreationPanel : public QtMgx3DOperationPanel
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
	QtGeomEntityByRevolutionCreationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGeomEntityByRevolutionCreationPanel ( );

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return		Les entités soumises à la révolution.
	 */
	virtual std::vector<std::string> getEntities ( ) const;

	/**
	 * \return		La rotation.
	 */
	virtual Utils::Math::Rotation getRotation ( ) const;

	/**
	 * \return		<I>true</I> s'il faut conserver les entités initiales,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool keepEntities ( ) const;

	/**
	 * \param		La dimension de filtrage de l'entité soumise à la rotation.
	 */
	virtual void setDimension (Mgx3D::Utils::SelectionManagerIfc::DIM dim);

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
	 * \see		autoUpdate
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );


	protected :

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * Invoque <I>preview (false, true).</I>
	 * \see		preview
	 */
	virtual void operationCompleted ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtGeomEntityByRevolutionCreationPanel (
								const QtGeomEntityByRevolutionCreationPanel&);
	QtGeomEntityByRevolutionCreationPanel& operator = (
								const QtGeomEntityByRevolutionCreationPanel&);

	/** L'entité soumise à la rotation. */
	QtEntityByDimensionSelectorPanel*	_entityPanel;

	/** La rotation. */
	QtMgx3DRotationPanel*				_rotationPanel;

	/** Conserver les entités initiales ? */
	QCheckBox*							_keepEntitiesCheckBox;
};	// class QtGeomEntityByRevolutionCreationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeomEntityByRevolutionCreationPanel</I> de création d'entité
 * géométrique par révolution.
 */
class QtGeomEntityByRevolutionCreationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeomEntityByRevolutionCreationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtGeomEntityByRevolutionCreationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeomEntityByRevolutionCreationAction ( );

	/**
	 * \return		Le panneau d'édition de l'entité géométrique associée.
	 */
	virtual QtGeomEntityByRevolutionCreationPanel*
											getGeomEntityByRevolutionPanel ( );

	/**
	 * Créé/modifie l'entité géométrique avec le paramétrage de son panneau
	 * associé.
	 * Invoque préalablement
	 * <I>QtMgx3DtopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGeomEntityByRevolutionCreationAction (
								const QtGeomEntityByRevolutionCreationAction&);
	QtGeomEntityByRevolutionCreationAction& operator = (
								const QtGeomEntityByRevolutionCreationAction&);
};  // class QtGeomEntityByRevolutionCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOM_ENTITY_BY_REVOLUTION_CREATION_ACTION_H
