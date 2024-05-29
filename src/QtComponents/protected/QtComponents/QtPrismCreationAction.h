/**
 * \file		QtPrismCreationAction.h
 * \author		Charles PIGNEROL
 * \date		14/11/2014
 *
 *
 *
 * Modified on: 21/02/2022
 *      Author: Simon C
 *      ajout de la possibilité de conserver les entités géométriques
 */
#ifndef QT_PRISM_CREATION_ACTION_H
#define QT_PRISM_CREATION_ACTION_H


#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DVectorPanel.h"


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau de création d'un volume de type prisme à partir d'une surface.
 */
class QtPrismCreationPanel : public QtMgx3DOperationPanel
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
	QtPrismCreationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtPrismCreationPanel ( );

	/**
	 * \return		Les noms des surfaces à extruder.
	 */
	virtual std::vector<std::string> getSurfaceNames ( ) const;

	/**
	 * \return		Le vecteur définissant l'extrusion efectuée.
	 */
	virtual Mgx3D::Utils::Math::Vector getVector ( ) const;

	/**
	 * \return		<I>true</I> s'il faut également extruder les entités
	 *				topologiques associées.
	 * \see			getGeomEntitiesNames
	 */
	virtual bool withTopology ( ) const;

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
     * \return		<I>true</I> s'il faut conserver les entités initiales,
    *				<I>false</I> dans le cas contraire.
    */
    virtual bool keepEntities ( ) const;

	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */
	 virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

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
	QtPrismCreationPanel (
									const QtPrismCreationPanel&);
	QtPrismCreationPanel& operator = (
									const QtPrismCreationPanel&);

	/** Les surfaces à partir desquelles sont créées les volumes (et les blocs si demandé). */
	QtMgx3DEntityPanel*				_surfacesPanel;

	/** Le vecteur appliqué à la surface pour extruder le prisme. */
	QtMgx3DVectorPanel*				_vectorPanel;

	/** Case à cocher <I>avec la topologie</I>. */
	QCheckBox*						_withTopologyCheckBox;

    /** Conserver les entités initiales ? */
    QCheckBox*						_keepEntitiesCheckBox;

};	// class QtPrismCreationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtPrismCreationPanel</I> de création d'un bloc par rotation
 * d'arêtes.
 */
class QtPrismCreationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtPrismCreationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtPrismCreationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtPrismCreationAction ( );

	/**
	 * \return		Le panneau d'édition du bloc associé.
	 */
	virtual QtPrismCreationPanel* getPrismPanel ( );

	/**
	 * Créé/modifie le bloc avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DtopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtPrismCreationAction (
								const QtPrismCreationAction&);
	QtPrismCreationAction& operator = (
								const QtPrismCreationAction&);
};  // class QtPrismCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_PRISM_CREATION_ACTION_H
