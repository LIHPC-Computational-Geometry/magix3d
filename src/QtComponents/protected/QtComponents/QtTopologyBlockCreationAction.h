/**
 * \file		QtTopologyBlockCreationAction.h
 * \author		Charles PIGNEROL
 * \date		20/12/2013
 */
#ifndef QT_TOPOLOGY_BLOCK_CREATION_ACTION_H
#define QT_TOPOLOGY_BLOCK_CREATION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtTopologyCreationAction.h"


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de création d'un bloc topologique 
 * par association à un volume géométrique.
 */
class QtTopologyBlockCreationPanel : public QtTopologyCreationPanel
{
	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyBlockCreationPanel (
			QWidget* parent, 
			const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyBlockCreationPanel ( );

	/**
	 * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
	 * cf. spécifications de la classe de base.
	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * cf. spécifications de la classe de base.
	 */
	virtual void autoUpdate ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyBlockCreationPanel (const QtTopologyBlockCreationPanel&);
	QtTopologyBlockCreationPanel& operator = (
							const QtTopologyBlockCreationPanel&);
};	// class QtTopologyBlockCreationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyBlockCreationPanel</I> de création de bloc topologique associé
 * à un volume géométrique.
 */
class QtTopologyBlockCreationAction : public QtTopologyCreationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyBlockCreationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyBlockCreationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyBlockCreationAction ( );

	/**
	 * Créé le bloc topologique conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyBlockCreationAction (const QtTopologyBlockCreationAction&);
	QtTopologyBlockCreationAction& operator = (
									const QtTopologyBlockCreationAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_BLOCK_CREATION_ACTION_H
