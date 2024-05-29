/**
 * \file		QtGeometryRotationAction.h
 * \author		Charles PIGNEROL
 * \date		22/05/2014
 */
#ifndef QT_GEOMETRY_ROTATION_ACTION_H
#define QT_GEOMETRY_ROTATION_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DRotationPanel.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"

#include "Utils/SelectionManagerIfc.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

class QtMgx3DGroupNamePanel;

/**
 * Panneau d'édition des paramètres de rotation d'entités géométriques.
 */
class QtGeometryRotationPanel : public QtMgx3DOperationPanel
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
	QtGeometryRotationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGeometryRotationPanel ( );

	/**
	 * \param		Nouvelle dimension de saisie des entités géométriques.
	 */
	virtual void setDimension (Mgx3D::Utils::SelectionManagerIfc::DIM dim);

	/**
	 * \return		<I>true</I> s'il faut soumettre toutes les entités
	 * 				géométriques à la rotation, <I>false</I> s'il faut juste
	 * 				traiter la sélection désignée par l'utilisateur.
	 * \see			getInvolvedEntities
	 */
	virtual bool processAllGeomEntities ( ) const;

	/**
	 * \return		<I>true</I> s'il faut copier les entités avant la transformation
	 */
	virtual bool copyEntities ( ) const;

	/**
	 * @return		Le nom du groupe créé en cas de copie.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		<I>true</I> s'il faut également copier les entités topologiques
	 */
	virtual bool copyTopoEntities ( ) const;

	/**
	 * \return		La liste des entités géométriques devant effectuer une
	 *				rotation.
	 * \see			getRotation
	 * \see			doPropagate
	 * \see			processAllGeomEntities
	 */
	virtual std::vector<std::string> getGeomEntitiesNames ( ) const;

	/**
	 * \return		La rotation à effectuer.
	 * \see			getGeomEntitiesNames
	 */
	virtual Utils::Math::Rotation getRotation ( ) const;

	/**
	 * \return		<I>true</I> s'il faut également faire effectuer la rotation
	 *				aux entités topologiques associées.
	 * \see			getGeomEntitiesNames
	 */
	virtual bool doPropagate ( ) const;

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
	 * Appelé lorsque la case à cocher "toutes les entités" change d'état.
	 * (In)active la zone de saisie des entités soumises à la transformation.
	 */
	virtual void allEntitiesCallback ( );

	/**
	 * Appelé lorsque l'on active ou non la copie avant transformation.
	 * (In)active la zone de sélection du nom d'un groupe ainsi que
	 * l'option de copie de la topologie associée.
	 */
	virtual void copyCallback();

	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtGeometryRotationPanel (const QtGeometryRotationPanel&);
	QtGeometryRotationPanel& operator = (
										const QtGeometryRotationPanel&);

	/** Les dimensions possibles pour les entités géométriques suivant la
	 * rotation. */
	QtEntityByDimensionSelectorPanel*			_geomEntitiesPanel;

	/** Case à cocher <I>Toutes les entités</I>. */
	QCheckBox*									_allEntitiesCheckBox;

	/** Case à cocher <I>propager</I>. */
	QCheckBox*									_propagateCheckBox;

	/** Le panneau décrivant la rotation. */
	QtMgx3DRotationPanel*						_rotationPanel;

	/** Case à cocher copier. */
	QCheckBox*									_copyCheckBox;

	/** Le nom du groupe créé (en cas de copie). */
	QtMgx3DGroupNamePanel*						_namePanel;

	/** Case à cocher copier avec la topologie</I>. */
	QCheckBox*									_withTopologyCheckBox;

};	// class QtGeometryRotationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeometryRotationPanel</I> de rotation d'entités géométriques.
 */
class QtGeometryRotationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeometryRotationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtGeometryRotationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtGeometryRotationAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de rotation d'entités
	 *				géométriques.
	 */
	virtual QtGeometryRotationPanel* getRotationPanel ( );

	/**
	 * Actualise le paramétrage des entités géométriques avec le paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGeometryRotationAction (const QtGeometryRotationAction&);
	QtGeometryRotationAction& operator = (
									const QtGeometryRotationAction&);
};  // class QtGeometryRotationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_GEOMETRY_ROTATION_ACTION_H
