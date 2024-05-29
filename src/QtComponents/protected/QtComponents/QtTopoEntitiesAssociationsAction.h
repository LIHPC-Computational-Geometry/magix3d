/**
 * \file		QtTopoEntitiesAssociationsAction.h
 * \author		Charles PIGNEROL
 * \date		20/11/2013
 */
#ifndef QT_TOPO_ENTITIES_ASSOCIATIONS_ACTION_H
#define QT_TOPO_ENTITIES_ASSOCIATIONS_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"

#include "Utils/SelectionManagerIfc.h"

#include <QCheckBox>
#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau de base d'édition des paramètres d'associations d'entités
 * topologiques avec des entités géométriques.
 */
class QtTopoToGeomAssociationPanel : public QtMgx3DOperationsSubPanel
{
	Q_OBJECT

	public :

	/**
	 * La méthode de projection utilisée :<BR>
	 * <OL>
	 * <LI>Entités topologiques vers une entité géométrique,
	 * <LI>Sommets topologiques vers les entités géométriques les plus proches,
	 * <LI>Arêtes topologiques vers les courbes géométriques les plus proches,
	 * <LI>Faces topologiques vers les surfaces géométriques les plus proches
	 * </OL>
	 */
	enum PROJECTION_METHOD { TOPO_ENTITIES_GEOM_ENTITY, VERTICES_GEOM_ENTITIES,
				EDGES_CURVES, FACES_SURFACES };

	/**
	 * Constructeur.
	 * \param	Widget parent
	 * \param	Méthode de projection.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Dimensions proposées par défaut
	 */
	QtTopoToGeomAssociationPanel (
			QWidget* parent, PROJECTION_METHOD method,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::Utils::SelectionManagerIfc::DIM defaultDimensions);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopoToGeomAssociationPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
	 * Restitue l'environnement dans son état initial.
	 * \see			autoUpdate
	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * \see			cancel
	 */
	virtual void autoUpdate ( );

	/**
	 * Arrête (éventuellement) la sélection interactive en cours.
	 * Méthode à surcharger si nécessaire, ne fait rien par défaut.
	 */
	virtual void stopSelection ( );

	/**
	 * Vide les champs de sélection
	 */
	virtual void clearSelection ( );

	/**
	 * \return		La méthode de projection utilisée.
	 */
	virtual QtTopoToGeomAssociationPanel::PROJECTION_METHOD
											getProjectionMethod ( ) const;

	/**
	 * \return		La/les entités topologiques projetées.
	 */
	virtual std::vector< std::string > getTopoEntitiesNames ( ) const;

	/**
	 * \return		La/les entités géométriques associées.
	 */
	virtual std::vector< std::string > getGeomEntitiesNames ( ) const;

	/**
	 * @return		<I>true</I> s'il faut projeter toutes les entités
	 *				topologiques (courbes ou faces, selon la méthode),
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool projectAllTopoEntities ( ) const;


	protected :

	/**
 	 * \return		La liste des entités impliquées dans l'opération.
 	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );


	protected slots :

	/**
	 * Modification d'une dimension de saisie des entités topologiques ou
	 * géométrique. Actualise le panneau.
	 */
	virtual void dimensionsModifiedCallback ( );

	/**
	 * Appelé lorsque l'utilisateur modifie l'état de la case à cocher
	 * "projeter toutes les entités topologiques".
	 * Active/Inactive le panneau de saisie des entités topologiques.
	 */
	virtual void projectAllTopoEntitiesModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopoToGeomAssociationPanel (const QtTopoToGeomAssociationPanel&);
	QtTopoToGeomAssociationPanel& operator = (
								const QtTopoToGeomAssociationPanel&);

	/** La méthode de projection représentée. */
	PROJECTION_METHOD							_projectionMethod;

	/** Les dimensions possibles pour les entités topologiques à associer. */
	QtEntityByDimensionSelectorPanel*			_topoEntitiesPanel;

	/** L'éventuelle entité géométrique à associer. */
	QtEntityByDimensionSelectorPanel*			_geomEntitiesPanel;

	/** Faut il projeter toutes les entités topologiques ? */
	QCheckBox*									_projectAllCheckBox;
};	// class QtTopoToGeomAssociationPanel


