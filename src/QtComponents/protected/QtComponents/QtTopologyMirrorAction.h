/**
 * \file		QtTopologyMirrorAction.h
 * \author		Eric B
 * \date		13/4/2016
 */
#ifndef QT_TOPOLOGY_MIRROR_ACTION_H
#define QT_TOPOLOGY_MIRROR_ACTION_H


#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"
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

/**
 * Panneau d'édition des paramètres de symétrie d'entités géomtriques.
 */
class QtTopologyMirrorPanel :
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
	QtTopologyMirrorPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtTopologyMirrorPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * \param		Nouvelle dimension de saisie des entités topologiques.
	 */
	virtual void setDimension (Mgx3D::Utils::SelectionManagerIfc::DIM dim);


	/**
	 * \return		La liste des entités topologiques devant effectuer une
	 *				symétrie.
	 * \see			getTranslation
	 */
	virtual std::vector<std::string> getTopoEntitiesNames ( ) const;

	/// les coordonnées du point définissant le plan de symétrie
	virtual Mgx3D::Utils::Math::Point getPoint ( ) const;
	/**
	 * Les coordonnées du vecteur définissant la normale au plan.
	 */
	virtual Mgx3D::Utils::Math::Vector getNormal ( ) const;

	/**
	 * \return		<I>true</I> s'il faut également faire effectuer la symétrie
	 *				aux entités géométriques associées.
	 * \see			getTopoEntitiesNames
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



	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyMirrorPanel (const QtTopologyMirrorPanel&);
	QtTopologyMirrorPanel& operator = (
										const QtTopologyMirrorPanel&);

	/** Les dimensions possibles pour les entités topologiques suivant la
	 * translation. */
	QtEntityByDimensionSelectorPanel*				_topoEntitiesPanel;

	/** Case à cocher <I>propager</I> à la géométrie. */
	QCheckBox*										_propagateCheckBox;

	/** Coordonnées du point définissant le plan. */
	QtMgx3DPointPanel*								_pointPanel;

	/** Le vecteur normal au plan. */
	QtMgx3DVectorPanel*								_normalPanel;

};	// class QtTopologyMirrorPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyMirrorPanel</I> de translation d'entités topologiques.
 */
class QtTopologyMirrorAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyMirrorPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyMirrorAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyMirrorAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de symétrie d'entités
	 *				topologiques.
	 */
	virtual QtTopologyMirrorPanel* getMirrorPanel ( );

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
	QtTopologyMirrorAction (const QtTopologyMirrorAction&);
	QtTopologyMirrorAction& operator = (
									const QtTopologyMirrorAction&);
};  // class QtTopologyMirrorAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_MIRROR_ACTION_H
