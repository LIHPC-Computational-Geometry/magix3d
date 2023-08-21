/**
 * \file		QtBoxOperationAction.h
 * \author		Charles PIGNEROL
 * \date		06/09/2013
 */
#ifndef QT_BOX_OPERATION_ACTION_H
#define QT_BOX_OPERATION_ACTION_H

#include "Utils/Point.h"

#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DPointPanel.h"
#include "QtComponents/QtTopologyPanel.h"

#include <QtUtil/Qt3DDataPanel.h>
#include <QtUtil/QtDoubleTextField.h>

#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération géométrique touchant
 * une boite.
 */
class QtBoxOperationPanel : public QtMgx3DOperationPanel
{
	public :

	/** La méthode de création/modification de la boite. */
	enum OPERATION_METHOD {POINT_POINT };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Politique de création/modification du groupe
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtBoxOperationPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtBoxOperationPanel ( );

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return		Le nom du groupe créé/modifié.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		La méthode de création/modification de la boite.
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * Les coordonnées du premier point définissant la boite.
	 */
	virtual Utils::Math::Point getPoint1() const;

	/**
	 * Les coordonnées du second point définissant la boite.
	 */
	virtual Utils::Math::Point getPoint2() const;

	/**
	 * \return		<I>true</I> s'il faut créer une topologie avec, <I>false</I>
	 *				dans le cas contraire.
	 */
	virtual bool createTopology ( ) const;

	/**
	 * \return	Le type de topologie à créer.
	 * \see		createTopology
	 */
	virtual QtTopologyPanel::TOPOLOGY_TYPE getTopologyType ( ) const;

	/**
	 * \return	Le nombre de bras suivant l'axe des abscisses, l'axe des
	 *			ordonnées et l'axe des élévations.
	 */
	virtual int getNi ( ) const;
	virtual int getNj ( ) const;
	virtual int getNk ( ) const;

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

    /**
     * Méthode appelée pour vérifier les paramètres saisis par l'utilisateur.
     * En cas de paramètrage invalide cette méthode doit leve une exception de
     * type <I>TkUtil::Exception</I> contenant un descriptif des erreurs
     * rencontrées.
     */
    virtual void validate ( );

    /**
     * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
     * Restitue l'environnement dans son état initial.
	 * \see		autoUpdate
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );

	/**
	 * \param		<I>true</I> pour prévisualiser les entités créées,
	 *              <I>false</I> pour arrêter la prévisualisation.
	 * \param		<I>true</I> s'il faut détruire l'éventuel interacteur dans
	 * 				le cas d'un arrêt de la prévisualisation.
	 */
	virtual void preview (bool show, bool destroyInteractor);


	protected :

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * Invoque <I>preview (false, true).</I>
	 * \see		preview
	 */
	virtual void operationCompleted ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtBoxOperationPanel (const QtBoxOperationPanel&);
	QtBoxOperationPanel& operator = (const QtBoxOperationPanel&);

	/** Le nom de la boite. */
	QtMgx3DGroupNamePanel*			_namePanel;

	/** La méthode de création/modification de la boite. */
	QComboBox*						_operationMethodComboBox;

	/** Coordonnées du premier point de la boite. */
	QtMgx3DPointPanel*				_point1Panel;

	/** Coordonnées du second point de la boite. */
	QtMgx3DPointPanel*				_point2Panel;

	/** L'éventuelle topologie associée. */
	QtTopologyPanel*				_topologyPanel;
};	// class QtBoxOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtBoxOperationPanel</I> de création/modification d'une boite.
 */
class QtBoxOperationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtBoxOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtBoxOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtBoxOperationAction ( );

	/**
	 * \return		Le panneau d'édition de la boite associée.
	 */
	virtual QtBoxOperationPanel* getBoxPanel ( );

	/**
	 * Créé/modifie la boite avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtBoxOperationAction (const QtBoxOperationAction&);
	QtBoxOperationAction& operator = (
									const QtBoxOperationAction&);
};  // class QtBoxOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_BOX_OPERATION_ACTION_H
