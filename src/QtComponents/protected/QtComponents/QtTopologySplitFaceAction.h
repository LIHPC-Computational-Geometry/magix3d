/**
 * \file		QtTopologySplitFaceAction.h
 * \author		Charles PIGNEROL
 * \date		17/03/2014
 */
#ifndef QT_TOPOLOGY_SPLIT_FACE_ACTION_H
#define QT_TOPOLOGY_SPLIT_FACE_ACTION_H

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
 * Panneau d'édition des paramètres de découpe d'une face topologique.
 */
class QtTopologySplitFacePanel : public QtMgx3DOperationPanel
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
	QtTopologySplitFacePanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologySplitFacePanel ( );

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
	 * \return		Les nom de la face à découper.
	 * \see			getEdgeName
	 */
	virtual std::string getFaceName ( ) const;

	/**
	 * \return		Le nom de l'arête orthogonale au plan de coupe.
	 * \see			getFaceName
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
	 * \return		<I>true</I> s'il faut projeter les sommets créés sur la
	 *				discrétisation initiale, <I>false</I> dans le cas contraire.
	 */
	virtual bool projectCreatedVertices ( ) const;

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


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologySplitFacePanel (const QtTopologySplitFacePanel&);
	QtTopologySplitFacePanel& operator = (const QtTopologySplitFacePanel&);

	/** La face à découper. */
	QtMgx3DEntityPanel*			_facePanel;

	/** La définition de la position de la découpe. */
	QButtonGroup*				_cutDefinitionButtonGroup;

	/** L'arête orthogonale au plan de coupe. */
	QtMgx3DEntityPanel*			_edgePanel;

	/** Le ratio de découpage de l'arête. */
	QtDoubleTextField*			_ratioTextField;

	/** L'entité de définition du positionnement de la coupe. */
	QtMgx3DEntityPanel*			_cutPointEntityPanel;

	/** Faut-il projeter les sommets créés sur la discrétisation initiale ? */
	QCheckBox*					_projectVerticesCheckBox;
};	// class QtTopologySplitFacePanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologySplitFacePanel</I> de découpe d'une face topologique.
 */
class QtTopologySplitFaceAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologySplitFacePanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologySplitFaceAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologySplitFaceAction ( );

	/**
	 * \return		Le panneau de paramétrage de la découpe.
	 */
	virtual QtTopologySplitFacePanel* getTopologySplitFacePanel ( );

	/**
	 * Découpe la face topologique conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologySplitFaceAction (const QtTopologySplitFaceAction&);
	QtTopologySplitFaceAction& operator = (const QtTopologySplitFaceAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_SPLIT_FACE_ACTION_H
