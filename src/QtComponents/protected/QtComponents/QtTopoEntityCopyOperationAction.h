/**
 * \file		QtTopoEntityCopyOperationAction.h
 * \author		Charles PIGNEROL
 * \date		25/06/2014
 */
#ifndef QT_TOPO_ENTITY_COPY_OPERATION_ACTION_H
#define QT_TOPO_ENTITY_COPY_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include "Utils/SelectionManagerIfc.h"



namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de copie d'entités topologiques.
 */
class QtTopoEntityCopyOperationPanel : public QtMgx3DOperationPanel
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
	QtTopoEntityCopyOperationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtTopoEntityCopyOperationPanel ( );

	/**
	 * \return		La liste des entités topologiques à copier.
	 * \see			getGeomEntityName
	 */
	virtual std::vector<std::string> getTopoEntitiesNames ( ) const;

	/**
	 * \return		L'entité géométrique à associer.
	 * \see			getTopoEntitiesNames
	 */
	virtual std::string getGeomEntityName ( ) const;

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
	QtTopoEntityCopyOperationPanel (const QtTopoEntityCopyOperationPanel&);
	QtTopoEntityCopyOperationPanel& operator = (
										const QtTopoEntityCopyOperationPanel&);

	/** Les entités topologiques à copier. */
	QtMgx3DEntityPanel*							_topoEntitiesPanel;

	/** L'entité géométrique associée. */
	QtMgx3DEntityPanel*							_geomEntityPanel;
};	// class QtTopoEntityCopyOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopoEntityCopyOperationPanel</I> de copie d'entités topologiques.
 */
class QtTopoEntityCopyOperationAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopoEntityCopyOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopoEntityCopyOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopoEntityCopyOperationAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de copie d'entités
	 *				topologiques.
	 */
	virtual QtTopoEntityCopyOperationPanel* getCopyPanel ( );

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
	QtTopoEntityCopyOperationAction (const QtTopoEntityCopyOperationAction&);
	QtTopoEntityCopyOperationAction& operator = (
									const QtTopoEntityCopyOperationAction&);
};  // class QtTopoEntityCopyOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPO_ENTITY_COPY_OPERATION_ACTION_H
