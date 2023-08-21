/**
 * \file		QtTopologyBlockExtractionOperationAction.h
 * \author		Charles PIGNEROL
 * \date		11/05/2015
 */
#ifndef QT_TOPOLOGY_BLOCK_EXTRACTION_ACTION_H
#define QT_TOPOLOGY_BLOCK_EXTRACTION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"

#include "Utils/SelectionManagerIfc.h"



namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres d'extractions de blocs topologiques
 * (copie dans un groupe, suppression des originaux).
 */
class QtTopologyBlockExtractionOperationPanel : public QtMgx3DOperationPanel
{
	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Politique de création/modification du groupe
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyBlockExtractionOperationPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtTopologyBlockExtractionOperationPanel ( );

	/**
	 * \return		Le nom du groupe recevant les blocs extraits.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		La liste des blocs à extraire.
	 */
	virtual std::vector<std::string> getBlockNames ( ) const;

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
	QtTopologyBlockExtractionOperationPanel (
								const QtTopologyBlockExtractionOperationPanel&);
	QtTopologyBlockExtractionOperationPanel& operator = (
								const QtTopologyBlockExtractionOperationPanel&);

	/** Le nom du groupe recevant les blocs extraits. */
	QtMgx3DGroupNamePanel*						_namePanel;

	/** Les blocs à extraire. */
	QtMgx3DEntityPanel*							_topoEntitiesPanel;
};	// class QtTopologyBlockExtractionOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyBlockExtractionOperationPanel</I> d'extraction de blocs
 * topologiques.
 */
class QtTopologyBlockExtractionOperationAction :	
										public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyBlockExtractionOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyBlockExtractionOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyBlockExtractionOperationAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres d'extraction de blocs
	 *				topologiques.
	 */
	virtual QtTopologyBlockExtractionOperationPanel* getExtractionPanel ( );

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
	QtTopologyBlockExtractionOperationAction (
							const QtTopologyBlockExtractionOperationAction&);
	QtTopologyBlockExtractionOperationAction& operator = (
							const QtTopologyBlockExtractionOperationAction&);
};  // class QtTopologyBlockExtractionOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_BLOCK_EXTRACTION_ACTION_H
