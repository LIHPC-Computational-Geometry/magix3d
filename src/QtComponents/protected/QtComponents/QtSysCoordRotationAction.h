/**
 * \file		QtSysCoordRotationAction.h
 * \author		Eric Brière de l'Isle
 * \date		7/6/2018
 */
#ifndef QT_SYSCOORD_ROTATION_ACTION_H
#define QT_SYSCOORD_ROTATION_ACTION_H


#include "QtComponents/QtMgx3DRotationPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"

#include "Utils/SelectionManagerIfc.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

class QtMgx3DGroupNamePanel;
class QtMgx3DEntityPanel;

/**
 * Panneau d'édition des paramètres de rotation d'entités topologiques.
 */
class QtSysCoordRotationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtSysCoordRotationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtSysCoordRotationPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );


	/**
	 * \return		le nom du repère sélectionné
	 * \see			getRotation
	 */
	virtual std::string getSysCoordName ( ) const;

	/**
	 * \return		La rotation à effectuer.
	 * \see			getTopoEntitiesNames
	 */
	virtual Utils::Math::Rotation getRotation ( ) const;

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
     * Invoque <I>preview (false, true)</I>.
	 * \see		autoUpdate
	 * \see		preview
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true, true)</I>.
	 * \see		cancel
	 * \see		preview
	 */
	virtual void autoUpdate ( );

	/**
	 * \return		<I>true</I> s'il faut copier les entités avant la transformation
	 */
	virtual bool copyEntities ( ) const;

	/**
	 * @return		Le nom du groupe créé en cas de copie.
	 */
	virtual std::string getGroupName ( ) const;

	protected :

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );

protected slots :

	/**
	 * Appelé lorsque l'on active ou non la copie avant transformation.
	 * (In)active la zone de sélection du nom d'un groupe
	 */
	virtual void copyCallback();

private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtSysCoordRotationPanel (const QtSysCoordRotationPanel&);
	QtSysCoordRotationPanel& operator = (
										const QtSysCoordRotationPanel&);

	/** Le repère à modifier */
	QtMgx3DEntityPanel*			_syscoordPanel;

	/** Le panneau décrivant la rotation. */
	QtMgx3DRotationPanel*						_rotationPanel;

	/** Case à cocher copier. */
	QCheckBox*									_copyCheckBox;

	/** Le nom du groupe créé (en cas de copie). */
	QtMgx3DGroupNamePanel*						_namePanel;

};	// class QtSysCoordRotationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtSysCoordRotationPanel</I> de rotation d'entités topologiques.
 */
class QtSysCoordRotationAction : public QtMgx3DOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtSysCoordRotationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtSysCoordRotationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSysCoordRotationAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de rotation d'entités
	 *				topologiques.
	 */
	virtual QtSysCoordRotationPanel* getRotationPanel ( );

	/**
	 * Actualise le paramétrage des entités topologiques avec le paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtSysCoordRotationAction (const QtSysCoordRotationAction&);
	QtSysCoordRotationAction& operator = (
									const QtSysCoordRotationAction&);
};  // class QtSysCoordRotationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_SYSCOORD_ROTATION_ACTION_H
