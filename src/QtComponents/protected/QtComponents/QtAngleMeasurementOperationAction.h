/**
 * \file		QtAngleMeasurementOperationAction.h
 * \author		Eric Brière de l'Isle
 * \date		10/03/2017
 */
#ifndef QT_ANGLE_MEASUREMENT_OPERATION_ACTION_H
#define QT_ANGLE_MEASUREMENT_OPERATION_ACTION_H

#include "Internal/ContextIfc.h"

#include "Utils/Entity.h"
#include "Geom/GeomEntity.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DEntityPanel.h"

#include <TkUtil/Mutex.h>

#include <QLabel>

#include <memory>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau de mesure de l'angle entre 3 points.
 */
class QtAngleMeasurementOperationPanel :
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
	QtAngleMeasurementOperationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action,
			const std::string& helpURL, const std::string& helpTag);

	/**
	 * RAS.
	 */
	virtual ~QtAngleMeasurementOperationPanel ( );

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

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
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Si l'entité modifiée est une entité participant à l'angle mesuré
	 * alors effectue à nouveau la mesure.
	 */
	virtual void commandModifiedCallback (Mgx3D::Internal::InfoCommand&);

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
	 * \return		Les entités.
	 */
	virtual const Mgx3D::Utils::Entity* getEntityCenter ( ) const;
	virtual const Mgx3D::Utils::Entity* getEntity1 ( ) const;
	virtual const Mgx3D::Utils::Entity* getEntity2 ( ) const;

	/**
 	 * Actualise le champ "angle".
 	 */
	virtual void updateAngle (double d);


	protected slots :

	/**
	 * Callback Qt. Effectue la mesure.
	 */
	virtual void selectionModifiedCallback ( );

	/**
	 * Ferme cette fenêtre.
	 */
	virtual void closeCallback ( );

	/**
	 * Affiche l'aide associée au panneau.
	 */
	virtual void helpCallback ( );

	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtAngleMeasurementOperationPanel (
								const QtAngleMeasurementOperationPanel&);
	QtAngleMeasurementOperationPanel& operator = (
								const QtAngleMeasurementOperationPanel&);

	/** Les entités dont on évalue l'angle. */
	QtMgx3DEntityPanel*		_entityCenterPanel;
	QtMgx3DEntityPanel*		_entity1Panel;
	QtMgx3DEntityPanel*		_entity2Panel;

	/** L'angle entre les entités. */
	QLabel*									_angleLabel;

	/** Le nom du fichier contenant l'aide contextuelle associée à ce panneau
	 * (chemin relatif par rapport à la racine de l'aide contextuelle du
	 * logiciel).
	 */
	std::string										_helpURL;

	/** La balise identifiant le paragraphe d'aide contextuelle associée à ce
	 * panneau dans le fichier d'aide contextuelle référencé par <I>_helpURL</I>
	 * .*/
	std::string										_helpTag;

};	// class QtAngleMeasurementOperationPanel


/**
 * Classe d'action associée à un panneau type
 * <I>QtAngleMeasurementOperationPanel</I> de calcul d'angle entre 3
 * entités géométriques.
 */
class QtAngleMeasurementOperationAction :
					public Mgx3D::QtComponents::QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtAngleMeasurementOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte.
	 * \param		Tooltip décrivant l'action.
	 */
	QtAngleMeasurementOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtAngleMeasurementOperationAction ( );

	/**
	 * \return		Le panneau de mesure de l'angle associé.
	 */
	virtual QtAngleMeasurementOperationPanel* getAngleMeasurementPanel ( );

	/**
	 * Effectue le calcul de l'angle entre les entités sélectionnées.
	 * Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtAngleMeasurementOperationAction (
								const QtAngleMeasurementOperationAction&);
	QtAngleMeasurementOperationAction& operator = (
								const QtAngleMeasurementOperationAction&);

	/** Un mutex non récursif pour empêcher toute réentrance.
	 * cf. executeOperation pour davantage d'explications. */
	TkUtil::Mutex							_mutex;
};  // class QtAngleMeasurementOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_ANGLE_MEASUREMENT_OPERATION_ACTION_H
