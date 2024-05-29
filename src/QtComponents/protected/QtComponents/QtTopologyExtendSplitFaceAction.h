/**
 * \file		QtTopologyExtendSplitFaceAction.h
 * \author		Charles PIGNEROL
 * \date		22/05/2014
 */
#ifndef QT_TOPOLOGY_EXTEND_SPLIT_FACE_ACTION_H
#define QT_TOPOLOGY_EXTEND_SPLIT_FACE_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de prolongation de découpe d'une face
 * topologique à partir d'un sommet localisé sur une arête de la face.
 */
class QtTopologyExtendSplitFacePanel : public QtMgx3DOperationPanel
{
	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyExtendSplitFacePanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyExtendSplitFacePanel ( );

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
	 * \return		Le nom de la face à découper.
	 * \see			getVertexName
	 */
	virtual std::string getFaceName ( ) const;

	/**
	 * \return		Le nom du sommet de définition de la prolongation de
	 * 				la coupe.
	 * \see			getFaceName
	 */
	virtual std::string getVertexName ( ) const;

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


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyExtendSplitFacePanel (const QtTopologyExtendSplitFacePanel&);
	QtTopologyExtendSplitFacePanel& operator = (
										const QtTopologyExtendSplitFacePanel&);

	/** La face à découper. */
	QtMgx3DEntityPanel*			_facePanel;

	/** Le sommet situé sur une arête de prolongation de la coupe. */
	QtMgx3DEntityPanel*			_vertexPanel;
};	// class QtTopologyExtendSplitFacePanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyExtendSplitFacePanel</I> de découpe de face topologique.
 */
class QtTopologyExtendSplitFaceAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyExtendSplitFacePanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyExtendSplitFaceAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyExtendSplitFaceAction ( );

	/**
	 * \return		Le panneau de paramétrage de prolongation de la la découpe.
	 */
	virtual QtTopologyExtendSplitFacePanel* getTopologyExtendSplitFacePanel ( );

	/**
	 * Prolonge la découpe de la face topologique conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyExtendSplitFaceAction (const QtTopologyExtendSplitFaceAction&);
	QtTopologyExtendSplitFaceAction& operator = (
									const QtTopologyExtendSplitFaceAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_EXTEND_SPLIT_FACE_ACTION_H
