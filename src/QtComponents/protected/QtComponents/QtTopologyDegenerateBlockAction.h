/**
 * \file		QtTopologyDegenerateBlockAction.h
 * \author		Charles PIGNEROL
 * \date		07/03/2014
 */
#ifndef QT_TOPOLOGY_DEGENERATE_BLOCK_ACTION_H
#define QT_TOPOLOGY_DEGENERATE_BLOCK_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * <P>Panneau d'édition des paramètres de collage de 2 vertices topologiques
 * entre-eux.</P>
 * <P>On récupère directement des vertices, ou les vertices d'arêtes ou faces
 * sélectionnées.</P
 */
class QtTopologyDegenerateBlockPanel : public QtMgx3DOperationPanel
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
	QtTopologyDegenerateBlockPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyDegenerateBlockPanel ( );

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
	 * \return		Le nom du premier vertex à coller.
	 * \see			getVertice2Name
	 */
	virtual std::string getVertice1Name ( ) const;

	/**
	 * \return		Le nom du seconde vertex à coller.
	 * \see			getVertice1Name
	 */
	virtual std::string getVertice2Name ( ) const;

	/**
	 * \return		<I>true</I> si le second sommet doit être projeté sur le 
	 * 				premier, <I>false</I> s'il le résultat doit être situé
	 * 				entre les 2 sommets.
	 */
	virtual bool projectOnFirst ( ) const;

	/**
	 * \param		Nouvelle dimension de saisie des entités topologiques.
	 */
	virtual void setDimension (Mgx3D::Utils::SelectionManagerIfc::DIM dim);


	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );


	protected slots :

	/**
	 * Appelé lorsque la dimension d'un champ de saisie est modifiée. Affecte
	 * la même dimension à l'autre champ.
	 */
	virtual void dimensionModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyDegenerateBlockPanel (const QtTopologyDegenerateBlockPanel&);
	QtTopologyDegenerateBlockPanel& operator = (const QtTopologyDegenerateBlockPanel&);

	/** Les vertices à coller. */
	QtEntityByDimensionSelectorPanel*	_vertex1Panel;
	QtEntityByDimensionSelectorPanel*	_vertex2Panel;

	/** La case à cocher "projeter sur le premier". */
	QCheckBox*							_projectOnFirstCheckbox;
};	// class QtTopologyDegenerateBlockPanel


/**
 * <P>Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyDegenerateBlockPanel</I> de collage de 2 vertices topologiques
 * entre-eux.</P>
 *
 * <P>On récupère directement des vertices, ou les vertices d'arêtes ou faces
 * sélectionnées.</P
 */
class QtTopologyDegenerateBlockAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyDegenerateBlockPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyDegenerateBlockAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyDegenerateBlockAction ( );

	/**
	 * \return		Le panneau de paramétrage du collage.
	 */
	virtual QtTopologyDegenerateBlockPanel* getTopologyDegenerateBlockPanel ( );

	/**
	 * Colle les vertices topologiques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyDegenerateBlockAction (const QtTopologyDegenerateBlockAction&);
	QtTopologyDegenerateBlockAction& operator = (
									const QtTopologyDegenerateBlockAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_DEGENERATE_BLOCK_ACTION_H
