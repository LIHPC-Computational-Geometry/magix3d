/**
 * \file		QtTopologyCreationAction.h
 * \author		Charles PIGNEROL
 * \date		17/12/2013
 */
#ifndef QT_TOPOLOGY_CREATION_ACTION_H
#define QT_TOPOLOGY_CREATION_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/QtTopologyPanel.h"

#include "Utils/SelectionManagerIfc.h"

#include <QCheckBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * <P>Panneau d'édition des paramètres de création d'une entité topologique 
 * par association à une entité géométrique.</P>
 *
 * \warning		<B>Classe non instanciable, à dériver pour spécialisation.</P>
 */
class QtTopologyCreationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	protected :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 * \param	Dimension de l'entité topologique à créer.
	 * \param	Type de topologie proposée par défaut.
	 * \param	Type de système de coordonnées.
	 * \param   helpURL
	 * \param   helpTag
	 */
	QtTopologyCreationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action,
			Mgx3D::Utils::SelectionManagerIfc::DIM dimension,
			QtTopologyPanel::TOPOLOGY_TYPE defaultTopology,
			QtTopologyPanel::COORDINATES_TYPE defaultCoords,
			const std::string& helpURL,
			const std::string& helpTag);


	public :

	/**
	 * RAS.
	 */
	virtual ~QtTopologyCreationPanel ( );

	/**
	 * \return		L'entité géométrique associée à l'entité topologique à
	 * 				créer.
	 * \see			getGeomEntityDimension
	 * \see			setGeomEntityName
	 */
	virtual std::string getGeomEntityName ( ) const;

	/**
	 * \return		L'éventuel groupe associée à l'entité topologique à
	 * 				créer.
	 * \see			getGeomEntityName
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		Le type de topologie à créer.
	 */
	virtual QtTopologyPanel::TOPOLOGY_TYPE getTopologyType ( ) const;

	/**
	 * \return		Le ratio de l'éventuel O-grid.
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


	protected :

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * Invoque <I>preview (false, true).</I>
	 * \see		preview
	 */
	virtual void operationCompleted ( );

	/**
	 * \param		L'éventuelle entité géométrique à associer à l'entité
	 *				topologique à créer.
	 * \see			getGeomEntityName
	 */
	virtual void setGeomEntityName (const std::string& name);

	/**
 	 * \return		Une référence sur le panneau topologie.
 	 */
	virtual QtTopologyPanel& getTopologyPanel ( );
	virtual const QtTopologyPanel& getTopologyPanel ( ) const;


	protected slots :

	/**
	 * Appelé lorsque le type de topologie est modifié. Actualise l'IHM.
	 */
	virtual void topologyModifiedCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyCreationPanel (const QtTopologyCreationPanel&);
	QtTopologyCreationPanel& operator = (const QtTopologyCreationPanel&);

	/** L'entité géométrique à associer. */
	QtMgx3DEntityPanel*							_geomEntityPanel;

	/** La topologie créée. */
	QtTopologyPanel*							_topologyPanel;
};	// class QtTopologyCreationPanel


/**
 * <P>Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyCreationPanel</I> de création d'entité topologique associée
 * à une entité géométrique.</P>
 *
 * \warning		<B>Classe non instanciable, à dériver pour spécialiser.</B>
 */
class QtTopologyCreationAction : public QtMgx3DTopoOperationAction
{
	protected :

	/**
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \warning		<B>Ne créé pas le panneau associé, à faire dans les classes
	 * 				dérivées.</B>
	 */
	QtTopologyCreationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);


	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyCreationAction ( );

	/**
	 * \return		Le panneau d'édition des paramètres de création de l'entité
	 * 				topologique associé.
	 */
	virtual QtTopologyCreationPanel* getTopologyCreationPanel ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyCreationAction (const QtTopologyCreationAction&);
	QtTopologyCreationAction& operator = (const QtTopologyCreationAction&);
};  // class QtTopologyCreationAction


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_CREATION_ACTION_H
