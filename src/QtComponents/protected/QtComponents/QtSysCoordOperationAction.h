/**
 * \file		QtSysCoordOperationAction.h
 * \author		Eric Brière de l'Isle
 * \date		7/6/2018
 */
#ifndef QT_SYSCOORD_OPERATION_ACTION_H
#define QT_SYSCOORD_OPERATION_ACTION_H

#include "Utils/Point.h"

#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DPointPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"


#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération de création d'un repère
 */
class QtSysCoordOperationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

public :

	/** La méthode de création du repère. */
	enum OPERATION_METHOD {DEFAUT, CENTRE, TROIS_POINTS };

	/**
	 * Créé l'ihm.
	 * \param parent Widget parent.
	 * \param panelName Nom du panneau.
	 * \param creationPolicy Politique de création du groupe
	 * \param mainWindow Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param action Eventuelle action associée à ce panneau.
	 */
	QtSysCoordOperationPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtSysCoordOperationPanel ( );

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return		Le nom du groupe créé/modifié.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		La méthode de création du repère.
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * Les coordonnées du centre définissant le repère.
	 */
	virtual Utils::Math::Point getCenter() const;

	/**
	 * Les coordonnées du premier point définissant le repère.
	 */
	virtual Utils::Math::Point getPoint1() const;

	/**
	 * Les coordonnées du second point définissant le repère.
	 */
	virtual Utils::Math::Point getPoint2() const;

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

    /**
     * Méthode appelée pour vérifier les paramètres saisis par l'utilisateur.
     * En cas de paramètrage invalide cette méthode doit lever une exception de
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

protected slots :

	/**
	 * Appelé lorsque la méthode change. Actualise le panneau de saisie des
	 * paramètres.
	 */
	virtual void operationMethodCallback ( );

private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtSysCoordOperationPanel (const QtSysCoordOperationPanel&);
	QtSysCoordOperationPanel& operator = (const QtSysCoordOperationPanel&);

	/** Le nom de la boite. */
	QtMgx3DGroupNamePanel*			_namePanel;

	/** La méthode de création du repère. */
	QComboBox*						_operationMethodComboBox;

	/** Coordonnées du centre du repère. */
	QtMgx3DPointPanel*				_centerPanel;

	/** Coordonnées du premier point du repère (Ox). */
	QtMgx3DPointPanel*				_point1Panel;

	/** Coordonnées du second point du repère (oY). */
	QtMgx3DPointPanel*				_point2Panel;

};	// class QtSysCoordOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtSysCoordOperationPanel</I> de création d'un repère.
 */
class QtSysCoordOperationAction : public QtMgx3DOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtSysCoordOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création du groupe
	 */
	QtSysCoordOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSysCoordOperationAction ( );

	/**
	 * \return		Le panneau d'édition de la boite associée.
	 */
	virtual QtSysCoordOperationPanel* getSysCoordPanel ( );

	/**
	 * Créé/modifie le repère avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtSysCoordOperationAction (const QtSysCoordOperationAction&);
	QtSysCoordOperationAction& operator = (
									const QtSysCoordOperationAction&);
};  // class QtSysCoordOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_SYSCOORD_OPERATION_ACTION_H
