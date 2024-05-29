#include "Internal/ContextIfc.h"

#include "QtComponents/QtExpansionTreeRestorer.h"

#include <assert.h>
#include <iostream>
#include <memory>
#include <vector>


using namespace std;


QtExpansionTreeRestorer::ItemAttributes::ItemAttributes (
												const QTreeWidgetItem& item)
	: hidden (item.isHidden ( )), expanded (item.isExpanded ( )),
	  selected (item.isSelected ( ))
{
}	// ItemAttributes::ItemAttributes


QtExpansionTreeRestorer::ItemAttributes::ItemAttributes (
							const QtExpansionTreeRestorer::ItemAttributes& attr)
	: hidden (attr.hidden), expanded (attr.expanded), selected (attr.selected)
{
}	// ItemAttributes::ItemAttributes


QtExpansionTreeRestorer::ItemAttributes&
				QtExpansionTreeRestorer::ItemAttributes::operator = (
							const QtExpansionTreeRestorer::ItemAttributes& attr)
{
	if (&attr != this)
	{
		hidden		= attr.hidden;
		expanded	= attr.expanded;
		selected	= attr.selected;
	}	// if (&attr != this)

	return *this;
}	// ItemAttributes::operator =


QtExpansionTreeRestorer::QtExpansionTreeRestorer ( )
	: _items ( )
{
}	// QtExpansionTreeRestorer::QtExpansionTreeRestorer


QtExpansionTreeRestorer::QtExpansionTreeRestorer (QTreeWidget& treeWidget)
	: _items ( )
{
	record (treeWidget);
}	// QtExpansionTreeRestorer::QtExpansionTreeRestorer


QtExpansionTreeRestorer::QtExpansionTreeRestorer (QTreeWidgetItem& item)
	: _items ( )
{
	record (item);
}	// QtExpansionTreeRestorer::QtExpansionTreeRestorer


QtExpansionTreeRestorer::QtExpansionTreeRestorer (
											const QtExpansionTreeRestorer&)
	: _items ( )
{
	assert (0 && "QtExpansionTreeRestorer copy constructor is not allowed.");
}	// QtExpansionTreeRestorer::QtExpansionTreeRestorer


QtExpansionTreeRestorer& QtExpansionTreeRestorer::operator = (
											const QtExpansionTreeRestorer&)
{
	assert (0 && "QtExpansionTreeRestorer operator = is not allowed.");
	return *this;
}	// QtExpansionTreeRestorer::operator =


void QtExpansionTreeRestorer::record (QTreeWidget& treeWidget)
{
	for (size_t i = 0; i < treeWidget.topLevelItemCount ( ); i++)
		record (*(treeWidget.topLevelItem (i)));
}	// QtExpansionTreeRestorer::record


void QtExpansionTreeRestorer::record (QTreeWidgetItem& item)
{
	QtExpansionTreeRestorer::ItemAttributes	attr (item);
	_items.push_back (pair <QTreeWidgetItem*, ItemAttributes>(&item, attr));
	for (size_t i = 0; i < item.childCount ( ); i++)
		record (*(item.child (i)));
}	// QtExpansionTreeRestorer::record


void QtExpansionTreeRestorer::restore ( )
{
	for (vector < pair <QTreeWidgetItem*, ItemAttributes> >::iterator it =
				_items.begin ( ); _items.end ( ) != it; it++)
	{
		(*it).first->setHidden ((*it).second.hidden);
		(*it).first->setExpanded ((*it).second.expanded);
		(*it).first->setSelected ((*it).second.selected);
	}	// for (vector < pair <QTreeWidgetItem*, ItemAttributes> >::iterator it
}	// QtExpansionTreeRestorer::restore

