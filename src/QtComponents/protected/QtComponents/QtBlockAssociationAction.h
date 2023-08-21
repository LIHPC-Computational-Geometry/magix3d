/**
 * \file		QtBlockAssociationAction.h
 * \author		Charles PIGNEROL
 * \date		15/11/2013
 */
#ifndef QT_BLOCK_ASSOCIATION_ACTION_H
#define QT_BLOCK_ASSOCIATION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres d'associations de blocs topologiques à un
 * volume géométrique.
 */
class QtBlockAssociationPanel : public QtMgx3DOperationPanel
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
	QtBlockAssociationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtBlockAssociationPanel ( );

	/**
	 * \return		La liste des blocs à associer au volume.
	 * \see			setBlocksNames
	 */
	virtual std::vector<std::string> getBlocksNames ( ) const;

	/**
	 * \return		L'éventuel volume associé au bloc.
	 * \see			setVolumeName
	 */
	virtual std::string getVolumeName ( ) const;

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
	 * \param		Eventuels noms de blocs à associer au volume.
	 * \see			getBlocksNames
	 */
	virtual void setBlocksNames (const std::vector<std::string>& names);

	/**
	 * \param		Eventuel volume à associer aux blocs.
	 * \see			setVolumeName
	 */
	virtual void setVolumeName (const std::string& name);

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtBlockAssociationPanel (const QtBlockAssociationPanel&);
	QtBlockAssociationPanel& operator = (const QtBlockAssociationPanel&);

	/** Les blocs à asocier. */
	QtMgx3DEntityPanel*							_blocksPanel;

	/** L'éventuel volume à asocier. */
	QtMgx3DEntityPanel*							_volumePanel;
};	// class QtBlockAssociationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtBlockAssociationPanel</I> de modification des éventuelles
 * associations blocs topologiques vers volume géométrique.
 */
class QtBlockAssociationAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtBlockAssociationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtBlockAssociationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtBlockAssociationAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres d'association associé.
	 */
	virtual QtBlockAssociationPanel* getAssociationPanel ( );

	/**
	 * Actualise le paramétrage des blocs avec le paramétrage de son
	 * panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtBlockAssociationAction (const QtBlockAssociationAction&);
	QtBlockAssociationAction& operator = (
									const QtBlockAssociationAction&);
};  // class QtBlockAssociationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_BLOCK_ASSOCIATION_ACTION_H
