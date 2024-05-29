/**
 * \file		QtTopologyVerticesAlignmentAction.h
 * \author		Charles PIGNEROL
 * \date		16/12/2014
 */
#ifndef QT_TOPOLOGY_VERTICES_ALIGNMENT_ACTION_H
#define QT_TOPOLOGY_VERTICES_ALIGNMENT_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de projection de sommets topologiques sur
 * une droite définie par 2 points.
 */
class QtTopologyVerticesAlignmentPanel : public QtMgx3DOperationPanel
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
	QtTopologyVerticesAlignmentPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyVerticesAlignmentPanel ( );

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
	 * \return		Le premier point définissant la droite de projection.
	 */
	virtual Mgx3D::Utils::Math::Point getPoint1 ( ) const;

	/**
	 * \return		Le second point définissant la droite de projection.
	 */
	virtual Mgx3D::Utils::Math::Point getPoint2 ( ) const;

	/**
	 * \return		Les noms des sommets à aligner.
	 * \see			snapAllVertices
	 */
	virtual std::vector<std::string> getVerticesNames ( ) const;


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
	QtTopologyVerticesAlignmentPanel (const QtTopologyVerticesAlignmentPanel&);
	QtTopologyVerticesAlignmentPanel& operator = (
										const QtTopologyVerticesAlignmentPanel&);
	/** Les 2 points définissant la droite de projection. */
	QtEntityIDTextField			*_point1TextField, *_point2TextField;

	/** Les sommets à aligner. */
	QtMgx3DEntityPanel*			_verticesPanel;
};	// class QtTopologyVerticesAlignmentPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyVerticesAlignmentPanel</I> d'alignement de sommets topologiques.
 */
class QtTopologyVerticesAlignmentAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyVerticesAlignmentPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyVerticesAlignmentAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyVerticesAlignmentAction ( );

	/**
	 * \return		Le panneau de paramétrage de l'alignement.
	 */
	virtual QtTopologyVerticesAlignmentPanel* getTopologyVerticesAlignmentPanel ( );

	/**
	 * Aligne les sommets topologiques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyVerticesAlignmentAction (const QtTopologyVerticesAlignmentAction&);
	QtTopologyVerticesAlignmentAction& operator = (
									const QtTopologyVerticesAlignmentAction&);
};  // class QtTopologyVerticesAlignmentAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_VERTICES_ALIGNMENT_ACTION_H
