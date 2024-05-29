/**
 * \file		QtTopologyUnrefineBlockAction.h
 * \author		Charles PIGNEROL
 * \date		11/12/2014
 */
#ifndef QT_TOPOLOGY_UNREFINE_BLOCK_ACTION_H
#define QT_TOPOLOGY_UNREFINE_BLOCK_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QtUtil/QtIntTextField.h>

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de déraffinement d'un bloc topologique.
 */
class QtTopologyUnrefineBlockPanel : public QtMgx3DOperationPanel
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
	QtTopologyUnrefineBlockPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyUnrefineBlockPanel ( );

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
	 * \return		Le nom du bloc à déraffiner.
	 * \see			getEdgeName
	 */
	virtual std::string getBlockName ( ) const;

	/**
	 * \return		Le nom de l'arête le long de laquelle s'effectue le
	 * 				déraffinement.
	 * \see			getBlockName
	 */
	virtual std::string getEdgeName ( ) const;

	/**
	 * \return		Le ratio de déraffinement.
	 */
	virtual unsigned short getRatio ( ) const;

	/**
	 * \param		<I>true</I> pour prévisualiser les entités concernées par
	 * 				le déraffinement, <I>false</I> pour arrêter la
	 * 				prévisualisation.
	 * \param		<I>true</I> s'il faut détruire l'éventuel interacteur dans
	 * 				le cas d'un arrêt de la prévisualisation.
	 */
//	virtual void preview (bool show, bool destroyInteractor);


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
	QtTopologyUnrefineBlockPanel (const QtTopologyUnrefineBlockPanel&);
	QtTopologyUnrefineBlockPanel& operator = (
										const QtTopologyUnrefineBlockPanel&);

	/** Le bloc à déraffiner. */
	QtMgx3DEntityPanel*			_blockPanel;

	/** L'arête le long de laquelle s'effectue le déraffinement. */
	QtMgx3DEntityPanel*			_edgePanel;

	/** La fréquence de déraffinement. */
	QtIntTextField*				_ratioTextField;
};	// class QtTopologyUnrefineBlockPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyUnrefineBlockPanel</I> de déraffinement de bloc topologique.
 */
class QtTopologyUnrefineBlockAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyUnrefineBlockPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyUnrefineBlockAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyUnrefineBlockAction ( );

	/**
	 * \return		Le panneau de déraffinement du bloc.
	 */
	virtual QtTopologyUnrefineBlockPanel* getTopologyUnrefineBlockPanel ( );

	/**
	 * Déraffine le bloc topologique conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyUnrefineBlockAction (const QtTopologyUnrefineBlockAction&);
	QtTopologyUnrefineBlockAction& operator = (
									const QtTopologyUnrefineBlockAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_UNREFINE_BLOCK_ACTION_H
