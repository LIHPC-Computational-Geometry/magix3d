/**
 * \file		QtGeometryGlueCurvesAction.h
 * \author		Charles PIGNEROL
 * \date		20/11/2014
 */
#ifndef QT_GEOMETRY_GLUE_CURVES_ACTION_H
#define QT_GEOMETRY_GLUE_CURVES_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de collage de courbes géométriques.
 */
class QtGeometryGlueCurvesPanel : public QtMgx3DOperationPanel
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
	QtGeometryGlueCurvesPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeometryGlueCurvesPanel ( );

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
	 * \return		Les noms des courbes à coller.
	 */
	virtual std::vector<std::string> getCurvesNames ( ) const;


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
	QtGeometryGlueCurvesPanel (const QtGeometryGlueCurvesPanel&);
	QtGeometryGlueCurvesPanel& operator = (const QtGeometryGlueCurvesPanel&);

	/** Les courbes à coller. */
	QtMgx3DEntityPanel*			_curvesPanel;
};	// class QtGeometryGlueCurvesPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeometryGlueCurvesPanel</I> de collage de courbes géométriques
 * entre-elles.
 */
class QtGeometryGlueCurvesAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeometryGlueCurvesPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtGeometryGlueCurvesAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeometryGlueCurvesAction ( );

	/**
	 * \return		Le panneau de paramétrage du collage.
	 */
	virtual QtGeometryGlueCurvesPanel* getGeometryGlueCurvesPanel ( );

	/**
	 * Colle les courbes géométriques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGeometryGlueCurvesAction (const QtGeometryGlueCurvesAction&);
	QtGeometryGlueCurvesAction& operator = (
									const QtGeometryGlueCurvesAction&);
};  // class QtGeometryCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOMETRY_GLUE_CURVES_ACTION_H
