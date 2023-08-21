/**
 * \file		QtTopologyInsertHoleAction.h
 * \author		Charles PIGNEROL
 * \date		23/01/2015
 */
#ifndef QT_TOPOLOGY_INSERT_HOLE_ACTION_H
#define QT_TOPOLOGY_INSERT_HOLE_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres d'insertion d'un trou dans une topologie par
 * duplication de cofaces.
 */
class QtTopologyInsertHolePanel : public QtMgx3DOperationPanel
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
	QtTopologyInsertHolePanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyInsertHolePanel ( );

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
	 * \return		Le nom des cofaces à dupliquer.
	 */
	virtual std::vector<std::string> getFacesNames ( ) const;


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
	QtTopologyInsertHolePanel (const QtTopologyInsertHolePanel&);
	QtTopologyInsertHolePanel& operator = (const QtTopologyInsertHolePanel&);

	/** La liste de faces à dupliquer. */
	QtMgx3DEntityPanel*			_facesPanel;
};	// class QtTopologyInsertHolePanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyInsertHolePanel</I> de duplication de cofaces topologiques.
 */
class QtTopologyInsertHoleAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyInsertHolePanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyInsertHoleAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyInsertHoleAction ( );

	/**
	 * \return		Le panneau de paramétrage d'insertion du trou.
	 */
	virtual QtTopologyInsertHolePanel* getTopologyInsertHolePanel ( );

	/**
	 * Duplique les cofaces topologiques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyInsertHoleAction (const QtTopologyInsertHoleAction&);
	QtTopologyInsertHoleAction& operator = (
									const QtTopologyInsertHoleAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_INSERT_HOLE_ACTION_H
