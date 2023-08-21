/*
 * QtMgx3DEntityNamePanel.cpp
 *
 *  Created on: 20 mars 2013
 *      Author: ledouxf
 */
#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtMgx3DEntityNamePanel.h"

#include "Group/Group0D.h"
#include "Group/Group1D.h"
#include "Group/Group2D.h"
#include "Group/Group3D.h"
#include "Group/GroupManagerIfc.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <QComboBox>
#include <QHBoxLayout>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Group;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::QtComponents;


namespace Mgx3D
{

namespace QtComponents
{

// ===========================================================================
//                        LA CLASSE QtMgx3DEntityNamePanel
// ===========================================================================

QtMgx3DEntityNamePanel::QtMgx3DEntityNamePanel (
    QWidget* parent, const string& name, QtMgx3DMainWindow& mainWindow,
    SelectionManagerIfc::DIM dimensions, FilterEntity::objectType types)
    : QWidget (parent), ValidatedField ( ),
      _names(0), _mainWindow (&mainWindow), _label(0)

{
    QHBoxLayout*    layout  = new QHBoxLayout (this);
    layout->setMargin (QtConfiguration::margin);
    layout->setSpacing (QtConfiguration::spacing);
    setLayout (layout);
    _label   = new QLabel (name.c_str(), this);
    layout->addWidget (_label);
    _names = new QtEntityIDTextField (this, &mainWindow, dimensions, types);
	connect (_names, SIGNAL (selectionModified (QString)),
	         this, SLOT (selectionModifiedCallback (QString)));
	connect (_names, SIGNAL (entitiesAddedToSelection (QString)), this,
	         SLOT (entitiesAddedToSelectionCallback (QString)));
	connect (_names, SIGNAL (entitiesRemovedFromSelection (QString)), this,
	         SLOT (entitiesRemovedFromSelectionCallback (QString)));
    _names->setInteractiveMode (false);
    _names->setMultiSelectMode (true);

    layout->addWidget (_names, 1000);
}   // QtMgx3DEntityNamePanel::QtMgx3DEntityNamePanel


QtMgx3DEntityNamePanel::QtMgx3DEntityNamePanel (const QtMgx3DEntityNamePanel&)
    : QWidget (0), ValidatedField ( ),
      _names (0), _mainWindow (0), _label(0)
{
    MGX_FORBIDDEN ("QtMgx3DEntityNamePanel copy constructor is not allowed.");
}   // QtMgx3DEntityNamePanel::QtMgx3DEntityNamePanel (const QtMgx3DEntityNamePanel&)


QtMgx3DEntityNamePanel& QtMgx3DEntityNamePanel::operator = (
                                                const QtMgx3DEntityNamePanel&)
{
    MGX_FORBIDDEN ("QtMgx3DEntityNamePanel assignment operator is not allowed.");
    return *this;
}   // QtMgx3DEntityNamePanel::QtMgx3DEntityNamePanel (const QtMgx3DEntityNamePanel&)


QtMgx3DEntityNamePanel::~QtMgx3DEntityNamePanel ( )
{
}   // QtMgx3DEntityNamePanel::~QtMgx3DEntityNamePanel


ContextIfc& QtMgx3DEntityNamePanel::getContext ( )
{
    CHECK_NULL_PTR_ERROR (_mainWindow)
    return _mainWindow->getContext ( );
}   // QtMgx3DEntityNamePanel::getContext


vector<string> QtMgx3DEntityNamePanel::getNames ( ) const
{
    CHECK_NULL_PTR_ERROR (_names)
    vector<string> v;
    string full_names =  _names->text( ).toStdString ( );
    bool end_reached = false;
    while(!full_names.empty() && !end_reached)
    {
        int space_pos = full_names.find_first_of(" ");
        if(space_pos==-1){
            end_reached = true;
            v.push_back(full_names);
        }
        else {
            string s = full_names.substr(0,space_pos);
            v.push_back(s);
            full_names = full_names.substr(space_pos+1,full_names.size()-space_pos);
        }
     }
    return v;

}   // QtMgx3DEntityNamePanel::getNames


void QtMgx3DEntityNamePanel::setNames (const vector<string>& names)
{
	CHECK_NULL_PTR_ERROR (_names)
	UTF8String	text (Charset::UTF_8);
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		if (0 != text.length ( ))
			text << ' ';
		text << *it;
	}

	_names->setText (UTF8TOQSTRING (text));
}	// QtMgx3DEntityNamePanel::setNames


void QtMgx3DEntityNamePanel::reset ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_names)
		_names->setText (QString (""));
	}
	catch (...)
	{
	}
}	// QtMgx3DEntityNamePanel::reset



void QtMgx3DEntityNamePanel::clear ( )
{
	CHECK_NULL_PTR_ERROR (_names)
	_names->setText (QString (""));
}	// QtMgx3DEntityNamePanel::clear


void QtMgx3DEntityNamePanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_names)
	_names->setInteractiveMode (false);
}	// QtMgx3DEntityNamePanel::stopSelection


void QtMgx3DEntityNamePanel::validate ( )
{
// ??? TODO[FL]
}   // QtMgx3DEntityNamePanel::validate


void QtMgx3DEntityNamePanel::autoUpdate ( )
{
#ifdef AUTO_UPDATE_OLD_SCHEME
// ??? TODO[FL]
#else
    clear ( );
#endif	// AUTO_UPDATE_OLD_SCHEME
}   // QtMgx3DEntityNamePanel::autoUpdate


void QtMgx3DEntityNamePanel::setLabel (const std::string& label)
{
    _label->setText(QString::fromUtf8(label.c_str()));
}

void QtMgx3DEntityNamePanel::setDimensions (SelectionManagerIfc::DIM dimensions)
{
    _names->selectionCleared();
    _names->setFilteredDimensions (dimensions);
}	// QtMgx3DEntityNamePanel::setDimensions


void QtMgx3DEntityNamePanel::setFilteredTypes (FilterEntity::objectType types)
{
    _names->selectionCleared();
    _names->setFilteredTypes (types);
}

void QtMgx3DEntityNamePanel::setMultiSelectMode (bool enable)
{
    _names->setMultiSelectMode (enable);
}


QtEntityIDTextField* QtMgx3DEntityNamePanel::getNamesTextField ( )
{
	return _names;
}


void QtMgx3DEntityNamePanel::selectionModifiedCallback (QString selection)
{
	emit selectionModified (selection);
}	// QtMgx3DEntityNamePanel::selectionModifiedCallback


void QtMgx3DEntityNamePanel::entitiesAddedToSelectionCallback (
														QString entitiesNames)
{
	emit entitiesAddedToSelection (entitiesNames);
}	// QtMgx3DEntityNamePanel::entitiesAddedToSelectionCallback


void QtMgx3DEntityNamePanel::entitiesRemovedFromSelectionCallback (
														QString entitiesNames)
{
	emit entitiesRemovedFromSelection (entitiesNames);
}	// QtMgx3DEntityNamePanel::entitiesRemovedFromSelectionCallback

// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}   // namespace QtComponents

}   // namespace Mgx3D
