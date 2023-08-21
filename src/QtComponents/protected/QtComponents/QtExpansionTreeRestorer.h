#ifndef QT_EXPANSION_TREE_RESTORER_H
#define QT_EXPANSION_TREE_RESTORER_H

#include <QTreeWidget>

#include <algorithm>	// binary_function
#include <utility>		// pair
#include <string>
#include <vector>


/**
 * <P>Classe permettant d'enregistrer/restaurer le caractère "ouvert/expansé"
 * d'un ensemble de <I>QTreeWidgetItem</I>. Cette classe a été créée sous
 * Qt 4.4.1 : lors d'une opération de <I>takeTopLevelItem</I>, ou de
 * <I>removeChild</I>, utilisées ici lors d'opération de tri, les items enlevés
 * de la représentation graphique perdent le caractère "expansé". Cette classe
 * vise à leur restituer automatiquement.
 * </P>
 * <P>Cette classe gère en plus le caractère sélectionné des items 
 * sous <I>Qt 4.6.2</I>.
 * </P>
 */
class QtExpansionTreeRestorer
{
	public :

	/**
	 * Constructeur. RAS.
	 */
	QtExpansionTreeRestorer ( );

	/**
	 * Constructeur. Enregistre de manière récursive toute l'arborescence de
	 * l'arbre tranmis en argument.	
	 */
	QtExpansionTreeRestorer (QTreeWidget& treeWidget);
	QtExpansionTreeRestorer (QTreeWidgetItem& item);

	/**
	 * Destructeur. RAS.
	 */
	 virtual ~QtExpansionTreeRestorer ( )
	{ }

	/**
	 * Ajoute de manière récursive l'arborescence transmise en argument aux
	 * enregistrements déjà effectués.
	 * @warning		La version actuelle ne vérifie pas si les items traités sont
	 *				déjà enregistrés (risque d'enregistrements multiples).
	 */
	virtual void record (QTreeWidget& item);
	virtual void record (QTreeWidgetItem& item);

	/**
	 * Restaure les caractères "expansés" préalablement enregistrés.
	 */
	virtual void restore ( );

	/**
	 * Attributs de l'item à reproduire lors de la restauration.
	 */
	struct ItemAttributes
	{
		ItemAttributes (const QTreeWidgetItem&);
		ItemAttributes (const ItemAttributes&);
		ItemAttributes& operator = (const ItemAttributes&);
		/** L'item est-il affiché ? */
		bool	hidden;
		/** L'item est-il expansé (ouvert) ? */
		bool	expanded;
		/** L'item est-il sélectionné ? */
		bool	selected;
	};	// struct ItemAttributes


	private :

	/**
	 * Opérateur = et constructeur de copie : interdits.
	 */
	QtExpansionTreeRestorer (const QtExpansionTreeRestorer&);
	QtExpansionTreeRestorer& operator = (const QtExpansionTreeRestorer&);

	std::vector < std::pair <QTreeWidgetItem*, ItemAttributes> >	_items;
};	// class QtExpansionTreeRestorer


template <class ViewItemPtr> struct TypeNameSortGreater :
				public std::binary_function<ViewItemPtr*,ViewItemPtr*,bool>
{
	bool operator ( ) (const ViewItemPtr& left, const ViewItemPtr& right) const
	{
		const std::string	l (left->text (0).toStdString ( ));
		const std::string	r (right->text (0).toStdString ( ));

		return l >= r;
	}	// operator ( )
};	// template struct TypeNameSortGreater


template <class ViewItemPtr> struct TypeNameSortLesser :
				public std::binary_function<ViewItemPtr*,ViewItemPtr*,bool>
{
	bool operator ( ) (const ViewItemPtr& left, const ViewItemPtr& right) const
	{
		const std::string	l (left->text (0).toStdString ( ));
		const std::string	r (right->text (0).toStdString ( ));

		return l <= r;
	}	// operator ( )
};	// template struct TypeNameSortLesser


#endif	// QT_EXPANSION_TREE_RESTORER_H
