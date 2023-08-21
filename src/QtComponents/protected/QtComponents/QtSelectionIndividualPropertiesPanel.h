/**
 * \file		QtSelectionIndividualPropertiesPanel.h
 * \author		Charles PIGNEROL
 * \date		12/09/2013
 */
#ifndef QT_SELECTION_INDIVIDUAL_PROPERTIES_PANEL_H
#define QT_SELECTION_INDIVIDUAL_PROPERTIES_PANEL_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtSelectionPropertiesTreeItem.h"
#include "QtComponents/Qt3DGraphicalWidget.h"
#include "Utils/SelectionManager.h"

#include <QtUtil/QtGroupBox.h>

#include <QSettings>
#include <QMenu>
#include <QTreeWidget>

#include <memory>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * \brief		Item représentant une entité.
 */
class QtEntityPropertiesItem : public QtSelectionPropertiesTreeItem
{
	public :

	/**
	 * \param		Parent de l'item
	 * \param		Entité représentée
	 * \param		true si l'entité fourni la totalité de sa description, false
	 * 				si elle ne fournit que les informations non calculées
	 * \param		Eventuel gestionnaire de rendus 3D, pour les mises en
	 * 				évidence (highlighting)
	 */
	QtEntityPropertiesItem (QTreeWidget* parent, Mgx3D::Utils::Entity&,
				bool alsoComputed, Mgx3D::QtComponents::RenderingManager* rm);
	QtEntityPropertiesItem (QTreeWidgetItem* parent, Mgx3D::Utils::Entity&,
				bool alsoComputed, Mgx3D::QtComponents::RenderingManager* rm);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtEntityPropertiesItem ( );

	/**
	 * Finalise la construction de l'objet.
	 */
	virtual void initialize ( );

	/**
	 * Actualise la description de l'entité.
	 */
	virtual void updateDescription ( );

	/**
	 * \return		L'entité représentée.
	 */
	virtual Mgx3D::Utils::Entity& getEntity ( );
	virtual const Mgx3D::Utils::Entity& getEntity ( ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtEntityPropertiesItem (const QtEntityPropertiesItem&);
	QtEntityPropertiesItem& operator = (const QtEntityPropertiesItem&);

	/** L'entité représentée. */
	Mgx3D::Utils::Entity*					_entity;
};	// class QtEntityPropertiesItem


/**
 * Classe représentant une sélection d'entités sous forme textuelle à
 * partir de leur description.
 * \see		Mgx3D::Utils::Entity::getDescription
 */
class QtSelectionIndividualPropertiesTreeWidget : public QTreeWidget
{
	Q_OBJECT

	public :

	/**
	 * Constructeur.
	 * \param		L'éventuel gestionnaire de rendu associé pour les
	 * 				affichages 3D.
	 */
	QtSelectionIndividualPropertiesTreeWidget (
					QWidget* parent, Mgx3D::QtComponents::RenderingManager* rm);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSelectionIndividualPropertiesTreeWidget ( );

	/**
	 * L'éventuel gestionnaire de rendus 3D pour les affichages.
	 */
	virtual Mgx3D::QtComponents::RenderingManager* getRenderingManager ( );
	virtual void setRenderingManager (Mgx3D::QtComponents::RenderingManager*);


	protected slots :

	virtual void itemExpandedCallback (QTreeWidgetItem* item);


	protected :

	/**
	 * Met en évidence les entités sélectionnées, annule cette mise en évidence
	 * pour celles qui sont désélectionnées.
	 */
	virtual void selectionChanged (
			const QItemSelection& selected, const QItemSelection& deselected);
	virtual void highlightSelection (
			const std::vector<QTreeWidgetItem*>& selection, bool highlight);

	/**
	 * <P>Les listes Qt transmises à QTreeWidget::selectionChanged contiennent
	 * des cellules sélectionnées. Un item peut donc y apparaître autant de fois
	 * qu'il a de cellules (nombre de colonnes).</P>
	 *
	 * <P>Cette fonction réduit la liste d'entités (dé)sélectionnées tranmise en
	 * argument à une liste d'instance de QTreeWidgetItem où chaque instance est
	 * présente de manière unique.</P>
	 */
	virtual std::vector<QTreeWidgetItem*> filteredSelectionList (const QItemSelection& list);


	private :

	/**
	 * Constructeur de copie et opérateur = interdits.
	 */
	QtSelectionIndividualPropertiesTreeWidget (const QtSelectionIndividualPropertiesTreeWidget&);
	QtSelectionIndividualPropertiesTreeWidget& operator = (
							const QtSelectionIndividualPropertiesTreeWidget&);