/**
 * Panneau d'édition des paramètres d'associations d'entités topologiques et
 * d'entités géométriques.
 */
class QtTopoEntitiesAssociationsPanel : public QtMgx3DOperationPanel
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
	 * \param	Dimensions proposées par défaut
	 */
	QtTopoEntitiesAssociationsPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action,
			Mgx3D::Utils::SelectionManagerIfc::DIM defaultDimensions);

	/**
	 * RAS.
	 */
	virtual ~QtTopoEntitiesAssociationsPanel ( );

	/**
	 * \return		La méthode de projection utilisée.
	 */
	virtual QtTopoToGeomAssociationPanel::PROJECTION_METHOD
											getProjectionMethod ( ) const;

	/**
	 * \return		La liste des entités topologiques à associer à l'entité
	 *				géométrique.
	 */
	virtual std::vector<std::string> getTopoEntitiesNames ( ) const;
	virtual std::vector<Mgx3D::Topo::TopoEntity*> getTopoEntities ( ) const;
	virtual std::vector<Mgx3D::Topo::Vertex*> getVertices ( ) const;
	virtual std::vector<Mgx3D::Topo::CoEdge*> getCoEdges ( ) const;
	virtual std::vector<Mgx3D::Topo::CoFace*> getCoFaces ( ) const;

	/**
	 * \return		Les éventuelles entités géométriques associées aux entités
	 *				topologiques.
	 */
	virtual std::vector< std::string > getGeomEntitiesNames ( ) const;
	virtual std::vector<Mgx3D::Geom::GeomEntity*> getGeomEntities ( ) const;

	/**
	 * @return		<I>true</I> s'il faut projeter toutes les entités
	 *				topologiques (courbes ou faces, selon la méthode),
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool projectAllTopoEntities ( ) const;

	/**
	 * \return		<I>true</I> si les sommets topologiques doivent être
	 * 				déplacés lors de la modification d'association.
	 */
	virtual bool moveVertices ( ) const;

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
	 * \param		<I>true</I> pour prévisualiser la discrétisation,
	 *				<I>false</I> pour arrêter la prévisualisation.
	 * \param		Inutilisé
	 */
	virtual void preview (bool show, bool destroyInteractor);

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );


	protected slots :

	/**
	 * Appelé lorsque la méthode de projection est modifiée. Actualise
	 * le panneau.
	 */
	virtual void projectionMethodCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopoEntitiesAssociationsPanel (const QtTopoEntitiesAssociationsPanel&);
	QtTopoEntitiesAssociationsPanel& operator = (
										const QtTopoEntitiesAssociationsPanel&);

	/** La méthode de projection utilisée. */
	QComboBox*									_projectionComboBox;

	/** Le widget parent du sous-panneau courant. */
	QWidget*									_currentPanelParentWidget;

	/** Le sous-panneau courant. */
	QtTopoToGeomAssociationPanel*				_currentPanel;

	/** Les sous-panneaux associés aux différentes méthodes de projection. */
	QtTopoToGeomAssociationPanel				*_topoToGeomEntityPanel,
												*_verticesToGeomEntitiesPanel,
												*_edgesToCurvesPanel,
												*_facesToSurfacesPanel;

	/** Faut-il déplacer les sommets topologiques lors du changement
	 * d'association ?
	 */
	QCheckBox*									_moveVerticiesCheckBox;
};	// class QtTopoEntitiesAssociationsPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopoEntitiesAssociationsPanel</I> de modification des éventuelles
 * associations entités topologiques vers entité géométrique.
 */
class QtTopoEntitiesAssociationsAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopoEntitiesAssociationsPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param 		Dimension proposée par défaut
	 */
	QtTopoEntitiesAssociationsAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip,
		Mgx3D::Utils::SelectionManagerIfc::DIM defaultDimensions);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopoEntitiesAssociationsAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres d'association associé.
	 */
	virtual QtTopoEntitiesAssociationsPanel* getAssociationPanel ( );

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
	QtTopoEntitiesAssociationsAction (const QtTopoEntitiesAssociationsAction&);
	QtTopoEntitiesAssociationsAction& operator = (
									const QtTopoEntitiesAssociationsAction&);
};  // class QtTopoEntitiesAssociationsAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPO_ENTITIES_ASSOCIATIONS_ACTION_H
