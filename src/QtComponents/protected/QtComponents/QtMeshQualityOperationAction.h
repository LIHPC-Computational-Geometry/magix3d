/**
 * \file		QtMeshQualityOperationAction.h
 * \author		Charles PIGNEROL
 * \date		18/07/2013
 */
#ifndef QT_MESH_QUALITY_OPERATION_ACTION_H
#define QT_MESH_QUALITY_OPERATION_ACTION_H


#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DMeshOperationAction.h"
#include "QtComponents/QtMgx3DQualifWidget.h"

#include <TkUtil/Mutex.h>

#include <QPushButton>

#include <memory>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau de paramètrage d'une opération de qualité de maillage.
 */
class QtMeshQualityOperationPanel :
					public Mgx3D::QtComponents::QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 * \param	Fichier d'aide contextuelle associé à ce panneau.
	 * \param	Balise dans le fichier d'aide contextuelle associé à ce panneau
	 *			identifiant le paragraphe sur ce panneau.
	 */
	QtMeshQualityOperationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action,
			const std::string& helpURL, const std::string& helpTag);

	/**
	 * RAS.
	 */
	virtual ~QtMeshQualityOperationPanel ( );

	/**
	 * Ne fait pas QtMgx3DOperationPanel::setVisible qui est invoque cancel
	 * ou autoUpdate (API opérations). Ici on reste à l'aspect visible ou non
	 * de la fenêtre et de son contenu.
	 * Cette surcharge est due au fait que dans un système d'onglets, quand on
	 * affiche un onglet celui qui était au premier plan devient non visible ...
	 * => appel de cancel, clear, ...
	 */
	virtual void setVisible (bool visible);

	/**
	 * Si l'entité modifiée est une entité maillage, réinitialise le panneau et
	 * suspend tout affichage.
	 */
	virtual void commandModifiedCallback (Mgx3D::Internal::InfoCommand&);

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
	 * \return		Une référence sur le widget "qualité de maillage".
	 */
	virtual QtMgx3DQualifWidget& getQualityWidget ( );

	/**
	 * Supprime les éléments temporaires en cours exception faite des séries.
	 * \see			clearSeries
	 */
	virtual void clear ( );

	/**
	 * Supprime les séries en cours.
	 */
	virtual void clearSeries ( );


	protected slots :

	/**
	 * Affiche les mailles des classes sélectionnées.
	 */
	virtual void displayCellsCallback ( );

	/**
	 * Callback Qt. Réinitialise l'histogramme en fonction de la sélection en
	 * cours en appelant <I>autoUpdate</I>.
	 */
	virtual void reinitializeCallback ( );

	/**
	 * Ferme cette fenêtre.
	 */
	virtual void closeCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtMeshQualityOperationPanel (const QtMeshQualityOperationPanel&);
	QtMeshQualityOperationPanel& operator = (
										const QtMeshQualityOperationPanel&);

	QtMgx3DQualifWidget*					_qualifWidget;
	QPushButton*							_displayCellsButton;
	QPushButton*							_initializeButton;

	/** Les entités de maillage soumises à l'analyses qualitative. */
	std::vector<Mgx3D::Mesh::MeshEntity*>				_analysedMeshEntities;
	
	/** Les éléments de maillage temporaires, correspondant au découpage en
	 * classes d'histogrammes fait par Qualif. */
	std::vector<Mgx3D::Mesh::MeshEntity*>				_meshEntities;

	/** Les éléments de maillage <I>GMDS</I> temporaires. */
	std::vector<std::string>							_gmdsSurfaces;
	std::vector<std::string>							_gmdsVolumes;
};	// class QtMeshQualityOperationPanel


/**
 * Classe d'action associée à un panneau type <I>QtMeshQualityOperationPanel</I>
 * d'évaluation de qualité de maillage.
 */
class QtMeshQualityOperationAction :
					public Mgx3D::QtComponents::QtMgx3DMeshOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtMeshQualityOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte.
	 * \param		Tooltip décrivant l'action.
	 */
	QtMeshQualityOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMeshQualityOperationAction ( );

	/**
	 * \return		Le panneau d'édition de l'explorateur de maillage associé.
	 */
	virtual QtMeshQualityOperationPanel* getMeshQualityPanel ( );

	/**
	 * Effectue l'évaluation de la qualité du maillage avec le paramétrage de
	 * son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DMeshOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMeshQualityOperationAction (const QtMeshQualityOperationAction&);
	QtMeshQualityOperationAction& operator = (
									const QtMeshQualityOperationAction&);

	/** Les observateurs de l'exploration. */
	std::vector<TkUtil::ObjectBase*>		_observers;

	/** Un mutex non récursif pour empêcher toute réentrance.
	 * cf. executeOperation pour davantage d'explications. */
	TkUtil::Mutex							_mutex;
};  // class QtMeshQualityOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MESH_QUALITY_OPERATION_ACTION_H
