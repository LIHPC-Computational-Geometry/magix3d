/**
 * \file		QtGeomEntityCopyByOffsetAction.h
 * \author		Charles PIGNEROL
 * \date		25/09/2019
 */
#ifndef QT_GEOM_ENTITY_COPY_BY_OFFSET_ACTION_H
#define QT_GEOM_ENTITY_COPY_BY_OFFSET_ACTION_H


#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"

#include <QtUtil/QtDoubleTextField.h>

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de création par copie d'entités 
 * géométriques avec décallage de l'entité créée.
 */
class QtGeomEntityCopyByOffsetPanel : public QtMgx3DOperationPanel
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
	QtGeomEntityCopyByOffsetPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGeomEntityCopyByOffsetPanel ( );

	/**
	 * @return		Le nom du groupe créé.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		L'entité géométriques à copier.
	 */
	virtual std::string getGeomEntityName ( ) const;
	
	/**
	 * @return		La distance séparant l'entité de sa copie.
	 */
	 virtual double getOffset ( ) const;

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
     * Invoque <I>preview (false)</I>.
	 * \see		autoUpdate
	 * \see		preview
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true)</I>.
	 * \see		cancel
	 * \see		preview
	 */
	virtual void autoUpdate ( );


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


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtGeomEntityCopyByOffsetPanel (const QtGeomEntityCopyByOffsetPanel&);
	QtGeomEntityCopyByOffsetPanel& operator = (const QtGeomEntityCopyByOffsetPanel&);

	/** Le nom du groupe créé. */
	QtMgx3DGroupNamePanel*						_namePanel;

	/** L'entité géométrique à copier. */
	QtMgx3DEntityPanel*							_geomEntityPanel;

	/** La distance entre l'entité copiée et la copie. */
	QtDoubleTextField*							_offsetTextField;
};	// class QtGeomEntityCopyByOffsetPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeomEntityCopyByOffsetPanel</I> de création par copie d'entités 
 * géométriques avec décallage de l'entité créée.
 */
class QtGeomEntityCopyByOffsetAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe <I>QtGeomEntityCopyByOffsetPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtGeomEntityCopyByOffsetAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeomEntityCopyByOffsetAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de copie d'entité
	 *				géométrique avec décallage.
	 */
	virtual QtGeomEntityCopyByOffsetPanel* getCopyPanel ( );

	/**
	 * Actualise le paramétrage de l'entité géométrique avec le paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGeomEntityCopyByOffsetAction (const QtGeomEntityCopyByOffsetAction&);
	QtGeomEntityCopyByOffsetAction& operator = (const QtGeomEntityCopyByOffsetAction&);
};  // class QtGeomEntityCopyByOffsetAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOM_ENTITY_COPY_BY_OFFSET_ACTION_H
