/**
 * \file		QtGeomEntityCopyOperationAction.h
 * \author		Charles PIGNEROL
 * \date		25/06/2014
 */
#ifndef QT_GEOM_ENTITY_COPY_OPERATION_ACTION_H
#define QT_GEOM_ENTITY_COPY_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"

#include "Utils/SelectionManagerIfc.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de copie d'entités géométriques.
 */
class QtGeomEntityCopyOperationPanel : public QtMgx3DOperationPanel
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
	QtGeomEntityCopyOperationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGeomEntityCopyOperationPanel ( );

	/**
	 * @return		Le nom du groupe créé.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * @param		Nouvelle dimension de saisie des entités topologiques.
	 */
	virtual void setDimension (Mgx3D::Utils::SelectionManagerIfc::DIM dim);

	/**
	 * \return		La liste des entités géométriques à copier.
	 * \see			withTopology
	 */
	virtual std::vector<std::string> getGeomEntitiesNames ( ) const;

	/**
	 * \return		<I>true</I> s'il faut également copier les entités
	 *				topologiques associées.
	 * \see			getGeomEntitiesNames
	 */
	virtual bool withTopology ( ) const;

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
	QtGeomEntityCopyOperationPanel (const QtGeomEntityCopyOperationPanel&);
	QtGeomEntityCopyOperationPanel& operator = (
										const QtGeomEntityCopyOperationPanel&);

	/** Le nom du groupe créé. */
	QtMgx3DGroupNamePanel*						_namePanel;

	/** Les entités géométriques à copier. */
	QtEntityByDimensionSelectorPanel*			_geomEntitiesPanel;

	/** Case à cocher <I>avec la topologie</I>. */
	QCheckBox*									_withTopologyCheckBox;
};	// class QtGeomEntityCopyOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeomEntityCopyOperationPanel</I> de copie d'entités géométriques.
 */
class QtGeomEntityCopyOperationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeomEntityCopyOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtGeomEntityCopyOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeomEntityCopyOperationAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de copie d'entités
	 *				géométriques.
	 */
	virtual QtGeomEntityCopyOperationPanel* getCopyPanel ( );

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
	QtGeomEntityCopyOperationAction (const QtGeomEntityCopyOperationAction&);
	QtGeomEntityCopyOperationAction& operator = (
									const QtGeomEntityCopyOperationAction&);
};  // class QtGeomEntityCopyOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOM_ENTITY_COPY_OPERATION_ACTION_H
