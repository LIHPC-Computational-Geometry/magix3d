/**
 * \file		QtGeomSurfaceCreationAction.h
 * \author		Charles PIGNEROL
 * \date		20/11/2014
 */
#ifndef QT_GEOM_SURFACE_CREATION_ACTION_H
#define QT_GEOM_SURFACE_CREATION_ACTION_H

#include "Internal/Context.h"

#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de création de surface géométrique.
 */
class QtGeomSurfaceCreationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtGeomSurfaceCreationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGeomSurfaceCreationPanel ( );

	/**
	 * \return		Le nom du groupe concerné, éventuellement créé.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		La liste des courbes ou points géométriques définissant le
	 * 				contour de la surface à créer.
	 */
	virtual std::vector<std::string> getEntitiesNames ( ) const;

	/**
	 * \return		La dimension des entités définissant le contour de la 
	 *				surface à créer.
	 */
	virtual int getDimension ( ) const;

	/**
	 * \param		La dimension des entités définissant le contour de la 
	 *				surface à créer.
	 */
	virtual void setDimension (int dimension);

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
	 * Appelé lorsque la dimension des groupes sélectionnés est modifiée.
	 * Actualise la liste des noms de groupe.
	 */
	virtual void dimensionsModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtGeomSurfaceCreationPanel (const QtGeomSurfaceCreationPanel&);
	QtGeomSurfaceCreationPanel& operator = (
										const QtGeomSurfaceCreationPanel&);

	/** Le nom du groupe créé. */
	QtMgx3DGroupNamePanel*				_namePanel;

	/** Les courbes (ou points) géométriques définissant le contour de la
	 * surface à créer.*/
	QtEntityByDimensionSelectorPanel*	_contourEntitiesPanel;
};	// class QtGeomSurfaceCreationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeomSurfaceCreationPanel</I> de création de surface géométrique.
 */
class QtGeomSurfaceCreationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeomSurfaceCreationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtGeomSurfaceCreationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeomSurfaceCreationAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres d'addition d'entités
	 *				géométriques.
	 */
	virtual QtGeomSurfaceCreationPanel* getSurfaceCreationPanel ( );

	/**
	 * Créé la surface conformément au paramétrage demandé.
	 * Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGeomSurfaceCreationAction (const QtGeomSurfaceCreationAction&);
	QtGeomSurfaceCreationAction& operator = (
									const QtGeomSurfaceCreationAction&);
};  // class QtGeomSurfaceCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOM_SURFACE_CREATION_ACTION_H
