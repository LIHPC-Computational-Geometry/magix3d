/**
 * \file		QtGeomPlaneCutAction.h
 * \author		Charles PIGNEROL
 * \date		22/11/2013
 */
#ifndef QT_GEOM_PLANE_CUT_ACTION_H
#define QT_GEOM_PLANE_CUT_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DPointPanel.h"
#include "QtComponents/QtMgx3DVectorPanel.h"
#include "QtComponents/RenderingManager.h"

#include <QtUtil/Qt3DDataPanel.h>
#include <QtUtil/QtDoubleTextField.h>

#include <QComboBox>
#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération de coupe par un plan
 * d'entités géométriques.
 */
class QtGeomPlaneCutPanel :
	public QtMgx3DOperationPanel,
	public Mgx3D::QtComponents::RenderingManager::InteractorObserver
{
	Q_OBJECT

	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Politique de création/modification du groupe
	 * \param	Label du champ de saisie des entités
	 * \param	Dimensions des entités sélectionnables
	 * \param	Types des entités sélectionnables
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtGeomPlaneCutPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			const std::string& entitiesLabel,
			Mgx3D::Utils::SelectionManagerIfc::DIM	dimensions, 
			Mgx3D::Utils::FilterEntity::objectType types,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGeomPlaneCutPanel ( );

	/**
	 * \return		Le nom du groupe créé/modifié.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * Les coordonnées du point définissant le plan.
	 */
	virtual Mgx3D::Utils::Math::Point getPoint ( ) const;

	/**
	 * Les coordonnées du vecteur définissant la normale au plan.
	 */
	virtual void getNormal (double& dx, double& dy, double& dz) const;
	virtual Mgx3D::Utils::Math::Vector getNormal ( ) const;

	/**
	 * \return		Les entités à soumettre à la coupe.
	 */
	virtual std::vector<std::string> getCutEntities ( ) const;

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
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true, true)</I>.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );

	/**
	 * Méthode appelée lorsque la définition du plan est modifiée par
	 * l'utilisateur via un interacteur 3D dans la fenêtre graphique.
	 * Actualise le panneau.
	 */
	virtual void planeModifiedCallback (
			Mgx3D::Utils::Math::Point point, Mgx3D::Utils::Math::Vector normal);

	/**
	 * Réinitialise le champ contenant les entités à soumettre à la coupe.
	 */
	virtual void clearEntitiesField ( );

	/**
	 * \param	<I>true</I> pour prévisualiser les entités concernées par
	 * 			l'association, <I>false</I> pour arrêter la prévisualisation.
	 * \param	<I>true</I> s'il faut détruire l'éventuel interacteur dans le
	 *			cas d'un arrêt de la prévisualisation.
	 */
	virtual void preview (bool show, bool destroyInteractor);


	protected :

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return		Un pointeur sur l'éventuel interacteur en cours
	 * 				d'utilisation.
	 */
	virtual Mgx3D::QtComponents::RenderingManager::PlaneInteractor*
															getInteractor ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );


	protected slots :

	/**
	 * Appelé lorsqu'un paramètre de la coupe est modifié.
	 * Appelle <I>preview (true)</I>.
	 */
	virtual void cutModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtGeomPlaneCutPanel (const QtGeomPlaneCutPanel&);
	QtGeomPlaneCutPanel& operator = (const QtGeomPlaneCutPanel&);

	/** L'éventuel nom du groupe. */
	QtMgx3DGroupNamePanel*							_namePanel;

	/** Coordonnées du point définissant le plan. */
	QtMgx3DPointPanel*								_pointPanel;

	/** Le vecteur normal au plan. */
	QtMgx3DVectorPanel*								_normalPanel;

	/** Filtre sur la dimension des données concernées. */
	QtEntityByDimensionSelectorPanel*				_entitiesPanel;
};	// class QtGeomPlaneCutPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeomPlaneCutPanel</I> de définition d'un plan de coupe.
 */
class QtGeomPlaneCutAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeomPlaneCutPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Dimensions des entités géométriques concernées.
	 * \param		Types des entités sélectionnables
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtGeomPlaneCutAction (
		const QIcon& icon, const QString& text,
		Mgx3D::Utils::SelectionManagerIfc::DIM	dimensions, 
		Mgx3D::Utils::FilterEntity::objectType types,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeomPlaneCutAction ( );

	/**
	 * \return		Le panneau d'édition du plan de coupe associée.
	 */
	virtual QtGeomPlaneCutPanel* getPlaneCutPanel ( );

	/**
	 * Créé/modifie la coupe avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGeomPlaneCutAction (const QtGeomPlaneCutAction&);
	QtGeomPlaneCutAction& operator = (const QtGeomPlaneCutAction&);
};  // class QtGeomPlaneCutAction


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOM_PLANE_CUT_ACTION_H
