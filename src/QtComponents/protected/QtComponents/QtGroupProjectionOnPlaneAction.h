/**
 * \file		QtGroupProjectionOnPlaneAction.h
 * \author		Charles PIGNEROL
 * \date		09/04/2018
 */
#ifndef QT_GROUP_PROJECTION_ON_PLANE_ACTION_H
#define QT_GROUP_PROJECTION_ON_PLANE_ACTION_H

#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DMeshOperationAction.h"

#include <QButtonGroup>


namespace Mgx3D 
{

namespace QtComponents
{

class QtMgx3DGroupNamePanel;

/**
 * Panneau d'édition des paramètres de projection sur un plan d'un groupe
 * d'entités 2D de type maillage.
 */
class QtGroupProjectionOnPlanePanel : public QtMgx3DOperationPanel
{
	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *		notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtGroupProjectionOnPlanePanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGroupProjectionOnPlanePanel ( );

	/**
	 * \return		Le nom du groupe dont on projette les entités.
	 * \see			getProjection
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		La projection à effectuer.
	 * \see			getGroupName
	 */
	virtual Utils::Math::Plane::XYZ getProjection ( ) const;

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
	 * Actualise le panneau en fonction du contexte.
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
	QtGroupProjectionOnPlanePanel (const QtGroupProjectionOnPlanePanel&);
	QtGroupProjectionOnPlanePanel& operator = (const QtGroupProjectionOnPlanePanel&);

	/** Le groupe à projeter. */
	QtMgx3DGroupNamePanel*		_groupPanel;

	/** Le plan de projection. */
	QButtonGroup*			_planeButtonGroup;
};	// class QtGroupProjectionOnPlanePanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGroupProjectionOnPlanePanel</I> de projection sur un plan d'un groupe
 * d'entités 2D de type maillage.
 */
class QtGroupProjectionOnPlaneAction : public QtMgx3DMeshOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGroupProjectionOnPlanePanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte et le panneau contenant
	 *			les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtGroupProjectionOnPlaneAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGroupProjectionOnPlaneAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de translation d'entités
	 *			géométriques.
	 */
	virtual QtGroupProjectionOnPlanePanel* getProjectionPanel ( );

	/**
	 * Effectue la projection selon le paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DMeshOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGroupProjectionOnPlaneAction (const QtGroupProjectionOnPlaneAction&);
	QtGroupProjectionOnPlaneAction& operator = (const QtGroupProjectionOnPlaneAction&);
};  // class QtGroupProjectionOnPlaneAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GROUP_PROJECTION_ON_PLANE_ACTION_H
