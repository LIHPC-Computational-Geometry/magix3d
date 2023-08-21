/**
 * \file		QtExtremaMeshingEdgeLengthOnEdgeAction.h
 * \author		Eric BRIERE DE L'ISLE
 * \date		05/12/2016
 */
#ifndef QT_EXTREMA_MESHING_EDGE_LENGTH_OPERATION_ACTION_H
#define QT_EXTREMA_MESHING_EDGE_LENGTH_OPERATION_ACTION_H

#include "Internal/ContextIfc.h"

#include "Utils/Entity.h"
#include "Geom/GeomEntity.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtEntityByDimensionSelectorPanel.h"

#include <TkUtil/Mutex.h>

#include <QLabel>

#include <memory>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau de mesure de distance entre 2 entités géométriques.
 */
class QtExtremaMeshingEdgeLengthOnEdgePanel :
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
	QtExtremaMeshingEdgeLengthOnEdgePanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action,
			const std::string& helpURL, const std::string& helpTag);

	/**
	 * RAS.
	 */
	virtual ~QtExtremaMeshingEdgeLengthOnEdgePanel ( );

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
	 * Si l'arête est modifiée alors effectue à nouveau la mesure.
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
	 * \return		Les entités bornant les extrémités.
	 */
	virtual const Mgx3D::Topo::CoEdge* getCoEdge ( ) const;

	/**
 	 * Actualise les champs "longueurs".
 	 */
	virtual void updateLengths (double d1, double d2);

	/**
	 * Affiche le sens de la discrétisation de l'arête
	 */
	virtual void viewDirection (bool show, bool destroyInteractor);

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
	QtExtremaMeshingEdgeLengthOnEdgePanel (
								const QtExtremaMeshingEdgeLengthOnEdgePanel&);
	QtExtremaMeshingEdgeLengthOnEdgePanel& operator = (
								const QtExtremaMeshingEdgeLengthOnEdgePanel&);

	/** L'arête dont on cherche les longueurs de bras aux extrémités. */
	QtMgx3DEntityPanel*		                _coedgePanel;

	/** La longueur du bras au départ */
	QLabel*									_startLengthLabel;

	/** La longueur du bras à l'arrivée */
	QLabel*									_endLengthLabel;

	/** Le nom du fichier contenant l'aide contextuelle associée à ce panneau
	 * (chemin relatif par rapport à la racine de l'aide contextuelle du
	 * logiciel).
	 */
	std::string										_helpURL;

	/** La balise identifiant le paragraphe d'aide contextuelle associée à ce
	 * panneau dans le fichier d'aide contextuelle référencé par <I>_helpURL</I>
	 * .*/
	std::string										_helpTag;

};	// class QtExtremaMeshingEdgeLengthOnEdgePanel


/**
 * Classe d'action associée à un panneau type
 * <I>QtExtremaMeshingEdgeLengthOnEdgePanel</I> de calcul de longueur des bras aux extrémités d'une arête
 */
class QtExtremaMeshingEdgeLengthOnEdgeAction :
					public Mgx3D::QtComponents::QtMgx3DOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtExtremaMeshingEdgeLengthOnEdgePanel</I>.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte.
	 * \param		Tooltip décrivant l'action.
	 */
	QtExtremaMeshingEdgeLengthOnEdgeAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtExtremaMeshingEdgeLengthOnEdgeAction ( );

	/**
	 * \return		Le panneau de mesure des longueurs associées.
	 */
	virtual QtExtremaMeshingEdgeLengthOnEdgePanel* getExtremaMeshingEdgeLengthPanel ( );

	/**
	 * Effectue le calcul de la distance entre les entités sélectionnées.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtExtremaMeshingEdgeLengthOnEdgeAction (
								const QtExtremaMeshingEdgeLengthOnEdgeAction&);
	QtExtremaMeshingEdgeLengthOnEdgeAction& operator = (
								const QtExtremaMeshingEdgeLengthOnEdgeAction&);

	/** Un mutex non récursif pour empêcher toute réentrance.
	 * cf. executeOperation pour davantage d'explications. */
	TkUtil::Mutex							_mutex;
};  // class QtExtremaMeshingEdgeLengthOnEdgeAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_EXTREMA_MESHING_EDGE_LENGTH_OPERATION_ACTION_H
