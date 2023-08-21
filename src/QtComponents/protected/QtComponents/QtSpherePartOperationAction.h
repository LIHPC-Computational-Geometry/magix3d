/**
 * \file		QtSpherePartOperationAction.h
 * \author		Eric
 * \date		30/3/2016
 *
 * à partir de QtSphereOperationAction fait par Charles
 */
#ifndef QT_SPHERE_PART_OPERATION_ACTION_H
#define QT_SPHERE_PART_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtAnglePanel.h"
#include "QtComponents/QtTopologyPanel.h"

#include <QtUtil/QtDoubleTextField.h>

#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération géométrique touchant
 * une aiguille (portion de sphère).
 */
class QtSpherePartOperationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/** La méthode de création/modification de l'aiguille. */
	enum OPERATION_METHOD {RAYON };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Politique de création/modification du groupe
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtSpherePartOperationPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtSpherePartOperationPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return		Le nom du groupe créé/modifié.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		<I>true</I> si l'aiguille est creuse, <I>false</I> si elle
	 *				est pleine.
	 */
	virtual bool isHollowed ( ) const;

	/**
	 * \return		La méthode de création/modification de l'aiguille.
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * Les rayons de l'aiguille.
	 */
	virtual double getExternalRadius ( ) const;
	virtual double getInternalRadius ( ) const;

	/**
	 * L'angle d'ouverture autour de Ox dans le plan Oxy
	 */
	virtual double getAngleY ( ) const;

	/**
	 * L'angle d'ouverture autour de Ox dans le plan Oxz
	 */
	virtual double getAngleZ ( ) const;

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
	 * \return	Le nombre de bras suivant l'axe des
	 *			ordonnées, l'axe des élévations et l'axe des abscisses
	 */
	virtual int getNi ( ) const;
	virtual int getNj ( ) const;
	virtual int getNk ( ) const;

    /**
     * Méthode appelée pour vérifier les paramètres saisis par l'utilisateur.
     * En cas de paramètrage invalide cette méthode doit leve une exception de
     * type <I>TkUtil::Exception</I> contenant un descriptif des erreurs
     * rencontrées.
     */
    virtual void validate ( );

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
	QtSpherePartOperationPanel (const QtSpherePartOperationPanel&);
	QtSpherePartOperationPanel& operator = (const QtSpherePartOperationPanel&);

	/** Le nom de l'aiguille. */
	QtMgx3DGroupNamePanel*			_namePanel;

	/** L'aiguille est elle creuse ou pleine ? */
	QCheckBox*						_hollowedCheckBox;

	/** La méthode de création/modification de l'aiguille. */
	QComboBox*						_operationMethodComboBox;

	/** Rayons de l'aiguille. */
	QtDoubleTextField*				_externalRadiusTextField;
	QtDoubleTextField*				_internalRadiusTextField;
	QLabel*							_internalRadiusLabel;

	/** L'angle de l'aiguille suivant Y. */
	QtAnglePanel*					_angleYPanel;
	/** L'angle de l'aiguille suivant Z. */
	QtAnglePanel*					_angleZPanel;

	/** L'éventuelle topologie associée. */
	QtTopologyPanel*				_topologyPanel;
};	// class QtSpherePartOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtSpherePartOperationPanel</I> de création/modification d'une sphère.
 */
class QtSpherePartOperationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtSpherePartOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtSpherePartOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSpherePartOperationAction ( );

	/**
	 * \return		Le panneau d'édition de l'aiguille associée.
	 */
	virtual QtSpherePartOperationPanel* getSpherePartPanel ( );

	/**
	 * Créé/modifie l'aiguille avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtSpherePartOperationAction (const QtSpherePartOperationAction&);
	QtSpherePartOperationAction& operator = (
									const QtSpherePartOperationAction&);
};  // class QtSpherePartOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_SPHERE_PART_OPERATION_ACTION_H
