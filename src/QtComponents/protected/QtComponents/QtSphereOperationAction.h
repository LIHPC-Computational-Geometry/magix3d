/**
 * \file		QtSphereOperationAction.h
 * \author		Charles PIGNEROL
 * \date		27/05/2014
 */
#ifndef QT_SPHERE_OPERATION_ACTION_H
#define QT_SPHERE_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtAnglePanel.h"
#include "QtComponents/QtMgx3DPointPanel.h"
#include "QtComponents/QtTopologyPanel.h"

#include <QtUtil/QtDoubleTextField.h>

#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération géométrique touchant
 * une sphère.
 */
class QtSphereOperationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/** La méthode de création/modification de la sphère. */
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
	QtSphereOperationPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtSphereOperationPanel ( );

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return		Le nom du groupe créé/modifié.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		<I>true</I> si la sphère est creuse, <I>false</I> si elle
	 *				est pleine.
	 */
	virtual bool isHollowed ( ) const;

	/**
	 * \return		La méthode de création/modification de la sphère.
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * Les coordonnées du centre de la sphère.
	 */
	virtual Utils::Math::Point getCenter() const;

	/**
	 * Les rayons de la sphère.
	 */
	virtual double getExternalRadius ( ) const;
	virtual double getInternalRadius ( ) const;

	/**
	 * La portion de sphère (0 si libre => utiliser getAngle ( )).
	 * \see		getAngle
	 */
	virtual Mgx3D::Utils::Portion::Type getPortion ( ) const;

	/**
	 * La portion de sphère.
	 * \see		getPortion
	 */
	virtual double getAngle ( ) const;

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
	 * \see		getOGridRatio
	 */
	virtual int getNj ( ) const;
	virtual int getNk ( ) const;

	/**
	 * \return	Le ratio de l'éventuel O-grid.
	 * \see		createTopology
	 * \see		getNj
	 * \see		getNk
	 */
	virtual double getOGridRatio ( ) const;

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


	protected slots :

	/**
	 * Appelé lorsque le caractère creux/plein est modifié. Actualise l'écran.
	 */
	virtual void hollowedModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtSphereOperationPanel (const QtSphereOperationPanel&);
	QtSphereOperationPanel& operator = (const QtSphereOperationPanel&);

	/** Le nom de la sphère. */
	QtMgx3DGroupNamePanel*			_namePanel;

	/** La sphère est elle creuse ou pleine ? */
	QCheckBox*						_hollowedCheckBox;

	/** La méthode de création/modification de la sphère. */
	QComboBox*						_operationMethodComboBox;

	/** Coordonnées du centre de la sphère. */
	QtMgx3DPointPanel*				_centerPanel;

	/** Rayons de la sphère. */
	QtDoubleTextField*				_externalRadiusTextField;
	QtDoubleTextField*				_internalRadiusTextField;
	QLabel*							_internalRadiusLabel;

	/** L'angle de la sphère (cas d'une portion). */
	QtAnglePanel*					_anglePanel;

	/** L'éventuelle topologie associée. */
	QtTopologyPanel*				_topologyPanel;
};	// class QtSphereOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtSphereOperationPanel</I> de création/modification d'une sphère.
 */
class QtSphereOperationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtSphereOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtSphereOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSphereOperationAction ( );

	/**
	 * \return		Le panneau d'édition de la sphère associée.
	 */
	virtual QtSphereOperationPanel* getSpherePanel ( );

	/**
	 * Créé/modifie la sphère avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtSphereOperationAction (const QtSphereOperationAction&);
	QtSphereOperationAction& operator = (
									const QtSphereOperationAction&);
};  // class QtSphereOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_SPHERE_OPERATION_ACTION_H
