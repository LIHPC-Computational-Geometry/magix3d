/**
 * \file		QtTopologyFaceCreationAction.h
 * \author		Charles PIGNEROL
 * \date		20/12/2013
 */
#ifndef QT_TOPOLOGY_FACE_CREATION_ACTION_H
#define QT_TOPOLOGY_FACE_CREATION_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtTopologyCreationAction.h"


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de création d'une face topologique 
 * par association à une surface géométrique.
 */
class QtTopologyFaceCreationPanel : public QtTopologyCreationPanel
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
	QtTopologyFaceCreationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyFaceCreationPanel ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyFaceCreationPanel (const QtTopologyFaceCreationPanel&);
	QtTopologyFaceCreationPanel& operator = (
							const QtTopologyFaceCreationPanel&);
};	// class QtTopologyFaceCreationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyFaceCreationPanel</I> de création de face topologique associée
 * à une surface géométrique.
 */
class QtTopologyFaceCreationAction : public QtTopologyCreationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyFaceCreationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyFaceCreationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyFaceCreationAction ( );

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
	QtTopologyFaceCreationAction (const QtTopologyFaceCreationAction&);
	QtTopologyFaceCreationAction& operator = (
									const QtTopologyFaceCreationAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_FACE_CREATION_ACTION_H
