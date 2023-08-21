/**
 * \file		QtGeomEntityCommon2DOperationAction.h
 * \author		Charles PIGNEROL
 * \date		11/09/2018
 */
#ifndef QT_GEOM_ENTITY_COMMON2D_OPERATION_ACTION_H
#define QT_GEOM_ENTITY_COMMON2D_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DEntityPanel.h"

#include "Utils/SelectionManagerIfc.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres d'intersection d'entités géométriques
 * de même dimension (1 ou 2) entre-elles.
 */
class QtGeomEntityCommon2DOperationPanel : public QtMgx3DOperationPanel
{
	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *		notamment pour récupérer le contexte.
	 * \param	Dimension des entités à intersequer
	 * \param	Politique de création/modification du groupe
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtGeomEntityCommon2DOperationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			int dimension, QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGeomEntityCommon2DOperationPanel ( );

	/**
	 * @return		Le nom du groupe créé.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		La première entité participant à l'intersection.
	 * \see			getSecondGeomEntityName
	 */
	virtual std::string getFirstGeomEntityName ( ) const;

	/**
	 * \return		La second entité participant à l'intersection.
	 * \see			getFirstGeomEntityName
	 */
	virtual std::string getSecondGeomEntityName ( ) const;

	/**
	 * \return		La dimension des entités intersequées.
	 */
	virtual int getDimension ( ) const;

	/**
	 * \return		<I>true</I> s'il faut copier les entités avant l'opération
	 */
	virtual bool copyEntities ( ) const;

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


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtGeomEntityCommon2DOperationPanel (const QtGeomEntityCommon2DOperationPanel&);
	QtGeomEntityCommon2DOperationPanel& operator = (const QtGeomEntityCommon2DOperationPanel&);

	/** La dimension des entités intersequées. */
	int							_dimension;

	/** Case à cocher copier. */
	QCheckBox*									_copyCheckBox;

	/** Le nom du groupe créé. */
	QtMgx3DGroupNamePanel*					_namePanel;

	/** La première entité géométrique à intersequer. */
	QtMgx3DEntityPanel*					_firstEntityPanel;

	/** La seconde entité géométrique à intersequer. */
	QtMgx3DEntityPanel*					_secondEntityPanel;
};	// class QtGeomEntityCommon2DOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeomEntityCommon2DOperationPanel</I> d'intersection d'entités géométriques.
 */
class QtGeomEntityCommon2DOperationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeomEntityCommon2DOperationPanel</I>.
	 * \param	Icône représentant l'action.
	 * \param	Texte représentant l'action.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *		notamment pour récupérer le contexte et le panneau contenant
	 *		les icônes.
	 * \param	Dimension des entités à intersequer
	 * \param	Tooltip décrivant l'action.
	 * \param	Politique de création/modification du groupe
	 */
	QtGeomEntityCommon2DOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow, int dimension,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeomEntityCommon2DOperationAction ( );

	/**
	 * \return	Le panneau d'édition des paramètres d'intersection d'entités
	 *		géométriques.
	 */
	virtual QtGeomEntityCommon2DOperationPanel* getCommon2DPanel ( );

	/**
	 * Actualise le paramétrage des entités géométriques avec le paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGeomEntityCommon2DOperationAction (const QtGeomEntityCommon2DOperationAction&);
	QtGeomEntityCommon2DOperationAction& operator = (const QtGeomEntityCommon2DOperationAction&);
};  // class QtGeomEntityCommon2DOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOM_ENTITY_COMMON2D_OPERATION_ACTION_H
