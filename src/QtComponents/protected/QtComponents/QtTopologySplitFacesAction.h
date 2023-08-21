/**
 * \file		QtTopologySplitFacesAction.h
 * \author		Charles PIGNEROL
 * \date		17/03/2014
 */
#ifndef QT_TOPOLOGY_SPLIT_FACES_ACTION_H
#define QT_TOPOLOGY_SPLIT_FACES_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QtUtil/QtDoubleTextField.h>

#include <QButtonGroup>
#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de découpe de faces topologiques.
 */
class QtTopologySplitFacesPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * Le positionnement de la coupe.
	 */
	enum CUT_DEFINITION_METHOD { CDM_RATIO, CDM_POINT };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologySplitFacesPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologySplitFacesPanel ( );

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
	 * \return		<I>true</I> si toutes les faces sont soumises à la découpe,
	 * 				sinon <I>false</I>.
	 * \see			getFacesNames
	 */
	virtual bool allFaces ( ) const;

	/**
	 * \return		Les noms des faces à découper.
	 * \see			allFaces
	 * \see			getEdgeName
	 */
	virtual std::vector<std::string> getFacesNames ( ) const;

	/**
	 * \return		Le nom de l'arête orthogonale au plan de coupe.
	 * \see			getFacesNames
	 */
	virtual std::string getEdgeName ( ) const;

	/**
	 * \return		La méthode de définition de la coupe.
	 */
	virtual CUT_DEFINITION_METHOD getCutDefinitionMethod ( ) const;

	/**
	 * \return		Le ratio de découpage de l'arête au niveau du plan de coupe
	 * 				orthogonal à l'arête.
	 */
	virtual double getRatio ( ) const;

	/**
	 * \return		Le point de découpe.
	 */
	virtual Mgx3D::Utils::Math::Point getCutPoint ( ) const;

	/**
	 * \return		Le ratio de découpage de l'o-grid en cas de découpage en
	 * 				0-grid (ratio non nul).
	 */
	virtual double getOGridRatio ( ) const;

	/**
	 * \param		<I>true</I> pour prévisualiser les entités concernées par
	 * 				le découpage, <I>false</I> pour arrêter la prévisualisation.
	 * \param		<I>true</I> s'il faut détruire l'éventuel interacteur dans 
	 * 				le cas d'un arrêt de la prévisualisation.
	 */
	virtual void preview (bool show, bool destroyInteractor);


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
	 * Appelé lorsque l'utilisateur change de méthode de définition de la
	 * découpe. Actualise l'IHM.
	 */
	virtual void cutDefinitionModifiedCallback ( );

	/**
	 * Appelé lorsque la case à cocher "Toutes les faces" est modifiée.
	 * Actualise l'IHM et appelle <I>parametersModifiedCallback</I>.
	 * \see		parametersModifiedCallback
	 */
	virtual void facesModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologySplitFacesPanel (const QtTopologySplitFacesPanel&);
	QtTopologySplitFacesPanel& operator = (const QtTopologySplitFacesPanel&);

	/** Les faces à découper. */
	QtMgx3DEntityPanel*			_facesPanel;

	/** La définition de la position de la découpe. */
	QButtonGroup*				_cutDefinitionButtonGroup;

	/** L'arête orthogonale au plan de coupe. */
	QtMgx3DEntityPanel*			_edgePanel;

	/** Le ratio de découpage de l'arête. */
	QtDoubleTextField*			_ratioTextField;

	/** L'entité de définition du positionnement de la coupe. */
	QtMgx3DEntityPanel*			_cutPointEntityPanel;

	/** Le ratio de découpage de l'o-grid. */
	QtDoubleTextField*			_oGridRatioTextField;

	/** La case à cocher "Toutes les faces ?". */
	QCheckBox*					_allFacesCheckBox;
};	// class QtTopologySplitFacesPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologySplitFacesPanel</I> de découpe de faces topologiques.
 */
class QtTopologySplitFacesAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologySplitFacesPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologySplitFacesAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologySplitFacesAction ( );

	/**
	 * \return		Le panneau de paramétrage de la découpe.
	 */
	virtual QtTopologySplitFacesPanel* getTopologySplitFacesPanel ( );

	/**
	 * Découpe les faces topologiques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologySplitFacesAction (const QtTopologySplitFacesAction&);
	QtTopologySplitFacesAction& operator = (const QtTopologySplitFacesAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_SPLIT_FACES_ACTION_H