	/** L'éventuel gestionnaire de rendus 3D pour les affichages. */
	Mgx3D::QtComponents::RenderingManager*	_renderingManager;
};	// class QtSelectionIndividualPropertiesTreeWidget


/**
 * \brief		Panneau de l'IHM <I>Magix 3D</I> affichant les propriétés
 *				des entités sélectionnées.
 */
class QtSelectionIndividualPropertiesPanel :
			public QtGroupBox, public Mgx3D::Utils::SelectionManagerObserver
{
	public :

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Eventuel gestionnaire de sélection.
	 */
	QtSelectionIndividualPropertiesPanel (
		QWidget* parent, const std::string& name, Utils::SelectionManagerIfc*);

	/**
	 * RAS.
	 */
	virtual ~QtSelectionIndividualPropertiesPanel ( );

	/**
	 * Divers IHM.
	 */
	//@{

	/**
	 * Enregistre les paramètres d'affichage (taille, position, ...) de cette
	 * fenêtre.
	 * \see		readSettings
	 */
	virtual void writeSettings (QSettings& settings);

	/**
	 * Lit et s'applique les paramètres d'affichage (taille, position, ...) de
	 * cette fenêtre.
	 * \see		writeSettings
	 */
	virtual void readSettings (QSettings& settings);

	//@}	// Divers IHM

	/**
	 * La gestion de la sélection.
	 */
	//@{

	/**
	 * Affiche ou non les propriétés individuelles de la sélection.
	 */
	virtual bool displayProperties ( ) const;
	virtual void displayProperties (bool);

	/**
	 * Affiche ou non les propriétés calculées individuelles de la sélection.
	 */
	virtual bool displayComputedProperties ( ) const;
	virtual void displayComputedProperties (bool);

	/**
	 * Actualise l'affichage.
	 * \param		Nouveau gestionnaire de sélection. Peut être nul.
	 */
	virtual void setSelectionManager (
				Mgx3D::Utils::SelectionManagerIfc* selectionManager);

	/**
	 * @return		L'item correspondant à l'entité transmise en argument,
	 *			ou 0 s'il n'y en a pas.
	 */
	virtual QtEntityPropertiesItem* findEntityItem (
				const Mgx3D::Utils::Entity& entity) const;

	/**
	 * @return		L'index de l'item correspondant à l'entité transmise en argument,
	 *			ou (size_t)-1 s'il n'y en a pas.
	 */
	virtual size_t findEntityItemIndex (
				const Mgx3D::Utils::Entity& entity) const;

	/**
	 * Appellées lorsque des entités sont ajoutées ou enlevées de la sélection.
	 * Actualisent l'affichage.
	 * \param		Entités ajoutées ou enlevées
	 * \param		<I>true</I> s'il s'agit d'un <I>clear selection</I>.
	 */
	virtual void entitiesAddedToSelection (
			const std::vector<Mgx3D::Utils::Entity*>& entities);
	virtual void entitiesRemovedFromSelection (
			const std::vector<Mgx3D::Utils::Entity*>& entities, bool clear);

	//@}	// La gestion de la sélection.

	/*
	 * Les affichages graphiques.
	 */
	//@{

	/**
	 * \return		L'éventuelle fenêtre graphique associée pour les
	 * 				affichages 3D.
	 * \see			setGraphicalWidget
	 */
	virtual Mgx3D::QtComponents::Qt3DGraphicalWidget* getGraphicalWidget ( );

	/**
	 * \param		La fenêtre graphique à utiliser pour les affichages 3D,
	 *				ou 0.
	 * \exception	Une exception est levée si la fenêtre transmise en
	 *				argument est non nulle et qu'une autre fenêtre est déjà
	 *				affectée.
	 * \see			getGraphicalWidget
	 */
	virtual void setGraphicalWidget (Qt3DGraphicalWidget* widget3D);

	//@}	// Les affichages graphiques.


	protected :


	public :


	protected :

	// Opérations interdites :
	QtSelectionIndividualPropertiesPanel (const QtSelectionIndividualPropertiesPanel&);


	private :

	QtSelectionIndividualPropertiesPanel& operator = (const QtSelectionIndividualPropertiesPanel&);

	/** Le widget représentant les groupes d'entités. */
	QtSelectionIndividualPropertiesTreeWidget*		_entitiesWidget;

	/** L'éventuelle fenêtre graphique associée, pour les affichages 3D. */
	Qt3DGraphicalWidget*					_graphicalWidget;

	/** Affichage ou non de toutes ou non les propriétés. */
	bool							_displayProperties;
	bool							_displayComputedProperties;
};	// class QtSelectionIndividualPropertiesPanel


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_SELECTION_INDIVIDUAL_PROPERTIES_PANEL_H
