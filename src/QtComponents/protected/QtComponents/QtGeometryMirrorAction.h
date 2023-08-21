/**
 * \file		QtGeometryMirrorAction.h
 * \author		Eric B
 * \date		12/4/2016
 */
#ifndef QT_GEOMETRY_MIRROR_ACTION_H
#define QT_GEOMETRY_MIRROR_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DPointPanel.h"
#include "QtComponents/QtMgx3DVectorPanel.h"
#include "QtComponents/RenderingManager.h"

#include "Utils/Vector.h"
#include "Utils/SelectionManagerIfc.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

class QtMgx3DGroupNamePanel;

/**
 * Panneau d'édition des paramètres de symétrie d'entités géomtriques.
 */
class QtGeometryMirrorPanel :
		public QtMgx3DOperationPanel,
		public Mgx3D::QtComponents::RenderingManager::InteractorObserver
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
	 */
	QtGeometryMirrorPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGeometryMirrorPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * \param		Nouvelle dimension de saisie des entités géométriques.
	 */
	virtual void setDimension (Mgx3D::Utils::SelectionManagerIfc::DIM dim);

//	/**
//	 * \return		<I>true</I> s'il faut soumettre toutes les entités
//	 * 				géométriques à la symétrie, <I>false</I> s'il faut juste
//	 * 				traiter la sélection désignée par l'utilisateur.
//	 * \see			getInvolvedEntities
//	 */
//	virtual bool processAllGeomEntities ( ) const;

	/**
	 * \return		<I>true</I> s'il faut copier les entités avant la transformation
	 */
	virtual bool copyEntities ( ) const;

	/**
	 * @return		Le nom du groupe créé en cas de copie.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		<I>true</I> s'il faut également copier les entités topologiques
	 */
	virtual bool copyTopoEntities ( ) const;

	/**
	 * \return		La liste des entités géométriques devant effectuer une
	 *				symétrie.
	 * \see			getTranslation
	 * \see			doPropagate
	 * \see			processAllGeomEntities
	 */
	virtual std::vector<std::string> getGeomEntitiesNames ( ) const;

	/// les coordonnées du point définissant le plan de symétrie
	virtual Mgx3D::Utils::Math::Point getPoint ( ) const;
	/**
	 * Les coordonnées du vecteur définissant la normale au plan.
	 */
	virtual Mgx3D::Utils::Math::Vector getNormal ( ) const;


	/**
	 * \return		<I>true</I> s'il faut également faire effectuer la symétrie
	 *				aux entités géométriques associées.
	 * \see			getGeomEntitiesNames
	 */
	virtual bool doPropagate ( ) const;

    /**
     * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
     * Restitue l'environnement dans son état initial.
     * Invoque <I>preview (false, true)</I>.
	 * \see		autoUpdate
	 * \see		preview
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true, true)</I>.
	 * \see		cancel
	 * \see		preview
	 */
	virtual void autoUpdate ( );


	protected :

	/**
	 * \param	<I>true</I> pour prévisualiser les entités concernées par
	 * 			l'association, <I>false</I> pour arrêter la prévisualisation.
	 * \param	<I>true</I> s'il faut détruire l'éventuel interacteur dans le
	 *			cas d'un arrêt de la prévisualisation.
	 */
	virtual void preview (bool show, bool destroyInteractor);

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return		Un pointeur sur l'éventuel interacteur en cours
	 * 				d'utilisation.
	 */
	virtual Mgx3D::QtComponents::RenderingManager::PlaneInteractor*
															getInteractor ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );


	protected slots :
	/**
	 * Appelé lorsqu'un paramètre du plan est modifié.
	 * Appelle <I>preview (true)</I>.
	 */
	virtual void cutModifiedCallback ( );

	//
//	/**
//	 * Appelé lorsque la case à cocher "toutes les entités" change d'état.
//	 * (In)active la zone de saisie des entités soumises à la transformation.
//	 */
//	virtual void allEntitiesCallback ( );

	/**
	 * Appelé lorsque l'on active ou non la copie avant transformation.
	 * (In)active la zone de sélection du nom d'un groupe ainsi que
	 * l'option de copie de la topologie associée.
	 */
	virtual void copyCallback();


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtGeometryMirrorPanel (const QtGeometryMirrorPanel&);
	QtGeometryMirrorPanel& operator = (
										const QtGeometryMirrorPanel&);

	/** Les dimensions possibles pour les entités géométriques suivant la
	 * translation. */
	QtEntityByDimensionSelectorPanel*				_geomEntitiesPanel;

//	/** Case à cocher <I>Toutes les entités</I>. */
//	QCheckBox*										_allEntitiesCheckBox;

	/** Case à cocher <I>propager</I>. */
	QCheckBox*										_propagateCheckBox;

	/** Coordonnées du point définissant le plan. */
	QtMgx3DPointPanel*								_pointPanel;

	/** Le vecteur normal au plan. */
	QtMgx3DVectorPanel*								_normalPanel;

	/** Case à cocher copier. */
	QCheckBox*										_copyCheckBox;

	/** Le nom du groupe créé (en cas de copie). */
	QtMgx3DGroupNamePanel*							_namePanel;

	/** Case à cocher copier avec la topologie</I>. */
	QCheckBox*										_withTopologyCheckBox;
};	// class QtGeometryMirrorPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeometryMirrorPanel</I> de translation d'entités géométriques.
 */
class QtGeometryMirrorAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeometryMirrorPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtGeometryMirrorAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeometryMirrorAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de symétrie d'entités
	 *				géométriques.
	 */
	virtual QtGeometryMirrorPanel* getMirrorPanel ( );

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
	QtGeometryMirrorAction (const QtGeometryMirrorAction&);
	QtGeometryMirrorAction& operator = (
									const QtGeometryMirrorAction&);
};  // class QtGeometryMirrorAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOMETRY_MIRROR_ACTION_H
