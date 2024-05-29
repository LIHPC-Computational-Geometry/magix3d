/**
 * \file		QtMgx3DEntityPanel.cpp
 * \author		Charles PIGNEROL
 * \date		09/09/2013
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DEntityPanel.h"
#include <QtUtil/QtErrorManagement.h>
#include "Utils/Common.h"

#include <QtUtil/QtConfiguration.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>

#include <QLayout>
#include <QLabel>

#include <limits>
#include <iostream>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtComponents
{


QtMgx3DEntityPanel::QtMgx3DEntityPanel (
	QWidget* parent, const string& title, bool editable,
	const string& tfLabel, const string& name,
	QtMgx3DMainWindow* mainWindow, SelectionManagerIfc::DIM dimensions,
	FilterEntity::objectType types)
	: QWidget (parent),
	  _nameTextField (0), _initialUniqueName (name), _label (0), _title (title)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setSpacing (0);
	setLayout (layout);
//	layout->setSizeConstraint (QLayout::SetFixedSize);
	layout->setContentsMargins (0, 0, 0, 0);

	QHBoxLayout*	hlayout	= 0;
	// Titre :
	if (0 != title.length ( ))
	{
		hlayout = new QHBoxLayout (0);
		hlayout->setContentsMargins (0, 0, 0, 0);
		layout->addLayout (hlayout);
		QLabel* titleLabel  = new QLabel (title.c_str ( ), this);
		QFont   font (titleLabel->font ( ));
		font.setBold (true);
		titleLabel->setFont (font);
		hlayout->addWidget (titleLabel);
	}   // if (0 != title.length ( ))

	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins (0, 0, 0, 0);
//	hlayout->setSizeConstraint (QLayout::SetFixedSize);
	hlayout->setSpacing (0);
	layout->addLayout (hlayout);
	_label	= new QLabel (tfLabel.c_str ( ), this);
	_label->setMinimumSize (_label->sizeHint ( ));
	hlayout->addWidget (_label);
	_nameTextField		= new QtEntityIDTextField (
				this, mainWindow, dimensions, types);
	connect (_nameTextField, SIGNAL (selectionModified (QString)),
	         this, SLOT (selectionModifiedCallback (QString)));
	connect (_nameTextField, SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_nameTextField, SIGNAL (entitiesRemovedFromSelection (QString)),
	         this, SLOT (entitiesRemovedFromSelectionCallback (QString)));
	hlayout->addWidget (_nameTextField);
	if (false == editable)
		_nameTextField->setReadOnly (true);
	setUniqueName (name);

// adjustSize fait en gros un resize (sizeHint ( )), et sizeHint ( ) peut
// tailler un peu large ...
	adjustSize ( );
}	// QtMgx3DEntityPanel::QtMgx3DEntityPanel


QtMgx3DEntityPanel::QtMgx3DEntityPanel (const QtMgx3DEntityPanel&)
	: QWidget (0),
	  _nameTextField (0), _initialUniqueName (0), _label (0),
	  _title ("Invalid QtMgx3DEntityPanel")
{
	MGX_FORBIDDEN ("QtMgx3DEntityPanel copy constructor is forbidden.")
}	// QtMgx3DEntityPanel::QtMgx3DEntityPanel (const QtMgx3DEntityPanel&)


QtMgx3DEntityPanel& QtMgx3DEntityPanel::operator = (const QtMgx3DEntityPanel&)
{
	MGX_FORBIDDEN ("QtMgx3DEntityPanel assignment operator is forbidden.")
	return *this;
}	// QtMgx3DEntityPanel::operator =


QtMgx3DEntityPanel::~QtMgx3DEntityPanel ( )
{
}	// QtMgx3DEntityPanel::~QtMgx3DEntityPanel


void QtMgx3DEntityPanel::setLabel (const string& label)
{
	CHECK_NULL_PTR_ERROR (_label)
	_label->setText (QString::fromUtf8(label.c_str ( )));
	_label->setMinimumSize (_label->sizeHint ( ));
	adjustSize ( );
}	// QtMgx3DEntityPanel::setLabel


bool QtMgx3DEntityPanel::isModified ( ) const
{
	return getUniqueName ( ) == _initialUniqueName ? false : true;
}	// QtMgx3DEntityPanel::isModified


bool QtMgx3DEntityPanel::isMultiSelectMode ( ) const
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	return _nameTextField->isMultiSelectMode ( );
}	// QtMgx3DEntityPanel::isMultiSelectMode


void QtMgx3DEntityPanel::setMultiSelectMode (bool multi)
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	_nameTextField->setMultiSelectMode (multi);
}	// QtMgx3DEntityPanel::setMultiSelectMode


void QtMgx3DEntityPanel::setUniqueName (const string& name)
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
//	_nameTextField->setText (name);
	vector<string>	names;
	names.push_back (name);
	setUniqueNames (names);
}	// QtMgx3DEntityPanel::setUniqueName


string QtMgx3DEntityPanel::getUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	return _nameTextField->getText ( );
}	// QtMgx3DEntityPanel::getUniqueName


vector<string> QtMgx3DEntityPanel::getUniqueNames ( ) const
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	return _nameTextField->getUniqueNames ( );
}	// QtMgx3DEntityPanel::getUniqueNames


void QtMgx3DEntityPanel::setUniqueNames (const vector<string>& names)
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	_nameTextField->setUniqueNames (names);
}	// QtMgx3DEntityPanel::setUniqueNames


QtEntityIDTextField* QtMgx3DEntityPanel::getNameTextField ( )
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	return _nameTextField;
}	// QtMgx3DEntityPanel::getNameTextField


const QtEntityIDTextField* QtMgx3DEntityPanel::getNameTextField ( ) const
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	return _nameTextField;
}	// QtMgx3DEntityPanel::getNameTextField


void QtMgx3DEntityPanel::setFilteredDimensions (SelectionManagerIfc::DIM dimensions)
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	_nameTextField->setFilteredDimensions (dimensions);
}	// QtMgx3DEntityPanel::setFilteredDimensions


SelectionManagerIfc::DIM QtMgx3DEntityPanel::getFilteredDimensions ( ) const
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	return _nameTextField->getFilteredDimensions ( );
}	// QtMgx3DEntityPanel::getFilteredDimensions


FilterEntity::objectType QtMgx3DEntityPanel::getFilteredTypes ( ) const
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	return _nameTextField->getFilteredTypes ( );
}	// QtMgx3DEntityPanel::getFilteredTypes


void QtMgx3DEntityPanel::setFilteredTypes (FilterEntity::objectType types)
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	_nameTextField->setFilteredTypes (types);
}	// QtMgx3DEntityPanel::getFilteredTypes


void QtMgx3DEntityPanel::reset ( )
{
	try
	{
		_initialUniqueName.clear ( );
		CHECK_NULL_PTR_ERROR (_nameTextField)
		_nameTextField->reset ( );
	}
	catch (...)
	{
	}
}	// QtMgx3DEntityPanel::reset


void QtMgx3DEntityPanel::clearSelection ( )
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	_nameTextField->clearSelection ( );
}	// QtMgx3DEntityPanel::clearSelection


void QtMgx3DEntityPanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	_nameTextField->setInteractiveMode (false);
}	// QtMgx3DEntityPanel::stopSelection


bool QtMgx3DEntityPanel::actualizeGui (bool removeDestroyed)
{
	CHECK_NULL_PTR_ERROR (_nameTextField)
	return _nameTextField->actualizeGui (removeDestroyed);
}	// QtMgx3DEntityPanel::actualizeGui


void QtMgx3DEntityPanel::selectionModifiedCallback (QString selection)
{
	emit selectionModified (selection);
}	// QtMgx3DEntityPanel::selectionModifiedCallback


void QtMgx3DEntityPanel::entitiesAddedToSelectionCallback (QString entitiesNames)
{
	emit entitiesAddedToSelection (entitiesNames);
}	// QtMgx3DEntityPanel::entitiesAddedToSelectionCallback


void QtMgx3DEntityPanel::entitiesRemovedFromSelectionCallback (QString entitiesNames)
{
	emit entitiesRemovedFromSelection (entitiesNames);
}	// QtMgx3DEntityPanel::entitiesRemovedFromSelectionCallback


}	// namespace QtComponents

}	// namespace Mgx3D
