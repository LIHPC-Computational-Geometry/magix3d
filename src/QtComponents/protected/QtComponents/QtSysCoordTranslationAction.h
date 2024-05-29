/**
 * \file		QtSysCoordTranslationAction.h
 * \author		Eric Brière de l'Isle
 * \date		8/6/2018
 */
#ifndef QT_SYSCOORD_TRANSLATION_ACTION_H
#define QT_SYSCOORD_TRANSLATION_ACTION_H

#include "Internal/ContextIfc.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"

#include "Utils/Vector.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

class QtMgx3DGroupNamePanel;
class QtMgx3DEntityPanel;
class QtMgx3DVectorPanel;

/**
 * Panneau d'édition des paramètres de Translation d'entités topologiques.
 */
class QtSysCoordTranslationPanel : public QtMgx3DOperationPanel
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
	QtSysCoordTranslationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtSysCoordTranslationPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );


	/**
	 * \return		le nom du repère sélectionné
	 * \see			getTranslation
	 */
	virtual std::string getSysCoordName ( ) const;

	/**
	 * \return		La Translation à effectuer.
	 * \see			getTopoEntitiesNames
	 */
	virtual Utils::Math::Vector getTranslation ( ) const;

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
	QtSysCoordTranslationPanel (const QtSysCoordTranslationPanel&);
	QtSysCoordTranslationPanel& operator = (
										const QtSysCoordTranslationPanel&);

	/** Le repère à modifier */
	QtMgx3DEntityPanel*						_syscoordPanel;

	/** Le panneau décrivant la Translation. */
	QtMgx3DVectorPanel*						_translationPanel;

	/** Case à cocher copier. */
	QCheckBox*								_copyCheckBox;

	/** Le nom du groupe créé (en cas de copie). */
	QtMgx3DGroupNamePanel*					_namePanel;

};	// class QtSysCoordTranslationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtSysCoordTranslationPanel</I> de Translation d'entités topologiques.
 */
class QtSysCoordTranslationAction : public QtMgx3DOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtSysCoordTranslationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtSysCoordTranslationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSysCoordTranslationAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de Translation d'entités
	 *				topologiques.
	 */
	virtual QtSysCoordTranslationPanel* getTranslationPanel ( );

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
	QtSysCoordTranslationAction (const QtSysCoordTranslationAction&);
	QtSysCoordTranslationAction& operator = (
									const QtSysCoordTranslationAction&);
};  // class QtSysCoordTranslationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_SYSCOORD_TRANSLATION_ACTION_H
