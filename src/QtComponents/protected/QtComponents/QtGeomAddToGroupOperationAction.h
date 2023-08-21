/**
 * \file		QtGeomAddToGroupOperationAction.h
 * \author		Charles PIGNEROL
 * \date		09/09/2014
 */
#ifndef QT_GEOM_ADD_TO_GROUP_OPERATION_ACTION_H
#define QT_GEOM_ADD_TO_GROUP_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"

#include "Utils/SelectionManagerIfc.h"

#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres d'addition/suppression/affectation
 * d'entités géométriques.
 */
class QtGeomAddToGroupOperationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * L'opération a effectuer : ajouter/enlever/affecter les entités au groupe
	 * spécifié.
	 */
	enum OPERATION { ADD, REMOVE, SET };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtGeomAddToGroupOperationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGeomAddToGroupOperationPanel ( );

	/**
	 * \return		Le nom du groupe concerné, éventuellement créé.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		L'opération à effectuer.
	 */
	virtual OPERATION getOperation ( ) const;

	/**
	 * \return		La dimension des entités sélectionnées
	 * \see			setDimension
	 */
	virtual Mgx3D::Utils::SelectionManagerIfc::DIM getDimension ( ) const;

	/**
	 * \param		Nouvelle dimension de saisie des entités géométrique.
	 * \see			getDimension
	 */
	virtual void setDimension (Mgx3D::Utils::SelectionManagerIfc::DIM dim);

	/**
	 * \return		La liste des entités géométriques à ajouter.
	 */
	virtual std::vector<std::string> getGeomEntitiesNames ( ) const;

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
	QtGeomAddToGroupOperationPanel (const QtGeomAddToGroupOperationPanel&);
	QtGeomAddToGroupOperationPanel& operator = (
										const QtGeomAddToGroupOperationPanel&);

	/** Le nom du groupe créé. */
	QtMgx3DGroupNamePanel*						_namePanel;

	/** L'opération à effectuer. */
	QComboBox*									_operationComboBox;

	/** Les entités géométriques à ajouter. */
	QtEntityByDimensionSelectorPanel*			_geomEntitiesPanel;
};	// class QtGeomAddToGroupOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeomAddToGroupOperationPanel</I> d'addition d'entités géométriques.
 */
class QtGeomAddToGroupOperationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeomAddToGroupOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtGeomAddToGroupOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeomAddToGroupOperationAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres d'addition d'entités
	 *				géométriques.
	 */
	virtual QtGeomAddToGroupOperationPanel* getAdditionPanel ( );

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
	QtGeomAddToGroupOperationAction (const QtGeomAddToGroupOperationAction&);
	QtGeomAddToGroupOperationAction& operator = (
									const QtGeomAddToGroupOperationAction&);
};  // class QtGeomAddToGroupOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOM_ADD_TO_GROUP_OPERATION_ACTION_H
