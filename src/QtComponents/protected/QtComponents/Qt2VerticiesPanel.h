/**
 * \file		Qt2VerticiesPanel.h
 * \author		Charles PIGNEROL
 * \date		23/05/2014
 */
#ifndef QT_2_VERTICES_PANEL_H
#define QT_2_VERTICES_PANEL_H

#include "Internal/ContextIfc.h"

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
 * Panneau permettant de saisir les extrémités d'un segment, matérialisées
 * par 2 points/sommets.
 */
class Qt2VerticiesPanel : public QtMgx3DOperationsSubPanel
{
	Q_OBJECT

	public :

	/**
	 * \param		Widget parent
	 * \param		Le nom de l'application, pour les éventuels messages
	 *				d'erreur.
	 * \param		Fenêtre principale associée, pour la saisie des points/sommets.
	 * \param		Types d'entités acceptées pour la saisie des points/sommets.
	 */
	Qt2VerticiesPanel (QWidget* parent, const std::string& appTitle,
					Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
					Mgx3D::Utils::FilterEntity::objectType types);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~Qt2VerticiesPanel ( );

    /**
     * Méthode appelée lorsque l'utilisateur suspend l'édition de
     * l'opération.
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
	 * Les coordonnées des 2 vertex.
	 */
	virtual std::string getVertex1UniqueName ( ) const;
	virtual std::string getVertex2UniqueName ( ) const;

	/**
	 * \param	En retour, les points définissant l'arête.
	 * \except	Une exception est levée en cas d'absence d'arête.
	 */
	virtual void getPoints (Utils::Math::Point&, Utils::Math::Point&) const;

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Actualisation de la représentation de la sélection.
	 * \return	<I>true</I> en cas de modification, <I>false</I> dans le cas
	 * 			contraire.
	 */
	virtual bool actualizeGui (bool removeDestroyed);


	signals :

	/**
	 * Signal émis lorsqu'un point est ajoutée à la sélection.
	 * \param	Nom du point ajouté.
	 * \see	pointRemovedFromSelection
	 */
	void pointAddedToSelection (QString);

	/**
	 * Signal émis lorsqu'un point est enlevé de la sélection.
	 * \param	Nom du point enlevé.
	 * \see	pointRemovedFromSelection
	 */
	void pointRemovedFromSelection (QString);


	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */	
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return	Une référence sur le contexte <I>Magix 3D</I> associé à
	 *			l'opération.
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
	virtual void pointsAddedToSelectionCallback (QString pointNames);

	/**
	 * Invoqué lorsqu'un/plusieurs point est enleveé de la sélection.
	 * Emet le signal pointRemovedFromSelection.
	 */
	virtual void pointRemovedFromSelectionCallback (QString pointName);
	virtual void pointsRemovedFromSelectionCallback (QString pointNames);


	private :

	/**
	 * Constructeur de copie, opérateur =. Interdits.
	 */
	Qt2VerticiesPanel (const Qt2VerticiesPanel&);
	Qt2VerticiesPanel& operator = (const Qt2VerticiesPanel&);

	/** Le contexte <I>Magix 3D</I> de l'opération. */
	Mgx3D::QtComponents::QtMgx3DMainWindow*			_mainWindow;

	/** Les 2 vertex. */
	QtMgx3DEntityPanel				*_vertex1Panel, *_vertex2Panel;
};	// class Qt2VerticiesPanel


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_2_VERTICES_PANEL_H
