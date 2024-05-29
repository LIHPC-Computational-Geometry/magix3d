/**
 * \file		Qt3VerticiesPanel.h
 * \author		Charles PIGNEROL
 * \date		08/09/2014
 */
#ifndef QT_3_VERTICES_PANEL_H
#define QT_3_VERTICES_PANEL_H


#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DEntityPanel.h"
#include "Internal/Context.h"

#include <QtUtil/Qt3DDataPanel.h>
#include <QtUtil/QtDoubleTextField.h>

#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau permettant de saisir les extrémités d'un arc de cercle, matérialisées
 * par 2 points/sommets, et un 3ème (centre de l'arc de cercle).
 * Ou simplement par 3 points (pour un cercle)
 */
class Qt3VerticiesPanel : public QtMgx3DOperationsSubPanel
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Le nom de l'application, pour les éventuels messages
	 *				d'erreur.
	 * \param		Fenêtre principale associée, pour la saisie des points/sommets.
	 * \param		Types d'entités acceptées pour la saisie des points/sommets.
	 * \param       vrai pour le cas avec 2 extrémités et un centre
	 */
	Qt3VerticiesPanel (QWidget* parent, const std::string& appTitle,
					Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
					Mgx3D::Utils::FilterEntity::objectType types,
					bool extremities);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~Qt3VerticiesPanel ( );

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
	 * Les coordonnées des 3 vertex (départ/fin/centre).
	 */
	virtual std::string getStartVertexUniqueName ( ) const;
	virtual std::string getEndVertexUniqueName ( ) const;
	virtual std::string getCenterVertexUniqueName ( ) const;

	/**
	 * \param	En retour, les points définissant l'arête (départ/fin/centre).
	 * \except	Une exception est levée en cas d'absence d'arête.
	 */
	virtual void getPoints (Utils::Math::Point&, Utils::Math::Point&, Utils::Math::Point&) const;

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );


	signals :

	/**
	 * Signal émis lorsqu'un point est ajoutée à la sélection.
	 * \param	Nom du point ajouté.
	 * \see		pointRemovedFromSelection
	 */
	void pointAddedToSelection (QString);

	/**
	 * Signal émis lorsqu'un point est enlev de la sélection.
	 * \param	Nom du point enlevé.
	 * \see		pointAddedToSelection
	 */
	void pointRemovedFromSelection (QString);


	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */	
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return	Une référence sur le contexte <I>Magix 3D</I> associé à l'opération.
	 */
	virtual const Mgx3D::Internal::ContextIfc& getContext ( ) const;

	/** \return le point pour un sommet topo ou géom */
	Utils::Math::Point getPoint (const std::string& name) const;

	protected slots :

	/**
	 * Invoqué lorsqu'un/plusieurs point est ajouté à la sélection.
	 * Emet le signal pointAddedToSelection.
	 */
	virtual void pointAddedToSelectionCallback (QString pointName);
	virtual void pointsAddedToSelectionCallback (QString pointName);

	/**
	 * Invoqué lorsqu'un/plusieurs point est enlevé de la sélection.
	 * Emet le signal pointRemovedFromSelection.
	 */
	virtual void pointRemovedFromSelectionCallback (QString pointName);
	virtual void pointsRemovedFromSelectionCallback (QString pointName);


	private :

	/**
	 * Constructeur de copie, opérateur =. Interdits.
	 */
	Qt3VerticiesPanel (const Qt3VerticiesPanel&);
	Qt3VerticiesPanel& operator = (const Qt3VerticiesPanel&);

	/** Le contexte <I>Magix 3D</I> de l'opération. */
	Mgx3D::QtComponents::QtMgx3DMainWindow*			_mainWindow;

	/** Les 3 vertex. */
	QtMgx3DEntityPanel				                *_vertex1Panel, *_vertex2Panel, *_vertex3Panel;

//	/** Contexte d'utilisation du panneau. */
//	Mgx3D::Internal::ContextIfc*	_context;
};	// class Qt3VerticiesPanel


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_3_VERTICES_PANEL_H
