/**
 * \file		QtTopologyFuse2FacesAction.h
 * \author		Charles PIGNEROL
 * \date		06/03/2014
 */
#ifndef QT_TOPOLOGY_FUSE_2_FACES_ACTION_H
#define QT_TOPOLOGY_FUSE_2_FACES_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de fusion de 2 faces (ou 2 listes de faces)
 * topologiques entre-elles.
 */
class QtTopologyFuse2FacesPanel : public QtMgx3DOperationPanel
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
	QtTopologyFuse2FacesPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyFuse2FacesPanel ( );

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
	 * \return		Le nom des premières faces à coller.
	 * \see			getFaces2Names
	 */
	virtual std::vector<std::string> getFaces1Names ( ) const;

	/**
	 * \return		Le nom des secondes faces à coller.
	 * \see			getFaces1Names
	 */
	virtual std::vector<std::string> getFaces2Names ( ) const;


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
	QtTopologyFuse2FacesPanel (const QtTopologyFuse2FacesPanel&);
	QtTopologyFuse2FacesPanel& operator = (const QtTopologyFuse2FacesPanel&);

	/** Les (listes de) faces à coller. */
	QtMgx3DEntityPanel*			_faces1Panel;
	QtMgx3DEntityPanel*			_faces2Panel;
};	// class QtTopologyFuse2FacesPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyFuse2FacesPanel</I> de collage de 2 faces topologiques
 * entre-elles.
 */
class QtTopologyFuse2FacesAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyFuse2FacesPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyFuse2FacesAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyFuse2FacesAction ( );

	/**
	 * \return		Le panneau de paramétrage du collage.
	 */
	virtual QtTopologyFuse2FacesPanel* getTopologyFuse2FacesPanel ( );

	/**
	 * Colle les faces topologiques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyFuse2FacesAction (const QtTopologyFuse2FacesAction&);
	QtTopologyFuse2FacesAction& operator = (
									const QtTopologyFuse2FacesAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_FUSE_2_FACES_ACTION_H
