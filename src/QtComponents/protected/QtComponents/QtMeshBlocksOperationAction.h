/**
 * \file		QtMeshBlocksOperationAction.h
 * \author		Charles PIGNEROL
 * \date		06/09/2013
 */
#ifndef QT_MESH_BLOCKS_OPERATION_ACTION_H
#define QT_MESH_BLOCKS_OPERATION_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DMeshOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DEntityPanel.h"

#include <QVBoxLayout>

#include <memory>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération de maillage de tout ou
 * partie des blocs topologiques.
 */
class QtMeshBlocksOperationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 * \param	Fichier d'aide contextuelle associé à ce panneau.
	 * \param	Balise dans le fichier d'aide contextuelle associé à ce panneau
	 *			identifiant le paragraphe sur ce panneau.
	 */
	QtMeshBlocksOperationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action,
			const std::string& helpURL, const std::string& helpTag);

	/**
	 * RAS.
	 */
	virtual ~QtMeshBlocksOperationPanel ( );

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

	/**
	 * \return	<I>true</I> s'il faut tout mailler, <I>false</I> dans le
	 * 			cas contraire.
	 * \see		getSurfaces
	 */
	virtual bool meshAll ( ) const;

	/**
	 * \return	La liste des blocks à mailler (dans l'hypothèse où l'on
	 *			ne maille qu'une sélection).
	 * \see		meshAll
	 */
	virtual std::vector<std::string> getBlocks ( ) const;

	/**
	 * Arrête la sélection en mode interactif.
	 */
	virtual void stopInteractiveMode ( );


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


	protected slots :

	/**
	 * Callback sur le changement d'état du checkbox "Tout mailler".
	 * Actualise le panneau.
	 */
	virtual void meshedSelectionCallback ( );


	protected :

	/**
	 * \return		Le gestionnaire de mise en forme du panneau.
	 */
	virtual QVBoxLayout& getVLayout ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtMeshBlocksOperationPanel (const QtMeshBlocksOperationPanel&);
	QtMeshBlocksOperationPanel& operator = (
										const QtMeshBlocksOperationPanel&);

	/** Le nom du feuillet. */
	QtMgx3DGroupNamePanel*				_namePanel;

	/** Faut-il tout mailler ? */
	QCheckBox*							_meshAllCheckBox;

	/** Les blocs à mailler (cas d'une sélection). */
	QtMgx3DEntityPanel*					_blocksPanel;

	/** Le gestionnaire de mise en forme du panneau. */
	QVBoxLayout*						_verticalLayout;
};	// class QtMeshBlocksOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtMeshBlocksOperationPanel</I> de contrôle d'une opération de maillage
 * des blocs topologiques.
 */
class QtMeshBlocksOperationAction : public QtMgx3DMeshOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtMeshBlocksOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et la barre d'icônes
	 *				d'opérations Magix 3D.
	 * \param		Tooltip décrivant l'action.
	 */
	QtMeshBlocksOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMeshBlocksOperationAction ( );

	/**
	 * \return		Le panneau d'édition de la génération de maillage.
	 */
	virtual QtMeshBlocksOperationPanel* getMeshPanel ( );

	/**
	 * Créé/modifie les maillages des blocs topologiques.
	 * Invoque préalablement
	 * <I>QtMgx3DMeshOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	protected :


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMeshBlocksOperationAction (const QtMeshBlocksOperationAction&);
	QtMeshBlocksOperationAction& operator = (
									const QtMeshBlocksOperationAction&);
};  // class QtMeshBlocksOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MESH_BLOCKS_OPERATION_ACTION_H
