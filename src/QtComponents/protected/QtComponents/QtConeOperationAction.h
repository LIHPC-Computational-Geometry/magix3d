/**
 * \file		QtConeOperationAction.h
 * \author		Charles PIGNEROL
 * \date		06/09/2013
 */
#ifndef QT_CONE_OPERATION_ACTION_H
#define QT_CONE_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DVectorPanel.h"
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
 * un cône.
 */
class QtConeOperationPanel : public QtMgx3DOperationPanel
{
	public :

	/** La méthode de création/modification du cône. */
	enum OPERATION_METHOD { RAYS_LENGTH };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Politique de création/modification du groupe
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtConeOperationPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtConeOperationPanel ( );

	/**
	 * \return		Le nom du groupe créé/modifié.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		La méthode de création/modification du cône.
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * \return		Les rayons de troncature.
	 */
	virtual double getRay1 ( ) const;
	virtual double getRay2 ( ) const;

	/**
	 * \return		Le vecteur pour l'axe et la longueur.
	 */
	virtual Mgx3D::Utils::Math::Vector getAxis ( ) const;

	/**
	 * \return		L'angle du cône.
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
	 * \return	Le nombre de bras suivant l'axe du cône.
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


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtConeOperationPanel (const QtConeOperationPanel&);
	QtConeOperationPanel& operator = (const QtConeOperationPanel&);

	/** Le nom du cône. */
	QtMgx3DGroupNamePanel*			_namePanel;

	/** La méthode de création/modification du cône. */
	QComboBox*						_operationMethodComboBox;

	/** Premier rayon (à l'origine) du cône. */
	QtDoubleTextField*				_ray1Panel;

	/** Second rayon (à l'origine) du cône. */
	QtDoubleTextField*				_ray2Panel;

	/** Vecteur axe/longueur. */
	QtMgx3DVectorPanel*				_vectorPanel;

	/** Angle du cône. */
	QtAnglePanel*			     	_anglePanel;

	/** L'éventuelle topologie associée. */
	QtTopologyPanel*				_topologyPanel;

};	// class QtConeOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtConeOperationPanel</I> de création/modification d'un cône.
 */
class QtConeOperationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtConeOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtConeOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtConeOperationAction ( );

	/**
	 * \return		Le panneau d'édition du cône associée.
	 */
	virtual QtConeOperationPanel* getConePanel ( );

	/**
	 * Créé/modifie le cône avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtConeOperationAction (const QtConeOperationAction&);
	QtConeOperationAction& operator = (
									const QtConeOperationAction&);
};  // class QtConeOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_CONE_OPERATION_ACTION_H
