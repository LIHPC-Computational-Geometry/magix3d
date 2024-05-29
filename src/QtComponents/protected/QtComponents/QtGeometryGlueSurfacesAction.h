/**
 * \file		QtGeometryGlueSurfacesAction.h
 * \author		Charles PIGNEROL
 * \date		01/12/2014
 */
#ifndef QT_GEOMETRY_GLUE_SURFACES_ACTION_H
#define QT_GEOMETRY_GLUE_SURFACES_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de collage de surfaces géométriques.
 */
class QtGeometryGlueSurfacesPanel : public QtMgx3DOperationPanel
{
	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtGeometryGlueSurfacesPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeometryGlueSurfacesPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
	 * Restitue l'environnement dans son état initial.
	 * Invoque <I>preview (false)</I>.
	 * \see	preview
	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true, true)</I>.
	 * \see	preview
	 */
	virtual void autoUpdate ( );

	/**
	 * \return		Les noms des surfaces à coller.
	 */
	virtual std::vector<std::string> getSurfacesNames ( ) const;


	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
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
	QtGeometryGlueSurfacesPanel (const QtGeometryGlueSurfacesPanel&);
	QtGeometryGlueSurfacesPanel& operator = (const QtGeometryGlueSurfacesPanel&);

	/** Les surfaces à coller. */
	QtMgx3DEntityPanel*			_surfacesPanel;
};	// class QtGeometryGlueSurfacesPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeometryGlueSurfacesPanel</I> de collage de surfaces géométriques
 * entre-elles.
 */
class QtGeometryGlueSurfacesAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeometryGlueSurfacesPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtGeometryGlueSurfacesAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeometryGlueSurfacesAction ( );

	/**
	 * \return		Le panneau de paramétrage du collage.
	 */
	virtual QtGeometryGlueSurfacesPanel* getGeometryGlueSurfacesPanel ( );

	/**
	 * Colle les surfaces géométriques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGeometryGlueSurfacesAction (const QtGeometryGlueSurfacesAction&);
	QtGeometryGlueSurfacesAction& operator = (
									const QtGeometryGlueSurfacesAction&);
};  // class QtGeometryCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOMETRY_GLUE_SURFACES_ACTION_H
