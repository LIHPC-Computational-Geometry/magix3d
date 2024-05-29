/**
 * \file		QtTopologySnapVerticesAction.h
 * \author		Charles PIGNEROL
 * \date		31/03/2014
 */
#ifndef QT_TOPOLOGY_SNAP_VERTICES_ACTION_H
#define QT_TOPOLOGY_SNAP_VERTICES_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de projection de sommets topologiques
 * sur leur géométrie associée.
 */
class QtTopologySnapVerticesPanel : public QtMgx3DOperationPanel
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
	QtTopologySnapVerticesPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologySnapVerticesPanel ( );

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
	 * Faut-il projeter tous les sommets ou uniquement ceux saisis ?
	 * \see			getVerticesNames
	 */
	virtual bool snapAllVertices ( ) const;

	/**
	 * \return		Les noms des sommets à projeter.
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
	QtTopologySnapVerticesPanel (const QtTopologySnapVerticesPanel&);
	QtTopologySnapVerticesPanel& operator = (
										const QtTopologySnapVerticesPanel&);

	/** Les sommets à projeter. */
	QtMgx3DEntityPanel*			_verticesPanel;

	/** Faut-il projeter tous les sommets ? */
	QCheckBox					*_allVerticesCheckBox;
};	// class QtTopologySnapVerticesPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologySnapVerticesPanel</I> de projection de sommets topologiques
 * sur leur géométrie associée.
 */
class QtTopologySnapVerticesAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologySnapVerticesPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologySnapVerticesAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologySnapVerticesAction ( );

	/**
	 * \return		Le panneau de paramétrage de la projection.
	 */
	virtual QtTopologySnapVerticesPanel* getTopologySnapVerticesPanel ( );

	/**
	 * Projete les sommets topologiques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologySnapVerticesAction (const QtTopologySnapVerticesAction&);
	QtTopologySnapVerticesAction& operator = (
									const QtTopologySnapVerticesAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_SNAP_VERTICES_ACTION_H
