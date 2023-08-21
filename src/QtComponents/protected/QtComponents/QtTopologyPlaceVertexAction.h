/**
 * \file		QtTopologyPlaceVertexAction.h
 * \author		Charles PIGNEROL
 * \date		01/02/2017
 */
#ifndef QT_TOPOLOGY_PLACE_VERTEX_ACTION_H
#define QT_TOPOLOGY_PLACE_VERTEX_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QtUtil/QtIntTextField.h>
#include <QtUtil/QtDoubleTextField.h>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau de définition de la position d'un sommet topologique.
 */
class QtTopologyPlaceVertexPanel :
	public QtMgx3DOperationPanel,
	public Mgx3D::QtComponents::RenderingManager::InteractorObserver
{
	Q_OBJECT

	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyPlaceVertexPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyPlaceVertexPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
	 * Restitue l'environnement dans son état initial.
	 * Invoque <I>preview (false)</I>.
	 * \see	preview
	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true, true)</I>.
	 * \see	preview
	 */
	virtual void autoUpdate ( );

	/**
	 * Méthode appelée lorsque la définition du point est modifiée par
	 * l'utilisateur via un interacteur 3D dans la fenêtre graphique.
	 * Actualise le panneau.
	 */
	virtual void pointModifiedCallback (Mgx3D::Utils::Math::Point point);

	/**
	 * \param		<I>true</I> pour prévisualiser les entités concernées par
	 * 				l'association, <I>false</I> pour arrêter la
	 * 				prévisualisation.
	 * \param		<I>true</I> s'il faut détruire l'éventuel interacteur dans
	 * 				le cas d'un arrêt de la prévisualisation.
	 */
	virtual void preview (bool show, bool destroyInteractor);

	/**
	 * \return		Le nom du sommet à positionner.
	 */
	virtual std::string getVertexName ( ) const;

	/**
	 * \return		Les nouvelles composantes du sommet.
	 */
	virtual Mgx3D::Utils::Math::Point getPoint ( ) const;
	virtual double getX ( ) const;
	virtual double getY ( ) const;
	virtual double getZ ( ) const;

	/**
	 * \return	<I>true</I> si le positionnement est contraint sur une
	 *		entité, <I>false</I> dans le cas contraire.
	 */
	virtual bool isContrained ( ) const;


	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return		L'éventuel sommet à positionner.
	 */
	virtual Mgx3D::Topo::Vertex* getVertex ( ) const;

	/**
	 * \return		Un pointeur sur l'éventuel interacteur en cours
	 * 			d'utilisation.
	 */
	virtual Mgx3D::QtComponents::RenderingManager::PointInteractor* getInteractor ( );

	/**
	 * \return		Un pointeur sur l'éventuel interacteur en cours
	 * 			d'utilisation.
	 */
	virtual Mgx3D::QtComponents::RenderingManager::ConstrainedPointInteractor* getConstrainedInteractor ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );

	/**
	 * Invoqué lorsque des entités sont ajoutées à la sélection.
	 * Actualise les champs de saisie X, Y et Z, puis invoque
	 * QtMgx3DOperationsPanel::entitiesAddedToSelectionCallback.
	 */
	virtual void entitiesAddedToSelectionCallback (QString entitiesNames);

	/**
	 * Invoqué lorsque des entités sont enlevées de la sélection.
	 * Affecte 0. aux champs de saisie X, Y et Z, puis invoque
	 * QtMgx3DOperationsPanel::entitiesRemovedFromSelectionCallback.
	 */
	virtual void entitiesRemovedFromSelectionCallback (QString entitiesNames);

	/**
	 * \return		Les dimensions souhaitées de la boite de l'interacteur.
	 */
	virtual void getInteractorBoxDims (double& dx, double& dy, double& dz);


	protected slots :

	/**
	 * Invoqué lorsque l'utilisateur modifie les coordonnées du point via les
	 * champs de saisie. Actualise l'interacteur.
	 */
	virtual void coordinatesModifiedCallback ( );

	/**
	 * Modifier la taille de l'interacteur et donc l'amplitude de déplacement
	 * du sommet.
	 */
	virtual void interactorSizeCallback ( );

	/**
	 * Invoqué lorsque l'utilisateur souhaite passer du mode contraint au mode
	 * non contraint ou réciproquement.
	 */
	virtual void constraintCallback ( );

	/**
	 * La contrainte est (dé)sélectionnée. Actualise l'interacteur.
	 */
	virtual void constraintSelectedCallback (QString);
	virtual void constraintUnselectedCallback (QString);


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyPlaceVertexPanel (const QtTopologyPlaceVertexPanel&);
	QtTopologyPlaceVertexPanel& operator = (
										const QtTopologyPlaceVertexPanel&);

	/** Le sommet à positionner. */
	QtMgx3DEntityPanel			*_vertexPanel;

	/** Les nouvelles composantes des coordonnées du sommet. */
	QtDoubleTextField			*_xTextField, *_yTextField, *_zTextField;

	/** Modifier l'amplitude de déplacement. */
	QLabel					*_sliderLabel;
	QSlider					*_interactorSlider;

	/** La saisie est elle contrainte sur une entité visible ? */
	QCheckBox				*_constraintCheckBox;

	/** La contrainte. */
	QtEntityByDimensionSelectorPanel	*_entityConstraintPanel;

	/** Le facteur de raffinement de l'entité de contrainte (facteur multiplicatif
	 * de discrétisation de l'affichage de cette entité pour saisie interactive
	 * précise). */
	QtIntTextField				*_refinementFactorTextField;
};	// class QtTopologyPlaceVertexPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyPlaceVertexPanel</I> de positionnement d'un sommet topologique.
 */
class QtTopologyPlaceVertexAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyPlaceVertexPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyPlaceVertexAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyPlaceVertexAction ( );

	/**
	 * \return		Le panneau de paramétrage du positionnement.
	 */
	virtual QtTopologyPlaceVertexPanel* getTopologyProjectVerticesPanel ( );

	/**
	 * Positionne le sommet topologique conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyPlaceVertexAction (const QtTopologyPlaceVertexAction&);
	QtTopologyPlaceVertexAction& operator = (
									const QtTopologyPlaceVertexAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_PLACE_VERTEX_ACTION_H
