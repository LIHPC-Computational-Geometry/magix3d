/**
 * \file		QtSelectionCommonPropertiesPanel.h
 * \author		Charles PIGNEROL
 * \date		07/10/2013
 */
#ifndef QT_SELECTION_COMMON_PROPERTIES_PANEL_H
#define QT_SELECTION_COMMON_PROPERTIES_PANEL_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtSelectionPropertiesTreeItem.h"
#include "Utils/SelectionManager.h"

#include <QtUtil/QtGroupBox.h>

#include <QSettings>
#include <QMenu>
#include <QTableWidget>
#include <QTreeWidget>

#include <memory>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * \brief		Item représentant les propriétés communes d'une selection
 *				d'entités.
 */
class QtCommonPropertiesItem : public QtSelectionPropertiesTreeItem
{
	public :

	/**
	 * \param		Parent de l'item
	 * \param		Entités représentées
	 * \param		true si les entités fournissent la totalité de leur
	 * 				description, false si elle ne fournissent que les
	 * 				informations non calculées
	 */
	QtCommonPropertiesItem (
				QTreeWidget* parent, const std::vector<Mgx3D::Utils::Entity*>&,
				bool alsoComputed);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtCommonPropertiesItem ( );

	/**
	 * Finalise la construction de l'objet.
	 */
	virtual void initialize ( );

	/**
	 * Actualise la description des propriétés communes de la sélection
	 * d'entités représentée.
	 */
	virtual void updateDescription ( );

	/**
	 * \return		Les entités représentées.
	 */
	virtual const std::vector<Mgx3D::Utils::Entity*>& getEntities ( ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtCommonPropertiesItem (const QtCommonPropertiesItem&);
	QtCommonPropertiesItem& operator = (const QtCommonPropertiesItem&);

	/** L'entité représentée. */
	std::vector<Mgx3D::Utils::Entity*>			_entities;
};	// class QtCommonPropertiesItem


/**
 * Classe représentant une sélection d'entités sous forme textuelle à
 * partir de leur description.
 * \see		Mgx3D::Utils::Entity::getDescription
 */
class QtSelectionCommonPropertiesTreeWidget : public QTreeWidget
{
	Q_OBJECT

	public :

	/**
	 * Constructeur : RAS.
	 */
	QtSelectionCommonPropertiesTreeWidget (QWidget* parent);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSelectionCommonPropertiesTreeWidget ( );


	protected slots :

	virtual void itemExpandedCallback (QTreeWidgetItem* item);


	protected :

	/**
	 * <P>Les listes Qt transmises à QTreeWidget::selectionChanged contiennent
	 * des cellules sélectionnées. Un item peut donc y apparaître autant de fois
	 * qu'il a de cellules (nombre de colonnes).</P>
	 *
	 * <P>Cette fonction réduit la liste d'entités (dé)sélectionnées tranmise en
	 * argument à une liste d'instance de QTreeWidgetItem où chaque instance est
	 * présente de manière unique.</P>
	 */
	virtual std::vector<QTreeWidgetItem*> filteredSelectionList (
													const QItemSelection& list);


	private :

	/**
	 * Constructeur de copie et opérateur = interdits.
	 */
	QtSelectionCommonPropertiesTreeWidget (const QtSelectionCommonPropertiesTreeWidget&);
	QtSelectionCommonPropertiesTreeWidget& operator = (
										const QtSelectionCommonPropertiesTreeWidget&);
};	// class QtSelectionCommonPropertiesTreeWidget


/**
 * \brief		Panneau de l'IHM <I>Magix 3D</I> affichant les propriétés
 *				des entités sélectionnées.
 */
class QtSelectionCommonPropertiesPanel :
			public QtGroupBox, public Mgx3D::Utils::SelectionManagerObserver
{
	public :

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Eventuel gestionnaire de sélection.
	 */
	QtSelectionCommonPropertiesPanel (
		QWidget* parent, const std::string& name, Utils::SelectionManagerIfc*);

	/**
	 * RAS.
	 */
	virtual ~QtSelectionCommonPropertiesPanel ( );

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
	 * Affiche ou non les propriétés commune de la sélection.
	 */
	virtual bool displayProperties ( ) const;
	virtual void displayProperties (bool);

	/**
	 * Affiche ou non les propriétés calculées commune de la sélection.
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
	 * Appelé lorsque la sélection change. Actualise l'affichage.
	 */
	virtual void selectionModified ( );

	//@}	// La gestion de la sélection.


	protected :


	public :


	protected :

	// Opérations interdites :
	QtSelectionCommonPropertiesPanel (const QtSelectionCommonPropertiesPanel&);


	private :

	QtSelectionCommonPropertiesPanel& operator = (
									const QtSelectionCommonPropertiesPanel&);

	/** La boite englobant la sélection. */
	QTableWidget*								_boundingBoxWidget;

	/** Le widget représentant les groupes d'entités. */
	QtSelectionCommonPropertiesTreeWidget*		_entitiesWidget;

	/** Affichage ou non de toutes ou non les propriétés. */
	bool										_displayProperties;
	bool										_displayComputedProperties;
};	// class QtSelectionCommonPropertiesPanel


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_SELECTION_COMMON_PROPERTIES_PANEL_H
