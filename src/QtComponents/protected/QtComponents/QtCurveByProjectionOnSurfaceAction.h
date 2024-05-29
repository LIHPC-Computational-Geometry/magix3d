/**
 * \file		QtCurveByProjectionOnSurfaceAction.h
 * \author		Charles PIGNEROL
 * \date		19/05/2015
 */
#ifndef QT_CURVE_BY_PROJECTION_ON_SURFACE_ACTION_H
#define QT_CURVE_BY_PROJECTION_ON_SURFACE_ACTION_H


#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DEntityPanel.h"


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau de création d'une courbe par projection orthogonale d'une courbe sur 
 * une surface.
 */
class QtCurveByProjectionOnSurfacePanel : public QtMgx3DOperationPanel
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
	QtCurveByProjectionOnSurfacePanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtCurveByProjectionOnSurfacePanel ( );

	/**
	 * \return		Le nom du groupe auquel est rattachée la courbe créée.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		Le nom de la courbe à projeter.
	 * \see			getSurfaceName
	 */
	virtual std::string getCurveName ( ) const;

	/**
	 * \return		Le nom de la surface sur laquelle est projetée la courbe.
	 * \see			getCurveName
	 */
	virtual std::string getSurfaceName ( ) const;

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
	 * \return	La liste des entités impliquées dans l'opération.
	 */
	 virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

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
	QtCurveByProjectionOnSurfacePanel (
									const QtCurveByProjectionOnSurfacePanel&);
	QtCurveByProjectionOnSurfacePanel& operator = (
									const QtCurveByProjectionOnSurfacePanel&);

	/** Le groupe de rattachement de la courbe créée. */
	QtMgx3DGroupNamePanel*			_namePanel;

	/** La courbe projectée. */
	QtMgx3DEntityPanel*				_curvePanel;

	/** La surface de projection. */
	QtMgx3DEntityPanel*				_surfacePanel;
};	// class QtCurveByProjectionOnSurfacePanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtCurveByProjectionOnSurfacePanel</I> de création d'une courbe par
 * projection orthogonale d'une courbe sur une surface.
 */
class QtCurveByProjectionOnSurfaceAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtCurveByProjectionOnSurfacePanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtCurveByProjectionOnSurfaceAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtCurveByProjectionOnSurfaceAction ( );

	/**
	 * \return		Le panneau d'édition de la courbe associé.
	 */
	virtual QtCurveByProjectionOnSurfacePanel* getCurvePanel ( );

	/**
	 * Créé/modifie le bloc avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DtopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtCurveByProjectionOnSurfaceAction (
								const QtCurveByProjectionOnSurfaceAction&);
	QtCurveByProjectionOnSurfaceAction& operator = (
								const QtCurveByProjectionOnSurfaceAction&);
};  // class QtCurveByProjectionOnSurfaceAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_CURVE_BY_PROJECTION_ON_SURFACE_ACTION_H
