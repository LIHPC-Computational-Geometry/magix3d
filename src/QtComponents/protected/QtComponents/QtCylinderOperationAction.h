/**
 * \file		QtCylinderOperationAction.h
 * \author		Charles PIGNEROL
 * \date		11/12/2012
 */
#ifndef QT_CYLINDER_OPERATION_ACTION_H
#define QT_CYLINDER_OPERATION_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtAnglePanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DPointPanel.h"
#include "QtComponents/QtTopologyPanel.h"
#include "QtComponents/QtMgx3DVectorPanel.h"

//#include <QtUtil/Qt3DDataPanel.h>
#include <QtUtil/QtDoubleTextField.h>
#include <QtUtil/QtIntTextField.h>

#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération géométrique touchant
 * un cylindre.
 */
class QtCylinderOperationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/** La méthode de création/modification du cylindre. */
	enum OPERATION_METHOD {CENTER_RADIUS_VECTOR_ANGLE };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Politique de création/modification du groupe
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtCylinderOperationPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtCylinderOperationPanel ( );

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return		Le nom du groupe créé/modifié.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		<I>true</I> si le cylindre est creux, <I>false</I> si il
	 *				est plein.
	 */
	virtual bool isHollowed ( ) const;

	/**
	 * \return		La méthode de création/modification du cylindre.
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * \return		Les coordonnées du centre de la base du cylindre.
	 */
	virtual Utils::Math::Point getCenter() const;

	/**
	 * \return		Les rayons du cylindre.
	 */
	virtual double getExternalRadius ( ) const;
	virtual double getInternalRadius ( ) const;

	/**
	 * \return		Le vecteur pour l'axe et la longueur.
	 */
	virtual Mgx3D::Utils::Math::Vector getAxis ( ) const;

	/**
	 * \return		La portion de cylindre.
	 */
	virtual double getAngle ( ) const;

	/**
	 * \return		<I>true</I> s'il faut créer une topologie avec, <I>false</I>
	 *				dans le cas contraire.
	 */
	virtual bool createTopology ( ) const;

	/**
	 * \return		Le type de topologie à créer.
	 * \see			createTopology
	 */
	virtual QtTopologyPanel::TOPOLOGY_TYPE getTopologyType ( ) const;

	/**
	 * \return		Le ratio de l'éventuel O-grid.
	 * see			createTopology
	 */
	virtual double getOGridRatio ( ) const;

	/**
	 * \return	Le nombre de bras suivant l'axe du cylindre.
	 */
	virtual int getAxeEdgesNum ( ) const;

	/**
	 * \return	Le nombre de bras entre le point triple et le plan de
	 *			symétrie.
	 */
	virtual int getiEdgesNum ( ) const;

	/**
	 * \return	Le nombre de couches entre le bloc central et la surface
	 *			externe.
	 */
	virtual int getrLayersNum ( ) const;

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
	QtCylinderOperationPanel (const QtCylinderOperationPanel&);
	QtCylinderOperationPanel& operator = (const QtCylinderOperationPanel&);

	/** Le nom du cylindre. */
	QtMgx3DGroupNamePanel*			_namePanel;

	/** Le cylindre est il creux ou plein ? */
	QCheckBox*						_hollowedCheckBox;

	/** La méthode de création/modification du cylindre. */
	QComboBox*						_operationMethodComboBox;

	/** Coordonnées du centre de la base du cylindre. */
	QtMgx3DPointPanel*				_coordinatesPanel;

	/** Rayons du cylindre. */
	QtDoubleTextField*				_externalRadiusTextField;
	QtDoubleTextField*				_internalRadiusTextField;
	QLabel							*_internalRadiusLabel;

	/** Vecteur définissant le cylindre. */
	QtMgx3DVectorPanel*				_vectorPanel;

	/** La portion de cylindre à créer/modifier. */
	QtAnglePanel*					_anglePanel;

	/** L'éventuelle topologie associée. */
	QtTopologyPanel*				_topologyPanel;

};	// class QtCylinderOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtCylinderOperationPanel</I> de création/modification d'un cylindre.
 */
class QtCylinderOperationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtCylinderOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtCylinderOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtCylinderOperationAction ( );

	/**
	 * \return		Le panneau d'édition du cylindre associé.
	 */
	virtual QtCylinderOperationPanel* getCylinderPanel ( );

	/**
	 * Créé/modifie le cylindre avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtCylinderOperationAction (const QtCylinderOperationAction&);
	QtCylinderOperationAction& operator = (
									const QtCylinderOperationAction&);
};  // class QtCylinderOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_CYLINDER_OPERATION_ACTION_H
