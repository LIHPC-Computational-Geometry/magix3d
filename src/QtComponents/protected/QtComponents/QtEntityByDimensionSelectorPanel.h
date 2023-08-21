/**
 * \file		QtEntityByDimensionSelectorPanel.h
 * \author		Charles PIGNEROL
 * \date		05/12/2013
 */

#ifndef QT_ENTITY_BY_DIMENSION_SELECTOR_PANEL_H
#define QT_ENTITY_BY_DIMENSION_SELECTOR_PANEL_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtDimensionsSelectorPanel.h"
#include "QtComponents/QtMgx3DEntityPanel.h"


namespace Mgx3D
{

namespace QtComponents
{

/**
 * Panneau sélecteur interactif d'entités permettant de filtrer les entités
 * selon leur dimension.
 *
 * \see		QtDimensionsSelectorPanel
 * \see		QtMgx3DEntityPanel
 */
class QtEntityByDimensionSelectorPanel : public QWidget
{
	Q_OBJECT

	public :

	/**
	 * \param	Widget parent.
	 * \param	Fenêtre principale associée, pour la saisie interactive des
	 * 			entités.
	 * \param	Libellé du champ de texte.
	 * \param	Dimensions sélectionnables
	 * \param	Types d'"entités" sélectionnables
	 * \param	Dimensions proposées par défaut
	 * \param	<I>true</I> si la sélection d'entités de dimensions différentes
	 *			est autorisée, <I>false> dans le cas contraire.
	 */
	QtEntityByDimensionSelectorPanel (
				QWidget* parent,
				Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
				const std::string& label,
				Mgx3D::Utils::SelectionManagerIfc::DIM allowedDimensions,
				Mgx3D::Utils::FilterEntity::objectType types,
				Mgx3D::Utils::SelectionManagerIfc::DIM defaultDimensions,
				bool allowMultipleDimensions);

	/**
	 * RAS.
	 */
	virtual ~QtEntityByDimensionSelectorPanel ( );

	/**
	 * \return		La liste des noms d'entités sélectionnées (nom unique si
	 *				<I>isMultiSelectMode</I> retourne <I>false</I>.
	 * \see			setEntitiesNames
	 */
	virtual std::vector<std::string> getEntitiesNames ( ) const;

	/**
	 * \param		La nouvelle liste des noms d'entités sélectionnées (nom unique si
	 *				<I>isMultiSelectMode</I> retourne <I>false</I>.
	 * \see			getEntitiesNames
	 */
	virtual void setEntitiesNames (const std::vector<std::string>&);

	/**
	 * \return		Le panneau de saisie interactive des noms d'entités.
	 */
	virtual Mgx3D::QtComponents::QtMgx3DEntityPanel* getEntitiesPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	 virtual void reset ( );

	/**
	 * Annule la sélection en cours.
	 */
	virtual void clearSelection ( );

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );

	/**
	 * Actualisation de la représentation de la sélection.
	 * \return	<I>true</I> en cas de modification, <I>false</I> dans le cas
	 * 			contraire.
	 */
	virtual bool actualizeGui (bool removeDestroyed);

	/**
	 * \return		Les dimensions sélectionnées par l'utilisateur.
	 * \see			setDimensions
	 */
	virtual Mgx3D::Utils::SelectionManagerIfc::DIM getDimensions ( ) const;

	/**
	 * \param		Nouvelles dimensions.
	 * \see			getDimensions
	 */
	virtual void setDimensions (Mgx3D::Utils::SelectionManagerIfc::DIM dims);

	/**
	 * \return		Les dimensions autorisées.
	 * \see			setAllowedDimensions
	 * \see			getAllowedTypes
	 */
	virtual Mgx3D::Utils::SelectionManagerIfc::DIM getAllowedDimensions ( ) const;

	/**
	 * \param		Les dimensions à autoriser.
	 * \see			getAllowedDimensions
	 * \see			setFilteredDimensions
	 */
	virtual void setAllowedDimensions (Mgx3D::Utils::SelectionManagerIfc::DIM);

	/**
	 * \return		Les types autorisés.
	 * \see			getAllowedDimensions
	 */
	virtual Mgx3D::Utils::FilterEntity::objectType getAllowedTypes ( ) const;

	/**
	 * \return		<I>true</I> si le panneau permet la saisie multiple,
	 *				<I>false</I> dans le cas contraire.
	 * \see			setMultiSelectMode
	 */
	virtual bool isMultiSelectMode ( ) const;

	/**
	 * \param		<I>true</I> si le panneau doit permettre la saisie
	 * 				multiple, <I>false</I> dans le cas contraire (saisie
	 * 				d'un seul nom).
	 * \see			isMultiSelectMode
	 */
	virtual void setMultiSelectMode (bool multi);


	signals :

	/**
	 * Signal émis lorsque les dimensions sont modifiées par l'utilisateur.
	 */
	 void dimensionsModified ( );

	/**
	 * Signal émis lorsque la sélection est modifiée par l'utilisateur.
	 * \param		Chaine décrivant la nouvelle sélection.
	 */
	 void selectionModified (QString);

	/**
	 * Signal émis lorsque des entités sont ajoutées à la sélection.
	 * \param		Nom des entités ajoutées.
	 * \see			entitiesRemovedFromSelection
	 * \see			selectionModified
	 */
	void entitiesAddedToSelection (QString);

	/**
	 * Signal émis lorsque des entités sont enlevées de la sélection.
	 * \param		Nom des entités enlevées.
	 * \see			entitiesAddedToSelection
	 * \see			selectionModified
	 */
	void entitiesRemovedFromSelection (QString);


	protected slots :

	/**
	 * Callback sur la modification de la dimension. Actualise le panneau et
	 * emet le signal <I>dimensionsModified</I>.
	 */
	virtual void dimensionsModifiedCallback ( );

	/**
	 * Callback sur la modification de la sélection. Emet le signal
	 * <I>selectionModified</I>.
	 */
	virtual void selectionModifiedCallback (QString);

	/**
	 * Appelé lorsque l'utilisateur ajoute des entités à la sélection.
	 * \param		Entités ajoutées à la sélection
	 * \see			removeFromSelection
	 * \see			setSelectedEntity
	 * \see			setSelectedEntities
	 */
	virtual void entitiesAddedToSelectionCallback (QString entitiesNames);

	/**
	 * Appelé lorsque l'utilisateur enlève des entités de la sélection.
	 * \param		Entités enlevées de la sélection
	 * \see			addToSelection
	 * \see			setSelectedEntity
	 * \see			setSelectedEntities
	 */
	virtual void entitiesRemovedFromSelectionCallback (QString entitiesNames);


	protected :

	/**
	 * \return		La fenêtre principale.
	 */
	virtual Mgx3D::QtComponents::QtMgx3DMainWindow& getMainWindow ( );


	private :

	/**
	 * Constructeur de copie/opérateur = : Opérations interdites :
	 */
	QtEntityByDimensionSelectorPanel (const QtEntityByDimensionSelectorPanel&);
	QtEntityByDimensionSelectorPanel& operator = (const QtEntityByDimensionSelectorPanel&);

	/** Le choix des dimensions. */
	QtDimensionsSelectorPanel*				_dimensionsPanel;

	/** La saisie des entités. */
	QtMgx3DEntityPanel*						_entitiesPanel;

	/** La fenêtre principale. */
	Mgx3D::QtComponents::QtMgx3DMainWindow*	_mainWindow;
};	// class QtEntityByDimensionSelectorPanel

}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_ENTITY_BY_DIMENSION_SELECTOR_PANEL_H
