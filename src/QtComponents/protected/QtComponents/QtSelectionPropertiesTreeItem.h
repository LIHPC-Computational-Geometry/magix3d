/**
 * \file		QtSelectionIndividualPropertiesPanel.h
 * \author		Charles PIGNEROL
 * \date		07/10/2013
 */
#ifndef QT_SELECTION_PROPERTIES_TREE_ITEM_H
#define QT_SELECTION_PROPERTIES_TREE_ITEM_H

#include "Utils/SerializedRepresentation.h"
#include "QtComponents/RenderingManager.h"
#include "Internal/Context.h"

#include <QTreeWidget>

#include <memory>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * \brief		Item représentant une rubrique de description d'entité.
 */
class QtSelectionPropertiesTreeItem : public QTreeWidgetItem
{
	public :

	/**
	 * \param		Parent de l'item
	 * \param		Description (adoptée) de la rubrique représentée.
	 * \param		true si l'entité fourni la totalité de sa description, false
	 * 				si elle ne fournit que les informations non calculées
	 * \param		Eventuel afficheur 3D, pour les mises en évidence
	 *				(highlighting d'entité par exemple).
	 */
	QtSelectionPropertiesTreeItem (
			QTreeWidget* p, Mgx3D::Utils::SerializedRepresentation*,
			bool alsoComputed, Mgx3D::QtComponents::RenderingManager* rm);
	QtSelectionPropertiesTreeItem (
			QTreeWidgetItem* p, Mgx3D::Utils::SerializedRepresentation*,
			bool alsoComputed, Mgx3D::QtComponents::RenderingManager* rm);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSelectionPropertiesTreeItem ( );

	/**
	 * \return		La description de la rubrique représentée.
	 * \see			setDescription
	 */
	virtual const Mgx3D::Utils::SerializedRepresentation& getDescription ( ) const;

	/**
	 * \return		true si les propriétés calculées sont affichées, false
	 * 				dans le cas contraire.
	 */
	virtual bool displayComputedProperties ( ) const;

	/**
	 * Finalize la construction de l'objet.
	 */
	virtual void initialize ( );

	/**
	 * Optimisation. Affiche les propriétés de la rubrique. Appelé par le 
	 * tree widget parent lorsque l'utilisateur demande l'ouverture de la
	 * sous-arborescence.
	 */
	virtual void displayContents ( );

	/**
	 * \return		L'éventuel gestionnaire de rendus 3D.
	 */
	virtual Mgx3D::QtComponents::RenderingManager* getRenderingManager ( );

	/**
	 * \return	Une référence sur le contexte <I>Magix 3D</I> associé à
	 *			l'opération.
	 */
	virtual const Mgx3D::Internal::ContextIfc& getContext ( ) const;


	protected :

	/**
	 * \param		La description de la rubrique représentée (adoptée).
	 * \see			getDescription
	 */
	virtual void setDescription (Mgx3D::Utils::SerializedRepresentation*);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtSelectionPropertiesTreeItem (const QtSelectionPropertiesTreeItem&);
	QtSelectionPropertiesTreeItem& operator = (const QtSelectionPropertiesTreeItem&);

	/** La description de la rubrique représentée. */
	std::unique_ptr<Mgx3D::Utils::SerializedRepresentation>	_description;

	/** true si l'entité fourni la totalité de sa description, false si elle ne
	 * fournit que les informations non calculées. */
	bool					_displayComputedProperties;

	/** L'éventuel gestionnaire de rendus 3D. */
	Mgx3D::QtComponents::RenderingManager*	_renderingManager;
};	// class QtPropertiesItem


}	// namespace QtComponents


}	// namespace Mgx3D 


#endif	// QT_SELECTION_PROPERTIES_TREE_ITEM_H
